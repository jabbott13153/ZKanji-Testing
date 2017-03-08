//---------------------------------------------------------------------------

#ifndef dockerH
#define dockerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "baseform.h"
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------


enum TDockAction { daNone, daWResize, daHResize };
enum TDockState { dsDocking, dsUndocking, dsDocked, dsUndocked, dsNone };
enum TDockLayout { dlExpandKanji, dlExpandDict, dlUndecided };

class TfDock : public TBaseForm
{
__published: //IDE-managed Components
    TMainMenu *mMenu;
    TMenuItem *miDictionary;
    TMenuItem *JapaneseEnglish1;
    TMenuItem *EnglishJapanese1;
    TMenuItem *BrowseJapanese1;
    TMenuItem *N1;
    TMenuItem *miTools;
    TMenuItem *Clipboard1;
    TMenuItem *Kanapractice1;
    TMenuItem *Settings1;
    TMenuItem *Window1;
    TMenuItem *miExpandDict;
    TMenuItem *miExpandKanji;
    TMenuItem *Help1;
    TMenuItem *About1;
    TMenuItem *Exit2;
    TMenuItem *N5;
    TMenuItem *Strokorderdiagrameditor1;
    TMenuItem *miKanjiList;
    TMenuItem *miWordList;
    TMenuItem *miKanjiGroups;
    TMenuItem *miDictSelect;
    TMenuItem *miShowHideEx;
    TMenuItem *oggleFilter1;
    TMenuItem *miToggleFilter;
    TMenuItem *N6;
    TMenuItem *Anystartofword1;
    TMenuItem *Anyendofword1;
    TMenuItem *oggleinfconj1;
    TMenuItem *N7;
    TMenuItem *N8;
    TMenuItem *BETATestBETA1;
    TMenuItem *N9;
    TMenuItem *CheckforUpdates1;
    TMenuItem *N10;
    TMenuItem *Submenu1;
    TMenuItem *Longtermstudylist1;
    TMenuItem *JLPTvocabularylisteditor1;
    TMenuItem *N2;
    TMenuItem *Editfilters1;
    TMenuItem *N4;
    TMenuItem *miKInfo;
    TMenuItem *miKTranslate;
    TMenuItem *miKKanjiDef;
    TMenuItem *miKGenerate;
    TMenuItem *N11;
    TMenuItem *N12;
    TMenuItem *N13;
    TMenuItem *N14;
    TMenuItem *miKCopy;
    TMenuItem *miKAppend;
    TMenuItem *N15;
    TMenuItem *miKCopyAll;
    TMenuItem *miKAppendAll;
    TMenuItem *miWToVocab;
    TMenuItem *N16;
    TMenuItem *miWEditWord;
    TMenuItem *miWNewWord;
    TMenuItem *miWDeleteWord;
    TMenuItem *N17;
    TMenuItem *miWCopyKanji;
    TMenuItem *miWAppendKanji;
    TMenuItem *N18;
    TMenuItem *miWCopyKana;
    TMenuItem *miWAppendKana;
    TMenuItem *miGKKanjiInfo;
    TMenuItem *N19;
    TMenuItem *miGKTranslateKanji;
    TMenuItem *N20;
    TMenuItem *miGKKanjiDef;
    TMenuItem *N21;
    TMenuItem *miGKGenerate;
    TMenuItem *N22;
    TMenuItem *miGWToVocab;
    TMenuItem *N24;
    TMenuItem *miGWTranslateWritten;
    TMenuItem *miGWTranslateKana;
    TMenuItem *N25;
    TMenuItem *miGWEditWord;
    TMenuItem *N26;
    TMenuItem *miGWPrintVocab;
    TMenuItem *miGKCopyKanji;
    TMenuItem *miGKAppendKanji;
    TMenuItem *N23;
    TMenuItem *miGKCopyAll;
    TMenuItem *miGKAppendAll;
    TMenuItem *miGWCopyKana;
    TMenuItem *miGWAppendKana;
    TMenuItem *File1;
    TMenuItem *N28;
    TMenuItem *miExit;
    TMenuItem *N3;
    TMenuItem *miExamples;
    TMenuItem *miWordGroups;
    TMenuItem *N29;
    TMenuItem *miGWCopyWord;
    TMenuItem *miGWAppendWord;
    TMenuItem *N30;
    TMenuItem *N31;
    TMenuItem *miOpenFolder;
    TMenuItem *Export1;
    TMenuItem *Import1;
    TMenuItem *Groupimport1;
    TMenuItem *Dictionaryimport1;
    TMenuItem *Fulldictionaryimport1;
    TMenuItem *miWRevertWord;
    TMenuItem *N27;
    TMenuItem *miShowKanjiList;
    TMenuItem *miShowGroupList;
    TMenuItem *miShowDictionary;
    void __fastcall FormResize(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall miKInfoClick(TObject *Sender);
    void __fastcall miKTranslateClick(TObject *Sender);
    void __fastcall miKKanjiDefClick(TObject *Sender);
    void __fastcall miKGenerateClick(TObject *Sender);
    void __fastcall miKCopyClick(TObject *Sender);
    void __fastcall miKAppendClick(TObject *Sender);
    void __fastcall miKCopyAllClick(TObject *Sender);
    void __fastcall miKAppendAllClick(TObject *Sender);
    void __fastcall miWToVocabClick(TObject *Sender);
    void __fastcall miWEditWordClick(TObject *Sender);
    void __fastcall miWNewWordClick(TObject *Sender);
    void __fastcall miWDeleteWordClick(TObject *Sender);
    void __fastcall miWCopyKanjiClick(TObject *Sender);
    void __fastcall miWAppendKanjiClick(TObject *Sender);
    void __fastcall miWCopyKanaClick(TObject *Sender);
    void __fastcall miWAppendKanaClick(TObject *Sender);
    void __fastcall miGKKanjiInfoClick(TObject *Sender);
    void __fastcall miGKTranslateKanjiClick(TObject *Sender);
    void __fastcall miGKKanjiDefClick(TObject *Sender);
    void __fastcall miGKGenerateClick(TObject *Sender);
    void __fastcall miGWToVocabClick(TObject *Sender);
    void __fastcall miGWTranslateWrittenClick(TObject *Sender);
    void __fastcall miGWTranslateKanaClick(TObject *Sender);
    void __fastcall miGWEditWordClick(TObject *Sender);
    void __fastcall miGWPrintVocabClick(TObject *Sender);
    void __fastcall miGKCopyKanjiClick(TObject *Sender);
    void __fastcall miGKAppendKanjiClick(TObject *Sender);
    void __fastcall miGKCopyAllClick(TObject *Sender);
    void __fastcall miGKAppendAllClick(TObject *Sender);
    void __fastcall miGWCopyKanaClick(TObject *Sender);
    void __fastcall miGWAppendKanaClick(TObject *Sender);
    void __fastcall miGGExportClick(TObject *Sender);
    void __fastcall miGImportWakanClick(TObject *Sender);
    void __fastcall miGExportClick(TObject *Sender);
    void __fastcall miGImportClick(TObject *Sender);
    void __fastcall miExamplesClick(TObject *Sender);
    void __fastcall miGWCopyWordClick(TObject *Sender);
    void __fastcall miGWAppendWordClick(TObject *Sender);
    void __fastcall miOpenFolderClick(TObject *Sender);
    void __fastcall Export1Click(TObject *Sender);
    void __fastcall Groupimport1Click(TObject *Sender);
    void __fastcall Dictionaryimport1Click(TObject *Sender);
    void __fastcall Fulldictionaryimport1Click(TObject *Sender);
    void __fastcall miWRevertWordClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall miExpandKanjiClick(TObject *Sender);
    void __fastcall miShowKanjiListClick(TObject *Sender);
private: //User declarations
    typedef TBaseForm inherited;

    struct TMenuMoveItem
    {
        TMenuItem *owner;
        TMenuItem *moved;
        int index;
    };
    typedef TTemplateList<TMenuMoveItem, true> TMenuMoveList;
    TMenuMoveList *moves;

    TNotifyEvent oldresize;

    int fdragpos; // Mouse position within the vertical divider next to the kanji list, to respect the divider's left side's position.
    int fdivsize;

    // bool fdocking;

    int wpos;
    int hpos;
    int wdiv;
    int hdiv;
    double hrate;

    TRect margins;

    TDockAction action;
    TDockState fstate;

    TDockLayout flayout;

    TWinControl *activectrl;

    // Move a menu item from another window to this one, remembering its old position and owner.
    // From and count when specified gives the index of the first and the number of menu items to move. set count to -1 when all items should be moved.
    // Newpos is the new position of the menu item in its new place. -1 means add to end.
    //void __fastcall MoveMenu(TMenuItem *what, TMenuItem *to, int from, int count, int newpos = -1);
    //void __fastcall MoveMenu(TMenuItem *what, TMenuItem *to, int newpos = -1);
    //void __fastcall RestoreMenus();

    void onactivate(void *caller);
    void ondeactivate(void *caller);

    void __fastcall kanjiResize(TObject *Sender);
    void UpdateMenuCheck();
protected:

    void Resized(bool manual, bool forced = false);
    void set_layout(TDockLayout val);

    void WMSysCommand(TWMSysCommand &msg);
    void z_enable(TMessage &msg);
public: //User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_SYSCOMMAND, TWMSysCommand, WMSysCommand)
        MESSAGE_HANDLER(WM_Z_ENABLE, TMessage, z_enable)
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfDock(TComponent* Owner);
    virtual __fastcall ~TfDock();

    void dock(bool hidden);
    void undock(bool show);

    void SetCaption();

    void init();

    __property int splitwidth = { read = wdiv, write = wdiv };
    __property double heightrate = { read = hrate, write = hrate };

    __property TDockState state = { read = fstate };
    __property TDockLayout layout = { read = flayout, write = set_layout };
};
//---------------------------------------------------------------------------
void __fastcall DockWindows(bool show);
void __fastcall UndockWindows(bool show);
//---------------------------------------------------------------------------
extern PACKAGE TfDock *fDock;
//---------------------------------------------------------------------------
#endif
