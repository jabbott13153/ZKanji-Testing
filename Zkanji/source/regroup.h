//---------------------------------------------------------------------------

#ifndef regroupH
#define regroupH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"

class TVocabListObj;
class TWordCollection;
class TWordIndexList;
struct TWordIndex;

class TfRegroup : public TBaseForm
{
__published: //IDE-managed Components
    TBevel *Bevel3;
    TZPaintBox *pbWords;
    TPanel *pToolbar;
    TShape *sSplit;
    TPanel *pButtons2;
    TLabel *Label3;
    TSpeedButton *btnRemove2;
    TBitBtn *btnDuplicates;
    TComboBox *cbType;
    TPanel *pButtons;
    TLabel *Label1;
    TSpeedButton *btnAdd;
    TLabel *Label2;
    TSpeedButton *btnRemove;
    TSpeedButton *btnUp;
    TSpeedButton *btnDown;
    TComboBox *cbGroups;
    TBitBtn *btnCheck;
    TBitBtn *btnOk;
    TBitBtn *btnCancel;
    TZPaintBox *pbGWords;
    TBevel *Bevel2;
    TPanel *pGroupStat;
    TLabel *lbGroup;
    TLabel *lbGroupCount;
    TLabel *lbGroupPos;
    TPanel *pStudyStat;
    TLabel *lbStudy;
    TLabel *lbStudyCount;
    TLabel *lbStudyPos;
    TBevel *Bevel1;
    TBevel *Bevel4;
    void __fastcall cbGroupsChange(TObject *Sender);
    void __fastcall btnAddClick(TObject *Sender);
    void __fastcall btnRemoveClick(TObject *Sender);
    void __fastcall btnCheckClick(TObject *Sender);
    void __fastcall cbTypeChange(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall btnRemove2Click(TObject *Sender);
    void __fastcall btnUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnDownMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pbWordsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbGWordsKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall pButtons2MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pButtons2MouseEnter(TObject *Sender);
    void __fastcall pButtons2MouseLeave(TObject *Sender);
    void __fastcall pButtons2MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall pButtons2MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall FormResize(TObject *Sender);
private: //User declarations
    TVocabListObj *gobj;
    TVocabListObj *wobj;

    TWordCollection *fcollection;
    TWordIndexList *l;

    TWordIndexList *kanji;
    TWordIndexList *kana;
    TWordIndexList *meaning;
    TTemplateList<TWordIndexList, true> *kanjidat;
    TTemplateList<TWordIndexList, true> *kanadat;
    TTemplateList<TWordIndexList, true> *meaningdat;

    int mousey;
    int mousesize;
    bool mousesizing;

    void gcallback();
    void wcallback();

    void show_panel(int p);
    void delete_lists();
    void construct_lists();

    int wsize;
    void SetSplitSize(int s);

    friend int kanjicmp(TWordIndex **a, TWordIndex **b);
    friend int kanacmp(TWordIndex **a, TWordIndex **b);
    friend int meaningcmp(TWordIndex **a, TWordIndex **b);
public: //User declarations
    __fastcall TfRegroup(TComponent* Owner);
    __fastcall virtual ~TfRegroup();

    int ShowModalEx(TWordIndexList *al);
};
//---------------------------------------------------------------------------
extern PACKAGE TfRegroup *fRegroup;
//---------------------------------------------------------------------------
#endif
