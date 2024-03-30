//
// Created by edrog on 2024-03-30.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wav.h"

/**
 * Check if the file is a .wav file by checking the format field in the header
 *
 * @param header the header of the .wav file
 * @return 1 if the file is a .wav file, 0 otherwise
*/
int checkFormat(WAVHEADER header) {
    return header.format[0] == 'W' && header.format[1] == 'A' &&
           header.format[2] == 'V' && header.format[3] == 'E';
}

/**
 * Drives the program.
*/
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Invalid usage: ./pitch.exe ./soundClips/audio.wav\n");
        return 1;
    }

    char* fileWAVPath = argv[2];

    FILE* inputAudio = fopen(fileWAVPath, "rb");
    if (inputAudio == NULL) {
        printf("Error, %s not found\n", fileWAVPath);
        return 1;
    }

    WAVHEADER header;
    if(fread(&header, sizeof(WAVHEADER), 1, inputAudio) != 1) {
        printf("Error reading WAV header\n");
        return 1;
    }

    char* fileWAV = strrchr(fileWAVPath, '/') + 1;

    if (!checkFormat(header)) {
        printf("Error, %s is not a .wav file\n", fileWAV);
        return 1;
    }

    // TODO: Need to add the write data feature for the pitch functionality


    fclose(inputAudio);

    return 0;
}
