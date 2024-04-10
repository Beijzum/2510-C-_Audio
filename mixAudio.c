#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wav.h"

// ./insertAudio.exe ./nativeSoundClips/test.wav ./nativeSoundClips/test2.wav ./out.wav

int min(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

int checkFormat(WAVHEADER header) {
    char wave[4];
    if (!strcmp(strncpy(wave, header.format, 4), "WAVE")) {
        return 1;
    } else return 0;
}

int getBlockSize(WAVHEADER header) {
    int channels = header.numChannels;
    int BytesPerSample = header.bitsPerSample / 8;
    return channels * BytesPerSample;
}

int compareBlockSizes(WAVHEADER header1, WAVHEADER header2) {
    if (getBlockSize(header1) != getBlockSize(header2)) {
        return 0;
    } 
    return 1;
}

WAVHEADER constructNewHeader(WAVHEADER header1, WAVHEADER header2, long chunkSize, long subchunk2Size) {
    WAVHEADER newHeader;
    
    // set header variables
    strcpy(newHeader.chunkID, header1.chunkID);
    strcpy(newHeader.format, header1.format);
    strcpy(newHeader.subchunk1ID, header1.subchunk1ID);
    strcpy(newHeader.subchunk2ID, header1.subchunk2ID);

    newHeader.chunkSize = chunkSize;
    newHeader.subchunk2Size = subchunk2Size;
    
    newHeader.audioFormat = header1.audioFormat;
    newHeader.subchunk1Size = header1.subchunk1Size;
    newHeader.blockAlign = header1.blockAlign;
    newHeader.bitsPerSample = header1.bitsPerSample;
    newHeader.numChannels = header1.numChannels;
    newHeader.sampleRate = min(header1.sampleRate, header2.sampleRate) * 2;
    newHeader.byteRate = 6000;

    return newHeader;
}

int main(int argc, char* argv[]) {

    // ensure valid usage
    if (argc != 4) {
        printf("Invalid usage: ./mixAudio.exe ./audio1.wav ./audio2.wav ./out.wav\n");
        return 1;
    }

    // ensure out file has .wav extension
    char* fileExtension = &argv[2][strlen(argv[2]) - 4];
    if (strcmp(fileExtension, ".wav")) {
        printf("Invalid usage, ensure outfile has .wav file extension\n");
        return 1;
    }

    // Open audio files
    FILE* inputAudio1 = fopen(argv[1], "rb");
    if (inputAudio1 == NULL) {
        printf("Error, %s not found\n", argv[1]);
        return 1;
    }
    FILE* inputAudio2 = fopen(argv[2], "rb");
    if (inputAudio2 == NULL) {
        printf("Error, %s not found\n", argv[2]);
        return 1;
    }

    // read headers
    WAVHEADER header1;
    WAVHEADER header2;
    fread(&header1, sizeof(WAVHEADER), 1, inputAudio1);
    fread(&header2, sizeof(WAVHEADER), 1, inputAudio2);

    // check wav formats
    if (!checkFormat(header1) || !checkFormat(header2)) {
        printf("Error, one of the two files is not a valid .wav file\n");
        return 1;
    }

    // open output file
    FILE* outputAudio = fopen(argv[3], "wb");

    // check if files can be combined
    if (!compareBlockSizes(header1, header2)) {
        printf("Error, block size differs, cannot combine audio files\n");
        return 1;
    }

    WORD blockSize = getBlockSize(header1);
    WORD buffer1[blockSize], buffer2[blockSize]; 

    // write data
    fseek(outputAudio, sizeof(WAVHEADER), SEEK_SET);

    // use larger file for outer loop
    if (header1.subchunk2Size > header2.subchunk2Size) {
        
        while (fread(&buffer1, blockSize, 1, inputAudio1)) {
            fwrite(&buffer1, blockSize, 1, outputAudio);
            fread(&buffer2, blockSize, 1, inputAudio2);
            if (buffer2) fwrite(&buffer2, blockSize, 1, outputAudio);
        }
    } else {

        while (fread(&buffer2, blockSize, 1, inputAudio2)) {
            fwrite(&buffer2, blockSize, 1, outputAudio);
            fread(&buffer1, blockSize, 1, inputAudio1);
            if (buffer1) fwrite(&buffer1, blockSize, 1, outputAudio);
        }
    }

    // write header
    long chunkSize = ftell(outputAudio) - 8, subChunk2Size = ftell(outputAudio) - sizeof(WAVHEADER);
    WAVHEADER newHeader = constructNewHeader(header1, header2, chunkSize, subChunk2Size);
    fseek(outputAudio, 0, SEEK_SET);

    fwrite(&newHeader, sizeof(WAVHEADER), 1, outputAudio);

    return 0;
}