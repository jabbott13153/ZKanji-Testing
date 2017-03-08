//---------------------------------------------------------------------------

#ifndef radicalsH
#define radicalsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Tabs.hpp>
#include "ZPaintBox.h"
//---------------------------------------------------------------------------

#include "baseform.h"
#include "edit2.h"
#include <ComCtrls.hpp>

class TKanjiEdit;

class TfRadicals : public TBaseForm
{
__published: //IDE-managed Components
    TZPaintBox *pbRad;
    TPanel *Panel1;
    TButton *btnCancel;
    TLabel *lbInfo;
    TButton *btnClear;
    TBevel *Bevel1;
    TPanel *pKanji;
    TZPaintBox *edKanji;
    TBevel *Bevel4;
    TLabel *Label1;
    TSpeedButton *btnGroup;
    TSpeedButton *btnExact;
    TSpeedButton *btnNames;
    TZPaintBox *pbSelected;
    TButton *btnAdd;
    TLabel *Label2;
    TEdit2 *edStrokes;
    TBevel *b1;
    TButton *btnOk;
    TBevel *b0;
    TBevel *Bevel2;
    TBevel *Bevel5;
    TPanel *Panel2;
    TTabControl *tcSelect;
    void __fastcall pbRadPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall pbRadMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnClearClick(TObject *Sender);
    void __fastcall tcSelectClick(TObject *Sender);
    void __fastcall btnGroupClick(TObject *Sender);
    void __fastcall btnNamesClick(TObject *Sender);
    void __fastcall edKanjiKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall pbRadKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbSelectedPaint(TObject *Sender);
    void __fastcall btnAddClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall pbRadMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall pbRadMouseLeave(TObject *Sender);
    void __fastcall cbStrokesChange(TObject *Sender);
    void __fastcall edStrokesChange(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall pbRadKeyPress(TObject *Sender, wchar_t &Key);
private: //User declarations
    void __fastcall WMSizing(TWMSizing &msg);
    int cols;
    int rows;
    int fselcount;

    HFONT Kfont;

    int cellsize;
    byte rowheight;

    TNumEdit *numStrokes;

    int *wid; //width for each radical button in classical2 view
    int *halves; //where appropriate, make the readings into two rows
    bool *sel; //selected radicals in classical 2 view
    int *ind; //index for first element of these radicals on every row
    int linecnt;
    void kcallback();
    void Paint2();
    void InvalidateRad(word pos);
    void MouseDown2(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y);
    void MeasureBoxSize2();
    TIntegerList *filter; //list of indexes of radicals selected by the currently entered filter text
    void get_radical_data(int pos, int &rad, UnicodeString &kanji, UnicodeString &names);
    void setselcount(int ix);
    int mouseonrad2(int x, int y);

    void FilterKanji();
public: //User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_SIZING, TWMSizing, WMSizing)
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfRadicals(TComponent* Owner);
    virtual __fastcall ~TfRadicals();

    void AddSelected2(TIntegerList *list);

    TKanjiEdit *edkanji;

    bool selected[RAD_COUNT];
    void select(int ix, bool multi);

    __property int selcount = { read = fselcount, write = setselcount };
};
//---------------------------------------------------------------------------
extern PACKAGE TfRadicals *fRadicals;
//---------------------------------------------------------------------------
#endif
