//---------------------------------------------------------------------------

#ifndef recognizerH
#define recognizerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include <Menus.hpp>

#include "strokebase.h"

class TCloseButton;
class TKanjiPanelObj;
struct TKanjiElement;
class TKanjiEdit;
enum TRecognizePosition;

class TfRecognize : public TBaseForm
{
__published: //IDE-managed Components
    TZPaintBox *pbDraw;
    TPanel *pResults;
    TBevel *Bevel22;
    TSpeedButton *btnResBack;
    TSpeedButton *btnResFore;
    TZPaintBox *pbResults;
    TSpeedButton *btnUndo;
    TSpeedButton *btnReset;
    TBevel *bv;
    TPopupMenu *pmPanel;
    TMenuItem *miCopy;
    TMenuItem *miAppend;
    TSpeedButton *btnGen;
    TSpeedButton *btnGrid;
    TMenuItem *N1;
    TMenuItem *miInfo;
    void __fastcall FormResize(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall btnResBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnResForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnResetClick(TObject *Sender);
    void __fastcall btnUndoClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall pbResultsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall miCopyClick(TObject *Sender);
    void __fastcall miAppendClick(TObject *Sender);
    void __fastcall btnGenClick(TObject *Sender);
    void __fastcall btnGridClick(TObject *Sender);
    void __fastcall miInfoClick(TObject *Sender);
    void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);

private: //User declarations
    TRect oldsize;
    TCloseButton *closebtn;
    TStrokeObj *sobj;
    TSpeedButton *fbutton;

    TKanjiEdit *feditor;

    TKanjiPanelObj *poresults;
    void pcallback();

    bool finited;
    bool fresetsize;
    bool fresetpos;
    bool fautohide;
    bool fshowpopup;
    TRecognizePosition fposition;

    int savedleft, savedtop;

    TFuncPtr fcallback;

    void repaintProc(void *caller);

    void appDeactivate(void *caller);
    void roundbounds(int fw, int fh);

    void cbgptr(TCanvas *c, TRect const & r);
    void scallback();

    void set_buttons();

    bool get_showhint();
    void set_showhint(bool val);
    bool get_allowerror();
    void set_allowerror(bool val);

    void rcolor(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected, TColor &background, TColor &fontcolor);
    void rdraw(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected);
protected:
    void WMSizing(TWMSizing &msg);
    void WMNCActivate(TMessage &msg);
    void WMNCCalcSize(TMessage &msg);
    void WMNCHitTest(TMessage &msg);
    void WMNCPaint(TMessage &msg);
    void WMZInit(TMessage &msg);

    using TBaseForm::Show;
public: //User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_SIZING, TWMSizing, WMSizing)
        MESSAGE_HANDLER(WM_NCACTIVATE, TMessage, WMNCActivate)
        MESSAGE_HANDLER(WM_NCCALCSIZE, TMessage, WMNCCalcSize)
        MESSAGE_HANDLER(WM_NCHITTEST, TMessage, WMNCHitTest)
        MESSAGE_HANDLER(WM_NCPAINT, TMessage, WMNCPaint)
        MESSAGE_HANDLER(WM_Z_INIT, TMessage, WMZInit)
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfRecognize(TComponent* Owner);
    virtual __fastcall ~TfRecognize();

    void SetDimensions(int aleft, int atop, int awidth);
    void ShowEx(TKanjiEdit *aedit, TSpeedButton *abutton, bool aautohide, bool ashowhint, bool aallowerror, int acntlimit, bool ageneral, bool ashowpopup, TRecognizePosition apos);
    void Place(TKanjiEdit *aedit, TRecognizePosition apos);
    void Hide();

    void Reset();

    __property TKanjiEdit *editor = { read = feditor };
    __property bool resetsize = { read = fresetsize, write = fresetsize };
    __property bool resetpos = { read = fresetpos, write = fresetpos };
    __property bool showhint = { read = get_showhint, write = set_showhint };
    __property bool allowerror = { read = get_allowerror, write = set_allowerror };
    __property bool showpopup = { read = fshowpopup, write = fshowpopup };

  //only used by settings to save the left and top coordinates without having to reposition the window
    __property int SavedLeft = { read = savedleft };
    __property int SavedTop = { read = savedtop };

    __property TFuncPtr callback = { read = fcallback, write = fcallback };
};

extern PACKAGE TfRecognize *fRecognize;

#endif
