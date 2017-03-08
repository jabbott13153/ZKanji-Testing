//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "mouseovers.h"
#include "zpaintbox.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(112);
#endif

//---------------------------------------------------------------------------
TMouseOvers::TMouseOvers(TControl * ctrl, TMouseOversPrepareProc prepareproc, TMouseOversMeasureItemProc measureproc, TMouseOversDrawItemProc drawproc, TMouseOversChangeProc changeproc) :
        controlkind(ckUndef), canvas(NULL), fhorzscroll(0), fvertscroll(0), cnt(0), ptpos(-1, -1), ixpos(-1), rects(NULL), locked(false), visible(true), enabled(true), ftag(0), /*drawx(0),drawy(0),*/
        fprepare(prepareproc), fmeasure(measureproc), fdraw(drawproc), fchange(changeproc)
{
    control.form = dynamic_cast<TForm *>(ctrl);
    if (control.form)
    {
        canvas = control.form->Canvas;
        controlkind = ckForm;

        fmouseleave = control.form->OnMouseLeave;
        fmousemove = control.form->OnMouseMove;

        control.form->OnMouseLeave = &DoMouseLeave;
        control.form->OnMouseMove = &DoMouseMove;
    }

    if (controlkind == ckUndef)
    {
        control.paintbox = dynamic_cast<TPaintBox *>(ctrl);
        if (control.paintbox)
        {
            canvas = control.paintbox->Canvas;
            controlkind = ckPB;

            fmouseleave = control.paintbox->OnMouseLeave;
            fmousemove = control.paintbox->OnMouseMove;

            control.paintbox->OnMouseLeave = &DoMouseLeave;
            control.paintbox->OnMouseMove = &DoMouseMove;
        }
    }

    if (controlkind == ckUndef)
    {
        control.zpaintbox = dynamic_cast<TZPaintBox *>(ctrl);
        if (control.zpaintbox)
        {
            canvas = control.zpaintbox->Canvas;
            controlkind = ckZPB;

            fmouseleave = control.zpaintbox->OnMouseLeave;
            fmousemove = control.zpaintbox->OnMouseMove;

            control.zpaintbox->OnMouseLeave = &DoMouseLeave;
            control.zpaintbox->OnMouseMove = &DoMouseMove;
        }
    }

    if (controlkind == ckUndef)
        THROW(L"TMouseOvers currently only works with TForm, TPaintBox and TZPaintBox objects!");

}
//---------------------------------------------------------------------------
TMouseOvers::~TMouseOvers()
{
    reset(0);
    switch (controlkind)
    {
    case ckForm:
        control.form->OnMouseLeave = fmouseleave;
        control.form->OnMouseMove = fmousemove;
        break;
    case ckPB:
        control.paintbox->OnMouseLeave = fmouseleave;
        control.paintbox->OnMouseMove = fmousemove;
        break;
    case ckZPB:
        control.zpaintbox->OnMouseLeave = fmouseleave;
        control.zpaintbox->OnMouseMove = fmousemove;
        break;
    default:
        break;
    }
}
//---------------------------------------------------------------------------
TControl* TMouseOvers::get_control()
{
    switch (controlkind)
    {
    case ckForm:
        return control.form;
    case ckPB:
        return control.paintbox;
    case ckZPB:
        return control.zpaintbox;
    default:
        THROW(L"No way!");
    }
}
//---------------------------------------------------------------------------
void TMouseOvers::reset(int count)
{
    delete rects;
    rects = NULL;

    cnt = count;
}
//---------------------------------------------------------------------------
int TMouseOvers::add(const TRect &r, int data, bool active)
{
    if (!rects)
        rects = new TItemRectList;

    return rects->Add(new TItemRect(r, data, active));
}
//---------------------------------------------------------------------------
void TMouseOvers::draw(int &x, int &y)
{
    if (!visible)
        return;

    TRect r;
    int drawx = x; //+fhorzscroll;
    int drawy = y; //+fvertscroll;

    for (int ix = 0; ix < cnt; ++ix)
    {
        bool prep = true;
        if (!rects || rects->Count == ix)
        {
            int d = 0;
            prep = false;
            bool active = true;
            prepare_init(ix, ixpos == ix);
            if (fmeasure)
                fmeasure(canvas, ix, x, y, r, d, active);
            OffsetRect(r, fhorzscroll, fvertscroll);
            add(r, d, active);
            drawx = x; //+fhorzscroll;
            drawy = y; //+fvertscroll;
        }
        else
            drawy = rects->Items[ix]->r.Top - fvertscroll;
        draw_item(ix, ixpos == ix, prep);

        if (!prep)
            prepare_restore();
    }
    x = drawx; //-fhorzscroll;
    y = drawy; //-fvertscroll;
}
//---------------------------------------------------------------------------
void TMouseOvers::draw_item(int ix, bool selected, bool prepare)
{
    TItemRect *i = rects->Items[ix];
    selected = selected && i->active;
    if (prepare)
        prepare_init(ix, selected);

    TRect r = i->r;
    OffsetRect(r, -fhorzscroll, -fvertscroll);
    if (r.Right < 0 || r.Bottom < 0 || r.Left >= Control->ClientWidth || r.Top >= Control->ClientHeight)
        return;
    fdraw(canvas, r, ix, i->data, selected);

    if (prepare)
        prepare_restore();
}
//---------------------------------------------------------------------------
void TMouseOvers::scroll(int horzscroll, int vertscroll)
{
    fhorzscroll = horzscroll;
    fvertscroll = vertscroll;
}
//---------------------------------------------------------------------------
void TMouseOvers::lock()
{
    locked = true;
}
//---------------------------------------------------------------------------
void TMouseOvers::unlock()
{
    locked = false;
    updatepos();
}
//---------------------------------------------------------------------------
void TMouseOvers::set_visible(bool val)
{
    if (val == visible)
        return;
    visible = val;
    if (visible)
        updatepos();
    else
        ixpos = -1;
}
//---------------------------------------------------------------------------
void TMouseOvers::set_enabled(bool val)
{
    if (val == enabled)
        return;
    enabled = val;
    if (enabled)
        updatepos();
    else
        ixpos = -1;
}
//---------------------------------------------------------------------------
void __fastcall TMouseOvers::DoMouseLeave(TObject *Sender)
{
    if (fmouseleave)
        fmouseleave(Sender);

    if (!rects || !visible || locked)
        return;

    bool changed = false;
    TRect cr;
    int cix;
    int cdata;

    ptpos = Point(-1, -1);
    if (ixpos >= 0 && ixpos < rects->Count)
    {
        draw_item(ixpos, false, true);

        changed = true;
        cr = rects->Items[ixpos]->r;
        cix = ixpos;
        cdata = rects->Items[ixpos]->data;
    }
    ixpos = -1;

    if (enabled && changed)
        DoChange(cr, cix, cdata, false);
}
//---------------------------------------------------------------------------
void __fastcall TMouseOvers::DoMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    bool changed = false;
    TRect cr;
    int cix;
    int cdata;

    if (rects && visible && !locked)
    {
        ptpos = Point(X + fhorzscroll, Y + fvertscroll);

        if (ixpos >= 0 && ixpos < rects->Count && !PtInRect(rects->Items[ixpos]->r, ptpos))
        {
            draw_item(ixpos, false, true);

            changed = true;
            cr = rects->Items[ixpos]->r;
            cix = ixpos;
            cdata = rects->Items[ixpos]->data;

            ixpos = -1;
        }

        if (enabled && changed)
            DoChange(cr, cix, cdata, false);

        changed = false;

        for (int ix = 0; ix < rects->Count; ++ix)
        {
            if (PtInRect(rects->Items[ix]->r, ptpos))
            {
                if (ixpos == ix || !rects->Items[ix]->active)
                    break;

                if (ixpos >= 0)
                    THROW(L"Item areas shouldn't overlap!");

                ixpos = ix;
                draw_item(ixpos, true, true);

                changed = true;
                cr = rects->Items[ixpos]->r;
                cix = ixpos;
                cdata = rects->Items[ixpos]->data;

                break;
            }
        }

        if (enabled && changed)
            DoChange(cr, cix, cdata, true);
    }

    if (fmousemove)
        fmousemove(Sender, Shift, X, Y);
}
//---------------------------------------------------------------------------
void TMouseOvers::prepare_init(int ix, bool selected)
{
    saved.fst = canvas->Font->Style;
    saved.fc = canvas->Font->Color;
    saved.fn = canvas->Font->Name;
    saved.fs = canvas->Font->Size;

    saved.ps = canvas->Pen->Style;
    saved.pm = canvas->Pen->Mode;
    saved.pc = canvas->Pen->Color;
    saved.pw = canvas->Pen->Width;

    saved.bs = canvas->Brush->Style;
    saved.bc = canvas->Brush->Color;

    fprepare(canvas, ix, selected);

}
//---------------------------------------------------------------------------
void TMouseOvers::prepare_restore()
{
    canvas->Brush->Color = saved.bc;
    canvas->Brush->Style = saved.bs;

    canvas->Pen->Width = saved.pw;
    canvas->Pen->Color = saved.pc;
    canvas->Pen->Mode = saved.pm;
    canvas->Pen->Style = saved.ps;

    canvas->Font->Size = saved.fs;
    canvas->Font->Name = saved.fn;
    canvas->Font->Color = saved.fc;
    canvas->Font->Style = saved.fst;
}
//---------------------------------------------------------------------------
void TMouseOvers::updatepos()
{
    TPoint p;
    GetCursorPos(&p);
    HWND h = ChildWindowFromPointEx(GetDesktopWindow(), p, CWP_SKIPINVISIBLE | CWP_SKIPDISABLED);

    if (locked || !visible || h != Handle && ixpos < 0)
        return;

    if (h != Handle)
        DoMouseLeave(Control);
    else
    {
        p = Control->ScreenToClient(p);
        DoMouseMove(Control, TShiftState(), p.x, p.y);
    }
}
//---------------------------------------------------------------------------
HWND TMouseOvers::get_handle()
{
    if (controlkind == ckForm)
        return control.form->Handle;
    TForm *f = (TForm*)Control->Owner;

    if (f)
        return f->Handle;
    return NULL;
}
//---------------------------------------------------------------------------
int TMouseOvers::get_pos()
{
    if (!enabled)
        return -1;
    return ixpos;
}
//---------------------------------------------------------------------------
void TMouseOvers::DoChange(TRect r, int index, int data, bool selected)
{
    if (!fchange)
        return;
    OffsetRect(&r, -fhorzscroll, -fvertscroll);
    fchange(r, index, data, selected);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(113);
#endif

