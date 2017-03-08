//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanjihint.h"
#include "unicodehint.h"
#include "settings.h"
#include "kanji.h"
#include "collection.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(86);
#endif

//---------------------------------------------------------------------------
TKanjiHint *kanjihint = 0;

void HideKanjiHint(bool reset)
{
    if (!kanjihint)
        return;
    kanjihint->Hide(reset);
}
void HideKanjiHintNow()
{
    if (!kanjihint)
        return;
    kanjihint->Hide(true, true);
}
void HideOwnedKanjiHint(TControl *form)
{
    if (!kanjihint || !form || !kanjihint->Control)
        return;
    if (kanjihint->Control == form || kanjihint->Control->Owner == form)
        HideKanjiHint(true);
}
void ShowKanjiHint(TControl *Sender, int ax, int ay, wchar_t akanji, TWordCollection *acoll)
{
    if (!kanjihint)
        return;
    kanjihint->Show(Sender, ax, ay, akanji, acoll);
}

bool KanjiHintVisible()
{
    return kanjihint->Visible;
}

void CALLBACK timeproc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    kanjihint->TimePassed();
}
//---------------------------------------------------------------------------
TKanjiHint::TKanjiHint() : timeid(0), fvisible(false), kanjich(0), coll(NULL), control(NULL), bmp(NULL), Kfont(NULL)
{
    SettingsChangeEvent(this, settingsChanged, true);
}
//---------------------------------------------------------------------------
TKanjiHint::~TKanjiHint()
{
    SettingsChangeEvent(this, settingsChanged, false);
    StopShowHint();
    if (Kfont)
        DeleteObject(Kfont);
    Kfont = 0;
}
//---------------------------------------------------------------------------
void TKanjiHint::settingsChanged(void *caller, TSettingsProcState state)
{
    if (state != spsEnd)
        return;

    if (Kfont)
        DeleteObject(Kfont);
    Kfont = 0;
}
//---------------------------------------------------------------------------
void TKanjiHint::Show(TControl *Sender, int ax, int ay, wchar_t akanji, TWordCollection *acoll)
{
    TPoint p = Sender->ClientToScreen(Point(ax, ay));

    if (Sender == control && akanji == kanjich && coll == acoll)
    {
        if (!fvisible)
            fx = p.x, fy = p.y;
        return;
    }

    StopShowHint();

    if (!akanji)
        return;

    kanjich = akanji;
    coll = acoll;
    control = Sender;
    fx = p.x;
    fy = p.y;

    timeid = SetTimer(NULL, 0, Application->HintPause, (void(__stdcall *)(HWND, UINT, UINT_PTR, DWORD)) & timeproc);
}
//---------------------------------------------------------------------------
void TKanjiHint::Hide(bool reset, bool immediate)
{
    StopShowHint(!reset, immediate);
}
//---------------------------------------------------------------------------
void TKanjiHint::StopShowHint(bool inner, bool immediate)
{
    if (!inner)
        kanjich = 0;

    if (!fvisible)
    {
        if (timeid)
        {
            KillTimer(NULL, timeid);
            timeid = 0;
        }
        return;
    }

    hint->Hide(immediate);
    fvisible = false;

    delete bmp;
    bmp = NULL;

    if (!timeid)
        return;
    KillTimer(NULL, timeid);
    timeid = 0;
}
//---------------------------------------------------------------------------
void TKanjiHint::TimePassed()
{
    KillTimer(NULL, timeid);
    timeid = 0;

    if (fvisible)
    {
        StopShowHint();
        return;
    }

    if (!kanjich)
        return;

    fvisible = true;
    if (settings.hide_kanji_hint)
        timeid = SetTimer(NULL, 0, settings.kanji_hint_time * 1000, (void(__stdcall *)(HWND, UINT, UINT_PTR, DWORD)) & timeproc);

    //show timer
    TRect r;
    fpaintproc(NULL, hint->Canvas, r, &r);
    OffsetRect(r, fx, fy);
    hint->Popup(r, fpaintproc, NULL);
}
//---------------------------------------------------------------------------
int DrawReadings(TCanvas *c, wchar_t **arr, int cnt, const TRect &r, bool draw, int *lw = NULL)
{
    int h = -2;
    TSize s;
    int w = r.Width();
    int x = 0, y = 0;

    if (lw)
        * lw = 0;

    int comma;
    GetTextExtentPoint32W(c->Handle, L",", 1, &s);
    comma = s.cx;
    int l, k;

    for (int i = 0; i < cnt; i++)
    {
        if (i == cnt - 1)
            comma = 0;

        wchar_t *dpos = wcschr(arr[i], L'.');
        if (dpos)
        {
            GetTextExtentPoint32W(c->Handle, arr[i], dpos - arr[i], &s);
            l = s.cx;
            k = s.cy;
            GetTextExtentPoint32W(c->Handle, dpos + 1, wcslen(dpos + 1), &s);
            l += s.cx;
            k = MAX(k, (int)s.cy);
        }
        else
        {
            GetTextExtentPoint32W(c->Handle, arr[i], wcslen(arr[i]), &s);
            l = s.cx;
            k = s.cy;
        }

        if (x && x + l + comma > w)
        {
            x = 0;
            y = h + 2;
        }
        if (!x)
            h += k + 2;

        if (draw)
        {
            TRect r2;
            if (dpos)
            {
                c->Font->Color = clBlack;
                r2 = Rect(r.Left + x, r.top + y, r.Left + x + l - s.cx, r.top + y + k);
                ExtTextOutW(c->Handle, r2.Left, r2.Top, ETO_CLIPPED/*| ETO_OPAQUE*/, &r2, arr[i], dpos - arr[i], NULL);
                c->Font->Color = (TColor)0xffb000;
                r2 = Rect(r2.Right, r2.top, r2.Right + s.cx, r2.bottom);
                ExtTextOutW(c->Handle, r2.Left, r2.Top, ETO_CLIPPED/*| ETO_OPAQUE*/, &r2, dpos + 1, wcslen(dpos + 1), NULL);
                c->Font->Color = clBlack;
            }
            else
            {
                r2 = Rect(r.Left + x, r.top + y, r.Left + x + l, r.top + y + k);
                ExtTextOutW(c->Handle, r2.Left, r2.Top, ETO_CLIPPED/*| ETO_OPAQUE*/, &r2, arr[i], wcslen(arr[i]), NULL);
            }
            if (comma)
            {
                r2 = Rect(r.Left + x + l, r.top + y, r.Left + x + l + comma, r.top + y + k);
                ExtTextOutW(c->Handle, r2.Left, r2.Top, ETO_CLIPPED/*| ETO_OPAQUE*/, &r2, L",", 1, NULL);
            }
        }
        x += l + comma;
        if (lw)
            * lw = MAX(x, *lw);
    }
    return h;
}

extern wchar_t *infostr[KANJI_REFERENCE_COUNT];
void TKanjiHint::fpaintproc(TObject *Sender, TCanvas *Canvas, TRect area, void *data)
{
    bool measure = data;

    int bmpwidth = 201 * scaleres;

    int kix = kanjich ? kanjis->IndexOf(kanjich) : -1;
    if (kix < 0)
    {
        Hide(true, true);
        return;
    }

    TKanji *k = kanjis->Kanjis[kix];

    int h;
    UnicodeString z;
    UnicodeString str;
    TRect r = Rect(0, 0, (bmpwidth - 11 * scaleres) / 2, 1 * scaleres);

    Canvas->Font->Name = settings.kanafont;
    Canvas->Font->Charset = settings.kanacharset;
    Canvas->Font->Size = 8;

    int kunwidth, onwidth, kunheight, onheight;
    onheight = DrawReadings(Canvas, k->on, k->oncnt, r, false, &onwidth);
    kunheight = DrawReadings(Canvas, k->kun, k->kuncnt, r, false, &kunwidth);
    h = MAX(onheight, kunheight);
    if (onheight >= kunheight)
    {
        r.right = bmpwidth - 11 * scaleres - kunwidth;
        onheight = DrawReadings(Canvas, k->on, k->oncnt, r, false, &onwidth);
        if (onheight == h)
            kunwidth = onwidth = bmpwidth / 2 - 4 * scaleres;
        else
            kunwidth = bmpwidth - 11 * scaleres - onwidth;
        //r = Rect(0,0,kunwidth,1);
        //kunheight = DrawReadings(Canvas,k->kun,k->kuncnt,r,false,&kunwidth);
    }
    else
    {
        r.right = bmpwidth - 11 * scaleres - onwidth;
        kunheight = DrawReadings(Canvas, k->kun, k->kuncnt, r, false, &kunwidth);
        if (kunheight == h)
            kunwidth = onwidth = bmpwidth / 2 - 4 * scaleres;
        else
            onwidth = bmpwidth - 11 * scaleres - kunwidth;
        //r = Rect(0,0,onwidth,1);
        //onheight = DrawReadings(Canvas,k->on,k->oncnt,r,false,&onwidth);
    }
    h = max(onheight, kunheight);

    Canvas->Font->Name = SystemFont;
    Canvas->Font->Charset = DEFAULT_CHARSET;
    Canvas->Font->Size = 8;
    r = Rect(0, 0, bmpwidth - 6 * scaleres, 1 * scaleres);
    str = k->meaning;
    if (coll != dictionary->Items[0] && coll->kanjidat[k->index].meaning)
        str = coll->kanjidat[k->index].meaning;
    DrawTextW(Canvas->Handle, str.c_str(), str.Length(), &r, DT_CALCRECT | DT_LEFT | DT_NOPREFIX | DT_TOP | DT_WORDBREAK);

    int bmpheight = 32 * scaleres + 4 * scaleres + h + 4 * scaleres + r.Height() + 4 * scaleres;
    if (measure)
    {
        *(TRect*)data = Rect(0, 0, bmpwidth, bmpheight);
        return;
    }

    bmp = new TBmp;
    bmp->Canvas->Brush->Assign(Canvas->Brush);
    bmp->Canvas->Brush->Color = clWhite;
    bmp->SetSize(bmpwidth, bmpheight);
    TCanvas *c = bmp->Canvas;

    BitBlt(c->Handle, 0, 0, bmp->Width, bmp->Height, Canvas->Handle, area.Left, area.Top, SRCCOPY);

    c->Pen->Color = clSilver;
    c->Font->Color = clBlack;

    /*TPoint points[6] = { Point(0,0), Point(bmp->Width-25*scaleres,0), Point(bmp->Width-25*scaleres,6*scaleres), Point(bmp->Width-1*scaleres,6*scaleres),
     Point(bmp->Width-1*scaleres,bmp->Height-1*scaleres), Point(0,bmp->Height-1*scaleres) };
     c->Polygon(points,5);
     */
    c->Brush->Style = bsClear;

    c->Pen->Color = COLORINC(clSilver, 0x1a1a1a);

    SetTextAlign(c->Handle, TA_CENTER | TA_BASELINE);
    c->Font->Name = settings.kanjifont;
    c->Font->Charset = settings.kanjicharset;
    c->Font->Size = 23;
    r = Rect(2 * scaleres, 2 * scaleres, 30 * scaleres, 30 * scaleres);

    HFONT oldf = 0;
    if (settings.kanjiantialias)
    {
        oldf = c->Font->Handle;
        if (!Kfont)
            Kfont = create_localfont(c->Font);
        SelectObject(c->Handle, Kfont);
    }
    ExtTextOutW(c->Handle, r.Left + r.Width() / 2, r.Top + r.Height()*0.9, ETO_CLIPPED/*| ETO_OPAQUE*/, &r, &kanjich, 1, NULL);
    if (oldf)
        SelectObject(c->Handle, oldf);

    c->MoveTo(2*scaleres, 31*scaleres);
    c->LineTo(31*scaleres, 31*scaleres);
    c->LineTo(31*scaleres, 1*scaleres);

    c->Font->Name = SystemFont;
    c->Font->Size = 8;
    c->Font->Charset = DEFAULT_CHARSET;

    switch (settings.reference_index)
    {
    case kdiJouyou:
        str = k->jouyou <= 10 ? (k->jouyou < 9 ? IntToStr(k->jouyou) : (UnicodeString)L"名" + (k->jouyou)) : (UnicodeString)L"-";
        break;
    case kdiJLPTold:
        str = k->JLPT;
        break;
    case kdiSnH:
        str = (k->SnH[0] != 'x' ? k->SnH : 0);
        break;
    case kdiKnK:
        str = k->knk;
        break;
    case kdiNelson:
        str = k->nelson;
        break;
    case kdiHalpern:
        str = k->halpern;
        break;
    case kdiHeisig:
        str = k->heisig;
        break;
    case kdiGakken:
        str = k->gakken;
        break;
    case kdiHenshall:
        str = k->henshall;
        break;
    case kdiSKIP:
        str = (UnicodeString)k->SKIP1 + "-" + k->SKIP2 + "-" + k->SKIP3;
        break;
    case kdi4C:
        str = (k->c4[0] == 'x' ? (UnicodeString)"-" : (UnicodeString)k->c4 + "." + k->c4b);
        break;
    case kdiUnicode:
        str = wtoAS(k->_uni).UpperCase();
        break;
    case kdiJIS:
        str = wtoAS(k->_jis).UpperCase();
        break;
    case kdiSJIS:
        str = wtoAS(JIStoShiftJIS(k->_jis)).UpperCase();
        break;
    case kdiEUC:
        str = wtoAS(JIStoEUC(k->_jis)).UpperCase();
        break;
    case kdiFrequency:
        str = int(k->frequency);
        break;
    case kdiKuten:
        str = JIStoKuten(k->_jis);
        break;
    case kdiNJLPT:
        str = k->NJLPT ? UnicodeString(L"N") +int(k->NJLPT) : UnicodeString(L"-");
        break;
    }
    if (str == "0")
        str = "-";
    str = (UnicodeString)infostr[settings.reference_index] + " " + str;
    r = Rect(102 * scaleres, 2 * scaleres, bmp->Width - 26 * scaleres, 14 * scaleres);
    ExtTextOutW(c->Handle, r.Left + r.Width() / 2, r.Top + r.Height()*0.9, ETO_CLIPPED/*| ETO_OPAQUE*/, &r, str.c_str(), str.Length(), NULL);

    c->MoveTo(bmp->Width / 2 - 1*scaleres, 15*scaleres);
    c->LineTo(bmp->Width / 2 - 1*scaleres, 1*scaleres);

    str = (UnicodeString)"Grade: ";
    if (k->jouyou == 0)
        str += '-';
    else if (k->jouyou < 9)
        str += int(k->jouyou);
    else
        str += (UnicodeString)L"名" +int(k->jouyou);
    r = Rect(34 * scaleres, 2 * scaleres, bmp->Width / 2 - 3 * scaleres, 14 * scaleres);
    ExtTextOutW(c->Handle, r.Left + r.Width() / 2, r.Top + r.Height()*0.9, ETO_CLIPPED/*| ETO_OPAQUE*/, &r, str.c_str(), str.Length(), NULL);

    c->MoveTo(33*scaleres, 15*scaleres);
    c->LineTo(bmp->Width - 25*scaleres, 15*scaleres);
    c->LineTo(bmp->Width - 25*scaleres, 7*scaleres);

    SetTextAlign(c->Handle, TA_CENTER | TA_BASELINE);
    str = (UnicodeString)"Strokes: 00";

    r = Rect(35 * scaleres, 17 * scaleres, 99 * scaleres, 30 * scaleres);
    str = (UnicodeString)"Strokes: " + (int)k->strokes;
    ExtTextOutW(c->Handle, r.Left + r.Width() / 2, r.Top + r.Height()*0.9, ETO_CLIPPED/*| ETO_OPAQUE*/, &r, str.c_str(), str.Length(), NULL);

    c->MoveTo(33*scaleres, 31*scaleres);
    c->LineTo(99*scaleres, 31*scaleres);
    c->LineTo(99*scaleres, 16*scaleres);

    SetTextAlign(c->Handle, TA_RIGHT | TA_BASELINE);
    r = Rect(103 * scaleres, 18 * scaleres, bmp->Width - 27 * scaleres, 30 * scaleres);
    str = (UnicodeString)"Radical: " + (int)k->radical;
    ExtTextOutW(c->Handle, r.Right - 2*scaleres, r.Top + r.Height()*0.9, ETO_CLIPPED/*| ETO_OPAQUE*/, &r, str.c_str(), str.Length(), NULL);

    SetTextAlign(c->Handle, TA_CENTER | TA_BASELINE);
    SetRadicalFont(c->Font);
    c->Font->Size = 14;
    r = Rect(bmp->Width - 24 * scaleres, 8 * scaleres, bmp->Width - 2 * scaleres, 30 * scaleres);
    ExtTextOutW(c->Handle, r.Left + r.Width() / 2, r.Top + r.Height()*0.86, ETO_CLIPPED/*| ETO_OPAQUE*/, &r, &radicals[k->radical - 1], 1, 0);

    c->MoveTo(101*scaleres, 31*scaleres);
    c->LineTo(bmp->Width - 2*scaleres, 31*scaleres);
    c->MoveTo(2*scaleres, 33*scaleres);
    c->LineTo(bmp->Width - 2*scaleres, 33*scaleres);
    c->MoveTo(3*scaleres + onwidth, 35*scaleres);
    c->LineTo(3*scaleres + onwidth, 35*scaleres + h + 2*scaleres);

    SetTextAlign(c->Handle, TA_LEFT | TA_TOP);
    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = 8;
    c->Font->Color = clBlack;
    r = Rect(2 * scaleres, 36 * scaleres, 2 * scaleres + onwidth, 36 * scaleres + h);
    DrawReadings(c, k->on, k->oncnt, r, true);

    r = Rect(bmp->Width - 3 * scaleres - kunwidth, 36 * scaleres, bmp->Width - 3 * scaleres, 36 * scaleres + h);
    DrawReadings(c, k->kun, k->kuncnt, r, true);

    c->MoveTo(2*scaleres, 35*scaleres + h + 3*scaleres);
    c->LineTo(bmp->Width - 2*scaleres, 35*scaleres + h + 3*scaleres);
    c->MoveTo(2*scaleres, 35*scaleres + h + 5*scaleres);
    c->LineTo(bmp->Width - 2*scaleres, 35*scaleres + h + 5*scaleres);

    c->Font->Name = SystemFont;
    c->Font->Charset = DEFAULT_CHARSET;
    c->Font->Size = 8;
    r = Rect(3 * scaleres, 36 * scaleres + h + 6 * scaleres, bmp->Width - 3 * scaleres, bmp->Height - 1 * scaleres);
    str = k->meaning;
    if (coll != dictionary->Items[0])
    {
        if (coll->kanjidat[k->index].meaning)
            str = coll->kanjidat[k->index].meaning;
        else
            c->Font->Color = (TColor)0x00d08040;
    }
    DrawTextW(c->Handle, str.c_str(), str.Length(), &r, DT_LEFT | DT_NOPREFIX | DT_TOP | DT_WORDBREAK);

    Canvas->Draw(area.Left, area.Top, bmp);

    delete bmp;
    bmp = NULL;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(87);
#endif

