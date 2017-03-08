//---------------------------------------------------------------------------

#ifndef mouseoversH
#define mouseoversH
//---------------------------------------------------------------------------

class TZPaintBox;

//TMouseOvers class: set of rectangles on a control with a canvas. When the mouse is moved into a rectangle the item's state is
//visually updated. Use for custom controls that need rectangular items on them that can be hovered and/or clicked.

typedef void(__closure * TMouseOversPrepareProc)(TCanvas * c, int index, bool selected);
typedef void(__closure * TMouseOversMeasureItemProc)(/*in*/TCanvas * c, int index, /*in and out*/int &x, int &y, /*out*/TRect & rect, int &data, bool &active);
typedef void(__closure * TMouseOversDrawItemProc)(TCanvas * c, const TRect & r, int index, int data, bool selected);
typedef void(__closure * TMouseOversChangeProc)(const TRect & r, int index, int data, bool selected);

class TMouseOvers
{
    struct TSavedSettings
    {
        TFontStyles fst;
        TColor fc;
        UnicodeString fn;
        int fs;

        TPenStyle ps;
        TPenMode pm;
        TColor pc;
        int pw;

        TBrushStyle bs;
        TColor bc;
    };
    TSavedSettings saved;

    enum TControlKind { ckUndef, ckForm, ckPB, ckZPB };
    union TCurrentControl
    {
        TForm *form;
        TPaintBox *paintbox;
        TZPaintBox *zpaintbox;
    };

    TCurrentControl control;
    TControlKind controlkind;
    TCanvas *canvas;

    int cnt;

    TPoint ptpos; //current mouse coordinate within the control. uses scrollbar positions to decide
    int ixpos; //current rectangle the mouse is over

    bool locked;
    bool visible;
    bool enabled;

    int fhorzscroll;
    int fvertscroll;

    int ftag;

    //int drawx;
    //int drawy;

    struct TItemRect
    {
        TRect r;
        int data;
        bool active;

        TItemRect(const TRect &r, int data, bool active) : r(r), data(data), active(active)
        {
            ;
        }
    };
    typedef TTemplateList<TItemRect, true> TItemRectList;
    TItemRectList *rects;

    void draw_item(int ix, bool selected, bool prepare);
    void prepare_init(int ix, bool selected);
    void prepare_restore();

    //events saved from control
    TNotifyEvent fmouseleave;
    TMouseMoveEvent fmousemove;
    void __fastcall DoMouseLeave(TObject *Sender);
    void __fastcall DoMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void DoChange(TRect r, int index, int data, bool selected);

    //custom events
    TMouseOversPrepareProc fprepare;
    TMouseOversMeasureItemProc fmeasure;
    TMouseOversDrawItemProc fdraw;
    TMouseOversChangeProc fchange;

    void updatepos();
    HWND get_handle();

    void set_visible(bool val);
    void set_enabled(bool val);
    int get_pos();

    TControl* get_control();
    __property TControl* Control = { read = get_control };
    __property HWND Handle = { read = get_handle };
public:
    TMouseOvers(TControl *ctrl, TMouseOversPrepareProc prepareproc, TMouseOversMeasureItemProc measureproc, TMouseOversDrawItemProc drawproc, TMouseOversChangeProc changeproc);
    ~TMouseOvers();

    int add(const TRect &r, int data, bool active); //either use add or measureproc but not both. in case add is used, set measureproc to NULL when object is created
    void reset(int count); //clears rectangles list and sets rectangles count to new value
    void draw(int &x, int &y); //measures when needed and draws items to canvas. x and y should be true coordinates modified by scroll position, not original position! (but call scroll to set positions)

    void scroll(int horzscroll, int vertscroll); //call when owner control is scrolled

    void lock(); //locks selection, mouse moves don't count
    void unlock(); //unlocks selection so move of mouse can update state of items

    //void show(); // call show when items need to be drawn again after a hide
    //void hide(); // hides items. stops drawing them and also stops any mouse movement tracking on the owner

    __property TCanvas *Canvas = { read = canvas, write = canvas };
    __property int Position = { read = get_pos };
    __property bool Visible = { read = visible, write = set_visible };
    __property bool Enabled = { read = enabled, write = set_enabled };

    __property int Tag = { read = ftag, write = ftag };
};

#endif
