#include <fcntl.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct _localhead {
    uint32_t signature;
    uint16_t version;
    uint16_t flag;
    uint16_t compression;
    uint16_t time;
    uint16_t date;
    uint32_t CRC;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t fileNameLength;
    uint16_t ExtraFieldLength;
} __attribute__((packed)) localHeader;

typedef struct _ctrl {
    char signature[4];
    uint16_t versionMadeby;
    uint16_t versionNeededExtract;
    uint16_t generalPurposeBitFlag;
    uint16_t compression;
    uint16_t fileLastModificationTime;
    uint16_t fileLastModificationDate;
    uint32_t CRC;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t fileNameLength;
    uint16_t extraFieldLength;
    uint16_t fileCommentLength;
    uint16_t fileStarts;
    uint16_t internalFileAttributes;
    uint32_t externalFileAttributes;
    uint32_t relativeOffset;
} __attribute__((packed)) ctrlHeader;

typedef struct _File {
    char* Name;
    char* extraField;
} fileData;
fileData* FileArray;
int filesize = 0, capacity = 16;
int compare_reverse(const void* a, const void* b) {
    char* A = ((fileData*)a)->Name;
    char* B = ((fileData*)b)->Name;
    return -strcmp(A, B);
}
int compare(const void* a, const void* b) {
    char* A = ((fileData*)a)->Name;
    char* B = ((fileData*)b)->Name;
    return strcmp(A, B);
}

int findnextslash(const char* this, int now) {
    while (this[now] != 0x00 && this[now] != '/') ++now;
    return now;
}
int check(const char* this, int sz, int ind) {
    for (int i = 0; i < ind; ++i) {
        if (strncmp(this, FileArray[i].Name, sz) == 0 &&
            FileArray[i].Name[strlen(FileArray[i].Name) - 1] != '/')
            return 1;
    }
    return 0;
}
int firstTime = 0;
bool findDown(const char* this, int ind, int now) {
    int offset = 1;
    while (ind + offset < filesize &&
           strncmp(this, FileArray[ind + offset].Name, now) == 0)
        ++offset;
    // printf("%d", offset);
    if (offset == 1) {
        if (findnextslash(FileArray[ind + offset].Name, now + 1) < strlen(this))
            return 0;
    } else {
        --offset;
    }
    // printf("%s", FileArray[ind + offset].Name);
    int ns = findnextslash(this, now + 1);
    // printf("%d", ns);
    // for (int i = 0; i < ns; ++i) {
    //     printf("%c", FileArray[ind + offset].Name[i]);
    // }
    // printf("|");
    // for (int i = 0; i < ns; ++i) {
    //     printf("%c", this[i]);
    // }
    // printf("%s %s %d", this, FileArray[ind + offset].Name, ns);
    if (this[ns] == 0x00) {
        if (strncmp(this, FileArray[ind + offset].Name, now) == 0) return 1;
        return 0;
    } else if (strncmp(this, FileArray[ind + offset].Name, ns) == 0)
        return 0;
    return 1;
}
void print(const char* this, int ind) {
    int nowindex = 0;
    int nowslash = 0;

    int last = 0;
    int f = 1;
    while (nowindex < strlen(this)) {
        nowslash = findnextslash(this, nowindex);
        if (this[nowslash] == 0x00) {
            // if (last) printf("+");
            // if (f) printf("+");
            printf("+-- ");
            for (int i = nowindex; i < nowslash; ++i) printf("%c", this[i]);
        } else {
            if (!check(this, nowslash, ind)) {
                printf("+-- ");
                for (int i = nowindex; i <= nowslash; ++i)
                    printf("%c", this[i]);
                if (this[nowslash + 1] != 0x00) printf(" --");
                last = 0;
            } else {
                if (findDown(this, ind, nowindex)) {
                    printf("|");

                } else
                    printf(" ");
                printf("   ");
                for (int i = nowindex; i <= nowslash; ++i) printf(" ");
                printf("   ");
                last = 1;
            }
        }
        if (this[nowslash] == 0x00) break;
        nowindex = nowslash + 1;
        f = 0;
    }
    printf("\n");
    if (ind < filesize - 1) {
        int blankindex = 0;
        int blankslash = 0;

        if (findDown(this, ind, blankindex)) {
            printf("|");
        } else {
            printf(" ");
        }
        int offset = 1;
        while (ind + offset < filesize &&
               FileArray[ind + offset]
                       .Name[strlen(FileArray[ind + offset].Name) - 1] == '/')
            ++offset;
        if (ind + offset == filesize) {
            return;
        }
        while (blankindex < strlen(this)) {
            if (this[blankindex] == '/') {
                if (strncmp(FileArray[ind + offset].Name, this,
                            blankindex + 1) == 0) {
                    printf("   ");
                    int i = 0;
                    for (i = blankindex - 1; i >= 0; --i) {
                        printf(" ");
                        if (this[i] == '/') break;
                    }
                    if (i < 0) printf(" ");
                    printf("   ");
                    blankslash = findnextslash(this, blankindex + 1);
                    if (findDown(this, ind, blankindex)) {
                        printf("|");
                    } else
                        printf(" ");
                }
            }
            ++blankindex;
        }
        printf("\n");
    }
}
int main(int argc, char* argv[]) {
    char c;
    int opt = 1;
    int wrong = 0;
    while ((c = getopt(argc, argv, "ad")) != -1) {
        switch (c) {
            case 'a':
                opt = 1;
                break;
            case 'd':
                opt = 2;
                break;
            default:
                wrong = 1;
                break;
        }
    }
    if (wrong) {
        perror("no this option");
        return 0;
    }

    char* zipName = argc[argv - 1];
    // printf("%s\n", zipName);
    int fd = open(zipName, O_RDONLY); /* 1 syscall */
    if (fd == -1) {
        perror("can not open");
        return 0;
    }
    localHeader* head;
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    uint8_t* file = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    int64_t offset = 0;
    FileArray = calloc(capacity, sizeof(fileData));
    while (offset < size) {
        head = (localHeader*)(file + offset);
        // printf("%08x\n", head->signature);
        if (head->signature == 0x04034b50) {
            // Local file header
            // printf("%ld:\n", offset);
            offset += sizeof(localHeader);
            // if (*(char*)(file + offset + head->fileNameLength - 1) == '/') {
            //     offset += head->fileNameLength;
            //     offset += head->ExtraFieldLength;
            // } else {
            if (filesize >= capacity) {
                capacity <<= 1;
                FileArray = realloc(FileArray, sizeof(fileData) * capacity);
                for (int i = capacity >> 1; i < capacity; ++i) {
                    FileArray[i].extraField = NULL;
                    FileArray[i].Name = NULL;
                }
            }
            if (FileArray[filesize].Name != NULL) {
                free(FileArray[filesize].Name);
                FileArray[filesize].Name = NULL;
            }
            FileArray[filesize].Name =
                calloc(head->fileNameLength + 1, sizeof(char));
            memcpy(FileArray[filesize].Name, file + offset,
                   head->fileNameLength * sizeof(char));
            FileArray[filesize].Name[head->fileNameLength] = 0x00;
            offset += head->fileNameLength;
            if (FileArray[filesize].extraField != NULL) {
                free(FileArray[filesize].extraField);
                FileArray[filesize].extraField = NULL;
            }
            FileArray[filesize].extraField =
                calloc(head->ExtraFieldLength + 1, sizeof(char));
            memcpy(FileArray[filesize].extraField, file + offset,
                   head->ExtraFieldLength * sizeof(char));
            FileArray[filesize].extraField[head->ExtraFieldLength] = 0x00;
            offset += head->ExtraFieldLength;

            // printf("%d %d %s %d %s\n", index, head->fileNameLength,
            //        FileArray[index].Name, head->ExtraFieldLength,
            //        FileArray[index].extraField);
            ++filesize;
            // }
        } else if (head->signature == 0x08074b50) {
            // Data descriptor
            offset += 16;

        } else if (head->signature == 0x02014b50) {
            // Central directory file header
            offset += 46;
            offset += ((ctrlHeader*)head)->fileNameLength;
            offset += ((ctrlHeader*)head)->extraFieldLength;
            offset += ((ctrlHeader*)head)->fileCommentLength;
        } else if (head->signature == 0x06054b50) {
            // End of central directory record
            offset += 22;
            break;
        } else {
            ++offset;
        }
    }
    if (opt == 1) {
        qsort(FileArray, filesize, sizeof(fileData), compare);
    } else {
        qsort(FileArray, filesize, sizeof(fileData), compare_reverse);
    }
    char* nowFolder;
    nowFolder = calloc(1, sizeof(char));
    for (int i = 0; i < filesize; ++i) {
        int nowloc = 0;

        if (FileArray[i].Name[strlen(FileArray[i].Name) - 1] == '/') {
            int haveChild = 0;
            for (int j = 0; j < filesize; ++j) {
                if (j == i) continue;
                if (strncmp(FileArray[i].Name, FileArray[j].Name,
                            strlen(FileArray[i].Name)) == 0) {
                    haveChild = 1;
                    break;
                }
            }
            if (!haveChild) {
                print(FileArray[i].Name, i);
                firstTime = 1;
            }
        } else {
            print(FileArray[i].Name, i);
            firstTime = 1;
        }
    }
    for (int i = 0; i < filesize; ++i) {
        free(FileArray[i].Name);
        free(FileArray[i].extraField);
    }
    free(FileArray);
    munmap(file, size);
    close(fd);
}