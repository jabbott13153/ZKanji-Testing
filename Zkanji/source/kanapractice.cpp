//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanapractice.h"
#include "kanatest.h"
#include "kanawritingtest.h"
#include "settings.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(66);
#endif

TfKana *fKana;
//---------------------------------------------------------------------------

wchar_t *hints[104] = {
    L"ka", L"ki", L"ku", L"ke", L"ko", L"sa", L"shi", L"su", L"se", L"so",
    L"ta", L"chi", L"tsu", L"te", L"to", L"na", L"ni", L"nu", L"ne", L"no",
    L"ha", L"hi", L"fu", L"he", L"ho", L"ma", L"mi", L"mu", L"me", L"mo",
    L"ya", L"yu", L"yo", L"ra", L"ri", L"ru", L"re", L"ro",
    L"wa", L"wo", L"ga", L"gi", L"gu", L"ge", L"go",
    L"za", L"ji", L"zu", L"ze", L"zo", L"da", L"di", L"du", L"de", L"do",
    L"ba", L"bi", L"bu", L"be", L"bo", L"pa", L"pi", L"pu", L"pe", L"po",
    L"kya", L"kyu", L"kyo", L"sha", L"shu", L"sho", L"cha", L"chu", L"cho",
    L"hya", L"hyu", L"hyo", L"nya", L"nyu", L"nyo", L"gya", L"gyu", L"gyo",
    L"ja", L"ju", L"jo", L"dya", L"dyu", L"dyo", L"bya", L"byu", L"byo",
    L"pya", L"pyu", L"pyo", L"rya", L"ryu", L"ryo",
    L"a", L"i", L"u", L"e", L"o", L"n'"
};

//---------------------------------------------------------------------------
__fastcall TfKana::TfKana(TComponent* Owner) : TBaseForm(Owner)
{
}
//---------------------------------------------------------------------------
extern bool hasrecdata;
int TfKana::ShowModalEx()
{
    int r = mrOk;

    while (r == mrOk || r == mrYes)
    {
        cnt = sizeof(hints) / sizeof(hints[0]);
        kata = settings.bitskata;
        hira = settings.bitshira;
        hira.limit_length(cnt);
        kata.limit_length(cnt);
        curr = &hira;

        btnRStart->Enabled = (hira.checked_count() + kata.checked_count() >= 2);
        btnWStart->Enabled = btnRStart->Enabled && hasrecdata;
        lbInstruction->Visible = !btnRStart->Enabled;
        if (btnRStart->Enabled && hira.equals(false))
            tcKana->TabIndex = 1;
        tcKanaChange(tcKana);

        r = ShowModal();
        if (r == mrOk || r == mrYes)
        {
            settings.bitskata = kata;
            settings.bitshira = hira;

            settings.kanatests = true;

            if (r == mrOk)
            {
                fKanaTest = new TfKanaTest(Application);
                r = fKanaTest->ShowModalEx(hira, kata);
                delete fKanaTest;
            }
            else
            {
                fKanaWriting = new TfKanaWriting(Application);
                r = fKanaWriting->ShowModalEx(hira, kata);
                delete fKanaWriting;
            }
        }
    }
    return r;
}
//---------------------------------------------------------------------------
void __fastcall TfKana::CheckBox1Click(TObject *Sender)
{
    TCheckBox *cb = (TCheckBox*)Sender;
    (*curr)[GetPos(cb->Hint)] = cb->Checked;

    btnRStart->Enabled = (hira.checked_count() + kata.checked_count() >= 2);
    btnWStart->Enabled = btnRStart->Enabled && hasrecdata;
    lbInstruction->Visible = !btnRStart->Enabled;
}
//---------------------------------------------------------------------------
void __fastcall TfKana::sbPlusClick(TObject *Sender)
{
    bool pos = (Sender == sbPlus);

    for (int i = 0; i < tcKana->ControlCount; i++)
        if (tcKana->Controls[i]->ClassType() == __classid(TCheckBox))
            ((TCheckBox*)tcKana->Controls[i])->Checked = pos;

    btnRStart->Enabled = (hira.checked_count() + kata.checked_count() >= 2);
    btnWStart->Enabled = btnRStart->Enabled && hasrecdata;
    lbInstruction->Visible = !btnRStart->Enabled;
}
//---------------------------------------------------------------------------
void __fastcall TfKana::CheckBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TCheckBox *cb = (TCheckBox*)Sender;
    if (!cb->Enabled)
        return;

    for (int i = 0; i < tcKana->ControlCount; i++)
        if (cb != tcKana->Controls[i] && tcKana->Controls[i]->ClassType() == __classid(TCheckBox) && ((TCheckBox*)tcKana->Controls[i])->Tag == cb->Tag && Shift.Contains(ssCtrl))
            ((TCheckBox*)tcKana->Controls[i])->Checked = !cb->Checked;

    btnRStart->Enabled = (hira.checked_count() + kata.checked_count() >= 2);
    btnWStart->Enabled = btnRStart->Enabled && hasrecdata;
    lbInstruction->Visible = !btnRStart->Enabled;
}
//---------------------------------------------------------------------------
int TfKana::GetPos(UnicodeString s)
{
    s = s.LowerCase();

    if (s.Pos(" "))
        s.SetLength(s.Pos(" ") - 1);

    for (int i = 0; i < cnt; i++)
        if (s == hints[i])
            return i;

    //MsgBox("The programmer is blind. Tell him! (And what you did when this happened also)","This is a bug!",MB_OK);
    THROW(L"The programmer is blind. Tell him! (And what you did when this happened also)"/*,"This is a bug!"*/);
}
//---------------------------------------------------------------------------
void __fastcall TfKana::FormPaint(TObject *Sender)
{
    DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, Rect(0, 0, ClientWidth, tcKana->Top - 2));
    DrawCloseBar(this, bv2, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfKana::tcKanaChange(TObject *Sender)
{
    Bits b = (tcKana->TabIndex == 0 ? hira : kata);
    curr = &b;

    for (int i = 0; i < tcKana->ControlCount; i++)
    {
        if (tcKana->Controls[i]->ClassType() == __classid(TCheckBox))
            ((TCheckBox*)tcKana->Controls[i])->Checked = b[GetPos(((TCheckBox*)tcKana->Controls[i])->Hint)];
        if (tcKana->Controls[i]->ClassType() == __classid(TLabel))
        {
            if (((TLabel*)tcKana->Controls[i])->Name.Pos("Label") == 1)
                ((TLabel*)tcKana->Controls[i])->Caption = (tcKana->TabIndex == 0 ? ((TLabel*)tcKana->Controls[i])->Caption.LowerCase() : ((TLabel*)tcKana->Controls[i])->Caption.UpperCase());
        }
    }
    curr = &(tcKana->TabIndex == 0 ? hira : kata);
    lbPage->Caption = tcKana->TabIndex == 0 ? "Hiragana syllables" : "Katakana syllables";
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(67);
#endif

