//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "stats.h"
#include "collection.h"
#include "words.h"
#include "kanji.h"
#include "kanjielement.h"
#include "gengraph.h"
#include "qsrt.h"
#include "romajizer.h"
#include "localeinfo.h"
#include "genworddata.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(154);
#endif

TfStats *fStats;

extern bool hasexamples;

//bool SameWord(TWord *w1, TWord *w2, bool check_o); // from searchtree.h

int __thread_sort_proc(TfStats::TCollectDataThread *thread, TWord **a, TWord **b, bool &terminated)
{
    if (*a == *b)
        return 0;

    if (thread->Ending || (*a)->meaningcnt != (*b)->meaningcnt)
    {
        terminated = thread->Ending;
        return (*a)->meaningcnt - (*b)->meaningcnt;
    }

    int r = 0;
    for (int ix = 0; ix < (*a)->meaningcnt && !r && !thread->Ending; ++ix)
        r = wcscmp((*a)->data[ix].meaning, (*b)->data[ix].meaning);

    return r;
}
int __thread_sort_proc2(TfStats::TCollectDataThread *thread, TWord **a, TWord **b, bool &terminated)
{
    if (*a == *b)
        return 0;

    if (thread->Ending || (*a)->meaningcnt != (*b)->meaningcnt)
    {
        terminated = thread->Ending;
        return (*a)->meaningcnt - (*b)->meaningcnt;
    }

    int r = 0;
    for (int ix = 0; ix < (*a)->meaningcnt && !r && !thread->Ending; ++ix)
        r = wcscmp((*a)->data[ix].meaning, (*b)->data[ix].meaning);

    if (r)
        return r;

    r = wcscmp((*a)->kana, (*b)->kana);
    if (r)
        return r;

    return wcscmp((*a)->kanji, (*b)->kanji);
}
//---------------------------------------------------------------------------
TfStats::TCollectDataThread::TCollectDataThread(byte acoll, TDoneProc aproc) : coll(acoll), proc(aproc), ffinished(false), fending(false), fcompleted(false), pop(0), med(0), non(0), cnt(0), mcnt(0), TThread(true)
{
    OnTerminate = ActOnTerminate;
}
//---------------------------------------------------------------------------
bool TfStats::TCollectDataThread::samemeaning(TWord *a, TWord *b)
{
    if (a->meaningcnt != b->meaningcnt)
        return false;
    for (int ix = 0; ix < a->meaningcnt && !Terminated; ++ix)
        if (wcscmp(a->data[ix].meaning, b->data[ix].meaning))
            return false;
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TfStats::TCollectDataThread::Execute()
{
    TWordCollection *c = dictionary->Items[coll];
    if (!c->words->Count)
        return;

    TTemplateList<TWord, false> *l = new TTemplateList<TWord, false>;
    TIntegerList *li = new TIntegerList;

    l->Capacity = c->words->Count;
    for (int ix = 0; ix < c->words->Count && !Terminated; ++ix)
        l->Add(c->words->Items[ix]);

    if (Terminated)
    {
        delete l;
        delete li;
        return;
    }

    if (Terminated)
    {
        delete l;
        delete li;
        return;
    }

    quicksortthread(l->List, 0, l->Count - 1, this, &__thread_sort_proc);

    if (Terminated)
    {
        delete l;
        delete li;
        return;
    }

    int current = 0;
    while (current < l->Count && !Terminated)
    {
        TWord *w1 = l->Items[current];

        int left = current;
        int right = l->Count - 1;
        int mid = min(current + 4, right);

        while (left < right && !Terminated)
        {
            TWord *w2 = l->Items[mid];
            if (samemeaning(w1, w2))
            {
                if (mid == right)
                    break;
                left = mid;
                mid = min(mid + 4, (mid + right + 1) / 2);
            }
            else
            {
                right = mid - 1;
                mid = (left + right + 1) / 2;
            }
        }
        if (Terminated)
            break;

        li->Add(mid);
        current = mid + 1;
    }
    mcnt = li->Count;

    TStringList *currentkanjis = new TStringList;
    TStringList *currentkanas = new TStringList;

    TStringList *kanjis = new TStringList;
    TStringList *kanas = new TStringList;
    kanjis->Sorted = true;
    kanas->Sorted = true;
    kanjis->Duplicates = dupIgnore;
    kanas->Duplicates = dupIgnore;

    current = 0;
    int licurrent = -1;
    while (current < l->Count)
    {
        if (licurrent < 0 || li->Items[licurrent] < current)
        {
            licurrent++;

            currentkanas->Clear();
            currentkanjis->Clear();

            for (int ix = current; current != li->Items[licurrent] && ix <= li->Items[licurrent]; ++ix)
            {
                TWord *w = l->Items[ix];
                UnicodeString ka = Hiraganize(w->kana);
                UnicodeString kka = Hiraganize(w->kanji);

                if (kka.Length() > 1 && kka[1] == L'御' && (ka[1] == L'お' || ka[1] == L'ご'))
                    kka[1] = ka[1];
                for (int iy = 2; iy <= kka.Length() && !Terminated; ++iy)
                {
                    if (kka[iy] == L'々')
                        kka[iy] = kka[iy - 1];
                }
                currentkanjis->Add(kka);
                currentkanas->Add(ka);
            }

        }

        kanjis->Clear();
        kanas->Clear();

        TWord *w1 = l->Items[current];

        int currpos = current - (licurrent ? li->Items[licurrent - 1] + 1 : 0);

        for (int ix = currpos; ix < currentkanjis->Count; ++ix)
        {
            if (!l->Items[(licurrent ? li->Items[licurrent - 1] + 1 : 0) + ix])
                continue;
            if (currentkanas->Strings[currpos] == currentkanas->Strings[ix] || currentkanjis->Strings[currpos] == currentkanjis->Strings[ix])
            {
                kanjis->Add(currentkanjis->Strings[ix]);
                kanas->Add(currentkanas->Strings[ix]);
            }
        }

        int kanjiscnt = 0;
        int kanascnt = 0;

        while (kanjiscnt != kanjis->Count || kanascnt != kanas->Count)
        {
            kanjiscnt = kanjis->Count;
            kanascnt = kanas->Count;
            for (int ix = 0; ix < kanjis->Count; ++ix)
                for (int iy = currpos; iy < currentkanjis->Count; ++iy)
                {
                    if (!l->Items[(licurrent ? li->Items[licurrent - 1] + 1 : 0) + iy])
                        continue;
                    if (kanjis->Strings[ix] == currentkanjis->Strings[iy])
                        kanas->Add(currentkanas->Strings[iy]);
                }
            for (int ix = 0; ix < kanas->Count; ++ix)
                for (int iy = currpos; iy < currentkanas->Count; ++iy)
                {
                    if (!l->Items[(licurrent ? li->Items[licurrent - 1] + 1 : 0) + iy])
                        continue;
                    if (kanas->Strings[ix] == currentkanas->Strings[iy])
                        kanjis->Add(currentkanjis->Strings[iy]);
                }
        }

        l->Items[current] = NULL;
        current++;
        cnt++;
        currpos++;

        if (w1->frequency >= 3500)
            pop++;
        else if (w1->frequency >= 1500)
            med++;
        else
            non++;

        for (int ix = currpos; ix < currentkanjis->Count && !Terminated; ++ix)
        {
            TWord *w2 = l->Items[(licurrent ? li->Items[licurrent - 1] + 1 : 0) + ix];
            if (!w2)
                continue;

            UnicodeString ka = currentkanas->Strings[ix];
            UnicodeString kka = currentkanjis->Strings[ix];

            bool added = false;
            for (int j = 0; j < kanjis->Count && !added; ++j)
            {
                UnicodeString kkb = kanjis->Strings[j];

                if (kka == kkb/*|| SameWord(w1,w2,true)*/)
                {
                    if (w2->frequency >= 3500)
                        pop++;
                    else if (w2->frequency >= 1500)
                        med++;
                    else
                        non++;

                    l->Items[(licurrent ? li->Items[licurrent - 1] + 1 : 0) + ix] = NULL;
                    added = true;
                }
            }
            for (int j = 0; j < kanas->Count && !added; ++j)
            {
                UnicodeString kb = kanas->Strings[j];

                if (ka == kb/*|| SameWord(w1,w2,true)*/)
                {
                    if (w2->frequency >= 3500)
                        pop++;
                    else if (w2->frequency >= 1500)
                        med++;
                    else
                        non++;

                    l->Items[(licurrent ? li->Items[licurrent - 1] + 1 : 0) + ix] = NULL;
                    added = true;
                }
            }

        }

        while (current < l->Count && l->Items[current] == NULL)
            current++;
    }

    delete l;
    delete li;
    delete kanjis;
    delete kanas;
    delete currentkanjis;
    delete currentkanas;

    if (!Terminated)
        Synchronize(this, &Done);
}
//---------------------------------------------------------------------------
void __fastcall TfStats::TCollectDataThread::Done()
{
    fcompleted = true;
    proc(coll);
}
//---------------------------------------------------------------------------
void __fastcall TfStats::TCollectDataThread::ActOnTerminate(TObject *Sender)
{
    ffinished = true;
}
//---------------------------------------------------------------------------
void TfStats::TCollectDataThread::Terminate()
{
    if (fending)
        return;

    fending = true;
    inherited::Terminate();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TfStats::TCollectDataThreadList::TCollectDataThreadList(TfStats *aowner) : terminating(false), owner(aowner), inherited()
{

}
//---------------------------------------------------------------------------
TfStats::TCollectDataThreadList::~TCollectDataThreadList()
{
    for (int ix = 0; ix < Count; ++ix)
        delete Items[ix];
    Clear();
}
//---------------------------------------------------------------------------
void TfStats::TCollectDataThreadList::Start(byte ix)
{
    if (terminating)
        return;

    while (Count <= ix)
        inherited::Add(NULL);

    if (Items[ix])
        return;

    Items[ix] = new TCollectDataThread(ix, &ThreadCompleted);
    Items[ix]->Start();
}
//---------------------------------------------------------------------------
bool TfStats::TCollectDataThreadList::Completed(byte ix)
{
    return (Count > ix && Items[ix] && Items[ix]->Completed);
}
//---------------------------------------------------------------------------
void TfStats::TCollectDataThreadList::ThreadCompleted(byte coll)
{
    owner->FillData(coll);
}
//---------------------------------------------------------------------------
bool TfStats::TCollectDataThreadList::Terminate()
{
    bool r = true;
    terminating = true;

    for (int ix = 0; ix < Count; ++ix)
        if (Items[ix] != NULL && !Items[ix]->Finished)
        {
            if (!Items[ix]->Ending)
                Items[ix]->Terminate();
            if (!Items[ix]->Finished)
                r = false;
        }

    return r;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TfStats::TfStats(TComponent* Owner) : TBaseForm(Owner)
{
    threads = new TCollectDataThreadList(this);
    Constrain(pStats->Width + Width - ClientWidth, Height - ClientHeight + b2->Top + b2->Height + (ClientHeight - b3->Top) + lbInfo->Height * 6);
}
//---------------------------------------------------------------------------
__fastcall TfStats::~TfStats()
{
    delete threads;
}
//---------------------------------------------------------------------------
bool TfStats::ShowModalEx(int ix)
{
    for (int ix = 0; ix < dictionary->Count; ++ix)
    {
        cbDict->Items->Add(dictionary->Strings[ix]);
        threads->Start(ix);
    }

    SelectDictionary(ix);
    return ShowModal();
}
//---------------------------------------------------------------------------
void TfStats::SelectDictionary(int ix)
{
    cbDict->ItemIndex = ix;

    fcol = dictionary->Items[ix];
    Caption = "Dictionary information: " + dictionary->Strings[ix];

    lbWCnt->Caption = locale.commalize(fcol->words->Count);
    int kc = (!ix ? kanjis->Count : 0);
    if (ix)
    {
        for (int i = 0; i < kanjis->Count; i++)
            if (fcol->kanjidat[i].meaning)
                kc++;
    }

    int ke = 0;
    for (int i = 0; i < elements->Count; ++i)
        if (elements->Items[i]->owner < NO_OWNER)
            ke++;

    lbKWCnt->Caption = locale.commalize(kc);
    lbKCnt->Caption = locale.commalize(kanjis->Count);
    lbKECnt->Caption = locale.commalize(ke);

    mInfo->Text = dictionary->Items[ix]->info;

    const wchar_t *months[12] = { L"January", L"February", L"March", L"April", L"May", L"June", L"July", L"August", L"September", L"October", L"November", L"December" };

    if (dictionary_build_version > 0)
    {
        lbBuild->Caption = L"The current base dictionary was built under zkanji v" + FormatFloat("0.0##", dictionary_build_version) + L" on " + UnicodeString(months[dictionary_date.wMonth - 1]) + " " + (UnicodeString)dictionary_date.wDay + " ," + (UnicodeString)dictionary_date.wYear;
    }
    else
    {
        lbBuild->Caption = L"The current base dictionary does not contain build version and date information.";
    }
    if (hasexamples)
    {
        if (dictionary_build_version > 0)
            lbBuildEx->Caption = (UnicodeString)L"Example sentence data build date: " + months[example_date.wMonth - 1] + " " + example_date.wDay + " ," + example_date.wYear;
        else
            lbBuildEx->Caption = L"The current example sentence data file does not contain date information.";
    }
    else
        lbBuildEx->Caption = L"Example sentence data file not loaded.";

    if (threads->Completed(ix))
        FillData(ix);
    else
        ClearData();
}
//---------------------------------------------------------------------------
void TfStats::FillData(byte ix)
{
    if (cbDict->ItemIndex != ix || !Visible)
        return;

    lbPop->Caption = locale.commalize(threads->Items[ix]->Popular);
    lbMed->Caption = locale.commalize(threads->Items[ix]->Medium);
    lbNon->Caption = locale.commalize(threads->Items[ix]->Nonfrequent);

    lbCnt->Caption = locale.commalize(threads->Items[ix]->WordCount);
    lbMCnt->Caption = locale.commalize(threads->Items[ix]->MeaningCount);
}
//---------------------------------------------------------------------------
void TfStats::ClearData()
{
    if (!Visible)
        return;

    lbPop->Caption = L"COMPUTING";
    lbMed->Caption = L"COMPUTING";
    lbNon->Caption = L"COMPUTING";
    lbCnt->Caption = L"COMPUTING";
    lbMCnt->Caption = L"COMPUTING";
}
//---------------------------------------------------------------------------
void __fastcall TfStats::cbDictClick(TObject *Sender)
{
    SelectDictionary(cbDict->ItemIndex);
}
//---------------------------------------------------------------------------
void __fastcall TfStats::FormPaint(TObject *Sender)
{
    DrawPillowPanel(this, NULL, b0);
    DrawPillowPanel(this, b0, b1, false, true);
    DrawPillowPanel(this, b1, b2);
    DrawPillowPanel(this, b2, b3, false, true);
    DrawCloseBar(this, b4, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfStats::FormResize(TObject *Sender)
{
    TRect r = Rect(0, b4->Top + b4->Height, ClientWidth, ClientHeight);
    RedrawWindow(Handle, &r, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE);
    r = Rect(0, b2->Top + b2->Height, ClientWidth, b3->Top);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE);
}
//---------------------------------------------------------------------------
void __fastcall TfStats::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose = threads->Terminate();
    if (!CanClose)
    {
        cbDict->Enabled = false;
        PostMessage(Handle, WM_Z_HIDE, 0, 0);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfStats::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caFree;
}
//---------------------------------------------------------------------------
void TfStats::z_hide(TMessage &msg)
{
    Close();
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(155);
#endif


