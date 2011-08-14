#ifndef PPU_H_INCLUDED
#define PPU_H_INCLUDED

#include "mmc.h"
#include "video.h"


#define PPU_DISPLAY_P_WIDTH     256
#define PPU_DISPLAY_P_HEIGHT    240

#define PPU_DISPLAY_N_WIDTH     256
#define PPU_DISPLAY_N_HEIGHT    224

#define PPU_VMIRROR_VERTICAL    0x01
#define PPU_VMIRROR_HORIZONTAL  0x00
#define PPU_VMIRROR_SINGLE      0x03
#define PPU_VMIRROR_4_LAYOUT    0x08

struct BackGround {

    byte name      [0x03C0];
    byte attribute [0x0040];

    inline void write(word offset, byte data) {
        word off = (offset % 0x0400);
        if (off<0x03C0) {
            name[off] = data;
        } else {
            attribute[off-0x03C0] = data;
        }
    }

    inline byte read(word offset) {
        word off = (offset % 0x0400);
        if (off<0x03C0) {
            return name[off];
        } else {
            return attribute[off-0x03C0];
        }
    }
};

struct PPU {

private:
    BackGround  bg[4];

    BackGround *bg0;
    BackGround *bg1;
    BackGround *bg2;
    BackGround *bg3;

    byte bkPalette[16];     /* ������ɫ��                          */
    byte spPalette[16];     /* ��ͨ��ɫ��                          */

    byte spWorkRam[256];    /* ��ͨ�����ڴ�                        */
    word spWorkOffset;      /* ��ͨ����ҳ���׵�ַ                  */
    word ppu_ram_p;         /* ppu�Ĵ���ָ��                       */
    byte addr_add;          /* ��ַ�����ۼ�ֵ                      */
    enum { pH, pL } ppuSW;  /* д��ppu�Ĵ�����λ�� $0000-$3FFF     */

    word winX;
    word winY;
    enum { wX, wY } w2005;  /* д����һ������                      */

    word spRomOffset;       /* ��ͨ�ֿ��׵�ַ                      */
    word bgRomOffset;       /* �����ֿ��׵�ַ                      */
    enum { t8x8, t8x16 } spriteType;

    byte *NMI;
    byte bkleftCol;         /* ������ʾ��һ��                      */
    byte spleftCol;         /* ��ͨ��ʾ��һ��                      */
    byte bkAllDisp;         /* ����ȫ��ʾ                          */
    byte spAllDisp;         /* ��ͨȫ��ʾ                          */

    byte hasColor;          /* ����ɫ��                            */
    byte red;               /* ��ɫ��ɫ                            */
    byte green;             /* ��ɫ��ɫ                            */
    byte blue;              /* ��ɫ��ɫ                            */

    byte spOverflow;        /* ��ͨ8�����                         */
    byte spClash;           /* ��ͨ��ͻ?                           */

    MMC   *mmc;
    Video *video;

    void control_2000(byte data);
    void control_2001(byte data);

    void write(byte);       /* д����                              */
    byte read();            /* ������                              */
    BackGround* swBg();     /* ����ppu_ram_p��ֵ�õ���Ӧ�ı���ָ�� */

public:
    PPU(MMC *mmc, Video *video);

    void reset();
    /* cpuͨ��д0x2000~0x2007(0x3FFF)����PPU                       */
    void controlWrite(word addr, byte data);
    /* cpuͨ����0x2000~0x2007(0x3FFF)�õ�PPU״̬                   */
    byte readState(word addr);
    /* �л���Ļ����                                                */
    void swithMirror(byte type);
    /* ����cpu��NMI��ַ��                                          */
    void setNMI(byte* cpu_nmi);
    /* ����һ������                                                */
    void drawNextPixel();
    /* �������Ʊ����ֿ�                                            */
    void drawTileTable();
};

#endif // PPU_H_INCLUDED
