//---------------------------------------------------------------------------

#ifndef writetestH
#define writetestH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include "edit2.h"
//---------------------------------------------------------------------------
#include "baseform.h"
#include "strokebase.h"

class TStrokeTester;

class TWriteObj : public TStrokeObjBase
{
private:
    TStrokeTester *tester;
    TFuncPtr fcallback;
    bool fdebug;
    bool fhint;

    void set_elem(int ix);
    void set_debug(bool val);
    void set_hint(bool val);
    int get_elem();
    void set_stroke(int ix);
    int get_stroke();
protected:
    void procstroke();
public:
    TWriteObj(TZPaintBox *fb);
    virtual ~TWriteObj();

    __property bool hint = { read = fhint, write = set_hint };
    __property bool debug = { read = fdebug, write = set_debug };
    __property int element = { read = get_elem, write = set_elem };
    __property int stroke = { read = get_stroke, write = set_stroke };
    __property TFuncPtr callback = { read = fcallback, write = fcallback };
};

class TfWriter : public TBaseForm
{
__published:
    TZPaintBox *pb;
    TBitBtn *btnClose;
    TEdit2 *edElem;
    TLabel *Label1;
    TZPaintBox *pbChar;
    TLabel *Label2;
    TEdit2 *edStroke;
    TMemo *memo;
    TSpeedButton *btnLockStroke;
    TCheckBox *cbDebug;
    TCheckBox *cbHint;
    void __fastcall pbCharPaint(TObject *Sender);
    void __fastcall cbDebugClick(TObject *Sender);
    void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall cbHintClick(TObject *Sender);
private:
    TWriteObj *wobj;
    TNumEdit *nelem;
    TNumEdit *nstroke;
    wchar_t ch;

    void ncallback();
    void scallback();
    void wcallback();
public:
    __fastcall TfWriter(TComponent* Owner);
    virtual __fastcall ~TfWriter();

    void ShowModalEx(short element, bool showmatch);
};
//---------------------------------------------------------------------------
extern PACKAGE TfWriter *fWriter;
//---------------------------------------------------------------------------

#endif
