//---------------------------------------------------------------------------

#ifndef wordgenH
#define wordgenH
//---------------------------------------------------------------------------
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include "edit2.h"

class TKanjiList;
class TWordListObj;
class TWordIndexList;
class TKanjiGrid;

class TfGen : public TBaseForm
{
__published: //IDE-managed Components
    TBitBtn *btnGen;
    TBitBtn *btnClose;
    TZPaintBox *pbWords;
    TBitBtn *btnAdd;
    TCheckBox *cbSave;
    TComboBox *cbSrc;
    TSpeedButton *sbLink;
    TComboBox *cbDest;
    TLabel *Label2;
    TCheckBox *cbOnly;
    TComboBox *cbGroups;
    TLabel *Label9;
    TComboBox *cbReading;
    TLabel *Label5;
    TComboBox *cbPos;
    TLabel *Label8;
    TComboBox *cbNum;
    TComboBox *cbGrad;
    TLabel *Label7;
    TLabel *Label6;
    TLabel *Label4;
    TComboBox *cbLen;
    TEdit2 *edPop;
    TLabel *Label12;
    TLabel *Label3;
    TZPaintBox *pbGrid;
    TBevel *b6;
    TBevel *b7;
    TBevel *b2;
    TLabel *lbCount;
    TLabel *lbPos;
    TLabel *Label10;
    TBevel *b0;
    TBevel *b1;
    TBevel *b4;
    TBevel *b5;
    TBevel *b3;
    TLabel *Label11;
    TLabel *Label13;
    void __fastcall cbSrcChange(TObject *Sender);
    void __fastcall sbLinkClick(TObject *Sender);
    void __fastcall cbDestChange(TObject *Sender);
    void __fastcall edPopChange(TObject *Sender);
    void __fastcall edPopExit(TObject *Sender);
    void __fastcall cbGroupsChange(TObject *Sender);
    void __fastcall btnGenClick(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall cbReadingDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
    void __fastcall cbReadingChange(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
private: //User declarations
    typedef TBaseForm inherited;

    int findex; //index of selected kanji
    int *reading;

    TKanjiList *klist;
    TKanjiGrid *kobj;

    TWordListObj *wobj;
    TWordIndexList *fbase;

    void wcallback();
    void kcallback();
    void SyncDest();

    void EnableButtons();
    void set_index(int val);

public: //User declarations
    __fastcall TfGen(TComponent* Owner);
    virtual __fastcall ~TfGen();

    int ShowModalEx(TKanjiList *akanjis);

    __property TWordIndexList *base = { read = fbase };
    __property int index = { read = findex, write = set_index };
};
//---------------------------------------------------------------------------
extern PACKAGE TfGen *fGen;
//---------------------------------------------------------------------------
#endif
