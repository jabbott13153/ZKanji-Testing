//---------------------------------------------------------------------------

#ifndef vocabformH
#define vocabformH
//---------------------------------------------------------------------------
#include "ZPaintBox.h"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include "edit2.h"

class TWordList;
class TWordListObj;
class TWordCollection;
class TZMenuRows;

class TfAddVocab : public TBaseForm
{
__published:
    TComboBox *cbGroups;
    TLabel *Label1;
    TEdit2 *edMeaning;
    TButton *btnOk;
    TButton *btnCancel;
    TBevel *b0;
    TLabel *Label3;
    TComboBox *cbMeaning;
    TBevel *b3;
    TLabel *Label4;
    TLabel *Label5;
    TBevel *b1;
    TComboBox *cbCollection;
    TLabel *Label7;
    TBevel *b2;
    TZPaintBox *pbMeanings;
    TLabel *lbWarning;
    TBevel *b4;
    TCheckBox *cbDefinition;
    TLabel *lbInfo;
    TBevel *Bevel2;
    TStatusBar *sbInfo;
    TPaintBox *pbKanji;
    TPaintBox *pbKana;
    TBevel *b5;
    TLabel *lbMeaning;
    void __fastcall pbKanjiPaint(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall cbGroupsChange(TObject *Sender);
    void __fastcall cbCollectionChange(TObject *Sender);
    void __fastcall cbDefinitionClick(TObject *Sender);
    void __fastcall cbMeaningChange(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall pbMeaningsMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pbMeaningsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall edMeaningExit(TObject *Sender);
private:
    TStatusPanel *p1, *p2, *pgroups;

    int findex;
    byte fmeaning;
    TWordList *words; // = collection->words
    TWordListObj *wobj;

    TZMenuRows *menurows;

    int srcindex;
    int editword;
    int editmeaning;
    int editcoll;

    UnicodeString poptext;

    void change_buttons(bool change_dictionary = false);
    void ShowHidePanels(bool show);

    int get_collectionindex();
    int get_groupindex();
    void wcallback();

    void WMNCHitTest(TWMNCHitTest& msg);

    void popupcallback(TZMenuRows *rows, int col, int row);
public: //User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_NCHITTEST, TWMNCHitTest, WMNCHitTest)
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfAddVocab(TComponent* Owner);
    __fastcall virtual ~TfAddVocab();
    int ShowModalEx(TWordCollection *collection, int aindex);

    __property int CollectionIndex = { read = get_collectionindex };
    __property int GroupIndex = { read = get_groupindex };
    __property int index = { read = findex };
    __property byte meaning = { read = fmeaning };
};
//---------------------------------------------------------------------------
extern PACKAGE TfAddVocab *fAddVocab;
//---------------------------------------------------------------------------
#endif
