#ifndef MMC_H_INCLUDED
#define MMC_H_INCLUDED

#include "rom.h"

struct MapperImpl;

/* �л�ҳ��Ľӿ�, �����л�ROM��VROM, off=(0x8000-0xFFFF)  *
 * offΪ��ַ, valueΪд�������                            */
typedef void (*SWITCH_PAGE)(MapperImpl*, word off, byte value);
/* ת��ROM��ַ�Ľӿ�, off=(0x8000-0xFFFF), ����ת����ĵ�ַ*
 * ת����ĵ�ַҲ������(0x8000-0xFFFF)                     */
typedef word (*ROM_READER )(MapperImpl*, word off);
/* ��ROM_READER����                                        */
typedef word (*VROM_READER)(MapperImpl*, word off);


struct MapperImpl {
    /* ����ڴ�д��,�л�ҳ��,���Կ�   */
    SWITCH_PAGE     sw;
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
};

#endif // MMC_H_INCLUDED
