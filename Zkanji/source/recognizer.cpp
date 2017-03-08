//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "recognizer.h"
#include "elementpoints.h"
#include "kanjiobj.h"
#include "wordlist.h"
#include "popupdict.h"
#include "closebutton.h"
#include "kanjielement.h"
#include "mouseevent.h"
#include "collection.h"
#include "settings.h"
#include "kanjiedit.h"
#include "kanji.h"
#include "kanjiinfo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(128);
#endif

TfRecognize *fRecognize;

//---------------------------------------------------------------------------
__fastcall TfRecognize::TfRecognize(TComponent* Owner) : oldsize(Rect(-1, -1, -1, -1)), fresetsize(true), fresetpos(true), finited(false), fautohide(false), fshowpopup(true), savedleft(0), savedtop(0), fcallback(NULL), TBaseForm(Owner)
{
    poresults = new TKanjiPanelObj(pbResults, pcallback, rcolor, rdraw);
    poresults->bgcolor = (TColor)0x00f7f3ee;

    sobj = new TStrokeObj(pbDraw);
    sobj->callback = scallback;

    closebtn = new TCloseButton(this, sbpTopRight, 1, 2, &cbgptr);
    roundbounds(ClientWidth, ClientHeight);

    EraseBackground = false;

    AppActivateEvent(this, appDeactivate, false, true);
    AddEvent(this, repaintProc, eltActivate);
    AddEvent(this, repaintProc, eltDeactivate);

    PostMessage(Handle, WM_Z_INIT, 0, 0);

    Shadowed = true;
}
//---------------------------------------------------------------------------
void TfRecognize::WMZInit(TMessage &msg)
{
    ActivateOnShow = false;
}
//---------------------------------------------------------------------------
__fastcall TfRecognize::~TfRecognize()
{
    RemoveEvent(this, repaintProc, eltDeactivate);
    RemoveEvent(this, repaintProc, eltActivate);

    delete sobj;
    AppActivateEvent(this, appDeactivate, false, false);
    delete poresults;
    delete closebtn;

}
//---------------------------------------------------------------------------
void TfRecognize::Hide()
{
    inherited::Hide();
    OnTop = false;
}
//---------------------------------------------------------------------------
void TfRecognize::SetDimensions(int aleft, int atop, int awidth)
{
    if (awidth == -1)
    {
        finited = false;
        return;
    }

    Left = aleft;
    Top = atop;
    Width = awidth;

    savedleft = Left;
    savedtop = Top;

    finited = true;
}
//---------------------------------------------------------------------------
void TfRecognize::ShowEx(TKanjiEdit *aedit, TSpeedButton *abutton, bool aautohide, bool ashowhint, bool aallowerror, int acntlimit, bool ageneral, bool ashowpopup, TRecognizePosition apos)
{
    Recreate(true);

    TPoint pt = aedit->ScreenBounds.TopLeft();

    poresults->collection = dictionary->Items[settings.dictionary_index];
    feditor = aedit;
    fbutton = abutton;
    fautohide = aautohide;
    fposition = apos;
    fshowpopup = ashowpopup;

    btnGen->Enabled = ageneral;
    sobj->GeneralChar = btnGen->Down && ageneral;
    sobj->StrokeLimit = acntlimit;

    showhint = ashowhint;
    allowerror = aallowerror;

    if (resetsize || !finited)
        Width = 214;

    Left = savedleft;
    Top = savedtop;

    if (resetpos || !finited || apos != rpDefault)
    {
        tagMONITORINFO inf = MonitorInfo(pt.X, pt.Y);

        bool above = apos == rpDefault || apos == rpAbove;
        if (above && pt.y - 5 - Height <= inf.rcMonitor.top + 2)
            above = false;
        else if (!above && pt.y + feditor->Height + 6 + Height > inf.rcMonitor.bottom - 2)
            above = true;

        if (above)
            Top = pt.y - 5 - Height;
        else
            Top = pt.y + feditor->Height + 6;
        Left = max((int)inf.rcMonitor.left + 2, min((int)pt.x - 28, (int)inf.rcMonitor.right - 2 - Width));
    }

    finited = true;

    Show();
}
//---------------------------------------------------------------------------
void TfRecognize::Place(TKanjiEdit *aedit, TRecognizePosition apos)
{
    if (apos == rpDefault && !resetpos)
        return;

    if (apos == rpDefault)
        apos = fposition == rpDefault ? rpAbove : fposition;

    TPoint pt = aedit->ScreenBounds.TopLeft();

    tagMONITORINFO inf = MonitorInfo(pt.X, pt.Y);

    bool above = apos == rpAbove;
    if (apos == rpAbove && pt.y - 5 - Height <= inf.rcMonitor.top + 2)
        above = false;
    else if (apos != rpAbove && pt.y + aedit->Height + 6 + Height > inf.rcMonitor.bottom - 2)
        above = true;

    if (above)
        Top = pt.y - 5 - Height;
    else
        Top = pt.y + aedit->Height + 6;
    Left = max((int)inf.rcMonitor.left + 2, min((int)pt.x - 28, (int)inf.rcMonitor.right - 2 - Width));

    savedleft = Left;
    savedtop = Top;
}
//---------------------------------------------------------------------------
void TfRecognize::appDeactivate(void *caller)
{
    Hide();
}
//---------------------------------------------------------------------------
void TfRecognize::WMSizing(TWMSizing &msg)
{
    oldsize = Rect(Left, Top, Left + Width, Top + Height);

    ConstraintSizing(msg, 12 + (164 + 28)*scaleres, 12 + 164*scaleres + pResults->Height, min(Screen->Height, Screen->Width)*0.6, min(Screen->Height, Screen->Width)*0.6);
    int uw = msg.lprc->Width() - 12 - 28 * scaleres;
    int uh = msg.lprc->Height() - 12 - pResults->Height;
    if (uw < uh)
    {
        if (msg.fwSide == WMSZ_LEFT || msg.fwSide == WMSZ_RIGHT)
            msg.lprc->Top += uh - uw;
        if (msg.fwSide == WMSZ_TOP || msg.fwSide == WMSZ_BOTTOM)
            msg.lprc->Right += uh - uw;
        if (msg.fwSide == WMSZ_TOPLEFT || msg.fwSide == WMSZ_BOTTOMLEFT)
            msg.lprc->Left -= uh - uw;
        if (msg.fwSide == WMSZ_TOPRIGHT || msg.fwSide == WMSZ_BOTTOMRIGHT)
            msg.lprc->Right += uh - uw;
    }
    else if (uh < uw)
    {
        if (msg.fwSide == WMSZ_LEFT || msg.fwSide == WMSZ_RIGHT)
            msg.lprc->Top -= uw - uh;
        if (msg.fwSide == WMSZ_TOP || msg.fwSide == WMSZ_BOTTOM)
            msg.lprc->Right -= uw - uh;
        if (msg.fwSide == WMSZ_TOPLEFT || msg.fwSide == WMSZ_TOPRIGHT)
            msg.lprc->Top -= uw - uh;
        if (msg.fwSide == WMSZ_BOTTOMLEFT || msg.fwSide == WMSZ_BOTTOMRIGHT)
            msg.lprc->Bottom += uw - uh;
    }

    roundbounds(msg.lprc->Width() - 12, msg.lprc->Height() - 12);
}
//---------------------------------------------------------------------------
void TfRecognize::WMNCActivate(TMessage &msg)
{
    msg.Result = true;
}
//---------------------------------------------------------------------------
void TfRecognize::WMNCCalcSize(TMessage &msg)
{
    if (!msg.WParam)
    {
        TRect *r = ((TRect*)msg.LParam);
        InflateRect(r, -6, -6);
        msg.Result = 0;
    }
    else
    {
        NCCALCSIZE_PARAMS *n = ((NCCALCSIZE_PARAMS*)msg.LParam);
        memmove(n->rgrc + 1, n->rgrc, sizeof(TRect*)*2);
        InflateRect(n->rgrc, -6, -6);
    }
}
//---------------------------------------------------------------------------
void TfRecognize::WMNCHitTest(TMessage &msg)
{
    TPoint p;
    p.x = GET_X_LPARAM(msg.LParam);
    p.y = GET_Y_LPARAM(msg.LParam);
    p = ScreenToClient(p);

    if (ControlAtPos(p, false, true, false))
        msg.Result = HTCLIENT;
    else if (p.x < 5 && p.y < 5)
        msg.Result = HTTOPLEFT;
    else if (p.x >= ClientWidth - 5 && p.y < 5)
        msg.Result = HTTOPRIGHT;
    else if (p.x >= ClientWidth - 5 && p.y >= ClientHeight - 5)
        msg.Result = HTBOTTOMRIGHT;
    else if (p.x < 5 && p.y >= ClientHeight - 5)
        msg.Result = HTBOTTOMLEFT;
    else if (p.x < 0)
        msg.Result = HTLEFT;
    else if (p.x >= ClientWidth)
        msg.Result = HTRIGHT;
    else if (p.y < 0)
        msg.Result = HTTOP;
    else if (p.y >= ClientHeight)
        msg.Result = HTBOTTOM;
    else
        msg.Result = HTCAPTION;
}
//---------------------------------------------------------------------------
void TfRecognize::WMNCPaint(TMessage &msg)
{
    HDC hdc;
    hdc = GetWindowDC(Handle);
    if (!hdc)
        return;

    TRect r;
    HPEN p;
    HPEN pold;
    TColor pcol;
    int c = (int)ColorToRGB(clBtnFace);
    bool d = (c & 0xff) >= BTN_DARK_TRESH && ((c >> 8) & 0xff) >= BTN_DARK_TRESH && ((c >> 16) & 0xff) >= BTN_DARK_TRESH;

    GetWindowRect(Handle, &r);
    OffsetRect(r, -r.Left, -r.Top);
    ExcludeClipRect(hdc, 6, 6, r.Width() - 6, r.Height() - 6);

    pcol = d ? COLORDEC(clBtnFace, 0x343434) : COLORINC(clBtnFace, 0x343434);
    if (Active)
        pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.6);
    p = CreatePen(PS_SOLID, 0, pcol);
    pold = (HPEN)SelectObject((HDC)hdc, p);

    TPoint pts2[5] = { Point(2, r.Height() - 3), Point(2, 2), Point(r.Width() - 3, 2), Point(r.Width() - 3, r.Height() - 3), Point(2, r.Height() - 3) };
    Polyline(hdc, pts2, 5);
    SelectObject(hdc, pold);
    DeleteObject(p);

    pcol = d ? COLORDEC(clBtnFace, 0x282828) : COLORINC(clBtnFace, 0x282828);
    if (Active)
        pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.3);
    p = CreatePen(PS_SOLID, 0, pcol);
    pold = (HPEN)SelectObject((HDC)hdc, p);

    TPoint pts[5] = { Point(3, r.Height() - 4), Point(3, 3), Point(r.Width() - 4, 3), Point(r.Width() - 4, r.Height() - 4), Point(3, r.Height() - 4) };
    Polyline(hdc, pts, 5);
    SelectObject(hdc, pold);
    DeleteObject(p);

    pcol = d ? COLORDEC(clBtnFace, 0x1f1f1f) : COLORINC(clBtnFace, 0x1f1f1f);
    if (Active)
        pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.2);
    p = CreatePen(PS_SOLID, 0, pcol);
    pold = (HPEN)SelectObject((HDC)hdc, p);

    TPoint pts3[5] = { Point(4, r.Height() - 5), Point(4, 4), Point(r.Width() - 5, 4), Point(r.Width() - 5, r.Height() - 5), Point(4, r.Height() - 5) };
    Polyline(hdc, pts3, 5);
    SelectObject(hdc, pold);
    DeleteObject(p);

    pcol = d ? COLORDEC(clBtnFace, 0x3a3a3a) : COLORINC(clBtnFace, 0x3a3a3a);
    if (Active)
        pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.6);
    p = CreatePen(PS_SOLID, 0, pcol);
    pold = (HPEN)SelectObject((HDC)hdc, p);

    TPoint pts4[5] = { Point(5, r.Height() - 6), Point(5, 5), Point(r.Width() - 6, 5), Point(r.Width() - 6, r.Height() - 6), Point(5, r.Height() - 6) };
    Polyline(hdc, pts4, 5);
    SelectObject(hdc, pold);
    DeleteObject(p);

    pcol = d ? COLORDEC(clBtnShadow, 0x404040) : COLORINC(clBtnShadow, 0x404040);
    p = CreatePen(PS_SOLID, 0, pcol);
    pold = (HPEN)SelectObject((HDC)hdc, p);

    LOGBRUSH lb;
    lb.lbStyle = BS_SOLID;
    pcol = d ? COLORDEC(clBtnFace, 0x404040) : COLORINC(clBtnFace, 0x404040);
    if (Active)
        pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.5);
    lb.lbColor = pcol;

    HBRUSH b = CreateBrushIndirect(&lb);
    HBRUSH bold = (HBRUSH)SelectObject((HDC)hdc, b);

    ExcludeClipRect(hdc, 2, 2, r.Width() - 2, r.Height() - 2);
    RoundRect(hdc, 0, 0, r.Width(), r.Height(), 9, 9);
    SelectObject(hdc, bold);
    SelectObject(hdc, pold);
    DeleteObject(p);
    DeleteObject(b);
    ReleaseDC(Handle, hdc);
}
//---------------------------------------------------------------------------
void TfRecognize::repaintProc(void *caller)
{
    SendMessage(Handle, WM_NCPAINT, 0, 0);
}
//---------------------------------------------------------------------------
void TfRecognize::roundbounds(int fw, int fh)
{
    fw += 12;
    fh += 12;
    HRGN rgn = CreateRoundRectRgn(0, 0, fw + 1, fh + 1, 9, 9);
    SetWindowRgn(Handle, rgn, false);
}
//---------------------------------------------------------------------------
void TfRecognize::rcolor(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected, TColor &background, TColor &fontcolor)
{
    if (elem->owner > NO_OWNER)
    {
        if (HIRAGANA(elem->owner))
        {
            background = (TColor)(!selected ? 0xbcf7bc : 0xbcf7ac);
        }
        else
        {
            background = (TColor)(!selected ? 0xf8dfbc : 0xffdaa5);
        }
    }
}
//---------------------------------------------------------------------------
void TfRecognize::rdraw(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected)
{
    ;
}
//---------------------------------------------------------------------------
bool TfRecognize::get_showhint()
{
    return poresults->showhint;
}
//---------------------------------------------------------------------------
void TfRecognize::set_showhint(bool val)
{
    poresults->showhint = val;
}
//---------------------------------------------------------------------------
bool TfRecognize::get_allowerror()
{
    return sobj->AllowError;
}
//---------------------------------------------------------------------------
void TfRecognize::set_allowerror(bool val)
{
    sobj->AllowError = val;
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::FormResize(TObject *Sender)
{
    if (pbDraw->ClientWidth != pbDraw->ClientHeight)
        Height += pbDraw->ClientWidth - pbDraw->ClientHeight;
    if (Visible)
    {
        HRGN hr = CreateRectRgn(Left + 6, Top + 6, Left + Width - 6, Top + Height - 6), orgn = CreateRectRgnIndirect(&oldsize);
        CombineRgn(hr, hr, orgn, RGN_XOR);
        try
        {
            RedrawWindow(DESKTOPHANDLE, 0, hr, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_VALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
        }
        catch (...)
        {
            ;
        }
        DeleteObject(hr);
        DeleteObject(orgn);

        TRect r = Rect(bv->Left + 1, 0, ClientWidth, pResults->Top);
        InvalidateRect(Handle, &r, false);
    }
    else
        roundbounds(ClientWidth, ClientHeight);

    if (poresults->count - poresults->pos - pbResults->Width / pbResults->Height < 0)
        poresults->pos = poresults->count - pbResults->Width / pbResults->Height;
    set_buttons();
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::FormHide(TObject *Sender)
{
    fbutton->Down = false;
    if (fposition == rpDefault || fresetpos)
    {
        savedleft = Left;
        savedtop = Top;
    }
    feditor->StopRecognize();
}
//---------------------------------------------------------------------------
extern bool ThemesAreTurnedOn;
void TfRecognize::cbgptr(TCanvas *c, TRect const & r)
{
    if (ThemesAreTurnedOn)
    {
        TRect r2 = Rect(bv->Left + 1, 0, ClientWidth, pResults->Top);
        OffsetRect(&r2, -r.Left, -r.Top);
        GradientFillCanvas(c, COLORINC(clBtnFace, 0x080808), COLORDEC(clBtnFace, 0x080808), r2, gdHorizontal);
    }
    else
    {
        c->FillRect(Rect(0, 0, r.Width(), r.Height()));
    }
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::FormPaint(TObject *Sender)
{
    if (!ThemesAreTurnedOn)
        return;

    TRect r = Rect(bv->Left + 1, 0, ClientWidth, pResults->Top);
    GradientFillCanvas(Canvas, COLORINC(clBtnFace, 0x080808), COLORDEC(clBtnFace, 0x080808), r, gdHorizontal);
}
//---------------------------------------------------------------------------
void TfRecognize::scallback()
{
    TIntegerList *l = new TIntegerList;
    try
    {
        if (sobj->Count)
            for (int ix = 0; ix < sobj->ResultCount; ++ix)
            {
                word val = elements->Items[sobj->Results[ix]]->owner;
                l->Add(val < NO_OWNER ? val : -sobj->Results[ix]);
            }
        poresults->set_items(l);
        set_buttons();
    }
    catch (...)
    {
        ;
    }
    delete l;

    if (fcallback)
        fcallback();
}
//---------------------------------------------------------------------------
void TfRecognize::pcallback()
{
    set_buttons();
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::btnResBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    MouseRepeat(btnResBackMouseDown, Sender, Button, Shift, X, Y);
    poresults->pos--;
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::btnResForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    MouseRepeat(btnResForeMouseDown, Sender, Button, Shift, X, Y);
    poresults->pos++;
}
//---------------------------------------------------------------------------
void TfRecognize::set_buttons()
{
    btnUndo->Enabled = sobj->Count;
    btnReset->Enabled = sobj->Count;
    btnResBack->Enabled = poresults->pos;
    btnResFore->Enabled = pbResults->Width / pbResults->Height + poresults->pos < poresults->count;
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::btnResetClick(TObject *Sender)
{
    Reset();
}
//---------------------------------------------------------------------------
void TfRecognize::Reset()
{
    sobj->reset();
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::btnUndoClick(TObject *Sender)
{
    sobj->undo();
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::FormShow(TObject *Sender)
{
    OnTop = true;
    fbutton->Down = true;
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::pbResultsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft && Button != mbRight)
        return;

    wchar_t k = poresults->CharAtPos(X);
    if (!k)
        return;
    if (Button == mbRight)
    {
        if (!fshowpopup)
            return;
        TPoint p = pbResults->ClientToScreen(Point(X, Y));
        miInfo->Enabled = KANJI(k);
        pmPanel->Tag = (int)k;
        pmPanel->Popup(p.x, p.y);
        return;
    }
    editor->Insert(k);
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key != VK_ESCAPE)
    {
        editor->SetFocus();
        PostMessage(editor->Handle, WM_KEYDOWN, Key, 0);
        //editor->pbKeyDown(Sender, Key, Shift);
        Key = 0;
    }
    else
        Hide();
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::FormKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key != VK_ESCAPE)
    {
        editor->SetFocus();
        //PostMessage(editor->Handle, WM_CHAR, Key, 0);
        //editor->pbKeyPress(Sender, Key);
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key != VK_ESCAPE)
    {
        editor->SetFocus();
        PostMessage(editor->Handle, WM_KEYUP, Key, 0);
        //editor->pbKeyDown(Sender, Key, Shift);
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::miCopyClick(TObject *Sender)
{
    ClipReplace((wchar_t)pmPanel->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::miAppendClick(TObject *Sender)
{
    ClipAppend((wchar_t)pmPanel->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::btnGenClick(TObject *Sender)
{
    sobj->GeneralChar = btnGen->Down;
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::btnGridClick(TObject *Sender)
{
    sobj->Grid = btnGrid->Down;
}
//---------------------------------------------------------------------------
void __fastcall TfRecognize::miInfoClick(TObject *Sender)
{
    ShowKanjiInfo(dictionary->Index[poresults->collection], kanjis->Kanjis[kanjis->IndexOf((wchar_t)pmPanel->Tag)]);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(129);
#endif


