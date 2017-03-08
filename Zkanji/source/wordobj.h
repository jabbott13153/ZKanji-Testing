//---------------------------------------------------------------------------

#ifndef wordobjH
#define wordobjH
//---------------------------------------------------------------------------
#include "wordgrid.h"
#include "ZPaintBox.h"
//---------------------------------------------------------------------------

class TWordIndexList;
class TWordCollection;
struct TWord;
struct TKanji;
struct TWordIndex;
class TWordListObjBase;
class TWordFilterSelection;
struct TInfList;
class TWordGroups;

class TCustomListControlObj;

enum TWordSelectingState { wssNone, wssStarted, wssSelecting };
enum TWordColumnType { wcPop, wcKanji, wcKana, wcMeaning, wcCustom };
enum TColumnControlType { cbtCheckBox };
enum TWordColor { wcUncolored, wcRed, wcGreen, wcBlue, wcYellow, wcLightBrown };
typedef void(__closure * TPopupWordProc)(int x, int y, TWordListObjBase * wobj, TWordColumnType wcc, TWord * w, TKanji * k);

typedef UnicodeString(__closure *TWordListGetHeaderProc)(int col);
typedef void(__closure * TWordListDrawHeaderProc)(int col, TCanvas * c, const TRect & r);
typedef void(__closure * TWordListPrepareProc)(int col, int row, TWordColumnType ctype, TCanvas * c);
typedef void(__closure * TWordListDrawCellProc)(int col, int row, TCanvas * c, const TRect & r);
typedef UnicodeString(__closure *TWordListMeaningProc)(int row);
class TWordListObjBase : public TWordListBase
{
private:
    typedef TWordListBase inherited;

    struct TColumnControlElem
    {
        TColumnControlType type;
        byte column;
    };

    TWordListGetHeaderProc fgetheader;
    TWordListDrawHeaderProc fdrawhead;
    TWordListPrepareProc fprepare;
    TWordListDrawCellProc fdraw;
    TWordListMeaningProc fmeaning;

    TWordColumnType *coltypes;
    TAlignment *colalign;
    TWordColor *colors;
    int colorcount;
    TPopupWordProc fselfunc;

    bool fautoresize;
    bool fcanresize; // Do autoresize
    bool fshowhint;

    bool fshowgroup; // Indicate with an icon that a word is in a group.

    int hintline; // Line where kanji was last hinted.
    TPoint lastFreqhintpos;

    bool fwordselect;
    TWordSelectingState fselecting;
    TPoint fselstartingpos;

    int ftextselcol; // Column of selection.
    int ftextsellength; // Length of selected text.
    int ftextselstart; // Starting position of selection.
    int ftextselpos; // Mouse position over selected text.

    int lastcount;

    TListCheckboxObj *lobj;

    int get_rowsize();
    void set_rowsize(int val);

    TWordColumnType get_coltype(byte col);
    void set_coltype(byte col, TWordColumnType val);
    TAlignment get_colalign(byte col);
    void set_colalign(byte col, TAlignment val);
    UnicodeString get_meaningtext(int ix);

    bool get_selecting();
    int get_selcolumn();

    void set_getheaderproc(TWordListGetHeaderProc aheadfunc);
    void set_drawheadproc(TWordListDrawHeaderProc adrawhead);
    void set_meaningproc(TWordListMeaningProc ameaning);
    void set_prepareproc(TWordListPrepareProc aprepare);
    void set_drawproc(TWordListDrawCellProc adraw);

    void wordgroup(void *caller, TWordCollection *coll, TWord *w);

    //TCustomListControlObj and ancestor property handlers

    TListCustomControlChanged get_checkproc();
    TListVirtualCheckControlGetter get_virtualcheckproc();

    bool get_checkcolumn(byte col);
    void set_checkcolumn(byte col, bool val);
    bool get_checked(int row, byte col);
    void set_checked(int row, byte col, bool val);
    bool get_checkenabled(int row, byte col);
    void set_checkenabled(int row, byte col, bool val);

    bool get_multicheck();
    void set_multicheck(bool val);
    bool get_multichecksel();
    void set_multichecksel(bool val);
protected:
    // Drawing functions
    virtual void DoDrawHead(int col, TRect r);
    virtual void DoDrawCell(int col, int row, TRect r);
    virtual void DoPrepareDraw(int col, int row);

    virtual void DoSetColumnCount(const byte &acolcount, const byte &oldcolcount);
    virtual void DoSettingsChanged(TSettingsProcState state);

    // Returns the index of character at the x coordinate within a kana or kanji.
    int word_position(int row, int col, int x, bool out_of_range);

    UnicodeString get_wnotes();

    //------------------------------------------------------------------------
    // Getter functions for word drawing and member check.
    //------------------------------------------------------------------------
    virtual TWordIndex get_item(unsigned int i) = 0;

    // By default returns the word's data if the list supports words. Override if not.
    virtual word itemfreq(unsigned int i);
    virtual UnicodeString itemkanji(unsigned int i);
    virtual UnicodeString itemkana(unsigned int i);
    virtual byte itemmeaningcount(unsigned int i);
    virtual UnicodeString itemmeaning(unsigned int i, byte m);
    virtual void itemtags(unsigned int i, byte m, int &types, int &notes, byte &fields, word &nametag);
    virtual TWordGroups* itemgroups(unsigned int i);
    //------------------------------------------------------------------------

    int get_index(unsigned int i);
    char get_meaningindex(unsigned int i);
    TWord* get_word(unsigned int i);

    int get_current();
    int get_currentmeaning();
    TWord* get_currentword();
    UnicodeString get_selstring();
    virtual TWordCollection* get_collection() = 0;

    // Row selection getters and setters.
    void set_autoresize(bool val);
    void set_showgroup(bool val);
    virtual void ResetDefaults();

    TWordColor get_color(unsigned int ix);
    void set_color(unsigned int ix, TWordColor val);

    void set_pos2(int val, bool resetsel);

    void cellshift(int acol, int arow, int &x, int &y); // Modifies x and y mouse coordinate to a virtual cell that is shifted by controls placed inside the cell by a TCustomListControlObj.

    //void drawcontrols(int col, int row, TRect &r);

    //events
    virtual void DoPaint();
    virtual void DoMouseLeave();
    virtual void DoMouseDown(int acol, int arow, TMouseButton Button, TShiftState Shift, int x, int y);
    virtual void DoMouseMove(int acol, int arow, TShiftState Shift, int x, int y);
    virtual void DoMouseUp(int acol, int arow, TMouseButton Button, TShiftState Shift, int x, int y);
    virtual void DoResize();
    virtual void DoVScroll(TScrollCode ScrollCode, int &ScrollPos);
    virtual void DoHScroll(TScrollCode ScrollCode, int &ScrollPos);
    virtual void DoDeactivate(bool focused);
    virtual void DoWordGroup(TWordCollection *coll, TWord *w);
    virtual void DoChanged();
    virtual void DoColResizing(bool newval);

    __property TWordIndex items[unsigned int] = { read = get_item };
public:
    TWordListObjBase(TZPaintBox *apaintbox, int arowsize, TFuncPtr afunc, TPopupWordProc aselfunc);
    virtual ~TWordListObjBase();

    wchar_t KanjiAtPos(int x, int y);
    int IndexAtPos(int y);
    TWord* WordAtPos(int y, int &index, char &meaning); // Return word and sets index to the index of the word.

    int MeasureCellText(UnicodeString str); // Measures a string's width if it would be drawn with the current settings. must be called from drawproc or it doesn't give reliable results.
    void DrawCellText(TRect r, UnicodeString str, TAlignment alignment); // Draws a string in the cell with the given alignment.

    void set_cols(int acolcnt, int *acolwidths, int *acoldefaultwidths, bool *acolvisible, TWordColumnType *acoltypes, TAlignment *acolalign, int acount);

    void Refresh();

    void RemoveColor(bool invalidate);

    void SetCheckboxHandler(TListCustomControlChanged fcallback); // The state of boxes is saved in the word object.
    void SetCheckboxHandler(TListCustomControlChanged fcallback, TListVirtualCheckControlGetter fgcallback); // Virtual checkbox handler. The boxes are handled by the caller.

    __property TWordListGetHeaderProc getheaderproc = { read = fgetheader, write = set_getheaderproc };
    __property TWordListDrawHeaderProc drawheaderproc = { read = fdrawhead, write = set_drawheadproc };
    __property TWordListMeaningProc meaningproc = { read = fmeaning, write = set_meaningproc };
    __property TWordListPrepareProc prepareproc = { read = fprepare, write = set_prepareproc };
    __property TWordListDrawCellProc drawproc = { read = fdraw, write = set_drawproc };

    __property TWord *currentword = { read = get_currentword };
    __property int indexes[unsigned int] = { read = get_index };
    __property char meaningindexes[unsigned int] = { read = get_meaningindex };

    __property TWord* words[unsigned int] = { read = get_word };
    __property bool wordselect = { read = fwordselect, write = fwordselect }; // Allow selecting part of word.

    __property int current = { read = get_current };
    __property int currentmeaning = { read = get_currentmeaning };
    __property UnicodeString wordnotes = { read = get_wnotes };

    __property UnicodeString selstring = { read = get_selstring };
    __property bool selecting = { read = get_selecting };
    __property int selcolumn = { read = get_selcolumn };

    __property TWordCollection *collection = { read = get_collection }; // Dictionary of the currently selected list.

    __property TWordColor rowcolors[unsigned int] = { read = get_color, write = set_color };

    __property TWordColumnType coltype[byte ix] = { read = get_coltype, write = set_coltype };
    __property TAlignment colalignment[byte ix] = { read = get_colalign, write = set_colalign };

    __property int rowsize = { read = get_rowsize, write = set_rowsize };
    __property bool autoresize = { read = fautoresize, write = set_autoresize };
    __property bool showhint = { read = fshowhint, write = fshowhint };
    __property bool showgroup = { read = fshowgroup, write = set_showgroup };

    __property UnicodeString meaningtext[int ix] = { read = get_meaningtext };

    //TListCustomControlChanged and ancestor properties
    __property bool checkcolumn[byte col] = { read = get_checkcolumn, write = set_checkcolumn }; // Add checkbox handler functions with SetCheckboxHandler first.
    __property bool checked[int row][byte col] = { read = get_checked, write = set_checked };
    __property bool checkenabled[int row][byte col] = { read = get_checkenabled, write = set_checkenabled };
    __property bool multicheckcolums = { read = get_multicheck, write = set_multicheck };
    __property bool multicheckselected = { read = get_multichecksel, write = set_multichecksel };
};

// Word list with setter function and continuous listing mode.
class TWordListObj : public TWordListObjBase
{
private:
    typedef TWordListObjBase inherited;

    bool fcontinuous;
    bool ffiltered;
    TWordCollection *fcoll;
    int fcollection;

    TWordIndexList *flist;

    virtual TWordCollection* get_collection();

    using inherited::set_cols;
protected:
    virtual TWordIndex get_item(unsigned int i);

    virtual void DoPrepareDraw(int col, int row);
    virtual void update_count(int &count);

    void set_pos_vpos(int apos, int vpos);
public:
    TWordListObj(TZPaintBox *apaintbox, int arowsize, TFuncPtr afunc, TPopupWordProc aselfunc);
    virtual ~TWordListObj();

    void set_by_indexes(TWordIndexList *indexes, bool holdposition = false);
    void set_cols(TWordIndexList *indexes, int acolcnt, int *acolwidths, int *acoldefaultwidths, bool *acolvisible, TWordColumnType *acoltypes, TAlignment *colalign);
    int IndexOf(int ix);

    // Turn browsing mode on or off. Set acoll to NULL to make the list non-continuous.
    void set_to_continuous(TWordCollection *acoll, const wchar_t *aword, bool filtered);
    void find_in_continuous(const wchar_t *aword);

    __property bool continuous = { read = fcontinuous };
};

class TWordFilterCopyList;

// Word list with accessible wordlist property.
class TVocabListObj : public TWordListObjBase
{
private:
    typedef TWordListObjBase inherited;

    TWordIndexList *flist;
    TIntegerList *filtered;
    UnicodeString filterstr;
    TWordFilterCopyList *setfilters;

    void set_wordlist(TWordIndexList *val);
    void filter_list();

protected:
    virtual TWordIndex get_item(unsigned int i);

    virtual TWordCollection* get_collection();
    virtual void update_count(int &count);

    int get_order(unsigned int ix);
public:
    TVocabListObj(TZPaintBox *apaintbox, int arowsize, TFuncPtr afunc, TPopupWordProc aselfunc);
    virtual ~TVocabListObj();

    UnicodeString get_mnotes();
    int FindWord(int aindex, byte ameaning, bool in_filtered);
    virtual void Update(int aselstart, int asellength);
    virtual void Update(int row);

    void set_filtered_list(TWordIndexList *awordlist, UnicodeString afilterstr, TWordFilterSelection *afilter);
    void set_filter(UnicodeString afilterstr, TWordFilterSelection *afilter);

    __property UnicodeString meaningnotes = { read = get_mnotes };
    __property TWordIndexList *wordlist = { write = set_wordlist };

    __property int order[unsigned int ix] = { read = get_order }; // Returns the index of the ixth visible word in wordlist. The order is same as ix when there is no filtering.
};

// Word list with callback function for getting word index for each line.
typedef int(__closure *TVirtualIndexProc)(unsigned int ix);
class TVirtualWordListObj : public TWordListObjBase
{
private:
    typedef TWordListObjBase inherited;

    TVirtualIndexProc findex;

    TWordCollection *coll;

    virtual TWordCollection* get_collection()
    {
        return coll;
    }
    virtual void ResetDefaults();
    virtual void update_count(int &count);

protected:
    virtual TWordIndex get_item(unsigned int i);
public:
    int Tag;

    TVirtualWordListObj(TZPaintBox *apaintbox, int arowsize, TFuncPtr afunc, TPopupWordProc aselfunc);
    virtual ~TVirtualWordListObj();

    // Must be called first after construction but before the list is displayed.
    void set_cols(TWordCollection *acoll, int acolcnt, int *acolwidths, int *acoldefaultwidths, bool *acolvisible, TWordColumnType *acoltypes, TAlignment *acolalign, int acount, TVirtualIndexProc aindex);

    // Update virtual count.
    void set_virtual(TWordCollection *acoll, unsigned int newcount, TVirtualIndexProc aindex);
};

// Word list with several callback functions for getting word parts for each virtual line.
enum TWordNumProcWanted : byte { wnpwFreq, wnpwMeaningCount };
enum TWordStringProcWanted : byte { wspwKanji, wspwKana, wspwMeaning };
typedef word (__closure *TCallbackWordNumProc)(unsigned int ix, TWordNumProcWanted wanted);
typedef UnicodeString (__closure *TCallbackWordStringProc)(unsigned int ix, byte m, TWordStringProcWanted wanted);
typedef void (__closure *TCallbackWordTagProc)(unsigned int i, byte m, int &types, int &notes, byte &fields, word &nametag);
class TCallbackWordListObj : public TWordListObjBase
{
private:
    typedef TWordListObjBase    inherited;

    TCallbackWordNumProc fnum;
    TCallbackWordStringProc fstr;
    TCallbackWordTagProc ftag;
protected:
    // Dummy functions for abstract base class.
    virtual void ResetDefaults();
    virtual TWordCollection* get_collection();
    virtual void TCallbackWordListObj::update_count(int &count);

    virtual TWordIndex get_item(unsigned int i);
    virtual word itemfreq(unsigned int i);
    virtual UnicodeString itemkanji(unsigned int i);
    virtual UnicodeString itemkana(unsigned int i);
    virtual byte itemmeaningcount(unsigned int i);
    virtual UnicodeString itemmeaning(unsigned int i, byte m);
    virtual void itemtags(unsigned int i, byte m, int &types, int &notes, byte &fields, word &nametag);
public:
    TCallbackWordListObj(TZPaintBox *apaintbox, int arowsize, TFuncPtr afunc, TPopupWordProc aselfunc);
    virtual ~TCallbackWordListObj();

    __property TCallbackWordNumProc NumProc = { read = fnum, write = fnum };
    __property TCallbackWordStringProc StringProc = { read = fstr, write = fstr };
    __property TCallbackWordTagProc WordTagProc = { read = ftag, write = ftag };
};

#endif
