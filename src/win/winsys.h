/*----------------------------------------------------------------------------*|
|*                                                                            *|
|* FC ģ���� (Famicom��Nintendo��˾��1983��7��15�����ձ����۵�8λ��Ϸ��)      *|
|*                                                                            *|
|* $ C++���Եĵ�һ����Ŀ,���������ְ�                                         *|
|* $ è��д��, �����ñ����������ע������                                     *|
|* $ ���߶�ʹ�ñ�������ɵĺ�������κ�����                                   *|
|* $ �಻��Դ���Ĺ���ԭ������һ������,�����ش������벦��119 & 911           *|
|*                                                                            *|
|* > ʹ�� [Code::Block 10.05] ��������                                        *|
|* > ������ʹ�� [MinGW 3.81] [gcc 4.4.1]                                      *|
|* > �ο������� [http://nesdev.parodius.com] ��վ������                       *|
|* > ��л [Flubba] ��ƵĲ��Գ���, ����������Ч�ʳ�ָ������                   *|
|*                                                                            *|
|* ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ | CatfoOD |^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ *|
|*                                           | yanming-sohu@sohu.com          *|
|* [ TXT CHARSET WINDOWS-936 / GBK ]         | https://github.com/yanmingsohu *|
|*                                           | qQ:412475540                   *|
|*----------------------------------------------------------------------------*/
#ifndef WINSYS_H_INCLUDED
#define WINSYS_H_INCLUDED

#include    <windows.h>
#include      <stdio.h>
#include "../nes_sys.h"
#include      "ddraw.h"

struct win_info;

int WINAPI  WinMain       (HINSTANCE, HINSTANCE, LPSTR, int);
win_info*   bg_panel      (HINSTANCE, PPU*);
win_info*   tile_panel    (HINSTANCE, PPU*);
void        initHdcColor  (HDC hdc);
int         createWindow  (win_info*);

struct win_info {

    HWND      hwnd;
    HINSTANCE hInstance;
    int       nCmdShow;
    int       x;
    int       y;
    int       width;
    int       height;
    char*     szClassName;
    char*     titleName;
    HMENU     menu;
    WNDPROC   procedure;

    win_info() : nCmdShow(SW_SHOW), x(CW_USEDEFAULT), y(CW_USEDEFAULT)
               , width(500), height(350)
               , menu(NULL), procedure(NULL)
    {/* do nothing.. */}

    void show() {
        ShowWindow(hwnd, SW_RESTORE);
    }
};

/* ʹ��GUI��ͼ */
class WindowsVideo : public Video {

private:
    HWND    m_hwnd;
    HDC     hdc;
    HDC     hMemDC;
    HBITMAP hBitmap;
    int     x_off;
    int     y_off;
    int     width;
    int     height;

public:
    WindowsVideo(HWND hwnd);
    WindowsVideo(HWND hwnd, int w, int h);
    ~WindowsVideo();

    void setOffset(int x, int y);
    void drawPixel(int x, int y, T_COLOR color);
    void refresh();
    void clear(T_COLOR color);
};

/* ʹ��DirectX��ͼ */
class DirectXVideo : public Video {

private:
    LPDIRECTDRAW4         lpDD4;                /* DirectDraw���� */
    LPDIRECTDRAWSURFACE4  lpDDSPrimary;         /* DirectDraw��ҳ�� */
    int                   success;
    T_COLOR              *pixel;
    HWND                  m_hwnd;
    DDSURFACEDESC2        ddsd;
    POINT                 point;
    int                   m_width;
    int                   m_height;

public:
    DirectXVideo(HWND hwnd, int width = 256, int height = 256);
    ~DirectXVideo();

    int isSuccess();
    void drawPixel(int x, int y, T_COLOR color);
    void refresh();
    void clear(T_COLOR color);
};

/* 0 : 0x30, 9 : 0x39
 * A : 0x41, W : 0x57, S : 0x53, D : 0x44
 * F : 0x46, H : 0x48, J : 0x4A, K : 0x4B */
class WinPad : public PlayPad {

private:
    static const byte K_A = 0x41;
    static const byte K_W = 0x57;
    static const byte K_S = 0x53;
    static const byte K_D = 0x44;
    static const byte K_F = 0x46;
    static const byte K_H = 0x48;
    static const byte K_J = 0x4A;
    static const byte K_K = 0x48;

    byte p1_key_map[8];

public:
    WinPad();
    byte keyPushed(FC_PAD_KEY key, byte id);
};

#endif
