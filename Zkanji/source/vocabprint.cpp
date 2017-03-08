//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "vocabprint.h"
#include "words.h"
#include "collection.h"
#include "regroup.h"
#include "settings.h"
#include "mouseevent.h"
#include "searchtree.h"
#include "main.h"
#include "module.h"
#include "modallist.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(172);
#endif

TfVocabPrint *fVocabPrint;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::z_init(TMessage &msg)
{
    if (!Visible)
        THROW(L"?");
    pbBar->Position = 0;
    pbView->Cursor = crHourGlass;
    pBar->Visible = true;
    Application->ProcessMessages();
    try
    {
        CreateMetaFiles();
    }
    catch (...)
    {
        delete l;
        l = NULL;
        FreeMetaFiles();
        FreeBmp();
        MsgBox(L"An error occurred during print preview initialization.", L"Error", MB_OK);
    }
    pbView->Cursor = crSizeAll;
}
//---------------------------------------------------------------------------
__fastcall TfVocabPrint::TfVocabPrint(TComponent* Owner) : TBaseForm(Owner)
{
    //RescaleStatusbar(sbInfo);

    Constrain(542*scaleres + (Width - ClientWidth), 246*scaleres + (Height - ClientHeight));

    state = psNone;
    l = NULL;
    buff = new TBmp;
    firsts = new TIntegerList;
    lefts = rights = NULL;
    p = Printer();
    p->PrinterIndex = settings.printerindex;
    p->Orientation = settings.vportrait == true ? poPortrait : poLandscape;
    pdSet->PrintToFile = settings.vtofile;
    dragging = false;
    image = NULL;
    imageb = NULL;
    printDC = NULL;
    printBMP = NULL;
    meaningfont = NULL;
    japanesefont = NULL;
    furiganafont = NULL;
    pagefont = NULL;
    margin = 8;
}
//---------------------------------------------------------------------------
__fastcall TfVocabPrint::~TfVocabPrint()
{
    free_fonts();
    if (l)
        delete l;
    delete firsts;
    if (lefts)
        delete[] lefts;
    if (rights)
        delete[] rights;
    settings.printerindex = p->PrinterIndex;
    settings.vportrait = p->Orientation == poPortrait;
    settings.vtofile = pdSet->PrintToFile;
    FreeMetaFiles();
    FreeBmp();
}
//---------------------------------------------------------------------------
int TfVocabPrint::ShowModalEx(TWordIndexList *al)
{
    l = new TWordIndexList(al);
    return ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::psdDrawFullPage(TObject *Sender, TCanvas *Canvas, TRect &PageRect, bool &DoneDrawing)
{
    Canvas->Brush->Color = clBlack;
    Canvas->FillRect(PageRect);
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::btnSetClick(TObject *Sender)
{
    if (ExecuteDialog(pdSet))
        CreateMetaFiles();
    pbView->SetFocus();
}
//---------------------------------------------------------------------------
void TfVocabPrint::FreeBmp()
{
    if (printBMP)
    {
        SelectObject(oldDC, printBMP);
        DeleteObject(printBMP);
        DeleteDC(printDC);
        printBMP = NULL;
        printDC = NULL;
    }
}
//---------------------------------------------------------------------------
void TfVocabPrint::FreeMetaFiles()
{
    if (image)
    {
        DeleteEnhMetaFile(image);
        image = NULL;
    }
    if (imageb)
    {
        DeleteEnhMetaFile(imageb);
        imageb = NULL;
    }
}
//---------------------------------------------------------------------------
void TfVocabPrint::CreateMetaFiles()
{
    FreeBmp();

    printDC = CreateCompatibleDC(p->Handle);
    printBMP = CreateCompatibleBitmap(p->Handle, p->PageWidth, p->PageHeight);
    oldDC = (HDC)SelectObject(printDC, printBMP);
    resx = GetDeviceCaps(printDC, HORZSIZE);
    resy = GetDeviceCaps(printDC, VERTSIZE);

    float d = (p->PageHeight / resy);
    marginh = margin * d;
    pagew = p->PageWidth - marginh * 2;
    pageh = p->PageHeight - marginh * 2;

    RecreatePages();

    MeasureScrollSize();
}
//---------------------------------------------------------------------------
void TfVocabPrint::create_fonts()
{
    free_fonts();
    HDC dc = GetDC(DESKTOPHANDLE);
    int lpy = GetDeviceCaps(dc, LOGPIXELSY);
    ReleaseDC(DESKTOPHANDLE, dc);

    meaningfont = create_localfont(settings.vdictfonts ? settings.romajifont : settings.vromajifont, DEFAULT_CHARSET, double(-(lineh * 0.5) * lpy) / 72 / scaleres, TFontStyles());
    japanesefont = create_localfont(settings.vdictfonts ? settings.kanafont : settings.vkanafont, settings.vkanacharset, double(-(lineh * 0.5) * lpy) / 72 / scaleres, TFontStyles());
    furiganafont = create_localfont(settings.vdictfonts ? settings.kanafont : settings.vkanafont, settings.vkanacharset, double(-(lineh * 0.26) * lpy) / 72 / scaleres, TFontStyles());
    pagefont = create_localfont(L"Arial", DEFAULT_CHARSET, double(p->PageHeight / resy) * 3.0, TFontStyles());
}
//---------------------------------------------------------------------------
void TfVocabPrint::free_fonts()
{
    if (!meaningfont)
        return;

    DeleteObject(meaningfont);
    DeleteObject(japanesefont);
    DeleteObject(furiganafont);
    DeleteObject(pagefont);
    meaningfont = NULL;
    japanesefont = NULL;
    furiganafont = NULL;
    pagefont = NULL;
}
//---------------------------------------------------------------------------
int TfVocabPrint::vget_width(TCanvas *c, TVPrintItem it, TWordIndexList *l, int ix)
{
    if (it == viNothing)
        return 0;

    HFONT oldfont;
    TSize s;
    if (it == viMeaning)
    {
        oldfont = (HFONT)SelectObject(c->Handle, meaningfont);
        wchar_t *str = l->Words[ix]->data[l->Items[ix]->meaning].meaning;
        GetTextExtentPoint32W(c->Handle, str, wcslen(str), &s);
        SelectObject(c->Handle, oldfont);
    }
    else if (it == viKanji || it == viKana)
    {
        oldfont = (HFONT)SelectObject(c->Handle, japanesefont);
        wchar_t *str = (it == viKanji ? l->Words[ix]->kanji : l->Words[ix]->kana);
        GetTextExtentPoint32W(c->Handle, str, wcslen(str), &s);
        SelectObject(c->Handle, oldfont);
    }
    return s.cx;
}
//return width in pixels of space a text takes up, when it can extend to maxw at most (it might be shorter)
int TfVocabPrint::vused_width(TCanvas *c, TVPrintItem it, TWordIndexList *l, int ix, int maxw, TFuriganaData *dat, int datcnt)
{
    if (it == viNothing)
        return 0;

    HFONT oldfont = NULL;
    TSize s;

    if (it == viKanji || it == viKana)
    {
        oldfont = (HFONT)SelectObject(c->Handle, japanesefont);
        wchar_t *str = (it == viKanji ? l->Words[ix]->kanji : l->Words[ix]->kana);
        int len = wcslen(str);
        int maxlen;
        int maxsize;
        int *sizes = new int[len];
        int datpos;

        try
        {
            GetTextExtentExPointW(c->Handle, str, len, maxw, &maxlen, sizes, &s);
            if (maxlen > len / 2)
            {
                maxlen = len * 0.67;
                while (sizes[maxlen - 1] > maxw)
                    maxlen--;
            }
            if (it == viKanji)
            {
                for (datpos = 0; datpos < datcnt && dat[datpos].kanji + dat[datpos].kanjilen <= maxlen; datpos++);
                if (datpos < datcnt && dat[datpos].kanji < maxlen && dat[datpos].kanji)
                    maxlen = dat[datpos].kanji;
            }

            maxsize = sizes[maxlen - 1];

            int pos = maxlen;
            while (pos < len)
            {
                GetTextExtentExPointW(c->Handle, str + pos, len - pos, maxw, &maxlen, sizes, &s);

                if (it == viKanji)
                {
                    for (; datpos < datcnt && dat[datpos].kanji + dat[datpos].kanjilen <= maxlen + pos; datpos++);
                    if (datpos < datcnt && dat[datpos].kanji < maxlen + pos && dat[datpos].kanji - pos)
                        maxlen = dat[datpos].kanji - pos;
                }
                maxsize = MAX(maxsize, sizes[maxlen - 1]);

                pos += maxlen;
            }

        }
        catch (...)
        {
            ;
        }
        delete[] sizes;
        SelectObject(c->Handle, oldfont);
        return maxsize;

    }
    else if (it == viMeaning)
    {
        oldfont = (HFONT)SelectObject(c->Handle, meaningfont);
        wchar_t *str = l->Words[ix]->data[l->Items[ix]->meaning].meaning;
        int k = wcslen(str);
        int cnum;
        int pos = 0;

        int maxmax = 0;

        int *sizes = new int[k];

        try
        {
            while (k)
            {
                GetTextExtentExPointW(c->Handle, str + pos, k, maxw, &cnum, sizes, &s);
                if (k > cnum)
                {
                    while (cnum && str[pos + cnum] != ' ' && str[pos + cnum - 1] != '-')
                        cnum--;
                    if (!cnum)
                        cnum = k; //doesn't fit anyway, leave it all in there
                }

                maxmax = MAX(maxmax, sizes[cnum - 1]);

                k -= cnum;
                pos += cnum;
                while (k && str[pos] == ' ')
                {
                    pos++;
                    k--;
                }
            }
        }
        catch (...)
        {
            maxmax = maxw;
        }
        delete[] sizes;

        SelectObject(c->Handle, oldfont);
        return maxmax;
    }

    return maxw;
}
void TfVocabPrint::vdraw_furigana(TCanvas *c, int x, int y, wchar_t *kanji, wchar_t *kana, int pos, int len, int *sizes, TFuriganaData *dat, int datcnt)
{
    TSize s;

    int oldalign = GetTextAlign(c->Handle);
    SetTextAlign(c->Handle, TA_LEFT | TA_BASELINE);

    //int oldsize = c->Font->Size;
    //c->Font->Size = c->Font->Size*0.52;
    HFONT oldfont = (HFONT)SelectObject(c->Handle, furiganafont/*c->Font->Handle*/);

    TRect r;
    int x2;
    try
    {
        int i = 0;
        while (i < datcnt && dat[i].kanji + dat[i].kanjilen <= pos + len)
        {
            if (dat[i].kanji < pos)
            {
                i++;
                continue;
            }

            int ksize = sizes[dat[i].kanji - pos + dat[i].kanjilen - 1] - (dat[i].kanji > pos ? sizes[dat[i].kanji - pos - 1] : 0)/*- fw*0.2*/;
            int fsize = ksize / dat[i].kanalen;
            x2 = x + (dat[i].kanji > pos ? sizes[dat[i].kanji - pos - 1] : 0);

            for (int j = 0; j < dat[i].kanalen; j++)
            {
                GetTextExtentPoint32W(c->Handle, kana + dat[i].kana + j, 1, &s);

                float ys = 1;
                if (dat[i].kanjilen*2 < dat[i].kanalen)
                    ys = 0.8;

                r = Rect(x2 + (fsize - s.cx * ys) / 2, y - s.cy, x2 + (fsize - s.cx * ys) / 2 + s.cx, y);

                ScaledTextOutExW(c, ys, 1, x2 + (fsize - s.cx*ys) / 2, y - s.cy*0.2, r, kana + dat[i].kana + j, 1);
                x2 += fsize;
            }
            i++;
        }
    }
    catch (...)
    {
        ;
    }
    //c->Font->Size = oldsize;
    SelectObject(c->Handle, oldfont);
    SetTextAlign(c->Handle, oldalign);
}
void TfVocabPrint::vdraw_single(TCanvas *c, TVPrintItem it, TWordIndexList *l, int ix, int x, int y, int linew, int lineh, bool left, TFuriganaData *dat, int datcnt)
{
    if (it == viNothing)
        return;

    int d = 0;
    if (settings.vfurigana && (!settings.vseparate && (settings.vleft == viKanji || settings.vright == viKanji) || it == viKanji) && has_kanji(l->Words[ix]->kanji))
        d = lineh * 0.3;

    HFONT oldfont = NULL;
    switch (it)
    {
    case viKana:
    case viKanji:
        oldfont = (HFONT)SelectObject(c->Handle, japanesefont);
        break;
    case viMeaning:
        oldfont = (HFONT)SelectObject(c->Handle, meaningfont);
        break;
    }
    //vset_font(c,it,lineh);
    TRect r;
    if (left)
    {
        SetTextAlign(c->Handle, TA_BASELINE | TA_LEFT);
        r = Rect(x, y, x + linew, y + lineh + d);
    }
    else
    {
        SetTextAlign(c->Handle, TA_BASELINE | TA_RIGHT);
        r = Rect(x - linew, y, x, y + lineh + d);
    }
    if (it == viMeaning)
    {
        wchar_t *str = l->Words[ix]->data[l->Items[ix]->meaning].meaning;
        ExtTextOutW(c->Handle, x, y + lineh*0.8 + d, ETO_CLIPPED, &r, str, wcslen(str), NULL);
    }
    else if (it == viKanji || it == viKana)
    {
        wchar_t *str = (it == viKanji ? l->Words[ix]->kanji : l->Words[ix]->kana);
        int len = wcslen(str);
        ExtTextOutW(c->Handle, x, y + lineh*0.8 + d, ETO_CLIPPED, &r, str, len, NULL);
        if (it == viKanji && datcnt)
        {
            int *sizes = new int[len];
            TSize s;
            GetTextExtentExPointW(c->Handle, str, len, linew, NULL, sizes, &s);
            try
            {
                vdraw_furigana(c, r.left, y + lineh / 2, str, l->Words[ix]->kana, 0, len, sizes, dat, datcnt);
            }
            catch (...)
            {
                ;
            }
            delete[] sizes;
        }
    }
    SelectObject(c->Handle, oldfont);
}
int TfVocabPrint::vdraw_text(TCanvas *c, TVPrintItem it, TWordIndexList *l, int ix, int x, int y, int linew, int lineh, bool left, bool measure, TFuriganaData *dat, int datcnt)
{
    if (it == viNothing)
        return lineh;

    HFONT oldfont = NULL;
    switch (it)
    {
    case viKana:
    case viKanji:
        oldfont = (HFONT)SelectObject(c->Handle, japanesefont);
        break;
    case viMeaning:
        oldfont = (HFONT)SelectObject(c->Handle, meaningfont);
        break;
    }
    //vset_font(c,it,lineh);

    TSize s;
    TRect r;

    int d = 0;
    if (settings.vfurigana && (!settings.vseparate && (settings.vleft == viKanji || settings.vright == viKanji) || it == viKanji) && has_kanji(l->Words[ix]->kanji))
        d = lineh * 0.3;

    int re = 0;

    if (it == viKanji || it == viKana)
    {
        wchar_t *str = (it == viKanji ? l->Words[ix]->kanji : l->Words[ix]->kana);
        int len = wcslen(str);
        int *sizes;
        int maxlen;
        int rows = 0;
        int pos = 0;
        int datpos = 0;

        if (!measure)
        {
            if (left)
            {
                SetTextAlign(c->Handle, TA_LEFT | TA_BASELINE);
                r = Rect(x, y + lineh * 0.2, x + linew, y + lineh + d);
            }
            else
            {
                SetTextAlign(c->Handle, TA_RIGHT | TA_BASELINE);
                r = Rect(x - linew, y + lineh * 0.2, x, y + lineh + d);
            }
        }

        sizes = new int[len];
        try
        {
            while (pos < len)
            {
                rows++;

                GetTextExtentExPointW(c->Handle, str + pos, len - pos, linew, &maxlen, sizes, &s);
                if (it == viKanji)
                {
                    for (; datpos < datcnt && dat[datpos].kanji + dat[datpos].kanjilen <= maxlen + pos; datpos++);
                    if (datpos < datcnt && dat[datpos].kanji < maxlen + pos && dat[datpos].kanji - pos)
                        maxlen = dat[datpos].kanji - pos;
                }

                if (!measure)
                {
                    ExtTextOutW(c->Handle, x, y + lineh*0.8 + d, ETO_CLIPPED, &r, str + pos, maxlen, 0);
                    if (it == viKanji && datcnt)
                        vdraw_furigana(c, r.right - sizes[maxlen - 1], y + lineh / 2, str, l->Words[ix]->kana, pos, maxlen, sizes, dat, datcnt);

                    OffsetRect(r, 0, lineh*0.8 + d);
                    y += lineh * 0.8 + d;
                }

                pos += maxlen;
            }
        }
        catch (...)
        {
            ;
        }
        delete[] sizes;

        re = lineh * (rows > 1 ? 1.2 : 1) + (lineh * 0.8 * (rows - 1)) + rows * d;

    }
    else if (it == viMeaning)
    {
        wchar_t *str = l->Words[ix]->data[l->Items[ix]->meaning].meaning;
        int k = wcslen(str);
        int pos = 0;
        int cnum;
        y += d;

        if (!measure)
        {
            if (left)
            {
                SetTextAlign(c->Handle, TA_LEFT | TA_BASELINE);
                r = Rect(x, y, x + linew, y + lineh);
            }
            else
            {
                SetTextAlign(c->Handle, TA_RIGHT | TA_BASELINE);
                r = Rect(x - linew, y, x, y + lineh);
            }
        }

        while (k)
        {
            re += lineh * 0.8;
            GetTextExtentExPointW(c->Handle, str + pos, k, linew, &cnum, NULL, &s);
            if (k > cnum)
            {
                while (cnum && str[pos + cnum] != ' ' && str[pos + cnum - 1] != '-')
                    cnum--;
                if (!cnum)
                    cnum = k; //doesn't fit anyway, leave it all in there
            }

            if (!measure)
            {
                ExtTextOutW(c->Handle, x, y + lineh*0.8, ETO_CLIPPED, &r, str + pos, cnum, 0);
                y += lineh * 0.8;
                OffsetRect(r, 0, lineh*0.8);
            }

            k -= cnum;
            pos += cnum;
            while (k && str[pos] == ' ')
            {
                pos++;
                k--;
            }
        }
        re -= lineh * 0.8;
        if (!re)
            re = lineh;
        else
            re += lineh * 1.2;
        re += d;
    }

    SelectObject(c->Handle, oldfont);

    return re;
}
int TfVocabPrint::DrawPage(TCanvas *c, TRect r, int page, int ix, bool measure)
{
    int y = r.top;
    int dy;
    int gap = marginh / 2;
    int sidewidth = (r.Width() - gap) / 2;

    int sx = ix;

    int mm1 = (p->PageHeight / resy);

    int w1, w2;
    int d1, d2;
    int h1, h2;
    bool rightside = settings.vright != viNothing && !settings.vseparate;
    bool oddpage = settings.vseparate && (page % 2);
    TVPrintItem leftitem = (!oddpage ? settings.vleft : settings.vright);
    TVPrintItem rightitem = settings.vright;
    TVPrintSizes *ls = NULL;
    TVPrintSizes *rs = NULL;
    while (ix < l->Count)
    {
        ls = lefts + ix;
        rs = rights + ix;
        if (!measure && y + MAX(ls->height, rs->height) > r.bottom)
            break;

        if (!measure)
        {
            if (settings.vshade && (ix - sx + (r.left == marginh ? 0 : 1)) % 2)
            {
                c->Brush->Color = (TColor)0xf0f0f0;
                c->Brush->Style = bsSolid;
                SelectObject(c->Handle, c->Brush);
                c->FillRect(Rect(r.left - mm1, y, r.right + mm1, y + MAX(ls->height, rs->height)));
            }
            c->Brush->Style = bsClear;
            SelectObject(c->Handle, c->Brush);
        }

        if (measure)
        {
            //vset_font(c,leftitem,lineh);
            d1 = w1 = vget_width(c, leftitem, l, ix);
            if (!rightside)
                d2 = w2 = 0;
            else
            {
                //vset_font(c,rightitem,lineh);
                d2 = w2 = vget_width(c, rightitem, l, ix);
            }
            if (!oddpage)
            {
                ls->width = w1;
                if (rightside)
                    rs->width = w2;
                else if (!settings.vseparate)
                    rs->width = 0;
            }
            else
                rs->width = w1;
        }

        TFuriganaData *dat = NULL;
        int datcnt = 0;

        if ((leftitem == viKanji || rightitem == viKanji) && settings.vfurigana)
            datcnt = FindFurigana(l->Words[ix], dat);

        try
        {
            if (ls->width + rs->width < sidewidth * 2 || oddpage && rs->width <= r.Width())
            {
                if (!measure)
                {
                    vdraw_single(c, leftitem, l, ix, r.left, y, (oddpage ? rs->width : ls->width), lineh, true, dat, datcnt);
                    if (rightside)
                        vdraw_single(c, rightitem, l, ix, r.right, y, rs->width, lineh, false, dat, datcnt);
                }
                else
                {
                    bool haskanji = false;
                    if (settings.vfurigana && (leftitem == viKanji || rightitem == viKanji))
                        haskanji = has_kanji(l->Words[ix]->kanji);

                    ls->height = rs->height = MAX4(ls->height, rs->height, (int)((double)lineh * (haskanji ? 1.5 : 1)), 0);
                }
                dy = ls->height;
            }
            else
            {
                h2 = 0;

                if (measure)
                {
                    if (!settings.vseparate && w1 + w2 > sidewidth * 2 && (w1 <= sidewidth || w2 <= sidewidth))
                    {
                        if (!w1)
                            w2 = r.Width();
                        else if (!w2)
                            w1 = r.Width();
                        else if (w2 > w1)
                            w2 = sidewidth * 2 - w1;
                        else if (w1 > w2)
                            w1 = sidewidth * 2 - w2;
                        else
                            w1 = w2 = sidewidth;
                    }
                    else if (settings.vright == viNothing || settings.vseparate)
                        w1 = r.Width();
                    else if (w1 + w2 > sidewidth * 2)
                    {
                        w1 = MIN(((double)(sidewidth * 2) / (w1 + w2)) * w1, (float)(sidewidth * 2) * 0.7);
                        w2 = sidewidth * 2 - w1;
                    }

                    //maybe we can win some space
                    if (!settings.vseparate && w1 && w2 && (w1 != d1 || w2 != d2))
                    {
                        if (w1 != d1)
                        {
                            //vset_font(c,leftitem,lineh);
                            w1 = vused_width(c, leftitem, l, ix, w1, dat, datcnt);
                        }
                        if (w2 != d2)
                        {
                            //vset_font(c,rightitem,lineh);
                            w2 = vused_width(c, rightitem, l, ix, w2, dat, datcnt);
                        }

                        if (d1 - w1 && d1 - w1 < d2 - w2 && d1 + w2 <= sidewidth * 2)
                        {
                            w1 = d1;
                            w2 = sidewidth * 2 - w1;
                        }
                        else if (d2 - w2 && d2 - w2 < d1 - w1 && d2 + w1 <= sidewidth * 2)
                        {
                            w2 = d2;
                            w1 = sidewidth * 2 - w2;
                        }
                        else if (w1 + w2 < sidewidth * 2)
                        {
                            if (leftitem == viMeaning)
                                w1 = sidewidth * 2 - w2;
                            else if (rightitem == viMeaning)
                                w2 = sidewidth * 2 - w1;
                        }
                    }

                    h1 = vdraw_text(c, leftitem, l, ix, r.left, y, w1, lineh, true, measure, dat, datcnt);
                    if (rightside)
                        h2 = vdraw_text(c, rightitem, l, ix, r.right, y, w2, lineh, false, measure, dat, datcnt);
                    dy = MAX(h1, h2);

                    if (!oddpage)
                    {
                        ls->width = w1;
                        if (rightside)
                            rs->width = w2;
                    }
                    if (oddpage)
                        rs->width = w1;

                    if (settings.vseparate)
                    {
                        if (page % 2)
                        {
                            rs->height = MAX(ls->height, h1);
                            dy = ls->height = rs->height;
                        }
                        else
                        {
                            ls->height = MAX(rs->height, h1);
                            dy = rs->height = ls->height;
                        }
                    }
                    else
                    {
                        ls->height = h1;
                        rs->height = h2;
                    }

                }
                else
                {
                    vdraw_text(c, leftitem, l, ix, r.left, y, (oddpage ? rs->width : ls->width), lineh, true, measure, dat, datcnt);
                    if (rightside)
                        vdraw_text(c, rightitem, l, ix, r.right, y, rs->width, lineh, false, measure, dat, datcnt);
                    dy = MAX(ls->height, rs->height);
                }
            }
        }
        catch (...)
        {
            ;
        }

        if (dat && datcnt)
            delete[] dat;

        if (!measure && settings.vlines && y > r.top)
        {
            c->MoveTo(r.left - mm1, y);
            c->LineTo(r.right + mm1, y);
        }

        if (y + dy > r.bottom)
            break;

        y += dy;
        ix++;
    }

    if (!measure && ls && rs && settings.vlines && y > r.top)
    {
        c->MoveTo(r.left - mm1, y);
        c->LineTo(r.right + mm1, y);
    }

    return ix;
}
//---------------------------------------------------------------------------
void TfVocabPrint::ProcessLines()
{
    firsts->Clear();
    if (lefts)
    {
        delete[] lefts;
        lefts = NULL;
    }
    if (rights)
    {
        delete[] rights;
        lefts = NULL;
    }
    firsts->Add(0);
    if (l->Count)
    {
        lefts = new TVPrintSizes[l->Count];
        rights = new TVPrintSizes[l->Count];
        memset(lefts, 0, sizeof(TVPrintSizes)*l->Count);
        memset(rights, 0, sizeof(TVPrintSizes)*l->Count);
    }

    int ix = 0;
    int w = p->PageWidth - marginh * 2;
    if (settings.vdouble)
        w = (w - marginh) / 2;

    TRect r = Rect(marginh, marginh, w + marginh, p->PageHeight - marginh);

    TCanvas *c = new TCanvas;
    c->Handle = printDC;
    create_fonts();

    try
    {
        pbBar->Max = l->Count;
        pbBar->Position = 0;
        pbView->Cursor = crHourGlass;
        state = psPreparing;

        btnPrint->Enabled = false;
        btnSet->Enabled = false;
        btnSettings->Enabled = false;
        btnList->Enabled = false;

        pBar->Visible = true;
        while (ix < l->Count && state == psPreparing)
        {
            Application->ProcessMessages();
            if (settings.vseparate)
                ix = MIN(DrawPage(c, r, (firsts->Count - 1) * 2 + 1, ix, true), DrawPage(c, r, (firsts->Count - 1) * 2, ix, true));
            else
                ix = DrawPage(c, r, firsts->Count - 1, ix, true);
            Application->ProcessMessages();
            pbBar->Position = ix;
            firsts->Add(ix);
            Application->ProcessMessages();
        }

        pagecnt = firsts->Count - 1;
        if (settings.vdouble)
            pagecnt = (pagecnt + 1) / 2;
        if (settings.vseparate)
            pagecnt *= 2;
    }
    catch (...)
    {
        ;
    }
    //btnAbort->Enabled = true;
    pBar->Visible = false;
    pbView->Cursor = crSizeAll;

    btnPrint->Enabled = true;
    btnSet->Enabled = true;
    btnSettings->Enabled = true;
    btnList->Enabled = true;

    delete c;

    if (state != psPreparing)
        Close();
    else
        state = psNone;
}
//---------------------------------------------------------------------------
void TfVocabPrint::RecreatePages()
{
    int lineheights[] = { 4, 5, 6, 7, 8 };

    float d = (p->PageHeight / resy);
    lineh = d * lineheights[settings.vheight];

    ProcessLines();

    btnTwo->Enabled = pagecnt > 1;

    page = 0;
    if (pdSet->MaxPage != pagecnt)
    {
        pdSet->FromPage = MIN(pagecnt, pdSet->FromPage);
        pdSet->ToPage = pdSet->ToPage == pdSet->MaxPage ? pagecnt : MIN(pagecnt, pdSet->ToPage);
        pdSet->MaxPage = pagecnt;
    }

    UpdatePages();

    if (!btnTwo->Enabled && !btnOne->Down)
    {
        btnOne->Down = true;
        btnOne->Click();
    }
}
//---------------------------------------------------------------------------
void TfVocabPrint::UpdatePages()
{
    FreeMetaFiles();
    CreatePage(image, page);
    if (pagecnt > 1 && page + 1)
        CreatePage(imageb, page + 1);
}
//---------------------------------------------------------------------------
void TfVocabPrint::CreatePage(HENHMETAFILE &meta, int page)
{
    if (page < 0 || page >= pagecnt)
    {
        meta = NULL;
        return;
    }

    TRect r = Rect(0, 0, (resx) * 100 - 1, (resy) * 100 - 1);
    HDC mdc = CreateEnhMetaFileW(printDC, NULL, &r, NULL);
    TCanvas *c = new TCanvas;
    HFONT oldfont = (HFONT)SelectObject(mdc, c->Font->Handle);
    c->Pen->Width = MAX(1.0, ((double)p->PageHeight / resy) * 0.2);
    HPEN oldpen = (HPEN)SelectObject(mdc, c->Pen->Handle);
    HBRUSH oldbrush = (HBRUSH)SelectObject(mdc, c->Brush->Handle);

    int ix;
    int w = p->PageWidth - marginh * 2;
    if (settings.vdouble)
        w = (w - marginh) / 2;

    try
    {
        c->Handle = mdc;
        create_fonts();

        SelectObject(mdc, pagefont);
        SetTextAlign(mdc, TA_TOP | TA_CENTER);
        if (settings.vnumber)
        {
            UnicodeString str = (UnicodeString)L"= " + (page + 1) + L" =";
            ExtTextOutW(mdc, p->PageWidth / 2, p->PageHeight - marginh*0.8, 0, &r, str.c_str(), str.Length(), 0);
        }

        r = Rect(marginh, marginh, w + marginh, p->PageHeight - marginh);
        ix = firsts->Items[(settings.vseparate ? page / 2 : page) * (settings.vdouble ? 2 : 1)];
        DrawPage(c, r, page, ix, false);

        if (settings.vdouble)
        {
            r = Rect(p->PageWidth - marginh - w, marginh, p->PageWidth - marginh, p->PageHeight - marginh);
            ix = firsts->Items[(settings.vseparate ? page / 2 : page) * (settings.vdouble ? 2 : 1) + 1];
            DrawPage(c, r, page, ix, false);
        }

    }
    catch (...)
    {
        ;
    }
    SelectObject(mdc, oldfont);
    SelectObject(mdc, oldpen);
    SelectObject(mdc, oldbrush);
    meta = CloseEnhMetaFile(mdc);
    delete c;
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::btnListClick(TObject *Sender)
{
    fRegroup = new TfRegroup(Application);
    bool changed = false;
    try
    {
        changed = fRegroup->ShowModalEx(l) == mrOk;
    }
    catch (...)
    {
        delete fRegroup;
        MsgBox(L"An error occurred while managing the word list.", L"Error", MB_OK);
    }
    delete fRegroup;
    if (changed)
    {
        RecreatePages();
        pbView->Refresh();
    }
    pbView->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::pbViewPaint(TObject *Sender)
{
    TRect r, r2;

    if (buff->Width != pbView->ClientWidth || buff->Height != pbView->ClientHeight)
    {
        buff->Width = pbView->ClientWidth;
        buff->Height = pbView->ClientHeight;
    }

    int pw = 0;
    int ph = 0;
    TCanvas *c = buff->Canvas;
    Printers::TPrinterOrientation ori = p->Orientation;

    if (!image)
    {
        c->Brush->Color = clAppWorkSpace;
        c->FillRect(pbView->ClientRect);
    }
    else if (btnTwo->Down)
    {
        if (ori == poPortrait)
        {
            ph = pbView->ClientHeight - 50;
            pw = (pbView->ClientWidth / 2) - 20;
        }
        else
        {
            ph = (pbView->ClientHeight / 2) - 20;
            pw = pbView->ClientWidth - 50;
        }
        pw = MIN((double)pw, ((double)p->PageWidth / p->PageHeight) * ph);
        ph = MIN((double)ph, ((double)p->PageHeight / p->PageWidth) * pw);

        if (ori == poPortrait)
            r = Rect((pbView->ClientWidth / 2 - pw) / 2, (pbView->ClientHeight - ph) / 2, (pbView->ClientWidth / 2 + pw) / 2, (pbView->ClientHeight + ph) / 2);
        else
            r = Rect((pbView->ClientWidth - pw) / 2, (pbView->ClientHeight / 2 - ph) / 2, (pbView->ClientWidth + pw) / 2, (pbView->ClientHeight / 2 + ph) / 2);

        r2 = Rect(0, 0, pbView->ClientWidth / (ori == poPortrait ? 2 : 1), pbView->ClientHeight / (ori == poPortrait ? 1 : 2));

        c->Brush->Color = clAppWorkSpace;
        c->FillRect(Rect(r2.left, r2.top, r2.right, r.top));
        c->FillRect(Rect(r2.left, r.bottom, r2.right, r2.bottom));
        c->FillRect(Rect(r2.left, r.top, r.left, r.bottom));
        c->FillRect(Rect(r.right, r.top, r2.right, r.bottom));
        c->Brush->Color = clWhite;

        c->FillRect(r);
        PlayEnhMetaFile(c->Handle, image, &r);

        if (ori == poPortrait)
        {
            OffsetRect(r, pbView->ClientWidth / 2, 0);
            OffsetRect(r2, pbView->ClientWidth / 2, 0);
        }
        else
        {
            OffsetRect(r, 0, pbView->ClientHeight / 2);
            OffsetRect(r2, 0, pbView->ClientHeight / 2);
        }

        c->Brush->Color = clAppWorkSpace;
        c->FillRect(Rect(r2.left, r2.top, r2.right, r.top));
        c->FillRect(Rect(r2.left, r.bottom, r2.right, r2.bottom));
        c->FillRect(Rect(r2.left, r.top, r.left, r.bottom));
        c->FillRect(Rect(r.right, r.top, r2.right, r.bottom));
        c->Brush->Color = clWhite;

        if (imageb)
        {
            c->FillRect(r);
            PlayEnhMetaFile(c->Handle, imageb, &r);
        }
        else
        {
            c->Brush->Color = clAppWorkSpace;
            c->FillRect(r);
        }
    }
    else
    {
        if (btnFit->Down)
        {
            if (ori == poPortrait)
            {
                ph = pbView->ClientHeight - 50;
                pw = pbView->ClientWidth - 40;
            }
            else
            {
                pw = pbView->ClientWidth - 50;
                ph = pbView->ClientHeight - 40;
            }
            pw = MIN((double)pw, ((double)p->PageWidth / p->PageHeight) * ph);
            ph = MIN((double)ph, ((double)p->PageHeight / p->PageWidth) * pw);
        }
        else
        {
            pw = ((double)pbView->ClientWidth * zoom) / 100;
            ph = ((double)p->PageHeight / p->PageWidth) * (((float)pbView->ClientWidth * zoom) / 100);
        }

        if (btnFit->Down)
            r = Rect((pbView->ClientWidth - pw) / 2, (pbView->ClientHeight - ph) / 2, (pbView->ClientWidth + pw) / 2, (pbView->ClientHeight + ph) / 2);
        else
        {
            r = Rect(0, 0, pw, ((float)p->PageHeight / p->PageWidth) * (((float)pbView->ClientWidth * zoom) / 100));
            if (pw < pbView->ClientWidth)
                OffsetRect(r, (pbView->ClientWidth - pw) / 2, 0);
            if (ph < pbView->ClientHeight)
                OffsetRect(r, 0, (pbView->ClientHeight - ph) / 2);
        }
        if (!btnFit->Down)
            OffsetRect(r, -pbView->HPos, -pbView->VPos);
        r2 = Rect(0, 0, pbView->ClientWidth, pbView->ClientHeight);
        c->Brush->Color = clAppWorkSpace;
        if (r.top > 0)
            c->FillRect(Rect(r2.left, r2.top, r2.right, r.top));
        if (r.bottom < pbView->ClientHeight)
            c->FillRect(Rect(r2.left, r.bottom, r2.right, r2.bottom));
        if (r.left > 0)
            c->FillRect(Rect(r2.left, r.top, r.left, r.bottom));
        if (r.right < pbView->ClientWidth)
            c->FillRect(Rect(r.right, r.top, r2.right, r.bottom));

        c->Brush->Color = clWhite;
        c->FillRect(r);
        PlayEnhMetaFile(c->Handle, image, &r);
    }
    pbView->Canvas->Draw(0, 0, buff);
    draww = pw;
    drawh = ph;
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::pbViewResize(TObject *Sender)
{
    pbView->Refresh();
}
//---------------------------------------------------------------------------
void TfVocabPrint::set_page(int val)
{
    fpage = MAX(0, MIN(pagecnt, val));
    lbPage->Caption = IntToStr(fpage + 1) + " / " + IntToStr(pagecnt);
    btnLeft->Enabled = fpage > 0;
    btnRight->Enabled = fpage < pagecnt - 1;

    FreeMetaFiles();
    CreatePage(image, page);
    if (pagecnt > 1 && fpage < pagecnt - 1)
        CreatePage(imageb, page + 1);
    pbView->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::FormPaint(TObject *Sender)
{
    DrawButtonBar(this, NULL, bv0);
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::btnTwoClick(TObject *Sender)
{
    btnFill->Enabled = ((TSpeedButton*)Sender)->Tag == 1;
    btnFit->Enabled = ((TSpeedButton*)Sender)->Tag == 1;
    btnOut->Enabled = ((TSpeedButton*)Sender)->Tag == 1;
    btnIn->Enabled = ((TSpeedButton*)Sender)->Tag == 1;
    if (btnFit->Enabled && btnFill->Down)
    {
        btnFit->Down = true;
        btnFit->Click();
    }
    else
        MeasureScrollSize();
    pbView->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::FormResize(TObject *Sender)
{
    MeasureScrollSize();
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::btnFillClick(TObject *Sender)
{
    btnFill->AllowAllUp = false;
    zoom = 100;
    pbView->VPos = 0;
    pbView->HPos = 0;
    pbView->Refresh();
    MeasureScrollSize();
}
//---------------------------------------------------------------------------
void TfVocabPrint::MeasureScrollSize()
{
    if (btnFit->Down || btnTwo->Down)
    {
        pbView->VPos = 0;
        pbView->HPos = 0;
        pbView->VMax = 0;
        pbView->HMax = 0;
        pbView->Refresh();
        SetInfo();
        return;
    }

    int sh = GetSystemMetrics(SM_CYHSCROLL);
    int sw = GetSystemMetrics(SM_CXVSCROLL);
    int ch = pbView->ClientHeight + (pbView->HMax > pbView->ClientWidth ? sh : 0);
    int cw = pbView->ClientWidth + (pbView->VMax > pbView->ClientHeight ? sw : 0);

    if (draww > cw)
    {
        ch -= sh;
        if (drawh > ch)
            cw -= sw;
    }
    else if (drawh > ch)
    {
        cw -= sw;
        if (draww > cw)
            ch -= sh;
    }

    float vp = pbView->VMax ? (float)(pbView->VPos + ch / 2) / pbView->VMax : 0;
    float hp = pbView->HMax ? (float)(pbView->HPos + cw / 2) / pbView->HMax : 0;

    pbView->HMax = (zoom > 100 ? (((float)cw * zoom) / 100) : 0);
    pbView->VMax = ((float)p->PageHeight / p->PageWidth) * (((float)cw * zoom) / 100);
    pbView->VertScrollBar->ThumbSize = pbView->ClientHeight - 1;
    pbView->HorzScrollBar->ThumbSize = pbView->ClientWidth - 1;

    pbView->VPos = pbView->VMax * vp - ch / 2;
    pbView->HPos = pbView->HMax * hp - cw / 2;

    pbView->VertScrollBar->LargeChange = ch - 30;
    pbView->HorzScrollBar->LargeChange = cw - 30;
    pbView->VertScrollBar->SmallChange = ch / 20;
    pbView->HorzScrollBar->SmallChange = cw / 20;

    if (pbView->Visible)
        pbView->Refresh();
    SetInfo();
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::btnFitClick(TObject *Sender)
{
    btnFit->AllowAllUp = false;
    MeasureScrollSize();
    pbView->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::btnOutClick(TObject *Sender)
{
    if (btnFit->Down)
        zoom = ((float)draww / pbView->ClientWidth) * 100;
    int oldzoom = zoom;
    zoom = MAX(zoom - 25, 25);
    btnFill->AllowAllUp = true;
    btnFill->Down = false;
    btnFit->Down = false;
    if (oldzoom != zoom)
    {
        pbView->Refresh();
        MeasureScrollSize();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::btnInClick(TObject *Sender)
{
    if (btnFit->Down)
        zoom = ((float)draww / pbView->ClientWidth) * 100;
    int oldzoom = zoom;
    zoom = MIN(zoom + 25, 400);
    btnFill->AllowAllUp = true;
    btnFill->Down = false;
    btnFit->Down = false;
    if (oldzoom != zoom)
    {
        pbView->Refresh();
        MeasureScrollSize();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::pbViewMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (btnFit->Down || btnTwo->Down)
        return;
    dragpos = Point(x + pbView->HPos, y + pbView->VPos);
    dragging = Button == mbLeft;
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::pbViewMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    if (!dragging)
        return;
    static TPoint oldpos = pbView->ClientToScreen(Point(x, y));

    TPoint newpos = pbView->ClientToScreen(Point(x, y));

    if (pbView->HPos > 0 && oldpos.x < newpos.x && newpos.x >= Monitor->Left + Monitor->Width - 2)
    {
        newpos.x = Monitor->Left;
        dragpos.x -= Monitor->Width;
    }
    else if (pbView->HPos < pbView->HMax - pbView->ClientWidth && oldpos.x > newpos.x && newpos.x <= Monitor->Left)
    {
        newpos.x += Monitor->Width - 1;
        dragpos.x += Monitor->Width;
    }

    if (pbView->VPos > 0 && oldpos.y < newpos.y && newpos.y >= Monitor->Top + Monitor->Height - 2)
    {
        newpos.y = Monitor->Top;
        dragpos.y -= Monitor->Height;
    }
    else if (pbView->VPos < pbView->VMax - pbView->ClientHeight && oldpos.y > newpos.y && newpos.y <= Monitor->Top)
    {
        newpos.y = Monitor->Height - 1;
        dragpos.y += Monitor->Height;
    }

    TPoint setpos = pbView->ScreenToClient(newpos);
    if (setpos.x != x || setpos.y != y)
    {
        SetCursorPos(newpos.x, newpos.y);
        x = setpos.x;
        y = setpos.y;
    }
    oldpos = pbView->ClientToScreen(Point(x, y));

    pbView->VPos += (dragpos.y - pbView->VPos) - y;
    pbView->HPos += (dragpos.x - pbView->HPos) - x;
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::pbViewMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    dragging = false;
}
//---------------------------------------------------------------------------
void TfVocabPrint::SetInfo()
{
    UnicodeString s = IntToStr(zoom);
    if (btnFit->Down || btnTwo->Down)
        s = IntToStr((int)((float)draww / pbView->ClientWidth * 100));

    sbInfo->Panels->Items[1]->Text = s + L"%";
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::btnSettingsClick(TObject *Sender)
{
    bool vdictfonts = settings.vdictfonts;
    UnicodeString vkanafont = settings.vkanafont;
    UnicodeString vromajifont = settings.vromajifont;
    UnicodeString kanafont = settings.vkanafont;
    UnicodeString romajifont = settings.vromajifont;
    int vheight = settings.vheight;
    TVPrintItem vleft = settings.vleft;
    TVPrintItem vright = settings.vright;
    bool vseparate = settings.vseparate;
    bool vdouble = settings.vdouble;
    bool vfurigana = settings.vfurigana;

    if (ShowSettings(L"Word list printing"))
    {
        if ((vdictfonts != settings.vdictfonts) || (!vdictfonts && (settings.vkanafont != vkanafont || settings.vromajifont != vromajifont)) || (vdictfonts && (settings.kanafont != kanafont || settings.romajifont != romajifont)) ||

            settings.vheight != vheight || settings.vleft != vleft || settings.vright != vright || settings.vseparate != vseparate || settings.vdouble != vdouble || (settings.vfurigana != vfurigana && (settings.vleft == viKanji || settings.vright == viKanji)))
            RecreatePages();
        else
            UpdatePages();
        pbView->Refresh();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::btnPrintClick(TObject *Sender)
{
    state = psPrinting;
    HENHMETAFILE img = 0;
    TRect r;
    p->BeginDoc();

    DisableAll();

    pBar->Enabled = true;
    //btnAbort->Enabled = true;
    pbBar->Enabled = true;

    pbView->Cursor = crHourGlass;
    pBar->Visible = true;
    try
    {
        pbBar->Max = pdSet->ToPage - pdSet->FromPage + 1;
        pbBar->Position = 0;
        for (int i = pdSet->FromPage - 1; i < pdSet->ToPage && state == psPrinting; i++ , pbBar->Position++)
        {

            Application->ProcessMessages();
            if (state != psPrinting)
                break;

            CreatePage(img, i);

            Application->ProcessMessages();
            if (state != psPrinting)
                break;

            r = Rect(0, 0, p->PageWidth, p->PageHeight);
            PlayEnhMetaFile(p->Canvas->Handle, img, &r);

            Application->ProcessMessages();
            if (state != psPrinting)
                break;

            DeleteEnhMetaFile(img);
            img = 0;

            Application->ProcessMessages();
            if (state != psPrinting)
                break;

            if (i < pdSet->ToPage - 1)
                p->NewPage();
        }
        if (state != psPrinting)
        {
            p->Abort();
            MsgBox(L"Printing aborted by user.", L"Message", MB_OK);
        }
        else
            p->EndDoc();
    }
    catch (...)
    {
        p->Abort();
        MsgBox(L"An error occurred while sending the page to the printer.", L"Error", MB_OK);
    }
    if (img)
        DeleteEnhMetaFile(img);
    state = psNone;
    pBar->Visible = false;
    pbView->Cursor = crSizeAll;

    EnableAll();

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::btnCloseClick(TObject *Sender)
{
    if (pBar->Visible)
    {
        btnAbort->Click();
        return;
    }
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::btnAbortClick(TObject *Sender)
{
    state = psNone;
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::btnRightMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    MouseRepeat(btnRightMouseDown, Sender, Button, Shift, x, y);
    page++;
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::btnLeftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    MouseRepeat(btnLeftMouseDown, Sender, Button, Shift, x, y);
    page--;
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::FormShow(TObject *Sender)
{
    //PostMessage(Handle,WM_SYSCOMMAND,SC_MAXIMIZE,0);
    PostMessage(Handle, WM_Z_INIT, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::pdSetShow(TObject *Sender)
{
    data->ModalBegin(NULL, ((TCommonDialog*)Sender)->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TfVocabPrint::pdSetClose(TObject *Sender)
{
    data->ModalEnd(((TCommonDialog*)Sender)->Handle);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(173);
#endif

