//---------------------------------------------------------------------------

#ifndef kanjidefimportH
#define kanjidefimportH
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

class TKanjiGrid;
class TVirtualWordListObj;
class TImportList;
enum TListVirtualCheckControlWantedValue : byte;
enum TWordColumnType;

class TfKanjiDefImport : public TBaseForm
{
__published: // IDE-managed Components
    TZPaintBox *pbKanji;
    TBitBtn *btnNext;
    TButton *btnAbort;
    TBevel *bvBottom;
    TZPaintBox *pbDef;
    TBevel *b1;
    TLabel *Label1;
    TLabel *Label2;
    void __fastcall pbDefVScroll(TObject *Sender, TScrollCode ScrollCode, int &ScrollPos);
    void __fastcall pbKanjiPaint(TObject *Sender);
    void __fastcall btnNextClick(TObject *Sender);
    void __fastcall btnAbortClick(TObject *Sender);
    void __fastcall pbDefResize(TObject *Sender);
    void __fastcall pbKanjiEnter(TObject *Sender);
    void __fastcall pbDefKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall pbDefEnter(TObject *Sender);
    void __fastcall pbDefExit(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);

private: // User declarations
    TVirtualWordListObj *dobj;
    TImportList *list;

    TIntegerList *kanjilist; // List containing the indexes of kanji that has a definition to be imported. Contains -1-index for indexes where the old index will be kept.
    int returns0(unsigned int ix); // Dummy function for returning 0 for each line in the virtual word list.

    void checkchange(unsigned int row, byte index, bool &checked);
    bool getcheck(unsigned int row, byte index, TListVirtualCheckControlWantedValue wanted);

    UnicodeString getheader(int col);
    void drawcell(int col, int row, TCanvas *c, const TRect &r);
    void preparecell(int col, int row, TWordColumnType ctype, TCanvas *c);
    void dcallback();
public: // User declarations
    __fastcall TfKanjiDefImport(TComponent* Owner);
    virtual __fastcall ~TfKanjiDefImport();

    int ShowModalEx(TImportList *list);
};
//---------------------------------------------------------------------------
extern PACKAGE TfKanjiDefImport *fKanjiDefImport;
//---------------------------------------------------------------------------
#endif

