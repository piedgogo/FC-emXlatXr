#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#define HELP_FNC static inline

typedef unsigned char       byte;
typedef unsigned short int  word;
typedef unsigned int        dword;
/* ���ɫ��������ֽ�Ϊ0xFF͸��0xTTRRGGBB */
typedef dword               T_COLOR;

const byte SUCCESS = 0;
const byte FAILED  = -1;

void welcome();
void printArr(byte* arr, int startIdx, int length);

/*---------------------------| �����������ʾ��ص�DEBUG��Ϣ |----*/
/* ��ʾNMI�����뷵�ص���Ϣ                                        */
// #define NMI_DEBUG
/* ������ʾCPU�������ش���                                      */
#define SHOW_CPU_OPERATE
/* ������Ч�ڴ�ʱ�������                                         */
#define SHOW_ERR_MEM_OPERATE
/*-----------------------------------------------| END DEBUG |----*/

#endif // TYPE_H_INCLUDED
