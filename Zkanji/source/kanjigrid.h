//---------------------------------------------------------------------------

#ifndef kanjigridH
#define kanjigridH
//---------------------------------------------------------------------------
#include "ZPaintBox.h"

#include "customgrid.h"

struct TWordCollection;
class TZPaintBox;
struct TKanji;
class TKanjiList;
class TKanjiItemList;
struct TKanjiCardItem;
class TKanjiCardList;
enum TKanjiSortType;

typedef void(__closure * TPaintInfoProc)(int index, TCanvas * c, const TRect & r);
typedef void(__closure * TGridColorProc)(int index, TColor & pen, TColor & font, TColor & brush);

class TCustomKanjiGrid : public TCustomMultiselectGridObj
{
private:
    typedef TCustomMultiselectGridObj inherited;
    static HDC Xdc;
    static void* Xbits;
    static HBITMAP Xbitmap;
    static int Xcnt;

    HFONT Kfont;

    TWordCollection* fcollection;

    int hintpos; //last position where the mouse was
    int changing;
    bool fcopy;

    bool fshowhint;

    TPaintInfoProc fpaintinfoproc;
    TGridColorProc fcolorproc;

    void settingsChanged(void *caller, TSettingsProcState state);

    TKanji* get_kanjiitem(word ix);
    TKanji* get_kanji();
    void set_kanji(TKanji *k);
    void set_collection(TWordCollection *acollection);
    void invalidate_cell(int ix);
    bool drawcell(int ix);
    void set_paintinfo(TPaintInfoProc val);
    void set_colorproc(TGridColorProc val);
    UnicodeString get_selstring();
    UnicodeString get_kanjistring();

    void set_mousecell(int ix);
protected:
    virtual TKanjiList* get_list() = 0;
    virtual void DoDrawCell(TCanvas *c, const TRect &r, int acell);
    virtual void DoMouseLeave();
    virtual void DoChangePos(int &anewpos, bool &resetsel);
    virtual void DoSetColors(int ix, TColor &pen, TColor &font, TColor &brush);
    virtual void DoPaintInfo(TCanvas *c, int ix, TRect r);
    virtual void DoMouseMove(TShiftState Shift, int x, int y);
    virtual void DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y);
    virtual void DoSetItems(TKanjiList *alist) = 0;
    virtual void DoScroll(int apos);
    virtual void DoKeyDown(Word &Key, TShiftState Shift);
    virtual void DoKeyPress(wchar_t &Key);

    __property TKanjiList *list = { read = get_list };
public:
    TCustomKanjiGrid(TZPaintBox *apaintbox, int aboxsize, bool amultiselect, TFuncPtr afunc);
    virtual ~TCustomKanjiGrid();

    void set_items(TKanjiList *alist, TWordCollection *acollection, int asel = -1, int asellength = 1, TKanji *aselected = NULL);
    void set_items(TKanjiList *alist, TWordCollection *acollection, TKanji *aselected);

    TKanji* KanjiAtPos(TPoint p);

    __property TKanji *kanji = { read = get_kanji, write = set_kanji };
    __property TKanji *kanjis[word ix] = { read = get_kanjiitem };

    __property TWordCollection *collection = { read = fcollection, write = set_collection };
    __property UnicodeString selstring = { read = get_selstring };
    __property UnicodeString kanjistring = { read = get_kanjistring };

    __property bool allowcopy = { read = fcopy, write = fcopy };
    __property bool showhint = { read = fshowhint, write = fshowhint };

    __property TPaintInfoProc paintinfoproc = { read = fpaintinfoproc, write = set_paintinfo };
    __property TGridColorProc setcolorproc = { read = fcolorproc, write = set_colorproc };
};

// Grid of kanji. Only a copy of the list is used in set_items.
class TKanjiGrid : public TCustomKanjiGrid
{
private:
    TKanjiItemList *flist;
    TKanjiSortType fsortby;

    void set_sortby(TKanjiSortType i);
protected:
    virtual int get_count();
    virtual TKanjiList* get_list()
    {
        return (TKanjiList*)flist;
    }
    virtual void DoSetItems(TKanjiList *alist);
    void DoSetColors(int ix, TColor &pen, TColor &font, TColor &brush);
public:
    TKanjiGrid(TZPaintBox *apaintbox, int aboxsize, bool amultiselect, TFuncPtr afunc);
    virtual ~TKanjiGrid();

    __property TKanjiSortType sort_by = { read = fsortby, write = set_sortby };
};

// Grid of kanji cards. The list passed in set_items is used, and must exist till a new list is selected.
class TKanjiCardGrid : public TCustomKanjiGrid
{
private:
    bool get_isexample(int kix, int wix);
    TKanjiCardItem* get_card(int ix);
    TKanjiCardItem* get_item();

    TKanjiCardList *flist;

protected:
    virtual int get_count();
    virtual TKanjiList* get_list()
    {
        return (TKanjiList*)flist;
    }
    virtual void DoSetItems(TKanjiList *alist);
public:
    TKanjiCardGrid(TZPaintBox *apaintbox, int aboxsize, bool amultiselect, TFuncPtr afunc);
    virtual ~TKanjiCardGrid();

//void AddExample(int kix, int wix);
//void RemoveExample(int kix, int wix);

    __property bool isexample[int kix][int wix] = { read = get_isexample };
    __property TKanjiCardItem *cards[int ix] = { read = get_card };
    __property TKanjiCardItem *card = { read = get_item };
};

extern TTemplateList<TCustomKanjiGrid, false> *kanjigrids;

#endif
