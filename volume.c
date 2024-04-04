#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wav.h"

int checkFormat(WAVHEADER header) {
    return header.format[0] == 'W' && header.format[1] == 'A' && header.format[2] == 'V' && header.format[3] == 'E';
}


int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Invalid usage: ./volume.exe ./ soundClips/audio.wav ./out.wav factor\n");
        return 1;
    }

    char* fileWAVPath = argv[1];
    FILE* inputAudio = fopen(fileWAVPath, "rb");
    if (inputAudio == NULL) {
        printf("Error: %s not found\n", fileWAVPath);
        return 1;
    }

    WAVHEADER header;
    if(fread(&header, sizeof(WAVHEADER), 1, inputAudio) != 1) {
        printf("Error reading WAV header\n");
        return 1;
    }

    char* fileWAV = strrchr(fileWAVPath, '/') + 1;
    if(!checkFormat(header)) {
        printf("Error: %s is not a .wav file\n", fileWAV);
        fclose(inputAudio);
        return 1;
    }

    char* fileOutPath = argv[2];
    FILE *outputAudio = fopen(fileOutPath, "w");
    if (outputAudio == NULL) {
        printf("Error: could not open %s\n", fileOutPath);
        return 1;
    }

    float factor = atof(argv[3]);

    int16_t buffer;
    while (fread(&buffer, sizeof(int16_t), 1, inputAudio)) {
        //update volume, factor is a floating point number
        buffer *= factor;
        fwrite(&buffer, sizeof(int16_t), 1, outputAudio);
    }

    fclose(inputAudio);
    fclose(outputAudio);
    free(fileOutPath);
    return 0;
}