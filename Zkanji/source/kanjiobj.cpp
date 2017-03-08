//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanjiobj.h"
#include "kanji.h"
#include "settings.h"
#include "collection.h"
#include "kanjielement.h"
#include "kanjihint.h"
#include "modallist.h"
#include "myfonts.h"
#include "gdipbmp.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

#define INIT_PLAY_BIT   -8

#ifdef _DEBUG
static TGlobalTest tester1(92);
#endif

//colors for strokes and numbers
int scolors[32] = { 0xa41900, 0xa43000, 0xa45200, 0xa46f00, 0xa48800, 0xa49e00, 0x9ea400, 0x85a400, 0x68a400, 0x4aa400, 0x2ca400, 0x13a400, 0x00a400, 0x00a410, 0x00a42a, 0x00a446, 0x00a466, 0x00a481, 0x00a495, 0x00a0a4, 0x008ea4, 0x0071a4, 0x0056a4, 0x0039a4, 0x0038a4, 0x0018a4, 0x0008a4, 0x1008a4, 0x2008a4, 0x3008a4, 0x4008a4, 0x5008a4 };

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TKanjiDrawObj::TKanjiDrawObj(TZPaintBox *apaintbox, TDrawStrokeStatePtr func)
{
    paused = false;
    Kfont = 0;
    felement = -1;
    fpensize = 10;
    fmargin = 10;
    fbitsize = 6;
    needupdate = true;
    playfunc = func;
    playmaxbit = 0;
    destroying = false;
    fpaintbox = apaintbox;
    fgrid = false;
    forder = false;
    fshadow = false;
    fnumbers = false;
    fdots = false;
    fkanji = 0;
    fspeed = dsNormalSpeed;
    fpaintbox->OnPaint = onPaint;
    bmp = new TBmp;
    bmp->SetSize(fpaintbox->Width, fpaintbox->Height);
    //bmp = new TBitmap2(fpaintbox->Width,fpaintbox->Height);
    playing = false;
    numlist = new TPointList;
    dotlist = new TPointList;
    time = NULL;
}
//---------------------------------------------------------------------------
TKanjiDrawObj::~TKanjiDrawObj()
{
    destroying = true;
    Stop();
    delete bmp;
    delete numlist;
    delete dotlist;

    if (Kfont)
        DeleteObject(Kfont);
    Kfont = 0;
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::beginpropertychange()
{
    paused = playing && time->Enabled;
    if (paused)
        Pause();
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::endpropertychange()
{
    Update();
    Application->ProcessMessages();
    if (paused)
        Play();
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::set_grid(bool val)
{
    if (fgrid == val)
        return;

    beginpropertychange();
    fgrid = val;
    endpropertychange();
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::set_order(bool val)
{
    if (forder == val)
        return;

    beginpropertychange();
    //Stop();
    forder = val;
    //Update();
    endpropertychange();
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::set_shadow(bool val)
{
    if (fshadow == val)
        return;

    beginpropertychange();
    fshadow = val;
    if (felement < 0 && !paused)
        return;
    endpropertychange();
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::set_numbers(bool val)
{
    if (fnumbers == val)
        return;

    beginpropertychange();
    fnumbers = val;
    if (felement < 0 && !paused)
        return;
    endpropertychange();
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::set_dots(bool val)
{
    if (fdots == val)
        return;

    beginpropertychange();
    fdots = val;
    if (felement < 0 && !paused)
        return;
    endpropertychange();
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::set_pensize(double val)
{
    val = MAX(1, val);
    if (fpensize == val)
        return;

    beginpropertychange();

    char oldmaxbit = playmaxbit;

    fpensize = val * 0.9;
    fbitsize = MAX(2.0, fpensize / 1.2) * (fspeed == dsNormalSpeed ? 0.5 : fspeed == dsFast ? 0.75 : 1.0);

    if (paused)
    {
        TRect r = fpaintbox->ClientRect;
        InflateRect(&r, -fmargin, -fmargin);
        playmaxbit = elements->MeasureBit(r, fpensize, felement, 0, playstroke, fbitsize) - 1;
        playbit = MIN(playbit * ((double)playmaxbit / oldmaxbit) + 1, (double)playmaxbit - 1);
    }

    if (felement < 0 && !paused)
        return;

    endpropertychange();
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::set_speed(TDrawSpeed val)
{
    if (fspeed == val)
        return;

    beginpropertychange();

    char oldmaxbit = playmaxbit;

    fspeed = val;
    if (time)
        time->Interval = timer_speed();
    fbitsize = MAX(2.0, (double)fpensize / 1.2) * (fspeed == dsNormalSpeed ? 0.5 : fspeed == dsFast ? 0.75 : 1);

    if (paused)
    {
        TRect r = fpaintbox->ClientRect;
        InflateRect(&r, -fmargin, -fmargin);
        playmaxbit = elements->MeasureBit(r, fpensize, felement, 0, playstroke, fbitsize) - 1;
        playbit = MIN(playbit * ((double)playmaxbit / oldmaxbit) + 1, (double)playmaxbit - 1);
    }

    if (felement < 0 && !paused)
        return;
    endpropertychange();
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::set_margin(char val)
{
    val = MAX(0, val);
    if (fmargin == val)
        return;

    beginpropertychange();
    fmargin = val;
    if (felement < 0 && !paused)
        return;
    endpropertychange();
}
//---------------------------------------------------------------------------
wchar_t TKanjiDrawObj::get_kanji()
{
    if (!fkanji)
        return 0;
    return fkanji->ch;
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::set_kanji(wchar_t val)
{
    if (kanji == val)
        return;

    if (val)
    {
        fkanji = kanjis->Kanjis[kanjis->IndexOf(val)];
        felement = fkanji->element;
    }
    else
    {
        fkanji = 0;
        felement = -1;
    }

    if (felement >= 0)
        playmaxstroke = elements->Items[felement]->variants[0].strokecount;
    else
        playmaxstroke = 0;

    Stop();
    Update();
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::set_element(short val)
{
    if (val >= elements->Count)
        THROW(L"Element index too large!");

    forder = val >= 0;
    felement = val;
    if (felement >= 0)
    {
        if (elements->Items[felement]->owner < NO_OWNER)
            fkanji = kanjis->Kanjis[elements->Items[felement]->owner];
        else
            fkanji = NULL;
        playmaxstroke = elements->Items[felement]->variants[0].strokecount;
    }
    else
    {
        playmaxstroke = 0;
        fkanji = NULL;
    }

    Stop();
    Update();
}
//---------------------------------------------------------------------------
void __fastcall TKanjiDrawObj::onPaint(TObject *Sender)
{
    if (needupdate)
        repaint();
    fpaintbox->Canvas->Draw(0, 0, bmp);
    needupdate = false;
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::Play()
{
    if (playing && time && !time->Enabled)
    {
        time->Enabled = true;
        if (playfunc)
            playfunc(dssStarted);
    }
    if (playing || felement < 0)
        return;

    if (!playing)
    {
        numlist->Clear();
        dotlist->Clear();
    }

    playing = true;
    playstroke = 0;
    playbit = INIT_PLAY_BIT;
    TRect r = fpaintbox->ClientRect;
    InflateRect(&r, -fmargin, -fmargin);
    playmaxbit = elements->MeasureBit(r, fpensize, felement, 0, playstroke, fbitsize);
    forder = true;

    create_timer();
    time->Enabled = true;
    Update();

    if (playfunc)
        playfunc(dssStarted);
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::Stop()
{
    if (time)
    {
        time->Enabled = false;
        delete time;
        time = NULL;
    }

    playstroke = playmaxstroke - 1;
    if (felement >= 0 && !destroying)
    {
        TRect r = fpaintbox->ClientRect;
        InflateRect(&r, -fmargin, -fmargin);

        playmaxbit = elements->MeasureBit(r, fpensize, felement, 0, playstroke, fbitsize);
    }
    playbit = playmaxbit;
    numlist->Clear();
    dotlist->Clear();

    if (!playing)
        return;
    playing = false;

    if (destroying)
        return;

    Update();
    if (playfunc)
        playfunc(dssStopped);
}
//---------------------------------------------------------------------------
void __fastcall TKanjiDrawObj::onPlay(TObject *Sender)
{
    if (felement < 0)
        return;

    Update();
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::Pause()
{
    if (!playing || !time)
        return;
    time->Enabled = false;
    if (playfunc)
        playfunc(dssPaused);
}
//---------------------------------------------------------------------------
int TKanjiDrawObj::get_position()
{
    if (felement < 0)
        return 0;
    if (!playing)
        return fkanji ? fkanji->strokes - 1 : felement >= 0 ? elements->Items[felement]->variants[0].strokecount : 0;
    return (playbit <= 0 ? playstroke - 1 : playstroke);
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::create_timer()
{
    time = new TTimer(fpaintbox->Owner);
    time->Enabled = false;
    time->Interval = timer_speed();

    time->OnTimer = onPlay;
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::Update()
{
    if (Kfont)
        DeleteObject(Kfont);
    Kfont = 0;

    needupdate = true;

    TForm *f = ((TForm*)fpaintbox->Owner);
    if (!f->Parent && f->Visible || f->Parent && ((TForm*)f->Parent)->Visible)
        fpaintbox->Invalidate();
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::repaint()
{
    int fsize = fpaintbox->ClientWidth;

    if (bmp->Width != fpaintbox->ClientWidth || bmp->Height != fpaintbox->ClientHeight)
    {
        bmp->SetSize(fpaintbox->ClientWidth, fpaintbox->ClientHeight);

        numlist->Clear();
        dotlist->Clear();
    }

    TCanvas *c = bmp->Canvas;
    TRect r = fpaintbox->ClientRect;

    c->Brush->Style = bsSolid;
    c->Brush->Color = clWhite;

    if (felement < 0 && !fkanji || !playing || !time->Enabled || !playstroke && playbit == INIT_PLAY_BIT)
        c->FillRect(fpaintbox->ClientRect);

    if (!fkanji && felement < 0)
        return;

    if (fgrid && (!playing || !time->Enabled || !playstroke && playbit == INIT_PLAY_BIT))
    {
        const int b = fsize / 16;
        const int gsize = fsize - b * 2;

        const int n = gsize / 4;

        c->Pen->Color = (TColor)0x00d0e0d0;

        c->Pen->Width = 2;
        c->Pen->Style = psSolid;
        c->MoveTo(b, b);
        c->LineTo(fsize - b, b);
        c->LineTo(fsize - b, fsize - b);
        c->LineTo(b, fsize - b);
        c->LineTo(b, b - 1);

        c->Pen->Width = 1;
        c->MoveTo(b + gsize / 2, b / 2);
        c->LineTo(b + gsize / 2, fsize - b / 2);
        c->MoveTo(b / 2, b + gsize / 2);
        c->LineTo(fsize - b / 2, b + gsize / 2);

        c->Pen->Style = psDot;

        for (int ix = 0; ix < 4; ++ix)
        {
            int dx = (ix / 2) * gsize / 2;
            int dy = (ix % 2) * gsize / 2;
            c->MoveTo(b + n + dx, b*4 / 3 + dy);
            c->LineTo(b + n + dx, fsize / 2 - b / 3 + dy);
            c->MoveTo(b*4 / 3 + dx, b + n + dy);
            c->LineTo(fsize / 2 - b / 3 + dx, b + n + dy);
        }

        c->Pen->Style = psSolid;
    }

    if (forder && fshadow && felement >= 0 && (!playing || !time->Enabled || !playstroke && playbit == INIT_PLAY_BIT))
    {
        //c->Pen->Color = (TColor)0xdadada;
        InflateRect(&r, -fmargin, -fmargin);
        //c->Pen->Width = MAX(1,fpensize-1);
        //c->Pen->Mode = pmMask;

        int offset = max(2, strokewidth / 3);
        OffsetRect(&r, offset, offset);
        elements->Draw(r, c->Handle, MAX(1, fpensize - 1), (TColor)0xdadada, felement);
        OffsetRect(&r, -offset, -offset);
        InflateRect(&r, fmargin, fmargin);
        //c->Pen->Mode = pmCopy;
    }

    if (forder && felement >= 0)
    {
        InflateRect(&r, -fmargin, -fmargin);
        //c->Pen->Color = clBlack;
        //c->Pen->Width = fpensize;
        //c->Pen->Style = psSolid;
        if (!playing)
        {
            elements->Draw(r, c->Handle, fpensize, 0, felement);
            draw_dots();
            draw_numbers();
        }
        else
        {
            if (!time->Enabled)
            {
                TPoint p;
                elements->Draw(r, c->Handle, fpensize, 0, felement, 0, 0, playstroke - 1);
                for (int i = 0; i < playbit; ++i)
                    elements->DrawBit(r, c->Handle, fpensize, 0, felement, 0, playstroke, i, fbitsize);
                draw_dots();
                draw_numbers();
                return;
            }

            if (playbit >= 0)
            {
                TDoublePoint p;
                TDrawBitDirection dir;
                TRect bitr;
                playmaxbit = elements->DrawBit(r, c->Handle, fpensize, 0, felement, 0, playstroke, playbit, fbitsize, (playbit == 0 ? &p : NULL), &dir, &bitr) - 1;

                if (bitr != Rect(-1, -1, -1, -1))
                {
                    TTemplateList<TRect, true> *gl = new TTemplateList<TRect, true>;
                    try
                    {
                        for (int ix = 0; ix < dotlist->Count && fdots; ++ix)
                        {
                            TRect drect = dot_rect(ix);
                            if (RectOverlap(drect, bitr))
                            {
                                draw_dot(ix);
                                gl->Add(new TRect(drect));
                                //*gl->Items[gl->Count - 1] = drect;
                            }
                        }

                        for (int ix = 0; ix < numlist->Count && fnumbers; ++ix)
                        {
                            TRect nrect = number_rect(ix);
                            if (RectOverlap(nrect, bitr))
                                draw_number(ix);
                            else
                            {
                                for (int j = 0; j < gl->Count && fdots; ++j)
                                    if (RectOverlap(nrect, *gl->Items[j]))
                                        draw_number(ix);
                            }
                        }
                    }
                    catch (...)
                    {
                        ;
                    }
                    //for (int ix = 0; ix < gl->Count; ++ix)
                    //    delete gl->Items[ix];
                    delete gl;
                }

                if (playbit == 0)
                {
                    add_number(p, dir);
                    add_dot(p);

                    for (int ix = 0; ix < numlist->Count && fnumbers && fdots; ++ix)
                        if (RectOverlap(number_rect(ix), dot_rect(dotlist->Count - 1)))
                            draw_number(ix);
                }
            }

            if (playbit == playmaxbit)
            {
                if (playstroke == playmaxstroke - 1)
                {
                    Stop();
                }
                else
                {
                    playbit = INIT_PLAY_BIT;
                    playmaxbit = (playstroke < playmaxstroke - 1 ? elements->MeasureBit(r, fpensize, felement, 0, playstroke + 1, fbitsize) - 1 : 0);
                    playstroke++;
                }
            }
            else
            {
                playbit++;
                if (playbit > 0 && playfunc)
                {
                    if (playstroke < playmaxstroke && playbit == 1)
                        playfunc(dssStroke);
                    else
                        playfunc(dssStrokeBit);
                }
            }
        }

        return;
    }

    c->Brush->Style = bsClear;

    c->Font->Name = settings.kanjifont;
    c->Font->Charset = settings.kanjicharset;
    c->Font->Size = double(fsize) * 0.7 / scaleres;
    TSize s;
    GetTextExtentPoint32W(c->Handle, &fkanji->ch, 1, &s);
    bool small = false;
    if (fsize < s.cy)
    {
        c->Font->Size = double(fsize) * 0.6 / scaleres;
        small = true;
    }

    SetTextAlign(c->Handle, TA_CENTER | VTA_BASELINE);

    HFONT oldf = NULL;
    if (settings.kanjiantialias)
    {
        oldf = c->Font->Handle;
        if (!Kfont)
            Kfont = create_localfont(c->Font);
        SelectObject(c->Handle, Kfont);
    }
    ExtTextOutW(c->Handle, fsize / 2, fsize*(small ? 0.8 : 0.85), ETO_CLIPPED, &r, &fkanji->ch, 1, 0);
    if (oldf)
        SelectObject(c->Handle, oldf);
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::stroke_init()
{
    if (felement < 0 || playing)
        return;

    playing = true;
    create_timer();

    TRect r = fpaintbox->ClientRect;
    InflateRect(&r, -fmargin, -fmargin);
    playstroke = elements->Variants[felement][0]->strokecount - 1;
    playmaxbit = elements->MeasureBit(r, fpensize, felement, 0, playstroke, fbitsize) - 1;
    playbit = playmaxbit;
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::StrokeBack()
{
    if (felement < 0 || playbit <= 0 && playstroke <= 0)
        return;

    if (!playing)
        stroke_init();
    else if (time->Enabled)
        Pause();

    TRect r = fpaintbox->ClientRect;
    InflateRect(&r, -fmargin, -fmargin);

    if (numlist->Count)
        numlist->Delete(numlist->Count - 1);
    if (dotlist->Count)
        dotlist->Delete(dotlist->Count - 1);

    if (playbit > 0)
    {
        playbit = 0;
        Update();
        if (playfunc)
            playfunc(dssStroke);
    }
    else
    {
        playstroke--;
        playmaxbit = elements->MeasureBit(r, fpensize, felement, 0, playstroke, fbitsize) - 1;
        playbit = 0;
        Update();
        if (playfunc)
            playfunc(dssStroke);
    }
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::StrokeForward()
{
    if (felement < 0 || playbit == playmaxbit && playstroke == playmaxstroke - 1)
        return;

    if (!playing)
        stroke_init();
    else if (time->Enabled)
        Pause();

    TRect r = fpaintbox->ClientRect;
    InflateRect(&r, -fmargin, -fmargin);

    playstroke++;
    playbit = 0;

    Update();

    if (playstroke == playmaxstroke)
        Stop();
    else
    {
        playmaxbit = elements->MeasureBit(r, fpensize, felement, 0, playstroke, fbitsize) - 1;
        if (playfunc)
            playfunc(dssStroke);
    }
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::StrokeRewind()
{
    if (felement < 0 || playbit <= 0 && playstroke <= 0)
        return;

    if (!playing)
        stroke_init();
    else if (time->Enabled)
        Pause();

    TRect r = fpaintbox->ClientRect;
    InflateRect(&r, -fmargin, -fmargin);

    playbit = 0;
    playstroke = 0;
    playmaxbit = elements->MeasureBit(r, fpensize, felement, 0, playstroke, fbitsize) - 1;
    numlist->Clear();
    dotlist->Clear();
    Update();
    if (playfunc)
        playfunc(dssStroke);
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::StrokeFastForward()
{
    if (!playing || felement < 0 || playbit == playmaxbit && playstroke == playmaxstroke - 1)
        return;

    Stop();
}
//---------------------------------------------------------------------------
int TKanjiDrawObj::timer_speed()
{
    return (fspeed == dsNormalSpeed ? 35 : fspeed == dsFast ? 16 : 10);
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::draw_numbers()
{
    if (felement < 0 || playstroke < 0)
        return;

    for (int i = numlist->Count; i <= (playbit <= 0 ? playstroke - 1 : playstroke); ++i)
    {
        TDoublePoint p;
        //TCanvas *c = bmp->Canvas;
        TRect r = fpaintbox->ClientRect;
        InflateRect(&r, -fmargin, -fmargin);
        TDrawBitDirection dir;
        elements->MeasureBit(r, fpensize, felement, 0, i, fbitsize, &p, &dir);
        add_number(p, dir);
    }

    for (int i = 0; i < numlist->Count; ++i)
        draw_number(i);
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::add_number(TDoublePoint p, TDrawBitDirection dir)
{
    if (!fnumbers || felement < 0 || p.x == -1 && p.y == -1)
        return;

    TDoubleRect pos[7];
    bool nopos[7];
    memset(nopos, 0, sizeof(nopos));

    TCanvas *c = bmp->Canvas;

    UnicodeString str = (numlist->Count + 1);
    TSize s;

    UnicodeString oldfn = c->Font->Name;
    TFontCharset oldcs = c->Font->Charset;
    int oldfs = c->Font->Size;
    TFontStyles oldfst = c->Font->Style;

    c->Font->Name = "Arial";
    c->Font->Charset = DEFAULT_CHARSET;
    c->Font->Style = TFontStyles() << fsBold;
    c->Font->Size = MIN(11, MAX(8, fpensize + 1));
    GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);

    switch (dir)
    {
    case dbdDown:
        pos[0] = DoubleRect(p.x - ((double)s.cx * 1) - (fpensize / 1.6) - 1, p.y - ((double)s.cy * 0.6) + 1, 0, 0);
        pos[1] = DoubleRect(p.x - ((double)s.cx * 1) - (fpensize / 1.6) - 1, p.y - ((double)s.cy * 0.3) + 1, 0, 0);
        pos[2] = DoubleRect(p.x - ((double)s.cx * 1) - (fpensize / 1.6) - 1, p.y + ((double)s.cy * 0.1) + 1, 0, 0);
        pos[3] = DoubleRect(p.x + ((double)s.cx * 0.3), p.y - ((double)s.cy * 0.16) + 1, 0, 0);
        pos[4] = DoubleRect(p.x - ((double)s.cx * 1) - (fpensize / 1.1) - 1, p.y - ((double)s.cy * 1.1) + 1, 0, 0);
        pos[5] = DoubleRect(p.x + ((double)s.cx * 0.3), p.y + ((double)s.cy * 0.3) + 1, 0, 0);
        pos[6] = DoubleRect(p.x - ((double)s.cx * 1) - (fpensize / 1.3) - 1, p.y + ((double)s.cy * 0.3) + 1, 0, 0);
        break;
    case dbdRight:
        pos[0] = DoubleRect(p.x + ((double)s.cx * 0) + (fpensize / 3.5) - 1, p.y - ((double)s.cy * 1.2) + 1, 0, 0);
        pos[1] = DoubleRect(p.x + ((double)s.cx * 0) + 1 - 1, p.y - ((double)s.cy * 1.1) + 1, 0, 0);
        pos[2] = DoubleRect(p.x - ((double)s.cx * 0) - (fpensize / 3.5) - 1, p.y - ((double)s.cy * 1.1) + 1, 0, 0);
        pos[3] = DoubleRect(p.x - ((double)s.cx * 1) - 1, p.y - ((double)s.cy * 1.2) + 1, 0, 0);
        pos[4] = DoubleRect(p.x - ((double)s.cx * 1) - (fpensize / 1.3) - 1, p.y - ((double)s.cy * 0.5) + 1, 0, 0);
        pos[5] = DoubleRect(p.x + ((double)s.cy * 0.1) + (fpensize / 3.5) - 1, p.y - ((double)s.cy * 1) - 1 + 1, 0, 0);
        pos[6] = DoubleRect(p.x - ((double)s.cy * 0) - (fpensize / 3.5) - 1, p.y - ((double)s.cy * 0.9) + 1, 0, 0);
        break;
    case dbdDownLeft:
        pos[0] = DoubleRect(p.x - ((double)s.cx * 1) - (fpensize / 1.3) - 1, p.y - ((double)s.cy * 1) + 1, 0, 0);
        pos[1] = DoubleRect(p.x - ((double)s.cx * 1) - 1, p.y - ((double)s.cy * 1.2) + 1, 0, 0);
        pos[2] = DoubleRect(p.x - ((double)s.cx * 1) - fpensize - 1, p.y - ((double)s.cy * 0.2) + 1, 0, 0);
        pos[3] = DoubleRect(p.x - ((double)s.cx * 1) - (fpensize * 1.5) - 1, p.y + ((double)s.cy * 0.2) + 1, 0, 0);
        pos[4] = DoubleRect(p.x + ((double)s.cx * 0) + (fpensize / 2) - 1, p.y - ((double)s.cy * 1.3) + 1, 0, 0);
        pos[5] = DoubleRect(p.x + ((double)s.cx * 0.5) - 1, p.y - ((double)s.cy * 1.2) + 1, 0, 0);
        pos[6] = DoubleRect(p.x + ((double)s.cx * 0.5) - 1, p.y + ((double)s.cy * 0.5) + 1, 0, 0);
        break;
    case dbdDownRight:
        pos[0] = DoubleRect(p.x + ((double)s.cx * 0.5) - 1, p.y - ((double)s.cy * 1) + 1, 0, 0);
        pos[1] = DoubleRect(p.x - ((double)s.cx * 1) - (fpensize / 1.3) - 1, p.y - ((double)s.cy * 1) + 1, 0, 0);
        pos[2] = DoubleRect(p.x - ((double)s.cx * 1) - 1, p.y - ((double)s.cy * 1.3) + 1, 0, 0);
        pos[3] = DoubleRect(p.x + ((double)s.cx * 0) + 1 - 1, p.y - ((double)s.cy * 1.3) + 1, 0, 0);
        pos[4] = DoubleRect(p.x - ((double)s.cx * 1) - 1, p.y + ((double)s.cy * 0.4) + 1, 0, 0);
        pos[5] = DoubleRect(p.x + ((double)s.cx * 0.5) - 1, p.y - ((double)s.cy * 1.2) + 1, 0, 0);
        pos[6] = DoubleRect(p.x + ((double)s.cx * 0.5) + (fpensize / 1.3) - 1, p.y + ((double)s.cy * 0) + 1, 0, 0);
        break;
    case dbdUpRight:
        pos[0] = DoubleRect(p.x - ((double)s.cx * 0) - (fpensize / 1.1) - 1, p.y - ((double)s.cy * 1.16) + 1, 0, 0);
        pos[1] = DoubleRect(p.x - ((double)s.cx * 1) - (fpensize / 1.3) - 1, p.y - ((double)s.cy * 1) + 1, 0, 0);
        pos[2] = DoubleRect(p.x + ((double)s.cx * 0) + 1 - 1, p.y - ((double)s.cy * 1.3) + 1, 0, 0);
        pos[3] = DoubleRect(p.x - ((double)s.cx * 1) - 1, p.y + ((double)s.cy * 0.4) + 1, 0, 0);
        pos[4] = DoubleRect(p.x + ((double)s.cx * 0.5) - 1, p.y - ((double)s.cy * 1.2) + 1, 0, 0);
        pos[5] = DoubleRect(p.x + ((double)s.cx * 0.5) - 1, p.y + ((double)s.cy * 0.5) + 1, 0, 0);
        pos[6] = DoubleRect(p.x + ((double)s.cx * 0.5) - 1, p.y + ((double)s.cy * 0.5) + 1, 0, 0);
        break;
    default:
        pos[0] = DoubleRect(p.x - ((double)s.cx * 1) - (fpensize / 1.3) - 1, p.y - ((double)s.cy * 1) + 1, 0, 0);
        pos[1] = DoubleRect(p.x - ((double)s.cx * 1) - 1, p.y - ((double)s.cy * 1.3) + 1, 0, 0);
        pos[2] = DoubleRect(p.x + ((double)s.cx * 0) + 1 - 1, p.y - ((double)s.cy * 1.3) + 1, 0, 0);
        pos[3] = DoubleRect(p.x - ((double)s.cx * 1) - 1, p.y + ((double)s.cy * 0.4) + 1, 0, 0);
        pos[4] = DoubleRect(p.x + ((double)s.cx * 0.5) - 1, p.y - ((double)s.cy * 1.2) + 1, 0, 0);
        pos[5] = DoubleRect(p.x + ((double)s.cx * 0.5) - 1, p.y + ((double)s.cy * 0.5) + 1, 0, 0);
        pos[6] = DoubleRect(p.x + ((double)s.cx * 0.5) - 1, p.y + ((double)s.cy * 0.5) + 1, 0, 0);
        break;
    }

    for (int j = 0; j < (int)(sizeof(nopos) / sizeof(bool)); ++j)
    {
        pos[j].Right = pos[j].Left + s.cx + 3;
        pos[j].Bottom = pos[j].Top + s.cy - 2;
    }

    for (int i = 0; i < numlist->Count; ++i)
    {
        TPoint p2 = *numlist->Items[i];
        if (p2.x == -1 && p2.y == -1)
            continue;
        TRect pos2 = number_rect(i);
        for (int j = 0; j < (int)(sizeof(nopos) / sizeof(bool)); ++j)
        {
            if (nopos[j])
                continue;
            if (RectOverlap(pos2, pos[j].rect()))
                nopos[j] = true;
        }
    }
    c->Font->Style = oldfst;
    c->Font->Size = oldfs;
    c->Font->Name = oldfn;
    c->Font->Charset = oldcs;

    TDoublePoint pp = Point(-1, -1);
    for (int j = 0; j < (int)(sizeof(nopos) / sizeof(bool)); ++j)
    {
        if (!nopos[j])
        {
            pp = DoublePoint(pos[j].Left + 1, pos[j].Top - 1);
            break;
        }
    }
    numlist->Add(pp);

    draw_number(numlist->Count - 1);
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::draw_number(int i)
{
    if (!fnumbers)
        return;

    if (i < 0 || i >= numlist->Count)
        THROW(L"Bad index for stroke number while drawing!");
    TDoublePoint p = *numlist->Items[i];
    if (fnear(p.x, -1) && fnear(p.y, -1))
        return;
    TRect r = DoubleRect(p.x, p.y, p.x, p.y).rect();

    TCanvas *c = bmp->Canvas;
    UnicodeString oldfn = c->Font->Name;
    TFontCharset oldcs = c->Font->Charset;
    TFontStyles oldfst = c->Font->Style;
    int oldfs = c->Font->Size;

    c->Font->Name = "Arial";
    c->Font->Size = MIN(11, MAX(8, fpensize + 1));
    c->Font->Style = TFontStyles() << fsBold;
    c->Font->Charset = DEFAULT_CHARSET;

    int ta = GetTextAlign(c->Handle);
    SetTextAlign(c->Handle, TA_LEFT | TA_TOP);

    UnicodeString str = (i + 1);
    TSize s;
    GetTextExtentPoint32(c->Handle, str.c_str(), str.Length(), &s);

    TColor oldfc = c->Font->Color;

    TColor oldbc = c->Brush->Color;
    c->Brush->Color = (TColor)scolors[i];
    c->Pen->Style = psClear;
    c->RoundRect(p.x - 1, p.y + 1, p.x + s.cx + 2, p.y + s.cy, 4, 4);
    c->Pen->Style = psSolid;
    c->Brush->Color = oldbc;

    c->Brush->Style = bsClear;
    c->Font->Color = clWhite;
    ExtTextOut(c->Handle, p.x, p.y, 0, &r, str.c_str(), str.Length(), NULL);
    c->Font->Color = oldfc;
    c->Brush->Style = bsSolid;

    SetTextAlign(c->Handle, ta);
    c->Font->Size = oldfs;
    c->Font->Style = oldfst;
    c->Font->Name = oldfn;
    c->Font->Charset = oldcs;
}
//---------------------------------------------------------------------------
TRect TKanjiDrawObj::number_rect(int i)
{
    if (i < 0 || i >= numlist->Count)
        THROW(L"Bad index for stroke number rectangle!");

    TPoint p = *numlist->Items[i];
    TRect r = Rect(p.x, p.y, p.x, p.y);
    if (p.x == -1 && p.y == -1)
        return r;

    TCanvas *c = bmp->Canvas;
    UnicodeString oldfn = c->Font->Name;
    TFontCharset oldcs = c->Font->Charset;
    TFontStyles oldfst = c->Font->Style;
    int oldfs = c->Font->Size;
    int ta = GetTextAlign(c->Handle);

    try
    {
        c->Font->Name = "Arial";
        c->Font->Size = MIN(11, MAX(8, fpensize + 1));
        c->Font->Style = TFontStyles() << fsBold;
        c->Font->Charset = DEFAULT_CHARSET;

        SetTextAlign(c->Handle, TA_LEFT | TA_TOP);

        UnicodeString str = (i + 1);
        TSize s;
        GetTextExtentPoint32(c->Handle, str.c_str(), str.Length(), &s);
        r = Rect(p.x, p.y + 2, p.x + s.cx + 1, p.y + s.cy - 2);

    }
    catch (...)
    {
        ;
    }

    SetTextAlign(c->Handle, ta);
    c->Font->Size = oldfs;
    c->Font->Style = oldfst;
    c->Font->Name = oldfn;
    c->Font->Charset = oldcs;

    return r;
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::draw_dots()
{
    if (felement < 0 || playstroke < 0)
        return;

    for (int i = dotlist->Count; i <= (playbit <= 0 ? playstroke - 1 : playstroke); ++i)
    {
        TDoublePoint p;
        //TCanvas *c = bmp->Canvas;
        TRect r = fpaintbox->ClientRect;
        InflateRect(&r, -fmargin, -fmargin);
        TDrawBitDirection dir;
        elements->MeasureBit(r, fpensize, felement, 0, i, fbitsize, &p, &dir);
        add_dot(p);
    }

    for (int ix = 0; ix < dotlist->Count; ++ix)
        draw_dot(ix);
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::add_dot(TDoublePoint p)
{
    if (!fdots || felement < 0 || p.x < 0 && p.y < 0)
        return;

    dotlist->Add(p);

    draw_dot(dotlist->Count - 1);
}
//---------------------------------------------------------------------------
TRect TKanjiDrawObj::dot_rect(int i)
{
    if (i < 0 || i >= dotlist->Count)
        THROW(L"Bad index for stroke dot rectangle!");

    TRect r = Rect(round(dotlist->Items[i]->x) - fpensize / 1.8, round(dotlist->Items[i]->y) - fpensize / 1.8, round(dotlist->Items[i]->x) + fpensize / 1.8, round(dotlist->Items[i]->y) + fpensize / 1.8);
    return r;
}
//---------------------------------------------------------------------------
void TKanjiDrawObj::draw_dot(int i)
{
    if (!fdots)
        return;

    gdi->Circle(DoublePoint(dotlist->Items[i]->x, dotlist->Items[i]->y), fpensize / 3, GdiSettings(bmp->Canvas->Handle) << GdiFill(clRed) << goAntialiasHi);
    /*
     TCanvas *c = bmp->Canvas;

     TColor oldc = c->Pen->Color;
     int oldw = c->Pen->Width;
     try {

     c->Pen->Width = fpensize;
     c->Pen->Color = clRed;
     c->MoveTo(round(dotlist->Items[i]->x),round(dotlist->Items[i]->y));
     c->LineTo(round(dotlist->Items[i]->x),round(dotlist->Items[i]->y));

     } catch (...) {
    ;
     }

     c->Pen->Width = oldw;
     c->Pen->Color = oldc;
     */

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
TKanjiPanelObj::TKanjiPanelObj(TZPaintBox *apaintbox, TFuncPtr acallback, TKanjiPanelColorEvent colorproc, TKanjiPanelDrawEvent drawproc) :
        Kfont(0), foncolor(colorproc), fondraw(drawproc), fcoll(NULL), cnt(0), fpos(0), fmpos(-1), items(NULL), pb(apaintbox), fshowhint(true), fselpart(false),
        wdelta(0), fcallback(acallback)
{
    bmp = new TBmp;
    bmp->SetSize(pb->Width, pb->Height);

    onmousemove = pb->OnMouseMove;
    onmouseleave = pb->OnMouseLeave;
    onresize = pb->OnResize;

    pb->OnPaint = pbpaint;
    pb->OnMouseMove = pbmove;
    pb->OnMouseLeave = pbleave;
    pb->OnResize = pbresize;

    oldproc = pb->WindowProc;
    pb->WindowProc = boxwndproc;

    SettingsChangeEvent(this, settingsChanged, true);
}
//---------------------------------------------------------------------------
TKanjiPanelObj::~TKanjiPanelObj()
{
    pb->WindowProc = oldproc;

    pb->OnResize = onresize;
    pb->OnMouseMove = onmousemove;
    pb->OnMouseLeave = onmouseleave;

    if (items)
        delete[] items;
    delete bmp;

    SettingsChangeEvent(this, settingsChanged, false);

    if (Kfont)
        DeleteObject(Kfont);
    Kfont = 0;
}
//---------------------------------------------------------------------------
void __fastcall TKanjiPanelObj::boxwndproc(TMessage &msg)
{
    if (msg.Msg == WM_MOUSEWHEEL || msg.Msg == WM_MOUSEHWHEEL)
    {
        UINT sl;
        SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &sl, 0);
        wdelta += GET_WHEEL_DELTA_WPARAM(msg.WParam) * (msg.Msg == WM_MOUSEWHEEL ? 1 : -1);
        int dif = 0;
        while (wdelta < 0)
            dif += sl, wdelta += 120;
        while (wdelta > 0)
            dif -= sl, wdelta -= 120;

        pos += dif;

        TPoint pt = pb->ScreenToClient(Point(GET_X_LPARAM(msg.LParam), GET_Y_LPARAM(msg.LParam)));
        SendMessage(pb->Handle, WM_MOUSEMOVE, GET_KEYSTATE_WPARAM(msg.WParam), *((short*)&pt.x) | (*((short*)&pt.y) << 16));

        msg.Result = 0;
        return;
    }
    oldproc(msg);
}
//---------------------------------------------------------------------------
void TKanjiPanelObj::settingsChanged(void *caller, TSettingsProcState state)
{
    if (state != spsEnd)
        return;

    if (Kfont)
        DeleteObject(Kfont);
    Kfont = 0;

    TForm *f = (TForm*)pb->Owner;
    if (!f->Parent && f->Visible || f->Parent && ((TForm*)f->Parent)->Visible)
        pb->Invalidate();
}
//---------------------------------------------------------------------------
void TKanjiPanelObj::set_items(TIntegerList *l)
{
    delete[] items;

    if (!l)
    {
        cnt = 0;
        fpos = 0;
        items = NULL;
        pb->Invalidate();
        fmpos = -1;
        if (fcallback)
            fcallback();
        return;
    }

    fmpos = -1;
    TPoint p;
    GetCursorPos(&p);
    p = pb->ScreenToClient(p);

    if (p.y >= 0 && p.y < pb->ClientHeight && p.x >= 0 && p.x < pb->ClientWidth)
    {
        fmpos = p.x / pb->ClientHeight + fpos;
        if (fmpos >= cnt)
            fmpos = -1;
    }

    cnt = l->Count;
    items = new int[l->Count];
    for (int i = 0; i < l->Count; ++i)
    {
        items[i] = l->Items[i];
    }
    fpos = 0;

    pb->Invalidate();
    if (fcallback)
        fcallback();
}
//---------------------------------------------------------------------------
void TKanjiPanelObj::set_pos(int val)
{
    val = MAX(0, MIN(cnt - pb->Width / pb->Height, val));
    if (val == fpos)
        return;
    fpos = val;
    pb->Invalidate();
    if (fcallback)
        fcallback();
}
//---------------------------------------------------------------------------
TColor saturate(TColor c, byte light, byte dark)
{
    if ((c & 0xff) > ((c & 0xff00) >> 8) && (c & 0xff) > ((c & 0xff0000) >> 16))
        c = COLORINC(c, (TColor)light);
    if (((c & 0xff00) >> 8) > (c & 0xff) && ((c & 0xff00) >> 8) > ((c & 0xff0000) >> 16))
        c = COLORINC(c, (TColor)(light << 8));
    if (((c & 0xff0000) >> 16) > ((c & 0xff00) >> 8) && ((c & 0xff0000) >> 16) > (c & 0xff))
        c = COLORINC(c, (TColor)(light << 16));
    if ((c & 0xff) < ((c & 0xff00) >> 8) && (c & 0xff) < ((c & 0xff0000) >> 16))
        c = COLORDEC(c, (TColor)dark);
    if (((c & 0xff00) >> 8) < (c & 0xff) && ((c & 0xff00) >> 8) < ((c & 0xff0000) >> 16))
        c = COLORDEC(c, (TColor)(dark << 8));
    if (((c & 0xff0000) >> 16) < ((c & 0xff00) >> 8) && ((c & 0xff0000) >> 16) < (c & 0xff))
        c = COLORDEC(c, (TColor)(dark << 16));
    return c;
}
void __fastcall TKanjiPanelObj::pbpaint(TObject *Sender)
{
    if (bmp->Width != pb->Width || bmp->Height != pb->Height)
        bmp->SetSize(pb->Width, pb->Height);

    TCanvas *c = bmp->Canvas;

    SetTextAlign(c->Handle, TA_BASELINE | TA_CENTER);
    c->Font->Name = settings.kanjifont;
    c->Font->Charset = settings.kanjicharset;
    c->Font->Size = double(pb->ClientHeight) * 0.7 / scaleres;

    TRect r = Rect(0, 0, pb->ClientHeight, pb->ClientHeight);
    for (int i = fpos; i < cnt && r.Left < pb->ClientWidth; ++i)
    {
        c->Brush->Color = fbg;

        c->Font->Color = clBlack;
        c->Pen->Color = clBlack;

        c->Brush->Style = bsSolid;
        TKanjiElement *e = NULL;
        if (items[i] < 0)
            e = elements->Items[-items[i]];
        else if (kanjis->Kanjis[items[i]]->element >= 0)
            e = elements->Items[kanjis->Kanjis[items[i]]->element];

        bool sel = fmpos == i && (items[i] >= 0 || e && e->owner > NO_OWNER || settings.shownonkanjielements && hasstrokes);
        if (sel)
            c->Brush->Color = COLORDEC(saturate(fbg, 0x08, 0x08), 0x040404);

        if (foncolor)
        {
            TColor bc = c->Brush->Color;
            TColor fc = c->Font->Color;
            foncolor(c, r, i, e, sel, bc, fc);
            c->Brush->Color = bc;
            c->Pen->Color = fc;
            c->Font->Color = fc;
        }

        GradientFillCanvas(c, COLORDEC(c->Brush->Color, 0x080808), COLORINC(c->Brush->Color, 0x080808), r, gdVertical);

        if (fondraw)
            fondraw(c, r, i, e, fmpos == i);

        c->Brush->Style = bsClear;
        if (items[i] >= 0 || e && e->owner > NO_OWNER)
        {
            HFONT oldf = NULL;
            if (settings.kanjiantialias)
            {
                oldf = c->Font->Handle;
                if (!Kfont)
                    Kfont = create_localfont(c->Font);
                SelectObject(c->Handle, Kfont);
            }
            wchar_t ch = items[i] >= 0 ? kanjis->Kanjis[items[i]]->ch : e->owner;
            ExtTextOutW(c->Handle, r.Left + r.Width()*0.5, r.Height()*0.82, ETO_CLIPPED, &r, &ch, 1, NULL);
            if (oldf)
                SelectObject(c->Handle, oldf);
        }
        else if (items[i] < 0)
        {
            InflateRect(&r, -2, -2);
            elements->Draw(r, c->Handle, 1.5, c->Pen->Color, -items[i], 0);
            InflateRect(&r, 2, 2);
        }
        else
            THROW(L"?");

        c->Pen->Width = 1;
        c->Pen->Color = COLORDEC(fbg, 0x202020);
        c->MoveTo(r.Right - 1, r.Top);
        c->LineTo(r.Right - 1, r.Bottom);
        if (sel)
        {
            c->Pen->Color = COLORDEC(saturate(COLORDEC(fbg, 0x202020), 0xc0, 0x60), 0x202020);
            c->MoveTo(r.Left, r.Top);
            c->LineTo(r.Right - 2, r.Top);
            c->LineTo(r.Right - 2, r.Bottom - 1);
            c->LineTo(r.Left, r.Bottom - 1);
            c->LineTo(r.Left, r.Top);
        }

        r.Left += pb->ClientHeight;
        r.Right += pb->ClientHeight;
    }

    r.Right = pb->ClientWidth;
    c->Brush->Color = COLORDEC(fbg, 0x101010);
    GradientFillCanvas(c, COLORDEC(fbg, 0x080808), COLORDEC(fbg, 0x181818), r, gdVertical);

    pb->Canvas->Draw(0, 0, bmp);
}
//---------------------------------------------------------------------------
void TKanjiPanelObj::set_bg(TColor val)
{
    if (fbg == val)
        return;
    fbg = val;
    pb->Invalidate();
}
//---------------------------------------------------------------------------
void TKanjiPanelObj::set_selpart(bool val)
{
    if (fselpart == val)
        return;
    fselpart = val;
    pb->Invalidate();
}
//---------------------------------------------------------------------------
int TKanjiPanelObj::ElementAtPos(int x)
{
    x /= pb->ClientHeight;
    x += fpos;
    if (x >= cnt)
        return -1;
    if (items[x] < 0)
        return -items[x];
    return kanjis->Kanjis[items[x]]->element;
}
//---------------------------------------------------------------------------
int TKanjiPanelObj::KanjiAtPos(int x)
{
    x /= pb->ClientHeight;
    x += fpos;
    if (x >= cnt)
        return -1;
    if (items[x] >= 0)
        return items[x];
    return -1;
}
//---------------------------------------------------------------------------
wchar_t TKanjiPanelObj::CharAtPos(int x)
{
    x /= pb->ClientHeight;
    x += fpos;
    if (x >= cnt)
        return 0;
    return items[x] >= 0 ? kanjis->Kanjis[items[x]]->ch : elements->Items[-items[x]]->owner > NO_OWNER ? elements->Items[-items[x]]->owner : 0;
}
//---------------------------------------------------------------------------
void __fastcall TKanjiPanelObj::pbmove(TObject *Sender, TShiftState Shift, int x, int y)
{
    if (onmousemove)
        onmousemove(Sender, Shift, x, y);

    if (fmpos != x / pb->ClientHeight + fpos)
    {
        fmpos = x / pb->ClientHeight + fpos;
        if (fmpos >= cnt)
        {
            fmpos = -1;
            HideKanjiHint();
        }
        else if (fshowhint)
        {
            int k = KanjiAtPos(x);
            if (k >= 0 && collection && settings.show_kanji_hint)
                ShowKanjiHint(pb, x, y, kanjis->Kanjis[k]->ch, collection);
            else
                HideKanjiHint();
        }
        pb->Invalidate();
    }

    UnicodeString str = "";

    if (!KanjiHintVisible())
    {
        int eix = ElementAtPos(x);
        if (eix >= 0)
            str = elements->VariantName(eix, 0, true);
    }

    if (str != pb->Hint && fshowhint)
    {
        TWMMouse msg;
        msg.Pos = SmallPoint(-1, -1);
        Application->HintMouseMessage(pb, *(TMessage*)&msg);
        TPoint p = pb->ClientToScreen(Point(x, y));
        pb->Hint = str;
    }

}
//---------------------------------------------------------------------------
void __fastcall TKanjiPanelObj::pbleave(TObject *Sender)
{
    if (onmouseleave)
        onmouseleave(Sender);

    HideKanjiHint();

    pb->Hint = "";

    if (fmpos < 0)
        return;
    fmpos = -1;
    pb->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TKanjiPanelObj::pbresize(TObject *Sender)
{
    if (onresize)
        onresize(Sender);
    if (fcallback)
        fcallback();
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(93);
#endif

