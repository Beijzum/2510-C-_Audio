# 2510-C-_Audio

## Header Properties
- If you modify the sampleRate in the header, its going to speed up / slow down the audio depending on if you increase / decrease it respectively
- soundClips consists of mp3 audio converted into .wav files via online converter. Doing this causes loss of data (the samples), so some audio manipulation can't be done thanks to this loss of data. The particular bug with online converter is the loss of subChunk2Size, which causes it to show 26 instead of chunkSize - 36.

## Sample Properties
- To mix audio together, alternate populating the data from one audio clip to the other, and x2, 3, 4... the sample rate depending on much audio tracks you are mixing together.