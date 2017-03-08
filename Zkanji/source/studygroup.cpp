//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "studygroup.h"
#include "studyroom.h"
#include "settings.h"
#include "wordgroupstudy.h"
#include "words.h"
#include "regroup.h"
#include "gengraph.h"
#include "mouseevent.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(158);
#endif

TfWordStudy *fWordStudy;
//---------------------------------------------------------------------------

#define PAGE_INTRO       0
#define PAGE_M1          1
#define PAGE_M2          2
#define PAGE_COMMON      3
#define PAGE_FINAL       4

/*
wchar_t *winstructions[] =
{
    L"First you have to select a study method. Selecting a method will determine what kind of rules can be set and how the testing takes place. Each method tests the same knowledge (eg. meaning), but the construction of the lists can be much different.",
    L"Select the option you would like to practice. For example when you select kanji, only the kanji will be hidden in the study window before you select or type in the correct answer. Try one of the random modes if you are more confident about your knowledge.",
    L"Checking this option lets you type the right answer in a kana editbox when either the \"Kana\" or the \"Kana and Kanji\" mode is selected. This only tests your knowledge about the kana, but you might not want to see the Kanji either, as it might help too much.",
    L"Select the way you want to test yourself. You can either find the right answer in five given options, or press the \"Correct answer\" \\ \"Wrong answer\" buttons. (Except when testing kana and typing.)",
    L"Try one of the timing modes to find out whether your knowledge is confident enough or not. When the time is up, you lose a point.",
    L"This test is for remembering new words. You should try to memorize each word before the round starts. Select the maximum number of words you can easily keep in your head.",
    L"In each round one new word is added to the test. When studying new words it is better to keep some link between them, to be able to recall them more easily. If you select the order \"As they were added\" then each new word will be asked last for the next round. The opposite is \"Backwards\" where the latest words are asked first. Random order is random.",
    L"When this option is not checked, the words are added to the test in the order as they appeared in the study group. Check to randomize initial order of words.",
    L"Type in the number of words you would like to test from the words that make up this study group. The words will be selected randomly.",
    L"The selection will be random, but some words are more likely to be included. Select the priority of important points to influence the outcome.",
    L"Move your mouse over an option to see instructions.",
    L"Create the study group and start the test right away.",
    L"Set up the study group and save it for later use.",
    L"You might not want to see the hiragana and katakana that is part of the kanji definition when you are testing your knowledge about the kana itself. Check this option to see a \"*\" instead of the kana.",
    L"Update the list of words that appear in the study group. You can add and remove words or entire groups of words.",
    L"When the first option is selected you can remove and add words from the test in each round. Select any other option to let zkanji decide which words to include.",
    L"This option only has effect when zkanji determines the included words. The inclusion method can be influenced by the global scores or only by the local scores of the test.",
    L"New words are added to the test in each round, but you can select how many. (No new words are added when an answer was incorrect in the last round.)"
};
*/
//---------------------------------------------------------------------------
__fastcall TfWordStudy::TfWordStudy(TComponent* Owner) : TBaseForm(Owner)
{
    EraseBackground = false;
    edName->MaxLength = group_name_length_limit;

    fmethod_ori = smUndefined;
    oldcq = -1;

    cbMethod->ItemIndex = 0;

    cbMethodChange(cbMethod);
    lbM2Inc->ItemIndex = 0;
    lbM2IncClick(lbM2Inc);

    numWords = new TNumEdit(edM2Num, 10, 10, false);
}
//---------------------------------------------------------------------------
__fastcall TfWordStudy::~TfWordStudy()
{
    delete numWords;
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::cbMethodChange(TObject *Sender)
{
    //cbType->Enabled = (rgPractice->ItemIndex == 1 || rgPractice->ItemIndex == 2);
    //cbChoices->Enabled = (!cbType->Enabled || !cbType->Checked) && fmethod >= 0;

    cbQuestionChange(cbQuestion);
    lbChanged->Visible = (fmethod_ori != smUndefined && cbMethod->ItemIndex != fmethod_ori);
    if (cbMethod->ItemIndex == 0)
    {
        pGradual->Visible = true;
        pRefresh->Visible = false;
    }
    else
    {
        pRefresh->Visible = true;
        pGradual->Visible = false;
    }
}
//---------------------------------------------------------------------------
const int ttimesec[] = { 0, 3, 5, 10, 15, 20, 30, 60 };
//---------------------------------------------------------------------------
void __fastcall SetOrder(TListBox *box, int val)
{
    if (!val)
        return;

    const wchar_t *items[] = { L"1Number of inclusions", L"2Date of last inclusion", L"3Global score", L"4Word popularity" };

    box->Items->BeginUpdate();
    box->Items->Clear();
    try
    {
        byte a[4] = { (val & 3), ((val >> 2) & 3), ((val >> 4) & 3), ((val >> 6) & 3) };

        for (int i = 0; i <= 3; i++)
        {
            for (int j = 0; j <= 3; j++)
            {
                if (a[j] == i)
                    box->Items->Add(items[j]);
            }
        }
    }
    catch (...)
    {
        ;
    }
    box->Items->EndUpdate();
    fWordStudy->lbM2IncClick(fWordStudy->lbM2Inc);
}
//---------------------------------------------------------------------------
int GetOrder(TListBox *box)
{
    int result = 0;
    result = result | (1 << ((box->Items->Strings[1][1] - '1') * 2));
    result = result | (2 << ((box->Items->Strings[2][1] - '1') * 2));
    result = result | (3 << ((box->Items->Strings[3][1] - '1') * 2));
    return result;
}
//---------------------------------------------------------------------------
TWordGroupStudy* __fastcall TfWordStudy::ShowModalEx(TWordCollection *acollection, TWordIndexList *al, UnicodeString aname)
{
    fcollection = acollection;
    l = al;
    fgroupname = aname;
    tl = new TWordIndexList(l);
    edName->Text = aname;
    edName->Enabled = true;

    cbM1Num->ItemIndex = settings.M1Num;
    cbM1Order->ItemIndex = settings.M1Order;
    cbM1Rand->Checked = settings.M1Rand;
    cbM1Add->ItemIndex = settings.M1Add - 1;
    cbM1Selection->ItemIndex = settings.M1Selection;
    cbM1Global->Checked = settings.M1Global;

    SetOrder(lbM2Inc, settings.M2Inc);
    lbM2Inc->ItemIndex = 0;

    if (settings.M0Practice != 255)
    {
        switch (settings.M0Practice)
        {
        case 1:
            cbQuestion->ItemIndex = 3;
        case 2:
            cbQuestion->ItemIndex = 1;
        case 3:
            cbQuestion->ItemIndex = 2;
        case 4:
            cbQuestion->ItemIndex = 5;
        case 5:
            cbQuestion->ItemIndex = 6;
        case 6:
            cbQuestion->ItemIndex = 7;
        case 7:
            cbQuestion->ItemIndex = 4;
        case 0:
        default:
            cbQuestion->ItemIndex = 0;
        }
        cbQuestionChange(cbQuestion);

        if (settings.M0Type)
            cbInputMode->ItemIndex = 2;
        else
        {
            switch (settings.M0Choices)
            {
            case 1:
                cbInputMode->ItemIndex = 0;
            case 0:
            default:
                cbInputMode->ItemIndex = 1;
            }
        }
    }
    else
    {
        cbQuestion->ItemIndex = settings.M0Question;
        cbQuestionChange(cbQuestion);
        cbInputMode->ItemIndex = settings.M0Answer;
    }
    cbTimer->ItemIndex = settings.M0Timer;

    cbHideKana->Checked = settings.M0HideKana;

    numWords->max = tl->Count;
    numWords->value = min(settings.M2Num, tl->Count);

    cbQuestionChange(cbQuestion);
    if (ShowModal() == mrCancel)
    {
        delete tl;
        return NULL;
    }
    l->Assign(tl);
    delete tl;

    settings.M1Num = cbM1Num->ItemIndex;
    settings.M1Order = cbM1Order->ItemIndex;
    settings.M1Rand = cbM1Rand->Checked;
    settings.M1Add = cbM1Add->ItemIndex + 1;
    settings.M1Selection = cbM1Selection->ItemIndex;
    settings.M1Global = cbM1Global->Checked;

    settings.M2Num = numWords->value;
    settings.M2Inc = GetOrder(lbM2Inc);

    settings.M0Practice = 255;
    settings.M0Choices = 255;
    settings.M0Question = cbQuestion->ItemIndex;
    settings.M0Answer = cbInputMode->ItemIndex;
    settings.M0Timer = cbTimer->ItemIndex;
    settings.M0HideKana = cbHideKana->Checked;

    TWordGroupStudy *fgroup;

    TWordGroupM1 *m1;
    TWordGroupM2 *m2;

    TStudyAnswerType fatype;
    if (cbInputMode->ItemIndex == 0)
        fatype = saCorrectWrong;
    else if (cbInputMode->ItemIndex == 1)
        fatype = sa5Choices;
    else
    {
        int cq = cbQuestion->ItemIndex;
        int ca = cbInputMode->ItemIndex;
        if ((cq == sqKana || cq == sqMOrKana || cq == sqKOrK || cq == sqMOrKOrK) && ca == 2)
            fatype = saTypeKana;
        else if (((cq == sqKanji || cq == sqMOrKanji) && ca == 2) || ((cq == sqKOrK || cq == sqMOrKOrK) && ca == 3))
            fatype = saTypeKanji;
        else
            fatype = saTypeKK;
    }

    // Update group properties:
    switch (cbMethod->ItemIndex)
    {
    case smGradual:
        m1 = new TWordGroupM1(fcollection, fgroupname, (TStudyQuestionType)cbQuestion->ItemIndex, fatype, ttimesec[cbTimer->ItemIndex], cbHideKana->Checked && cbHideKana->Enabled, cbM1Num->Items->Strings[cbM1Num->ItemIndex].ToInt(), (EM1Order)cbM1Order->ItemIndex, cbM1Rand->Checked, cbM1Add->ItemIndex + 1, cbM1Selection->ItemIndex, cbM1Global->Checked);
        fgroup = m1;
        break;
    case smRefresh:
        m2 = new TWordGroupM2(fcollection, fgroupname, (TStudyQuestionType)cbQuestion->ItemIndex, fatype, ttimesec[cbTimer->ItemIndex], cbHideKana->Checked && cbHideKana->Enabled, GetOrder(lbM2Inc), numWords->value);
        fgroup = m2;
        break;
    }

    return fgroup;
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::update_caption()
{
    Caption = L"Study group attributes and rules - Words:" + IntToStr(tl->Count);
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::FormShow(TObject *Sender)
{
    update_caption();
    cbInputModeChange(cbInputMode);
}
//---------------------------------------------------------------------------
TWordGroupStudy* __fastcall TfWordStudy::ShowModalEx2(TWordGroupStudy *agroup)
{
    fcollection = agroup->collection;
    fgroupname = agroup->name;
    edName->Text = agroup->name;
    edName->Enabled = false;
    fmethod_ori = agroup->method;
    tl = new TWordIndexList(agroup->list);
    cbMethod->ItemIndex = agroup->method;
    cbMethodChange(cbMethod);
    pGradual->Visible = (agroup->method == smGradual);
    pRefresh->Visible = (agroup->method == smRefresh);
    numWords->max = tl->Count;

    if (agroup->method == smGradual)
    {
        cbMethod->ItemIndex = smGradual;
        cbM1Num->ItemIndex = ((TWordGroupM1*)agroup)->initial - 3;
        cbM1Order->ItemIndex = (int)((TWordGroupM1*)agroup)->order;
        cbM1Rand->Checked = ((TWordGroupM1*)agroup)->randomize;

        cbM1Add->ItemIndex = ((TWordGroupM1*)agroup)->wordadd - 1;
        cbM1Selection->ItemIndex = ((TWordGroupM1*)agroup)->selection;
        cbM1Global->Checked = ((TWordGroupM1*)agroup)->useglobal;

        numWords->value = tl->Count;

    }
    else if (agroup->method == smRefresh)
    {
        cbMethod->ItemIndex = smRefresh;
        numWords->value = StrToInt(((TWordGroupM2*)agroup)->number);
        SetOrder(lbM2Inc, (int)((TWordGroupM2*)agroup)->inclusion);
    }
    else
        THROW(L"Not finished this one! (bug)");

    cbQuestion->ItemIndex = (int)agroup->question;
    switch (agroup->timer)
    {
    case 0:
        cbTimer->ItemIndex = 0;
        break;
    case 3:
        cbTimer->ItemIndex = 1;
        break;
    case 5:
        cbTimer->ItemIndex = 2;
        break;
    case 10:
        cbTimer->ItemIndex = 3;
        break;
    case 15:
        cbTimer->ItemIndex = 4;
        break;
    case 20:
        cbTimer->ItemIndex = 5;
        break;
    case 30:
        cbTimer->ItemIndex = 6;
        break;
    case 60:
        cbTimer->ItemIndex = 7;
        break;
    }

    cbHideKana->Checked = agroup->hidekana;

    cbQuestionChange(cbQuestion);
    switch (agroup->answertype)
    {
    case saCorrectWrong:
    case sa5Choices:
        cbInputMode->ItemIndex = (int)agroup->answertype;
        break;
     case saTypeKana:
        cbInputMode->ItemIndex = 2;
        break;
     case saTypeKanji:
        cbInputMode->ItemIndex = cbInputMode->Items->Count > 3 ? 3 : 2;
        break;
     case saTypeKK:
        cbInputMode->ItemIndex = 4;
        break;
    }
    cbInputModeChange(cbInputMode);
    if (ShowModal() == mrCancel)
    {
        delete tl;
        return NULL;
    }

    TWordGroupM1 *m1;
    TWordGroupM2 *m2;

    TWordGroupStudy *result;

    TStudyAnswerType fatype;
    if (cbInputMode->ItemIndex == 0)
        fatype = saCorrectWrong;
    else if (cbInputMode->ItemIndex == 1)
        fatype = sa5Choices;
    else
    {
        int cq = cbQuestion->ItemIndex;
        int ca = cbInputMode->ItemIndex;
        if ((cq == sqKana || cq == sqMOrKana || cq == sqKOrK || cq == sqMOrKOrK) && ca == 2)
            fatype = saTypeKana;
        else if (((cq == sqKanji || cq == sqMOrKanji) && ca == 2) || ((cq == sqKOrK || cq == sqMOrKOrK) && ca == 3))
            fatype = saTypeKanji;
        else
            fatype = saTypeKK;
    }

    // Update group properties:
    switch (cbMethod->ItemIndex)
    {
    case smGradual:
        m1 = new TWordGroupM1(agroup->collection, fgroupname, (TStudyQuestionType)cbQuestion->ItemIndex, fatype, ttimesec[cbTimer->ItemIndex], cbHideKana->Checked && cbHideKana->Enabled, cbM1Num->Items->Strings[cbM1Num->ItemIndex].ToInt(), (EM1Order)cbM1Order->ItemIndex, cbM1Rand->Checked, cbM1Add->ItemIndex + 1, cbM1Selection->ItemIndex, cbM1Global->Checked);
        result = m1;
        break;
    case smRefresh:
        m2 = new TWordGroupM2(agroup->collection, fgroupname, (TStudyQuestionType)cbQuestion->ItemIndex, fatype, ttimesec[cbTimer->ItemIndex], cbHideKana->Checked && cbHideKana->Enabled, GetOrder(lbM2Inc), numWords->value);
        result = m2;
        break;
    default:
        THROW(L"Shouldn't do this! (bug)");
    }
    for (int i = 0; i < tl->Count; i++)
        result->AddWord(tl->Items[i]->index, tl->Items[i]->meaning);
    delete tl;

    return result;
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::lbM2IncDrawItem(TWinControl *Control, int Index, TRect &r, TOwnerDrawState State)
{
    ExtTextOutW(lbM2Inc->Canvas->Handle, r.Left, r.Top, ETO_CLIPPED | ETO_OPAQUE, &r, lbM2Inc->Items->Strings[Index].SubString(2, lbM2Inc->Items->Strings[Index].Length() - 1).c_str(), lbM2Inc->Items->Strings[Index].Length() - 1, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::lbM2IncClick(TObject *Sender)
{
    sbM2up->Enabled = lbM2Inc->ItemIndex > 0;
    sbM2down->Enabled = lbM2Inc->ItemIndex >= 0 && lbM2Inc->ItemIndex < 3;
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::sbM2upMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft)
        return;

    MouseRepeat(sbM2upMouseDown, Sender, Button, Shift, X, Y, NULL);

    int i = lbM2Inc->ItemIndex;
    lbM2Inc->Items->BeginUpdate();
    lbM2Inc->Items->Exchange(i, i - 1);
    lbM2Inc->ItemIndex = i - 1;
    lbM2Inc->Items->EndUpdate();

    lbM2IncClick(lbM2Inc);
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::sbM2downMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft)
        return;

    MouseRepeat(sbM2downMouseDown, Sender, Button, Shift, X, Y, NULL);

    int i = lbM2Inc->ItemIndex;
    lbM2Inc->Items->BeginUpdate();
    lbM2Inc->Items->Exchange(i, i + 1);
    lbM2Inc->ItemIndex = i + 1;
    lbM2Inc->Items->EndUpdate();

    lbM2IncClick(lbM2Inc);
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::btnWordListClick(TObject *Sender)
{
    fRegroup = new TfRegroup(Application);
    fRegroup->ShowModalEx(tl);
    delete fRegroup;
    numWords->max = tl->Count;
    numWords->value = min(numWords->value, tl->Count);
    update_caption();
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if (tl->Count < 10 && ModalResult != mrCancel)
        MsgBox(L"The group has less than 10 words. You won't be able to run the test unless you add more.", L"Warning", MB_OK);
    fgroupname = fgroupname.Trim();
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::edNameExit(TObject *Sender)
{
    if (edName->Text.Trim().IsEmpty())
        edName->Text = fgroupname.Trim();

    fgroupname = edName->Text.Trim();
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::FormPaint(TObject *Sender)
{
    DrawPillowPanel(this, NULL, b1, false, false);
    DrawPillowPanel(this, b1, b2, false, true);
    DrawPillowPanel(this, b2, b3, false, false);
    DrawPillowPanel(this, b3, b4, false, true);
    DrawPillowPanel(this, b4, b5, false, false);
    Canvas->Pen->Color = Color;
    Canvas->MoveTo(0, b5->Top + 1);
    Canvas->LineTo(ClientWidth, b5->Top + 1);
    DrawCloseBar(this, b6, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::cbQuestionChange(TObject *Sender)
{
/*
Meaning only
Kana only      k 1
Kanji only     K 2
Both kanji and kana
Both meaning and kana
Randomly selected: meaning OR kana      k 5
Randomly selected: meaning OR kanji     K 6
Randomly selected: meaning OR kanji and kana
Randomly selected: kanji OR kana   k 8 K 8
Randomly selected: meaning OR kanji OR kana     k 9 K 9
*/

    int cq = (TStudyQuestionType)cbQuestion->ItemIndex;
    int ca = cbInputMode->ItemIndex;

    int delta = 0;

    cbInputMode->Items->BeginUpdate();
    if (cq == sqKana || cq == sqMOrKana || cq == sqKOrK || cq == sqMOrKOrK)
    {
        if (oldcq != sqKana && oldcq != sqMOrKana && oldcq != sqKOrK && oldcq != sqMOrKOrK)
        {
            if (ca >= 2)
                ++ca;
            cbInputMode->Items->Insert(2, L"Type the answer for kana");
        }
        ++delta;
    }
    else if ((oldcq == sqKana || oldcq == sqMOrKana || oldcq == sqKOrK || oldcq == sqMOrKOrK) && cq != sqKana && cq != sqMOrKana && cq != sqKOrK && cq != sqMOrKOrK)
    {
        if (ca > 2)
            --ca;
        else if (ca == 2)
            ca = 0;
        cbInputMode->Items->Delete(2);
    }


    if (cq == sqKanji || cq == sqMOrKanji || cq == sqKOrK || cq == sqMOrKOrK)
    {
        if (oldcq != sqKanji && oldcq != sqMOrKanji && oldcq != sqKOrK && oldcq != sqMOrKOrK)
        {
            if (ca >= 2 + delta)
                ++ca;
            cbInputMode->Items->Insert(2 + delta, L"Type the answer for kanji");
        }
        ++delta;
    }
    else if ((oldcq == sqKanji || oldcq == sqMOrKanji || oldcq == sqKOrK || oldcq == sqMOrKOrK) && cq != sqKanji && cq != sqMOrKanji && cq != sqKOrK && cq != sqMOrKOrK)
    {
        if (ca > 2 + delta)
            --ca;
        else if (ca == 2 + delta)
            ca = 0;
        cbInputMode->Items->Delete(2 + delta);
    }

    if (cq == sqKOrK || cq == sqMOrKOrK)
    {
        if (oldcq != sqKOrK && oldcq != sqMOrKOrK)
        {
            if (ca >= 4 + delta)
                ++ca;
            cbInputMode->Items->Insert(2 + delta, L"Type the answer for kana and kanji");
        }
        ++delta;
    }
    else if ((oldcq == sqKOrK || oldcq == sqMOrKOrK) && cq != sqKOrK && cq != sqMOrKOrK)
    {
        if (ca > 2 + delta)
            --ca;
        else if (ca == 2 + delta)
            ca = 0;
        cbInputMode->Items->Delete(2 + delta);
    }

    cbInputMode->ItemIndex = ca;
    cbInputMode->Items->EndUpdate();

    oldcq = cq;

    cbInputModeChange(cbInputMode);
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::cbInputModeChange(TObject *Sender)
{
    int cq = (TStudyQuestionType)cbQuestion->ItemIndex;
    int ca = cbInputMode->ItemIndex;
    cbHideKana->Enabled = (cq == sqKana || cq == sqMKana || cq == sqMOrKana || cq == sqKOrK || cq == sqMOrKOrK) && ca >= 2;
}
//---------------------------------------------------------------------------
void __fastcall TfWordStudy::btnStudyClick(TObject *Sender)
{
    if (!WordGroupStudyValid((TStudyQuestionType)cbQuestion->ItemIndex, tl))
        return;

    ModalResult = mrYes;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(159);
#endif





