//---------------------------------------------------------------------------

#ifndef strokebaseH
#define strokebaseH
//---------------------------------------------------------------------------

#include "ZPaintBox.h"

class TPointListEx;
class TRecPointList;

//Base class for all character drawing surfaces. Override the procstroke method and use the points
//property to do something with the drawn lines one by one.
//This class does not store the drawn lines, only the last one in the points property.
class TStrokeObjBase
{
private:
    bool fdrawing;
    TPoint lastmouse;
    TPointListEx *fpoints;
    TFuncPtr fcallback;
    double lastpenwidth;

    TNotifyEvent fonpaint;
    TMouseEvent fonmousedown;
    TMouseMoveEvent fonmousemove;
    TMouseEvent fonmouseup;
    TNotifyEvent fonformdeactivate;
    TNotifyEvent fonexit;
    TNotifyEvent fonresize;
    void __fastcall pbPaint(TObject *Sender);
    void __fastcall pbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y);
    void __fastcall pbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y);
    void __fastcall pbMouseMove(TObject *Sender, TShiftState Shift, int x, int y);
    void __fastcall formDeactivate(TObject *Sender);
    void appDeactivate(void *caller);
    void __fastcall pbExit(TObject *Sender);
    void __fastcall pbResize(TObject *Sender);
    void __fastcall deactivated();

protected:
    virtual void DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y);

    virtual void procstroke() = 0; //called when the current line drawing is finished
    virtual void redraw(); //override this method to draw on the surface. the base method clears the bitmap
    virtual void reset(); //empties points and clears the bitmap

    TBmp *bmp; //use this bitmap to display the drawing surface
    TZPaintBox *pb; //the paintbox which will receive the mouse input and show the bmp
    void clear_bmp(); //empties the bmp and invalidates the paintbox

    void draw(TPointListEx *pts); //draws a line consisting of a list of points on the bmp

    __property TPointListEx *points = { read = fpoints }; //list of the points last drawn on the surface
    __property bool drawing = { read = fdrawing }; //true when drawing is in progress
public:
    TStrokeObjBase(TZPaintBox *fpb);
    virtual ~TStrokeObjBase();

    __property TFuncPtr callback = { read = fcallback, write = fcallback };
};

class TStrokeObj : public TStrokeObjBase
{
private:
    const int RESULT_COUNT_MAX;

    TRecPointList *strokes;

    int *results;
    int resultcnt;

    bool fallowerror;
    bool fgen;
    bool fkan;
    bool fgrid;
    int fcntlimit;

    void redraw();

    int get_results(int ix)
    {
        return results[ix];
    }
    int get_cnt();

    void set_grid(bool agrid);
    void set_kan(bool val);
    void set_gen(bool val);
protected:
    virtual void DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y);
    virtual void procstroke();
public:
    TStrokeObj(TZPaintBox *fpb);
    virtual ~TStrokeObj();

    virtual void reset();
    void undo();

    __property int Results[int ix] = { read = get_results };
    __property int ResultCount = { read = resultcnt };
    __property int Count = { read = get_cnt };

    __property bool AllowError = { read = fallowerror, write = fallowerror }; //allow mistaking order of adjacent strokes
    __property bool GeneralChar = { read = fgen, write = set_gen }; //recognize general characters, eg. kana, abc
    __property bool KanjiChar = { read = fkan, write = set_kan }; //recognize general kanji characters. either this or the GeneralChar must be set to true
    __property int StrokeLimit = { read = fcntlimit, write = fcntlimit }; //skip characters with a stroke count difference over this limit. 0 means no skip (characters with strokes less than the drawn -3 or half of the drawn are still skipped)

    __property bool Grid = { read = fgrid, write = set_grid };
};

#endif
