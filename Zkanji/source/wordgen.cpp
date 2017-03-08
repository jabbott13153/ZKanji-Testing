//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "wordgen.h"
#include "wordobj.h"
#include "settings.h"
#include "drawscroll.h"
#include "kanji.h"
#include "collection.h"
#include "wordgroup.h"
#include "words.h"
#include "msgform.h"
#include "kanjigrid.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(178);
#endif

TfGen *fGen;
//---------------------------------------------------------------------------

__fastcall TfGen::TfGen(TComponent* Owner) : TBaseForm(Owner)
{
    cbGroups->MaxLength = group_name_length_limit;

    wobj = new TWordListObj(pbWords, rowsizes(settings.dictfontsize), wcallback, (TPopupWordProc)NULL);
    wobj->wordselect = false;

    findex = -1;

    fbase = NULL;
    kobj = new TKanjiGrid(pbGrid, 50 * scaleres, false, kcallback);
    kobj->scrollbars = true;

    int cx = Width - ClientWidth;
    Constrain(pbGrid->Left + GetSystemMetrics(SM_CXVSCROLL) + cx + kobj->BoxSize * 7, Height - pbWords->Height + 60, 999999, 999999, pbGrid->Left + GetSystemMetrics(SM_CXVSCROLL) + cx, kobj->BoxSize, 0, 0);
}
//---------------------------------------------------------------------------
__fastcall TfGen::~TfGen()
{
    delete wobj;
    delete kobj;
    if (base)
        delete base;
}
//---------------------------------------------------------------------------
int TfGen::ShowModalEx(TKanjiList *akanjis)
{
    klist = akanjis;

    pbGrid->TabStop = klist->Count > 1;

    cbPos->Enabled = klist->Count == 1;
    for (int i = 0; i < dictionary->Count; i++)
    {
        cbSrc->Items->Add(dictionary->Strings[i]);
        cbDest->Items->Add(dictionary->Strings[i]);
    }

    cbSave->Checked = settings.save_gen;
    if (settings.save_gen)
    {
        cbSrc->ItemIndex = MIN(settings.gen_src, cbSrc->Items->Count - 1);
        cbDest->ItemIndex = MIN(settings.gen_dest, cbDest->Items->Count - 1);
        sbLink->Down = settings.gen_linked;
        cbLen->ItemIndex = settings.gen_len;
        edPop->Text = IntToStr(settings.gen_pop);
        cbPos->ItemIndex = settings.gen_pos;
        cbOnly->Checked = settings.gen_only;
        cbGrad->ItemIndex = settings.gen_grad;
        cbNum->ItemIndex = settings.gen_num;

        cbDest->Enabled = !sbLink->Down;
        cbSrcChange(cbSrc);
        cbDestChange(cbDest);

        if (cbGroups->Items->IndexOf(settings.gen_group) >= 0)
            cbGroups->ItemIndex = cbGroups->Items->IndexOf(settings.gen_group);
    }
    else
    {
        cbSrc->ItemIndex = 0;
        cbSrcChange(cbSrc);
        if (cbDest->Enabled)
            cbDest->ItemIndex = 0;
        cbDestChange(cbDest);
    }

    wcallback();
    reading = new int[klist->Count];
    memset(reading, 0, klist->Count*sizeof(int));

    kobj->set_items(akanjis, dictionary->Items[cbSrc->ItemIndex]);

    int r;
    try
    {
        index = 0;
        r = ShowModal();
    }
    catch (...)
    {
        ;
    }
    delete[] reading;

    return r;
}
//---------------------------------------------------------------------------
void TfGen::kcallback()
{
    set_index(kobj->pos);
}
//---------------------------------------------------------------------------
void TfGen::wcallback()
{
    lbCount->Caption = wobj->count;
    lbPos->Caption = wobj->pos + 1;
}
//---------------------------------------------------------------------------
void __fastcall TfGen::cbSrcChange(TObject *Sender)
{
    SyncDest();
    kobj->collection = dictionary->Items[cbSrc->ItemIndex];
}
//---------------------------------------------------------------------------
void TfGen::SyncDest()
{
    if (cbDest->Enabled || cbDest->ItemIndex == cbSrc->ItemIndex)
        return;
    cbDest->ItemIndex = cbSrc->ItemIndex;
    cbDestChange(cbDest);
}
//---------------------------------------------------------------------------
void __fastcall TfGen::sbLinkClick(TObject *Sender)
{
    cbDest->Enabled = !sbLink->Down;
    SyncDest();
}
//---------------------------------------------------------------------------
void __fastcall TfGen::cbDestChange(TObject *Sender)
{
    cbGroups->Items->Clear();
    TWordGroupList *l = dictionary->Items[cbDest->ItemIndex]->groups;
    for (int i = 0; i < l->Count; i++)
        cbGroups->Items->Add(l->Items[i]->name);
    cbGroups->ItemIndex = 0;
    EnableButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfGen::edPopChange(TObject *Sender)
{
    //remove any non number from the box to stop text pasting bandits!
    int ss = edPop->SelStart;
    int nss = ss;

    UnicodeString s = L"";
    for (int i = 1; i <= edPop->Text.Length(); i++)
    {
        if (edPop->Text[i] >= '0' && edPop->Text[i] <= '9')
            s = s + edPop->Text[i];
        else if (i <= ss)
            nss--;
    }
    if (s.Length() != edPop->Text.Length())
    {
        edPop->Text = s;
        edPop->SelStart = nss;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGen::edPopExit(TObject *Sender)
{
    if (edPop->Text.Trim().Length() < 1)
        edPop->Text = L"0";
    if (edPop->Text.ToInt() > 9999)
        edPop->Text = L"9999";
}
//---------------------------------------------------------------------------
void __fastcall TfGen::cbGroupsChange(TObject *Sender)
{
    EnableButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfGen::btnGenClick(TObject *Sender)
{
    int lenval[] = { 9999, 2, 3, 4, 5, 6, 7, 8 };
    int gradval[] = { 10, 1, 2, 3, 4, 5, 6, 8, 9 };
    int numval[] = { 999, 1, 2, 3, 4, 5 };

    ShowMessageForm(L"Please wait, generating list...");
    Application->ProcessMessages();

    TIntegerList *l = NULL;
    try
    {
        l = new TIntegerList;
        for (int i = 0; i < klist->Count; i++)
        {
            for (int j = 0; j < dictionary->Items[cbSrc->ItemIndex]->kanjidat[klist->Kanjis[i]->index].wordscnt; j++)
                l->Add(dictionary->Items[cbSrc->ItemIndex]->kanjidat[klist->Kanjis[i]->index].words[j]);
            Application->ProcessMessages();
        }
        l->Sort();
        for (int i = l->Count - 1; i >= 0; i--)
        {
            if (!(i % 100))
                Application->ProcessMessages();

            if (i > 0 && l->Items[i] == l->Items[i - 1])
            {
                l->Delete(i);
                continue;
            }

            TWord *w = dictionary->Items[cbSrc->ItemIndex]->words->Items[l->Items[i]];
            bool del;
            int wlen = wcslen(w->kanji);
            del = (w->frequency < edPop->Text.ToInt()) || (int)wcslen(w->kana) > lenval[cbLen->ItemIndex];
            del = del || klist->Count == 1 && (cbPos->ItemIndex == 1 && w->kanji[0] != klist->Kanjis[0]->ch || cbPos->ItemIndex == 2 && w->kanji[wlen - 1] != klist->Kanjis[0]->ch);

            int p = 0;
            int kc = 0;
            while (!del && (cbOnly->Checked || cbGrad->ItemIndex > 0 || cbNum->ItemIndex > 0) && p < wlen)
            {
                if (KANJI(w->kanji[p]))
                {
                    kc++;
                    if (kc > numval[cbNum->ItemIndex])
                        del = true;
                    if (!del && kanjis->Kanjis[kanjis->IndexOf(w->kanji[p])]->jouyou > gradval[cbGrad->ItemIndex])
                        del = true;
                    if (!del && cbOnly->Checked)
                    {
                        del = true;
                        for (int k = 0; k < klist->Count && del; ++k)
                            if (w->kanji[p] == klist->Kanjis[k]->ch)
                                del = false;
                    }
                }
                p++;
            }

            for (int i = 0; i < klist->Count && !del; i++)
            {
                int r = real_index(klist->Kanjis[i], reading[i]) - 1;
                if (r >= 0 && wcschr(w->kanji, klist->Kanjis[i]->ch) && !IsReading(klist->Kanjis[i], w, r + 1/*(
                         r < klist->Kanjis[i]->oncnt ? klist->Kanjis[i]->on[r] :
                         klist->Kanjis[i]->kun[r-klist->Kanjis[i]->oncnt])*/))
                    del = true;
            }

            if (del)
                l->Delete(i);
        }

        if (base)
            delete base;

        fbase = new TWordIndexList(dictionary->Items[cbSrc->ItemIndex]/*,false*/);
        for (int i = 0; i < l->Count; i++)
            base->Add(l->Items[i]);

        wobj->set_by_indexes(base);
        EnableButtons();
    }
    catch (...)
    {
        ;
    }

    delete l;
    HideMessageForm();
}
//---------------------------------------------------------------------------
void TfGen::EnableButtons()
{
    btnAdd->Enabled = cbGroups->Text.Trim().Length() && wobj->count;
}
//---------------------------------------------------------------------------
void __fastcall TfGen::FormResize(TObject *Sender)
{
    if (!Visible)
        return;

    TRect r = Rect(0, ChildRect(b5).Top + 1, ClientWidth, ClientHeight);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
}
//---------------------------------------------------------------------------
void __fastcall TfGen::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    TWordCollection *src = dictionary->Items[cbSrc->ItemIndex];
    TWordCollection *dst = dictionary->Items[cbDest->ItemIndex];

    if (src != dst && ModalResult == mrOk)
    {
        int r = MsgBox(L"The source and destination dictionaries are different. New words might be added to the destination dictionary, if they were not present already! Do you want to continue?", L"Warning", MB_YESNOCANCEL);
        CanClose = r != IDCANCEL;
        if (r == IDNO)
            ModalResult = mrCancel;
    }
    if (ModalResult == mrOk)
    {
        settings.save_gen = cbSave->Checked;
        settings.gen_src = cbSrc->ItemIndex;
        settings.gen_dest = cbDest->ItemIndex;
        settings.gen_linked = sbLink->Down;
        settings.gen_group = cbGroups->Text.Trim();
        settings.gen_len = cbLen->ItemIndex;
        settings.gen_pop = edPop->Text.ToInt();
        settings.gen_pos = cbPos->ItemIndex;
        settings.gen_only = cbOnly->Checked;
        settings.gen_grad = cbGrad->ItemIndex;
        settings.gen_num = cbNum->ItemIndex;
    }
}
//---------------------------------------------------------------------------
void TfGen::set_index(int val)
{
    if (findex == val || val < 0 || val >= klist->Count)
        return;

    findex = val;

    cbReading->Items->BeginUpdate();
    cbReading->Items->Clear();
    for (int i = 0; i < compact_count(klist->Kanjis[findex]); i++)
    {
        cbReading->Items->Add(L"");
    }
    cbReading->ItemIndex = reading[findex];
    cbReading->Items->EndUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TfGen::cbReadingDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State)
{
    SetTextAlign(cbReading->Canvas->Handle, TA_BASELINE | TA_LEFT);
    if (!Index)
    {
        cbReading->Canvas->Font->Name = cbReading->Font->Name;
        cbReading->Canvas->Font->Size = cbReading->Font->Size;
        cbReading->Canvas->Font->Charset = cbReading->Font->Charset;
        ExtTextOutW(cbReading->Canvas->Handle, Rect.left + 2, Rect.top + Rect.Height()*0.85, ETO_CLIPPED | ETO_OPAQUE, &Rect, L"Any reading", 11, NULL);
        cbReading->Canvas->Font->Name = settings.kanafont;
        cbReading->Canvas->Font->Charset = settings.kanacharset;
        return;
    }

    wchar_t *c;

    cbReading->Canvas->Font->Name = settings.kanafont;
    cbReading->Canvas->Font->Charset = settings.kanacharset;
    cbReading->Canvas->Font->Size = Rect.Height() * 0.66;
    Index = real_index(klist->Kanjis[findex], Index) - 1;
    if (Index < klist->Kanjis[findex]->oncnt)
        c = klist->Kanjis[findex]->on[Index];
    else
    {
        c = klist->Kanjis[findex]->kun[Index - klist->Kanjis[findex]->oncnt];
        wchar_t *cc = wcschr(c, '.');
        if (cc)
            * cc = 0;
    }

    ExtTextOutW(cbReading->Canvas->Handle, Rect.left + 2, Rect.top + Rect.Height()*0.85, ETO_CLIPPED | ETO_OPAQUE, &Rect, c, wcslen(c), NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfGen::cbReadingChange(TObject *Sender)
{
    reading[findex] = cbReading->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TfGen::FormPaint(TObject *Sender)
{
    DrawPillowPanel(Canvas, Rect(0, 0, b2->Left, b0->Top), true);
    DrawPillowPanel(Canvas, Rect(0, b0->Top + 1, b2->Left, b1->Top), true);
    DrawPillowPanel(Canvas, Rect(0, b1->Top + 1, b2->Left, b4->Top), true);
    DrawPillowPanel(Canvas, Rect(b2->Left + 1, 0, ClientWidth, b3->Top), true);

    DrawStatusBar(this, b5, b6);
    DrawCloseBar(this, b7, NULL);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(179);
#endif

