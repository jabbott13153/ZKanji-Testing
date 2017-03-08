//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "warningbubble.h"
#include "closebutton.h"
#include "settings.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(174);
#endif

TfBubble *fBubble;

const TColor bubble_border_color = (TColor)0x00AE7600;
const int bubble_showing_time = 6; //in seconds
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void BubbleWarning(int num)
{
    if (ScreenSaverOn() || fBubble)
        return;

    fBubble = NULL;
    try
    {
        fBubble = new TfBubble(Application);
        fBubble->lbDue->Caption = num;
        fBubble->Left = Screen->WorkAreaLeft + Screen->WorkAreaWidth - fBubble->Width - 10;
        fBubble->Top = Screen->WorkAreaTop + Screen->WorkAreaHeight - fBubble->Height - 30;
        fBubble->Show();
        profilesettings.klastwarning = Now();
    }
    catch (...)
    {
        delete fBubble;
        fBubble = NULL;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TfBubble::TfBubble(TComponent* Owner) : state(bssShowing), TBaseForm(Owner)
{
    ActivateOnShow = false;
    OnTop = true;
    NoOwner = true;

    lb2->Left = lb1->Left + lb1->Width;
    tTimer->Tag = (1000 / tTimer->Interval) * bubble_showing_time;

    btnClose = new TSmallCloseButton(this, sbpTopRight, 4, 4, &cbgptr);
}
//---------------------------------------------------------------------------
__fastcall TfBubble::~TfBubble()
{
    delete btnClose;
}
//---------------------------------------------------------------------------
void TfBubble::cbgptr(TCanvas *c, TRect const &r)
{
    c->Brush->Color = clWhite;
    c->FillRect(r);
}
//---------------------------------------------------------------------------
void __fastcall TfBubble::FormPaint(TObject *Sender)
{
    TCanvas *c = Canvas;
    c->Pen->Width = 2;
    c->Pen->Color = bubble_border_color;
    c->MoveTo(-1, 1);
    c->LineTo(ClientWidth - 1, 1);
    c->LineTo(ClientWidth - 1, ClientHeight - 1);
    c->LineTo(1, ClientHeight - 1);
    c->LineTo(1, 1);
}
//---------------------------------------------------------------------------
void __fastcall TfBubble::FormShow(TObject *Sender)
{
    tTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfBubble::tTimerTimer(TObject *Sender)
{
    if (state == bssShowing)
    {
        AlphaBlendValue = AlphaBlendValue + 5;
        if (AlphaBlendValue == 255)
            state = bssNormal;
    }
    else if (state == bssNormal)
    {
        if (tTimer->Tag > 0)
            tTimer->Tag--;
        else
            state = bssHiding;
    }
    else if (state == bssHiding)
    {
        AlphaBlendValue = AlphaBlendValue - 5;
        if (AlphaBlendValue == 0)
        {
            tTimer->Enabled = false;
            Close();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfBubble::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caFree;
    fBubble = NULL;
}
//---------------------------------------------------------------------------
void TfBubble::WMEnable(TMessage &msg)
{
    if (!msg.WParam)
        PostMessage(Handle, WM_Z_ENABLE, 0, 0);
    else
        msg.Result = DefWindowProc(Handle, msg.Msg, msg.WParam, msg.LParam);
}
//---------------------------------------------------------------------------
void TfBubble::WMZEnable(TMessage &msg)
{
    EnableWindow(Handle, true);
    SetWindowPos(Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(175);
#endif

