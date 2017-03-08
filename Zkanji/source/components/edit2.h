//---------------------------------------------------------------------------

#ifndef edit2H
#define edit2H
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TEdit2 : public TEdit
{
private:
  typedef TEdit inherited;

  bool fselall;
protected:
  DYNAMIC TMouseActivate __fastcall MouseActivate(TMouseButton Button, Classes::TShiftState Shift, int X, int Y, int HitTest);
  DYNAMIC void __fastcall MouseUp(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
public:
  __fastcall TEdit2(TComponent* Owner);
  virtual void __fastcall DefaultHandler(void *);
__published:
};
//---------------------------------------------------------------------------
#endif
