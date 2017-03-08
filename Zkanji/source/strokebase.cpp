//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "strokebase.h"
#include "elementpoints.h"
#include "angle.h"
#include "recobj.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(156);
#endif


//---------------------------------------------------------------------------
TStrokeObjBase::TStrokeObjBase(TZPaintBox *fpb) : pb(fpb), lastpenwidth(2), fdrawing(false), fcallback(NULL)
{
    bmp = new TBmp;
    bmp->Width = pb->ClientWidth;
    bmp->Height = pb->ClientHeight;
    bmp->Canvas->Brush->Color = clWhite;
    bmp->Canvas->Pen->Color = clBlack;
    bmp->Canvas->Pen->Width = 2;

    fpoints = new TPointListEx;

    fonpaint = pb->OnPaint;
    fonmousedown = pb->OnMouseDown;
    fonmouseup = pb->OnMouseUp;
    fonmousemove = pb->OnMouseMove;
    fonformdeactivate = ((TForm*)pb->Owner)->OnDeactivate;
    fonexit = pb->OnExit;
    fonresize = pb->OnResize;
    pb->OnPaint = pbPaint;
    pb->OnMouseDown = pbMouseDown;
    pb->OnMouseUp = pbMouseUp;
    pb->OnMouseMove = pbMouseMove;
    pb->OnResize = pbResize;
    ((TForm*)pb->Owner)->OnDeactivate = formDeactivate;

    AppActivateEvent(this, appDeactivate, false, true);
    pb->OnExit = &pbExit;

    clear_bmp();
}
//---------------------------------------------------------------------------
TStrokeObjBase::~TStrokeObjBase()
{
    AppActivateEvent(this, appDeactivate, false, false);
    deactivated();

    delete fpoints;
    pb->OnResize = fonresize;
    pb->OnPaint = fonpaint;
    pb->OnMouseDown = fonmousedown;
    pb->OnMouseUp = fonmouseup;
    pb->OnMouseMove = fonmousemove;
    ((TForm*)pb->Owner)->OnDeactivate = fonformdeactivate;
    pb->OnExit = fonexit;
    delete bmp;
}
//---------------------------------------------------------------------------
void __fastcall TStrokeObjBase::pbPaint(TObject *Sender)
{
    pb->Canvas->Draw(0, 0, bmp);
}
//---------------------------------------------------------------------------
void __fastcall TStrokeObjBase::pbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    DoMouseDown(Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void TStrokeObjBase::DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (fdrawing || Button != mbLeft)
        return;

    lastpenwidth = max(2, bmp->Width / 45) - 1;
    fpoints->clear();

    fdrawing = true;
    bmp->Canvas->MoveTo(x, y);
    lastmouse = Point(x, y);

    pbMouseMove(NULL, Shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TStrokeObjBase::pbMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    if (!fdrawing)
        return;

    fpoints->add(x*10000 / pb->ClientWidth, y*10000 / pb->ClientHeight);

    if (fpoints->Count == 1 || lastmouse == Point(x, y))
    {
        lastmouse = Point(x, y);
        return;
    }

    TCanvas *c = bmp->Canvas;

    bmp->Canvas->MoveTo(lastmouse.x, lastmouse.y);

    double d = min(bmp->Width / 12, max(max(2, bmp->Width / 65), (double)bmp->Width / (Len(Point(x, y), lastmouse) * 20)));
    lastpenwidth = d < lastpenwidth ? max(d, lastpenwidth * 0.9) : d > lastpenwidth ? min(d, lastpenwidth * 1.1) : lastpenwidth;

    c->Pen->Width = lastpenwidth;
    c->Pen->Color = (TColor)0xd0d0d0;
    c->LineTo(x, y);

    TRect r = Rect(lastmouse.x, lastmouse.y, x, y);
    lastmouse = Point(x, y);

    if (r.Left > r.Right)
        swap(r.left, r.right);
    if (r.Top > r.Bottom)
        swap(r.top, r.bottom);
    InflateRect(&r, c->Pen->Width, c->Pen->Width);
    InvalidateRect(pb->Handle, &r, false);
}
//---------------------------------------------------------------------------
void __fastcall TStrokeObjBase::pbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (!fdrawing || Button != mbLeft)
        return;

    pbMouseMove(Sender, Shift, x, y);
    fdrawing = false;

    procstroke();
}
//---------------------------------------------------------------------------
void __fastcall TStrokeObjBase::formDeactivate(TObject *Sender)
{
    deactivated();
    if (fonformdeactivate)
        fonformdeactivate(Sender);
}
//---------------------------------------------------------------------------
void TStrokeObjBase::appDeactivate(void *caller)
{
    deactivated();
}
//---------------------------------------------------------------------------
void __fastcall TStrokeObjBase::pbExit(TObject *Sender)
{
    if (fonexit)
        fonexit(Sender);
    deactivated();
}
//---------------------------------------------------------------------------
void __fastcall TStrokeObjBase::deactivated()
{
    if (!fdrawing)
        return;
    fdrawing = false;
    fpoints->clear();
}
//---------------------------------------------------------------------------
void TStrokeObjBase::draw(TPointListEx *pts)
{
    TCanvas *c = bmp->Canvas;
    c->Pen->Color = clBlack;
    c->Pen->Width = max(3, bmp->Width / 60);
    TDoublePoint p = *pts->Items[0];
    c->MoveTo((p.x*pb->ClientWidth) / 10000.0, (p.y*pb->ClientHeight) / 10000.0);
    for (int ix = 1; ix < pts->Count; ++ix)
    {
        p = *pts->Items[ix];
        c->LineTo((p.x*pb->ClientWidth) / 10000.0, (p.y*pb->ClientHeight) / 10000.0);
    }

    pb->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TStrokeObjBase::pbResize(TObject *Sender)
{
    if (fonresize)
        fonresize(Sender);

    if (pb->ClientWidth != pb->ClientHeight)
        return;

    bmp->SetSize(pb->ClientWidth, pb->ClientHeight);
    redraw();
}
//---------------------------------------------------------------------------
void TStrokeObjBase::clear_bmp()
{
    TCanvas *c = bmp->Canvas;
    c->Brush->Color = clWhite;
    c->FillRect(Rect(0, 0, bmp->Width, bmp->Height));
    pb->Invalidate();
}
//---------------------------------------------------------------------------
void TStrokeObjBase::reset()
{
    deactivated();
    fpoints->clear();
    redraw();
    pb->Invalidate();
    if (fcallback)
        fcallback();
}
//---------------------------------------------------------------------------
void TStrokeObjBase::redraw()
{
    clear_bmp();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TStrokeObj::TStrokeObj(TZPaintBox *fpb) : RESULT_COUNT_MAX(8000), fallowerror(true), fcntlimit(-1), fkan(true), fgen(true), fgrid(false), TStrokeObjBase(fpb)
{
    strokes = new TRecPointList;
    results = new int[RESULT_COUNT_MAX]; //should be enough for all kanji and some more characters
    resultcnt = 0;
}
//---------------------------------------------------------------------------
TStrokeObj::~TStrokeObj()
{
    delete[] results;
    delete strokes;
}
//---------------------------------------------------------------------------
void TStrokeObj::redraw()
{
    TStrokeObjBase::redraw();
    TCanvas *c = bmp->Canvas;

    if (fgrid)
    {
        c->Pen->Color = (TColor)0xc0c0c0;
        c->Pen->Style = psSolid;
        c->Pen->Width = 1;

        const int n = pb->ClientWidth / 10;

        c->MoveTo(n, n / 2);
        c->LineTo(n, pb->ClientHeight - n / 2);
        c->MoveTo(pb->ClientWidth - n, n / 2);
        c->LineTo(pb->ClientWidth - n, pb->ClientHeight - n / 2);
        c->MoveTo(n / 2, n);
        c->LineTo(pb->ClientWidth - n / 2, n);
        c->MoveTo(n / 2, pb->ClientHeight - n);
        c->LineTo(pb->ClientWidth - n / 2, pb->ClientHeight - n);

        c->MoveTo(pb->ClientWidth / 2, pb->ClientHeight / 2 - n / 2);
        c->LineTo(pb->ClientWidth / 2, pb->ClientHeight / 2 + n / 2 + 1);
        c->MoveTo(pb->ClientWidth / 2 - n / 2, pb->ClientHeight / 2);
        c->LineTo(pb->ClientWidth / 2 + n / 2 + 1, pb->ClientHeight / 2);
    }

    if (!strokes->Count && !drawing)
    {
        c->Font->Name = "Tahoma";
        if (fgrid)
            c->Font->Size = MIN(MAX(8, bmp->Width / 27), 14);
        else
            c->Font->Size = MIN(MAX(8, bmp->Width / 20), 14);
        c->Font->Color = clSilver;
        UnicodeString s = "Try drawing a Japanese character here and select a candidate below! (Right-click removes drawing)";
        TRect r;
        if (fgrid)
            r = Rect(bmp->Width * 0.15, bmp->Height * 0.15, bmp->Width * 0.85, bmp->Height * 0.85);
        else
            r = Rect(15, 15, bmp->Width - 15, bmp->Height - 15);
        DrawTextW(c->Handle, s.c_str(), s.Length(), &r, DT_LEFT | DT_NOPREFIX | DT_TOP | DT_WORDBREAK);
    }
    else
        for (int ix = 0; ix < strokes->Count; ++ix)
            draw(strokes->original[ix]);

    pb->Invalidate();
}
//---------------------------------------------------------------------------
void TStrokeObj::procstroke()
{
    try
    {
        strokes->add(points);
        redraw();
        resultcnt = RESULT_COUNT_MAX;
        strokes->compare(results, resultcnt, fallowerror ? 1 : 0, fcntlimit, fkan, fgen);
        if (callback)
            callback();
    }
    catch (...)
    {
        ;
    }
}
//---------------------------------------------------------------------------
void TStrokeObj::reset()
{
    strokes->Clear();
    TStrokeObjBase::reset();
}
//---------------------------------------------------------------------------
void TStrokeObj::undo()
{
    if (strokes->Count)
        strokes->remove(strokes->Count - 1);

    redraw();

    resultcnt = RESULT_COUNT_MAX;
    strokes->compare(results, resultcnt, fallowerror ? 1 : 0, fcntlimit, fkan, fgen);
    if (callback)
        callback();
}
//---------------------------------------------------------------------------
int TStrokeObj::get_cnt()
{
    return strokes->Count;
}
//---------------------------------------------------------------------------
void TStrokeObj::DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (!drawing && Button == mbRight)
        reset();

    TStrokeObjBase::DoMouseDown(Button, Shift, x, y);

    if (!Count && Button == mbLeft)
        redraw();
}
//---------------------------------------------------------------------------
void TStrokeObj::set_grid(bool agrid)
{
    if (fgrid == agrid)
        return;
    fgrid = agrid;
    redraw();
}
//---------------------------------------------------------------------------
void TStrokeObj::set_kan(bool val)
{
    if (fkan == val)
        return;
    fkan = val;
    if (!fkan)
        fgen = true;
}
//---------------------------------------------------------------------------
void TStrokeObj::set_gen(bool val)
{
    if (fgen == val)
        return;
    fgen = val;
    if (!fgen)
        fkan = true;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(157);
#endif

