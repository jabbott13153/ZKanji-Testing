//---------------------------------------------------------------------------

#ifndef kanjiformH
#define kanjiformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include "ZPaintBox.h"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"

class TKanjiGrid;
class TKanjiList;
class TKanjiItemList;

class TfKToGroup : public TBaseForm
{
__published: //IDE-managed Components
    TZPaintBox *pbGrid;
    TSpeedButton *btnAdd;
    TSpeedButton *btnDel;
    TPanel *pBottom;
    TBitBtn *btnOk;
    TBitBtn *btnCancel;
    TBevel *b0;
    TBevel *b1;
    TBevel *b2;
    TLabel *Label1;
    TComboBox *cbDictionary;
    TLabel *Label2;
    TComboBox *cbGroup;
    TBevel *Bevel1;
    TBevel *Bevel2;
    void __fastcall cbDictionaryChange(TObject *Sender);
    void __fastcall cbGroupChange(TObject *Sender);
    void __fastcall btnAddClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall btnDelMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pbGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
private: //User declarations
    TKanjiGrid *kobj;
    TKanjiItemList *list; //the original kanji list
    TKanjiItemList *blist; //kanji currently in kobj;
    int cellsize;
public: //User declarations
    __fastcall TfKToGroup(TComponent* Owner);
    virtual __fastcall ~TfKToGroup();

    void UpdateButtons();

    int ShowModalEx(TKanjiItemList *alist);
};
//---------------------------------------------------------------------------
extern PACKAGE TfKToGroup *fKToGroup;
//---------------------------------------------------------------------------
#endif
