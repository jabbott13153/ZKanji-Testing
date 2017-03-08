//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "docker.h"
#include "kanjilist.h"
#include "wordlist.h"
#include "grouplist.h"
#include "settings.h"
#include "module.h"
#include "main.h"
#include "collection.h"
#include "gengraph.h"
#include "kanjiinfo.h"
#include "clipview.h"
#include "popupdict.h"
#include "exportform.h"
#include "dictsel.h"
#include "import.h"
#include "sentences.h"
#include "zkformats.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(26);
#endif

TfDock *fDock;

extern TFormSizeInfo *fsi;
extern bool hasexamples;

#ifdef _DEBUG
static TGlobalTest tester2(27);
#endif

//---------------------------------------------------------------------------
void TfDock::WMSysCommand(TWMSysCommand &msg)
{
    msg.Result = 0;

    if ((msg.CmdType & 0xFFF0) == SC_MINIMIZE || (msg.CmdType & 0xFFF0) == SC_ICON)
    {
        fMain->Minimize();
        if (settings.minimizetotray)
            return;
    }
    else if (settings.intray && !settings.minimizetotray && (msg.CmdType & 0xFFF0) == SC_MAXIMIZE)
    {
        data->Restore();
        return;
    }

    inherited::Dispatch(&msg);
}
//---------------------------------------------------------------------------
__fastcall TfDock::TfDock(TComponent* Owner) : action(daNone), fstate(dsNone), oldresize(NULL), fdivsize(4), hrate(0), wpos(0), hdiv(0), activectrl(NULL), TBaseForm(Owner)
{
    margins = Rect(0, 0, 0, 0);
    EraseBackground = false;
    Constrain(782 * scaleres + (Width - ClientWidth), 496 * scaleres + (Height - ClientHeight));
    moves = new TMenuMoveList;

    KeepSize = true;

    AppWindow = true;
}
//---------------------------------------------------------------------------
__fastcall TfDock::~TfDock()
{
    delete moves;
}
//---------------------------------------------------------------------------
void __fastcall TfDock::FormResize(TObject *Sender)
{
    Resized(false);
}
//---------------------------------------------------------------------------
void TfDock::Resized(bool manual, bool forced)
{
    if (fstate != dsDocked && (fstate != dsDocking || !forced))
        return;

    TRect area = Rect(margins.Left, margins.Top, ClientWidth - margins.Right, ClientHeight - margins.Bottom);

    HDWP dp = BeginDeferWindowPos(3);

    if (manual)
        hrate = (double)hdiv / area.Height();
    if (hdiv == 0 || hdiv == -1)
        hrate = hdiv - 1;
    else if (hrate != 0 || hdiv == 0)
        hdiv = area.Height() * hrate;

    int nwpos = wdiv;
    int nhpos = hdiv;

    if (nwpos == 0)
        nwpos = fdivsize;
    else if (nwpos == -1)
        nwpos = area.Width() - fdivsize * 2;
    else
    {
        int dif = GetSystemMetrics(SM_CXVSCROLL);
        nwpos = max(5 * fKanji->boxsize, ((nwpos - dif + (fKanji->boxsize / 2)) / fKanji->boxsize) * fKanji->boxsize) + dif;
        while (area.Width() - nwpos - fdivsize < 511 * scaleres)
            nwpos -= fKanji->boxsize;
    }

    if (nhpos == 0)
        nhpos = fdivsize;
    else if (nhpos == -1)
        nhpos = area.Height() - fdivsize * 2;
    else
        nhpos = min(max(nhpos, 337 * scaleres), area.Height() - 155 * scaleres - fdivsize);

    if (manual)
    {
        wdiv = wdiv != 0 && wdiv != -1 ? nwpos : wdiv;
        hdiv = hdiv != 0 && hdiv != -1 ? nhpos : hdiv;
    }

    if (!forced && (nwpos == wpos && (fKanji->Visible == (wdiv != 0)) && ((fKanji->Visible ? fKanji->Width : 0) + (wdiv == 0 || wdiv == -1 ? fdivsize * 2 : fdivsize) + fGroup->Width == area.Width()) && (fGroup->Visible == (hdiv != 0)) && (nhpos == hpos && (fGroup->Visible ? fGroup->Height : 0) + (hdiv == 0 || hdiv == -1 ? fdivsize * 2 : fdivsize) + fDict->Height == area.Height())))
        return;

    wpos = nwpos;
    hpos = nhpos;

    int oldw = fKanji->Width;

    bool kshown;
    bool gshown;
    bool dshown;

    if (flayout == dlUndecided)
        flayout = Screen->Width > 700 ? dlExpandKanji : dlExpandDict;
    if (flayout == dlExpandKanji)
    {
        if (wdiv != 0)
            kshown = true, dp = DeferWindowPos(dp, fKanji->Handle, NULL, area.Left, area.Top, wpos, area.Height(), (fKanji->Visible ? 0 : SWP_SHOWWINDOW) | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
        else
            kshown = false, dp = DeferWindowPos(dp, fKanji->Handle, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);
        if (wdiv != -1 && hdiv != 0)
            gshown = true, dp = DeferWindowPos(dp, fGroup->Handle, NULL, wpos + fdivsize + area.Left, area.Top, area.Width() - wpos - fdivsize, hpos, (fGroup->Visible ? 0 : SWP_HIDEWINDOW) | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
        else
            gshown = false, dp = DeferWindowPos(dp, fGroup->Handle, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);
        if (wdiv != -1 && hdiv != -1)
            dshown = true, dp = DeferWindowPos(dp, fDict->Handle, NULL, wpos + fdivsize + area.Left, hpos + fdivsize + area.Top, area.Width() - wpos - fdivsize, area.Height() - hpos - fdivsize, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
        else
            dshown = false, dp = DeferWindowPos(dp, fDict->Handle, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);
    }
    else if (flayout == dlExpandDict)
    {
        if (hdiv != 0 && wdiv != 0)
            kshown = true, dp = DeferWindowPos(dp, fKanji->Handle, NULL, area.Left, area.Top, wpos, hpos, (fKanji->Visible ? 0 : SWP_SHOWWINDOW) | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
        else
            kshown = false, dp = DeferWindowPos(dp, fKanji->Handle, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);
        if (hdiv != 0 && wdiv != -1)
            gshown = true, dp = DeferWindowPos(dp, fGroup->Handle, NULL, wpos + fdivsize + area.Left, area.Top, area.Width() - wpos - fdivsize, hpos, (fGroup->Visible ? 0 : SWP_HIDEWINDOW) | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
        else
            gshown = false, dp = DeferWindowPos(dp, fGroup->Handle, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);
        if (hdiv != -1)
            dshown = true, dp = DeferWindowPos(dp, fDict->Handle, NULL, area.Left, hpos + fdivsize + area.Top, area.Width(), area.Height() - hpos - fdivsize, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
        else
            dshown = false, dp = DeferWindowPos(dp, fDict->Handle, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);
    }
    EndDeferWindowPos(dp);

    if (Visible && kshown != fKanji->Visible) // DeferWindowPos already hides the window, but it's not reflected in the property.
        fKanji->Visible = kshown;
    if (Visible && gshown != fGroup->Visible) // DeferWindowPos already hides the window, but it's not reflected in the property.
        fGroup->Visible = gshown;
    if (Visible && dshown != fDict->Visible) // DeferWindowPos already hides the window, but it's not reflected in the property.
        fDict->Visible = dshown;

    TRect r;

    if (oldw < fKanji->Width)
    {
        r = Rect(oldw, 0, fKanji->Width, fKanji->Height);
        RedrawWindow(fKanji->Handle, &r, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
    }

    if (flayout == dlExpandKanji)
    {
        r = Rect((wdiv != 0 ? wpos : 0) + area.Left, area.Top, wpos + fdivsize + area.Left, area.Height() + area.Top);
        InvalidateRect(Handle, &r, false);
        r = Rect(wpos + fdivsize + area.Left, (hdiv != 0 ? hpos : 0) + area.Top, area.Right, (hdiv != -1 ? hpos + fdivsize + area.Top : area.Bottom));
        InvalidateRect(Handle, &r, false);

        miExpandKanji->Visible = false;
        miExpandDict->Visible = true;
    }
    else if (flayout == dlExpandDict)
    {
        r = Rect((wdiv != 0 ? wpos : 0) + area.Left, area.Top, (wdiv != -1 ? wpos + fdivsize + area.Left : area.Right), hpos + area.Top);
        InvalidateRect(Handle, &r, false);
        r = Rect(area.Left, (hdiv != 0 ? hpos : 0) + area.Top, area.Right, (hdiv != -1 ? hpos + fdivsize + area.Top : area.Bottom));
        InvalidateRect(Handle, &r, false);

        miExpandKanji->Visible = true;
        miExpandDict->Visible = false;
    }

    UpdateWindow(Handle);
}
//---------------------------------------------------------------------------
void TfDock::init()
{
    if (!fsi[5].width || !fsi[5].height || fsi[5].width > Screen->Width + 20 || fsi[5].height > Screen->Height + 20 || fsi[5].top > Screen->Height || fsi[5].left > Screen->Width || fsi[5].top + fsi[5].height <= 0 || fsi[5].left + fsi[5].width <= 0)
    {
        fsi[5].width = Screen->Width > 990 ? 990 : 790;
        fsi[5].height = Screen->Height > 750 ? 750 : 590;
        fsi[5].left = (Screen->Width - fsi[5].width) / 2;
        fsi[5].top = (Screen->Height - fsi[5].height) / 2;
    }
    SizeInfo = fsi[5];

    miKanjiGroups->Visible = fGroup->Pages->TabIndex == 0;
    miWordGroups->Visible = fGroup->Pages->TabIndex == 1;

    RestorePosition();

    fKanji->KeepSize = false;

    for (int ix = 0; kanjieditlist && ix < kanjieditlist->Count; ++ix)
        kanjieditlist->Items[ix]->RestoreWndProc();

    fKanji->Shadowed = false;
    fKanji->ManualDock(fDock, NULL, alClient);
    fGroup->Shadowed = false;
    fGroup->ManualDock(fDock, NULL, alClient);
    fDict->Shadowed = false;
    fDict->ManualDock(fDock, NULL, alClient);

    for (int ix = 0; kanjieditlist && ix < kanjieditlist->Count; ++ix)
        kanjieditlist->Items[ix]->SetWndProc();

    fKanji->Show();
    fGroup->Show();
    fDict->Show();

    fKanji->KeepSize = true;

    SizeInfo.maxed = fsi[5].maxed;
    if (SizeInfo.maxed)
        WindowState = wsMaximized;
    else
    {
        WindowState = wsNormal;
    }

    TRect area = Rect(margins.Left, margins.Top, ClientWidth - margins.Right, ClientHeight - margins.Bottom);
    if (!hrate)
        hrate = 0.6;
    if (hrate == -1 || hrate == -2)
        hdiv = hrate + 1;
    else
        hdiv = area.Height() * hrate;

    if (wdiv == 1)
        wdiv = fKanji->boxsize * (Screen->Width > 790 ? 6 : 5) + fKanji->pbGrid->Width - fKanji->pbGrid->ClientWidth; /*GetSystemMetrics(SM_CXVSCROLL)*/;

    fstate = dsDocked;

    if (settings.intray && oldresize == NULL) // The size of the kanji list is not restored correctly if starting with a visible main window. Update the size and set the callbacks elsewhere.
    {
        Resized(false, true);
        UpdateMenuCheck();

        oldresize = fKanji->OnResize;
        fKanji->OnResize = &kanjiResize;
    }

    AddEvent(this, onactivate, eltActivate);
    AddEvent(this, ondeactivate, eltDeactivate);

    KeepSize = true;

    fill_menu();
    fDock->SetCaption();
}
//---------------------------------------------------------------------------
void __fastcall TfDock::FormShow(TObject *Sender)
{
    if (oldresize == NULL && !settings.intray) // The size of the kanji list is not restored correctly if starting with a visible main window. Update the size and set the callbacks here.
    {
        Resized(false, true);
        UpdateMenuCheck();

        oldresize = fKanji->OnResize;
        fKanji->OnResize = &kanjiResize;
    }
}
//---------------------------------------------------------------------------
void TfDock::dock(bool hidden)
{
    if (fstate != dsUndocked)
        return;

    fstate = dsDocking;
    try
    {
        bool kinf = fInfo && fInfo->Visible;
        bool clpv = fClipboard && fClipboard->Visible;
        if (kinf)
            fInfo->Close();
        if (clpv)
            fClipboard->Close();

        SizeInfo = fsi[5];

        for (int ix = 0; kanjieditlist && ix < kanjieditlist->Count; ++ix)
            kanjieditlist->Items[ix]->RestoreWndProc();

        fKanji->Shadowed = false;
        fKanji->ManualDock(this, NULL, alClient);

        if (!fKanji->Visible)
        {
            fKanji->Show();
            ActiveControl = activectrl;
        }

        for (int ix = 0; kanjieditlist && ix < kanjieditlist->Count; ++ix)
            kanjieditlist->Items[ix]->SetWndProc();

        if (hidden)
            Hide();
        if (SizeInfo.maxed)
            WindowState = wsMaximized;
        else
            WindowState = wsNormal;
        Resized(false, true);

        oldresize = fKanji->OnResize;
        fKanji->OnResize = &kanjiResize;

        AddEvent(this, onactivate, eltActivate);
        AddEvent(this, ondeactivate, eltDeactivate);

        if (kinf)
            fInfo->Show();
        if (clpv)
            fClipboard->Show();
    }
    catch (...)
    {
        fstate = dsNone;
        throw;
    }
    fstate = dsDocked;
}
//---------------------------------------------------------------------------
void TfDock::undock(bool show)
{
    if (fstate != dsDocked)
        return;

    fstate = dsUndocking;
    try
    {
        RemoveEvents(this);

        bool kinf = fInfo && fInfo->Visible;
        bool clpv = fClipboard && fClipboard->Visible;
        if (kinf)
            fInfo->Close();
        if (clpv)
            fClipboard->Close();

        fsi[5] = SizeInfo;
        TRect area = Rect(margins.Left, margins.Top, ClientWidth - margins.Right, ClientHeight - margins.Bottom);

        activectrl = ActiveControl;
        fKanji->Hide();

        fKanji->OnResize = oldresize;
        oldresize = NULL;

        for (int ix = 0; kanjieditlist && ix < kanjieditlist->Count; ++ix)
            kanjieditlist->Items[ix]->RestoreWndProc();

        fKanji->ManualFloat(Rect(0, 0, fKanji->Width, fKanji->Height));

        for (int ix = 0; kanjieditlist && ix < kanjieditlist->Count; ++ix)
            kanjieditlist->Items[ix]->SetWndProc();
    }
    catch (...)
    {
        fstate = dsNone;
        throw;
    }
    fstate = dsUndocked;
}
//---------------------------------------------------------------------------
void __fastcall TfDock::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caNone;
    //dock(true);
    fMain->Close();
}
//---------------------------------------------------------------------------
void TfDock::onactivate(void *caller)
{
    SendMessage(fKanji->Handle, CM_ACTIVATE, 0, 0);
    SendMessage(fGroup->Handle, CM_ACTIVATE, 0, 0);
    SendMessage(fDict->Handle, CM_ACTIVATE, 0, 0);
}
//---------------------------------------------------------------------------
void TfDock::ondeactivate(void *caller)
{
    SendMessage(fKanji->Handle, CM_DEACTIVATE, 0, 0);
    SendMessage(fGroup->Handle, CM_DEACTIVATE, 0, 0);
    SendMessage(fDict->Handle, CM_DEACTIVATE, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::FormMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button != mbLeft)
        return;

    TRect area = Rect(margins.Left, margins.Top, ClientWidth - margins.Right, ClientHeight - margins.Bottom);
    x -= area.Left;
    y -= area.Top;

    if (flayout == dlExpandKanji)
    {
        if (x < wpos + fdivsize || (!fGroup->Visible && !fDict->Visible))
            action = daWResize;
        else
            action = daHResize;
    }
    else if (flayout == dlExpandDict)
    {
        if (y < hpos && (fKanji->Visible || fGroup->Visible))
            action = daWResize;
        else
            action = daHResize;
    }

    if (action == daWResize)
        fdragpos = x - wpos;
    if (action == daHResize)
        fdragpos = y - hpos;
}
//---------------------------------------------------------------------------
void __fastcall TfDock::FormMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    TRect area = Rect(margins.Left, margins.Top, ClientWidth - margins.Right, ClientHeight - margins.Bottom);
    x -= area.Left;
    y -= area.Top;

    if (action == daNone)
    {
        if (flayout == dlExpandKanji)
        {
            if (x < wpos + fdivsize || (!fGroup->Visible && !fDict->Visible))
                Cursor = crHSplit;
            else
                Cursor = crVSplit;
        }
        else if (flayout == dlExpandDict)
        {
            if (y < hpos && (fKanji->Visible || fGroup->Visible))
                Cursor = crHSplit;
            else
                Cursor = crVSplit;
        }
    }
    else if (action == daWResize)
    {
        int dif = GetSystemMetrics(SM_CXVSCROLL);
        int ndiv;
        ndiv = max(x - fdragpos, fKanji->minlimit + dif);

        if (x - fdragpos - margins.Left < 80 * scaleres)
            ndiv = 0;
        else if (x - fdragpos > ClientWidth - margins.Left - margins.Right - 80 * scaleres)
            ndiv = -1;

        if (wdiv != ndiv)
        {
            wdiv = ndiv;
            Resized(true);
            UpdateMenuCheck();
        }
    }
    else if (action == daHResize)
    {
        double ndiv = (double)(y - fdragpos);
        if (y - fdragpos - margins.Top < 80 * scaleres)
            ndiv = 0;
        else if (y - fdragpos > ClientHeight - margins.Top - margins.Bottom - 80 * scaleres)
            ndiv = -1;

        if (hdiv != ndiv)
        {
            hdiv = ndiv;
            Resized(true);
            UpdateMenuCheck();
        }
    }
}
//---------------------------------------------------------------------------
void TfDock::UpdateMenuCheck()
{
    miShowKanjiList->Checked = wdiv != 0 && (hdiv != 0 || layout != dlExpandDict);
    miShowGroupList->Checked = wdiv != -1 && hdiv != 0;
    miShowDictionary->Checked = hdiv != -1 && (wdiv != -1 || layout != dlExpandKanji);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::FormMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (action == daWResize || action == daHResize)
        action = daNone;
}
//---------------------------------------------------------------------------
void __fastcall TfDock::FormPaint(TObject *Sender)
{
    TCanvas *c = Canvas;
    c->Pen->Color = (TColor)0xacacac;
    c->Brush->Color = (TColor)0xceced0;

    TRect area = Rect(margins.Left, margins.Top, ClientWidth - margins.Right, ClientHeight - margins.Bottom);

    if (!hrate && ClientHeight)
    {
        area = Rect(0, 0, ClientWidth, ClientHeight);
        hrate = (double)hdiv / ClientHeight;
    }

    if (flayout == dlExpandKanji)
    {
        c->MoveTo(wpos + area.Left, area.Top);
        c->LineTo(wpos + area.Left, area.Height() + area.Top);
        if (wdiv != -1)
        {
            c->MoveTo(wpos + fdivsize - 1 + area.Left, area.Top);
            c->LineTo(wpos + fdivsize - 1 + area.Left, hpos + 1 + area.Top);
            c->MoveTo(wpos + fdivsize - 1 + area.Left, hpos + fdivsize - 1 + area.Top);
            c->LineTo(wpos + fdivsize - 1 + area.Left, area.Bottom);
        }
        else
        {
            c->MoveTo(wpos + fdivsize - 1 + area.Left, area.Top);
            c->LineTo(wpos + fdivsize - 1 + area.Left, area.Bottom);
        }
        c->MoveTo(wpos + fdivsize + area.Left, hpos + area.Top);
        c->LineTo(area.Right, hpos + area.Top);
        c->MoveTo(wpos + fdivsize + area.Left, hpos + fdivsize - 1 + area.Top);
        c->LineTo(area.Right, hpos + fdivsize - 1 + area.Top);

        if (wdiv == 0)
        {
            c->Brush->Color = clBtnFace;
            c->FillRect(Rect(area.Left, area.Top, wpos + area.Left, area.Bottom));
            c->Brush->Color = (TColor)0xceced0;
            c->FillRect(Rect(wpos + 1 + area.Left, area.Top, wpos + fdivsize - 1 + area.Left, area.Bottom));
        }
        else if (wdiv == -1)
        {
            c->FillRect(Rect(wpos + 1 + area.Left, area.Top, wpos + area.Left + fdivsize - 1, area.Bottom));
            c->Brush->Color = clBtnFace;
            c->FillRect(Rect(wpos + area.Left + fdivsize, area.Top, area.Right, area.Bottom));
            c->Brush->Color = (TColor)0xceced0;
        }
        else
            c->FillRect(Rect(wpos + 1 + area.Left, area.Top, wpos + fdivsize - 1 + area.Left, area.Bottom));
        if (wdiv != -1)
        {
            c->FillRect(Rect(wpos + fdivsize - 1 + area.Left, hpos + 1 + area.Top, area.Right, hpos + fdivsize - 1 + area.Top));
            c->Brush->Color = clBtnFace;
            if (hdiv == 0)
                c->FillRect(Rect(wpos + fdivsize + area.Left, area.Top, area.Right, hpos + area.Top));
            else if (hdiv == -1)
                c->FillRect(Rect(wpos + fdivsize + area.Left, hpos + fdivsize + area.Top, area.Right, area.Bottom));
            c->Brush->Color = (TColor)0xceced0;
        }
    }
    else if (flayout == dlExpandDict)
    {
        c->MoveTo(area.Left, hpos + fdivsize - 1 + area.Top);
        c->LineTo(area.Right, hpos + fdivsize - 1 + area.Top);

        if (hdiv != 0)
        {
            c->MoveTo(area.Left, hpos + area.Top);
            c->LineTo(wpos + 1 + area.Left, hpos + area.Top);
            c->MoveTo(wpos + fdivsize - 1 + area.Left, hpos + area.Top);
            c->LineTo(area.Right, hpos + area.Top);
            c->MoveTo(wpos + area.Left, area.Top);
            c->LineTo(wpos + area.Left, hpos + area.Top);
            c->MoveTo(wpos + fdivsize - 1 + area.Left, area.Top);
            c->LineTo(wpos + fdivsize - 1 + area.Left, hpos + area.Top);
        }
        else
        {
            c->MoveTo(area.Left, hpos + area.Top);
            c->LineTo(area.Right, hpos + area.Top);
        }

        if (hdiv != 0)
        {
            c->FillRect(Rect(wpos + 1 + area.Left, area.Top, wpos + fdivsize - 1 + area.Left, hpos + 1 + area.Top));
            if (wdiv == 0)
            {
                c->Brush->Color = clBtnFace;
                c->FillRect(Rect(area.Left, area.Top, wpos + area.Left, hpos + area.Top));
                c->Brush->Color = (TColor)0xceced0;
            }
            else if (wdiv == -1)
            {
                c->Brush->Color = clBtnFace;
                c->FillRect(Rect(wpos + fdivsize + area.Left, area.Top, area.Right, hpos + area.Top));
                c->Brush->Color = (TColor)0xceced0;
            }
        }
        else
        {
            c->Brush->Color = clBtnFace;
            c->FillRect(Rect(area.Left, area.Top, area.Right, hpos + area.Top));
            c->Brush->Color = (TColor)0xceced0;
        }
        c->FillRect(Rect(area.Left, hpos + 1 + area.Top, area.Right, hpos + fdivsize - 1 + area.Top));
        if (hdiv == -1)
        {
            c->Brush->Color = clBtnFace;
            c->FillRect(Rect(area.Left, hpos + fdivsize + area.Top, area.Right, area.Bottom));
            c->Brush->Color = (TColor)0xceced0;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDock::kanjiResize(TObject *Sender)
{
    if (oldresize)
        oldresize(Sender);

    if (wdiv != 0 && wdiv != -1 && wpos != fKanji->Width && fKanji->Width != 0)
    {
        wdiv = fKanji->Width;
        Resized(false);
    }
}
//---------------------------------------------------------------------------
extern wchar_t Version_String[];
void TfDock::SetCaption()
{
    UnicodeString str = (UnicodeString)L"zkanji v" + Version_String;
    if (dictionary->Count > 1)
        str += L" - Dictionary: " + dictionary->Strings[settings.dictionary_index] + L" - Groups: " + dictionary->Strings[fGroup->cbDictionary->ItemIndex];
    if (settings.debug)
        str += L" +debug";
    Caption = str;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall DockWindows(bool show)
{
    if (fDock && fDock->state != dsUndocked)
        return;

    fDock->dock(show);
}
//---------------------------------------------------------------------------
void __fastcall UndockWindows(bool show)
{
    if (!fDock || fDock->state != dsDocked)
        return;

    fDock->undock(show);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miKInfoClick(TObject *Sender)
{
    fKanji->miInfoClick(miKInfo);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miKTranslateClick(TObject *Sender)
{
    fKanji->miTranslateClick(miKTranslate);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miKKanjiDefClick(TObject *Sender)
{
    fKanji->miKanjiDefClick(miKKanjiDef);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miKGenerateClick(TObject *Sender)
{
    fKanji->miGenerateClick(miKGenerate);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miKCopyClick(TObject *Sender)
{
    fKanji->miCopyClick(miKCopy);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miKAppendClick(TObject *Sender)
{
    fKanji->miAppendClick(miKAppend);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miKCopyAllClick(TObject *Sender)
{
    fKanji->miCopyAllClick(miKCopyAll);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miKAppendAllClick(TObject *Sender)
{
    fKanji->miAppendAllClick(miKAppendAll);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miWToVocabClick(TObject *Sender)
{
    fDict->miToVocabClick(miWToVocab);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miWEditWordClick(TObject *Sender)
{
    fDict->miEditWordClick(miWEditWord);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miWNewWordClick(TObject *Sender)
{
    fDict->miNewWordClick(miWNewWord);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miWDeleteWordClick(TObject *Sender)
{
    fDict->miDeleteWordClick(miWDeleteWord);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miWRevertWordClick(TObject *Sender)
{
    fDict->miRevertWordClick(miWRevertWord);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miWCopyKanjiClick(TObject *Sender)
{
    fDict->miCopyKanjiClick(miWCopyKanji);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miWAppendKanjiClick(TObject *Sender)
{
    fDict->miAppendKanjiClick(miWAppendKanji);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miWCopyKanaClick(TObject *Sender)
{
    fDict->miCopyKanaClick(miWCopyKana);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miWAppendKanaClick(TObject *Sender)
{
    fDict->miAppendKanaClick(miWAppendKana);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGKKanjiInfoClick(TObject *Sender)
{
    fGroup->miKanjiInfoClick(miGKKanjiInfo);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGKTranslateKanjiClick(TObject *Sender)
{
    fGroup->miTranslateKanjiClick(miGKTranslateKanji);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGKKanjiDefClick(TObject *Sender)
{
    fGroup->miKanjiDefClick(miGKKanjiDef);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGKGenerateClick(TObject *Sender)
{
    fGroup->miGenerateClick(miGKGenerate);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGWToVocabClick(TObject *Sender)
{
    fGroup->miToVocabClick(miGWToVocab);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGWTranslateWrittenClick(TObject *Sender)
{
    fGroup->miTranslateWrittenClick(miGWTranslateWritten);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGWTranslateKanaClick(TObject *Sender)
{
    fGroup->miTranslateKanaClick(miGWTranslateKana);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGWEditWordClick(TObject *Sender)
{
    fGroup->miEditWordClick(miGWEditWord);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGWPrintVocabClick(TObject *Sender)
{
    fGroup->miPrintVocabClick(miGWPrintVocab);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGKCopyKanjiClick(TObject *Sender)
{
    fGroup->miCopyKanjiClick(miGKCopyKanji);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGKAppendKanjiClick(TObject *Sender)
{
    fGroup->miAppendKanjiClick(miGKAppendKanji);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGKCopyAllClick(TObject *Sender)
{
    fGroup->miCopyAllClick(miGKCopyAll);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGKAppendAllClick(TObject *Sender)
{
    fGroup->miAppendAllClick(miGKAppendAll);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGWCopyKanaClick(TObject *Sender)
{
    fGroup->miCopyKanaClick(miGWCopyKana);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGWAppendKanaClick(TObject *Sender)
{
    fGroup->miAppendKanaClick(miGWAppendKana);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGGExportClick(TObject *Sender)
{
    //fGroup->miGExportClick(miGGExport);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGImportWakanClick(TObject *Sender)
{
    //fGroup->miImportWakanClick(miGImportWakan);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGExportClick(TObject *Sender)
{
    //fGroup->miExportClick(miGExport);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGImportClick(TObject *Sender)
{
    //fGroup->miImportClick(miGImport);
}
//---------------------------------------------------------------------------
void TfDock::set_layout(TDockLayout val)
{
    if (val == flayout)
        return;
    flayout = val;
    Resized(false, true);
    UpdateMenuCheck();
}
//---------------------------------------------------------------------------
bool import_examples(const wchar_t *inputfile, const wchar_t *outputfile);
void __fastcall TfDock::miExamplesClick(TObject *Sender)
{
    try
    {
        if (ExecuteDialog(data->dlgImportEx))
        {
            wchar_t temppath[MAX_PATH];
            GetTempPath(MAX_PATH, temppath);

            fDict->disable_examples();
            fPopupDict->disable_examples();
            store->ClearExampleData();

            //Examples->clear_cache();
            delete Examples;
            Examples = NULL;
            if (import_examples(data->dlgImportEx->FileName.c_str(), (UnicodeString(temppath) + L"zkanjiexamples.zkj").c_str()))
            {
                CopyFile((UnicodeString(temppath) + L"zkanjiexamples.zkj").c_str(), (paths.installdatapath + L"examples.zkj").c_str(), FALSE);
                DeleteFile((UnicodeString(temppath) + L"zkanjiexamples.zkj").c_str());
                delete Examples;
                Examples = NULL;
                store->ClearExampleData();
            }
            else
            {
                DeleteFile((UnicodeString(temppath) + L"zkanjiexamples.zkj").c_str());
                delete Examples;
                Examples = NULL;
                store->ClearExampleData();
            }
            hasexamples = FileExists(paths.installdatapath + L"examples.zkj") && load_examples(paths.installdatapath + L"examples.zkj");
            if (hasexamples)
            {
                fDict->enable_examples();
                fPopupDict->enable_examples();
            }
        }
    }
    catch (...)
    {
        ;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGWCopyWordClick(TObject *Sender)
{
    fGroup->miCopyWordClick(miGWCopyWord);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miGWAppendWordClick(TObject *Sender)
{
    fGroup->miAppendWordClick(miGWAppendWord);
}
//---------------------------------------------------------------------------
void TfDock::z_enable(TMessage &msg)
{
    Enabled = msg.WParam == 1;
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miOpenFolderClick(TObject *Sender)
{
    ShellExecuteW(Handle, L"open", paths.inipath.c_str(), 0, 0, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TfDock::Export1Click(TObject *Sender)
{
    fExport = NULL;
    try
    {
        TWordCollection *coll = DictionarySelect(L"Please select the dictionary containing the data you would like to export:", true, settings.dictionary_index);
        if (coll)
        {
            fExport = new TfExport(Application);
            fExport->ShowModalEx(coll);
        }
    }
    catch (...)
    {
        ;
    }
    delete fExport;
}
//---------------------------------------------------------------------------
void __fastcall TfDock::Groupimport1Click(TObject *Sender)
{
    GroupImport();
}
//---------------------------------------------------------------------------
void __fastcall TfDock::Dictionaryimport1Click(TObject *Sender)
{
    DictImport();
}
//---------------------------------------------------------------------------
void __fastcall TfDock::Fulldictionaryimport1Click(TObject *Sender)
{
    FullImport();
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miExpandKanjiClick(TObject *Sender)
{
    layout = (TDockLayout)((TMenuItem*)Sender)->Tag;
}
//---------------------------------------------------------------------------
void __fastcall TfDock::miShowKanjiListClick(TObject *Sender)
{
    int who = ((TMenuItem*)Sender)->Tag;
    if (layout == dlExpandKanji)
    {
        switch (who)
        {
        case 0:
            if (miShowKanjiList->Checked)
                wdiv = 0;
            else
                wdiv = 1;
            break;
        case 1:
            if (miShowGroupList->Checked)
            {
                if (hdiv == -1)
                    wdiv = -1;
                else
                    hdiv = 0;
            }
            else if (wdiv != -1)
                hdiv = Height / 2;
            else
            {
                wdiv = Width / 2;
                hdiv = -1;
            }
            break;
        case 2:
            if (miShowDictionary->Checked)
            {
                if (hdiv == 0)
                    wdiv = -1;
                else
                    hdiv = -1;
            }
            else if (wdiv != -1)
                hdiv = Height / 2;
            else
            {
                wdiv = Width / 2;
                hdiv = 0;
            }
            break;
        }
    }
    else if (layout == dlExpandDict)
    {
        switch (who)
        {
        case 0:
            if (miShowKanjiList->Checked)
            {
//                if (wdiv == -1)
//                    hdiv = 0;
//                else
                    wdiv = 0;
            }
            else if (hdiv != 0)
                wdiv = 1;
            else
            {
                hdiv = Height / 2;
                wdiv = -1;
            }
            break;
        case 1:
            if (miShowGroupList->Checked)
            {
//                if (wdiv == 0)
//                    hdiv = 0;
//                else
                    wdiv = -1;
            }
            else if (hdiv != 0)
                wdiv = Width / 2;
            else
            {
                hdiv = Height / 2;
                wdiv = 0;
            }
            break;
        case 2:
            if (miShowDictionary->Checked)
                hdiv = -1;
            else
                hdiv = Height / 2;
            break;
        }
    }
    Resized(true);
    UpdateMenuCheck();
}
//---------------------------------------------------------------------------

