//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "redefine.h"
#include "wordobj.h"
#include "settings.h"
#include "collection.h"
#include "words.h"
#include "main.h"
#include "numedit.h"
#include "zkformats.h"
#include "gengraph.h"
#include "mouseevent.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(130);
#endif

TfRedefine *fRedefine;
//---------------------------------------------------------------------------

__fastcall TfRedefine::TfRedefine(TComponent* Owner) : TBaseForm(Owner)
{
    wobj = new TWordListObj(pbWords, rowsizes(settings.dictfontsize), &wcallback, (TPopupWordProc)NULL);
    wobj->wordselect = false;
    numFreq = new TNumEdit(edFreq, 0, 10000);
    numFreq->callback = ncallback;
    modified = false;

    delword = false;

    typestart = 0;
    while (pBoxes->Controls[typestart] != cb1)
        typestart++;

    notestart = 0;
    while (pBoxes->Controls[notestart] != cb24)
        notestart++;

    fieldstart = 0;
    while (pBoxes->Controls[fieldstart] != cb45)
        fieldstart++;

    edFreq->Hint = "This number is set for all meanings. Above 3500 is popular,\nbelow 1500 is unpopular.\n\nPress \"Save changes\" to modify.";
}
//---------------------------------------------------------------------------
__fastcall TfRedefine::~TfRedefine()
{
    delete wobj;
    delete numFreq;
}
//---------------------------------------------------------------------------
void TfRedefine::ncallback()
{
    if (numFreq->value != fcollection->words->Items[findex]->frequency)
        modified = true;
}
//---------------------------------------------------------------------------
void TfRedefine::wcallback()
{
    if (wobj->pos < 0)
        return;
    lbPos->Caption = IntToStr(wobj->pos + 1);

    TWord *w = fcollection->words->Items[findex];

    if (fmeaning != wobj->pos && savequery(false) == IDYES)
    {
        btnSaveClick(btnSave);
        return; //the function will be called again anyway
    }

    edMeaning->Text = w->data[wobj->pos].meaning;
    edMeaning->SelStart = 0;
    edMeaning->SelLength = edMeaning->Text.Length();
    fmeaning = wobj->pos;

    btnUp->Enabled = wobj->pos > 0;
    btnDown->Enabled = wobj->pos < wobj->count - 1;

    //check the type boxes
    for (int i = typestart; i < typestart + WT_COUNT; i++)
        ((TCheckBox*)pBoxes->Controls[i])->Checked = w->data[wobj->pos].types & (1 << (i - typestart));
    for (int i = notestart; i < notestart + WN_COUNT; i++)
        ((TCheckBox*)pBoxes->Controls[i])->Checked = w->data[wobj->pos].notes & (1 << (i - notestart));
    for (int i = fieldstart; i < fieldstart + WF_COUNT; i++)
        ((TCheckBox*)pBoxes->Controls[i])->Checked = w->data[wobj->pos].fields & (1 << (i - fieldstart));

    modified = false;

    bool ch = false;
    if (wobj->count == 1 && !delword)
    {
        btnDelete->Caption = "&Delete word";
        btnDelete->Font->Color = clMaroon;
        btnDelete->Font->Style = TFontStyles() << fsBold;
        btnDelete->Enabled = fcollection != dictionary->Items[0] || dictionary->Originals->IsAdded(w);
        delword = true;
        ch = true;
    }
    else if (wobj->count != 1 && delword)
    {
        btnDelete->Caption = "&Delete definition";
        btnDelete->Font->Color = clBlack;
        btnDelete->Font->Style = TFontStyles();
        delword = false;
        ch = true;
    }
    if (ch)
    {
        TRect r = Rect(bv3->Left + 2, bv1->Top, ClientWidth, bv2->Top);
        InvalidateRect(fRedefine->Handle, &r, false);
    }
}
//---------------------------------------------------------------------------
int TfRedefine::ShowModalEx(TWordCollection *acollection, int aindex, char ameaning)
{
    fcollection = acollection;
    findex = aindex;

    fill_definitions();
    if (ameaning > 0)
        wobj->pos = ameaning;

    return ShowModal();
}
//---------------------------------------------------------------------------
void TfRedefine::fill_definitions()
{
    TWord *w = fcollection->words->Items[findex];

    numFreq->value = w->frequency;

    TWordIndexList *l = new TWordIndexList(fcollection);
    try
    {
        for (int i = 0; i < w->meaningcnt; i++)
            l->Add(findex, i);
        wobj->set_by_indexes(l, true);
        lbCount->Caption = IntToStr(wobj->count) + " :";
    }
    catch (...)
    {
        ;
    }
    delete l;
}
//---------------------------------------------------------------------------
void __fastcall TfRedefine::edMeaningChange(TObject *Sender)
{
    btnNew->Enabled = edMeaning->Text.Trim().Length() && wobj->count < 100;
    btnSave->Enabled = edMeaning->Text.Trim().Length() && fmodified;
    if (fmeaning == wobj->pos)
        modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TfRedefine::cb1Click(TObject *Sender)
{
    modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TfRedefine::btnSaveClick(TObject *Sender)
{
    //TWord *w = fcollection->words->Items[findex];

    int ix = dictionary->IndexOfObject((TObject*)fcollection);

    int wtypes = 0;
    int wnotes = 0;
    int wfields = 0;
    for (int i = typestart; i < typestart + WT_COUNT; i++)
        if (((TCheckBox*)pBoxes->Controls[i])->Checked)
            wtypes = wtypes | (1 << (i - typestart));
    for (int i = notestart; i < notestart + WN_COUNT; i++)
        if (((TCheckBox*)pBoxes->Controls[i])->Checked)
            wnotes = wnotes | (1 << (i - notestart));
    for (int i = fieldstart; i < fieldstart + WF_COUNT; i++)
        if (((TCheckBox*)pBoxes->Controls[i])->Checked)
            wfields = wfields | (1 << (i - fieldstart));

    MODON
    dictionary->BeginWordChange(ix);
    try
    {
        dictionary->ChangeWordMeaning(ix, findex, fmeaning, edMeaning->Text.Trim().c_str(), wtypes, wnotes, wfields, 0, edFreq->Text.ToInt());
        modified = false;

        int newpos = wobj->pos;
        fill_definitions();
        wobj->pos = newpos;
    }
    catch (...)
    {
        ;
    }
    dictionary->EndWordChange();
    fcollection->Modify(true);
    MODOFF
}
//---------------------------------------------------------------------------
int TfRedefine::savequery(bool cancelbutton)
{
    if (btnSave->Enabled && modified)
        return MsgBox("The word's new definition will be lost! Do you want to save it now?", "Warning", cancelbutton ? MB_YESNOCANCEL : MB_YESNO);
    else
        return IDNO;
}
//---------------------------------------------------------------------------
void __fastcall TfRedefine::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if (ModalResult == mrCancel)
        return;

    int r = savequery(true);
    if (r == IDYES)
        btnSaveClick(btnSave);
    else if (r == IDCANCEL)
        CanClose = false;

    if (r != IDCANCEL)
    {
        // 2013.04.30 - Words can't have the same definition twice.
        TWord *w = fcollection->words->Items[findex];
        int cnt = w->meaningcnt;
        bool doubled = false;
        for (int ix = 0; !doubled && ix != cnt - 1; ++ix)
        {
            for (int iy = ix + 1; !doubled && iy != cnt; ++iy)
            {
                if (!wcscmp(w->data[ix].meaning, w->data[iy].meaning))
                    doubled = true;
            }
        }
        if (doubled)
        {
            MsgBox(L"The word contains the same meaning text more than once. Make the meanings differ from each other.", L"Message", MB_OK);
            CanClose = false;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfRedefine::btnDeleteClick(TObject *Sender)
{
    int ix = dictionary->Index[fcollection];
    if (wobj->count > 1)
    {
        if (MsgBox("This definition will be removed from every group that contains it, including its study statistics.\nDo you wish to continue?", "Warning", MB_YESNO) == IDNO)
            return;
        MODON
        dictionary->BeginWordChange(ix);
        try
        {
            dictionary->DeleteWordMeaning(ix, findex, wobj->pos);
        }
        catch (...)
        {
            ;
        }
        dictionary->EndWordChange();
        modified = false;
        int newpos = wobj->pos;
        fill_definitions();
        wobj->pos = newpos;

        fcollection->Modify(true);
        MODOFF
        btnNew->Enabled = edMeaning->Text.Trim().Length() && wobj->count < 100;
    }
    else
    {
        if (MsgBox("The word will be removed from this dictionary and from every group in the\ndictionary that contains it. The word's study statistics will be removed as well.\nDo you wish to continue?", "Warning", MB_YESNO) == IDNO)
            return;
        wobj->set_by_indexes(NULL);
        lbCount->Caption = "0";
        lbPos->Caption = "0";

        MODON
        dictionary->BeginWordChange(ix);
        try
        {
            dictionary->DeleteWord(ix, findex);
        }
        catch (...)
        {
            ;
        }
        dictionary->EndWordChange();
        fcollection->Modify(true);
        MODOFF
        modified = false;
        ModalResult = mrCancel;
        //Close();
    }
    //UpdateDictionaries(ix);
}
//---------------------------------------------------------------------------
void __fastcall TfRedefine::btnNewClick(TObject *Sender)
{
    //TWord *w = fcollection->words->Items[findex];
    int ix = dictionary->Index[fcollection];

    MODON
    dictionary->BeginWordChange(ix);

    try
    {
        //dictionary->AddWordMeaning(ix,findex,w,0,edMeaning->Text.Trim());

        int wtype = 0;
        int wnote = 0;
        int wfield = 0;
        for (int i = typestart; i < typestart + 23; i++)
            if (((TCheckBox*)pBoxes->Controls[i])->Checked)
                wtype = wtype | (1 << (i - typestart));
        for (int i = notestart; i < notestart + 21; i++)
            if (((TCheckBox*)pBoxes->Controls[i])->Checked)
                wnote = wnote | (1 << (i - notestart));
        for (int i = fieldstart; i < fieldstart + 8; i++)
            if (((TCheckBox*)pBoxes->Controls[i])->Checked)
                wfield = wfield | (1 << (i - fieldstart));

        dictionary->AddWordMeaning(ix, findex, edMeaning->Text.Trim().c_str(), wtype, wnote, wfield, 0);

        modified = false;
    }
    catch (...)
    {
        ;
    }

    dictionary->EndWordChange();
    fcollection->Modify(true);
    MODOFF

    fill_definitions();
    wobj->pos = wobj->count - 1;
    btnNew->Enabled = edMeaning->Text.Trim().Length() && wobj->count < 100;
}
//---------------------------------------------------------------------------

void TfRedefine::set_modified(bool val)
{
    if (fmodified == val)
        return;
    fmodified = val;
    bool dis = !edMeaning->Text.Trim().Length();
    btnSave->Flat = !fmodified;
    btnSave->Enabled = fmodified && !dis;
}
//---------------------------------------------------------------------------
void __fastcall TfRedefine::FormPaint(TObject *Sender)
{
    DrawCaptionBar(Canvas, Rect(0, 0, bv4->Left, bv0->Top), false);
    DrawPillowPanel(Canvas, Rect(bv4->Left + bv4->Width, 0, ClientWidth, bv0->Top), false, false);
    DrawPillowPanel(Canvas, Rect(0, bv1->Top + 2, bv3->Left, bv2->Top), true, false);
    DrawPillowPanel(Canvas, Rect(bv3->Left + 2, bv1->Top, ClientWidth, bv2->Top), true, true);

    DrawStatusBar(Canvas, Rect(0, pbWords->Top + pbWords->Height, ClientWidth, bv1->Top));

    TRect r = ChildRect(pBoxes);
    DrawPillowPanel(Canvas, Rect(r.Left, r.Top, ChildRect(bvV).Left, ChildRect(bvH).Top), true);
    DrawPillowPanel(Canvas, Rect(r.Left, ChildRect(bvH).Top + 2, ChildRect(bvV).Left, r.Bottom), true);
    DrawPillowPanel(Canvas, Rect(ChildRect(bvV).Left + 2, r.Top, r.Right, r.Bottom), true);
}
//---------------------------------------------------------------------------
void __fastcall TfRedefine::FormKeyPress(TObject *Sender, wchar_t &Key)
{
    if (fmodified && Key == L'\n' && (GetKeyState(VK_CONTROL) & (1 << 15)))
    {
        btnSave->Click();
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfRedefine::btnUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    MouseRepeat(btnUpMouseDown, Sender, Button, Shift, X, Y);

    int ix = dictionary->Index[fcollection];
    int dif = ((TSpeedButton*)Sender)->Tag;
    MODON
    dictionary->BeginWordChange(ix);
    try
    {
        dictionary->MoveWordMeaning(ix, findex, wobj->pos, wobj->pos + dif);
    }
    catch (...)
    {
        ;
    }
    dictionary->EndWordChange();
    modified = false;
    int newpos = wobj->pos + dif;
    fill_definitions();
    wobj->pos = newpos;

    fcollection->Modify(true);
    MODOFF
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(131);
#endif


