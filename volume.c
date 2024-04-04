#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "wav.h"

const int HEADER_SIZE = 44;


int checkFormat(WAVHEADER header) {
    return header.format[0] == 'W' && header.format[1] == 'A' && header.format[2] == 'V' && header.format[3] == 'E';
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Invalid usage: ./volume.exe ./ soundClips/audio.wav ./out.wav\n");
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
        return 1;
    }

}