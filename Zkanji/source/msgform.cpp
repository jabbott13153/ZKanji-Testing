//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "msgform.h"
#include "module.h"
#include "gengraph.h"
#include "popupdict.h"
#include "kanjilist.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(108);
#endif

TfMsg *fMsg;
//---------------------------------------------------------------------------

__fastcall TfMsg::TfMsg(TComponent* Owner) : TBaseForm(Owner)
{
    AppWindow = true;
    last = GetActiveWindow();
    data->ModalBegin(this, Handle);
    p = DisableTaskWindows(0);

    AppActivateEvent(this, &appactivate, true, true);

    if ((fPopupDict && fPopupDict->Visible) || (fKanji && fKanji->ontop))
    {
        ontop = true;
        OnTop = true;
    }
}
//---------------------------------------------------------------------------
__fastcall TfMsg::~TfMsg()
{
    AppActivateEvent(this, &appactivate, true, false);

    data->ModalEnd(Handle);
    EnableTaskWindows(p);

    if (fPopupDict->Visible)
        fPopupDict->LockPopup(true);
    Application->ProcessMessages();
    if (fPopupDict->Visible)
    {
        fPopupDict->LockPopup(false);
        SetActiveWindow(last);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfMsg::CreateParams(TCreateParams &Params)
{
    inherited::CreateParams(Params);
    if (MatchVersion(5, 1))
        Params.WindowClass.style = Params.WindowClass.style | CS_DROPSHADOW;
    if (NewStyleControls)
        Params.ExStyle = WS_EX_TOOLWINDOW;
    if (ontop)
        Params.ExStyle = WS_EX_TOPMOST;
}
//---------------------------------------------------------------------------
void __fastcall TfMsg::ShowEx(UnicodeString amsg, void (*abortcallback)(), UnicodeString abortcaption)
{
    lbMsg->Caption = amsg;
    abortproc = abortcallback;
    btnAbort->Visible = abortproc != NULL;
    lbMsg->Margins->Right = 3 * scaleres + (btnAbort->Visible ? 80 * scaleres : 0);
    if (abortproc != NULL && !abortcaption.IsEmpty())
        btnAbort->Caption = abortcaption;

    TCanvas *c = Canvas;
    TSize s;
    HFONT h = lbMsg->Font->Handle;
    HFONT of = (HFONT)SelectObject(c->Handle, h);
    GetTextExtentPoint(c->Handle, amsg.c_str(), amsg.Length(), &s);
    SelectObject(c->Handle, of);
    if (lbMsg->Width < s.cx + 32 * scaleres)
        Width += s.cx + 32 * scaleres - (lbMsg->Width);

    Show();
    Refresh();
    Activate();
    SetWindowPos(Handle, ontop ? HWND_TOPMOST : HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}
//---------------------------------------------------------------------------
void __fastcall TfMsg::FormPaint(TObject *Sender)
{
    TCanvas *c = Canvas;
    c->Pen->Color = clBlack;
    c->Rectangle(0, 0, ClientWidth, ClientHeight);
    TRect r = Rect(1, 1, ClientWidth - 2, ClientHeight - 2);
    DrawPillowPanel(Canvas, r);
}
//---------------------------------------------------------------------------
void TfMsg::appactivate(void *caller)
{
    SetWindowPos(Handle, ontop ? HWND_TOPMOST : HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}
//---------------------------------------------------------------------------
void __fastcall TfMsg::FormActivate(TObject *Sender)
{
    SetWindowPos(Handle, ontop ? HWND_TOPMOST : HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}
//---------------------------------------------------------------------------
void __fastcall TfMsg::btnAbortClick(TObject *Sender)
{
    abortproc();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ShowMessageForm(UnicodeString message, void (*abortcallback)(), UnicodeString abortcaption)
{
    if (fMsg)
        HideMessageForm();

    fMsg = new TfMsg(Application);
    fMsg->ShowEx(message, abortcallback, abortcaption);

    Application->ProcessMessages();
}
//---------------------------------------------------------------------------
void HideMessageForm()
{
    if (!fMsg)
        return;

    delete fMsg;
    fMsg = NULL;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(109);
#endif


