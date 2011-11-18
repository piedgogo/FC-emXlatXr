/*----------------------------------------------------------------------------*\
 * windows UI�����, �ṩ�򵥿��                                             *
 * CatfoOD 2011                                                               *
 * yanming-sohu@sohu.com                                                      *
\*----------------------------------------------------------------------------*/

#ifndef WIT_H_INCLUDED
#define WIT_H_INCLUDED

#include <windows.h>

#define EVENT_FUNC_DEF(_n)  void wWindow::_n
#define EVENT_FUNC_DECL(_n) virtual void _n

/* -------------------------------------------------- *|
 * �����¼�����,                                   -- *|
 * inThread==true �������µ��߳�, �ú�����������   -- *|
 * ����ֱ�����еĴ��ڶ������ٺ�, �ú����ŷ���      -- *|
 * ��ε���ֻ������һ���¼�������                  -- *|
 * -------------------------------------------------- */
void startMessageLoop(bool inThread);

/* -------------------------------------------------- *|
/* ����һ��windowsϵͳ�¼�����������               -- *|
 * �������false˵�����յ�WM_QUIT��Ϣ              -- *|
 * -------------------------------------------------- */
bool wPeekAMessage();


/* �¼��������Ĳ��� */
struct WIT_EVENT_PARM {
    HWND    hwnd;
    UINT    message;
    WPARAM  wParam;
    LPARAM  lParam;
    LRESULT ret;     /* ��Ϣ�ķ���ֵ                  */
    bool    defproc; /* ���true, ��ʹ��Ĭ�ϴ�����  */
};

class wMenu {

friend class wWindow;

private:
    /* �����ڹر�,ϵͳ�Զ��ͷ��Ѿ�������window�Ĳ˵� */
    HMENU menu;
    UINT position;

    wMenu *parent;
    wMenu *lastsub;
    wMenu *previous;

    wMenu(HMENU _menu);
    wMenu(wMenu *parent, HMENU _menu);
    ~wMenu();

    void _free();

public:
    /* ���ز˵����ID */
    UINT    addItem     (UINT id,  char* text);
    wMenu*  createSub   (UINT id,  char* text);
    void    setText     (UINT item,char* text);
    void    setEnable   (UINT id,  bool  b   );
    void    setChecked  (UINT id,  bool  b   );
    void    separator   ();
};

typedef wMenu* pwMenu;

/* �ö�����Ҫ�ֹ��ͷ��ڴ�, ��window�����ٵ�ʱ���Զ��ͷ� */
class wWindow {

private:
    HWND      hwnd;
    HINSTANCE hInstance;
    pwMenu    hmenu;
    UINT      menuCount;
    double    scale;

    void _init(HWND parent);

public:
    /* ��������һ������, ������ʾ */
    wWindow();
    /* ����һ���Ӵ��� */
    wWindow(HWND parent);
    wWindow(wWindow *parent);

    virtual ~wWindow();

    /* �������ٸô���, �ͷ��ڴ�, ��֮��صĶ��󶼲���ʹ�� */
    void destroy();
    /* ���ô����Ƿ���ʾ */
    void setVisible(bool show);
    /* ���ù������ߴ� */
    void setClientSize(int, int);
    /* �ı䴰�ڵ�λ�� */
    void setPosition(int, int);
    /* ���ñ��� */
    void setTitle(char *txt);
    /* �����ı� */
    void setText(char *txt);
    /* ��������Ƿ����� */
    void setEnable(bool en);
    /* ���ô��ڱ���, 0������, w/h */
    void setScale(double s);
    /* ����һ����ʱ������ʼ��ʱ,�ﵽuElapseʱ��(����),on_time���������� */
    void startTimer(UINT timer_id, UINT uElapse_ms);
    /* ������ֹһ����ʱ�� */
    void stopTimer(UINT timer_id);
    /* ȡ�õ�ǰ���ھ�� */
    HWND getWindowHandle();
    /* ���ز˵�����(�����ͷ�), ��һ�ε��ûᴴ��һ�� */
    pwMenu getMenuBar();

// *-------------------------------------------------------------------- //--//
//  �����д���¼��������, �򲻻��ٵ���DefWindowProc
//  Ĭ��ʵ��ʹ�� DEF_EVENT_FUNC_IMPL(...),
//  WIT_EVENT_PARM �������¼����������غ�ʧЧ
// *-------------------------------------------------------------------- //--//
#define DEF_EVENT_FUNC_IMPL(_n) EVENT_FUNC_DECL(_n)(WIT_EVENT_PARM *p) \
                                { p->defproc = true; }

    /* ���ڵ��¼�������, �ͻ���Ҫֱ�ӵ��� */
    LRESULT _event_procedure(UINT message, WPARAM wParam, LPARAM lParam);

    /* Ĭ�ϵĲ˵��������, ʲô������ */
    EVENT_FUNC_DECL     (on_menu)   (WIT_EVENT_PARM*, UINT menu_id);
    /* Ĭ�ϵĿؼ�(��ť��)�������, ʲô������ */
    EVENT_FUNC_DECL     (on_widget) (WIT_EVENT_PARM*, HWND widget);

    /* Ĭ�ϻ����ٸô��� */
    EVENT_FUNC_DECL     (on_close)  (WIT_EVENT_PARM*);
    /* Ĭ�ϻ��ӡ����, ��������˶�ʱ��������д�÷��� */
    EVENT_FUNC_DECL     (on_time)   (UINT timer_id);

    DEF_EVENT_FUNC_IMPL (on_paint);

#undef DEF_EVENT_FUNC_IMPL
};

typedef wWindow* pwWindow;

#endif // WIT_H_INCLUDED
