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
#ifndef D3DVIDEO_H_INCLUDED
#define D3DVIDEO_H_INCLUDED

/* ---------------------------- fix d3dx9.h ---- */
#ifndef DECLSPEC_DEPRECATED
#if (_MSC_VER >= 1300) && !defined(MIDL_PASS)
#define DECLSPEC_DEPRECATED __declspec(deprecated)
#define DEPRECATE_SUPPORTED
#else
#define DECLSPEC_DEPRECATED
#undef DEPRECATE_SUPPORTED
#endif
#endif
/* --------------------------------------------- */

#include "../video.h"
#include <windows.h>
#include <d3d9.h>

class DirectX3DVideo : public Video {

struct PANELVERTEX {
    FLOAT x, y, z;
    DWORD color;
    FLOAT u, v;
};

private:
    static const DWORD D3DFVF_PANELVERTEX
        = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);

    LPDIRECT3D9             pD3D;
    LPDIRECT3DDEVICE9       pd3dDevice;
    LPDIRECT3DVERTEXBUFFER9 pVertices;
    LPDIRECT3DTEXTURE9      pTexture;
    T_COLOR*                pixel;
    UINT                    width, height;

    void _initVertices(int w, int h);
    void _createTexture(int w, int h);
    void _initMatrix(int w, int h);

public:
    DirectX3DVideo(HWND hwnd, int w, int h);
    ~DirectX3DVideo();

    void drawPixel(int x, int y, T_COLOR color);
    void refresh();
    void clear(T_COLOR color);
    int  prepareSuccess();
    void resize(int w, int h);
};

#endif // D3DVIDEO_H_INCLUDED
