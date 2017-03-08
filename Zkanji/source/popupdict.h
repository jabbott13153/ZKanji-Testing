//---------------------------------------------------------------------------

#ifndef popupdictH
#define popupdictH
//---------------------------------------------------------------------------
#include <Buttons.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
#include "ZPaintBox.h"
//---------------------------------------------------------------------------

#include "baseform.h"
#include "edit2.h"

class TLineObj;
class TWordListObj;
struct TKanji;
class TKanjiEdit;
class TChangedWordList;
class TMidCloseButton;
class TPinButton;
class TMinButton;
class TFloatButton;

//---------------------------------------------------------------------------
class TfPopupDict : public TBaseForm
{
__published: //IDE-managed Components
    TZPaintBox *pbWords;
    TPanel *pToolbar;
    TPanel *pRomaji;
    TLabel *Label1;
    TPanel *pKanji;
    TLabel *Label2;
    TZPaintBox *edKanji;
    TSpeedButton *sbRomajiEnd;
    TSpeedButton *sbKanjiEnd;
    TComboBox *edRomaji;
    TSpeedButton *sbKanjiStart;
    TPanel *pSelect;
    TSpeedButton *btnKanji;
    TSpeedButton *btnRomaji;
    TPanel *pJapanese;
    TLabel *Label3;
    TZPaintBox *edJapanese;
    TSpeedButton *btnJapanese;
    TPanel *pExamples;
    TBevel *Bevel6;
    TZPaintBox *pbExamples;
    TPanel *Panel1;
    TSpeedButton *sbExamples;
    TBevel *Bevel1;
    TPanel *Panel3;
    TSpeedButton *sbInfCon;
    TPanel *pExPage;
    TSpeedButton *btnExJump;
    TLabel *lbExNum;
    TLabel *lbExMax;
    TBevel *Bevel7;
    TBevel *bvExPage;
    TEdit2 *edExJump;
    TPanel *pExPageBtn;
    TPanel *pExType;
    TSpeedButton *sbEx2;
    TSpeedButton *sbEx3;
    TSpeedButton *sbEx1;
    TBevel *Bevel8;
    TBevel *bvExType;
    TPanel *pExTypeBtn;
    TPanel *pExArrows;
    TSpeedButton *sbExLeft;
    TSpeedButton *sbExRight;
    TBevel *Bevel9;
    TBevel *Bevel12;
    TSpeedButton *sbRecognize;
    TBevel *Bevel3;
    TSpeedButton *btnFilter;
    TBevel *Bevel5;
    TBevel *Bevel2;
    TBevel *Bevel10;
    TBevel *bR;
    void __fastcall edRomajiChange(TObject *Sender);
    void __fastcall btnKanjiClick(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall edRomajiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall edRomajiExit(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall FormDeactivate(TObject *Sender);
    void __fastcall pbWordsKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall pbWordsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbWordsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbKanjiStartClick(TObject *Sender);
    void __fastcall sbKanjiEndClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall sbExamplesClick(TObject *Sender);
    void __fastcall sbEx1Click(TObject *Sender);
    void __fastcall sbEx2Click(TObject *Sender);
    void __fastcall sbEx3Click(TObject *Sender);
    void __fastcall sbExRightMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbExLeftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pExPageBtnMouseEnter(TObject *Sender);
    void __fastcall pExPageBtnMouseLeave(TObject *Sender);
    void __fastcall pExPageBtnClick(TObject *Sender);
    void __fastcall btnExJumpClick(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall btnFilterMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnFilterClick(TObject *Sender);
    void __fastcall btnKanjiMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall edRomajiKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);

private: //User declarations
    typedef TBaseForm inherited;

    TWordListObj *wobj;
    TLineObj *lobj;

    TKanji *popkanji;
    UnicodeString poptext;
    int fnoupdate;
    bool ffilterchanged;

    TNumEdit *numJump;

    bool kanjichanged;
    __int64 kanjisavetime;
    bool romajichanged;
    __int64 romajisavetime;
    __int64 lasttime;

    int lockcnt;
    bool pinned;
    bool floating;

    TRect floatsize;
    TRect minsize;

    void repaintProc(void *caller);

    void UpdateWords(bool holdposition = true);
    void WMNCHitTest(TWMNCHitTest &msg);
    void WMNCPaint(TWMNCPaint &msg);
    void WMNCCalcSize(TMessage &msg);
    void WMDestroy(TMessage &msg);

    void set_autosize(bool val);
    bool get_autosize();

    void lcallback();
    void wcallback();
    void ecallback();
    void jcallback();
    void xcallback();
    void set_noupdate(bool val);
    bool get_noupdate();
    void focus_edit();

    friend void SetFilter();

    TMidCloseButton *closebtn;
    TPinButton *pinbtn;
    TMinButton *minbtn;
    TFloatButton *floatbtn;

    void closefunc();
    void pinfunc();
    void minfunc();
    void floatfunc();
    void btndrawfunc(TCanvas *c, TRect const &r);

public: //User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_NCHITTEST, TWMNCHitTest, WMNCHitTest)
        MESSAGE_HANDLER(WM_NCPAINT, TWMNCPaint, WMNCPaint)
        MESSAGE_HANDLER(WM_NCCALCSIZE, TMessage, WMNCCalcSize)
        MESSAGE_HANDLER(WM_DESTROY, TMessage, WMDestroy)
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfPopupDict(TComponent* Owner);

    TKanjiEdit *edkanji;
    TKanjiEdit *edjapanese;

    void LockPopup(bool lock);
    void SelectDictionary(byte coll);
    void DictionaryDeleted(byte coll);

    void Float(bool val);
    void Popup(bool show, TDictionaryMode mode = dmKanji);
    void SaveSettings();
    void WordChanged(byte coll, TChangedWordList *list);
    void disable_examples();
    void enable_examples();

    void SearchWord(byte coll, TWord *w);
    void Search(byte coll, UnicodeString what);

    void Time(__int64 sec); // Timer function that must be called every second.


    void settingsChanged(void *caller, TSettingsProcState state);

    __property bool Autosize = { read = get_autosize, write = set_autosize };
    __property bool Pinned = { read = pinned };
    __property bool Floating = { read = floating };
    __property bool noupdate = { read = get_noupdate, write = set_noupdate };
    __property bool filterchanged = { read = ffilterchanged, write = ffilterchanged };
};

//---------------------------------------------------------------------------
extern PACKAGE TfPopupDict *fPopupDict;
//---------------------------------------------------------------------------
#endif
