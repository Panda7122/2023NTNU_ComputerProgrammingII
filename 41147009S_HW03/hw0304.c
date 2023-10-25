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
#define HP_ADDR 0x366
#define MAXHP_ADDR 0x368
#define STRENGTH_ADDR 0x36E
#define MAXSTRENGTH_ADDR 0x0000  // unknow
#define LEVEL_ADDR 0x362
#define EXP_ADDR 0x364
#define WEAPON_ADDR 0x372
#define ARMOR_ADDR 0x374
#define KIATT_ADDR 0x394
#define KI_ADDR 0x396
#define MAXKI_ADDR 0x398
#define ATK_ADDR 0x39A
#define QINGGONG_ADDR 0x39C
#define DEF_ADDR 0x39E
#define HEALTH_ADDR 0x3A0
#define TOX_ADDR 0x3A2
#define DETOX_ADDR 0x3A4
#define PHYSIQUE_ADDR 0x3A4
#define FIST_ADDR 0x3A8
#define EDGEWORTH_ADDR 0x3AA
#define JUGGLING_ADDR 0x3AC
#define SPECIALWEAPON_ADDR 0x3AE
#define ASSWEAPON_ADDR 0x3B0
#define MORALITY_ADDR 0x3B4
#define FIGHTLR_ADDR 0x3B8
#define PRESTIGE_ADDR 0x3BA
#define WORTH_ADDR 0x3BC
#define ART_ADDR(i) (0x3C2 + (i - 1) * 2)
#define ARTLVL_ADDR(i) (0x3D6 + (i - 1) * 2)
#define ITEM_ADDR(i) (0x24 + (i - 1) * 4)
#define ITEMCNT_ADDR(i) (0x26 + (i - 1) * 4)
#define setADDR(val, ADDR, name, minV, maxV) \
    if (val >= minV && val < maxV)           \
        *(uint16_t*)(addr + ADDR) = val;     \
    else                                     \
        printf("%s should in [%d,%d)\n", name, minV, maxV);
uint8_t* addr = NULL;
void printMenu() {
    printf("0.Exit\n");
    printf("1.Modify Player attributes\n");
    printf("2.Modify Item(and Money)\n");
    printf("3.Modify Art\n");
}
void printItem() {
    printf("your Item list:(item name is in README)\n");
    int c = 1;
    printf(
        "----------------------------------------------------------------------"
        "--------------\n");
    printf("    ");
    for (int i = 1; i <= 8; ++i) {
        printf(" %02d       ", i);
    }
    printf("\n");
    for (int i = 0; i < 16; ++i) {
        printf("%03d:", 8 * i);
        for (int j = 0; j < 8; ++j) {
            if (*(int16_t*)(addr + ITEM_ADDR(c)) == -1)
                printf(" ff(00000)");
            else
                printf(" %02x(%05hd)", *(int16_t*)(addr + ITEM_ADDR(c)),
                       *(int16_t*)(addr + ITEMCNT_ADDR(c)));
            ++c;
        }
        printf("\n");
    }
    printf(
        "----------------------------------------------------------------------"
        "--------------\n");
}
void printArt() {
    printf("your Art list:(id name is in README)\n");
    printf("---------------\n");
    for (int i = 1; i <= 10; ++i) {
        printf("%2d: %02x EXP:%04x", i, *(int16_t*)(addr + ART_ADDR(i)),
               *(int16_t*)(addr + ARTLVL_ADDR(i)));
        printf("\n");
    }
    printf("---------------\n");
}
void printPlayer() {
    printf("your attribute(detail in README):\n");
    printf("-------------------------\n");
    printf(" 1               HP: %hd/%hd\n", *(int16_t*)(addr + HP_ADDR),
           *(int16_t*)(addr + MAXHP_ADDR));
    printf(" 2         STRENGTH: %hd\n", *(int16_t*)(addr + STRENGTH_ADDR));
    printf(" 3               KI: %hd/%hd TYPE %hd\n",
           *(int16_t*)(addr + KI_ADDR), *(int16_t*)(addr + MAXKI_ADDR),
           *(int16_t*)(addr + KIATT_ADDR));
    printf(" 4               LV: %hd(%hd)\n", *(int16_t*)(addr + LEVEL_ADDR),
           *(int16_t*)(addr + EXP_ADDR));
    printf(" 5              ATK: %hd\n", *(int16_t*)(addr + ATK_ADDR));
    printf(" 6              DEF: %hd\n", *(int16_t*)(addr + ATK_ADDR));
    printf(" 7         QINGGONG: %hd\n", *(int16_t*)(addr + QINGGONG_ADDR));
    printf(" 8           HEALTH: %hd\n", *(int16_t*)(addr + HEALTH_ADDR));
    printf(" 9              TOX: %hd\n", *(int16_t*)(addr + TOX_ADDR));
    printf("10            DETOX: %hd\n", *(int16_t*)(addr + DETOX_ADDR));
    printf("11             FIST: %hd\n", *(int16_t*)(addr + FIST_ADDR));
    printf("12        EDGEWORTH: %hd\n", *(int16_t*)(addr + EDGEWORTH_ADDR));
    printf("13         JUGGLING: %hd\n", *(int16_t*)(addr + JUGGLING_ADDR));
    printf("14   SPECIAL WEAPON: %hd\n",
           *(int16_t*)(addr + SPECIALWEAPON_ADDR));
    printf("15  ASSASSIN WEAPON: %hd\n", *(int16_t*)(addr + ASSWEAPON_ADDR));
    printf("16            WORTH: %hd\n", *(int16_t*)(addr + WORTH_ADDR));
    printf("17         PRESTIGE: %hd\n", *(int16_t*)(addr + PRESTIGE_ADDR));
    printf("18         MORALITY: %hd\n", *(int16_t*)(addr + MORALITY_ADDR));
    printf("19      HOLD WEAPON: %hd\n", *(int16_t*)(addr + WEAPON_ADDR));
    printf("20       HOLD ARMOR: %hd\n", *(int16_t*)(addr + ARMOR_ADDR));
    printf("21 FIGHT LEFT RIGHT: %hd\n", *(int16_t*)(addr + FIGHTLR_ADDR));
    printf("-------------------------\n");
}
void modifyPlayer() {
    printPlayer();
    printf("which you want to modify?");
    int opt = 0;
    scanf("%d", &opt);
    uint16_t val;
    switch (opt) {
        case 1:
            printf("HP:");
            scanf("%hu", &val);
            setADDR(val, HP_ADDR, "HP", 0x0000, 0x8000);
            printf("MAXHP:");
            scanf("%hu", &val);
            setADDR(val, MAXHP_ADDR, "MAXHP", 0x0000, 0x8000);

            break;
        case 2:
            printf("STRENGTH:");
            scanf("%hu", &val);
            setADDR(val, STRENGTH_ADDR, "STRENGTH", 0x0000, 0x8000);
            break;
        case 3:
            printf("KI:");
            scanf("%hu", &val);
            setADDR(val, KI_ADDR, "KI", 0x0000, 0x8000);
            printf("MAXKI:");
            scanf("%hu", &val);
            setADDR(val, MAXKI_ADDR, "MAXKI", 0x0000, 0x8000);
            printf("KIATT:");
            scanf("%hu", &val);
            setADDR(val, KIATT_ADDR, "KIATT", 0, 3);
            break;
        case 4:
            printf("LV:");
            scanf("%hu", &val);
            setADDR(val, LEVEL_ADDR, "LV", 0x0000, 0x8000);
            printf("EXP:");
            scanf("%hu", &val);
            setADDR(val, EXP_ADDR, "EXP", 0x0000, 0x8000);
            break;
        case 5:
            printf("ATK:");
            scanf("%hu", &val);
            setADDR(val, ATK_ADDR, "ATK", 0x0000, 0x8000);
            break;
        case 6:
            printf("DEF:");
            scanf("%hu", &val);
            setADDR(val, DEF_ADDR, "DEF", 0x0000, 0x8000);
            break;
        case 7:
            printf("QINGGONG:");
            scanf("%hu", &val);
            setADDR(val, QINGGONG_ADDR, "QINGGONG", 0x0000, 0x8000);
            break;
        case 8:
            printf("HEALTH:");
            scanf("%hu", &val);
            setADDR(val, HEALTH_ADDR, "HEALTH", 0x0000, 0x8000);
            break;
        case 9:
            printf("TOX:");
            scanf("%hu", &val);
            setADDR(val, TOX_ADDR, "TOX", 0x0000, 0x8000);
            break;
        case 10:
            printf("DETOX:");
            scanf("%hu", &val);
            setADDR(val, DETOX_ADDR, "DETOX", 0x0000, 0x8000);
            break;
        case 11:
            printf("FIST:");
            scanf("%hu", &val);
            setADDR(val, FIST_ADDR, "FIST", 0x0000, 0x8000);
            break;
        case 12:
            printf("EDGEWORTH:");
            scanf("%hu", &val);
            setADDR(val, EDGEWORTH_ADDR, "EDGEWORTH", 0x0000, 0x8000);
            break;
        case 13:
            printf("JUGGLING:");
            scanf("%hu", &val);
            setADDR(val, JUGGLING_ADDR, "JUGGLING", 0x0000, 0x8000);
            break;
        case 14:
            printf("SPECIAL WEAPON:");
            scanf("%hu", &val);
            setADDR(val, SPECIALWEAPON_ADDR, "SPECIAL WEAPON", 0x0000, 0x8000);
            break;
        case 15:
            printf("ASSASSIN WEAPON:");
            scanf("%hu", &val);
            setADDR(val, ASSWEAPON_ADDR, "ASSASSIN WEAPON", 0x0000, 0x8000);
            break;
        case 16:
            printf("WORTH:");
            scanf("%hu", &val);
            setADDR(val, WORTH_ADDR, "WORTH", 0x0000, 0x8000);
            break;
        case 17:
            printf("PRESTIGE:");
            scanf("%hu", &val);
            setADDR(val, PRESTIGE_ADDR, "PRESTIGE", 0x0000, 0x8000);
            break;
        case 18:
            printf("MORALITY:");
            scanf("%hu", &val);
            setADDR(val, MORALITY_ADDR, "MORALITY", 0x0000, 0x8000);
            break;
        case 19:
            printf("HOLD WEAPON:");
            scanf("%hu", &val);
            setADDR(val, WEAPON_ADDR, "HOLD WEAPON", 0x0000, 0x8000);
            break;
        case 20:
            printf("HOLD ARMOR:");
            scanf("%hu", &val);
            setADDR(val, ARMOR_ADDR, "HOLD ARMOR", 0x0000, 0x8000);
            break;
        case 21:
            printf("FIGHT LEFT RIGHT:");
            scanf("%hu", &val);
            setADDR(val, FIGHTLR_ADDR, "FIGHT LEFT RIGHT", 0x000000, 0x000002);
            break;
        default:
            printf("wrong key\n");
            break;
    }
}
void modifyItem() {
    printItem();
    printf("which you want to modify?");
    int opt = 0;
    scanf("%d", &opt);
    if (1 <= opt && opt <= 128) {
        uint16_t val;
        printf("set item to:");
        scanf("%hu", &val);
        if (val == 0xFFFF) {
            *(uint16_t*)(addr + ITEM_ADDR(opt)) = val;
            *(uint16_t*)(addr + ITEMCNT_ADDR(opt)) = (uint16_t)0;
            // setADDR(0, ITEMCNT_ADDR(opt), "ITEM COUNT", 0x0000, 0x8000);
        } else {
            setADDR(val, ITEM_ADDR(opt), "ITEM", 0x0000, 0x00C6);
            printf("how many?");
            scanf("%hu", &val);
            setADDR(val, ITEMCNT_ADDR(opt), "ITEM COUNT", 0x0000, 0x8000);
        }
    } else {
        printf("you only have 128 items\n");
    }
}
void modifyArt() {
    printArt();
    printf("which you want to modify?");
    int opt = 0;
    scanf("%d", &opt);
    if (1 <= opt && opt <= 10) {
        uint16_t val;
        printf("set ART to:");
        scanf("%hu", &val);
        setADDR(val, ART_ADDR(opt), "ART", 0x0000, 0x005D);
        if (val >= 0 && val < 0x005D) {
            printf("how many exp?");
            scanf("%hu", &val);
            setADDR(val, ARTLVL_ADDR(opt), "ART EXP", 0x0000, 0x8000);
        }
    } else {
        printf("you only have 10 ART\n");
    }
}
int main(int argc, char* argv[]) {
    char GameFilePath[4096 + 1];
    printf("Your Game Folder(relative path):");
    fgets(GameFilePath, 4096, stdin);
    if (GameFilePath[strlen(GameFilePath) - 1] == '\n')
        GameFilePath[strlen(GameFilePath) - 1] = 0x00;
    printf("Which Save you want to change(1-3)?");
    char save[2] = "1";
    scanf("%c", &(save[0]));
    strcat(GameFilePath, "/R");
    strcat(GameFilePath, save);
    strcat(GameFilePath, ".GRP");
    int fd = open(GameFilePath, O_RDWR);
    if (fd == -1) {
        perror("open game save error");
        return 0;
    }
    int filesize = lseek(fd, 0, SEEK_END);
    addr = mmap(NULL, filesize, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    int opt = 0;
    while (1) {
        printMenu();
        printf("your option:");
        scanf("%d", &opt);
        switch (opt) {
            case 1:
                modifyPlayer();
                break;
            case 2:
                modifyItem();
                break;
            case 3:
                modifyArt();
                break;
            case 0:
                munmap(addr, filesize);
                close(fd);
                return 0;
            default:
                printf("wrong option\n");
                break;
        }
    }
    munmap(addr, filesize);
    close(fd);
}