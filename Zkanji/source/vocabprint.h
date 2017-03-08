//---------------------------------------------------------------------------

#ifndef vocabprintH
#define vocabprintH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

/*
Japanese on the left, meaning on the right
Meaning on the left, japanese on the right
Japanese on the left, no meaning
Meaning on the left, no japanese
Japanese and meaning on separate pages
*/

#include "baseform.h"

class TWordCollection;
class TWordIndexList;
struct TFuriganaData;
enum TVPrintItem;

class TfVocabPrint : public TBaseForm
{
__published: //IDE-managed Components
    TPrintDialog *pdSet;
    TStatusBar *sbInfo;
    TPanel *pBar;
    TProgressBar *pbBar;
    TButton *btnAbort;
    TBitBtn *btnPrint;
    TBitBtn *btnSet;
    TBitBtn *btnSettings;
    TBitBtn *btnList;
    TSpeedButton *btnLeft;
    TLabel *Label1;
    TLabel *lbPage;
    TSpeedButton *btnRight;
    TBevel *Bevel3;
    TSpeedButton *btnTwo;
    TSpeedButton *btnOne;
    TSpeedButton *btnFit;
    TSpeedButton *btnFill;
    TSpeedButton *btnOut;
    TSpeedButton *btnIn;
    TBitBtn *btnClose;
    TBevel *Bevel2;
    TBevel *bv0;
    TZPaintBox *pbView;
    void __fastcall psdDrawFullPage(TObject *Sender, TCanvas *Canvas, TRect &PageRect, bool &DoneDrawing);
    void __fastcall btnSetClick(TObject *Sender);
    void __fastcall btnListClick(TObject *Sender);
    void __fastcall pbViewPaint(TObject *Sender);
    void __fastcall pbViewResize(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall btnTwoClick(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall btnFillClick(TObject *Sender);
    void __fastcall btnFitClick(TObject *Sender);
    void __fastcall btnOutClick(TObject *Sender);
    void __fastcall btnInClick(TObject *Sender);
    void __fastcall pbViewMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pbViewMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall pbViewMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnSettingsClick(TObject *Sender);
    void __fastcall btnPrintClick(TObject *Sender);
    void __fastcall btnCloseClick(TObject *Sender);
    void __fastcall btnAbortClick(TObject *Sender);
    void __fastcall btnRightMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnLeftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall pdSetShow(TObject *Sender);
    void __fastcall pdSetClose(TObject *Sender);
private: //User declarations

    enum TPrintState { psNone, psPreparing, psPrinting };

    struct TVPrintSizes
    {
        int height;
        int width;
    };

    TBmp *buff;

    HDC oldDC;
    HDC printDC;
    HBITMAP printBMP;
    HENHMETAFILE image, imageb;
    TPrinter *p;
    TWordIndexList *l;

    TIntegerList *firsts;
    TVPrintSizes *lefts;
    TVPrintSizes *rights;

    HFONT meaningfont;
    HFONT japanesefont;
    HFONT furiganafont;
    HFONT pagefont;

    TPoint dragpos;
    bool dragging;

    TPrintState state;

  //in millimeters
    int resx;
    int resy;
    int margin;
  //in pixels
    int lineh;
    int marginh;
    int pagew;
    int pageh;
    int draww;
    int drawh;

    int linecnt;
    int pagecnt;
    int fpage;

    int zoom; //100 means 100% of screen width

    void /*__fastcall*/ ProcessLines();
    int /*__fastcall*/ DrawPage(TCanvas *c, TRect r, int page, int ix, bool measure);

    int /*__fastcall*/ vget_width(TCanvas *c, TVPrintItem it, TWordIndexList *l, int ix);
    int /*__fastcall*/ vused_width(TCanvas *c, TVPrintItem it, TWordIndexList *l, int ix, int maxw, TFuriganaData *dat, int datcnt);
    void /*__fastcall*/ vdraw_furigana(TCanvas *c, int x, int y, wchar_t *kanji, wchar_t *kana, int pos, int len, int *sizes, TFuriganaData *dat, int datcnt);
    void /*__fastcall*/ vdraw_single(TCanvas *c, TVPrintItem it, TWordIndexList *l, int ix, int x, int y, int linew, int lineh, bool left, TFuriganaData *dat, int datcnt);
    int /*__fastcall*/ vdraw_text(TCanvas *c, TVPrintItem it, TWordIndexList *l, int ix, int x, int y, int linew, int lineh, bool left, bool measure, TFuriganaData *dat, int datcnt);

    void /*__fastcall*/ CreateMetaFiles();
    void /*__fastcall*/ FreeMetaFiles();
    void /*__fastcall*/ FreeBmp();
    void /*__fastcall*/ RecreatePages();
    void /*__fastcall*/ UpdatePages();
    void /*__fastcall*/ CreatePage(HENHMETAFILE &meta, int page);

    void /*__fastcall*/ set_page(int val);

    void /*__fastcall*/ MeasureScrollSize();

    void /*__fastcall*/ create_fonts();
    void /*__fastcall*/ free_fonts();

    void /*__fastcall*/ SetInfo();

//void __fastcall WMSysCommand(TWMSysCommand &msg);
    void __fastcall z_init(TMessage &msg);

public: //User declarations
    BEGIN_MESSAGE_MAP
        //MESSAGE_HANDLER(WM_SYSCOMMAND, TWMSysCommand, WMSysCommand)
        MESSAGE_HANDLER(WM_Z_INIT, TMessage, z_init)
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfVocabPrint(TComponent* Owner);
    virtual __fastcall ~TfVocabPrint();

    int ShowModalEx(TWordIndexList *al);

    __property int page = { read = fpage, write = set_page };
};
//---------------------------------------------------------------------------
extern PACKAGE TfVocabPrint *fVocabPrint;
//---------------------------------------------------------------------------
#endif
