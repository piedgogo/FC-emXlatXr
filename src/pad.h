#ifndef PAD_H_INCLUDED
#define PAD_H_INCLUDED

/* �ֱ����� */
enum FC_PAD_KEY {
    FC_PAD_BU_A       = 0,
    FC_PAD_BU_B       = 1,
    FC_PAD_BU_SELECT  = 2,
    FC_PAD_BU_START   = 3,
    FC_PAD_BU_DOWN    = 4,
    FC_PAD_BU_UP      = 5,
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

public:
    PlayPad() : wcount(0), rcount(0)
    {
    }

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
        if (port==0x4016) {
            return keyPushed(FC_PAD_KEY(rcount++), 0);
        }
        return 0;
    }
};

#endif // PAD_H_INCLUDED
