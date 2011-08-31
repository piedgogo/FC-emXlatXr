#include  <windows.h>
#include    <stdio.h>
#include     <time.h>
#include  <Winuser.h>
#include   "winsys.h"
#include "../debug.h"


static LRESULT     CALLBACK WindowProcedure   (HWND, UINT, WPARAM, LPARAM  );
static void        displayCpu                 (cpu_6502*, HWND             );
static void        start_game                 (HWND, PMSG, HINSTANCE       );
static HMENU       createMainMenu             ();

/*  Make the class name into a global variable  */
static char szClassName[ ] = "CodeBlocksWindowsApp";
static char titleName  [ ] = "FC ģ���� DEmo. -=CatfoOD=-";
static bool active = 1;
static win_info* bgpanel = NULL;

int WINAPI WinMain ( HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow )
{
    MSG messages;            /* Here messages to the application are saved */
    win_info wi;

    wi.procedure   = WindowProcedure;
    wi.hInstance   = hThisInstance;
    wi.szClassName = szClassName;
    wi.titleName   = titleName;
    wi.nCmdShow    = nCmdShow;
    wi.menu        = createMainMenu();

    if (!createWindow(&wi)) {
        return -1;
    }

    start_game(wi.hwnd, &messages, hThisInstance);

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

//#define ROM "rom/Tennis.nes"
//#define ROM "rom/Dr_Mario.nes"
//#define ROM "rom/test.nes"
//#define ROM "rom/F-1.nes"
#define ROM "rom/dkk.nes"
void start_game(HWND hwnd, PMSG messages, HINSTANCE hInstance) {

    PlayPad *pad = new WinPad();
    Video *video = new DirectXVideo(hwnd); // WindowsVideo | DirectXVideo
    NesSystem fc(video, pad);

    if (int ret = fc.load_rom(ROM)) {
        MessageBox(hwnd, parseOpenError(ret), "����", 0);
        return;
    }

	cpu_6502* cpu = fc.getCpu();
    clock_t usetime = clock();

    bgpanel = bg_panel(hInstance, fc.getPPU());
    if (!bgpanel) {
        MessageBox(hwnd, "δ֪�Ĵ���", "����", 0);
        return;
    }

    /* Run the message loop. It will run until GetMessage() returns 0 */
    for(;;)
    {
    	if (PeekMessage(messages, NULL, 0, 0, PM_REMOVE)) {
    		if (messages->message==WM_QUIT) {
				break;
    		}
			TranslateMessage(messages);
			DispatchMessage(messages);
    	}

        /* ����,���ǲ���׼ȷ */
    	if (clock()-usetime<20) continue;
    	usetime = clock();

        //debugCpu(&fc);
        fc.drawFrame();
        displayCpu(cpu, hwnd);

        if (active) video->refresh();
    }

    delete video;
}

void displayCpu(cpu_6502* cpu, HWND hwnd) {
    static long frameC = 0, f2c = 0;
    static clock_t time = clock();

    frameC++; f2c++;
    if (frameC%20!=0) return;

    HDC hdc = GetDC(hwnd);
    int x = 300;
	int y = 0;
	int xIn = 70;
	int yIn = 18;
	char buf[128];

    sprintf(buf, "A: %02X", cpu->A);
    TextOut(hdc, x, y, buf, 5);
    sprintf(buf, "X: %02X", cpu->X);
    TextOut(hdc, x+=xIn, y, buf, 5);
    sprintf(buf, "Y: %02X", cpu->Y);
    TextOut(hdc, x+=xIn, y, buf, 5);
    y += yIn;
    x = 300;
    sprintf(buf, "SP: %02X", cpu->SP);
    TextOut(hdc, x, y, buf, 6);
    sprintf(buf, "PC: %04X", cpu->PC);
    TextOut(hdc, x+=xIn, y, buf, 8);
    sprintf(buf, "FG: %02X", cpu->FLAGS);
    TextOut(hdc, x+=xIn, y, buf, 6);

    double utime = (double)(clock() - time)/CLOCKS_PER_SEC;

    sprintf(buf, "frame: %09ld", frameC);
    TextOut(hdc, x+=xIn, y, buf, 16);
    sprintf(buf, "rate : %02lf/s", f2c/utime);
    TextOut(hdc, x, y-yIn, buf, 11);

    if (f2c>50) {
        time = clock();
        f2c = 0;
    }
}

#define MENU_FILE   1
#define MENU_DEUBG  2

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)              /* handle the messages */
    {
    case WM_ACTIVATE:
        {
        switch((LOWORD(wParam)))
            {
            case WA_ACTIVE:
            case WA_CLICKACTIVE:
                // �, ���Լ�������ҳ��滭��.
                active = true;
                break;
            case WA_INACTIVE:
                // ���, ֹͣ����ҳ��滭.
                //active = false;
                break;
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage (0);      /* send a WM_QUIT to the message queue */
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case MENU_FILE:
            break;

        case MENU_DEUBG:
            bgpanel->show();
            break;
        }
        break;

    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

HMENU createMainMenu() {
    HMENU hmenu = CreateMenu();
	InsertMenu(hmenu, 0, MF_BYPOSITION | MF_STRING, MENU_FILE,  "�ļ�");
	InsertMenu(hmenu, 1, MF_BYPOSITION | MF_STRING, MENU_DEUBG, "����");
	return hmenu;
}
