#ifndef NES_SYS_H_INCLUDED
#define NES_SYS_H_INCLUDED

#include "cpu.h"
#include "mem.h"
#include "rom.h"
#include "video.h"
#include "pad.h"

struct NesSystem {

private:
    nes_file*   rom;
    cpu_6502*   cpu;
    memory*     ram;
    MMC*        mmc;
    PPU*        ppu;
    PlayPad*    pad;
    int         state;

public:
    /* PlayPad �ڸ�������ʱ���� */
    NesSystem(Video* video, PlayPad*);
    ~NesSystem();

    /* ��ȡrom�ļ�,�ɹ�����0,ʧ�ܷ��ش������, *
     * ͨ��parseOpenError()�õ�����ԭ��        */
    int load_rom(string filename);

    cpu_6502    *getCpu();
    PPU         *getPPU();
    memory      *getMem();

    /* ����һ֡ */
    void drawFrame();

    /* ��ʼcpu����ִ��,ͬʱ��Ҫ�򿪱��뿪�� */
    void debug();
};


#endif // NES_SYS_H_INCLUDED
