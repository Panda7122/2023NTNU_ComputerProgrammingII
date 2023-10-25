#include "ContributionCalculate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const char *month[13] = {"",    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static bool firsttime = 0;
void searchCommitInformationByHashVal(const char *hashVal) {
    FILE *logfile;
    FILE *save;
    logfile = fopen("frontend.txt", "r");
    if (!logfile) {
        printf("error, can not open frontend.txt\n");
        return;
    }
    if (firsttime)
        save = fopen("contribution.txt", "a");
    else
        save = fopen("contribution.txt", "w+");
    firsttime = 1;
    if (!save) {
        save = fopen("contribution.txt", "w");
    }
    char *text;
    text = calloc(10005, sizeof(char));
    int cnt = 0;
    int locateBytes = 0;
    int last = ftell(logfile);
    while (fgets(text, 10000, logfile)) {
        // printf("%ld\n", strlen(text));
        if (strncmp(text, "commit", strlen("commit")) == 0) {
            // printf("%s:\n", text);
            int tail = strlen("commit");
            while (text[tail] != 0x00 && text[tail] == ' ') {
                tail++;
            }
            // printf("%s\n", text + tail);
            if (strncmp(text + tail, hashVal, strlen(hashVal)) == 0) {
                cnt++;
                locateBytes = last;
                if (cnt > 1) break;
            }
        }
        last = ftell(logfile);
    }
    if (cnt == 0) {
        fprintf(save, "(%s) Not found\n", hashVal);
    } else if (cnt > 1) {
        fprintf(save, "(%s) More than two search results\n", hashVal);
    } else {
        fseek(logfile, locateBytes, SEEK_SET);

        char *name = NULL;
        char *mail = NULL;
        char *date = NULL;
        int mon = 0, day = 0, h = 0, m = 0, s = 0, year = 0;
        int haveday = 0, haveh = 0, havem = 0, haves = 0, haveyear = 0;

        int file = 0, insert = 0, deleted = 0;
        int havefile = 0, haveinsert = 0, havedeleted = 0;
        int have = 0;
        while (fgets(text, 10000, logfile)) {
            if (text[strlen(text) - 1] == '\n') {
                text[strlen(text) - 1] = 0x00;
            }
            if (strlen(text)) {
                if (strncmp(text, "commit ", strlen("commit ")) == 0) {
                    if (have) break;
                    have = 1;
                } else if (strncmp(text, "Author:", strlen("Author:")) == 0) {
                    int tail = strlen("Author:");
                    while (text[tail] != 0x00 && text[tail] == ' ') {
                        ++tail;
                    }
                    int head = tail - 1;
                    while (text[tail] != 0x00 && text[tail] != '\n') {
                        if (text[tail] == '<') {
                            break;
                        }
                        ++tail;
                    }
                    name = calloc(tail - head + 1, sizeof(char));
                    if (tail - head - 1 > 0)
                        strncpy(name, text + head, tail - head - 1);
                    head = tail + 1;
                    while (text[tail] != 0x00 && text[tail] != '\n') {
                        if (text[tail] == '>') {
                            break;
                        }
                        ++tail;
                    }
                    mail = calloc(tail - head + 1, sizeof(char));
                    if (tail - head - 1 > 0)
                        strncpy(mail, text + head, tail - head);

                } else if (strncmp(text, "Date: ", strlen("Date: ")) == 0) {
                    int tail = strlen("Date: ");
                    while (text[tail] != 0x00 && text[tail] == ' ') {
                        ++tail;
                    }
                    int head = tail - 1;
                    while (text[tail] != 0x00 && text[tail] != '\n') {
                        ++tail;
                    }
                    date = calloc(tail - head + 1, sizeof(char));
                    if (tail - head - 1 > 0)
                        strncpy(date, text + head, tail - head);
                    int now = 0;
                    while (date[now] != 0x00) {
                        for (int i = 1; i <= 12; ++i) {
                            if (strlen(date + now) < strlen(month[i])) break;
                            if (strncmp(date + now, month[i],
                                        strlen(month[i])) == 0) {
                                mon = i;
                                break;
                            }
                        }
                        if (date[now] >= '0' && date[now] <= '9') {
                            int t = now;
                            int val = 0;
                            while (date[t] >= '0' && date[t] <= '9') {
                                val *= 10;
                                val += date[t] - '0';
                                ++t;
                            }
                            now = t;
                            if (haveday == 0) {
                                day = val;
                                haveday = 1;
                            } else if (haveh == 0) {
                                h = val;
                                haveh = 1;
                            } else if (havem == 0) {
                                m = val;
                                havem = 1;
                            } else if (haves == 0) {
                                s = val;
                                haves = 1;
                            } else if (haveyear == 0) {
                                year = val;
                                haveyear = 1;
                            }
                        }
                        ++now;
                    }
                    free(date);
                } else if (strncmp(text, "Merge: ", strlen("Merge: ")) == 0) {
                } else {
                    int now = 0;
                    int val = -1;
                    while (text[now] != 0x00) {
                        if (text[now] >= '0' && text[now] <= '9') {
                            int t = now;
                            val = 0;
                            while (text[t] >= '0' && text[t] <= '9') {
                                val *= 10;
                                val += text[t] - '0';
                                ++t;
                            }
                            now = t;
                        } else if (strlen(text + now) >=
                                       strlen("files changed") &&
                                   strncmp(text + now, "files changed",
                                           strlen("files changed")) == 0) {
                            if (val != -1) {
                                file = val;
                                val = -1;
                            }

                        } else if (strlen(text + now) >=
                                       strlen("file changed") &&
                                   strncmp(text + now, "file changed",
                                           strlen("file changed")) == 0) {
                            if (val != -1) {
                                file = val;
                                val = -1;
                            }

                        } else if (strlen(text + now) >=
                                       strlen("insertions(+)") &&
                                   strncmp(text + now, "insertions(+)",
                                           strlen("insertions(+)")) == 0) {
                            if (val != -1) {
                                insert = val;
                                val = -1;
                            }

                        } else if (strlen(text + now) >=
                                       strlen("insertion(+)") &&
                                   strncmp(text + now, "insertion(+)",
                                           strlen("insertion(+)")) == 0) {
                            if (val != -1) {
                                insert = val;
                                val = -1;
                            }

                        } else if (strlen(text + now) >=
                                       strlen("deletions(-)") &&
                                   strncmp(text + now, "deletions(-)",
                                           strlen("deletions(-)")) == 0) {
                            if (val != -1) {
                                deleted = val;
                                val = -1;
                            }

                        } else if (strlen(text + now) >=
                                       strlen("deletion(-)") &&
                                   strncmp(text + now, "deletion(-)",
                                           strlen("deletion(-)")) == 0) {
                            if (val != -1) {
                                deleted = val;
                                val = -1;
                            }
                        }
                        ++now;
                    }
                }
            }
        }

        if (name != NULL && mail != NULL && mon && haveyear && haveh && havem &&
            haves && haveyear && haveday) {
            fprintf(save, "(%s)\n", hashVal);
            fprintf(save, "- %s\n", name);
            fprintf(save, "    - %s\n", mail);
            fprintf(save, "    - %04d/%02d/%02d/%02d:%02d:%02d\n", year, mon,
                    day, h, m, s);
            fprintf(save, "    - %d", file);
            if (file > 1)
                fprintf(save, " files changed\n");
            else
                fprintf(save, " file changed\n");
            fprintf(save, "    - %d", insert);
            if (insert > 1)
                fprintf(save, " insertions\n");
            else
                fprintf(save, " insertion\n");
            fprintf(save, "    - %d", deleted);
            if (deleted > 1)
                fprintf(save, " deletions\n");
            else
                fprintf(save, " deletion\n");
        } else {
            fprintf(save, "Error\n");
        }
        if (name != NULL) free(name);
        if (mail != NULL) free(mail);
    }
    fprintf(save, "\n");
    free(text);
    fclose(logfile);
    fclose(save);
}
void searchMonthlyContribution(const char *monthAbbrev) {
    FILE *logfile;
    FILE *save;
    int testMon = 0;

    logfile = fopen("frontend.txt", "r");
    if (!logfile) {
        printf("error, can not open frontend.txt\n");
        return;
    }
    if (firsttime)
        save = fopen("contribution.txt", "a");
    else
        save = fopen("contribution.txt", "w+");
    firsttime = 1;
    if (!save) {
        save = fopen("contribution.txt", "w");
    }
    if (monthAbbrev == NULL) {
        fprintf(save, "Error\n");
        fclose(save);
        fclose(logfile);
        return;
    }
    for (int i = 1; i <= 12; ++i) {
        if (strlen(monthAbbrev) != strlen(month[i])) continue;
        if (strncmp(monthAbbrev, month[i], strlen(month[i])) == 0) {
            testMon = i;
            break;
        }
    }
    if (testMon == 0) {
        fprintf(save, "Error\n");
        fclose(save);
        fclose(logfile);
        return;
    }
    char *text;
    text = calloc(10000, sizeof(char));
    int cnt = 0;

    char **name = NULL;
    int *file = NULL, *insert = NULL, *deleted = NULL;
    int mon = 0;
    int *commitCnt = NULL;
    int capacity = 16;

    file = calloc(capacity, sizeof(int));
    insert = calloc(capacity, sizeof(int));
    deleted = calloc(capacity, sizeof(int));
    name = calloc(capacity, sizeof(char *));
    commitCnt = calloc(capacity, sizeof(int));
    char *date;
    for (int i = 0; i < capacity; ++i) {
        name[i] = calloc(1, sizeof(char));
    }
    int nowCommit = 0;
    int sz = 0;
    while (fgets(text, 10000, logfile)) {
        if (text[strlen(text) - 1] == '\n') {
            text[strlen(text) - 1] = 0x00;
        }
        if (strlen(text)) {
            if (strncmp(text, "commit", strlen("commit")) == 0) {
            } else if (strncmp(text, "Author:", strlen("Author:")) == 0) {
                int tail = strlen("Author:");
                while (text[tail] != 0x00 && text[tail] == ' ') {
                    ++tail;
                }

                int head = tail;
                while (text[tail] != 0x00 && text[tail] != '\n') {
                    if (text[tail] == '<') {
                        break;
                    }
                    ++tail;
                }

                nowCommit = -1;
                for (int i = 0; i < sz; ++i) {
                    if (strncmp(text + head, name[i], strlen(name[i])) == 0) {
                        nowCommit = i;
                        break;
                    }
                }
                if (nowCommit == -1) {
                    nowCommit = sz;
                    if (sz >= capacity) {
                        capacity <<= 1;
                        name = realloc(name, capacity * sizeof(char *));
                        file = realloc(file, capacity * sizeof(int));
                        insert = realloc(insert, capacity * sizeof(int));
                        commitCnt = realloc(commitCnt, capacity * sizeof(int));
                        deleted = realloc(deleted, capacity * sizeof(int));
                        for (int i = capacity >> 1; i < capacity; ++i) {
                            file[i] = 0;
                            insert[i] = 0;
                            commitCnt[i] = 0;
                            deleted[i] = 0;
                            name[i] = calloc(1, sizeof(char));
                        }
                    }
                    name[sz] = realloc(name[sz], tail - head + 1);
                    strncpy(name[sz], text + head, tail - head - 1);
                    name[sz][tail - head - 1] = 0x00;
                    ++sz;
                }

            } else if (strncmp(text, "Date: ", strlen("Date: ")) == 0) {
                int tail = strlen("Date: ");
                mon = 0;
                while (text[tail] != 0x00 && text[tail] == ' ') {
                    ++tail;
                }
                int head = tail;
                while (text[tail] != 0x00 && text[tail] != '\n') {
                    ++tail;
                }
                date = calloc(tail - head + 1, sizeof(char));
                if (tail - head - 1 > 0) {
                    strncpy(date, text + head, tail - head);
                }
                int now = 0;
                while (date[now] != 0x00) {
                    for (int i = 1; i <= 12; ++i) {
                        if (strlen(date + now) < strlen(month[i])) break;
                        if (strncmp(date + now, month[i], strlen(month[i])) ==
                            0) {
                            mon = i;
                            break;
                        }
                    }
                    if (mon == testMon) commitCnt[nowCommit]++;
                    if (mon) {
                        break;
                    }
                    ++now;
                }
                free(date);
            } else if (strncmp(text, "Merge: ", strlen("Merge: ")) == 0) {
            } else {
                int now = 0;
                int val = -1;
                while (text[now] != 0x00) {
                    if (text[now] >= '0' && text[now] <= '9') {
                        int t = now;
                        val = 0;
                        while (text[t] >= '0' && text[t] <= '9') {
                            val *= 10;
                            val += text[t] - '0';
                            ++t;
                        }
                        now = t;
                    } else if (strlen(text + now) >= strlen("files changed") &&
                               strncmp(text + now, "files changed",
                                       strlen("files changed")) == 0) {
                        if (val != -1) {
                            if (nowCommit >= 0 && mon == testMon)
                                file[nowCommit] += val;
                            val = -1;
                        }
                    } else if (strlen(text + now) >= strlen("file changed") &&
                               strncmp(text + now, "file changed",
                                       strlen("file changed")) == 0) {
                        if (val != -1) {
                            if (nowCommit >= 0 && mon == testMon)
                                file[nowCommit] += val;
                            val = -1;
                        }
                    } else if (strlen(text + now) >= strlen("insertions(+)") &&
                               strncmp(text + now, "insertions(+)",
                                       strlen("insertions(+)")) == 0) {
                        if (val != -1) {
                            if (nowCommit >= 0 && mon == testMon)
                                insert[nowCommit] += val;
                            val = -1;
                        }

                    } else if (strlen(text + now) >= strlen("insertion(+)") &&
                               strncmp(text + now, "insertion(+)",
                                       strlen("insertion(+)")) == 0) {
                        if (val != -1) {
                            if (nowCommit >= 0 && mon == testMon)
                                insert[nowCommit] += val;
                            val = -1;
                        }

                    } else if (strlen(text + now) >= strlen("deletions(-)") &&
                               strncmp(text + now, "deletions(-)",
                                       strlen("deletions(-)")) == 0) {
                        if (val != -1) {
                            if (nowCommit >= 0 && mon == testMon)
                                deleted[nowCommit] += val;
                            val = -1;
                        }

                    } else if (strlen(text + now) >= strlen("deletion(-)") &&
                               strncmp(text + now, "deletion(-)",
                                       strlen("deletion(-)")) == 0) {
                        if (val != -1) {
                            if (nowCommit >= 0 && mon == testMon)
                                deleted[nowCommit] += val;
                            val = -1;
                        }
                    }
                    ++now;
                }
            }
        }
    }
    if (sz) {
        fprintf(save, "(%s)\n", monthAbbrev);
        for (int i = 0; i < sz; ++i) {
            fprintf(save, "- %s\n", name[i]);
            fprintf(save, "    - %d", commitCnt[i]);
            if (commitCnt[i] > 1)
                fprintf(save, " commits\n");
            else
                fprintf(save, " commit\n");
            fprintf(save, "    - %d", file[i]);
            if (file[i] > 1)
                fprintf(save, " files changed\n");
            else
                fprintf(save, " file changed\n");
            fprintf(save, "    - %d", insert[i]);
            if (insert[i] > 1)
                fprintf(save, " insertions\n");
            else
                fprintf(save, " insertion\n");
            fprintf(save, "    - %d", deleted[i]);
            if (deleted[i] > 1)
                fprintf(save, " deletions\n");
            else
                fprintf(save, " deletion\n");
        }
    } else {
        fprintf(save, "Error\n");
    }
    for (int i = 0; i < capacity; ++i) {
        free(name[i]);
    }
    if (file != NULL) free(file);
    if (insert != NULL) free(insert);
    if (deleted != NULL) free(deleted);
    if (commitCnt != NULL) free(commitCnt);
    if (name != NULL) free(name);

    fprintf(save, "\n");
    free(text);
    fclose(logfile);
    fclose(save);
}