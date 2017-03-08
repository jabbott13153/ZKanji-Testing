//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "clipview.h"
#include "main.h"
#include "settings.h"
#include "wordlist.h"
#include "kanjiedit.h"
#include "modallist.h"
#include "module.h"
#include "closebutton.h"
#include "wordlist.h"
#include "popupdict.h"
#include "collection.h"
#include "docker.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(14);
#endif

TfClipboard *fClipboard;
//---------------------------------------------------------------------------

void TfClipboard::WMNCActivate(TMessage &msg)
{
    msg.Result = true;
}
//---------------------------------------------------------------------------
__fastcall TfClipboard::TfClipboard(TComponent* Owner) : oldsize(0, 0, 0, 0), ftext(L""),
        fselstart(0), fsellength(0), fselecting(false), sizex(0), sizey(0), TBaseForm(Owner)
{
    //RescaleStatusbar(sbInfo);

    ActivateOnShow = false;
    EraseBackground = false;

    KeepSize = true;

    //OnTop = true;

    closebtn = new TCloseButton(this, sbpTopRight, 3, 2, &cbgptr);

    Shadowed = true;

    AppActivateEvent(this, repaintProc, true, true);
    AppActivateEvent(this, repaintProc, false, true);
    AddEvent(this, repaintProc, eltActivate);
    AddEvent(this, repaintProc, eltDeactivate);
}
//---------------------------------------------------------------------------
__fastcall TfClipboard::~TfClipboard()
{
    AppActivateEvent(this, repaintProc, false, false);
    AppActivateEvent(this, repaintProc, true, false);
    RemoveEvent(this, repaintProc, eltDeactivate);
    RemoveEvent(this, repaintProc, eltActivate);

    Shadowed = false;
    delete closebtn;
}
//---------------------------------------------------------------------------
void TfClipboard::repaintProc(void *caller)
{
    if (!Visible)
        return;

    SendMessage(Handle, WM_NCPAINT, 0, 0);
    closebtn->Redraw();
    TRect r = Rect(0, 0, ClientWidth, bv1->Top);
    RedrawWindow(Handle, &r, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_ALLCHILDREN);
    /*
     InvalidateRect(Handle,&r,false);
     cbAuto->Invalidate();*/
}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::btnGetClick(TObject *Sender)
{
    UnicodeString w; //clipboard text
    UnicodeString c; //current word
    fsellength = 0;
    btnTranslate->Enabled = false;
    GetClipText(w, 2048);
    int s = 1; //start-end position

    while (s <= w.Length())
    {
        if (w[s] <= 0x0020)
        {
            if (s <= 1 || w[s - 1] != L' ')
                c += L" ";
            s++;
        }
        else
            c += w[s++];
    }

    ftext = c;
    sbInfo->Panels->Items[1]->Text = c.Length();

    pbLines->VPos = 0;
    measure_box_size();
    pbLines->Refresh();
}
//---------------------------------------------------------------------------
void TfClipboard::measure_box_size()
{
    if (!sizex || !sizey)
        return;

    int n = 0;

    cols = (pbLines->ClientWidth - 4) / sizex;
    rows = (pbLines->ClientHeight - 4) / sizey;

    if (cols)
        n = (ftext.Length() + cols - 1) / cols;

    pbLines->VertScrollBar->Enabled = (n > rows || pbLines->VMax < pbLines->ClientHeight + 1);
    if (n > rows)
    {
        pbLines->VMax = n - 1;
        pbLines->VertScrollBar->ThumbSize = rows;
        pbLines->VertScrollBar->LargeChange = MAX(1, rows - 1);
    }
    else
    {
        pbLines->VPos = 0;
        pbLines->VMax = 9999;
        pbLines->VertScrollBar->Enabled = false;
    }

}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::pbLinesPaint(TObject *Sender)
{
    TCanvas *c = pbLines->Canvas;

    c->Brush->Color = clWhite;
    //fill edges
    TRect r;
    c->FillRect(Rect(0, 0, pbLines->ClientWidth, 2));
    c->FillRect(Rect(0, pbLines->ClientHeight - 2, pbLines->ClientWidth, pbLines->ClientHeight));
    c->FillRect(Rect(0, 2, 2, pbLines->ClientHeight - 2));
    c->FillRect(Rect(pbLines->ClientWidth - 2, 2, pbLines->ClientWidth, pbLines->ClientHeight - 2));

    SetTextAlign(c->Handle, TA_CENTER | TA_TOP);

    //draw the text
    int ch = cols * pbLines->VPos;
    int len = ftext.Length();
    int x = 2;
    int y = 2;
    while (ch < len && y < pbLines->ClientHeight - 2)
    {
        c->Brush->Color = (ch < selstart || ch >= selstart + sellength ? clWhite : clHighlight);
        c->Font->Color = (ch < selstart || ch >= selstart + sellength ? clBlack : clHighlightText);
        r = Rect(x, y, x + sizex, y + sizey);
        if ((int)(ftext.c_str()[ch]) < 0xbb8)
        {
            c->Font->Name = settings.romajifont;
            c->Font->Charset = DEFAULT_CHARSET;
            c->Font->Size = 14;
        }
        else
        {
            c->Font->Name = settings.kanafont;
            c->Font->Charset = settings.kanacharset;
            c->Font->Size = 16;
        }
        ExtTextOutW(c->Handle, x + sizex / 2, y, ETO_CLIPPED | ETO_OPAQUE, &r, ftext.c_str() + ch, 1, NULL);
        ch++;
        x += sizex;
        if (x + sizex > pbLines->ClientWidth - 2 && ch < len)
        {
            x = 2;
            y += sizey;
        }
    }
    c->Brush->Color = clWhite;
    c->FillRect(Rect(x, y, pbLines->ClientWidth - 2, y + sizey));
    c->FillRect(Rect(2, y + sizey, pbLines->ClientWidth - 2, pbLines->ClientHeight - 2));
}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::pbLinesResize(TObject *Sender)
{
    measure_box_size();
    pbLines->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::btnClearClick(TObject *Sender)
{
    SetClipText(L"");
    btnGetClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::pbLinesMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    fselstart = MAX(0, (int)MIN((double)ftext.Length(), (y / sizey + pbLines->VPos) * cols + (x + sizex * 0.5) / sizex));
    btnTranslate->Enabled = false;
    if (fsellength)
    {
        fsellength = 0;
        pbLines->Refresh();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::pbLinesMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    if (!Shift.Contains(ssLeft))
        return;
    int a = MAX(0, (int)MIN((double)ftext.Length(), (y / sizey + pbLines->VPos) * cols + (x + sizex * 0.5) / sizex));
    if (a != fselstart)
    {
        fsellength += (fselstart - a);
        fselstart = a;
        btnTranslate->Enabled = !cbAuto->Checked && (sellength > 0);

        if (cbAuto->Checked)
            btnTranslateClick(Sender);
        pbLines->Refresh();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::btnTranslateClick(TObject *Sender)
{
    bool jap = false;
    UnicodeString c;
    c = ftext.SubString(selstart + 1, sellength);
    if (!c.Length())
        return;
    for (int i = 1; i <= c.Length(); i++)
    {
        if (JAPAN(c[i]))
        {
            jap = true;
            break;
        }
    }
    if (!jap)
    {
        if (!fDict->btnRomaji->Down)
            fDict->btnRomaji->Click();
        fDict->edRomaji->Text = c.c_str();
        fDict->edRomajiChange(fDict->edRomaji);
    }
    else
    {
        if (!fDict->btnKanji->Down)
            fDict->btnKanji->Click();
        fDict->edkanji->Text = c;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::cbAutoClick(TObject *Sender)
{
    fsellength = 0;
    pbLines->Refresh();
    btnTranslate->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::FormShow(TObject *Sender)
{
    btnGet->Click();
    roundbounds(Width, Height);
}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::pbLinesKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == 0x03 && sellength) // Ctrl-C
        SetClipText(ftext.SubString(selstart + 1, sellength));
}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::pbLinesKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
//    switch (Key)
//    {
//    case ('C'):
//        if (Shift.Contains(ssCtrl) && sellength)
//            SetClipText(ftext.SubString(selstart + 1, sellength));
//        break;
//    }
}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    /*if (Shift.Contains(ssCtrl) && Key == VK_TAB) {
     ChangeFocus(Shift.Contains(ssShift));
     return;
     } else*/ if (Key == VK_ESCAPE)
        Close();
    else if (Key >= '1' && Key <= '9' && Shift.Contains(ssAlt))
    {
        int coll = Key - '1';
        if (coll < dictionary->Count && settings.dictionary_index != coll)
            fDict->SelectDictionary(coll);
    }
    else if (!modallist || !modallist->Count)
    {
        switch (Key)
        {
        case VK_F2:
            if (!settings.intray)
            {
                //if (settings.docked)
                fDock->SetFocus();
                //else
                //fDict->SetFocus();
                fDict->btnKanji->Click();
            }
            else
            {
                fPopupDict->SetFocus();
                fPopupDict->btnKanji->Click();
            }
            break;
        case VK_F3:
            if (!settings.intray)
            {
                //if (settings.docked)
                fDock->SetFocus();
                //else
                //fDict->SetFocus();
                fDict->btnRomaji->Click();
            }
            else
            {
                fPopupDict->SetFocus();
                fPopupDict->btnRomaji->Click();
            }
            break;
        case VK_F4:
            if (!settings.intray)
            {
                //if (settings.docked)
                fDock->SetFocus();
                //else
                //fDict->SetFocus();
                fDict->btnJapanese->Click();
            }
            else
            {
                fPopupDict->SetFocus();
                fPopupDict->btnJapanese->Click();
            }
            break;
        }
    }
}
//---------------------------------------------------------------------------
int TfClipboard::get_selstart()
{
    if (fsellength < 0)
        return (fselstart + fsellength);
    if (fsellength > 0)
        return (fselstart);
    return 0;
}
//---------------------------------------------------------------------------
int TfClipboard::get_sellength()
{
    return abs(fsellength);
}
//---------------------------------------------------------------------------
void TfClipboard::Show()
{
    Recreate(true);

    TCanvas *c = pbLines->Canvas;
    TSize s;
    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = 16;
    GetTextExtentPoint32W(c->Handle, L"め", 1, &s);
    sizex = s.cx + 2;
    sizey = s.cy + 2;

    SetWindowPos(Handle, NULL, 0, 0, Width - ((pbLines->ClientWidth - 4) % sizex), Height - ((pbLines->ClientHeight - 4) % sizey), SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    measure_box_size();

    if (ClientHeight <= 0)
        ClientHeight = bv1->Top + sbInfo->Height + sizey;

    Constrain(MAX(Width - ClientWidth + cbAuto->Left + cbAuto->Width + 66*scaleres, Width - pbLines->ClientWidth + sizex*12), Height - pbLines->ClientHeight + sizey*5, 200000, 200000, Width - pbLines->ClientWidth + 4, sizex, Height - pbLines->ClientHeight + 4, sizey);
    oldsize = Rect(Left, Top, Left + Width, Top + Height);

    roundbounds(Width, Height);
    //the first call of SetWindowRgn after the window was shown causes flicker, so make that "flicker" happen now instead of when first changing the window shape for real
    HRGN rgn = CreateRectRgn(0, 0, 0, 0);
    int err = GetWindowRgn(Handle, rgn) == ERROR;
    HRGN rgn2 = CreateRectRgn(0, 0, 0, 0);
    SetWindowRgn(Handle, rgn2, false);
    Visible = true;
    SetWindowRgn(Handle, err ? NULL : rgn, true);
    if (err)
        DeleteObject(rgn);

}
//---------------------------------------------------------------------------
extern bool ThemesAreTurnedOn;
void TfClipboard::cbgptr(TCanvas *c, TRect const &r)
{
    if (ThemesAreTurnedOn)
    {
        TRect r2 = Rect(0, 0, r.Width(), bv1->Top);
        OffsetRect(&r2, 0, -r.Top);
        //DrawPillowPanel(c,r2);
        DrawCaptionBar(c, r2, Active);
    }
    else
    {
        c->FillRect(Rect(0, 0, r.Width(), r.Height()));
    }
}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::FormPaint(TObject *Sender)
{
    //DrawPillowPanel(this,NULL,bv1);
    DrawCaptionBar(this, NULL, bv1, Active);
}
//---------------------------------------------------------------------------
void TfClipboard::WMNCCalcSize(TMessage &msg)
{
    if (!msg.WParam)
    {
        TRect *r = ((TRect*)msg.LParam);
        InflateRect(r, -4, -4);
        msg.Result = 0;
    }
    else
    {
        NCCALCSIZE_PARAMS *n = ((NCCALCSIZE_PARAMS*)msg.LParam);
        memmove(n->rgrc + 1, n->rgrc, sizeof(tagRECT)*2);
        InflateRect(n->rgrc, -4, -4);
        msg.Result = WVR_VALIDRECTS;
    }
}
//---------------------------------------------------------------------------
void TfClipboard::WMNCHitTest(TMessage &msg)
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
void TfClipboard::WMNCPaint(TMessage &msg)
{
    HDC hdc;
    hdc = GetWindowDC(Handle);
    if (!hdc)
        return;

    TRect r;
    HPEN p;
    HPEN pold;
    int c = (int)ColorToRGB(clBtnFace);
    bool d = (c & 0xff) >= BTN_DARK_TRESH && ((c >> 8) & 0xff) >= BTN_DARK_TRESH && ((c >> 16) & 0xff) >= BTN_DARK_TRESH;

    GetWindowRect(Handle, &r);
    OffsetRect(r, -r.Left, -r.Top);
    ExcludeClipRect(hdc, 4, 4, r.Width() - 4, r.Height() - 4);

    TColor pcol;

    pcol = d ? COLORDEC(clBtnFace, 0x101010) : COLORINC(clBtnFace, 0x101010);
    if (Active)
        pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.2);
    p = CreatePen(PS_SOLID, 0, pcol);
    pold = (HPEN)SelectObject((HDC)hdc, p);

    TPoint pts[5] = { Point(3, r.Height() - 4), Point(3, 3), Point(r.Width() - 4, 3), Point(r.Width() - 4, r.Height() - 4), Point(3, r.Height() - 4) };
    Polyline(hdc, pts, 5);
    SelectObject(hdc, pold);
    DeleteObject(p);

    pcol = d ? COLORDEC(clBtnFace, 0x202020) : COLORINC(clBtnFace, 0x202020);
    if (Active)
        pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.4);
    p = CreatePen(PS_SOLID, 0, pcol);
    pold = (HPEN)SelectObject((HDC)hdc, p);

    TPoint pts2[5] = { Point(2, r.Height() - 3), Point(2, 2), Point(r.Width() - 3, 2), Point(r.Width() - 3, r.Height() - 3), Point(2, r.Height() - 3) };
    Polyline(hdc, pts2, 5);
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
void TfClipboard::roundbounds(int fw, int fh)
{
    HRGN rgn = CreateRoundRectRgn(0, 0, fw + 1, fh + 1, 9, 9);
    SetWindowRgn(Handle, rgn, Visible);
}
//---------------------------------------------------------------------------
void __fastcall TfClipboard::FormResize(TObject *Sender)
{
    if (!Visible)
        return;

    roundbounds(Width, Height);

    TRect r = Rect(Left, Top, Left + Width, Top + Height), r2;

    r2 = Rect(oldsize.Left, oldsize.Top, oldsize.Left + 4, oldsize.Top + 4);
    RedrawWindow(DESKTOPHANDLE, &r2, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    r2 = Rect(oldsize.Right - 4, oldsize.Top, oldsize.Right, oldsize.Top + 4);
    RedrawWindow(DESKTOPHANDLE, &r2, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    r2 = Rect(oldsize.Left, oldsize.Bottom - 4, oldsize.Left + 4, oldsize.Bottom);
    RedrawWindow(DESKTOPHANDLE, &r2, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    r2 = Rect(oldsize.Right - 4, oldsize.Bottom - 4, oldsize.Right, oldsize.Bottom);
    RedrawWindow(DESKTOPHANDLE, &r2, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);

    r2 = Rect(r.Left, r.Top, r.Left + 4, r.Top + 4);
    RedrawWindow(DESKTOPHANDLE, &r2, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    r2 = Rect(r.Right - 4, r.Top, r.Right, r.Top + 4);
    RedrawWindow(DESKTOPHANDLE, &r2, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    r2 = Rect(r.Left, r.Bottom - 4, r.Left + 4, r.Bottom);
    RedrawWindow(DESKTOPHANDLE, &r2, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    r2 = Rect(r.Right - 4, r.Bottom - 4, r.Right, r.Bottom);
    RedrawWindow(DESKTOPHANDLE, &r2, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);

    if (oldsize.Height() < r.Height())
    {
        r2 = Rect(0, ClientHeight - (r.Height() - oldsize.Height()), ClientWidth, ClientHeight);
        RedrawWindow(Handle, &r2, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }
    if (oldsize.Width() < r.Width())
    {
        r2 = Rect(ClientWidth - (r.Width() - oldsize.Width()), 0, ClientWidth, ClientHeight);
        RedrawWindow(Handle, &r2, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }

    oldsize = Rect(Left, Top, Left + Width, Top + Height);

    closebtn->Redraw();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ClipAppend(AnsiString const &s)
{
    ClipAppend(UnicodeString(s));
}
//---------------------------------------------------------------------------
void ClipAppend(wchar_t const &c)
{
    ClipAppend(UnicodeString(c));
}
//---------------------------------------------------------------------------
void ClipAppend(wchar_t *c)
{
    ClipAppend(UnicodeString(c));
}
//---------------------------------------------------------------------------
void ClipAppend(UnicodeString const &s)
{
    UnicodeString str;
    GetClipText(str, 0);
    SetClipText(str + s);
    if (settings.showclipappend && !settings.intray && !modallist->Count)
        ShowClipboard();
}
//---------------------------------------------------------------------------
void ClipReplace(AnsiString const &s)
{
    ClipReplace(UnicodeString(s));
}
//---------------------------------------------------------------------------
void ClipReplace(wchar_t const &c)
{
    ClipReplace(UnicodeString(c));
}
//---------------------------------------------------------------------------
void ClipReplace(wchar_t *c)
{
    ClipReplace(UnicodeString(c));
}
//---------------------------------------------------------------------------
void ClipReplace(UnicodeString const &s)
{
    SetClipText(s);
    if (settings.showclipcopy && !settings.intray && !modallist->Count)
        ShowClipboard();
}
//---------------------------------------------------------------------------
void ShowClipboard()
{
    if (!fClipboard->Visible)
    {
        fClipboard->Show();
        //fClipboard->OnTop = true;
    }
    else
    {
        fClipboard->btnGet->Click();
    }
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(15);
#endif


