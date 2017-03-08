//---------------------------------------------------------------------------

#ifndef wordreviewH
#define wordreviewH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include "ZPaintBox.h"
//---------------------------------------------------------------------------

#include "baseform.h"

class TWordIndexList;
class TWordStudyStateList;
class TWordGroupStudy;
struct TWordStatGlobal;
typedef TTemplateList<TWordStatGlobal, false> TWordStatGlobalList;

class TfWReview : public TBaseForm
{
__published: // IDE-managed Components
    TZPaintBox *pbList;
    TStatusBar *sbInfo;
    TBevel *b0;
    TLabel *Label1;
    TImageList *ilCheck;
    TSpeedButton *btnAll;
    TBevel *Bevel3;
    TSpeedButton *btnRemove;
    TButton *btnGenerate;
    TBevel *Bevel5;
    TComboBox *cbGroup;
    TLabel *Label2;
    TSpeedButton *btnRandom10;
    TSpeedButton *btnRandom20;
    TSpeedButton *btnRandom30;
    TSpeedButton *btnForward;
    TSpeedButton *btnBackward;
    TSpeedButton *btnCompact;
    TButton *btnContinue;
    TLabel *Label6;
    TBevel *b1;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall pbListResize(TObject *Sender);
    void __fastcall pbListPaint(TObject *Sender);
    void __fastcall pbListMouseLeave(TObject *Sender);
    void __fastcall pbListMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall pbListMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pbListKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbListMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y);
    void __fastcall pbListEnter(TObject *Sender);
    void __fastcall btnAllClick(TObject *Sender);
    void __fastcall btnRemoveClick(TObject *Sender);
    void __fastcall btnGenerateClick(TObject *Sender);
    void __fastcall cbGroupChange(TObject *Sender);
    void __fastcall btnRandom10Click(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall btnBackwardClick(TObject *Sender);
    void __fastcall btnCompactClick(TObject *Sender);
    void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall pbListKeyPress(TObject *Sender, wchar_t &Key);
private:
    void __fastcall WMSizing(TWMSizing &msg);
    void __fastcall count_sizes();

    bool canalter;
    bool canremove;

    TWordIndexList *flist;
    TWordStudyStateList *fstats;
    TWordStatGlobalList *fglobals;

    int frowsize; // Size of a whole row, 3 times the line's size.
    int flinesize; // Size of one line drawn (eg. the meaning's line).
    int fromajisize;
    int fcompactsize;
    int fnumbersize;
    TWordGroupStudy *fgroup;

    // Attributes of the fake control created with a paintbox.
    int fsel;
    int fmouserow;
    int foncheck; // 0: none, 1: X, 2: minus
    int fmousecheck; // The checkbox the mouse button is down on.
    bool oldminusdisabled; // The minus checkboxes were disabled the last time something was checked.

    int prob;
    int wrong;
    int skip;

    int fround;
    int ffnotw;
    int flw;

    void set_selected(int ix);
    void UpdateScore();
    void Check(bool key);
    void InvalidateRow(int ix);
    int realrow(int ix);
    int mouserow(int y);

protected:
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_SIZING, TWMSizing, WMSizing)
    END_MESSAGE_MAP(TBaseForm)

    void __fastcall DrawCheck(int ix);

    __property int selected = { read = fsel, write = set_selected };
public:
    __fastcall TfWReview(TComponent* Owner);

    int ShowModalEx(TWordGroupStudy *agroup, TWordIndexList *alist, TWordStudyStateList *astats, TWordStatGlobalList *aglobals, int afirstnotwrong, int alastwrong, int around, bool alter, bool remove, int random);
};
//---------------------------------------------------------------------------
extern PACKAGE TfWReview *fWReview;
//---------------------------------------------------------------------------
#endif
