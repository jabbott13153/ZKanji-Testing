//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "numinput.h"
#include "numedit.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(120);
#endif

TfNumInput *fNumInput;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TfNumInput::TfNumInput(TComponent* Owner) : TBaseForm(Owner)
{
    numedit = 0;
    floatedit = 0;
}
//---------------------------------------------------------------------------
__fastcall TfNumInput::~TfNumInput()
{
}
//---------------------------------------------------------------------------
int TfNumInput::ShowModalEx(UnicodeString ACaption, UnicodeString AMessage, int &aval, int amin, int amax)
{
    numedit = new TNumEdit(edNum, amin, amax);
    try
    {
        Caption = ACaption;
        lbMsg->Caption = AMessage;
        if (amin <= amax)
            lbInterval->Caption = (UnicodeString)L"Valid range: " + amin + " - " + amax;
        else
            lbInterval->Caption = "";
        edNum->Text = max(min(aval, amax), amin);

        ShowModal();
    }
    catch (...)
    {
        ModalResult = mrCancel;
    }

    if (ModalResult == mrOk)
        aval = numedit->value;
    delete numedit;
    numedit = 0;

    return ModalResult;
}
//---------------------------------------------------------------------------
int TfNumInput::ShowModalEx(UnicodeString ACaption, UnicodeString AMessage, double &aval, double amin, double amax)
{
    floatedit = new TFloatEdit(edNum, amin, amax);
    try
    {
        Caption = ACaption;
        lbMsg->Caption = AMessage;
        if (amin <= amax)
            lbInterval->Caption = (UnicodeString)L"Valid range: " + amin + " - " + amax;
        else
            lbInterval->Caption = "";
        edNum->Text = max(min(aval, amax), amin);

        ShowModal();
    }
    catch (...)
    {
        ModalResult = mrCancel;
    }

    if (ModalResult == mrOk)
        aval = floatedit->value;
    delete floatedit;
    floatedit = 0;

    return ModalResult;
}
//---------------------------------------------------------------------------
bool InputQueryInt(UnicodeString AMessage, UnicodeString ACaption, int &Value, int AMin, int AMax)
{
    fNumInput = new TfNumInput(Application);
    int res;
    try
    {
        res = fNumInput->ShowModalEx(ACaption, AMessage, Value, AMin, AMax);
    }
    catch (...)
    {
        res = mrCancel;
    }
    delete fNumInput;

    return res == mrOk;
}
//---------------------------------------------------------------------------
bool InputQueryFloat(UnicodeString AMessage, UnicodeString ACaption, double &Value, double AMin, double AMax)
{
    fNumInput = new TfNumInput(Application);
    int res;
    try
    {
        res = fNumInput->ShowModalEx(ACaption, AMessage, Value, AMin, AMax);
    }
    catch (...)
    {
        res = mrCancel;
    }
    delete fNumInput;

    return res == mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfNumInput::FormPaint(TObject *Sender)
{
    DrawPillowPanel(Canvas, Rect(0, 0, ClientWidth, ClientHeight));
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(121);
#endif

