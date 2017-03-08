//---------------------------------------------------------------------------

#ifndef exportformH
#define exportformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "baseform.h"
#include <CheckLst.hpp>
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <Dialogs.hpp>

class TVocabListObj;
class TWordListObj;
class TKanjiCardGrid;
class TWordCollection;
class TfExport : public TBaseForm
{
__published: //IDE-managed Components
    TPageControl *pTabs;
    TTabSheet *tabIntro;
    TTabSheet *tabWord;
    TTabSheet *tabKanji;
    TBevel *bvBottom;
    TPanel *Panel1;
    TButton *btnNext;
    TButton *btnCancel;
    TPanel *pSections;
    TBevel *Bevel1;
    TBevel *Bevel2;
    TTabSheet *tabStudy;
    TButton *btnBack;
    TLabel *Label1;
    TLabel *Label3;
    TBevel *Bevel3;
    TCheckListBox *clbWG;
    TZPaintBox *pbWG;
    TLabel *Label6;
    TSpeedButton *btnWChkAll;
    TSpeedButton *btnWChkNone;
    TLabel *Label7;
    TLabel *Label8;
    TLabel *Label9;
    TBevel *Bevel4;
    TZPaintBox *pbKG;
    TLabel *Label11;
    TPanel *pKW;
    TZPaintBox *pbKEx;
    TSpeedButton *btnSCheckAll;
    TLabel *Label12;
    TLabel *Label13;
    TBevel *Bevel5;
    TLabel *Label14;
    TSpeedButton *btnSCheckNone;
    TCheckListBox *clbSG;
    TZPaintBox *ZPaintBox1;
    TLabel *Label15;
    TLabel *Label16;
    TPanel *Panel2;
    TLabel *Label2;
    TCheckBox *cbWords;
    TPanel *Panel3;
    TCheckBox *cbKanji;
    TLabel *Label4;
    TPanel *Panel4;
    TCheckBox *cbStudy;
    TLabel *Label5;
    TLabel *Label17;
    TPanel *pKG;
    TLabel *Label10;
    TCheckListBox *clbKG;
    TCheckBox *cbKEx;
    TCheckBox *cbKMeaning;
    TSpeedButton *btnKChkAllClick;
    TSpeedButton *btnKChkNoneClick;
    TSaveDialog *dlgExport;
    TCheckBox *cbGroupMeaning;
    TCheckBox *cbKUser;
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall cbWordsClick(TObject *Sender);
    void __fastcall btnNextClick(TObject *Sender);
    void __fastcall btnBackClick(TObject *Sender);
    void __fastcall clbWGClick(TObject *Sender);
    void __fastcall clbWGClickCheck(TObject *Sender);
    void __fastcall btnWChkAllClick(TObject *Sender);
    void __fastcall btnWChkNoneClick(TObject *Sender);
    void __fastcall btnKChkAllClickClick(TObject *Sender);
    void __fastcall btnKChkNoneClickClick(TObject *Sender);
    void __fastcall clbKGClick(TObject *Sender);
    void __fastcall cbKExClick(TObject *Sender);
    void __fastcall clbSGClick(TObject *Sender);
    void __fastcall clbSGClickCheck(TObject *Sender);
    void __fastcall tabKanjiMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
    void __fastcall tabKanjiMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall tabKanjiMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
private: //User declarations
    void UpdateButtons();
    TVocabListObj *wobj;
    TKanjiCardGrid *kobj;
    TWordListObj *kwobj;
    TWordCollection *coll;

    double sizer;
    bool sizing;
    int sizegap;
    int rightgap;
    int grabpos;

    void kcallback();

    void SizeKanjiPanels(int h);

    void Export(UnicodeString fname);
public: //User declarations
    __fastcall TfExport(TComponent* Owner);
    virtual __fastcall ~TfExport();

    int ShowModalEx(TWordCollection *coll);
};
//---------------------------------------------------------------------------
extern PACKAGE TfExport *fExport;
//---------------------------------------------------------------------------
#endif
