//---------------------------------------------------------------------------

#ifndef lineobjH
#define lineobjH
//---------------------------------------------------------------------------

#include "ZPaintBox.h"

class TDrawScroll;
class TWordCollection;
class TfWordBox;
struct TExampleData;
//struct TWordExampleData;
struct TStoredWord;
struct TExampleWordData;
class TZMenuRows;
struct TKanji;

enum TLineMode { lmJapanese, lmTranslated, lmBoth };

class TLineDrawObj
{
private:

    struct TLineObjRect
    {
        TRect area;
        char index; //-1 for parts not in the data
        byte pos;
        byte len;
    }
    *rects;

    int fselstart;
    int fsellength;
    bool fselecting;
    TWordCollection* fcoll;

    byte count;
    char factive;
    bool fhover;
    char fhighlighted;
    bool freverse;

    TBmp *bmp;

    TRect get_area(byte ix);
    char get_index(byte ix);
    byte get_pos(byte ix);
    byte get_len(byte ix);
public:
    TLineDrawObj(int aheight);
    virtual ~TLineDrawObj();

    void Resize(int NewWidth);
    void TextOut(TFont *font, wchar_t *text, int x, int y, int top, int height);
    void Recount(TFont *font, wchar_t *text, int x, int y, TExampleWordData *data, byte datalen);
    void DrawText(TFont *font, wchar_t *text, int top, int height);
    void SelectByIndex(int ix);
    char Highlight(int x, int y);

    __property TBmp *Bitmap = { read = bmp };
    __property bool Hovering = { read = fhover, write = fhover };
    __property char Highlighted = { read = fhighlighted };
    __property TRect Areas[byte ix] = { read = get_area };
    __property char Indexes[byte ix] = { read = get_index };
    __property byte Positions[byte ix] = { read = get_pos };
    __property byte Lengths[byte ix] = { read = get_len };
    __property bool GradReverse = { read = freverse, write = freverse };
    __property bool Selecting = { read = fselecting, write = fselecting };
    __property int SelStart = { read = fselstart, write = fselstart };
    __property int SelLength = { read = fsellength, write = fsellength };
    __property TWordCollection *Collection = { write = fcoll };
    __property byte Count = { read = count };
};

class TLineObj
{
private:
    TfWordBox *fbox;

    TFuncPtr fcallback;
    TLineDrawObj *dobj;

    TLineMode fmode;
    TWordCollection* fcoll;
    TZPaintBox *pb;
    TDrawScroll *scroller;
    bool fsimple;
    bool mouseinbox;
    bool flocked;
    int fwordindex;

    bool fselecting;
    int fselstart;
    int fsellength;

    TStoredWord *fdata;
    word fdatapos;

    UnicodeString poptranslate;
    UnicodeString poptext;
    TZMenuRows *menurows;

    UnicodeString poptext2;
    TKanji *popkanji;
    TZMenuRows *sentencerows;
    TZMenuRows *kanjirows;

    TNotifyEvent fonformdeactivate;
    TNotifyEvent fonpaint;
    TNotifyEvent fonmouseenter;
    TNotifyEvent fonmouseleave;
    TMouseMoveEvent fonmousemove;
    TMouseEvent fonmousedown;
    TMouseEvent fonmouseup;
    void __fastcall formDeactivate(TObject *Sender);
    void __fastcall pbPaint(TObject *Sender);
    void __fastcall pbMouseEnter(TObject *Sender);
    void __fastcall pbMouseLeave(TObject *Sender);
    void __fastcall pbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y);
    void __fastcall pbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y);
    void __fastcall pbMouseMove(TObject *Sender, TShiftState Shift, int x, int y);
    void settingsChanged(void *caller, TSettingsProcState state);

    void popupcallback(TZMenuRows *rows, int col, int row);
    void scallback();
    void SelectPopup(TPoint p, UnicodeString str);
    void SentencePopup(TPoint p);
    void KanjiPopup(TPoint p, wchar_t k);

    void BoxMouseLeave();
    void BoxMouseSelect(byte ix);

    int word_position(int x, int y, bool limit, bool selection = true);

    void set_datapos(word val);
    void set_mode(TLineMode val);

    void deactivated(void *caller);
    void recount(void *caller);
    void refresh();

    void HideBox(bool force = false);
    void ShowBox();

    char get_highlighted();

    int get_count();

    friend TfWordBox;
public:
    TLineObj(TZPaintBox *apb, bool asimple, TFuncPtr acallback);
    virtual ~TLineObj();

    void set_data(TWordCollection *acollection, TStoredWord *data);
    void set_all(TWordCollection *acollection, TStoredWord *data, word position);

    void Lock(bool setlock);

    __property word Pos = { read = fdatapos, write = set_datapos };
    __property word Count = { read = get_count };
    __property TLineMode Mode = { read = fmode, write = set_mode };
    __property int WordIndex = { read = fwordindex };
    __property TWordCollection *Collection = { read = fcoll };

    __property TLineDrawObj *DrawObj = { read = dobj };
};

#endif
