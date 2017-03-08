//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "regroup.h"
#include "words.h"
#include "wordobj.h"
#include "settings.h"
#include "wordgroup.h"
#include "collection.h"
#include "mouseevent.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(132);
#endif

TfRegroup *fRegroup;

//---------------------------------------------------------------------------
__fastcall TfRegroup::TfRegroup(TComponent* Owner) : TBaseForm(Owner)
{
    Constrain(pButtons->Width + btnOk->Width + btnCancel->Width + 8*scaleres + (Width - ClientWidth), pToolbar->Height + 246*scaleres + (Height - ClientHeight));
    wsize = -1;

    EraseBackground = false;

    gobj = new TVocabListObj(pbGWords, rowsizes(settings.dictfontsize), gcallback, (TPopupWordProc)NULL);
    gobj->selecttype = wlsMultiSelect;
    gobj->wordselect = false;
    wobj = new TVocabListObj(pbWords, rowsizes(settings.dictfontsize), wcallback, (TPopupWordProc)NULL);
    wobj->selecttype = wlsMultiSelect;
    wobj->wordselect = false;

    mousesizing = false;

    kanji = NULL;
    kana = NULL;
    meaning = NULL;
    kanjidat = NULL;
    kanadat = NULL;
    meaningdat = NULL;
    l = NULL;

    pbGWords->Align = alNone;
    pbWords->Align = alNone;
    pToolbar->Align = alNone;
    pGroupStat->Align = alNone;
    pStudyStat->Align = alNone;

    pbWords->Width = ClientWidth - 1; //force splitsize
    SetSplitSize(pbWords->Height);
}
//---------------------------------------------------------------------------
__fastcall TfRegroup::~TfRegroup()
{
    delete gobj;
    delete wobj;
    if (l)
        delete l;
}
//---------------------------------------------------------------------------
int TfRegroup::ShowModalEx(TWordIndexList *al)
{
    l = new TWordIndexList(al);
    fcollection = l->collection;
    wobj->wordlist = l;
    for (int i = 0; i < fcollection->groups->Count; i++)
        cbGroups->Items->Add(fcollection->groups->Names[i]);
    cbGroups->ItemIndex = MIN(cbGroups->Items->Count - 1, 0);
    if (cbGroups->Items->Count)
        cbGroupsChange(cbGroups);

    int r = ShowModal();
    if (r == mrOk)
        al->Assign(l);

    return r;
}
//---------------------------------------------------------------------------
void TfRegroup::gcallback()
{
    TTemplateList<TWordIndexList, true> *lists[3] = { kanjidat, kanadat, meaningdat };

    if (pButtons->Visible)
    {
        btnAdd->Enabled = gobj->count;
        lbGroupCount->Caption = IntToStr(fcollection->groups->Items[cbGroups->ItemIndex]->list->Count) + " :";
        lbGroupPos->Caption = IntToStr(gobj->pos + 1);
        wobj->wordlist = l;
    }
    else
    {
        if (gobj->count && gobj->pos >= 0)
            wobj->wordlist = lists[cbType->ItemIndex]->Items[gobj->pos];
        else
            wobj->wordlist = NULL;
    }
}
//---------------------------------------------------------------------------
void TfRegroup::wcallback()
{
    if (pButtons->Visible)
    {
        btnRemove->Enabled = wobj->count;
        btnUp->Enabled = wobj->count && wobj->selstart > 0;
        btnDown->Enabled = wobj->count && wobj->selstart + wobj->sellength < wobj->count;
        lbStudyCount->Caption = IntToStr(l->Count) + " :";
        lbStudyPos->Caption = IntToStr(wobj->pos + 1);
    }
    else
    {
        btnRemove2->Enabled = wobj->count && wobj->pos >= 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::cbGroupsChange(TObject *Sender)
{
    if (!cbGroups->Items->Count)
        return;
    gobj->wordlist = fcollection->groups->Lists[cbGroups->ItemIndex];
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::btnAddClick(TObject *Sender)
{
    int i, j;
    int s = l->Count;
    for (i = 0; i < gobj->sellength; i++)
    {
        for (j = 0; j < s; j++)
            if (l->Items[j]->index == fcollection->groups->Lists[cbGroups->ItemIndex]->Items[i + gobj->selstart]->index && l->Items[j]->meaning == fcollection->groups->Lists[cbGroups->ItemIndex]->Items[i + gobj->selstart]->meaning)
                break;
        if (j < s)
            continue;
        l->Add(fcollection->groups->Lists[cbGroups->ItemIndex]->Items[i + gobj->selstart]);
    }
    if (s == l->Count)
        return;
    wobj->Update(s, l->Count - s);
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::btnRemoveClick(TObject *Sender)
{
    for (int i = 0; i < wobj->sellength; i++)
        l->Delete(wobj->selstart + wobj->sellength - 1 - i);
    wobj->Update(MIN(l->Count - 1, wobj->selstart));
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::btnCheckClick(TObject *Sender)
{
    show_panel(((TBitBtn*)Sender)->Tag);
}
//---------------------------------------------------------------------------
void TfRegroup::show_panel(int p)
{
    switch (p)
    {
    case 0:
        delete_lists();
        pButtons->Visible = true;
        pButtons2->Visible = false;
        cbGroupsChange(cbGroups);
        break;
    case 1:
        construct_lists();
        pButtons2->Visible = true;
        pButtons->Visible = false;
        cbTypeChange(cbType);
        break;
    }
    lbGroup->Enabled = p == 0;
    lbGroupCount->Enabled = p == 0;
    lbGroupPos->Enabled = p == 0;
    lbStudy->Enabled = p == 0;
    lbStudyCount->Enabled = p == 0;
    lbStudyPos->Enabled = p == 0;
    gobj->selecttype = p == 0 ? wlsMultiSelect : wlsSingleSelect;
    wobj->selecttype = p == 0 ? wlsMultiSelect : wlsSingleSelect;
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::cbTypeChange(TObject *Sender)
{
    TWordIndexList *lists[3] = { kanji, kana, meaning };
    gobj->wordlist = lists[cbType->ItemIndex];
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::FormClose(TObject *Sender, TCloseAction &Action)
{
    gobj->wordlist = NULL;
    wobj->wordlist = NULL;
    delete_lists();
}
//---------------------------------------------------------------------------
void TfRegroup::delete_lists()
{
    if (kanji)
    {
        delete kanji;
        /*
        while (kanjidat->Count)
        {
            delete(TWordIndexList*)kanjidat->Items[kanjidat->Count - 1];
            kanjidat->Delete(kanjidat->Count - 1);
        }
        */
        delete kanjidat;
    }
    kanji = NULL;
    kanjidat = NULL;
    if (kana)
    {
        delete kana;
        /*
        while (kanadat->Count)
        {
            delete(TWordIndexList*)kanadat->Items[kanadat->Count - 1];
            kanadat->Delete(kanadat->Count - 1);
        }
        */
        delete kanadat;
    }
    kana = NULL;
    kanadat = NULL;
    if (meaning)
    {
        delete meaning;
        /*
        while (meaningdat->Count)
        {
            delete(TWordIndexList*)meaningdat->Items[meaningdat->Count - 1];
            meaningdat->Delete(meaningdat->Count - 1);
        }
        */
        delete meaningdat;
    }
    meaning = NULL;
    meaningdat = NULL;
}
//---------------------------------------------------------------------------
//sort functions
int kanjicmp(TWordIndex **a, TWordIndex **b)
{
    TWord *wa = fRegroup->fcollection->words->Items[(*a)->index];
    TWord *wb = fRegroup->fcollection->words->Items[(*b)->index];
    return wcscmp(wa->kanji, wb->kanji);
}
int kanacmp(TWordIndex **a, TWordIndex **b)
{
    TWord *wa = fRegroup->fcollection->words->Items[(*a)->index];
    TWord *wb = fRegroup->fcollection->words->Items[(*b)->index];
    return wcscmp(wa->kana, wb->kana);
}
int meaningcmp(TWordIndex **a, TWordIndex **b)
{
    TWord *wa = fRegroup->fcollection->words->Items[(*a)->index];
    TWord *wb = fRegroup->fcollection->words->Items[(*b)->index];
    return wcscmp(wa->data[(*a)->meaning].meaning, wb->data[(*b)->meaning].meaning);
}
void TfRegroup::construct_lists()
{
    int i;
    kanji = new TWordIndexList(fcollection/*,true*/);
    kana = new TWordIndexList(fcollection/*,true*/);
    meaning = new TWordIndexList(fcollection/*,true*/);
    kanjidat = new TTemplateList<TWordIndexList, true>;
    kanadat = new TTemplateList<TWordIndexList, true>;
    meaningdat = new TTemplateList<TWordIndexList, true>;

    TWordIndexList *tmp = new TWordIndexList(l);
    TWordIndexList *t2;
    tmp->Sort(kanjicmp);
    i = 1;
    while (i < tmp->Count)
    {
        if (!wcscmp(tmp->Words[i]->kanji, tmp->Words[i - 1]->kanji))
        {
            kanji->Add(tmp->Items[i - 1]);
            kanjidat->Add((t2 = new TWordIndexList(fcollection/*,true*/)));
            t2->Add(tmp->Items[i - 1]);
            while (i < tmp->Count && !wcscmp(tmp->Words[i]->kanji, tmp->Words[i - 1]->kanji))
                t2->Add(tmp->Items[i++]);
        }
        i++;
    }
    delete tmp;

    tmp = new TWordIndexList(l);
    tmp->Sort(kanacmp);
    i = 1;
    while (i < tmp->Count)
    {
        if (!wcscmp(tmp->Words[i]->kana, tmp->Words[i - 1]->kana))
        {
            kana->Add(tmp->Items[i - 1]);
            kanadat->Add((t2 = new TWordIndexList(fcollection/*,true*/)));
            t2->Add(tmp->Items[i - 1]);
            while (i < tmp->Count && !wcscmp(tmp->Words[i]->kana, tmp->Words[i - 1]->kana))
                t2->Add(tmp->Items[i++]);
        }
        i++;
    }
    delete tmp;

    tmp = new TWordIndexList(l);
    tmp->Sort(meaningcmp);
    i = 1;
    while (i < tmp->Count)
    {
        if (!wcscmp(tmp->Meanings[i], tmp->Meanings[i - 1]))
        {
            meaning->Add(tmp->Items[i - 1]);
            meaningdat->Add((t2 = new TWordIndexList(fcollection/*,true*/)));
            t2->Add(tmp->Items[i - 1]);
            while (i < tmp->Count && !wcscmp(tmp->Meanings[i], tmp->Meanings[i - 1]))
                t2->Add(tmp->Items[i++]);
        }
        i++;
    }
    delete tmp;
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::btnRemove2Click(TObject *Sender)
{
    TTemplateList<TWordIndexList, true> *lists[3] = { kanjidat, kanadat, meaningdat };
    TWordIndexList *blists[3] = { kanji, kana, meaning };
    TWordIndexList *tmp;
    int ix = lists[cbType->ItemIndex]->Items[gobj->pos]->Indexes[wobj->pos];
    for (int l = 0; l < 3; l++)
    {
        for (int k = lists[l]->Count - 1; k >= 0; k--)
        {
            tmp = lists[l]->Items[k];
            if (tmp->Count <= 1)
                continue;
            for (int j = tmp->Count - 1; j >= 0; j--)
                if (tmp->Indexes[j] == ix)
                {
                    tmp->Delete(j);
                    if (tmp->Count == 1)
                    {
                        blists[l]->Delete(gobj->pos);
                        //delete tmp;
                        lists[l]->Delete(k);
                    }
                    break;
                }
        }
    }
    for (int i = 0; i < l->Count; i++)
        if (l->Indexes[i] == ix)
        {
            l->Delete(i);
            break;
        }
    gobj->Update(gobj->pos);
    wobj->Update(wobj->pos);
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::btnUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (!wobj->selstart)
        return;
    MouseRepeat(btnUpMouseDown, Sender, Button, Shift, x, y);

    for (int i = 0; i < wobj->sellength; i++)
        l->Exchange(wobj->selstart - 1 + i, wobj->selstart + i);
    wobj->Update(wobj->selstart - 1, wobj->sellength);
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::btnDownMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (wobj->selstart + wobj->sellength - 1 == wobj->count - 1)
        return;
    MouseRepeat(btnDownMouseDown, Sender, Button, Shift, x, y);

    for (int i = 0; i < wobj->sellength; i++)
        l->Exchange(wobj->selstart + wobj->sellength - i, wobj->selstart + wobj->sellength - i - 1);
    wobj->Update(wobj->selstart + 1, wobj->sellength);
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::pbWordsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    switch (Key)
    {
    case VK_DELETE:
        if (pButtons->Visible && btnRemove->Enabled)
            btnRemoveClick(NULL);
        else if (!pButtons->Visible && btnRemove2->Enabled)
            btnRemove2Click(NULL);
        break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::pbGWordsKeyPress(TObject *Sender, wchar_t &Key)
{
    switch (Key)
    {
    case VK_SPACE:
        if (pButtons->Visible && btnAdd->Enabled)
            btnAddClick(NULL);
        break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::FormPaint(TObject *Sender)
{
    //DrawPillowPanel(Canvas,ChildRect(pGroupStat),false,true);
    //DrawPillowPanel(Canvas,ChildRect(pToolbar));
    DrawGrad(Canvas, clBtnFace, COLORDEC(clBtnFace, 0x101010), ChildRect(pToolbar));
    DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, ChildRect(pGroupStat));

    DrawPillowPanel(Canvas, ChildRect(pStudyStat), false);
}
//---------------------------------------------------------------------------

void __fastcall TfRegroup::pButtons2MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TPoint p;
    GetCursorPos(&p);
    mousey = p.y;
    mousesize = pbWords->Height;
    mousesizing = Screen->Cursor == crVSplit; //crSizeNS;
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::pButtons2MouseEnter(TObject *Sender)
{
    Screen->Cursor = crVSplit;
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::pButtons2MouseLeave(TObject *Sender)
{
    Screen->Cursor = crDefault;
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::pButtons2MouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    if (!Shift.Contains(ssLeft))
        mousesizing = false;
    if (mousesizing)
    {
        TPoint p;
        GetCursorPos(&p);
        SetSplitSize(mousesize + (mousey - p.y));
        wsize = pbWords->Height;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::pButtons2MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    mousesizing = false;
}
//---------------------------------------------------------------------------
void TfRegroup::SetSplitSize(int s)
{
    if (wsize >= 0)
    { //kwsize is < 0 only for the very first time
        s = MAX(MIN(s, ClientHeight - 160), 120);
    }
    else
        wsize = s;

    if (s == pbWords->Height && pbWords->Width == ClientWidth && pbGWords->Height == ClientHeight - pStudyStat->Height - pGroupStat->Height - pToolbar->Height - s)
        return;

    HDWP dp = BeginDeferWindowPos(5);
    int gheight = ClientHeight - s - pStudyStat->Height - pGroupStat->Height - pToolbar->Height;
    dp = DeferWindowPos(dp, pbGWords->Handle, NULL, 0, 0, ClientWidth, gheight, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    dp = DeferWindowPos(dp, pGroupStat->Handle, NULL, 0, gheight, ClientWidth, pGroupStat->Height, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    dp = DeferWindowPos(dp, pToolbar->Handle, NULL, 0, gheight + pGroupStat->Height, ClientWidth, pToolbar->Height, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    dp = DeferWindowPos(dp, pbWords->Handle, NULL, 0, gheight + pGroupStat->Height + pToolbar->Height, ClientWidth, s, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    dp = DeferWindowPos(dp, pStudyStat->Handle, NULL, 0, gheight + pGroupStat->Height + pToolbar->Height + s, ClientWidth, pStudyStat->Height, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    EndDeferWindowPos(dp);
}
//---------------------------------------------------------------------------
void __fastcall TfRegroup::FormResize(TObject *Sender)
{
    SetSplitSize(MIN(MAX(wsize, pbWords->Height), ClientHeight - 160));
    UpdateWindow(pGroupStat->Handle);
    UpdateWindow(pStudyStat->Handle);
    UpdateWindow(pToolbar->Handle);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(133);
#endif

