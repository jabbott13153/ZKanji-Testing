//---------------------------------------------------------------------------

#ifndef wordgridH
#define wordgridH
//---------------------------------------------------------------------------

#include "customlist.h"
#include "listcontrols.h"

class TAlphaImageList;

typedef void(__closure * TWordListColumnPressProc)(int oldcol);
typedef void(__closure * TWordListColumnChangeProc)(int oldcol, int col);
typedef void(__closure * TWordListBasePrepareDrawHeaderProc)(int col, TCanvas * c);

enum THeaderButtonDirection { hbdNone, hbdDown, hbdUp };

struct TSequenceItem
{
    int pos;
    int len;
};
class TSequenceList
{
private:
    TTemplateList<TSequenceItem, true> list;

    int find_seq(int ix); // Index of the sequence containing ix, or the sequence coming after it.

    bool get_item(int ix);
    void set_item(int ix, bool val);

    void set_item2(int seq, int ix, bool val);

    int get_seqcnt();
    TSequenceItem* get_seq(int ix);
public:
    void SetSequence(int start, int count, bool set);
    void Clear();
    void Limit(int limit);
    bool Toggle(int index); // Sets or unsets a single item depending on its current state. Returns whether the item is toggled as a result.
    int Next(int pos); // Index of the first selected item from the given position, including the position itself.
    int Prev(int pos); // Index of the last selected item from the given position, including the position itself.
    int SequenceLength(int seq); // Length of a sequence containing the 'seq' index.
    bool Single(); // True if the sequence consists of a single sequence with a length of 1.

    __property bool Items[int] = { read = get_item, write = set_item };
    __property int SequenceCount = { read = get_seqcnt };
    __property TSequenceItem* Sequences[int] = { read = get_seq };
};

enum TWordListSelectionType : byte {
    wlsNoSelection, // No selection of rows. Position maintained by scrollbar.
    wlsSingleSelect, // Single row can be selected at a time.
    wlsMultiSelect // Multiple rows can be selected by clicking them holding the Shift or Control keys.
};
enum TWordListMultiSelection : byte {
    wlmsContinuous, // Only continuous rows can be selected at a time holding the Shift key.
    wlmsRandom, // Rows can be selected randomly holding the Control key.
    wlmsContinuousRandom // Rows can be selected continuously holding the Shift key or randomly holding the Control key.
};
class TWordListBase : public TCustomListObj
{
private:
    typedef TCustomListObj inherited;

    TFuncPtr ffunc; // Callback function for when anything changes, eg. position.

    bool *fresize;
    int *fdefsize;

    bool fvertline;
    bool fhorzline;
    bool fdarkensel;

    bool fallownopos;

    bool fresizing;
    int fsizedcolumn;
    int fsizing;

    TWordListColumnPressProc fheaderpressproc;
    TWordListColumnChangeProc fheaderselectproc;
    TWordListBasePrepareDrawHeaderProc fheadprepare;

    TColor fheaderbuttoncolor;
    bool fheaderbutton;
    bool fheaderbuttonhover;
    int fheaderbuttoncolumn;
    THeaderButtonDirection fheaderbuttoncolumnorder; // 0 not ordered, 1 ordered down (arrow on top), 3 ordered up (arrow on bottom).
    bool fheaderbuttondown;
    int fcolselected;
    TAlphaImageList *arrowimg;
    bool *fcolhover;

    bool fordered;

    TWordListSelectionType fseltype;
    TWordListMultiSelection fmulsel;
    bool fmultirightclick;
    int fpos;
    int fsellength;
    TSequenceList fselection;

    int get_colsize(byte col);
    void set_colsize(byte col, int val);
    int get_defsize(byte col);
    void set_defsize(byte col, int val);
    bool get_colresize(byte col);
    void set_colresize(byte col, bool val);
    void set_colresizing(bool val);
    bool get_colhover(byte col);
    void set_colhover(byte col, bool val);

    void set_headerbutton(bool val);
    void set_headerbuttoncolor(TColor color);
    void set_ordered(bool val);
    //void set_colselected(int col);
    bool get_colreverse();
    //void set_colreverse(bool val);
    void generate_arrows();

    int get_rowsize();
    void set_rowsize(int val);
    int get_headersize();
    void set_headersize(int val);

    void set_vertline(bool val);
    void set_horzline(bool val);
    void set_darkensel(bool val);

    //void set_rowselect(bool val);
    //void set_multiselect(bool val);
    void set_seltype(TWordListSelectionType seltype);
    void set_mulsel(TWordListMultiSelection mulsel);
    int get_selstart();
    int get_sellength();
    bool get_selected(int row);
    void set_selected(int row, bool sel);
    int get_pos();
    void set_pos(int val);

    void set_allownopos(bool val);

    void set_headprepproc(TWordListBasePrepareDrawHeaderProc val);

    bool get_multiplesel(); // Returns true if more than one row is selected.
    int get_seqcnt();
    const TSequenceItem& get_selseq(int ix);

    virtual void DoPrepareDrawHeader(int col);
    virtual void DoDrawHeader(int col, TRect r);
    virtual void DoDraw(int col, int row, TRect r);
protected:
    void set_pos2(int val, bool resetsel);

    void headrect(int col, TRect &r);

    using inherited::set_cols;
    void set_cols(int acolcnt, int *acolwidths, int *acoldefaultwidths, bool *acolvisible, int acount);

    void invalidateheadercell(int col);

    virtual void DoCallback();
    virtual void DoPrepareDraw(int col, int row);
    virtual void DoMouseMove(int acol, int arow, TShiftState shift, int x, int y);
    virtual void DoMouseDown(int acol, int arow, TMouseButton button, TShiftState shift, int x, int y);
    virtual void DoMouseLeave();
    virtual void DoMouseUp(int acol, int arow, TMouseButton button, TShiftState shift, int x, int y);
    virtual void DoKeyDown(WORD &Key, TShiftState Shift);
    virtual void DoKeyPress(wchar_t &Key);
    virtual void DoSetColumnCount(const byte &acolcount, const byte &oldcolcount);

    virtual void DoDrawHead(int col, TRect r) = 0; // Separate header drawing from cell drawing, because the inner rectangle dimensions are different.
    virtual void DoDrawCell(int col, int row, TRect r) = 0;
    virtual void DoActivate(bool focused);
    virtual void DoDeactivate(bool focused);

    virtual void DoSetCount(int &acount, int &oldcount);

    virtual void DoColResizing(bool newval);

    virtual void update_count(int &count) = 0;
public:
    TWordListBase(TZPaintBox *abox, int arowheight, bool aheader, TFuncPtr afunc);
    virtual ~TWordListBase();

    // Select row(s) while updating the list on screen.
    virtual void Update(int aselstart, int asellength);
    virtual void Update(int row);

    void invalidateselected();
    TRect selectionbox();

    void SelectOrderColumn(int col, bool reverse);
    void ToggleSelected(int selstart, int sellength, bool selected); // Only valid for wlsMultiSelectRandom
    void SelectAll();
    void DeselectAll();

    // Draw the default header with a given string.
    void DrawHeadGen(int col, TRect r, UnicodeString str, TAlignment alignment = taLeftJustify);
    static void DrawHeadNormal(TCanvas *c, int col, TRect r, UnicodeString str, TAlignment alignment = taLeftJustify); // Called from other objects to paint a rectangle in the style of a word grid header.

    __property TWordListColumnPressProc headerpressproc = { read = fheaderpressproc, write = fheaderpressproc };
    __property TWordListColumnChangeProc headerselectproc = { read = fheaderselectproc, write = fheaderselectproc };
    __property TWordListBasePrepareDrawHeaderProc prepareheaderproc = { read = fheadprepare, write = set_headprepproc };

    __property int colsize[byte] = { read = get_colsize, write = set_colsize };
    __property int defsize[byte] = { read = get_defsize, write = set_defsize };
    __property bool colresize[byte] = { read = get_colresize, write = set_colresize };
    __property bool colresizing = { read = fresizing, write = set_colresizing };
    __property int colselected = { read = fcolselected/*, write=set_colselected*/ };
    __property bool colreverseorder = { read = get_colreverse/*, write=set_colreverse*/ };
    __property bool colhover[byte] = { read = get_colhover, write = set_colhover };

    __property TColor headerbuttoncolor = { read = fheaderbuttoncolor, write = set_headerbuttoncolor };

    __property bool horzline = { read = fhorzline, write = set_horzline };
    __property bool vertline = { read = fvertline, write = set_vertline };
    __property bool darkenselectedline = { read = fdarkensel, write = set_darkensel };

    //__property bool rowselect = { read = frowselect, write = set_rowselect };
    //__property bool multiselect = { read = fmultiselect, write = set_multiselect };
    __property TWordListSelectionType selecttype = { read = fseltype, write = set_seltype };
    __property TWordListMultiSelection multiselection = { read = fmulsel, write = set_mulsel };
    __property bool multirightclick = { read = fmultirightclick, write = fmultirightclick };
    __property bool multipleselected = { read = get_multiplesel };
    __property int selseqcount = { read = get_seqcnt };
    __property const TSequenceItem& selseq[int] = { read = get_selseq };

    __property int pos = { read = get_pos, write = set_pos };
    __property int selstart = { read = get_selstart };
    __property int sellength = { read = get_sellength };
    __property bool selected[int i] = { read = get_selected, write = set_selected }; // Setter only works when selection type is wlsMultiSelectRandom.
    __property bool allownopos = { read = fallownopos, write = set_allownopos };

    __property int rowsize = { read = get_rowsize, write = set_rowsize };
    __property int headersize = { read = get_headersize, write = set_headersize };

    __property bool headerbutton = { read = fheaderbutton, write = set_headerbutton };
    __property bool ordered = { read = fordered, write = set_ordered };
};

#endif
