#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

// #define _WIN32_WINNT 0x0500

#include <windows.h>
#include <Wincon.h>
#include <amaudio.h>
#include <math.h>
#include <iostream>

typedef double Frequency;
typedef byte NoteType;

class DXSound;
class DXChannel;
class CreateSample;


struct BufferDesc {
    DWORD SamplesPerSec;
    WORD  BitsPerSample;
    WORD  Channels;
    DWORD BufferBytes;
};
extern BufferDesc DEFAULT_BUFF_DESC;


class CreateSample {
public:
    LPVOID        lpvWrite;
    DWORD         dwLength;   // ���鳤��
    WAVEFORMATEX  desc;
    double        maxval;     // �������

    CreateSample(BufferDesc &bd);
    CreateSample(DXChannel &ds);

    ~CreateSample() {
        dwLength = 0;
        lpvWrite = NULL;
    }

    NoteType& operator[](int i) {
        return *( ((NoteType*)(lpvWrite))+i );
    }
};


class DXSound {
    LPDIRECTSOUND8 sound;
public:
    DXSound();
    ~DXSound();

friend DXChannel;
};


class DXChannel {
private:
    LPDIRECTSOUNDBUFFER buffer;
    DSBUFFERDESC        bufdesc;
    WAVEFORMATEX        wfx;
    DXSound             &dxs;
    BufferDesc          bdesc;

    void init_desc(BufferDesc&);
    void free();
    void lockBuffer(CreateSample &cs);
    void freeBuffer(CreateSample &cs);
    void play(DWORD f = DSBPLAY_LOOPING);

public:

    DXChannel(DXSound &ds, BufferDesc desc = DEFAULT_BUFF_DESC);
    ~DXChannel();

    void stop();

    template<class SType>
    void play(SType &filter, DWORD flag = DSBPLAY_LOOPING) {
        stop();
        CreateSample cs(*this);
        lockBuffer(cs);
        filter(cs);
        freeBuffer(cs);
        play(flag);
    }

    const WAVEFORMATEX& getWfx() { return wfx; }
    const BufferDesc& getBufDsc() { return bdesc; }
};


// ����������
void noise(CreateSample &cs);

// ���Ҳ�
class osc {
    Frequency hz;
public:
    osc(Frequency h) : hz(h) {}
    void operator()(CreateSample &cs);
};

// ����
class square {
    Frequency hz;
public:
    square(Frequency z) : hz(z) {}
    void operator()(CreateSample &cs);
};

// ���ǲ�
class triangle {
    Frequency hz;
public:
    triangle(Frequency z) : hz(z) {}
    void operator()(CreateSample &cs);
};

// �����𽥼�С
template<class T>
class fall_off {
    typedef T parent;
    T &filter;
public:
    fall_off(T &f) : filter(f) {}

    void operator()(CreateSample &cs) {
        filter(cs);
        double half = cs.maxval / 2;

        for (int i=0; i<cs.dwLength; ++i) {
            cs[i] = NoteType((double(cs[i]) - half) * (double(cs.dwLength-i)/cs.dwLength) + half);
        }
    }
};

// ��������
template<class T>
class wave_filter {
    typedef T parent;
    T &filter;
    double amplitude;   // ��� 0~1, 1=ȫ���
    double speed;       // ���ٶ�

public:
    wave_filter(T &f, double amp = 0.5, double s = 0.001)
        : filter(f), amplitude(amp), speed(s) {}

    void operator()(CreateSample &cs) {
        filter(cs);
        double a = 0;
        double half = cs.maxval / 2;

        for (int i=0; i<cs.dwLength; ++i) {
            cs[i] = NoteType( (double(cs[i])-half) * ((sin(a)+1)/2 * amplitude + (1-amplitude)) + half );
            a += speed;
        }
    }
};

// ��ͷ
template<class T>
class sound_head {
    typedef T parent;
    T &filter;
public:
    sound_head(T &f) : filter(f) {}

    void operator()(CreateSample &cs) {
        filter(cs);
        double half = cs.maxval / 2;

        for (int i=0; i<cs.dwLength; ++i) {
            cs[i] = (double(cs[i]) - half) * sqrt( double(cs.dwLength-i)/cs.dwLength ) + half;
           // std::cout << i << " " << cs[i] << " " << sqrt( double(cs.dwLength-i)/cs.dwLength ) << "\n";
        }
    }
};

// ����˥��
template<class T>
class weaken_filter {
    typedef T parent;
    T &filter;
    double weak;
public:
    weaken_filter(T &f, double w) : filter(f), weak(w) {}

    void operator()(CreateSample &cs) {
        filter(cs);

        for (int i=0; i<cs.dwLength; ++i) {
            cs[i] *= weak;
        }
    }
};

// ����������ͬʱ����
class ChannelPool {
    DXSound &base;
    DXChannel **chs;
    int size;
    int chs_idx;

public:
    ChannelPool(DXSound &b, int s=16);
    ~ChannelPool();
    void stop();

    template<class SType>
    void play(SType &st, DWORD flag = DSBPLAY_LOOPING) {
        CreateSample cs(*chs[chs_idx]);
        chs[chs_idx]->play(st, flag);

        if (++chs_idx >= size)
            chs_idx = 0;
    }

    template<class SType>
    void render(SType &st, DWORD flag = DSBPLAY_LOOPING) {
        play(st, flag);
    }

    template<class SType>
    void mix(SType &st) {
        play(st, 0);
    }
};

// Ԥ����Ⱦһ������������
class SoundPreRender {
    BufferDesc &desc;
    NoteType *data;
public:
    SoundPreRender(BufferDesc &b) : desc(b) {
        data = new NoteType[desc.BufferBytes / sizeof(NoteType)];
        memset(data, 0, desc.BufferBytes);
    }

    ~SoundPreRender() {
        delete [] data;
    }

    template<class SType>
    void render(SType &st, DWORD notuse) {
        CreateSample cs(desc);
        cs.lpvWrite = data;
        cs.dwLength = desc.BufferBytes / sizeof(NoteType);
        st(cs);
    }

    template<class SType>
    void mix(SType &st) {
        NoteType *src = new NoteType[desc.BufferBytes / sizeof(NoteType)];
        CreateSample cs(desc);
        cs.lpvWrite = src;
        cs.dwLength = desc.BufferBytes / sizeof(NoteType);
        st(cs);

        for (int i=0; i<cs.dwLength; ++i) {
            // http://blog.csdn.net/chinabinlang/article/details/8086443
            // Y = A + B - A * B / 255
            data[i] = data[i] + src[i] - double(data[i]) * src[i] / cs.maxval;
        }

        delete[] src;
    }

    void operator()(CreateSample &cs) {
        memcpy(cs.lpvWrite, data, cs.dwLength * sizeof(NoteType));
    }
};

#endif // SOUND_H_INCLUDED
