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
 * Check if the file is a .wav file by checking the format field in the header
 *
 * @param header the header of the .wav file
 * @return stream block of bytes
 */
int getBlockSize(WAVHEADER header) {
    int channels = header.numChannels;
    int BytesPerSample = header.bitsPerSample / 8;
    return channels * BytesPerSample;
}


/**
 * Format file name output.
 *
 * @param newFileName is the pointer to the new file name
 * @param fileName to be formatted
 * @return newFileName formatted with copy prepended
 */
char* formatFileName(const char *fileName) {
    char* newFileName = (char*) malloc(MAX_FILE_NAME * sizeof(char));
    if (newFileName == NULL) {
        printf("Error: cannot allocate memory for new file name.\n");
        return NULL;
    }

    strcpy(newFileName, "./output/");
    size_t baseFileNameLength = strlen(fileName) - 4;
    strncat(newFileName, fileName, baseFileNameLength);
    strcat(newFileName, "-pitched.wav");
    return newFileName;
}

/**
 * Updates the header after increasing the sampling rate by 1.5x.
 *
 * @param header the header of the .wav file
 * @param scalingFactor the scaling factor applied to the audio samples
 */
void updateHeader(WAVHEADER *header, double scalingFactor) {
    header->subchunk2Size *= scalingFactor;
    header->chunkSize = 36 + header->subchunk2Size;
}

/**
 * Drives the program.
*/
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Invalid usage: ./pitch.exe ./soundClips/audio.wav\n");
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

    char* outputPath = formatFileName(fileWAV);
    FILE* outputAudio = fopen(outputPath, "wb");

    WORD blockSize = getBlockSize(header);
    WORD buffer[blockSize];
    WORD maxSampleValue = 0;
    double scalingFactor = 1.0;

    while (fread(&buffer, blockSize, 1, inputAudio)) {
        for (int i = 0; i < blockSize; i++) {
            buffer[i] = (buffer[i] * 3) / 2;
            if (buffer[i] > maxSampleValue) {
                maxSampleValue = buffer[i];
            }
        }

        if (header.bitsPerSample == 8) {
            scalingFactor = 128.0 / maxSampleValue;
        } else if (header.bitsPerSample == 16) {
            scalingFactor = 32768.0 / maxSampleValue;
        }
        for (int i = 0; i < blockSize; i++) {
            buffer[i] = (WORD)(buffer[i] * scalingFactor);
        }

        fwrite(&buffer, blockSize, 1, outputAudio);
    }

    updateHeader(&header, scalingFactor);

    fseek(outputAudio, 0, SEEK_SET);
    fwrite(&header, sizeof(WAVHEADER), 1, outputAudio);

    fclose(inputAudio);
    fclose(outputAudio);
    free(outputPath);

    return 0;
}
