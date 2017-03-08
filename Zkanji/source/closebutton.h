//---------------------------------------------------------------------------

#ifndef closebuttonH
#define closebuttonH
//---------------------------------------------------------------------------

enum TSysButtonState { sbsNormal, sbsHover, sbsPressed, sbsDown };
enum TSysButtonPosition { sbpTopLeft, sbpTopRight, sbpBottomLeft, sbpBottomRight };
class TBaseForm;
class TAlphaImageList;

typedef void(__closure * TDrawFuncPtr)(TCanvas * c, TRect const &r);

class TSysButtonBase
{
private:

    struct TSysButtonForm
    {
        TBaseForm *form;
        TSysButtonBase **buttons;
        int buttoncnt;

        TNotifyEvent fonPaint;
        TMouseEvent fonMouseDown;
        TMouseEvent fonMouseUp;
        TMouseMoveEvent fonMouseMove;
        TNotifyEvent fonMouseLeave;
        void __fastcall wndprc(TMessage &msg);

        void appDeactivate(void *caller);
        void formDeactivate(void *caller);
        void settingsChanged(void *caller, TSettingsProcState state);
        void __fastcall formPaint(TObject *Sender);
        void __fastcall formMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall formMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall formMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
        void __fastcall formMouseLeave(TObject *Sender);

        TSysButtonForm(TBaseForm *aform);
        ~TSysButtonForm();
    };
    friend struct TSysButtonForm;

    class TSysButtonForms : TTemplateList<TSysButtonForm, true>
    {
    private:
        typedef TTemplateList<TSysButtonForm, true> inherited;
    public:
        //TSysButtonForms();
        virtual ~TSysButtonForms()
        {
            Clear();
        }

        void AddButton(TBaseForm *form, TSysButtonBase *btn);
        void RemoveButton(TBaseForm *form, TSysButtonBase *btn);

        __property Count;
    };
    static TSysButtonForms *forms;

    TBmp *btn;

    TSysButtonPosition fpos;
    int fposx;
    int fposy;

    bool fvisible;

    TRect oldsize;
    TSysButtonState state; //state of the button
    bool pressed; //mouse is pressed on the button
    bool hover; //mouse is hovering on the button
    bool down; //button is down

    UnicodeString hint;

    int &counter;
    TAlphaImageList* &images;

    TBaseForm *owner;

    TDrawFuncPtr fbgdraw;
    TFuncPtr fproc;

    TRect buttonrect(const TRect &formrect);
    TRect get_clientrect();

    void set_visible(bool val);
    void set_down(bool val);
    int get_height();
    int get_width();
public:
  //pos specifies the corner where the button is placed. distx and disty is the distance of the nearest corner of the button to the corner of the window.
    TSysButtonBase(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, const wchar_t *ahint, TDrawFuncPtr bgdraw_func, TFuncPtr aproc, int &acounter, TAlphaImageList* &aimages, TImageList *normalimages, TImageList *alphaimages);
    virtual ~TSysButtonBase();

    void Redraw();

    __property int Height = { read = get_height };
    __property int Width = { read = get_width };
    __property TBaseForm *Owner = { read = owner };
    __property bool Visible = { read = fvisible, write = set_visible };
    __property bool Down = { read = down, write = set_down };

    __property TRect ClientRect = { read = get_clientrect };
};

class TCloseButton : public TSysButtonBase
{
private:
    typedef TSysButtonBase inherited;

    static int usagecnt;
    static TAlphaImageList *imglist;

    void closeproc();
public:
    TCloseButton(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, TDrawFuncPtr bgdraw_func, TFuncPtr aproc = NULL);
    virtual ~TCloseButton();
};

class TMidCloseButton : public TSysButtonBase
{
private:
    typedef TSysButtonBase inherited;

    static int usagecnt;
    static TAlphaImageList *imglist;

    void closeproc();
public:
    TMidCloseButton(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, TDrawFuncPtr bgdraw_func, TFuncPtr aproc = NULL);
    virtual ~TMidCloseButton();
};

class TSmallCloseButton : public TSysButtonBase
{
private:
    typedef TSysButtonBase inherited;

    static int usagecnt;
    static TAlphaImageList *imglist;

    void closeproc();
public:
    TSmallCloseButton(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, TDrawFuncPtr bgdraw_func, TFuncPtr aproc = NULL);
    virtual ~TSmallCloseButton();
};

class TPinButton : public TSysButtonBase
{
private:
    typedef TSysButtonBase inherited;

    static int usagecnt;
    static TAlphaImageList *imglist;
public:
    TPinButton(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, TDrawFuncPtr bgdraw_func, TFuncPtr fproc);
    virtual ~TPinButton();
};

class TMinButton : public TSysButtonBase
{
private:
    typedef TSysButtonBase inherited;

    static int usagecnt;
    static TAlphaImageList *imglist;
public:
    TMinButton(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, TDrawFuncPtr bgdraw_func, TFuncPtr fproc);
    virtual ~TMinButton();
};

class TFloatButton : public TSysButtonBase
{
private:
    typedef TSysButtonBase inherited;

    static int usagecnt;
    static TAlphaImageList *imglist;
public:
    TFloatButton(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, TDrawFuncPtr bgdraw_func, TFuncPtr fproc);
    virtual ~TFloatButton();
};


#endif
