//---------------------------------------------------------------------------

#ifndef redefineH
#define redefineH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include "ZPaintBox.h"
//---------------------------------------------------------------------------

#include "baseform.h"
#include "edit2.h"

class TWordListObj;
class TWordCollection;

class TfRedefine : public TBaseForm
{
__published: //IDE-managed Components
    TZPaintBox *pbWords;
    TLabel *Label1;
    TBevel *bv1;
    TEdit2 *edMeaning;
    TLabel *Label2;
    TPanel *pBoxes;
    TCheckBox *cb1;
    TCheckBox *cb2;
    TCheckBox *cb3;
    TCheckBox *cb4;
    TCheckBox *cb8;
    TCheckBox *cb5;
    TCheckBox *cb6;
    TCheckBox *cb7;
    TCheckBox *cb12;
    TCheckBox *cb11;
    TCheckBox *cb10;
    TCheckBox *cb9;
    TCheckBox *cb16;
    TCheckBox *cb15;
    TCheckBox *cb14;
    TCheckBox *cb13;
    TCheckBox *cb20;
    TCheckBox *cb19;
    TCheckBox *cb18;
    TCheckBox *cb17;
    TCheckBox *cb23;
    TCheckBox *cb22;
    TCheckBox *cb21;
    TBevel *bvH;
    TCheckBox *cb24;
    TCheckBox *cb25;
    TCheckBox *cb26;
    TCheckBox *cb27;
    TCheckBox *cb28;
    TCheckBox *cb29;
    TCheckBox *cb30;
    TCheckBox *cb31;
    TCheckBox *cb32;
    TCheckBox *cb33;
    TCheckBox *cb34;
    TCheckBox *cb35;
    TCheckBox *cb36;
    TCheckBox *cb37;
    TCheckBox *cb38;
    TCheckBox *cb39;
    TCheckBox *cb40;
    TCheckBox *cb41;
    TCheckBox *cb42;
    TCheckBox *cb43;
    TCheckBox *cb44;
    TBevel *bvV;
    TCheckBox *cb45;
    TCheckBox *cb46;
    TCheckBox *cb47;
    TCheckBox *cb48;
    TCheckBox *cb49;
    TCheckBox *cb50;
    TCheckBox *cb51;
    TCheckBox *cb52;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TBevel *bv2;
    TSpeedButton *btnDelete;
    TBevel *bv3;
    TSpeedButton *btnNew;
    TSpeedButton *btnSave;
    TBitBtn *BitBtn1;
    TLabel *Label6;
    TEdit2 *edFreq;
    TLabel *lbCount;
    TLabel *lbPos;
    TBevel *Bevel1;
    TBevel *bv0;
    TSpeedButton *btnUp;
    TSpeedButton *btnDown;
    TBevel *bv4;
    void __fastcall edMeaningChange(TObject *Sender);
    void __fastcall cb1Click(TObject *Sender);
    void __fastcall btnSaveClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall btnDeleteClick(TObject *Sender);
    void __fastcall btnNewClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall btnUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
private: //User declarations
    TWordListObj *wobj;
    TNumEdit *numFreq;
    void wcallback();
    void ncallback();

    void set_modified(bool val);
    int savequery(bool cancelbutton);

    TWordCollection *fcollection; //container of word
    int findex; //index of word

    int fmeaning; //currently selected (and probably changed) meaning

    int typestart;
    int notestart;
    int fieldstart;

    void fill_definitions();

    bool delword;
    bool fmodified;
public: //User declarations
    __fastcall TfRedefine(TComponent* Owner);
    virtual __fastcall ~TfRedefine();

    int ShowModalEx(TWordCollection *acollection, int aindex, char ameaning);
    __property bool modified = { read = fmodified, write = set_modified };
};
//---------------------------------------------------------------------------
extern PACKAGE TfRedefine *fRedefine;
//---------------------------------------------------------------------------
#endif
