//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "customgrid.h"
#include "drawscroll.h"
#include "settings.h"
#include "baseform.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(22);
#endif

//---------------------------------------------------------------------------
TCustomGridObj::TCustomGridObj(TZPaintBox *apb, int aboxsize, TFuncPtr acallback) :
        pb(apb), fori(goVertical), fscrollsize(0), fshowbar(true), scroll(NULL), fcallback(acallback),
        fsize(aboxsize), buffcnt(0), buff(NULL), topline(0), requested(false), immediate(false),
        flen(-1), fsiz(-1), fpainting(false), objfocused(false), fmousecell(-1)
{
    pb->AutoSizeScrollbars = false;

    pb->InvalidateOnScroll = true;
    pb->TabStop = false;

    pb->HorzScrollBar->Enabled = true;
    pb->HorzScrollBar->Range = 99999;
    pb->HorzScrollBar->Position = 0;
    pb->HorzScrollBar->Visible = false;

    pb->VertScrollBar->Visible = true;
    pb->VertScrollBar->Enabled = true;
    pb->VertScrollBar->Range = 99999;
    pb->VertScrollBar->Position = 0;

    pb->VertScrollBar->Enabled = false;

    fonvscroll = pb->OnVScroll;
    fonhscroll = pb->OnHScroll;
    fonmouseenter = pb->OnMouseEnter;
    fonmouseleave = pb->OnMouseLeave;
    fonmousemove = pb->OnMouseMove;
    fonresize = pb->OnResize;
    fonmousedown = pb->OnMouseDown;
    fonmouseup = pb->OnMouseUp;
    fonkeydown = pb->OnKeyDown;
    fonkeypress = pb->OnKeyPress;
    fonenter = pb->OnEnter;
    fonexit = pb->OnExit;

    pb->OnVScroll = pbVScroll;
    pb->OnHScroll = pbHScroll;
    pb->OnPaint = pbPaint;
    pb->OnResize = pbResize;
    pb->OnMouseDown = pbMouseDown;
    pb->OnMouseUp = pbMouseUp;
    pb->OnKeyDown = pbKeyDown;
    pb->OnKeyPress = pbKeyPress;
    pb->OnEnter = pbEnter;
    pb->OnExit = pbExit;
    pb->OnMouseMove = pbMouseMove;
    pb->OnMouseEnter = pbMouseEnter;
    pb->OnMouseLeave = pbMouseLeave;

    ((TBaseForm*)pb->Owner)->AddEvent(this, formActivate, eltActivate);
    ((TBaseForm*)pb->Owner)->AddEvent(this, formDeactivate, eltDeactivate);

    pbscrollfunc = pbVScroll;

    AppActivateEvent(pb, appActivate, true, true);
    AppActivateEvent(pb, appDeactivate, false, true);
}
//---------------------------------------------------------------------------
TCustomGridObj::~TCustomGridObj()
{
    clear_buffer();
    buffcnt = 0;
    delete[]buff;

    pb->OnResize = fonresize;
    pb->OnMouseUp = fonmouseup;
    pb->OnMouseDown = fonmousedown;
    pb->OnKeyDown = fonkeydown;
    pb->OnKeyPress = fonkeypress;
    pb->OnEnter = fonenter;
    pb->OnExit = fonexit;

    ((TBaseForm*)pb->Owner)->RemoveEvents(this);

    pb->OnMouseMove = fonmousemove;
    pb->OnMouseLeave = fonmouseleave;
    pb->OnMouseEnter = fonmouseenter;
    pb->OnHScroll = fonhscroll;
    pb->OnVScroll = fonvscroll;

    AppActivateEvent(pb, appActivate, true, false);
    AppActivateEvent(pb, appDeactivate, false, false);

    delete scroll;
}
//---------------------------------------------------------------------------
void TCustomGridObj::set_orientation(TGridOrientation val)
{
    if (fori == val)
        return;

    DoScroll(0);
    clear_buffer();

    if (fshowbar && fscrollsize)
        scroll->Orientation = val == goHorizontal ? dsoHorizontal : dsoVertical;
    else if (fshowbar)
    {
        Bar->Enabled = true;
        Bar->Position = 0;
        Bar->Range = 99999;
        Bar->Visible = false;
        if (topline)
            DoScroll(topline);
    }

    fori = val;

    if (!fscrollsize && fshowbar)
    {
        Bar->Visible = true;
        Bar->Enabled = true;
        Bar->Position = 0;
        Bar->Range = 99999;
        Bar->Enabled = false;
        if (topline)
            DoScroll(topline);
    }
    else if (!fshowbar)
        topline = 0;

    if (val == goHorizontal)
        pbscrollfunc = pbHScroll;
    else
        pbscrollfunc = pbVScroll;

    measure_box_size();
    pb->Invalidate();
}
//---------------------------------------------------------------------------
void TCustomGridObj::set_scrollsize(int ascrollsize)
{
    ascrollsize = MAX(ascrollsize, 4);
    if (fscrollsize == ascrollsize)
        return;
    if (!fshowbar)
    {
        fscrollsize = ascrollsize;
        return;
    }

    if (ascrollsize)
    {
        scroll = new TDrawScroll(pb, ascrollsize, scallback);
        scroll->Orientation = fori == goHorizontal ? dsoHorizontal : dsoVertical;
        scroll->UnitSize = fsize;

        Bar->Enabled = true;
        Bar->Position = 0;
        Bar->Range = 9999;
        Bar->Visible = false;
        fscrollsize = ascrollsize;
    }
    else
    {
        fscrollsize = 0;

        delete scroll;
        scroll = 0;

        Bar->Visible = true;
        Bar->Position = 0;
        Bar->Range = 9999;
        Bar->Enabled = false;
        if (topline && fshowbar)
            DoScroll(topline);
    }

    measure_box_size();
    pb->Invalidate();
}
//---------------------------------------------------------------------------
void TCustomGridObj::set_scrollbars(bool val)
{
    if (fshowbar == val)
        return;
    fshowbar = val;
    if (!fshowbar)
    {
        Bar->Enabled = true;
        if (fscrollsize)
        {
            delete scroll;
            scroll = 0;
        }
        else
            Bar->Visible = false;
        topline = 0;
    }
    else
    {
        if (fscrollsize)
        {
            scroll = new TDrawScroll(pb, fscrollsize, scallback);
            scroll->Orientation = fori == goHorizontal ? dsoHorizontal : dsoVertical;
            scroll->UnitSize = fsize;
        }
        else
        {
            Bar->Visible = true;
            Bar->Position = 0;
            Bar->Range = 9999;
            Bar->Enabled = false;
            if (topline)
                DoScroll(topline);
        }
    }

    measure_box_size();
}
//---------------------------------------------------------------------------
int TCustomGridObj::get_width()
{
    if (!fscrollsize || !fshowbar)
        return pb->ClientWidth;
    return scroll->ClientWidth;
}
//---------------------------------------------------------------------------
int TCustomGridObj::get_height()
{
    if (!fscrollsize || !fshowbar)
        return pb->ClientHeight;
    return scroll->ClientHeight;
}
//---------------------------------------------------------------------------
TRect TCustomGridObj::get_rect()
{
    return Rect(0, 0, ClientWidth, ClientHeight);
}
//---------------------------------------------------------------------------
int TCustomGridObj::get_clen()
{
    return fori == goVertical ? ClientHeight : ClientWidth;
}
//---------------------------------------------------------------------------
int TCustomGridObj::get_csiz()
{
    return fori == goVertical ? ClientWidth : ClientHeight;
}
//---------------------------------------------------------------------------
TZControlScrollBar* TCustomGridObj::get_bar()
{
    return fori == goVertical ? pb->VertScrollBar : pb->HorzScrollBar;
}
//---------------------------------------------------------------------------
void TCustomGridObj::set_mousecell(int ix)
{
    int fold = fmousecell;
    fmousecell = ix;
    if (fold != fmousecell)
    {
        InvalidateCell(fold);
        InvalidateCell(fmousecell);
    }
}
//---------------------------------------------------------------------------
int TCustomGridObj::get_scrollpos()
{
    if (!fshowbar)
        return topline;
    if (!fscrollsize)
        return Bar->Position;
    return scroll->Position;
}
//---------------------------------------------------------------------------
void TCustomGridObj::set_scrollpos(int val)
{
    if (!fshowbar)
    {
        int ix = MAX(0, MIN(val, count - Rows));
        if (ix != topline)
            DoScroll(ix);
    }
    else if (!fscrollsize)
    {
        Bar->Position = val;
        if (topline != val)
            DoScroll(val);
    }
    else
    {
        scroll->Position = val;
    }
}
//---------------------------------------------------------------------------
int TCustomGridObj::get_scrollrange()
{
    if (!fscrollsize || !fshowbar)
        return Bar->Range;
    return scroll->Range;
}
//---------------------------------------------------------------------------
void TCustomGridObj::set_scrollrange(int val)
{
    if (!fscrollsize || !fshowbar)
        Bar->Range = val;
    else
        scroll->Range = val;
}
//---------------------------------------------------------------------------
void TCustomGridObj::measure_box_size()
{
    int osiz = fsiz;

    fsiz = MAX(1, clientsiz / fsize);
    flen = MAX(1, clientlen / fsize);

    int n = ((count + fsiz - 1) / fsiz);

    if (osiz != fsiz && buff)
    {
        if (fshowbar)
            topline = ScrollPos;
        else
            topline = MAX(0, MIN(topline, n - flen));

        clear_buffer();

        delete[]buff;
        buffcnt = 0;
        buff = NULL;
    }

    if (!buffcnt || buffcnt != flen + ((clientlen % fsize) && clientlen > fsize ? 1 : 0))
    {
        TBmp **tmp = buff;
        int r = flen + ((clientlen % fsize) && clientlen > fsize ? 1 : 0);
        if (r)
        {
            buff = new TBmp *[r];
            memset(buff, 0, sizeof(void*)*r);
            if (tmp)
                memcpy(buff, tmp, MIN(r, buffcnt)*(int)sizeof(void*));
        }
        else
            buff = NULL;

        if (tmp && r < buffcnt)
            for (int i = r; i < buffcnt; i++)
                delete tmp[i];
        delete[]tmp;
        buffcnt = r;
    }

    if (!fscrollsize && fshowbar)
        Bar->Enabled = (topline || n > flen || Bar->Range < clientlen + 1);

    if (n > flen)
    {
        int k = MIN(topline, n - flen);
        if (!fscrollsize || !fshowbar)
        {
            Bar->Range = n - 1;
            Bar->ThumbSize = flen;
            Bar->LargeChange = MAX(1, flen - 1);
            if (topline != Bar->Position && fshowbar)
            { //set topline when vpos changes
                Bar->Position = k;
                if (topline != k)
                    DoScroll(k);
            }
            else if (!fshowbar)
                topline = k;
        }
        else
        {
            scroll->Range = clientlen + (n - flen);
            scroll->LargeChange = MAX(1, flen - 1);
            if (k != scroll->Position)//set topline when vpos changes
                        scroll->Position = k;
        }
    }
    else
    {
        if (!fscrollsize || !fshowbar)
        {
            Bar->Position = 0;
            Bar->Range = 9999;
        }
        else
        {
            scroll->Position = 0;
            scroll->Range = 0;
        }
        if (topline)
            DoScroll(0);
        topline = 0;

        if (!fscrollsize && fshowbar)
            Bar->Enabled = false;
    }

}
//---------------------------------------------------------------------------
void TCustomGridObj::clear_buffer()
{
    for (int i = 0; i < buffcnt; i++)
        if (buff[i])
        {
            delete buff[i];
            buff[i] = 0;
        }
}
//---------------------------------------------------------------------------
void __fastcall TCustomGridObj::pbVScroll(TObject* Sender, TScrollCode ScrollCode, int &ScrollPos)
{
    if (fori == goVertical && ScrollPos != topline)
        DoScroll(ScrollPos);
    else if (fonvscroll)
        fonvscroll(Sender, ScrollCode, ScrollPos);
}
//---------------------------------------------------------------------------
void __fastcall TCustomGridObj::pbHScroll(TObject* Sender, TScrollCode ScrollCode, int &ScrollPos)
{
    if (fori == goHorizontal && ScrollPos != topline)
        DoScroll(ScrollPos);
    else if (fonhscroll)
        fonhscroll(Sender, ScrollCode, ScrollPos);
}
//---------------------------------------------------------------------------
void TCustomGridObj::scallback()
{
    if (topline != scroll->Position)
        DoScroll(scroll->Position);

    int sc = ScrollPos;
    if (fori == goVertical && fonvscroll)
        fonvscroll(pb, scPosition, sc);
    if (fonhscroll && fonhscroll)
        fonhscroll(pb, scPosition, sc);
    if (sc != scroll->Position)
        scroll->Position = sc;

    pb->Invalidate();
}
//---------------------------------------------------------------------------
void TCustomGridObj::DoScroll(int apos)
{
    for (int i = 0; i < buffcnt; i++)
        if (topline + i < apos || topline + i >= apos + buffcnt && buff[i])
        {
            delete buff[i];
            buff[i] = 0;
        }

    if (abs(topline - apos) < buffcnt)
    {
        memmove(buff + (topline < apos ? 0 : topline - apos), buff + (topline < apos ? apos - topline : 0), sizeof(void*)*(buffcnt - abs(topline - apos)));
        for (int i = 0; i < abs(topline - apos); i++)
            buff[i + (topline < apos ? buffcnt - abs(topline - apos) : 0)] = 0;
    }
    topline = apos;
    if (!GetCapture())
    {
        TPoint p;
        GetCursorPos(&p);
        p = pb->ScreenToClient(p);
        TRect r = ClientRect;
        if (PtInRect(r, p))
            mousecell = CellAtPos(p);
        else
            mousecell = -1;
    }

    if (scrollbars && !fscrollsize)
    {
        int t = ScrollPos;
        pbscrollfunc(pb, scPosition, t);
    }
}
//---------------------------------------------------------------------------
void TCustomGridObj::invalidate_cell(int ix)
{
    TRect r;
    r.left = (ix % fsiz) * fsize - 1;
    r.top = (ix / fsiz - ScrollPos) * fsize - 1;
    if (fori == goHorizontal)
        memswap(r.left, r.top);

    r.right = r.left + fsize + 1;
    r.bottom = r.top + fsize + 1;
    InvalidateRect(pb->Handle, &r, false);
}
//---------------------------------------------------------------------------
void TCustomGridObj::InvalidateCell(int ix)
{
    if (drawcell(ix))
        invalidate_cell(ix);
}
//---------------------------------------------------------------------------
void TCustomGridObj::Redraw()
{
    clear_buffer();
    pb->Invalidate();
}
//---------------------------------------------------------------------------
bool TCustomGridObj::drawcell(int ix)
{
    int b = (ix / fsiz) - topline;
    if (ix < 0 || b < 0 || b >= buffcnt || !buff[b])
        return false;

    TCanvas *c = buff[b]->Canvas;

    int j = ix % fsiz;
    TRect r = Rect(j * fsize, 0, (j + 1) * fsize - 1, fsize - 1);
    if (fori == goHorizontal)
    {
        memswap(r.left, r.top);
        memswap(r.right, r.bottom);
    }

    c->Font->Assign(pb->Font);

    DoDrawCell(c, r, ix);

    return true;
}
//---------------------------------------------------------------------------
int TCustomGridObj::CellAtPos(TPoint p)
{
    TRect r = pb->ClientRect;
    if (!PtInRect(&r, p))
        return -1;

    int a = p.x, b = p.y;

    if (fori == goHorizontal)
        memswap(a, b);

    if (a >= fsiz * fsize)
        return -1;

    int ix = (ScrollPos + b / fsize) * fsiz + a / fsize;
    if (ix < 0 || ix >= count)
        return -1;
    return ix;
}
//---------------------------------------------------------------------------
void __fastcall TCustomGridObj::pbPaint(TObject *Sender)
{
    if (painting)
        return;

    fpainting = true;

    try
    {
        TCanvas *c = pb->Canvas;

        DoBeginPaint();

        int i = 0;

        for (; i < buffcnt; i++)
        {
            int ix = (i + topline) * fsiz;

            bool check = false;

            if (buff[i])
                for (int j = 0; j < fsiz && ix + j < count && !check; j++)
                    check = DoNeedRedraw(ix + j);

            if (!buff[i] || check)
            {

                if (!check)
                {
                    buff[i] = new TBmp;
                    if (fori == goVertical)
                        buff[i]->SetSize(fsiz*fsize, fsize);
                    else
                        buff[i]->SetSize(fsize, fsiz*fsize);
                    if (ix + fsiz >= count)
                    {
                        buff[i]->Canvas->Brush->Color = (TColor)colorsettings.background;
                        buff[i]->Canvas->FillRect(Rect(0, 0, buff[i]->Width, buff[i]->Height));
                    }
                }

                for (int j = 0; j < fsiz && ix + j < count; j++)
                {
                    if (check && !DoNeedRedraw(ix + j))
                        continue;

                    drawcell(ix + j);
                }
            }

            if (fori == goVertical)
                c->Draw(0, i*fsize, buff[i]);
            else
                c->Draw(i*fsize, 0, buff[i]);

            if (ix >= count)
                break;
        }

        c->Brush->Color = (TColor)colorsettings.background; //clWhite;

        if (fori == goVertical)
        {
            c->FillRect(Rect(fsize*fsiz, 0, ClientWidth, ClientHeight));
            c->FillRect(Rect(0, i*fsize, ClientWidth, ClientHeight));
        }
        else
        {
            c->FillRect(Rect(0, fsize*fsiz, ClientWidth, ClientHeight));
            c->FillRect(Rect(i*fsize, 0, ClientWidth, ClientHeight));
        }

        DoEndPaint();

    }
    catch (...)
    {
        int err = GetLastError();
        MsgBox(((UnicodeString)L"Error in painting list! (code : " + err + ")").c_str(), L"Error!", MB_OK);
    }
    fpainting = false;

    DoCallback();
}
//---------------------------------------------------------------------------
void TCustomGridObj::changed(int newpos)
{
    clear_buffer();
    measure_box_size();
    DoChanged(newpos);
    CallbackNow();
    pb->Invalidate();
}
//---------------------------------------------------------------------------
void TCustomGridObj::RequestCallback()
{
    if (!fcallback)
        return;
    requested = true;
    if (immediate)
        DoCallback();
}
//---------------------------------------------------------------------------
void TCustomGridObj::CallbackNow()
{
    immediate = true;
    RequestCallback();
}
//---------------------------------------------------------------------------
void TCustomGridObj::DoCallback()
{
    static bool called = false;
    immediate = false;
    if (!requested || !fcallback || called)
        return;
    called = true;
    try
    {
        fcallback();
    }
    catch (...)
    {
        ;
    }
    called = false;
    requested = false;
}
//---------------------------------------------------------------------------
void TCustomGridObj::formActivate(void *caller)
{
    objfocused = pb->Focused();
    Activated();
}
//---------------------------------------------------------------------------
void TCustomGridObj::formDeactivate(void *caller)
{
    Deactivated();
    objfocused = false;
}
//---------------------------------------------------------------------------
void TCustomGridObj::appActivate(void *caller)
{
    objfocused = pb->Focused();
    Activated();
}
//---------------------------------------------------------------------------
void TCustomGridObj::appDeactivate(void *caller)
{
    Deactivated();
    objfocused = false;
}
//---------------------------------------------------------------------------
void __fastcall TCustomGridObj::pbEnter(TObject *Sender)
{
    objfocused = true;
    Activated();

    if (fonenter)
        fonenter(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TCustomGridObj::pbExit(TObject *Sender)
{
    Deactivated();

    objfocused = false;
    if (fonexit)
        fonexit(Sender);
}
//---------------------------------------------------------------------------
void TCustomGridObj::Deactivated()
{
    immediate = true;
    if (objfocused)
        DoDeactivate();
}
//---------------------------------------------------------------------------
void TCustomGridObj::Activated()
{
    immediate = true;
    if (objfocused)
        DoActivate();
}
//---------------------------------------------------------------------------
void TCustomGridObj::DoActivate()
{
    pb->Invalidate();
}
//---------------------------------------------------------------------------
void TCustomGridObj::DoDeactivate()
{
    pb->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TCustomGridObj::pbResize(TObject *Sender)
{
    measure_box_size();

    DoResize();

    if (fonresize)
        fonresize(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TCustomGridObj::pbMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    DoMouseMove(Shift, x, y);

    if (GetCapture())
        mousecell = -1;
    else
        mousecell = CellAtPos(Point(x, y));

    if (fonmousemove)
        fonmousemove(Sender, Shift, x, y);
}
//---------------------------------------------------------------------------
void TCustomGridObj::DoMouseMove(TShiftState Shift, int x, int y)
{
}
//---------------------------------------------------------------------------
void __fastcall TCustomGridObj::pbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    DoMouseUp(Button, Shift, x, y);

    if (fonmouseup)
        fonmouseup(Sender, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void TCustomGridObj::DoMouseUp(TMouseButton Button, TShiftState Shift, int x, int y)
{
}
//---------------------------------------------------------------------------
void __fastcall TCustomGridObj::pbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    DoMouseDown(Button, Shift, x, y);

    if (fonmousedown)
        fonmousedown(Sender, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void TCustomGridObj::DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y)
{
    mousecell = -1;
}
//---------------------------------------------------------------------------
void __fastcall TCustomGridObj::pbMouseEnter(TObject *Sender)
{
    DoMouseEnter();

    if (fonmouseenter)
        fonmouseenter(Sender);
}
//---------------------------------------------------------------------------
void TCustomGridObj::DoMouseEnter()
{
    ;
}
//---------------------------------------------------------------------------
void __fastcall TCustomGridObj::pbMouseLeave(TObject *Sender)
{
    DoMouseLeave();

    if (fonmouseleave)
        fonmouseleave(Sender);
}
//---------------------------------------------------------------------------
void TCustomGridObj::DoMouseLeave()
{
    mousecell = -1;
}
//---------------------------------------------------------------------------
void __fastcall TCustomGridObj::pbKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    DoKeyDown(Key, Shift);

    if (fonkeydown)
        fonkeydown(Sender, Key, Shift);
}
//---------------------------------------------------------------------------
void __fastcall TCustomGridObj::pbKeyPress(TObject *Sender, wchar_t &Key)
{
    DoKeyPress(Key);

    if (fonkeypress)
        fonkeypress(Sender, Key);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
TCustomMultiselectGridObj::TCustomMultiselectGridObj(TZPaintBox *apb, int aboxsize, bool amultiselect, TFuncPtr acallback) :
        fpos(-1), fmulti(amultiselect), pfocused(false), inherited(apb, aboxsize, acallback)
{
    fsellist = new TGapList<bool>(false, 200);
    psellist = new TGapList<bool>(false, 200);
}
//---------------------------------------------------------------------------
TCustomMultiselectGridObj::~TCustomMultiselectGridObj()
{
    delete fsellist;
    delete psellist;
}
//---------------------------------------------------------------------------
void TCustomMultiselectGridObj::set_pos(int ix)
{
    set_pos2(ix, fsellist->TrueCount);
}
//---------------------------------------------------------------------------
void TCustomMultiselectGridObj::set_pos2(int i, bool resetsel)
{
    if (i < 0 || i >= count || painting)
        return;

    DoChangePos(i, resetsel);

    bool changesel = resetsel && fsellist->TrueCount > 1;

    TRect r;
    int foldpos = fpos;
    fpos = i;

    if (changesel || fpos != foldpos && fsellist->TrueCount == 1 || fsellist->TrueCount == 0)
    {
        fsellist->Clear();
        fsellist->Items[i] = true;
    }

    if (!ScrollInView(fpos, foldpos) && (fpos != foldpos || changesel))
    {
        if (changesel)
            pb->Invalidate();
        else
        {
            invalidate_cell(foldpos);
            invalidate_cell(fpos);
        }
    }

    if (foldpos != fpos || changesel)
        RequestCallback();
}
//---------------------------------------------------------------------------
bool TCustomMultiselectGridObj::get_selected(int ix)
{
    if (!count || ix < 0 || ix >= count)
        return false;

    return fsellist->Items[ix];
}
//---------------------------------------------------------------------------
void TCustomMultiselectGridObj::set_selected(int ix, bool val)
{
    if (!count || ix < 0 || ix >= count || fsellist->Items[ix] == val)
        return;

    fsellist->Items[ix] = val;
    InvalidateCell(ix);
}
//---------------------------------------------------------------------------
int TCustomMultiselectGridObj::get_selcount()
{
    return fsellist->TrueCount;
}
//---------------------------------------------------------------------------
unsigned int TCustomMultiselectGridObj::get_selpos(unsigned int ix)
{
    if (ix >= (fsellist->TrueCount ? fsellist->TrueCount : fpos >= 0 ? 1 : 0))
        THROW(L"Bad selection position index!");

    return fsellist->TruePos[ix];
}
//---------------------------------------------------------------------------
bool TCustomMultiselectGridObj::get_simplesel()
{
    return (fsellist->PosCount <= 1);
}
//---------------------------------------------------------------------------
bool TCustomMultiselectGridObj::get_hasselected()
{
    return fsellist->TrueCount > 1;
}
//---------------------------------------------------------------------------
void TCustomMultiselectGridObj::set_multiselect(bool val)
{
    if (fmulti == val)
        return;
    fmulti = val;

    if (!val && fsellist->TrueCount > 1)
        set_pos2(pos, true);

}
//---------------------------------------------------------------------------
bool TCustomMultiselectGridObj::ScrollInView(int apos, int aoldpos)
{
    bool moved = false;

    //int newpos;
    if (apos / Columns > Rows + ScrollPos - 1)
    {
        moved = true;
        if (aoldpos < apos)
            ScrollPos = apos / Columns - Rows + 1;
        else
            ScrollPos = apos / Columns - Rows / 2 + 1;
    }
    else if (apos / Columns < ScrollPos)
    {
        moved = true;
        if (aoldpos > apos)
            ScrollPos = apos / Columns;
        else
            ScrollPos = apos / Columns - Rows / 2;
    }
    return moved;
}
//---------------------------------------------------------------------------
void TCustomMultiselectGridObj::DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y)
{
    int acell;
    if ((acell = CellAtPos(Point(x, y))) < 0 || Button == mbMiddle)
        return;

    if (fmulti && Button == mbLeft && (Shift.Contains(ssShift) || Shift.Contains(ssCtrl)))
    {
        if (Shift.Contains(ssShift))
        {
            fsellist->Clear();
            fsellist->Items[acell] = true;

            if (acell != fpos)
                for (int i = MIN(fpos, acell); i <= MAX(fpos, acell); i++)
                    fsellist->Items[i] = true;

            ScrollInView(acell, fpos);
            RequestCallback();
            pb->Invalidate();
        }
        else
        {
            if (fsellist->Items[acell])
            {
                if (fsellist->TrueCount > 1)
                {
                    fsellist->ClearIndex(acell);
                    if (fpos == acell)
                        set_pos2(fsellist->TruePos[0], false);
                    else
                    {
                        InvalidateCell(acell);
                        RequestCallback();
                    }
                }
            }
            else
            {
                fsellist->Items[acell] = true;
                set_pos2(acell, false);
            }
        }
    }
    else if (Button == mbLeft || !fsellist->Items[acell])
        pos = acell;
    else
        set_pos2(acell, false);
}
//---------------------------------------------------------------------------
void TCustomMultiselectGridObj::DoResize()
{
    if (pos < 0)
        return;

    if (pos / Columns > Rows + ScrollPos - 1)
        ScrollPos += (pos / Columns) - (Rows + ScrollPos - 1) + Rows / 2;
    else if (pos / Columns < ScrollPos)
        ScrollPos -= (ScrollPos) - (pos / Columns) + Rows / 2;
    else
        pb->Invalidate();
}
//---------------------------------------------------------------------------
void TCustomMultiselectGridObj::DoKeyDown(Word &Key, TShiftState Shift)
{
    switch (Key)
    {
    case 'A':
        if (!fmulti || !Shift.Contains(ssCtrl))
            break;
        fsellist->SetAll(count, true);
        set_pos2(0, false);
        RequestCallback();
        pb->Invalidate();
        break;
    case 'D':
        if (!fmulti || !Shift.Contains(ssCtrl) || fsellist->TrueCount <= 1)
            break;
        fsellist->Clear();
        set_pos2(pos, true);
        RequestCallback();
        pb->Invalidate();
        break;
    case VK_LEFT:
        if (orientation == goVertical)
            pos = MAX(0, fpos - 1);
        else if (fpos - Columns >= 0)
            pos = fpos - Columns;
        break;
    case VK_UP:
        if (orientation == goHorizontal)
        {
            if (fpos % Columns)
                pos = MAX(0, fpos - 1);
        }
        else if (fpos - Columns >= 0)
            pos = fpos - Columns;
        break;
    case VK_RIGHT:
        if (orientation == goVertical)
            pos = MIN(count - 1, fpos + 1);
        else if (fpos + Columns < count)
            pos = fpos + Columns;
        break;
    case VK_DOWN:
        if (orientation == goHorizontal)
        {
            if ((fpos + 1) % Columns)
                pos = MIN(count - 1, fpos + 1);
        }
        else if (fpos + Columns < count)
            pos = fpos + Columns;
        break;
    case VK_PRIOR:
        if (orientation == goVertical)
            pos = MAX(fpos % Columns, fpos - Columns * (Rows - 1) - Columns);
        else
        {
            if (Shift.Contains(ssCtrl) || Columns == 1)
                pos = 0;
            else
                pos = fpos - fpos % Columns;
        }
        break;
    case VK_NEXT:
        if (orientation == goVertical)
            pos = MIN(count - 1, fpos + Columns * (Rows - 1) + Columns);
        else
        {
            if (Shift.Contains(ssCtrl) || Columns == 1)
                pos = count - 1;
            else
                pos = MIN(count - 1, fpos + Columns - (fpos % Columns) - 1);
        }
        break;
    case VK_HOME:
        if (orientation == goVertical)
        {
            if (Shift.Contains(ssCtrl) || Columns == 1)
                pos = 0;
            else
                pos = fpos - fpos % Columns;
        }
        else
            pos = MAX(fpos % Columns, fpos - Columns * (Rows - 1) - Columns);
        break;
    case VK_END:
        if (orientation == goVertical)
        {
            if (Shift.Contains(ssCtrl) || Columns == 1)
                pos = count - 1;
            else
                pos = MIN(count - 1, fpos + Columns - (fpos % Columns) - 1);
        }
        else
            pos = MIN(count - 1, fpos + Columns * (Rows - 1) + Columns);
        break;
    }
}
//---------------------------------------------------------------------------
bool TCustomMultiselectGridObj::DoNeedRedraw(int ix)
{
    return psellist->Items[ix] != fsellist->Items[ix] || (psellist->Items[ix] || fsellist->Items[ix]) && pfocused != focused;
}
//---------------------------------------------------------------------------
void TCustomMultiselectGridObj::DoEndPaint()
{
    psellist->Assign(fsellist);
    pfocused = focused;
}
//---------------------------------------------------------------------------
void TCustomMultiselectGridObj::DoChanged(int newpos)
{
    inherited::DoChanged(newpos);

    pb->TabStop = count;

    fpos = MIN(MAX(0, newpos), count - 1);

    fsellist->Clear();
    if (fpos >= 0)
        fsellist->Items[fpos] = true;
}
//---------------------------------------------------------------------------
void TCustomMultiselectGridObj::DoChanged(int newpos, int newsellength)
{
    DoChanged(newpos);
    if (fpos >= 0 && count)
        for (int j = 1; j < newsellength; ++j)
            fsellist->Items[fpos + j] = true;
}
//---------------------------------------------------------------------------
void TCustomMultiselectGridObj::update_selected()
{
    psellist->Clear();
    pfocused = -1;
}
//---------------------------------------------------------------------------
void TCustomMultiselectGridObj::changed(int newpos)
{
    inherited::changed(newpos);
}
//---------------------------------------------------------------------------
void TCustomMultiselectGridObj::changed(int newpos, int newsellength)
{
    clear_buffer();
    measure_box_size();
    DoChanged(newpos, newsellength);
    CallbackNow();
    pb->Invalidate();
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(23);
#endif

