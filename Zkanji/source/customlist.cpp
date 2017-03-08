//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "customlist.h"
#include "settings.h"
#include "baseform.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(24);
#endif

//---------------------------------------------------------------------------
TCustomListObj::TCustomListObj(TZPaintBox *abox)
{
    signalcount = 0;

    fcount = 0;
    fcols = NULL;
    fcolcnt = 0;

    fbox = abox;
#ifdef _DEBUG
    stmptmp = fbox->Name + " : " + fbox->Owner->Name;
#endif

    ffixvertbar = false;
    ffixhorzbar = false;

    fbox->InvalidateOnScroll = true;
    fbox->VertScrollBar->Enabled = true;
    fbox->HorzScrollBar->Enabled = true;

    fmousecol = -1;
    fmouserow = -1;
    fheader = true;
    frowsize = 1;
    fheadersize = 1;

    fbox->TabStop = false;

    fonmousedown = abox->OnMouseDown;
    fonmouseup = abox->OnMouseUp;
    fonmousemove = abox->OnMouseMove;
    fonenter = abox->OnEnter;
    fonexit = abox->OnExit;
    fonresize = abox->OnResize;
    fonpaint = abox->OnPaint;
    fonmouseenter = abox->OnMouseEnter;
    fonmouseleave = abox->OnMouseLeave;
    fonvscroll = abox->OnVScroll;
    fonhscroll = abox->OnHScroll;
    fonkeydown = abox->OnKeyDown;
    fonkeyup = abox->OnKeyUp;
    fonkeypress = abox->OnKeyPress;

    fbox->OnExit = exitproc;
    fbox->OnEnter = enterproc;
    fbox->OnMouseMove = mousemoveproc;
    fbox->OnMouseUp = mouseupproc;
    fbox->OnMouseDown = mousedownproc;
    fbox->OnResize = resizeproc;
    fbox->OnPaint = paintproc;
    fbox->OnMouseLeave = mouseleaveproc;
    fbox->OnMouseEnter = mouseenterproc;
    fbox->OnVScroll = vscrollproc;
    fbox->OnHScroll = hscrollproc;
    fbox->OnKeyDown = keydownproc;
    fbox->OnKeyUp = keyupproc;
    fbox->OnKeyPress = keypressproc;

    ((TBaseForm*)abox->Owner)->AddEvent(this, formactivateproc, eltActivate);
    ((TBaseForm*)abox->Owner)->AddEvent(this, formdeactivateproc, eltDeactivate);

    AppActivateEvent(fbox, appactivateproc, true, true);
    AppActivateEvent(fbox, appdeactivateproc, false, true);
    SettingsChangeEvent(fbox, settingsChanged, true);

    fbox->VertScrollBar->SmallChange = 1;
    fbox->HorzScrollBar->SmallChange = 1;
}
//---------------------------------------------------------------------------
TCustomListObj::~TCustomListObj()
{
    if (fcols)
        delete[] fcols;

    SettingsChangeEvent(fbox, settingsChanged, false);
    AppActivateEvent(fbox, appdeactivateproc, false, false);
    AppActivateEvent(fbox, appactivateproc, true, false);
    ((TBaseForm*)fbox->Owner)->RemoveEvents(this);

    fbox->OnKeyPress = fonkeypress;
    fbox->OnKeyUp = fonkeyup;
    fbox->OnKeyDown = fonkeydown;
    fbox->OnHScroll = fonhscroll;
    fbox->OnVScroll = fonvscroll;
    fbox->OnMouseLeave = fonmouseleave;
    fbox->OnMouseEnter = fonmouseenter;
    fbox->OnPaint = fonpaint;
    fbox->OnResize = fonresize;
    fbox->OnMouseDown = fonmousedown;
    fbox->OnMouseUp = fonmouseup;
    fbox->OnMouseMove = fonmousemove;
    fbox->OnEnter = fonenter;
    fbox->OnExit = fonexit;
}
//---------------------------------------------------------------------------
TCustomListObj::StartUpdating::StartUpdating(TCustomListObj *aowner) : owner(aowner), used(true)
{
    owner->signalcount++;
}
//---------------------------------------------------------------------------
TCustomListObj::StartUpdating::~StartUpdating()
{
    StopUpdating();
}
//---------------------------------------------------------------------------
void TCustomListObj::StartUpdating::StopUpdating()
{
    if (!used)
        return;
    used = false;
    owner->signalcount--;

    if (!owner->signalcount)
        owner->fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TCustomListObj::set_count(int val)
{
    if (val == count)
        return;
    set_cols(fcolcnt, NULL, NULL, frowsize, fheadersize, val, fheader, false);
    measure_row_size();
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
int TCustomListObj::get_width()
{
    return fbox->ClientWidth;
}
//---------------------------------------------------------------------------
int TCustomListObj::get_height()
{
    return fbox->ClientHeight;
}
//---------------------------------------------------------------------------
TCanvas* TCustomListObj::get_canvas()
{
    return fbox->Canvas;
}
//---------------------------------------------------------------------------
void TCustomListObj::set_rowsize(int val)
{
    set_cols(fcolcnt, NULL, NULL, val, fheadersize, fcount, fheader);
}
//---------------------------------------------------------------------------
void TCustomListObj::set_headersize(int val)
{
    set_cols(fcolcnt, NULL, NULL, frowsize, val, fcount, fheader);
}
//---------------------------------------------------------------------------
int TCustomListObj::get_colsize(byte col)
{
    if (col >= fcolcnt)
        THROW(L"Column out of range!");
    return fcols[col];
}
//---------------------------------------------------------------------------
void TCustomListObj::set_colsize(byte col, int val)
{
    if (col >= fcolcnt)
        THROW(L"Column out of range!");
    val = MAX(4, val
        /**scaleres*/);
    if (val == fcols[col])
        return;

    fcols[col] = val;
    measure_row_size();
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TCustomListObj::set_colcount(byte val)
{
    set_cols(val, NULL, NULL, frowsize, fheadersize, fcount, fheader);
}
//---------------------------------------------------------------------------
void TCustomListObj::set_colvisible(byte col, bool val)
{
    if (col >= fcolcnt)
        THROW(L"Column out of range!");
    if (colvis[col] == val)
        return;
    colvis[col] = val;
    measure_row_size();
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
bool TCustomListObj::get_colvisible(byte col)
{
    if (col >= fcolcnt)
        THROW(L"Column out of range!");
    return colvis[col];
}
//---------------------------------------------------------------------------
byte TCustomListObj::get_visiblecolcount()
{
    byte result = 0;
    for (byte ix = 0; ix < fcolcnt; ++ix)
        if (colvis[ix])
            result++;

    return result;
}
//---------------------------------------------------------------------------
byte TCustomListObj::get_realcolindex(byte col)
{
    byte viscnt = visiblecolcount;

    if (col >= viscnt)
        THROW(L"Visible column out of range!");

    for (byte ix = 0; ix < fcolcnt; ++ix)
    {
        if (colvis[ix])
        {
            if (!col)
                return ix;
            else
                col--;
        }
    }

    THROW(L"Visible column out of range!");
}
//---------------------------------------------------------------------------
byte TCustomListObj::get_visiblecolindex(byte col)
{
    if (col >= fcolcnt)
        return -1;
//THROW(L"Real column out of range!";
    if (!colvisible[col])
        THROW(L"Column not visible!");

    byte result = 0;
    for (byte ix = 0; ix < col; ++ix)
    {
        if (colvis[ix])
            result++;
    }

    return (byte)result;
}
//---------------------------------------------------------------------------
void TCustomListObj::set_header(bool val)
{
    set_cols(fcolcnt, NULL, NULL, frowsize, fheadersize, fcount, val);
}
//---------------------------------------------------------------------------
void TCustomListObj::set_fixvertbar(bool val)
{
    if (val == ffixvertbar)
        return;
    ffixvertbar = val;
    measure_row_size();
}
//---------------------------------------------------------------------------
void TCustomListObj::set_fixhorzbar(bool val)
{
    if (val == ffixhorzbar)
        return;
    ffixhorzbar = val;
    measure_row_size();
}
//---------------------------------------------------------------------------
void TCustomListObj::cellpos(TPoint p, int &acol, int &arow)
{
    acol = -1;
    if (p.y < 0 || p.y >= MIN(height, frowsize * (fcount - fbox->VPos) + (fheader ? fheadersize : 0)))
        arow = -2;
    else if (fheader && p.y < fheadersize)
        arow = -1;
    else
        arow = (p.y - (fheader ? fheadersize : 0)) / frowsize + fbox->VPos;
    if (p.x < 0 || p.x >= width)
        return;

    int fsize = 0;
    byte viscnt = visiblecolcount;
    do
    {
        acol++;
        if (acol >= viscnt)
            break;
        fsize += fcols[realcolindex[acol]];
    }
    while (acol < viscnt && p.x + fbox->HPos >= fsize);

    acol = realcolindex[acol];
}
//---------------------------------------------------------------------------
TRect TCustomListObj::cellbox(int col, int row)
{
    if (col < 0 || row < (fheader ? -1 : 0) || col >= fcolcnt || row >= fcount)
        THROW(L"Cell out of range!");
    if (!colvisible[col])
        THROW(L"Column not visible!");

    byte viscnt = visiblecolcount;

    int lsize = 0;
    for (int i = 0; i < viscnt; i++)
    {
        byte ci = realcolindex[i];
        if (ci == col)
            break;
        lsize += fcols[ci];
    }

    TRect r;
    if (fheader && row == -1)
        r = Rect(lsize - fbox->HPos, 0, lsize - fbox->HPos + fcols[col], fheadersize);
    else
        r = Rect(lsize - fbox->HPos, (row - fbox->VPos) * frowsize + (fheader ? fheadersize : 0), lsize - fbox->HPos + fcols[col], (row - fbox->VPos + 1) * frowsize + (fheader ? fheadersize : 0));
    return r;
}
//---------------------------------------------------------------------------
TRect TCustomListObj::rowbox(int row)
{
    return Rect(0, (row - fbox->VPos)*frowsize + (fheader ? fheadersize : 0) - 1, width, (row - fbox->VPos + 1)*frowsize + (fheader ? fheadersize : 0));
}
//---------------------------------------------------------------------------
int TCustomListObj::on_column(int x)
{
    x += fbox->HPos;
    byte viscnt = visiblecolcount;

    int lsize = 0;
    for (int i = 0; i < viscnt; i++)
    {
        byte ci = realcolindex[i];
        lsize += fcols[ci];
        if (lsize > x)
            return ci;
    }

    return -1;
}
//---------------------------------------------------------------------------
int TCustomListObj::columnleft(byte col)
{
    if (col >= fcolcnt)
        THROW(L"Column out of range!");
    if (!colvisible[col])
        THROW(L"Column not visible!");

    byte viscnt = visiblecolcount;

    int x = 0;
    for (int i = 0; i < viscnt; i++)
    {
        byte ci = realcolindex[i];
        if (ci == col)
            break;
        x += fcols[ci];
    }

    return x;
}
//---------------------------------------------------------------------------
void TCustomListObj::measure_row_size()
{
    byte viscnt = visiblecolcount;

    int size = 0;
    for (int i = 0; i < viscnt; i++)
        size += fcols[realcolindex[i]];

    if (!ffixhorzbar || size > width)
    {
        fbox->HorzScrollBar->Enabled = true;
        fbox->HMax = size - 1;
        fbox->HorzScrollBar->ThumbSize = fbox->ClientWidth;
    }
    else if (size <= width && fbox->HorzScrollBar->Enabled)
    {
        fbox->HPos = 0;
        fbox->HMax = 999999;
        fbox->HorzScrollBar->Enabled = false;
    }

    int frowmax = ((fheader ? height - fheadersize : height) / frowsize);
    int ocnt = MAX(0, fcount - frowmax);
    if (ocnt)
    {
        fbox->VertScrollBar->Enabled = true;
        fbox->VMax = fcount - 1;
        fbox->VertScrollBar->ThumbSize = frowmax;
        fbox->VertScrollBar->LargeChange = MAX(1, frowmax - 1);
    }
    else if (!ffixvertbar)
    {
        fbox->VertScrollBar->Enabled = true;
        fbox->VMax = 0;
    }
    else
    {
        if (fbox->VertScrollBar->Enabled)
        {
            fbox->VPos = 0;
            fbox->VMax = 999999;
            fbox->VertScrollBar->Enabled = false;
        }
    }
    fvpos = fbox->VPos;
    fhpos = fbox->HPos;
}
//---------------------------------------------------------------------------
void TCustomListObj::invalidatecell(unsigned int col, int row)
{
    if ((int)col >= fcolcnt || row < (fheader ? -1 : 0) || row >= fcount)
        return;
    if (!colvisible[col])
        return;

    TRect r = cellbox(col, row);
    if (r.Right < 0 || r.Left >= width || r.Top >= height || r.Bottom < 0)
        return;
    InvalidateRect(fbox->Handle, &r, false);
}
//---------------------------------------------------------------------------
void TCustomListObj::invalidaterow(int row)
{
    if (row < (fheader ? -1 : 0) || row >= fcount)
        return;

    TRect r = rowbox(row);
    if (r.Right < 0 || r.Left >= width || r.Top >= height || r.Bottom < 0)
        return;
    InvalidateRect(fbox->Handle, &r, false);
}
//---------------------------------------------------------------------------
void TCustomListObj::invalidaterows(int startrow, int count)
{
    if (count < 0)
    {
        startrow += count + 1;
        count = -count;
    }

    if (startrow < (fheader ? -1 : 0) || startrow >= fcount)
        return;

    TRect r = Rect(0, max(fheader ? rowsize : 0, (int)rowbox(startrow).Top), fbox->ClientWidth, min(fbox->ClientHeight, (int)rowbox(startrow + count - 1).Bottom));
    if (r.Top >= height || r.Bottom < 0 || r.Top == r.Bottom)
        return;
    InvalidateRect(fbox->Handle, &r, false);
}
//---------------------------------------------------------------------------
void TCustomListObj::invalidatecol(int col)
{
    if (col >= fcolcnt || !colvisible[col] || !fcount)
        return;

    TRect r = cellbox(col, 0);
    r.Bottom = height;
    if (r.Right < 0 || r.Left >= width || r.Top >= height || r.Bottom < 0)
        return;
    InvalidateRect(fbox->Handle, &r, false);
}
//---------------------------------------------------------------------------
void TCustomListObj::set_cols(int acolumncount, int *acolwidths, bool *acolvisible, int arowheight, int aheaderheight, int acount, bool aheader, bool invalidate)
{
    if (acount == fcount && fcolcnt == acolumncount && arowheight == frowsize && aheaderheight == fheadersize && aheader == fheader && acolwidths == NULL && acolvisible == NULL)
        return;

    if (acolumncount > 255)
        THROW(L"Maximum column count is 255!");

    int* tmp = NULL;
    bool changed = false;
    bool columncountset = false;
    bool countset = false;
    int fcolcntold = fcolcnt;
    int fcountold = fcount;

    if (acolumncount > 0 && fcolcnt != acolumncount)
    {
        if (fcolcnt != acolumncount)
        {
            tmp = fcols;
            fcols = new int[acolumncount];
        }
        try
        {
            if (!acolwidths)
            { /*
        memcpy(fcols,acolwidths,sizeof(int)*acolumncount);
      else if (fcolcnt != acolumncount) {*/
                memcpy(fcols, tmp, sizeof(int)*MIN(acolumncount, fcolcnt));
                for (int ix = fcolcnt; ix < acolumncount; ++ix)
                    fcols[ix] = 6;
//if (acolumncount > fcolcnt)
//memset(fcols+fcolcnt,0,sizeof(int)*(acolumncount-fcolcnt));
            }
        }
        catch (...)
        {
            memset(fcols, 0, sizeof(int)*acolumncount);
        }

/*if (acolvisible)
      for (int ix = 0; ix < acolumncount; ++ix)
        colvis[ix] = acolvisible[ix];
*/
        if (/*fcolcnt < acolumncount &&*/ !acolvisible)
            for (int ix = min(fcolcnt, acolumncount); ix < acolumncount; ++ix)
                colvis[ix] = true;

        delete[] tmp;

        columncountset = fcolcnt != acolumncount;

        fcolcnt = acolumncount;
        changed = true;
    }

    if (fcolcnt > 0)
    {
        if (acolwidths)
        {
            memcpy(fcols, acolwidths, sizeof(int)*fcolcnt);
            changed = true;
        }
        if (acolvisible)
        {
            for (int ix = 0; ix < fcolcnt; ++ix)
                colvis[ix] = acolvisible[ix];
            changed = true;
        }
    }

    if (fcount != acount && acount >= 0)
    {
        countset = true;
        fcount = acount;
        fbox->TabStop = fcount;
        changed = true;
    }

    if (frowsize != arowheight && arowheight >= 1)
    {
        frowsize = arowheight;
        fbox->HorzScrollBar->SmallChange = frowsize;
        fbox->HorzScrollBar->LargeChange = MIN((int)(width * 0.8), width - frowsize);
        changed = true;
    }

    if (fheadersize != aheaderheight && aheaderheight >= 1)
    {
        fheadersize = aheaderheight;
        changed = true;
    }

    if (fheader != aheader)
    {
        fheader = aheader;
        changed = true;
    }

    if (columncountset)
        DoSetColumnCount(fcolcnt, fcolcntold);
    if (countset)
        DoSetCount(fcount, fcountold);

    if (changed)
    {
        measure_row_size();
        DoChanged();
        if (invalidate)
            fbox->Invalidate();
    }
}
//---------------------------------------------------------------------------
void TCustomListObj::appactivateproc(void *caller)
{
    ffocused = fbox->Focused();
    Activated();
}
//---------------------------------------------------------------------------
void TCustomListObj::appdeactivateproc(void *caller)
{
    Deactivated();
    ffocused = false;
}
//---------------------------------------------------------------------------
void TCustomListObj::formactivateproc(void *caller)
{
    ffocused = fbox->Focused();
    Activated();
}
//---------------------------------------------------------------------------
void TCustomListObj::formdeactivateproc(void *caller)
{
    Deactivated();
    ffocused = false;
}
//---------------------------------------------------------------------------
void TCustomListObj::settingsChanged(void *caller, TSettingsProcState state)
{
    DoSettingsChanged(state);
}
//---------------------------------------------------------------------------
void TCustomListObj::DoSettingsChanged(TSettingsProcState state)
{
    ;
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::exitproc(TObject *Sender)
{
    Deactivated();
    ffocused = false;

    if (fonexit)
        fonexit(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::enterproc(TObject *Sender)
{
    ffocused = true;
    Activated();

    if (fonenter)
        fonenter(Sender);
}
//---------------------------------------------------------------------------
void TCustomListObj::Activated()
{
    DoActivate(ffocused);
}
//---------------------------------------------------------------------------
void TCustomListObj::Deactivated()
{
    DoDeactivate(ffocused);
}
//---------------------------------------------------------------------------
void TCustomListObj::DoActivate(bool focused)
{
    ;
}
//---------------------------------------------------------------------------
void TCustomListObj::DoDeactivate(bool focused)
{
    ;
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::resizeproc(TObject *Sender)
{
    DoResize();

    if (fonresize)
        fonresize(Sender);
}
//---------------------------------------------------------------------------
void TCustomListObj::DoResize()
{
    measure_row_size();
    fbox->HorzScrollBar->LargeChange = MIN((int)(width * 0.8), width - frowsize);
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::mousemoveproc(TObject *Sender, TShiftState shift, int x, int y)
{
    int fcol, frow;

    cellpos(Point(x, y), fcol, frow);
    if (!shift.Contains(ssLeft))
    {
        if (fmousecol != fcol || fmouserow != frow)
            DoMouseCellChange(fmousecol, fmouserow, fcol, frow);
    }

    DoMouseMove(fcol, frow, shift, x, y);
    if (!shift.Contains(ssLeft))
    {
        fmousecol = fcol;
        fmouserow = frow;
    }

    if (fonmousemove)
        fonmousemove(Sender, shift, x, y);
}
//---------------------------------------------------------------------------
void TCustomListObj::DoMouseCellChange(int oldcol, int oldrow, int newcol, int newrow)
{
}
//---------------------------------------------------------------------------
void TCustomListObj::DoMouseMove(int acol, int arow, TShiftState shift, int x, int y)
{
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::mousedownproc(TObject *Sender, TMouseButton button, TShiftState shift, int x, int y)
{
    if (button == mbLeft || !shift.Contains(ssLeft))
    {
        int fcol, frow;
        cellpos(Point(x, y), fcol, frow);

        DoMouseDown(fcol, frow, button, shift, x, y);
        fmousecol = fcol;
        fmouserow = frow;
    }

    if (fonmousedown)
        fonmousedown(Sender, button, shift, x, y);
}
//---------------------------------------------------------------------------
void TCustomListObj::DoMouseDown(int acol, int arow, TMouseButton button, TShiftState shift, int x, int y)
{
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::mouseupproc(TObject *Sender, TMouseButton button, TShiftState shift, int x, int y)
{
    if (button == mbLeft || !shift.Contains(ssLeft))
    {
        int fcol, frow;
        cellpos(Point(x, y), fcol, frow);
        DoMouseUp(fcol, frow, button, shift, x, y);
        fmousecol = fcol;
        fmouserow = frow;
    }

    if (fonmouseup)
        fonmouseup(Sender, button, shift, x, y);
}
//---------------------------------------------------------------------------
void TCustomListObj::DoMouseUp(int acol, int arow, TMouseButton button, TShiftState shift, int x, int y)
{
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::paintproc(TObject *Sender)
{
    if (!signalcount)
        DoPaint();
}
//---------------------------------------------------------------------------
void TCustomListObj::DoPaint()
{
    int col, row = fbox->VPos;
    TRect r;

    byte viscnt = visiblecolcount;

    int left = 0;
    int lsize = fcols[0];
    while (lsize <= fbox->HPos)
        lsize += fcols[realcolindex[++left]];

    if (fheader)
    {
        for (col = left; col < viscnt; col++)
        {
            byte ci = realcolindex[col];
            r = cellbox(ci, -1);

            DoPrepareDrawHeader(ci);
            DoDrawHeader(ci, r);

            if (r.Right >= width)
                break;
        }
    }

    while (row < fcount && (!fheader || height > fheadersize))
    {
        for (col = left; col < viscnt; col++)
        {
            byte ci = realcolindex[col];
            r = cellbox(ci, row);

            DoPrepareDraw(ci, row);
            DoDraw(ci, row, r);

            if (r.Right >= width)
                break;
        }

        if (r.Bottom > height)
            break;
        row++;
    }

    TCanvas *c = Canvas;
    c->Brush->Style = bsSolid;
    c->Brush->Color = (TColor)colorsettings.background;
    c->FillRect(Rect(0, r.bottom, width, height));
    c->FillRect(Rect(r.right, 0, width, r.bottom));
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::mouseenterproc(TObject *Sender)
{
    DoMouseEnter();
}
//---------------------------------------------------------------------------
void TCustomListObj::DoMouseEnter()
{
    ;
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::mouseleaveproc(TObject *Sender)
{
    DoMouseLeave();
}
//---------------------------------------------------------------------------
void TCustomListObj::DoMouseLeave()
{
    if (fmousecol != -1 && fmouserow >= (fheader ? -1 : 0))
        DoMouseCellChange(fmousecol, fmouserow, -1, -2);

    fmousecol = -1;
    fmouserow = -1;
}
//---------------------------------------------------------------------------
TShiftState TCustomListObj::getshiftstate()
{
    TShiftState shift;
    bool sw = GetSystemMetrics(SM_SWAPBUTTON);
    if (GetAsyncKeyState(VK_CONTROL) & (1 << 15))
        shift = shift << ssCtrl;
    if (GetAsyncKeyState(VK_SHIFT) & (1 << 15))
        shift = shift << ssShift;
    if (GetAsyncKeyState(VK_MENU) & (1 << 15))
        shift = shift << ssAlt;
    if (GetAsyncKeyState(sw ? VK_RBUTTON : VK_LBUTTON) & (1 << 15))
        shift = shift << ssLeft;
    if (GetAsyncKeyState(!sw ? VK_RBUTTON : VK_LBUTTON) & (1 << 15))
        shift = shift << ssRight;
    if (GetAsyncKeyState(VK_MBUTTON) & (1 << 15))
        shift = shift << ssMiddle;

    return shift;
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::vscrollproc(TObject* Sender, TScrollCode ScrollCode, int &ScrollPos)
{
    if (fvpos == ScrollPos)
        return;
    fvpos = ScrollPos;

    int col, row;
    TPoint p;
    GetCursorPos(&p);
    p = fbox->ScreenToClient(p);

    TShiftState shift = getshiftstate();
    if (!shift.Contains(ssLeft) && !shift.Contains(ssRight))
    {
        cellpos(p, col, row);
        if (col != fmousecol || row != fmouserow)
            DoMouseCellChange(fmousecol, fmouserow, col, row);
        fmousecol = col;
        fmouserow = row;
    }

    DoVScroll(ScrollCode, ScrollPos);

    if (fonvscroll)
        fonvscroll(Sender, ScrollCode, ScrollPos);
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::hscrollproc(TObject* Sender, TScrollCode ScrollCode, int &ScrollPos)
{
    if (fhpos == ScrollPos)
        return;
    fhpos = ScrollPos;

    int col, row;
    TPoint p;
    GetCursorPos(&p);
    p = fbox->ScreenToClient(p);

    TShiftState shift = getshiftstate();
    if (!shift.Contains(ssLeft) && !shift.Contains(ssRight))
    {
        cellpos(p, col, row);
        if (col != fmousecol || row != fmouserow)
            DoMouseCellChange(fmousecol, fmouserow, col, row);
        fmousecol = col;
        fmouserow = row;
    }

    DoHScroll(ScrollCode, ScrollPos);

    if (fonhscroll)
        fonhscroll(Sender, ScrollCode, ScrollPos);
}
//---------------------------------------------------------------------------
void TCustomListObj::DoVScroll(TScrollCode ScrollCode, int &ScrollPos)
{
    TPoint p;
    GetCursorPos(&p);
    p = fbox->ScreenToClient(p);
    TShiftState shift = getshiftstate();
    if (p.x >= 0 && p.y >= 0 && p.x < fbox->ClientWidth && p.y < fbox->ClientHeight)
        mousemoveproc(fbox, shift, p.x, p.y);
}
//---------------------------------------------------------------------------
void TCustomListObj::DoHScroll(TScrollCode ScrollCode, int &ScrollPos)
{
    TPoint p;
    GetCursorPos(&p);
    p = fbox->ScreenToClient(p);
    TShiftState shift = getshiftstate();
    if (p.x >= 0 && p.y >= 0 && p.x < fbox->ClientWidth && p.y < fbox->ClientHeight)
        mousemoveproc(fbox, shift, p.x, p.y);
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::keydownproc(TObject *Sender, WORD &Key, TShiftState Shift)
{
    DoKeyDown(Key, Shift);

    if (fonkeydown)
        fonkeydown(Sender, Key, Shift);
}
//---------------------------------------------------------------------------
void TCustomListObj::DoKeyDown(WORD &Key, TShiftState Shift)
{
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::keyupproc(TObject *Sender, WORD &Key, TShiftState Shift)
{
    DoKeyUp(Key, Shift);

    if (fonkeyup)
        fonkeyup(Sender, Key, Shift);
}
//---------------------------------------------------------------------------
void TCustomListObj::DoKeyUp(WORD &Key, TShiftState Shift)
{
}
//---------------------------------------------------------------------------
void __fastcall TCustomListObj::keypressproc(TObject *Sender, wchar_t &Key)
{
    DoKeyPress(Key);

    if (fonkeypress)
        fonkeypress(Sender, Key);
}
//---------------------------------------------------------------------------
void TCustomListObj::DoKeyPress(wchar_t &Key)
{
}
//---------------------------------------------------------------------------
void TCustomListObj::DoPrepareDrawHeader(int col)
{
}
//---------------------------------------------------------------------------
void TCustomListObj::DoDrawHeader(int col, TRect r)
{
}
//---------------------------------------------------------------------------
void TCustomListObj::DoSetColumnCount(const byte &acolcount, const byte &oldcolcount)
{
}
//---------------------------------------------------------------------------
void TCustomListObj::DoSetCount(int &acount, int &oldcount)
{
}
//---------------------------------------------------------------------------
void TCustomListObj::DoChanged()
{
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(25);
#endif

