//---------------------------------------------------------------------------

#ifndef statobjH
#define statobjH
//---------------------------------------------------------------------------
#include "ZPaintBox.h"
//---------------------------------------------------------------------------

class TDrawScroll;
enum TStatGraph
{
    sgColumns, sgArea
};
struct TStatColumn;
class TFastBuffer;

typedef UnicodeString(__closure *TStatNumProc)(int val);
typedef UnicodeString(__closure *TStatKeyProc)(int ix, int key);
typedef UnicodeString(__closure *TStatHintProc)(int ix, const TStatColumn &column);

struct TStatColumn
{
    int *values;
    int valcnt;

    bool stacked;

    int key;
};
class TStatColumns : public TTemplateList<TStatColumn, true>
{
protected:
    typedef TTemplateList<TStatColumn, true> inherited;
    virtual void FreeItem(TStatColumn *item, int ix);
public:
    TStatColumns();
    virtual ~TStatColumns()
    {
        Clear();
    }

    int* Add(int valuecount, int keyindex, bool stacked); //add an item to the statistic list. valuecount is the number of values represented in that column, keyindex is an index for the column (eg. the day of a data), stacked is true when the items should come on top of each other
    void AddEmptyKey(int keyindex); //add an empty column with the given key index (eg. the integer part of a datetime)

    void Assign(TStatColumns *acols);
};

class TStatObj
{
private:
    TZPaintBox *pb;
    TDrawScroll *scroll;
    TNotifyEvent fonresize;
    TNotifyEvent fonmouseleave;
    TMouseMoveEvent fonmousemove;
    TMouseEvent fonmouseup;
    TMouseEvent fonmousedown;
    TNotifyEvent fonpaint;

    TCursor fcursor;

    int fpos;
    int buffpos;
    int buffcnt;
    TBmp **buff;
    TBmp *vnumbmp;
    TBmp *hnumbmp;
    bool drawvnum;
    bool drawhnum;

    int fbasekey;
    int fkeyinc;

    bool fmoving;
    int fmousex;
    int fscroll;
    int fredline;

    bool fpopup;

    int fnumw; //number area width
    int fnumh; //number area height
    int fsize; //width of column including padding
    int fpad; //padding of column
    TStatGraph fgraph; //columns or area graph

    UnicodeString xstr;
    UnicodeString ystr;
    int fmax; //maximum value vertically
    int fhsize; //minimum height of a row
    int fnumdiv; //lowest division of values shown on the left side

    int colh; //height of tallest column in pixels. 3% less than draw area
    double fdivh; //vertical division in pixels
    int fdivval; //value of a single division, set to 10 for decimal, 300 for time/seconds

    TRect get_drawarea();
    void set_max(int val);

    void draw_numbers();
    void draw_buff(int i);
    bool division_zero(int val);
    int division_size(int val, int &i, double h);
    void measure();
    void allocate();
    void count_numw();
    void count_numh();
    void set_numfont(TCanvas *c);
    void set_popup(bool val);
    void set_hintproc(TStatHintProc hproc);
    void set_pos(int val);
    int get_count();
    const TStatColumn* get_col(int ix);

    TStatNumProc fnumproc;
    TStatKeyProc fkeyproc;
    TStatHintProc fhintproc;

    TStatColumns *cols;

    TFastBuffer *buffer;

protected:
    void __fastcall pbPaint(TObject *Sender);
    void __fastcall pbResize(TObject *Sender);
    void __fastcall pbMouseMove(TObject *Sender, TShiftState shift, int x, int y);
    void __fastcall pbMouseLeave(TObject *Sender);
    void __fastcall pbMouseDown(TObject *Sender, TMouseButton button, TShiftState shift, int x, int y);
    void __fastcall pbMouseUp(TObject *Sender, TMouseButton button, TShiftState shift, int x, int y);

    void appActivate(void *caller);
    void appDeactivate(void *caller);
    void formActivate(void *caller);
    void formDeactivate(void *caller);

    void scrollproc();
    void clear_buffer();
public:
    TStatObj(TZPaintBox *apb);
    virtual ~TStatObj();

    void set_items(TStatColumns *acols, UnicodeString xaxis, UnicodeString yaxis, int maxval, int numberdivision, int redline, TStatNumProc anumproc, TStatKeyProc akeyproc, TStatHintProc ahintproc);

    __property TRect drawarea = { read = get_drawarea };
    __property int maxvalue = { read = fmax, write = set_max };
    __property int count = { read = get_count };
    __property bool showpopup = { read = fpopup, write = set_popup };
    __property const TStatColumn *columns[int ix] = { read = get_col };

    __property int column_size = { read = fsize };
    __property int pos = { read = fpos, write = set_pos };
    __property int redline = { read = fredline };

    __property int basekey = { read = fbasekey, write = fbasekey }; //when no columns present this will be the key for the first one
    __property int keyincrement = { read = fkeyinc, write = fkeyinc }; //increment for the keys of columns after the last one that had data

    __property TCursor cursor = { read = fcursor };
};

#endif
