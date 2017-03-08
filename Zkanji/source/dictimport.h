//---------------------------------------------------------------------------

#ifndef dictimportH
#define dictimportH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "baseform.h"
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "edit2.h"
#include <ComCtrls.hpp>

class TWordCollection;
class TKanjiEdit;
class TWordListObj;
class TImportList;
struct TImportOptions;
class TCallbackWordListObj;
class TVocabListObj;
class TImportedWord;
class TReplacedWord;
class TExportedWord;
class TWordIndexList;
enum TWordNumProcWanted : byte;
enum TWordStringProcWanted : byte;
enum TListVirtualCheckControlWantedValue : byte;
enum TWordColumnType;
class TfDictImport : public TBaseForm
{
__published: // IDE-managed Components
    TLabel *Label1;
    TBevel *b1;
    TZPaintBox *wbox;
    TBitBtn *btnNext;
    TButton *btnAbort;
    TBevel *bvBottom;
    TLabel *Label2;
    TBevel *b3;
    TPageControl *pcSelector;
    TTabSheet *tabWords;
    TLabel *lbOrig1;
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
    TTabSheet *tabMeanings;
    TPanel *pIMeanings;
    TZPaintBox *ibox;
    TLabel *Label8;
    TLabel *Label4;
    TPanel *pDMeanings;
    TLabel *Label3;
    TCheckBox *cbDelUnmatched;
    TZPaintBox *dbox;
    TLabel *Label5;
    TButton *btnWordSel;
    TLabel *Label7;
    TLabel *Label9;
    TLabel *lbOrig2;
    TLabel *lbOrig3;
    TButton *btnSelect;
    void __fastcall btnAbortClick(TObject *Sender);
    void __fastcall btnKanjiClick(TObject *Sender);
    void __fastcall btnSelectClick(TObject *Sender);
    void __fastcall edRomajiChange(TObject *Sender);
    void __fastcall btnWordSelClick(TObject *Sender);
    void __fastcall cbDelUnmatchedClick(TObject *Sender);
    void __fastcall btnNextClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall edRomajiEnter(TObject *Sender);
    void __fastcall pbKanjiKeyPress(TObject *Sender, wchar_t &Key);

private: // User declarations
    typedef TBaseForm   inherited;

    bool groupimport;

    TImportList *list;
    TImportOptions *options;

    TCallbackWordListObj *wobj; // Word object showing words to import.
    TCallbackWordListObj *iobj; // Word object showing imported meanings.
    TVocabListObj *dobj; // Word object showing existing meanings.

    TIntegerList words;
    //TTemplateList<TImportedWord, false> words;

    TWordIndexList *ddefs; // List of meanings of a word from the current dictionary.

    TKanjiEdit *edkanji;
    TWordListObj *edobj;

    TGapList<int> wordstaken; // List of word indexes used by import entries, to avoid duplicates when selecting a new entry for words.

    bool dupdating; // Set to avoid action in dcallback while it is populated or its selection changed.

    void ecallback();
    void edcallback();
    void UpdateWords();

    word wnumproc(unsigned int ix, TWordNumProcWanted wanted);
    UnicodeString wstrproc(unsigned int ix, byte m, TWordStringProcWanted wanted);
    void wtagproc(unsigned int i, byte m, int &types, int &notes, byte &fields, word &nametag);
    void wcheck(unsigned int row, byte index, bool &checked);
    bool wgetcheck(unsigned int row, byte index, TListVirtualCheckControlWantedValue wanted);
    void wcallback();

    word inumproc(unsigned int ix, TWordNumProcWanted wanted);
    UnicodeString istrproc(unsigned int ix, byte m, TWordStringProcWanted wanted);
    void itagproc(unsigned int i, byte m, int &types, int &notes, byte &fields, word &nametag);
    void icheck(unsigned int row, byte index, bool &checked);
    bool igetcheck(unsigned int row, byte index, TListVirtualCheckControlWantedValue wanted);
    void icallback();

    void dcallback();

    int imeaningcnt(int pos);
    int dmeaningcnt(int meaning);

    void idraw(int col, int row, TCanvas *c, const TRect &r);
    void dprep(int col, int row, TWordColumnType ctype, TCanvas *c);
    void ddraw(int col, int row, TCanvas *c, const TRect &r);

    TImportedWord* get_iword(int ix);
    TReplacedWord* get_rword(int ix);
    TExportedWord* get_eword(int ix);

    int getecnt();
    __property int edefcnt = { read = getecnt }; // Number of definitions listed for the current word.

    __property TImportedWord* iwords[int] = { read = get_iword };
    __property TReplacedWord* rwords[int] = { read = get_rword };
    __property TExportedWord* ewords[int] = { read = get_eword };
public: // User declarations
    __fastcall TfDictImport(TComponent* Owner);
    virtual __fastcall ~TfDictImport();

    int ShowModalEx(TImportList *list, TImportOptions *options, bool group);
};
//---------------------------------------------------------------------------
extern PACKAGE TfDictImport *fDictImport;
//---------------------------------------------------------------------------
#endif
