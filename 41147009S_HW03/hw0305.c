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
#include <unistd.h>
#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define MTHD 0x4D546864
#define MTRK 0x4D54726B
// #define BPM 120
#define VOICE 0x20
#define TPQN 480
#define TPB 480
#define PITCH 12
#define TRACKIDFIRST 0xC0
int BPM = 120;
const int NODES[8] = {0, 60, 62, 64, 65, 67, 69, 71};
const char INSTRUMENT[][50] = {"Acoustic Grand Piano",
                               "Bright Acoustic Piano",
                               "Electric Grand Piano",
                               "Honky-tonk Piano",
                               "Electric Piano 1 (Rhodes Piano)",
                               "Electric Piano 2 (Chorused Pian",
                               "Harpsichord",
                               "Clavinet",
                               "Celesta",
                               "Glockenspiel",
                               "Music box",
                               "Vibraphone",
                               "Marimba",
                               "Xylophone",
                               "Tubular Bells",
                               "Dulcimer (Santur)",
                               "Drawbar Organ (Hammond)",
                               "Percussive Organ",
                               "Rock Organ",
                               "Church Organ",
                               "Reed Organ",
                               "Accordion (French)",
                               "Harmonica",
                               "Tango Accordion (Band neon)",
                               "Acoustic Guitar (nylon)",
                               "Acoustic Guitar (steel)",
                               "Electric Guitar (jazz)",
                               "Electric Guitar (clean)",
                               "Electric Guitar (muted)",
                               "Overdriven Guitar",
                               "Distortion Guitar",
                               "Guitar Harmonics",
                               "Acoustic Bass",
                               "Electric Bass (fingered)",
                               "Electric Bass (picked)",
                               "Fretless Bass",
                               "Slap Bass 1",
                               "Slap Bass 2",
                               "Synth Bass 1",
                               "Synth Bass 2",
                               "Violin",
                               "Viola",
                               "Cello",
                               "Contrabass",
                               "Tremolo Strings",
                               "Pizzicato Strings",
                               "Orchestral Harp",
                               "Timpani",
                               "String Ensemble 1 (strings)",
                               "String Ensemble 2 (slow strings)",
                               "Synth Strings 1",
                               "Synth Strings 2",
                               "Choir Aahs",
                               "Voice Oohs",
                               "Synth Voice",
                               "Orchestra Hit",
                               "Trumpet",
                               "Trombone",
                               "Tuba",
                               "Muted Trumpet",
                               "French Horn",
                               "Brass Section",
                               "Synth Brass 1",
                               "Synth Brass 2",
                               "Soprano Sax",
                               "Alto Sax",
                               "Tenor Sax",
                               "Baritone Sax",
                               "Oboe",
                               "English Horn",
                               "Bassoon",
                               "Clarinet",
                               "Piccolo",
                               "Flute",
                               "Recorder",
                               "Pan Flute",
                               "Blown Bottle",
                               "Shakuhachi",
                               "Whistle",
                               "Ocarina",
                               "Lead 1 (square wave)",
                               "Lead 2 (sawtooth wave)",
                               "Lead 3 (calliope)",
                               "Lead 4 (chiffer)",
                               "Lead 5 (charang)",
                               "Lead 6 (voice solo)",
                               "Lead 7 (fifths)",
                               "Lead 8 (bass + lead)",
                               "Pad 1 (new age Fantasia)",
                               "Pad 2 (warm)",
                               "Pad 3 (polysynth)",
                               "Pad 4 (choir space voice)",
                               "Pad 5 (bowed glass)",
                               "Pad 6 (metallic pro)",
                               "Pad 7 (halo)",
                               "Pad 8 (sweep)",
                               "FX 1 (rain)",
                               "FX 2 (soundtrack)",
                               "FX 3 (crystal)",
                               "FX 4 (atmosphere)",
                               "FX 5 (brightness)",
                               "FX 6 (goblins)",
                               "FX 7 (echoes, drops)",
                               "FX 8 (sci-fi, star theme)",
                               "Sitar",
                               "Banjo",
                               "Shamisen",
                               "Koto",
                               "Kalimba",
                               "Bag pipe",
                               "Fiddle",
                               "Shana",
                               "Tinkle Bell",
                               "Agogo",
                               "Steel Drums",
                               "Woodblock",
                               "Taiko Drum",
                               "Melodic Tom",
                               "Synth Drum",
                               "Reverse Cymbal",
                               "Guitar Fret Noise",
                               "Breath Noise",
                               "Seashore",
                               "Bird Tweet",
                               "Telephone Ring",
                               "Helicopter",
                               "Applause",
                               "Gunshot"};
char inputFileName[2048];
char outputFileName[2048];
typedef struct _headchunk {
    char MThd[4];
    uint32_t headerLength;
    uint16_t format;
    uint16_t trackCount;
    uint16_t ticks;
} __attribute__((packed)) headChunk;
typedef struct _trackchunk {
    char MTrk[4];
    uint32_t trackLength;
    uint16_t trackID;
    uint8_t instrument;

} __attribute__((packed)) trackChunk;
int getLength(double beat, char **Ans) {
    uint32_t ticks = beat * TPB;
    int cnt = 0;
    Ans = calloc(1, sizeof(char));
    while (ticks) {
        int now = ticks & 0x7F;  // last 7 bits
        now |= (cnt != 0) << 7;  // x0000000
        ticks >>= 7;
        cnt++;
        *Ans = realloc(*Ans, sizeof(char) * cnt);
        (*Ans)[cnt - 1] = now;
    }
    for (int i = 0; i < cnt >> 1; ++i) {
        char tmp = (*Ans)[i];
        (*Ans)[i] = (*Ans)[cnt - i - 1];
        (*Ans)[cnt - i - 1] = tmp;
    }
    return cnt;
}
int findRightBracket(char *addr, int now) {
    while (addr[now] != ']') {
        ++now;
    }
    return now;
}
int whichInstrument(char *addr, int length) {
    for (int i = 0; i < 128; ++i) {
        if (strncmp(addr, INSTRUMENT[i], length) == 0) {
            // int j = 0;
            // while (addr[j] != 0x00 && addr[j] != ']') {
            //     printf("%c", addr[j]);
            //     ++j;
            // }
            // printf("\n");
            return i;
        }
    }
    return 0;
}
typedef struct _node {
    uint8_t pitch;
    double beat;
    uint8_t value;
} __attribute__((packed)) node;
void freeNode(node **pNode, int64_t *capacity, int64_t *size) {
    if (*pNode != NULL) {
        free(*pNode);
    }
    *pNode = NULL;
}
void initNode(node **pNode, int64_t *capacity, int64_t *size) {
    if (*pNode != NULL) {
        freeNode(pNode, capacity, size);
    }
    *capacity = 16;
    *size = 0;
    *pNode = calloc(*capacity, sizeof(node));
    // printf(".");
    for (int i = 0; i < *capacity; ++i) {
        ((*pNode)[i]).beat = 1;
        ((*pNode)[i]).pitch = 0;
        ((*pNode)[i]).value = 0x20;
    }
}
int getSize(int num) {
    int cnt = 0;
    while (num) {
        ++cnt;
        num >>= 7;
    }
    return cnt;
}
void pushTrack(trackChunk trackHeader, node *track, int64_t capacity,
               int64_t size, int fd) {
    trackHeader.trackLength = 7 + 3 + 1 + getSize(60000000 / BPM);
    for (int i = 0; i < size; ++i) {
        int c = getSize(track[i].beat * TPB);
        trackHeader.trackLength += c;

        trackHeader.trackLength += 7;
    }

    write(fd, &trackHeader, 4);
    uint8_t buf;
    buf = trackHeader.trackLength >> 24;
    write(fd, &buf, 1);
    buf = (trackHeader.trackLength >> 16) & 0xFF;
    write(fd, &buf, 1);
    buf = (trackHeader.trackLength >> 8) & 0xFF;
    write(fd, &buf, 1);
    buf = trackHeader.trackLength & 0xFF;
    write(fd, &buf, 1);
    buf = trackHeader.trackID >> 8;
    write(fd, &buf, 1);
    buf = trackHeader.trackID & 0xFF;
    write(fd, &buf, 1);
    buf = trackHeader.instrument;
    write(fd, &buf, 1);
    buf = 0x00;
    write(fd, &buf, 1);
    buf = 0xFF;
    write(fd, &buf, 1);
    buf = 0x51;
    write(fd, &buf, 1);
    buf = getSize(60000000 / BPM);
    write(fd, &buf, 1);
    uint8_t *buffett = NULL;
    buffett = calloc(buf, sizeof(uint8_t));
    int tmp = 60000000 / BPM;
    int l = buf - 1;
    while (tmp) {
        int now = tmp & (0xFF);
        buffett[l] = now;
        tmp >>= 8;
        l--;
    }
    write(fd, buffett, buf);
    free(buffett);
    buf = 0x00;
    write(fd, &buf, 1);
    // buf = 0xFF;
    // write(fd, &buf, 1);
    // buf = 0x2F;
    // write(fd, &buf, 1);
    // buf = 0x00;
    // write(fd, &buf, 1);
    buffett = NULL;
    for (int i = 0; i < size; ++i) {
        buf = NOTE_ON;
        write(fd, &buf, 1);
        write(fd, &(track[i].pitch), 1);
        write(fd, &(track[i].value), 1);
        tmp = (track[i].beat * TPB);
        int sz = getSize(track[i].beat * TPB);
        buffett = calloc(sz, sizeof(uint8_t));
        l = sz - 1;
        while (tmp) {
            int now = tmp & (0x7F);
            if (l < sz - 1) now |= 0x80;
            buffett[l] = now;
            tmp >>= 7;
            l--;
        }
        write(fd, buffett, sz);
        free(buffett);
        buffett = NULL;
        buf = NOTE_OFF;
        write(fd, &buf, 1);
        write(fd, &(track[i].pitch), 1);
        write(fd, &(track[i].value), 1);
        buf = 0x00;
        write(fd, &buf, 1);
    }
    buf = 0xFF;
    write(fd, &buf, 1);
    buf = 0x2F;
    write(fd, &buf, 1);
    buf = 0x00;
    write(fd, &buf, 1);
}
int main(int argc, char *argv[]) {
    int opt = 0;
    char *endptr = NULL;
    while ((opt = getopt(argc, argv, "b:")) != -1) {
        switch (opt) {
            case 'b':
                BPM = (int)strtol(optarg, &endptr, 10);
                break;
        }
    }
    printf("Welcome to Cool Sound Infinity Exporter!\n");
    printf("Please enter input and output file name.\n");
    printf("Input file name: ");
    fgets(inputFileName, 2048, stdin);
    if (inputFileName[strlen(inputFileName) - 1] == '\n')
        inputFileName[strlen(inputFileName) - 1] = 0x00;
    printf("OutPut file name: ");
    fgets(outputFileName, 2048, stdin);
    if (outputFileName[strlen(outputFileName) - 1] == '\n')
        outputFileName[strlen(outputFileName) - 1] = 0x00;
    headChunk midiHeader;
    trackChunk trackHeader;
    midiHeader.MThd[0] = 0x4D;
    midiHeader.MThd[1] = 0x54;
    midiHeader.MThd[2] = 0x68;
    midiHeader.MThd[3] = 0x64;
    trackHeader.MTrk[0] = 0x4D;
    trackHeader.MTrk[1] = 0x54;
    trackHeader.MTrk[2] = 0x72;
    trackHeader.MTrk[3] = 0x6B;
    midiHeader.format = 0x0100;
    midiHeader.headerLength = 0x06000000;
    midiHeader.trackCount = 0;
    midiHeader.ticks = TPQN;
    int textfd = open(inputFileName, O_RDONLY);
    int textSize = lseek(textfd, 0, SEEK_END);
    int midifd = open(outputFileName, O_WRONLY | O_CREAT, 0666);
    if (!(~textfd)) {
        perror("can not open file descriptor");
        return 0;
    }
    uint8_t *addr = mmap(NULL, textSize, PROT_READ, MAP_PRIVATE, textfd, 0);
    int64_t now = 0;
    int bracket;
    int instrument;
    node *track = NULL;
    int64_t trackSize = 0;
    int64_t trackCapacity = 16;
    initNode(&track, &trackCapacity, &trackSize);
    for (int i = 0; i < textSize; ++i) {
        if (*((char *)addr + i) == '[') ++midiHeader.trackCount;
    }
    // printf("%c%c%c%c\n%x\n%x\n%x\n%x\n", midiHeader.MThd[0],
    // midiHeader.MThd[1],
    //        midiHeader.MThd[2], midiHeader.MThd[3], midiHeader.headerLength,
    //        midiHeader.format, midiHeader.trackCount, midiHeader.ticks);
    uint8_t buf;
    write(midifd, &midiHeader, 10);
    buf = midiHeader.trackCount >> 8;
    write(midifd, &buf, 1);
    buf = midiHeader.trackCount & (0xFF);
    write(midifd, &buf, 1);
    buf = midiHeader.ticks >> 8;
    write(midifd, &buf, 1);
    buf = midiHeader.ticks & (0xFF);
    write(midifd, &buf, 1);

    int nowtrack = 0;
    int i;
    while (now < textSize) {
        switch (*(((char *)addr) + now)) {
            case '[':
                bracket = findRightBracket((char *)addr, now);
                if (nowtrack > 0) {
                    pushTrack(trackHeader, track, trackCapacity, trackSize,
                              midifd);
                    initNode(&track, &trackCapacity, &trackSize);
                }
                trackHeader.instrument =
                    whichInstrument((char *)addr + now + 1, bracket - now - 1);
                now = bracket;
                trackHeader.trackID = TRACKIDFIRST + nowtrack;
                // printf("%d %x\n", trackHeader.instrument,
                // trackHeader.trackID);
                ++nowtrack;
                break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                if (trackSize >= trackCapacity) {
                    trackCapacity <<= 1;
                    track = realloc(track, trackCapacity * sizeof(node));
                    for (i = trackCapacity >> 1; i < trackCapacity; ++i) {
                        track[i].beat = 1;
                    }
                }
                if (*(((char *)addr) + now) == '0') {
                    track[trackSize].value = 0;
                } else {
                    track[trackSize].value = 0x20;
                }
                track[trackSize].pitch = NODES[*(((char *)addr) + now) - '0'];
                ++trackSize;
                break;
            case '-':
                track[trackSize - 1].beat *= 2;
                break;
            case '.':
                track[trackSize - 1].beat *= 1.5;
                break;
            case ';':
                track[trackSize - 1].beat *= 0.5;
                break;
            case '\'':
                track[trackSize - 1].pitch += 12;

                break;
            case ',':
                track[trackSize - 1].pitch -= 12;
                break;
            case '#':
                track[trackSize - 1].pitch++;
                break;
            case 'b':
                track[trackSize - 1].pitch--;
                break;
            default:
                break;
        }
        ++now;
    }
    pushTrack(trackHeader, track, trackCapacity, trackSize, midifd);
    freeNode(&track, &trackCapacity, &trackSize);
    munmap(addr, textSize);
    close(textfd);
    close(midifd);
}