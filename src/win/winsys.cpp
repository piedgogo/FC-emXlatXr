#include "winsys.h"

void initHdcColor(HDC hdc) {
    HFONT font = CreateFont(10, 0, 0, 0, 100
                , FALSE, FALSE, FALSE, GB2312_CHARSET
                , OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
                , PROOF_QUALITY, DEFAULT_PITCH, "����" );

    SelectObject(hdc, font);
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkColor(hdc, RGB(0, 0, 0));
}

int createWindow(win_info* wi) {
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance     = wi->hInstance;
    wincl.lpszClassName = wi->szClassName;
    wincl.lpfnWndProc   = wi->procedure;      /* This function is called by windows */
    wincl.style         = CS_DBLCLKS;         /* Catch double-clicks */
    wincl.cbSize        = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon   = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                /* No menu */
    wincl.cbClsExtra   = 0;                   /* No extra bytes after the window class */
    wincl.cbWndExtra   = 0;                   /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    wi->hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               wi->szClassName,     /* Classname */
               wi->titleName,       /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
               wi->x,               /* Windows decides the position */
               wi->y,               /* where the window ends up on the screen */
               wi->width,           /* The programs width */
               wi->height,          /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               wi->menu,            /* menu */
               wi->hInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
               );

    /* Make the window visible on the screen */
    ShowWindow(wi->hwnd, wi->nCmdShow);
    return 1;
}

/****|WindowsVideo|************************************************************/

WindowsVideo::WindowsVideo(HWND hwnd) {
    WindowsVideo(hwnd, PPU_DISPLAY_P_WIDTH, PPU_DISPLAY_P_HEIGHT);
}

WindowsVideo::WindowsVideo(HWND hwnd, int w, int h)
            : x_off(0), y_off(0), width(w), height(h)
{
    m_hwnd = hwnd;
    hdc = GetDC(hwnd);
    hMemDC = CreateCompatibleDC(hdc);

    hBitmap = CreateCompatibleBitmap(hdc, width, height);

    SelectObject(hMemDC, hBitmap);
    initHdcColor(hdc);
}

void WindowsVideo::setOffset(int x, int y) {
    x_off = x;
    y_off = y;
}

void WindowsVideo::drawPixel(int x, int y, T_COLOR color) {
    SetPixel(hMemDC, x, y, color);
}

void WindowsVideo::refresh() {
    BitBlt(hdc, x_off, y_off, width, height, hMemDC, 0, 0, SRCCOPY);
}

void WindowsVideo::clear(T_COLOR c) {
    HBRUSH hBrush = CreateSolidBrush(c);
    RECT r = {0, 0, width, height};
    FillRect(hMemDC, &r, hBrush);
    DeleteObject(hBrush);
}

WindowsVideo::~WindowsVideo() {
    DeleteDC(hMemDC);
    ReleaseDC(m_hwnd, hdc);
}

/****|DirectXVideo|************************************************************/

DirectXVideo::DirectXVideo(HWND hwnd, int width, int height)
    :lpDD4(0), lpDDSPrimary(0), success(0), pixel(0), m_hwnd(hwnd)
{
    LPDIRECTDRAW lpDD;
    pixel    = new T_COLOR[width * height];
    m_height = height;
    m_width  = width;

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
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    // ������ҳ�����
    if ( lpDD4->CreateSurface( &ddsd, &lpDDSPrimary, NULL ) ) {
        printf("DX::Create main surface fail.\n");
        return;
    }

    success = 1;
}

int DirectXVideo::isSuccess() {
    return success;
}

void DirectXVideo::drawPixel(int x, int y, T_COLOR color) {
    pixel[x + (y*m_width)] = color;
}

void DirectXVideo::clear(T_COLOR color) {
    memset(pixel, color, m_width * m_height * sizeof(int));
}

void DirectXVideo::refresh() {
    RECT lpDestRect;

    /* ת������� */
    point.x = point.y = 0;
    ClientToScreen(m_hwnd, &point);

    lpDestRect.top    = point.y;
    lpDestRect.left   = point.x;
    lpDestRect.bottom = point.y + m_height;
    lpDestRect.right  = point.x + m_width;

    if (lpDDSPrimary->Lock(&lpDestRect, &ddsd,
             DDLOCK_SURFACEMEMORYPTR, NULL) != DD_OK) {
        return;
    }

    UINT *buffer = (UINT*)ddsd.lpSurface;
    UINT nPitch = ddsd.lPitch>>2;

    for (int x=0, y=0;;) {
        buffer[x + y*nPitch] = pixel[x + y*m_width];
        if (++x >= m_width) {
            x = 0;
            if (++y >= m_height) break;
        }
    }

    lpDDSPrimary->Unlock(&lpDestRect);
}

DirectXVideo::~DirectXVideo() {
    if( lpDDSPrimary != NULL ) {
        lpDDSPrimary->Restore();
        lpDDSPrimary->Release();
        lpDDSPrimary = NULL;
    }
    if( lpDD4 != NULL ) {
        lpDD4->Release();
        lpDD4 = NULL;
    }
    delete [] pixel;
    pixel = NULL;
}

/****|WinPad|******************************************************************/

WinPad::WinPad() {
    p1_key_map[ FC_PAD_BU_A      ] = K_J;
    p1_key_map[ FC_PAD_BU_B      ] = K_K;
    p1_key_map[ FC_PAD_BU_START  ] = K_H;
    p1_key_map[ FC_PAD_BU_SELECT ] = K_F;
    p1_key_map[ FC_PAD_BU_UP     ] = K_W;
    p1_key_map[ FC_PAD_BU_DOWN   ] = K_S;
    p1_key_map[ FC_PAD_BU_LEFT   ] = K_A;
    p1_key_map[ FC_PAD_BU_RIGHT  ] = K_D;
}

byte WinPad::keyPushed(FC_PAD_KEY key, byte id) {
    return (GetAsyncKeyState( p1_key_map[key] ) & 0x8000) ? 1 : 0;
}
