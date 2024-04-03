#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "wav.h"

typedef uint8_t   BYTE;
typedef uint16_t  WORD;
typedef uint32_t  DWORD;

typedef struct
{
    BYTE   chunkID[4];
    DWORD  chunkSize;
    BYTE   format[4];
    BYTE   subchunk1ID[4];
    DWORD  subchunk1Size;
    WORD   audioFormat;
    WORD   numChannels;
    DWORD  sampleRate;
    DWORD  byteRate;
    WORD   blockAlign;
    WORD   bitsPerSample;
    BYTE   subchunk2ID[4];
    DWORD  subchunk2Size;
} WavHeader;

int main() {

    FILE *outputFile;
    WavHeader header;
    int16_t *samples;
    int16_t *reversedSamples;
    size_t i;

    FILE *inputFile; //buffer for File to be opened
    inputFile = fopen("test.wav", "rb");   // Open input WAV file
    if (inputFile == NULL) {
        printf("Error: Unable to open input WAV file\n"); return 1;
    }

    fread(&header, sizeof(WAVHEADER), 1, inputFile);

    // Allocate memory
    samples = (int16_t *)malloc(header.subchunk2Size);
    if (samples == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(inputFile); return 1;
    }

    // Read audio samples
    fread(samples, sizeof(int16_t), header.subchunk2Size / sizeof(int16_t), inputFile);

    // Close input WAV file
    fclose(inputFile);

    // Allocate memory for reversed audio samples
    reversedSamples = (int16_t *)malloc(header.subchunk2Size);
    if (reversedSamples == NULL) {
        printf("Error: Memory allocation failed\n");
        free(samples); return 1;
    }

    // Reverse audio samples
    for (i = 0; i < header.subchunk2Size / sizeof(int16_t); i++) {
        reversedSamples[i] = samples[header.subchunk2Size / sizeof(int16_t) - i - 1];
    }

    // Open output WAV file
    outputFile = fopen("reversedAudio.wav", "wb");
    if (outputFile == NULL) {
        printf("Error: Unable to create output WAV file\n");
        free(samples); free(reversedSamples); //FREE if error
        return 1;
    }

    // Update file size in header
    header.chunkSize = sizeof(WavHeader) + header.subchunk2Size - 8;

    // Write WAV header
    fwrite(&header, sizeof(WavHeader), 1, outputFile);

    // Write reversed audio samples
    fwrite(reversedSamples, sizeof(int16_t), header.subchunk2Size / sizeof(int16_t), outputFile);

    // Close output WAV file
    fclose(outputFile);

    // Free allocated memory
    free(samples);
    free(reversedSamples);

    printf("WAV file reversed successfully\n");

    return 0;
}
