#ifndef ROM_H_INCLUDED
#define ROM_H_INCLUDED

#include "type.h"
#include "string"
#include "stdio.h"

using std::string;

struct nes_file {

#define NES_FILE_MAGIC      0x1A53454E //4E 45 53 1A
#define NES_FILE_HEAD_LEN   16
#define NES_FILE_HAS_TRA(x) (x->t1 & 0x04)
#define NES_FILE_TRA_SIZE   512

    union {
	byte magic[4];	// �ַ�����NES^Z������ʶ��.NES�ļ�
	int  magic_i;
    };
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

	byte trainer[NES_FILE_TRA_SIZE];  /* 7000~71ff��512���ֽڣ�
                     * Mapper�����ͷ��ڸõط���ROM�Ĵ������������ǡ�
                     * ��ϷTrainer�����ɲ˵�������� */

	byte *rom;		// 16KxM  ROM���������У��������trainer��
					// ����512�ֽڰ���ROM��֮ǰ
	byte *vrom;		// 8KxN  VROM��, ��������

    void printRom(int offset, int len) {
        if (offset%16!=0) printf("\n0x%x: ", offset);

        for (int i=offset; i<len+offset; ++i) {
            if (i%16==0) printf("\n0x%x: ", i);
            printf(" %02x", rom[i]);
        }

        printf("\n");
    }
};


/**
 * ��ȡfilename�ļ���rom�ṹ��,�ɹ�����0������rom����
 * rom������������Ч��, ʧ�ܷ���-1
 */
int load_rom(nes_file* rom, const string* filename);

#endif // ROM_H_INCLUDED
