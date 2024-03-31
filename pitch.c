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
 * @param originalDataSize the original size of the audio data
 */
void updateHeader(WAVHEADER *header, double scalingFactor, long originalDataSize) {
    // Update subchunk2Size based on the scaling factor and original data size
    header->subchunk2Size *= scalingFactor;

    // Update chunkSize by adding the size of subchunk1Size, subchunk2Size, and 36 bytes (fixed header size)
    header->chunkSize = 36 + originalDataSize + (header->subchunk2Size * scalingFactor);
}

/**
 * Finds the max sample value of the audio samples.
 *
 * @param buffer block of audio samples
 * @param blockSize size of the audio samples
 * @return maximum value of the audio samples
 */
WORD findMaxSampleValue(WORD *buffer, int blockSize) {
    WORD maxSampleValue = 0;
    for (int i = 0; i < blockSize; i++) {
        if (buffer[i] > maxSampleValue) {
            maxSampleValue = buffer[i];
        }
    }
    return maxSampleValue;
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
    if (outputAudio == NULL) {
        printf("Error creating output file\n");
        fclose(inputAudio);
        return 1;
    }

    // Write original header to output file
    fwrite(&header, sizeof(WAVHEADER), 1, outputAudio);

    WORD blockSize = getBlockSize(header);
    WORD buffer[blockSize];
    double scalingFactor = 1.0;

    // Find max sample value in the entire audio file
    WORD maxSampleValue = 0;
    while (fread(&buffer, blockSize, 1, inputAudio)) {
        WORD currentMaxSampleValue = findMaxSampleValue(buffer, blockSize);
        if (currentMaxSampleValue > maxSampleValue) {
            maxSampleValue = currentMaxSampleValue;
        }
    }

    // Calculate scaling factor
    if (header.bitsPerSample == 8) {
        scalingFactor = 128.0 / maxSampleValue;
    } else if (header.bitsPerSample == 16) {
        scalingFactor = 32768.0 / maxSampleValue;
    }

    // Reset file pointer to the beginning
    fseek(inputAudio, sizeof(WAVHEADER), SEEK_SET);

    // Process audio data and write to output file
    while (fread(&buffer, blockSize, 1, inputAudio)) {
        for (int i = 0; i < blockSize; i++) {
            buffer[i] = (WORD)(buffer[i] * scalingFactor);
        }
        fwrite(&buffer, blockSize, 1, outputAudio);
    }

    fseek(inputAudio, 0, SEEK_END);
    long originalDataSize = ftell(inputAudio) - sizeof(WAVHEADER);
    fseek(inputAudio, sizeof(WAVHEADER), SEEK_SET);

    // Update header
    updateHeader(&header, scalingFactor, originalDataSize);

    // Move file pointer to the beginning and write updated header to output file
    fseek(outputAudio, 0, SEEK_SET);
    fwrite(&header, sizeof(WAVHEADER), 1, outputAudio);

    fclose(inputAudio);
    fclose(outputAudio);
    free(outputPath);

    return 0;
}
