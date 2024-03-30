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
 * @return newFileName formatted with copy prepended
 */
void formatFileName(char *newFileName, const char *fileName) {
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
        fclose(inputAudio);
        return 1;
    }

    fseek(inputAudio, sizeof(WAVHEADER), SEEK_SET);

    double pitchFactor = 1.2;
    int newAudioSize = (int) (header.subchunk2Size / pitchFactor);
    int16_t* audioData = malloc(header.subchunk2Size);
    int16_t* newAudioData = malloc(newAudioSize * sizeof(int16_t));
    fread(audioData, sizeof(int16_t), header.subchunk2Size / sizeof(int16_t), inputAudio);

    for (int i = 0; i < newAudioSize; i++) {
        int sourceIndex = (int)(i * pitchFactor);
        newAudioData[i] = audioData[sourceIndex];
    }

    char newFileName[MAX_FILE_NAME];
    formatFileName(newFileName, fileWAV);

    FILE* outputAudio = fopen(newFileName, "wb");
    if (outputAudio == NULL) {
        printf("Error creating output file\n");
        fclose(inputAudio);
        free(audioData);
        free(newAudioData);
        return 1;
    }

    header.subchunk2Size = newAudioSize * sizeof(int16_t);
    fwrite(&header, sizeof(WAVHEADER), 1, outputAudio);
    fwrite(newAudioData, sizeof(int16_t), newAudioSize, outputAudio);

    fclose(inputAudio);
    fclose(outputAudio);
    free(newAudioData);
    free(audioData);

    return 0;
}
