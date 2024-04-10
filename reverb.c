#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wav.h"

#define DELAY_SAMPLES 44100 // 1 second delay for 44100 Hz sample rate

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

int main(int argc, char* argv[]) {

    // ensure valid usage
    if (argc != 3) {
        printf("Invalid usage: ./insertAudio.exe ./soundClips/audio.wav ./out.wav\n");
        return 1;
    }

    // ensure out file has .wav extension
    char* fileExtension = &argv[2][strlen(argv[2]) - 4];
    if (strcmp(fileExtension, ".wav")) {
        printf("Invalid usage, ensure outfile has .wav file extension\n");
        return 1;
    }

    // Open audio file
    FILE* inputAudio = fopen(argv[1], "rb");
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
    FILE* outputAudio = fopen(argv[2], "wb");

    // write header to file
    fwrite(&header, sizeof(WAVHEADER), 1, outputAudio);

    WORD buffer[blockSize];
    WORD echoBuffer[blockSize];

    int i, echoIndex;

    // Read a block of data from the input audio file
    while (fread(&buffer, blockSize, 1, inputAudio)) {

        // Write the original audio data to the output file
        fwrite(&buffer, blockSize, 1, outputAudio);

        // Calculate delayed and attenuated echo
        for (i = 0; i < blockSize; i++) {
            // Calculate the index for the delayed echo
            echoIndex = i - DELAY_SAMPLES;
            // Check if echoIndex is within valid range
            if (echoIndex >= 0 && echoIndex < blockSize) {
                // Calculate attenuated echo by adding delayed sample (attenuated by half) to current sample
                echoBuffer[i] = buffer[i] + (buffer[echoIndex] >> 1);
            } else {
                // If echoIndex is out of range, copy current sample to echoBuffer
                echoBuffer[i] = buffer[i];
            }
        }

        // Write the echo effect to the output file
        fwrite(&echoBuffer, blockSize, 1, outputAudio);
    }

    fclose(inputAudio);
    fclose(outputAudio);

    return 0;
}
