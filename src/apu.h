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
#ifndef aPU_H_INCLUDED
#define aPU_H_INCLUDED
#include "win/sound.h"
#include "type.h"


struct Register {
    //--- Register 0 = Rectangle/Noise
    // 4bit fix volume
    byte volume;
    // 1bit, 0=Envelope/Decay, 1=Fixed Volume
    byte type;
    // 1bit,
    // 0: Disable Looping, stay at 0 on end of decay [ \_____ ]
    // 1: Enable Looping, restart decay at F         [ \\\\\\ ]
    byte looping;
    // 2bit
    byte duty_type;

    //--- Register 0 = Triangle
    // 6bit linear counter load register
    byte linear;
    // length counter clock disable / linear counter start
    byte use_linear;

    //--- Register 1 = Rectangle
    // 3bit, Sweep right shift amount (S=0..7)
    byte sw_rs;
    // 1bit, Sweep Direction (0=[+]Increase, 1=[-]Decrease)
    byte sw_dir;
    // 3bit, Sweep update rate (N=0..7), NTSC=120Hz/(N+1), PAL=96Hz/(N+1)
    byte sw_rete;
    // 1bit, Sweep enable (0=Disable, 1=Enable)
    byte sw_en;
    
    //--- Register 2 = Rectangle/Triangle
    byte wavelength_l;
    // F = 1.79MHz/(N+1)/16 for Rectangle channels
    // F = 1.79MHz/(N+1)/32 for Triangle channel
    float freq;
    
    //--- Register 2 = Noise
    word nfreq;
    byte rand_type;
    
    //--- Register 3 = ALL
    // 3bit, Upper 3 bits of wavelength (unused on noise channel)
    byte wavelength_h;
    // 5bit, Length counter load register 
    byte length_counter;
};


class Apu {
private:
    word frame_counter;
    byte* irq;
    byte tmp;

    Register r1, r2, ns, tr;
    
public:
    void write(const word offset, const byte data);
    // only 0x4015 can read
    byte read();
    void setIrq(byte* i);

    Apu() : irq(&tmp) {}
};


#endif // aPU_H_INCLUDED
