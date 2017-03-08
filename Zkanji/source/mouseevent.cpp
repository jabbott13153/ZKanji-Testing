//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "mouseevent.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(110);
#endif

//---------------------------------------------------------------------------
TMouseEvent event = NULL;
bool mousedown = true;
HHOOK thishook = NULL;
long lasttime = 0;
TObject *Sender;
TMouseButton Button;
TShiftState Shift;
TRect activearea;
int x;
int y;
int timeh = 0;
int mouseskip;
//---------------------------------------------------------------------------
class TDummyDummyDummy { // in search of __closure
  public:
  void prc(void *caller);
} d1;

void TDummyDummyDummy::prc(void *caller)
{
  mousedown = false;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode >= 0 && wParam == WM_LBUTTONUP) // the end
    mousedown = false;
  return CallNextHookEx(thishook,nCode,wParam,lParam);
}
//---------------------------------------------------------------------------
void CALLBACK timeprc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
  if (mouseskip)
    mouseskip--;
  else
    MouseRepeat(event,Sender,Button,Shift,x,y);
}
//---------------------------------------------------------------------------
void MouseRepeat(TMouseEvent aevent,TObject *fSender, TMouseButton fButton, TShiftState fShift, int fx, int fy, TRect *aarea)
{
  static bool skip = false;
  Sender = fSender;
  Button = fButton;
  Shift = fShift;
  x = fx;
  y = fy;

  if (!Sender || !((TControl*)Sender)->Enabled || (!fShift.Contains(ssLeft) && !fShift.Contains(ssRight)))
    mousedown = false;

  if (!event || !mousedown) {
    if (!mousedown) {
      if (timeh)
        KillTimer(NULL,timeh);
      timeh = 0;
      mousedown = true; // reset this for next call
      if (thishook)
        UnhookWindowsHookEx(thishook);
      event = NULL;
      AppActivateEvent(NULL,d1.prc,false,false);
      return;
    }
    mouseskip = 6;
    AppActivateEvent(NULL,d1.prc,false,true);
    event = aevent;
    activearea = Rect(0,0,((TControl*)Sender)->Width,((TControl*)Sender)->Height);
    mousedown = true;
    thishook = SetWindowsHookEx(WH_MOUSE,(LRESULT (__stdcall *)(int,WPARAM,LPARAM))&MouseProc,NULL,GetCurrentThreadId());
    lasttime = GetTickCount();
    timeh = SetTimer(NULL,0,40,(void (__stdcall *)(HWND, UINT, UINT_PTR, DWORD))&timeprc);
    return;
  }
  if (aarea)
    activearea = *aarea;
  if (Sender) {
    TPoint p;
    GetCursorPos(&p);
    p = ((TControl*)Sender)->ScreenToClient(p);
    x = p.x;
    y = p.y;
  }
  if (skip || !PtInRect(activearea,Point(x,y))) {
    if (aarea)
      activearea = *aarea;
    return;
  }
  skip = true;
  event(Sender,Button,Shift,x,y);
  skip = false;
}
//---------------------------------------------------------------------------
void MouseStopRepeat()
{
  mousedown = false;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(111);
#endif

