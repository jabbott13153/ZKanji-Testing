//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "importgroups.h"
#include "settings.h"
#include "gengraph.h"
#include "import.h"
#include "collection.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "edit2"
#pragma resource "*.dfm"
TfImportGroups *fImportGroups;
//---------------------------------------------------------------------------
__fastcall TfImportGroups::TfImportGroups(TComponent* Owner) : TBaseForm(Owner)
{
}
//---------------------------------------------------------------------------
int TfImportGroups::ShowModalEx(TImportOptions *aoptions)
{
    options = aoptions;

    cbDict->Items->Assign(dictionary);
    cbDict->ItemIndex = settings.dictionary_index;
    UpdateButtons();

    int result = ShowModal();

    if (result == mrOk)
    {
        options->wordgroups = cbWord->Checked;
        options->modifydict = cbWord->Checked && cbWModify->Checked;
        options->wduplicate = (TGroupImportOptions)cbWReplace->ItemIndex;
        options->wordsubst = cbWord->Checked && cbWMissing->Checked;

        options->kanjigroups = cbKanji->Checked;
        options->kanjiuserdef = cbKDefinition->Checked;
        options->kduplicate = (TGroupImportOptions)cbKReplace->ItemIndex;
        options->kanjiex = cbKExamples->Checked;
        options->kanjisubst = cbKExamples->Checked && cbKMissing->Checked;
        options->kanjiexdef = cbKExamples->Checked && cbKExDefinition->Checked;

        options->entries = false;
        options->kanjidictdef = false;
        options->kanjidefselect = false;
        options->skipconflict = false;
        options->typediff = false;
        options->updatetype = false;

        options->fullimport = false;
    }

    return result;
}
//---------------------------------------------------------------------------
TWordCollection* TfImportGroups::get_coll()
{
    return dictionary->Items[cbDict->ItemIndex];
}
//---------------------------------------------------------------------------
void TfImportGroups::UpdateButtons()
{
    cbWModify->Enabled = cbWord->Checked;
    cbWReplace->Enabled = cbWord->Checked;
    cbWMissing->Enabled = cbWord->Checked;

    cbKReplace->Enabled = cbKanji->Checked;
    cbKMissing->Enabled = cbKExamples->Checked;
    cbKExDefinition->Enabled = cbKExamples->Checked;

    btnImport->Enabled = cbKanji->Checked || cbWord->Checked || cbKDefinition->Checked || cbKExamples->Checked;
}
//---------------------------------------------------------------------------
UnicodeString TfImportGroups::get_file()
{
    return edFile->Text;
}

//---------------------------------------------------------------------------
//-- Event Handlers --
//---------------------------------------------------------------------------
void __fastcall TfImportGroups::cbWordClick(TObject *Sender)
{
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroups::btnSelectFileClick(TObject *Sender)
{
    dlgOpen->InitialDir = (edFile->Text == L"" ? settings.exportdir : ExtractFileDir(edFile->Text));
    if (ExecuteDialog(dlgOpen))
    {
        edFile->Text = dlgOpen->FileName;
        settings.exportdir = ExtractFileDir(dlgOpen->FileName);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroups::btnImportClick(TObject *Sender)
{
    if (edFile->Text == L"")
        btnSelectFile->Click();

    if (edFile->Text == L"") // No file was selected. Close silently.
        ModalResult = mrCancel;
    else
        ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroups::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroups::FormPaint(TObject *Sender)
{
    DrawCloseBar(this, bBottom, NULL);
    DrawPillowPanel(this, NULL, bTop, true);
    DrawPillowPanel(this, bTop, bBottom, true);
}
//---------------------------------------------------------------------------

