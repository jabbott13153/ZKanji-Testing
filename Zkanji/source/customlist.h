//---------------------------------------------------------------------------

#ifndef customlistH
#define customlistH
//---------------------------------------------------------------------------
#include "ZPaintBox.h"
#include "bits.h"
//---------------------------------------------------------------------------

class TCustomListObj/*: public TObject*/
{
private:
#ifdef _DEBUG
    UnicodeString stmptmp;
#endif

    int *fcols; //column widths
    byte fcolcnt; //number of columns
    int frowsize; //height of a single row
    int fheadersize; //height of a single row

    Bits colvis; //visibility of columns, true/false values for each

    bool fdopaint; //when set to false no painting occurs. should be used while updating items in the list

    int fcount; //number of rows

    int fmousecol; //column under mouse
    int fmouserow; //row under mouse

    bool ffixvertbar;
    bool ffixhorzbar;

    bool ffocused; //paintbox should appear focused

    bool fheader; //the grid has a header row with an index of -1, this means an additional row not present in fcount

    int fvpos; //old scroll position
    int fhpos; //old scroll position

    TShiftState inline getshiftstate();

    int get_width();
    int get_height();
    TCanvas* get_canvas();
    void set_rowsize(int val);
    void set_headersize(int val);
    int get_colsize(byte col);
    void set_colsize(byte col, int val);
    void set_colcount(byte val);
    void set_header(bool val);
    void set_count(int val);
    void set_fixvertbar(bool val);
    void set_fixhorzbar(bool val);
    void set_colvisible(byte col, bool val);
    bool get_colvisible(byte col);
    byte get_visiblecolcount();
    byte get_realcolindex(byte col); //returns real index of the col'th visible column
    byte get_visiblecolindex(byte col);

  //events
    TMouseEvent fonmousedown;
    TMouseEvent fonmouseup;
    TMouseMoveEvent fonmousemove;
    TKeyEvent fonkeydown;
    TKeyEvent fonkeyup;
    TKeyPressEvent fonkeypress;
    TNotifyEvent fonenter;
    TNotifyEvent fonexit;
    TNotifyEvent fonmouseenter;
    TNotifyEvent fonmouseleave;
    TNotifyEvent fonresize;
    TNotifyEvent fonpaint;
    TScrollEvent fonvscroll;
    TScrollEvent fonhscroll;

    void appactivateproc(void *caller);
    void appdeactivateproc(void *caller);
    void formactivateproc(void *caller);
    void formdeactivateproc(void *caller);
    void settingsChanged(void *caller, TSettingsProcState state);

    void __fastcall exitproc(TObject *Sender);
    void __fastcall enterproc(TObject *Sender);
    void __fastcall mousemoveproc(TObject *Sender, TShiftState shift, int x, int y);
    void __fastcall mouseupproc(TObject *Sender, TMouseButton button, TShiftState shift, int x, int y);
    void __fastcall mousedownproc(TObject *Sender, TMouseButton button, TShiftState shift, int x, int y);
    void __fastcall resizeproc(TObject *Sender);
    void __fastcall paintproc(TObject *Sender);
    void __fastcall mouseenterproc(TObject *Sender);
    void __fastcall mouseleaveproc(TObject *Sender);
    void __fastcall keydownproc(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall keyupproc(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall keypressproc(TObject *Sender, wchar_t &Key);

    void __fastcall vscrollproc(TObject* Sender, TScrollCode ScrollCode, int &ScrollPos);
    void __fastcall hscrollproc(TObject* Sender, TScrollCode ScrollCode, int &ScrollPos);

    void Activated();
    void Deactivated();

    byte signalcount;

protected:
    // Prevents painting list while changing things.
    class StartUpdating
    {
    private:
        bool used;
        TCustomListObj *owner;

        void StopUpdating();
    public:
        StartUpdating(TCustomListObj *owner);
        ~StartUpdating();
    };
    friend StartUpdating;

    TZPaintBox *fbox;

    int on_column(int x);
    int columnleft(byte col);
    void measure_row_size();

    void set_cols(int acolumncount, int *acolwidths, bool *acolvisible, int arowheight, int aheaderheight, int acount, bool aheader, bool refresh = true);

    virtual void DoPaint();
    virtual void DoResize();
    virtual void DoMouseMove(int acol, int arow, TShiftState shift, int x, int y);
    virtual void DoMouseDown(int acol, int arow, TMouseButton button, TShiftState shift, int x, int y);
    virtual void DoMouseUp(int acol, int arow, TMouseButton button, TShiftState shift, int x, int y);
    virtual void DoActivate(bool focused);
    virtual void DoDeactivate(bool focused);

    virtual void DoPrepareDraw(int col, int row) = 0;
    virtual void DoDraw(int col, int row, TRect r) = 0;
    virtual void DoPrepareDrawHeader(int col);
    virtual void DoDrawHeader(int col, TRect r);

    virtual void DoMouseCellChange(int oldcol, int oldrow, int newcol, int newrow);

    virtual void DoMouseEnter();
    virtual void DoMouseLeave();
    virtual void DoSettingsChanged(TSettingsProcState state);
    virtual void DoVScroll(TScrollCode ScrollCode, int &ScrollPos);
    virtual void DoHScroll(TScrollCode ScrollCode, int &ScrollPos);
    virtual void DoKeyDown(WORD &Key, TShiftState Shift);
    virtual void DoKeyUp(WORD &Key, TShiftState Shift);
    virtual void DoKeyPress(wchar_t &Key);

    virtual void DoSetColumnCount(const byte &acolcount, const byte &oldcolcount);
    virtual void DoSetCount(int &acount, int &oldcount);
    virtual void DoChanged(); //called when the count, column count, row height, header etc. changes
public:
    TCustomListObj(TZPaintBox *abox);
    virtual ~TCustomListObj();

    void cellpos(TPoint p, int &col, int &row);
    TRect cellbox(int col, int row);
    TRect rowbox(int row);

    void invalidatecell(unsigned int col, int row);
    void invalidaterow(int row);
    void invalidaterows(int startrow, int count);
    void invalidatecol(int col);

    __property TCanvas *Canvas = { read = get_canvas };

    __property bool fixvertbar = { read = ffixvertbar, write = set_fixvertbar };
    __property bool fixhorzbar = { read = ffixhorzbar, write = set_fixhorzbar };

    __property bool header = { read = fheader, write = set_header };
    __property int count = { read = fcount, write = set_count };
    __property int rowsize = { read = frowsize, write = set_rowsize };
    __property int headersize = { read = fheadersize, write = set_headersize };
    __property int colsize[byte] = { read = get_colsize, write = set_colsize };
    __property byte colcount = { read = fcolcnt, write = set_colcount };
    __property bool colvisible[byte] = { read = get_colvisible, write = set_colvisible };
    __property byte realcolindex[byte] = { read = get_realcolindex };
    __property byte visiblecolcount = { read = get_visiblecolcount };
    __property byte visiblecolindex[byte] = { read = get_visiblecolindex };
    __property int width = { read = get_width };
    __property int height = { read = get_height };

    __property bool focused = { read = ffocused };

    __property int mousecol = { read = fmousecol };
    __property int mouserow = { read = fmouserow };
};

#endif
