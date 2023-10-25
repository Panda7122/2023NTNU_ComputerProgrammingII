#include "mystring.h"

#include <stdlib.h>
#include <string.h>
char *mystrchr(const char *s, int c) {
    if (s == NULL) return NULL;
    if (c > 127 || c < -128) return NULL;
    for (int i = 0; s[i] != 0x00; ++i) {
        if (s[i] == c) {
            char *ret = calloc(strlen(s + i) + 1, sizeof(char));
            for (int j = 0; j < strlen(s + i); ++j) {
                ret[j] = s[i + j];
            }
            ret[strlen(s + i)] = 0x00;
            return ret;
        }
    }
    return NULL;
}
char *mystrrchr(const char *s, int c) {
    if (s == NULL) return NULL;
    if (c > 127 || c < -128) return NULL;
    for (int i = strlen(s) - 1; i >= 0; --i) {
        if (s[i] == c) {
            char *ret = calloc(strlen(s + i) + 1, sizeof(char));
            for (int j = 0; j < strlen(s + i); ++j) {
                ret[j] = s[i + j];
            }
            ret[strlen(s + i)] = 0x00;
            return ret;
        }
    }
    return NULL;
}
size_t mystrspn(const char *s, const char *accept) {
    if (s == NULL) return 0;
    if (accept == NULL) return 0;
    for (int i = 0; s[i] != 0x00; ++i) {
        int check = 0;
        for (int j = 0; accept[j] != 0x00; ++j) {
            if (s[i] == accept[j]) {
                check = 1;
                break;
            }
        }
        if (!check) return i;
    }
    return strlen(s);
}
size_t mystrcspn(const char *s, const char *reject) {
    if (s == NULL) return 0;
    if (reject == NULL) return strlen(s);
    for (int i = 0; s[i] != 0x00; ++i) {
        int check = 0;
        for (int j = 0; reject[j] != 0x00; ++j) {
            if (s[i] == reject[j]) {
                check = 1;
                break;
            }
        }
        if (check) return i;
    }
    return strlen(s);
}
char *mystrpbrk(const char *s, const char *accept) {
    if (s == NULL) return NULL;
    if (accept == NULL) return NULL;
    for (int i = 0; s[i] != 0x00; ++i) {
        for (int j = 0; accept[j] != 0x00; ++j) {
            if (s[i] == accept[j]) {
                char *ret = calloc(strlen(s + i) + 1, sizeof(char));
                for (int k = 0; k < strlen(s + i); ++k) {
                    ret[k] = s[i + k];
                }
                ret[strlen(s + i)] = 0x00;
                return ret;
            }
        }
    }
    return NULL;
}
char *mystrstr(const char *haystack, const char *needle) {
    if (haystack == NULL) return NULL;
    if (needle == NULL) return NULL;
    for (int i = 0; haystack[i] != 0x00; ++i) {
        int check = 1;
        for (int j = 0; needle[j] != 0x00; ++j) {
            if (haystack[i + j] != needle[j]) {
                check = 0;
                break;
            }
        }
        if (check) {
            // printf("%d", i);
            char *ret = calloc(strlen(haystack + i) + 1, sizeof(char));
            for (int k = 0; k < strlen(haystack + i); ++k) {
                ret[k] = haystack[i + k];
            }
            ret[strlen(haystack + i)] = 0x00;
            return ret;
        }
    }
    return NULL;
}
char *mystrtok(char *str, const char *delim) {
    static char *token = NULL;
    if (str)
        token = str;
    else if (!token)
        return NULL;

    char *st = token;

    while (*token != 0x00) {
        int check = 0;
        if (delim != NULL) {
            for (int i = 0; delim[i] != 0x00; ++i) {
                if (*token == delim[i]) {
                    check = 1;
                    break;
                }
            }
        }
        if (check) {
            *token = '\0';
            ++token;
            while (*token != 0x00) {
                int flag = 0;
                if (delim != NULL) {
                    for (int i = 0; delim[i] != 0x00; ++i) {
                        if (*token == delim[i]) {
                            flag = 1;
                            break;
                        }
                    }
                }
                if (flag)
                    ++token;
                else
                    break;
            }
            return st;
        }
        ++token;
    }
    if (st == token) return NULL;
    return st;
}