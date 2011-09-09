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
#ifndef MEM_H_INCLUDED
#define MEM_H_INCLUDED

#include "type.h"
#include "rom.h"
#include "mmc.h"
#include "ppu.h"
#include "pad.h"

/*--------------------------------------------------*-
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
    /** �����ڴ�״̬ȫ����0                        */
    void hard_reset();
    /** ������,��������ڴ�                        */
    void soft_reset();
    /** ���Զ�ȡȫ����ַ                           */
    byte read(const word offset);
    /** ����д��ȫ����ַ                           */
    void write(const word offset, const byte data);
    /** ר��������ȡ���� offset=(0x8000,0xFFFF)    */
    byte readPro(const word offset);
};

#endif // MEM_H_INCLUDED
