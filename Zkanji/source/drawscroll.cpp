//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Math.h>
#include "drawscroll.h"
#include "mouseevent.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(28);
#endif

//---------------------------------------------------------------------------
TDrawScroll::TDrawScroll(TZPaintBox *apb, int asize, TFuncPtr acallback)
{
    pb = apb;
    fori = dsoHorizontal;
    size = asize;
    fenabled = false;
    sscroll = spagescroll = false;
    fcallback = acallback;
    fthumb = -1;
    flargech = -1;
    funitsize = 1;

    sbpos = 0;
    frange = 0;

    fonpaint = pb->OnPaint;
    fonmousedown = pb->OnMouseDown;
    fonmouseup = pb->OnMouseUp;
    fonmousemove = pb->OnMouseMove;
    fonresize = pb->OnResize;

    pb->OnPaint = pbPaint;
    pb->OnMouseDown = pbMouseDown;
    pb->OnMouseUp = pbMouseUp;
    pb->OnMouseMove = pbMouseMove;
    pb->OnResize = pbResize;
}
//---------------------------------------------------------------------------
TDrawScroll::~TDrawScroll()
{
    pb->OnPaint = fonpaint;
    pb->OnMouseDown = fonmousedown;
    pb->OnMouseUp = fonmouseup;
    pb->OnMouseMove = fonmousemove;
    pb->OnResize = fonresize;
}
//---------------------------------------------------------------------------
void TDrawScroll::Invalidate()
{
    TRect r = ScrollRect;
    InvalidateRect(pb->Handle, &r, false);
}
//---------------------------------------------------------------------------
void __fastcall TDrawScroll::pbPaint(TObject *Sender)
{
    TCanvas *c = pb->Canvas;
    TColor pc = c->Pen->Color;
    TColor bc = c->Brush->Color;

    c->Pen->Color = clGray;

    TRect r = ScrollRect;

    c->MoveTo(r.Left, r.Top);
    if (fori == dsoHorizontal)
    {
        c->LineTo(r.Right, r.Top);
        r.Top++;
    }
    else
    {
        c->LineTo(r.Left, r.Bottom);
        r.Left++;
    }

    if (!fenabled)
    {
//c->Brush->Color = clScrollBar;

//c->FillRect(r);
        GradientFillCanvas(c, COLORINC(clScrollBar, 0x060606), COLORDEC(clScrollBar, 0x101010), r, fori == dsoHorizontal ? gdVertical : gdHorizontal);
    }
    else
    {
        TRect grayr = fori == dsoHorizontal ? Rect(sbarpos, r.Top, sbarpos + ssize, pb->ClientHeight) : Rect(r.Left, sbarpos, pb->ClientWidth, sbarpos + ssize);

        c->Brush->Color = clWhite;
        c->FillRect(Rect(r.Left, r.Top, fori == dsoHorizontal ? sbarpos : pb->ClientWidth, fori == dsoHorizontal ? pb->ClientHeight : sbarpos));
        c->FillRect(Rect(fori == dsoHorizontal ? sbarpos + ssize : r.Left, fori == dsoHorizontal ? r.Top : sbarpos + ssize, pb->ClientWidth, pb->ClientHeight));

        c->Brush->Color = clGray;
        c->FillRect(grayr);
    }

    c->Pen->Color = pc;
    c->Brush->Color = bc;

    if (fonpaint)
        fonpaint(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TDrawScroll::repeatfunc(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    int a = x, b = y, as = pb->ClientWidth, bs = pb->ClientHeight;
    if (fori == dsoVertical)
    {
        memswap(a, b);
        memswap(as, bs);
    }

    if (b >= bs - size && Button == mbLeft)
        pbMouseDown(Sender, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TDrawScroll::pbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    int a = x, b = y, as = pb->ClientWidth, bs = pb->ClientHeight;
    if (fori == dsoVertical)
    {
        memswap(a, b);
        memswap(as, bs);
    }

    if (b >= bs - size)
    {
        if (!fenabled || Button != mbLeft)
            return;

        sscroll = !spagescroll;
        if ((spagescroll || sscroll) && (a < sbarpos || a >= sbarpos + ssize))
        {
            spagescroll = true;
            int opos = sbpos;
            sscroll = false;
            sbpos += (a < sbarpos ? -1 : 1) * (flargech <= 0 ? as : flargech);
            sbpos = MIN(Range - as, MAX(0, sbpos));
            if (sbpos != opos)
            {
                UpdateBar();
                if (fcallback)
                    fcallback();
            }
            TRect r = Rect((a < sbarpos ? 0 : sbarpos + ssize), bs - size, (a < sbarpos ? sbarpos : as), bs);
            if (fori == dsoVertical)
            {
                memswap(r.left, r.top);
                memswap(r.right, r.bottom);
            }
            MouseRepeat(repeatfunc, Sender, Button, Shift, x, y, &r);
            return;
        }
        else if (spagescroll)
        {
            MouseStopRepeat();
            return;
        }

        if (sscroll)
            sdragpos = a - sbarpos;

    }
    else if (fonmousedown)
        fonmousedown(Sender, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TDrawScroll::pbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (sscroll || spagescroll)
    {
        if (!fenabled)
            return;

        sscroll = false;
        spagescroll = false;

        UpdateBar();
    }
    else if (fonmouseup)
        fonmouseup(Sender, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TDrawScroll::pbMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    if (sscroll || spagescroll)
    {
        if (!fenabled || !sscroll || !frange)
            return;

        int opos = sbarpos;
        int obpos = sbpos;

        int rangesize = (fori == dsoHorizontal ? pb->ClientWidth : pb->ClientHeight);
        int a = (fori == dsoHorizontal ? x : y);

        sbarpos = MAX(0, MIN(rangesize - ssize, a - sdragpos));

        double block = (double)(rangesize - ssize) / (frange - rangesize);
        sbpos = ((double)sbarpos + block / 2) / block;

        if (opos != sdragpos)
            Invalidate();

        if (sbpos != obpos && fcallback)
            fcallback();
    }
    else if (fonmousemove)
        fonmousemove(Sender, Shift, x, y);
}
//---------------------------------------------------------------------------
void TDrawScroll::set_thumbsize(int val)
{
    if (fthumb == val)
        return;
    fthumb = MAX(val, 4);

    UpdateBar();
}
//---------------------------------------------------------------------------
void TDrawScroll::set_unitsize(int val)
{
    if (funitsize == val)
        return;
    funitsize = MAX(val, 1);

    UpdateBar();
}
//---------------------------------------------------------------------------
bool TDrawScroll::get_scrolling()
{
    return sscroll || spagescroll;
}
//---------------------------------------------------------------------------
void __fastcall TDrawScroll::pbResize(TObject *Sender)
{
    if (fonresize)
        fonresize(Sender);
    UpdateBar();
}
//---------------------------------------------------------------------------
void TDrawScroll::set_orientation(TDrawScrollOrientation val)
{
    if (fori == val)
        return;
    fori = val;

    UpdateBar();
    if (fonresize)
        fonresize(pb);
}
//---------------------------------------------------------------------------
void TDrawScroll::set_pos(int val)
{
    val = MIN(Range - (fori == dsoHorizontal ? pb->ClientWidth : pb->ClientHeight), MAX(0, val));

    if (sbpos == val)
        return;
    sbpos = val;

    UpdateBar();
}
//---------------------------------------------------------------------------
void TDrawScroll::set_range(int val)
{
    val = MAX(val, 0);
    if (frange == val)
        return;
    frange = val;

    UpdateBar();
}
//---------------------------------------------------------------------------
void TDrawScroll::UpdateBar()
{
    int rangesize = (fori == dsoHorizontal ? pb->ClientWidth : pb->ClientHeight);

    fenabled = frange > rangesize;

    sbpos = MAX(0, MIN(sbpos, frange - rangesize));

    if (fenabled)
    {
        if (fthumb < 0)
        {
            int unitrange = (frange - rangesize) * funitsize + rangesize;
            ssize = MAX(32, rangesize * ((double)rangesize / unitrange));
        }
        else
            ssize = MAX(32, MIN(fthumb, rangesize - 1));
        sbarpos = (double)sbpos * (rangesize - ssize) / (frange - rangesize);
    }

    Invalidate();

    if (fcallback)
        fcallback();
}
//---------------------------------------------------------------------------
TRect TDrawScroll::get_client()
{
    return Rect(0, 0, ClientWidth, ClientHeight);
}
//---------------------------------------------------------------------------
TRect TDrawScroll::get_rect()
{
    switch (fori)
    {
    case dsoHorizontal:
        return Rect(0, ClientHeight, pb->ClientWidth, pb->ClientHeight);
    case dsoVertical:
    default:
        return Rect(ClientWidth, 0, pb->ClientWidth, pb->ClientHeight);
    }
}
//---------------------------------------------------------------------------
int TDrawScroll::get_clientwidth()
{
    return fori == dsoHorizontal ? pb->ClientWidth : pb->ClientWidth - size;
}
//---------------------------------------------------------------------------
int TDrawScroll::get_clientheight()
{
    return fori == dsoHorizontal ? pb->ClientHeight - size : pb->ClientHeight;
}
//---------------------------------------------------------------------------
int TDrawScroll::get_largechange()
{
    if (flargech < 0)
        return ssize;
    return flargech;
}
//---------------------------------------------------------------------------
void TDrawScroll::set_largechange(int val)
{
    if (flargech == val)
        return;

    flargech = val;
    if (!flargech)
        flargech = -1;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(29);
#endif

