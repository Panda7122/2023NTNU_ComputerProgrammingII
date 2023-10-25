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
        // printf("%s\n", pJSON[jsonSize].value);
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
int main() {
    char str[8195];
    memset(str, 0, sizeof(str));
    printf("Please enter the JSON string:\n");
    fgets(str, 8195, stdin);
    str[strlen(str) - 1] = 0x00;
    while (1) {
        printf("Choice (0:Exit ,1:Get) : ");
        int step;
        scanf("%d", &step);
        getchar();
        if (step == 1) {
            printf("Key: ");
            char key[8195];
            char search[8195];
            fgets(key, 8192, stdin);
            key[strlen(key) - 1] = 0x00;
            char Value[8195];
            int index = -1;
            char Ans[8195] = {0};
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
                        printf("there is not a number in []\n");
                        break;
                    }
                    char tmp[8195] = {0};
                    int cnt = 0;
                    int sz = 0;
                    if (Ans[0] != '[') {
                        check = 0;
                        printf("there is not an array\n");
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
                        printf("there have no this term\n");
                        check = 0;
                        break;
                    }
                    strcpy(Ans, tmp);
                } else if (key[i] == '.') {
                    ++i;
                    getAttribute(search, &i, key);
                    if (!analyzeAtt(nowJS, &nowJSsz, Ans)) {
                        check = 0;
                        printf("error, there is not a {}\n");
                        break;
                    }
                    int have = 0;
                    for (int j = 0; j <= nowJSsz; ++j) {
                        if (strcmp(nowJS[j].key, search) == 0) {
                            have = 1;
                            strncpy(Ans, nowJS[j].value,
                                    strlen(nowJS[j].value));
                            Ans[strlen(nowJS[j].value)] = 0x00;
                            break;
                        }
                    }
                    --i;
                    if (!have) {
                        check = 0;
                        printf("error, there have no this attribute %s\n",
                               search);
                        break;
                    }

                } else {
                    getAttribute(search, &i, key);
                    int have = 0;
                    for (int j = 0; j <= nowJSsz; ++j) {
                        if (strcmp(nowJS[j].key, search) == 0) {
                            have = 1;
                            strncpy(Ans, nowJS[j].value,
                                    strlen(nowJS[j].value));
                            Ans[strlen(nowJS[j].value)] = 0x00;
                            break;
                        }
                    }
                    --i;
                    if (!have) {
                        check = 0;
                        printf("error, there have no this attribute %s\n",
                               search);
                        break;
                    }
                }
            }
            if (check) {
                printf("Value: ");
                for (int i = 0; Ans[i] != 0x00; ++i) {
                    if (Ans[i] == '\\' && Ans[i + 1] == 'n') {
                        printf("\n");
                        ++i;
                    } else if (Ans[i] == '\\' && Ans[i + 1] == 'r') {
                        printf("\r");
                        ++i;
                    } else {
                        printf("%c", Ans[i]);
                    }
                }
                printf("\n");
            }
            free(nowJS);
        } else if (step == 0) {
            printf("bye\n");
            return 0;
        } else {
            printf("error, you should key value in [0,1]\n");
        }
    }
}