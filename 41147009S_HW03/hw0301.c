#include <getopt.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
enum options { K, I, O };
uint8_t x = 0;
int8_t type = -1;
int main(int argc, char *argv[]) {
    x = 0;
    int c = 0;

    int wrong = 0;
    char keyFileName[300];
    char inputFileName[300];
    char outputFileName[300];
    while ((c = getopt(argc, argv, "edk:i:o:")) != -1) {
        switch (c) {
            case 'e':
                if (type != -1) wrong = 1;
                type = 0;
                break;
            case 'd':
                if (type != -1) wrong = 1;
                type = 1;
                break;
            case 'k':
                if (x & 1 << K) wrong = 1;
                x |= 1 << K;
                if (optarg == NULL) wrong = 2;
                strcpy(keyFileName, optarg);
                break;
            case 'i':
                if (x & 1 << I) wrong = 1;
                x |= 1 << I;
                if (optarg == NULL) wrong = 2;
                strcpy(inputFileName, optarg);

                break;
            case 'o':
                if (x & 1 << O) wrong = 1;
                x |= 1 << O;
                if (optarg == NULL) wrong = 2;
                strcpy(outputFileName, optarg);
                break;
            default:
                wrong = 1;
                break;
        }
    }
    if (wrong == 1 || type == -1) {
        printf("key wrong\n");
        return 0;
    }
    if (wrong == 2) {
        printf("there have parameter missing\n");
        return 0;
    }
    FILE *keyF;
    FILE *inF;
    FILE *outF;
    int8_t tmpin;
    int8_t tmpkey;
    keyF = fopen(keyFileName, "r");
    if (keyF == NULL) {
        printf("%s not exist\n", keyFileName);
        return 0;
    }
    inF = fopen(inputFileName, "r");
    if (inF == NULL) {
        printf("%s not exist\n", inputFileName);
        return 0;
    }
    outF = fopen(outputFileName, "w+");
    while (fread(&tmpin, 1, 1, inF) != 0) {
        if (fread(&tmpkey, 1, 1, keyF) == 0) {
            fseek(keyF, 0, SEEK_SET);
            if (fread(&tmpkey, 1, 1, keyF) == 0) {
                printf("key is NULL!\n");
                break;
            }
        }
        // printf("%c %c->%c\n", tmpin, tmpkey, (int8_t)(tmpin ^ tmpkey));
        int8_t tmpout = (tmpin ^ tmpkey);
        fwrite(&tmpout, 1, 1, outF);
        // printf("%ld %ld\n", ftell(inF), ftell(keyF));
    }

    fclose(keyF);
    fclose(inF);
    fclose(outF);
}