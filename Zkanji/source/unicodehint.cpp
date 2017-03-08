//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Themes.hpp>
#include <vsstyle.h>

#include "unicodehint.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(166);
#endif

//---------------------------------------------------------------------------
THintFormW *hint;

__fastcall THintFormW::THintFormW(TComponent* Owner) : fborders(true), fmargin(5*scaleres), timing(false), TCustomControl(Owner)
{
    Canvas->Font = Screen->HintFont;
    if (jfonts->IndexOf("MS Mincho"))
    {
        Canvas->Font->Name = L"MS Mincho";
        Canvas->Font->Charset = SHIFTJIS_CHARSET;
    }
    else
        SetSystemFont(Canvas->Font);
    Canvas->Font->Color = (TColor)GetSysColor(COLOR_INFOTEXT);
    Canvas->Brush->Style = bsClear;
    Canvas->Brush->Color = Color;

    bmp = new TBmp;
    bmp->Canvas->Font->Assign(Canvas->Font);
    bmp->Canvas->Brush->Assign(Canvas->Brush);
    bmp->Canvas->Pen->Assign(Canvas->Pen);

    ParentWindow = Application->Handle;
    ShowWindow(Handle, SW_HIDE); //DO NOT REMOVE! must hide after parentwindow change because for some reason the window is considered visible then.
}
//---------------------------------------------------------------------------
__fastcall THintFormW::~THintFormW()
{
    timing = false;
    KillTimer(Handle, 1);
    delete bmp;
}
//---------------------------------------------------------------------------
void __fastcall THintFormW::CreateParams(TCreateParams &Params)
{
    inherited::CreateParams(Params);

    Params.Style = WS_POPUP;

    Params.WindowClass.style = Params.WindowClass.style | CS_SAVEBITS;
    if (NewStyleControls)
        Params.ExStyle = WS_EX_TOOLWINDOW;

    Params.ExStyle |= WS_EX_TOPMOST | WS_EX_LAYERED;

    if (MatchVersion(5, 1))
        Params.WindowClass.style = Params.WindowClass.style | CS_DROPSHADOW;

    AddBiDiModeExStyle(Params.ExStyle);
}
//---------------------------------------------------------------------------
void THintFormW::Popup(TRect apos, TZPaintProc aonpaint, void *adata)
{
    BOOL b;
    SystemParametersInfo(SPI_GETTOOLTIPANIMATION, 0, &b, 0);

    timing = false;
    KillTimer(Handle, 1);
    Visible = false;

    //RecreateWnd();
    //Application->ProcessMessages();

    bmp->SetSize(apos.Width(), apos.Height());

    TRect r = Rect(0, 0, apos.Width(), apos.Height());
    GradientFillCanvas(bmp->Canvas, (TColor)0xffffff, (TColor)0xf8f8f8, r, gdVertical);
    if (aonpaint)
        aonpaint(this, bmp->Canvas, r, adata);

    xpos = apos.left;
    ypos = apos.top;

    tagMONITORINFO inf = MonitorInfo(apos.Left, apos.Top);

    int ch = GetSystemMetrics(SM_CYCURSOR) * 2 / 3;
    int w = apos.Width() + (fborders ? fmargin * 2 : 0);
    int h = apos.Height() + (fborders ? fmargin * 2 : 0);
    int l = MAX((int)inf.rcMonitor.left, MIN((int)apos.left, (int)inf.rcMonitor.right - w));
    int t;
    if (apos.top + h + ch >= inf.rcMonitor.bottom)
        t = apos.top - h - 4;
    else
        t = apos.top + ch;
    SetBounds(l, t, w, h);
    SetWindowPos(Handle, HWND_TOPMOST, Left, Top, Width, Height, SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    HRGN rgn = CreateRoundRectRgn(0, 0, Width + 1, Height + 1, 9, 9);
    SetWindowRgn(Handle, rgn, false);
    ::SetLayeredWindowAttributes(Handle, 0, b == TRUE ? 0 : 255, LWA_ALPHA);
    Visible = true;

    if (b == TRUE)
    {
        alpha = 0;
        showing = true;
        timing = true;
        SetTimer(Handle, 1, 30, NULL);
    }
}
//---------------------------------------------------------------------------
void __fastcall THintFormW::WMTimer(TWMTimer &msg)
{
    if (!timing)
        return;
    if (showing)
    {
        alpha = min(alpha + 51, 255);
        if (alpha == 255)
        {
            timing = false;
            KillTimer(Handle, 1);
        }
    }
    else
    {
        alpha = max(alpha - 51, 0);
        if (alpha == 0)
        {
            timing = false;
            KillTimer(Handle, 1);
            Visible = false;
        }
    }
    ::SetLayeredWindowAttributes(Handle, 0, alpha, LWA_ALPHA);
}
//---------------------------------------------------------------------------
void __fastcall THintFormW::WMNCHitTest(TWMNCHitTest &msg)
{
    msg.Result = HTTRANSPARENT;
}
//---------------------------------------------------------------------------
void __fastcall THintFormW::Erase(TMessage &msg)
{
    msg.Result = 1;
}
//---------------------------------------------------------------------------
void __fastcall THintFormW::Paint()
{
    TCanvas *c = Canvas;
    c->Brush->Style = bsSolid;

    HRGN rgn = CreateRoundRectRgn(0, 0, Width + 1, Height + 1, 9, 9);
    HRGN rgn2 = CreateRoundRectRgn(fmargin, fmargin, Width - fmargin + 1, Height - fmargin + 1, 4, 4);
    CombineRgn(rgn, rgn, rgn2, RGN_DIFF);
    SelectClipRgn(c->Handle, rgn);
    DeleteObject(rgn);

    GradientFillCanvas(c, (TColor)0xeaf5f5, (TColor)0xd2dada, ClientRect, gdVertical);

    SelectClipRgn(c->Handle, rgn2);
    DeleteObject(rgn2);

    SelectClipRgn(c->Handle, NULL);
    c->Brush->Style = bsClear;
    c->Pen->Color = (TColor)0xa0a0a0;
    RoundRect(c->Handle, 0, 0, Width, Height, 9, 9);

    TRect r = ClientRect;

    if (fborders)
        InflateRect(&r, -fmargin, -fmargin);

    Canvas->Draw(r.Left, r.Top, bmp);
}
//---------------------------------------------------------------------------
void THintFormW::Hide(bool immediate)
{
    BOOL b;
    SystemParametersInfo(SPI_GETTOOLTIPANIMATION, 0, &b, 0);

    if (b == TRUE && !immediate)
    {
        showing = false;
        timing = true;
        SetTimer(Handle, 1, 40, NULL);
    }
    else
        Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall THintFormW::WMNCPaint(TMessage &msg)
{
    msg.Result = 0;
}

//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(167);
#endif

