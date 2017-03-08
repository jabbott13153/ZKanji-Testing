//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanawritingtest.h"
#include "kanjiobj.h"
#include "kanjielement.h"
#include "strokebase.h"
#include "mouseevent.h"
#include "romajizer.h"
#include "settings.h"
#include "gengraph.h"
#include "kanjielement.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"
TfKanaWriting *fKanaWriting;

extern wchar_t *syllables[104];
extern wchar_t *roomaji[104];

//---------------------------------------------------------------------------
__fastcall TfKanaWriting::TfKanaWriting(TComponent* Owner) : playing(false), typing(true), TBaseForm(Owner)
{
    cnt = sizeof(syllables) / sizeof(syllables[0]);
}
//---------------------------------------------------------------------------
int TfKanaWriting::ShowModalEx(const Bits &hira, const Bits &kata)
{
    poresults = new TKanjiPanelObj(pbResults, pcallback, rcolor, rdraw);
    poresults->bgcolor = (TColor)clBtnFace;
    poresults->showhint = false;

    sobj = new TStrokeObj(pb);
    sobj->Grid = true;
    sobj->KanjiChar = false;
    sobj->AllowError = false;
    sobj->callback = scallback;

    kdraw = new TKanjiDrawObj(pbK, kproc);

    kdraw->strokewidth = (double)pbK->Width / 40;
    kdraw->strokemargin = pbK->Width / 25;
    kdraw->strokespeed = dsFast;
    kdraw->strokedots = true;
    kdraw->grid = true;

    lbPrevA->Font->Name = settings.kanafont;
    lbPrevA->Font->Charset = settings.kanacharset;
    lbPrevA->Font->Size = pbAnswer->ClientHeight * 0.6;

    int r = mrCancel;
    hiracnt = 0;
    katacnt = 0;
    for (int i = 0; i < cnt; i++)
    {
        if (hira[i])
            hiracnt++;
        if (kata[i])
            katacnt++;
    }
    wbuff = new wchar_t*[hiracnt + katacnt];
    rbuff = new wchar_t*[hiracnt + katacnt];
    testlist = new TIntegerList;
    try
    {
        int pos = 0;
        for (int i = 0; i < cnt && hiracnt; i++)
        {
            if (hira[i])
            {
                wbuff[pos] = syllables[i];
                rbuff[pos++] = roomaji[i];
            }
        }
        for (int i = 0; i < cnt && katacnt; i++)
        {
            if (kata[i])
            {
                wbuff[pos] = syllables[i];
                rbuff[pos++] = roomaji[i];
            }
        }

        btnGen->Click();
        r = ShowModal();
    }
    catch (...)
    {
        ;
    }
    delete testlist;
    delete[] wbuff;
    delete[] rbuff;

    delete kdraw;

    delete sobj;

    delete poresults;
    return r;
}
//---------------------------------------------------------------------------
void __fastcall TfKanaWriting::btnGenClick(TObject *Sender)
{
    if (btnStopAnswer->Visible)
    {
        playing = false;
        btnStopAnswer->Visible = false;
        pbK->Visible = false;
        pb->Visible = true;
        tK->Enabled = false;
    }
    testlist->Clear();
    donegood = false;
    donewrong = false;
    lbPrevQ->Visible = false;
    lbPrevA->Visible = false;
    t->Enabled = false;
    lbAnswer->Visible = true;
    pbAnswer->Visible = true;
    typing = true;
    btnShowAnswer->Visible = true;
    pb->Enabled = true;
    lbType->Visible = true;
    TIntegerList *elems = new TIntegerList;
    try
    {
        while (testlist->Count < 15)
        {
            for (int ix = 0; ix < hiracnt + katacnt; ++ix)
                elems->Add(ix);

            while (elems->Count)
            {
                int k = Random(elems->Count);
                testlist->Add(elems->Items[k]);
                elems->Delete(k);
            }
        }
    }
    catch (...)
    {
        ;
    }
    delete elems;
    pos = -1;
    mistakecnt = 0;
    mistakes = 0;
    lbMis->Visible = false;
    starttime = GetTickCount();
    tTime->Enabled = true;
    step();
}
//---------------------------------------------------------------------------
void TfKanaWriting::step()
{
    pos++;
    answer = "";
    lbTested->Caption = IntToStr(pos) + "/" + IntToStr(testlist->Count);
    sobj->reset();
    pbAnswer->Invalidate();
    if (pos < testlist->Count)
    {
        lbQuestion->Caption = UnicodeString(testlist->Items[pos] < hiracnt ? "(h) " : "(K) ") + rbuff[testlist->Items[pos]];
        lbQuestion->Font->Color = (TColor)(testlist->Items[pos] < hiracnt ? 0x6ca75c : 0xcf8a55);
    }
    else
    {
        pb->Enabled = false;
        tTime->Enabled = false;
        lbAnswer->Visible = false;
        pbAnswer->Visible = false;
        typing = false;
        btnShowAnswer->Visible = false;
        lbQuestion->Caption = "Finished";
        lbType->Visible = false;
        if (mistakecnt)
        {
            lbMis->Caption = "Mistakes: " + IntToStr(mistakecnt);
            lbMis->Visible = true;
        }
    }
}
//---------------------------------------------------------------------------
void TfKanaWriting::add_char(wchar_t kana)
{
    donegood = false;
    donewrong = false;
    lbPrevQ->Visible = false;
    lbPrevA->Visible = false;
    t->Enabled = false;

    answer += kana;
    UnicodeString str = wbuff[testlist->Items[pos]];
    if (testlist->Items[pos] >= hiracnt)
        str = ToKatakana(str);
    if (wcsncmp(answer.c_str(), str.c_str(), answer.Length()))
    { //error
        mistakes++;
        if (mistakes == 2)
        {
            mistakes = 0;
            donewrong = true;
            lbPrevQ->Caption = lbQuestion->Caption;
            lbPrevQ->Font->Color = lbQuestion->Font->Color;
            lbPrevQ->Visible = true;
            lbPrevA->Caption = testlist->Items[pos] < hiracnt ? wbuff[testlist->Items[pos]] : ToKatakana(wbuff[testlist->Items[pos]]).c_str();
            lbPrevA->Font->Color = clRed;
            lbPrevA->Visible = true;
            lbAnswer->Visible = false;
            pbAnswer->Visible = false;
            t->Enabled = true;
            testlist->Move(pos, testlist->Count - 1);
        }
        else
            mistakecnt++;

        pos--;
        step();
    }
    else if (answer == str)
    { //correct answer
        donegood = true;
        lbPrevQ->Caption = lbQuestion->Caption;
        lbPrevQ->Font->Color = lbQuestion->Font->Color;
        lbPrevQ->Visible = true;
        lbPrevA->Caption = testlist->Items[pos] < hiracnt ? wbuff[testlist->Items[pos]] : ToKatakana(wbuff[testlist->Items[pos]]).c_str();
        lbPrevA->Font->Color = clGreen;
        lbPrevA->Visible = true;
        lbAnswer->Visible = false;
        pbAnswer->Visible = false;
        t->Enabled = true;

        mistakes = 0;
        step();
    }
    else //half answer
                pbAnswer->Invalidate();

    if (typing && !lbPrevQ->Visible)
    {
        lbAnswer->Visible = true;
        pbAnswer->Visible = true;
    }
}
//---------------------------------------------------------------------------
void TfKanaWriting::rcolor(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected, TColor &background, TColor &fontcolor)
{
    if (elem->owner > NO_OWNER)
    {
        if (HIRAGANA(elem->owner))
        {
            background = (TColor)(!selected ? 0xbcf7bc : 0xbcf7ac);
        }
        else
        {
            background = (TColor)(!selected ? 0xf8dfbc : 0xffdaa5);
        }
    }
}
//---------------------------------------------------------------------------
void TfKanaWriting::rdraw(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected)
{
    ;
}
//---------------------------------------------------------------------------
void TfKanaWriting::scallback()
{
    TIntegerList *l = new TIntegerList;
    try
    {
        if (sobj->Count)
            for (int ix = 0; ix < sobj->ResultCount; ++ix)
            {
                l->Add(-sobj->Results[ix]);
            }
        poresults->set_items(l);
        set_buttons();
    }
    catch (...)
    {
        ;
    }
    delete l;
}
//---------------------------------------------------------------------------
void TfKanaWriting::set_buttons()
{
    btnUndo->Enabled = sobj->Count;
    btnReset->Enabled = sobj->Count;
    btnResBack->Enabled = poresults->pos;
    btnResFore->Enabled = pbResults->Width / pbResults->Height + poresults->pos < poresults->count;
}
//---------------------------------------------------------------------------
void TfKanaWriting::pcallback()
{
    set_buttons();
}
//---------------------------------------------------------------------------
void __fastcall TfKanaWriting::btnResBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    MouseRepeat(btnResBackMouseDown, Sender, Button, Shift, X, Y);
    poresults->pos--;
}
//---------------------------------------------------------------------------
void __fastcall TfKanaWriting::btnResForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    MouseRepeat(btnResForeMouseDown, Sender, Button, Shift, X, Y);
    poresults->pos++;
}
//---------------------------------------------------------------------------
void __fastcall TfKanaWriting::pbResultsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft)
        return;

    wchar_t k = poresults->CharAtPos(X);
    if (!k)
        return;
    add_char(k);
}
//---------------------------------------------------------------------------
void __fastcall TfKanaWriting::btnUndoClick(TObject *Sender)
{
    sobj->undo();
}
//---------------------------------------------------------------------------
void __fastcall TfKanaWriting::btnResetClick(TObject *Sender)
{
    sobj->reset();
}
//---------------------------------------------------------------------------
void __fastcall TfKanaWriting::pbAnswerPaint(TObject *Sender)
{
    TCanvas *c = pbAnswer->Canvas;
    c->Brush->Color = clBtnFace;
    c->FillRect(pbAnswer->ClientRect);
    c->Pen->Color = pbAnswer->Focused() ? clBtnText : coloravg(clBtnText, clBtnFace, 0.4);

    c->Pen->Width = 2 * scaleres;

    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = pbAnswer->ClientHeight * 0.6;
    c->Font->Color = clBlack;

    TRect r = pbAnswer->ClientRect;
    r.Bottom -= 4;
    int y = 3;

    int x = 0;
    while (x < pbAnswer->ClientWidth)
    {
        c->MoveTo(x, pbAnswer->ClientHeight - 2*scaleres);
        c->LineTo(x + 10*scaleres, pbAnswer->ClientHeight - 2*scaleres);
        x += 16 * scaleres;
    }

    ExtTextOutW(c->Handle, 0, y, ETO_CLIPPED, &r, answer.c_str(), answer.Length(), 0);
}
//---------------------------------------------------------------------------
void __fastcall TfKanaWriting::tTimeTimer(TObject *Sender)
{
    int ctime = GetTickCount() - starttime;
    int tsec = ctime / 1000;
    int tmin = tsec / 60;
    tsec -= tmin * 60;
    if (lbMin->Caption != (tmin < 10 ? "0" : "") + UnicodeString(min(99, tmin)) + ":")
        lbMin->Caption = (tmin < 10 ? "0" : "") + UnicodeString(min(99, tmin)) + ":";
    if (lbSec->Caption != (tsec < 10 ? "0" : "") + UnicodeString(min(59, tsec)))
        lbSec->Caption = (tsec < 10 ? "0" : "") + UnicodeString(min(59, tsec));
}
//---------------------------------------------------------------------------
void __fastcall TfKanaWriting::FormPaint(TObject *Sender)
{
    DrawCaptionBar(this, NULL, bv1, false);
    DrawCloseBar(this, bv4, NULL);
    TRect r = Rect(bvr->Left + 1, pb->Top, ClientWidth, bv3->Top);
    DrawGrad(Canvas, COLORINC(clBtnFace, 0x282828), COLORDEC(clBtnFace, 0x080808), r, false);
}
//---------------------------------------------------------------------------
void __fastcall TfKanaWriting::tTimer(TObject *Sender)
{
    donegood = false;
    donewrong = false;
    lbPrevQ->Visible = false;
    lbPrevA->Visible = false;
    t->Enabled = false;
    if (typing)
    {
        lbAnswer->Visible = true;
        pbAnswer->Visible = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfKanaWriting::btnShowAnswerClick(TObject *Sender)
{
    kpos = testlist->Items[pos];
    donegood = false;
    donewrong = false;
    lbPrevQ->Visible = false;
    lbPrevA->Visible = false;
    t->Enabled = false;

    btnStopAnswer->Visible = true;
    btnShowAnswer->Visible = false;

    kdraw->element = elements->IndexOfChar((kpos < hiracnt ? wbuff[kpos] : ToKatakana(wbuff[kpos]).c_str())[0]);

    pbK->Visible = true;
    pb->Visible = false;
    sobj->reset();

    playing = true;
    kdraw->Play();

    if (typing)
    {
        lbAnswer->Visible = true;
        pbAnswer->Visible = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfKanaWriting::btnStopAnswerClick(TObject *Sender)
{
    playing = false;
    btnStopAnswer->Visible = false;
    btnShowAnswer->Visible = true;
    pbK->Visible = false;
    pb->Visible = true;
    tK->Enabled = false;

    if (!mistakes)
        mistakecnt++;
    else
        mistakes = 0;
    testlist->Move(pos, testlist->Count - 1);

    pos--;
    step();
}
//---------------------------------------------------------------------------
void TfKanaWriting::kproc(TDrawStrokeState state)
{
    if (playing && state == dssStopped)
        tK->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfKanaWriting::tKTimer(TObject *Sender)
{
    tK->Enabled = false;
    int wlen = wcslen((kpos < hiracnt ? wbuff[kpos] : ToKatakana(wbuff[kpos]).c_str()));
    if (wlen > 1)
    {
        if (kdraw->element == elements->IndexOfChar((kpos < hiracnt ? wbuff[kpos] : ToKatakana(wbuff[kpos]).c_str())[0]))
            kdraw->element = elements->IndexOfChar((kpos < hiracnt ? wbuff[kpos] : ToKatakana(wbuff[kpos]).c_str())[1]);
        else
            kdraw->element = elements->IndexOfChar((kpos < hiracnt ? wbuff[kpos] : ToKatakana(wbuff[kpos]).c_str())[0]);
    }
    kdraw->Play();
}
//---------------------------------------------------------------------------
