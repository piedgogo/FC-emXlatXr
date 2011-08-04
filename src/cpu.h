#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include "type.h"
#include "mem.h"

struct cpu_6502 {

private:
/* NTSC��ʽ��������Ƶ��Ϊ1.7897725 MHz
 * PAL��ʽ��������Ƶ��Ϊ1.773447 MHz                       */
#define CPU_NTSC    1.7897725
#define CPU_PAL     1.773447

    byte A; 	    /* �ۼ���                              */
    byte Y;	    	/* �����ݴ���                          */
    byte X;	    	/* �����ݴ���                          */

    union {
        struct { byte PCH; byte PCL; };
        word PC;
    };  			/* ���������,ָ����һ��Ҫִ�е�ָ��   */

    word SP;		/* �ѵ�ָʾ�� 0x0100-0x01FF
                     * �ݼ���, ָ��'��', ÿ��2�ֽ�         */

#define CPU_FLAGS_NEGATIVE    (1<<7)  /* ��ֵ              */
#define CPU_FLAGS_OVERFLOW    (1<<6)  /* �����            */
#define CPU_FLAGS_BREAK       (1<<4)  /* ���ж�            */
#define CPU_FLAGS_DECIMAL     (1<<3)  /* ʮ����            */
#define CPU_FLAGS_INTERDICT   (1<<2)  /* ��ֹ�ж�          */
#define CPU_FLAGS_ZERO        (1<<1)  /* ���Ϊ��          */
#define CPU_FLAGS_CARRY       1       /* �н�λ            */

    byte FLAGS;	    /* ״̬�ݴ���  N V 1 B D I Z C
					 * N��ֵ 	V���		B�ж�����
					 * Dʮ����	I��Ͻ���	Z��ֵ
					 * C��λ                               */
    memory *ram;    /* �ڴ�                                */

public:
    cpu_6502(memory* ram);

    void    reset();            /* ����cpu״̬             */
    void    push(byte d);       /* ���ջ��ѹ��            */
    byte    pop();              /* �Ӷ�ջ��ȡ��            */
    void    debug();            /* ��ӡcpu״̬             */
    void    process();          /* ����ǰ���ָ����һ������ */
};

struct command_6502 {
    char name[5];
    byte time;
    byte len;
};

command_6502 command_list_6502[] = {
    {"BRK", 7, 1}
};

#endif // CPU_H_INCLUDED
