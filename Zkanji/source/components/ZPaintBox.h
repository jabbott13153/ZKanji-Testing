//---------------------------------------------------------------------------

#ifndef ZPaintBoxH
#define ZPaintBoxH
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#define CM_CAPTURECHANGED       (CM_BASE + WM_CAPTURECHANGED)
#define CM_SCROLLRESIZE         (CM_BASE + 101)

enum TZBorderStyle { bsNone, bsSingle };

class PACKAGE TZCustomPaintBox;
struct TZScrollOverflow;

typedef void __fastcall (__closure *TNCPaintEvent)(TObject *Sender, void *hdc, TRect winrect);
typedef void __fastcall (__closure *TEraseBkgndEvent)(TObject *Sender, void *hdc);
typedef void __fastcall (__closure *TGetScrollOverflowEvent)(TObject *Sender, int clientwidth, int clientheight, TZScrollOverflow &data);

struct TZScrollOverflow
{
  int visiblewidth;
  int visibleheight;
  int hiddenwidth;
  int hiddenheight;
};

class PACKAGE TZControlScrollBar : public TPersistent {
private:
  TZCustomPaintBox *fowner;
  TScrollBarKind fkind;
  int flinestep;
  int fpagestep;
  bool fvisible;
  int fpage;

  int fwheeldelta;

  int ftag;

  int fpos;
  int fmax;
  bool fenabled;

//  bool fauto;

  void __fastcall set_pos(int value);
  void __fastcall set_visible(bool value);
  void __fastcall set_max(int value);
  void __fastcall set_page(int value);
  void __fastcall set_enabled(bool value);

  void __fastcall HandleMessage(TMessage &Msg, bool invalidate);
  void SetPosition(int value, TScrollCode code = scPosition, bool invalidate = true);
  void Set(int newmax, int newpage);

  void setscrolldata();

  __property int Tag = { read=ftag, write=ftag };
  friend TZCustomPaintBox;
public:
  __fastcall TZControlScrollBar(TZCustomPaintBox *AOwner, TScrollBarKind AKind);
  void __fastcall Assign(TPersistent *Source);
  void __fastcall Assign(TZControlScrollBar *Source);
  void __fastcall Recreate();

  __property TScrollBarKind  Kind = { read=fkind };
__published:
  __property int SmallChange = { read=flinestep, write=flinestep, default=1, stored=true };
  __property int LargeChange = { read=fpagestep, write=fpagestep, default=10, stored=true };
  __property int Range = { read=fmax, write=set_max, default=0, stored=true };
  __property int Position = { read=fpos, write=set_pos, default=0, stored=true };
  __property int ThumbSize = { read=fpage, write=set_page, default=1, stored=true };
  __property bool Visible = { read=fvisible, write=set_visible, default=true, stored=true };
  __property bool Enabled = { read=fenabled, write=set_enabled, default=true, stored=true };
//  __property bool AutoThumbSize = { read=fauto, write=fauto, default=true, stored=true };
};

class PACKAGE TZCustomPaintBox : public TCustomControl
{
private:
  TZBorderStyle FBorderStyle;

  TNotifyEvent FOnPaint;
//  TNotifyEvent FOnResize;
  TNCPaintEvent FOnNCPaint;
  TScrollEvent FOnVScroll;
  TScrollEvent FOnHScroll;
  TNotifyEvent FOnMouseEnter;
  TNotifyEvent FOnMouseLeave;
  TEraseBkgndEvent FOnEraseBkgnd;
  TNotifyEvent FOnCaptureChanged;

  TGetScrollOverflowEvent FOnGetOverflow;

  TZControlScrollBar *FVertScrollBar;
  TZControlScrollBar *FHorzScrollBar;
  bool FInvalidateOnScroll;
//  bool mouse_captured;
//  byte mouse_capture_down;
  bool fkeyrepeat;
  bool FWantTab;
  bool fautosizescroll;

  bool fscrollresizing;

  void scrollbarscancallthis(TScrollBarKind kind, int newpos, TScrollCode code);
  void scrollinvalidate(TScrollBarKind kind, int newpos);
  void GetOverflow(int &uw, int &uh, int &hw, int &hh);
  friend TZControlScrollBar;

  bool __fastcall get_tabstop();
  void __fastcall set_tabstop(bool val);

  void __fastcall SetScrollBars(TScrollStyle Value);
  void __fastcall SetBorderStyle(TZBorderStyle Value);
  void __fastcall SetVertScrollBar(TZControlScrollBar *Value);
  void __fastcall SetHorzScrollBar(TZControlScrollBar *Value);
  TZControlScrollBar* __fastcall GetVertScrollBar();
  TZControlScrollBar* __fastcall GetHorzScrollBar();
  void __fastcall SetHMax(int Value);
  void __fastcall SetVMax(int Value);
  void __fastcall SetHPos(int Value);
  void __fastcall SetVPos(int Value);
  int __fastcall GetHMax();
  int __fastcall GetVMax();
  int __fastcall GetHPos();
  int __fastcall GetVPos();
  void __fastcall SetEnabled(bool val);
  bool __fastcall GetEnabled();
  TRect __fastcall GetClientRect();
  DYNAMIC void __fastcall MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);

  void __fastcall WMVScroll(TMessage &msg);
  void __fastcall WMHScroll(TMessage &msg);
  void __fastcall WMSize(TWMSize &msg);
  void __fastcall WMEraseBkgnd(TWMEraseBkgnd &msg);
  void __fastcall CMCtl3DChanged(TMessage &msg);

  void __fastcall WMMouseWheel(TMessage &msg);
  void __fastcall WMMouseHWheel(TMessage &msg);

  void __fastcall CMFontChanged(TMessage &msg);
  void __fastcall CMParentFontChanged(TMessage &msg);

  void __fastcall WMCaptureChanged(TMessage &msg);
  void __fastcall CMCaptureChanged(TMessage &msg);

  void __fastcall CMScrollResize(TMessage &msg);
protected:
#pragma warn -8118
BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_ERASEBKGND, TWMEraseBkgnd, WMEraseBkgnd);
  MESSAGE_HANDLER(CM_CTL3DCHANGED, TMessage, CMCtl3DChanged);
  MESSAGE_HANDLER(WM_SIZE,TWMSize,WMSize);
  MESSAGE_HANDLER(WM_VSCROLL,TMessage,WMVScroll);
  MESSAGE_HANDLER(WM_HSCROLL,TMessage,WMHScroll);
  MESSAGE_HANDLER(CM_FONTCHANGED, TMessage, CMFontChanged);
  MESSAGE_HANDLER(CM_PARENTFONTCHANGED, TMessage, CMParentFontChanged);
  MESSAGE_HANDLER(WM_MOUSEWHEEL, TMessage, WMMouseWheel);
  MESSAGE_HANDLER(WM_MOUSEHWHEEL, TMessage, WMMouseHWheel);
  MESSAGE_HANDLER(WM_CAPTURECHANGED, TMessage, WMCaptureChanged);
  MESSAGE_HANDLER(CM_CAPTURECHANGED, TMessage, CMCaptureChanged);
  MESSAGE_HANDLER(CM_SCROLLRESIZE, TMessage, CMScrollResize);
END_MESSAGE_MAP(TCustomControl)
#pragma warn .8118

  void __fastcall CreateParams(TCreateParams &Params);
  void __fastcall Paint();
  void __fastcall SubWndProc(TMessage &Message);

  TForm* GetTopOwner();

  __property TZBorderStyle BorderStyle = {read=FBorderStyle, write=SetBorderStyle, default=1};
  __property TZControlScrollBar *VertScrollBar = { read=GetVertScrollBar, write=SetVertScrollBar, stored=true };
  __property TZControlScrollBar *HorzScrollBar = { read=GetHorzScrollBar, write=SetHorzScrollBar, stored=true };
  __property Font;
  __property Align;
  __property Anchors;
  __property Color = { default=clWindow };
  __property Ctl3D;
  __property ParentColor = { default=false };
  __property bool InvalidateOnScroll = { read=FInvalidateOnScroll, write=FInvalidateOnScroll, default=false };
  __property bool Enabled = { read=GetEnabled, write=SetEnabled, default=true };
  __property bool WantTab = { read=FWantTab, write=FWantTab, default=false };
  __property bool AutoSizeScrollbars = { read=fautosizescroll, write=fautosizescroll, default=false };

  __property TNotifyEvent OnPaint = { read=FOnPaint, write=FOnPaint };
  __property TNCPaintEvent OnNCPaint = { read=FOnNCPaint, write=FOnNCPaint };
  __property TScrollEvent OnVScroll = { read=FOnVScroll, write=FOnVScroll };
  __property TScrollEvent OnHScroll = { read=FOnHScroll, write=FOnHScroll };
  __property TNotifyEvent OnMouseEnter = { read=FOnMouseEnter, write=FOnMouseEnter };
  __property TNotifyEvent OnMouseLeave = { read=FOnMouseLeave, write=FOnMouseLeave };
  __property TEraseBkgndEvent OnEraseBkgnd = { read=FOnEraseBkgnd, write=FOnEraseBkgnd };
  __property TGetScrollOverflowEvent OnGetOverflow = { read=FOnGetOverflow, write=FOnGetOverflow };
  __property TNotifyEvent OnCaptureChanged = { read=FOnCaptureChanged, write=FOnCaptureChanged };

  __property bool TabStop = { read=get_tabstop, write=set_tabstop, default=true};
public:
  bool RectVisible(TRect ARect);
  bool PointVisible(TPoint APoint);
  void FocusNext();
  void RecreateWnd();

  void ScrollResize();

  __fastcall TZCustomPaintBox(TComponent* Owner);
  __fastcall ~TZCustomPaintBox();
  __property TRect ClientRect = { read=GetClientRect };
  __property Canvas;
  __property int HPos = { read=GetHPos, write=SetHPos};
  __property int HMax = { read=GetHMax, write=SetHMax};
  __property int VPos = { read=GetVPos, write=SetVPos};
  __property int VMax = { read=GetVMax, write=SetVMax};
  __property bool KeyRepeat = { read=fkeyrepeat };
  __property TForm* TopOwner = { read=GetTopOwner };
__published:

};

class PACKAGE TZPaintBox : public TZCustomPaintBox
{
public:
  __fastcall TZPaintBox(TComponent* Owner);
__published:
/* Properties */
  __property Align;
  __property Anchors;
  __property AutoSizeScrollbars;
  __property BorderStyle = { default=1 };
  __property ClientWidth;
  __property ClientHeight;
  __property Color = { default=clWindow };
  __property Ctl3D;
  __property Cursor;
  __property DragCursor;
  __property DragKind;
  __property DragMode;
  __property Enabled = { default=true };
  __property Font;
  __property Height = { default=41 };
  __property HelpContext;
  __property Hint;
  __property HorzScrollBar;
  __property InvalidateOnScroll = { default=false };
  __property KeyRepeat;
  __property Left;
  __property Name;
  __property ParentColor = { default=false };
  __property ParentCtl3D;
  __property ParentFont;
  __property ParentShowHint;
  __property PopupMenu;
  __property ShowHint;
  __property TabOrder;
  __property TabStop = { default=True };
  __property Tag;
  __property Top;
  __property VertScrollBar;
  __property Visible;
  __property Width = { default=185 };
  __property WantTab = { default = false };
/* Events */
  __property OnPaint;
  __property OnNCPaint;
  __property OnVScroll;
  __property OnHScroll;
  __property OnKeyDown;
  __property OnKeyPress;
  __property OnKeyUp;
  __property OnEnter;
  __property OnExit;
  __property OnResize;
  __property OnClick;
  __property OnDblClick;
  __property OnMouseDown;
  __property OnMouseMove;
  __property OnMouseUp;
  __property OnMouseEnter;
  __property OnMouseLeave;
  __property OnEraseBkgnd;
  __property OnGetOverflow;
  __property OnMouseActivate;
  __property OnCaptureChanged;
};

//---------------------------------------------------------------------------
#endif
