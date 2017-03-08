//---------------------------------------------------------------------------

#ifndef importdictH
#define importdictH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "edit2.h"
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#include "baseform.h"

class TWordCollection;
class TImportOptions;
class TfImportDict : public TBaseForm
{
__published: // IDE-managed Components
    TLabel *Label7;
    TEdit2 *edFile;
    TButton *btnSelectFile;
    TLabel *Label4;
    TLabel *Label1;
    TLabel *Label2;
    TBevel *Bevel1;
    TLabel *Label3;
    TLabel *Label5;
    TCheckBox *cbSkipConflict;
    TCheckBox *cbTypeDiff;
    TCheckBox *cbUpdateType;
    TBevel *Bevel3;
    TBevel *bBottom;
    TButton *btnImport;
    TButton *btnCancel;
    TOpenDialog *dlgOpen;
    TLabel *Label6;
    TLabel *Label8;
    TBevel *Bevel2;
    TLabel *Label9;
    TComboBox *cbNewWords;
    TComboBox *cbNewMeanings;
    TComboBox *cbMissingMeanings;
    TComboBox *cbDict;
    TLabel *Label10;
    TBevel *bTop;
    TCheckBox *cbKDefinition;
    TCheckBox *cbKDefSelect;
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall cbSkipConflictClick(TObject *Sender);
    void __fastcall cbTypeDiffClick(TObject *Sender);
    void __fastcall btnSelectFileClick(TObject *Sender);
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall btnImportClick(TObject *Sender);
    void __fastcall cbDictChange(TObject *Sender);
    void __fastcall cbKDefinitionClick(TObject *Sender);
private: // User declarations
    typedef TBaseForm   inherited;

    void UpdateButtons();

    TImportOptions *options;

    UnicodeString get_file();
    TWordCollection* get_coll();
public: // User declarations
    __fastcall TfImportDict(TComponent* Owner);

    int ShowModalEx(TImportOptions *options);
    __property UnicodeString FileName = { read=get_file };
    __property TWordCollection* Collection = { read = get_coll };
};
//---------------------------------------------------------------------------
extern PACKAGE TfImportDict *fImportDict;
//---------------------------------------------------------------------------
#endif
