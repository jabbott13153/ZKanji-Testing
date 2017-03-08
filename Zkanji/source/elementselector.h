//---------------------------------------------------------------------------

#ifndef elementselectorH
#define elementselectorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "baseform.h"
#include <ExtCtrls.hpp>
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include "edit2.h"
#include "customgrid.h"
//---------------------------------------------------------------------------
class TKanjiEdit;

class TElementSelectGrid : public TCustomMultiselectGridObj
{
private:
    typedef TCustomMultiselectGridObj inherited;

    int *fitems;
    int fcount;
    int get_item(int ix);
protected:
    virtual int get_count();
    virtual void DoDrawCell(TCanvas *c, const TRect &r, int acell);
public:
    TElementSelectGrid(TZPaintBox *apb, int aboxsize, TFuncPtr acallback);
    virtual ~TElementSelectGrid();

    void set_items(TIntegerList *items, int newpos = -1);
    int indexof(int elem);
    int elemindex(int x, int y);

    __property int items[int ix] = { read = get_item };
};

class TfSelectElem : public TBaseForm
{
__published: //IDE-managed Components
    TCheckBox *cbStroke;
    TEdit2 *edStroke;
    TLabel *Label1;
    TComboBox *cbPattern;
    TLabel *Label3;
    TLabel *Label2;
    TPanel *p1;
    TLabel *Label7;
    TCheckBox *cbStroke1;
    TEdit2 *edStroke1;
    TComboBox *cbPattern1;
    TLabel *Label8;
    TComboBox *cbS11;
    TComboBox *cbS12;
    TComboBox *cbS13;
    TComboBox *cbS14;
    TLabel *Label9;
    TPanel *p2;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    TCheckBox *cbStroke2;
    TEdit2 *edStroke2;
    TComboBox *cbPattern2;
    TComboBox *cbS21;
    TComboBox *cbS22;
    TComboBox *cbS23;
    TComboBox *cbS24;
    TPanel *p3;
    TLabel *Label10;
    TLabel *Label11;
    TLabel *Label12;
    TCheckBox *cbStroke3;
    TEdit2 *edStroke3;
    TComboBox *cbPattern3;
    TComboBox *cbS32;
    TComboBox *cbS33;
    TComboBox *cbS34;
    TPanel *p4;
    TLabel *Label13;
    TLabel *Label14;
    TLabel *Label15;
    TCheckBox *cbStroke4;
    TEdit2 *edStroke4;
    TComboBox *cbPattern4;
    TComboBox *cbS41;
    TComboBox *cbS42;
    TComboBox *cbS43;
    TComboBox *cbS44;
    TZPaintBox *pbGrid;
    TBevel *b2;
    TCheckBox *cb1;
    TCheckBox *cb2;
    TCheckBox *cb3;
    TCheckBox *cb4;
    TComboBox *cbS31;
    TSpeedButton *sbReset;
    TBitBtn *btnOk;
    TBitBtn *BitBtn2;
    TBevel *b3;
    TEdit2 *edMeaning;
    TZPaintBox *edReading;
    TEdit2 *edIndex;
    TCheckBox *cbOn;
    TCheckBox *cbKun;
    TCheckBox *cbOku;
    TCheckBox *cbMeaning;
    TCheckBox *cbReading;
    TCheckBox *cbIndex;
    TBevel *b1;
    void __fastcall cbPattern1MeasureItem(TWinControl *Control, int Index, int &Height);
    void __fastcall cbPattern1DrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
    void __fastcall BitBtn2Click(TObject *Sender);
    void __fastcall btnOkClick(TObject *Sender);
    void __fastcall cbPatternChange(TObject *Sender);
    void __fastcall cbPattern1Change(TObject *Sender);
    void __fastcall cbStroke1Click(TObject *Sender);
    void __fastcall sbResetClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall edMeaningChange(TObject *Sender);
    void __fastcall cbMeaningClick(TObject *Sender);
    void __fastcall cbReadingClick(TObject *Sender);
    void __fastcall cbIndexClick(TObject *Sender);
    void __fastcall cb1Click(TObject *Sender);
    void __fastcall cbStrokeClick(TObject *Sender);
    void __fastcall pbGridMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
private: //User declarations
    TNumEdit *numStroke;
    TNumEdit *numStroke1;
    TNumEdit *numStroke2;
    TNumEdit *numStroke3;
    TNumEdit *numStroke4;
    TNumEdit *numIndex;

    TKanjiEdit *edreading;

    void set_enabled(TPanel *p, bool enable);
    void ncallback();
    void n1callback();
    void n2callback();
    void n3callback();
    void n4callback();
    void ecallback();
    void icallback();
    void rcallback();

    __property bool enabled[TPanel * p] = { write = set_enabled };

    void Reset(bool filter);
    void Filter();
    bool fnofilter;

    TElementSelectGrid *egrid;

public: //User declarations
    __fastcall TfSelectElem(TComponent* Owner);
    virtual __fastcall ~TfSelectElem();

    int ShowModalEx(int &val);
};
//---------------------------------------------------------------------------
extern PACKAGE TfSelectElem *fSelectElem;
//---------------------------------------------------------------------------
#endif
