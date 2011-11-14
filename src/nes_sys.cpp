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
    : pad(_pad), _cyc(0)
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
#define MID_CPU_CYC(x)   ((x)*3)
#define MID_CYC(x)       ((x)/4)
#define ONE_LINE_CYC     1364
#define START_CYC        MID_CYC(ONE_LINE_CYC*20)
#define END_CYC          MID_CYC(ONE_LINE_CYC)
#define END_CYC_EVERY    MID_CYC(ONE_LINE_CYC-2)
#define ONE_CYC          MID_CYC(ONE_LINE_CYC)
#define HBLANK_CYC       MID_CYC(340)

/* ������δ�Ż� */
void NesSystem::drawFrame() {

    ppu->startNewFrame();
    cpu_run(START_CYC);

    ppu->clearVBL();
    cpu_run(ONE_CYC);

    ppu->drawSprite(PPU::bpBehind);

    int x=0, y=0;

    while (y<240) {
        ppu->startNewLine();
        /* ����һ�� */
        for (;;) {
            ppu->drawPixel(x++, y);

            if (--_cyc < 0) {
                _cyc += MID_CPU_CYC( cpu->process() );
            }

            if (x>=256) {
                break;
            }
        }

        mmc->drawLineOver();
        x=0; y++;

        /* ˮƽ�������� */
        cpu_run(HBLANK_CYC);
    }

    ppu->drawSprite(PPU::bpFront);

    if (every_f) {
        cpu_run(END_CYC_EVERY);
    } else {
        cpu_run(END_CYC);
    }
    every_f = !every_f;

    /* ����Ϊֹ����һ֡���� */
    ppu->sendingNMI();
    /* ����VBL=1 */
    ppu->oneFrameOver();
}

void NesSystem::warmTime() {
    ppu->startNewFrame();
    cpu_run( MID_CYC(ONE_LINE_CYC*241) );
    ppu->oneFrameOver();
    ppu->sendingNMI();

    ppu->startNewFrame();
    cpu_run( MID_CYC(ONE_LINE_CYC*262) );
    ppu->oneFrameOver();
    ppu->sendingNMI();
}

void NesSystem::cpu_run(int cyc) {
    while (_cyc <= cyc) {
        _cyc += MID_CPU_CYC( cpu->process() );
    }
    _cyc -= cyc;
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
            ppu->switchMirror(rom->t1 & 0x05);
            ppu->reset();
            cpu->RES = 1;
            _cyc = 0;
            every_f = false;

            warmTime();
        } else {
            res = ER_LOAD_ROM_BADMAP;
        }
    }

    return res;
}

cpu_6502* NesSystem::getCpu() {
    return cpu;
}

PPU* NesSystem::getPPU() {
    return ppu;
}

memory* NesSystem::getMem() {
    return ram;
}

PlayPad* NesSystem::getPad() {
    return pad;
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
