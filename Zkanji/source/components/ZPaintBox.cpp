//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "ZPaintBox.h"
#pragma package(smart_init)

  #define MAX(a,b)      ((a) < (b) ? (b) : (a))
  #define MIN(a,b)      ((a) < (b) ? (a) : (b))

//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TZPaintBox *)
{
  new TZPaintBox(NULL);
}
//---------------------------------------------------------------------------

#define WHEEL   120
//---------------------------------------------------------------------------
//--- TZCONTROLSCROLLBAR - class for paintbox scrollbars --------------------
//===========================================================================
__fastcall TZControlScrollBar::TZControlScrollBar(TZCustomPaintBox *AOwner, TScrollBarKind AKind) : TPersistent()
{
  ftag = -1;
  fwheeldelta = 0;
  fowner = AOwner;
  fkind = AKind;
  flinestep = 1;
  fpagestep = 10;
  fmax = 0;
  fpos = 0;
  fenabled = true;
  fpage = 1;
  fvisible = true;
//  fauto = true;

  if (fowner->HandleAllocated()) {
    Recreate();
    SCROLLINFO inf;
    memset(&inf,0,sizeof(SCROLLINFO));
    inf.cbSize = sizeof(SCROLLINFO);
    inf.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
    GetScrollInfo(fowner->Handle,fkind,&inf);
    if (inf.nPage != 0) {
      fmax = inf.nMax;
      fpos = inf.nPos;
      fpage = inf.nPage;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TZControlScrollBar::Assign(TZControlScrollBar *Source)
{
  fenabled = Source->Enabled;
  fvisible = Source->Visible;
  fmax = Source->Range;
  fpage = MAX(1,Source->ThumbSize);
  fpos = MAX(0,MIN(Source->Position, fmax - (fpage - 1)));
  flinestep = Source->SmallChange;
  fpagestep = Source->LargeChange;
//  fauto = Source->AutoThumbSize;
}
//---------------------------------------------------------------------------
void __fastcall TZControlScrollBar::Assign(TPersistent *Source)
{
  Assign((TZControlScrollBar*)Source);
}
//---------------------------------------------------------------------------
void __fastcall TZControlScrollBar::HandleMessage(TMessage &msg, bool invalidate)
{
  if (!fenabled)
    return;

  int wp = msg.WParam;
  if ((fkind == sbVertical && msg.Msg == WM_MOUSEWHEEL) || (fkind == sbHorizontal && msg.Msg == WM_MOUSEHWHEEL))
  {
    if (!fvisible || !fenabled)
      return;
    short delta = ((wp & 0xffff0000) >> 16);

    int val;
    SystemParametersInfo(SPI_GETWHEELSCROLLLINES,0,&val,0);
    //delta *= val;

    delta += fwheeldelta;

    int step = (delta / WHEEL);
    fwheeldelta = delta - step * WHEEL;
    if (step)
    {
      step *= flinestep * val;
      if (step > 0)
        SetPosition(MAX(fpos - step, 0), scLineUp, invalidate);
      else
        SetPosition(MIN(fpos - step, fmax - (fpage-1)), scLineDown, invalidate);
    }
    return;
  }

  short cmd = wp & 0xffff;

  SCROLLINFO inf;
  inf.cbSize = sizeof(SCROLLINFO);

  int npos;

  switch (cmd)
  {
    case SB_THUMBPOSITION:
      npos = GetScrollPos(fowner->Handle, fkind == sbHorizontal ? SB_HORZ : SB_VERT);
      SetPosition(npos, scPosition, invalidate);
    break;
    case SB_THUMBTRACK:
      inf.fMask = SIF_TRACKPOS;
      if (!GetScrollInfo(fowner->Handle, fkind == sbHorizontal ? SB_HORZ : SB_VERT, &inf))
      {
        // failed, revert to 16 bit
        inf.nTrackPos = (wp & 0xffff0000) >> 16;
      }
      SetPosition(inf.nTrackPos, scTrack, invalidate);
    break;
    case SB_TOP:
      SetPosition(0, scTop, invalidate);
    break;
    case SB_BOTTOM:
      SetPosition(Range - 1, scBottom, invalidate);
    break;
    case SB_LINEUP:
      SetPosition(MAX(fpos - flinestep, 0), scLineUp, invalidate);
    break;
    case SB_LINEDOWN:
      SetPosition(MIN(fpos + flinestep, fmax - (fpage - 1)), scLineDown, invalidate);
    break;
    case SB_PAGEUP:
      SetPosition(MAX(fpos - fpagestep, 0), scPageUp, invalidate);
    break;
    case SB_PAGEDOWN:
      SetPosition(MIN(fpos + fpagestep, fmax - (fpage - 1)), scPageDown, invalidate);
    break;
    case SB_ENDSCROLL:
      if (fvisible)
        fowner->scrollbarscancallthis(fkind, fpos, scEndScroll);
    break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TZControlScrollBar::set_pos(int value)
{
    SetPosition(value, scPosition, true);
}
//---------------------------------------------------------------------------
void TZControlScrollBar::SetPosition(int value, TScrollCode code, bool invalidate /*, ScrollEventType eventtype*/)
{
  if (value < 0)
    value = 0;
  if (value > fmax - (fpage - 1))
    value = MAX(0, fmax - (fpage - 1));
  if (value == fpos)
    return;

//  int oldpos = fpos;
  fpos = value;
  if (!fvisible || !fenabled)
      return;

  setscrolldata();

  if (invalidate)
    fowner->Invalidate();

  fowner->scrollbarscancallthis(fkind, fpos, code);
}
//---------------------------------------------------------------------------
void __fastcall TZControlScrollBar::set_visible(bool value)
{
  if (value == fvisible)
    return;
  fvisible = value;
  if (fowner->AutoSizeScrollbars)
    fowner->ScrollResize();
  setscrolldata();
}
//---------------------------------------------------------------------------
void __fastcall TZControlScrollBar::set_enabled(bool value)
{
  if (fenabled == value)
    return;
  fenabled = value;

  if (fvisible) {
    EnableScrollBar(fowner->Handle,fkind,(value && (fowner->Enabled || fowner->ComponentState.Contains(csDesigning)) ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH));
    if (fenabled)
      Recreate();
  }
}
//---------------------------------------------------------------------------
void __fastcall TZControlScrollBar::Recreate()
{
//  ShowScrollBar(fowner->Handle,fkind,fvisible);
//  EnableScrollBar(fowner->Handle,fkind,(fenabled && (fowner->Enabled || fowner->ComponentState.Contains(csDesigning)) ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH));

  SCROLLINFO inf;
  inf.cbSize = sizeof(SCROLLINFO);
  inf.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
  inf.nMax = !fvisible /*|| !fenabled*/ ? 0 : fmax;
  inf.nMin = 0;
  inf.nPos = fvisible && fenabled ? fpos : 0;
  inf.nPage = fpage;
  SetScrollInfo(fowner->Handle, fkind, &inf, true);

  if (!GetScrollInfo(fowner->Handle,fkind,&inf)) {
    inf.cbSize = sizeof(SCROLLINFO);
    inf.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
    inf.nMax = 101;
    inf.nMin = 0;
    inf.nPos = fvisible && fenabled ? fpos : 0;
    inf.nPage = fpage;
    SetScrollInfo(fowner->Handle, fkind, &inf, false);

    inf.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
    inf.nMax = !fvisible ? 0 : fmax;
    inf.nMin = 0;
    inf.nPos = fvisible && fenabled ? fpos : 0;
    inf.nPage = fpage;
    SetScrollInfo(fowner->Handle, fkind, &inf, true);
  }

  if (!fenabled && (fvisible || fowner->ComponentState.Contains(csDesigning)))
    EnableScrollBar(fowner->Handle,fkind,ESB_DISABLE_BOTH);
}
//---------------------------------------------------------------------------
void TZControlScrollBar::Set(int newmax, int newpage)
{
    if (newpage < 1)
        newpage = 1;

    if (newmax < 0)
        newmax = 0;

    if (newmax == fmax && newpage == fpage)
        return;

    fmax = newmax;
    fpage = newpage;

    if (!fvisible)
        return;

    setscrolldata();

    if (fpos > fmax - (fpage - 1))
    {
        int oldpos = fpos;
        fpos = MAX(0, fmax - (fpage - 1));
        if (!fvisible || fpos == oldpos)
            return;
        fowner->scrollbarscancallthis(fkind, fpos, scPosition);
        if (fowner->InvalidateOnScroll)
            fowner->scrollinvalidate(fkind, fpos);
    }
}
//---------------------------------------------------------------------------
void __fastcall TZControlScrollBar::set_page(int value)
{
    if (value < 1)
        value = 1;

    if (fpage == value)
        return;

    fpage = value;

    if (!fvisible || !fenabled)
        return;

    setscrolldata();

    if (fpos > fmax - (fpage - 1))
    {
        int oldpos = fpos;
        fpos = MAX(0, fmax - (fpage - 1));
        if (!fvisible || fpos == oldpos)
            return;
        fowner->scrollbarscancallthis(fkind, fpos, scPosition);
        if (fowner->InvalidateOnScroll)
            fowner->scrollinvalidate(fkind, fpos);
    }
}
//---------------------------------------------------------------------------
void __fastcall TZControlScrollBar::set_max(int value)
{
    if (value < 0)
        value = 0;

    if (fmax == value)
        return;

    fmax = value;
    int oldpos = fpos;
    if (fpos > fmax - (fpage - 1))
        fpos = MAX(0, fmax - (fpage - 1));

    if (!fvisible || !fenabled)
        return;

    setscrolldata();

    if (fpos != oldpos)
        fowner->scrollbarscancallthis(fkind,fpos, scPosition);
    if (fowner->InvalidateOnScroll)
        fowner->scrollinvalidate(fkind, fpos);
}
//---------------------------------------------------------------------------
void TZControlScrollBar::setscrolldata()
{
  SCROLLINFO inf;
  memset(&inf,0,sizeof(SCROLLINFO));
  inf.cbSize = sizeof(SCROLLINFO);
  inf.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
  inf.nMax = !fvisible ? 0 : fmax;
  inf.nMin = 0;
  inf.nPos = fvisible && fenabled ? fpos : 0;
  inf.nPage = fpage;
  SetScrollInfo(fowner->Handle, fkind, &inf, true);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//--- TZCUSTOMPAINTBOX - base class for TZPaintBox --------------------------
//===========================================================================
__fastcall TZCustomPaintBox::TZCustomPaintBox(TComponent* Owner) : FVertScrollBar(0),FHorzScrollBar(0),fautosizescroll(false),TCustomControl(Owner)
{
  fscrollresizing = false;

  ControlStyle << csCaptureMouse << csDoubleClicks << csClickEvents << csNeedsBorderPaint;
  if (!NewStyleControls)
    ControlStyle << csFramed;
  FBorderStyle = ::bsSingle;
  FWantTab = false;
  TabStop = true;
  ParentColor = false;
  FVertScrollBar = new TZControlScrollBar(this,sbVertical);
  FHorzScrollBar = new TZControlScrollBar(this,sbHorizontal);
  Height = 41;
  Width = 185;
  Color = clWindow;

  DoubleBuffered = false;

  FOnPaint = NULL;
  FOnVScroll = NULL;
  FOnHScroll = NULL;
  FOnMouseEnter = NULL;
  FOnMouseLeave = NULL;
  FOnEraseBkgnd = NULL;
  FOnCaptureChanged = NULL;

  WindowProc = SubWndProc;
}

//---------------------------------------------------------------------------
__fastcall TZCustomPaintBox::~TZCustomPaintBox()
{
  WindowProc = WndProc;
  delete FVertScrollBar;
  delete FHorzScrollBar;
}
//---------------------------------------------------------------------------
#define sgn(a) (a < 0 ? -1 : 1)
void __fastcall TZCustomPaintBox::WMMouseWheel(TMessage &msg)
{
//  static int vdelta = 0;
  if ((msg.WParam & 0xffff) & MK_SHIFT) {
    msg.Msg = WM_MOUSEHWHEEL;
    msg.WParam = MAKEWPARAM(GET_KEYSTATE_WPARAM(msg.WParam), GET_WHEEL_DELTA_WPARAM(msg.WParam) * -1);
    WMMouseHWheel(msg);
    msg.Result = 0;
    return;
  }

  if (FVertScrollBar->Visible && FVertScrollBar->Enabled) {
    FVertScrollBar->HandleMessage(msg,false);
    if (InvalidateOnScroll && FVertScrollBar->Tag != VPos)
      Invalidate();
    FVertScrollBar->Tag = VPos;
  }

/*  unsigned int val;

  if (FVertScrollBar->Visible && FVertScrollBar->Enabled) {
    SystemParametersInfo(SPI_GETWHEELSCROLLLINES,0,&val,0);
    msg.WheelDelta *= val;
    vdelta += msg.WheelDelta;
    while (abs(vdelta) >= 120) {
      TWMVScroll m;
      m.Msg = WM_VSCROLL;
      m.ScrollCode = (vdelta > 0 ? SB_LINEUP : SB_LINEDOWN);
      m.ScrollBar = 0;
      m.Pos = 0;

      if (m.ScrollCode == SB_LINEUP && VPos > 0 || m.ScrollCode == SB_LINEDOWN && VPos < VMax-ClientHeight)
        WMVScroll(m);

      vdelta -= 120*sgn(vdelta);
    }
  }*/
  msg.Result = 0;
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::WMMouseHWheel(TMessage &msg)
{
  if (FHorzScrollBar->Visible && FHorzScrollBar->Enabled) {
    msg.WParam = MAKEWPARAM(GET_KEYSTATE_WPARAM(msg.WParam), GET_WHEEL_DELTA_WPARAM(msg.WParam) * -1);
    HorzScrollBar->HandleMessage(msg, false);
    if (InvalidateOnScroll && FHorzScrollBar->Tag != HPos)
      Invalidate();
    FHorzScrollBar->Tag = HPos;
  }

/*  static int hdelta = 0;

  unsigned int val;

  if (FHorzScrollBar->Visible && FHorzScrollBar->Enabled) {
    if (!SystemParametersInfo(SPI_GETWHEELSCROLLCHARS,0,&val,0))
      SystemParametersInfo(SPI_GETWHEELSCROLLLINES,0,&val,0);

    msg.WheelDelta *= val;
    hdelta += msg.WheelDelta;
    while (abs(hdelta) >= 90) {
      TWMHScroll m;
      m.Msg = WM_HSCROLL;
      m.ScrollCode = (hdelta > 0 ? SB_LINEUP : SB_LINEDOWN);
      m.ScrollBar = 0;
      m.Pos = 0;

      if (m.ScrollCode == SB_LINEUP && HPos > 0 || m.ScrollCode == SB_LINEDOWN && HPos < HMax-ClientWidth)
        WMHScroll(m);

      hdelta -= 90*sgn(hdelta);
    }
  }*/
  msg.Result = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::WMVScroll(TMessage &msg)
{
  if (msg.LParam == 0 && FVertScrollBar->Visible && FVertScrollBar->Enabled)
  {
    if (Visible && CanFocus() && TabStop && !ComponentState.Contains(csDesigning))
      SetFocus();
    FVertScrollBar->HandleMessage(msg, false);
//    if (FOnVScroll)
//      FOnVScroll(this,(TScrollCode)msg.ScrollCode,Pos);
    if (InvalidateOnScroll && FVertScrollBar->Tag != VPos || msg.WParam & SB_ENDSCROLL)
      Invalidate();
    FVertScrollBar->Tag = VPos;
  }
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::WMHScroll(TMessage &msg)
{
  if (msg.LParam == 0 && FHorzScrollBar->Visible && FHorzScrollBar->Enabled)
  {
    if (Visible && CanFocus() && TabStop && !ComponentState.Contains(csDesigning))
      SetFocus();
    FHorzScrollBar->HandleMessage(msg, false);
//    if (FOnHScroll)
//      FOnHScroll(this,(TScrollCode)msg.ScrollCode,Pos);
    if (InvalidateOnScroll && FHorzScrollBar->Tag != HPos || msg.WParam & SB_ENDSCROLL)
      Invalidate();
    FHorzScrollBar->Tag = HPos;
  }
}
//---------------------------------------------------------------------------
void TZCustomPaintBox::scrollbarscancallthis(TScrollBarKind kind, int newpos, TScrollCode code)
{
  if (kind == sbHorizontal) {
    if (FOnHScroll)
      FOnHScroll(this,code,newpos);
  } else {
    if (FOnVScroll)
      FOnVScroll(this,code,newpos);
  }
}
//---------------------------------------------------------------------------
void TZCustomPaintBox::scrollinvalidate(TScrollBarKind kind, int newpos)
{
    TZControlScrollBar *bar = (kind == sbHorizontal ? FHorzScrollBar : FVertScrollBar);
    if (InvalidateOnScroll && bar->Tag != newpos)
    {
        Invalidate();
        bar->Tag = newpos;
    }
}
//---------------------------------------------------------------------------
void TZCustomPaintBox::GetOverflow(int &uw, int &uh, int &hw, int &hh)
{
  if (FOnGetOverflow) {
    TZScrollOverflow data;
    FOnGetOverflow(this,uw,uh,data);
    uw = data.visiblewidth;
    uh = data.visibleheight;
    hw = data.hiddenwidth;
    hh = data.hiddenheight;
    return;
  }
  if (ComponentState.Contains(csDesigning)) {
    uw = FHorzScrollBar->ThumbSize;
    uh = FVertScrollBar->ThumbSize;
    hw = MAX(0,FHorzScrollBar->Range - FHorzScrollBar->ThumbSize);
    hh = MAX(0,FVertScrollBar->Range - FVertScrollBar->ThumbSize);
  } else {
    hw = 0;//MAX(0,FHorzScrollBar->Range - uw);
    hh = 0;//MAX(0,FVertScrollBar->Range - uh);
  }
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::CMScrollResize(TMessage &msg)
{
  ScrollResize();
}
//---------------------------------------------------------------------------
void TZCustomPaintBox::ScrollResize()
{
  if (!fautosizescroll || ComponentState.Contains(csDesigning) || fscrollresizing)
    return;

  fscrollresizing = true;

  TRect r;
  ::GetClientRect(Handle,&r);
  int vwidth = GetSystemMetrics(SM_CXVSCROLL);
  int hheight = GetSystemMetrics(SM_CYHSCROLL);

  int w = r.Width();
  int h = r.Height();

  bool vvis = false;
  bool hvis = false;

/*
  if (w + vwidth <= Width && FVertScrollBar->Visible)
      w += vwidth;
  if (h + hheight <= Height && FHorzScrollBar->Visible)
      h += hheight;
*/

  SCROLLINFO inf;
  memset(&inf,0,sizeof(SCROLLINFO));

  inf.cbSize = sizeof(SCROLLINFO);
  inf.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;

  if (GetScrollInfo(Handle,SB_VERT,&inf) && /*h < inf.nMax &&*/ inf.nMax > (int)inf.nPage && inf.nPage > 0) {
//    if (FVertScrollBar->Enabled)
      w += vwidth;
//    else
//      vvis = true;
  }

  if (GetScrollInfo(Handle,SB_HORZ,&inf) && /*w < inf.nMax &&*/ inf.nMax > (int)inf.nPage && inf.nPage > 0) {
//    if (FHorzScrollBar->Enabled)
      h += hheight;
//    else
//      hvis = true;
  }


  int uw, uh, hw, hh;
  bool changed;
  do
  {
    uw = w;
    uh = h;
    GetOverflow(uw,uh,hw,hh);
    if (hw < 0)
      hw = 0;
    if (hh < 0)
      hh = 0;

    changed = false;
    if (vvis && hh <= 0)
    {
      vvis = false;
      changed = true;
    }
    else if (!vvis && hh > 0 && FVertScrollBar->Visible)
    {
      vvis = true;
      changed = true;
      w -= vwidth;
    }

    if (hvis && hw <= 0)
    {
      hvis = false;
      changed = true;
    }
    else if (!hvis && hw > 0 && FHorzScrollBar->Visible)
    {
      hvis = true;
      changed = true;
      h -= hheight;
    }
  } while (changed);

  if (uw < 0)
    uw = 0;
  if (uh < 0)
    uh = 0;

  if (!vvis)
    FVertScrollBar->Set(0,0);
  if (!hvis)
    FHorzScrollBar->Set(0,0);

  if (vvis)
  {
//    if (FVertScrollBar->AutoThumbSize)
      FVertScrollBar->Set(hh+uh - 1,uh);
//    else
//      FVertScrollBar->Set(hh,1);
  }
  if (hvis)
  {
//    if (FHorzScrollBar->AutoThumbSize)
      FHorzScrollBar->Set(hw+uw - 1,uw);
//    else
//      FHorzScrollBar->Set(hw,1);
  }

  fscrollresizing = false;
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::WMSize(TWMSize &msg)
{
//  FHorzScrollBar->Resize();
//  FVertScrollBar->Resize();
  ScrollResize();
  if (! ComponentState.Contains(csLoading) )
    Resize();
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::CMCtl3DChanged(TMessage &msg)
{
  if (ControlStyle.Contains(csFramed) && Parent != NULL && HandleAllocated() && IsWindowVisible(Handle)) {
  // copied from vcl source. it is not accessible... grrr
    TRect R = BoundsRect;
    InflateRect(&R, 1, 1);
    InvalidateRect(Parent->Handle, &R, true);
  }

  NotifyControls(CM_PARENTCTL3DCHANGED);
  RecreateWnd();
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::WMEraseBkgnd(TWMEraseBkgnd &msg)
{
  if (ComponentState.Contains(csDesigning))
  {
    Canvas->Brush->Color = clWhite;
    Canvas->Pen->Color = clBlack;
    Canvas->Pen->Style = psDash;
    Canvas->Rectangle(ClientRect);
  } else if (FOnEraseBkgnd)
    FOnEraseBkgnd(this,msg.DC);
  msg.Result = true;
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::WMCaptureChanged(TMessage &msg)
{
  TCustomControl::Dispatch(&msg);
  PostMessage(Handle,CM_CAPTURECHANGED,msg.WParam,msg.LParam);
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::CMCaptureChanged(TMessage &msg)
{
  if (FOnCaptureChanged)
    FOnCaptureChanged(this);
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::CreateParams(TCreateParams &Params)
{
  TCustomControl::CreateParams(Params);
  Params.Style |= WS_CHILDWINDOW;
//  Params.Style |= WS_VSCROLL;
//  Params.Style |= WS_HSCROLL;
  Params.WindowClass.style = CS_DBLCLKS;
  if (FBorderStyle == ::bsSingle)
    if (NewStyleControls & Ctl3D)
    {
      Params.Style &= ~WS_BORDER;
      Params.ExStyle |= WS_EX_CLIENTEDGE;
    } else
      Params.Style |= WS_BORDER;
}

//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::SetBorderStyle(TZBorderStyle Value)
{
  if (FBorderStyle != Value)
  {
    FBorderStyle = Value;
    RecreateWnd();
  }
}

//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::SetVertScrollBar(TZControlScrollBar *Value)
{
  FVertScrollBar->Assign(Value);
}

//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::SetHorzScrollBar(TZControlScrollBar *Value)
{
  FHorzScrollBar->Assign(Value);
}

//---------------------------------------------------------------------------
TZControlScrollBar* __fastcall TZCustomPaintBox::GetVertScrollBar()
{
  return FVertScrollBar;
}
//---------------------------------------------------------------------------
TZControlScrollBar* __fastcall TZCustomPaintBox::GetHorzScrollBar()
{
  return FHorzScrollBar;
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::SetHMax(int Value)
{
  HorzScrollBar->Range = Value;
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::SetVMax(int Value)
{
  VertScrollBar->Range = Value;
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::SetHPos(int Value)
{
  if (HorzScrollBar->Position == Value)
    return;

  HorzScrollBar->Position = Value;
  Value = HorzScrollBar->Position;
  if (FOnHScroll)
    FOnHScroll(this,scPosition,Value);
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::SetVPos(int Value)
{
  if (VertScrollBar->Position == Value)
    return;

  VertScrollBar->Position = Value;
  Value = VertScrollBar->Position;
  if (FOnVScroll)
    FOnVScroll(this,scPosition,Value);
}
//---------------------------------------------------------------------------
int __fastcall TZCustomPaintBox::GetHMax()
{
  return HorzScrollBar->Range;
}
//---------------------------------------------------------------------------
int __fastcall TZCustomPaintBox::GetVMax()
{
  return VertScrollBar->Range;
}
//---------------------------------------------------------------------------
int __fastcall TZCustomPaintBox::GetHPos()
{
  return HorzScrollBar->Position;
}
//---------------------------------------------------------------------------
int __fastcall TZCustomPaintBox::GetVPos()
{
  return VertScrollBar->Position;
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::Paint()
{
  Canvas->Font = Font;
  if (FOnPaint)
    FOnPaint(this);
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::SubWndProc(TMessage &Message) {
  static bool modaldefocused = false;
  HDC hddc;
  TRect r;
  TPoint p;

  if (! ComponentState.Contains(csDesigning)) {
    switch (Message.Msg) {
      case WM_GETDLGCODE:
        Message.Result = DLGC_WANTARROWS | DLGC_WANTCHARS;
        if (FWantTab)
          Message.Result = Message.Result | DLGC_WANTTAB;
      return;
      case CM_MOUSELEAVE:
        if (FOnMouseLeave)
          FOnMouseLeave(this);
      break;
      case WM_KILLFOCUS:
        if (TabStop && OnExit && !Focused()) {
          modaldefocused = true;
          OnExit(this);
        }
      break;
      case CM_MOUSEENTER:
        if (FOnMouseEnter)
          FOnMouseEnter(this);
      break;
      case WM_CREATE:
      case CM_RECREATEWND:
        if (HorzScrollBar)
          HorzScrollBar->Recreate();
        if (VertScrollBar)
          VertScrollBar->Recreate();
      break;
      case WM_SETFOCUS:
        if (TabStop && OnEnter && modaldefocused && Focused()) {
          modaldefocused = false;
          OnEnter(this);
        }
      break;
      case WM_KEYDOWN:
        fkeyrepeat = ((Message.LParam >> 30) & 1);
        if (fkeyrepeat)
          Message.Result = 1;
      break;
      case WM_NCPAINT:
        if (FOnNCPaint) {            
          hddc = GetWindowDC(Handle);
          GetWindowRect(Handle,&r);
          p = ClientToScreen(Point(0,0));
          r.Left = r.Left-p.x;
          r.Top = r.Top-p.y;
          r.Right = r.Left+Width;
          r.Bottom = r.Top+Height;
//          OffsetRect(r,-r.Left,-r.Top);
          FOnNCPaint(this,hddc,r);
          ReleaseDC(Handle, hddc);
          Message.Result = 0;
          return;
        }
      break;
    }
  } else { // design time messages
    switch(Message.Msg) {
      case WM_SIZE:
        WMEraseBkgnd(*((TWMEraseBkgnd*)&Message) );
      break;
    }
  }
  WndProc(Message);
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
{
    if (Visible && !ComponentState.Contains(csDesigning))
    {
        if (CanFocus() && TabStop)
            SetFocus();
        else if (!TabStop && TopOwner && TopOwner->HandleAllocated() && GetForegroundWindow() != TopOwner->Handle)
            TopOwner->SetFocus();
    }
    TCustomControl::MouseDown(Button,Shift,X,Y);
}
//---------------------------------------------------------------------------
bool TZCustomPaintBox::RectVisible(TRect ARect)
{
  TRect a;
  return IntersectRect(a,ARect,ClientRect);
}

//---------------------------------------------------------------------------
bool TZCustomPaintBox::PointVisible(TPoint APoint)
{
  return PtInRect(ClientRect,APoint);
}

//---------------------------------------------------------------------------
TRect __fastcall TZCustomPaintBox::GetClientRect()
{
  return TWinControl::GetClientRect();
}
//---------------------------------------------------------------------------

void __fastcall TZCustomPaintBox::SetEnabled(bool val) {
  if (!ComponentState.Contains(csDesigning)) {
    EnableScrollBar(Handle,SB_HORZ,(val && HorzScrollBar->Enabled ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH)	);
    EnableScrollBar(Handle,SB_VERT,(val && VertScrollBar->Enabled ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH)	);
  }

  TCustomControl::Enabled = val;
}
//---------------------------------------------------------------------------
bool __fastcall TZCustomPaintBox::GetEnabled()
{
  return TCustomControl::Enabled;
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::CMFontChanged(TMessage &msg)
{
  Canvas->Font = Font;
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::CMParentFontChanged(TMessage &msg)
{
  if (ParentFont) {
    Canvas->Font = Font;
  }
}
//---------------------------------------------------------------------------
bool __fastcall TZCustomPaintBox::get_tabstop()
{
  return TCustomControl::TabStop;
}
//---------------------------------------------------------------------------
void __fastcall TZCustomPaintBox::set_tabstop(bool val)
{
  TCustomControl::TabStop = val;
  if (!val && Focused())
    FocusNext();
}
//---------------------------------------------------------------------------
void TZCustomPaintBox::FocusNext()
{
    TList *l = NULL;
    TComponent *o = Owner;
    try {
        l = new TList;
        bool found = false;
        while (true)
        {
            ((TWinControl*)o)->GetTabOrderList(l);
            if (l->Count > 1)
            {
                int base = l->IndexOf(this);
                int pos = base;
                do
                {
                    pos++;
                    if (pos >= l->Count)
                        pos = 0;
                    if (((TWinControl*)l->Items[pos])->CanFocus() && ((TWinControl*)l->Items[pos])->TabStop)
                    {
                        found = true;
                        ((TWinControl*)l->Items[pos])->SetFocus();
                        break;
                    }
                }
                while (pos != base);
            }
            l->Clear();
            if (!found && o != TopOwner)
                o = TopOwner;
            else
                break;
        }
    }
    catch(...)
    {
        ;
    }
    delete l;
}
//---------------------------------------------------------------------------
void TZCustomPaintBox::RecreateWnd()
{
  TCustomControl::RecreateWnd();
}
//---------------------------------------------------------------------------
TForm* TZCustomPaintBox::GetTopOwner()
{
    if (Owner == NULL)
        return NULL;
    TComponent *o = Owner;
    while (o->Owner)
        o = o->Owner;
    return dynamic_cast<TForm*>(o);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//--- TZPAINTBOX ------------------------------------------------------------
//===========================================================================
__fastcall TZPaintBox::TZPaintBox(TComponent* Owner) : TZCustomPaintBox(Owner)
{
}
//---------------------------------------------------------------------------
namespace Zpaintbox
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TZPaintBox)};
     RegisterComponents("Z Controls", classes, 0);
  }
}
//---------------------------------------------------------------------------

