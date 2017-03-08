//---------------------------------------------------------------------------

#ifndef kanjidefH
#define kanjidefH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include "ZPaintBox.h"
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include "edit2.h"

class TKanjiDrawObj;
class TWordCollection;
class TWordListObj;
class TKanjiList;
struct TKanji;
class TZMenuRows;

class TfDef : public TBaseForm
{
__published: //IDE-managed Components
    TLabel *Label1;
    TEdit2 *edMeaning;
    TButton *btnOk;
    TBitBtn *btnCancel;
    TLabel *Label2;
    TBevel *bvleft;
    TButton *btnNext;
    TCheckBox *cbSkip;
    TBevel *bv6;
    TZPaintBox *pbWords;
    TComboBox *cbDictionary;
    TPanel *pUC;
    TLabel *lbUC;
    TStatusBar *sbInfo;
    TButton *btnBack;
    TLabel *lbOrig;
    TBevel *bv1;
    TBevel *bv2;
    TBevel *bv3;
    TBevel *bv4;
    TBevel *bv5;
    TBevel *bv7;
    TZPaintBox *pbKanji;
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall btnOkClick(TObject *Sender);
    void __fastcall edMeaningChange(TObject *Sender);
    void __fastcall btnNextClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall cbDictionaryChange(TObject *Sender);
    void __fastcall pbWordsEnter(TObject *Sender);
    void __fastcall pbWordsExit(TObject *Sender);
    void __fastcall pbWordsKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall pbWordsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall edMeaningKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall btnBackClick(TObject *Sender);
    void __fastcall pbWordsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pbWordsMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
private: //User declarations
    wchar_t ch;
    TKanjiList *list;
    int ix;
    byte coll;
    bool changed;

    int kdef;
    TIntegerList *history;

    bool info;
    byte oldcoll;
    TKanji *oldkanji;

    TWordListObj *wobj;
    TKanjiDrawObj *kobj;

    TZMenuRows *menurows;
    TZMenuRows *wordmenurows;

    TKanji *popkanji;
    UnicodeString poptext;
    void popupcallback(TZMenuRows *rows, int col, int row);

    void appActivate(void *caller);
    void appDeactivate(void *caller);

    void wcallback();

    void SetCaptions();
    void SetDef();
    void SetChanged();

    void WMNCActivate(TMessage &msg);
public: //User declarations
/*BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_NCACTIVATE,TMessage,WMNCActivate);
END_MESSAGE_MAP(TBaseForm)*/

    __fastcall TfDef(TComponent* Owner);
    virtual __fastcall ~TfDef();

    int ShowModalEx(byte collection, TKanjiList *klist, int index);
};
//---------------------------------------------------------------------------
extern PACKAGE TfDef *fDef;
//---------------------------------------------------------------------------
#endif
