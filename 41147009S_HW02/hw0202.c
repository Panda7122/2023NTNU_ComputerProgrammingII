#include <dirent.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int cmp(const struct dirent **a, const struct dirent **b) {
    // int sza = strlen((*a)->d_name);
    // int szb = strlen((*b)->d_name);
    // int sz = sza;
    // if (szb < sz) sz = szb;
    // for (int i = 0; i < sz; ++i) {
    //     if ((*a)->d_name[i] < (*b)->d_name[i])
    //         return 0;
    //     else if ((*a)->d_name[i] > (*b)->d_name[i])
    //         return 1;
    // }
    // if (sz == sza) return 0;
    return strcmp((*a)->d_name, (*b)->d_name) >= 0;
}
void print(const char *name) {
    // DIR *file = NULL;
    // file = opendir(name);
    // if (file == NULL) {
    //     printf("can not open %s\n", name);
    //     return;
    // }
    struct dirent **tmp;
    int sz;
    sz = scandir(name, &tmp, 0, cmp);
    // for (int i = 0; i < sz; ++i) {
    // printf("%s:\n", name);
    printf("\e[38;5;196m%s\e[m:\n", name);
    for (int i = 0; i < sz; ++i) {
        if (strcmp(tmp[i]->d_name, ".") && strcmp(tmp[i]->d_name, "..")) {
            if (tmp[i]->d_type == 4 || tmp[i]->d_type == 10) {
                // printf("%s  ", tmp[i]->d_name);
                printf("\e[38;5;33m%s\e[m  ", tmp[i]->d_name);
            } else {
                printf("%s  ", tmp[i]->d_name);
            }
        }
    }
    printf("\n");
    // closedir(file);
}
typedef struct _linkedlist {
    char *value;
    struct _linkedlist *next, *pref;
} linkedList;
linkedList *fuckyouPotatoBall;
int size = 1;
char **havePass;
int32_t havePassSize = 0;
int32_t havePassCapacity = 16;
bool check(const char *path) {
    // char oripath[PATH_MAX];
    char tmppath[PATH_MAX];
    // strncpy(oripath, path, strlen(path));
    realpath(path, tmppath);
    // printf("%s->%s\n", path, tmppath);
    for (int i = 0; i < havePassSize; ++i) {
        if (strcmp(tmppath, havePass[i]) == 0) {
            return 0;
        }
    }

    havePass[havePassSize] = calloc(strlen(tmppath) + 1, sizeof(char));
    strncpy(havePass[havePassSize], tmppath, strlen(tmppath));
    ++havePassSize;
    if (havePassCapacity <= havePassSize) {
        havePassCapacity <<= 1;
        havePass = realloc(havePass, havePassCapacity * sizeof(char *));
    }
    return 1;
}
int main() {
    // f(".");
    fuckyouPotatoBall = calloc(1, sizeof(linkedList));
    fuckyouPotatoBall->next = calloc(1, sizeof(linkedList));
    fuckyouPotatoBall->pref = fuckyouPotatoBall->next;
    fuckyouPotatoBall->next->value = calloc(2, sizeof(char));
    fuckyouPotatoBall->next->next = fuckyouPotatoBall;
    fuckyouPotatoBall->next->pref = fuckyouPotatoBall;
    strncpy(fuckyouPotatoBall->next->value, ".", 1);
    char tmppath[PATH_MAX];
    realpath(".", tmppath);
    // printf("%s\n", tmppath);
    havePass = calloc(havePassCapacity, sizeof(char *));
    havePass[havePassSize] = calloc(strlen(tmppath) + 1, sizeof(char));
    strncpy(havePass[havePassSize], tmppath, strlen(tmppath));
    ++havePassSize;
    if (havePassCapacity <= havePassSize) {
        havePassCapacity <<= 1;
        havePass = realloc(havePass, havePassCapacity * sizeof(char *));
    }
    while (size) {
        print(fuckyouPotatoBall->next->value);
        // DIR *file = opendir(fuckyouPotatoBall->next->value);
        // struct dirent *tmp = readdir(file);

        struct dirent **tmp;
        int sz;
        sz = scandir(fuckyouPotatoBall->next->value, &tmp, 0, cmp);
        for (int i = 0; i < sz; ++i) {
            if (strcmp(tmp[i]->d_name, ".") && strcmp(tmp[i]->d_name, "..")) {
                if (tmp[i]->d_type == 4 || tmp[i]->d_type == 10) {
                    char *str = calloc(strlen(fuckyouPotatoBall->next->value) +
                                           5 + strlen(tmp[i]->d_name),
                                       sizeof(char));
                    strncpy(str, fuckyouPotatoBall->next->value,
                            strlen(fuckyouPotatoBall->next->value) + 1);
                    strncat(str, "/", 2);
                    strncat(str, tmp[i]->d_name, strlen(tmp[i]->d_name) + 1);
                    if (check(str)) {
                        linkedList *newnode = calloc(1, sizeof(linkedList));
                        newnode->value = str;
                        linkedList *tail = fuckyouPotatoBall->pref;
                        tail->next = newnode;
                        newnode->pref = tail;
                        fuckyouPotatoBall->pref = newnode;
                        newnode->next = fuckyouPotatoBall;
                        size++;
                    }
                }
                // free(str);
            }
        }
        // while (tmp != NULL) {
        //     tmp = readdir(file);
        // }
        linkedList *newnextptr = fuckyouPotatoBall->next->next;
        linkedList *delete = fuckyouPotatoBall->next;
        fuckyouPotatoBall->next = newnextptr;
        newnextptr->pref = fuckyouPotatoBall;
        --size;
        free(delete->value);
        free(delete);
    }
    for (int i = 0; i < havePassSize; ++i) free(havePass[i]);
    free(havePass);
    return 0;
}