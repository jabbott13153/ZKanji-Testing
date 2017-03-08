//---------------------------------------------------------------------------

#ifndef wordfilterH
#define wordfilterH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include "checktreeview.h"

class TDrawnPlusbutton;
class TDrawnMinusbutton;

enum TPlusMinusExtensionSelection { pmesNone, pmesPlus, pmesMinus };
class TPlusMinusCheckTreeElem : public TCheckTreeElem
{
private:
    typedef TCheckTreeElem inherited;

    TCheckTreeButton plus;
    TCheckTreeButton minus;

    static int instances;
    static TDrawnPlusbutton *drawnplus;
    static TDrawnMinusbutton *drawnminus;

    void drawbox(TPlusMinusExtensionSelection which);
    TRect buttonrect(TPlusMinusExtensionSelection which);
public:
    TPlusMinusCheckTreeElem(/*TCheckboxTreeView *aowner, TTreeNode *anode,*/ const wchar_t *atitle, const bool usebox, const void *adata);
    virtual ~TPlusMinusCheckTreeElem();

    virtual void Draw();
    virtual void MouseMove(TShiftState Shift, int X, int Y);
    virtual void MouseDown(TMouseButton Button, TShiftState Shift, int X, int Y);
    virtual void MouseUp(TMouseButton Button, TShiftState Shift, int X, int Y);
    virtual void ReleaseButton();
    virtual void MouseLeave();
};

class TCheckboxTreeView;
struct TWordFilter;
class TfFilter : public TBaseForm
{
__published: //IDE-managed Components
    TBitBtn *BitBtn1;
    TTreeView *tvBoxes;
    TBevel *bvB;
    TBitBtn *BitBtn2;
    TPanel *pControls;
    TLabel *Label4;
    TLabel *Label7;
    TBevel *bv1;
    TBevel *bv2;
    TBevel *bv3;
    TBevel *bv4;
    TLabel *Label1;
    TSpeedButton *sbUp;
    TListBox *lbFilters;
    TComboBox *cbFilters;
    TPanel *Panel1;
    TRadioButton *rbAnyType;
    TRadioButton *rbAllTypes;
    TPanel *Panel2;
    TRadioButton *rbAnyUsage;
    TRadioButton *rbAllUsages;
    TSpeedButton *sbDown;
    TSpeedButton *sbRename;
    TSpeedButton *sbDelete;
    TButton *btnAddFilter;
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall cbFiltersChange(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall cbN5Click(TObject *Sender);
    void __fastcall btnAddFilterClick(TObject *Sender);
    void __fastcall lbFiltersClick(TObject *Sender);
    void __fastcall sbDeleteClick(TObject *Sender);
    void __fastcall sbUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbRenameClick(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall cbFiltersKeyPress(TObject *Sender, wchar_t &Key);

private: //User declarations
    TCheckboxTreeView *bobj;
    void AddItems(unsigned int itemcount, const wchar_t *lines[], const bool useboxes[], const void *customdata[]);

    static const wchar_t *treewordtypes[];
    static const wchar_t *treewordusages[];
    static const wchar_t *treewordfields[];
    static const wchar_t *treewordJLPT[];
    static const bool treewordtypesboxes[];
    static const bool treewordusagesboxes[];
    static const bool treewordfieldsboxes[];
    static const bool treewordJLPTboxes[];
    static const int treewordtypesorder[];
    static const int treewordusagesorder[];
    static const int treewordfieldsorder[];
    static const int treewordJLPTorder[];

    void ccallback(bool checked, const void *customdata);

    int UniqueId();
    void UpdateButtons();
    void FilterChanged(); //current filter was changed by the user
    void CollectFilter(TWordFilter &filter);
    void ApplyFilter(const TWordFilter &filter);
public: //User declarations
    __fastcall TfFilter(TComponent* Owner);
    virtual __fastcall ~TfFilter();
};
//---------------------------------------------------------------------------
extern PACKAGE TfFilter *fFilter;
//---------------------------------------------------------------------------

//initialize filters dialog and show window
void SetFilter();
enum TWordTypeInclusion { wtiAny, wtiAll };

//memcopy used to copy contents. don't add UnicodeString
struct TWordFilter
{
    int types;
    int notes;
    byte fields; //changed to byte in Nov 2010, v0.57
    bool JLPT[6];
    TWordTypeInclusion typeinclusion;
    bool allusages;
};

bool operator == (const TWordFilter &a, const TWordFilter &b);
bool operator != (const TWordFilter &a, const TWordFilter &b);

struct TWordFilterItem
{
    UnicodeString name;
    int id;
    TWordFilter filter;
};

class TWordFilterList : public TTemplateList<TWordFilterItem, true>
{
private:
    typedef TTemplateList<TWordFilterItem, true> inherited;
    using inherited::Add;
    using inherited::IndexOf;

    int get_id(int ix);
    UnicodeString get_name(int ix);
    const TWordFilter& get_filter(int ix);

    void normalize(TWordFilter &f);
public:
    TWordFilterList();
    virtual ~TWordFilterList();

    int IndexOf(UnicodeString name);
    int IndexOf(int id);
    int Add(UnicodeString name, const TWordFilter &filter);

    __property int Id[int index] = { read = get_id };
    __property UnicodeString Names[int index] = { read = get_name };
    __property const TWordFilter& Filters[int index] = { read = get_filter };
};

struct TWordFilterCopy
{
    bool exclude;
    TWordFilter filter;
};
class TWordFilterCopyList : public TTemplateList<TWordFilterCopy, true>
{
private:
    typedef TTemplateList<TWordFilterCopy, true> inherited;
public:
    TWordFilterCopyList() : inherited()
    {
    }
    virtual ~TWordFilterCopyList()
    {
        Clear();
    }

    bool match(TWord *w, char meaning = -1);
};

//list of Ids in TWordFilterList. Negative numbers mark filters to exclude words that match.
class TWordFilterSelection : public TIntegerList
{
private:
    typedef TIntegerList inherited;

    UnicodeString get_name(int ix);
    const TWordFilter& get_filter(int ix);
    bool get_exclude(int ix);

public:
    TWordFilterSelection() : inherited()
    {
    }

    bool match(TWord *w, char meaning = -1);
    bool filters_match(TWordFilterCopyList *copy);
    void create_copy(TWordFilterCopyList *copy);

    __property UnicodeString FilterName[int index] = { read = get_name };
    __property const TWordFilter &Filters[int index] = { read = get_filter };
    __property bool Exclude[int index] = { read = get_exclude };
};

int generate_unique_filter_id();

extern TWordFilterList *wordfilters;
extern TWordFilterSelection *dictionaryfilters;
extern TWordFilterSelection *kanjiwordfilters;
extern TWordFilterSelection *wordgroupfilters;

#endif
