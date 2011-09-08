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

void NesSystem::drawFrame2() {

}

#define START_FRAME (N_HLINE_CPU_CYC + N_HBLANK_CPU_CYC) \
                   *(N_VLINE_COUNT - PPU_DISPLAY_N_HEIGHT)

void NesSystem::drawFrame() {
    static int cpu_cyc = 0;

#ifdef SHOW_PPU_DRAW_INFO
    printf("һ֡��ʼ,n֡�����ػ���\n");
#endif
    ppu->oneFrameOver();
    /* �Ȼ���20֡(�д���֤) */
    while (cpu_cyc < START_FRAME) {
        cpu_cyc += cpu->process();
    }
    cpu_cyc -= START_FRAME;

    ppu->startNewFrame();
    ppu->drawSprite(PPU::bpBehind);

    int x = 0, y = 0;
    double ppu_cyc = 0, cyc = 0;

    while (y<PPU_DISPLAY_N_HEIGHT) {
        /* ����һ�� */
        while (cpu_cyc<N_HLINE_CPU_CYC) {
            cyc = cpu->process();
            cpu_cyc += cyc;
            ppu_cyc += cyc;

            while (ppu_cyc>=N_PIXEL_CPU_CYC) {
                ppu->drawPixel(x++, y);
                ppu_cyc -= N_PIXEL_CPU_CYC;
            }
        }
        cpu_cyc -= N_HLINE_CPU_CYC;
        x = 0; y++;
#ifdef SHOW_PPU_DRAW_INFO
        printf("һ�����,ˮƽ���� %X,%X,,", x, y);
#endif

        /* ˮƽ�������� */
        while (cpu_cyc<N_HBLANK_CPU_CYC) {
            cpu_cyc += cpu->process();
        }
        cpu_cyc -= N_HBLANK_CPU_CYC;
#ifdef SHOW_PPU_DRAW_INFO
        printf("��������\n");
#endif
    }
    ppu->drawSprite(PPU::bpFront);

#ifdef SHOW_PPU_DRAW_INFO
    printf("����һ֡����\n");
#endif
}

int NesSystem::load_rom(string filename) {
    int res = ::load_rom(rom, &filename);

    if (!res) {
        rom->romInfo();

        if (mmc->loadNes(rom)) {
            printf("INT vector(0xFFFA-0xFFFF): ");
            rom->printRom(0xFFFA - 0x8000, 6);
            ram->reset();
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
