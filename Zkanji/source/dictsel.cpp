//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "dictsel.h"
#include "collection.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfDictSel *fDictSel;
//---------------------------------------------------------------------------
__fastcall TfDictSel::TfDictSel(TComponent* Owner) : TBaseForm(Owner)
{
}
//---------------------------------------------------------------------------
int TfDictSel::ShowModalEx(UnicodeString message, bool main, int defaultix)
{
    lbMsg->Caption = message;
    lbMsg->Width = ClientWidth - lbMsg->Left * 2;
    lbMsg->AutoSize = false;
    lbMsg->AutoSize = true;
    pControls->Top = lbMsg->Top + lbMsg->Height;
    ClientHeight = pControls->Top + pControls->Height;

    for (int ix = main ? 0 : 1; ix < dictionary->Count; ++ix)
        cbDict->Items->Add(dictionary->Strings[ix]);
    if (defaultix >= (main ? 0 : 1) && defaultix < dictionary->Count)
        cbDict->ItemIndex = defaultix - (main ? 0 : 1);
    else
        cbDict->ItemIndex = 0;

    int r = ShowModal();
    if (r == mrCancel)
        return -1;
    else
        return cbDict->ItemIndex + (main ? 0 : 1);
}
//---------------------------------------------------------------------------
UnicodeString TfDictSel::ShowModalEx(UnicodeString message, bool main, UnicodeString name)
{
    if (dictionary->Count == 255)
    {
        int ix = ShowModalEx(message, main, dictionary->IndexOf(name));
        if (ix < 0)
            return UnicodeString();
        return dictionary->Strings[ix];
    }

    name = name.Trim();
    if (name.Length() > dictionary_name_length_limit)
        name.SetLength(dictionary_name_length_limit);

    lbMsg->Caption = message;
    lbMsg->Width = ClientWidth - lbMsg->Left * 2;
    lbMsg->AutoSize = false;
    lbMsg->AutoSize = true;
    pControls->Top = lbMsg->Top + lbMsg->Height;
    ClientHeight = pControls->Top + pControls->Height;

    cbDict->MaxLength = dictionary_name_length_limit;

    cbDict->Style = csDropDown;

    int sel = -1;
    for (int ix = (main ? 0 : 1); ix < dictionary->Count; ++ix)
    {
        cbDict->Items->Add(dictionary->Strings[ix]);
        if (sel != -1 && (dictionary->Strings[ix].LowerCase() == name.LowerCase() || name.Length() == 0))
            sel = ix;
    }
    if (sel == -1)
        cbDict->Text = name;
    else
        cbDict->ItemIndex = sel - (main ? 0 : 1);

    int r = ShowModal();
    if (r == mrCancel)
        return UnicodeString();

    cbDict->Text = cbDict->Text.Trim();
    int ix = RealIndex(cbDict);
    if (ix >= 0)
        return cbDict->Items->Strings[ix];
    return cbDict->Text;
}
//---------------------------------------------------------------------------
void __fastcall TfDictSel::btnOkClick(TObject *Sender)
{
    if (cbDict->Style == csDropDown && !ValidFileName(cbDict->Text.Trim().SubString(0, dictionary_name_length_limit)))
        MsgBox(L"Dictionary name is not valid. Valid names cannot contain the following:\n \\ / : * ? \" < > |.", L"Bad input", MB_OK);
    else
        ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfDictSel::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfDictSel::FormPaint(TObject *Sender)
{
    DrawPillowPanel(this, NULL, bvBottom, true, false);
    DrawCloseBar(this, bvBottom, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfDictSel::cbDictChange(TObject *Sender)
{
    if (cbDict->Style == csDropDown)
        btnOk->Enabled = cbDict->Text.Trim().Length() > 0 && ((cbDict->Items->Count && cbDict->Items->Strings[0] == dictionary->Strings[0]) || cbDict->Text.Trim().LowerCase() != dictionary->Strings[0].LowerCase());
}
//---------------------------------------------------------------------------
void __fastcall TfDictSel::cbDictClick(TObject *Sender)
{
    cbDictChange(Sender);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordCollection* DictionarySelect(UnicodeString message, bool main, int defaultix)
{
    if (dictionary->Count == 1)
        return (main ? dictionary->Items[0] : NULL);
    if (dictionary->Count == 2 && !main)
        return dictionary->Items[1];

    TWordCollection *col = NULL;
    fDictSel = NULL;
    try
    {
        fDictSel = new TfDictSel(Application);
        int r = fDictSel->ShowModalEx(message, main, defaultix);
        if (r >= 0)
            col = dictionary->Items[r];
    }
    catch(...)
    {
        ;
    }
    delete fDictSel;

    return col;
}
//---------------------------------------------------------------------------
UnicodeString DictionarySelect(UnicodeString message, bool main, UnicodeString name)
{
    fDictSel = NULL;
    try
    {
        fDictSel = new TfDictSel(Application);
        name = fDictSel->ShowModalEx(message, main, name);
    }
    catch(...)
    {
        ;
    }
    delete fDictSel;

    return name;
}
//---------------------------------------------------------------------------

