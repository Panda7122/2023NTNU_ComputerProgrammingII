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
static struct option long_options[] = {
    /* These options set a flag. */
    {"width", required_argument, 0, 'w'},
    {"height", required_argument, 0, 'h'},
    {"number", required_argument, 0, 'n'},
    {"input", required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'},
    {"help", no_argument, 0, 0},
    {0, 0, 0, 0}};
void printHelp() {
    printf(
        "-x: mandatory, x value.\n"
        "-y: mandatory, y value.\n"
        "-w, --width: mandatory, w value.\n"
        "-h, --height: mandatory, h value.\n"
        "-n, --number: optional, n value. Default n is 2.\n"
        "-i, --input: mandatory, the input BMP file name.\n"
        "-o, --output: optional, the output BMP file name. Default name is "
        "output.bmp.\n"
        "-help: print the usage help.\n");
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
    long x = -1;
    long y = -1;
    int w = -1;
    int h = -1;
    int n = 2;
    int help = 0;
    char* input = NULL;
    char* output = NULL;
    output = strdup("output.bmp");
    char* endptr = NULL;
    while ((c = getopt_long(argc, argv, "x:y:w:h:n:i:o:", long_options,
                            NULL)) != -1) {
        switch (c) {
            case 'x':
                x = strtol(optarg, &endptr, 10);
                if (*endptr != 0x00) {
                    x = -1;
                }
                break;
            case 'y':
                y = strtol(optarg, &endptr, 10);
                if (*endptr != 0x00) {
                    y = -1;
                }
                break;
            case 'w':
                w = strtol(optarg, &endptr, 10);
                if (*endptr != 0x00) {
                    w = -1;
                }
                break;
            case 'h':
                h = strtol(optarg, &endptr, 10);
                if (*endptr != 0x00) {
                    h = -1;
                }
                break;
            case 'n':
                n = strtol(optarg, &endptr, 10);
                if (*endptr != 0x00) {
                    n = -1;
                }
                break;
            case 'i':
                if (input != NULL) {
                    free(input);
                    input = NULL;
                }
                input = strdup(optarg);

                break;
            case 'o':
                if (output != NULL) {
                    free(output);
                    output = NULL;
                }
                output = strdup(optarg);
                break;
            case 0:
                printHelp();
                help = 1;
                break;
            case '?':
                printf(
                    "unknown argument\nplease use ./hw0303 --help to get more "
                    "information\n");
                help = 1;
                break;
        }
    }
    if (help) {
        if (input) free(input);
        if (output) free(output);

        return 0;
    }
    if (!(~x & ~y & ~w & ~h) || input == NULL ||
        output == NULL) {  // any of xywh is -1 or io is NULL
        if (input) free(input);
        if (output) free(output);
        printf(
            "you need more arguments\nplease use ./hw0303 --help to get more "
            "information\n");
        return 0;
    }
    if (x < 0 || y < 0 || w < 0 || h < 0) {
        if (input) free(input);
        if (output) free(output);
        printf("x, y, w, h should greater or equal then 0\n");
        return 0;
    }
    int inputBMPfd = open(input, O_RDONLY);
    int outputBMPfd = open(output, O_WRONLY | O_CREAT, 0666);
    if (inputBMPfd == -1) {
        perror("can not open input file");
        if (input) free(input);
        if (output) free(output);
        return 0;
    }
    if (outputBMPfd == -1) {
        perror("can not write|create output file");
        if (input) free(input);
        if (output) free(output);
        return 0;
    }
    uint8_t buffett[1024];
    int nbytes = 0;
    int size = 0;
    while ((nbytes = read(inputBMPfd, buffett, 1024)) > 0) {
        write(outputBMPfd, buffett, nbytes);
        size += nbytes;
    }
    close(inputBMPfd);
    close(outputBMPfd);
    outputBMPfd = open(output, O_RDWR, 0666);

    // if (lseek(outputBMPfd, 0, SEEK_SET) == -1) {
    //     perror("lseek error");
    //     if (input) free(input);
    //     if (output) free(output);
    //     return 0;
    // }
    int8_t* addr;
    addr = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, outputBMPfd, 0);
    // printf("%ld\n", sizeof(headfile));
    headfile* head = (headfile*)addr;
    // printf("st\n");
    int padding = 0;
    while ((head->width * (head->bitperpixel / 8) + padding) % 4) {
        ++padding;
    }
    pixel* now;
    pixel new;
    long long R, G, B;
    int counter = 0;
    x = head->height - (x + h);
    if (x < 0) {
        h += x;
        x = 0;
    }
    for (long long i = x; i < x + h && i < head->height; i += n) {
        for (long long j = y; j < y + w && j < head->width; j += n) {
            B = 0;
            G = 0;
            R = 0;
            counter = 0;
            for (long long kx = 0; kx < n && i + kx < head->height; ++kx) {
                for (long long ky = 0; ky < n && j + ky < head->width; ++ky) {
                    now = ((pixel*)(addr + sizeof(headfile) +
                                    ((i + kx) * (head->width) + (j + ky)) *
                                        (head->bitperpixel / 8) +
                                    (i + kx) * padding));
                    ++counter;
                    B += now->B;
                    G += now->G;
                    R += now->R;
                }
            }
            new.B = B / (counter);
            new.G = G / (counter);
            new.R = R / (counter);

            for (long long kx = 0; kx < n && i + kx < head->height; ++kx) {
                for (long long ky = 0; ky < n && j + ky < head->width; ++ky) {
                    memcpy((addr + sizeof(headfile) +
                            ((i + kx) * (head->width) + (j + ky)) *
                                (head->bitperpixel / 8) +
                            (i + kx) * padding),
                           &new, head->bitperpixel / 8);
                }
            }
        }
    }
    munmap(addr, size);
    close(outputBMPfd);
    if (input) free(input);
    if (output) free(output);
}