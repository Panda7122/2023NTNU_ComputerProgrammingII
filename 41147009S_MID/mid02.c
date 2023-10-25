#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int n, m;
int PIECE[7][4][4][4] = {0};  // I L J O S T Z
void reset() {
    PIECE[0][0][0][0] = 1;
    PIECE[0][0][1][0] = 1;
    PIECE[0][0][2][0] = 1;
    PIECE[0][0][3][0] = 1;
    PIECE[0][1][0][0] = 1;
    PIECE[0][1][0][1] = 1;
    PIECE[0][1][0][2] = 1;
    PIECE[0][1][0][3] = 1;
    PIECE[0][2][0][0] = 1;
    PIECE[0][2][1][0] = 1;
    PIECE[0][2][2][0] = 1;
    PIECE[0][2][3][0] = 1;
    PIECE[0][3][0][0] = 1;
    PIECE[0][3][0][1] = 1;
    PIECE[0][3][0][2] = 1;
    PIECE[0][3][0][3] = 1;

    PIECE[1][0][0][0] = 1;
    PIECE[1][0][1][0] = 1;
    PIECE[1][0][2][0] = 1;
    PIECE[1][0][2][1] = 1;
    PIECE[1][1][0][0] = 1;
    PIECE[1][1][0][1] = 1;
    PIECE[1][1][0][2] = 1;
    PIECE[1][1][1][0] = 1;
    PIECE[1][2][0][1] = 1;
    PIECE[1][2][1][1] = 1;
    PIECE[1][2][2][1] = 1;
    PIECE[1][2][0][0] = 1;
    PIECE[1][3][1][0] = 1;
    PIECE[1][3][1][1] = 1;
    PIECE[1][3][1][2] = 1;
    PIECE[1][3][0][2] = 1;

    PIECE[2][0][0][1] = 1;
    PIECE[2][0][1][1] = 1;
    PIECE[2][0][2][1] = 1;
    PIECE[2][0][2][0] = 1;
    PIECE[2][1][1][0] = 1;
    PIECE[2][1][1][1] = 1;
    PIECE[2][1][1][2] = 1;
    PIECE[2][1][0][0] = 1;
    PIECE[2][2][0][0] = 1;
    PIECE[2][2][1][0] = 1;
    PIECE[2][2][2][0] = 1;
    PIECE[2][2][0][1] = 1;
    PIECE[2][3][0][0] = 1;
    PIECE[2][3][0][1] = 1;
    PIECE[2][3][0][2] = 1;
    PIECE[2][3][1][2] = 1;

    PIECE[3][0][0][0] = 1;
    PIECE[3][0][0][1] = 1;
    PIECE[3][0][1][0] = 1;
    PIECE[3][0][1][1] = 1;
    PIECE[3][1][0][0] = 1;
    PIECE[3][1][0][1] = 1;
    PIECE[3][1][1][0] = 1;
    PIECE[3][1][1][1] = 1;
    PIECE[3][2][0][0] = 1;
    PIECE[3][2][0][1] = 1;
    PIECE[3][2][1][0] = 1;
    PIECE[3][2][1][1] = 1;
    PIECE[3][3][0][0] = 1;
    PIECE[3][3][0][1] = 1;
    PIECE[3][3][1][0] = 1;
    PIECE[3][3][1][1] = 1;

    PIECE[4][0][0][1] = 1;
    PIECE[4][0][0][2] = 1;
    PIECE[4][0][1][0] = 1;
    PIECE[4][0][1][1] = 1;
    PIECE[4][1][0][0] = 1;
    PIECE[4][1][1][0] = 1;
    PIECE[4][1][1][1] = 1;
    PIECE[4][1][2][1] = 1;
    PIECE[4][2][0][1] = 1;
    PIECE[4][2][0][2] = 1;
    PIECE[4][2][1][0] = 1;
    PIECE[4][2][1][1] = 1;
    PIECE[4][3][0][0] = 1;
    PIECE[4][3][1][0] = 1;
    PIECE[4][3][1][1] = 1;
    PIECE[4][3][2][1] = 1;

    PIECE[5][0][0][1] = 1;
    PIECE[5][0][1][0] = 1;
    PIECE[5][0][1][1] = 1;
    PIECE[5][0][1][2] = 1;
    PIECE[5][1][0][0] = 1;
    PIECE[5][1][1][0] = 1;
    PIECE[5][1][1][1] = 1;
    PIECE[5][1][2][0] = 1;
    PIECE[5][2][0][0] = 1;
    PIECE[5][2][0][1] = 1;
    PIECE[5][2][0][2] = 1;
    PIECE[5][2][1][1] = 1;
    PIECE[5][3][0][1] = 1;
    PIECE[5][3][1][0] = 1;
    PIECE[5][3][1][1] = 1;
    PIECE[5][3][2][1] = 1;

    PIECE[6][0][0][0] = 1;
    PIECE[6][0][0][1] = 1;
    PIECE[6][0][1][1] = 1;
    PIECE[6][0][1][2] = 1;
    PIECE[6][1][0][1] = 1;
    PIECE[6][1][1][0] = 1;
    PIECE[6][1][1][1] = 1;
    PIECE[6][1][2][0] = 1;
    PIECE[6][2][0][0] = 1;
    PIECE[6][2][0][1] = 1;
    PIECE[6][2][1][1] = 1;
    PIECE[6][2][1][2] = 1;
    PIECE[6][3][0][1] = 1;
    PIECE[6][3][1][0] = 1;
    PIECE[6][3][1][1] = 1;
    PIECE[6][3][2][0] = 1;
}
int **board;
int w, h;
int input[3];
// int inputsz = 0;
int score = 0;
int test(int height) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (PIECE[input[0] - 1][input[1] % 4][i][j]) {
                if (input[2] + j >= w || input[2] + j < 0 || height + i < 0 ||
                    height + i >= h) {
                    return 0;
                }
                if (board[height + i][input[2] + j]) return 0;
            }
        }
    }
    return 1;
}
void clear() {
    for (int i = h - 1; i >= 0; --i) {
        int cnt = 1;
        for (int j = 0; j < w; ++j) {
            if (!board[i][j]) {
                cnt = 0;
                break;
            }
        }
        if (cnt) {
            for (int j = i - 1; j >= 0; --j) {
                for (int k = 0; k < w; ++k) {
                    board[j + 1][k] = board[j][k];
                }
            }
            for (int j = 0; j < w; ++j) board[0][j] = 0;
            ++i;
            ++score;
        }
    }
}
void print(int type) {
    switch (type) {
        case 0:
            printf(" ");
            break;
        case 1:
            printf("\e[38;5;75m*\e[0m");
            break;
        case 2:
            printf("\e[38;5;20m*\e[0m");
            break;
        case 3:
            printf("\e[38;5;82m*\e[0m");

            break;
        case 4:
            printf("\e[38;5;226m*\e[0m");

            break;
        case 5:
            printf("\e[38;5;196m*\e[0m");

            break;
        case 6:
            printf("\e[38;5;208m*\e[0m");

            break;
        case 7:
            printf("\e[38;5;93m*\e[0m");

            break;
    }
}
int main() {
    reset();
    FILE *inputFile = NULL;
    // char fileName[300];
    // printf("Please enter the tetris file:");
    // fgets(fileName, 260, stdin);
    // if (fileName[strlen(fileName) - 1] == '\n')
    //     fileName[strlen(fileName) - 1] = 0x00;
    if ((inputFile = fopen("tetris.txt", "r")) == NULL) {
        printf("can not open %s\n", "tetris.txt");
        return 0;
    }
    fscanf(inputFile, "%d%d", &w, &h);
    // scanf("%d%d", &w, &h);

    board = calloc(h, sizeof(int *));
    for (int i = 0; i < h; ++i) board[i] = calloc(w, sizeof(int));
    // input = calloc(1, sizeof(int *));
    int have = 0;
    int i = 0;
    while (~fscanf(inputFile, "%d%d%d", &input[0], &input[1], &input[2])) {
        // ++inputsz;

        have = 0;
        int loc = 0;
        for (int j = 0; j < h; ++j) {
            if (test(j)) {
                loc = j;
                have = 1;
            } else
                break;
        }
        // printf("%d\n", loc);
        if (!have) {
            printf("The Game ends at line %d.\n", i + 1);
            printf("Eliminate: %d line(s)\n", score);
            for (int i = 0; i < h; ++i) {
                for (int j = 0; j < w; ++j) {
                    print(board[i][j]);
                }
                printf("\n");
            }
            break;
        }
        for (int x = 0; x < 4; ++x) {
            for (int y = 0; y < 4; ++y) {
                if (PIECE[input[0] - 1][input[1] % 4][x][y]) {
                    board[loc + x][input[2] + y] = input[0];
                }
            }
        }
        clear();
        ++i;
    }
    if (have) {
        printf("The Game ends at line %d.\n", i);
        printf("Eliminate: %d line(s)\n", score);
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                print(board[i][j]);
            }
            printf("\n");
        }
    }
    for (int i = 0; i < h; ++i) free(board[i]);
    free(board);
}