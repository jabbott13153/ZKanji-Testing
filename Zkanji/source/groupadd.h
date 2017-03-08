//---------------------------------------------------------------------------

#ifndef groupaddH
#define groupaddH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "baseform.h"
//---------------------------------------------------------------------------

class TWordCollection;
class TWordIndexList;

//---------------------------------------------------------------------------
class TfWordToGroup : public TBaseForm
{
__published: //IDE-managed Components
    TComboBox *cbGroup;
    TButton *btnAdd;
    TButton *Button2;
    TLabel *Label2;
    TLabel *Label3;
    TComboBox *cbMeaning;
    TBevel *b2;
    TLabel *Label1;
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall cbGroupChange(TObject *Sender);
private: //User declarations
    TWordCollection *fcollection;
public: //User declarations
    __fastcall TfWordToGroup(TComponent* Owner);

    int ShowModalEx(TWordCollection *acoll, int awordindex, int &agroupindex);
    int ShowModalEx(TWordIndexList *list, int &agroupindex);
};
//---------------------------------------------------------------------------
extern PACKAGE TfWordToGroup *fWordToGroup;
//---------------------------------------------------------------------------

int WordToGroup(TWordCollection *acoll, int awordindex, int &agroupindex);
int WordToGroup(TWordIndexList *list, int &agroupindex);

#endif
