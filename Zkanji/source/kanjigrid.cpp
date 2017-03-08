//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanjigrid.h"
#include "kanji.h"
#include "kanjihint.h"
#include "kanjicard.h"
#include "collection.h"
#include "settings.h"
#include "myfonts.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester1(82);
#endif

HDC TCustomKanjiGrid::Xdc = 0;
void* TCustomKanjiGrid::Xbits = 0;
HBITMAP TCustomKanjiGrid::Xbitmap = 0;
int TCustomKanjiGrid::Xcnt = 0;
//---------------------------------------------------------------------------
TCustomKanjiGrid::TCustomKanjiGrid(TZPaintBox *apaintbox, int aboxsize, bool amultiselect, TFuncPtr afunc) :
        Kfont(NULL), fcopy(false), fshowhint(true), inherited(apaintbox, aboxsize, amultiselect, afunc)
{
    fpaintinfoproc = NULL;
    fcolorproc = NULL;

    Xcnt++;
    if (Xdc == 0)
    {
        Xdc = CreateCompatibleDC(apaintbox->Canvas->Handle);
        tagBITMAPINFO binfo;
        memset(&binfo, 0, sizeof(binfo));
        binfo.bmiHeader.biSize = sizeof(tagBITMAPINFOHEADER);
        binfo.bmiHeader.biWidth = 12;
        binfo.bmiHeader.biHeight = -12;
        binfo.bmiHeader.biPlanes = 1;
        binfo.bmiHeader.biBitCount = 32;
        binfo.bmiHeader.biCompression = BI_RGB;
        binfo.bmiHeader.biSizeImage = 12 * 12 * 4;
        Xbitmap = CreateDIBSection(Xdc, &binfo, DIB_RGB_COLORS, &Xbits, NULL, 0);
        SelectObject(Xdc, Xbitmap);
        memset(Xbits, 0, 12*12*4);

        for (int i = 0; i < 12 * 12; i++)
        {
            int x = i % 12 - 2;
            int y = i / 12 - 2;
            if (x < 0 || x >= 9 || y < 0 || y >= 9 ||

                ((x + (y < 5 ? 2 : 3) <= y || x >= y + (y < 4 ? 2 : 1)) && (x + y <= (y < 4 ? 6 : 7) || x + y >= (y < 5 ? 10 : 11)))

                    )
            {
                ((int*)Xbits)[i] = 0x00000000;
                continue;
            }

            unsigned int alpha = (MAX(MIN(MAX(0, (9 - x + 10 - y)) * 0xff / 11, 0xa0), 0x20) << 24);
            ((int*)Xbits)[i] = 0xa01010 | alpha;
        }
    }

    if (!kanjigrids)
        kanjigrids = new TTemplateList<TCustomKanjiGrid, false>;
    kanjigrids->Add(this);

    fcollection = NULL;

    hintpos = -1;

    changing = 0;
    SettingsChangeEvent(pb, settingsChanged, true);
}
//---------------------------------------------------------------------------
TCustomKanjiGrid::~TCustomKanjiGrid()
{
    Xcnt--;
    if (!Xcnt)
    {
        if (Xdc)
        {
            DeleteObject(Xbitmap);
            Xbitmap = 0;
            DeleteObject(Xdc);
            Xdc = 0;
        }
        if (Kfont)
            DeleteObject(Kfont);
        Kfont = 0;
    }

    if (kanjigrids)
        kanjigrids->Remove(this);

    SettingsChangeEvent(pb, settingsChanged, false);
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::settingsChanged(void *caller, TSettingsProcState state)
{
    if (state != spsEnd)
        return;

    if (Kfont)
        DeleteObject(Kfont);
    Kfont = 0;

    update_selected();
    Redraw();
}
//---------------------------------------------------------------------------
TKanji* TCustomKanjiGrid::get_kanji()
{
    if (selcount != 1)
        return NULL;
    return kanjis[pos];
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::set_kanji(TKanji *k)
{
    int n = list->IndexOf(k->index);
    if (n >= 0)
        pos = n;
}
//---------------------------------------------------------------------------
TKanji* TCustomKanjiGrid::get_kanjiitem(word ix)
{
    if (ix >= list->Count)
        throw ERangeError("Kanji index out of range!");
    return list->Kanjis[ix];
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::DoMouseLeave()
{
    HideKanjiHint();
    inherited::DoMouseLeave();
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::DoKeyDown(Word &Key, TShiftState Shift)
{
//    if (fcopy && Key == 'C' && Shift.Contains(ssCtrl))
//        ClipReplace(selstring);
    inherited::DoKeyDown(Key, Shift);
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::DoKeyPress(wchar_t &Key)
{
    if (fcopy && Key == 0x03) // Ctrl-C
        ClipReplace(selstring);
    inherited::DoKeyPress(Key);
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::DoChangePos(int &anewpos, bool &resetsel)
{
    HideKanjiHint(false);
    //inherited::DoChangePos(anewpos,resetsel);
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::DoDrawCell(TCanvas *c, const TRect &r, int acell)
{
    TColor pen = c->Pen->Color, font = c->Font->Color, brush = c->Brush->Color;
    DoSetColors(acell, pen, font, brush);

    c->Font->Name = settings.kanjifont;
    c->Font->Charset = settings.kanjicharset;
    c->Font->Size = double(BoxSize) * 0.7 / scaleres;
    if (acell == mousecell)
        brush = coloravg((TColor)0xe6fbfb, brush, 0.5);
    c->Pen->Color = pen;
    c->Font->Color = font;
    c->Brush->Color = brush;

    SetTextAlign(c->Handle, TA_BASELINE | TA_CENTER);

    c->MoveTo(r.left, r.bottom);
    c->LineTo(r.right, r.bottom); //horz
    c->LineTo(r.right, -1); //vert

    HFONT oldf = NULL;
    if (settings.kanjiantialias)
    {
        oldf = c->Font->Handle;
        if (!Kfont)
            Kfont = create_localfont(c->Font);
        SelectObject(c->Handle, Kfont);
    }
    ExtTextOutW(c->Handle, r.left + (r.Width() + 1) / 2, r.Height()*0.84, ETO_OPAQUE | ETO_CLIPPED, &r, &list->Kanjis[acell]->ch, 1, 0);
    if (oldf)
        SelectObject(c->Handle, oldf);

    DoPaintInfo(c, acell, r);
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::DoSetColors(int ix, TColor &pen, TColor &font, TColor &brush)
{
    pen = (TColor)colorsettings.gridline;
    if (selected[ix])
    {
        brush = focused ? (TColor)colorsettings.highlight : coloravg((TColor)colorsettings.highlight, (TColor)colorsettings.background, 0.65);
        font = (TColor)colorsettings.maintext;
    }
    else
    {
        brush = (TColor)colorsettings.background;
        font = (TColor)colorsettings.maintext;
    }

    if (fcollection && !fcollection->kanjidat[list->Kanjis[ix]->index].wordscnt)
        font = clSilver;
    else if (!list->Kanjis[ix]->frequency)
        font = coloravg(font, (TColor)colorsettings.background, 0.7);

    if (fcolorproc)
        fcolorproc(ix, pen, font, brush);

    fixtextcolor(font, brush);
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::DoPaintInfo(TCanvas *c, int ix, TRect r)
{
    if (fcollection && fcollection != dictionary->Items[0] && !fcollection->kanjidat[list->Kanjis[ix]->index].meaning)
    {

        _BLENDFUNCTION bfunc;
        bfunc.BlendOp = AC_SRC_OVER;
        bfunc.BlendFlags = 0;
        bfunc.SourceConstantAlpha = 255;
        bfunc.AlphaFormat = AC_SRC_ALPHA;
        if (!AlphaBlend(c->Handle, r.Right - 12, r.Bottom - 12, 12, 12, Xdc, 0, 0, 12, 12, bfunc))
        {
            static int err = GetLastError();
        }
    }

    if (fpaintinfoproc)
        fpaintinfoproc(ix, c, r);
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::DoMouseMove(TShiftState Shift, int x, int y)
{
    if (!Shift.Contains(ssLeft) && !Shift.Contains(ssRight) && settings.show_kanji_hint)
    {
        TKanji *k = KanjiAtPos(Point(x, y));
        if (hintpos != ScrollPos + y / BoxSize + x / BoxSize)
        {
            hintpos = ScrollPos + y / BoxSize + x / BoxSize;
            HideKanjiHint();
        }
        if (k && fshowhint)
            ShowKanjiHint(pb, x, y, k->ch, collection);
        else
            HideKanjiHint();
    }

    inherited::DoMouseMove(Shift, x, y);
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y)
{
    HideKanjiHint(false);

    inherited::DoMouseDown(Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::set_items(TKanjiList *alist, TWordCollection *acollection, TKanji *aselected)
{
    set_items(alist, acollection, -1, -1, aselected);
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::set_items(TKanjiList *alist, TWordCollection *acollection, int asel, int asellength, TKanji *aselected)
{
    fcollection = acollection;
    HideKanjiHint(false);

    DoSetItems(alist);

    int oldpos = pos;

    int i = -1;
    if (asel >= count)
        asel = count - 1;

    if (asel >= 0)
        i = asel;
    else if (aselected)
    {
        i = list->IndexOf(aselected->index);
        asellength = 1;
    }

    if (i >= 0)
        changed(i, asellength);
    else
        changed(0, asellength);

    int scrollval = pos + (pos + (asellength < 0 ? asellength + 1 : 0) > oldpos ? asellength - 1 : 0);

    if (ScrollPos > (scrollval / Columns) || ScrollPos + Rows <= (scrollval / Columns))
        ScrollPos = (scrollval / Columns) - (Rows / 2);

}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::set_collection(TWordCollection *acollection)
{
    if (fcollection == acollection)
        return;

    fcollection = acollection;
    Redraw();

    RequestCallback();
}
//---------------------------------------------------------------------------
UnicodeString TCustomKanjiGrid::get_kanjistring()
{
    UnicodeString result;
    result.SetLength(list->Count);
    for (int i = 1; i <= list->Count; i++)
        result[i] = list->Kanjis[i - 1]->ch;

    return result;
}
//---------------------------------------------------------------------------
UnicodeString TCustomKanjiGrid::get_selstring()
{
    if (!pos < 0)
        return L"";

    UnicodeString result;
    result.SetLength(selcount);
    for (int i = 0; i < selcount; i++)
        result[i + 1] = list->Kanjis[selpos[i]]->ch;
    return result;
}
//---------------------------------------------------------------------------
TKanji* TCustomKanjiGrid::KanjiAtPos(TPoint p)
{
    int ix = CellAtPos(p);
    if (ix < 0 || ix >= count)
        return NULL;
    return kanjis[ix];
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::DoScroll(int apos)
{
    HideKanjiHint(false);

    inherited::DoScroll(apos);

    if (settings.show_kanji_hint)
    {
        TPoint p;
        GetCursorPos(&p);
        p = pb->ScreenToClient(p);
        if (p.x >= 0 && p.y >= 0 && p.x < pb->ClientWidth && p.y < pb->ClientHeight)
        {
            TKanji *k = KanjiAtPos(Point(p.x, p.y));
            if (hintpos != ScrollPos + p.y / BoxSize + p.x / BoxSize)
                hintpos = ScrollPos + p.y / BoxSize + p.x / BoxSize;
            if (k && fshowhint)
                ShowKanjiHint(pb, p.x, p.y, k->ch, collection);
        }
    }
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::set_paintinfo(TPaintInfoProc val)
{
    if (fpaintinfoproc == val)
        return;

    fpaintinfoproc = val;
    Redraw();
}
//---------------------------------------------------------------------------
void TCustomKanjiGrid::set_colorproc(TGridColorProc val)
{
    if (fcolorproc == val)
        return;

    fcolorproc = val;
    Redraw();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TKanjiGrid::TKanjiGrid(TZPaintBox *apaintbox, int aboxsize, bool amultiselect, TFuncPtr afunc) : fsortby(stUnsorted), TCustomKanjiGrid(apaintbox, aboxsize, amultiselect, afunc)
{
    flist = new TKanjiItemList;
}
//---------------------------------------------------------------------------
TKanjiGrid::~TKanjiGrid()
{
    delete flist;
}
//---------------------------------------------------------------------------
void TKanjiGrid::set_sortby(TKanjiSortType i)
{
    if (fsortby == i)
        return;

    fsortby = i;
    if (!count)
        return;

    int c = list->Kanjis[pos]->index;
    flist->SortBy(fsortby);

    pos = list->IndexOf(c);
}
//---------------------------------------------------------------------------
void TKanjiGrid::DoSetItems(TKanjiList *alist)
{
    flist->Assign(alist);
    flist->SortBy(fsortby);
}
//---------------------------------------------------------------------------
int TKanjiGrid::get_count()
{
    return flist->Count;
}
//---------------------------------------------------------------------------
void TKanjiGrid::DoSetColors(int ix, TColor &pen, TColor &font, TColor &brush)
{
    TCustomKanjiGrid::DoSetColors(ix, pen, font, brush);
    if (!selected[ix] && (sort_by == stJouyou && (list->Kanjis[ix]->jouyou > 10 || list->Kanjis[ix]->jouyou == 0) || sort_by == stNJLPT && !list->Kanjis[ix]->NJLPT || sort_by == stJLPTold && !list->Kanjis[ix]->JLPT || sort_by == stFrequency && !list->Kanjis[ix]->frequency || sort_by == stWordFrequency && !list->Kanjis[ix]->word_freq))
        brush = (TColor)colorsettings.kanjiunsorted;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TKanjiCardGrid::TKanjiCardGrid(TZPaintBox *apaintbox, int aboxsize, bool amultiselect, TFuncPtr afunc) : TCustomKanjiGrid(apaintbox, aboxsize, amultiselect, afunc)
{
    flist = 0;
}
//---------------------------------------------------------------------------
TKanjiCardGrid::~TKanjiCardGrid()
{
}
//---------------------------------------------------------------------------
void TKanjiCardGrid::DoSetItems(TKanjiList *alist)
{
    flist = (TKanjiCardList*)alist;
}
//---------------------------------------------------------------------------
int TKanjiCardGrid::get_count()
{
    return (flist ? flist->Count : 0);
}
//---------------------------------------------------------------------------
bool TKanjiCardGrid::get_isexample(int kix, int wix)
{
    if (!flist)
        THROW(L"Invalid access in examples!");

    return flist->IsExample(kix, wix);
}
//---------------------------------------------------------------------------
TKanjiCardItem* TKanjiCardGrid::get_card(int ix)
{
    return flist->Items[ix];
}
//---------------------------------------------------------------------------
TKanjiCardItem* TKanjiCardGrid::get_item()
{
    if (pos >= 0 && pos < count)
        return cards[pos];
    return NULL;
}
//---------------------------------------------------------------------------
/*void TKanjiCardGrid::AddExample(int kix, int wix)
 {
 if (!flist)
 THROW(L"Invalid access in examples!";

 flist->AddExample(kix,wix);
 }
 //---------------------------------------------------------------------------
 void TKanjiCardGrid::RemoveExample(int kix, int wix)
 {
 if (!flist)
 THROW(L"Invalid access in examples!";

 flist->RemoveExample(kix,wix);
 }*/
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(83);
#endif

