//---------------------------------------------------------------------------

#ifndef importqueryH
#define importqueryH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include "ZPaintBox.h"
//---------------------------------------------------------------------------

#include "baseform.h"

class TVocabListObj;

class TfImportQuery : public TBaseForm
{
__published: //IDE-managed Components
    TZPaintBox *pb;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *lbMeaning;
    TLabel *lbFreqNew;
    TLabel *lbFreqOri;
    TLabel *lbLabels;
    TBitBtn *BitBtn1;
    TBitBtn *BitBtn2;
    TBitBtn *BitBtn3;
    TLabel *Label7;
    TLabel *Label3;
    void __fastcall BitBtn3Click(TObject *Sender);
    void __fastcall BitBtn2Click(TObject *Sender);
    void __fastcall BitBtn1Click(TObject *Sender);
private: //User declarations
    TVocabListObj *wobj;
    byte fsel;
public: //User declarations
    __fastcall TfImportQuery(TComponent* Owner);
    virtual __fastcall ~TfImportQuery();

    int ShowModalEx(byte acoll, int wordix, const wchar_t *mm, word freq, int atypes, int anotes, byte afields, word anametag);

    __property byte sel = { read = fsel };
};
//---------------------------------------------------------------------------
extern PACKAGE TfImportQuery *fImportQuery;
//---------------------------------------------------------------------------
#endif
