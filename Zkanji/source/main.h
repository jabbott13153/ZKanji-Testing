//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <StdCtrls.hpp>
#include <ImgList.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include <pngimage.hpp>
#include <jpeg.hpp>

class TTextSearchTree;
class TWordIndexList;
class TWordCollection;
struct TKanji;
struct TWord;
class TKanjiList;
class TKanjiItemList;
class TKanjiDrawObj;
class TCustomKanjiGrid;
class TWordFilterSelection;
//---------------------------------------------------------------------------
class TfMain : public TBaseForm
{
__published: //IDE-managed Components
    TBitBtn *bbHide;
    TImage *imgLogo;
    TLabel *Label3;
    TPanel *pLoad;
    TLabel *lbLoad;
    TLabel *lbLoadMsg;
    TImage *imgTopBg;
    TLabel *lbVer;
    TLabel *lbReleaseDate;
    TLabel *lbStrokes;
    TLabel *Label1;
    TImage *Image3;
    TImage *imgTR;
    TImage *imgBR;
    TImage *imgBL;
    TImage *imgL;
    TImage *imgR;
    TImage *imgT;
    TImage *imgB;
    TPanel *Panel2;
    TLinkLabel *LinkLabel2;
    TLinkLabel *LinkLabel1;
    TLabel *lb1;
    TPanel *pJMDICT;
    TRichEdit *reJMDICT;
    void __fastcall bbHideClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall LinkLabel1LinkClick(TObject *Sender, const UnicodeString Link, TSysLinkType LinkType);
    void __fastcall LinkLabel2LinkClick(TObject *Sender, const UnicodeString Link, TSysLinkType LinkType);
    void __fastcall FormShow(TObject *Sender);
private: //User declarations
    typedef TBaseForm inherited;

    void z_restore(TMessage &msg);
    void z_minimize(TMessage &msg);
    void z_modal(TMessage &msg);
    void z_hide(TMessage &msg);
    void z_dock(TMessage &msg);
    void z_undock(TMessage &msg);
    void z_themechange(TMessage &msg);
    void z_closeupdate(TMessage &msg);
    void z_update(TMessage &msg);
    void z_init(TMessage &msg);

    int inited;

    class TUpdateTasksThread : public TThread
    {
    private:
        typedef TThread inherited;
    public:
        static TUpdateTasksThread *thread;
        static bool waiting;
        static UnicodeString langname;

        static void SetTaskbarTasks();

        __fastcall TUpdateTasksThread();
        virtual __fastcall ~TUpdateTasksThread();
        virtual void __fastcall Execute();
    };

    void NoStrokeText();
    void HasStrokeText();

    __int64 secTimer;
    __int64 checkTimer;
    bool inTimer;
    __int64 lastAutosave;
    __int64 secAutosave; //<= 0 when turned off
    int lastTimedDay;

    //void WMNCActivate(TMessage &msg);
    void WMNCCalcSize(TMessage &msg);
    void z_composition(TMessage &msg);
    //void WMNCPaint(TWMNCPaint &msg);
    //int DoTerminate();

    //void roundbounds();

    void WMTimer(TMessage &msg);

    void z_link(TMessage &msg);

    virtual void __fastcall WndProc(TMessage &msg);

    void SearchW(TWordIndexList *l, bool start, bool end, bool infl, UnicodeString c1, UnicodeString c2, TWordFilterSelection *filter = NULL);

    //friend bool terminateproc();
public: //User declarations
    BEGIN_MESSAGE_MAP
        //MESSAGE_HANDLER(WM_HOTKEY, TWMHotkey, WMHotkey)
        MESSAGE_HANDLER(WM_Z_RESTORE, TMessage, z_restore)
        MESSAGE_HANDLER(WM_Z_MINIMIZE, TMessage, z_minimize)
        MESSAGE_HANDLER(WM_Z_DOCK, TMessage, z_dock)
        MESSAGE_HANDLER(WM_Z_UNDOCK, TMessage, z_undock)
        MESSAGE_HANDLER(WM_Z_HIDE, TMessage, z_hide)
        MESSAGE_HANDLER(WM_TIMER, TMessage, WMTimer)
        MESSAGE_HANDLER(WM_Z_MODALBEGIN, TMessage, z_modal)
        MESSAGE_HANDLER(WM_THEMECHANGED, TMessage, z_themechange)
        MESSAGE_HANDLER(WM_NCCALCSIZE, TMessage, WMNCCalcSize)
        MESSAGE_HANDLER(WM_DWMCOMPOSITIONCHANGED, TMessage, z_composition)
        MESSAGE_HANDLER(WM_Z_CLOSEUPDATE, TMessage, z_closeupdate)
        MESSAGE_HANDLER(WM_Z_LINK, TMessage, z_link)
        MESSAGE_HANDLER(WM_Z_UPDATE, TMessage, z_update)
        MESSAGE_HANDLER(WM_Z_INIT, TMessage, z_init)
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfMain(TComponent* Owner);
    void Init();

    void showlink(UnicodeString link);

    void SetTaskbarTasks();

    //virtual void Recreate(bool setbounds);

    void SearchLatin(TWordIndexList *l, bool start, bool end, bool infl, UnicodeString c, TWordFilterSelection *filter = NULL);
    void SearchJapan(TWordIndexList *l, bool start, bool end, bool infl, UnicodeString c, TWordFilterSelection *filter = NULL);

    void SearchK(TWordIndexList *l, bool start, bool end, bool infl, UnicodeString c, TWordFilterSelection *filter = NULL);

    enum TMinimizeAction { maMinimize = 0, maPopupJE, maPopupEJ, maKanji };
    void Minimize(TMinimizeAction maction = maMinimize);
    void Restore();
    //void ResetPositions(int pos);
    void StartAutosave(int minutes);
    void StopAutosave();

    __int64 RunningTime(); // Seconds passed since the program was started.
};
//---------------------------------------------------------------------------
extern PACKAGE TfMain *fMain;
//---------------------------------------------------------------------------
//extern TTemplateList<TBaseForm,false> *WindowList;

void DictionaryAdded(UnicodeString s); //add items to menu
void DictionaryDeleted(byte coll);

//void ChangeFocus(bool back);
//return true when exp expression matches the w string
bool reg_match(const wchar_t *w, const wchar_t *exp);
//void RestoreFormSize(TBaseForm *f, TFormSizeInfo size, bool dimensions = true);
void EditWordDef(TWordCollection *collection, int wordindex, char wordmeaning);
void DeleteWordEntry(byte collection, int wordindex, char meaning);
void RevertWordEntry(int wordindex);
void AngleTextOutEx(TCanvas *c, int angle, int x, int y, TRect r, UnicodeString str);
//void ScaledTextOutExA(TCanvas *c, float sx, float sy, int x, int y, TRect r, char *str, int length);
void ScaledTextOutExW(TCanvas *c, float sx, float sy, int x, int y, TRect r, wchar_t *str, int length);
void ChangeDef(int collection, wchar_t k, int pos);
void ChangeDef(int collection, TKanjiList *list, int pos);
void Repaint(TWinControl*);
//void UpdateDictionaries(byte coll); // use when word added or deleted
void KanjiToGroup(TKanji *k);
void KanjiToGroup(TKanjiItemList *k);
void GenerateWordGroup(TKanji *k);
void GenerateWordGroup(TKanjiList *kl);
bool HideInfoWindows(bool inkeydown = false);
bool InfoWindowVisible();
void FormActivated(TBaseForm *f);
//void ImportElements();
void HideAllWindows();
void ShowAllWindows();
void KanaPractice();

void fill_menu(); // Fills dictionary menus in all windows.

#endif
