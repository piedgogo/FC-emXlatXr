#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED


typedef unsigned char byte;
typedef unsigned short int word;

const byte SUCCESS = 0;
const byte FAILED  = -1;

struct nes_file {

#define NES_FILE_MAGIC 0x1A53454E //4E 45 53 1A

	byte magic[4];	// �ַ�����NES^Z������ʶ��.NES�ļ�
	byte rom_size;	// 16kB ROM����Ŀ
	byte vrom_size;	// 8kB VROM����Ŀ
	byte t1;		/*  D0��1����ֱ����0��ˮƽ����
				��  ��  D1��1���е�ؼ��䣬SRAM��ַ$6000-$7FFF
				��  ��  D2��1����$7000-$71FF��һ��512�ֽڵ�trainer
				��  ��  D3��1��4��ĻVRAM����
				��  ��  D4��D7��ROM Mapper�ĵ�4λ */
	byte t2;		/*	D0��D3��������������0��׼����Ϊ��Mapper��^_^��
				��  ��  D4��D7��ROM Mapper�ĸ�4λ  */
	byte zero[8];	// ������������0
	byte trainer[512];
	byte *rom;		// 16KxM  ROM���������У��������trainer��
					// ����512�ֽڰ���ROM��֮ǰ
	byte *vrom;		// 8KxN  VROM��, ��������
};

struct cpu_6502 {
	/* NTSC��ʽ��������Ƶ��Ϊ1.7897725 MHz
	 * PAL��ʽ��������Ƶ��Ϊ1.773447 MHz */

    byte A; 	    // �ۼ���
    byte Y;	    	// �����ݴ���
    byte X;	    	// �����ݴ���

    union {
    struct {
        byte PCL;   // intel�ߵ�λ����
        byte PCH;
    };
    word PC;
    };  			// ���������,ָ����һ��Ҫִ�е�ָ��

    word SP;		/* �ѵ�ָʾ�� 0x0100-0x01FF
                       �ݼ���, ָ��'��', ÿ��2�ֽ� */

#define CPU_FLAGS_NEGATIVE    (1<<7)  // ��ֵ
#define CPU_FLAGS_OVERFLOW    (1<<6)  // �����
#define CPU_FLAGS_BREAK       (1<<4)  // ���ж�
#define CPU_FLAGS_DECIMAL     (1<<3)  // ʮ����
#define CPU_FLAGS_INTERDICT   (1<<2)  // ��ֹ�ж�
#define CPU_FLAGS_ZERO        (1<<1)  // ���Ϊ��
#define CPU_FLAGS_CARRY       1       // �н�λ

    byte FLAGS;	    /* ״̬�ݴ���  N V 1 B D I Z C
					   N��ֵ 	V���		B�ж�����
					   Dʮ����	I��Ͻ���	Z��ֵ
					   C��λ */
};

union memory {
    struct {
	byte start[0x0100];	// $0000-$00FF ϵͳ��ҳ
	byte stact[0x0100];	// $0100-$01FF ϵͳ��ջ
	byte graph[0x0200];	// $0200-$03FF ��ͨͼ�ζ���
	byte data [0x0400];	// $0400-$07FF CPU�����ݴ�
	byte null [0x1800];	// $0800-$1FFF ����
	byte user [0x6000];	// $2000-$7FFF i/o�����û�������
	byte rom  [0x8000];	/* $8000-$FFFF ��Ϸ���򵱳���>32KB����
							$8000-$BFFF��16KB����д洢�л� */
    };
    byte idx  [0x10000];
};

union vram {
    struct {
	byte part [0x1000];	// $0000-$0FFF ��ͨͼ�ο�
	byte bgg  [0x1000];	// $1000-$1FFF �����ַ�ͼ��
	byte bg1m [0x03C0];	// $2000-$23BF ������һҳӳ��
	byte bg1c [0x0040];	// $23C0-$23FF ������һҳ��ɫ��
	byte bg2m [0x03C0];	// $2400-$27BF �����ڶ�ҳӳ��
	byte bg2c [0x0040];	// $27C0-$27FF �����ڶ�ҳ��ɫ��
	byte bg3m [0x03C0];	// $2800-$2BBF ��������ҳӳ��
	byte bg3c [0x0040];	// $2BC0-$2BFF ��������ҳ��ɫ��
	byte bg4m [0x03C0];	// $2C00-$2FBF ��������ҳӳ��
	byte bg4c [0x0040];	// $2FC0-$2FFF ��������ҳ��ɫ��
	byte nul1 [0x0F00];	// $3000-$3EFF ��
	byte cdat [0x0020];	// $3F00-$3F1F ������ͨ��ɫ��������
	byte nul2 [0x00E0];	// $3F20-$3FFF Ϊ��
	};
	byte idx  [0x4000];
};

#endif // TYPE_H_INCLUDED
