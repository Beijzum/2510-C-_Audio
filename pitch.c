/**
 * Changes the pitch of a given audio .wav file.
 *
 * @author Edro Gonzales A01257468
 * @version 2024
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wav.h"
#define MAX_FILE_NAME 50


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
 * Format file name output.
 *
 * @param newFileName is the pointer to the new file name
 * @param fileName to be formatted
 */
void formatFileName(char* newFileName, const char *fileName) {
    if (newFileName == NULL) {
        printf("Error: cannot allocate memory for new file name.\n");
        return;
    }

    strcpy(newFileName, "./output/");
    size_t baseFileNameLength = strlen(fileName) - 4;
    strncat(newFileName, fileName, baseFileNameLength);
    strcat(newFileName, "-pitched.wav");
}


/**
 * Drives the program.
 *
 * How to use:
 * To run program edit the configurations to have arguments "./nativeSoundClips/test.wav 1.5"
 * The first argument is the file to be pitched
 * Second argument if the pitch factor which will decrease or increase the pitch sampling of the file
 * The outputted file will be in the output folder for listening
 * WARNING: be aware the outputted .WAV can be quite loud so turn down the volume before listening
*/
int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Invalid usage: ./pitch.exe ./soundClips/audio.wav 1.0 (pitch factor)\n");
        return 1;
    }

    char* fileWAVPath = argv[1];
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
        fclose(inputAudio);
        return 1;
    }

    char *convert;
    double pitchFactor = strtod(argv[2], &convert);
    if (convert == argv[2]) {
        printf("Error: input is not a valid double.\n");
        return 1;
    }

    char* outputPath = (char*) malloc(MAX_FILE_NAME * sizeof(char));
    formatFileName(outputPath, fileWAV);
    FILE* outputAudio = fopen(outputPath, "wb");
    if (outputAudio == NULL) {
        printf("Error creating output file\n");
        fclose(inputAudio);
        return 1;
    }

    header.sampleRate = (DWORD) (header.sampleRate * pitchFactor);
    header.byteRate = header.sampleRate * header.numChannels * (header.bitsPerSample / 8);

    fwrite(&header, sizeof(WAVHEADER), 1, outputAudio);

    BYTE buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, sizeof(BYTE), sizeof(buffer), inputAudio)) > 0) {
        fwrite(buffer, sizeof(BYTE), bytesRead, outputAudio);
    }

    fclose(inputAudio);
    fclose(outputAudio);
    free(outputPath);

    return 0;
}