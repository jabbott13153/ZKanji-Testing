//---------------------------------------------------------------------------

#ifndef newwordH
#define newwordH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include "edit2.h"

class TKanjiEdit;

class TfNewWord : public TBaseForm
{
__published: //IDE-managed Components
    TZPaintBox *edReading;
    TBevel *b1;
    TLabel *Label2;
    TBevel *b2;
    TLabel *Label4;
    TButton *btnNew;
    TPanel *pBoxes;
    TBevel *bvH;
    TBevel *bvV;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TCheckBox *cb1;
    TCheckBox *cb2;
    TCheckBox *cb3;
    TCheckBox *cb4;
    TCheckBox *cb5;
    TCheckBox *cb6;
    TCheckBox *cb7;
    TCheckBox *cb8;
    TCheckBox *cb9;
    TCheckBox *cb10;
    TCheckBox *cb11;
    TCheckBox *cb12;
    TCheckBox *cb13;
    TCheckBox *cb14;
    TCheckBox *cb15;
    TCheckBox *cb16;
    TCheckBox *cb17;
    TCheckBox *cb18;
    TCheckBox *cb19;
    TCheckBox *cb20;
    TCheckBox *cb21;
    TCheckBox *cb22;
    TCheckBox *cb23;
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
    TCheckBox *cb45;
    TCheckBox *cb46;
    TCheckBox *cb47;
    TCheckBox *cb48;
    TCheckBox *cb49;
    TCheckBox *cb50;
    TCheckBox *cb51;
    TCheckBox *cb52;
    TEdit2 *edMeaning;
    TLabel *Label8;
    TComboBox *cbFrequency;
    TBitBtn *BitBtn1;
    TZPaintBox *edKanji;
    TSpeedButton *sbRecognize;
    TLabel *Label1;
    TBevel *b0;
    TLabel *Label3;
    TBevel *b3;
    TComboBox *cbDict;
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall edMeaningClick(TObject *Sender);
private: //User declarations
    void edcallback();
    int get_type();
    int get_field();
    word get_nametag();
    int get_note();
    word get_frequency();
    UnicodeString get_kanji();
    UnicodeString get_kana();
    UnicodeString get_meaning();

    int typestart;
    int notestart;
    int fieldstart;

    TKanjiEdit *edreading;
    TKanjiEdit *edkanji;

    byte get_coll();
public: //User declarations
    __fastcall TfNewWord(TComponent* Owner);
    virtual __fastcall ~TfNewWord();

    int ShowModalEx(byte coll);

    __property int type = { read = get_type };
    __property int field = { read = get_field };
    __property word nametag = { read = get_nametag };
    __property int note = { read = get_note };
    __property word frequency = { read = get_frequency };
    __property UnicodeString kanji = { read = get_kanji };
    __property UnicodeString kana = { read = get_kana };
    __property UnicodeString meaning = { read = get_meaning };
    __property byte CollectionIndex = { read = get_coll };
};
//---------------------------------------------------------------------------
extern PACKAGE TfNewWord *fNewWord;
//---------------------------------------------------------------------------
void AddNewWord(byte collection);

#endif
