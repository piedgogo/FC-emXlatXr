#ifndef MEM_H_INCLUDED
#define MEM_H_INCLUDED

#include "type.h"
#include "rom.h"
#include "mmc.h"
#include "ppu.h"
#include "pad.h"

/*--------------------------------------------------*
 * $0000-$00FF ϵͳ��ҳ                             *
 * $0100-$01FF ϵͳ��ջ                             *
 * $0200-$03FF ��ͨͼ�ζ���                         *
 * $0400-$07FF CPU�����ݴ�                          *
 * $0800-$1FFF ����                                 *
 * $2000-$7FFF i/o�����û�������                    *
 * $6000-$7FFF Ϊ��ش浵(������)                   *
 * $8000-$FFFF ��Ϸ���򵱳���>32KB����              *
 * $8000-$BFFF ��16KB����д洢�л�(����ֱ��ʹ��)   *
 * $C000-$FFFF ����ֱ�ӷ���                         *
-*--------------------------------------------------*/
struct memory {

private:
    MMC     *mmc;
    PPU     *ppu;
    PlayPad *pad;
    byte    ram[0x07FF];

public:
    memory(MMC *mmc, PPU *_ppu, PlayPad* _pad);
    /** �����ڴ�״̬ȫ����0 */
    void reset();
    /** ���Զ�ȡȫ����ַ */
    byte read(const word offset);
    /** ����д��ȫ����ַ */
    void write(const word offset, const byte data);
    /** ר��������ȡ���� offset=(0x8000,0xFFFF) */
    byte readPro(const word offset);
};

#endif // MEM_H_INCLUDED
