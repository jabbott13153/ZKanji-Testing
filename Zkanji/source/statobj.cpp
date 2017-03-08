//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Math.h>

#include "statobj.h"
#include "main.h"
#include "drawscroll.h"
#include "fastbuffer.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester1(152);
#endif

TStatColumns::TStatColumns()
{
}
//---------------------------------------------------------------------------
void TStatColumns::FreeItem(TStatColumn *item, int ix)
{
    if (item->valcnt)
        delete[] item->values;
    delete item;
}
//---------------------------------------------------------------------------
void TStatColumns::AddEmptyKey(int keyindex)
{
    TStatColumn *item = new TStatColumn;
    item->valcnt = 0;
    item->key = keyindex;

    inherited::Add(item);
}
//---------------------------------------------------------------------------
int* TStatColumns::Add(int valuecount, int keyindex, bool stacked)
{
    if (!valuecount)
        THROW(L"Use empty key for empty keys!");

    TStatColumn *item = new TStatColumn;
    item->valcnt = valuecount;
    item->key = keyindex;
    item->values = new int[valuecount];
    item->stacked = stacked;
    memset(item->values, 0, sizeof(int)*valuecount);
    inherited::Add(item);
    return item->values;
}
//---------------------------------------------------------------------------
void TStatColumns::Assign(TStatColumns *acols)
{
    Clear();
    Capacity = acols->Count;
    for (int i = 0; i < acols->Count; i++)
    {
        TStatColumn *c = acols->Items[i];
        TStatColumn *item = new TStatColumn;
        item->key = c->key;
        item->stacked = c->stacked;
        item->valcnt = c->valcnt;
        if (c->valcnt)
        {
            item->values = new int[item->valcnt];
            memcpy(item->values, c->values, sizeof(int)*c->valcnt);
        }
        inherited::Add(item);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TStatObj::TStatObj(TZPaintBox *apb) : pb(apb), fpopup(true), drawvnum(true), drawhnum(true), fbasekey(1), fkeyinc(1),
        fmoving(false), fredline(-1), fnumproc(NULL), fcursor(crSizeWE), buffer(NULL),
        fkeyproc(NULL), fhintproc(NULL), xstr(""), ystr(""), fnumdiv(10)
{
    cols = new TStatColumns;

    pb->TabStop = false;
    pb->VertScrollBar->Visible = false;
    pb->HorzScrollBar->Visible = false;

    vnumbmp = new TBmp;
    hnumbmp = new TBmp;

    fnumw = 40;
    fnumh = 38;
    fsize = 66; //width of a column
    fpad = 4; //padding pixels for each column
    fhsize = 30; //min height of a row
    fmax = 10;
    fpos = 0;
    buffpos = 0;
    fgraph = sgColumns;

    buff = NULL;
    buffcnt = 0;

    fonpaint = pb->OnPaint;
    fonresize = pb->OnResize;
    fonmouseleave = pb->OnMouseLeave;
    fonmousemove = pb->OnMouseMove;
    fonmousedown = pb->OnMouseDown;
    fonmouseup = pb->OnMouseUp;
    pb->OnPaint = pbPaint;
    pb->OnResize = pbResize;
    pb->OnMouseMove = pbMouseMove;
    pb->OnMouseLeave = pbMouseLeave;
    pb->OnMouseDown = pbMouseDown;
    pb->OnMouseUp = pbMouseUp;
    ((TBaseForm*)pb->Owner)->AddEvent(this, formActivate, eltActivate);
    ((TBaseForm*)pb->Owner)->AddEvent(this, formDeactivate, eltDeactivate);

    AppActivateEvent(pb, appActivate, true, true);
    AppActivateEvent(pb, appDeactivate, false, true);

    scroll = new TDrawScroll(pb, 6, scrollproc);

    measure();
}
//---------------------------------------------------------------------------
TStatObj::~TStatObj()
{
    delete scroll;

    AppActivateEvent(pb, appActivate, true, false);
    AppActivateEvent(pb, appDeactivate, false, false);

    ((TBaseForm*)pb->Owner)->RemoveEvents(this);

    pb->OnMouseUp = fonmouseup;
    pb->OnMouseDown = fonmousedown;
    pb->OnMouseLeave = fonmouseleave;
    pb->OnMouseMove = fonmousemove;
    pb->OnResize = fonresize;
    pb->OnPaint = fonpaint;
    clear_buffer();

    delete vnumbmp;
    delete hnumbmp;

    delete cols;
}
//---------------------------------------------------------------------------
void TStatObj::count_numh()
{
    if (hnumbmp->Width != pb->ClientWidth || hnumbmp->Height != fnumh)
    {
        hnumbmp->SetSize(pb->ClientWidth, fnumh);
        TRect r = Rect(0, scroll->ClientHeight - fnumh, scroll->ClientWidth, scroll->ClientHeight);
        InvalidateRect(pb->Handle, &r, false);
        drawhnum = true;
    }
}
//---------------------------------------------------------------------------
void TStatObj::count_numw()
{
    UnicodeString str = "";
    int y = drawarea.Height() - fdivh;
    int val = fdivval;
    TCanvas *c = pb->Canvas;
    TSize s;
    s.cx = 0;

    if (fnumproc && y >= 0)
    {
        while (y >= 0)
        {
            TSize ts;
            UnicodeString tstr = fnumproc(val);
            GetTextExtentPoint32W(c->Handle, tstr.c_str(), tstr.Length(), &ts);
            if (s.cx < ts.cx)
                s = ts;

            y -= fdivh;
            val += fdivval;
        }
    }
    else
    {
        str = fmax;
        GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);
    }

    str += " ";

    set_numfont(c);

    fnumw = s.cx + 15;
    GetTextExtentPoint32W(c->Handle, ystr.c_str(), ystr.Length(), &s);
    fnumw += s.cy + 4;

    if (vnumbmp->Width != fnumw || vnumbmp->Height != scroll->ClientHeight - fnumh)
    {
        vnumbmp->SetSize(fnumw, scroll->ClientHeight - fnumh);
        drawvnum = true;
    }
}
//---------------------------------------------------------------------------
TRect TStatObj::get_drawarea()
{
    return Rect(fnumw, 0, scroll->ClientWidth, scroll->ClientHeight - fnumh);
}
//---------------------------------------------------------------------------
void TStatObj::set_max(int val)
{
    if (val == fmax)
        return;
    fmax = val;
    clear_buffer();
    measure();
    if (pb->Visible && ((TForm*)pb->Owner)->Visible)
        pb->Invalidate();
}
//---------------------------------------------------------------------------
void TStatObj::clear_buffer()
{
    for (int i = 0; i < buffcnt; i++)
        if (buff[i])
            delete buff[i];
    if (buffcnt)
        delete[] buff;
    buff = NULL;
    buffcnt = 0;
}
//---------------------------------------------------------------------------
bool TStatObj::division_zero(int val)
{
    int local = 1;
    int zero = 0;
    int bval = val;
    while ((bval /= fnumdiv) > 0)
        local++;
    while (!(val % fnumdiv) && val)
    {
        val /= fnumdiv;
        zero++;
    }
    return local / 2 <= zero;
}
//---------------------------------------------------------------------------
int l_primes[20] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71 };
int TStatObj::division_size(int val, int &i, double h)
{
    while (i >= 0 && (val % l_primes[i] || !division_zero(val / l_primes[i])))
        i--;
    if (i < 0)
        return 1;

    h /= l_primes[i];
    val /= l_primes[i];
    int divs = 1;
    if (h < fhsize)
        return 0;

    for (int j = i; j >= 0; j--)
        divs = MAX(divs, division_size(val, j, h));

    return divs*l_primes[i];
}
void TStatObj::measure()
{
    count_numh();

    int rheight = drawarea.Height();

    colh = rheight - fhsize;
    int fmaxd = fmax;
    int p = 1;
    while (!division_zero(fmaxd) || fmax / fmaxd < 1.005 && fmaxd / p >= fnumdiv)
    {
        p *= fnumdiv;
        if (division_zero(fmaxd) && fmax / ((int)fmaxd / p) * p > 1.3)
        {
            p /= fnumdiv;
            break;
        }
        fmaxd = ((int)(fmaxd / p)) * p;
    }

    double colhd = ((double)colh / fmax) * fmaxd;

    int primecnt = sizeof(l_primes) / sizeof(int);

    int divs = 0;
    int norm = fmaxd / p;
    int fcmaxd = fmaxd;

    do
    {
        int ixp = -1;
        double bcolhd = ((double)colh / fmax) * (norm * p);
        for (int i = 0; i < primecnt; i++)
            if (bcolhd / l_primes[i] >= fhsize && l_primes[i] <= norm*p)
                ixp = i;
            else
                break;
        if (ixp < 0)
            break;

        int bdivs = MAX(divs, division_size(norm * p, ixp, bcolhd));

        bool accept = false;

        if (bdivs && bdivs <= norm * p)
        {
            int fdivvald = (norm * p) / bdivs;
            int i = 1;
            accept = division_zero(fdivvald);

            while (accept && fdivvald*i <= fmaxd)
                i++;
            accept = accept && division_zero(fdivvald * (i - 1));
        }

        double dh = (bdivs ? (double)bcolhd / bdivs : bcolhd);
        int rleft = rheight - ((int)((double)rheight / dh)) * dh;

        if (accept && (divs <= 0 || ((int)bdivs - 1 > (int)divs && (bcolhd / bdivs) + bcolhd >= colh)) && rleft > 10)
        {
            divs = bdivs;
            colhd = bcolhd;
            fcmaxd = norm * p;
        }

        norm--;

    }
    while (norm > (fmaxd * 2) / (p * 3));

    if (!divs)
    {
        fdivh = colhd;
        fdivval = fmaxd;
    }
    else
    {
        fdivh = (double)colhd / divs;
        fdivval = fcmaxd / divs;
    }

    count_numw();

    TRect r = drawarea;
    static bmph = r.Height();
    if (bmph != r.Height())
    {
        bmph = r.Height();
        clear_buffer();
    }
    else
        allocate();
}
//---------------------------------------------------------------------------
void TStatObj::allocate()
{
    TRect r = drawarea;
    int newcnt = MAX(0, (r.Width() + fsize - 1) / fsize) + 1;
    if (newcnt != buffcnt)
    {
        TBmp **tmp = buff;
        buff = new TBmp *[newcnt];
        if (buffcnt)
        {
            memcpy(buff, tmp, MIN(newcnt, buffcnt)*sizeof(TBmp*));
            for (int i = newcnt; i < buffcnt; i++)
                if (tmp[i])
                    delete tmp[i];
            delete[] tmp;
        }
        for (int i = buffcnt; i < newcnt; i++)
            buff[i] = NULL;
        buffcnt = newcnt;
    }
}
//---------------------------------------------------------------------------
void __fastcall TStatObj::pbResize(TObject *Sender)
{
    measure();
    scroll->Range = fsize * count + fnumw;
    if (fonresize)
        fonresize(Sender);
}
//---------------------------------------------------------------------------
void TStatObj::set_numfont(TCanvas *c)
{
    c->Font->Style = TFontStyles();
    c->Font->Name = SystemFont;
    c->Font->Charset = DEFAULT_CHARSET;
    c->Font->Size = 8;
    c->Font->Color = clBlack;
}
//---------------------------------------------------------------------------
void set_trivertex(_TRIVERTEX &tv, int x, int y, TColor c)
{
    tv.x = x;
    tv.y = y;
    tv.Alpha = 0;
    tv.Red = (0xff & c) << 8;
    tv.Green = (0xff00 & c);
    tv.Blue = (0xff0000 & c) >> 8;
}
void set_gradtriangle(_GRADIENT_TRIANGLE &gt, int v1, int v2, int v3)
{
    gt.Vertex1 = v1;
    gt.Vertex2 = v2;
    gt.Vertex3 = v3;
}
void TStatObj::draw_numbers()
{
    //vertical axis
    if (drawvnum)
    {
        TCanvas *c = vnumbmp->Canvas;

        set_numfont(c);
        c->Brush->Color = (TColor)0xe9e1df;
        GradientFillCanvas(c, (TColor)0xe9e1df, COLORINC(0xe9e1df, 0x282828), Rect(0, 0, vnumbmp->Width, vnumbmp->Height), gdHorizontal);

        c->Pen->Color = (TColor)0xa39d9c;
        c->MoveTo(vnumbmp->Width - 1, 0);
        c->LineTo(vnumbmp->Width - 1, vnumbmp->Height);
        c->Pen->Color = c->Font->Color;

        UnicodeString str;

        SetTextAlign(c->Handle, TA_RIGHT | TA_BASELINE);

        c->Brush->Style = bsClear;
        int sval = fdivval;
        int ydiv = 1;
        int y = vnumbmp->Height;
        while (y - fdivh * ydiv >= 0)
        {
            if (fnumproc)
                str = fnumproc(sval);
            else
                str = IntToStr(sval);

            c->TextOut(fnumw - 8, y - fdivh*ydiv + 5 + 0.5, str.c_str());
            c->MoveTo(fnumw - 4, y - fdivh*ydiv + 0.5);
            c->LineTo(fnumw, y - fdivh*ydiv + 0.5);
            ydiv++;
            sval += fdivval;
        }

        //number field explanation
        c->Font->Size = 10;
        c->Font->Color = clGray;
        c->Font->Style = TFontStyles() << fsBold;

        SetTextAlign(c->Handle, TA_CENTER | TA_TOP);
        TSize s;
        GetTextExtentPoint32(c->Handle, ystr.c_str(), ystr.Length(), &s);
        TRect r = Rect(0, 0, s.cy + 2, vnumbmp->Height);
        AngleTextOutEx(c, 90, r.Left, (r.Bottom) / 2, r, ystr);
        c->Brush->Style = bsSolid;

        drawvnum = false;
    }

    //horizontal axis
    if (drawhnum)
    {
        TCanvas *c = hnumbmp->Canvas;
        c->Brush->Color = (TColor)0xe9e1df;
        set_numfont(c);

        _TRIVERTEX tv[4];
        set_trivertex(tv[0], 0, 0, (TColor)0xe9e1df);
        set_trivertex(tv[1], fnumw, 0, COLORINC(0xe9e1df, 0x282828));
        set_trivertex(tv[2], fnumw, hnumbmp->Height, (TColor)0xe9e1df);
        set_trivertex(tv[3], 0, hnumbmp->Height, (TColor)0xe9e1df);
        _GRADIENT_TRIANGLE gt[2];
        set_gradtriangle(gt[0], 0, 1, 3);
        set_gradtriangle(gt[1], 1, 3, 2);
        GradientFill(c->Handle, tv, 4, gt, 2, GRADIENT_FILL_TRIANGLE);

        GradientFillCanvas(c, COLORINC(0xe9e1df, 0x282828), (TColor)0xe9e1df, Rect(fnumw, 0, hnumbmp->Width, hnumbmp->Height), gdVertical);

        c->Pen->Color = (TColor)0xa39d9c;
        c->MoveTo(fnumw - 1, 0);
        c->LineTo(hnumbmp->Width, 0);
        c->Pen->Color = c->Font->Color;

        int x = (fsize / 2) - (fpos % fsize);
        TSize s;
        SetTextAlign(c->Handle, TA_CENTER | TA_TOP);

        HRGN rgn = CreateRectRgn(fnumw - 4, 0, hnumbmp->Width, hnumbmp->Height);
        try
        {
            SelectClipRgn(c->Handle, rgn);
        }
        catch (...)
        {
            ;
        }
        DeleteObject(rgn);
        c->Brush->Style = bsClear;
        UnicodeString str;
        int y = 4;
        if (buffpos > 0)
        {
            str = fkeyproc(buffpos - 1, count ? cols->Items[buffpos - 1]->key : 0);
            GetTextExtentPoint32(c->Handle, str.c_str(), str.Length(), &s);
        }
        else
            s.cx = 0;

        int i = buffpos;
        while (x + fnumw / 2 < hnumbmp->Width)
        {
            str = fkeyproc(i, i < count ? cols->Items[i]->key : count ? cols->Items[count - 1]->key + (i - count + 1) * fkeyinc : fbasekey + i * fkeyinc);

            TSize ts;

            GetTextExtentPoint32(c->Handle, str.c_str(), str.Length(), &ts);
            if (i % 2 && s.cx + 5 > fsize)
                y += s.cy + 2;
            else
                y = 4;

            s = ts;

            c->TextOut(x + fnumw, y, str);
            x += fsize;

            i++;
        }
        SelectClipRgn(c->Handle, NULL);

        //number field explanation
        c->Font->Size = 10;
        c->Font->Color = clGray;
        c->Font->Style = TFontStyles() << fsBold;

        SetTextAlign(c->Handle, TA_CENTER | TA_TOP);
        GetTextExtentPoint32(c->Handle, xstr.c_str(), xstr.Length(), &s);
        c->TextOut(hnumbmp->Width / 2, hnumbmp->Height - s.cy - 2, xstr);
        c->Brush->Style = bsSolid;

        drawhnum = false;
    }

    buffer->draw(vnumbmp->Canvas->Handle, 0, 0, vnumbmp->Width, vnumbmp->Height);
    buffer->draw(hnumbmp->Canvas->Handle, 0, scroll->ClientHeight - fnumh, hnumbmp->Width, hnumbmp->Height);
    //pb->Canvas->Draw(0,0,vnumbmp);
    //pb->Canvas->Draw(0,scroll->ClientHeight-fnumh,hnumbmp);
}
//---------------------------------------------------------------------------
void TStatObj::draw_buff(int i)
{
    if (!buff[i])
        buff[i] = new TBmp;
    buff[i]->SetSize(fsize, drawarea.Height());

    int ix = i + fpos / fsize;

    TCanvas *c = buff[i]->Canvas;
    TRect r = Rect(0, 0, buff[i]->Width - 1, buff[i]->Height);

    c->Pen->Color = (TColor)0xa9bb8c;
    c->MoveTo(r.Right, 0);
    c->LineTo(r.Right, r.Height());

    if (fredline < 0 || ix <= fredline)
        GradientFillCanvas(c, (TColor)0xfcf4f2, (TColor)0xf5d4c1, r, gdVertical);
    else
        GradientFillCanvas(c, (TColor)0xf0f2f6, (TColor)0xcacfe1, r, gdVertical);

    double y = r.Bottom;
    int ydiv = 0;

    c->Pen->Color = (TColor)0xccc1cb;
    while (y - fdivh * ydiv >= 0)
    {
        c->MoveTo(0, (int)(y - fdivh*ydiv + 0.5));
        c->LineTo(r.Width(), (int)(y - fdivh*ydiv + 0.5));
        ydiv++;
    }

    TStatColumn *item = (count <= ix ? NULL : cols->Items[ix]);

    if (item && item->valcnt)
    {
        UnicodeString str;
        TSize s;

        //green 0x24a914    0x82ec75 - 0x5bbc50
        //blue 0xae541f     0xe9986b - 0xbe6836
        //yellow 0x17b9bb   0xc6feff - 0x84eef2
        //red 0x119d11      0x6d6dfe - 0x3737d7
        TColor brd[4] = { (TColor)0x24a914, (TColor)0xae541f, (TColor)0x17b9bb, (TColor)0x1817a3 };
        TColor lt[4] = { (TColor)0x82ec75, (TColor)0xe9986b, (TColor)0xc6feff, (TColor)0x6d6dfe };
        TColor dk[4] = { (TColor)0x5bbc50, (TColor)0xbe6836, (TColor)0x84eef2, (TColor)0x3737d7 };

        int *vals = new int[item->valcnt];
        try
        {
            memcpy(vals, item->values, sizeof(int)*item->valcnt);
            if (item->stacked)
            {
                for (int i = item->valcnt - 2; i >= 0; i--)
                    vals[i] += vals[i + 1];
            }
            for (int i = 0; i < item->valcnt - 1; i++)
                if (vals[i] < vals[i + 1])
                    vals[i] = vals[i + 1];

            int *heights = new int[item->valcnt];
            try
            {
                for (int i = 0; i < item->valcnt; i++)
                    heights[i] = ((double)colh / fmax) * vals[i] + 0.5;

                if (fnumproc)
                    str = fnumproc(vals[0]);
                else
                    str = IntToStr(vals[0]);

                c->Font->Style = TFontStyles() << fsBold;
                c->Font->Color = (TColor)0x307010;
                c->Brush->Style = bsClear;
                GetTextExtentPoint32(c->Handle, str.c_str(), str.Length(), &s);

                TRect r2;
                if (s.cx < fsize - fpad * 2)
                {
                    SetTextAlign(c->Handle, TA_BOTTOM | TA_CENTER);
                    r2 = Rect((r.Width() - s.cx) / 2 - 2, y - heights[0] - s.cy - 2, (r.Width() + s.cx + 1) / 2 + 2, y - heights[0]);
                    ExtTextOut(c->Handle, r.Width() / 2, y - heights[0] - 1, ETO_CLIPPED, &r2, str.c_str(), str.Length(), NULL);
                }
                else
                {
                    SetTextAlign(c->Handle, TA_LEFT | TA_TOP);
                    r2 = Rect(0, 0, fsize - fpad, 1);
                    int h = DrawText(c->Handle, str.c_str(), str.Length(), &r2, DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK);
                    r2 = Rect((r.Width() - r2.Width()) / 2, y - heights[0] - 2 - h, (r.Width() + r2.Width()) / 2, y - heights[0] - 2);
                    DrawText(c->Handle, str.c_str(), str.Length(), &r2, DT_NOPREFIX | DT_WORDBREAK | DT_CENTER);
                }
                c->Brush->Style = bsSolid;

                bool topline = false;

                for (int i = 0; i < item->valcnt; i++)
                {
                    if (i < item->valcnt - 1 && heights[i] == heights[i + 1])
                        continue;

                    c->Pen->Color = brd[i];
                    int ty = r.Height() - (int)heights[i];
                    int by = MAX(ty + 1, r.Height() - (int)(i < item->valcnt - 1 ? heights[i + 1] : 0));
                    if (!topline)
                    {
                        topline = true;
                        c->MoveTo(fpad, ty);
                        c->LineTo(r.Width() - fpad, ty);
                        ty++;
                    }

                    r2 = Rect(fpad + 1, r.Height() - colh, r.Width() - fpad - 1, r.Height());

                    c->MoveTo(fpad, ty);
                    c->LineTo(fpad, by);
                    c->MoveTo(r.Width() - fpad - 1, ty);
                    c->LineTo(r.Width() - fpad - 1, by);

                    HRGN rgn = CreateRectRgn(fpad, ty, r.Width() - fpad, by);
                    SelectClipRgn(c->Handle, rgn);
                    DeleteObject(rgn);

                    GradientFillCanvas(c, lt[i], dk[i], r2, gdVertical);

                    SelectClipRgn(c->Handle, NULL);
                }
            }
            catch (...)
            {
                ;
            }
            delete[] heights;
        }
        catch (...)
        {
            ;
        }
        delete[] vals;
    }

    if (fredline >= 0)
    {
        if (fredline == ix || fredline + 1 == ix)
        {
            int x = fredline == ix ? fsize - 1 : 0;
            c->Pen->Width = 2;
            c->Pen->Color = clRed;
            c->MoveTo(x, 0);
            c->LineTo(x, buff[i]->Height);

            c->Pen->Width = 1;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TStatObj::pbPaint(TObject *Sender)
{
    if (!buffcnt)
        allocate();

    buffer = new TFastBuffer(pb->ClientWidth, pb->ClientHeight - scroll->Size);
    try
    {
        //TCanvas *c = pb->Canvas;

        TRect r = drawarea;

        int i = 0;
        int x = -fpos % fsize + r.Left;

        /*HRGN rgn = CreateRectRgn(fnumw,0,scroll->ClientWidth,scroll->ClientHeight-fnumh);
         try {
         SelectClipRgn(buffer->Handle,rgn);
         } catch(...) {
        ;
         }
         DeleteObject(rgn);
         */
        buffer->clip(Rect(fnumw, 0, scroll->ClientWidth, scroll->ClientHeight - fnumh));

        while (i < buffcnt)
        {
            if (!buff[i])
                draw_buff(i);

            //c->Draw(x,0,buff[i]);
            buffer->draw(buff[i]->Canvas->Handle, x, 0, buff[i]->Width, buff[i]->Height);

            x += fsize;
            i++;
        }

        //SelectClipRgn(c->Handle,NULL);
        buffer->noclip();
        draw_numbers();

        buffer->blit(pb->Canvas->Handle);
    }
    catch (...)
    {
        ;
    }

    delete buffer;
    buffer = NULL;

    if (fonpaint)
        fonpaint(Sender);
}
//---------------------------------------------------------------------------
void TStatObj::set_items(TStatColumns *acols, UnicodeString xaxis, UnicodeString yaxis, int maxval, int numberdivision, int redline, TStatNumProc anumproc, TStatKeyProc akeyproc, TStatHintProc ahintproc)
{
    hint8->ReleaseHandle();

    cols->Assign(acols);
    fredline = redline;

    fnumdiv = numberdivision;

    xstr = xaxis;
    ystr = yaxis;
    if (!count || !maxval)
        cols->Clear();

    fmax = maxval;

    fkeyproc = akeyproc;
    fnumproc = anumproc;
    fhintproc = ahintproc;
    if (fhintproc == NULL)
        fpopup = false;
    fpos = 0;
    drawvnum = drawhnum = true;

    clear_buffer();
    measure();

    Screen->Cursor = crDefault;

    scroll->Range = fsize * count + fnumw;
    scroll->Position = 0;

    pb->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TStatObj::pbMouseDown(TObject *Sender, TMouseButton button, TShiftState shift, int x, int y)
{
    if (Screen->Cursor == fcursor && button == mbLeft)
    {
        hint8->ReleaseHandle();
        fmoving = true;
        fscroll = scroll->Position;
        fmousex = x;
    }

    if (fonmousedown)
        fonmousedown(Sender, button, shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TStatObj::pbMouseUp(TObject *Sender, TMouseButton button, TShiftState shift, int x, int y)
{
    if (fmoving)
        fmoving = false;

    if (scroll->Range > scroll->ClientWidth && x > fnumw && y < scroll->ClientHeight)
        Screen->Cursor = fcursor;
    else if (!fmoving)
        Screen->Cursor = crDefault;

    if (fonmouseup)
        fonmouseup(Sender, button, shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TStatObj::pbMouseMove(TObject *Sender, TShiftState shift, int x, int y)
{
    if (fmoving)
    {
        int dx = fmousex - x;
        scroll->Position = fscroll + dx;
        return;
    }

    if (!scroll->Scrolling && scroll->Range > scroll->ClientWidth && x > fnumw && y < scroll->ClientHeight)
        Screen->Cursor = fcursor;
    else if (!fmoving)
        Screen->Cursor = crDefault;

    if (!fpopup || !fhintproc)
    {
        if (fonmousemove)
            fonmousemove(Sender, shift, x, y);
        return;
    }

    int X = x;
    x -= fnumw;
    if (x < 0 || y > scroll->ClientHeight - fnumh || (x + fpos) / fsize >= count)
    {
        hint8->ReleaseHandle();
        if (fonmousemove)
            fonmousemove(Sender, shift, X, y);
        return;
    }

    int ix = (x + fpos) / fsize;
    UnicodeString str = fhintproc(ix, *cols->Items[ix]);
    if (str.Length())
    {
        TPoint p = pb->ClientToScreen(Point(x + fnumw, y + 24));
        TRect r = hint8->CalcHintRect(9999, str, NULL);
        OffsetRect(&r, p.x, p.y);
        hint8->ActivateHint(r, str);
    }
    else
        hint8->ReleaseHandle();

    if (fonmousemove)
        fonmousemove(Sender, shift, X, y);
}
//---------------------------------------------------------------------------
void __fastcall TStatObj::pbMouseLeave(TObject *Sender)
{
    Screen->Cursor = crDefault;

    hint8->ReleaseHandle();
    if (fonmouseleave)
        fonmouseleave(Sender);
}
//---------------------------------------------------------------------------
void TStatObj::set_popup(bool val)
{
    if (fpopup == val)
        return;
    fpopup = val;
    if (!fpopup)
        hint8->ReleaseHandle();
}
//---------------------------------------------------------------------------
void TStatObj::set_hintproc(TStatHintProc hproc)
{
    fhintproc = hproc;
    hint8->ReleaseHandle();
}
//---------------------------------------------------------------------------
void TStatObj::appActivate(void *caller)
{
}
//---------------------------------------------------------------------------
void TStatObj::appDeactivate(void *caller)
{
    fmoving = false;
    Screen->Cursor = crDefault;
    hint8->ReleaseHandle();
}
//---------------------------------------------------------------------------
void TStatObj::formActivate(void *caller)
{
    ;
}
//---------------------------------------------------------------------------
void TStatObj::formDeactivate(void *caller)
{
    fmoving = false;
    Screen->Cursor = crDefault;

    hint8->ReleaseHandle();
}
//---------------------------------------------------------------------------
void TStatObj::scrollproc()
{
    fpos = scroll->Position;
    int nbuff = fpos / fsize;
    if (nbuff != buffpos)
    {
        if (abs(nbuff - buffpos) >= buffcnt)
        {
            for (int i = 0; i < buffcnt; i++)
            {
                delete buff[i];
                buff[i] = NULL;
            }
        }
        else
        {
            if (nbuff < buffpos)
            {
                for (int i = 1; i <= buffpos - nbuff; i++)
                    delete buff[buffcnt - i];

                memmove(buff + (buffpos - nbuff), buff, sizeof(TBmp*)*(buffcnt - buffpos + nbuff));
                memset(buff, 0, sizeof(TBmp*)*(buffpos - nbuff));
            }
            else
            { //always true: if (buffpos < nbuff)
                for (int i = buffpos; i < nbuff; i++)
                    delete buff[i - buffpos];

                memmove(buff, buff + (nbuff - buffpos), sizeof(TBmp*)*(buffcnt - nbuff + buffpos));
                memset(buff + (buffcnt - nbuff + buffpos), 0, sizeof(TBmp*)*(nbuff - buffpos));
            }
        }
    }
    buffpos = nbuff;

    drawhnum = true;
    pb->Invalidate();
}
//---------------------------------------------------------------------------
void TStatObj::set_pos(int val)
{
    val = MAX(0, MIN(val, fsize * count + fnumw));
    if (fpos == val)
        return;

    if (fmoving)
        fmoving = false;

    scroll->Position = val;

}
//---------------------------------------------------------------------------
int TStatObj::get_count()
{
    if (cols)
        return cols->Count;
    return 0;
}
//---------------------------------------------------------------------------
const TStatColumn* TStatObj::get_col(int ix)
{
    return cols->Items[ix];
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(153);
#endif

