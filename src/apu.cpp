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
#include "apu.h"


static word noise_freq [] = {
    0x002,0x004,0x008,0x010,
    0x020,0x030,0x040,0x050,
    0x065,0x07F,0x0BE,0x0FE,
    0x17D,0x1FC,0x3F9,0x7F2
    };

static float main_freq = 1.79 * 1000 * 1000;


void setr0(Register &r, byte v) {
    r.volume    = (0x0F & v);
    r.type      = (0x10 & v);
    r.looping   = (0x20 & v);
    r.duty_type = (0xC0 & v) >> 6;
}


void setr0tri(Register &r, byte v) {
    r.use_linear = (0x80 & v);
    r.linear     = (0x7F & v);
}


void setr1(Register &r, byte v) {
    r.sw_rs   = (0x07 & v);
    r.sw_dir  = (0x08 & v);
    r.sw_rete = (0x70 & v) >> 4;
    r.sw_en   = (0x80 & v);
}


void setr2n(Register &r, byte v) {
    r.nfreq = noise_freq[0x0F & v];
    r.rand_type = 0x80 & v;
}


void setr2(Register &r, byte v) {
    r.wavelength_l = v;
}


void setr3(Register &r, byte v) {
    r.wavelength_h   = (0x07 & v);
    r.length_counter = (0xF8 & v) >> 3;
}


byte Apu::read() {
    byte ret = 0;
    if (*irq) {
        ret |= 0x40;
    }
    return ret;
}


void Apu::write(const word offset, const byte data) {
    switch (offset) {
        
    case 0x4000:
        setr0(r1, data); break;
    case 0x4004:
        setr0(r2, data); break;
    case 0x4008:
        setr0tri(tr, data); break;
    case 0x400C:
        setr0(ns, data); break;
    
    case 0x4001:
        setr1(r1, data); break;
    case 0x4005:
        setr1(r2, data); break;
    
    case 0x4002:
        setr2(r1, data); break;
    case 0x4006:
        setr2(r2, data); break;
    case 0x400A:
        setr2(tr, data); break;
    case 0x400E:
        setr2n(ns, data); break;
    
    case 0x4003:
        setr3(r1, data); break;
    case 0x4007:
        setr3(r2, data); break;
    case 0x400B:
        setr3(tr, data); break;
    case 0x400F:
        setr3(ns, data); break;
        
    case 0x4017:
        if (data & 0x40) {
            *irq = 0;
        } else {
            *irq = 1;
        }
        break;
    }
}


void Apu::setIrq(byte* i) {
    irq = i;
}