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

#ifdef ANY_WHERE_STEPDBG
bool __stop_and_debug__ = 0;
#endif


NesSystem::NesSystem(PlayPad *_pad, HWND hwnd)
    : pad(_pad), _cyc(0)
{
    apu = new Apu(hwnd);
    mmc = new MMC();
    ppu = new PPU(mmc);
    ram = new memory(mmc, ppu, pad, apu);
    cpu = new cpu_6502(ram);
    rom = new nes_file();

    apu->stop();
    ppu->setNMI(&cpu->NMI);
    mmc->setPPU(ppu);
    mmc->setIRQ(&cpu->IRQ);
    mmc->setApu(apu);
    apu->setIrq(&cpu->IRQ);
}

NesSystem::~NesSystem() {
    delete apu;
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
void NesSystem::drawFrame(Video* video) {

    PPU::pPainter ptr = ppu->startNewFrame(video);
    cpu_run(START_CYC);

    ptr->clearVBL();
    cpu_run(ONE_CYC);

    ptr->drawSprite(PPU::bpBehind);

    int x=0, y=0;

    while (y<240) {
        ptr->startNewLine();
        /* ����һ�� */
        for (;;) {
            ptr->drawNextPixel();

            if (--_cyc < 0) {
                #ifdef ANY_WHERE_STEPDBG
                if (__stop_and_debug__) {
                    debugCpu(this);
                }
                #endif
                _cyc += MID_CPU_CYC( cpu->process() );
            }

            if (++x>=256) {
                break;
            }
        }

        mmc->drawLineOver();
        x=0; y++;

        /* ˮƽ�������� */
        cpu_run(HBLANK_CYC);
    }

    ptr->drawSprite(PPU::bpFront);

    if (every_f = !every_f && ppu->enableBG()) {
        cpu_run(END_CYC_EVERY);
    } else {
        cpu_run(END_CYC);
    }

    /* ����Ϊֹ����һ֡���� */
    ptr->sendingNMI();
    /* ����VBL=1 */
    ppu->oneFrameOver(ptr);
}

void NesSystem::warmTime(Video* video) {
    /*
    pPainter ptr = ppu->startNewFrame(video);
    cpu_run( MID_CYC(ONE_LINE_CYC*241) );
    prt->oneFrameOver();
    ptr->sendingNMI();
    ppu->oneFrameOver(ptr);

    ptr = ppu->startNewFrame(video);
    cpu_run( MID_CYC(ONE_LINE_CYC*262) );
    ppu->oneFrameOver();
    ppu->sendingNMI();
    ppu->oneFrameOver(ptr);
    */
}

void NesSystem::cpu_run(int cyc) {
    while (_cyc <= cyc) {
        #ifdef ANY_WHERE_STEPDBG
        if (__stop_and_debug__) {
            debugCpu(this);
        }
        #endif
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
            apu->stop();
            cpu->RES = 1;
            _cyc = 0;
            every_f = false;

            warmTime(0);
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
