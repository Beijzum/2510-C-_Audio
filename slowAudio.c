#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wav.h"

/**
 * In order to run this program, you must type "gcc slowAudio.c -o slowAudio.exe" into terminal.
 * Next type in "./slowAudio.exe ./SoundClips/input.wav ./output/output.wav"
 * For example, "./slowAudio.exe ./SoundClips/huh.wav ./output/huh_slow.wav"
 */
int checkFormat(WAVHEADER header) {
    if (header.format[0] == 'W' && header.format[1] == 'A' && header.format[2] == 'V' && header.format[3] == 'E') {
        return 1;
    } else return 0;
}

int getBlockSize(WAVHEADER header) {
    int channels = header.numChannels;
    int BytesPerSample = header.bitsPerSample / 8;
    return channels * BytesPerSample;
}

int main(int argc, char *argv[]) {

    // ensure valid usage
    if (argc != 3) {
        printf("Invalid usage: ./slowAudio.exe ./soundClips/audio.wav ./out.wav\n");
        return 1;
    }

    // ensure out file has .wav extension
    char *fileExtension = &argv[2][strlen(argv[2]) - 4];
    if (strcmp(fileExtension, ".wav")) {
        printf("Invalid usage, ensure outfile has .wav file extension\n");
        return 1;
    }

    // Open audio file
    FILE *inputAudio = fopen(argv[1], "rb");
    if (inputAudio == NULL) {
        printf("Error, %s not found\n", argv[1]);
        return 1;
    }

    // read header
    WAVHEADER header;
    fread(&header, sizeof(WAVHEADER), 1, inputAudio);

    // check wav format
    if (!checkFormat(header)) {
        printf("Error, %s is not a .wav file\n", argv[1]);
        return 1;
    }

    // get data block size
    WORD blockSize = getBlockSize(header);

    // open output file
    FILE *outputAudio = fopen(argv[2], "wb");

    // change sample rate
    header.sampleRate = header.sampleRate / 2;

    // write header to file
    fwrite(&header, sizeof(WAVHEADER), 1, outputAudio);

    WORD buffer[blockSize];

    // write data
    while (fread(&buffer, blockSize, 1, inputAudio)) {
        fwrite(&buffer, blockSize, 1, outputAudio);
    }

    fclose(inputAudio);
    fclose(outputAudio);

    return 0;
}