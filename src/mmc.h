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
#ifndef MMC_H_INCLUDED
#define MMC_H_INCLUDED

#include "rom.h"

struct MapperImpl;

/* �л�ҳ��Ľӿ�, �����л�ROM��VROM, off=(0x8000-0xFFFF)  *
 * offΪ��ַ, valueΪд�������                            */
typedef void  (*SWITCH_PAGE)(MapperImpl*, word off, byte value);
/* ת��ROM��ַ�Ľӿ�, off=(0x8000-0xFFFF), ����ת����ĵ�ַ*
 * ת����ĵ�ַ���������ⷶΧ                              */
typedef dword (*ROM_READER )(MapperImpl*, word off);
/* ��ROM_READER���� off=(0x0-0x1FFF)                       */
typedef dword (*VROM_READER)(MapperImpl*, word off);


struct MapperImpl {
    /* ����ڴ�д��,�л�ҳ��,���Կ�   */
    SWITCH_PAGE     sw_page;
    /* ��ȡ����ROM,����Ϊ��           */
    ROM_READER      r_prom;
    /* ��ȡ�Դ�ROM,��ʱΪ��(δʵ��)   */
    VROM_READER     r_vrom;

    const nes_file* rom;
    /* ��ǰ����ҳ��                   */
    byte            pr_page;
    /* ��ǰ�Դ�ҳ��                   */
    byte            vr_page;
};

/**
 * ͨ��ֻҪһ��MMC���󼴿�,ͨ����ȡ��ͬ��rom
 * ���ı��ȡrom�ķ�ʽ
 */
struct MMC {

private:
    nes_file* rom;
    MapperImpl *sw;

public:
    MMC();
    /* �л�rom, �ò�����ı�Mapper, �����֧��MMC����false */
    bool loadNes(nes_file* rom);
    /* ��ȡ����� addr=(0x8000-0xFFFF)                     */
    byte readRom(const word addr);
    /* ��ȡvrom�����, ��ʱδʵ�� addr=(0x0000-0x1FFF)     */
    byte readVRom(const word addr);
    /* �����ڴ�д����ʱִ�л�ҳ����                        */
    void checkSwitch(const word addr, const byte value);
    /* ����vrom�ĳ���(�ֽ�)                                */
    int  vromSize();
};

#endif // MMC_H_INCLUDED
