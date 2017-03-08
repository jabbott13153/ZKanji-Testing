//---------------------------------------------------------------------------

#ifndef importgroupsH
#define importgroupsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#include "baseform.h"
#include <ExtCtrls.hpp>
#include "edit2.h"

class TWordCollection;
struct TImportOptions;
class TfImportGroups : public TBaseForm
{
__published:	// IDE-managed Components
    TOpenDialog *dlgOpen;
    TCheckBox *cbWModify;
    TLabel *Label1;
    TCheckBox *cbWord;
    TCheckBox *cbKanji;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    TCheckBox *cbWMissing;
    TCheckBox *cbKMissing;
    TBevel *Bevel1;
    TBevel *Bevel2;
    TBevel *Bevel4;
    TBevel *Bevel5;
    TBevel *Bevel6;
    TEdit2 *edFile;
    TLabel *Label7;
    TButton *btnSelectFile;
    TButton *btnImport;
    TButton *btnCancel;
    TComboBox *cbWReplace;
    TLabel *Label2;
    TComboBox *cbKReplace;
    TLabel *Label8;
    TCheckBox *cbKExDefinition;
    TBevel *Bevel7;
    TBevel *bBottom;
    TCheckBox *cbKExamples;
    TBevel *Bevel3;
    TLabel *Label9;
    TComboBox *cbDict;
    TLabel *Label10;
    TBevel *bTop;
    TCheckBox *cbKDefinition;
    void __fastcall cbWordClick(TObject *Sender);
    void __fastcall btnSelectFileClick(TObject *Sender);
    void __fastcall btnImportClick(TObject *Sender);
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
private:	// User declarations
    typedef TBaseForm   inherited;

    void UpdateButtons();

    TImportOptions *options;

    UnicodeString get_file();
    TWordCollection* get_coll();
public:		// User declarations
    __fastcall TfImportGroups(TComponent* Owner);

    int ShowModalEx(TImportOptions *options);
    __property UnicodeString FileName = { read = get_file };
    __property TWordCollection* Collection = { read = get_coll };
};
//---------------------------------------------------------------------------
extern PACKAGE TfImportGroups *fImportGroups;
//---------------------------------------------------------------------------
#endif
