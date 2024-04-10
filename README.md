# Audio Manipulation in C

## Members

- Edro Gonzales
- Ian Chan
- Jonathan Liu
- Jason Chow
- Richard Maceda

## Overview

This is a repo dedicated to our project where we showcase different ways to manipulate audio.

## Relevant Links

- WAV Documentation [link](https://docs.fileformat.com/audio/wav/)
- WAV Header [link](https://isip.piconepress.com/projects/speech/software/tutorials/production/fundamentals/v1.0/section_02/s02_01_p05.html#:~:text=WAV%20Header%20Synopsis%3A&text=The%20size%20of%20the%20WAV,This%20is%20usually%2016.&text=Type%20of%20WAV%20format.,or%20a%20value%20of%200x01.)

## Header Property Notes
- If you modify the sampleRate in the header, its going to speed up / slow down the audio depending on if you increase / decrease it respectively
- soundClips consists of mp3 audio converted into .wav files via online converter. Doing this causes loss of data (the samples), so some audio manipulation can't be done thanks to this loss of data. The particular bug with online converter is the loss of subChunk2Size, which causes it to show 26 instead of chunkSize - 36.
- wav.h defines the wavheader struct and has __attribute__((packed)), this is done because normal struct will add some bytes padding in memory to improve performance (CPU reads data one WORD at a time, not bytes, so packing data together causes more reads than required, costing performance). By adding __attribute__((packed)), we tell the program NOT to add padding, since adding padding will cause the bytes to not align with what .wav header files are supposed to be.

## Sample Properties
- To mix audio together, alternate populating the data from one audio clip to the other, and x2, 3, 4... the sample rate depending on much audio tracks you are mixing together.