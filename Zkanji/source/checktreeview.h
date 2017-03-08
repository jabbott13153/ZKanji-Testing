//---------------------------------------------------------------------------

#ifndef checktreeviewH
#define checktreeviewH
//---------------------------------------------------------------------------

class TZPaintBox;
class TCheckboxTreeView;

typedef void (__closure * TCheckboxTreeViewCheckProc)(bool checked, const void *customdata);
struct TCheckTreeCheckBox
{
    bool checked;
    bool hovered;
    bool down;
};
struct TCheckTreeButton
{
    bool hovered;
    bool down;
};

//enum TCheckElemExtensionType { etSimple, etPlusMinus }; // one for each derived TCheckTreeElem template class
class TCheckTreeElem
{
private:
    TCheckboxTreeView *fowner;

//TCheckElemExtensionType ftype; // type of extension data

    const void *fdata; //custom user data

    bool fusebox; //this item has a checkbox
    TCheckTreeCheckBox fbox;

    UnicodeString ftitle;
    TTreeNode *fnode;

    TCustomTreeView *get_tree();
    bool get_checked();
    void set_checked(bool val);

    TCheckTreeElem* get_parent();
protected:
    void updateparents(bool check);
    void updatenodechilds();

    bool has_checked();

    virtual void drawcheckbox();
    virtual TRect checkboxrect();
    virtual void Draw();
    virtual void MouseMove(TShiftState Shift, int X, int Y);
    virtual void MouseDown(TMouseButton Button, TShiftState Shift, int X, int Y);
    virtual void MouseUp(TMouseButton Button, TShiftState Shift, int X, int Y);
    virtual void ReleaseButton();
    virtual void MouseLeave();

    TCheckTreeElem(/*TCheckElemExtensionType atype,*/ const wchar_t *atitle, const bool usebox, const void *adata);

    void SetChecked(bool val, bool notify);
    friend TCheckboxTreeView;
public:
    virtual ~TCheckTreeElem()
    {
    };

    void CheckAll(bool check);
//virtual void WidthChanged(int oldwidth);

    __property TCheckboxTreeView *Owner = { read = fowner };
    __property TCheckTreeElem *Parent = { read = get_parent };
    __property TCustomTreeView *Tree = { read = get_tree };
//__property TCheckElemExtensionType Type = { read=ftype };
    __property bool HasCheckbox = { read = fusebox };
    __property bool Checked = { read = get_checked, write = set_checked };
    __property bool HasChecked = { read = has_checked };
    __property const void *Data = { read = fdata };
    __property UnicodeString Title = { read = ftitle };
    __property TTreeNode *Node = { read = fnode };
};

class TSimpleCheckTreeElem : public TCheckTreeElem
{
private:
    typedef TCheckTreeElem inherited;
public:
    TSimpleCheckTreeElem(/*TCheckboxTreeView *aowner,/ TTreeNode *anode,*/ const wchar_t *atitle, const bool usebox, const void *adata);
    virtual ~TSimpleCheckTreeElem()
    {
        ;
    }
};

class TCheckboxTreeView
{
private:
    friend TCheckTreeElem;

    TCheckTreeElem *lastelem;
    bool fmousedown;
    typedef TTemplateList<TCheckTreeElem, true> TTreeElemList;
    TTreeElemList *listitems;

    TTreeView *tree;

    UnicodeString searchstr;

    int update_counter;

    // Own events
    TCheckboxTreeViewCheckProc foncheck;

    // Borrowed events from the TTreeView
    TTVAdvancedCustomDrawItemEvent onAdvancedCustomDrawItem;
    TMouseMoveEvent onMouseMove;
    TMouseEvent onMouseDown;
    TMouseEvent onMouseUp;
    TNotifyEvent onMouseLeave;
    TKeyPressEvent onKeyPress;
    TKeyEvent onKeyDown;
    TNotifyEvent onExit;

    void __fastcall AdvancedCustomDrawItem(TCustomTreeView *Sender, TTreeNode *Node, TCustomDrawState State, TCustomDrawStage Stage, bool &PaintImages, bool &DefaultDraw);
    void __fastcall MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall MouseLeave(TObject *Sender);
    void __fastcall KeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall Exit(TObject *Sender);

    void Deactivate(void *caller);

    bool findnext(TCheckTreeElem *elem, wchar_t Key);

    int get_count(); //number of all items
    TTreeNode* get_current();

    const void* get_data(int ix);
    bool get_checked(int ix);
    void set_checked(int ix, bool val);
    bool has_checked();
public:
    TCheckboxTreeView(TTreeView *tv);
    ~TCheckboxTreeView();

    void UpdateNodes(); //after the treeview is recreated we have to update the node values saved in the checktree elements.

    void AddChild(TCheckTreeElem *parent, TCheckTreeElem *child);

    void Clear();

    void OpenChecked(TTreeNode *root = NULL);
    void BeginUpdate();
    void EndUpdate();

    __property int Count = { read = get_count };
    __property const void *Data[int index] = { read = get_data };
    __property bool Checked[int index] = { read = get_checked, write = set_checked };
    __property bool HasChecked = { read = has_checked };
    __property TCheckboxTreeViewCheckProc OnCheck = { read = foncheck, write = foncheck };
};

//-----------------------------------------------------------------------
//TZTreeView class:
//-----------------------------------------------------------------------
class TZTreeView;
class TZTreeNode;

// Event function pointers

typedef void __fastcall(__closure * TZTreeViewCheck)(TObject * Sender, TZTreeNode * node, bool updating);

struct TTreeCheckBox
{
    bool checked;
    bool hovered;
    bool down;
};

class TZTreeNode
{
private:
    TZTreeView *tree;

    bool hovered;

    bool expanded;
    bool glyphhovered;

    TZTreeNode *parent;
    TZTreeNode *firstchild;
    TZTreeNode *prev;
    TZTreeNode *next;

    UnicodeString text;
    void *data;

    bool showbox;
    TTreeCheckBox box;

    int drawy; //the current y coordinate of this node when drawn

    TZTreeNode* get_last();
    void add_after(TZTreeNode* node);
    void add_child(TZTreeNode *node);
    void clear();

    int get_level();

    bool has_checked();
    bool has_unchecked();

    bool get_checked();
    void set_checked(bool val);

    void set_expanded(bool val);

    bool get_vis();
    bool get_hovered();

    TRect displayrect(bool textonly);
    TRect checkboxrect();
    TRect glyphrect();
    void drawbkg(bool selected);
    void drawglyph();
    void drawcheckbox();

    virtual void MouseMove(TShiftState Shift, int X, int Y);
    virtual void MouseDown(TMouseButton Button, TShiftState Shift, int X, int Y);
    virtual void MouseUp(TMouseButton Button, TShiftState Shift, int X, int Y);
    virtual void ReleaseButton();
    virtual void MouseLeave();

    TZTreeNode* get_prevvis();
    TZTreeNode* get_nextvis();

    TZTreeNode* get_absnext();

    void updateexpanded(bool val, bool notify);
    void updatechecked(bool val, bool notify);
    friend TZTreeView;
protected:
    virtual void Draw(int y, bool selected);
public:
    TZTreeNode(UnicodeString atext, bool checkbox);
    TZTreeNode(UnicodeString atext, bool checkbox, void *adata);

    virtual ~TZTreeNode();

    int ComparePosition(TZTreeNode *node); //return -1 if this node is above the other, 1 if below, 0 for match

    __property bool HasCheckbox = { read = showbox };

    __property TZTreeNode* Parent = { read = parent }; //node this one belongs to
    __property TZTreeNode* FirstChild = { read = firstchild }; //first child node under this one if it has children. otherwise NULL
    __property TZTreeNode* Previous =     { read = prev }; //previous node on same level
    __property TZTreeNode* Next = { read = next }; //next node on same level

    __property TZTreeNode* AbsoluteNext = { read = get_absnext }; //next node independent of what level it is on, not necessarily a sibling

    __property TZTreeNode* PreviousVisible = { read = get_prevvis }; //next item in the tree view, not always on the same level
    __property TZTreeNode* NextVisible = { read = get_nextvis }; //next item in the tree view, not always on the same level

    __property int Level = { read = get_level };

    __property TZTreeNode* LastOnLevel = { read = get_last };

    __property void* Data = { read = data, write = data };

    __property bool HasChecked = { read = has_checked };
    __property bool HasUnchecked = { read = has_unchecked };
    __property bool Visible = { read = get_vis }; //all parents are expanded

    __property bool Hovered = { read = get_hovered }; //only returns true if the owner's HotTrack is on

    __property bool Expanded = { read = expanded, write = set_expanded };
    __property bool Checked = { read = get_checked, write = set_checked };
};

class TZTreeThemeHandler
{
private:
    HTHEME treetheme;

    TNotifyEvent themeevent;
    void __fastcall ThemeEvent(TObject *Sender);

    int usecnt;

    void DrawThemedBG(TCanvas *Canvas, const TRect &r, bool selected, bool hovered);
    void DrawThemedGlyph(TCanvas *Canvas, const TRect &r, bool open, bool hovered);
    void GetGlyphSize(TCanvas *Canvas, TSize &s);

    void Register();
    void Unregister();

    friend TZTreeView;
    friend TZTreeNode;
public:
    TZTreeThemeHandler();
};

class TZTreeView : public TObject
{
private:
    static TZTreeThemeHandler handler;

    TZPaintBox *pb;
    TNotifyEvent fonpaint;
    TNotifyEvent fonresize;
    TGetScrollOverflowEvent fongetoverflow;

    TMouseMoveEvent fonmousemove;
    TMouseEvent fonmousedown;
    TMouseEvent fonmouseup;
    TNotifyEvent fonmouseleave;
    TNotifyEvent foncapturechanged;
    TKeyPressEvent fonkeypress;
    TKeyEvent fonkeydown;
    TNotifyEvent fonexit;
    TScrollEvent fonvscroll;
    TScrollEvent fonhscroll;

  //own events:
    TZTreeViewCheck foncheck; //single item was checked
    TNotifyEvent fonmulticheck; //many items were checked between BeginUpdate EndUpdate
    TNotifyEvent fonselchange; //new items are selected or a single item clicked
  //------------

    TZTreeNode *lastnode; //node under mouse or with button pressed
    UnicodeString searchstr; //string typed

    bool fhottrack;
    bool fmulti;

    TZTreeNode *firstsel;
    TZTreeNode *lastsel;
    bool backsel; //lastsel is above firstsel

    int fsize; //height of a single row
    int flines; //number of visible lines depending on collapsed/expanded state of nodes

    int fmargin; //left margin for root elements in pixels
    int fident; //horizontal identation between node levels in pixels

    TZTreeNode *root;

    TZTreeNode *topnode; //node last drawn at the top of the treeview

    bool has_checked();
    bool has_unchecked();

    bool has_sel();
    bool more_sel();

    TZTreeNode* get_selected(int ix);

    int updating;
    bool fshowbox;
    void set_showbox(bool val);

    void set_multi(bool val);

    bool fchange; //set to true on any item change. When false no data update necessary (ie. to count number of visible lines)
    void update(bool changed);

    bool fcheckchange; //set to true when item was checked during an update
    void checkupdate(bool checked);

    void countlines();
    void measure_sizes();
    void updateselection();

    void InvalidateSelected();

    TCanvas* get_canvas();
    void __fastcall pbPaint(TObject *Sender);
    void __fastcall pbResize(TObject *Sender);
    void __fastcall pbGetOverflow(TObject *Sender, int clientwidth, int clientheight, TZScrollOverflow &data);

    void __fastcall pbMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall pbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pbMouseLeave(TObject *Sender);
    void __fastcall pbCaptureChanged(TObject *Sender);
    void __fastcall pbKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall pbKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbExit(TObject *Sender);
    void __fastcall pbVScroll(TObject *Sender, TScrollCode ScrollCode, int &ScrollPos);
    void __fastcall pbHScroll(TObject *Sender, TScrollCode ScrollCode, int &ScrollPos);

    int lastscrollpos;

    bool findnext(TZTreeNode *node, wchar_t Key);

    friend TZTreeNode;
public:
    TZTreeView(TZPaintBox *abox);
    virtual __fastcall ~TZTreeView();

    void BeginUpdate();
    void EndUpdate();
    void StopUpdate(); //sets updating to 0

    void Clear();

    void AddNode(TZTreeNode *parent, TZTreeNode *child);

    TZTreeNode* LastOnLevel(TZTreeNode *levelnode);
    TZTreeNode* GetNodeAt(int y);
    TZTreeNode* FirstVisibleNode();

    int ComparePosition(TZTreeNode *anode, TZTreeNode *bnode);

    void Select(TZTreeNode *node, bool multiend); //multiend specifies that this should be one end of the selection because the shift key was pressed
    bool IsSelected(TZTreeNode *node);

    bool InView(TZTreeNode *node);
    void ScrollInView(TZTreeNode *node);

    __property TZTreeNode* RootNode = { read = root };

    __property bool HasChecked = { read = has_checked };
    __property bool HasUnchecked = { read = has_unchecked };
    __property bool ShowBoxes = { read = fshowbox, write = set_showbox };
    __property TCanvas *Canvas = { read = get_canvas };

    __property bool HotTrack = { read = fhottrack, write = fhottrack };
    __property bool MultiSelect = { read = fmulti, write = set_multi };

    __property TZTreeNode *FirstSelected = { read = firstsel }; //one end of the current selection. not necessarily the top
    __property TZTreeNode *LastSelected = { read = lastsel }; //other end of the current selection
    __property bool FirstIsLast = { read = backsel }; //true when FirstSelected is the bottom one

    __property bool HasSelected = { read = has_sel };
    __property bool MoreSelected = { read = more_sel };

    __property TZTreeViewCheck OnCheck = { read = foncheck, write = foncheck };
    __property TNotifyEvent OnMultiCheck = { read = fonmulticheck, write = fonmulticheck };
    __property TNotifyEvent OnSelectionChange = { read = fonselchange, write = fonselchange };
};

#endif
