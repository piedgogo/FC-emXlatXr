/*----------------------------------------------------------------------------*|
|*                                                                            *|
|* FC ģ���� (Famicom��Nintendo��˾��1983��7��15�����ձ����۵�8λ��Ϸ��)      *|
|*                                                                            *|
|* $ C++���Եĵ�һ����Ŀ,���������ְ�                                         *|
|* $ è��д��, �����ñ����������ע������                                     *|
|* $ ���߶�ʹ�ñ�������ɵĺ�������κ�����                                   *|
|* $ �಻��Դ���Ĺ���ԭ������һ������,�����ش������벦��119 & 911           *|
|*                                                                            *|
|* > ʹ�� [Code::Block 10.05] ��������                                        *|
|* > ������ʹ�� [MinGW 3.81] [gcc 4.4.1]                                      *|
|* > �ο������� [http://nesdev.parodius.com] ��վ������                       *|
|* > ��л [Flubba] ��ƵĲ��Գ���, ����������Ч�ʳ�ָ������                   *|
|*                                                                            *|
|* ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ | CatfoOD |^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ *|
|*                                           | yanming-sohu@sohu.com          *|
|* [ TXT CHARSET WINDOWS-936 / GBK ]         | https://github.com/yanmingsohu *|
|*                                           | qQ:412475540                   *|
|*----------------------------------------------------------------------------*/
#include "nes_sys.h"
#include "debug.h"
#include <stdio.h>

NesSystem::NesSystem(Video* video, PlayPad *_pad) : pad(_pad) {
    mmc = new MMC();
    ppu = new PPU(mmc, video);
    ram = new memory(mmc, ppu, pad);
    cpu = new cpu_6502(ram);
    rom = new nes_file();

    ppu->setNMI(&cpu->NMI);
    mmc->setPPU(ppu);
    mmc->setIRQ(&cpu->IRQ);

    state = 0;
}

NesSystem::~NesSystem() {
    delete cpu;
    delete ram;
    delete ppu;
    delete mmc;
    delete rom;
    delete pad;
}

/* ����ʱ�Ӷ��ǻ���cpu*3��ppu/4 */
#define MID_CYC(x)       (x/4)
#define ONE_LINE_CYC     1364
#define START_CYC        MID_CYC(ONE_LINE_CYC*21)
#define END_CYC          MID_CYC(ONE_LINE_CYC)
#define HBLANK_CYC       MID_CYC(340)

void NesSystem::drawFrame() {
    static int _cyc = 0;

    ppu->oneFrameOver();

    while (_cyc < START_CYC) {
        _cyc += cpu->process() * 3;
    }
    _cyc -= START_CYC;

    ppu->startNewFrame();
    ppu->drawSprite(PPU::bpBehind);

    int x=0, y=0;

    while (y<240) {
        /* ����һ�� */
        for (;;) {
            ppu->drawPixel(x++, y);
            if (x>=256) {
                break;
            }

            if (--_cyc <= 0) {
                _cyc += cpu->process() * 3;
            }
        }

        mmc->drawLineOver();
        x = 0; y++;

        /* ˮƽ�������� */
        while (_cyc < HBLANK_CYC) {
            _cyc += cpu->process() * 3;
        }
        _cyc -= HBLANK_CYC;
    }

    ppu->drawSprite(PPU::bpFront);

    while (_cyc < END_CYC) {
        _cyc += cpu->process() * 3;
    }
    _cyc -= END_CYC;
}

int NesSystem::load_rom(string filename) {
    int res = ::load_rom(rom, &filename);

    if (!res) {
        rom->romInfo();

        if (mmc->loadNes(rom)) {
            int s = rom->rom_size * 16 * 1024 - 6;
            printf("INT vector(0xFFFA-0xFFFF) rom offset %X: ", s);
            rom->printRom(s, 6);

            ram->hard_reset();
            ppu->switchMirror(rom->t1 & 0x0B);
            ppu->reset();
            cpu->RES = 1;
        } else {
            res = ER_LOAD_ROM_BADMAP;
        }
        state = res;
    }

    return res;
}

cpu_6502* NesSystem::getCpu() {
    if (state) {
        printf("> NesSystem::��δ��ʼ��rom.\n");
    }
    return cpu;
}

PPU* NesSystem::getPPU() {
    return ppu;
}

memory* NesSystem::getMem() {
    return ram;
}

void NesSystem::debug() {
    static int debuging = 0;

    if (!debuging) {
        debuging = 1;
        printf("NES::start step debug.\n");
        debugCpu(this);
        debuging = 0;
    }
}
