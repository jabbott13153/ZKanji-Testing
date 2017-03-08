//---------------------------------------------------------------------------

#ifndef kanjilistH
#define kanjilistH
//---------------------------------------------------------------------------
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------

#include "radicals.h"
#include "kanjiedit.h"
#include "edit2.h"
#include "zrads.h"

class TKanjiDrawObj;
class TKanjiGrid;
struct TKanji;
class TWordCollection;
class TMidCloseButton;
class TKanjiItemList;
class TChangedWordList;
class TZMenuRows;
//---------------------------------------------------------------------------
// Enum of items for the selectable search filter where a string must be entered, strokes, jlpt etc. excluded.
enum TKanjiSearchIndex { siGakken, siHalpern, siHeisig, siHenshall, siKnK, siNelson, siSnH, siUNICODE, siJIS, siSJIS, siEUC, siKuten };

class TfKanji : public TBaseForm
{
__published: // IDE-managed Components
    TPanel *pWhole;
    TStatusBar *sbInfo;
    TImageList *il4C;
    TPopupMenu *pm4C;
    TMenuItem *N11;
    TMenuItem *N21;
    TMenuItem *N31;
    TMenuItem *N41;
    TMenuItem *N51;
    TMenuItem *N61;
    TMenuItem *N71;
    TMenuItem *N81;
    TMenuItem *N91;
    TMenuItem *N92;
    TPanel *pGrid;
    TPanel *pSort;
    TZPaintBox *pbGrid;
    TPanel *pFilter6;
    TEdit2 *edCustom;
    TComboBox *cbCustom;
    TPanel *pFilter5;
    TLabel *Label7;
    TCheckBox *cb4C;
    TEdit2 *edC1;
    TEdit2 *edC3;
    TEdit2 *edC4;
    TEdit2 *edC2;
    TEdit2 *edC5;
    TPanel *pFilter4;
    TSpeedButton *sbS1;
    TSpeedButton *sbS2;
    TSpeedButton *sbS3;
    TSpeedButton *sbS4;
    TLabel *Label8;
    TLabel *Label9;
    TCheckBox *cbSKIP;
    TEdit2 *edS2;
    TEdit2 *edS3;
    TPanel *pFilter1;
    TEdit2 *edMeaning;
    TEdit2 *edStrokes;
    TPanel *pInfo;
    TZPaintBox *pbKanji;
    TBevel *Bevel2;
    TBevel *Bevel4;
    TZPaintBox *pbReadings;
    TBevel *bvRBottom;
    TPanel *pHide;
    TSpeedButton *btnHide;
    TPanel *pSortInner;
    TLabel *lbS;
    TComboBox *cbSortlist;
    TLabel *lbF;
    TComboBox *cbList;
    TBevel *Bevel11;
    TPanel *pFilter3;
    TBevel *Bevel3;
    TSpeedButton *btnRads;
    TCheckBox *cbOku;
    TCheckBox *cbKun;
    TCheckBox *cbOn;
    TZPaintBox *edReading;
    TBevel *bvHideLeft;
    TZPaintBox *pbMeanings;
    TMenuItem *miNull;
    TBevel *Bevel8;
    TZPaintBox *pbRadicals;
    TCheckBox *cbMeaning;
    TCheckBox *cbReading;
    TCheckBox *cbStrokes;
    TBevel *Bevel10;
    TPanel *pFilter2;
    TBevel *Bevel12;
    TEdit2 *edJouyou;
    TCheckBox *cbJouyou;
    TCheckBox *cbJLPT;
    TEdit2 *edJLPT;
    TBevel *Bevel14;
    TBevel *Bevel15;
    TBevel *Bevel16;
    TBevel *Bevel17;
    TBevel *Bevel18;
    TBevel *Bevel19;
    TBevel *Bevel20;
    TBevel *Bevel21;
    TBevel *Bevel22;
    TBevel *Bevel23;
    TBevel *Bevel24;
    TBevel *Bevel25;
    TBevel *Bevel5;
    TBevel *Bevel13;
    TBevel *Bevel1;
    TPanel *pFilterTop;
    TLabel *Label3;
    TSpeedButton *sbF1;
    TSpeedButton *sbF2;
    TSpeedButton *sbF3;
    TSpeedButton *sbF4;
    TSpeedButton *sbReset;
    TSpeedButton *sbFAll;
    TSpeedButton *sbF5;
    TSpeedButton *sbF6;
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall cbSortlistChange(TObject *Sender);
    void __fastcall edChange(TObject *Sender);
    void __fastcall pbRadicalsPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall sbResetClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall miInfoClick(TObject *Sender);
    void __fastcall miTranslateClick(TObject *Sender);
    void __fastcall miCopyClick(TObject *Sender);
    void __fastcall miAppendClick(TObject *Sender);
    void __fastcall pbGridMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbS1Click(TObject *Sender);
    void __fastcall N91MeasureItem(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height);
    void __fastcall N91Click(TObject *Sender);
    void __fastcall edC1Enter(TObject *Sender);
    void __fastcall edC5Change(TObject *Sender);
    void __fastcall edS2Change(TObject *Sender);
    void __fastcall sbF1Click(TObject *Sender);
    void __fastcall sbFAllClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall cbCustomChange(TObject *Sender);
    void __fastcall pbReadingsPaint(TObject *Sender);
    void __fastcall pbMeaningsPaint(TObject *Sender);
    void __fastcall btnHideClick(TObject *Sender);
    void __fastcall btnRadsClick(TObject *Sender);
    void __fastcall English1Click(TObject *Sender);
    void __fastcall pbGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall miNullMeasureItem(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height);
    void __fastcall miCopyAllClick(TObject *Sender);
    void __fastcall miAppendAllClick(TObject *Sender);
    void __fastcall miKanjiDefClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall pbRadicalsMouseEnter(TObject *Sender);
    void __fastcall pbRadicalsMouseLeave(TObject *Sender);
    void __fastcall pbRadicalsResize(TObject *Sender);
    void __fastcall edStrokesChange(TObject *Sender);
    void __fastcall edMeaningChange(TObject *Sender);
    void __fastcall cbMeaningClick(TObject *Sender);
    void __fastcall cbReadingClick(TObject *Sender);
    void __fastcall cbStrokesClick(TObject *Sender);
    void __fastcall edJouyouChange(TObject *Sender);
    void __fastcall edJLPTChange(TObject *Sender);
    void __fastcall cbJouyouClick(TObject *Sender);
    void __fastcall pFilterTopMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall cbJLPTClick(TObject *Sender);
    void __fastcall pbRadicalsClick(TObject *Sender);
    void __fastcall miGenerateClick(TObject *Sender);
    void __fastcall sbInfoMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbInfoMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbInfoMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall pbGridMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall miNewWordClick(TObject *Sender);
    void __fastcall miDeleteWordClick(TObject *Sender);
    void __fastcall edReadingResize(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
private: // User declarations
    typedef TBaseForm inherited;

    typedef TRadicalList::TRadGroup  TRadGroup;

    //void __fastcall WMSysCommand(TWMSysCommand &msg);
    void WMSizing(TWMSizing &msg);
    //void WMMovingAndWMMove(TMessage &msg);
    void WMChangeCBChain(TWMChangeCBChain &msg);
    void WMDrawClipboard(TMessage &msg);
    void WMNCCalcSize(TMessage &msg);
    void WMNCHitTest(TMessage &msg);
    void WMNCPaint(TMessage &msg);
    void WMDestroy(TMessage &msg);

    //TSearchOptions *options;

    TMidCloseButton *closebtn;

    bool clipviewing;

    TKanjiDrawObj *kbox;
    int fcolsize; // Width of each reading column in data field.
    int frowsize;

    bool rads[RAD_COUNT]; // Selected radicals.
    TRadGroup *rads2; // Selected radicals for classic2 search.
    bool radselected;

    byte fradtype; // Classical radicals are used for filtering.

    bool fontop;

    int fboxsize;
    TKanjiGrid *fkobj;
    bool nofilter;

    HWND clipnext; // The next clipboard viewer window.

    TZMenuRows *gridmenu;
    TZMenuRows *gridmenuextra;
    TZMenuRows *gridmenucombined;

    void popupcallback(TZMenuRows *rows, int col, int row);

    void kcallback();
    void ecallback();
    TKanji* get_kanji();
    void set_kanji(TKanji *k);

    void settingsChanged(void *caller, TSettingsProcState state);
    void appDeactivate(void *caller);

    void repaintProc(void *caller);

    void do_sizing(TRect r, bool forced);
    void ArrangeFilters(int changed = -1);
    void UpdateData();

    UnicodeString get_radicals();
    void set_radicals(UnicodeString s);

    void set_collection(TWordCollection *acollection);
    TWordCollection* get_collection();

    TKanji* get_kanjis(int ix);
    int get_count();

    void clearrads2(TRadGroup *&l);

    void bgdraw(TCanvas *c, TRect const &r);
    void unpop();

    int get_changewidth();
    int get_minlim();

    friend void kcallback1(TMenuItem *mi, int ix);
    friend void kcallback2(TMenuItem *mi, int ix);
protected:
    virtual void __fastcall CreateWnd();
public: // User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_SIZING, TWMSizing, WMSizing)
        MESSAGE_HANDLER(WM_CHANGECBCHAIN, TWMChangeCBChain, WMChangeCBChain)
        MESSAGE_HANDLER(WM_DRAWCLIPBOARD, TMessage, WMDrawClipboard)
        MESSAGE_HANDLER(WM_NCCALCSIZE, TMessage, WMNCCalcSize)
        MESSAGE_HANDLER(WM_NCHITTEST, TMessage, WMNCHitTest)
        MESSAGE_HANDLER(WM_NCPAINT, TMessage, WMNCPaint)
        MESSAGE_HANDLER(WM_DESTROY, TMessage, WMDestroy);
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfKanji(TComponent* Owner);
    virtual __fastcall ~TfKanji();

    TKanjiEdit *edreading;

    void DefChanged();
    void FilterKanji();
    //void SaveOptions();
    //void RestoreOptions();
    //void FreeOptions();
    void FillKanjiList(TKanjiItemList *kl, bool selected);
    UnicodeString KanjiString();
    UnicodeString SelString();
    void WordChanged(byte coll, TChangedWordList *list);

    //bool ChangeLimit(int width);

    void GenerateWordGroup();

    void StartClipviewing();
    void StopClipviewing();

    void Popup(bool show, bool activate = true);

    __property int boxsize = { read = fboxsize };
    __property TKanji* kanji = { read = get_kanji, write = set_kanji }; //selected kanji
    __property TKanji* Kanjis[int ix] = { read = get_kanjis };
    __property int Count = { read = get_count };
    __property byte radtype = { read = fradtype, write = fradtype };
    __property UnicodeString radicals = { read = get_radicals, write = set_radicals };
    __property TWordCollection *collection = { read = get_collection, write = set_collection };
    __property bool ontop = { read = fontop };

    __property int changewidth = { read = get_changewidth };
    __property int minlimit = { read = get_minlim };
};
//---------------------------------------------------------------------------
extern PACKAGE TfKanji *fKanji;
//---------------------------------------------------------------------------
#endif
