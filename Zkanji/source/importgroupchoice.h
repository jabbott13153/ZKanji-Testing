//---------------------------------------------------------------------------

#ifndef importgroupchoiceH
#define importgroupchoiceH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <CheckLst.hpp>
//---------------------------------------------------------------------------
#include "baseform.h"
#include <ExtCtrls.hpp>

class TImportList;
class TWordCollection;
class TVocabListObj;
class TCallbackWordListObj;
class TImportWordGroup;
class TImportKanjiGroup;
class TKanjiGrid;
enum TWordNumProcWanted : byte;
enum TWordStringProcWanted : byte;

class TfImportGroupChoice : public TBaseForm
{
__published: // IDE-managed Components
    TBevel *bvBottom;
    TPanel *pTop;
    TPanel *pBottom;
    TPanel *pButtons;
    TZPaintBox *pbOld;
    TZPaintBox *pbNew;
    TButton *btnOk;
    TButton *btnAbort;
    TShape *sSizer;
    TBevel *Bevel1;
    TPanel *pAction;
    TBevel *Bevel3;
    TBevel *Bevel2;
    TSpeedButton *btnRename;
    TSpeedButton *btnChkAll;
    TSpeedButton *btnChkNone;
    TPanel *pTopTitle;
    TLabel *Label1;
    TBevel *bvTopLeft;
    TListBox *cbOld;
    TCheckListBox *cbNew;
    TPanel *pBottomTitle;
    TLabel *Label2;
    TBevel *Bevel4;
    TLabel *Label3;
    TLabel *Label4;
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall sSizerMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sSizerMouseEnter(TObject *Sender);
    void __fastcall sSizerMouseLeave(TObject *Sender);
    void __fastcall sSizerMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall sSizerMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnAbortClick(TObject *Sender);
    void __fastcall btnOkClick(TObject *Sender);
    void __fastcall btnChkAllClick(TObject *Sender);
    void __fastcall btnChkNoneClick(TObject *Sender);
    void __fastcall cbNewClickCheck(TObject *Sender);
    void __fastcall btnRenameClick(TObject *Sender);
    void __fastcall cbOldClick(TObject *Sender);
    void __fastcall cbNewClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private: // User declarations
    double psize;

    bool wordgrp; // True for word group and false for kanji group editing.
    TImportList *list;
    TWordCollection *coll;

    TVocabListObj *wold;
    TCallbackWordListObj *wnew;

    TKanjiGrid *kold;
    TKanjiGrid *knew;

    bool sizing; // Sizing with the splitter.
    int sizepos; // Position of mouse in the splitter.
    double splitsize; // Current ratio of the top panel compared to the whole panel height.

    void SetSplitSize(double siz);
    void UpdateButtons();

    word wnumproc(unsigned int ix, TWordNumProcWanted wanted);
    UnicodeString wstrproc(unsigned int ix, byte m, TWordStringProcWanted wanted);
    void wtagproc(unsigned int i, byte m, int &types, int &notes, byte &fields, word &nametag);

    TImportWordGroup* get_wgroup();
    TImportKanjiGroup* get_kgroup();

    __property TImportWordGroup* wordgroup = { read = get_wgroup };
    __property TImportKanjiGroup* kanjigroup = { read = get_kgroup };
public: // User declarations
    __fastcall TfImportGroupChoice(TComponent* Owner);
    virtual __fastcall ~TfImportGroupChoice();
    int ShowModalEx(TWordCollection *acoll, TImportList *alist, bool awordgroups);
};
//---------------------------------------------------------------------------
extern PACKAGE TfImportGroupChoice *fImportGroupChoice;
//---------------------------------------------------------------------------
#endif
