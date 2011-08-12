#ifndef PPU_H_INCLUDED
#define PPU_H_INCLUDED

#define PPU_DISPLAY_N_WIDTH     256
#define PPU_DISPLAY_N_HEIGHT    224
#define PPU_DISPLAY_P_WIDTH     256
#define PPU_DISPLAY_P_HEIGHT    240

#include "mmc.h"

struct PPU {

private:
    byte vram[0x2000];
    MMC *mmc;

public:
    PPU(MMC *mmc);
    /* cpuͨ��д0x2000~0x2007(0x3FFF)����PPU              */
    void controlWrite(word addr, byte data);
    /* cpuͨ����0x2000~0x2007(0x3FFF)�õ�PPU״̬  */
    byte readState(word addr);
};

#endif // PPU_H_INCLUDED
