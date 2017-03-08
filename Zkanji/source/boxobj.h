//---------------------------------------------------------------------------

#ifndef boxobjH
#define boxobjH
//---------------------------------------------------------------------------

#include "wordgrid.h"

struct TKanji;
class TWordCollection;
class TExampleList;
class TKanjiCard;

class TReadingBoxObj : public TWordListBase
{
private:
    typedef TWordListBase inherited;

    int fmouseon; //index of row and checkbox the mouse is on
    bool fmousedown; //the mouse is pressed on a checkbox
    bool fmouseover; //the mouse is over the checkbox while the mouse button is pressed

    TKanji *fkanji;
    TWordCollection *fcollection;

    TKanji *fcallbackkanji;
    int fcallbackreading;

    bool fgrouping;
    int fkungroups; //number of kun readings for current kanji when it is grouped

    enum TCheckState { csNormal, csHot, csPushed, csDisabled };
    void draw_checkbox(TRect r, bool checked, TCheckState state, bool fillbg = true);
    TRect checkbox_rect(TRect r);
    void set_mouseon(int on);

    void count_count();
    void set_collection(TWordCollection *acoll);
    void set_grouping(bool val);

    int sel_find_up(int newsel);

    void set_kanji(TKanji *akanji);
protected:
    virtual void update_count(int &count);

    virtual void DoCallback();

    virtual void DoDrawHead(int col, TRect r);
    virtual void DoDrawCell(int col, int row, TRect r);
    virtual void DoPrepareDraw(int col, int row);

    virtual void DoMouseMove(int acol, int arow, TShiftState shift, int x, int y);
    virtual void DoMouseDown(int acol, int arow, TMouseButton button, TShiftState shift, int x, int y);
    virtual void DoMouseUp(int acol, int arow, TMouseButton button, TShiftState shift, int x, int y);
    virtual void DoMouseLeave();

    virtual void DoDeactivate(bool focused);
    virtual void DoSettingsChanged(TSettingsProcState state);
public:
    TReadingBoxObj(TZPaintBox *abox, int arowsize, TFuncPtr acallback);
    virtual ~TReadingBoxObj();

    void invalidate();
    int compact_index(int index = -1);
    int real_index(int index = -1);

    __property TKanji *kanji = { read = fkanji, write = set_kanji };
    __property TWordCollection *collection = { read = fcollection, write = set_collection };

    __property bool grouping = { read = fgrouping, write = set_grouping };
};

#endif
