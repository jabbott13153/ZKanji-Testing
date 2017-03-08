//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "wordobj.h"
#include "words.h"
#include "settings.h"
#include "zkformats.h"
#include "collection.h"
#include "searchtree.h"
#include "romajizer.h"
#include "kanjihint.h"
#include "kanji.h"
#include "module.h"
#include "wordgroup.h"
#include "listcontrols.h"
#include "genworddata.h"
#include "wordfilter.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(190);
#endif

//---------------------------------------------------------------------------
extern wchar_t *winflectiontext[];

TWordListObjBase::TWordListObjBase(TZPaintBox *apaintbox, int arowsize, TFuncPtr afunc, TPopupWordProc aselfunc) : inherited(apaintbox, arowsize, true, afunc)
{
    fgetheader = NULL;
    fdrawhead = NULL;
    fdraw = NULL;
    fprepare = NULL;
    fmeaning = NULL;

    coltypes = NULL;
    colalign = NULL;
    colors = NULL;
    colorcount = 0;
    hintline = -1;
    fautoresize = false;
    fcanresize = true;
    fshowhint = true;
    fshowgroup = false;

    fwordselect = true;
    fselecting = wssNone;
    fselfunc = aselfunc;

    lobj = NULL;

    int cw[4] = { arowsize - 1, 100 * scaleres, 120 * scaleres, 6250 * scaleres };
    TWordColumnType ct[4] = { wcPop, wcKanji, wcKana, wcMeaning };
    set_cols(4, cw, NULL, NULL, ct, NULL, 0);
    colresize[0] = false;
    colresize[3] = false;

    selecttype = wlsSingleSelect;
}
//---------------------------------------------------------------------------
TWordListObjBase::~TWordListObjBase()
{
    if (fshowgroup)
        WordGroupEvent(this, NULL);
    RemoveColor(false);
    delete[] coltypes;
    delete[] colalign;

    delete lobj;
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_cols(int acolcnt, int *acolwidths, int *acoldefaultwidths, bool *acolvisible, TWordColumnType *acoltypes, TAlignment *acolalign, int acount)
{
    inherited::set_cols(acolcnt, acolwidths, acoldefaultwidths, acolvisible, acount);

    if (acoltypes)
        memcpy(coltypes, acoltypes, sizeof(TWordColumnType)*colcount);
    if (acolalign)
        memcpy(colalign, acolalign, sizeof(TAlignment)*colcount);
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoSettingsChanged(TSettingsProcState state)
{
    if (state != spsEnd)
        return;

    for (int ix = 0; ix < colcount; ++ix)
    {
        if (colvisible[ix] && (coltypes[ix] == wcKanji || coltypes[ix] == wcKana))
        {
            fbox->Invalidate();
            break;
        }
    }
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoMouseLeave()
{
    HideKanjiHint();
    if (lobj)
        lobj->mouseleave();
    inherited::DoMouseLeave();
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoDrawHead(int col, TRect r)
{
    TCanvas *c = fbox->Canvas;
    if (coltype[col] == wcCustom && (fdrawhead || !fgetheader))
    {
        if (fdrawhead)
            fdrawhead(col, c, r);
        else
            DrawHeadGen(col, r, "", colalign[col]);
        return;
    }

    UnicodeString str;
    switch (coltype[col])
    {
    case wcPop:
        DrawHeadGen(col, r, "", colalign[col]);
        break;
    case wcKanji:
        DrawHeadGen(col, r, "Written", colalign[col]);
        break;
    case wcKana:
        DrawHeadGen(col, r, "Kana", colalign[col]);
        break;
    case wcMeaning:
        DrawHeadGen(col, r, "Meaning", colalign[col]);
        break;
    case wcCustom:
        if (fgetheader)
            DrawHeadGen(col, r, fgetheader(col), colalign[col]);
        else
            DrawHeadGen(col, r, "", colalign[col]);
        break;
    }
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoPrepareDraw(int col, int row)
{
    inherited::DoPrepareDraw(col, row);

    TCanvas *c = fbox->Canvas;

    if (rowcolors[row] != wcUncolored)
    {
        switch (rowcolors[row])
        {
        case wcRed:
            c->Brush->Color = coloravg(c->Brush->Color, (TColor)0xcacaff, 0.5);
            break;
        case wcGreen:
            c->Brush->Color = coloravg(c->Brush->Color, (TColor)0xcaffca, 0.5);
            break;
        case wcBlue:
            c->Brush->Color = coloravg(c->Brush->Color, (TColor)0xffcaca, 0.5);
            break;
        case wcYellow:
            c->Brush->Color = coloravg(c->Brush->Color, (TColor)0xceffff, 0.5);
            break;
        case wcLightBrown:
            c->Brush->Color = coloravg(c->Brush->Color, (TColor)0xe0eaf0, 0.5);
            break;
        }
    }

    TWord *w = words[row];

    if (w && w->data[max(0, meaningindexes[row])].notes & WN_KANAONLY && coltypes[col] == wcKanji)
    {
        //int r, g, b;
        //colordiff((TColor)colorsettings.maintext,(TColor)colorsettings.kanausage,r,g,b);
        c->Font->Color = (TColor)colorsettings.kanausage; //colormod(c->Font->Color,-r,-g,-b);
    }

    if (fprepare)
        fprepare(col, row, coltype[col], fbox->Canvas);
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoDrawCell(int col, int row, TRect r)
{
    TCanvas *c = fbox->Canvas;

    if (lobj)
        lobj->draw(c, col, row, r);

    if (coltype[col] == wcCustom)
    {
        if (fdraw)
        {
            int al = SetTextAlign(c->Handle, TA_TOP | TA_LEFT);
            fdraw(col, row, c, r);
            SetTextAlign(c->Handle, al);
        }
        else
            c->FillRect(r);
        return;
    }

    wchar_t popt[2] = L"a";
    UnicodeString str;
    TSize size;
    int m;
    const TInfList *l;

    TWord *w = words[row];
    TWordIndex wdat;

    int types, prevtypes;
    int notes, prevnotes;
    byte fields, prevfields;
    word nametag, prevnametag;

    TColor textcol = c->Font->Color;

    if (c->Brush->Color != (TColor)colorsettings.background)
        fixtextcolor(textcol, c->Brush->Color);

    c->Font->Color = textcol;

    wchar_t pops[3][3] = { L"q4r", L"qqq", L"JKL" };
    wchar_t* fonts[3] = { L"Webdings", L"Webdings", L"Wingdings" };
    switch (coltype[col])
    {
    case wcPop: // Popularity
        word freq;
        freq = w ? w->frequency : itemfreq(row);

        SetTextAlign(c->Handle, TA_TOP | TA_CENTER);
        c->Font->Name = fonts[settings.freqsymbols];
        c->Font->Height = (int)((float)rowsize * 1.1);
        c->Font->Charset = SYMBOL_CHARSET;
        if (freq > 3500)
        {
            c->Font->Color = (TColor)colorsettings.wordpop;
            popt[0] = pops[settings.freqsymbols][0];
        }
        else
        {
            if (freq > 1500)
            {
                c->Font->Color = (TColor)colorsettings.wordavg;
                popt[0] = pops[settings.freqsymbols][1];
            }
            else
            {
                c->Font->Color = (TColor)colorsettings.wordunpop;
                popt[0] = pops[settings.freqsymbols][2];
            }
        }

        if (rowsize < rowsizes(1))
            m = -1;
        else if (rowsize > rowsizes(1))
            m = -1;
        else
            m = 0;
        GetTextExtentPoint32W(c->Handle, popt, 1, &size);

        ExtTextOutW(c->Handle, (r.left + r.right) / 2 + 1, r.top + m, ETO_OPAQUE | ETO_CLIPPED, &r, popt, 1, 0);
        break;
    case wcKanji: // Kanji and
    case wcKana: // Kana

        c->Font->Name = settings.kanafont;
        c->Font->Charset = settings.kanacharset;
        c->Font->Size = double(rowsize) * 0.66 / scaleres;

        TColor ex, ex2;

        if (w)
        {
            if (coltype[col] == wcKanji)
                str = w->kanji;
            else
                str = w->kana;
        }
        else
        {
            if (coltype[col] == wcKanji)
                str = itemkanji(row);
            else
                str = itemkana(row);
        }

        if (fselecting == wssSelecting && pos == row && ftextselcol == col && ftextsellength > 0)
        {
            ex = c->Brush->Color;
            ex2 = c->Font->Color;
            m = r.Right;

            GetTextExtentPoint32W(c->Handle, str.c_str(), MIN(ftextselstart, ftextselpos), &size);
            r.Right = r.Left + size.cx + 3 * scaleres;
            ExtTextOutW(c->Handle, r.left + 3*scaleres, r.Top + rowsize*0.80, ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str(), MIN(ftextselstart, ftextselpos), 0);
            r.Left = r.Right;

            c->Brush->Color = clHighlight;
            c->Font->Color = clHighlightText;
            GetTextExtentPoint32W(c->Handle, str.c_str() + MIN(ftextselstart, ftextselpos), ftextsellength, &size);
            r.Right = r.Left + size.cx;

            ExtTextOutW(c->Handle, r.left, r.Top + rowsize*0.80, ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str() + MIN(ftextselstart, ftextselpos), ftextsellength, 0);
            r.Left = r.Right;
            r.Right = m;

            c->Brush->Color = ex;
            c->Font->Color = ex2;
            ExtTextOutW(c->Handle, r.Left, r.Top + rowsize*0.80, ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str() + MIN(ftextselstart, ftextselpos) + ftextsellength, str.Length() - ftextsellength - MIN(ftextselstart, ftextselpos), 0);
        }
        else
            ExtTextOutW(c->Handle, r.left + 3*scaleres, r.Top + rowsize*0.80, ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);

        break;
    case wcMeaning: // Meaning
        wdat = items[row];
        l = wdat.inf;

        bool showinf = settings.showinftext && l && (pos == row || settings.showinfalways);
        if (showinf) // Show inflections.
        {
            str = " ";
            int previnf = 0;
            while (l)
            {
                if ((previnf == WI_ERU || previnf == WI_RERU) && l->inf == WI_RARERU)
                    str += winflectiontext[(previnf == WI_ERU ? WI_RERU : WI_ERU) - 1];
                else if (l->inf == WI_RARERU && l->next && (l->next->inf == WI_RERU || l->next->inf == WI_ERU))
                    str += winflectiontext[(l->next->inf == WI_RERU ? WI_ERU : WI_RERU) - 1];
                else
                    str += winflectiontext[l->inf - 1];
                previnf = l->inf;
                if (l->next)
                    str += ", ";
                l = l->next;
            }
            c->Font->Name = settings.romajifont;
            c->Font->Size = double(rowsize) * 0.625 / scaleres;
            c->Font->Style = TFontStyles() << fsBold;
            c->Font->Charset = DEFAULT_CHARSET;
            GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &size);
            ExtTextOutW(c->Handle, r.left + 3*scaleres, r.Top + rowsize*0.76, ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);
            r.left = r.left + size.cx + 3 * scaleres;
            c->Font->Style = TFontStyles();
        }

        if (fmeaning)
        {
            DrawCellText(r, fmeaning(row).Trim(), colalignment[col]);
        }
        else
        {
            // Save font parameters set in the preparatory function to restore later when writing the meaning.
            UnicodeString fntname = c->Font->Name;
            TFontCharset fntchset = c->Font->Charset;
            int fntsize = c->Font->Size;

            // N level
            if (settings.showinJLPT && store)
            {
                TStoredWord *sw;
                if (w)
                    sw = store->FindWord(w->kanji, w->kana);
                else
                    sw = store->FindWord(itemkanji(row).c_str(), itemkana(row).c_str());

                if (sw && sw->jlptnlevel)
                {
                    byte level = sw->jlptnlevel;
                    c->Font->Name = settings.smallromajifont;
                    c->Font->Charset = DEFAULT_CHARSET;
                    c->Font->Size = 7;
                    c->Font->Style = TFontStyles();
                    TColor cold = c->Font->Color;
                    c->Font->Color = (TColor)colorsettings.N[level - 1];

                    str = UnicodeString(L"N") + level;
                    GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &size);
                    r.right = r.left + size.cx + 6 * scaleres;
                    ExtTextOutW(c->Handle, r.left + 3*scaleres, r.Top + rowsize*0.76, ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);
                    r.left = r.right;
                    c->Font->Color = cold;
                }
            }

            m = w ? w->meaningcnt : itemmeaningcount(row);
            for (int j = 0; j < m; j++)
            {
                if (wdat.meaning >= 0 && j != wdat.meaning)
                    continue;

                if (w)
                {
                    types = w->data[j].types;
                    notes = w->data[j].notes;
                    fields = w->data[j].fields;
                    nametag = w->data[j].nametag;
                    if (j > 0)
                    {
                        prevtypes = w->data[j - 1].types;
                        prevnotes = w->data[j - 1].notes;
                        prevfields = w->data[j - 1].fields;
                        prevnametag = w->data[j - 1].nametag;
                    }
                }
                else
                {
                    itemtags(row, j, types, notes, fields, nametag);
                    if (j > 0)
                        itemtags(row, j - 1, prevtypes, prevnotes, prevfields, prevnametag);
                }

                // Is this meaning in a group?
                bool g = false;
                if (fshowgroup && settings.showingroup)
                {
                    TWordGroups *ws = w ? w->group : itemgroups(row);
                    while (ws && ws->meaning != j)
                        ws = ws->next;

                    g = ws != NULL;
                }

                // Meaning number.
                if (wdat.meaning < 0 && m > 1)
                {
                    str = "";
                    c->Font->Name = settings.smallromajifont;
                    c->Font->Size = double(rowsize) * 0.6 / scaleres;
                    c->Font->Charset = DEFAULT_CHARSET;
                    if (j > 0)
                        str = "  ";
                    str = str + IntToStr(j + 1) + ".";
                    GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &size);
                    r.right = r.left + size.cx + 6 * scaleres;
                    ExtTextOutW(c->Handle, r.left + 3*scaleres, r.Top + rowsize*0.76, ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);
                    r.left = r.right;
                }

                // Pre-mentioned.
                c->Font->Name = settings.smallromajifont;
                c->Font->Charset = DEFAULT_CHARSET;
                c->Font->Size = 7;
                c->Font->Style = TFontStyles();

                if (wdat.meaning >= 0 || j == 0 || types != prevtypes || notes != prevnotes)
                {

                    c->Font->Color = (TColor)colorsettings.wordtype;
                    str = "";
                    for (int k = 0; k < WT_COUNT; ++k)
                    {
                        if (types /*w->data[j].types*/ & (1 << k))
                            str = str + wtypetext[k] + " ";
                    }
                    str = str.Trim();
                    if (str.Length() > 0)
                    {
                        GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &size);
                        r.right = r.left + size.cx + 5 * scaleres;
                        ExtTextOutW(c->Handle, r.left + 3*scaleres, r.Top + rowsize*0.76, ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);
                        r.left = r.right;
                    }

                    c->Font->Color = (TColor)colorsettings.wordusage;
                    str = "";
                    for (int k = 0; k < WN_COUNT; ++k)
                    {
                        if (notes & (1 << k))
                            str = str + wnotetext[k] + " ";
                    }
                    for (int k = 0; k < NM_COUNT; ++k)
                    {
                        if (nametag & (1 << k))
                            str = str + ntagtext[k] + " ";
                    }

                    str = str.Trim();
                    if (str.Length() > 0)
                    {
                        GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &size);
                        r.right = r.left + size.cx + 5 * scaleres;
                        ExtTextOutW(c->Handle, r.left + 3*scaleres, r.Top + rowsize*0.76, ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);
                        r.left = r.right;
                    }
                }

                // Show icon when word is in a group.
                if (g)
                {
                    c->FillRect(Rect(r.Left, r.Top, r.Left + 2*scaleres, r.Bottom));
                    r.left += 2 * scaleres;
                    int t = (r.Height() - 12 * scaleres) / 2 + r.Top;
                    c->FillRect(Rect(r.Left, r.Top, r.Left + data->imgSmall->Width, t));
                    data->imgSmall->BkColor = c->Brush->Color;
                    data->imgSmall->Draw(c, r.Left, t, 0, true);
                    c->FillRect(Rect(r.Left, t + data->imgSmall->Height, r.Left + data->imgSmall->Width, r.Bottom));
                    r.left += 12 * scaleres;
                    c->FillRect(Rect(r.Left, r.Top, r.Left + 2*scaleres, r.Bottom));
                    r.left += 2 * scaleres;
                }

                // Meaning.
                TColor cc = c->Brush->Color;

                c->Font->Style = TFontStyles();
                if (w)
                    str = w->data[j].meaning;
                else
                    str = itemmeaning(row, j);
                c->Font->Color = textcol;

                c->Font->Name = fntname;
                c->Font->Charset = fntchset;
                c->Font->Size = fntsize;

                GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &size);

                r.right = r.left + size.cx + 3 * scaleres;

                ExtTextOutW(c->Handle, r.left + 1*scaleres, r.Top + rowsize*0.76, ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);

                r.left = r.right;

                c->Brush->Color = cc;

                // Post-mentioned.
                c->Font->Name = settings.smallromajifont;
                c->Font->Charset = DEFAULT_CHARSET;
                c->Font->Size = 7;
                c->Font->Style = TFontStyles();

                if (fields != 0 /*&& (wdat.meaning >= 0 || j == 0 || fields != prevfields)*/ )
                {
                    c->Font->Color = (TColor)colorsettings.wordfield;
                    str = "";
                    for (int k = 0; k < WF_COUNT; ++k)
                    {
                        if (fields & (1 << k))
                            str = str + wfieldtext[k] + " ";
                    }
                    str = str.Trim();
                    if (str.Length() > 0)
                    {
                        GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &size);
                        r.right = r.left + size.cx + 3 * scaleres;
                        ExtTextOutW(c->Handle, r.left + 3*scaleres, r.Top + rowsize*0.76, ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);
                        r.left = r.right;
                    }
                }
                r.left = r.right;

                if (wdat.meaning >= 0 || m - 1 == j)
                {
                    r.right = fbox->ClientWidth;
                    c->FillRect(r);
                }
            }
        }
        break;
    }
}
//---------------------------------------------------------------------------
int TWordListObjBase::MeasureCellText(UnicodeString str)
{
    TCanvas *c = Canvas;

    TSize s;
    GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);

    return s.cx;
}
//---------------------------------------------------------------------------
void TWordListObjBase::DrawCellText(TRect r, UnicodeString str, TAlignment alignment)
{
    TCanvas *c = Canvas;
    unsigned int alold = SetTextAlign(c->Handle, TA_BASELINE | (alignment == taLeftJustify ? TA_LEFT : alignment == taRightJustify ? TA_RIGHT : TA_CENTER));
    int x;
    switch (alignment)
    {
    case taLeftJustify:
        x = r.Left + 3 * scaleres;
        break;
    case taRightJustify:
        x = r.Left + r.Width() - 4 * scaleres;
        break;
    default: // taCenter
        x = r.Left + r.Width() / 2;
    }

    ExtTextOutW(c->Handle, x, r.Top + rowsize*(c->Font->Charset == DEFAULT_CHARSET ? 0.76 : 0.8), ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);
    SetTextAlign(c->Handle, alold);
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoPaint()
{
    if (fautoresize && fcanresize && count)
    {
        TCanvas *c = fbox->Canvas;

        int col1 = 60;
        int col2 = 60;

        // Resize columns before painting.
        c->Font->Name = settings.kanafont;
        c->Font->Charset = settings.kanacharset;
        c->Font->Size = double(rowsize) * 0.66 / scaleres;
        int y = header ? headersize : 0;
        int i = fbox->VPos;
        TWord *w;
        TSize size;
        while (y < fbox->ClientHeight && i < count)
        {
            w = words[i++];
            UnicodeString s;
            if (w)
                GetTextExtentPoint32W(c->Handle, w->kanji, wcslen(w->kanji), &size);
            else
            {
                s = itemkanji(i);
                GetTextExtentPoint32W(c->Handle, s.c_str(), s.Length(), &size);
            }
            col1 = MAX(col1, (int)size.cx + 10);
            if (w)
                GetTextExtentPoint32W(c->Handle, w->kana, wcslen(w->kana), &size);
            else
            {
                s = itemkana(i);
                GetTextExtentPoint32W(c->Handle, s.c_str(), s.Length(), &size);
            }
            col2 = MAX(col2, (int)size.cx + 10);
            y += rowsize;
        }

        for (int col = 0; col < colcount; col++)
            if (coltype[col] == wcKanji)
                colsize[col] = col1;
            else if (coltype[col] == wcKana)
                colsize[col] = col2;
        fcanresize = false;
    }
    inherited::DoPaint();
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_getheaderproc(TWordListGetHeaderProc agetheader)
{
    fgetheader = agetheader;
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_drawheadproc(TWordListDrawHeaderProc adrawhead)
{
    fdrawhead = adrawhead;
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_meaningproc(TWordListMeaningProc ameaning)
{
    fmeaning = ameaning;
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_prepareproc(TWordListPrepareProc aprepare)
{
    fprepare = aprepare;
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_drawproc(TWordListDrawCellProc adraw)
{
    fdraw = adraw;
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_pos2(int val, bool resetsel)
{
    HideKanjiHint(false);

    inherited::set_pos2(val, resetsel);
}
//---------------------------------------------------------------------------
wchar_t TWordListObjBase::KanjiAtPos(int x, int y)
{
    int oncol = on_column(x);
    if (!count || y < (header ? headersize : 0) || oncol < 0 || coltype[oncol] != wcKanji)
        return 0;

    y = (y - (header ? headersize : 0)) / rowsize;
    if (y + fbox->VPos >= count)
        return 0;

    TRect r = cellbox(oncol, y);
    x -= r.left + 3 * scaleres;

    TCanvas *c = fbox->Canvas;

    TSize s;

    TWord *w = words[y + fbox->VPos];
    if (!w)
        return 0;

    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = double(rowsize) * 0.66 / scaleres;

    int n = wcslen(w->kanji);
    for (int i = n; i > 0; i--)
    {
        GetTextExtentPoint32W(c->Handle, w->kanji, i, &s);
        if (x > s.cx)
        {
            if (i == n || !KANJI(w->kanji[i]))
                return 0;
            return w->kanji[i];
        }
    }
    if (!KANJI(w->kanji[0]))
        return 0;
    return w->kanji[0];
}
//---------------------------------------------------------------------------
int TWordListObjBase::IndexAtPos(int y)
{
    if (!count || y < (header ? headersize : 0) || (y - (header ? headersize : 0)) / rowsize + fbox->VPos >= count)
        return -1;
    return indexes[(y - (header ? headersize : 0)) / rowsize + fbox->VPos];
}
//---------------------------------------------------------------------------
TWord* TWordListObjBase::WordAtPos(int y, int &index, char &meaning)
{
    index = IndexAtPos(y);
    meaning = -1;
    if (index < 0)
        return NULL;

    y = (y - (header ? headersize : 0)) / rowsize + fbox->VPos;

    meaning = meaningindexes[y]; /*items[y].meaning;*/
    return words[y];
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoResize()
{
    fcanresize = true;
    inherited::DoResize();
    if (fautoresize)
        Refresh();
    fcanresize = false;
}
//---------------------------------------------------------------------------
void TWordListObjBase::cellshift(int acol, int arow, int &x, int &y)
{
    if (!lobj)
        return;

    lobj->updatecoordinates(acol, arow, x, y);
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoMouseDown(int acol, int arow, TMouseButton Button, TShiftState Shift, int ax, int ay)
{
    HideKanjiHint(false);

    int x = ax;
    int y = ay;
    cellshift(acol, arow, x, y);

    if (!lobj || !lobj->mouseincontrol())
        inherited::DoMouseDown(acol, arow, Button, Shift, x, y);

    if (lobj)
        lobj->mousedown(acol, arow, Button, Shift, ax, ay);

    if (x < 0 || y < 0)
        return;

    if (fbox->Cursor != crHSplit && ay >= (header ? headersize : 0) && ay < (count - fbox->VPos) * rowsize + (header ? headersize : 0) && (selecttype == wlsSingleSelect || selecttype == wlsNoSelection || !Shift.Contains(ssShift)))
    {
        if (Button != mbRight)
        {
            ftextselcol = acol;
            if (Button == mbLeft && fwordselect && (coltype[ftextselcol] == wcKanji || coltype[ftextselcol] == wcKana))
            {
                ftextsellength = 0;
                ftextselpos = ftextselstart = word_position(ftextselcol, arow, x, false);
                if (ftextselpos >= 0)
                {
                    fselecting = wssStarted;
                    fselstartingpos = Point(x, y);
                    return;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoMouseMove(int acol, int arow, TShiftState Shift, int ax, int ay)
{
    int x = ax;
    int y = ay;

    if (fselecting != wssSelecting)
        inherited::DoMouseMove(acol, arow, Shift, x, y);

    cellshift(acol, arow, x, y);

    if (fselecting == wssStarted && (abs(fselstartingpos.x - x) > 3 || abs(fselstartingpos.y - y) > 3))
        fselecting = wssSelecting;

    if (fselecting == wssSelecting)
    {
        int a = word_position(ftextselcol, pos, x, true);
        if (a != ftextselpos)
        {
            ftextselpos = a;
            ftextsellength = abs(ftextselstart - ftextselpos);
            TRect r = cellbox(ftextselcol, pos);
            InvalidateRect(fbox->Handle, &r, false);
        }
    }
    else if (!Shift.Contains(ssLeft) && !Shift.Contains(ssRight) && (!lobj || !lobj->mouseincontrol()))
    {

        if (settings.show_kanji_hint)
        {
            wchar_t w = KanjiAtPos(x, y);
            if (hintline != fbox->VPos + (ay - (header ? headersize : 0)) / rowsize)
            {
                hintline = fbox->VPos + (ay - (header ? headersize : 0)) / rowsize;
                HideKanjiHint();
            }
            if (fshowhint)
            {
                if (w && settings.show_kanji_hint)
                    ShowKanjiHint(fbox, ax, ay, w, collection);
                else
                    HideKanjiHint();
            }
        }

        if (mouserow >= 0 && mouserow < count && mousecol >= 0 && mousecol < colcount && coltype[mousecol] == wcPop)
        {
            TWord *w = words[mouserow];

            UnicodeString str = (UnicodeString)L"Word frequency: " + (w ? w->frequency : itemfreq(mouserow));
            fbox->Hint = str;
            if (lastFreqhintpos != Point(ax, (ay - (header ? headersize : 0)) + fbox->VPos * rowsize))
            {
                lastFreqhintpos = Point(ax, (ay - (header ? headersize : 0)) + fbox->VPos * rowsize);
                Application->CancelHint();
                TMessage msg;
                msg.Msg = WM_MOUSEMOVE;
                msg.WParam = 0;
                short sx = ax;
                short sy = ay;
                msg.LParamLo = *reinterpret_cast<unsigned short*>(&sx);
                msg.LParamHi = *reinterpret_cast<unsigned short*>(&sy);
                Application->HintMouseMessage(fbox, msg);
            }
        }
        else if (lastFreqhintpos != Point(ax, (ay - (header ? headersize : 0)) + fbox->VPos * rowsize))
        {
            lastFreqhintpos = Point(ax, (ay - (header ? headersize : 0)) + fbox->VPos * rowsize);
            fbox->Hint = "";
            Application->CancelHint();
        }
    }

    if (lobj)
        lobj->mousemove(acol, arow, Shift, ax, ay);

}
//---------------------------------------------------------------------------
void TWordListObjBase::DoMouseUp(int acol, int arow, TMouseButton Button, TShiftState Shift, int ax, int ay)
{
    int x = ax;
    int y = ay;
    cellshift(acol, arow, x, y);

    if (fselfunc && (Button == mbRight && ay >= (header ? headersize : 0) && (ay - (header ? headersize : 0)) / rowsize + fbox->VPos < count || Button == mbLeft && selstring.Length()))
    {
        TPoint p = fbox->ClientToScreen(Point(ax, ay));
        TKanji *k = NULL;
        wchar_t kc = 0;
        if (!selecting && !multipleselected && pos >= 0)
            kc = KanjiAtPos(x, y);
        else if (selecting && selstring.Length() == 1 && KANJI(selstring[1]))
            kc = selstring[1];
        if (kc)
            k = kanjis->Kanjis[kanjis->IndexOf(kc)];
        fselfunc(p.x, p.y, this, coltype[selecting ? selcolumn : on_column(ax)], words[pos], k);
    }

    if (fselecting == wssSelecting)
    {
        ftextsellength = 0;
        invalidaterow(pos);
    }
    fselecting = wssNone;

    if (lobj)
        lobj->mouseup(acol, arow, Button, Shift, ax, ay);

    inherited::DoMouseUp(acol, arow, Button, Shift, ax, ay);
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoVScroll(TScrollCode ScrollCode, int &ScrollPos)
{
    HideKanjiHint(false);

    inherited::DoVScroll(ScrollCode, ScrollPos);

    if (fautoresize)
    {
        Refresh();
    }
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoHScroll(TScrollCode ScrollCode, int &ScrollPos)
{
    HideKanjiHint(false);

    inherited::DoHScroll(ScrollCode, ScrollPos);
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoSetColumnCount(const byte &acolcount, const byte &oldcolcount)
{
    TWordColumnType *tmp = coltypes;
    coltypes = new TWordColumnType[acolcount];

    int i = 0;
    if (tmp)
    {
        i = MIN(acolcount, oldcolcount);
        memcpy(coltypes, tmp, sizeof(TWordColumnType)*i);
        delete[] tmp;
    }
    for (; i < acolcount; i++)
        coltypes[i] = wcCustom;

    TAlignment *atmp = colalign;
    colalign = new TAlignment[acolcount];
    i = 0;
    if (atmp)
    {
        i = MIN(acolcount, oldcolcount);
        memcpy(colalign, atmp, sizeof(TAlignment)*i);
        delete[] atmp;
    }
    for (; i < acolcount; i++)
        colalign[i] = taLeftJustify;

    inherited::DoSetColumnCount(acolcount, oldcolcount);
}
//---------------------------------------------------------------------------
TWordColumnType TWordListObjBase::get_coltype(byte col)
{
    if (col >= colcount)
        THROW(L"Column index out of range!");

    return coltypes[col];
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_coltype(byte col, TWordColumnType val)
{
    if (col >= colcount)
        THROW(L"Column index out of range!");

    if (coltypes[col] == val)
        return;
    if (val == wcPop || val == wcMeaning)
        colresize[col] = false;
    else if (coltypes[col] == wcPop || coltypes[col] == wcMeaning)
        colresize[col] = true;

    coltypes[col] = val;
}
//---------------------------------------------------------------------------
TAlignment TWordListObjBase::get_colalign(byte col)
{
    if (col >= colcount)
        THROW(L"Column index out of range!");

    return colalign[col];
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_colalign(byte col, TAlignment val)
{
    if (col >= colcount)
        THROW(L"Column index out of range!");

    if (colalign[col] == val)
        return;
    colalign[col] = val;
    invalidatecol(col);
}
//---------------------------------------------------------------------------
UnicodeString TWordListObjBase::get_wnotes()
{
    if (pos < 0)
        return "";

    UnicodeString s = " ";

    bool multinotes = false;

    TWord *w = words[pos];
    byte m = w ? w->meaningcnt : itemmeaningcount(pos);

    if (!m)
        return UnicodeString();

    int types, prevtypes;
    int notes, prevnotes;
    byte fields, prevfields;
    word nametag, prevnametag;

    for (int j = 0; j < m; j++)
    {

        if (w)
        {
            types = w->data[j].types;
            notes = w->data[j].notes;
            fields = w->data[j].fields;
            nametag = w->data[j].nametag;
            if (j > 0)
            {
                prevtypes = w->data[j - 1].types;
                prevnotes = w->data[j - 1].notes;
                prevfields = w->data[j - 1].fields;
                prevnametag = w->data[j - 1].nametag;
            }
        }
        else
        {
            itemtags(pos, j, types, notes, fields, nametag);
            if (j > 0)
                itemtags(pos, j - 1, prevtypes, prevnotes, prevfields, prevnametag);
        }
        if (!j || types != prevtypes || notes != prevnotes || fields != prevfields)
        {
            if (j > 0)
            {
                s = s + IntToStr(j + 1) + ". ";
                multinotes = true;
            }

            for (int k = 0; k < WT_COUNT; ++k)
            {
                if (types & (1 << k))
                    s = s + wlongtypetext[k] + ", ";
            }

            for (int k = 0; k < WN_COUNT; ++k)
            {
                if (notes & (1 << k))
                    s = s + wlongnotetext[k] + ", ";
            }

            for (int k = 0; k < WF_COUNT; ++k)
            {
                if (fields & (1 << k))
                    s = s + wlongfieldtext[k] + ", ";
            }

            for (int k = 0; k < NM_COUNT; ++k)
            {
                if (nametag & (1 << k))
                    s = s + nlongtagtext[k] + ", ";
            }
        }
    }
    if (multinotes)
        s = " 1." + s;
    s = s.TrimRight();
    if (s.Length() && s[s.Length()] == ',')
        s = s.SubString(1, s.Length() - 1);

    if (settings.showinJLPT && store/*JLPT*/)
    {
        //TWordJLPTNData *dat = JLPT->FindData(words[pos]);
        TStoredWord *sw = store->FindWord(w ? w->kanji : itemkanji(pos).c_str(), w ? w->kana : itemkana(pos).c_str());
        if (sw && sw->jlptnlevel)
            s = UnicodeString(L"(N") +int(sw->jlptnlevel) + L") " + s;
    }

    return s;
}
//---------------------------------------------------------------------------
word TWordListObjBase::itemfreq(unsigned int i)
{
    TWord *w = words[i];
    if (w)
        return w->frequency;
    return 0;
}
//---------------------------------------------------------------------------
TWordGroups* TWordListObjBase::itemgroups(unsigned int i)
{
    TWord *w = words[i];
    if (w)
        return w->group;
    return NULL;
}
//---------------------------------------------------------------------------
UnicodeString TWordListObjBase::itemmeaning(unsigned int i, byte m)
{
    TWord *w = words[i];
    if (w)
        return w->data[m].meaning;
    return UnicodeString();
}
//---------------------------------------------------------------------------
byte TWordListObjBase::itemmeaningcount(unsigned int i)
{
    TWord *w = words[i];
    if (w)
        return w->meaningcnt;
    return 0;
}
//---------------------------------------------------------------------------
UnicodeString TWordListObjBase::itemkanji(unsigned int i)
{
    TWord *w = words[i];
    if (w)
        return w->kanji;
    return UnicodeString();
}
//---------------------------------------------------------------------------
UnicodeString TWordListObjBase::itemkana(unsigned int i)
{
    TWord *w = words[i];
    if (w)
        return w->kana;
    return UnicodeString();
}
//---------------------------------------------------------------------------
void TWordListObjBase::itemtags(unsigned int i, byte m, int &types, int &notes, byte &fields, word &nametag)
{
    TWord *w = words[i];
    if (w)
    {
        types = w->data[m].types;
        notes = w->data[m].notes;
        fields = w->data[m].fields;
        nametag = w->data[m].nametag;
        return;
    }
    types = 0;
    notes = 0;
    fields = 0;
    nametag = 0;
}
//---------------------------------------------------------------------------
//return the index of the current word from the words list, or -1 on error
int TWordListObjBase::get_current()
{
    if (!count)
        return -1;
    return indexes[pos];
}
//---------------------------------------------------------------------------
int TWordListObjBase::get_currentmeaning()
{
    if (!count)
        return -1;
    return meaningindexes[pos]; /*items[pos].meaning;*/
}
//---------------------------------------------------------------------------
TWord* TWordListObjBase::get_currentword()
{
    if (!count)
        return NULL;
    return words[pos];
}
//---------------------------------------------------------------------------
int TWordListObjBase::word_position(int col, int row, int x, bool out_of_range)
{
    if (pos < 0 || pos >= count || col < 0 || col >= colcount || coltype[col] != wcKanji && coltype[col] != wcKana)
        THROW(L"error");

    x -= 3 + columnleft(col) - fbox->HPos;

    TWord *w = words[row];
    UnicodeString s = (coltype[col] == wcKanji ? (w ? UnicodeString(w->kanji) : itemkanji(row)) : (w ? UnicodeString(w->kana) : itemkana(row)));
    TCanvas *c = fbox->Canvas;

    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = double(rowsize) * 0.66 / scaleres;
    c->Font->Style = TFontStyles();

    int last = 0;
    TSize p;
    for (int i = s.Length(); i > 0; i--)
    {
        GetTextExtentPoint32W(c->Handle, s.c_str(), i, &p);
        if (i == s.Length() && x > p.cx)
        {
            return (x - p.cx < 5 || out_of_range ? i : -1);
        }
        if (i > 0 && i < s.Length() && x > p.cx + (last - p.cx)*0.6)
            return i + 1;
        last = p.cx;
    }
    if (x > last * 0.6)
        return 1;
    return 0;
}
//---------------------------------------------------------------------------
UnicodeString TWordListObjBase::get_selstring()
{
    if (!ftextsellength || fselecting != wssSelecting)
        return L"";

    TWord *w = words[pos];
    if (w)
        return UnicodeString((coltype[ftextselcol] == wcKanji ? w->kanji : w->kana) + MIN(ftextselstart, ftextselpos), ftextsellength);
    else
        return (coltype[ftextselcol] == wcKanji ? itemkanji(pos) : itemkana(pos)).SubString(MIN(ftextselstart, ftextselpos), ftextsellength);
}
//---------------------------------------------------------------------------
bool TWordListObjBase::get_selecting()
{
    return (fselecting == wssSelecting && ftextsellength);
}
//---------------------------------------------------------------------------
int TWordListObjBase::get_selcolumn()
{
    if (!selecting)
        return -1;
    return ftextselcol;
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_autoresize(bool val)
{
    fautoresize = val;
    if (fautoresize)
        Refresh();
    else
        ResetDefaults();
}
//---------------------------------------------------------------------------
void TWordListObjBase::wordgroup(void *caller, TWordCollection *coll, TWord *w)
{
    DoWordGroup(coll, w);
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_showgroup(bool val)
{
    if (fshowgroup == val)
        return;
    fshowgroup = val;
    if (val)
        WordGroupEvent(this, wordgroup);
    else
        WordGroupEvent(this, NULL);

    Refresh();
}
//---------------------------------------------------------------------------
void TWordListObjBase::ResetDefaults()
{
    defsize[0] = rowsize;
    defsize[1] = 100;
    defsize[2] = 120;
    defsize[3] = 6250;
}
//---------------------------------------------------------------------------
void TWordListObjBase::Refresh()
{
    if (fautoresize)
    {
        fcanresize = true;
        fbox->Refresh();
        //fcanresize = false;
    }
    else
        fbox->Invalidate();
}
//---------------------------------------------------------------------------
int TWordListObjBase::get_index(unsigned int i)
{
    return items[i].index;
}
//---------------------------------------------------------------------------
char TWordListObjBase::get_meaningindex(unsigned int i)
{
    return items[i].meaning;
}
//---------------------------------------------------------------------------
TWord* TWordListObjBase::get_word(unsigned int i)
{
    if (i >= (unsigned int)count)
        return NULL;
    int ix = indexes[i];
    if (ix < 0 || ix >= collection->words->Count)
        return NULL;

    return collection->words->Items[ix];
}
//---------------------------------------------------------------------------
TWordColor TWordListObjBase::get_color(unsigned int ix)
{
    if (!colorcount)
        return wcUncolored;
    if ((int)ix >= count)
        THROW(L"Index out of bounds when accessing word color!");

    return colors[ix];
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_color(unsigned int ix, TWordColor val)
{
    if ((int)ix >= count)
        THROW(L"Index out of bounds when accessing word color!");
    if (!colorcount && val == wcUncolored || colorcount && val == colors[ix])
        return;

    if (val != wcUncolored && (!colorcount || colors[ix] == wcUncolored))
        colorcount++;
    if (val == wcUncolored)
        colorcount--;
    if (!colorcount)
    {
        delete[] colors;
        colors = NULL;
        invalidaterow(ix);
        return;
    }

    if (colorcount == 1 && val != wcUncolored)
    {
        colors = new TWordColor[count];
        memset(colors, 0, sizeof(TWordColor)*count);
    }
    colors[ix] = val;
    invalidaterow(ix);
}
//---------------------------------------------------------------------------
void TWordListObjBase::RemoveColor(bool invalidate)
{
    if (!colorcount)
        return;
    colorcount = NULL;
    delete[] colors;
    colors = NULL;
    if (invalidate)
        fbox->Invalidate();
}
//---------------------------------------------------------------------------
int TWordListObjBase::get_rowsize()
{
    return inherited::rowsize;
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_rowsize(int val)
{
    if (val == rowsize)
        return;

    fcanresize = true;

    for (int i = 0; i < colcount; i++)
    {
        if (coltype[i] != wcPop)
            continue;
        colsize[i] = val - 1;
    }
    inherited::rowsize = val;
}
//---------------------------------------------------------------------------
UnicodeString TWordListObjBase::get_meaningtext(int ix)
{
    UnicodeString s = "";
    TWord *w = words[ix];

    char mix = meaningindexes[ix];
    if (mix < 0)
    {
        byte m;
        if (w)
            m = w->meaningcnt;
        else
            m = itemmeaningcount(ix);
        for (int i = 0; i < m; i++)
        {
            if (i)
                s += " ";
            if (w->meaningcnt > 1)
                s += IntToStr(i + 1) + ") ";
            if (w)
                s += w->data[i].meaning;
            else
                s += itemmeaning(ix, i);
        }
    }
    else
    {
        if (w)
            s = w->data[mix].meaning;
        else
            s = itemmeaning(ix, mix);
    }
    return s;
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoDeactivate(bool focused)
{
    fselecting = wssNone;

    if (lobj)
        lobj->deactivated();

    inherited::DoDeactivate(focused);
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoWordGroup(TWordCollection *coll, TWord *w)
{
    int rows = (fbox->ClientHeight - (header ? headersize : 0) + rowsize - 1) / rowsize;
    int p = fbox->VPos;
    while (rows && p < count)
    {
        if (words[p] == w)
            invalidaterow(p);
        p++;
        rows--;
    }
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoChanged()
{
    if (lastcount == count)
    {
        inherited::DoChanged();
        return;
    }

    if (lobj)
        lobj->changed(count, lastcount);

    lastcount = count;

    inherited::DoChanged();
}
//---------------------------------------------------------------------------
void TWordListObjBase::DoColResizing(bool newval)
{
    inherited::DoColResizing(newval);

    if (newval)
        for (int ix = 0; ix < colcount; ++ix)
            if (coltype[ix] == wcPop || coltype[ix] == wcMeaning)
                colresize[ix] = false;
}
//---------------------------------------------------------------------------
TListCustomControlChanged TWordListObjBase::get_checkproc()
{
    if (!lobj)
        return NULL;
    return lobj->fcallback;
}
//---------------------------------------------------------------------------
TListVirtualCheckControlGetter TWordListObjBase::get_virtualcheckproc()
{
    if (!lobj)
        return NULL;
    return lobj->gcallback;
}
//---------------------------------------------------------------------------
void TWordListObjBase::SetCheckboxHandler(TListCustomControlChanged fcallback)
{
    delete lobj;
    lobj = NULL;

    if (fcallback)
        lobj = new TListCheckboxObj(this, fcallback);

}
//---------------------------------------------------------------------------
void TWordListObjBase::SetCheckboxHandler(TListCustomControlChanged fcallback, TListVirtualCheckControlGetter fgcallback)
{
    delete lobj;
    lobj = NULL;

    if (fcallback)
    {
        if (fgcallback)
            lobj = new TListCheckboxObj(this, fcallback, fgcallback);
        else
            lobj = new TListCheckboxObj(this, fcallback);
    }
}
//---------------------------------------------------------------------------
bool TWordListObjBase::get_checkcolumn(byte col)
{
    if (!lobj)
        THROW(L"Cannot get checkbox properties without specifying a check process first");

    return lobj->checkcolumn[col];
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_checkcolumn(byte col, bool val)
{
    if (!lobj)
        THROW(L"Cannot set checkbox properties without specifying a check process first");
    lobj->checkcolumn[col] = val;
}
//---------------------------------------------------------------------------
bool TWordListObjBase::get_checked(int row, byte col)
{
    if (!lobj)
        THROW(L"Cannot get checkbox properties without specifying a check process first");
    return lobj->checked[row][col];
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_checked(int row, byte col, bool val)
{
    if (!lobj)
        THROW(L"Cannot set checkbox properties without specifying a check process first");
    lobj->checked[row][col] = val;
}
//---------------------------------------------------------------------------
bool TWordListObjBase::get_checkenabled(int row, byte col)
{
    if (!lobj)
        THROW(L"Cannot get checkbox properties without specifying a check process first");
    return lobj->enabled[row][col];
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_checkenabled(int row, byte col, bool val)
{
    if (!lobj)
        THROW(L"Cannot set checkbox properties without specifying a check process first");
    lobj->enabled[row][col] = val;
}
//---------------------------------------------------------------------------
bool TWordListObjBase::get_multicheck()
{
    if (!lobj)
        THROW(L"Cannot get checkbox properties without specifying a check process first");
    return lobj->multicheckcolums;
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_multicheck(bool val)
{
    if (!lobj)
        THROW(L"Cannot set checkbox properties without specifying a check process first");
    lobj->multicheckcolums = val;
}
//---------------------------------------------------------------------------
bool TWordListObjBase::get_multichecksel()
{
    if (!lobj)
        THROW(L"Cannot get checkbox properties without specifying a check process first");
    return lobj->multicheckselected;
}
//---------------------------------------------------------------------------
void TWordListObjBase::set_multichecksel(bool val)
{
    if (!lobj)
        THROW(L"Cannot set checkbox properties without specifying a check process first");
    lobj->multicheckselected = val;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordListObj::TWordListObj(TZPaintBox *apaintbox, int arowsize, TFuncPtr afunc, TPopupWordProc aselfunc) : ffiltered(false), fcontinuous(false), TWordListObjBase(apaintbox, arowsize, afunc, aselfunc)
{
    flist = new TWordIndexList((TWordCollection*)NULL);
    fcoll = NULL;
}
//---------------------------------------------------------------------------
TWordListObj::~TWordListObj()
{
    delete flist;
}
//---------------------------------------------------------------------------
TWordIndex TWordListObj::get_item(unsigned int i)
{
    if (!fcontinuous)
    {
        if (!flist)
            THROW(L"Error! No list for wordlist item!");
        return *flist->Items[i];
    }

    TWordIndex index;
    index.index = dictionary->WordIndex(fcollection, i, ffiltered ? woFiltered : woSelect); //(int)dictionary->alphabet[fcollection]->Items[i];
    index.inf = NULL;
    index.meaning = -1;
    return index;
}
//---------------------------------------------------------------------------
TWordCollection* TWordListObj::get_collection()
{
    if (!fcontinuous)
        return flist->collection;
    return fcoll;
}
//---------------------------------------------------------------------------
void TWordListObj::update_count(int &count)
{
    if (!fcontinuous)
        count = flist ? flist->Count : 0;
    else
        count = dictionary->ListCount(fcollection, ffiltered ? woFiltered : woSelect); //dictionary->alphabet[fcollection]->Count;
}
//---------------------------------------------------------------------------
void TWordListObj::set_by_indexes(TWordIndexList *indexes, bool holdposition)
{
    StartUpdating signal(this);

    RemoveColor(false);

    fcontinuous = false;
    fcoll = NULL;

    if (!indexes || !indexes->Count)
    {
        count = 0;
        pos = -1;
        fbox->HPos = 0;
        flist->Clear();
        Refresh();
        DoCallback();
        return;
    }

    TWord *w = NULL;
    int vdiff = 0;
    if (holdposition && pos >= 0 && pos < flist->Count && indexes && indexes->Count && flist->Indexes[pos] < flist->collection->words->Count)
    {
        w = flist->Words[pos];
        vdiff = pos - fbox->VPos;

        int rowcnt = fbox->ClientHeight / rowsize - (header ? 1 : 0);
        if (rowcnt > 1 && rowcnt <= 5)
            vdiff = min(max(vdiff, 1), rowcnt - 2);
        else
            vdiff = min(max(vdiff, 2), rowcnt - 2);
    }

    flist->Assign(indexes);
    count = flist->Count;
    fbox->HPos = 0;

    if (w)
    {
        int ix = 0;
        for (; ix < flist->Count; ix++)
        {
            TWord *tw = flist->Words[ix];
            if (!wcscmp(tw->kanji, w->kanji) && !wcscmp(tw->kana, w->kana))
            {
                pos = ix;

                if (vdiff >= 0 && vdiff < (fbox->ClientHeight - (header ? headersize : 0) - rowsize) / rowsize + 1)
                    fbox->VPos = pos - vdiff;
                break;
            }
        }

        if (ix == flist->Count)
            w = NULL;
    }

    if (!w)
        pos = 0;

    Refresh();
    DoCallback();
}
//---------------------------------------------------------------------------
void TWordListObj::set_cols(TWordIndexList *indexes, int acolcnt, int *acolwidths, int *acoldefaultwidths, bool *acolvisible, TWordColumnType *acoltypes, TAlignment *acolalign)
{
    RemoveColor(false);

    fcontinuous = false;
    fcoll = NULL;

    if (!indexes || !indexes->Count)
    {
        set_cols(acolcnt, acolwidths, acoldefaultwidths, acolvisible, acoltypes, acolalign, 0);
        count = 0;
        pos = -1;
        fbox->HPos = 0;
        flist->Clear();
        Refresh();
        DoCallback();
        return;
    }

    flist->Assign(indexes);

    inherited::set_cols(acolcnt, acolwidths, acoldefaultwidths, acolvisible, acoltypes, acolalign, indexes->Count);
    count = indexes->Count;
    pos = 0;
    fbox->HPos = 0;

    Refresh();
    DoCallback();
}
//---------------------------------------------------------------------------
void TWordListObj::set_pos_vpos(int apos, int vpos)
{
    fbox->VPos = vpos;
    pos = apos;
}
//---------------------------------------------------------------------------
void TWordListObj::find_in_continuous(const wchar_t *s)
{
    if (!fcontinuous)
        THROW(L"Word list is not in continuous mode");

    if (!s || ! *s || !count)
    {
        pos = 0;
        return;
    }

    int i;
    if (!wcscmp(currentword->kana, s) || (i = fcoll->WordPosition(NULL, s, ffiltered ? woFiltered : woSelect)) == pos)
        return;

    if (i >= 0)
        set_pos_vpos(i, i);
    DoCallback();
}
//---------------------------------------------------------------------------
void TWordListObj::set_to_continuous(TWordCollection *acoll, const wchar_t *aword, bool afiltered)
{
    ffiltered = afiltered;
    fcontinuous = true;
    fcoll = acoll;
    fcollection = dictionary->Index[fcoll];

    StartUpdating signal(this);

    count = dictionary->ListCount(fcollection, ffiltered ? woFiltered : woSelect);
    if (!count)
    {
        pos = -1;
    }
    else if (!aword)
        pos = 0;
    else
    {
        int i = fcoll->WordPosition(NULL, aword, ffiltered ? woFiltered : woSelect);
        if (i < 0)
            i = -i;
        if (i >= 0)
        {
            int h = (fbox->ClientHeight - (header ? headersize : 0)) / rowsize;
            set_pos_vpos(i, i - max(i ? 1 : 0, min(h*(double(i) / count), h - 1)));
        }
        else if (pos >= count)
            pos = count - 1;
        else
            DoCallback();
    }

    Refresh();
}
//---------------------------------------------------------------------------
int TWordListObj::IndexOf(int ix)
{
    if (!flist)
        return -1;
    return flist->IndexOf(ix);
}
//---------------------------------------------------------------------------
void TWordListObj::DoPrepareDraw(int col, int row)
{
    TWordListObjBase::DoPrepareDraw(col, row);
    if (items[row].inf)
    {
        int r, g, b;
        colordiff((TColor)colorsettings.background, fbox->Canvas->Brush->Color, r, g, b);
        fbox->Canvas->Brush->Color = colormod((TColor)colorsettings.wordinf, -r, -g, -b);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TVocabListObj::TVocabListObj(TZPaintBox *apaintbox, int arowsize, TFuncPtr afunc, TPopupWordProc aselfunc) : filtered(NULL), filterstr(L""), setfilters(NULL), flist(NULL), TWordListObjBase(apaintbox, arowsize, afunc, aselfunc)
{
    setfilters = new TWordFilterCopyList;
}
//---------------------------------------------------------------------------
TVocabListObj::~TVocabListObj()
{
    delete filtered;
    delete setfilters;
}
//---------------------------------------------------------------------------
void TVocabListObj::update_count(int &count)
{
    count = filtered ? filtered->Count : flist ? flist->Count : 0;
}
//---------------------------------------------------------------------------
void TVocabListObj::set_wordlist(TWordIndexList *val)
{
    if (flist == val)
        return;

    StartUpdating signal(this);

    RemoveColor(false);

    flist = val;
    filter_list();
    count = filtered ? filtered->Count : flist ? flist->Count : 0;

    pos = 0;
    fbox->HPos = 0;
    Refresh();

    DoCallback();
}
//---------------------------------------------------------------------------
void TVocabListObj::set_filter(UnicodeString afilterstr, TWordFilterSelection *afilter)
{
    afilterstr = Hiraganize(afilterstr.c_str());
    if (filterstr == afilterstr && (!afilter && !setfilters->Count || afilter && afilter->filters_match(setfilters)))
        return;

    int i = -1;
    char m = -1;
    if (pos >= 0)
    {
        i = indexes[pos]; /*items[pos].index;*/
        m = meaningindexes[pos]; /*items[pos].meaning;*/
    }

    StartUpdating signal(this);

    RemoveColor(false);

    filterstr = afilterstr;
    if (!afilter)
        setfilters->Clear();
    else
        afilter->create_copy(setfilters);
    filter_list();

    Update(FindWord(i, m, true));
}
//---------------------------------------------------------------------------
void TVocabListObj::set_filtered_list(TWordIndexList *wlist, UnicodeString afilterstr, TWordFilterSelection *afilter)
{
    afilterstr = Hiraganize(afilterstr.c_str());
    if (flist == wlist && filterstr == afilterstr && (!afilter && !setfilters->Count || afilter && afilter->filters_match(setfilters)))
        return;

    StartUpdating signal(this);

    RemoveColor(false);

    if (afilter)
        afilter->create_copy(setfilters);
    else
        setfilters->Clear();

    filterstr = afilterstr;

    flist = wlist;
    filter_list();
    count = filtered ? filtered->Count : flist ? flist->Count : 0;

    pos = 0;
    fbox->HPos = 0;
    Refresh();

    DoCallback();
}
//---------------------------------------------------------------------------
void TVocabListObj::filter_list()
{
    if (!filterstr.Length() && !setfilters->Count)
    {
        delete filtered;
        filtered = NULL;
        count = flist ? flist->Count : 0;
        return;
    }

    if (filtered)
        filtered->Clear();
    else
        filtered = new TIntegerList;

    if (!flist)
        return;

    bool strhaskanji = filterstr.Length() && has_kanji(filterstr.c_str());
    for (int ix = 0; ix < flist->Count; ++ix)
    {
        if (filterstr.Length())
        {
            if (strhaskanji)
            {
                if (wcsncmp(Hiraganize(flist->Words[ix]->kanji).c_str(), filterstr.c_str(), filterstr.Length()))
                    continue;
            }
            else
            {
                if (wcsncmp(Hiraganize(flist->Words[ix]->kana).c_str(), filterstr.c_str(), filterstr.Length()))
                    continue;
            }
        }
        if (setfilters->Count && !setfilters->match(flist->Words[ix], flist->Items[ix]->meaning))
            continue;
        filtered->Add(ix);
    }
}
//---------------------------------------------------------------------------
UnicodeString TVocabListObj::get_mnotes()
{
    if (pos < 0)
        return "";

    UnicodeString s = " ";

    int j = meaningindexes[pos]; /*items[pos].meaning;*/

    for (int k = 0; k < WT_COUNT; ++k)
    {
        if (words[pos]->data[j].types & (1 << k))
            s = s + wlongtypetext[k] + ", ";
    }

    for (int k = 0; k < WN_COUNT; ++k)
    {
        if (words[pos]->data[j].notes & (1 << k))
            s = s + wlongnotetext[k] + ", ";
    }

    for (int k = 0; k < WF_COUNT; ++k)
    {
        if (words[pos]->data[j].fields & (1 << k))
            s = s + wlongfieldtext[k] + ", ";
    }

    for (int k = 0; k < NM_COUNT; ++k)
    {
        if (words[pos]->data[j].nametag & (1 << k))
            s = s + nlongtagtext[k] + ", ";
    }

    s = s.TrimRight();
    if (s.Length() && s[s.Length()] == ',')
        s = s.SubString(1, s.Length() - 1);
    return s;
}
//---------------------------------------------------------------------------
int TVocabListObj::FindWord(int aindex, byte ameaning, bool in_filtered)
{
    if (!flist)
        return -1;
    if (!in_filtered || filtered == NULL)
        return flist->IndexOf(aindex, ameaning);

    for (int ix = 0; ix < filtered->Count; ++ix)
    {
        //TWordIndex item = items[ix];
        //if (item.index == aindex && item.meaning == ameaning)
        //    return ix;

        if (indexes[ix] == aindex && meaningindexes[ix] == ameaning)
            return ix;
    }
    return -1;
}
//---------------------------------------------------------------------------
TWordCollection* TVocabListObj::get_collection()
{
    if (!flist)
        return NULL;
    return flist->collection;
}
//---------------------------------------------------------------------------
TWordIndex TVocabListObj::get_item(unsigned int i)
{
    if (!flist)
        THROW(L"Error! No list for wordlist item!");
    if (filtered)
        return *flist->Items[filtered->Items[i]];
    return *flist->Items[i];
}
//---------------------------------------------------------------------------
int TVocabListObj::get_order(unsigned int ix)
{
    if (!flist)
        THROW(L"Error! No list for wordlist item!");
    if (filtered)
        return filtered->Items[ix];
    return ix;
}
//---------------------------------------------------------------------------
void TVocabListObj::Update(int aselstart, int asellength)
{
    if (filtered)
    {
        filter_list();
        count = filtered->Count;
    }
    inherited::Update(aselstart, asellength);
}
//---------------------------------------------------------------------------
void TVocabListObj::Update(int row)
{
    if (filtered)
    {
        filter_list();
        count = filtered->Count;
    }
    inherited::Update(row);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TVirtualWordListObj::TVirtualWordListObj(TZPaintBox *apaintbox, int arowsize, TFuncPtr afunc, TPopupWordProc aselfunc) : TWordListObjBase(apaintbox, arowsize, afunc, aselfunc)
{
    findex = NULL;
    Tag = 0;
    colresize[0] = true;
}
//---------------------------------------------------------------------------
TVirtualWordListObj::~TVirtualWordListObj()
{
}
//---------------------------------------------------------------------------
void TVirtualWordListObj::ResetDefaults()
{
}
//---------------------------------------------------------------------------
TWordIndex TVirtualWordListObj::get_item(unsigned int i)
{
    TWordIndex index;
    index.meaning = -1;
    index.inf = NULL;
    if (findex)
        index.index = findex(i);
    else
        index.index = 0;
    return index;
}
//---------------------------------------------------------------------------
void TVirtualWordListObj::update_count(int &count)
{
    ;
}
//---------------------------------------------------------------------------
void TVirtualWordListObj::set_virtual(TWordCollection *acoll, unsigned int newcount, TVirtualIndexProc aindex)
{
    StartUpdating signal(this);

    if (!aindex)
        newcount = 0;

    RemoveColor(false);

    coll = acoll;

    findex = aindex;
    count = newcount;

    if (pos >= count)
        pos = count - 1;
    else if (pos < 0)
        pos = 0;

    fbox->HPos = 0;
    Refresh();

    DoCallback();
}
//---------------------------------------------------------------------------
void TVirtualWordListObj::set_cols(TWordCollection *acoll, int acolcnt, int *acolwidths, int *acoldefaultwidths, bool *acolvisible, TWordColumnType *acoltypes, TAlignment *acolalign, int acount, TVirtualIndexProc aindex)
{
    if (!aindex)
        acount = 0;

    RemoveColor(false);

    if (acoll)
        coll = acoll;

    findex = aindex;

    inherited::set_cols(acolcnt, acolwidths, acoldefaultwidths, acolvisible, acoltypes, acolalign, acount);

    if (pos >= count)
        pos = count - 1;
    else if (pos < 0)
        pos = 0;

    fbox->HPos = 0;
    Refresh();

    DoCallback();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
TCallbackWordListObj::TCallbackWordListObj(TZPaintBox *apaintbox, int arowsize, TFuncPtr afunc, TPopupWordProc aselfunc)
        : inherited(apaintbox, arowsize, afunc, aselfunc), fnum(NULL), fstr(NULL), ftag(NULL)
{
}
//---------------------------------------------------------------------------
TCallbackWordListObj::~TCallbackWordListObj()
{
}
//---------------------------------------------------------------------------
void TCallbackWordListObj::ResetDefaults()
{
}
//---------------------------------------------------------------------------
TWordCollection* TCallbackWordListObj::get_collection()
{
    return NULL;
}
//---------------------------------------------------------------------------
void TCallbackWordListObj::update_count(int &count)
{
    ;
}
//---------------------------------------------------------------------------
TWordIndex TCallbackWordListObj::get_item(unsigned int i)
{
    TWordIndex result;
    result.index = -1;
    result.inf = NULL;
    result.meaning = -1;
    return result;
}
//---------------------------------------------------------------------------
word TCallbackWordListObj::itemfreq(unsigned int i)
{
    if (fnum)
        return fnum(i, wnpwFreq);
    return 0;
}
//---------------------------------------------------------------------------
UnicodeString TCallbackWordListObj::itemkanji(unsigned int i)
{
    if (fstr)
        return fstr(i, 0, wspwKanji);
    return UnicodeString();
}
//---------------------------------------------------------------------------
UnicodeString TCallbackWordListObj::itemkana(unsigned int i)
{
    if (fstr)
        return fstr(i, 0, wspwKana);
    return UnicodeString();
}
//---------------------------------------------------------------------------
byte TCallbackWordListObj::itemmeaningcount(unsigned int i)
{
    if (fnum)
        return fnum(i, wnpwMeaningCount);
    return 0;
}
//---------------------------------------------------------------------------
UnicodeString TCallbackWordListObj::itemmeaning(unsigned int i, byte m)
{
    if (fstr)
        return fstr(i, m, wspwMeaning);
    return UnicodeString();
}
//---------------------------------------------------------------------------
void TCallbackWordListObj::itemtags(unsigned int i, byte m, int &types, int &notes, byte &fields, word &nametag)
{
    types = 0;
    notes = 0;
    fields = 0;
    nametag = 0;
    if (ftag)
        ftag(i, m, types, notes, fields, nametag);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(191);
#endif

