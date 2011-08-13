#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#define HELP_FNC static inline

typedef unsigned char       byte;
typedef unsigned short int  word;
typedef unsigned int        dword;
typedef dword               T_COLOR; /* ���ɫ��������ֽ�Ϊ0xFF͸��0xTTRRGGBB */

const byte SUCCESS = 0;
const byte FAILED  = -1;

void welcome();

/* ��ʵ������,������˵��vram�Ľṹ */
union vram {
    /* ÿ�� 32�� x 30�� ��ͼ�ε�Ԫ, ����ʾ960����Ԫ            */
    struct {
    /*----------------------------------------| vrom ӳ�� |----*/
    /* ÿ��ͼ�ε�Ԫ8x8����,16�ֽ�,ÿ���Ᵽ��256��ͼ�ε�Ԫ      *
     * ÿ��ͬ��64����ͨ��Ԫ(һ��ҳ0x100�ֽ�)                   *
     * ��ͨ�������ڴ���, 4�ֽ�: 1.Y 2.�ֿ���� 3.��״ 4.X      */

	byte part [0x1000];	/* $0000-$0FFF ��ͨͼ�ο�              */
	byte bgg  [0x1000];	/* $1000-$1FFF �����ַ�ͼ��            */

	/*----------------------------------------| vram ӳ�� |----*/
	byte bg1m [0x03C0];	// $2000-$23BF ������һҳӳ�� 960(�ֽ�)��ͼ�ε�Ԫ
	byte bg1c [0x0040];	// $23C0-$23FF ������һҳ��ɫ�� 64(�ֽ�)����ɫ��Ԫ 0x3FF 1KB

	byte bg2m [0x03C0];	// $2400-$27BF �����ڶ�ҳӳ��
	byte bg2c [0x0040];	// $27C0-$27FF �����ڶ�ҳ��ɫ�� 0x7FF 2KB

	byte bg3m [0x03C0];	// $2800-$2BBF ��������ҳӳ��
	byte bg3c [0x0040];	// $2BC0-$2BFF ��������ҳ��ɫ�� 0xBFF 3KB

	byte bg4m [0x03C0];	// $2C00-$2FBF ��������ҳӳ��
	byte bg4c [0x0040];	// $2FC0-$2FFF ��������ҳ��ɫ�� 0xFFF 4KB

	byte nul1 [0x0F00];	// $3000-$3EFF $2000 - $2EFF �ľ���
	byte cdat [0x0020];	// $3F00-$3F1F ������ͨ��ɫ�������� ��16�ֽ�
	byte nul2 [0x00E0];	// $3F20-$3FFF Ϊ��  $3F00-$3F1F ��7�ξ���
                        // $4000-$FFFF $0000 - $3FFF �ľ���
	};
	byte idx  [0x4000];
};

#endif // TYPE_H_INCLUDED
