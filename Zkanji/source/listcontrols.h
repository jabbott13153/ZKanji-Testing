//---------------------------------------------------------------------------

#ifndef listcontrolsH
#define listcontrolsH
//---------------------------------------------------------------------------

#include "controlimages.h"

typedef void (__closure *TListCustomControlChanged)(unsigned int row, byte index, bool &checked);

class TWordListObjBase;
class TAlphaImageList;
enum TDrawnControlboxState;

class TCustomListControlObj
{
private:
    TWordListObjBase *fowner;
    TDrawnCheckbox *checkboximages;
    TDrawnRadiobox *radioboximages;

    int get_checkboxwidth();
    int get_checkboxheight();

    int get_radioboxwidth();
    int get_radioboxheight();
protected:
    void drawcheckbox(TCanvas *c, int x, int y, TDrawnControlboxState state);
    void drawcheckbox(TCanvas *c, int x, int y, TDrawnControlboxState state, TColor bk);

    void drawradiobox(TCanvas *c, int x, int y, TDrawnControlboxState state);
    void drawradiobox(TCanvas *c, int x, int y, TDrawnControlboxState state, TColor bk);

    virtual void draw(TCanvas *c, int col, int row, TRect &r) = 0;
    virtual bool mouseincontrol() = 0;
    virtual void mousedown(int acol, int arow, TMouseButton Button, TShiftState Shift, int x, int y) = 0;
    virtual void mousemove(int acol, int arow, TShiftState Shift, int x, int y) = 0;
    virtual void mouseup(int acol, int arow, TMouseButton Button, TShiftState Shift, int x, int y) = 0;
    virtual void mouseleave() = 0;
    virtual void deactivated() = 0;
    virtual void changed(int count, int lastcount) = 0;
    virtual void updatecoordinates(int acol, int arow, int &x, int &y) = 0;

    __property int checkboxwidth = { read = get_checkboxwidth };
    __property int checkboxheight = { read = get_checkboxheight };
    __property int radioboxwidth = { read = get_radioboxwidth };
    __property int radioboxheight = { read = get_radioboxheight };

    friend TWordListObjBase;
    __property TWordListObjBase *owner = { read = fowner };

    TCustomListControlObj(TWordListObjBase *aowner);
    virtual ~TCustomListControlObj();
};

// Callback function prototypes for virtual checkbox objects.
enum TListVirtualCheckControlWantedValue : byte { ccwvBoxColumn, ccwvChecked, ccwvEnabled };
typedef bool (__closure *TListVirtualCheckControlGetter)(unsigned int row, byte index, TListVirtualCheckControlWantedValue wanted); // If wanted is ccwvBoxColumn, the row will be INT_MAX. For any other wanted type, the index is the index of the nth column with checkbox. Other columns not included.
// Object of checkbox controls to given columns in descendents of TWordListObjBase class..
class TListCheckboxObj : public TCustomListControlObj
{
private:
    typedef TCustomListControlObj inherited;

    TListCustomControlChanged fcallback;
    TListVirtualCheckControlGetter gcallback;

    TGapList<bool> columns;
    bool **fchecked;
    bool **fenabled;

    bool virtuallist; // True if the checkboxes are not stored in the object, only callback functions for providing states.

    int mouserow; // Row of mouse. -1 when mouse is not on a checkbox.
    int mouseix; // Checkbox index under mouse in mouserow. -1 when mouse is not on a checkbox.
    bool mousehover;
    bool buttondown;

    bool fmulticheck;
    bool fmultichecksel;


    void mousecontrolpos(int col, int row, int x, int y, int &cix, int &crow);

    bool get_checked(unsigned int row, byte ix);
    bool get_enabled(unsigned int row, byte ix);
    void set_checked(unsigned int row, byte ix, bool val);
    void set_enabled(unsigned int row, byte ix, bool val);

    bool get_column(byte col);
    void set_column(byte col, bool val);

    friend class TWordListObjBase;
protected:
    virtual void draw(TCanvas *c, int col, int row, TRect &r);
    virtual bool mouseincontrol();
    virtual void mousedown(int acol, int arow, TMouseButton Button, TShiftState Shift, int x, int y);
    virtual void mousemove(int acol, int arow, TShiftState Shift, int x, int y);
    virtual void mouseup(int acol, int arow, TMouseButton Button, TShiftState Shift, int x, int y);
    virtual void mouseleave();
    virtual void deactivated();
    virtual void changed(int count, int lastcount); // Tell the checkbox list that the number of rows have changed.
    virtual void updatecoordinates(int acol, int arow, int &x, int &y);

    __property bool checked[unsigned int][byte] = { read = get_checked, write = set_checked };
    __property bool enabled[unsigned int][byte] = { read = get_enabled, write = set_enabled };

    __property bool checkcolumn[byte] = { read = get_column, write = set_column }; // Get and set whether the given column contains checkboxes.

    __property bool multicheckcolums = { read = fmulticheck, write = fmulticheck }; // Check all checkboxes in selected rows at the checked column.
    __property bool multicheckselected = { read = fmultichecksel, write = fmultichecksel }; // Check all checkboxes in selected rows in all columns.

    TListCheckboxObj(TWordListObjBase *aowner, TListCustomControlChanged acallback);
    TListCheckboxObj(TWordListObjBase *aowner, TListCustomControlChanged acallback, TListVirtualCheckControlGetter agcallback); // Creates a virtual listcheckbox
    virtual ~TListCheckboxObj();
};


#endif
