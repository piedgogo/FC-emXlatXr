#ifndef VIDEO_H_INCLUDED
#define VIDEO_H_INCLUDED

#include "type.h"

/* �������صĽӿ�, ÿ��ϵͳʵ�ֵĲ�ͬ */
class Video {
public:
    /* ����һ������ */
    virtual void drawPixel(int x, int y, T_COLOR color) = 0;
    /* ˢ�»��浽ҳ�� */
    virtual void refresh() {}
};

#endif // VIDEO_H_INCLUDED
