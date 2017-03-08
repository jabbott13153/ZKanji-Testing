//---------------------------------------------------------------------------

#ifndef studyroomH
#define studyroomH
//---------------------------------------------------------------------------
#include "baseform.h"
#include <Buttons.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
#include "ZPaintBox.h"
//---------------------------------------------------------------------------

class TWordList;
class TWordGroupStudy;
class TKanjiEdit;
//---------------------------------------------------------------------------

class TfWStudy : public TBaseForm
{
__published: //IDE-managed Components
    TTimer *tTimer;
    TZPaintBox *pbChoices;
    TLabel *lbChoices;
    TZPaintBox *pbTimer;
    TLabel *lbTitleMeaning;
    TPaintBox *pbKanji;
    TPaintBox *pbKana;
    TLabel *lbTitleKanji;
    TLabel *lbTitleKana;
    TBevel *Bevel1;
    TBevel *b7;
    TBevel *b8;
    TPaintBox *pbMeaning;
    TBevel *b9;
    TLabel *lbInstruction;
    TPanel *pButtons;
    TBevel *b11;
    TSpeedButton *btnWrong;
    TSpeedButton *btnCorrect;
    TPaintBox *pbInfo;
    TPanel *pKana;
    TZPaintBox *edKana;
    TLabel *Label1;
    TSpeedButton *btnAccept;
    TSpeedButton *btnSuspend;
    TSpeedButton *btnSolve;
    TSpeedButton *btnMShow;
    TBevel *b1;
    TBevel *b12;
    TBevel *bChoices;
    TPanel *pKanji;
    TLabel *Label2;
    TSpeedButton *btnAccept2;
    TZPaintBox *edKanji;
    TSpeedButton *sbKanji;
    void __fastcall tTimerTimer(TObject *Sender);
    void __fastcall pbKanaPaint(TObject *Sender);
    void __fastcall pbTimerPaint(TObject *Sender);
    void __fastcall pbKanjiPaint(TObject *Sender);
    void __fastcall btnSuspendClick(TObject *Sender);
    void __fastcall btnSolveClick(TObject *Sender);
    void __fastcall pbChoicesPaint(TObject *Sender);
    void __fastcall pbMeaningPaint(TObject *Sender);
    void __fastcall pbChoicesMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall pbChoicesMouseLeave(TObject *Sender);
    void __fastcall pbChoicesMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnCorrectClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbInfoPaint(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall btnAcceptClick(TObject *Sender);
    void __fastcall btnMShowClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);
private: //User declarations
    static HFONT Kfont;
    static HFONT Kfontb;

    unsigned long ticks;
    TWordGroupStudy *fgroup;
    bool fchoices;
    bool ftimer;

  //information about the current word
    int fword;

    bool fshown; //show every information about the current word

    bool fshowmeaning; //show meaning
    bool fshowkanji; //show kanji
    bool fshowkana; //show kana

    bool fnokanji; //fshowkanji true but there is no kanji in the word only kana

    bool fbtnmeaning;

    int fmeaningcorrect; //show meaning
    int fkanjicorrect; //show kanji
    int fkanacorrect; //show kana

    UnicodeString fmeaning;
    UnicodeString fkanji;
    UnicodeString fkana;

    int ftimepos;
    int ftimemax;

    int fchoice[5]; //index of 5 random choices
    byte fanswer; //what the answer is from the given choices
    char fselected; //0 based index of clicked answer when in choice mode. -1 while none

    void NextWord(); //sets up data to show, starts timer, sets buttons

    void CorrectAnswer(bool correct);
    //void ResetTest();

    void set_btnmeaning(bool val);

    TKanjiEdit *edkanji;
    TKanjiEdit *edkana;

    bool recognizereset;
    TRect recognizerect;
public: //User declarations
    __fastcall TfWStudy(TComponent* Owner);
    virtual __fastcall ~TfWStudy();
    int ShowModalEx(TWordGroupStudy *agroup);

    __property bool meaningbutton = { read = fbtnmeaning, write = set_btnmeaning };
    __property bool ShowingResult = { read = fshown }; //Only during tests.
};
//---------------------------------------------------------------------------
extern PACKAGE TfWStudy *fWStudy;
//---------------------------------------------------------------------------
extern const byte study_method_count;
extern char *study_method_name[];

#endif
