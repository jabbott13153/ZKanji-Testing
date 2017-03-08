//---------------------------------------------------------------------------

#ifndef wordlistH
#define wordlistH
//---------------------------------------------------------------------------
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include "edit2.h"

class TWordListObj;
class TLineObj;
class TKanjiEdit;
class TChangedWordList;

class TfDict : public TBaseForm
{
__published: //IDE-managed Components
    TPanel *pToolbar;
    TZPaintBox *pbWords;
    TMainMenu *MainMenu1;
    TMenuItem *miFile;
    TPanel *pRomaji;
    TLabel *Label1;
    TPanel *pKanji;
    TSpeedButton *sbAddKanji;
    TSpeedButton *sbKanjiStart;
    TZPaintBox *edKanji;
    TLabel *Label2;
    TSpeedButton *sbKanjiEnd;
    TSpeedButton *sbRomajiEnd;
    TComboBox *edRomaji;
    TPanel *pSelect;
    TSpeedButton *btnKanji;
    TSpeedButton *btnRomaji;
    TBevel *Bevel4;
    TStatusBar *sbInfo;
    TBevel *Bevel1;
    TMenuItem *tempsave1;
    TBevel *Bevel5;
    TSpeedButton *btnFilter;
    TBevel *Bevel3;
    TSpeedButton *btnJapanese;
    TPanel *pJapanese;
    TLabel *Label3;
    TZPaintBox *edJapanese;
    TMenuItem *miCheck;
    TMenuItem *reimportirreg1;
    TPanel *pExamples;
    TZPaintBox *pbExamples;
    TPanel *Panel1;
    TSpeedButton *sbExamples;
    TBevel *Bevel6;
    TPanel *pExType;
    TSpeedButton *sbEx2;
    TSpeedButton *sbEx3;
    TSpeedButton *sbEx1;
    TPanel *pExArrows;
    TSpeedButton *sbExLeft;
    TSpeedButton *sbExRight;
    TPanel *pExPage;
    TEdit2 *edExJump;
    TSpeedButton *btnExJump;
    TLabel *lbExNum;
    TLabel *lbExMax;
    TPanel *Panel4;
    TSpeedButton *sbInfCon;
    TPanel *pExPageBtn;
    TPanel *pExTypeBtn;
    TBevel *Bevel7;
    TBevel *Bevel8;
    TBevel *Bevel9;
    TBevel *bvExPage;
    TBevel *bvExType;
    TBevel *Bevel12;
    TSpeedButton *sbRecognize;
    TBevel *Bevel2;
    TBevel *Bevel10;
    TPopupMenu *pmDict;
    void __fastcall edRomajiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall btnKanjiClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall sbAddKanjiClick(TObject *Sender);
    void __fastcall pbWordsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall edRomajiChange(TObject *Sender);
    void __fastcall pbWordsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall edRomajiExit(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall sbKanjiStartClick(TObject *Sender);
    void __fastcall sbKanjiEndClick(TObject *Sender);
    void __fastcall pbWordsKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall miClick(TObject *Sender);
    void __fastcall miEditWordClick(TObject *Sender);
    void __fastcall miDeleteWordClick(TObject *Sender);
    void __fastcall miRevertWordClick(TObject *Sender);
    void __fastcall miToVocabClick(TObject *Sender);
    void __fastcall miAppendKanjiClick(TObject *Sender);
    void __fastcall miAppendKanaClick(TObject *Sender);
    void __fastcall miCopyKanjiClick(TObject *Sender);
    void __fastcall miCopyKanaClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall tempsave1Click(TObject *Sender);
    void __fastcall btnFilterClick(TObject *Sender);
    void __fastcall btnFilterMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall miCheckClick(TObject *Sender);
    void __fastcall reimportirreg1Click(TObject *Sender);
    void __fastcall sbExamplesClick(TObject *Sender);
    void __fastcall sbExLeftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbExRightMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbEx2Click(TObject *Sender);
    void __fastcall sbEx3Click(TObject *Sender);
    void __fastcall sbEx1Click(TObject *Sender);
    void __fastcall btnExJumpClick(TObject *Sender);
    void __fastcall pExPageBtnClick(TObject *Sender);
    void __fastcall pExPageBtnMouseEnter(TObject *Sender);
    void __fastcall pExPageBtnMouseLeave(TObject *Sender);
    void __fastcall miNewWordClick(TObject *Sender);
    void __fastcall btnKanjiMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall edRomajiKeyPress(TObject *Sender, wchar_t &Key);
private: //User declarations
    typedef TBaseForm   inherited;

    TWordListObj *wobj;
    TLineObj *lobj;
    int fnoupdate;
    bool ffilterchanged;

    TNumEdit *numJump;

    bool kanjichanged;
    __int64 kanjisavetime;
    bool romajichanged;
    __int64 romajisavetime;
    __int64 lasttime;

    void DoDeactivate(void *caller);
    void DoActivate(void *caller);

    int get_current(); // Return the index of the current word from the words list.
    //void WMSysCommand(TWMSysCommand &msg);
    void wcallback();
    void lcallback();
    void ecallback();
    void jcallback();
    void xcallback();

    void set_autosize(bool val);
    bool get_autosize();
    void set_noupdate(bool val);
    bool get_noupdate();

    void update_info();
public: //User declarations
    /*
     BEGIN_MESSAGE_MAP
     MESSAGE_HANDLER(WM_SYSCOMMAND, TWMSysCommand, WMSysCommand)
     END_MESSAGE_MAP(TBaseForm)
     */
    __fastcall TfDict(TComponent* Owner);
    __fastcall virtual ~TfDict();

    TKanjiEdit *edkanji;
    TKanjiEdit *edjapanese;

    void WordChanged(byte coll, TChangedWordList *list);
    void UpdateWords(bool holdposition = true);
    void UpdateKButtons(); // Update buttons when the kanji list updates.
    void DeselectDictionary();
    void SelectDictionary(byte coll); // Changes the current dictionary used in dictionary look-ups. Set force to true to update controls even if the dictionary selection wouldn't change.
    void DictionaryRenamed(byte coll, UnicodeString s);
    void SetCaptions(UnicodeString s);
    void DictionaryDeleted(byte coll);

    void SearchWord(byte coll, TWord *w);
    void Search(byte coll, UnicodeString what);

    void settingsChanged(void *caller, TSettingsProcState state);
    void SaveSettings();
    void disable_examples();
    void enable_examples();

    void Time(__int64 sec); // Timer function that must be called every second.

    __property int current = { read = get_current };
    __property bool Autosize = { read = get_autosize, write = set_autosize };
    __property bool noupdate = { read = get_noupdate, write = set_noupdate };
    __property bool filterchanged = { read = ffilterchanged, write = ffilterchanged };
};
//---------------------------------------------------------------------------
extern PACKAGE TfDict *fDict;
//---------------------------------------------------------------------------
void SetFilter();
#endif
