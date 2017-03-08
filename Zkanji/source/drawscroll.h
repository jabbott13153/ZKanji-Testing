//---------------------------------------------------------------------------

#ifndef drawscrollH
#define drawscrollH
//---------------------------------------------------------------------------

#include "ZPaintBox.h"

enum TDrawScrollOrientation { dsoHorizontal, dsoVertical };

//class for a painted scrollbar at the bottom of a zpaintbox
class TDrawScroll
{
private:
    TZPaintBox *pb;
    int size;
    int sbpos;

    int ssize;
    int flargech;
    int fthumb;
    int sbarpos;
    int frange;
    int funitsize;
    bool sscroll;
    bool spagescroll;
    int sdragpos;

    bool fenabled;

    TDrawScrollOrientation fori;

    TNotifyEvent fonpaint;
    TMouseEvent fonmousedown;
    TMouseEvent fonmouseup;
    TMouseMoveEvent fonmousemove;
    TNotifyEvent fonresize;

    TFuncPtr fcallback;

    void __fastcall repeatfunc(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y);

    void __fastcall pbPaint(TObject *Sender);
    void __fastcall pbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y);
    void __fastcall pbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y);
    void __fastcall pbMouseMove(TObject *Sender, TShiftState Shift, int x, int y);
    void __fastcall pbResize(TObject *Sender);

    void set_pos(int val);
    void set_range(int val);
    void set_orientation(TDrawScrollOrientation val);
    TRect get_client();
    TRect get_rect();
    int get_clientwidth();
    int get_clientheight();
    void set_thumbsize(int val);
    void set_unitsize(int val);
    int get_largechange();
    void set_largechange(int val);

    bool get_scrolling();

    void UpdateBar();

public:
    TDrawScroll(TZPaintBox *apb, int asize, TFuncPtr acallback);
    virtual ~TDrawScroll();

    void Invalidate(); //invalidates the area of the scrollbar

    __property int Size = { read = size };
    __property int Range = { read = frange, write = set_range };
    __property int Position = { read = sbpos, write = set_pos };
    __property int ThumbSize = { read = ssize, write = set_thumbsize };
    __property int UnitSize = { read = funitsize, write = set_unitsize };
    __property int LargeChange = { read = get_largechange, write = set_largechange };
    __property bool Scrolling = { read = get_scrolling };

    __property TDrawScrollOrientation Orientation = { read = fori, write = set_orientation };

    __property TRect ClientRect = { read = get_client };
    __property TRect ScrollRect = { read = get_rect };
    __property int ClientWidth = { read = get_clientwidth };
    __property int ClientHeight = { read = get_clientheight };
};

#endif
