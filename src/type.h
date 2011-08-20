#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#define HELP_FNC static inline

/*------------------| DIV�������� |----*/
#define DIV2(x)     ((x)>>1)
#define DIV4(x)     ((x)>>2)
#define DIV8(x)     ((x)>>3)
#define DIV16(x)    ((x)>>4)
#define DIV32(x)    ((x)>>5)
#define DIV64(x)    ((x)>>6)
/*---------------------| DIV over |----*/

typedef unsigned char       byte;
typedef unsigned short int  word;
typedef unsigned int        dword;
/* ���ɫ��������ֽ�Ϊ0xFF͸��0xTTRRGGBB */
typedef dword               T_COLOR;

const byte SUCCESS = 0;
const byte FAILED  = -1;

void welcome();
/* ���������ݴ�ӡ������̨ */
void printArr(byte* arr, int startIdx, int length);
/* ��load_rom�������صĴ������ת��Ϊ��Ϣ�ַ��� */
char* parseOpenError(int load_err_code);

/*---------------------------| �����������ʾ��ص�DEBUG��Ϣ |----*/
/* ��ʾNMI�����뷵�ص���Ϣ                                        */
//#define NMI_DEBUG
/* ������ʾCPU�������ش���                                      */
//#define SHOW_CPU_OPERATE
/* ������Ч�ڴ�ʱ�������                                         */
#define SHOW_ERR_MEM_OPERATE
/* ��ʾ����PPU����Ϣ                                              */
//#define SHOW_PPU_REGISTER
/* ��ʾCPUѰַ��Ϣ                                                */
#define SHOW_CPU_MEMORY_ADDRESSING
/*-----------------------------------------------| END DEBUG |----*/

#endif // TYPE_H_INCLUDED
