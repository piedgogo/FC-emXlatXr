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

NesSystem::NesSystem(Video* video, PlayPad *_pad)
    : pad(_pad)
    , state(0)
    , _cyc(0)
{
    mmc = new MMC();
    ppu = new PPU(mmc, video);
    ram = new memory(mmc, ppu, pad);
    cpu = new cpu_6502(ram);
    rom = new nes_file();

    ppu->setNMI(&cpu->NMI);
    mmc->setPPU(ppu);
    mmc->setIRQ(&cpu->IRQ);
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
#define MID_CPU_CYC(x)   (x*3)
#define MID_CYC(x)       (x/4)
#define ONE_LINE_CYC     1364
#define START_CYC        MID_CYC(ONE_LINE_CYC*20)
#define END_CYC          MID_CYC(ONE_LINE_CYC)
#define ONE_CYC          MID_CYC(ONE_LINE_CYC)
#define HBLANK_CYC       MID_CYC(340)
#define CLR_VBL_CYC      MID_CPU_CYC(2270)
/* 1֡��cpu���� - 1֡ʹ�õ����� = �հ����� */
#define VBLANK_CYC       (MID_CPU_CYC(1789772.5/60) - MID_CYC(ONE_LINE_CYC*262))
/* cpu��������ָ�������� */
#define CPU_RUN(cYc)     while (_cyc < cYc) { \
                            _cyc += MID_CPU_CYC( cpu->process() ); \
                         } \
                         _cyc -= cYc

void NesSystem::drawFrame() {
    CPU_RUN(START_CYC);

    ppu->clearVBL();
    ppu->startNewFrame();
    ppu->drawSprite(PPU::bpBehind);

    CPU_RUN(ONE_CYC);

    int x=0, y=0;

    while (y<240) {
        /* ����һ�� */
        for (;;) {
            ppu->drawPixel(x++, y);

            if (--_cyc <= 0) {
                _cyc += MID_CPU_CYC( cpu->process() );
            }

            if (x>=256) {
                break;
            }
        }

        mmc->drawLineOver();
        x=0; y++;

        /* ˮƽ�������� */
        CPU_RUN(HBLANK_CYC);
    }

    ppu->drawSprite(PPU::bpFront);

    CPU_RUN(END_CYC);
    ppu->oneFrameOver();
    ppu->sendingNMI();

return;
    /* ʵ��ʹ�õ�������cpu�����в��... */
    CPU_RUN(VBLANK_CYC);
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
            _cyc = 0;
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
