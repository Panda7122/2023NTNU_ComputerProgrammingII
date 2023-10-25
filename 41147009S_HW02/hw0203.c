#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct __att {
    char key[8195];
    char value[8195];
} attribute;

int findStr(int s, char *string) {
    int now = s + 1;
    while (string[now] != '\"') ++now;
    return now - 1;
}
int findAtt(int s, char *string) {
    int now = s + 1;
    int cnt = 1;
    while (string[now] != 0x00) {
        if (string[now] == '{')
            ++cnt;
        else if (string[now] == '}')
            --cnt;
        if (!cnt) break;
        ++now;
    }
    return now;
}
int findArr(int s, char *string) {
    int now = s + 1;
    int cnt = 1;
    while (string[now] != 0x00) {
        if (string[now] == '[')
            ++cnt;
        else if (string[now] == ']')
            --cnt;
        if (!cnt) break;
        ++now;
    }
    return now;
}
int analyzeAtt(attribute *pJSON, int *pJSONSize, const char *str) {
    if (strlen(str) == 0) return 0;
    if (str[0] != '{') return 0;
    int mode = 0;
    int sz = 0;
    char *s;
    s = malloc(sizeof(str) * strlen(str));
    memset(pJSON, 0, *pJSONSize * sizeof(attribute));
    *pJSONSize = 0;
    strncpy(s, str, strlen(str));
    for (int i = 1; s[i] != 0x00; ++i) {
        if (s[i] == '\"') {
            if (mode == 0) {
                int end = findStr(i, s);
                strncpy(pJSON[*pJSONSize].key, &s[i + 1], end - i);
                pJSON[*pJSONSize].key[end - i] = 0x00;
                i = end + 1;
            } else if (mode == 1) {
                int end = findStr(i, s);
                strncpy(pJSON[*pJSONSize].value, &s[i + 1], end - i);
                pJSON[*pJSONSize].value[end - i] = 0x00;
                i = end + 1;
            }
        } else if (str[i] == '{' && mode == 1) {
            int end = findAtt(i, s);
            strncpy(pJSON[*pJSONSize].value, &s[i], end - i + 1);
            pJSON[*pJSONSize].value[end - i + 1] = 0x00;
            i = end;
        } else if (str[i] == '[' && mode == 1) {
            int end = findArr(i, s);
            strncpy(pJSON[*pJSONSize].value, &s[i], end - i + 1);
            pJSON[*pJSONSize].value[end - i + 1] = 0x00;
            i = end;
        } else if (mode == 1 && s[i] == ',') {
            mode = 0;
            if (sz) {
                pJSON[*pJSONSize].value[sz] = 0x00;
            }
            sz = 0;
            ++*pJSONSize;
        } else if (mode == 0 && s[i] == ':') {
            mode = 1;
            sz = 0;
            memset(pJSON[*pJSONSize].value, 0, sizeof(pJSON[*pJSONSize].value));
        } else if (s[i] == '}' && mode == 1) {
            if (sz) {
                pJSON[*pJSONSize].value[sz] = 0x00;
            }
            sz = 0;
            mode = 0;
        } else if (s[i] != ' ' && mode == 1) {
            pJSON[*pJSONSize].value[sz] = s[i];
            sz++;
        }
    }
    return 1;
}
int getIndex(int *pIndex, const char *pStr) {
    int ret = 0;
    while (pStr[*pIndex] != ']') {
        ret *= 10;
        if (pStr[*pIndex] > '9' || pStr[*pIndex] < '0') return -1;
        ret += (pStr[*pIndex] - '0');
        ++(*pIndex);
    }
    return ret;
}
void getAttribute(char *pKey, int *pIndex, const char *pStr) {
    int st = *pIndex;
    while (pStr[*pIndex] != 0x00) {
        if (pStr[*pIndex] == '.' || pStr[*pIndex] == '[') break;
        ++(*pIndex);
    }
    strncpy(pKey, pStr + st, *pIndex - st);
    pKey[*pIndex - st] = 0x00;
    return;
}
char *test(const char *str, const char *key) {
    char search[8195];
    char Value[8195];
    int index = -1;
    char *Ans;
    Ans = calloc(8195, sizeof(char));
    attribute *nowJS;
    int nowJSsz = 0;
    nowJS = calloc(8195, sizeof(attribute));
    bool check = 1;
    strncpy(Ans, str, strlen(str));
    analyzeAtt(nowJS, &nowJSsz, Ans);
    for (int i = 0; key[i] != 0x00; ++i) {
        if (key[i] == '[') {
            ++i;
            index = getIndex(&i, key);
            if (index == -1) {
                check = 0;
                break;
            }
            char tmp[8195] = {0};
            int cnt = 0;
            int sz = 0;
            if (Ans[0] != '[') {
                check = 0;
                break;
            }
            for (int i = 1; Ans[i] != 0x00; ++i) {
                if (Ans[i] == '\"') {
                    int end = findStr(i, Ans);
                    strncpy(tmp, &Ans[i + 1], end - i);
                    tmp[end - i] = 0x00;
                    i = end + 1;
                } else if (Ans[i] == '[') {
                    int end = findArr(i, Ans);
                    strncpy(tmp, &Ans[i], end - i + 1);
                    tmp[end - i + 1] = 0x00;
                    i = end;
                } else if (Ans[i] == '{') {
                    int end = findAtt(i, Ans);
                    strncpy(tmp, &Ans[i], end - i + 1);
                    tmp[end - i + 1] = 0x00;
                    i = end;
                } else if (Ans[i] == ',') {
                    if (cnt == index) break;
                    cnt++;
                    if (sz) {
                        tmp[sz] = 0x00;
                    }
                    sz = 0;
                } else if (Ans[i] == ']') {
                    if (sz) {
                        tmp[sz] = 0x00;
                    }
                    sz = 0;
                } else if (Ans[i] != ' ') {
                    tmp[sz] = Ans[i];
                    ++sz;
                }
            }
            if (cnt != index) {
                check = 0;
                break;
            }
            // printf(" :%d  %s\n", cnt, tmp);
            strcpy(Ans, tmp);
        } else if (key[i] == '.') {
            ++i;
            getAttribute(search, &i, key);
            // printf("%s\n", search);
            if (!analyzeAtt(nowJS, &nowJSsz, Ans)) {
                check = 0;
                break;
            }
            int have = 0;
            for (int j = 0; j <= nowJSsz; ++j) {
                if (strcmp(nowJS[j].key, search) == 0) {
                    have = 1;
                    strncpy(Ans, nowJS[j].value, strlen(nowJS[j].value));
                    Ans[strlen(nowJS[j].value)] = 0x00;
                    break;
                }
            }
            --i;
            if (!have) {
                check = 0;
                break;
            }

        } else {
            getAttribute(search, &i, key);
            int have = 0;
            for (int j = 0; j <= nowJSsz; ++j) {
                if (strcmp(nowJS[j].key, search) == 0) {
                    have = 1;
                    strncpy(Ans, nowJS[j].value, strlen(nowJS[j].value));
                    Ans[strlen(nowJS[j].value)] = 0x00;
                    break;
                }
            }
            --i;
            if (!have) {
                check = 0;
                break;
            }
        }
    }
    if (check) {
        return Ans;
    }
    free(nowJS);
    return NULL;
}
bool compare(const char *a, const char *b) {
    // printf("%s %s\n", a, b);
    if (strlen(b) > strlen(a)) return 0;
    int sz = strlen(b);
    for (int i = 0; i < sz; ++i) {
        if (a[i] != b[i]) {
            if (((a[i] >= 'A' && a[i] <= 'Z') ||
                 (a[i] >= 'a' && a[i] <= 'z')) &&
                ((b[i] >= 'A' && b[i] <= 'Z') ||
                 (b[i] >= 'a' && b[i] <= 'z'))) {
                char tmpA = a[i], tmpB = b[i];
                if (tmpA > 'a') {
                    tmpA -= 32;
                }
                if (tmpB > 'a') {
                    tmpB -= 32;
                }
                if (tmpA != tmpB) return 0;
            } else {
                return 0;
            }
        }
    }
    // if (a[strlen(b)] == 0x00 || a[strlen(b)] == ' ' || a[strlen(b)] == '\n')
    // return 1;
    return 1;
}
int32_t *line;
int32_t capacity = 16;
int main() {
    char str[8195];
    line = calloc(capacity, sizeof(int));
    // while (1) {
    printf("Please enter the search target: ");
    char keyword[8195];
    fgets(keyword, 8192, stdin);
    keyword[strlen(keyword) - 1] = 0x00;
    FILE *bible;
    char *text;
    bible = fopen("bible.txt", "r");
    int cnt = 0;
    int i = 1;
    while (fgets(str, 8192, bible)) {
        str[strlen(str) - 1] = 0x00;
        text = test(str, "text");
        if (text) {
            // printf("%s ", test(str, "chapter"));
            // printf("%s\n", test(str, "verse"));
            for (int j = 0; text[j] != 0x00; ++j) {
                if (compare(text + j, keyword)) {
                    // printf("%d %s\n", i, text + j);
                    if (cnt >= capacity) {
                        capacity <<= 1;
                        // printf("%d %d\n", cnt, capacity);
                        line = realloc(line, capacity * sizeof(int32_t));
                    }
                    line[cnt] = i;
                    ++cnt;
                    break;
                }
            }
            free(text);
            text = NULL;
        }
        ++i;
    }
    fclose(bible);
    printf("Found %d time(s)\n", cnt);
    bible = fopen("bible.txt", "r");
    int now = 0;
    i = 1;
    while (fgets(str, 8192, bible)) {
        str[strlen(str) - 1] = 0x00;
        if (line[now] != i) {
            ++i;
            continue;
        }
        if (cnt == now) {
            break;
        }
        printf("%d. ", now + 1);
        printf("%s ", test(str, "book_id"));
        printf("%s:", test(str, "chapter"));
        printf("%s ", test(str, "verse"));
        printf("%s\n", test(str, "text"));
        now++;
        ++i;
    }
    fclose(bible);
}