//---------------------------------------------------------------------------

#ifndef clipviewH
#define clipviewH
//---------------------------------------------------------------------------
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include <ComCtrls.hpp>

class TCloseButton;

class TfClipboard : public TBaseForm
{
__published: //IDE-managed Components
    TZPaintBox *pbLines;
    TCheckBox *cbAuto;
    TBevel *Bevel2;
    TSpeedButton *btnTranslate;
    TSpeedButton *btnClear;
    TSpeedButton *btnGet;
    TBevel *bv1;
    TStatusBar *sbInfo;
    void __fastcall btnGetClick(TObject *Sender);
    void __fastcall pbLinesPaint(TObject *Sender);
    void __fastcall pbLinesResize(TObject *Sender);
    void __fastcall btnClearClick(TObject *Sender);
    void __fastcall pbLinesMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pbLinesMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall btnTranslateClick(TObject *Sender);
    void __fastcall cbAutoClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall pbLinesKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall pbLinesKeyPress(TObject *Sender, wchar_t &Key);
private: //User declarations
    TRect oldsize;

    TCloseButton *closebtn;
    void cbgptr(TCanvas *c, TRect const & r);

    UnicodeString ftext;

    int cols, rows; //number of visible rows and columns
    int sizex, sizey; //one character size

    int fselstart;
    int fsellength;
    bool fselecting;

    void repaintProc(void *caller);

    void measure_box_size();
    int get_selstart();
    int get_sellength();

    void roundbounds(int fw, int fh);
    __property int selstart = { read = get_selstart };
    __property int sellength = { read = get_sellength };

    friend void ShowClipboard();
protected:
    void WMNCActivate(TMessage &msg);
    void WMNCCalcSize(TMessage &msg);
    void WMNCHitTest(TMessage &msg);
    void WMNCPaint(TMessage &msg);
public: //User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_NCACTIVATE, TMessage, WMNCActivate)
        MESSAGE_HANDLER(WM_NCCALCSIZE, TMessage, WMNCCalcSize)
        MESSAGE_HANDLER(WM_NCHITTEST, TMessage, WMNCHitTest)
        MESSAGE_HANDLER(WM_NCPAINT, TMessage, WMNCPaint)
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfClipboard(TComponent* Owner);
    virtual __fastcall ~TfClipboard();

    void Show();
};

void ShowClipboard();

//---------------------------------------------------------------------------
extern PACKAGE TfClipboard *fClipboard;
//---------------------------------------------------------------------------
#endif
