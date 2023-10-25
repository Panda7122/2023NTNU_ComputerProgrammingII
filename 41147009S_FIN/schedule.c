
#include "schedule.h"

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

#include "linuxlist.h"
int compare(sJob *a, sJob *b) {
    if (a->arrival == b->arrival) {
        return a->duration - b->duration;
    }
    return a->arrival - b->arrival;
}
void swap(struct list_head *a, struct list_head *b) {
    struct list_head *p = a->prev;
    struct list_head *n = b->next;
    p->next = b;
    b->prev = p;
    b->next = a;
    a->prev = b;
    a->next = n;
    n->prev = a;
}
int add_job(struct list_head *pJobList, sJob *pJob) {
    if (pJobList == NULL || pJob == NULL) return -1;
    list_add_tail(&(pJob->list), pJobList);
    // struct list_head *now = pJobList->prev;
    // while (now != pJobList) {
    //     sJob *a = list_entry(now, sJob, list);
    //     // printf("%u %u %u\n", a->id, a->arrival, a->duration);
    //     now = now->prev;
    // }
    // now = pJobList->prev;
    // while (now->prev != pJobList) {
    //     sJob *a = list_entry(now->prev, sJob, list);
    //     sJob *b = list_entry(now, sJob, list);
    //     // printf("a%d %d %d\n", a->id, a->arrival, a->duration);
    //     // printf("b%d %d %d\n", b->id, b->arrival, b->duration);
    //     if (compare(a, b) > 0) {
    //         swap(now, now->prev);
    //     } else {
    //         break;
    //     }
    // }
    return 0;
}
int schedule(struct list_head *pJobList, struct list_head *pSchedJobList) {
    if (pJobList == NULL || list_empty(pJobList) || pSchedJobList == NULL)
        return -1;
    int size = list_entry(pJobList->prev, sJob, list)->id;
    bool *havepid = calloc(size + 1, sizeof(bool));
    int nowtime = 0;
    for (int i = 0; i < size; ++i) {
        int haveans = 0;
        struct list_head *answer = pJobList;
        struct list_head *now = pJobList->next;
        while (now != pJobList) {
            sJob *nowJob = list_entry(now, sJob, list);
            if (nowJob->arrival <= nowtime && !havepid[nowJob->id]) {
                if (answer != pJobList) {
                    sJob *a = list_entry(answer, sJob, list);
                    if (a->duration > nowJob->duration) {
                        answer = now;
                        haveans = 1;
                    }
                } else {
                    answer = now;
                    haveans = 1;
                }
            }
            now = now->next;
        }
        if (haveans) {
            sJob *ans = list_entry(answer, sJob, list);
            sSchedJob *push = calloc(1, sizeof(sSchedJob));
            push->id = ans->id;
            if (nowtime < ans->arrival) {
                push->start = ans->arrival;
                nowtime = ans->arrival;
            } else {
                push->start = nowtime;
            }
            push->stop = nowtime + ans->duration;
            nowtime += ans->duration;
            list_add_tail(&(push->list), pSchedJobList);
            havepid[ans->id] = 1;
        } else {
            ++nowtime;
            --i;
        }
    }
    free(havepid);
}