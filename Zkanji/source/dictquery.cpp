//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "dictquery.h"
#include "gengraph.h"
#include "collection.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "edit2"
#pragma resource "*.dfm"
TfDictQuery *fDictQuery;
//---------------------------------------------------------------------------
__fastcall TfDictQuery::TfDictQuery(TComponent* Owner, UnicodeString &name, UnicodeString &info) : TBaseForm(Owner), name(name), info(info)
{
    edName->MaxLength = dictionary_name_length_limit;
    Constrain(Width - ClientWidth + Label3->Left + Label3->Width + btnOk->Width, Height - ClientHeight + Label3->Top + Label3->Height * 4 + (ClientHeight - b3->Top));
}
//---------------------------------------------------------------------------
int TfDictQuery::ShowModalEx(const UnicodeString &caption)
{
    lasth = ClientHeight;
    Caption = caption;
    edName->Text = name;
    mInfo->Text = info;
    edNameChange(edName);
    return ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfDictQuery::edNameExit(TObject *Sender)
{
    edName->Text = edName->Text.Trim();
    if (edName->Text.Length() > dictionary_name_length_limit)
        edName->Text = edName->Text.SubString(1, dictionary_name_length_limit);
}
//---------------------------------------------------------------------------
void __fastcall TfDictQuery::FormPaint(TObject *Sender)
{
    DrawGrad(Canvas, Color, COLORDEC(Color, 0x101010), Rect(0, b3->Top - 24 * scaleres, ClientWidth, b3->Top), true);
    DrawCloseBar(this, b4, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfDictQuery::FormResize(TObject *Sender)
{
    int b = ClientHeight - b3->Top + 1 + 24 * scaleres;
    TRect r = Rect(0, min(ClientHeight, lasth) - b, ClientWidth, ClientHeight);
    //RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE);
    InvalidateRect(Handle, &r, true);
    lasth = ClientHeight;
}
//---------------------------------------------------------------------------
void __fastcall TfDictQuery::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfDictQuery::btnOkClick(TObject *Sender)
{
    edName->Text = edName->Text.Trim();
    if (edName->Text.IsEmpty())
        return;

    if (!ValidFileName(edName->Text))
    {
        MsgBox(L"Dictionary name is not valid. Valid names cannot contain the following:\n \\ / : * ? \" < > |.", L"Bad input", MB_OK);
        return;
    }
    if (edName->Text.LowerCase() != name.LowerCase() && dictionary->IndexOf(edName->Text) >= 0)
    {
        MsgBox(L"Dictionary with the same name already exists.", L"Name conflict", MB_OK);
        return;
    }


    name = edName->Text;
    info = mInfo->Text.Trim();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfDictQuery::mInfoEnter(TObject *Sender)
{
    btnOk->Default = false;
}
//---------------------------------------------------------------------------
void __fastcall TfDictQuery::mInfoExit(TObject *Sender)
{
    btnOk->Default = true;
}
//---------------------------------------------------------------------------
void __fastcall TfDictQuery::mInfoKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == VK_ESCAPE)
        btnCancel->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfDictQuery::edNameChange(TObject *Sender)
{
    btnOk->Enabled = edName->Text.Trim().Length() != 0 && edName->Text.Trim().Length() <= dictionary_name_length_limit;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool DictionaryQuery(const UnicodeString &caption, UnicodeString &name, UnicodeString &info)
{
    fDictQuery = new TfDictQuery(Application, name, info);
    int r;

    try
    {
        r = fDictQuery->ShowModalEx(caption);
    }
    catch(...)
    {
        r = mrCancel;
    }
    delete fDictQuery;
    fDictQuery = NULL;

    return r == mrOk;
}
//---------------------------------------------------------------------------





