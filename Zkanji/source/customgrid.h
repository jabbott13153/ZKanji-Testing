//---------------------------------------------------------------------------

#ifndef customgridH
#define customgridH
//---------------------------------------------------------------------------
#include "zpaintbox.h"

enum TGridOrientation { goHorizontal, goVertical };

class TDrawScroll;

class TCustomGridObj
{
private:
    int fsize; //size of a grid cell in pixels

    int flen; //number of visible rows in a vertical and number of visible columns in a horizontal orientation
    int fsiz; //number of visible columns in a vertical and number of visible rows in a horizontal orientation

    TGridOrientation fori;
    int fscrollsize; //> 0 use drawn scrollbar, <= 0 use built in scroll bar
    bool fshowbar;

    int fmousecell;
    bool objfocused;

    TFuncPtr fcallback;

    bool fpainting;

    int buffcnt;
    TBmp **buff;
    int topline;
    bool requested; //callback was requested
    bool immediate; //callback occurs before anything is repainted

    TNotifyEvent fonresize;
    TMouseEvent fonmouseup;
    TMouseEvent fonmousedown;
    TMouseMoveEvent fonmousemove;
    TNotifyEvent fonmouseleave;
    TNotifyEvent fonmouseenter;
    TKeyEvent fonkeydown;
    TKeyPressEvent fonkeypress;
    TNotifyEvent fonenter;
    TNotifyEvent fonexit;

    TScrollEvent fonvscroll;
    TScrollEvent fonhscroll;

    TScrollEvent pbscrollfunc; //either fonvscroll or fonhscroll, depending on the orientation

    void DoCallback();

    void __fastcall pbPaint(TObject *Sender);
    void __fastcall pbResize(TObject *Sender);
    void __fastcall pbMouseEnter(TObject *Sender);
    void __fastcall pbMouseLeave(TObject *Sender);
    void __fastcall pbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y);
    void __fastcall pbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y);
    void __fastcall pbMouseMove(TObject *Sender, TShiftState Shift, int x, int y);
    void __fastcall pbKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall pbVScroll(TObject* Sender, TScrollCode ScrollCode, int &ScrollPos);
    void __fastcall pbHScroll(TObject* Sender, TScrollCode ScrollCode, int &ScrollPos);
    void __fastcall pbEnter(TObject *Sender);
    void __fastcall pbExit(TObject *Sender);
    void formActivate(void *caller);
    void formDeactivate(void *caller);
    void appActivate(void *caller);
    void appDeactivate(void *caller);
    void Deactivated();
    void Activated();

    void set_mousecell(int ix);

    void set_orientation(TGridOrientation val);
    void set_scrollsize(int ascrollsize);
    void set_scrollbars(bool val);

    int get_width();
    int get_height();
    TRect get_rect();
    TZControlScrollBar* get_bar();

    int get_scrollpos();
    void set_scrollpos(int val);
    int get_scrollrange();
    void set_scrollrange(int val);

    int get_clen();
    int get_csiz();

    bool drawcell(int ix);

    void scallback();

    TDrawScroll *scroll;
    __property TZControlScrollBar *Bar = { read = get_bar };

protected:
    TZPaintBox *pb;

    void clear_buffer();

  /*do gooders*/
    virtual int get_count() = 0;
    virtual void DoDrawCell(TCanvas *c, const TRect &r, int acell) = 0;
    virtual void DoBeginPaint()
    {
    };
    virtual void DoEndPaint()
    {
    };
    virtual bool DoNeedRedraw(int ix)
    {
        return false;
    }
    virtual void DoChanged(int newsel)
    {
    };

    virtual void DoResize()
    {
        pb->Invalidate();
    };
    virtual void DoScroll(int apos);
    virtual void DoActivate();
    virtual void DoDeactivate();

    virtual void DoMouseMove(TShiftState Shift, int x, int y);
    virtual void DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y);
    virtual void DoMouseUp(TMouseButton Button, TShiftState Shift, int x, int y);
    virtual void DoMouseLeave();
    virtual void DoMouseEnter();
    virtual void DoKeyDown(Word &Key, TShiftState Shift) {}
    virtual void DoKeyPress(wchar_t &Key) {}
  /*----------*/

    void RequestCallback();
    void CallbackNow();
    void invalidate_cell(int ix);
    virtual void changed(int newpos);

    __property int clientlen = { read = get_clen };
    __property int clientsiz = { read = get_csiz };

    __property int mousecell = { read = fmousecell, write = set_mousecell };
    __property bool painting = { read = fpainting };

public:
    TCustomGridObj(TZPaintBox *apb, int aboxsize, TFuncPtr acallback);
    virtual ~TCustomGridObj();

    void InvalidateCell(int ix);
    void Redraw();

    int CellAtPos(TPoint p);

    void measure_box_size();

    __property TGridOrientation orientation = { read = fori, write = set_orientation };
    __property int scrollsize = { read = fscrollsize, write = set_scrollsize };
    __property bool scrollbars = { read = fshowbar, write = set_scrollbars };

    __property int ClientWidth = { read = get_width };
    __property int ClientHeight = { read = get_height };
    __property TRect ClientRect = { read = get_rect };
    __property int ScrollPos = { read = get_scrollpos, write = set_scrollpos };
    __property int ScrollRange = { read = get_scrollrange, write = set_scrollrange };

    __property int Columns = { read = fsiz };
    __property int Rows = { read = flen };

    __property int BoxSize = { read = fsize };

    __property int count = { read = get_count };
    __property bool focused = { read = objfocused };
};

class TCustomMultiselectGridObj : public TCustomGridObj
{
private:
    typedef TCustomGridObj inherited;

    int fpos;
    bool fmulti;

    TGapList<bool> *fsellist;
    TGapList<bool> *psellist;
    bool pfocused;

    void set_pos(int i);
    void set_pos2(int i, bool resetsel = false);
    bool get_selected(int ix);
    void set_selected(int ix, bool val);
    int get_selcount();
    unsigned int get_selpos(unsigned int ix);
    bool get_simplesel();
    bool get_hasselected();
    void set_multiselect(bool val);

    bool ScrollInView(int apos, int aoldpos);
protected:
  /*inherited virtual functions*/
    virtual void DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y);
    virtual void DoResize();
    virtual void DoKeyDown(Word &Key, TShiftState Shift);
    virtual bool DoNeedRedraw(int ix);
    virtual void DoEndPaint();
    virtual void DoChanged(int newpos);
    virtual void DoChanged(int newpos, int newsellength);

  /*own virtual functions*/
    virtual void update_selected();
    virtual void DoChangePos(int &anewpos, bool &resetsel)
    {
        ;
    }

    virtual void changed(int newpos);
    virtual void changed(int newpos, int newsellength); //call this when the list containing the items change
public:
    TCustomMultiselectGridObj(TZPaintBox *apb, int aboxsize, bool amultiselect, TFuncPtr acallback);
    virtual ~TCustomMultiselectGridObj();

    __property int pos = { read = fpos, write = set_pos };
    __property bool selected[int ix] = { read = get_selected, write = set_selected };
    __property bool hasselected = { read = get_hasselected };
    __property int selcount = { read = get_selcount };
    __property unsigned int selpos[unsigned int ix] = { read = get_selpos };
    __property bool simplesel = { read = get_simplesel };
    __property bool multiselect = { read = fmulti, write = set_multiselect };
};

#endif
