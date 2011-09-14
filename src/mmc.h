/*----------------------------------------------------------------------------*|
|*                                                                            *|
|* FC ģ���� (Famicom��Nintendo��˾��1983��7��15�����ձ����۵�8λ��Ϸ��)      *|
|*                                                                            *|
|* $ C++���Եĵ�һ����Ŀ,���������ְ�                                         *|
|* $ è��д��, �����ñ����������ע������                                     *|
|* $ ���߶�ʹ�ñ�������ɵĺ�������κ�����                                   *|
|* $ �಻��Դ���Ĺ���ԭ������һ������,�����ش������벦��119                 *|
|*                                                                            *|
|* > ʹ�� [Code::Block 10.05] ��������                                        *|
|* > ������ʹ�� [MinGW 3.81] [gcc 4.4.1]                                      *|
|* > �ο������� [http://nesdev.parodius.com] ��վ������                       *|
|* > ��л [Flubba,blargg] ��ƵĲ��Գ���, ����������Ч�ʳ�ָ������            *|
|*                                                                            *|
|* ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ | CatfoOD |^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ *|
|*                                           | yanming-sohu@sohu.com          *|
|* [ TXT CHARSET WINDOWS-936 / GBK ]         | https://github.com/yanmingsohu *|
|*                                           | qQ:412475540                   *|
|*----------------------------------------------------------------------------*/
#ifndef MMC_H_INCLUDED
#define MMC_H_INCLUDED

#include "rom.h"
#include <stdio.h>

#define MMC_CAPABILITY_CHECK_SWITCH     (   1) /* ��Ҫ���˿�д�����л�ҳ�� */
#define MMC_CAPABILITY_CHECK_LINE       (1<<1) /* ÿ�л��ƽ�����Ҫ֪ͨMMC    */
#define MMC_CAPABILITY_WRITE_VROM       (1<<2) /* ��VROM����VRAMʹ��         */

struct PPU;

class MapperImpl {

public:
    const nes_file* rom;
    PPU*            ppu;
    byte*           IRQ;

    /* �л�mapperʱ�ͷŵ�ǰʹ�õ���Դ */
    virtual ~MapperImpl() { rom = NULL; }

    /* �л�ҳ��Ľӿ�, �����л�ROM��VROM, off=(0x8000-0xFFFF)
       offΪ��ַ, valueΪд�������, Ĭ�ϴ�ӡ���� */
    virtual void sw_page(word off, byte value) {
#ifdef SHOW_ERR_MEM_OPERATE
        printf("MMC::������д������: 0x%x = 0x%x \n", off, value);
#endif
    }

    /* ת��ROM��ַ�Ľӿ�, off=(0x8000-0xFFFF), ����ת����ĵ�ַ������ */
    virtual byte r_prom(word off) = 0;

    /* ��ROM_READER���� off=(0x0-0x1FFF) */
    virtual byte r_vrom(word off) {
        return rom->vrom[off];
    }

    /* �еĿ���ʹ����չ�Դ�, ����ͨ�����ṩvrom, Ĭ�ϴ�ӡ���� */
    virtual void w_vrom(word off, byte value) {
#ifdef SHOW_ERR_MEM_OPERATE
        printf("MMC::can't write VROM $%04x=%02x.\n", off, value);
#endif
    }

    /* ppu����һ�н����÷��������� */
    virtual void draw_line() {}

    /* ��ʼ��,cpu��λʱ��Ҫִ�� */
    virtual void reset() {}

    /* ������Ҫ���õķ�������������ĺ���û�з���������������ᱻ���� */
    virtual uint capability() { return MMC_CAPABILITY_WRITE_VROM; }
};

/**
 * ͨ��ֻҪһ��MMC���󼴿�,ͨ����ȡ��ͬ��rom
 * ���ı��ȡrom�ķ�ʽ
 */
struct MMC {

private:
    nes_file*   rom;
    MapperImpl* sw;
    PPU*        ppu;
    byte*       IRQ;
    uint        capability;

public:
    MMC() : rom(0), sw(0)
    {
    }

    /* �л�rom, �ò�����ı�Mapper, �����֧��MMC����false
     * ͬʱǰһ��Mapper�ᱻ�ͷ� */
    bool loadNes(nes_file* rom);

    /* ��ȡ����� addr=(0x8000-0xFFFF) */
    byte readRom(const word addr) {
#ifdef SHOW_ERR_MEM_OPERATE
        if (addr<0x8000) {
            printf("MMC::��ȡROM����:ʹ������Ч�ĳ����ַ 0x%x\n", addr);
        }
#endif
        return sw->r_prom(addr);
    }

    /* ��ȡvrom�ֿ�� addr=(0x0000-0x1FFF) */
    byte readVRom(const word addr) {
        return sw->r_vrom(addr);
    }

    /* �еĿ�����д */
    void writeVRom(const word addr, const byte value) {
        if (capability & MMC_CAPABILITY_WRITE_VROM) {
            sw->w_vrom(addr, value);
        }
    }

    /* �����ڴ�д����ʱִ�л�ҳ���� */
    void checkSwitch(const word addr, const byte value) {
        if (capability & MMC_CAPABILITY_CHECK_SWITCH) {
            sw->sw_page(addr, value);
        }
    }

    /* ��ʼ��mapper��� */
    void resetMapper() {
        sw->reset();
    }

    /* ����vrom�ĳ���(�ֽ�) */
    int vromSize() {
        return rom->vrom_size * 8 * 1024;
    }

    /* һ�л��ƽ����÷��������� */
    void drawLineOver() {
        if (capability & MMC_CAPABILITY_CHECK_LINE) {
            sw->draw_line();
        }
    }

    void setPPU(PPU* p) {
        ppu = p;
    }

    void setIRQ(byte* i) {
        IRQ = i;
    }
};

#endif // MMC_H_INCLUDED
