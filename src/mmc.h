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

class MapperImpl {

public:
    const nes_file* rom;

    /* �л�mapperʱ�ͷŵ�ǰʹ�õ���Դ */
    virtual ~MapperImpl() { rom = NULL; }

    /* �л�ҳ��Ľӿ�, �����л�ROM��VROM, off=(0x8000-0xFFFF)
       offΪ��ַ, valueΪд������� */
    virtual void sw_page(word off, byte value) {}

    /* ת��ROM��ַ�Ľӿ�, off=(0x8000-0xFFFF), ����ת����ĵ�ַ
       ת����ĵ�ַӳ�䵽rom��(0~n) */
    virtual uint r_prom(word off) = 0;

    /* ��ROM_READER���� off=(0x0-0x1FFF) */
    virtual uint r_vrom(word off) {
        return off;
    }

    /* ��ʼ��,cpu��λʱ��Ҫִ�� */
    virtual void reset() {}
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
    /* �л�rom, �ò�����ı�Mapper, �����֧��MMC����false
     * ͬʱǰһ��Mapper�ᱻ�ͷ�                            */
    bool loadNes(nes_file* rom);
    /* ��ȡ����� addr=(0x8000-0xFFFF)                     */
    byte readRom(const word addr);
    /* ��ȡvrom�����, ��ʱδʵ�� addr=(0x0000-0x1FFF)     */
    byte readVRom(const word addr);
    /* �����ڴ�д����ʱִ�л�ҳ����                        */
    void checkSwitch(const word addr, const byte value);
    /* ��ʼ��mapper���                                    */
    void resetMapper();
    /* ����vrom�ĳ���(�ֽ�)                                */
    int  vromSize();
};

#endif // MMC_H_INCLUDED
