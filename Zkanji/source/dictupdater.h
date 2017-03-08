//---------------------------------------------------------------------------

#ifndef dictupdaterH
#define dictupdaterH
//---------------------------------------------------------------------------
#include "baseform.h"
#include "ZPaintBox.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
#include "edit2.h"
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>

struct TWordCollection;
class TVirtualWordListObj;
class TWordListObj;
class TVocabListObj;
class TKanjiEdit;
class TWordIndexList;
class TOriginalWordsList;
struct TOriginalWord;

struct TWordState
{
    int windex; // Index of word in dictionary.
    int newindex; // Index of word in updated dictionary. Its value is -1 for items that will be deleted or while their position is not decided yet (i.e. they were not in the new dictionary).
    bool matched; // The word in the new dictionary fully matches the entry in the old data.

    //TStateAction: Action to take when the user finished selecting the update options.
    // * saUpdate: The new index and meanings were selected by the user.
    // * saCopyOld: The user wants to use the old entry, overwriting the new one if it exists and creating a new one if not.
    // * saRemove: (Default) All data of this word should be removed from groups and tests.
    enum TStateAction { saUpdate, saCopyOld, saRemove };
    TStateAction action;

    int origix; // Index in the Originals of the dictionary.

    // Groups containing the word's meanings:
    struct TMeaningGroup // Meaning and group pair to list all meanings that appear in groups.
    {
        bool study; // This data is for study groups.

        int group; // Group the word is in.
        int pos; // Position of word in group.
        byte meaning; // Meaning of word in the group at the given position.
        short newmeaning; // Meaning in the updated dictionary or a negative number if this meaning should be removed with its data from groups.
    };
    int groupcnt; // Number of times a meaning is contained in groups.
    TMeaningGroup *groups; // Meaning and word group pairs.

    // Kanji containing the word as example.
    int kanjicnt; // Number of kanji that has this word as their example.
    struct TKanjiExample
    {
        int kanji; // Index of kanji containing this word as an example.
        int pos; // Index for the example in the kanji.
    };
    TKanjiExample *kanji;

    // Long-term study
    int longtermix; // If 0 or positive, this is an index of the word's meaning in the long-term study list. Negative number means it is not in the LT.

    // Has a redefined meaning for kanji cards.
    bool meaningredefined;
};

enum TWordStateExpand { wseWordGroup, wseStudyGroup, wseKanji, wseDictChange, wseLongTerm, wseMeaningDef, wseOriginal };
class TWordStateList : public TTemplateList<TWordState, true>
{
private:
    typedef TTemplateList<TWordState, true> inherited;

    TWordCollection *coll; // The source dictionary, set in Build().
    TWordCollection *dest; // The destination dictionary, set in Convert().

    TOriginalWordsList *orig;

    /* When defining new indexes for words, the indexes can be above the count of the dictionary's
     * original word count. In that case new words are added to it. This variable defines the number
     * of words added above the existing ones, to be able to make a new word index. */

    TWord* get_word(int ix);
protected:
    virtual void FreeItem(TWordState *item, int ix);
    int SortCompare(int a, int b);

    int FindPosition(int wordix);
public:
    TWordStateList();
    virtual ~TWordStateList();

    // Add a word to the list setting all its properties to zero and set the corresponding counter or data.
    int Add(int wordix, TWordStateExpand exp, int data);
    void Allocate();
    void Build(TWordCollection *acoll, TOriginalWordsList *aorig);

    void AddGroup(int windex, int group, int pos, byte meaning, bool study);
    void AddKanji(int windex, int kindex, int eindex);

    bool Convert(TGapList<int> &changes, TWordCollection *dest, UnicodeString updatetext, UnicodeString aborttext); // Checks if there are different definitions in the original and the destination dictionary. Shows a dialog to match old and new words, and updates their data in this list, but not in the dictionaries. Updatetext is the text to show in a message box before opening the window for conversion. If empty, no message is shown. Aborttext is shown when pressing the abort button.

    void Commit(); // Updates the destination dictionary to reflect changes selected by the user in Convert(). Swaps word related data so after the Commit(), the original dictionary will be updated and the destination can be deleted.

    __property TWord* Words[int index] = { read = get_word };
    __property TWordCollection *Collection = { read = coll }; // The words being checked belong to this dictionary.
    __property TWordCollection *Destination = { read = dest }; // List containing words in the destination dictionary.
};

struct TWordReplacement
{
    int stateindex; // Index of replacement data in TWordStateList *states of the form.
    int newindex; // Index of word in the new dictionary.

    enum TReplacementState { rsUntouched, rsProgress, rsDone };
    TReplacementState editstate;

    enum TReplacementAction { raNormal, raCopyOld, raRemove };
    TReplacementAction action; // Use a copy of the original word in the new dictionary, replacing the word with the same kanji and kana if it exists.

    // Holds replacement data for meanings. Only those meanings have such that are in some group.
    struct TMeaningReplacement
    {
        byte meaning; // Index of meaning of word in the original dictionary (by stateindex).
        byte newmeaning; // Index of meaning in the new dictionary.
    };

    int meaningcnt;
    TMeaningReplacement *meanings;
};

class TfDictUpdater : public TBaseForm
{
__published: // IDE-managed Components
    TZPaintBox *wbox;
    TLabel *Label1;
    TBevel *b1;
    TLabel *Label5;
    TLabel *Label7;
    TButton *btnAbort;
    TBitBtn *btnNext;
    TLabel *lbLongTerm;
    TLabel *lbEx;
    TSpeedButton *sbCopy;
    TBevel *b8;
    TLabel *lbCopy;
    TBevel *b0;
    TImageList *ilState;
    TSpeedButton *sbRemove;
    TPanel *pSelector;
    TSpeedButton *sbReplacement;
    TSpeedButton *sbMeanings;
    TPanel *pMeanings;
    TLabel *Label9;
    TLabel *Label4;
    TZPaintBox *pbMDest;
    TZPaintBox *pbMSrc;
    TLabel *Label2;
    TLabel *Label8;
    TLabel *lbGroups;
    TLabel *Label3;
    TBevel *b4;
    TBevel *b5;
    TBevel *b6;
    TBevel *b7;
    TPanel *pReplacement;
    TLabel *lbOrig;
    TPanel *Panel1;
    TBevel *Bevel3;
    TSpeedButton *btnRomaji;
    TSpeedButton *btnKanji;
    TLabel *Label6;
    TPanel *pRomaji;
    TEdit2 *edRomaji;
    TZPaintBox *pbDict;
    TPanel *pKanji;
    TSpeedButton *sbRecognize;
    TZPaintBox *pbKanji;
    TButton *btnSelect;
    TBevel *b10;
    TBevel *b9;
    TBevel *b2;
    TBevel *b3;
    void __fastcall btnKanjiClick(TObject *Sender);
    void __fastcall pcSelectorChanging(TObject *Sender, bool &AllowChange);
    void __fastcall btnSelectClick(TObject *Sender);
    void __fastcall sbCopyClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall sbRemoveClick(TObject *Sender);
    void __fastcall btnNextClick(TObject *Sender);
    void __fastcall btnAbortClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall sbReplacementClick(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall pbKanjiKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall pbKanjiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbKanjiEnter(TObject *Sender);

private: // User declarations
    TWordStateList *states;
    UnicodeString aborttext;

    class TWordReplacementList : public TTemplateList<TWordReplacement, true>
    {
    private:
        typedef TTemplateList<TWordReplacement, true> inherited;
    protected:
        virtual void FreeItem(TWordReplacement *item, int ix);
    public:
        virtual ~TWordReplacementList()
        {
            Clear();
        }
        int Add(int stateix, int newix);
        void AddMeanings(int ix, TIntegerList *meanings);
        void SetNewMeaning(int ix, byte meaning, byte newmeaning);
        void UnsetAll(int ix); // Sets 255 for every meaning, just like calling SetnewMeaning with 255 for newmeaning.

        int MeaningCount(int ix); // Number of meanings specified in the replacement list. Not in any of the dictionaries.
        bool MeaningSet(int ix, byte meaning); // Same as NewMeaning(ix, meaning) == 255.
        byte OldMeaning(int ix, byte meaning); // Returns the index of the old meaning in the old dictionary for the word at ix. Here "meaning" is only the index in the replacement list, not in any of the dictionaries.
        byte NewMeaning(int ix, byte meaning); // Returns the index of the old meaning in the new dictionary for the word at ix. Here "meaning" is only the index in the replacement list, not in any of the dictionaries.
        bool NewMeaningTaken(int ix, byte newmeaning); // Returns whether the passed new meaning index is specified as a replacement of any of the original meanings.
    };

    TWordReplacementList *words;
    TVirtualWordListObj *wobj;
    TWordListObj *dobj;

    TVocabListObj *msrc; // List of meanings appearing in groups in the source dictionary.
    TVocabListObj *mdest; // List of available meanings in the new dictionary.

    TWordIndexList *listsrc;
    TWordIndexList *listdest;

    TKanjiEdit *edkanji;

    bool usercall; // False when some callbacks are called only because state of a button changed as an already edited word was selected, and not when something was really clicked.

    void ShowPage(int page);

    void wcallback();
    void dcallback();
    void ecallback();
    void mcallbacksrc();
    void mcallbackdest();
    int windexproc(unsigned int ix);
    void wcelldraw(int col, int row, TCanvas *c, const TRect &r);
    void UpdateWords();
    void UpdateMeanings();
    void UpdateDisplay();
    void MatchMeanings(int wordsix); // Match every meaning index in word groups from the old dictionary with a meaning index in the new dictionary, trying to find exact matches if possible. The given meaning will be selected by default in the update dialog, and the user can change it to anything if necessary.
    void UpdateNextText();
public: // User declarations
    __fastcall TfDictUpdater(TComponent* Owner);
    virtual __fastcall ~TfDictUpdater();

    int ShowModalEx(TWordStateList *astates);
    __property UnicodeString AbortText = { read = aborttext, write = aborttext };
};

// ---------------------------------------------------------------------------
extern PACKAGE TfDictUpdater *fDictUpdater;
// ---------------------------------------------------------------------------

#endif
