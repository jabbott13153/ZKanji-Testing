//---------------------------------------------------------------------------

#ifndef numinputH
#define numinputH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include "edit2.h"

class TfNumInput : public TBaseForm
{
__published: //IDE-managed Components
    TPanel *Panel1;
    TBitBtn *BitBtn1;
    TBitBtn *BitBtn2;
    TEdit2 *edNum;
    TLabel *lbMsg;
    TLabel *lbInterval;
    void __fastcall FormPaint(TObject *Sender);
private: //User declarations
    TNumEdit *numedit;
    TFloatEdit *floatedit;

public: //User declarations
    __fastcall TfNumInput(TComponent* Owner);
    virtual __fastcall ~TfNumInput();

    int ShowModalEx(UnicodeString ACaption, UnicodeString AMessage, int &aval, int amin, int amax);
    int ShowModalEx(UnicodeString ACaption, UnicodeString AMessage, double &aval, double amin, double amax);
};
//---------------------------------------------------------------------------
extern PACKAGE TfNumInput *fNumInput;
//---------------------------------------------------------------------------
bool InputQueryInt(UnicodeString Message, UnicodeString Caption, int &Value, int AMin = 0, int AMax = -1);
bool InputQueryFloat(UnicodeString Message, UnicodeString Caption, double &Value, double AMin = 0, double AMax = -1);

#endif
