#ifndef PPU_H_INCLUDED
#define PPU_H_INCLUDED

#define PPU_DISPLAY_N_WIDTH     256
#define PPU_DISPLAY_N_HEIGHT    224
#define PPU_DISPLAY_P_WIDTH     256
#define PPU_DISPLAY_P_HEIGHT    240

#define PPU_VMIRROR_VERTICAL    0x01
#define PPU_VMIRROR_HORIZONTAL  0x00
#define PPU_VMIRROR_SINGLE      0x03
#define PPU_VMIRROR_4_LAYOUT    0x08

#include "mmc.h"

struct BackGround {
    byte name      [0x03C0];
    byte attribute [0x0040];
};

struct PPU {

private:
    BackGround  bg[4];

    BackGround *bg0;
    BackGround *bg1;
    BackGround *bg2;
    BackGround *bg3;

    MMC *mmc;

public:
    PPU(MMC *mmc);

    /* cpuͨ��д0x2000~0x2007(0x3FFF)����PPU      */
    void controlWrite(word addr, byte data);
    /* cpuͨ����0x2000~0x2007(0x3FFF)�õ�PPU״̬  */
    byte readState(word addr);
    /* �л���Ļ����                               */
    void swithMirror(byte type);
};

#endif // PPU_H_INCLUDED
