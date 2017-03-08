//---------------------------------------------------------------------------

#ifndef importexsubstH
#define importexsubstH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "edit2.h"
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "baseform.h"

class TCallbackWordListObj;
class TKanjiEdit;
class TWordListObj;
enum TWordNumProcWanted : byte;
enum TWordStringProcWanted : byte;
class TImportList;
class TExportedKanjiExample;
class TfExampleSub : public TBaseForm
{
__published: // IDE-managed Components
    TLabel *Label1;
    TBevel *b1;
    TLabel *Label2;
    TZPaintBox *wbox;
    TPanel *Panel1;
    TLabel *lbOrig1;
    TBevel *b3;
    TPanel *pOrig;
    TBevel *Bevel3;
    TSpeedButton *btnRomaji;
    TSpeedButton *btnKanji;
    TLabel *Label6;
    TPanel *pRomaji;
    TEdit2 *edRomaji;
    TZPaintBox *pbDict;
    TPanel *pKanji;
    TSpeedButton *sbRecognize;
    TZPaintBox *pbKanji;
    TBitBtn *btnNext;
    TButton *btnAbort;
    TBevel *bvBottom;
    TLabel *Label3;
    TButton *btnSelect;
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall btnNextClick(TObject *Sender);
    void __fastcall btnAbortClick(TObject *Sender);
    void __fastcall btnKanjiClick(TObject *Sender);
    void __fastcall edRomajiChange(TObject *Sender);
    void __fastcall btnSelectClick(TObject *Sender);
    void __fastcall pbKanjiKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall pbKanjiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbKanjiEnter(TObject *Sender);




private: // User declarations
    TCallbackWordListObj *wobj;
    TKanjiEdit *edkanji;
    TWordListObj *edobj;

    TImportList *list;
    int cix; // Index of collection.

    struct TExampleWordData
    {
        int kindex; // Index of kanji in list.
        TExportedKanjiExample *example; // Example of kanji missing from dictionary.
        TExampleWordData(int kindex, TExportedKanjiExample *example) : kindex(kindex), example(example) {}
    };
    TTemplateList<TExampleWordData, true> words;

    void wcallback();
    void kcallback();
    void edcallback();

    word wnumproc(unsigned int ix, TWordNumProcWanted wanted);
    UnicodeString wstrproc(unsigned int ix, byte m, TWordStringProcWanted wanted);
    void wtagproc(unsigned int i, byte m, int &types, int &notes, byte &fields, word &nametag);
    void wcheck(unsigned int row, byte index, bool &checked);
    void wdraw(int col, int row, TCanvas *c, const TRect &r);

    void UpdateWords();
public: // User declarations
    __fastcall TfExampleSub(TComponent* Owner);
    virtual __fastcall ~TfExampleSub();

    int ShowModalEx(TImportList *alist);
};
//---------------------------------------------------------------------------
extern PACKAGE TfExampleSub *fExampleSub;
//---------------------------------------------------------------------------
#endif
