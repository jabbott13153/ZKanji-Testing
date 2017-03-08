//---------------------------------------------------------------------------

#ifndef listimportH
#define listimportH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include <Buttons.hpp>
//---------------------------------------------------------------------------
#include "baseform.h"
#include <ExtCtrls.hpp>
#include "edit2.h"
#include <ComCtrls.hpp>

class TWordCollection;
class TWordListObj;
class TWordIndexList;
enum TWordColumnType;

struct TAddWordItemTemp
{
    int windex; //word index in collection's word list
    int dindex; //item index of word definition in study list. -1 if not in there
    UnicodeString meaning;
    bool qtype[3]; //is word selected for inclusion 0) kanji 1) kana 2) meaning

    bool onlykana; //this word is usually written with kana only. Uncheck kanji.
    bool nokanji; //this word has no kanji whatsoever. Disable it.
};
typedef TTemplateList<TAddWordItemTemp, false> TAddWordItemTempList;

class TfListImport : public TBaseForm
{
__published: //IDE-managed Components
    TLabel *Label2;
    TBevel *b2;
    TBevel *b1;
    TButton *btnSave;
    TBevel *b3;
    TEdit2 *edMeaning;
    TBevel *b4;
    TButton *btnReset;
    TPanel *Panel3;
    TStatusBar *sbInfo;
    TZPaintBox *pb;
    TLabel *Label1;
    TBitBtn *btnOk;
    TBitBtn *btnCancel;
    TButton *btnUncheckKanaonly;
    TLabel *Label3;
    TButton *sbTestKanji;
    TButton *sbTestKana;
    TButton *sbTestMeaning;
    void __fastcall sbTestKanjiClick(TObject *Sender);
    void __fastcall edMeaningChange(TObject *Sender);
    void __fastcall btnSaveClick(TObject *Sender);
    void __fastcall edMeaningEnter(TObject *Sender);
    void __fastcall edMeaningExit(TObject *Sender);
    void __fastcall edMeaningKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall pbKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall btnResetClick(TObject *Sender);
    void __fastcall btnUncheckKanaonlyClick(TObject *Sender);
    void __fastcall pbKeyPress(TObject *Sender, wchar_t &Key);
private: //User declarations
    TWordListObj *wobj;

    TAddWordItemTempList *words;
    TWordCollection *fcollection;

    UnicodeString meaningproc(int row);
    void prepareproc(int col, int row, TWordColumnType ctype, TCanvas *c);
    void wcallback();

    void checkproc(unsigned int row, byte index, bool &checked);
    void set_statustext();

    int wchk[3]; //count of checked (written/kana/meaning)
    int wenb[3]; //count of enabled

    TAddWordItemTemp* addtemp(int windex);
    void deletetemp(TAddWordItemTemp *tmp);
public: //User declarations
    __fastcall TfListImport(TComponent* Owner);
    virtual __fastcall ~TfListImport();

    int ShowModalEx(TWordIndexList *l);
};
//---------------------------------------------------------------------------
extern PACKAGE TfListImport *fListImport;
//---------------------------------------------------------------------------

int ListImport(TWordIndexList *l);

#endif
