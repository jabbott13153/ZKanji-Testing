//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Themes.hpp>
#include <vsstyle.h>

#include "boxobj.h"
#include "settings.h"
#include "kanji.h"
#include "collection.h"
#include "kanjicard.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

#define CHECKBOX_SIZE 14

#ifdef _DEBUG
static TGlobalTest tester1(10);
#endif

//---------------------------------------------------------------------------
TReadingBoxObj::TReadingBoxObj(TZPaintBox *abox, int arowsize, TFuncPtr acallback) : inherited(abox, arowsize, true, acallback)
{
    int cw[2] = { 19, 9999 };
    set_cols(2, cw, NULL, NULL, 0);
    colresizing = false;

    selecttype = wlsSingleSelect;
    vertline = false;
    fixvertbar = true;

    fmouseon = -1;
    fmousedown = false;

    fkanji = NULL;
    fcallbackkanji = NULL;
}
//---------------------------------------------------------------------------
TReadingBoxObj::~TReadingBoxObj()
{
}
//---------------------------------------------------------------------------
void TReadingBoxObj::set_collection(TWordCollection *acoll)
{
    if (fcollection == acoll)
        return;
    fcollection = acoll;
    kanji = NULL;
}
//---------------------------------------------------------------------------
void TReadingBoxObj::DoDrawHead(int col, TRect r)
{
    switch (col)
    {
    case 0:
        DrawHeadGen(col, r, "");
        break;
    case 1:
        DrawHeadGen(col, r, "Readings");
        break;
    }
}
//---------------------------------------------------------------------------
void TReadingBoxObj::DoSettingsChanged(TSettingsProcState state)
{
    if (state != spsEnd)
        return;

    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TReadingBoxObj::DoPrepareDraw(int col, int row)
{
    inherited::DoPrepareDraw(col, row);

    TCanvas *c = Canvas;

    int real = real_index(row);
    int first = first_index(fkanji, real);
    TKanjiCard *kc = collection->kanjidat[fkanji->index].card;
    int comp = compact_index(row);

    if (kc->ReadingExampleCount[comp] && real == first)
        c->Brush->Color = coloravg((row == 0 ? (TColor)0xdddddd : row <= fkanji->oncnt ? (TColor)0xcdcdff : (TColor)0xffdfcf), c->Brush->Color, 0.5);

    if (col)
    {
        c->Font->Name = settings.kanafont;
        c->Font->Charset = settings.kanacharset;
        c->Font->Size = double(rowsize) * 0.66 / scaleres;
    }
}
//---------------------------------------------------------------------------
void TReadingBoxObj::DoDrawCell(int col, int row, TRect r)
{
    TCanvas *c = Canvas;

    int real = real_index(row);
    int first = first_index(fkanji, real);
    TKanjiCard *kc = collection->kanjidat[fkanji->index].card;
    int comp = compact_index(row);

    if (col == 0)
    {
        if (!row)
            c->FillRect(r);
        else
            draw_checkbox(r, first == real && kc->Checked[comp], (first != real ? csDisabled : fmouseon == row ? fmousedown && fmouseover ? csPushed : csHot : csNormal));
    }
    else
    {
        c->Pen->Color = row == 0 ? clBlack : row <= fkanji->oncnt ? (TColor)0x004070ff : first != real ? (TColor)0x00f0e0d0 : (TColor)0x00ffa030;
        c->MoveTo(r.Left, r.Top);
        c->LineTo(r.Left, r.Bottom);
        c->MoveTo(r.Left + 1, r.Top);
        c->LineTo(r.Left + 1, r.Bottom);

        UnicodeString wstr = (!row ? (UnicodeString)fkanji->ch : comp <= fkanji->oncnt ? (UnicodeString)fkanji->on[comp - 1] : (UnicodeString)fkanji->kun[real - fkanji->oncnt - 1]);
        UnicodeString head;
        int p = wstr.Pos(L'.');
        if (p)
        {
            head = wstr.SubString(1, p - 1);
            wstr = wstr.SubString(p + 1, wstr.Length() - p);
        }
        else
            head = wstr;

        r.Left += 2;
        ExtTextOutW(c->Handle, r.Left + 2, r.Top + r.Height()*0.86, ETO_CLIPPED | ETO_OPAQUE, &r, head.c_str(), head.Length(), NULL);

        if (!fgrouping && p)
        {
            TSize s;
            GetTextExtentPointW(c->Handle, head.c_str(), head.Length(), &s);
            TColor clr = (TColor)0xffb000;
            fixtextcolor(clr, c->Brush->Color);
            c->Font->Color = clr;

            r.Left += 2 + s.cx;
            ExtTextOutW(c->Handle, r.Left, r.Top + r.Height()*0.86, ETO_CLIPPED | ETO_OPAQUE, &r, wstr.c_str(), wstr.Length(), NULL);

        }

    }
}
//---------------------------------------------------------------------------
int TReadingBoxObj::compact_index(int index)
{
    if (!fkanji)
        THROW(L"No kanji for compact reading index.");

    int ix = index >= 0 ? index : pos;
    return !fgrouping ? ::compact_index(fkanji, ix) : ix;
}
//---------------------------------------------------------------------------
int TReadingBoxObj::real_index(int index)
{
    if (!fkanji)
        THROW(L"No kanji for real reading index.");
    int ix = index >= 0 ? index : pos;
    return fgrouping ? ::real_index(fkanji, ix) : ix;
}
//---------------------------------------------------------------------------
TRect TReadingBoxObj::checkbox_rect(TRect r)
{
    return Rect(r.Left + r.Width() / 2 - CHECKBOX_SIZE / 2 + 1, r.Top + r.Height() / 2 - CHECKBOX_SIZE / 2 + 1, r.Left + r.Width() / 2 + CHECKBOX_SIZE / 2 + 1, r.Top + r.Height() / 2 + CHECKBOX_SIZE / 2 + 1);
}
//---------------------------------------------------------------------------
void TReadingBoxObj::draw_checkbox(TRect r, bool checked, TCheckState state, bool fillbg)
{
    TCanvas *c = fbox->Canvas;

    TRect r2 = checkbox_rect(r);
    if (fillbg)
    {
        c->FillRect(Rect(r.Left, r.Top, r.Right, r2.Top));
        c->FillRect(Rect(r.Left, r2.Top, r2.Left, r2.Bottom));
        c->FillRect(Rect(r2.Right - 1, r2.Top, r.Right, r2.Bottom));
        c->FillRect(Rect(r.Left, r2.Bottom - 1, r.Right, r.Bottom));
    }

    r = r2;

    int prevta = SetTextAlign(c->Handle, TA_TOP | TA_LEFT);
    TThemeServices *t = ThemeServices();
    if (!t || !t->ThemesAvailable || !t->ThemesEnabled)
        DrawFrameControl(c->Handle, &r, DFC_BUTTON, DFCS_BUTTONCHECK/*| DFCS_FLAT*/ | (checked ? DFCS_CHECKED : 0) | (state == csHot ? DFCS_HOT : 0) | (state == csDisabled ? DFCS_INACTIVE : 0) | (state == csPushed ? DFCS_PUSHED : 0));
    else
    {
        int boxstates[2][4] = {
            { CBS_UNCHECKEDNORMAL, CBS_UNCHECKEDHOT, CBS_UNCHECKEDPRESSED, CBS_UNCHECKEDDISABLED },
            { CBS_CHECKEDNORMAL, CBS_CHECKEDHOT, CBS_CHECKEDPRESSED, CBS_CHECKEDDISABLED }
        };

        TThemedElementDetails tb;
        tb.Element = teButton;
        tb.Part = BP_CHECKBOX;
        tb.State = boxstates[checked][state];
        t->DrawElement(c->Handle, tb, r, NULL);
    }
    SetTextAlign(c->Handle, prevta);
}
//---------------------------------------------------------------------------
void TReadingBoxObj::update_count(int &count)
{
    if (!fkanji)
        count = 0;
    else
        count = fgrouping ? compact_count(fkanji) : 1 + fkanji->oncnt + fkanji->kuncnt;
}
//---------------------------------------------------------------------------
void TReadingBoxObj::count_count()
{
    int acount;
    update_count(acount);
    if (count == acount)
        invalidate();
    else
        count = acount;
}
//---------------------------------------------------------------------------
void TReadingBoxObj::set_kanji(TKanji *akanji)
{
    fkanji = akanji;

    fbox->TabStop = fkanji;

    count_count();

    if (pos != 0)
        pos = 0;
    else
        DoCallback();
}
//---------------------------------------------------------------------------
void TReadingBoxObj::DoCallback()
{
    int nreading = fkanji ? compact_index(pos) : -1;

    if (fkanji != fcallbackkanji || fcallbackreading != nreading)
    {
        fcallbackkanji = fkanji;
        fcallbackreading = nreading;
        inherited::DoCallback();
    }
}
//---------------------------------------------------------------------------
void TReadingBoxObj::invalidate()
{
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TReadingBoxObj::set_grouping(bool val)
{
    if (fgrouping == val)
        return;

    int oldpos = pos;

    fgrouping = val;
    count_count();

    if (val)
        pos = ::compact_index(fkanji, oldpos);
    else
        pos = ::real_index(fkanji, oldpos);
}
//---------------------------------------------------------------------------
int TReadingBoxObj::sel_find_up(int newsel)
{
    if (!fkanji || newsel <= fkanji->oncnt + 1)
        return newsel;

    while (newsel > fkanji->oncnt + 1)
    {
        UnicodeString a, b;
        a = fkanji->kun[newsel - fkanji->oncnt - 1];
        b = fkanji->kun[newsel - fkanji->oncnt - 2];

        if (a.Pos(L'.'))
            a = a.SubString(1, a.Pos(L'.') - 1);
        if (b.Pos(L'.'))
            b = b.SubString(1, b.Pos(L'.') - 1);

        if (a != b)
            break;
        newsel--;
    }
    return newsel;
}
//---------------------------------------------------------------------------
void TReadingBoxObj::DoMouseLeave()
{
    if (fmouseon < 0 || fmousedown)
    {
        if (!fmousedown)
            set_mouseon(-1);
    }
    else
        set_mouseon(-1);
    inherited::DoMouseLeave();
}
//---------------------------------------------------------------------------
void TReadingBoxObj::DoMouseMove(int acol, int arow, TShiftState Shift, int x, int y)
{
    cellpos(TPoint(x, y), acol, arow);
    if (arow <= 0 || arow >= count || acol != 0)
        set_mouseon(-1);
    else
    {
        TRect r = cellbox(0, arow);
        r.bottom--;
        r = checkbox_rect(r);
        if (PtInRect(r, Point(x, y)))
            set_mouseon(arow);
        else
            set_mouseon(-1);
    }

    inherited::DoMouseMove(acol, arow, Shift, x, y);
}
//---------------------------------------------------------------------------
void TReadingBoxObj::DoMouseDown(int acol, int arow, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (fkanji && arow >= 0 && arow < count && acol == 0 && Button == mbLeft && fmouseon > 0)
    {
        TRect r;
        TKanjiCard *kc = collection->kanjidat[fkanji->index].card;
        int comp = compact_index(fmouseon);
        fmousedown = true;
        fmouseover = true;
        r = cellbox(0, fmouseon);
        r.bottom--;
        draw_checkbox(r, kc->Checked[comp], csPushed, false);
    }

    inherited::DoMouseDown(acol, arow, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void TReadingBoxObj::DoMouseUp(int acol, int arow, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (fmousedown)
    {
        fmousedown = false;
        TKanjiCard *kc = collection->kanjidat[fkanji->index].card;
        int comp = compact_index(fmouseon);
        TRect r = cellbox(0, fmouseon);
        r.bottom--;
        if (fmouseover)
        {
            MODON
            kc->Checked[comp] = !kc->Checked[comp];
            collection->Modify(false);
            MODOFF
        }
        draw_checkbox(r, kc->Checked[comp], (fmouseover ? csHot : csNormal), false);
    }
    inherited::DoMouseUp(acol, arow, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void TReadingBoxObj::DoDeactivate(bool focused)
{
    if (fmousedown)
    {
        fmousedown = false;
        fmouseover = false;
        set_mouseon(-1);
    }

    inherited::DoDeactivate(focused);
}
//---------------------------------------------------------------------------
void TReadingBoxObj::set_mouseon(int on)
{
    if (!fmousedown && on == fmouseon)
        return;

    TKanjiCard *kc = collection->kanjidat[fkanji->index].card;
    int comp;

    TRect r;

    if (fmouseon >= 0)
    {
        comp = compact_index(fmouseon);
        r = cellbox(0, fmouseon);
        r.bottom--;
        fmouseover = fmouseon && on == fmouseon;
        draw_checkbox(r, kc->Checked[comp], (fmousedown ? (on == fmouseon ? csPushed : csHot) : csNormal), false);
    }

    if (fmousedown)
        return;

    if (on > 0 && (fgrouping || first_index(fkanji, on) == on))
    {
        fmouseon = on;
        comp = compact_index(fmouseon);
        r = cellbox(0, fmouseon);
        r.bottom--;
        draw_checkbox(r, kc->Checked[comp], csHot, false);
    }
    else
        fmouseon = -1;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(11);
#endif

