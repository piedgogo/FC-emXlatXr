#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include "type.h"

extern cpu_6502 cpu;

/** ����cpu״̬ */
void        reset_cpu();
/** ���ջ��ѹ�� */
void        push     (byte d);
/** �Ӷ�ջ��ȡ�� */
byte        pop      ();

#endif // CPU_H_INCLUDED
