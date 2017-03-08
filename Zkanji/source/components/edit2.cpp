//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "edit2.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TEdit2 *)
{
  new TEdit2(NULL);
}
//---------------------------------------------------------------------------
__fastcall TEdit2::TEdit2(TComponent* Owner)
   : fselall(false),TEdit(Owner)
{
}
//---------------------------------------------------------------------------
TMouseActivate __fastcall TEdit2::MouseActivate(TMouseButton Button, Classes::TShiftState Shift, int X, int Y, int HitTest)
{
  fselall = AutoSelect && !Focused();
  return inherited::MouseActivate(Button,Shift,X,Y,HitTest);
}
//---------------------------------------------------------------------------
void __fastcall TEdit2::MouseUp(TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
{
  if (fselall && AutoSelect && !SelLength)
    SelectAll();
  fselall = false;

  inherited::MouseUp(Button,Shift,X,Y);
}
//---------------------------------------------------------------------------
void __fastcall TEdit2::DefaultHandler(void *msg)
{
  TMessage &m = *(TMessage*)msg;
  if (m.Msg == WM_CONTEXTMENU)
	m.Result = CallWindowProc((long(__stdcall*)(HWND, unsigned int, unsigned int, long))DefWndProc,Handle,m.Msg,m.WParam,m.LParam);
  else
    inherited::DefaultHandler(msg);
}
//---------------------------------------------------------------------------
namespace Edit2
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TEdit2)};
     RegisterComponents("Z Controls", classes, 0);
  }
}
//---------------------------------------------------------------------------
