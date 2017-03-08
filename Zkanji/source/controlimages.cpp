//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "controlimages.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(20);
#endif

Tcdata *cdata;

//---------------------------------------------------------------------------
__fastcall Tcdata::Tcdata(TComponent* Owner) : TDataModule(Owner)
{
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(21);
#endif

