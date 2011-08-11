#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include "type.h"
#include "mem.h"

struct command_6502;
struct command_parm;

struct cpu_6502 {

    /* NTSC��ʽ��������Ƶ��Ϊ1.7897725 MHz
     * PAL��ʽ��������Ƶ��Ϊ1.773447 MHz                   */
#define CPU_NTSC            1.7897725
#define CPU_PAL             1.773447

#define CPU_INTERRUPT_CYC   8 /* �ж������ִ������        */
#define CPU_RESET_CYC       6 /* ��λ����ִ������          */
#define CPU_NMI_CYC        17 /* ���������ж�ִ������      */

    byte A; 	    /* �ۼ���                              */
    byte Y;	    	/* �����ݴ���                          */
    byte X;	    	/* �����ݴ���                          */

    byte SP;		/* �ѵ�ָʾ�� 0x0100-0x01FF
                     * �ݼ���, ָ��'��', ÿ��1�ֽ�         */
    union {
        struct { byte PCL; byte PCH; };
        word PC;
    };  			/* ���������,ָ����һ��Ҫִ�е�ָ��   */

#define CPU_FLAGS_NEGATIVE    (1<<7)  /* ��ֵ              */
#define CPU_FLAGS_OVERFLOW    (1<<6)  /* �����            */
#define CPU_FLAGS_CONST       (1<<5)  /* ʼ��Ϊ1           */
#define CPU_FLAGS_BREAK       (1<<4)  /* ���ж�            */
#define CPU_FLAGS_DECIMAL     (1<<3)  /* ʮ����            */
#define CPU_FLAGS_INTERDICT   (1<<2)  /* ��ֹ�ж�          */
#define CPU_FLAGS_ZERO        (1<<1)  /* ���Ϊ��          */
#define CPU_FLAGS_CARRY        1      /* �н�λ            */

    byte FLAGS;	    /* ״̬�ݴ���  N V 1 B D I Z C         *
					 * N��ֵ 	V���		B�ж�����      *
					 * Dʮ����	I��Ͻ���	Z��ֵ          *
					 * C��λ                               */
    byte NMI;       /* ���>0��ִ�в�������֮�ж�          */
    byte IRQ;       /* ���>0��ִ�п�����֮�ж�            */
    byte RES;       /* ���>0��ִ�и�λ����                */

    memory *ram;    /* �ڴ�                                */
    command_parm* prev_parm;    /* ǰһ������Ĳ���        */

    cpu_6502(memory* ram);

    byte    reset();            /* ����cpu״̬,����ʹ������*/
    void    push(byte d);       /* ���ջ��ѹ��            */
    byte    pop();              /* �Ӷ�ջ��ȡ��            */
    char*   debug();            /* ����cpu״̬�ַ���       */
    byte    process();          /* ����ǰ���ָ����һ����
                                 * ��,����ʹ�õĴ��������� */
    byte    irq();              /* ������ִ���ж�,����ʹ�õ�
                                 * ��������                */
    void    rti();              /* ���жϹ����з���        */
    byte    nmi();              /* ���������ε��ж�      */
    void    jump(word addr);    /* ��ת��addr�ڴ�ָ��ĵ�ַ(addr, addr+1)
                                 * ������֮ǰ��PC��FLAGS   */
    char*   cmdInfo();          /* ������һ��ָ�������    */

    /* ���value���λΪ1 ��N=1,����Ϊ0,
     * ���value==0 ��Z=1,����Ϊ0 */
    inline void checkNZ(byte value) {
        if (value & 0x80) {
            FLAGS |= CPU_FLAGS_NEGATIVE;
        } else {
            FLAGS &= 0xFF ^ CPU_FLAGS_NEGATIVE;
        }

        checkZ(value);
    }

    inline void checkZ(byte value) {
        if (value) {
            FLAGS &= 0xFF ^ CPU_FLAGS_ZERO;
        } else {
            FLAGS |= CPU_FLAGS_ZERO;
        }
    }

    /* ���value��beforeOper�����λ��ͬ,��v=1����Ϊ0
     * beforeOper�ǲ�������֮ǰ���ۼ�����ֵ */
    inline void checkV(word value, byte beforeOper) {
        if (value & beforeOper & 0x80) {
            FLAGS |= CPU_FLAGS_OVERFLOW;
        } else {
            FLAGS &= 0xFF ^ CPU_FLAGS_OVERFLOW;
        }
    }

    inline void clearV() {
        FLAGS &= 0xFF ^ CPU_FLAGS_OVERFLOW;
    }
};

/* ������������ݲ���          */
struct command_parm {

    memory   *ram;
    cpu_6502 *cpu;

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
    inline byte read(const byte addressing_mode) {
        if (addressing_mode==ADD_MODE_imm) {
            return p1;
        } else
        if (addressing_mode==ADD_MODE_acc) {
            return cpu->A;
        }
        return ram->read( getAddr(addressing_mode) );
    }

    /*  �����ڴ�Ѱַ��ʽ��valueд�뵽�õ�ַ            */
    inline void write(const byte addressing_mode, byte value) {
        if (addressing_mode==ADD_MODE_imm) {
            printf("!��������������д����");
            return;
        } else
        if (addressing_mode==ADD_MODE_acc) {
            cpu->A = value;
            return;
        }
        cpu->ram->write(getAddr(addressing_mode), value);
    }

    inline word getAddr(const byte addressing_mode) {
        switch (addressing_mode) {

        case ADD_MODE_abs:
            return abs();

        case ADD_MODE_zpg:
            return zpg();

        case ADD_MODE_absX:
            return absX();

        case ADD_MODE_absY:
            return absY();

        case ADD_MODE_zpgX:
            return zpgX();

        case ADD_MODE_$zpg$Y:
            return $zpg$Y();

        case ADD_MODE_$zpgX$:
            return $zpgX$();
        }

        printf("!��Ч��Ѱַ����: %d\n", addressing_mode);
        return 0;
    }

/* ---------------------------- Ѱַ�㷨����, �������ص�ַ ---------- */
    inline word abs() {
        return (p2<<8) | p1;
    }

    inline word absX() {
        return abs() + cpu->X;
    }

    inline word absY() {
        return abs() + cpu->Y;
    }

    inline word zpg() {
        return p1 & 0x00FF;
    }

    inline word zpgX() {
        return (p1 + cpu->X) & 0x00FF;
    }

    inline word zpgY() {
        return (p1 + cpu->Y) & 0x00FF;
    }

    inline word $zpg$Y() {
        return $$$(0, cpu->Y);
    }

    inline word $zpgX$() {
        return $$$(cpu->X, 0);
    }

    inline word $$$(byte x, byte y) {
        word offset = 0;
        byte l = 0;
        byte h = 0;

        offset = ram->read( (p1 + x) & 0x00FF );
        l = ram->read( offset   );
        h = ram->read( offset+1 ) + y;
        return h<<8 | l;
    }
};

struct command_6502 {

    char    name[4];
    byte    time;
    byte    len;

    enum value_type {
      vt_op_not = 0
    , vt_op_imm   /* ����1              */
    , vt_op_zpg   /* ��ҳ2 , ��λΪ0x00 */
    , vt_op_rel   /* ���3              */
    , vt_op_abs   /* ����4              */
    , vt_op_ind   /* ���5              */
    } type;

    /* ָ��������ָ��, cmd ����������*/
    void (*op_func)(command_6502* cmd, command_parm* parm);
};


#endif // CPU_H_INCLUDED
