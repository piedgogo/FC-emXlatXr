#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include "type.h"
#include "mem.h"
#include "ppu.h"

struct command_6502;
struct command_parm;

struct cpu_6502 {

private:
    byte NMI_idle;               /* NMI������Ϊ1                     */
#ifdef SHOW_CPU_OPERATE
    byte showCmd;
#endif

public:
#define CPU_NTSC      1789772.5  /* Hz */
#define CPU_PAL       1773447

#define CPU_INTERRUPT_CYC   8    /* �ж������ִ������               */
#define CPU_RESET_CYC       6    /* ��λ����ִ������                 */
#define CPU_NMI_CYC        17    /* ���������ж�ִ������             */

    byte A; 	              /* �ۼ���                              */
    byte Y;	    	          /* �����ݴ���                          */
    byte X;	    	          /* �����ݴ���                          */

    byte SP;		          /* �ѵ�ָʾ�� 0x0100-0x01FF
                               * �ݼ���, ָ��'��', ÿ��1�ֽ�         */
    union {
        struct { byte PCL; byte PCH; };
        word PC;              /* ���������,ָ����һ��Ҫִ�е�ָ��   */
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
    command_parm *prev_parm;    /* ǰһ������Ĳ���                  */

    cpu_6502(memory* ram);

    byte    reset();            /* ����cpu״̬,����ʹ������          */
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
    void    checkV(word a, byte b);
                                /* ���a��b�����λ��ͬ,��v=1����Ϊ0
                                 * b�ǲ�������֮ǰ���ۼ�����ֵ       */
    void    clearV();

#ifdef SHOW_CPU_OPERATE
    void showCmds(byte show) {   /* �Ƿ��������������ʾ�����Ϣ      */
        showCmd = show;
    }
#endif
};

/* ������������ݲ���          */
struct command_parm {

    command_6502 *cmd;
    memory       *ram;
    cpu_6502     *cpu;

    byte op;  /* ����Ĵ���         */
    byte p1;  /* ��һ������(�����) */
    byte p2;  /* �ڶ�������(�����) */
    word addr;/* ��ǰָ��ĵ�ַ     */

    static const byte ADD_MODE_$zpgX$  = 0x00;
    static const byte ADD_MODE_zpg     = 0x04;
    static const byte ADD_MODE_imm     = 0x08;
    static const byte ADD_MODE_abs     = 0x0C;
    static const byte ADD_MODE_$zpg$Y  = 0x10;
    static const byte ADD_MODE_zpgX    = 0x14;
    static const byte ADD_MODE_absY    = 0x18;
    static const byte ADD_MODE_absX    = 0x1C;
    static const byte ADD_MODE_acc     = 0x28;

    /* �����ڴ�Ѱַ��ʽ���ظõ�ַ���ϵ�����            *
     * ��ͬѰַ���͵���ָͬ�����Ʒƫ������ͬ(Ҳ������)*
     * ����,������cpuָ���ȥƫ�Ƶõ���ָ���Ѱַ����  */
    inline byte read(const byte addressing_mode);

    /* �����ڴ�Ѱַ��ʽ��valueд�뵽�õ�ַ             */
    inline void write(const byte addressing_mode, byte value);

    /* �����ڴ�Ѱַ��ʽ��ȡ�õ�ַ������                */
    inline word getAddr(const byte addressing_mode);

    /*-----| Ѱַ�㷨����, �������ص�ַ |--------------*/
    word  abs   ();
    word  absX  ();
    word  absY  ();
    word  zpg   ();
    word  zpgX  ();
    word  zpgY  ();
    word  $zpg$Y();
    word  $zpgX$();
    word  $$$   (byte x, byte y);
};

struct command_6502 {

    char    name[4];
    byte    time;
    byte    len;

    /* ������Ͷ���ʵ��������...���ĵ������� */
    enum {
      vt_op_not = 0
    , vt_op_imm   /* ����1                   */
    , vt_op_zpg   /* ��ҳ2 , ��λΪ0x00      */
    , vt_op_rel   /* ���3                   */
    , vt_op_abs   /* ����4                   */
    , vt_op_ind   /* ���5                   */
    } type;

    /* ָ��������ָ��,                   */
    void (*op_func)(command_parm* parm);
};


#endif // CPU_H_INCLUDED
