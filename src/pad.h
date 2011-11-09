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
#ifndef PAD_H_INCLUDED
#define PAD_H_INCLUDED

#include <memory.h>

/* �ֱ����� */
enum FC_PAD_KEY {
    FC_PAD_BU_A       = 0,
    FC_PAD_BU_B       = 1,
    FC_PAD_BU_SELECT  = 2,
    FC_PAD_BU_START   = 3,
    FC_PAD_BU_UP      = 4,
    FC_PAD_BU_DOWN    = 5,
    FC_PAD_BU_LEFT    = 6,
    FC_PAD_BU_RIGHT   = 7
};

/**
 * ��Ϸ�ֱ�,1P&2P
 */
class PlayPad {

private:
    byte wcount;
    byte rcount;
    byte keySave[16];

public:
    static const int PLAYER_1 = 0;
    static const int PLAYER_2 = 1;

    PlayPad() : wcount(0), rcount(0) {
        memset(keySave, 0, sizeof(keySave));
    }

    virtual ~PlayPad() {};

    /* ����ü������򷵻�1, idΪ���������� 0==1P */
    virtual byte keyPushed(FC_PAD_KEY key, byte id) = 0;

    /* ��4016/4017�˿�д���� */
    void writePort(word port, byte data) {
        if (port==0x4016) {
            /* ����1,�ٷ���0��ʼ��ȡ��λ */
            if (wcount==0 && data==1) wcount++;
            if (wcount==1 && data==0) {
                rcount = 0;
                wcount = 0;
            }
        }
    }

    /* ��4016/4017�˿ڶ����� */
    byte readPort(word port) {
        if (port==0x4016 && rcount<8) {
            if (keySave[rcount]) {
                keySave[rcount] = 0;
                ++rcount;
                return 1;
            }
            return keyPushed(FC_PAD_KEY(rcount++), PLAYER_1);
        }
        return 0;
    }

    /* ʹ�ñ�̵ķ�������һ������,����ʱʹ�� */
    void pushKey(FC_PAD_KEY key, byte padid) {
        if (padid==PLAYER_1) {
            keySave[key] = 1;
        } else {
            keySave[key + 8] = 1;
        }
    }
};

#endif // PAD_H_INCLUDED
