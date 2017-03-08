//---------------------------------------------------------------------------

#ifndef baseformH
#define baseformH
//---------------------------------------------------------------------------

/*- LOCAL MESSAGES -------------------*/
#define WM_Z_CLOSEUPDATE (WM_APP+95)
//---------------------------------------
#define WM_Z_INIT  (WM_APP + 98)
//---------------------------------------
#define WM_Z_RESTORE     (WM_APP + 100)
#define WM_Z_HIDE        (WM_APP + 120)
#define WM_Z_DAYCHANGED  (WM_APP + 121)
#define WM_Z_MINIMIZE    (WM_APP + 122)
#define WM_Z_MODALBEGIN  (WM_APP + 123)
#define WM_Z_DOCK        (WM_APP + 124)
#define WM_Z_UNDOCK      (WM_APP + 125)
#define WM_Z_LINK        (WM_APP + 126)
#define WM_Z_UPDATE      (WM_APP + 127)
#define WM_Z_ENABLE      (WM_APP + 128)
/*------------------------------------*/

#include "ZPaintBox.h"

#define WINDOW_SIZE_COUNT    6
extern TFormSizeInfo *fsi;

class TBehindShadow;

class TBaseForm;
typedef TTemplateList<TBaseForm, false> TBaseFormList;

enum TBaseFormEventListType { eltActivate, eltDeactivate, eltHide };
class __declspec(delphiclass) TBaseForm : public TForm
{
private:
    typedef TForm inherited;

    struct TEventListElem
    {
        TCPPNotifyEvent event;
        void *sender;
        TBaseFormEventListType type;
    };
    typedef TTemplateList<TEventListElem, true> TEventList;
    TEventList *events;

    struct TWndProcListElem
    {
        void *caller;
        TWndMethod method;
    };
    typedef TTemplateList<TWndProcListElem, true> TWndProcList;
    TWndProcList *wndmethods;
    TWndMethod oldwndproc;

    TBaseFormList *modalchildren;

    TNotifyEvent fonerase;

    static TBaseForm *CurrentMainForm;

    static int hookcnt;
    static HHOOK hook;
    friend LRESULT CALLBACK __bf_cbtproc(int nCode, WPARAM wParam, LPARAM lParam);

    int minw;
    int minh;
    int maxw;
    int maxh;
    int wdiff;
    int modx;
    int hdiff;
    int mody;

    bool fontop;

    bool fshadowed;
    TBehindShadow *shadow;

    bool fshowing;
    bool fkeepsize;

    bool fnoo;

    void set_ontop(bool val);
    bool get_ontop();
    void DoSetOnTop(bool val);

    int fposition;
    bool positioning;

    TFormSizeInfo fsize;

    void ExecuteEvent(TBaseFormEventListType atype);
    void AutoSize();

    bool fappwnd;
    TBaseForm *popupowner;
    TBaseForm *modalowner;
    bool get_app();
    void set_app(bool val);
    void set_noo(bool val);

    void RescaleStatusbar(TStatusBar *bar);

    void FindCurrentMainForm();
    void ChangeModalOwner(TBaseForm *newowner);
    void ChangePopupOwner(TBaseForm *newowner);
    bool HasPopupOwner(TBaseForm *isowner);

    int FindWindowProc(void *who, TWndMethod proc);

    friend TBehindShadow;
protected:
    TControl *oldactive;
    bool frestoreold;

    bool fshowactivate;
    bool ferase;
    bool fconstraint;

    __property WindowProc;

    DYNAMIC void __fastcall Activate();
    DYNAMIC void __fastcall Deactivate();

    virtual void __fastcall CreateParams(TCreateParams &Params);

    TRect GetAbsRect(TWinControl *control);
//void WMSysCommand(TWMSysCommand &msg);
    void WMSizing(TMessage &msg);
    void WMGetMinMaxInfo(TWMGetMinMaxInfo &msg);
    void WMMoveAndMoving(TMessage &msg);
    void CMShowingChanged(TMessage &msg);
    void WMDestroy(TMessage &msg);
    void WMSize(TWMSize &msg);

    void WMEraseBkgnd(TWMEraseBkgnd &msg);
    void set_erase(bool val);
    void set_onerase(TNotifyEvent val);
    void set_keep(bool val);

    void set_shadow(bool val);
    void settingsproc(void *caller, TSettingsProcState state);

    virtual void __fastcall CreateWnd();

    __property OnActivate;
    __property OnDeactivate;
    __property OnHide;
public:
    BEGIN_MESSAGE_MAP MESSAGE_HANDLER(CM_SHOWINGCHANGED, TMessage, CMShowingChanged)
        //MESSAGE_HANDLER(WM_SYSCOMMAND, TWMSysCommand, WMSysCommand)
        MESSAGE_HANDLER(WM_ERASEBKGND, TWMEraseBkgnd, WMEraseBkgnd)
        MESSAGE_HANDLER(WM_SIZING, TMessage, WMSizing)
        MESSAGE_HANDLER(WM_GETMINMAXINFO, TWMGetMinMaxInfo, WMGetMinMaxInfo)
        MESSAGE_HANDLER(WM_MOVING, TMessage, WMMoveAndMoving)
        MESSAGE_HANDLER(WM_MOVE, TMessage, WMMoveAndMoving)
        MESSAGE_HANDLER(WM_SIZE, TWMSize, WMSize)
    END_MESSAGE_MAP(TForm)

    __fastcall TBaseForm(TComponent *AOwner);
    virtual __fastcall ~TBaseForm();

    void AddEvent(void *Sender, TCPPNotifyEvent aprc, TBaseFormEventListType atype);
    void RemoveEvent(void *Sender, TCPPNotifyEvent aprc, TBaseFormEventListType atype);
    void RemoveEvents(void *Sender);

    void SaveFormSize(bool forced = false);
    void SaveFormSize(TRect r);

    void Show();
    virtual int __fastcall ShowModal();
    virtual void Recreate(bool setbounds);

    void RestorePosition(bool dimensions = true, bool topmost = false);

    void SetWindowProc(void *who, TWndMethod proc);
    void RestoreWindowProc(void *who, TWndMethod proc);
    void CallWindowProc(void *who, TWndMethod proc, TMessage &msg);

    void EnableAll();
    void DisableAll();

    void ConstrainOff();
    void Constrain(int minwidth, int minheight);
    void Constrain(int minwidth, int minheight, int maxwidth, int maxheight);
    void Constrain(int minwidth, int minheight, int maxwidth, int maxheight, int widthdiff, int modx, int heightdiff, int mody);

    TRect ChildRect(TControl *child);
    TRect ChildRect(TControl *child, int leftoffs, int topoffs, int rightoffs, int bottomoffs);

    __property bool RestoreOldActive = { read = frestoreold, write = frestoreold };
    __property bool EraseBackground = { read = ferase, write = set_erase }; // Enables or disabled background erasure.
    __property bool OnTop = { read = get_ontop, write = set_ontop }; // The window is always on top of non-topmost windows.
    __property bool ActivateOnShow = { read = fshowactivate, write = fshowactivate }; // The form will be activated when shown.
    __property bool KeepSize = { read = fkeepsize, write = set_keep }; // Saves form dimensions in fsize when form is resized.
    __property TFormSizeInfo SizeInfo = { read = fsize, write = fsize };

    __property int MinWidth = { read = minw };
    __property int MinHeight = { read = minh };

    __property TNotifyEvent OnEraseBackground = { read = fonerase, write = set_onerase };
    __property bool AppWindow = { read = get_app, write = set_app }; // Show icon on taskbar.
    __property bool NoOwner = { read = fnoo, write = set_noo }; // Always has the desktop as owner. cannot be used with appwindow.
    __property bool Shadowed = { /*read=get_shadow,*/write = set_shadow };
};

void InvalidateEmptyArea(TWinControl *c, bool recursive = true);

extern TTemplateList<TBaseForm, false> *activelist;

#endif
