//---------------------------------------------------------------------------

#ifndef wordstudytestH
#define wordstudytestH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include <ExtCtrls.hpp>
#include "edit2.h"
#include <Buttons.hpp>
#include "baseform.h"
#include <ComCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TWordStudyList;
class TKanjiEdit;
enum TQuestionType;
enum TRepetitionAnswer;
class TMouseOvers;

//#define AUTOTEST 1
#ifdef AUTOTEST
#define WM_Z_TMP  (WM_APP + 555)
#endif

class TfTest : public TBaseForm
{
__published: //IDE-managed Components
    TPanel *pAnswer;
    TButton *btn1;
    TButton *btn0;
    TButton *btn2;
    TButton *btn3;
    TLabel *lb1;
    TLabel *lb2;
    TLabel *lb0;
    TLabel *lb3;
    TLabel *Label5;
    TPanel *pInput3;
    TLabel *Label6;
    TZPaintBox *pbKanji;
    TSpeedButton *sbRecognize;
    TLabel *lbAnswer;
    TZPaintBox *pbAnswer;
    TPanel *pInput2;
    TLabel *Label8;
    TEdit2 *edRomaji;
    TButton *btnRomajiAccept;
    TButton *btnKanjiAccept;
    TLabel *Label9;
    TLabel *Label10;
    TPanel *pInput1;
    TButton *btnShowAnswer;
    TStatusBar *sbInfo;
    TLabel *lbKanji;
    TLabel *lbKana;
    TLabel *lbMeaning;
    TBevel *bv;
    TLabel *lbKanjiQ;
    TLabel *lbKanaQ;
    TLabel *lbMeaningQ;
    TTimer *tTime;
    TPanel *pHint;
    TSpeedButton *sbHint;
    TLabel *lbNew;
    TLabel *lbFailed;
    TPanel *pLimit;
    TLabel *Label1;
    TBitBtn *btnLimit1;
    TBitBtn *btnLimit2;
    TBitBtn *btnLimit3;
    TBevel *bv1;
    TBevel *bv2;
    TPanel *pOptions;
    TBitBtn *btnOptions;
    TPopupMenu *pmOptions;
    TMenuItem *miGroup;
    TMenuItem *miQuestion;
    TMenuItem *N1;
    TMenuItem *miSites;
    TPanel *pSuspended;
    TLabel *Label2;
    TBevel *Bevel1;
    TBevel *Bevel2;
    TBitBtn *btnResumeSuspend;
    TBitBtn *btnQuitSuspend;
    TPanel *pUndo;
    TLabel *Label3;
    TSpeedButton *sbChange;
    TPopupMenu *pmChange;
    TMenuItem *miRetry;
    TMenuItem *N3;
    TMenuItem *miGood;
    TMenuItem *miEasy;
    TMenuItem *N2;
    TMenuItem *miHard;
    TPanel *pPause;
    TButton *btnPause;
    TLabel *lbWordTypes;
    TLabel *Label4;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall sbRecognizeClick(TObject *Sender);
    void __fastcall edRomajiKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall btnRomajiAcceptClick(TObject *Sender);
    void __fastcall btnKanjiAcceptClick(TObject *Sender);
    void __fastcall btnShowAnswerClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall sbHintClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbAnswerPaint(TObject *Sender);
    void __fastcall pbKanjiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall btn1Click(TObject *Sender);
    void __fastcall tTimeTimer(TObject *Sender);
    void __fastcall btnLimit2Click(TObject *Sender);
    void __fastcall btnLimit1Click(TObject *Sender);
    void __fastcall btnOptionsClick(TObject *Sender);
    void __fastcall miQuestionClick(TObject *Sender);
    void __fastcall miGroupClick(TObject *Sender);
    void __fastcall btnResumeSuspendClick(TObject *Sender);
    void __fastcall FormMouseLeave(TObject *Sender);
    void __fastcall miRetryClick(TObject *Sender);
    void __fastcall sbChangeClick(TObject *Sender);
    void __fastcall btnPauseClick(TObject *Sender);
    void __fastcall pbKanjiKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall edRomajiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);

private: //User declarations
    TMouseOvers *mobj;
    void doMOPrepare(TCanvas *c, int index, bool selected);
    void doMODraw(TCanvas *c, const TRect &r, int index, int data, bool selected);
    void doMOChange(const TRect &r, int index, int data, bool selected);

    double undogoodinterval;
    double undoeasyinterval;

    TWordStudyList *study;
    TKanjiEdit *edkanji;
    bool recognizereset;
    TRect recognizerect;

    TQuestionType q;
    UnicodeString entered;

    TQuestionType fhint;
    TQuestionType fsecondary;

    TDateTime lastinput; //time when a button or input bas was last changed. used to measure idle time when the user is potentially away

    bool checklimit;
    int itemlimit; //number of items that must be tested before showing the warning window
    TDateTime timestart; //time when measuring of the time limit started

    int gindex; //index of word group that was last used when adding a word to it

    TDateTime start; //date and time when the current test started

    double ETA; //time the test might take if average times count
    bool newitem; //the current item is tested for the first time
    bool faileditem; //the current item is a previously failed item tested again
    bool firsttime; //the current item is asked for the first time in the current test (even if it was tested in another previously)

    int due; //number of items due in the current test
    int tested; //number of items already tested during the current test
    int failed; //number of incorrect answers during the current test
    int newcnt; //number of new items in the current test
    bool answered; //at least one answer was given by pressing any button from Try again to Too easy.

    bool answering; //when true disable all drawing so we don't end up painting unexisting word data

    //numbers before answering previous question corresponding to the same ones above
    int undodue;
    int undotested;
    int undofailed;
    bool undofirsttime;
    bool undofaileditem;

    //void __fastcall deactivate(TObject *Sender);
    //void __fastcall activate(TObject *Sender);

    void accept(const UnicodeString &aanswer); //go to the scoring panel after we have entered something
    void skip(); //skip typing in the answer and go directly to the scoring panel

    void drawhinttext(TQuestionType type);
    const wchar_t* answer_text(TRepetitionAnswer answer);
    void set_undofields(TRepetitionAnswer answer);

    void initnext();
    void kcallback();
    void dcallback();

    void resetlimits();

    void update_info();

    template<typename T>
    void csuspend(T *c);
    template<typename T>
    void cresume(T *c);

    void resetsuspendtime();
    void suspend();
    void resume();

    void tmpmsg(TMessage &msg);
    void z_ncactivate(TWMNCActivate &msg);
    //void z_activate(TWMActivate &msg);
    void z_init(TMessage &msg);
    void z_pos(TWMWindowPosChanging &msg);

    void __fastcall siteClick(TObject *Sender);
public: //User declarations
    BEGIN_MESSAGE_MAP
#ifdef AUTOTEST
        MESSAGE_HANDLER(WM_Z_TMP, TMessage, tmpmsg)
#endif
        //MESSAGE_HANDLER(WM_ACTIVATE,TWMActivate, z_activate)
        MESSAGE_HANDLER(WM_NCACTIVATE, TWMNCActivate, z_ncactivate)
        MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosChanging, z_pos)
        MESSAGE_HANDLER(WM_Z_INIT, TMessage, z_init)
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfTest(TComponent* Owner);
    virtual __fastcall ~TfTest();

    int ShowModalEx(TWordStudyList *astudy);

    __property bool AnswerGiven = { read = answered };
};
//---------------------------------------------------------------------------
extern PACKAGE TfTest *fTest;
//---------------------------------------------------------------------------
#endif
