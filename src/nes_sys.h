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
    int         _cyc; /* ϵͳʱ��,ÿ֮֡�以�� */

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
    /* PPUԤ������ */
    void warmTime();

    /* ��ʼcpu����ִ��,ͬʱ��Ҫ�򿪱��뿪�� */
    void debug();
};


#endif // NES_SYS_H_INCLUDED
