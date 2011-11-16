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
#ifndef VIDEO_H_INCLUDED
#define VIDEO_H_INCLUDED

#include "type.h"

/* �������صĽӿ�, ÿ��ϵͳʵ�ֵĲ�ͬ, �߿�Ҫ��8�����������Ƴ���ȥ�Ĳ��� */
class Video {
public:
    /* ����һ������ */
    virtual void drawPixel(int x, int y, T_COLOR color) = 0;
    /* ˢ�»��浽ҳ�� */
    virtual void refresh() {}
    /* �����������ʹ��ָ������ɫ  */
    virtual void clear(T_COLOR color) = 0;
    /* ׼������򷵻�0, ���򷵻ش����� */
    virtual int prepareSuccess() { return 0; }
    /* �޸Ĵ��ڴ�С��, ���ø÷���, ������� */
    virtual void resize(int w, int h) {}

    virtual ~Video() {};
};

#endif // VIDEO_H_INCLUDED
