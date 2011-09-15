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
#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include "type.h"
#include "mem.h"
#include "ppu.h"

#define CPU_NTSC      1789772.5  /* Hz */
#define CPU_PAL       1773447

struct command_6502;
struct command_parm;
struct cpu_6502;

/* cpuѰַ��ʽ */
enum CPU_ADDRESSING_MODE {

    ADD_MODE_$zpgX$  = 0x00,
    ADD_MODE_zpg     = 0x04,
    ADD_MODE_imm     = 0x08,
    ADD_MODE_abs     = 0x0C,
    ADD_MODE_$zpg$Y  = 0x10,
    ADD_MODE_zpgX    = 0x14,
    ADD_MODE_zpgY    = 0x15,
    ADD_MODE_absY    = 0x18,
    ADD_MODE_absX    = 0x1C,
    ADD_MODE_acc     = 0x28, /* ʹ��A��ֵ               */
    ADD_MODE_rel     = 0xEF, /* ���Ѱַ                */
    ADD_MODE_not     = 0xFF, /* ��Ѱַ                  */

    ADD_MODE_inX     = 0x00, /* $zpgX$����              */
    ADD_MODE_inY     = 0x10, /* $zpgX$����              */
    ADD_MODE_zpX     = 0x14, /* zpgX����                */
    ADD_MODE_zpY     = 0x15, /* zpgY����                */
    ADD_MODE_abY     = 0x18, /* absX����                */
    ADD_MODE_abX     = 0x1C, /* absX����                */
};

/* ������������ݲ��� */
struct command_parm {

    command_6502 *cmd;
    memory       *ram;
    cpu_6502     *cpu;
    int          mem_time;   /* Ѱַ����ʱ��,�����Զ���λ  */

    byte op;                 /* ����Ĵ���                 */
    byte p1;                 /* ��һ������(�����)         */
    byte p2;                 /* �ڶ�������(�����)         */
    word addr;               /* ��ǰָ��ĵ�ַ             */

    /* �����ڴ�Ѱַ��ʽ���ظõ�ַ���ϵ�����            *
     * ��ͬѰַ���͵���ָͬ�����Ʒƫ������ͬ(Ҳ������)*
     * ����,������cpuָ���ȥƫ�Ƶõ���ָ���Ѱַ����  */
    inline byte read(const byte addressing_mode);

    /* �����ڴ�Ѱַ��ʽ��valueд�뵽�õ�ַ             */
    inline void write(const byte addressing_mode, byte value);

    /* �����ڴ�Ѱַ��ʽ��ȡ�õ�ַ������                */
    inline word getAddr(const byte addressing_mode);

    /*---------------| Ѱַ�㷨����, �������ص�ַ |----*/
    word  abs   ();
    word  absX  ();
    word  absY  ();
    word  zpg   ();
    word  zpgX  ();
    word  zpgY  ();
    word  $zpg$Y();
    word  $zpgX$();
    word  $ind$ (byte x);
};

struct cpu_6502 {

private:
    byte NMI_idle;               /* NMI������Ϊ1                     */
    byte m_showDebug;            /* SHOW_CPU_MEMORY_ADDRESSING,
                                  * SHOW_CPU_OPERATE ���ú�,�������� */

public:
#define CPU_INTERRUPT_CYC   8    /* �ж������ִ������               */
#define CPU_RESET_CYC       6    /* ��λ����ִ������                 */
#define CPU_NMI_CYC        17    /* ���������ж�ִ������             */

    byte A; 	                 /* �ۼ���                           */
    byte Y;	    	             /* �����ݴ���                       */
    byte X;	    	             /* �����ݴ���                       */

    byte SP;		             /* �ѵ�ָʾ�� 0x0100-0x01FF
                                  * �ݼ���, ָ��'��', ÿ��1�ֽ�      */
    union {
        struct { byte PCL; byte PCH; };
        word PC;                 /* ���������,ָ����һ��Ҫִ�е�ָ��*/
    };

#define CPU_FLAGS_NEGATIVE    (1<<7)  /* ��ֵ                        */
#define CPU_FLAGS_OVERFLOW    (1<<6)  /* �����                      */
#define CPU_FLAGS_CONST       (1<<5)  /* ʼ��Ϊ1                     */
#define CPU_FLAGS_BREAK       (1<<4)  /* ���ж�                      */
#define CPU_FLAGS_DECIMAL     (1<<3)  /* ʮ����                      */
#define CPU_FLAGS_INTERDICT   (1<<2)  /* ��ֹ�ж�                    */
#define CPU_FLAGS_ZERO        (1<<1)  /* ���Ϊ��                    */
#define CPU_FLAGS_CARRY        1      /* �н�λ                      */

    byte FLAGS;	    /* ״̬�ݴ���  N V 1 B D I Z C                   *
					 * N��ֵ 	V���		B�ж�����                *
					 * Dʮ����	I��Ͻ���	Z��ֵ                    *
					 * C��λ                                         */
    byte NMI;       /* ���>0��ִ�в�������֮�ж�                    */
    byte IRQ;       /* ���>0��ִ�п�����֮�ж�                      */
    byte RES;       /* ���>0��ִ�и�λ����                          */

    memory       *ram;          /* �ڴ�                              */
    command_parm  prev_parm;    /* ǰһ������Ĳ���                  */

    cpu_6502(memory* ram);

    byte    reset();            /* ����cpu״̬,����ʹ������(soft)    */
    void    push(byte d);       /* ���ջ��ѹ��                      */
    byte    pop();              /* �Ӷ�ջ��ȡ��                      */
    char*   debug();            /* ����cpu״̬�ַ���                 */
    byte    process();          /* ����ǰ���ָ����һ����
                                 * ��,����ʹ�õĴ���������           */
    byte    irq();              /* ������ִ���ж�,����ʹ�õ�
                                 * ��������                          */
    void    rti();              /* ���жϹ����з���                  */
    byte    nmi();              /* ���������ε��ж�                */
    void    jump(word addr);    /* ��ת��addr�ڴ�ָ��ĵ�ַ(addr, addr+1)
                                 * ������֮ǰ��PC��FLAGS             */
    char*   cmdInfo();          /* ������һ��ָ�������              */


    void    checkNZ(byte value);/* ���value���λΪ1 ��N=1,����Ϊ0, *
                                 * ���value==0 ��Z=1,����Ϊ0        */
    void    checkZ(byte value);
    void    setV(bool set);     /* set==true ����������������      */
    void    clearV();

    void showDebug(byte show) { /* �Ƿ��������������ʾ�����Ϣ      */
        m_showDebug = show;
    }

    byte isShowDebug() {
        return m_showDebug;
    }
};

struct command_6502 {

    char    name[4];
    byte    time;
    byte    len;

    CPU_ADDRESSING_MODE type;

    /* ָ��������ָ�� */
    void (*op_func)(command_parm* parm);
};


#endif // CPU_H_INCLUDED
