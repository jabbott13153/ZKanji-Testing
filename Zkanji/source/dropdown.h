//---------------------------------------------------------------------------

#ifndef dropdownH
#define dropdownH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "baseform.h"

#include <stdio.h>

typedef void(__closure * THistFunc)(int index, UnicodeString w, void *data);

class TDropDownList
{
private:
    bool funlimited;
    bool fduplicates;

    TStringList *items;

    UnicodeString get_string(int ix);
    void* get_data(int ix);
    int get_count();

    bool get_visible();
    TBaseForm* get_form();
    void* get_handle();
public:
    // unlimited - set to true when there is no limit for the number of saved items. Otherwise the specified value is used from the settings.
    TDropDownList(bool unlimited = false, bool duplicates = false);
    ~TDropDownList();

    void Popup(TRect acrect, THistFunc aevent, TBaseForm *aform, bool aautoupdate);
    void Add(UnicodeString s, void *data);
    void virtual Clear();
    void Delete(int ix);

    void Limit(); // Delete items from the end of the items list until settings.histcnt number of items remain. Does nothing for lists with unlimited items.

    void FillComboBox(TComboBox *cb);
    void SaveToFile(FILE *f);
    void LoadFromFile(FILE *f);

    void Capture();

    __property UnicodeString Strings[int] = { read = get_string };
    __property void* Data[int] = { read = get_data };
    __property int Count = { read = get_count };
    __property bool Visible = { read = get_visible };
    __property TBaseForm *Form = { read = get_form };
    __property void* Handle = { read = get_handle };
};

class TfDropDown : public TBaseForm
{
__published:
    TListBox *lb;
    void __fastcall lbMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall lbDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
    void __fastcall lbKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormDeactivate(TObject *Sender);
    void __fastcall lbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall lbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);
private:
    typedef TBaseForm   inherited;

    TDropDownList *flist;
    TBaseForm *fform;
    int fmouseon; // Mouse on the item in the listbox when mouse is inside.
    int laston; // Last item mouse was on before moving out of the listbox, can be -1 when mouse is still on the box.
    bool wason; // Mouse was on the listbox when user pressed the mouse button first.
    int fcount;
    int itemheight;
    THistFunc fonselect;
    bool fautoupdate;
    int fexx, fexy;

    bool scaled;

    //TWndMethod oldproc;

    void appdeactivate(void *caller);
    void __fastcall WndP(TMessage &msg);
    void set_mouseon(int val);

    __property int mouseon = { read = fmouseon, write = set_mouseon };
    int ItemAt(int x, int y);
protected:
    void __fastcall CreateParams(TCreateParams &Params);

    friend class TDropDownList;
public:
    __fastcall TfDropDown(TComponent* Owner);
    virtual __fastcall ~TfDropDown();

    void hide_me();
    void Popup(TRect r, TDropDownList *alist, THistFunc aevent, TBaseForm *aform, bool aautoupdate);
    void StartCapture();

    __property int count = { read = fcount };
};

extern TfDropDown *fDropDown;

//---------------------------------------------------------------------------
#endif
