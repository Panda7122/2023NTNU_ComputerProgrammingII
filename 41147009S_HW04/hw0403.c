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
#include <time.h>
#include <unistd.h>

#define PATH "/usr/share/dict/american-english"
typedef struct _vector {
    char** Value;
    size_t size;
    size_t capacity;
} vectorStr;
vectorStr dic;
const int color[3] = {243, 46, 11};
void push(char* str) {
    if (dic.size >= dic.capacity) {
        dic.capacity <<= 1;
        dic.Value = realloc(dic.Value, sizeof(char*) * dic.capacity);
        for (int i = dic.capacity >> 1; i < dic.capacity; ++i) {
            dic.Value[i] = NULL;
        }
    }
    dic.Value[dic.size] = strdup(str);
    for (int i = 0; i < 5; ++i) {
        if (dic.Value[dic.size][i] >= 'A' && dic.Value[dic.size][i] <= 'Z') {
            dic.Value[dic.size][i] += 32;
        }
    }
    ++dic.size;
}
int main(int argc, char* argv[]) {
    srand(time(NULL));
    FILE* dictionary = fopen(PATH, "r");
    if (dictionary == NULL) {
        printf("can not open %s\n", PATH);
        return 0;
    }
    dic.capacity = 16;
    dic.size = 0;
    dic.Value = calloc(dic.capacity, sizeof(char*));
    for (int i = 0; i < dic.capacity; ++i) {
        dic.Value[i] = NULL;
    }
    char nowStr[1024];
    while (!feof(dictionary)) {
        if (fgets(nowStr, 1024, dictionary) != NULL) {
            if (nowStr[strlen(nowStr) - 1] == '\n')
                nowStr[strlen(nowStr) - 1] = 0x00;
            if (strlen(nowStr) == 5) {
                int check = 1;
                for (int i = 0; i < 5; ++i) {
                    if (!((nowStr[i] >= 'a' && nowStr[i] <= 'z') ||
                          (nowStr[i] >= 'A' && nowStr[i] <= 'Z'))) {
                        check = 0;
                        break;
                    }
                }
                if (check) {
                    push(nowStr);
                }
            }
        }
    }

    int now = rand() % dic.size;
    char guess[1024];
    printf("Answer: %s\n", dic.Value[now]);
    int cnt = 1;
    while (1) {
        int Ans[5] = {0};  // 1->A, 2->B
        int used[5] = {0};
        bool have[26] = {0};
        printf("Guess %d: ", cnt);
        fgets(guess, 1024, stdin);
        if (guess[strlen(guess) - 1] == '\n') guess[strlen(guess) - 1] = 0x00;
        if (strlen(guess) != 5) {
            printf(
                "you should input a string with 5 letters\nPlease input "
                "again\n");
        } else {
            for (int i = 0; i < 5; ++i) {
                if (guess[i] >= 'A' && guess[i] <= 'Z') {
                    guess[i] += 32;
                }
            }
            int check = 1;
            for (int i = 0; i < 5; ++i) {
                if (!(guess[i] >= 'a' && guess[i] <= 'z')) {
                    check = 0;
                    break;
                }
            }
            if (check) {
                int indic = 0;
                for (int i = 0; i < dic.size; ++i) {
                    if (strcmp(dic.Value[i], guess) == 0) {
                        indic = 1;
                        break;
                    }
                }
                if (indic) {
                    for (int i = 0; i < 5; ++i) {
                        if (guess[i] == dic.Value[now][i]) {
                            Ans[i] = 1;
                            used[i] = 1;
                            have[guess[i] - 'a'] = 1;
                        }
                    }
                    for (int i = 0; i < 5; ++i) {
                        if (!Ans[i]) {
                            for (int j = 0; j < 5; ++j) {
                                if (!used[j]) {
                                    if (guess[i] == dic.Value[now][j]) {
                                        Ans[i] = 2;
                                        used[j] = 1;
                                        have[guess[i] - 'a'] = 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    for (int i = 0; i < 5; ++i) {
                        if (Ans[i] || have[guess[i] - 'a']) {
                            printf("\e[38;5;%dm%c\e[0m", color[Ans[i]],
                                   guess[i]);
                        } else {
                            printf("%c", guess[i]);
                        }
                    }
                    printf("\n");
                    int cor = 0;
                    for (int i = 0; i < 5; ++i) {
                        if (Ans[i] == 1) ++cor;
                    }
                    if (cor == 5) {
                        printf("Congratulations\n");
                        break;
                    } else if (cnt == 6) {
                        printf("Good Luck\n");
                        break;
                    }
                    cnt++;
                } else {
                    printf("this is not a word\nPlease input again\n");
                }
            } else {
                printf("all input should be a letter\nPlease input again\n");
            }
        }
    }
    for (int i = 0; i < dic.size; ++i) {
        free(dic.Value[i]);
    }

    free(dic.Value);
    fclose(dictionary);
}