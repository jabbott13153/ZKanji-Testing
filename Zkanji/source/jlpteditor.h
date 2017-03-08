//---------------------------------------------------------------------------

#ifndef jlpteditorH
#define jlpteditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "baseform.h"
#include <ComCtrls.hpp>
#include "edit2.h"
#include <Menus.hpp>
#include <Dialogs.hpp>

struct TWord;
class TVirtualWordListObj;
class TKanjiEdit;
enum TWordColumnType;

enum TTempJLPTArea : byte { jaFixed, jaFound, jaMatch };
enum TTempJLPTMark : byte { jmNone, jmLevel, jmKanji, jmFixed };

struct TTempJLPTItem
{
    bool kanaonly; // Tick next to kanji.
    bool usekana; // Tick next to kana.

    bool haskanji;
    wchar_t *meaningorig;
    int windex;
    byte Nlevel;
    TTempJLPTArea area;
    bool use;
    byte Nlevel_orig;

    TTempJLPTMark mark;

    TTempJLPTItem *next; // Next item with same windex. No item has NULL value, so once the next reaches the original item the circle is closed.
    wchar_t *definition; // Updated meaning of word. Shared pointer between all items with same windex.
};

class TTempJLPTItemList : public TTemplateList<TTempJLPTItem, true>
{
private:
    typedef TTemplateList<TTempJLPTItem, true> inherited;

    int fcnt; // Number of unique words (independent of items list's count).
    int defcnt; // Number of unique definitions set to words.

    TWord* get_word(unsigned int ix);
    UnicodeString get_area(unsigned int ix);
    void LoadN(int level);

    UnicodeString get_meaning(unsigned int ix);
    UnicodeString get_cmeaning(unsigned int ix);
    void set_meaning(unsigned int ix, UnicodeString val);
    UnicodeString get_dictmeaning(unsigned int ix);
    bool get_own(unsigned int ix);
    bool get_valid(unsigned int ix);
    bool get_npresent(unsigned int ix, byte n);
    bool get_nused(unsigned int ix, byte n);
    int get_nlvl(unsigned int ix);
    int get_klvl(unsigned int ix);

    virtual void Delete(int ix);
    virtual void DeleteRange(int ix, int cnt) { THROW(L"Not implemented"); }
public:
    TTempJLPTItemList();
    virtual ~TTempJLPTItemList();

    void ChangeIndex(unsigned int ix, int windex); // Modifies item's index, removing it from its group of same windex items, creating new group with new windex (only call this when new windex is not in the list already!).
    void AddToBunch(unsigned int ix, int bix); // modifies item, moving it from a group to another. bix is an arbitrary item in the other group.

    void AutoFix(unsigned int ix); // Changes kana usage and adds new elements depending on N level.
    void AutoFixKatakana(unsigned int ix); // Set definition from dictionary for katakana word.
    void AutoDefineWords(unsigned int ix, int max_word_len, int min_def_count); // Set definition from dictionary for katakana word.

    void Preload();
    virtual void Clear();
    void ResetKanji();

    void SaveToFile(UnicodeString filename);
    void LoadFromFile(UnicodeString filename);
    void Export(UnicodeString filename);

    __property int WordCount = { read = fcnt };
    __property int DefinitionCount = { read = defcnt };

    __property TWord *Words[unsigned int ix] = { read = get_word };
    __property UnicodeString Areas[unsigned int ix] = { read = get_area };
    __property UnicodeString Meanings[unsigned int ix] = { read = get_meaning, write = set_meaning };
    __property UnicodeString CollectedMeanings[unsigned int ix] = { read = get_cmeaning };
    __property UnicodeString DictionaryMeanings[unsigned int ix] = { read = get_dictmeaning };
    __property bool OwnMeaning[unsigned int ix] = { read = get_own };
    __property bool ValidItem[unsigned int ix] = { read = get_valid }; // Returns false if same item appears twice on same level or higher level has no kanji while lower one has.

    __property bool NPresent[unsigned int ix][byte N] = { read = get_npresent }; // True if N level present in one item of this word.
    __property bool NUsed[unsigned int ix][byte N] = { read = get_nused }; // True if N level used in one item of this word.
    __property int NLevelCount[unsigned int ix] = { read = get_nlvl }; // Returns number of different N levels in all the items of this word. (eg 2 when word has item in both N5 and N4).
    __property int KanjiLevel[unsigned int ix] = { read = get_klvl }; // Highest level (lowest number) kanji in word. 0 for no kanji in word.
};

class TfrmJLPTEdit : public TBaseForm
{
__published: //IDE-managed Components
    TZPaintBox *pb;
    TBitBtn *btnClose;
    TBevel *b1;
    TStatusBar *sbInfo;
    TBevel *bt;
    TPanel *Panel1;
    TLabel *Label2;
    TSpeedButton *sbN5;
    TSpeedButton *sbN4;
    TSpeedButton *sbN3;
    TSpeedButton *sbN2;
    TSpeedButton *sbN1;
    TLabel *Label3;
    TSpeedButton *sbAreaFixed;
    TSpeedButton *sbAreaFound;
    TSpeedButton *sbAreaMatch;
    TLabel *Label4;
    TZPaintBox *edKanji;
    TBevel *Bevel1;
    TSpeedButton *SpeedButton1;
    TPanel *pButtons;
    TBevel *Bevel2;
    TEdit2 *edMeaning;
    TLabel *lbMeaning;
    TSpeedButton *btnSave;
    TSpeedButton *btnOrig;
    TSpeedButton *btnDict;
    TLabel *Label1;
    TComboBox *cbN;
    TSpeedButton *sbPrev;
    TSpeedButton *sbNext;
    TBevel *Bevel4;
    TMainMenu *main;
    TMenuItem *File1;
    TMenuItem *miOpen;
    TMenuItem *miSaveAs;
    TMenuItem *miSave;
    TOpenDialog *dlgOpen;
    TSaveDialog *dlgSave;
    TSpeedButton *btnDelMeaning;
    TMenuItem *N1;
    TBevel *Bevel3;
    TSpeedButton *btnAlt;
    TPopupMenu *pmAlt;
    TMenuItem *ools1;
    TMenuItem *miAutoN3;
    TPanel *Panel2;
    TLabel *lbN5;
    TLabel *lb5;
    TLabel *lb3;
    TLabel *lb4;
    TLabel *lb2;
    TLabel *lb1;
    TLabel *lbN3;
    TLabel *lbN4;
    TLabel *lbN2;
    TLabel *lbN1;
    TBevel *Bevel5;
    TBevel *Bevel6;
    TBevel *Bevel7;
    TBevel *Bevel8;
    TBevel *Bevel9;
    TCheckBox *cbSum;
    TPanel *pAuto;
    TSpeedButton *btnAuto;
    TLabel *Label10;
    TEdit2 *edKwrd;
    TEdit2 *edKex;
    TEdit2 *edKKwf;
    TEdit2 *edKk;
    TLabel *Label11;
    TLabel *Label12;
    TLabel *Label13;
    TLabel *Label14;
    TLabel *Label15;
    TLabel *Label16;
    TEdit2 *edKwcnt;
    TLabel *Label17;
    TLabel *Label18;
    TEdit2 *edNkj;
    TLabel *Label19;
    TEdit2 *edNkf;
    TLabel *Label20;
    TEdit2 *edNwf;
    TLabel *Label21;
    TEdit2 *edNef;
    TLabel *Label22;
    TEdit2 *edKN3;
    TMenuItem *ools2;
    TMenuItem *AutoFix1;
    TMenuItem *AllN3toN21;
    TCheckBox *cbExclude;
    TPanel *Panel3;
    TLabel *lbK1;
    TLabel *Label28;
    TLabel *Label29;
    TLabel *lbK2;
    TLabel *lbK3;
    TLabel *lbK4;
    TLabel *lbK5;
    TBevel *Bevel10;
    TSpeedButton *btnL;
    TSpeedButton *btnK;
    TMenuItem *Edit1;
    TMenuItem *miL;
    TMenuItem *miK;
    TBevel *Bevel11;
    TPanel *Panel4;
    TLabel *lbDefinition;
    TSpeedButton *btnDefinition;
    TMenuItem *Katakanaautodef1;
    TMenuItem *Autodefshortword1;
    TMenuItem *miGroups;
    TSpeedButton *btnF;
    TMenuItem *miF;
    TLabel *Label5;
    TEdit2 *edNminf;
    TLabel *Label6;
    TEdit2 *edKLen;
    TMenuItem *N2;
    TMenuItem *JumptoSame1;
    TMenuItem *N3;
    TMenuItem *Export1;
    void __fastcall btnCloseClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall sbN5Click(TObject *Sender);
    void __fastcall SpeedButton1Click(TObject *Sender);
    void __fastcall btnSaveClick(TObject *Sender);
    void __fastcall btnOrigClick(TObject *Sender);
    void __fastcall btnDictClick(TObject *Sender);
    void __fastcall edMeaningChange(TObject *Sender);
    void __fastcall edMeaningKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall cbNChange(TObject *Sender);
    void __fastcall sbNextMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbLinkClick(TObject *Sender);
    void __fastcall miSaveClick(TObject *Sender);
    void __fastcall miSaveAsClick(TObject *Sender);
    void __fastcall miOpenClick(TObject *Sender);
    void __fastcall btnDelMeaningClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall edKanjiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall btnAltClick(TObject *Sender);
    void __fastcall miAltClick(TObject *Sender);
    void __fastcall btnAutoClick(TObject *Sender);
    void __fastcall cbSumClick(TObject *Sender);
    void __fastcall miAutoN3Click(TObject *Sender);
    void __fastcall AutoFix1Click(TObject *Sender);
    void __fastcall AllN3toN21Click(TObject *Sender);
    void __fastcall pbKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall btnLClick(TObject *Sender);
    void __fastcall miLClick(TObject *Sender);
    void __fastcall miKClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall btnDefinitionClick(TObject *Sender);
    void __fastcall Katakanaautodef1Click(TObject *Sender);
    void __fastcall Autodefshortword1Click(TObject *Sender);
    void __fastcall miFClick(TObject *Sender);
    void __fastcall JumptoSame1Click(TObject *Sender);
    void __fastcall Export1Click(TObject *Sender);
private: //User declarations
    UnicodeString filename;
    bool fmodified;

    TTempJLPTItemList *items;
    TIntegerList *view;

    TKanjiEdit *edkanji;

    bool freordering;

    TVirtualWordListObj *wobj;

    int NCount[5];

    UnicodeString getheaderproc(int col);
    void headerpressproc(int oldcol);
    void prepareproc(int col, int row, TWordColumnType ctype, TCanvas *c);
    void drawproc(int col, int row, TCanvas *c, const TRect &r);

    int windex(unsigned int ix);
    void wcallback();
    void ecallback();
    void chkcallback(unsigned int row, byte index, bool &checked);

    void Filter();

    int realrow(int row);
    void reorder(int col = -1, bool rev = false);
    void _reorder(int pos);

    void save(UnicodeString file);
    void open(UnicodeString file);
    void updatecaption();
    void updatenlabels();
    void updateklabels();

    void count_ncount();
    void autofix();
    void autofixkatakana();
    void autodefinewords(int max_meaning_length);

    void set_mod(bool val);

    void invalidate_lines(unsigned int windex);
    void focusmeaningbox();

    void clearalt();

    void __fastcall miFilterAClick(TObject *Sender);
    void __fastcall miFilterBClick(TObject *Sender);
    void __fastcall miCountN2KanjiClick(TObject *Sender);
    void __fastcall miMarkN3Click(TObject *Sender);
    void __fastcall miMarkN3KanaClick(TObject *Sender);
    void __fastcall miAddN3WordsClick(TObject *Sender);

    __property bool modified = { read = fmodified, write = set_mod };
public: //User declarations
    __fastcall TfrmJLPTEdit(TComponent* Owner);
    virtual __fastcall ~TfrmJLPTEdit();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmJLPTEdit *frmJLPTEdit;
//---------------------------------------------------------------------------
#endif
