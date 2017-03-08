//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "lineobj.h"
#include "collection.h"
#include "sentences.h"
#include "settings.h"
#include "drawscroll.h"
#include "words.h"
#include "wordlistbox.h"
#include "zmenu.h"
#include "module.h"
#include "kanji.h"
#include "kanjiinfo.h"
#include "clipview.h"
#include "genworddata.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

#define LINE_PADDING 4

#ifdef _DEBUG
static TGlobalTest tester1(94);
#endif

extern bool hasexamples;

//---------------------------------------------------------------------------
void PrettyDottedLine(TCanvas *c, int Left, int Right, int y)
{
    for (int i = Left; i < Right; i += 2)
        c->Pixels[i][y] = c->Pen->Color;
}
//---------------------------------------------------------------------------
TLineDrawObj::TLineDrawObj(int aheight)
{
    bmp = new TBmp;
    bmp->SetSize(1, aheight);

    rects = NULL;
    count = 0;
    factive = -1;
    fselecting = false;
    fhighlighted = -1;
    fhover = false;
}
//---------------------------------------------------------------------------
TLineDrawObj::~TLineDrawObj()
{
    if (rects)
        delete[] rects;
    delete bmp;
}
//---------------------------------------------------------------------------
void TLineDrawObj::Resize(int NewWidth)
{
    if (rects)
        delete[] rects;
    rects = NULL;
    count = 0;
    bmp->Width = NewWidth;
}
//---------------------------------------------------------------------------
void TLineDrawObj::TextOut(TFont *font, wchar_t *text, int x, int y, int Top, int height)
{
    SetTextAlign(bmp->Canvas->Handle, TA_LEFT | TA_BASELINE);
    TRect r = Rect(0, Top, bmp->Width, Top + height);
    bmp->Canvas->Font->Assign(font);
    ExtTextOutW(bmp->Canvas->Handle, x, y, ETO_CLIPPED | ETO_OPAQUE, &r, text, wcslen(text), NULL);
}
//---------------------------------------------------------------------------
void TLineDrawObj::Recount(TFont *font, wchar_t *text, int x, int y, TExampleWordData *data, byte datalen)
{
    TCanvas *c = bmp->Canvas;
    c->Font->Assign(font);

    TSize s;
    TRect r;

    TWordIndexList *li = new TWordIndexList(fcoll);

    if (!rects)
    {
        for (int i = 0; i < datalen; i++)
        {
            if (i && data[i].position == data[i - 1].position)
                continue;
            if (i && data[i].position > data[i - 1].position + data[i - 1].length || !i && data[i].position > 0)
                count++;
            count++;
        }
        if (data[datalen - 1].position + data[datalen - 1].length < (int)wcslen(text))
            count++;
        rects = new TLineObjRect[count];
        int p = 0;
        for (int i = 0; i < datalen; i++)
        {
            if (i && data[i].position == data[i - 1].position)
                continue;
            if (i && data[i].position > data[i - 1].position + data[i - 1].length || !i && data[i].position > 0)
            {
                rects[p].pos = (!p ? 0 : data[i - 1].position + data[i - 1].length);
                rects[p].len = data[i].position - rects[p].pos;
                rects[p].index = -1;
                p++;
            }
            rects[p].pos = data[i].position;
            rects[p].len = data[i].length;

            TStoredWord *sw = data[i].data;
            rects[p].index = sw && FindWordIndex(li, sw->kanji/*dictionary->Items[0]->words->Items[sw->wordindex]->kanji*/, sw->kana/*dictionary->Items[0]->words->Items[sw->wordindex]->kana*/) >= 0 ? i : -1;
            li->Clear();
            /*if (dictionary->ConvertIndex(dictionary->Items[0],data[i].index,fcoll) >= 0)
             rects[p].index = i;
             else
             rects[p].index = -1;
             */
            p++;
        }
        if (data[datalen - 1].position + data[datalen - 1].length < (int)wcslen(text))
        {
            rects[count - 1].pos = rects[count - 2].pos + rects[count - 2].len;
            rects[count - 1].len = wcslen(text) - rects[count - 1].pos;
            rects[count - 1].index = -1;
        }
        for (int i = 0; i < count; i++)
        {
            GetTextExtentPoint32W(c->Handle, text + rects[i].pos, rects[i].len, &s);
            rects[i].area = Rect((i > 0 ? rects[i - 1].area.Right : x), y - (double(font->Size) * 4.0 / 3.5) * scaleres, (i > 0 ? rects[i - 1].area.Right : x) + s.cx, y + ((float)font->Size / 3.5));
        }
    }

    delete li;
}
//---------------------------------------------------------------------------
void TLineDrawObj::DrawText(TFont *font, wchar_t *text, int Top, int height)
{
    TCanvas *c = bmp->Canvas;
    SetTextAlign(c->Handle, TA_LEFT | TA_BASELINE);
    c->Font->Assign(font);

    c->FillRect(Rect(0, Top, bmp->Width, Top + height));
    c->Brush->Style = bsClear;
    TRect r;
    c->Pen->Color = clGray;
    c->Pen->Width = 1;

    if (fhover && !fselecting)
        for (int i = 0; i < count; i++)
        {
            if (rects[i].index < 0)
                continue;
            r = rects[i].area;
            PrettyDottedLine(c, r.Left + 1, r.Right - 2, r.Bottom - 1);
        }
    c->Pen->Width = 1;

    for (int i = 0; i < count; i++)
    {
        TRect r = rects[i].area;
        if (!fselecting || rects[i].pos + rects[i].len <= fselstart || rects[i].pos >= fselstart + fsellength)
        {
            c->Font->Color = (factive == i ? clRed : clBlack);
            ExtTextOutW(c->Handle, r.Left, ROUND(r.Top + r.Height()*0.8), ETO_CLIPPED, &r, text + rects[i].pos, rects[i].len, NULL);
        }
        else
        {

            c->Font->Color = (factive == i ? clRed : clBlack);
            TSize s;
            int l = 0, l2;
            if (rects[i].pos < fselstart)
            {
                l = fselstart - rects[i].pos;
                GetTextExtentPoint32W(c->Handle, text + rects[i].pos, l, &s);
                r.Right = r.Left + s.cx;
                ExtTextOutW(c->Handle, r.Left, ROUND(r.Top + r.Height()*0.8), ETO_CLIPPED, &r, text + rects[i].pos, l, NULL);
                r.Left = r.Right;
            }
            l2 = MIN(rects[i].len - l, fselstart + fsellength - rects[i].pos - l);
            GetTextExtentPoint32W(c->Handle, text + rects[i].pos + l, l2, &s);
            r.Right = r.Left + s.cx;
            c->Brush->Style = bsSolid;
            c->Brush->Color = clHighlight;
            c->Font->Color = clHighlightText;
            ExtTextOutW(c->Handle, r.Left, ROUND(r.Top + r.Height()*0.8), ETO_CLIPPED | ETO_OPAQUE, &r, text + rects[i].pos + l, l2, NULL);
            c->Font->Color = (factive == i ? clRed : clBlack);
            c->Brush->Color = clWhite;
            c->Brush->Style = bsClear;
            r.Left = r.Right;
            l += l2;

            if (fselstart + fsellength < rects[i].pos + rects[i].len)
            {
                l2 = rects[i].len - l;
                GetTextExtentPoint32W(c->Handle, text + rects[i].pos + l, l2, &s);
                r.Right = r.Left + s.cx;
                ExtTextOutW(c->Handle, r.Left, ROUND(r.Top + r.Height()*0.8), ETO_CLIPPED, &r, text + rects[i].pos + l, l2, NULL);
                r.Left = r.Right;
            }

            c->Brush->Style = bsClear;
        }
    }

    if (!fselecting && fhighlighted >= 0)
    {
        r = rects[fhighlighted].area;
        c->Pen->Color = clBlack;
        r.Left -= 2, r.Right += 2, r.Top-- , r.Bottom++;
        c->Rectangle(r);
        InflateRect(&r, -1, -1);
        if (freverse)
            GradientFillCanvas(c, clWhite, (TColor)0x00e0e0e0, r, gdVertical);
        else
            GradientFillCanvas(c, (TColor)0x00e0e0e0, clWhite, r, gdVertical);

        if (factive == fhighlighted)
        {
            c->Font->Color = clRed;
        }
        else
            c->Font->Color = clBlack;
        TRect hirect = rects[fhighlighted].area;
        ExtTextOutW(c->Handle, hirect.Left, ROUND(hirect.Top + hirect.Height()*0.8), ETO_CLIPPED, &hirect, text + rects[fhighlighted].pos, rects[fhighlighted].len, NULL);

    }

    c->Brush->Style = bsSolid;
}
//---------------------------------------------------------------------------
void TLineDrawObj::SelectByIndex(int ix)
{
    if (!rects)
        THROW(L"Error! Example text sizes not measured yet!");

    int j = 0;
    factive = -1;

    while (j < count)
    {
        if (rects[j].index > ix)
            break;
        if (rects[j].index >= 0)
            factive = j;
        j++;
    }
}
//---------------------------------------------------------------------------
char TLineDrawObj::Highlight(int x, int y)
{
    fhighlighted = -1;
    if (y < 0)
        return fhighlighted;

    for (int i = 0; i < count; i++)
    {
        if (rects[i].index < 0)
            continue;
        if (PtInRect(rects[i].area, Point(x, y)))
        {
            fhighlighted = i;
            break;
        }
    }
    return fhighlighted;
}
//---------------------------------------------------------------------------
TRect TLineDrawObj::get_area(byte ix)
{
    if (ix >= count)
        THROW(L"Error! Word index too high for this line!");

    return rects[ix].area;
}
//---------------------------------------------------------------------------
byte TLineDrawObj::get_pos(byte ix)
{
    if (ix >= count)
        THROW(L"Error! Word index too high for this line!");

    return rects[ix].pos;
}
//---------------------------------------------------------------------------
byte TLineDrawObj::get_len(byte ix)
{
    if (ix >= count)
        THROW(L"Error! Word index too high for this line!");

    return rects[ix].len;
}
//---------------------------------------------------------------------------
char TLineDrawObj::get_index(byte ix)
{
    if (ix >= count)
        THROW(L"Error! Word index too high for this line!");

    return rects[ix].index;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TLineObj::TLineObj(TZPaintBox *apb, /*TWordCollection *acollection,*/bool asimple, TFuncPtr acallback) : pb(apb)/*, fcollection(acollection)*/, fsimple(asimple), fdata(NULL), fdatapos(0), fcallback(acallback)
{
    Examples->RegisterLineObj(this);

    fbox = NULL;

    fselecting = false;
    fselstart = -1;
    fsellength = 0;

    fcoll = dictionary->Items[0];

    flocked = false;
    fwordindex = -1;
    fonpaint = pb->OnPaint;
    fonmouseenter = pb->OnMouseEnter;
    fonmouseleave = pb->OnMouseLeave;
    fonmousemove = pb->OnMouseMove;
    fonmousedown = pb->OnMouseDown;
    fonmouseup = pb->OnMouseUp;
    fonformdeactivate = ((TForm*)pb->Owner)->OnDeactivate;
    pb->OnPaint = pbPaint;
    pb->OnMouseEnter = pbMouseEnter;
    pb->OnMouseLeave = pbMouseLeave;
    pb->OnMouseMove = pbMouseMove;
    pb->OnMouseDown = pbMouseDown;
    pb->OnMouseUp = pbMouseUp;
    ((TForm*)pb->Owner)->OnDeactivate = formDeactivate;

    fmode = lmBoth;
    scroller = new TDrawScroll(pb, 6, scallback);
    dobj = new TLineDrawObj(scroller->ClientHeight);
    dobj->Collection = fcoll;

    AppActivateEvent(pb, recount, true, true);
    AppActivateEvent(pb, deactivated, false, true);

    //popup menu rows
    menurows = new TZMenuRows(3);

    wchar_t *names[] = {
        L"Selection", L"Kanji Info", L"Translate",
        L"Clipboard", L"Copy", L"Append",
        L"Kanji", L"To Group...", L"Collect Words..."
    };
    TZMenuItemType types[] = {
        itTitle, itSimple, itSimple,
        itAltTitle, itSimple, itSimple,
        itTitle, itSimple, itSimple
    };
    TZPopupCommand cmd[] = {
        pcNone, pcKanjiInfo, pcTranslate,
        pcNone, pcCopy1, pcAppend1,
        pcNone, pcToGroup, pcCollect
    };

    for (unsigned int ix = 0; ix < (sizeof(names) / sizeof(wchar_t*)) / 3; ++ix)
    {
        TZMenuRow *row = menurows->AddRow();
        row->Setup(names + (ix*3), types + (ix*3), (void**)(cmd + (ix*3)));
    }
    menurows->AddSpan(1);

    //popup menu for the whole sentence
    sentencerows = new TZMenuRows(3);
    wchar_t *snames[] = {
        L"Sentence", L"Copy", L"Append",
        L"Translation", L"Copy", L"Append",
        L"Kanji", L"To Group...", L"Collect Words..."
    };
    TZMenuItemType stypes[] = {
        itAltTitle, itSimple, itSimple,
        itAltTitle, itSimple, itSimple,
        itTitle, itSimple, itSimple
    };
    TZPopupCommand scmd[] = {
        pcNone, pcCopy1, pcAppend1,
        pcNone, pcCopy2, pcAppend2,
        pcNone, pcToGroup, pcCollect
    };
    for (unsigned int ix = 0; ix < (sizeof(snames) / sizeof(wchar_t*)) / 3; ++ix)
    {
        TZMenuRow *row = sentencerows->AddRow();
        row->Setup(snames + (ix*3), stypes + (ix*3), (void**)(scmd + (ix*3)));
    }
    sentencerows->AddSpan(1);

    kanjirows = new TZMenuRows(3);
    wchar_t *knames[] = {
        L"Kanji", L"Kanji Info", L"Translate",
        L"Clpb: Sentence", L"Copy", L"Append",
        L"Clpb: Kanji", L"Copy", L"Append",
        L"Kanji", L"To Group...", L"Collect Words..."
    };
    TZMenuItemType ktypes[] = {
        itTitle, itSimple, itSimple,
        itAltTitle, itSimple, itSimple,
        itAltTitle, itSimple, itSimple,
        itTitle, itSimple, itSimple
    };
    TZPopupCommand kcmd[] = {
        pcNone, pcKanjiInfo, pcTranslate,
        pcNone, pcCopy1, pcAppend1,
        pcNone, pcCopy2, pcAppend2,
        pcNone, pcToGroup, pcCollect
    };
    for (unsigned int ix = 0; ix < (sizeof(knames) / sizeof(wchar_t*)) / 3; ++ix)
    {
        TZMenuRow *row = kanjirows->AddRow();
        row->Setup(knames + (ix*3), ktypes + (ix*3), (void**)(kcmd + (ix*3)));
    }
    kanjirows->AddSpan(2);

    SettingsChangeEvent(pb, settingsChanged, true);
}
//---------------------------------------------------------------------------
TLineObj::~TLineObj()
{
    Examples->UnregisterLineObj(this);

    SettingsChangeEvent(pb, settingsChanged, false);
    AppActivateEvent(pb, deactivated, false, false);
    AppActivateEvent(pb, recount, true, false);

    delete menurows;
    delete sentencerows;
    delete kanjirows;

    delete dobj;
    delete scroller;

    ((TForm*)pb->Owner)->OnDeactivate = fonformdeactivate;
    pb->OnMouseMove = fonmousemove;
    pb->OnMouseLeave = fonmouseleave;
    pb->OnMouseEnter = fonmouseenter;
    pb->OnMouseDown = fonmousedown;
    pb->OnMouseUp = fonmouseup;
    pb->OnPaint = fonpaint;

    if (fbox)
        delete fbox;
    fbox = NULL;
}
//---------------------------------------------------------------------------
void TLineObj::settingsChanged(void *caller, TSettingsProcState state)
{
    if (state != spsEnd || !fdata)
        return;
    recount(caller);
}
//---------------------------------------------------------------------------
void __fastcall TLineObj::formDeactivate(TObject *Sender)
{
    if (fonformdeactivate)
        fonformdeactivate(Sender);

    deactivated(Sender);
}
//---------------------------------------------------------------------------
void TLineObj::deactivated(void *caller)
{
    HideBox(true);
    dobj->Hovering = false;
    dobj->Highlight(-1, -1);
    fselecting = false;
    fselstart = 0;
    dobj->Selecting = false;

    refresh();
}
//---------------------------------------------------------------------------
void TLineObj::recount(void *caller)
{
    if (fdata)
    {
        try
        {
            if (!hasexamples)
                THROW(L"Examples not present!");
        }
        catch (...)
        {
            return;
        }
        TSize s;
        TExampleData *d = fdata->examples + fdatapos;
        TExampleSentence *es = Examples->sentences[d->block][d->num];
        wchar_t *en = es->translation;
        wchar_t *jp = es->japanese;
        int w = 0;
        TCanvas *c = pb->Canvas;
        TRect r;

        if (fmode == lmTranslated || fmode == lmBoth)
        {
            c->Font->Name = settings.romajifont;
            c->Font->Charset = DEFAULT_CHARSET;
            c->Font->Size = double(fmode == lmTranslated ? rowsizes(1) * 0.6 : rowsizes(2) * 0.5) / scaleres;
            GetTextExtentPoint32W(c->Handle, en, wcslen(en), &s);
            w = MAX((int)s.cx, w);
        }
        if (fmode == lmJapanese || fmode == lmBoth)
        {
            c->Font->Name = settings.kanafont;
            c->Font->Charset = settings.kanacharset;
            c->Font->Size = double(fmode == lmJapanese ? scroller->ClientHeight * 0.4 : rowsizes(2) * 0.8) / scaleres;
            GetTextExtentPoint32W(c->Handle, jp, wcslen(jp), &s);
            w = MAX((int)s.cx, w);
        }
        dobj->Resize(w + LINE_PADDING*2);

        if (fmode == lmJapanese || fmode == lmBoth)
        {
            c->Font->Name = settings.kanafont;
            c->Font->Charset = settings.kanacharset;
            c->Font->Size = double(fmode == lmJapanese ? scroller->ClientHeight * 0.4 : rowsizes(2) * 0.8) / scaleres;
            int h = (fmode == lmJapanese ? scroller->ClientHeight : scroller->ClientHeight / 1.8);
            dobj->Recount(c->Font, jp, LINE_PADDING, (fmode == lmJapanese ? h - (scroller->ClientHeight)*0.3 : h - rowsizes(2)*0.1), es->words, es->wordscnt);
            dobj->SelectByIndex(d->wordnum);
        }
        scroller->Range = dobj->Bitmap->Width;
    }

    if (fbox)
        delete fbox;
    fbox = NULL;
    dobj->Highlight(-1, -1);

    TPoint p;
    GetCursorPos(&p);
    p = pb->ScreenToClient(p);
    if (pb->Focused() && PtInRect(pb->ClientRect, p))
    {
        dobj->Hovering = true;
        pbMouseMove(pb, TShiftState(), p.x, p.y);
    }

    refresh();
}
//---------------------------------------------------------------------------
void TLineObj::refresh()
{
    if (fdata)
    {
        try
        {
            if (!Examples)
                THROW(L"Examples not present!");
        }
        catch (...)
        {
            return;
        }
        TCanvas *c = pb->Canvas;
        TExampleData *d = fdata->examples + fdatapos;
        TExampleSentence *es = Examples->sentences[d->block][d->num];

        wchar_t *en = es->translation;
        wchar_t *jp = es->japanese;

        if (fmode == lmTranslated || fmode == lmBoth)
        {
            c->Font->Name = settings.romajifont;
            c->Font->Charset = DEFAULT_CHARSET;
            c->Font->Size = double(fmode == lmTranslated ? rowsizes(1) * 0.6 : rowsizes(2) * 0.5) / scaleres;
            int t = (fmode == lmTranslated ? 0 : scroller->ClientHeight / 1.8);
            int h = scroller->ClientHeight - t;
            dobj->TextOut(c->Font, en, LINE_PADDING, (fmode == lmTranslated ? (h - rowsizes(1)) / 2 + rowsizes(1)*0.8 : t + h - rowsizes(2)*0.2), t, h);
        }
        if (fmode == lmJapanese || fmode == lmBoth)
        {
            c->Font->Name = settings.kanafont;
            c->Font->Charset = settings.kanacharset;
            c->Font->Size = double(fmode == lmJapanese ? scroller->ClientHeight * 0.4 : rowsizes(2) * 0.8) / scaleres;
            int h = (fmode == lmJapanese ? scroller->ClientHeight : scroller->ClientHeight / 1.8);
            dobj->DrawText(c->Font, jp, 0, h);
        }
    }
    pb->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TLineObj::pbPaint(TObject *Sender)
{
    TCanvas *c = pb->Canvas;

    if (!fdata)
    {
        c->FillRect(scroller->ClientRect);
        scroller->Range = 0;
    }
    else
    {
        HRGN old;
        GetClipRgn(c->Handle, old);
        HRGN r = CreateRectRgn(0, 0, pb->ClientWidth, scroller->ClientHeight);
        SelectClipRgn(c->Handle, r);
        DeleteObject(r);

        c->Draw(-scroller->Position, 0, dobj->Bitmap);
        if (dobj->Bitmap->Width - scroller->Position < pb->ClientWidth)
        {
            c->Brush->Color = clWhite;
            c->FillRect(Rect(dobj->Bitmap->Width - scroller->Position, 0, pb->ClientWidth, scroller->ClientHeight));
        }

        SelectClipRgn(c->Handle, old);
        DeleteObject(old);
    }
}
//---------------------------------------------------------------------------
void TLineObj::set_all(TWordCollection *acollection, TStoredWord *data, word data_pos)
{
    if (flocked || data == fdata && (fdatapos == data_pos || data_pos >= data->examplecnt) && acollection == fcoll)
        return;
    fdata = data;
    fdatapos = data_pos;

    fcoll = acollection;
    dobj->Collection = fcoll;

    recount(NULL);
    if (fcallback)
        fcallback();
}
//---------------------------------------------------------------------------
void TLineObj::set_data(TWordCollection *acollection, TStoredWord *data)
{
    if (fdata == data && acollection == fcoll)
        return;
    set_all(acollection, data, data == fdata ? fdatapos : 0);
}
//---------------------------------------------------------------------------
void TLineObj::set_datapos(word val)
{
    if (val == fdatapos)
        return;
    set_all(fcoll, fdata, val);
}
//---------------------------------------------------------------------------
void TLineObj::set_mode(TLineMode val)
{
    if (fmode == val)
        return;
    fmode = val;
    recount(NULL);

    if (fcallback)
        fcallback();
}
//---------------------------------------------------------------------------
void __fastcall TLineObj::pbMouseEnter(TObject *Sender)
{
    if (!fdata || !((TForm*)pb->Owner)->Active && (!((TForm*)pb->Owner)->Parent || !((TForm*)((TForm*)pb->Owner)->Parent)->Active))
        return;

    dobj->Hovering = true;
    refresh();
}
//---------------------------------------------------------------------------
void __fastcall TLineObj::pbMouseLeave(TObject *Sender)
{
    if (!fdata)
        return;

    HideBox();
    if (!fbox)
    {
        dobj->Hovering = false;
        dobj->Highlight(-1, -1);
        refresh();
    }
}
//---------------------------------------------------------------------------
int TLineObj::word_position(int x, int y, bool limit, bool selection)
{
    if (!fdata || fmode == lmTranslated)
        return -1;
    TRect r = dobj->Areas[0];
    if (limit && (y < r.Top || y >= r.Bottom || x < r.Left - 2))
        return -1;
    TCanvas *c = pb->Canvas;

    TSize s;
    TExampleData *d = fdata->examples + fdatapos;
    TExampleSentence *es = Examples->sentences[d->block][d->num];
    wchar_t *jp = es->japanese;

    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = double(fmode == lmJapanese ? scroller->ClientHeight * 0.4 : rowsizes(2) * 0.8) / scaleres;

    int jlen = wcslen(jp);
    int cnt = 0;
    int *sizes = new int[jlen];
    if (x >= 0)
    {
        GetTextExtentExPointW(c->Handle, jp, jlen, x, &cnt, sizes, &s);
        x -= LINE_PADDING + (cnt > 0 ? sizes[cnt - 1] : 0);
    }
    delete[] sizes;
    if (cnt < jlen)
    {
        GetTextExtentPoint32W(c->Handle, jp + cnt, 1, &s);
        if (selection && x > s.cx / 2)
            cnt++;
    }
    else if (limit && x > 2)
        return -1;

    return cnt;
}
//---------------------------------------------------------------------------
void KanjiToGroup(TKanjiItemList*);
void GenerateWordGroup(TKanjiList*);
void TLineObj::popupcallback(TZMenuRows *rows, int col, int row)
{
    TZMenuRow *r = rows->Rows[row];
    TZPopupCommand cmd = (TZPopupCommand)r->Data[col];

    if (cmd == pcNone)
        return;

    TKanjiItemList *kl = NULL;

    try
    {
        switch (cmd)
        {
        case pcKanjiInfo:
            ShowKanjiInfo(dictionary->Index[fcoll], popkanji);
            break;
        case pcTranslate:
            Translate(poptranslate);
            break;
        case pcCopy1:
            ClipReplace(poptext);
            break;
        case pcAppend1:
            ClipAppend(poptext);
            break;
        case pcCopy2:
            ClipReplace(poptext2);
            break;
        case pcAppend2:
            ClipAppend(poptext2);
            break;
        case pcToGroup:
            kl = new TKanjiItemList(false);
            kl->Add(poptranslate);
            KanjiToGroup(kl);
            break;
        case pcCollect:
            kl = new TKanjiItemList(false);
            kl->Add(poptranslate);
            GenerateWordGroup(kl);
            break;
        }
    }
    catch (...)
    {
        ;
    }

    delete kl;
}
//---------------------------------------------------------------------------
void TLineObj::SelectPopup(TPoint p, UnicodeString str)
{
    poptext = str;
    poptranslate = str;

    if (poptext.Length() && KANJI(poptext[1]))
        popkanji = kanjis->Kanjis[kanjis->IndexOf(poptext[1])];
    else
        popkanji = NULL;

    TZMenuRow *row = menurows->Rows[0];
    row->Enabled[1] = poptext.Length() == 1 && KANJI(poptext[1]);
    menurows->Rows[2]->Visible = has_kanji(poptext.c_str());

    menu->Popup(menurows, p.x, p.y, &popupcallback);
}
//---------------------------------------------------------------------------
void TLineObj::SentencePopup(TPoint p)
{
    TExampleData *d = fdata->examples + fdatapos;
    TExampleSentence *es = Examples->sentences[d->block][d->num];

    poptext = es->japanese;
    poptranslate = es->japanese;
    poptext2 = es->translation;

    sentencerows->Rows[0]->Visible = fmode != lmTranslated;
    sentencerows->Rows[1]->Visible = fmode != lmJapanese;
    sentencerows->Rows[2]->Visible = fmode != lmTranslated && has_kanji(poptext.c_str());

    menu->Popup(sentencerows, p.x, p.y, &popupcallback);
}
//---------------------------------------------------------------------------
void TLineObj::KanjiPopup(TPoint p, wchar_t k)
{
    TExampleData *d = fdata->examples + fdatapos;
    TExampleSentence *es = Examples->sentences[d->block][d->num];

    popkanji = kanjis->Kanjis[kanjis->IndexOf(k)];
    poptext = es->japanese;
    poptext2 = k;
    poptranslate = k;

    menu->Popup(kanjirows, p.x, p.y, &popupcallback);
}
//---------------------------------------------------------------------------
void __fastcall TLineObj::pbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    TPoint p = pb->ClientToScreen(Point(x, y));
    if (Button == mbLeft)
    {
        if (fselecting)
        {
            fselecting = false;
            if (fsellength)
            {
                TExampleData *d = fdata->examples + fdatapos;
                TExampleSentence *es = Examples->sentences[d->block][d->num];
                SelectPopup(p, ((UnicodeString)(es->japanese + MIN(fselstart, fselstart + fsellength))).SetLength(abs(fsellength)));
            }
            fselstart = -1;
            fsellength = 0;
            dobj->Selecting = false;

            if ((((TForm*)pb->Owner)->Active || ((TForm*)pb->Owner)->Parent && ((TForm*)((TForm*)pb->Owner)->Parent)->Active) && PtInRect(pb->ClientRect, Point(x, y)))
            {
                dobj->Hovering = true;
                pbMouseMove(Sender, TShiftState(), x, y);
                if (!fbox)
                    refresh();
            }
            else
                refresh();
        }
        else
        {
            if (fbox)
            {
                p = fbox->ScreenToClient(p);
                if (PtInRect(fbox->ClientRect, p))
                    fbox->FormMouseUp(Sender, Button, Shift, p.x, p.y);
            }
            if (dobj->Highlighted >= 0 && (!fbox || !PtInRect(fbox->ClientRect, p)))
            {
                TRect r = dobj->Areas[dobj->Highlighted];
                r.Left -= 2, r.Right += 2, r.Top-- , r.Bottom++;
                if (PtInRect(r, Point(x + scroller->Position, y)))
                    BoxMouseSelect(0);
            }
        }
    }
    else if (Button == mbRight && fdata)
    {
        int ix = 0;
        x += scroller->Position;

        if (fmode != lmTranslated && dobj->Count && dobj->Areas[0].Top <= y && dobj->Areas[0].Bottom > y)
        {
            while (ix < dobj->Count && dobj->Areas[ix].Right < x)
                ix++;
        }
        if (fmode == lmTranslated || ix == dobj->Count || dobj->Areas[0].Top > y || dobj->Areas[0].Bottom <= y)
        {
            SentencePopup(p);
            return;
        }

        TCanvas *c = pb->Canvas;
        TSize s;
        TExampleData *d = fdata->examples + fdatapos;
        TExampleSentence *es = Examples->sentences[d->block][d->num];

        c->Font->Name = settings.kanafont;
        c->Font->Charset = settings.kanacharset;
        c->Font->Size = double(fmode == lmJapanese ? scroller->ClientHeight * 0.4 : rowsizes(2) * 0.8) / scaleres;

        wchar_t *jp = es->japanese + dobj->Positions[ix];
        int jlen = dobj->Lengths[ix];
        int *sizes = new int[jlen];
        int cnt = 0;
        GetTextExtentExPointW(c->Handle, jp, jlen, x - dobj->Areas[ix].Left + 1, &cnt, sizes, &s);
        delete[] sizes;

        jp = es->japanese;
        jlen = wcslen(jp);
        if (dobj->Positions[ix] + cnt < jlen)
        {
            wchar_t k = jp[dobj->Positions[ix] + cnt];
            if (!KANJI(k))
            {
                SentencePopup(p);
                return;
            }
            KanjiPopup(p, k);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TLineObj::pbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button == mbLeft)
    {
        if (Shift.Contains(ssDouble))
        {
            int i = word_position(x + scroller->Position, y, true, false);
            if (i >= 0)
            {
                TExampleData *d = fdata->examples + fdatapos;
                TExampleSentence *es = Examples->sentences[d->block][d->num];
                wchar_t c = es->japanese[i];
                if (KANJI(c))
                {
                    HideBox(true);
                    dobj->Highlight(-1, -1);
                    dobj->Hovering = false;
                    ShowKanjiInfo(dictionary->Index[fcoll], kanjis->Kanjis[kanjis->IndexOf(c)]);
                    refresh();
                }
            }
        }
        else
        {
            fselstart = word_position(x + scroller->Position, y, true);
            fsellength = 0;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TLineObj::pbMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    if (!fdata || !((TForm*)pb->Owner)->Active && (!((TForm*)pb->Owner)->Parent || !((TForm*)((TForm*)pb->Owner)->Parent)->Active))
        return;

    int p;

    if (Shift.Contains(ssLeft))
    {
        if (fselstart >= 0)
        {
            p = word_position(x + scroller->Position, y, false);
            if (!fselecting && p != fselstart)
            {
                fselecting = true;
                dobj->Selecting = true;
                HideBox(true);
            }
        }
        if (fselecting)
        {

            if (fsellength != p - fselstart)
            {
                fsellength = p - fselstart;

                dobj->SelStart = MIN(fselstart, fselstart + fsellength);
                dobj->SelLength = abs(fsellength);
                refresh();
            }

        }
        else if (fbox)
        {
            TPoint p = Point(x, y);
            p = fbox->ScreenToClient(pb->ClientToScreen(p));
            if (PtInRect(fbox->ClientRect, p))
                fselstart = -1;
            fbox->FormMouseMove(Sender, Shift, p.x, p.y);
        }
        return;
    }

    int h = dobj->Highlighted;
    dobj->Highlight(x + scroller->Position, y);

    if (h != dobj->Highlighted)
    {
        if (dobj->Highlighted >= 0)
            ShowBox();
        else
            HideBox();

        refresh();
    }
}
//---------------------------------------------------------------------------
void TLineObj::ShowBox()
{
    HideBox(true);

    TExampleData *d = fdata->examples + fdatapos;
    TExampleSentence *es = Examples->sentences[d->block][d->num];
    int oldpos = es->words[dobj->Indexes[dobj->Highlighted]].position;

    TWordIndexList *l = new TWordIndexList(fcoll);
    TWordIndexList *l2 = new TWordIndexList(fcoll);

    try
    {
        TExampleWordData *wd;
        for (int i = dobj->Indexes[dobj->Highlighted]; i < es->wordscnt && oldpos == es->words[i].position; i++)
        {
            wd = es->words + i;

            int bix = wd->data ? FindWordIndex(l2, wd->data->kanji/*dictionary->Items[0]->words->Items[wd->data->wordindex]->kanji*/, wd->data->kana/*dictionary->Items[0]->words->Items[wd->data->wordindex]->kana*/) : -1;
            l2->Clear();

            //int bix = dictionary->ConvertIndex(NULL,wd->index,fcoll);
            if (bix >= 0)
                l->Add(bix);
        }

        if (l->Count == 1 && !has_kanji(l->Words[0]->kanji) && !wcsncmp(es->japanese + wd->position, l->Words[0]->kanji, wd->length))
        {
            delete l;
            l = NULL;
        }
        else
        {
            TPoint p = Point(0, 0);
            ::ClientToScreen(pb->Handle, &p);
            TRect r = dobj->Areas[dobj->Highlighted];
            OffsetRect(&r, p.x - scroller->Position, p.y);
            r.Left -= 2, r.Right += 2, r.Top-- , r.Bottom++;

            r.Left = MAX(p.x - 1, r.Left);
            r.Top = MAX(p.y, r.Top);
            p = pb->ClientToScreen(Point(pb->ClientWidth, pb->ClientHeight));
            r.Right = MIN(p.x + 1, r.Right);
            r.Bottom = MIN(p.y, r.Bottom);

            fbox = new TfWordBox(Application);
            try
            {
                fbox->ShowEx(this, l, r);
            }
            catch (...)
            {
                delete fbox;
                fbox = NULL;
            }
        }
    }
    catch (...)
    {
    }

    delete l;
    delete l2;

    dobj->GradReverse = (fbox && fbox->Position.Contains(bpTop));

}
//---------------------------------------------------------------------------
void TLineObj::HideBox(bool force)
{
    if (!fbox)
        return;

    TPoint p;
    GetCursorPos(&p);
    p = fbox->ScreenToClient(p);

    if (!force && AppActive && PtInRect(fbox->ClientRect, p))
        return;

    fbox->Visible = false;

    dobj->GradReverse = false;

    fbox->Release();
    fbox = NULL;
}
//---------------------------------------------------------------------------
void TLineObj::BoxMouseLeave()
{
    TPoint p;
    GetCursorPos(&p);
    if (WindowFromPoint(p) == pb->Handle)
        return;

    HideBox(true);
    dobj->Highlight(-1, -1);
    dobj->Hovering = false;
    refresh();
}
//---------------------------------------------------------------------------
void TLineObj::BoxMouseSelect(byte ix)
{
    TExampleData *d = fdata->examples + fdatapos;
    TExampleSentence *es = Examples->sentences[d->block][d->num];

    if (es->words[dobj->Indexes[dobj->Highlighted] + ix].data)
    {
        //TWord *w = dictionary->Items[0]->words->Items[ es->words[dobj->Indexes[dobj->Highlighted]+ix].data->wordindex ];
        TStoredWord *sw = es->words[dobj->Indexes[dobj->Highlighted] + ix].data;
        fwordindex = sw ? FindWordIndex(fcoll, sw->kanji, sw->kana) : -1;
    }
    //fwordindex = dictionary->ConvertIndex(dictionary->Items[0],es->words[dobj->Indexes[dobj->Highlighted]+ix].index,fcoll);
    if (fcallback)
        fcallback();
}
//---------------------------------------------------------------------------
char TLineObj::get_highlighted()
{
    return dobj->Highlighted;
}
//---------------------------------------------------------------------------
int TLineObj::get_count()
{
    if (fdata)
        return fdata->examplecnt;
    return 0;
}
//---------------------------------------------------------------------------
void TLineObj::Lock(bool setlock)
{
    if (flocked == setlock)
        return;
    flocked = setlock;

    if (!flocked)
    {
        fwordindex = -1;
        if (fcallback)
            fcallback();
    }
}
//---------------------------------------------------------------------------
void TLineObj::scallback()
{
    pb->Refresh();
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(95);
#endif

