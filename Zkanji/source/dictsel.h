//---------------------------------------------------------------------------

#ifndef dictselH
#define dictselH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"

class TfDictSel : public TBaseForm
{
__published: // IDE-managed Components
    TLabel *lbMsg;
    TPanel *pControls;
    TComboBox *cbDict;
    TLabel *Label1;
    TButton *btnOk;
    TButton *btnCancel;
    TBevel *bvBottom;
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall btnOkClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall cbDictChange(TObject *Sender);
    void __fastcall cbDictClick(TObject *Sender);
private: // User declarations
public:  // User declarations
    __fastcall TfDictSel(TComponent* Owner);
    int ShowModalEx(UnicodeString message, bool main, int defaultix);
    UnicodeString ShowModalEx(UnicodeString message, bool main, UnicodeString name);
};
//---------------------------------------------------------------------------
extern PACKAGE TfDictSel *fDictSel;
//---------------------------------------------------------------------------

class TWordCollection;
// Returns a dictionary selected by the user in the dictionary selector dialog. Does not show the dialog if there is no or only one choice.
// Set main to false if the main dictionary shouldn't be allowed. 0 in defaultix always means the main dictionary, even if main is false.
// If defaultix does not make sense, the first dictionary is the default.
TWordCollection* DictionarySelect(UnicodeString message, bool main, int defaultix);
// Returns the name of the selected dictionary, if an existing dictionary was selected, or the name of a new dictionary.
// Set name to the text initially entered to the combobox. The result is an empty string for the cancel button.
// The main dictionary is excluded if main is false, and its name is not allowed. Invalid file names are not allowed and will never be returned.
UnicodeString DictionarySelect(UnicodeString message, bool main, UnicodeString name);

#endif
