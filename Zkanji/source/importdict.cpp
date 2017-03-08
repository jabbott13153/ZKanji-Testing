//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "importdict.h"
#include "settings.h"
#include "gengraph.h"
#include "import.h"
#include "collection.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "edit2"
#pragma resource "*.dfm"
TfImportDict *fImportDict;
//---------------------------------------------------------------------------
__fastcall TfImportDict::TfImportDict(TComponent* Owner) : TBaseForm(Owner)
{
}
//---------------------------------------------------------------------------
int TfImportDict::ShowModalEx(TImportOptions *aoptions)
{
    options = aoptions;

    cbDict->Items->Assign(dictionary);
    cbDict->ItemIndex = settings.dictionary_index;
    cbDictChange(cbDict);

    int result = ShowModal();

    if (result == mrOk)
    {
        options->entries = true;
        options->skipconflict = cbSkipConflict->Checked;
        options->typediff = cbSkipConflict->Checked && cbTypeDiff->Checked;
        options->updatetype = cbUpdateType->Enabled && cbUpdateType->Checked;
        options->newword = (TImportWordOptions)cbNewWords->ItemIndex;
        options->newmeaning = (TImportWordOptions)cbNewMeanings->ItemIndex;
        options->missingmeaning = cbMissingMeanings->ItemIndex == 2 ? iwoAuto : cbMissingMeanings->ItemIndex == 1 ? iwoChoose : iwoSkip;
        options->kanjidictdef = cbKDefinition->Checked;
        options->kanjidefselect = cbKDefinition->Checked && cbKDefSelect->Checked;

        options->wordgroups = false;
        options->kanjigroups = false;
        options->kanjiuserdef = false;
        options->kanjiex = false;
        options->kanjiexdef = false;

        options->fullimport = false;
    }

    return result;
}
//---------------------------------------------------------------------------
void TfImportDict::UpdateButtons()
{
    cbTypeDiff->Enabled = cbSkipConflict->Checked;
    cbUpdateType->Enabled = !cbSkipConflict->Checked || !cbTypeDiff->Checked;
}
//---------------------------------------------------------------------------
UnicodeString TfImportDict::get_file()
{
    return edFile->Text;
}
//---------------------------------------------------------------------------
TWordCollection* TfImportDict::get_coll()
{
    return dictionary->Items[cbDict->ItemIndex];
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//-- Event Handlers --
//---------------------------------------------------------------------------
void __fastcall TfImportDict::FormPaint(TObject *Sender)
{
    DrawCloseBar(this, bBottom, NULL);
    DrawPillowPanel(this, NULL, bTop, true);
    DrawPillowPanel(this, bTop, bBottom, true);
}
//---------------------------------------------------------------------------
void __fastcall TfImportDict::cbSkipConflictClick(TObject *Sender)
{
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfImportDict::cbTypeDiffClick(TObject *Sender)
{
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfImportDict::btnSelectFileClick(TObject *Sender)
{
    dlgOpen->InitialDir = (edFile->Text == L"" ? settings.exportdir : ExtractFileDir(edFile->Text));
    if (ExecuteDialog(dlgOpen))
    {
        edFile->Text = dlgOpen->FileName;
        settings.exportdir = ExtractFileDir(dlgOpen->FileName);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfImportDict::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfImportDict::btnImportClick(TObject *Sender)
{
    if (edFile->Text == L"")
        btnSelectFile->Click();

    if (edFile->Text == L"") // No file was selected. Close silently.
        ModalResult = mrCancel;
    else
        ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfImportDict::cbDictChange(TObject *Sender)
{
    cbKDefinition->Enabled = cbDict->ItemIndex != 0;
    cbKDefSelect->Enabled = cbKDefinition->Enabled && cbKDefinition->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfImportDict::cbKDefinitionClick(TObject *Sender)
{
    cbKDefSelect->Enabled = cbKDefinition->Enabled && cbKDefinition->Checked;
}
//---------------------------------------------------------------------------

