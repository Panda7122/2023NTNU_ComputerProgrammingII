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

typedef struct _data {
    int32_t pid;
    char comm[2048];
    char state;
    int32_t ppid;
    int32_t pgrp;
    int32_t session;
    int32_t tty_nr;
    int32_t tpgid;
    uint32_t flags;
    uint64_t minflt;
    uint64_t cminflt;
    uint64_t majflt;
    uint64_t cmajflt;
    uint64_t utime;
    uint64_t stime;
    int64_t cutime;
    int64_t cstime;
    int64_t priority;
    int64_t nice;
    int64_t num_threads;
    int64_t itrealvalue;
    uint64_t starttime;
    uint64_t vsize;
    int64_t rss;
    uint64_t rsslim;
    uint64_t startcode;
    uint64_t endcode;
    uint64_t startstack;
    uint64_t kstkesp;
    uint64_t kstkeip;
    uint64_t signal;
    uint64_t blocked;
    uint64_t sigignore;
    uint64_t sigcatch;
    uint64_t wchan;
    uint64_t nswap;
    uint64_t cnswap;
    int32_t exit_signal;
    int32_t processor;
    uint32_t rt_priority;
    uint32_t policy;
    uint64_t delayacct_blkio_ticks;
    uint64_t guest_time;
    int64_t cguest_time;
    uint64_t start_data;
    uint64_t end_data;
    uint64_t start_brk;
    uint64_t arg_start;
    uint64_t arg_end;
    uint64_t env_start;
    uint64_t env_end;
    int32_t exit_code;
} data;
void printTime() {
    time_t rawtime;
    struct tm *info;
    char buffer[80];

    time(&rawtime);
    info = localtime(&rawtime);
    printf("%04d-%02d-%02d %02d:%02d:%02d ", info->tm_year + 1900,
           info->tm_mon + 1, info->tm_mday, info->tm_hour, info->tm_min,
           info->tm_sec);
}

int getpage() {
    char str[1024] = "\0";

    // printf("%s\n", str);
    FILE *page = fopen("/proc/self/smaps", "r");
    if (!page) {
        return -1;
    }
    int ans = 0;
    while (fgets(str, 1024, page)) {
        if (strncmp(str, "KernelPageSize:", strlen("KernelPageSize")) == 0) {
            for (int i = strlen("KernelPageSize:"); str[i] != 0x00; ++i) {
                if (str[i] >= '0' && str[i] <= '9') {
                    ans *= 10;
                    ans += str[i] - '0';
                }
            }
            break;
        }
    }
    fclose(page);
    return ans;
}

int getmem() {
    char str[1024] = "\0";
    // printf("%s\n", str);
    FILE *memory = fopen("/proc/meminfo", "r");
    if (!memory) {
        return -1;
    }
    int ans = 0;
    while (fgets(str, 1024, memory)) {
        if (strncmp(str, "MemTotal:", strlen("MemTotal:")) == 0) {
            for (int i = strlen("MemTotal:"); str[i] != 0x00; ++i) {
                if (str[i] >= '0' && str[i] <= '9') {
                    ans *= 10;
                    ans += str[i] - '0';
                }
            }
            break;
        }
    }
    fclose(memory);
    return ans;
}
int getActiveCPUs() {
    char str[1024] = "\0";
    // printf("%s\n", str);
    FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
    if (!cpuinfo) {
        return -1;
    }
    int ans = 0;
    while (fgets(str, 1024, cpuinfo)) {
        if (strncmp(str, "processor", strlen("processor")) == 0) {
            ++ans;
        }
    }
    fclose(cpuinfo);
    return ans;
}
double getTotalTime() {
    char str[1024] = "\0";
    sprintf(str, "/proc/uptime");
    // printf("%s\n", str);
    FILE *uptime = fopen(str, "r");
    if (!uptime) {
        return -1;
    }
    double ans = 0;
    fgets(str, 1024, uptime);
    // printf("%s\n", str);
    sscanf(str, "%lf", &ans);
    // printf("%lf\n", ans);
    fclose(uptime);
    return ans;
}
static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                       {"interval", required_argument, 0, 'i'},
                                       {"timestamp", no_argument, 0, 't'},
                                       {"count", required_argument, 0, 'c'},
                                       {0, 0, 0, 0}};
void printHelp() {
    printf(
        "Usage: hw0401 [options]... <pid>\nOptions:\n-i, --interval <int> "
        "Display the process information every <int>seconds.\nDefault value: "
        "1, Range: 1-100\n-c, --count <count > Display the process information "
        "<count > times.\nDefault behavior: infinite loop, Range: 1-1000\n-t, "
        "--timestamp Display the timestamp.\n-h, --help Display this "
        "descriptio&n.\n");
}

int main(int argc, char *argv[]) {
    int seconds = 1;
    int counts = -1;  // infinity
    bool timestamp = 0;
    bool help = 0;
    int page = getpage();
    int totalmem = getmem();
    int activeCPUs = getActiveCPUs();
    // printf("%d\n", activeCPUs);
    char c;
    char *endptr;
    int wrong = 0;
    int cnt = 1;
    int ticks = CLOCKS_PER_SEC;
    while ((c = getopt_long(argc, argv, "i:c:th", long_options, NULL)) != -1) {
        switch (c) {
            case 'i':
                seconds = strtol(optarg, &endptr, 10);
                if (*endptr != 0x00) {
                    wrong = 1;
                }
                cnt += 2;
                break;
            case 'c':
                counts = strtol(optarg, &endptr, 10);
                if (*endptr != 0x00) {
                    wrong = 1;
                }
                cnt += 2;
                break;
            case 't':
                if (timestamp) wrong = 1;
                timestamp = 1;
                ++cnt;
                break;
            case 'h':
                help = 1;
                break;
        }
    }
    if (help) {
        printHelp();
        return 0;
    }
    if (wrong) {
        printf("wrong argument\nplease use ./hw0401 -h to get more details\n");
        return 0;
    }
    char PID[1024];
    // int clockticks;
    if (cnt < argc) {
        strcpy(PID, argv[cnt]);
    } else {
        strcpy(PID, "1");
    }
    char path[1024];
    sprintf(path, "/proc/%s/stat", PID);
    // printf("%s\n", path);
    int64_t lastustime = 0;
    int64_t lastTotalTime = 0;

    while (counts) {
        char code[4096];
        // system("cat /proc/driver/rtc");
        sprintf(code, "cat %s > .tmpfile", path);
        system(code);

        // syscall(long sysno, ...)
        int fd = open(".tmpfile", O_RDONLY);
        if (fd == -1) {
            printf("this proc not exist\n");
        } else {
            // printf("%d ", fd);
            int size = lseek(fd, 0, SEEK_END);
            // printf("%d\n", size);
            char *addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
            char *strAddr;
            if (addr == MAP_FAILED) {
                printf("mmap error\n");
                close(fd);
                sleep(seconds);
                if (counts > 0) --counts;
                continue;
            }
            strAddr = calloc(size + 1, sizeof(char));
            strncpy(strAddr, addr, size);
            munmap(addr, size);
            close(fd);

            // printf("%s\n", strAddr);
            // printf("%s %d %d %d\n", PID, seconds, counts, timestamp);
            data n;
            sscanf(strAddr,
                   "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld "
                   "%ld %ld %ld %ld %lu "
                   "%lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu "
                   "%lu %d %d %u %u %lu %lu "
                   "%ld %lu %lu %lu %lu %lu %lu %lu %d",
                   &n.pid, n.comm, &n.state, &n.ppid, &n.pgrp, &n.session,
                   &n.tty_nr, &n.tpgid, &n.flags, &n.minflt, &n.cminflt,
                   &n.majflt, &n.cmajflt, &n.utime, &n.stime, &n.cutime,
                   &n.cstime, &n.priority, &n.nice, &n.num_threads,
                   &n.itrealvalue, &n.starttime, &n.vsize, &n.rss, &n.rsslim,
                   &n.startcode, &n.endcode, &n.startstack, &n.kstkesp,
                   &n.kstkeip, &n.signal, &n.blocked, &n.sigignore, &n.sigcatch,
                   &n.wchan, &n.nswap, &n.cnswap, &n.exit_signal, &n.processor,
                   &n.rt_priority, &n.policy, &n.delayacct_blkio_ticks,
                   &n.guest_time, &n.cguest_time, &n.start_data, &n.end_data,
                   &n.start_brk, &n.arg_start, &n.arg_end, &n.env_start,
                   &n.env_end, &n.exit_code);
            free(strAddr);
            if (timestamp) {
                printTime();
            }
            if (n.comm[strlen(n.comm) - 1] == ')')
                n.comm[strlen(n.comm) - 1] = '\0';
            char buffett[4096 + 1];
            uint64_t usertime, nicetime, systemtime, idletime;
            uint64_t ioWait = 0, irq = 0, softIrq = 0, steal = 0, guest = 0,
                     guestnice = 0;
            FILE *file = fopen("/proc/stat", "r");
            fgets(buffett, 4096, file);
            // printf("%s\n", buffett);
            // printf("%d\n", strncmp(buffett, "cpu", 3));
            if (strncmp(buffett, "cpu", 3)) return -1;
            fclose(file);
            sscanf(buffett,
                   "cpu  %16lu %16lu %16lu %16lu %16lu %16lu %16lu %16lu "
                   "%16lu %16lu",
                   &usertime, &nicetime, &systemtime, &idletime, &ioWait, &irq,
                   &softIrq, &steal, &guest, &guestnice);
            // printf(
            //     "cpu  %16lu %16lu %16lu %16lu %16lu %16lu %16lu %16lu "
            //     "%16lu %16lu\n",
            //     usertime, nicetime, systemtime, idletime, ioWait, irq,
            //     softIrq, steal, guest, guestnice);
            uint64_t idlealltime = idletime + ioWait;
            uint64_t systemalltime = systemtime + irq + softIrq;
            uint64_t virtalltime = guest + guestnice;
            uint64_t totaltime = usertime + nicetime + systemalltime +
                                 idlealltime + steal + virtalltime;
            double period = (double)(totaltime - lastTotalTime > 0
                                         ? totaltime - lastTotalTime
                                         : 0) /
                            activeCPUs;
            // printf("%ld %ld %lf %ld\n", n.utime, n.stime, nowtime,
            // n.starttime); printf("%d\n", ticks);
            printf("%d", 1);
            printf("%s: %lf(%%CPU) %lf(%%MEM) %s(COMMAND)\n", PID,
                   (period < 1E-6)
                       ? 0.0F
                       : ((n.utime + n.stime - lastustime) * 100.0 / period),
                   (double)n.rss * page * 100.0 / totalmem,
                   n.comm + (n.comm[0] == '('));
            lastustime = ((double)n.utime + (double)n.stime);
            lastTotalTime = totaltime;
        }
        system(code);
        sleep(seconds);
        if (counts > 0) --counts;
        sprintf(code, "rm .tmpfile");
        system(code);
    }
    // printf("%d(kb)", page);
}