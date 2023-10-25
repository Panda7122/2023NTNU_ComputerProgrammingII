#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <regex.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
int DIC[4][2] = {{0, 1}, {-1, 0}, {0, -1}, {1, 0}};
int DRAW[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
struct option long_option[] = {
    {"output", required_argument, 0, 'o'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0},

};
void printhelp() {
    printf(
        "Options:\n-a <int> Mandatory. Set the value of a where the unit is "
        "pixel.\nRange: 100 - 2000\n-n <int> Mandatory. Set the value of "
        "n.\n-o, --output Set the output file name.\n-h, --help Display this "
        "description.\n");
}
typedef struct _head {
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
} __attribute__((packed)) headfile;
typedef struct pixel {
    uint8_t B, G, R;
} __attribute__((packed)) pixel;
int main(int argc, char* argv[]) {
    int c;
    int n, a;

    bool outputtag = 0, ntag = 0, atag = 0, wrong = 0;
    bool help = 0;
    char* outputFileName = NULL;
    char* endptr;
    while ((c = getopt_long(argc, argv, "o:n:a:h", long_option, NULL)) != -1) {
        switch (c) {
            case 'o':
                if (outputtag)
                    wrong = 1;
                else {
                    outputFileName = strdup(optarg);
                    outputtag = 1;
                }
                break;
            case 'n':
                if (ntag)
                    wrong = 1;
                else {
                    n = strtol(optarg, &endptr, 10);
                    ntag = 1;
                    if (*endptr != 0x00) {
                        wrong = 1;
                    }
                }
                break;
            case 'a':
                if (atag)
                    wrong = 1;
                else {
                    a = strtol(optarg, &endptr, 10);
                    atag = 1;
                    if (*endptr != 0x00) {
                        wrong = 1;
                    }
                }
                break;
            case 'h':
                help = 1;
                break;
            default:
                break;
        }
    }
    if (help) {
        printhelp();
        if (outputFileName) {
            free(outputFileName);
        }
        return 0;
    }
    if (wrong || (atag == 0 || ntag == 0) || a > 2000 || a < 100 || n <= 0) {
        printf("error\nplease use ./fin01 --help to get more detail\n");
        if (outputFileName) {
            free(outputFileName);
        }
        return 0;
    }
    if (!outputFileName) {
        outputFileName = strdup("output.bmp");
    }
    double b = (a * 2) / (1 + sqrt(5));
    // printf("%d %d\n", a, b);
    int fd = open(outputFileName, O_WRONLY | O_CREAT, 0666);
    int padding = 0;
    while (((a + (int)b) * 3 + padding) & 3) ++padding;
    headfile header;
    header.BM[0] = 'B';
    header.BM[1] = 'M';

    header.size = ((a + (int)b) * 3 + padding) * a + sizeof(header);
    header.Reserved = 0;
    header.offset = sizeof(header);
    header.headerSize = 40;
    header.width = a + (int)b;
    header.height = a;
    header.planes = 1;
    header.bitperpixel = 24;
    header.compression = 0;
    header.imageSize = (a + (int)b + padding) * a;
    header.horizontal = 0;
    header.vertical = 0;
    header.palette = 0;
    header.important = 0;
    write(fd, &header, sizeof(header));
    for (int i = 0; i < a; ++i) {
        for (int j = 0; j < a + (int)b; ++j) {
            pixel p = {255, 255, 255};
            write(fd, &p, sizeof(pixel));
        }
        for (int j = 0; j < padding; ++j) {
            char p = 0;
            write(fd, &p, sizeof(char));
        }
    }
    close(fd);

    fd = open(outputFileName, O_RDWR, 0666);
    int8_t* addr =
        mmap(NULL, header.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    double nowa = a, nowb = b;
    int rotate = 0;
    double nowlocate[2] = {0, 0};
    // printf("%d\n", header.size);
    for (int i = 0; i < n; ++i) {
        nowlocate[0] += nowa * DIC[rotate][0];
        nowlocate[1] += nowa * DIC[rotate][1];
        // printf("%lf %lf", nowa, nowb);
        // printf(" (%lf %lf)\n", nowlocate[0], nowlocate[1]);
        for (int j = 0; j < nowa; ++j) {
            (*(pixel*)(addr + sizeof(header) +
                       ((int)nowlocate[0] < 0 ? 0 : (int)nowlocate[0]) *
                           ((header.width) * 3 + padding) +
                       ((int)nowlocate[1] < 0 ? 0 : (int)nowlocate[1]) * 3))
                .B = 0;
            (*(pixel*)(addr + sizeof(header) +
                       ((int)nowlocate[0] < 0 ? 0 : (int)nowlocate[0]) *
                           ((header.width) * 3 + padding) +
                       ((int)nowlocate[1] < 0 ? 0 : (int)nowlocate[1]) * 3))
                .G = 0;
            (*(pixel*)(addr + sizeof(header) +
                       ((int)nowlocate[0] < 0 ? 0 : (int)nowlocate[0]) *
                           ((header.width) * 3 + padding) +
                       ((int)nowlocate[1] < 0 ? 0 : (int)nowlocate[1]) * 3))
                .R = 0;
            nowlocate[0] += DRAW[rotate][0];
            nowlocate[1] += DRAW[rotate][1];

            // printf("%d %d %d\n", j, nowlocate[0], nowlocate[1]);
        }
        // nowlocate[0] -= DRAW[rotate][0];
        // nowlocate[1] -= DRAW[rotate][1];
        //  printf("\n");
        nowa = nowb;
        nowb = (1 + sqrt(5)) * nowa / 2 - nowa;

        rotate++;
        rotate %= 4;
    }
    munmap(addr, header.size);
    close(fd);

    free(outputFileName);
}