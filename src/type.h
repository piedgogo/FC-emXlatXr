/*----------------------------------------------------------------------------*|
|*                                                                            *|
|* FC ģ���� (Famicom��Nintendo��˾��1983��7��15�����ձ����۵�8λ��Ϸ��)      *|
|*                                                                            *|
|* $ C++���Եĵ�һ����Ŀ,���������ְ�                                         *|
|* $ è��д��, �����ñ����������ע������                                     *|
|* $ ���߶�ʹ�ñ�������ɵĺ�������κ�����                                   *|
|* $ �಻��Դ���Ĺ���ԭ������һ������,�����ش������벦��119                 *|
|*                                                                            *|
|* > ʹ�� [Code::Block 10.05] ��������                                        *|
|* > ������ʹ�� [MinGW 3.81] [gcc 4.4.1]                                      *|
|* > �ο������� [http://nesdev.parodius.com] ��վ������                       *|
|* > ��л [Flubba,blargg] ��ƵĲ��Գ���, ����������Ч�ʳ�ָ������            *|
|*                                                                            *|
|* ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ | CatfoOD |^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ *|
|*                                           | yanming-sohu@sohu.com          *|
|* [ TXT CHARSET WINDOWS-936 / GBK ]         | https://github.com/yanmingsohu *|
|*                                           | qQ:412475540                   *|
|*----------------------------------------------------------------------------*/
#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#define SF_NAME_JYM      "FC ģ���� DEmo. -=CatfoOD=- "
#define SF_VERSION_JYM   "v0.03"

/*-------------------------------------------| ����DEBUG��Ϣ |----*/
/* ��ʾNMI�����뷵�ص���Ϣ                                        */
#define NMI_DEBUG
/* ������ʾCPU�������ش���                                      */
//#define SHOW_CPU_OPERATE
/* ������Ч�ڴ�ʱ�������                                         */
#define SHOW_ERR_MEM_OPERATE
/* ��ʾ����PPU����Ϣ                                              */
//#define SHOW_PPU_REGISTER
/* ��ʾCPUѰַ��Ϣ                                                */
//#define SHOW_CPU_MEMORY_ADDRESSING
/* ����ȫ���жϱ���, ֻҪ����Ϊ1�������ж�ִ��, ����ʼ����        */
#define ANY_WHERE_STEPDBG
/*-----------------------------------------------| END DEBUG |----*/

#define HELP_FNC     static inline
#define _CSTR(x)     const_cast<char*>(x)

#define DBG_FILE_NAME_LEN 10
/* ���б�����Ϣ�Ĵ�ӡ��Ϣ,���ڵ��� */
#define PRINT(fmt,...) { char *___p = __FILE__ ; int ___len = strlen(___p); \
                         if (___len > DBG_FILE_NAME_LEN) \
                             ___p += ___len - DBG_FILE_NAME_LEN; \
                         printf("[ %s .%d ] ", ___p, __LINE__); \
                         printf((fmt), __VA_ARGS__); }

/* if __DEBUG_NES__ start */
#ifdef __DEBUG_NES__
#define TNUL(x,fmt, ...) if (!(x)) PRINT(fmt, __VA_ARGS__)
#define DBGPRT(fmt, ...) PRINT(fmt, __VA_ARGS__)
#else
/* else not define __DEBUG_NES__ */
#define DBGPRT(fmt, ...)
#define TNUL(x,fmt, ...)
#endif
/* if __DEBUG_NES__ over */

/*------------------| DIV�������� |----*/
#define DIV2(x)     ((x)>>1)
#define DIV4(x)     ((x)>>2)
#define DIV8(x)     ((x)>>3)
#define DIV16(x)    ((x)>>4)
#define DIV32(x)    ((x)>>5)
#define DIV64(x)    ((x)>>6)
/*---------------------| DIV over |----*/

typedef unsigned int        uint;
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

#ifdef ANY_WHERE_STEPDBG
extern bool __stop_and_debug__;
#endif

#endif // TYPE_H_INCLUDED
