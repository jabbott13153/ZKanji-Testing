//---------------------------------------------------------------------------

#ifndef studygroupH
#define studygroupH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include "edit2.h"

class TWordCollection;
class TWordGroupStudy;
class TWordIndexList;

enum TStudyMethodType : byte;
enum TStudyQuestionType;

class TfWordStudy : public TBaseForm
{
__published: //IDE-managed Components
    TButton *btnCancel;
    TButton *btnStudy;
    TComboBox *cbMethod;
    TLabel *lbChanged;
    TButton *btnSetup;
    TEdit2 *edName;
    TLabel *Label14;
    TLabel *Label16;
    TBevel *b1;
    TSpeedButton *btnWordList;
    TBevel *b6;
    TLabel *Label4;
    TComboBox *cbQuestion;
    TBevel *b3;
    TLabel *Label5;
    TComboBox *cbInputMode;
    TComboBox *cbTimer;
    TCheckBox *cbHideKana;
    TPanel *pRefresh;
    TLabel *Label10;
    TLabel *Label11;
    TSpeedButton *sbM2up;
    TSpeedButton *sbM2down;
    TEdit2 *edM2Num;
    TListBox *lbM2Inc;
    TPanel *pGradual;
    TLabel *Label3;
    TLabel *Label6;
    TLabel *Label9;
    TLabel *Label13;
    TComboBox *cbM1Order;
    TComboBox *cbM1Num;
    TCheckBox *cbM1Rand;
    TComboBox *cbM1Add;
    TComboBox *cbM1Selection;
    TCheckBox *cbM1Global;
    TLabel *Label7;
    TBevel *b4;
    TBevel *b2;
    TBevel *b5;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label8;
    void __fastcall cbMethodChange(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall lbM2IncDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
    void __fastcall lbM2IncClick(TObject *Sender);
    void __fastcall btnWordListClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall edNameExit(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall cbQuestionChange(TObject *Sender);
    void __fastcall btnStudyClick(TObject *Sender);
    void __fastcall cbInputModeChange(TObject *Sender);
    void __fastcall sbM2upMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall sbM2downMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
private:
    TStudyMethodType fmethod_ori; // Used only for ShowModalEx2 when the group is reset.

    int oldcq;

    TWordIndexList *l, *tl; // Temporary index list of words to be included in the study group.
    UnicodeString fgroupname;

    TNumEdit *numWords;

    TWordCollection *fcollection;

    void __fastcall update_caption();
public:
    TWordGroupStudy* __fastcall ShowModalEx(TWordCollection *acollection, TWordIndexList *al, UnicodeString aname); // Create new study list.
    TWordGroupStudy* __fastcall ShowModalEx2(TWordGroupStudy *agroup); // Modify existing study list.

    __fastcall TfWordStudy(TComponent* Owner);
    virtual __fastcall ~TfWordStudy();

    __property UnicodeString groupname = { read = fgroupname };
};
//---------------------------------------------------------------------------
extern PACKAGE TfWordStudy *fWordStudy;
//---------------------------------------------------------------------------
#endif
