#ifndef MEM_H_INCLUDED
#define MEM_H_INCLUDED

#include "type.h"
#include "rom.h"
#include "mmc.h"

/*
union memory {
    struct {
	byte start[0x0100];	// $0000-$00FF ϵͳ��ҳ
	byte stact[0x0100];	// $0100-$01FF ϵͳ��ջ
	byte graph[0x0200];	// $0200-$03FF ��ͨͼ�ζ���
	byte data [0x0400];	// $0400-$07FF CPU�����ݴ�
	byte null [0x1800];	// $0800-$1FFF ����
	byte user [0x6000];	// $2000-$7FFF i/o�����û�������
                        // $6000-$7FFF Ϊ��ش浵(������)
	byte rom  [0x8000];	// $8000-$FFFF ��Ϸ���򵱳���>32KB����
                        // $8000-$BFFF ��16KB����д洢�л�(����ֱ��ʹ��)
                        // $C000-$FFFF ����ֱ�ӷ���
    };
    byte idx  [0x10000];
};
*/

struct memory {

private:
    MMC *mmc;
    byte ram[0x07FF];

public:
    memory(MMC *mmc);
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
