#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <regex.h>
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

#define periodDate(y, m, d)                                          \
    ((y / 4 * (int64_t)(365 * 4 + 1) + DAYS[y % 4][m - 1] + d - 1) * \
     (int64_t)24 * (int64_t)60 * (int64_t)60)
static unsigned short DAYS[4][12] = {
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
    {366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700},
    {731, 762, 790, 821, 851, 882, 912, 943, 974, 1004, 1035, 1065},
    {1096, 1127, 1155, 1186, 1216, 1247, 1277, 1308, 1339, 1369, 1400, 1430},
};
const char MONTH[13][3] = {"\0",  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
// time_t s;
typedef struct _date {
    int32_t year;
    int8_t month;
    int8_t day;
} date;
typedef struct _data {
    date D;
    double open;
    double high;
    double low;
    double close;
    int32_t volume;
} data;
static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                       {0, 0, 0, 0}};
void printHelp() {
    printf(
        "-s : symbol of stock\n-i : input JSON file name\n-o : output JSON "
        "file name\n");
}
typedef struct _vector {
    data* value;
    int64_t capacity;
    int64_t size;
} vector;
void init(vector** vec) {
    *vec = calloc(1, sizeof(vector));
    (*vec)->capacity = 16;
    (*vec)->size = 0;
    (*vec)->value = calloc(16, sizeof(data));
}
void push_back(vector* vec, data val) {
    if (vec->size >= vec->capacity) {
        vec->capacity <<= 1;
        vec->value = realloc(vec->value, vec->capacity * sizeof(data));
    }
    vec->value[vec->size] = val;
    ++vec->size;
}
void clear(vector* vec) {
    free(vec->value);
    free(vec);
}
date gDate(const char* str) {
    date retDate = {0, 0, 0};
    int i = 0;
    // printf("%s\n", str);
    for (; str[i] != 0x00; ++i) {
        for (int j = 1; j <= 12; ++j) {
            if (strncmp(str + i, MONTH[j], 3) == 0) {
                retDate.month = j;
                break;
            }
        }
        if (retDate.month) {
            i += 3;
            break;
        }
    }
    // printf("%d\n", i);
    for (; str[i] != 0x00; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            for (; str[i] != ',' && str[i] != 0x00; ++i) {
                // printf("%c", str[i]);
                retDate.day *= 10;
                retDate.day += str[i] - '0';
            }
            break;
        }
        if (retDate.day) break;
    }
    // printf("\n");
    // printf("%d\n", i);

    for (; str[i] != 0x00; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            for (; str[i] != '\n' && str[i] != 0x00; ++i) {
                // printf("%c", str[i]);
                retDate.year *= 10;
                retDate.year += str[i] - '0';
            }
            break;
        }
        if (retDate.year) break;
    }
    // printf("\n");
    return retDate;
}
date translateDate(const char* str) {
    int j = 0;
    date retDate;
    retDate.day = retDate.month = retDate.year = 0;
    while (str[j] != 0x00 && str[j] != '-') {
        retDate.year *= 10;
        retDate.year += str[j] - '0';
        ++j;
    }
    ++j;
    while (str[j] != 0x00 && str[j] != '-') {
        retDate.month *= 10;
        retDate.month += str[j] - '0';
        ++j;
    }
    ++j;
    while (str[j] != 0x00 && str[j] != '-') {
        retDate.day *= 10;
        retDate.day += str[j] - '0';
        ++j;
    }
    return retDate;
}
int main(int argc, char* argv[]) {
    char c;
    char stockName[2048];
    char inputJSONName[2048];
    char outputJSONName[2048];
    int8_t check = 0;
    bool wrong = 0;
    bool help = 0;
    vector* vec;
    while ((c = getopt_long(argc, argv, "s:i:o:", long_options, NULL)) != -1) {
        // printf("%c", c);
        switch (c) {
            case 's':
                if (check & 1) {
                    wrong = 1;
                } else {
                    check |= 1;
                    strcpy(stockName, optarg);
                }
                break;
            case 'i':
                if (check & (1 << 1)) {
                    wrong = 1;
                } else {
                    check |= 1 << 1;
                    strcpy(inputJSONName, optarg);
                }
                break;
            case 'o':
                if (check & (1 << 2)) {
                    wrong = 1;
                } else {
                    check |= 1 << 2;
                    strcpy(outputJSONName, optarg);
                }
                break;
            case 'h':
                help = 1;
                break;
            default:
                wrong = 1;
                break;
        }
    }
    if (help) {
        printHelp();
        return 0;
    }
    if (check != (1 | (1 << 1) | (1 << 2)) || wrong) {
        printf("error\nplease use ./hw0405 --help\n");
        return 0;
    }
    cJSON *inputJSON, *outputJSON;
    int inputfd = open(inputJSONName, O_RDONLY);
    int fdsize = lseek(inputfd, 0, SEEK_END);
    char* inputJSONString = calloc(fdsize + 1, sizeof(char));
    lseek(inputfd, 0, SEEK_SET);
    read(inputfd, inputJSONString, fdsize);
    inputJSONString[fdsize] = 0x00;
    close(inputfd);
    // printf("%d\n", fdsize);
    // printf("%s\n", inputJSONString);
    inputJSON = cJSON_Parse(inputJSONString);
    char *start, *end;
    int size = cJSON_GetArraySize(inputJSON);
    outputJSON = cJSON_CreateArray();
    for (int i = 0; i < size; ++i) {
        cJSON* nowDateRange = cJSON_GetArrayItem(inputJSON, i);
        cJSON* st = cJSON_GetObjectItem(nowDateRange, "start");
        cJSON* ed = cJSON_GetObjectItem(nowDateRange, "end");
        start = st->valuestring;
        end = ed->valuestring;
        date startDate, endDate;
        startDate = translateDate(start);
        endDate = translateDate(end);

        // printf("%s -> %s\n", start, end);
        // printf("%d %d %d -> %d %d %d\n", startDate.year, startDate.month,
        //        startDate.day, endDate.year, endDate.month, endDate.day);
        struct tm tp;
        tp.tm_year = startDate.year - 1900;
        tp.tm_mon = startDate.month - 1;
        tp.tm_mday = startDate.day;
        tp.tm_hour = 12;
        tp.tm_min = 0;
        tp.tm_sec = 0;
        tp.tm_isdst = -1;
        time_t period1 = mktime(&tp);
        tp.tm_year = endDate.year - 1900;
        tp.tm_mon = endDate.month - 1;
        tp.tm_mday = endDate.day;
        time_t period2 = mktime(&tp);
        char url[4096] = "\0";
        sprintf(url,
                "https://finance.yahoo.com/quote/%s/"
                "history?period1=%ld&period2=%ld&interval=1d&filter=history&"
                "frequency=1d&includeAdjustedClose="
                "true",
                stockName, period1, period2);
        CURL* curl;
        CURLcode res;
        curl = curl_easy_init();
        int wrong = 0;
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            // curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
            FILE* xml = NULL;
            char xmlName[4096];
            sprintf(xmlName, ".%s_%s_%s.txt", stockName, start, end);

            // sprintf(xmlName, "");

            xml = fopen(xmlName, "w+");
            if (xml == NULL) {
                printf("can not open %s\n", xmlName);
                return 0;
            }
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, xml);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                printf("error\n");
                return 0;
            }

            curl_easy_cleanup(curl);
            fclose(xml);
            int xmlfd = open(xmlName, O_RDONLY);
            int xmlsize = lseek(xmlfd, 0, SEEK_END);
            lseek(xmlfd, 0, SEEK_SET);
            uint8_t* xmlstr =
                mmap(NULL, xmlsize, PROT_READ, MAP_PRIVATE, xmlfd, 0);
            int startloc = 0;
            for (int i = 0; i < xmlsize - strlen("<span>Volume</span>"); ++i) {
                if (strncmp((char*)xmlstr + i, "<span>Volume</span>",
                            strlen("<span>Volume</span>")) == 0) {
                    startloc = i + strlen("<span>Volume</span>");
                    break;
                }
            }
            int cnt = 0;
            data tmp;
            double max = 0, min = 2147483647;
            init(&vec);

            for (int i = startloc; i < xmlsize - strlen("<span>"); ++i) {
                if (strncmp(
                        (char*)xmlstr + i,
                        "<span>*Close price adjusted for splits.",
                        strlen("<span>*Close price adjusted for splits.")) ==
                    0) {
                    break;
                } else {
                    if (strncmp((char*)xmlstr + i, "<span>",
                                strlen("<span>")) == 0) {
                        int num = 0;
                        int cnter = 1;
                        int doub = 0;
                        char dates[26] = "\0";

                        if (*((char*)xmlstr + i + 6) >= '0' &&
                            *((char*)xmlstr + i + 6) <= '9') {
                            int j = 0;

                            for (; *((char*)xmlstr + i + 6 + j) != '<'; ++j) {
                                // printf("")
                                if (*((char*)xmlstr + i + 6 + j) == '.') {
                                    doub = 1;
                                } else if (*((char*)xmlstr + i + 6 + j) !=
                                           ',') {
                                    num *= 10;
                                    num += *((char*)xmlstr + i + 6 + j) - '0';
                                    if (doub) cnter *= 10;
                                }
                            }
                            i += j + 6;
                        } else {
                            // for (int k = 0; k < 8; ++k)
                            //     printf("%c", *((char*)xmlstr + i + k + 6));
                            // printf("\n");
                            if (strncmp((char*)xmlstr + i + 6, "Dividend",
                                        strlen("Dividend")) == 0) {
                                wrong = 1;
                                // cnt = 0;
                            } else {
                                int next = 0;
                                wrong = 0;
                                cnt = 0;
                                while (*((char*)xmlstr + i + 6 + next) != '<')
                                    ++next;
                                strncpy(dates, (char*)xmlstr + i + 6, next);
                                i += next;
                            }
                        }
                        // printf("%lf %s\n", (double)num / cnter, dates);
                        if (!wrong) {
                            switch (cnt % 7) {
                                case 0:  // date
                                    wrong = 0;
                                    tmp.D = gDate(dates);
                                    break;
                                case 1:  // open
                                    tmp.open = (double)num / cnter;
                                    break;
                                case 2:  // high
                                    tmp.high = (double)num / cnter;
                                    if (tmp.high > max) max = tmp.high;
                                    break;
                                case 3:  // low
                                    tmp.low = (double)num / cnter;
                                    if (tmp.low < min) min = tmp.low;
                                    break;
                                case 4:  // close
                                    tmp.close = (double)num / cnter;
                                    break;
                                case 5:  // adj close
                                    break;
                                case 6:  // volume
                                    tmp.volume = num / cnter;
                                    if (!wrong) push_back(vec, tmp);
                                    break;
                            }
                        }
                        ++cnt;
                    }
                }
            }
            cJSON* now = cJSON_CreateObject();
            cJSON_bool check;
            if (vec->size) {
                cJSON* DATA = cJSON_CreateArray();
                for (int i = 0; i < vec->size; ++i) {
                    cJSON* this = cJSON_CreateObject();
                    char tmpstr[1024];
                    sprintf(tmpstr, "%04d-%02d-%02d", vec->value[i].D.year,
                            vec->value[i].D.month, vec->value[i].D.day);
                    cJSON_AddStringToObject(this, "date", tmpstr);

                    cJSON_AddNumberToObject(this, "Open", vec->value[i].open);
                    cJSON_AddNumberToObject(this, "High", vec->value[i].high);
                    cJSON_AddNumberToObject(this, "Low", vec->value[i].low);
                    cJSON_AddNumberToObject(this, "Close", vec->value[i].close);
                    cJSON_AddNumberToObject(this, "Volume",
                                            vec->value[i].volume);
                    cJSON_AddItemToArray(DATA, this);
                    // printf("%d/%d/%d %lf %lf %lf %lf %d\n",
                    //        vec->value[i].D.year, vec->value[i].D.month,
                    //        vec->value[i].D.day, vec->value[i].open,
                    //        vec->value[i].high, vec->value[i].low,
                    //        vec->value[i].close, vec->value[i].volume);
                }
                cJSON_AddItemToObject(now, "data", DATA);
                cJSON_AddNumberToObject(now, "max", max);
                cJSON_AddNumberToObject(now, "min", min);
            } else {
                cJSON_AddNullToObject(now, "data");
                cJSON_AddNullToObject(now, "max");
                cJSON_AddNullToObject(now, "min");
            }
            // printf("%lf %lf\n", max, min);
            cJSON_AddItemToArray(outputJSON, now);
            clear(vec);
            munmap(xmlstr, xmlsize);
            close(xmlfd);
            char codestr[2048];
            sprintf(codestr, "rm .%s_%s_%s.txt", stockName, start, end);
            system(codestr);
        } else {
            printf("can not open %s\n", url);
        }
        memset(url, 0, sizeof(url));
    }
    char* result = cJSON_Print(outputJSON);
    int outputfd = open(outputJSONName, O_RDWR | O_CREAT, 0666);
    write(outputfd, result, strlen(result));
    close(outputfd);
}