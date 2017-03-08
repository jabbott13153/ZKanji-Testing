//---------------------------------------------------------------------------

#ifndef grouplistH
#define grouplistH
//---------------------------------------------------------------------------
#include <Buttons.hpp>
#include <CheckLst.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
#include "ZPaintBox.h"
//---------------------------------------------------------------------------

#include "baseform.h"
#include "edit2.h"

class TKanjiCardGrid;
class TKanjiGroupList;
class TWordListObj;
class TVirtualWordListObj;
class TVocabListObj;
class TReadingBoxObj;
class TWordCollection;
class TWordGroupList;
class TWordList;
class TKanjiList;
class TWordIndexList;
struct TKanji;
class TKanjiEdit;
class TChangedWordList;
enum TWordColumnType;
class TZMenuRows;

enum TMeaningEditState { mesUnchanged, mesUserDef, mesModified };

class TfGroup : public TBaseForm
{
__published: //IDE-managed Components
    TPanel *KanjiTab;
    TPanel *WordTab;
    TPanel *pWordButtons;
    TBevel *Bevel3;
    TZPaintBox *pbWords;
    TPanel *pTop;
    TPanel *pDict;
    TLabel *Label8;
    TSpeedButton *btnDeleteDict;
    TSpeedButton *btnNewDict;
    TSpeedButton *btnRenameDict;
    TComboBox *cbDictionary;
    TTabControl *Pages;
    TPanel *pKBack;
    TPanel *pKanjiButtons;
    TBevel *Bevel4;
    TCheckListBox *lbKGroups;
    TPanel *pKanjiTop;
    TPanel *pKTop;
    TZPaintBox *pbGrid;
    TCheckListBox *lbWGroups;
    TStatusBar *sbInfo;
    TPanel *pExBar;
    TSpeedButton *btnKCard;
    TBevel *Bevel1;
    TPanel *pKInfo;
    TLabel *lbKInfo1;
    TLabel *lbKInfo2;
    TLabel *lbKInfo3;
    TBevel *Bevel13;
    TPanel *pKControls1;
    TSpeedButton *sbAddWord;
    TComboBox *cbGroupEx;
    TPanel *pKControls2;
    TSpeedButton *sbKDef;
    TSpeedButton *sbKWords;
    TPanel *pKWords;
    TZPaintBox *pbKWords;
    TBevel *Bevel2;
    TLabel *lbKMeaning;
    TEdit2 *edKMeaning;
    TZPaintBox *pbEWords;
    TSpeedButton *btnKSave;
    TPanel *pKDef;
    TBevel *Bevel6;
    TLabel *Label4;
    TSpeedButton *btnKDSave;
    TSpeedButton *btnKDReset;
    TSpeedButton *btnKDClear;
    TSpeedButton *sbKRemove;
    TBevel *Bevel14;
    TSpeedButton *btnKClear;
    TSpeedButton *sbKUp;
    TSpeedButton *sbKDown;
    TZPaintBox *pbReadings;
    TSpeedButton *sbKMember;
    TMemo *mDefinition;
    TSpeedButton *sbFilter;
    TSpeedButton *sbAddToReading;
    TBevel *Bevel17;
    TBevel *Bevel11;
    TSpeedButton *sbMatch;
    TZPaintBox *pbKFilter;
    TLabel *Label10;
    TBevel *Bevel16;
    TPanel *pGroupBtn;
    TBevel *Bevel5;
    TBevel *Bevel7;
    TBevel *Bevel10;
    TBevel *bvGroupBtn;
    TLabel *Label9;
    TBevel *Bevel15;
    TCheckBox *cbKunGroup;
    TSpeedButton *btnKFilter;
    TPanel *pWordFilterControls;
    TBevel *Bevel8;
    TBevel *Bevel9;
    TSpeedButton *btnWFilter;
    TLabel *Label11;
    TZPaintBox *pbWFilter;
    TSpeedButton *btnWordDown;
    TSpeedButton *btnWordUp;
    TSpeedButton *btnWordDelete;
    TBevel *Bevel12;
    TShape *Shape3;
    TPanel *pKControls;
    TSpeedButton *btnKAdd;
    TSpeedButton *btnKRemove;
    TSpeedButton *btnKLeft;
    TSpeedButton *btnKRight;
    TShape *Shape4;
    TPanel *Panel1;
    TBevel *Bevel18;
    TLabel *Label7;
    TComboBox *cbToGroup;
    TSpeedButton *btnToGroup;
    TPanel *pKBButtons;
    TBevel *blKBottom;
    TSpeedButton *btnKEditStudy;
    TSpeedButton *btnKAddStudy;
    TPanel *pWBButtons;
    TBevel *blWBottom2;
    TSpeedButton *btnWEditStudy;
    TSpeedButton *btnWAddStudy;
    TPanel *pWTButtons;
    TSpeedButton *btnWChkNone;
    TSpeedButton *btnWChkAll;
    TSpeedButton *btnWRename;
    TSpeedButton *btnWDelete;
    TSpeedButton *btnWNew;
    TSpeedButton *btnWDown;
    TSpeedButton *btnWUp;
    TPanel *pKTButtons;
    TSpeedButton *btnKRename;
    TSpeedButton *btnKDelete;
    TSpeedButton *btnKNew;
    TSpeedButton *btnKDown;
    TSpeedButton *btnKUp;
    TSpeedButton *sbVocab;
    TSpeedButton *sbStudy;
    TSpeedButton *btnKChkAll;
    TSpeedButton *btnKChkNone;
    TBevel *blWTop;
    TBevel *blKTop;
    TPanel *pWSButtons;
    TBevel *blWBottom;
    TSpeedButton *btnWStudyList;
    TSpeedButton *btnWReset;
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall btnKNewClick(TObject *Sender);
    void __fastcall btnKDeleteClick(TObject *Sender);
    void __fastcall lbKGroupsClick(TObject *Sender);
    void __fastcall btnKRenameClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall btnKChkAllClick(TObject *Sender);
    void __fastcall btnKChkNoneClick(TObject *Sender);
    void __fastcall lbKGroupsClickCheck(TObject *Sender);
    void __fastcall btnWNewClick(TObject *Sender);
    void __fastcall btnWDeleteClick(TObject *Sender);
    void __fastcall btnWRenameClick(TObject *Sender);
    void __fastcall btnWChkAllClick(TObject *Sender);
    void __fastcall btnWChkNoneClick(TObject *Sender);
    void __fastcall sbVocabClick(TObject *Sender);
    void __fastcall lbWGroupsClick(TObject *Sender);
    void __fastcall lbWGroupsClickCheck(TObject *Sender);
    void __fastcall btnWordDeleteClick(TObject *Sender);
    void __fastcall btnWStudyListClick(TObject *Sender);
    void __fastcall btnWResetClick(TObject *Sender);
    void __fastcall pbKWordsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pbWordsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall cbDictionaryChange(TObject *Sender);
    void __fastcall btnNewDictClick(TObject *Sender);
    void __fastcall btnRenameDictClick(TObject *Sender);
    void __fastcall btnDeleteDictClick(TObject *Sender);
    void __fastcall miCopyKanjiClick(TObject *Sender);
    void __fastcall miAppendKanjiClick(TObject *Sender);
    void __fastcall miCopyKanaClick(TObject *Sender);
    void __fastcall miAppendKanaClick(TObject *Sender);
    void __fastcall miCopyWordClick(TObject *Sender);
    void __fastcall miAppendWordClick(TObject *Sender);
    void __fastcall miToVocabClick(TObject *Sender);
    void __fastcall miEditWordClick(TObject *Sender);
    void __fastcall miDeleteWordClick(TObject *Sender);
    void __fastcall PagesChange(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall btnKAddClick(TObject *Sender);
    void __fastcall btnKRemoveClick(TObject *Sender);
    void __fastcall cbGroupExChange(TObject *Sender);
    void __fastcall btnKCardClick(TObject *Sender);
    void __fastcall pExBarMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pExBarMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall pbGridMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall miCopyAllClick(TObject *Sender);
    void __fastcall miAppendAllClick(TObject *Sender);
    void __fastcall miKanjiInfoClick(TObject *Sender);
    void __fastcall miTranslateKanjiClick(TObject *Sender);
    void __fastcall miKanjiDefClick(TObject *Sender);
    void __fastcall miExportClick(TObject *Sender);
    void __fastcall miImportClick(TObject *Sender);
    void __fastcall pExBarMouseEnter(TObject *Sender);
    void __fastcall pExBarMouseLeave(TObject *Sender);
    void __fastcall pExBarMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnWordUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnKLeftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall miPrintVocabClick(TObject *Sender);
    void __fastcall sbMatchClick(TObject *Sender);
    void __fastcall pbWordsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall cbToGroupChange(TObject *Sender);
    void __fastcall btnToGroupClick(TObject *Sender);
    void __fastcall cbKunGroupClick(TObject *Sender);
    void __fastcall dlgOpenShow(TObject *Sender);
    void __fastcall dlgOpenClose(TObject *Sender);
    void __fastcall sbAddWordMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbKRemoveMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbKRemoveMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbAddWordMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall edKMeaningEnter(TObject *Sender);
    void __fastcall edKMeaningExit(TObject *Sender);
    void __fastcall edKMeaningChange(TObject *Sender);
    void __fastcall btnKSaveClick(TObject *Sender);
    void __fastcall edKMeaningKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbEWordsKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall sbKWordsClick(TObject *Sender);
    void __fastcall mDefinitionChange(TObject *Sender);
    void __fastcall btnKDSaveClick(TObject *Sender);
    void __fastcall mDefinitionEnter(TObject *Sender);
    void __fastcall mDefinitionExit(TObject *Sender);
    void __fastcall btnKDResetClick(TObject *Sender);
    void __fastcall btnKDClearClick(TObject *Sender);
    void __fastcall btnKClearClick(TObject *Sender);
    void __fastcall sbKUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall miTranslateWrittenClick(TObject *Sender);
    void __fastcall miTranslateKanaClick(TObject *Sender);
    void __fastcall sbKMemberClick(TObject *Sender);
    void __fastcall btnKUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall lbKGroupsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall btnKAddStudyClick(TObject *Sender);
    void __fastcall btnWAddStudyClick(TObject *Sender);
    void __fastcall pbKWordsKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall btnKEditStudyClick(TObject *Sender);
    void __fastcall sbAddToReadingClick(TObject *Sender);
    void __fastcall pbGridMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall miGExportClick(TObject *Sender);
    void __fastcall pKBackResize(TObject *Sender);
    void __fastcall miNewWordClick(TObject *Sender);
    void __fastcall miGenerateClick(TObject *Sender);
    void __fastcall pbKFilterKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbKWordsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall miImportWakanClick(TObject *Sender);
    void __fastcall btnWUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnKFilterMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnWFilterClick(TObject *Sender);
    void __fastcall btnWFilterMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pbWFilterKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbWordsKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);

private: // User declarations
    void z_daychange(TMessage &msg);

    TMeaningEditState feditstate;
    TMeaningEditState fdeditstate;

    bool mousesizing;
    int mousey;
    int mousesize;

    bool needexamples;

    int kwsize;

    int infoh; // Height of the status bar for the words list.

    TKanjiCardGrid *kobj;
    TWordListObj *kwobj; // Word list for group example words.
    TVirtualWordListObj *ewobj;
    TReadingBoxObj *kbox;
    TKanjiEdit *edKFilter;
    TKanjiEdit *edWFilter;
    TVocabListObj *wobj; // Word list for word groups.

    TIntegerList *kresults;

    TZMenuRows *gridmenu;
    TZMenuRows *gridmenuextra;
    TZMenuRows *gridmenucombined;

    TZMenuRows *wordmenu;
    TZMenuRows *kwordmenu;

    void popupcallback(TZMenuRows *r, int col, int row);

    void kcallback();
    void kwcallback();
    void kfcallback();
    void wfcallback();
    void bcallback();
    void wcallback();

    void ewcallback();
    int ewindex(unsigned int ix);
    UnicodeString ewmeaning(int ix);
    void ewprepare(int col, int row, TWordColumnType ctype, TCanvas *c);

    //TWordCollection* get_collection();
    TWordCollection *fcoll;

    int get_kanji_group();
    int get_word_group();

    TWordGroupList* get_wordgrouplist();
    TWordGroupList* get_wstudygroups();
    TWordGroupList* get_wgroups();

    TKanjiGroupList* get_kanjigrouplist();

    bool get_modified();
    TWordList* get_words();

    bool can_add_word();
    void color_examples();
    void add_del(bool add);
    void ewcolorize();
    void do_collect_examples();

    void AddCheckedWords(TWordIndexList *l, int selected, bool studygroup);

    void SetMeaningMemoText();

    void FilterKResults();
    void FilterWResults();

    __property TWordGroupList *wlist = { read = get_wordgrouplist }; // Select between wgroups or wstudygroups.
    __property TKanjiGroupList *klist = { read = get_kanjigrouplist }; // "compatibility" with wlist.

    __property TWordGroupList *wgroups = { read = get_wgroups };
    __property TWordGroupList *wstudygroups = { read = get_wstudygroups };

public: // User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_Z_DAYCHANGED, TMessage, z_daychange)
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfGroup(TComponent* Owner);
    virtual __fastcall ~TfGroup();

    bool AddVocabulary(TWordCollection *collection, int index); // Add a word with the given index to the vocabulary.

    void set_info();
    void set_kinfo();
    void DefChanged();

    void collect_examples();

    void FillWList();
    void FillKList();
    void UpdateWList();
    void UpdateKList();
    void UpdateWButtons(); // Update buttons when the listbox updates.
    void UpdateKButtons(); // Update buttons when the listbox updates.

    void GenerateWordGroup();

    void settingsChanged(void *caller, TSettingsProcState state);
    void SaveSettings();
    void LoadSettings();

    void SetSplitSize(int s);

    void SetCaption(); // Sets caption of add kanji to study button.

    void DeselectDictionary();
    void SelectDictionary(byte coll);
    void KanjiGroupAdded(UnicodeString s);
    void WordGroupAdded(UnicodeString s);
    void WordChanged(byte coll, TChangedWordList *list);
    void WordGroupAdded(byte coll, UnicodeString s);

    int AddWordGroup(byte coll, UnicodeString s);
    void WordsToGroup(byte coll, UnicodeString agroup, TWordIndexList *wl);

    int AddKanjiGroup(byte coll, UnicodeString s);
    void KanjiToGroup(byte coll, word group, TKanjiList *kl);

    __property TWordCollection *Collection = { read = fcoll };
    __property int kgroup = { read = get_kanji_group };
    __property int wgroup = { read = get_word_group };
    __property TWordList *words = { read = get_words };
    __property bool modified = { read = get_modified };
};
//---------------------------------------------------------------------------
extern PACKAGE TfGroup *fGroup;
//---------------------------------------------------------------------------
#endif
