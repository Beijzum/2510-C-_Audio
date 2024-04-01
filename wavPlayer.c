/**
 * Author: Richard Maceda A01378156
 * This will create an executable to which will play wav found in the soundclips directory
 *
 * To use use terminal and make sure current directory is where wavPlayer.c is found
 * type this: "gcc wavPlayer.c -o wavPlayerApp.exe -lwinmm"
 *
 * you can change executable name by renaming 'wavPlayerApp'
 *
 * Note:
 *  In the getWavName function there is a comment if you want to change directory of where to look for wav sounds
 */
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>

void printChoices() {
    printf("\n"
           "\n 1 - Play Again"
           "\n 2 - Play another Wav "
           "\n 3 - Exit \n");
}

void playMusic(char musicName[] ) {
    // pangitaa ang last occurence sa '\\' (Windows separator)
    char *filename = strrchr(musicName, '\\');

    if (filename == NULL) {
        // kung di mugana ang '\\' e try anf /
        filename = strrchr(musicName, '/');
    }

    if (filename != NULL) {
        // If the separator is found, move the pointer to the next character, iterate and char
        filename++;
    } else {
        // If no separator is found, use the full path
        filename = musicName;
    }

    printf("Currently playing %s \n", filename);

    if (PlaySound(TEXT(musicName), NULL, SND_SYNC) == FALSE )
    {
        printf("Error Occurred \n");
    }
}

void listWavFiles() {
    //I am heavily reliant on windows
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile(TEXT("soundclips\\*.wav"), &FindFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("No WAV files found.\n");
        return;
    }

    printf("WAV files in soundclips directory:\n");
    //using logic similar to iterator in java, but find files ending in .c
    do {
        printf("  -%s\n", FindFileData.cFileName);
    } while (FindNextFile(hFind, &FindFileData) != 0);

    printf("\n");

    FindClose(hFind);
}

char * getWavName(int nameLimit) {
    listWavFiles();

    char * musicNameBuffer = (char*) malloc(nameLimit * sizeof (char));

    printf("Enter the wav name inside the sound clip: ");
    fflush(stdin);
    scanf("%s", musicNameBuffer);

    // soundclips baby <-- Modify these if you want to chagne which directory to look into
    char *path = "soundclips\\";
    char *musicFullPath = (char *)malloc((strlen(path) + strlen(musicNameBuffer) + 1) * sizeof(char));
    strcpy(musicFullPath, path);
    strcat(musicFullPath, musicNameBuffer);

    return musicFullPath;
}

int main() {
    int nameLimit = 250;
    int dec;

    char * name = getWavName(nameLimit);
    playMusic(name);

    printChoices();
    scanf("%d", &dec);

    while (1) {
        switch(dec) {
            case 1: {
                playMusic(name);
                printChoices();

                scanf("%d", &dec);
                continue;
            }

            case 2: {
                name = getWavName(nameLimit);
                playMusic(name);
                printChoices();

                scanf("%d", &dec);
                continue;
            }

            case 3: {
                free(name);
                break;
            }
        }
        return 0;
    }
}