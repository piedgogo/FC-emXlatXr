#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED


typedef unsigned char byte;
typedef unsigned short int word;

const byte SUCCESS = 0;
const byte FAILED  = -1;


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
