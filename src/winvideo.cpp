#include "video.h"
#include "ppu.h"
#include "ddraw.h"
#include <windows.h>
#include <stdio.h>

static void initHdcColor(HDC hdc) {
    HFONT font = CreateFont(10, 0, 0, 0, 100
                , FALSE, FALSE, FALSE, GB2312_CHARSET
                , OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
                , PROOF_QUALITY, DEFAULT_PITCH, "����" );

    SelectObject(hdc, font);
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkColor(hdc, RGB(0, 0, 0));
}

/* ʹ��GUI��ͼ */
class WindowsVideo : public Video {

private:
    HWND    m_hwnd;
    HDC     hdc;
    HDC     hMemDC;
    HBITMAP hBitmap;

public:
    WindowsVideo(HWND hwnd) {
        m_hwnd = hwnd;
        hdc = GetDC(hwnd);
        hMemDC = CreateCompatibleDC(hdc);

        hBitmap = CreateCompatibleBitmap(
                hdc, PPU_DISPLAY_P_WIDTH, PPU_DISPLAY_P_HEIGHT);

        SelectObject(hMemDC, hBitmap);
        initHdcColor(hdc);
    }

    void drawPixel(int x, int y, T_COLOR color) {
        SetPixel(hMemDC, x, y, color);
    }

    void refresh() {
        BitBlt(hdc, 0, 0, PPU_DISPLAY_P_WIDTH,
               PPU_DISPLAY_P_HEIGHT, hMemDC, 0, 0, SRCCOPY);
    }

    ~WindowsVideo() {
        DeleteDC(hMemDC);
        ReleaseDC(m_hwnd, hdc);
    }
};

/* ʹ��DirectX��ͼ */
class DirectXVideo : public Video {

private:
    LPDIRECTDRAW4         lpDD4;                /* DirectDraw���� */
    LPDIRECTDRAWSURFACE4  lpDDSPrimary;         /* DirectDraw��ҳ�� */
    LPDIRECTDRAWSURFACE4  lpDDSBack;
    HDC                   hdc;
    int                   success;

public:
    DirectXVideo(HWND hwnd)
    :lpDD4(0), lpDDSPrimary(0), lpDDSBack(0), hdc(0), success(0)
    {
        LPDIRECTDRAW lpDD;

        //����DirectCraw����
        if ( DirectDrawCreate( NULL, &lpDD, NULL ) ) {
            printf("DX::Create DirectDraw fail.\n");
            return;
        }

        if ( lpDD->QueryInterface(IID_IDirectDraw4, (LPVOID *)&lpDD4) ) {
            printf("DX::Create DirectDraw4 fail.\n");
            lpDD->Release();
            return;
        }

        lpDD->Release();

        // ȡ�ö�ռ��ȫ��ģʽ: DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN
        if ( lpDD4->SetCooperativeLevel( hwnd, DDSCL_NORMAL ) ) {
            printf("DX::change Cooperative level fail.\n");
            return;
        }

        // ������ʾģʽ, ȫ����Ч
        //if ( lpDD->SetDisplayMode( 640, 480, 8 ) != DD_OK) return FALSE;

        // �����ҳ����Ϣ
        DDSURFACEDESC2 ddsd;
        ZeroMemory(&ddsd, sizeof(ddsd));
        ddsd.dwSize         = sizeof(ddsd);
        ddsd.dwFlags        = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

        // ������ҳ�����
        if ( lpDD4->CreateSurface( &ddsd, &lpDDSPrimary, NULL ) ) {
            printf("DX::Create main surface fail.\n");
            return;
        }

        // ���ü���
        LPDIRECTDRAWCLIPPER lpddClipper;
        lpDD4->CreateClipper(0, &lpddClipper, NULL);
        lpddClipper->SetHWnd(0, hwnd);
        lpDDSPrimary->SetClipper(lpddClipper);

        // ��������ҳ��
        ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
        ddsd.dwWidth        = PPU_DISPLAY_P_WIDTH;
        ddsd.dwHeight       = PPU_DISPLAY_P_HEIGHT;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

        if ( lpDD4->CreateSurface( &ddsd, &lpDDSBack, NULL ) ) {
            printf("DX::Create back surface fail.\n");
            return;
        }

        if ( lpDDSPrimary->GetDC(&hdc) ) {
            printf("DX::crate DC fail.\n");
            return;
        }

        initHdcColor(hdc);
        success = 1;
    }

    int isSuccess() {
        return success;
    }

    void drawPixel(int x, int y, T_COLOR color) {
        SetPixel(hdc, x, y, color);
    }

    void refresh() {
        RECT rest;
        rest.top = 0;
        rest.left = 0;
        rest.bottom = 240;
        rest.right = 256;
        lpDDSPrimary->Blt(&rest, lpDDSBack, &rest, DDBLT_COLORFILL, NULL);
    }

    ~DirectXVideo() {
        if (hdc!=NULL) {
            lpDDSBack->ReleaseDC(hdc);
            hdc  = NULL;
        }
        if (lpDDSBack!=NULL) {
            lpDDSBack->Release();
            lpDDSBack = NULL;
        }
        if( lpDDSPrimary != NULL ) {
            lpDDSPrimary->SetClipper(NULL);
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        }
        if( lpDD4 != NULL ) {
            lpDD4->Release();
            lpDD4 = NULL;
        }
    }
};

// GetAsyncKeyState(virtual key code)����=0x8000
