// #include <limit.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PATH_MAX 255
char ***people;
char **name;
char *tmp;
char *squad[11];
int sqsz = 0;
int *peopleValueSize;
int capacityPeople = 16;
int *capacityPeopleIndex;
int capacityName = 16;
const char *SQUZRD[27] = {"ls",  "st",  "rs",  "lw",  "lf",  "cf",  "rf",
                          "rw",  "lam", "cam", "ram", "lm",  "lcm", "cm",
                          "rcm", "rm",  "lwb", "ldm", "cdm", "rdm", "rwb",
                          "lb",  "lcb", "cb",  "rcb", "rb",  "gk"};
int findstring(int st) {
    while (tmp[st] != '\"' && tmp[st] != 0x00) {
        ++st;
    }
    return st;
}
int findvalue(int st) {
    if (tmp[st] != ',') {
        while (tmp[st] != ',' && tmp[st] != 0x00) {
            ++st;
        }
        return st;
    } else {
        return -1;
    }
}
int putsq(const char *str) {
    int i = 0;
    int havegk = 0;

    for (i = 0; str[i] != 0x00; ++i) {
        int end = i;
        while (str[end] != ' ' && str[end] != '\n' && str[end] != 0x00) {
            ++end;
        }
        int check = 0;
        if (sqsz == 11) {
            return -1;
        }
        squad[sqsz] = calloc(end - i + 5, sizeof(char));
        strncpy(squad[sqsz], str + i, end - i);
        i = end;

        for (int j = 0; j < 27; ++j) {
            if (strlen(squad[sqsz]) == strlen(SQUZRD[j]) &&
                strncmp(squad[sqsz], SQUZRD[j], strlen(SQUZRD[j])) == 0) {
                check = 1;
                if (j == 26) {
                    if (havegk) return -1;
                    havegk = 1;
                }
                break;
            }
        }
        if (!check) {
            return -1;
        }
        ++sqsz;
    }
    if (sqsz == 11 && havegk) return 0;
    return -1;
}
int main() {
    char *filename;
    filename = calloc(PATH_MAX + 5, sizeof(char));
    printf("Please enter the dataset: ");
    if (fgets(filename, PATH_MAX + 5, stdin) == NULL) return 0;
    if (filename[strlen(filename) - 1] == '\n')
        filename[strlen(filename) - 1] = 0x00;
    if (strlen(filename) > 255) {
        printf("your file name is too long\n");
        free(filename);
        return 0;
    }
    FILE *players = NULL;
    players = fopen(filename, "r");
    if (!players) {
        printf("error, there have no \"%s\" in file\n", filename);
        free(filename);
        return 0;
    }
    free(filename);
    int cnt = 0;
    int counter;
    int sz;
    people = calloc(capacityPeople, sizeof(char **));
    capacityPeopleIndex = calloc(capacityPeople, sizeof(int32_t));
    peopleValueSize = calloc(capacityPeople, sizeof(int32_t));
    for (int i = 0; i < capacityPeople; ++i) {
        capacityPeopleIndex[i] = 16;
        people[i] = calloc(capacityPeopleIndex[i], sizeof(char *));
    }

    tmp = calloc(10005, sizeof(char));
    name = calloc(capacityName, sizeof(char *));
    while (fgets(tmp, 10000, players)) {
        if (tmp == NULL) {
            free(tmp);
            free(name);
            for (int i = 0; i < capacityPeople; ++i) {
                free(people[i]);
            }
            free(capacityPeopleIndex);
            free(people);
            free(peopleValueSize);
            return 0;
        }
        if (tmp[strlen(tmp) - 1] == '\n') tmp[strlen(tmp) - 1] = 0x00;

        if (cnt) {
            if (cnt - 1 >= capacityPeople) {
                capacityPeople <<= 1;
                people = realloc(people, capacityPeople * sizeof(char **));
                capacityPeopleIndex = realloc(capacityPeopleIndex,
                                              capacityPeople * sizeof(int32_t));
                peopleValueSize =
                    realloc(peopleValueSize, capacityPeople * sizeof(int32_t));
                for (int i = capacityPeople >> 1; i < capacityPeople; ++i) {
                    capacityPeopleIndex[i] = 16;
                    people[i] = calloc(capacityPeopleIndex[i], sizeof(char *));
                }
            }
            counter = 0;
            for (int i = 0; tmp[i] != 0x00; ++i) {
                if (tmp[i] == ',') {
                    if (counter >= capacityPeopleIndex[cnt - 1]) {
                        capacityPeopleIndex[cnt - 1] <<= 1;
                        people[cnt - 1] = realloc(
                            people[cnt - 1],
                            capacityPeopleIndex[cnt - 1] * sizeof(char *));
                        for (int j = capacityPeopleIndex[cnt - 1] >> 1;
                             j < capacityPeopleIndex[cnt - 1]; ++j) {
                            people[cnt - 1][j] = NULL;
                        }
                    }
                    people[cnt - 1][counter] = calloc(1, sizeof(char));
                    people[cnt - 1][counter][0] = 0x00;
                    ++counter;
                } else {
                    int end = i;
                    if (tmp[i] == '\"') {
                        end = findstring(i + 1) - 1;
                    } else {
                        end = findvalue(i);
                    }
                    if (counter >= capacityPeopleIndex[cnt - 1]) {
                        capacityPeopleIndex[cnt - 1] <<= 1;
                        people[cnt - 1] = realloc(
                            people[cnt - 1],
                            capacityPeopleIndex[cnt - 1] * sizeof(char *));
                        for (int j = capacityPeopleIndex[cnt - 1] >> 1;
                             j < capacityPeopleIndex[cnt - 1]; ++j) {
                            people[cnt - 1][j] = NULL;
                        }
                    }
                    people[cnt - 1][counter] =
                        calloc(end - i + 5, sizeof(char));
                    if (tmp[i] == '\"') {
                        strncpy(people[cnt - 1][counter], tmp + i + 1, end - i);
                        i = end + 1;
                        while (tmp[i] != ',' && tmp[i] != 0x00) {
                            ++i;
                        }

                    } else {
                        strncpy(people[cnt - 1][counter], tmp + i, end - i);
                        i = end;
                    }
                    counter++;
                    peopleValueSize[cnt - 1] = counter;
                }
            }
        } else {
            sz = 0;
            for (int i = 0; tmp[i] != 0x00; ++i) {
                if (tmp[i] != ',') {
                    if (sz >= capacityName) {
                        capacityName <<= 1;
                        name = realloc(name, capacityName * sizeof(char *));
                    }
                    int end = findvalue(i);
                    name[sz] = calloc(end - i + 5, sizeof(char));
                    strncpy(name[sz], tmp + i, end - i);
                    i = end;
                    sz++;
                }
            }
        }
        ++cnt;
    }
    fclose(players);
    --cnt;

    printf("Please enter the squad: ");
    char *tmpsq;
    tmpsq = calloc(1000000, sizeof(char));
    while (1) {
        if (fgets(tmpsq, 1000000, stdin) == NULL) {
            printf("Please input a string\n");
            printf("Please enter the squad: ");
        } else {
            sqsz = 0;
            if (tmpsq[strlen(tmpsq) - 1] == '\n') {
                tmpsq[strlen(tmpsq) - 1] = 0x00;
            }
            if (putsq(tmpsq)) {
                printf(
                    "Please input 11 squads and there have one and only one "
                    "\"gk\"\n");
                printf("Please enter the squad: ");
                for (int i = 0; i < sqsz; ++i) free(squad[i]);

            } else {
                break;
            }
        }
    }

    printf("Budget:\n");
    int64_t money;
    bool *used;
    used = calloc(cnt, sizeof(bool));
    char moneybuffer;
    int64_t ans[11] = {0};
    for (int i = 0; i < 11; ++i) {
        printf("    %s: ", squad[i]);
        int checkMoney = 0;
        money = 0;
        while (1) {
            if (checkMoney) break;
            moneybuffer = getchar();
            if (moneybuffer == EOF) break;
            if (moneybuffer >= '0' && moneybuffer <= '9') {
                money *= 10;
                money += moneybuffer - '0';
            } else {
                if (moneybuffer != '\n') {
                    printf("error, please input a number\n    %s: ", squad[i]);
                    while (1) {
                        moneybuffer = getchar();
                        if (moneybuffer == '\n' || moneybuffer == EOF) break;
                    }
                    money = 0;
                } else {
                    checkMoney = 1;
                }
            }
        }
        // scanf("%ld", &money);
        // printf("%ld", money);
        double bestMoney = -1;
        int64_t bestID = -1;
        int64_t bestValue = -1;
        int64_t bestIndex = -1;
        for (int j = 0; j < cnt; ++j) {
            if (used[j]) continue;
            double nowMoney = -1;
            int64_t nowID = -1;
            int64_t nowValue = -1;
            int64_t nowIndex = j;
            char output[100];
            for (int k = 0; k < peopleValueSize[j]; ++k) {
                if (strlen(name[k]) == strlen("sofifa_id") &&
                    strcmp(name[k], "sofifa_id") == 0) {
                    if (strlen(people[j][k]))
                        sscanf(people[j][k], "%ld", &nowID);
                    else
                        nowID = -1;
                } else if (strlen(name[k]) == strlen(squad[i]) &&
                           strcmp(name[k], squad[i]) == 0) {
                    int64_t total = 0;
                    int64_t plus = 0;
                    int valueI = 0;
                    if (strlen(people[j][k]) == 0) {
                        nowValue = -1;
                    } else {
                        for (valueI = 0; people[j][k][valueI] != '+' &&
                                         people[j][k][valueI] != '-' &&
                                         people[j][k][valueI] != 0x00;
                             ++valueI) {
                            total *= 10;
                            total += people[j][k][valueI] - '0';
                        }
                        nowValue = total;
                        if (people[j][k][valueI] == '+' ||
                            people[j][k][valueI] == '-') {
                            int sign = people[j][k][valueI] == '+';
                            valueI++;
                            for (; people[j][k][valueI] != 0x00; ++valueI) {
                                plus *= 10;
                                plus += people[j][k][valueI] - '0';
                            }
                            if (sign)
                                nowValue += plus;
                            else
                                nowValue -= plus;
                        }
                    }
                    strncpy(output, people[j][k], strlen(people[j][k]) + 1);
                } else if (strcmp(name[k], "value_eur") == 0) {
                    if (strlen(people[j][k])) {
                        sscanf(people[j][k], "%lf", &nowMoney);
                    } else {
                        nowMoney = -1;
                    }
                }
                if (nowMoney >= 0 && nowID >= 0 && nowValue >= 0) {
                    break;
                }
            }

            if (nowMoney >= 0 && nowID >= 0 && nowValue >= 0) {
                if (bestValue == -1) {
                    if (nowMoney < money) {
                        bestMoney = nowMoney;
                        bestID = nowID;
                        bestValue = nowValue;
                        bestIndex = nowIndex;
                    }
                } else {
                    if (nowMoney < money) {
                        if (bestValue < nowValue) {
                            bestMoney = nowMoney;
                            bestID = nowID;
                            bestValue = nowValue;
                            bestIndex = nowIndex;

                        } else if (bestValue == nowValue) {
                            if (bestMoney > nowMoney) {
                                bestMoney = nowMoney;
                                bestID = nowID;
                                bestValue = nowValue;
                                bestIndex = nowIndex;

                            } else if (bestMoney == nowMoney) {
                                if (bestID > nowID) {
                                    bestMoney = nowMoney;
                                    bestID = nowID;
                                    bestValue = nowValue;
                                    bestIndex = nowIndex;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (bestValue == -1) {
            printf("error\n");
            --i;
        } else if (bestMoney > money) {
            printf("error, you are so poor\n");
            --i;
        } else {
            used[bestIndex] = 1;
            ans[i] = bestIndex;
        }
    }
    printf("Team:\n");
    for (int i = 0; i < 11; ++i) {
        int time = 0;
        printf("    %s: ", squad[i]);
        for (int k = 0; k < peopleValueSize[ans[i]]; ++k) {
            if (strlen(name[k]) == strlen("short_name") &&
                strcmp(name[k], "short_name") == 0) {
                if (strlen(people[ans[i]][k]))
                    printf("%s/", people[ans[i]][k]);
                else
                    printf("-/");
                break;
            }
        }
        for (int k = 0; k < peopleValueSize[ans[i]]; ++k) {
            if (strlen(name[k]) == strlen("club_name") &&
                strcmp(name[k], "club_name") == 0) {
                if (strlen(people[ans[i]][k]))
                    printf("%s/", people[ans[i]][k]);
                else
                    printf("-/");
                break;
            }
        }
        for (int k = 0; k < peopleValueSize[ans[i]]; ++k) {
            if (strlen(name[k]) == strlen(squad[i]) &&
                strcmp(name[k], squad[i]) == 0) {
                int64_t total = 0;
                int64_t plus = 0;
                int valueI = 0;
                if (strlen(people[ans[i]][k]) == 0) {
                    total = 0;
                } else {
                    for (valueI = 0; people[ans[i]][k][valueI] != '+' &&
                                     people[ans[i]][k][valueI] != '-' &&
                                     people[ans[i]][k][valueI] != 0x00;
                         ++valueI) {
                        total *= 10;
                        total += people[ans[i]][k][valueI] - '0';
                    }
                    if (people[ans[i]][k][valueI] == '+' ||
                        people[ans[i]][k][valueI] == '-') {
                        int sign = people[ans[i]][k][valueI] == '+';
                        valueI++;
                        for (; people[ans[i]][k][valueI] != 0x00; ++valueI) {
                            plus *= 10;
                            plus += people[ans[i]][k][valueI] - '0';
                        }
                        if (sign)
                            total += plus;
                        else
                            total -= plus;
                    }
                }
                printf("%ld/", total);
                break;
            }
        }
        for (int k = 0; k < peopleValueSize[ans[i]]; ++k) {
            if (strlen(name[k]) == strlen("value_eur") &&
                strcmp(name[k], "value_eur") == 0) {
                if (strlen(people[ans[i]][k]))
                    printf("%s/", people[ans[i]][k]);
                else
                    printf("-/");
                break;
            }
        }
        for (int k = 0; k < peopleValueSize[ans[i]]; ++k) {
            if (strlen(name[k]) == strlen("wage_eur") &&
                strcmp(name[k], "wage_eur") == 0) {
                if (strlen(people[ans[i]][k]))
                    printf("%s", people[ans[i]][k]);
                else
                    printf("-");
                break;
            }
        }
        printf("\n");
    }

    free(used);

    for (int i = 0; i < capacityPeople; ++i) {
        for (int j = 0; j < capacityPeopleIndex[i]; ++j) {
            if (!people[i][j]) {
                free(people[i][j]);
            }
        }
        free(people[i]);
    }
    free(people);
    for (int j = 0; j < capacityName; ++j) {
        free(name[j]);
    }
    free(name);
    for (int i = 0; i < sqsz; ++i) free(squad[i]);
}