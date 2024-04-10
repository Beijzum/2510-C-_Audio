/**
 * Change volume of a given .wav audio file.
 * @author Ian Chan A00910012
 * @version 2024
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wav.h"

int checkFormat(WAVHEADER header) {
    return header.format[0] == 'W' && header.format[1] == 'A' && header.format[2] == 'V' && header.format[3] == 'E';
}

/**
 * How to use:
 * 1. Compile the program into an .exe file (for example, clicking the "Run" button in Visual Studio Code)
 * 2. Use terminal and enter: "./volume ./nativeSoundClips/audio.wav ./output/out.wav volumeFactor(float)"
 * Example: ./volume ./nativeSoundClips/test.wav ./output/testVolume1.wav 2.0
 * This example will increase the volume (by double the original volume) of the test.wav file and output it to testVolume1.wav
 */

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Invalid usage: ./volume ./nativeSoundClips/test.wav ./output/testVolume.wav 2.0\n");
        return 1;
    }

    char* fileWAVPath = argv[1]; // input file path in argv[1]
    FILE* inputAudio = fopen(fileWAVPath, "rb"); // open input file and read in binary mode
    if (inputAudio == NULL) {
        printf("Error: %s not found\n", fileWAVPath);
        return 1;
    }

    WAVHEADER header; // create a header of type WAVHEADER
    if(fread(&header, sizeof(WAVHEADER), 1, inputAudio) != 1) { // read the header information from the input file and store it in header variable
        printf("Error reading WAV header\n");
        return 1;
    }

    char* fileWAV = strrchr(fileWAVPath, '/') + 1; // get file name, strrchr returns a pointer to the last occurrence of '/' in the string then + 1 to get file name
    if(!checkFormat(header)) { // check if the file is a .wav file, given the header
        printf("Error: %s is not a .wav file\n", fileWAV);
        fclose(inputAudio); // need to close the file because it was opened and read
        return 1;
    }

    char *convertVolume; // convertVolume is the pointer to the first character in the string that is not a valid double
    double volumeFactor = strtod(argv[3], &convertVolume); 
    if (convertVolume == argv[3]) { // if convertVolume is equal to argv[3] then the input is not a valid double
        printf("Error: factor input, '%s', is not a valid float.\n", argv[3]);
        return 1;
    }

    char* fileOutPath = argv[2]; // output file path in argv[2] of the command line
    FILE *outputAudio = fopen(fileOutPath, "wb"); // open output file and write in binary mode
    if (outputAudio == NULL) {
        printf("Error: could not create output file");
        return 1;
    }

    fwrite(&header, sizeof(WAVHEADER), 1, outputAudio); // write new header to output file

    uint16_t buffer[1024]; // 1024 bytes total. WAV is a 16-bit format, so we use int16_t to store the audio data, which means each sample is 2 bytes
    size_t bytesRead;
    while ((bytesRead = fread(buffer, sizeof(uint16_t), sizeof(buffer) / sizeof(uint16_t), inputAudio)) > 0) { // read inputAudio file and store into buffer variable
        for (size_t i = 0; i < bytesRead; i++) { // loop through buffer and multiply each byte by volumeFactor
            buffer[i] = (uint16_t)(buffer[i] * volumeFactor); // cast to int16_t to avoid overflow
        }
        fwrite(buffer, sizeof(uint16_t), bytesRead, outputAudio); // write updated buffer (changed volume) to output file
    }

    fclose(inputAudio);
    fclose(outputAudio);
    return 0;
}