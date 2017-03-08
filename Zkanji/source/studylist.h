//---------------------------------------------------------------------------

#ifndef studylistH
#define studylistH
//---------------------------------------------------------------------------

#include <stdio.h>
#include "kanji.h"
#include "searchtree.h"

/*
  Long-term study test data and method: (documentation in progress)

  Basic structures used in the long-term study:

  (class) TWordStudyDefinition : unique element for each word added to the
      long-term study list. This is a shared item between items of the same
      word, holding the redefined meaning of words and the last inclusion in a
      test. It also has a usage count and a byte holdling bits for the type of
      items.

  struct TWordStudyItem : base structure for all items added to the long-term
      study list. Only derived types are used. (TFreeWordStudyItem and
      TLockedWordStudyItem). It has a pointer to a TWordStudyDefinition to
      know which word the item is for. Every TWordStudyItems have a type for
      the word part (written, kana and meaning).

  struct TWordStudyList::TFreeWordStudyItem : Derived from TWordStudyItem.
      Those items that are added to the long-term study but haven't been
      selected into a test yet are in the form of this structure. It has a
      priority value apart from the inherited data.

  struct TWordStudyList::TLockedWordStudyItem : Derived from TWordStudyItem.
      Structure for items that have been in a test at least once. Holds a
      TRepetitionItem item, that is a transparent type used in the spaced
      repetition algorithm.


  Class and structures used in the running test:

  TTestElemList* testlist : Only exists while the test is running. It contains
      TTestElem items. Although it is a list, it also manages every feature of
      the test, returning the next item to test, giving information about it,
      the number of items due etc.
  struct TTestElem* (in testlist) : As items come in groups of 3 (written, kana,
      meaning), this can hold up to 3 TTestedItem items of the same word, and the
      turn of the last item asked.
  struct TTestedItem* ('item' in TTestElem) : holds a TWordStudyItem* for the
      tested word, a TTestIndexFrom to tell whether this is a new item, an item
      that last failed or a simple item due that wasn't asked in the current
      test.

  * None of these are saved when the program saves the data file.

*/

struct TRepStat;
struct TRepAnswerCount;
struct TDayStat;
class TRepetitionCategory;
struct TRepetitionItem;
typedef TTemplateList<TRepetitionItem> TRepetitionItemList;
class TWordCollection;
struct TStudyKItem;
class TKanjiStudyList;
struct TConvertItem;

struct TStudyItem; //-- to be removed after some versions! (first added v0.57 2010.11.03)

enum TQuestionType;
enum TStudyType;
enum TRepetitionAnswer;
enum TDateFormatSetting;
enum TQuestionType { qtKanji, qtKana, qtMeaning, qtMaxValue };
enum THintType { htKanji, htKana, htMeaning, htDefault, htMaxValue };

class TWordStudyList;

// Structure of words that were added to the long-term study list.
struct TWordStudyDefinition
{
    int fwordindex; // Word index in dictionary.
    wchar_t *fmeaning; // Current meaning given for the study item..
    byte ftakentype; // Bits of TQuestionType for each item that is in the long-term study.
    TDateTime flastinclude; // Date and time this item was last included in a test.

    TRepetitionItem *repgroup; // Holds a single repetition item to be used for grouping items in the repetition list.
};

// Structure of items that were added to the long-term study list. Each word can have 3 of this for written, kana and meaning.
struct TWordStudyItem
{
    TDateTime added; // Time this item was added to the long-term study.
    TWordStudyDefinition *def;
    TQuestionType type;

    THintType hinttype;
};

// Main class to handle all long-term study list related functions.
class TWordStudyList : public TTemplateList<TWordStudyDefinition, true>
{
private:
    typedef TTemplateList<TWordStudyDefinition, true> inherited;

    // not used after version 17!
    // item and list for word definitions that were removed from an updated dictionary
    struct TChangeItem
    {
        wchar_t *kanji;
        wchar_t *kana;
        int index;
    };
    typedef TGapList<TChangeItem> TChangeGapList;
    TChangeGapList* changegap;
  //--------------------------
protected:
    virtual void FreeItem(TWordStudyDefinition *item, int ix);
public:
    UnicodeString definition_text(TWordStudyDefinition *def);

    void changemeaning(TWordStudyDefinition *def, const UnicodeString &meaning);
    void changemeaning(int defix, const UnicodeString &meaning);
    bool hastype(TWordStudyDefinition *def, TQuestionType qtype);
    bool hastype(int defix, TQuestionType qtype);

    const wchar_t* wordkanji(TWordStudyDefinition *def);
    const wchar_t* wordkana(TWordStudyDefinition *def);
    const wchar_t* wordromaji(TWordStudyDefinition *def);
    const wchar_t* wordmeaning(TWordStudyDefinition *def);
    int wordindex(TWordStudyDefinition *def);
    TDateTime wordinclude(TWordStudyDefinition *def);
    TWord* wordfromdef(TWordStudyDefinition *def);

    const wchar_t* wordkanji(int defix);
    const wchar_t* wordkana(int defix);
    const wchar_t* wordromaji(int defix);
    const wchar_t* wordmeaning(int defix);
    int wordindex(int defix);
    TDateTime wordinclude(int defix);
    byte wordusage(int defix);
    TWord* wordfromindex(int defix);
private:

    // Type of items not yet included in the test, but added to the long-term study.
    struct TFreeWordStudyItem : public TWordStudyItem
    {
        byte priority; // Number between 1 and 10.
    };

    // Type of items that were included in a test already.
    struct TLockedWordStudyItem : public TWordStudyItem
    {
        TRepetitionItem *ritem; // Spaced repetition data, which is handled elsewhere.
    };

    TLockedWordStudyItem *undoitem; // Item used for undoing the last change.

    // Item and list for storing readings in tested words for the kanji reading test (Added: 2011.03.06 for v0.575)
    class TTestReadingList
    {
        struct TTestReadingItem
        {
            struct TTestReadingWord
            {
                int itemindex; // Locked index of item in the owner.
                byte pos; // Position of kanji in the word.

                bool newword;
                bool failed;
            };
            typedef TTemplateList<TTestReadingWord, true> TTestReadingWords;
            TTestReadingWords *words;

            int kanjiindex;
            byte reading;
        };
        typedef TTemplateList<TTestReadingItem, true> TTestReadings;
        TTestReadings *testlist; // Items to be tested.

        struct TTestReadingUndoData
        {
            TTestReadingItem::TTestReadingWord *word;
            bool failed;
        };
        typedef TTemplateList<TTestReadingUndoData, true> TReadingsUndoList;
        TReadingsUndoList *undolist;

        struct TTestedReadingItem
        {
            int kanjiindex;
            byte reading;
            int itemindex;
        };
        typedef TTemplateList<TTestedReadingItem, true> TTestedReadings;
        TTestedReadings *tested; // Items already tested. these items won't appear in readings test again but also won't be added with add_item twice.

        TWordStudyList *owner;
        bool due(int kanjiix);
        int testedpos(TTestedReadingItem *item);

        TTestReadingItem::TTestReadingWord* currentword(int wix); // Maps index of separate word of kanji to item in current item's word list.

        friend int __sort_tested_proc(TTestedReadingItem **a, TTestedReadingItem **b);
    public:
        TTestReadingList(TWordStudyList *aowner);
        virtual ~TTestReadingList();

        void SaveToFile(FILE *f);
        void LoadFromFile(FILE *f, int version);

        void clear();

        void add_item(int itemindex, bool newword, bool failed); // Inserts new TTestReading item by the given locked itemindex from owner. Sorts when inserting.
        void remove_item(int itemindex);
        void undo(bool failed);
        void resetundo();

        int firstdue(); // Returns index of first item that is due according to current settings.
        bool test(); // Starts the readings study test for readings due by the current profile. returns true when finished normally, false when test was terminated by the user.

        void next(); // Moves latest tested item to the tested list.
        int duecount(); // Number of kanji readings to be tested.

        int wordcount(); // Number of words for the current kanji and reading.
        int currentindex(int wix); // Returns the locked index of the ix'th example word for the current kanji/reading.
        int currentkanjipos(int wix); // Returns the position of the current kanji/reading in the given word example for the kanji/reading.
        int currentkanjiindex();
        int currentreading();
        bool currentwordfailed(int wix);
        bool currentwordnew(int wix);
    };

    enum TWordStudyItemType { witNone, witFree, witLocked, witMaxValue = witLocked };
    class TItemTree : public TTextSearchTreeBase
    {
    private:
        typedef TTextSearchTreeBase inherited;

        TWordStudyList *fowner;
        TWordStudyItemType ftype;

        typedef TTemplateList<TWordStudyItem, true> TTList;
        TTList *list;

        int get_count();
        TWordStudyItem* get_item(int ix);

        void FindWord(const wchar_t *c, TIntegerList *results);
        void FindKanjiWord(const UnicodeString &c, TIntegerList *results);

        virtual UnicodeString DoGetWord(int index);
    protected:
        __property TWordStudyList *owner = { read = fowner };
    public:
        TItemTree(TWordStudyList *aowner, TWordStudyItemType atype);
        virtual ~TItemTree();

        int Add(TWordStudyItem *item);
        void Delete(int ix);
        int IndexOf(TWordStudyItem *item);

        void Filter(const UnicodeString &str, TIntegerList *results);

        void SaveToFile(FILE *f);
        void LoadFromFile(FILE *f, int version);

        __property int Count = { read = get_count };
        __property TWordStudyItem* Items[int] = { read = get_item };
    };

    enum TTestIndexFrom { tifNew, tifDue, tifFailed };
    struct TTestedItem
    {
        double score; // A temporary score that is counted before every round in a test to determine order of questions.
        TTestIndexFrom from; // The list of the worditem. tifNew - freelist, tifDue - lockedlist, tifFailed - failedlist.
        TWordStudyItem *worditem;
        bool untested; // The current item wasn't asked before in the current test.
    };
    struct TTestElem
    {
        TTestedItem item[3];
        byte itemcount;

        word testindex; // The index'th question when one of the items was last tested.
    };

    class TTestElemList : protected TTemplateList<TTestElem, true>
    {
    private:
        typedef TTemplateList<TTestElem, true> inherited;

        struct TTestedItemIndex
        {
            TTestElem *elem;
            byte item;
        };
        typedef TTemplateList<TTestedItemIndex, true> TTestedItemIndexList;
        TTestedItemIndexList *newitems;

        struct TTestedUndoItem
        {
            TTestedItem item; // Original data of item before change.
            TRepetitionAnswer answer; // Last given answer that must be changed.
            TTestElem *elem; // Elem of the item to return this one. NULL when elem was deleted.
            int index; // Index of item in elem, -1 if this item was deleted from elem or if elem was deleted.
        };
        TTestedUndoItem undoitem;

        byte testitemindex; // Temporary variable holding index of the TTestedItem that was last tested. Only used when there are very few items in the study list and most of them are failed items. No need to save!

        TRepetitionItem *dummy; // For items in the freelist, that not yet have a repetition item. returned in repetitionitem().
        TWordStudyList *fowner;
        int fitemcount;
        int ffailedcount;

        int fstep; // Number of questions already asked during the current test.

        int listpos(int windex);
        TTestElem* createelem(TTestIndexFrom from, TWordStudyItem *sitem, bool untested);
        int insertelem(TTestElem* elem);

        int wordindex(TWordStudyItem *sitem);
        int wordindex(TTestElem *elem);

        byte questionitemindex();

        int get_newcount();

    public:
        TTestElemList(TWordStudyList *aowner);
        virtual ~TTestElemList();

        int addelem(TTestIndexFrom from, TWordStudyItem *sitem, bool untested);
        int indexofelem(TWordStudyItem *sitem);

        void startround();
        TWordStudyItem* questionword();
        TTestedItem& questionitem();
        TRepetitionItem* repetitionitem();
        TTestIndexFrom questionfrom();
        double testestimate();

        void answer(TRepetitionAnswer answer, TWordStudyItem *w);
        void undo(TRepetitionAnswer newanswer);

        void Clear();

        __property Count;

        __property int ItemCount = { read = fitemcount };
        __property int NewCount = { read = get_newcount };
        __property int FailedCount = { read = ffailedcount };
    };

    TWordCollection *fcollection;

    bool fstudying;

    typedef TTemplateList<TLockedWordStudyItem, false> TLockedWordStudyItemList;

    TRepetitionCategory *category;
    unsigned int fstudyid;
    int flasttest; // Day when last test was taken. computed with get_day for taking user preferences into account.
    int flastinclude; // Number of items that were included in the last test.

    TItemTree *freelist;
    TItemTree *lockedlist;

    TDateTime itemstart; // Time of previous startround call used to measure time spent on single items. This time is updated when the test is paused then resumed.
    TTestReadingList *testreadings;

    int newcount; // Number of new items to test. decremented each time a new item is tested.

    // Lists used in the tests for ordering:
    TLockedWordStudyItemList *duelist; // Items from lockedlist in the order of their date+interval.
    TLockedWordStudyItemList *failedlist; // Items in lockedlist that were answered incorrectly the last time they were tested.
    TLockedWordStudyItemList *newlist; // Items that were new when the test started, but have been added to the locked list after an answer.

    TTestElemList *testlist; // List used only during a test for the items due.

    unsigned int get_studyid();

    void setdefinition(TWordStudyItem *item, TWordStudyDefinition *def, bool locked);
    void unsetdefinition(TWordStudyItem *item, bool locked, bool deleteempty); // Set locked to true when deleting locked item. Set deleteempty to true to allow deletion of definition in case no items refer to this definition. (deleteempty is used when only temporarily deleting an item to move it to a different list.)

    int get_freecount();
    int get_freeindex(int freeix);
    TQuestionType get_freetype(int freeix);
    THintType get_freehint(int freeix);
    void set_freehint(int freeix, THintType val);
    UnicodeString get_freekanji(int freeix);
    UnicodeString get_freekana(int freeix);
    UnicodeString get_freemeaning(int freeix);
    void set_freemeaning(int freeix, UnicodeString val);
    TDateTime get_freeadded(int freeix);
    byte get_freepriority(int freeix);
    void set_freepriority(int freeix, byte val);
    UnicodeString get_freedeftext(int freeix);

    int get_lockedcount();
    int get_lockedindex(int lockix);
    TQuestionType get_lockedtype(int lockix);
    THintType get_lockedhint(int lockix);
    void set_lockedhint(int freeix, THintType val);
    UnicodeString get_lockedkanji(int lockix);
    UnicodeString get_lockedkana(int lockix);
    UnicodeString get_lockedmeaning(int lockix);
    void set_lockedmeaning(int lockix, UnicodeString val);
    TDateTime get_lockedadded(int lockix);
    byte get_loldscore(int ix);
    byte get_lscore(int ix);
    byte get_lolddeck(int ix);
    byte get_ldeck(int ix);
    word get_ltries(int ix);
    TDateTime get_lfirst(int ix);
    TDateTime get_llasttest(int ix);
    TDateTime get_llasttime(int ix);
    int get_llastday(int ix);
    const TRepAnswerCount& get_lanswers(int ix);
    TDateTime get_lnext(int ix);
    double get_linterval(int ix);
    UnicodeString get_lockeddeftext(int lockix);
    int get_lockedstatcount(int lockix);
    const TRepStat& get_lockedstat(int lockix, int statix);

    int duepos(TDateTime due); // Return the position of a new item that is due at the given datetime:
    int dueindex(TLockedWordStudyItem *item);

    int get_testdue();
    int get_faileddue();
    int get_testnew();
    UnicodeString get_testkanji();
    UnicodeString get_testkana();
    UnicodeString get_testmeaning();
    int get_testwordindex();
    bool get_testalladded();

    void generate_testitems();

    __property unsigned int studyid = { read = get_studyid };

    friend TItemTree;
    friend TTestElemList;
    friend int wordstudy_duelist_sort_proc(TLockedWordStudyItem **a, TLockedWordStudyItem **b);
    friend int wordstudy_testlist_sort_proc(TTestElem **a, TTestElem **b);

    // Statistics:
    unsigned int get_timestatcount(); // Number of statistics data.

    unsigned int get_statcount(); // Number of statistics data.
    const TDayStat& get_daystat(int ix); // Statistics for a given day.

    unsigned int get_timestat(); // Number of statistics data.
    double get_timeaverage(int deckix);
    double get_timeretries(int deckix);

    // Readings study:
    int get_rdue();
    int get_rwordcount();
    wchar_t get_rchar();
    UnicodeString get_rwordmeaning(int ix);
    int get_rwordindex(int ix);
    byte get_rcharpos(int ix);
    TWord* get_rword(int ix);
    TKanji* get_rkanji();
    byte get_rreading();
    bool get_rwordfailed(int ix);
    bool get_rwordnew(int ix);

    // Undo:
    bool get_hasundo();
    TRepetitionAnswer get_lastanswer();
    double get_undogoodint();
    double get_undoeasyint();

    void real_delete_locked(int ix);

    void CleanUp(); // Only used during dictionary update to remove items with negative index.
    friend class TWordStateList;
public:
    TWordStudyList(TWordCollection *acoll);
    virtual ~TWordStudyList();

    void SaveToFile(FILE *f);
    void LoadFromFile(TConvertItem *citem, int ccnt, FILE *f, int version);

    void RemoveLine(int windex);

    int Add(int windex, const wchar_t *meaning); // Add new word definition to the Long-term study list.
    int additem(int defindex, TQuestionType type, bool haskanji); // Add new item to the LTSL. defindex is index of previously added definition. type is new item question. haskanji must be set to false if no kanji was added to the list as question.

    // Set deletedef to true to allow deletion of definition if no items refer to it. Used mainly for determining whether we are only moving an item between lists instead of deleting it normally.
    void delete_free(int ix, bool deletedef);
    void delete_locked(int ix, bool deletedef);
    void queue_locked(int ix);

    UnicodeString generate_meaning(int windex); // Returns a string with the original meaning of a word.
    UnicodeString generate_types(int windex); // Returns a string with all the word types found in the given word.

    void filter_free(UnicodeString str, TIntegerList *l);
    void filter_locked(UnicodeString str, TIntegerList *l);

    void startround();
    void suspenditem();
    void resumeitem();

    TModalResult begintest();
    void finishtest();
    void readingstest();

    void answertest(TRepetitionAnswer answer);

    TQuestionType testquestion();
    TQuestionType testanswer();
    bool newquestion();
    bool failedquestion();
    bool firstquestion();
    double testinterval(TRepetitionAnswer answer);
    double testestimate(); // Return ETA for current test.
    UnicodeString testwordtype();

    bool allowchange(); // Returns true if the last statistics is older or same as today's date.

    double get_deckinterval(int ix, byte deck); // Interval of item by locked index if deck is changed.
    void set_deckinterval(int ix, byte deck, double interval); // Only valid if interval is a value got via get_deckinterval for the same item.

    void resetitem(int ix); // Removes study stats for item and resets its last test day to today, giving it a single stat.

    int wordcount();
    int kanjicount();
    int itemsdue(); // Total of due items in the long-term study list, including anything due tomorrow.
    //int currentdue(); // Number of items due today. returns 0 when the test finished with all due words for the day

    int firsttest();
    int lasttest();
    int testdays();
    int skippeddays();

    double overalltime();
    double testaverage();
    double answeraverage();

    // We assume that this is only called when we are not during a test.
    void daychanged(); // Clears the newlist.

    // For statistics:
    void deckcounts(TGapList<int> &l); // Number of items in a given deck.
    void trycounts(TGapList<int> &l); // Number of items that were tested for a given time.
    void duecounts(TGapList<int> &l); // List of the number of items due on a day.
    //void decktimeavg(TGapList<double> &l); // Average time spent on a deck.
    //void decktryavg(TGapList<int> &l); // Average number of retries in a deck.

    void next_reading();

    void undo(TRepetitionAnswer answer); // Undo last change in test. also sets a new answer.

//---------------------------------------------------------------------------
// To be removed with everything unused below! (first added v0.57 2010.11.03)
//---------------------------------------------------------------------------
    int addstudyitem(TStudyItem *sitem);
    void finishimport(unsigned int fid);
//---------------------------------------------------------------------------

#ifdef HACK_TEST_REMOVE
    void RemoveLastTest();
#endif

    __property TWordCollection *collection = { read = fcollection };

    // Properties for the free (not tested) items:
    __property int freecount = { read = get_freecount };
    __property int freeindex[int] = { read = get_freeindex };
    __property TQuestionType freetype[int] = { read = get_freetype };
    __property THintType freehint[int] = { read = get_freehint, write = set_freehint };
    __property UnicodeString freekanji[int] = { read = get_freekanji };
    __property UnicodeString freekana[int] = { read = get_freekana };
    __property UnicodeString freemeaning[int] = { read = get_freemeaning, write = set_freemeaning };
    __property TDateTime freeadded[int] = { read = get_freeadded };
    __property byte freepriority[int] = { read = get_freepriority, write = set_freepriority };
    __property UnicodeString freedeftext[int] = { read = get_freedeftext };

    // Properties for the locked (studied) items:
    __property int lockedcount = { read = get_lockedcount };
    __property int lockedindex[int] = { read = get_lockedindex };
    __property TQuestionType lockedtype[int] = { read = get_lockedtype };
    __property THintType lockedhint[int] = { read = get_lockedhint, write = set_lockedhint };
    __property UnicodeString lockedkanji[int] = { read = get_lockedkanji };
    __property UnicodeString lockedkana[int] = { read = get_lockedkana };
    __property UnicodeString lockedmeaning[int] = { read = get_lockedmeaning, write = set_lockedmeaning };
    __property TDateTime lockedadded[int] = { read = get_lockedadded };
    __property byte lockedoldscore[int] = { read = get_loldscore };
    __property byte lockedscore[int] = { read = get_lscore };
    __property byte lockedolddeck[int] = { read = get_lolddeck };
    __property byte lockeddeck[int] = { read = get_ldeck };
    __property word lockedtries[int] = { read = get_ltries };
    __property TDateTime lockedfirst[int] = { read = get_lfirst };
    __property TDateTime lockedlasttest[int] = { read = get_llasttest }; // Date and time when the last test started that included this item.
    __property TDateTime lockedlasttime[int] = { read = get_llasttime }; // Date and time when this item was last answered.
    __property int lockedlastday[int] = { read = get_llastday };
    __property const TRepAnswerCount& lockedlastanswers[int] = { read = get_lanswers };
    __property TDateTime lockednext[int] = { read = get_lnext };
    __property double lockedinterval[int] = { read = get_linterval };
    __property UnicodeString lockeddeftext[int] = { read = get_lockeddeftext };
    __property int lockedstatcount[int] = { read = get_lockedstatcount };
    __property const TRepStat& lockedstat[int][int] = { read = get_lockedstat };

    // Properties for the current test items.
    __property int testdue = { read = get_testdue };
    __property int testnew = { read = get_testnew };
    __property int faileddue = { read = get_faileddue };
    __property UnicodeString testkanji = { read = get_testkanji };
    __property UnicodeString testkana = { read = get_testkana };
    __property UnicodeString testmeaning = { read = get_testmeaning };
    __property int testwordindex = { read = get_testwordindex };
    __property bool testalladded = { read = get_testalladded };

    // For statistics:
    __property unsigned int statcount = { read = get_statcount }; // Number of statistics data.
    __property const TDayStat& daystat[int] = { read = get_daystat }; // Statistics for a given day.

    __property unsigned int timestatcount = { read = get_timestatcount }; // Number of statistics data.
    __property double timeaverage[int] = { read = get_timeaverage }; // Average time spent on a single item from the given deck.
    __property double timeretries[int] = { read = get_timeretries }; // Average number of repeats of a deck's item.

    // Properties for the readings study:
    __property int readingsdue = { read = get_rdue };
    __property wchar_t readingscharacter = { read = get_rchar };
    __property int readingsword_count = { read = get_rwordcount };
    __property UnicodeString readingsword_meaning[int index] = { read = get_rwordmeaning };
    __property int readingsword_index[int index] = { read = get_rwordindex };
    __property byte readingscharacter_pos[int index] = { read = get_rcharpos };
    __property bool readingswordfailed[int index] = { read = get_rwordfailed };
    __property bool readingswordnew[int index] = { read = get_rwordnew };
    __property TWord* readingsword[int index] = { read = get_rword };
    __property TKanji* readingskanji = { read = get_rkanji };
    __property byte readingsreading = { read = get_rreading };

    // Properties for UNDO:
    __property bool hasundo = { read = get_hasundo };
    __property TRepetitionAnswer lastanswer = { read = get_lastanswer };
    __property double undo_goodinterval = { read = get_undogoodint };
    __property double undo_easyinterval = { read = get_undoeasyint };
};

UnicodeString generate_datestring(int date, int dateNow, TDateFormatSetting format);
UnicodeString generate_intervalstring(double ival, double showdays = 60.0);
UnicodeString generate_duestring(TDateTime date, TDateTime dueNow);
//UnicodeString generate_datetimestring(TDateTime date, TDateTime dateNow);

//---------------------------------------------------------
//---------------------------------------------------------
//------ Everything below is for group version 7 or less!!!
//---------------------------------------------------------
//---------------------------------------------------------

enum TStudyType { sdKanjiToKana, sdKanaToKanji, sdToMeaning };

struct TStudyWItem
{
    int wordindex;
    char *meaning;

    word used; // Number of times this word is added to the study list.

    struct TItem
    {
        TStudyKItem *kitem;
        byte reading; // ON or KUN reading or none.
    };
    class TItems : public TTemplateList<TItem, true>
    {
    public:
        virtual ~TItems()
        {
            Clear();
        }

        int IndexOf(TStudyKItem *aitem);
    };
    TItems *kanjis;
};
struct TStudyKItem
{
    int kanjiindex;
    struct TItem
    {
        TStudyWItem *witem;
    };
    class TItems : public TTemplateList<TItem, true>
    {
    public:
        virtual ~TItems()
        {
            Clear();
        }

        int IndexOf(TStudyWItem *aitem);
    };
    TItems *words;
};

struct TStudyItem
{
    TRepetitionItem *ritem;

    TStudyWItem *item;
    TStudyType direction;
};

struct TTestItem
{
    int instances;

    byte mistakes;
    bool included;
    int rounds;

    TStudyItem *test;
};

class TStudyList
{
private:
    class TStudyItemList;

    //ftest - storing current test state
    class TTestItemList : public TTemplateList<TTestItem, true>
    {
    private:
        typedef TTemplateList<TTestItem, true> inherited;
        TStudyList *owner;

        bool frepeat;
        int fcurrent;
        int ffirsts;

        TStudyItem* get_current();
        TStudyWItem* get_words(int ix);
        void Insert(int apos, TTestItem *aitem);

        int get_testcount();
        int get_testcurrent();

        virtual void FreeItem(TTestItem *item, int ix);
        TStudyItem* get_item(int ix);

        TIntegerList *baselist;
        int get_basecount();
        int get_baseindex(int ix);
        bool get_firsttest(int ix);
        TTestItem* get_baseitem(int ix);

        virtual int Add(TTestItem *obj)
        {
            return inherited::Add(obj);
        }

        friend TStudyItemList;
    public:
        TTestItemList(TStudyList *aowner);
        virtual ~TTestItemList();

        void LoadFromFile(FILE *f, TRepetitionItemList *plist, bool suspended, int version);
        void SaveToFile(FILE *f, TStudyItemList *slist, bool suspended);

        void Clear();

        // Duplicates items with the given windex if they have an index below maxindex.
        void AddDouble(int windex, int maxindex);

        int Add(TRepetitionItem *fitem);
        void BeginUpdate();
        void EndUpdate();

        bool RemoveLine(int windex);
        virtual void Delete(int ix);
        virtual void DeleteRange(int ix, int cnt) { THROW(L"Not implemented"); }
        void Reset();

        void Next();
        void Prev();
        void Answer(bool correct);

        __property bool repeat = { read = frepeat };

        __property int basecount = { read = get_basecount };
        __property int baseindex[int ix] = { read = get_baseindex };
        __property TTestItem* baseitems[int ix] = { read = get_baseitem };
        __property int testcount = { read = get_testcount };
        __property int testcurrent = { read = get_testcurrent };

        __property int current = { read = fcurrent };

        __property bool firsttests[int ix] = { read = get_firsttest };
        __property TStudyItem *currentitem = { read = get_current };
        __property TStudyItem *tests[int ix] = { read = get_item };
        __property TStudyWItem *words[int ix] = { read = get_words };
    };

    class TStudyWordList;
    // fstudy - storing tested words and all results
    class TStudyItemList : public TTemplateList<TStudyItem, true>
    {
    private:
        typedef TTemplateList<TStudyItem, true> inherited;
        TStudyList *owner;
        TTestItemList *ftest;
        unsigned int studyid;

        int lastcount; // Number of items when last test finished.

        virtual void FreeItem(TStudyItem *item, int ix);

        void AddWord(TStudyWItem *witem, bool categorize);

        TRepetitionCategory* get_category();

        __property TRepetitionCategory *category = { read = get_category };

        friend TStudyList;
    public:
        TStudyItemList(TStudyList *aowner, TTestItemList *atest);
        virtual ~TStudyItemList();

        void Reset();

        void SaveToFile(FILE *f, TStudyWordList *wlist, bool suspended);
        void LoadFromFile(FILE *f, TStudyWordList *wlist, bool suspended, int version);

        bool RemoveLine(int windex);
        virtual void Delete(int i);
        void Delete(TStudyWItem *aitem);
        int Add(TStudyKItem *aitem); // Normal add.

        void BeginTest();
        void FinishTest();
        void AbortTest();
        void Revert();

        void Suspend(); // Suspend ongoing test.
        void Resume(); // Continue testing after suspended.

        void Fetch();

    };

    // wlist - storing all words
    class TStudyWordList
    {
    private:
        class TStudyWItems : public TTemplateList<TStudyWItem, true>
        {
        private:
            typedef TTemplateList<TStudyWItem, true> inherited;
            virtual void FreeItem(TStudyWItem *item, int ix);
        public:
            virtual ~TStudyWItems()
            {
                Clear();
            }

            int IndexOf(TStudyWItem *witem);
            int IndexOf(int word_index);
            int Add(TStudyWItem *aitem);
            int Add(int word_index, char *ameaning);
            int IndexOfKanji(int ix, int kanji_index);
            int AddKanji(int ix, TStudyKItem *aitem, byte areading);
            void DeleteKanji(int ix, TStudyKItem *aitem);
        };

        TStudyList *owner;

        TStudyWItems *witems;
        UnicodeString get_meaning(int word_index);
        void set_meaning(int word_index, UnicodeString val);
        TStudyWItem* get_item(int ix);
        int get_count();
    public:
        TStudyWordList(TStudyList *aowner);
        virtual ~TStudyWordList();

        void Assign(TTemplateList<TStudyWItem> *aobj);

        int IndexOf(TStudyWItem *witem);
        int IndexOf(int word_index);
        int AddItem(int word_index, char *ameaning);

        int IndexOfKanji(int ix, int kanji_index);
        int AddKanji(int ix, TStudyKItem *aitem, byte areading);
        void DeleteKanji(int ix, TStudyKItem *aitem);
        void Remove(int windex);
        bool RemoveLine(int windex);

        __property UnicodeString meaning[int word_index] = { read = get_meaning, write = set_meaning };
        __property TStudyWItem *Items[int ix] = { read = get_item };

        __property int Count = { read = get_count };
    };

    // klist - storing all kanji
    class TStudyKanjiList : public TKanjiList
    {
    protected:
    private:
        class TStudyKItems : public TTemplateList<TStudyKItem, true>
        {
        private:
            typedef TTemplateList<TStudyKItem, true> inherited;
            virtual void FreeItem(TStudyKItem *item, int ix);
        public:
            virtual ~TStudyKItems()
            {
                Clear();
            }

            int Add(TStudyKItem *aitem);
            int Add(int aindex);
            int AddWord(int ix, TStudyWItem *aitem/*, byte areading*/);
            void Exchange(int a, int b);
        };

        TStudyList *owner;
        TStudyKItems *kitems;

        int get_wordcount(int ix);
        TStudyKItem* get_item(int ix);
    protected:
    // = TKanjiList ==================================
    //virtual T-List* get_list();           //
        virtual int get_count()
        {
            return kitems ? kitems->Count : 0;
        }
        virtual void set_capacity(int cap)
        {
            kitems->Capacity = cap;
        }
        virtual int get_capacity()
        {
            return kitems->Capacity;
        }
        virtual TKanji* get_kanji(int ix); //
    //end TKanjiList ================================
    public:
        TStudyKanjiList(TStudyList *aowner);
        virtual ~TStudyKanjiList();

        void Assign(TTemplateList<TStudyKItem> *aobj);

        void ClearUp();
        virtual void Delete(int i); // = TKanjiList =

        int IndexOf(TStudyKItem *kitem);
        int IndexOf(wchar_t kanji_char);
        int RightIndexOf(int kanji_index);
        int AddItem(int aindex);
        int AddWord(int ix, TStudyWItem *aitem/*, byte areading*/);
        void Exchange(int a, int b);

        __property int wordcount[int ix] = { read = get_wordcount };
        __property TStudyKItem *Items[int ix] = { read = get_item };
    };
private:
    TWordCollection *fcoll;
    int fpos; //number of kanji included in tests so far

    // * variables for statistics - days must be saved separately because the user can change the settings when a day starts
    int ffirstday;
    int flastday;
    int flastrepday;
    TDateTime flasttest;
    TDateTime ffirsttest;
    TDateTime flastrepetition;
    int ftest_count; //number of tests so far

    //-- END variables for statistics

    int fround; //test round - used during test
    int lastpos; //backup for fpos - used during test
    int startday; //save day when last test started to know if it should be aborted after suspended the previous day
    TKanjiStudyList *kstudylist; //list of kanji with tested readings. NULL when test is not running and not suspended.
    bool ftesting;

    TWordCollection* get_coll();
    TKanjiList* get_kanjilist();

    TStudyItemList *fstudy;
    TTestItemList *ftest;

    TStudyKanjiList *klist;
    TStudyWordList *wlist;

    UnicodeString get_meaning(int word_index);
    void set_meaning(int word_index, UnicodeString val);
    //TOldStepStatList* get_statlist();

    bool get_wordintest(int windex);
    int get_kwordcount(int kix);
    int get_kwordindex(int kix, int wix);
    UnicodeString get_kwordmeaning(int kix, int wix);
    UnicodeString get_kwordreading(int kix, int wix);

    int get_testwcount();
    TTestItem* get_testwitem(int ix);

    TRepetitionItem* get_repitem(int ix);
    int get_repitemcount();

    int ShowReview(bool scoring, bool kanjiselected);
    void Abort();
    //int DoStudy();

    bool is_suspended(bool force_abort);
public:
    TStudyList(TWordCollection *acoll);
    virtual ~TStudyList();

    void LoadFromFile(FILE *f, int version);
    void Convert(TWordStudyList *ws);

    UnicodeString generate_meaning(int word_index);

    void ResetStats();

    int Add(int kindex);

    void Exchange(int a, int b);

    void DeleteKanji(int ix);

    void ItemUpdate(TTestItem *aitem, byte &adeck, TDateTime &adate, double &amininterval, double &amaxinterval, double &ascore);

    __property int pos = { read = fpos };
    __property TKanjiList* kanjilist = { read = get_kanjilist };

    __property bool word_in_test[int word_index] = { read = get_wordintest };
    __property UnicodeString meaning[int word_index] = { read = get_meaning, write = set_meaning }; //meaning of given word
    __property TWordCollection *collection = { read = fcoll };

    __property int round = { read = fround };

  //listing of words for a kanji

    __property int kanjiwordcount[int kanjipos] = { read = get_kwordcount };
    __property int kanjiwordindex[int kanjipos][int wordpos] = { read = get_kwordindex };
    __property UnicodeString kanjiwordmeaning[int kanjipos][int wordpos] = { read = get_kwordmeaning };
    __property UnicodeString kanjiwordreading[int kanjipos][int wordpos] = { read = get_kwordreading };

  //listing of elements in current test
    __property int testwordcount = { read = get_testwcount };
    __property TTestItem *testworditem[int] = { read = get_testwitem };
};

#endif
