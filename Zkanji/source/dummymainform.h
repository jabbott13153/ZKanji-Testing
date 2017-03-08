//---------------------------------------------------------------------------

#ifndef dummymainformH
#define dummymainformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "baseform.h"
class TACME : public TBaseForm
{
__published: //IDE-managed Components
private: //User declarations
public: //User declarations
    __fastcall TACME(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TACME *ACME;
//---------------------------------------------------------------------------
#endif
