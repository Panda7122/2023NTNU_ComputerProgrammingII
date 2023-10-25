#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define MIN(a, b) (a < b ? a : b)
char bpmName[2048];
char fileName[2048];
typedef struct _bmpHeader {
    char BM[2];
    int32_t size;
    int32_t Reserved;
    int32_t offset;

    int32_t headerSize;
    int32_t width;
    int32_t height;
    int16_t planes;
    int16_t bitperpixel;
    int32_t compression;
    int32_t imageSize;
    int32_t horizontal;
    int32_t vertical;
    int32_t palette;
    int32_t important;
} __attribute__((packed)) bmpHeader;
typedef struct _fileHeader {
    int32_t fileNameLength;
    int32_t filesize;
} __attribute__((packed)) fileHeader;
bool check(bmpHeader h) {
    if (h.BM[0] != 'B' || h.BM[1] != 'M') return 0;
    return 1;
}
int main(int argc, char* argv[]) {
    int c = 0;
    int opt = 0;
    int wrong = 0;
    if (argc > 4) {
        printf("there have too many option\n");
        return 0;
    } else if (argc < 3) {
        printf("there have too less option\n");
        return 0;
    }
    while ((c = getopt(argc, argv, "ie")) != -1) {
        switch (c) {
            case 'i':
                if (opt)
                    wrong = 1;
                else
                    opt = 1;
                break;
            case 'e':
                if (opt)
                    wrong = 1;
                else
                    opt = 2;
                break;
            default:
                wrong = 1;
                break;
        }
    }
    if (wrong || !opt) {
        printf("error wrong argument\n");
        return 0;
    }
    if (opt == 1) {
        strcpy(fileName, argv[argc - 2]);
        strcpy(bpmName, argv[argc - 1]);
        int fdBPM = open(bpmName, O_RDWR | O_APPEND, 0666);
        int fdFile = open(fileName, O_RDWR, 0666);
        if (fdBPM == -1 || fdFile == -1) {
            printf("file open error\n");
            return 0;
        }
        bmpHeader head;
        read(fdBPM, &head, sizeof(bmpHeader));
        if (!check(head)) {
            printf("this is not a BMP\n");
            close(fdBPM);
            close(fdFile);
            return 0;
        }
        int32_t size = lseek(fdFile, 0, SEEK_END);
        int32_t length = strlen(fileName);
        write(fdBPM, &length, sizeof(int32_t));
        write(fdBPM, &size, sizeof(int32_t));
        write(fdBPM, fileName, strlen(fileName));
        close(fdFile);
        fdFile = open(fileName, O_RDWR, 0666);
        uint8_t buffett[1024];
        int nbytes;
        while ((nbytes = read(fdFile, buffett, 1024)) > 0) {
            write(fdBPM, buffett, nbytes);
        }
        close(fdBPM);
        close(fdFile);
    } else {
        strcpy(bpmName, argv[argc - 1]);
        int fdBPM = open(bpmName, O_RDWR, 0666);
        bmpHeader header;
        fileHeader fheader;
        read(fdBPM, &header, sizeof(bmpHeader));
        if (!check(header)) {
            printf("this is not a BMP\n");
            close(fdBPM);
            return 0;
        }
        lseek(fdBPM, header.size, SEEK_SET);
        while (read(fdBPM, &fheader, sizeof(fileHeader)) > 0) {
            read(fdBPM, fileName, fheader.fileNameLength * sizeof(char));
            fileName[fheader.fileNameLength] = 0x00;
            int fdFile = open(fileName, O_CREAT | O_WRONLY, 0666);
            uint8_t buffett[1024];
            int cnt = 0;
            while (cnt < fheader.filesize) {
                int n = read(fdBPM, buffett, MIN(1024, fheader.filesize - cnt));
                write(fdFile, buffett, n);
                cnt += n;
            }
            close(fdFile);
        }
        close(fdBPM);
    }
}