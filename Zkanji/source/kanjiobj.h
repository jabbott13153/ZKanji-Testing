//---------------------------------------------------------------------------

#ifndef kanjiobjH
#define kanjiobjH
//---------------------------------------------------------------------------
#include "ZPaintBox.h"

class TWordCollection;
struct TKanji;
struct TKanjiElement;

class TGdiPlus;

enum TDrawBitDirection;

enum TDrawStrokeState { dssStarted, dssStopped, dssStroke, dssStrokeBit, dssPaused };
enum TDrawSpeed : byte { dsNormalSpeed, dsFast, dsFastest };

typedef void(__closure * TDrawStrokeStatePtr)(TDrawStrokeState state);

class TKanjiDrawObj
{
private:
    bool destroying;

    bool paused;

    HFONT Kfont;

    TGdiPlus *gdi;
    TZPaintBox *fpaintbox;
    TBmp *bmp;

    bool fgrid;
    bool forder;
    bool fshadow;
    bool fnumbers;
    bool fdots;
    TKanji *fkanji;
    char playstroke;
    byte playmaxstroke;
    short playbit;
    word playmaxbit;
    TDrawStrokeStatePtr playfunc;
    TDrawSpeed fspeed;
    short felement;
    double fpensize;
    char fmargin;
    char fbitsize;

    bool playing;
    TTimer *time;

    bool needupdate;

    TPointList *numlist;
    TPointList *dotlist;

    void set_grid(bool val);
    void set_order(bool val);
    void set_shadow(bool val);
    void set_numbers(bool val);
    void set_dots(bool val);
    void set_kanji(wchar_t val);
    void set_speed(TDrawSpeed val);
    int get_position();
    wchar_t get_kanji();
    void stroke_init();
    void create_timer();
    int timer_speed();
    void set_element(short val);
    void set_pensize(double val);
    void set_margin(char val);

    void beginpropertychange();
    void endpropertychange();

    void __fastcall onPaint(TObject *Sender);
    void __fastcall onPlay(TObject *Sender);

    void repaint();

    void draw_numbers();
    void draw_dots();
    void add_number(TDoublePoint p, TDrawBitDirection dir);
    void add_dot(TDoublePoint p);
    void draw_number(int i);
    void draw_dot(int i);
    TRect number_rect(int i);
    TRect dot_rect(int i);
public:

    TKanjiDrawObj(TZPaintBox *apaintbox, TDrawStrokeStatePtr func = NULL);
    virtual ~TKanjiDrawObj();

    void Play();
    void Stop();
    void Pause();

    void Update();

    void StrokeBack();
    void StrokeForward();
    void StrokeFastForward();
    void StrokeRewind();

    __property bool grid = { read = fgrid, write = set_grid } ;
    __property bool strokeorder = { read = forder, write = set_order } ;
    __property bool strokeshadow = { read = fshadow, write = set_shadow } ;
    __property bool strokenumbers = { read = fnumbers, write = set_numbers } ;
    __property bool strokedots = { read = fdots, write = set_dots } ;
    __property double strokewidth = { read = fpensize, write = set_pensize } ;
    __property char strokemargin = { read = fmargin, write = set_margin } ;
    __property TDrawSpeed strokespeed = { read = fspeed, write = set_speed } ;
    __property int strokeposition = { read = get_position/*, write=set_position*/ } ;
    __property TDrawStrokeStatePtr strokefunc = { read = playfunc, write = playfunc } ;
    __property wchar_t kanji = { read = get_kanji, write = set_kanji } ;
    __property short element = { read = felement, write = set_element } ;
} ;

typedef void(__closure * TKanjiPanelColorEvent)(TCanvas * c, TRect r, int index, TKanjiElement * elem, bool selected, TColor & background, TColor & fontcolor);
typedef void(__closure * TKanjiPanelDrawEvent)(TCanvas * c, TRect r, int index, TKanjiElement * elem, bool selected);

class TKanjiPanelObj
{
private:
    HFONT Kfont;

    TZPaintBox *pb;
    TBmp *bmp;
    TKanjiPanelColorEvent foncolor;
    TKanjiPanelDrawEvent fondraw;

    TWordCollection *fcoll;
    int fpos; // First visible item
    int fmpos; // Item under mouse
    TColor fbg;

    int *items;
    int cnt;

    bool fshowhint;
    bool fselpart;

    int wdelta;

    TNotifyEvent onmouseleave;
    TMouseMoveEvent onmousemove;
    TNotifyEvent onresize;

    // Paintbox events
    void __fastcall pbpaint(TObject *Sender);
    void __fastcall pbmove(TObject *Sender, TShiftState Shift, int x, int y);
    void __fastcall pbleave(TObject *Sender);
    void __fastcall pbresize(TObject *Sender);

    TFuncPtr fcallback;
    TWndMethod oldproc;
    void __fastcall boxwndproc(TMessage &Message);

    void set_pos(int val);
    void set_bg(TColor val);
    void set_selpart(bool val);

    void settingsChanged(void *caller, TSettingsProcState state);
public:
    TKanjiPanelObj(TZPaintBox *apaintbox, TFuncPtr acallback, TKanjiPanelColorEvent colorproc = NULL, TKanjiPanelDrawEvent drawproc = NULL);
    virtual ~TKanjiPanelObj();

    int ElementAtPos(int x);
    int KanjiAtPos(int x);
    wchar_t CharAtPos(int x);

    void set_items(TIntegerList *l);
    __property int pos = { read = fpos, write = set_pos } ;
    __property int count = { read = cnt } ;
    __property TColor bgcolor = { read = fbg, write = set_bg } ;
    __property TWordCollection *collection = { read = fcoll, write = fcoll } ;
    __property bool showhint = { read = fshowhint, write = fshowhint } ;
    __property bool partselectable = { read = fselpart, write = set_selpart } ;
} ;

#endif
