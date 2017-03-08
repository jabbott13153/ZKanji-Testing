//---------------------------------------------------------------------------

#ifndef zmenuH
#define zmenuH
//---------------------------------------------------------------------------
#include <Menus.hpp>
#include "templatelist.h"
//---------------------------------------------------------------------------

class TAlphaImageList;
class TZMenuRows;
class TZPopupMenu;
typedef void(__closure * TZMenuItemClickProc)(TZMenuRows * Menu, int Column, int Row);

// itEmpty, itTitle and itAltTitle are disabled by default (won't do anything on click).
enum TZMenuItemType : byte { itSimple, itBitmap, itEmpty, itCheck, itCheckAuto, itTitle, itAltTitle };

class TZMenuRow
{
private:
    struct TZItem
    {
        UnicodeString title;
        TZMenuItemType type;
        bool enabled;
        void *data;
        TZMenuItemClickProc callback;

        TZMenuRows *submenu;
    };
    TZItem *fitem;
    bool *fchecked;
    int fcolcnt;
    bool fvisible;

    UnicodeString get_title(int ix);
    void set_title(int ix, UnicodeString val);
    TZMenuItemType get_type(int ix);
    void set_type(int ix, TZMenuItemType val);
    void* get_data(int ix);
    void set_data(int ix, void* val);
    TZMenuItemClickProc get_callback(int ix);
    void set_callback(int ix, TZMenuItemClickProc val);
    bool get_check(int ix);
    void set_check(int ix, bool val);
    bool get_enabled(int ix);
    void set_enabled(int ix, bool val);
    TZMenuRows* get_subs(int ix);
    void set_subs(int ix, TZMenuRows *val);

    void CheckIndex(int ix)
    {
        if (ix < 0 || ix >= fcolcnt)
            throw L"Range check error in index!";
    }
    TZItem &get_item(int ix)
    {
        CheckIndex(ix);
        return fitem[ix];
    }
    __property TZItem &item[int ix] = { read = get_item };
public:
    TZMenuRow(int column_count);
    ~TZMenuRow();

    void Setup(wchar_t* titles[], TZMenuItemType types[], void *data[] = NULL, TZMenuItemClickProc callback[] = NULL, bool enabled[] = NULL);

    __property UnicodeString Titles[int ix] = { read = get_title, write = set_title };
    __property TZMenuItemType Types[int ix] = { read = get_type, write = set_type };
    __property void* Data[int ix] = { read = get_data, write = set_data };
    __property TZMenuItemClickProc Callbacks[int ix] = { read = get_callback, write = set_callback };
    __property bool Checked[int ix] = { read = get_check, write = set_check };
    __property bool Enabled[int ix] = { read = get_enabled, write = set_enabled };
    __property bool Visible = { read = fvisible, write = fvisible };
    __property TZMenuRows *Subrows[int ix] = { read = get_subs, write = set_subs };
};

struct TZMenuRowItem
{
    TZMenuRow* row;
    int refcount;
};

class TZMenuRows : public TTemplateList<TZMenuRowItem, true>
{
private:
    typedef TTemplateList<TZMenuRowItem, true> inherited;

    int fcolcnt;
    TIntegerList *spans;
    int ftag;

    int rowabove(int row); // Visible row index above.
    int rowbelow(int row); // Visible row index below.
    bool spanabove(int row); // Span above this row.
    bool spanbelow(int row); // Span below this row.
    bool differentabove(int row, int col); // Empty/non-empty or title/non-title difference between this cell and the cell above.
    bool differentbelow(int row, int col); // Empty/non-empty or title/non-title difference between this cell and the cell below.
    bool hasdifferentabove(int row); // Empty/non-empty or title/non-title difference between this row and the row above.
    bool hasdifferentbelow(int row); // Empty/non-empty or title/non-title difference between this row and the row below.
    bool differentleft(int row, int col); // Empty/non-empty difference between this cell and the cell left.
    bool differentright(int row, int col); // Empty/non-empty or title/non-title difference between this cell and the cell right.
    bool hasdifferentleft(int col); // Empty/non-empty difference between this row and the row left.
    bool hasdifferentright(int col); // Empty/non-empty or title/non-title difference between this row and the row right.
    int lastrow(); // Last visible row index.
    int firstrow(); // First visible row index.
    bool imageincolumn(int col); // Returns true if there is a bmp or a check to be drown in that column in a visible rows.
    bool incolumn(int col, TZMenuItemType type); // Returns true if a given type is in the column.

    virtual void FreeItem(TZMenuRowItem *item, int ix);
    TZMenuRow* get_row(int ix);

    friend TZPopupMenu;

    int Add(TZMenuRowItem *item);

    __property Items;
public:
    TZMenuRows(int column_count);
    virtual ~TZMenuRows();

    virtual void Clear();

    TZMenuRow* AddRow();
    void AddSpan(int after = -1);
    void AppendRows(TZMenuRows *rows, bool addspanbefore = true);

    __property int ColCount = { read = fcolcnt };
    __property TZMenuRow* Rows[int ix] = { read = get_row };
    __property int Tag = { read = ftag, write = ftag };
};

class TZPopupMenu
{
private:
    TAlphaImageList *checkmark;

    TPopupMenu *menu;
    TTemplateList<TZMenuRows, false> *rowlist;

    int ftag;

    TZMenuItemClickProc fcallback;

    void fill_menu(TZMenuRows *rows, TMenuItem *parent);

    void __fastcall miMeasureItemOld(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height);
    void __fastcall miDrawItemOld(TObject *Sender, TCanvas *c, TRect &r, TOwnerDrawState State);
    void __fastcall miSpanMeasureItemOld(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height);
    void __fastcall miSpanDrawItemOld(TObject *Sender, TCanvas *c, TRect &r, TOwnerDrawState State);

    void __fastcall miMeasureItem(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height);
    void __fastcall miDrawItem(TObject *Sender, TCanvas *c, TRect &r, TOwnerDrawState State);
    void __fastcall miSpanMeasureItem(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height);
    void __fastcall miSpanDrawItem(TObject *Sender, TCanvas *c, TRect &r, TOwnerDrawState State);
    void __fastcall miClick(TObject *Sender);

    //void afterpopup(TObject *Sender); // callback function for WM_EXITMENULOOP
    TZMenuRow* get_item(int ix, int subindex);
    int get_colcnt(int subindex);
    int get_rowcnt(int subindex);
    __property TZMenuRow* Items[int ix][int subindex] = { read = get_item };
    __property int ColCount[int subindex] = { read = get_colcnt };
    __property int RowCount[int subindex] = { read = get_rowcnt };
public:
    TZPopupMenu(/*, bool delete_after_popup = false*/);
    ~TZPopupMenu();

    void Popup(TZMenuRows *rows, int x, int y, TZMenuItemClickProc callback_override = NULL);
    __property int Tag = { read = ftag, write = ftag };
};

extern TZPopupMenu *menu;

#endif
