#ifndef VIDEO_H_INCLUDED
#define VIDEO_H_INCLUDED

#include "type.h"

/* �������صĽӿ�, ÿ��ϵͳʵ�ֵĲ�ͬ */
class Video {
public:
    virtual void drawPixel(int x, int y, T_COLOR color) = 0;
};

#endif // VIDEO_H_INCLUDED
