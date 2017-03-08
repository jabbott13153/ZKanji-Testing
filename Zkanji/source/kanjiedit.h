//---------------------------------------------------------------------------

#ifndef kanjieditH
#define kanjieditH
//---------------------------------------------------------------------------
#include <Buttons.hpp>

class TZPaintBox;
class TDropDownList;
enum TDropListButtonState { dbsNormal, dbsHover, dbsPressed };
class TfRecognize;
enum TRecognizePosition { rpDefault, rpAbove, rpBelow }; // The non default positions force the recognition window to be repositioned.

class TKanjiEdit
{
private:
    TZPaintBox *pb;
    TTimer *tBlink;
    bool fkanji; // Can paste kanji into the box.
    bool fgeneral; // The kanji edit box should accept full width romaji, brackets etc.
    int charwidth;
    int btnwidth;
    TDropListButtonState bstate;

    bool mousedown; // True when a button is pressed over the paint box.

    bool frecognizer;
    bool frecognize_autohide;
    bool frecognize_showhint;
    bool frecognize_allowerror;
    bool frecognize_generalchar;
    bool frecognize_cntlimit;
    bool frecognize_showpopup;
    TRecognizePosition frecognize_position;

    TSpeedButton *frecognizebutton;

    TRect textarea;

    byte fcollection;

    TNotifyEvent foldenter;
    TNotifyEvent foldexit;
    TNotifyEvent foldresize;
    TNotifyEvent foldmouseleave;
    TMouseEvent foldmousedown;
    TMouseEvent foldmouseup;
    TMouseMoveEvent foldmousemove;
    TKeyEvent foldkeydown;
    TKeyPressEvent foldkeypress;
    TNotifyEvent foldpaint;

    TNotifyEvent foldbuttonclick;
    void __fastcall OnRecognizeClick(TObject *Sender);

    TFuncPtr fcallback;

    TDropDownList *fdropdown;
    int fdroppos;

    int xpos; // x position to draw at.

    int fpos; // Where to insert next kanji.
    int fselstart;
    int fsellength;
    bool fselecting;
    word fmax;

    UnicodeString ftext;
    UnicodeString ptext;

    HIMC defaultcontext;
//HIMC context;
//WNDPROC oldproc;

    void CountPos(); // Count xpos (when caret not on screen etc.)
    void __fastcall DrawCaret(TObject *Sender);
    void DrawNC(HDC hdc, TRect r, bool frame);
    void DrawDropButton();
    void FocusKanjiEd(); // Focus editbox and draw caret when needed.
    void DeleteHalf(); // Delete not fully typed kana.
    void DeleteSelected(); // Delete selected text and place caret in its position.

    void dropButtonClick(int index, UnicodeString w, void *data);

    void set_text(UnicodeString str);
    void set_pos(int i);
    bool get_visible();
    void set_max(unsigned short val);

    bool get_rec();

    UnicodeString get_text();
    UnicodeString get_seltext();
    UnicodeString get_cleantext();
    int get_selstart();
    int get_left();
    int get_top();
    TRect get_sbounds();
    int get_width();
    int get_height();

    void Activate(void *caller); // Form events.
    void Deactivate(void *caller);
    void formHide(void *caller);

    HWND get_handle();

    void __fastcall pbEnter(TObject *Sender);
    void __fastcall pbExit(TObject *Sender);
    void __fastcall pbPaint(TObject *Sender);
    void __fastcall pbNCPaint(TObject *Sender, HDC hdc, TRect r);
    void __fastcall pbResize(TObject *Sender);
    void __fastcall pbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y);
    void __fastcall pbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y);
    void __fastcall pbMouseMove(TObject *Sender, TShiftState Shift, int x, int y);
    void __fastcall pbMouseLeave(TObject *Sender);

    __property int pos = { write = set_pos, read = fpos };
    __property bool recognizing = { read = get_rec };

    void appdeactivate(void *caller);

//friend LRESULT CALLBACK KanjiEditWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
    TKanjiEdit(TZPaintBox *apb, bool acceptkanji, bool acceptgeneral, TDropDownList *adropdown, TFuncPtr acallback);
    ~TKanjiEdit();

    void ShowRecognize();
    void StopRecognize(); // Don't call stoprecognize directly, call fRecognize->Hide() instead.

    void RestoreWndProc();
    void SetWndProc();

    bool Focused();
    void SetFocus();

    void __fastcall pbKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbKeyPress(TObject *Sender, wchar_t &Key);

    void Insert(UnicodeString c);

    void Paste();
    void Copy();
    void SelectAll();
    void DeselectAll();
    void Delete(); // Delete selected text.

    void ToHistory();

    bool CanInput(); // There is no ptext that needs finishing.

    void EnableRecognizer(TSpeedButton *arecognizebutton, bool aautohide = false, bool ashowhint = true, bool aallowerror = true, int arecognizecntlimit = -1, bool arecognizegeneral = true, bool arecognizeshowpopup = true, TRecognizePosition arecognizeposition = rpDefault);
    void DisableRecognizer();
    void UpdateRecognizer();
    void PlaceRecognize(TRecognizePosition arecognizeposition);

    bool HandleAllocated();

    __property UnicodeString Text = { read = get_text, write = set_text };
    __property UnicodeString CleanText = { read = get_cleantext }; // Returns text with only Japanese characters, unconverted and not fully typed syllables removed.
    __property bool Visible = { read = get_visible };
    __property unsigned short MaxLength = { read = fmax, write = set_max };
    __property int SelLength = { read = fsellength };
    __property int SelStart = { read = get_selstart };
    __property UnicodeString SelText = { read = get_seltext };
    __property int Left = { read = get_left };
    __property int Top = { read = get_top };
    __property int Width = { read = get_width };
    __property int Height = { read = get_height };
    __property TRect ScreenBounds = { read = get_sbounds }; // Bounding rectangle in screen coordinates.
    __property bool AcceptKanji = { read = fkanji, write = fkanji };
    __property bool AcceptGeneral = { read = fgeneral, write = fgeneral };
    __property HWND Handle = { read = get_handle };

    __property byte Collection = { read = fcollection, write = fcollection };
};

class TKanjiEditList : public TTemplateList<TKanjiEdit, false>
{
private:
    typedef TTemplateList<TKanjiEdit, false> inherited;

    UnicodeString keditfont;
    TFontCharset keditcharset;

    friend class TKanjiEdit;
public:
    TKanjiEditList();
    ~TKanjiEditList();
};

extern TDropDownList *romajihistory;
extern TDropDownList *kanjihistory;
extern TKanjiEditList *kanjieditlist;

#endif
