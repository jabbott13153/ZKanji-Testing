//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "sodpopup.h"
#include "kanjiobj.h"
#include "wordstudytest.h"
#include "mouseevent.h"
#include "settings.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(150);
#endif

TfSOD *fSOD;
//---------------------------------------------------------------------------
__fastcall TfSOD::TfSOD(TComponent* Owner) : factivating(false), wnd(NULL), TBaseForm(Owner)
{
    kobj = new TKanjiDrawObj(pb, &kproc);
    kobj->strokeorder = true;
    kobj->strokeshadow = true;
    kobj->strokenumbers = settings.ltsodnumber;
    kobj->strokespeed = dsFastest;
    kobj->strokewidth = 4.4;
    kobj->strokemargin = 2;

    sbNum->Flat = !MatchVersion(6, 0);

    sbNum->Down = settings.ltsodnumber;

    AppActivateEvent(this, activate, true, true);
    AppActivateEvent(this, deactivate, false, true);

    int fw = ClientWidth;
    int fh = ClientHeight;
    HRGN rgn = CreateRoundRectRgn(0, 0, fw + 1, fh + 1, 9, 9);
    SetWindowRgn(Handle, rgn, false);

    SetWindowPos(Handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);

    PostMessage(Handle, WM_Z_INIT, 0, 0);
}
//---------------------------------------------------------------------------
void TfSOD::z_init(TMessage &msg)
{
    ActivateOnShow = false;
}
//---------------------------------------------------------------------------
__fastcall TfSOD::~TfSOD()
{
    AppActivateEvent(this, deactivate, false, false);
    AppActivateEvent(this, activate, true, false);
    delete kobj;
}
//---------------------------------------------------------------------------
void TfSOD::kproc(TDrawStrokeState state)
{
    switch (state)
    {
    case dssStopped:
        sbStop->Down = true;
        sbPause->GroupIndex = 0;
        //lbStroke->Caption = (kobj->strokeposition < 9 ? "0" : "")+IntToStr(kobj->strokeposition+1);
        break;
    case dssStarted:
        sbPause->GroupIndex = 1;
        sbPlay->Down = true;
        break;
    case dssStroke:
                //lbStroke->Caption = (kobj->strokeposition < 9 ? "0" : "")+IntToStr(kobj->strokeposition+1);

        if (!sbStop->Down)
            break;
    case dssPaused:
        sbPause->GroupIndex = 1;
        sbPause->Down = true;
        break;
    }
}
//---------------------------------------------------------------------------
void TfSOD::deactivate(void *caller)
{
    if (!wnd)
        return;

    //Hide();
    PostMessage(wnd->Handle, WM_NCACTIVATE, false, 0);
}
//---------------------------------------------------------------------------
void TfSOD::activate(void *caller)
{
    if (!wnd)
        return;

    factivating = true;
    //Application->BringToFront();
    try
    {
        SetWindowPos(wnd->Handle, HWND_TOP, 0, 0, 0, 0, SWP_DEFERERASE | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE);
        SetWindowPos(Handle, HWND_TOP, 0, 0, 0, 0, SWP_DEFERERASE | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE);
        PostMessage(wnd->Handle, WM_NCACTIVATE, true, 0);
    }
    catch (...)
    {
        ;
    }
    factivating = false;
}
//---------------------------------------------------------------------------
void __fastcall TfSOD::FormMouseLeave(TObject *Sender)
{
    TPoint p;
    GetCursorPos(&p);
    HWND h = ChildWindowFromPointEx(GetDesktopWindow(), p, CWP_SKIPINVISIBLE | CWP_SKIPDISABLED);
    if (h == Handle)
        return;

    if (fproc)
        fproc();
}
//---------------------------------------------------------------------------
void TfSOD::ShowEx(TBaseForm *form, const TRect &pos, TFuncPtr aproc, wchar_t kanji)
{
    wnd = form;
    kobj->kanji = kanji;
    fproc = aproc;

    topside = pos.Top - Height >= 2;
    Left = (pos.Left + pos.Width() / 2) - Width / 2;
    Top = topside ? pos.Top - Height : pos.Bottom;
    if (topside)
    {
        pb->Top = 1;
        pan->Top = pb->Top + pb->Height;
    }
    else
    {
        pan->Top = 3;
        pb->Top = pan->Top + pan->Height;
    }

    rwidth = pos.Width();
    rpos = ScreenToClient(Point(pos.Left, 0)).x;

    Show();
}
//---------------------------------------------------------------------------
void __fastcall TfSOD::FormPaint(TObject *Sender)
{
    TCanvas *c = Canvas;
    c->Brush->Color = clWhite;
    c->Pen->Color = clBlack;
    c->Brush->Style = bsClear;

    ExcludeClipRect(c, Rect(rpos + 1, topside ? ClientHeight - 1 : 0, rpos + rwidth - 2, topside ? ClientHeight : 1));
    RoundRect(c->Handle, 0, 0, ClientWidth, ClientHeight, 9, 9);
    ClearClipRect(c);

    //c->FillRect(Rect(1,topside ? pb->Top+pb->Height : 1,ClientWidth-1,topside ? ClientHeight-1 : pb->Top));

    c->Pen->Color = clWhite;
    c->MoveTo(rpos + 1, topside ? ClientHeight - 1 : 0);
    c->LineTo(rpos + rwidth - 1, topside ? ClientHeight - 1 : 0);

    c->Brush->Style = bsSolid;
}
//---------------------------------------------------------------------------
void __fastcall TfSOD::sbStopClick(TObject *Sender)
{
    kobj->Stop();
}
//---------------------------------------------------------------------------
void __fastcall TfSOD::sbPlayClick(TObject *Sender)
{
    if (kobj->element >= 0)
        kobj->Play();
}
//---------------------------------------------------------------------------
void __fastcall TfSOD::sbPauseClick(TObject *Sender)
{
    kobj->Pause();
}
//---------------------------------------------------------------------------
void __fastcall TfSOD::sbRewClick(TObject *Sender)
{
    kobj->StrokeRewind();
}
//---------------------------------------------------------------------------
void __fastcall TfSOD::sbFFClick(TObject *Sender)
{
    kobj->StrokeFastForward();
}
//---------------------------------------------------------------------------
void __fastcall TfSOD::sbBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft)
        return;
    MouseRepeat(sbBackMouseDown, Sender, Button, Shift, X, Y);
    kobj->StrokeBack();
}
//---------------------------------------------------------------------------
void __fastcall TfSOD::sbForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft)
        return;
    MouseRepeat(sbForeMouseDown, Sender, Button, Shift, X, Y);
    kobj->StrokeForward();
}
//---------------------------------------------------------------------------
void __fastcall TfSOD::FormHide(TObject *Sender)
{
    sbStop->Click();
    wnd = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfSOD::sbNumClick(TObject *Sender)
{
    kobj->strokenumbers = sbNum->Down;
    settings.ltsodnumber = sbNum->Down;
}
//---------------------------------------------------------------------------
void TfSOD::z_pos(TWMWindowPosChanging &msg)
{
    msg.Result = DefWindowProc(Handle, WM_WINDOWPOSCHANGING, (*(TMessage*)(&msg)).WParam, (*(TMessage*)(&msg)).LParam);
    if (!wnd || factivating)
        return;

    HWND h = GetWindow(wnd->Handle, GW_HWNDPREV);
    if (!h)
        h = HWND_TOP;
    msg.WindowPos->hwndInsertAfter = h;
    msg.WindowPos->flags = msg.WindowPos->flags & ~(SWP_NOOWNERZORDER | SWP_NOZORDER);

}
//---------------------------------------------------------------------------
void __fastcall TfSOD::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose = false;
}
//---------------------------------------------------------------------------
void __fastcall TfSOD::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (!wnd)
        return;

    SendMessage(wnd->Handle, WM_KEYDOWN, Key, MapVirtualKey(Key, MAPVK_VK_TO_VSC) << 16);
}
//---------------------------------------------------------------------------
void __fastcall TfSOD::FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (!wnd)
        return;

    SendMessage(wnd->Handle, WM_KEYUP, Key, (MapVirtualKey(Key, MAPVK_VK_TO_VSC) << 16) & (3 << 30));
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(151);
#endif

