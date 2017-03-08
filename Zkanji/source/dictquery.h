//---------------------------------------------------------------------------

#ifndef dictqueryH
#define dictqueryH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "edit2.h"
//---------------------------------------------------------------------------
#include "baseform.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
class TfDictQuery : public TBaseForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TEdit2 *edName;
    TLabel *Label2;
    TBevel *b4;
    TBevel *b3;
    TMemo *mInfo;
    TButton *btnCancel;
    TButton *btnOk;
    TLabel *Label3;
    void __fastcall edNameExit(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall btnOkClick(TObject *Sender);
    void __fastcall mInfoEnter(TObject *Sender);
    void __fastcall mInfoExit(TObject *Sender);
    void __fastcall mInfoKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall edNameChange(TObject *Sender);
private:
    UnicodeString &name;
    UnicodeString &info;
    int lasth; // Client height before previous resize.
public:
    __fastcall TfDictQuery(TComponent* Owner, UnicodeString &name, UnicodeString &info);

    int ShowModalEx(const UnicodeString &caption);
};
//---------------------------------------------------------------------------
extern PACKAGE TfDictQuery *fDictQuery;
//---------------------------------------------------------------------------
// Shows the dictquery form with the given name and info. When adding a new dictionary, make sure the initial name is distinct, or empty.
bool DictionaryQuery(const UnicodeString &caption, UnicodeString &name, UnicodeString &info);

#endif
