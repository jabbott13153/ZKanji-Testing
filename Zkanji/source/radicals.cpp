//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "radicals.h"
#include "settings.h"
#include "kanjilist.h"
#include "kanjiedit.h"
#include "romajizer.h"
#include "zrads.h"
#include "numedit.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(124);
#endif

TfRadicals *fRadicals;
//---------------------------------------------------------------------------

byte radcnt[17] = { 6, 29, 60, 94, 117, 146, 166, 175, 186, 194, 200, 204, 208, 210, 211, 212, 214 };
byte partcnt[17] = { 6, 36, 81, 125, 153, 178, 199, 211, 222, 232, 241, 245, 248, 251, 251, 251, 252 };

//---------------------------------------------------------------------------
__fastcall TfRadicals::TfRadicals(TComponent* Owner) : linecnt(0), Kfont(NULL), TBaseForm(Owner)
{
    EraseBackground = false;

    numStrokes = new TNumEdit(edStrokes, 1, 17);
    numStrokes->allow_empty = true;

    cellsize = 34 * scaleres;

    memset(selected, 0, sizeof(bool)*252);
    fselcount = 0;
    Width = (Width - pbRad->ClientWidth) + cellsize * 21;
    Height = (Height - pbRad->ClientHeight) + cellsize * 13 - 1;
    //Left = (Screen->WorkAreaWidth-Width) / 2;
    //Top = (Screen->WorkAreaHeight-Height) / 2;

    filter = 0;
    wid = NULL;
    halves = NULL;
    sel = NULL;
    ind = NULL;

    edkanji = new TKanjiEdit(edKanji, false, false, NULL, kcallback);
}
//---------------------------------------------------------------------------
__fastcall TfRadicals::~TfRadicals()
{
    delete[] wid;
    delete[] halves;
    delete[] sel;
    delete[] ind;
    delete edkanji;
    radlist->Reset();
    delete numStrokes;
    if (Kfont)
        DeleteObject(Kfont);
    Kfont = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::WMSizing(TWMSizing &msg)
{
    int base = Width - pbRad->ClientWidth + 1; //plus width of window
    int w = msg.lprc->Right - msg.lprc->Left + 1 - base; //new width
    if (w % cellsize < cellsize / 2)
        w -= w % cellsize;
    else
        w += (cellsize) - (w % cellsize);
    if (w < cellsize * 10)
        w = cellsize * 10;

    switch (msg.fwSide)
    {
    case (WMSZ_BOTTOMLEFT):
    case (WMSZ_TOPLEFT):
    case (WMSZ_LEFT):
        msg.lprc->Left = msg.lprc->Right - w + 1 - base;
        break;
    case (WMSZ_BOTTOMRIGHT):
    case (WMSZ_TOPRIGHT):
    case (WMSZ_RIGHT):
        msg.lprc->Right = msg.lprc->Left + w - 1 + base;
        break;
    }

    base = Height - pbRad->ClientHeight + 1;
    int h = msg.lprc->Bottom - msg.lprc->Top + 1 - base;
    if (h < cellsize * 2 - 1)
    {
        h = cellsize * 2 - 1;

        switch (msg.fwSide)
        {
        case (WMSZ_TOPLEFT):
        case (WMSZ_TOPRIGHT):
        case (WMSZ_TOP):
            msg.lprc->Top = msg.lprc->Bottom - h + 1 - base;
            break;
        case (WMSZ_BOTTOMLEFT):
        case (WMSZ_BOTTOMRIGHT):
        case (WMSZ_BOTTOM):
            msg.lprc->Bottom = msg.lprc->Top + h - 1 + base;
            break;
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TfRadicals::pbRadPaint(TObject *Sender)
{
    if (tcSelect->TabIndex == 2)
    {
        Paint2();
        return;
    }

    TCanvas *c = pbRad->Canvas;
    TBrush *b = c->Brush;
    TPen *p = c->Pen;
    TFont *f = c->Font;
    TRect r;
    f->Charset = DEFAULT_CHARSET;

    p->Color = (TColor)0x000b0b0b0;

    int x = 0, y = 0, i = cols * pbRad->VPos, j = 0;

    SetTextAlign(c->Handle, TA_BASELINE | TA_CENTER);

    if (pbRad->VPos > 0)
        for (; j < 17; j++)
            if (i <= (tcSelect->TabIndex ? radcnt[j] : partcnt[j]))
            {
                i -= j + 1;
                b->Color = (!(j % 2) ? (TColor)0x000efefef : clWhite);
                j++;
                break;
            }

    for (; i < (tcSelect->TabIndex ? 214 : 252); i++)
    {
        if (j == 0 || i == (tcSelect->TabIndex ? radcnt[j - 1] : partcnt[j - 1]))
        {
            if (j == 14 && !tcSelect->TabIndex)
                j += 2;
            if (x + cellsize > pbRad->ClientWidth)
            {
                x = 0;
                y += cellsize;
            }
            f->Name = L"Lucida Sans Serif";
            f->Charset = DEFAULT_CHARSET;
            f->Size = double(cellsize) * 0.475 / scaleres;
            f->Style = TFontStyles() << fsBold;
            r = Rect(x, y, x + cellsize, y + cellsize);
            b->Color = (TColor)0x00c0c0c0;
            ExtTextOutW(c->Handle, x + cellsize / 2 + (j < 9 ? 8 : 4), y + cellsize*0.9, ETO_CLIPPED | ETO_OPAQUE, &r, IntToStr(j + 1).c_str(), IntToStr(j + 1).Length(), 0);
            p->Color = clGray;
            c->MoveTo(x - 1, y - 1);
            c->LineTo(x - 1, y + cellsize - 1);
            c->LineTo(x + cellsize - 1, y + cellsize - 1);
            c->LineTo(x + cellsize - 1, y - 1);
            c->LineTo(x - 1, y - 1);
            p->Color = (TColor)0x00b0b0b0;
            j++;
            x += cellsize;
        }
        if (x + cellsize > pbRad->ClientWidth)
        {
            x = 0;
            y += cellsize;
        }
        if (selected[i])
            b->Color = (TColor)0x00c0e0c0;
        else
            b->Color = (!((j - 1) % 2) ? (TColor)0x000efefef : clWhite);
        r = Rect(x, y, x + cellsize, y + cellsize);
        SetRadicalFont(f);
        f->Size = double(cellsize) * 0.57 / scaleres;
        f->Style = TFontStyles();

        HFONT oldf = 0;
        if (settings.kanjiantialias)
        {
            oldf = c->Font->Handle;
            if (!Kfont)
                Kfont = create_localfont(c->Font);
            SelectObject(c->Handle, Kfont);
        }
        ExtTextOutW(c->Handle, x + cellsize / 2, y + cellsize*0.66, ETO_CLIPPED | ETO_OPAQUE, &r, (tcSelect->TabIndex ? &radicals[i] : &parts[i]), 1, 0);
        if (oldf)
            SelectObject(c->Handle, oldf);

        f->Name = L"Tahoma";
        f->Size = 6.0 / scaleres;
        f->Charset = DEFAULT_CHARSET;

        r.top = r.bottom - 8;
        ExtTextOutW(c->Handle, x + cellsize / 2, y + cellsize*0.95, ETO_CLIPPED | ETO_OPAQUE, &r, IntToStr(i + 1).c_str(), IntToStr(i + 1).Length(), 0);

        c->MoveTo(x, y + cellsize - 1);
        c->LineTo(x + cellsize - 1, y + cellsize - 1);
        c->LineTo(x + cellsize - 1, y - 1);
        x += cellsize;
    }
    b->Color = clWhite;
    c->FillRect(Rect(x, y, pbRad->ClientWidth, y + cellsize));
    c->FillRect(Rect(0, y + cellsize, pbRad->ClientWidth, pbRad->ClientHeight));
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::FormResize(TObject *Sender)
{
    pbRad->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::pbRadMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (tcSelect->TabIndex == 2)
    {
        MouseDown2(Sender, Button, Shift, X, Y);
        return;
    }

    int x = 0, y = 0, i = cols * pbRad->VPos, j = 0;
    if (pbRad->VPos > 0)
        for (; j < 17; j++)
            if (i <= (tcSelect->TabIndex ? radcnt[j] : partcnt[j]))
            {
                i -= j + 1;
                j++;
                break;
            }
    //number of skipped radicals counted. now count those that are skipped before the mouse
    for (; i < (tcSelect->TabIndex ? 214 : 252); i++)
    {
        if (j == 0 || i == (tcSelect->TabIndex ? radcnt[j - 1] : partcnt[j - 1]))
        {
            if (x + cellsize > pbRad->ClientWidth)
            {
                x = 0;
                y += cellsize;
            }
            j++;
            x += cellsize;
        }
        if (x + cellsize > pbRad->ClientWidth)
        {
            x = 0;
            y += cellsize;
        }
        if (X >= x && X < x + cellsize && Y >= y && Y < y + cellsize)
        {
            select(i, Shift.Contains(ssShift) || Shift.Contains(ssCtrl));
            FilterKanji();
            break;
        }
        x += cellsize;
    }
}
//---------------------------------------------------------------------------
void TfRadicals::select(int ix, bool multi)
{
    if (!multi)
    {
        memset(selected, 0, sizeof(bool)*252);
        fselcount = 0;
    }

    selected[ix] = !selected[ix];
    fselcount = MAX(0, fselcount + (selected[ix] ? 1 : -1));
    pbRad->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::btnClearClick(TObject *Sender)
{
    if (tcSelect->TabIndex == 2)
    {
        edStrokes->Text = "";
        edkanji->Text = "";
        radlist->Reset();
        if (sel)
            memset(sel, 0, filter->Count*sizeof(bool));
        pbRad->Invalidate();
        pbSelected->Invalidate();
        selcount = 0;
        FilterKanji();
        edKanji->SetFocus();

        return;
    }

    memset(selected, 0, sizeof(bool)*252);
    fselcount = 0;
    pbRad->Invalidate();
    FilterKanji();
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::tcSelectClick(TObject *Sender)
{
    if (tcSelect->TabIndex == 2)
    {
        lbInfo->Caption = "Type name of radicals. Exact matches are selected automatically. Click on a radical to select or unselect. [ENTER] key adds selected radicals to filter.";
        pKanji->Top = 1;
        pKanji->Visible = true;
        if (!filter)
            filter = new TIntegerList;
        pbRad->VMax = pbRad->ClientHeight + 1;
        if (Visible)
            edKanji->SetFocus();
        else
            ActiveControl = edKanji;
        kcallback();
    }
    else if (pKanji->Visible)
    {
        hint8->ReleaseHandle();

        lbInfo->Caption = "Hold the CTRL or SHIFT key while selecting radical, to select more than one.";
        btnOk->Default = true;
        pKanji->Visible = false;
        pbRad->VMax = 0;
        delete filter;
        filter = NULL;
    }

    memset(selected, 0, sizeof(bool)*252);
    fselcount = 0;
    pbRad->VertScrollBar->Enabled = true;
    pbRad->VPos = 0;
    pbRad->Invalidate();
    FilterKanji();
}
//---------------------------------------------------------------------------
void TfRadicals::kcallback()
{
    if (!filter || !edkanji->CanInput())
    {
        if (sel)
            memset(sel, 0, filter->Count*sizeof(bool));
        return;
    }

    delete[] wid;
    wid = NULL;
    delete[] halves;
    halves = NULL;
    delete[] sel;
    sel = NULL;

    btnOk->Default = !edkanji->Text.Length();
    btnAdd->Enabled = !btnOk->Default;
    btnAdd->Default = !btnOk->Default;
    int prev = -1;
    int pos = 0;
    bool added;

    filter->Clear();
    if (edkanji->Text.Length() || numStrokes->value_valid && edStrokes->Text.Length())
    {
        radlist->Filter(edkanji->Text.c_str(), (edStrokes->Text.Length() ? numStrokes->value : 0), filter, btnExact->Down);

        if (btnGroup->Down)
        { //add first one of all radicals of the same kind for grouped radical search
            TIntegerList *l = new TIntegerList;
            try
            {
                for (int i = 0; i < filter->Count && pos < radlist->Count; i++)
                {
                    TKanjiRadical *r = radlist->Items[filter->Items[i]];
                    if (prev == r->radical)
                        continue;

                    added = false;
                    while (pos < radlist->Count && radlist->Items[pos]->radical <= r->radical)
                    {
                        if (radlist->Items[pos]->radical == r->radical && !added)
                        {
                            l->Add(pos);
                            prev = r->radical;
                            added = true;
                        }
                        pos++;
                    }

                }
            }
            catch (...)
            {
            }
            delete filter;
            filter = l;
        }
    }
    else
    {
        for (int i = 0; i < radlist->Count; i++)
        {
            if (btnGroup->Down && filter->Count && radlist->Items[filter->Items[filter->Count - 1]]->radical == radlist->Items[i]->radical)
                continue;
            filter->Add(i);
        }
    }

    MeasureBoxSize2();
    FilterKanji();
    hint8->ReleaseHandle();
}
//---------------------------------------------------------------------------
UnicodeString fixnames(UnicodeString s)
{
    wchar_t *c = new wchar_t[s.Length() + 1];
    wcscpy(c, s.c_str());

    wchar_t *str = wcstok(c, L" ");
    UnicodeString result;
    result = str;
    while ((str = wcstok(NULL, L" ")) != 0)
    {
        int p = result.Pos(str);
        int len = wcslen(str);
        if (!p || (p > 1 && result[p - 1] != L' ') || (p + len - 1 > result.Length() || p + len <= result.Length() && result[p + len] != L','))
            result += UnicodeString(L", ") + str;
    }

    delete[] c;
    return result;
}

void TfRadicals::get_radical_data(int pos, int &rad, UnicodeString &kanji, UnicodeString &names)
{
    int i = filter->Items[pos];

    TKanjiRadical *k = radlist->Items[i];
    rad = k->radical;
    kanji = k->ch;
    names = k->names;

    i++;
    while (btnGroup->Down && i < radlist->Count && radlist->Items[i]->radical == rad)
    {
        k = radlist->Items[i];
        if (!kanji.Pos(k->ch))
            kanji += UnicodeString(L", ") + k->ch;

        names += UnicodeString(L' ') + k->names;
        i++;
    }

    //remove duplicate from names and add commas
    names = fixnames(names);

}

void TfRadicals::Paint2()
{

    TCanvas *c = pbRad->Canvas;
    c->Brush->Color = clWhite;

    if (!linecnt)
    {
        c->FillRect(pbRad->ClientRect);
        return;
    }
    TFont *f = c->Font;

    int pos = 0;
    if (pbRad->VertScrollBar->Position > 0)
        pos = ind[pbRad->VertScrollBar->Position - 1];
    int y = 0;
    int x = 0;
    TSize s;
    TRect r;

    while (pos < filter->Count && y < pbRad->ClientHeight)
    {
        int rad;
        UnicodeString kanji;
        UnicodeString names;
        get_radical_data(pos, rad, kanji, names);

        SetRadicalFont(f);
        f->Size = double(cellsize) * 0.57 / scaleres;
        f->Style = TFontStyles();

        HFONT oldf = 0;
        if (settings.kanjiantialias)
        {
            oldf = c->Font->Handle;
            if (!Kfont)
                Kfont = create_localfont(c->Font);
            SelectObject(c->Handle, Kfont);
        }
        int extra;
        GetTextExtentPoint32W(c->Handle, kanji.c_str(), 1, &s);
        extra = (cellsize - s.cx) / 2; //make it even

        GetTextExtentPoint32W(c->Handle, kanji.c_str(), kanji.Length(), &s);

        r = Rect(x, y, x + s.cx + extra * 2, y + cellsize - 8 * scaleres);

        SetTextAlign(c->Handle, TA_BASELINE | TA_CENTER);

        c->Brush->Color = (!sel[pos] ? ((btnGroup->Down ? radlist->Items[filter->Items[pos]]->strokes : radlist->Items[filter->Items[pos]]->radical) % 2 ? (TColor)0xffffff : (TColor)0xefefef) : (TColor)0x00c0e0c0);

        ExtTextOutW(c->Handle, r.left + r.Width() / 2, r.top + cellsize*0.66, ETO_CLIPPED | ETO_OPAQUE, &r, kanji.c_str(), kanji.Length(), NULL);
        if (oldf)
            SelectObject(c->Handle, oldf);

        f->Size = 6.0 / scaleres;
        f->Name = "Tahoma";
        f->Charset = DEFAULT_CHARSET;
        r.top = r.bottom, r.bottom = y + cellsize;
        ExtTextOutW(c->Handle, r.left + r.Width() / 2, r.bottom - 2*scaleres, ETO_CLIPPED | ETO_OPAQUE, &r, IntToStr(rad).c_str(), IntToStr(rad).Length(), NULL);

        if (!btnNames->Down)
        {
            c->Brush->Color = (!sel[pos] ? COLORDEC(c->Brush->Color, 0x060606) : (TColor)0x00c0e0c0);
            c->Brush->Style = bsSolid;
            c->Font->Color = clBlack;

            SetTextAlign(c->Handle, TA_LEFT | TA_BASELINE);
            f->Name = settings.kanafont;
            f->Charset = settings.kanacharset;
            f->Size = double(cellsize) * 0.32 / scaleres;

            //get half of text
            /*byte half = (names.Length()+1) / 2;
             byte halfa = half;
             byte halfb = half;
             while (halfb <= names.Length() && halfa > 0 && names[halfb] != L' ' && names[halfa] != L' ')
             halfb++, halfa--;
             half = ((halfb < names.Length() && names[halfb] == L' ' || halfa == 0) ? halfb : halfa);
             */
            bool doubleline = halves[pos] > 0; //< names.Length();
            r = Rect(x + s.cx + extra * 2, y, x + wid[pos], y + cellsize / (doubleline ? 2 : 1));
            ExtTextOutW(c->Handle, r.left + extra, r.top + r.Height()*(doubleline ? 0.82 : 0.66), ETO_CLIPPED | ETO_OPAQUE, &r, names.c_str(), doubleline ? halves[pos] : names.Length()/*half*/, NULL);

            if (doubleline)
            {
                r = Rect(x + s.cx + extra * 2, y + cellsize / 2, x + wid[pos], y + cellsize);
                ExtTextOutW(c->Handle, r.left + extra, r.top + r.Height()*0.82, ETO_CLIPPED | ETO_OPAQUE, &r, names.c_str() + halves[pos], names.Length() - halves[pos], NULL);
            }
        }

        c->Pen->Color = (TColor)0x00b0b0b0;

        c->MoveTo(x + wid[pos] - 1, y);
        c->LineTo(x + wid[pos] - 1, y + cellsize - 1);
        c->LineTo(x - 1, y + cellsize - 1);

        x += wid[pos];
        pos++;

        if (pos < filter->Count && x + wid[pos] >= pbRad->ClientWidth)
        {
            c->Brush->Color = clWhite;
            c->FillRect(Rect(x, y, pbRad->ClientWidth, y + cellsize));
            x = 0, y += cellsize;
        }
    }

    if (x)
    {
        c->Brush->Color = clWhite;
        c->FillRect(Rect(x, y, pbRad->ClientWidth, y + cellsize));
        y += cellsize;
    }
    if (y < pbRad->ClientHeight)
        c->FillRect(Rect(0, y, pbRad->ClientWidth, pbRad->ClientHeight));
}
//---------------------------------------------------------------------------
void TfRadicals::InvalidateRad(word pos)
{
    int row = -1;
    for (int i = 0; i < linecnt && row < 0; i++)
        if (ind[i] > pos)
            row = i;

    if (row < 0 && pos < filter->Count)
        row = linecnt - 1;

    if (row < pbRad->VPos || row > pbRad->VPos + (pbRad->ClientHeight / cellsize))
        return;

    TRect r = Rect(0, (row - pbRad->VPos) * cellsize, 0, (row - pbRad->VPos + 1) * cellsize);
    for (int i = (row > 0 ? ind[row - 1] : 0); i < pos; i++)
        r.left += wid[i];
    r.right = r.left + wid[pos];
    InvalidateRect(pbRad->Handle, &r, false);
}
//---------------------------------------------------------------------------
int TfRadicals::mouseonrad2(int x, int y)
{
    int row = pbRad->VPos + y / cellsize;
    if (row >= linecnt)
        return -1;

    int pos = (row ? ind[row - 1] : 0);
    while (pos < filter->Count && x > wid[pos])
        x -= wid[pos++];

    if (pos >= filter->Count || (row != linecnt - 1 && ind[row] <= pos))
        pos = -1;

    return pos;
}
//---------------------------------------------------------------------------
void TfRadicals::MouseDown2(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    int pos = mouseonrad2(x, y);

    if (pos >= 0)
    {
        sel[pos] = !sel[pos];
        if (sel[pos])
            selcount++;
        else
            selcount--;
        InvalidateRad(pos);
        FilterKanji();
    }
}
//---------------------------------------------------------------------------
void TfRadicals::MeasureBoxSize2()
{
    TCanvas *c = pbRad->Canvas;
    TFont *f = c->Font;

    if (filter->Count && !wid)
    {
        wid = new int[filter->Count];
        halves = new int[filter->Count];
        sel = new bool[filter->Count];
        memset(sel, 0, filter->Count*sizeof(bool));
        fselcount = 0;
    }

    delete[] ind;
    ind = NULL;

    int widpos = 0;
    linecnt = 0;

    for (int i = 0; i < filter->Count; i++)
    {
        int rad;
        UnicodeString kanji;
        UnicodeString names;
        get_radical_data(i, rad, kanji, names);

        if (edkanji->Text.Length())
        {
            int p = names.Pos(edkanji->Text);
            if (filter->Count == 1)
            {
                sel[i] = true;
                selcount++;
            }
            else if (p == 1 || p > 1 && names[p - 1] == L' ')
            {

                bool found = false;
                wchar_t *c = new wchar_t[names.Length() + 1];
                try
                {
                    wcscpy(c, names.c_str());

                    wchar_t *str = wcstok(c, L", ");

                    while (!found && str)
                    {
                        found = edkanji->Text == str;
                        str = wcstok(NULL, L", ");
                    }
                }
                catch (...)
                {
                    ;
                }
                delete[] c;

                if (found)
                {
                    sel[i] = true;
                    selcount++;
                }
            }
        }

        SetRadicalFont(f);
        f->Size = double(cellsize) * 0.57 / scaleres;
        f->Style = TFontStyles();

        TSize s;

        HFONT oldf = 0;
        if (settings.kanjiantialias)
        {
            oldf = c->Font->Handle;
            if (!Kfont)
                Kfont = create_localfont(c->Font);
            SelectObject(c->Handle, Kfont);
        }

        int extra;
        GetTextExtentPoint32W(c->Handle, kanji.c_str(), 1, &s);
        extra = (cellsize - s.cx) / 2; //make it even

        GetTextExtentPoint32W(c->Handle, kanji.c_str(), kanji.Length(), &s);
        wid[i] = s.cx + extra * 2;
        if (oldf)
            SelectObject(c->Handle, oldf);

        if (!btnNames->Down)
        {
            f->Name = settings.kanafont;
            f->Charset = settings.kanacharset;
            f->Size = double(cellsize) * 0.32 / scaleres;

            //get half of text
            int half = (names.Length() + 1) / 2;
            int halfa = half;
            int halfb = half;
            while (halfb <= names.Length() && names[halfb] != L' ')
                halfb++;
            while (halfa > 0 && names[halfa] != L' ')
                halfa--;
            half = (halfb <= names.Length() && halfa == 0) ? halfb : (halfa > 0 && halfb > names.Length()) ? halfa : (max(names.Length() - halfa, halfa) < max(halfb, names.Length() - halfb)) ? halfa : halfb;
            bool doubleline = half <= names.Length();

            GetTextExtentPoint32W(c->Handle, names.c_str(), half, &s);
            halfa = s.cx;

            if (doubleline)
            {
                GetTextExtentPoint32W(c->Handle, names.c_str() + half, names.Length() - half, &s);
                halfb = s.cx;
            }
            else
            {
                halfb = 0;
                half = 0;
            }

            wid[i] += MAX(halfa, halfb) + extra * 2;
            halves[i] = half;
        }

        if (widpos + wid[i] >= pbRad->ClientWidth)
        {
            widpos = 0;
            linecnt++;
        }

        widpos += wid[i];
    }
    linecnt++;

    if (!fselcount)
        setselcount(0);

    if (filter->Count)
    {
        ind = new int[linecnt];

        widpos = 0;
        linecnt = 0;
        for (int i = 0; i < filter->Count; i++)
        {
            if ((!i || widpos != wid[i - 1]) && widpos + wid[i] >= pbRad->ClientWidth)
            {
                ind[linecnt] = i;
                widpos = 0;
                linecnt++;
            }
            widpos += wid[i];
        }
        ind[linecnt] = filter->Count;
        linecnt++;
    }

    if (linecnt - (pbRad->ClientHeight / cellsize) <= 0)
    {
        if (pbRad->VertScrollBar->Enabled == false && pbRad->VPos > 0)
            pbRad->VertScrollBar->Enabled = true;
        pbRad->VPos = 0;

        pbRad->VertScrollBar->Enabled = false;
    }
    else
    {
        pbRad->VertScrollBar->Enabled = true;
        pbRad->VPos = 0;
        pbRad->VMax = MAX(1, linecnt - 1);
        pbRad->VertScrollBar->ThumbSize = pbRad->ClientHeight / cellsize;
    }

    pbRad->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::btnGroupClick(TObject *Sender)
{
    kcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::btnNamesClick(TObject *Sender)
{
    MeasureBoxSize2();
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::edKanjiKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == VK_RETURN && fselcount)
    {
        radlist->BeginAdd();
        for (int i = 0; i < filter->Count; i++)
            if (sel[i])
                radlist->AddRad(filter->Items[i], btnGroup->Down);
        radlist->EndAdd();

        edStrokes->Text = "";
        edkanji->Text = "";
        pbSelected->Invalidate();

        FilterKanji();
        selcount = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::pbRadKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key >= 'A' && Key <= 'Z' || Key == VK_BACK)
    {
        edKanji->SetFocus();
        edkanji->pbKeyDown(Sender, Key, Shift);
    }
    if (Key == VK_RETURN || Key == VK_ESCAPE)
        edKanji->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::pbRadKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key >= 'A' && Key <= 'Z' || Key == VK_BACK || Key == VK_RETURN || Key == VK_ESCAPE)
    {
        edKanji->SetFocus();
        edkanji->pbKeyPress(Sender, Key);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::pbSelectedPaint(TObject *Sender)
{
    TCanvas *c = pbSelected->Canvas;
    TRect r = pbSelected->ClientRect;

    c->Pen->Color = clBtnShadow;
    TPoint p[] = { Point(0, 0), Point(r.right - 1, 0), Point(r.right - 1, r.bottom - 1), Point(0, r.bottom - 1), Point(0, 0) };
    c->Polyline(p, 4);
    c->Brush->Color = clWhite;
    InflateRect(&r, -1, -1);
    if (!radlist->GroupCount)
    {
        c->FillRect(r);
        return;
    }

    UnicodeString s;
    for (int i = 0; i < radlist->GroupCount; i++)
    {
        if (s.Length())
            s += L"+";
        s += radlist->GroupString[i];
    }

    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = double(r.Height()) * 0.65 / scaleres;
    SetTextAlign(c->Handle, TA_BASELINE | TA_LEFT);
    ExtTextOutW(c->Handle, 2, 1 + r.Height()*0.88, ETO_CLIPPED | ETO_OPAQUE, &r, s.c_str(), s.Length(), NULL);
}
//---------------------------------------------------------------------------
void TfRadicals::setselcount(int ix)
{
    fselcount = ix;
    btnAdd->Enabled = ix;
    btnAdd->Default = ix;

    btnOk->Default = !edkanji->Text.Length() && !ix;
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::btnAddClick(TObject *Sender)
{
    wchar_t k = VK_RETURN;
    edKanjiKeyPress(edKanji, k);
    edKanji->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::FormShow(TObject *Sender)
{
    //if (tcSelect->TabIndex == 2)
    //fKanji->FilterKanji();
}
//---------------------------------------------------------------------------
void TfRadicals::AddSelected2(TIntegerList *list)
{
    if (tcSelect->TabIndex != 2 || (!sel && filter->Count))
        THROW(L"Nothing to do here! Classic2 mode error!");

    for (int i = 0; i < filter->Count; i++)
        if (sel[i])
        {
            int ix = filter->Items[i];
            list->Add(ix);
            if (btnGroup->Down)
            {
                while (ix + 1 < radlist->Count && radlist->Items[ix]->radical == radlist->Items[ix + 1]->radical)
                    list->Add(++ix);
            }
        }
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::FormHide(TObject *Sender)
{
    if (btnAdd->Enabled && tcSelect->TabIndex == 2)
        btnAddClick(btnAdd);
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::pbRadMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    static TPoint pold = Point(-1, -1);
    static int oldrad = -1;
    static bool repeat = false;

    if (tcSelect->TabIndex != 2 || pold.x == x && pold.y == y)
        return;
    pold = Point(x, y);

    int pos = mouseonrad2(x, y);

    if (pos < 0)
    {
        hint8->ReleaseHandle();
        return;
    }

    int rad;
    UnicodeString kanji;
    UnicodeString names;
    get_radical_data(pos, rad, kanji, names);

    if (rad != oldrad)
        repeat = false;
    oldrad = rad;

    TPoint p = pbRad->ClientToScreen(Point(x, y));

    if (btnNames->Down && !repeat)
    {
        TRect r = hint8->CalcHintRect(120, names, NULL);
        OffsetRect(&r, p.x, p.y + GetSystemMetrics(SM_CYCURSOR)*2 / 3);
        hint8->ActivateHint(r, names);
        repeat = true;
    }
    else
        repeat = false;
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::pbRadMouseLeave(TObject *Sender)
{
    if (tcSelect->TabIndex != 2)
        return;

    hint8->ReleaseHandle();
}
//---------------------------------------------------------------------------
void TfRadicals::FilterKanji()
{
    if (!Visible)
        return;
    fKanji->FilterKanji();
}

void __fastcall TfRadicals::cbStrokesChange(TObject *Sender)
{
    kcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::edStrokesChange(TObject *Sender)
{
    if (numStrokes->value_valid)
        kcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfRadicals::FormPaint(TObject *Sender)
{
    DrawButtonBar(Canvas, ChildRect(pKanji));
    DrawCaptionBar(Canvas, ChildRect(tcSelect), false);
    Canvas->FillRect(Rect(0, b0->Top + b0->Height, ClientWidth, b1->Top));
    DrawCloseBar(this, b1, NULL);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(125);
#endif


