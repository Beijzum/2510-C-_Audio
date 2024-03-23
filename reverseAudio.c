#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "wav.h"

int check_format(WAVHEADER header);
int get_block_size(WAVHEADER header);

int main(int argc, char *argv[])
{
    // Open input file for reading
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Error, could not open file %s\n", argv[1]);
        return 1;
    }

    // Read header into an array
    WAVHEADER HEADER;
    fread(&HEADER, sizeof(WAVHEADER), 1, input);

    // Use check_format to ensure WAV format
    if (!check_format(HEADER))
    {
        printf("Error, %s is not a .wav file\n", argv[1]);
    }

    // Open output file for writing
    FILE *output = fopen(argv[2], "w");

    // Write header to file
    fwrite(&HEADER, sizeof(WAVHEADER), 1, output);

    // Use get_block_size to calculate size of block
    int bps = get_block_size(HEADER);

    // Write reversed audio to file
    uint16_t buffer[bps];

    fseek(input, -bps, SEEK_END);

    while (ftell(input) >= sizeof(WAVHEADER))
    {
        fread(&buffer, bps, 1, input);
        fwrite(&buffer, bps, 1, output);
        fseek(input, -bps*2, SEEK_CUR);
    }

    // close files
    fclose(input);
    fclose(output);

    return 0;
}

int check_format(WAVHEADER header)
{
    // Header format spells out WAVE
    if (header.format[0] == 'W' && header.format[1] == 'A' && header.format[2] == 'V' && header.format[3] == 'E') return 1;
    else return 0;
}

int get_block_size(WAVHEADER header)
{
    // # Channels x Bytes Per Sample
    int channels = header.numChannels;
    int ByPS = header.bitsPerSample/8;
    return channels*ByPS;
}