//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "dummymainform.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(32);
#endif

TACME *ACME;
//---------------------------------------------------------------------------
__fastcall TACME::TACME(TComponent* Owner) : TBaseForm(Owner)
{
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(33);
#endif

