#ifndef NES_SYS_H_INCLUDED
#define NES_SYS_H_INCLUDED

#include "cpu.h"
#include "mem.h"
#include "rom.h"
#include "video.h"

struct NesSystem {

private:
    nes_file*   rom;
    cpu_6502*   cpu;
    memory*     ram;
    MMC*        mmc;
    PPU*        ppu;
    int         state;

public:
    NesSystem(Video* video);
    ~NesSystem();

    /* ��ȡrom�ļ�,�ɹ�����0,ʧ�ܷ��ش������ */
    int load_rom(string filename);

    cpu_6502* getCpu();
    PPU *getPPU();
};


#endif // NES_SYS_H_INCLUDED
