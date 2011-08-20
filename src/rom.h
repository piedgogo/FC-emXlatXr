#ifndef ROM_H_INCLUDED
#define ROM_H_INCLUDED

#include "type.h"
#include "string"

using std::string;

struct nes_file {

#define NES_FILE_MAGIC      0x1A53454E // 4E 45 53 1A
#define NES_FILE_HEAD_LEN   16
#define NES_FILE_HAS_TRA(x) (x->t1 & 0x04)
#define NES_FILE_TRA_SIZE   512

    union {
	byte magic[4];	/* �ַ�����NES^Z������ʶ��.NES�ļ�                    */
	int  magic_i;
    };

	byte rom_size;	/* 16kB ROM����Ŀ                                   */
	byte vrom_size;	/* 8kB VROM����Ŀ                                   */
	byte t1;		/* D0��1����ֱ����0��ˮƽ����
				��   * D1��1���е�ؼ��䣬SRAM��ַ$6000-$7FFF
				��   * D2��1����$7000-$71FF��һ��512�ֽڵ�trainer
                     * D3��1��4��ĻVRAM����
				��   * D4��D7��ROM Mapper�ĵ�4λ                        */
	byte t2;		/* D0��D3��������������0��׼����Ϊ��Mapper��^_^��   *
				��   * D4��D7��ROM Mapper�ĸ�4λ                        */
	byte zero[8];	/* ������������0                                    */

	byte trainer[NES_FILE_TRA_SIZE];  /* 7000~71ff��512���ֽڣ� Mapper��*
                                       * ���ͷ��ڸõط���ROM�Ĵ���������*
                                       * ���ǡ� ��ϷTrainer�����ɲ˵��� *
                                       * �����                         */

	byte *rom;		/* 16KxM  ROM���������У��������trainer��
					 * ����512�ֽڰ���ROM��֮ǰ                         */
	byte *vrom;		/* 8KxN  VROM��, ��������                           */

    /* ��ӡROM������,offset����ʼ��ַ,lenΪ��ӡ����                     */
    void printRom(int offset, int len);
    /* ��ӡROM����Ϣ                                                    */
    void romInfo();
    /* ����Mapper������ֵ                                               */
    word mapperId();

    ~nes_file();
};

#define LOAD_ROM_SUCCESS         0
#define ER_LOAD_ROM_PARM        -1
#define ER_LOAD_ROM_OPEN        -2
#define ER_LOAD_ROM_HEAD        -3
#define ER_LOAD_ROM_TRAINER     -4
#define ER_LOAD_ROM_SIZE        -5
#define ER_LOAD_ROM_VSIZE       -6
#define ER_LOAD_ROM_BADMAP      -7
#define ER_LOAD_ROM_UNKNOW      -99

/**
 * ��ȡfilename�ļ���rom�ṹ��,�ɹ�����0������rom����
 * rom������������Ч��, ʧ�ܷ������涨��Ĵ�����
 */
int load_rom(nes_file* rom, const string* filename);

#endif // ROM_H_INCLUDED
