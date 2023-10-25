#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
enum pictureType { BM = 0, BA, CI, CP, IC, PT };
typedef struct _pic {
    enum pictureType type;
    uint32_t size;
    int32_t startLoc;
    int32_t width;
    int32_t height;
    int16_t pixelSize;
    int32_t compressWay;
    int32_t horizontalResolution;
    int32_t verticalResolution;
    uint32_t colorPalette;
    int32_t importantColors;
    int32_t DIBSize;
    char *fileName;
} picture;
typedef struct _pixel {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} pixel;
picture *init(char *locate) {
    FILE *pictureFILE;
    picture *ret = NULL;
    pictureFILE = fopen(locate, "rb");
    if (pictureFILE == NULL) return ret;
    ret = calloc(1, sizeof(picture));
    ret->fileName = calloc(strlen(locate) + 1, sizeof(char));
    // ret->fileName = locate;
    strncpy(ret->fileName, locate, strlen(locate));
    char DIBtype[3] = {0};
    fread(DIBtype, 1, 2, pictureFILE);
    if (DIBtype[0] == 'B' && DIBtype[1] == 'M') {
        ret->type = 0;
    } else if (DIBtype[0] == 'B' && DIBtype[1] == 'A') {
        ret->type = 1;
    } else if (DIBtype[0] == 'C' && DIBtype[1] == 'I') {
        ret->type = 2;
    } else if (DIBtype[0] == 'C' && DIBtype[1] == 'P') {
        ret->type = 3;
    } else if (DIBtype[0] == 'I' && DIBtype[1] == 'C') {
        ret->type = 4;
    } else if (DIBtype[0] == 'P' && DIBtype[1] == 'T') {
        ret->type = 5;
    }
    char size[5] = {0};
    fread(&ret->size, 1, 4, pictureFILE);
    // printf("%u\n", ret->size);
    int sz = 0;
    int32_t unuse;
    fread(&unuse, 1, 4, pictureFILE);
    fread(&ret->startLoc, 1, 4, pictureFILE);
    if (ret->type == BM) {
        fread(&ret->DIBSize, 1, 4, pictureFILE);
        fread(&ret->width, 1, 4, pictureFILE);
        fread(&ret->height, 1, 4, pictureFILE);
        fread(&unuse, 1, 2, pictureFILE);
        fread(&ret->pixelSize, 1, 2, pictureFILE);
        fread(&ret->compressWay, 1, 4, pictureFILE);
        fread(&unuse, 1, 4, pictureFILE);
        fread(&ret->horizontalResolution, 1, 4, pictureFILE);
        fread(&ret->verticalResolution, 1, 4, pictureFILE);
        fread(&ret->colorPalette, 1, 4, pictureFILE);
        if (!ret->colorPalette) {
            ret->colorPalette = 1 << ret->pixelSize;
        }
        fread(&ret->importantColors, 1, 4, pictureFILE);
    }
    fclose(pictureFILE);
    return ret;
}
pixel *getPixel(const picture *pic, const int row, const int col) {
    pixel *ret = NULL;
    if (pic == NULL) return ret;
    FILE *file = fopen(pic->fileName, "r");
    fseek(file, pic->startLoc, SEEK_SET);
    uint8_t tmp[10];
    ret = calloc(1, sizeof(pixel));
    // if (row - 1 >= 0)
    fseek(file, ftell(file) + (row)*pic->width * pic->pixelSize / 8, SEEK_SET);
    // if (col - 1 >= 0) {
    fseek(file, ftell(file) + (col)*pic->pixelSize / 8, SEEK_SET);
    // }
    fread(tmp, 1, pic->pixelSize / 8, file);
    ret->B = tmp[0];
    ret->G = tmp[1];
    ret->R = tmp[2];
    fclose(file);
    return ret;
}
void changePixel(picture *pic, const pixel *pxl, const int row, const int col) {
    if (pic == NULL) return;
    if (pxl == NULL) return;
    FILE *file = fopen(pic->fileName, "w");
    fseek(file, pic->startLoc, SEEK_SET);
    // if (row - 1 > 0)
    fseek(file, ftell(file) + (row)*pic->width * pic->pixelSize / 8, SEEK_SET);
    // if (col - 1 > 0) {
    fseek(file, ftell(file) + (col)*pic->pixelSize / 8, SEEK_SET);
    // }
    fwrite(&(pxl->B), 1, 1, file);
    fwrite(&(pxl->G), 1, 1, file);
    fwrite(&(pxl->R), 1, 1, file);
    fclose(file);
}

void freePixel(pixel **pxl) {
    free(*pxl);
    *pxl = NULL;
}
int main() {
    picture *original;
    char stringIn[300];
    char stringOut[300];
    printf("Input image: ");
    fgets(stringIn, 300, stdin);
    printf("Output image: ");
    fgets(stringOut, 300, stdin);
    if (stringIn[strlen(stringIn) - 1] == '\n')
        stringIn[strlen(stringIn) - 1] = 0x00;
    if (stringOut[strlen(stringOut) - 1] == '\n')
        stringOut[strlen(stringOut) - 1] = 0x00;
    original = init(stringIn);
    if (original == NULL) {
        return 0;
    }
    picture *new = NULL;
    new = calloc(1, sizeof(picture));
    memcpy(new, original, sizeof(picture));
    new->fileName = calloc(strlen(stringOut) + 1, sizeof(char));
    strncpy(new->fileName, stringOut, strlen(stringOut));
    FILE *file = fopen(original->fileName, "rb");
    FILE *out = fopen(new->fileName, "w");
    char Data[300];
    uint8_t *tmp;
    tmp = calloc(new->pixelSize / 8, sizeof(uint8_t));
    // fseek(file, 0, SEEK_SET);
    // fseek(out, 0, SEEK_SET);
    fread(Data, original->startLoc, 1, file);
    fwrite(Data, original->startLoc, 1, out);
    for (int i = 0; i < original->startLoc; ++i) {
    }
    int i = 0, j = 0;
    // int tmp[3] = {0, 0, 0};
    for (int i = 0; i < original->height; ++i) {
        for (int j = 0; j < original->width; ++j) {
            fwrite(tmp, new->pixelSize / 8, 1, out);
        }
    }
    fseek(out, new->startLoc, SEEK_SET);
    fseek(file, original->startLoc, SEEK_SET);
    int row, col;
    while (!feof(out)) {
        fread(tmp, new->pixelSize / 8, 1, file);
        row = i / 2;
        col = j / 2;
        fseek(out, new->startLoc, SEEK_SET);
        fseek(out, ftell(out) + (row) * new->width *new->pixelSize / 8,
              SEEK_SET);
        fseek(out, ftell(out) + (col) * new->pixelSize / 8, SEEK_SET);
        fwrite(tmp, new->pixelSize / 8, 1, out);
        row = i / 2;
        col = original->width - (j / 2) - 1;
        fseek(out, new->startLoc, SEEK_SET);
        fseek(out, ftell(out) + (row) * new->width *new->pixelSize / 8,
              SEEK_SET);
        fseek(out, ftell(out) + (col) * new->pixelSize / 8, SEEK_SET);
        fwrite(tmp, new->pixelSize / 8, 1, out);
        row = original->height - (i / 2) - 1;
        col = j / 2;
        fseek(out, new->startLoc, SEEK_SET);
        fseek(out, ftell(out) + (row) * new->width *new->pixelSize / 8,
              SEEK_SET);
        fseek(out, ftell(out) + (col) * new->pixelSize / 8, SEEK_SET);
        fwrite(tmp, new->pixelSize / 8, 1, out);
        row = original->height - (i / 2) - 1;
        col = original->width - (j / 2) - 1;
        fseek(out, new->startLoc, SEEK_SET);
        fseek(out, ftell(out) + (row) * new->width *new->pixelSize / 8,
              SEEK_SET);
        fseek(out, ftell(out) + (col) * new->pixelSize / 8, SEEK_SET);
        fwrite(tmp, new->pixelSize / 8, 1, out);

        ++j;
        if (j >= new->width) {
            j = 0;
            i++;
            if (i >= new->height) break;
        }
    }
    free(tmp);
    fclose(file);
    fclose(out);
    // free(new->fileName);
    // free(new);
    // free(original->fileName);
    // free(original);
}