//---------------------------------------------------------------------------

#ifndef wordgroupstudyH
#define wordgroupstudyH
//---------------------------------------------------------------------------
#include <Math.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "wordgroup.h"

//--UPDATE THE FOLLOWING: collection.cpp "DeleteWordMeaning" method when global word statistics changes!
//--------------------------------------------------------

struct TWordStatGlobal;
enum TWordState;
class TWordGroupStudy;

enum EM1Order { em1Added, em1Backwards, em1Random, em1CNT };
extern const wchar_t *CEM1Order[4];

struct TWordStudyState
{
    word correct;
    word incorrect;
    word chaincorrect;
    bool lastcorrect;
    char score; // -/+100 limit

    // Dummy only used for loading data correctly.
    TDateTime testtime; // The word was last tested at this date and time (the user selected an answer at this time).

    char skip; // # of times/days to skip a word. -1 means to skip it alltogether, the other values are yet to be determined.

    // Negative values are ignored: (used for setting real result in the review).
    char meaningerror; // The last answer to meaning was incorrect.
    char kanjierror; // The last answer to kanji was incorrect.
    char kanaerror; // The last answer to kana was incorrect.

    char startscore; // Score at beginning of test.
    word round; // Round the word was tested last.
};

// List for word statistics:
class TWordStudyStateList : public TTemplateList<TWordStudyState, true>
{
private:
    typedef TTemplateList<TWordStudyState, true> inherited;

    bool get_problematic(int ix);

    TWordList *words;
public:
    TWordStudyStateList(TWordList *awords) : words(awords), inherited()
    {
        ;
    }
    virtual ~TWordStudyStateList()
    {
        Clear();
    }

    void LoadFromFile(FILE *f, byte ver);
    void SaveToFile(FILE *f);

    void Answer(int ix, bool correct, bool meaning, bool kanji, bool kana);
};

//----------------------------------------------------------------------
// Remove when becomes unnecessary to convert from pre v0.731 version. Changed 2013 07 04.
enum TStudyTmpTmp { epMeaning, epKK, epKana, epKanji, epMeaningPerKana, epMeaningPerKanji, epMeaningPerKK, epMeaningKana };
/*
Meaning only
Kana only      k 1
Kanji only     K 2
Both kanji and kana
Both meaning and kana
Randomly selected: meaning OR kana      k 5
Randomly selected: meaning OR kanji     K 6
Randomly selected: meaning OR kanji and kana
Randomly selected: kanji OR kana   k 8 K 8
Randomly selected: meaning OR kanji OR kana     k 9 K 9
*/
enum TStudyMethodType : byte { smGradual, smRefresh, smUndefined = 255 };
enum TStudyQuestionType { sqMeaning, sqKana, sqKanji, sqKK, sqMKana, sqMOrKana, sqMOrKanji, sqMOrKK, sqKOrK, sqMOrKOrK };
enum TStudyAnswerType { saCorrectWrong, sa5Choices, saTypeKana, saTypeKanji, saTypeKK };

typedef TTemplateList<TWordStatGlobal, false> TWordStatGlobalList;

// Base class for study word groups:
class TWordGroupStudy : public TWordGroup
{
private:
    typedef TWordGroup inherited;

    TStudyMethodType fmethod; // Method of study group item

    // Old options for word study groups before v0.731. Removed 2013 07 04
    //TStudyPractice fpractice;
    //bool fchoices; // Do we want 5 choices or none
    //bool ftypekana;
    TStudyQuestionType fquestion;
    TStudyAnswerType fanswer;

    bool fhidekana;
    byte ftimer;

    // False by default; true when started a study but haven't finished it yet.
    // Before use, we have to initialize any important data (like included words).
    bool finited;

    bool fshowmeaning;
    bool fshowkanji;
    bool fshowkana;

protected:
    int fcurrent;
    TIntegerList *fwords; // The order of words appearing in a test. Holds indexes of TWordGroup::list.
    TWordStudyStateList *fanswers; // List of TWordStudyState, each item corresponds to a word in the base flist.
    int fcorrect;
    int fwrong;

    int fround;

    virtual UnicodeString get_wordmeanings(int ix);
    virtual UnicodeString get_wordkanjis(int ix);
    virtual UnicodeString get_wordkanas(int ix);

    virtual int get_currentindex();
    virtual bool DoInit();

    // Never use this constructor, only when loading from file and this one was called from the inherent class.
    TWordGroupStudy(TWordCollection *acollection, UnicodeString aname, TStudyMethodType amethod);
    void FillReviewLists(int first, int last, TWordIndexList *l, TWordStudyStateList *sl, TWordStatGlobalList *gl);
    void RefillReviewLists(int first, int last, TWordStudyStateList *sl);

    void Review(int first, int last, bool save, bool remove, int random);
    void Review(int first, int last, int firstwrong, int lastwrong, bool save, bool remove, int random);

    virtual void Step();
    //virtual void ExportHeader(FILE *f);

    // Functions needed by the study window.
    virtual UnicodeString get_methodname() = 0;

    virtual void CleanUp(); // Remove items with a negative index from the group. Only used on dictionary update!

    bool get_typeanswer(); // Returns whether the current answer is to be typed.
    bool get_choices(); // Whether the selected answer method is 1 out of 5 choices.
public:
    TWordGroupStudy(TWordCollection *acollection, UnicodeString aname, TStudyMethodType amethod, TStudyQuestionType aquestion, TStudyAnswerType aanswer, byte atimer, bool ahidekana);
    virtual ~TWordGroupStudy();

    virtual void DeleteWord(int ix);
    void Init();

    //virtual void Export(FILE *f);

    virtual void Reset();

    virtual void LoadFromFile(FILE *f, byte ver);
    virtual void SaveToFile(FILE *f);

    virtual void NextWord();
    virtual TModalResult Suspend(bool ashown); // Undo what NextWord did.
    virtual bool LastWord(); // Returns true when the round is over, because we've reached the last word.
    virtual bool RoundLastWord()
    {
        return false;
    };

    // Show review form with all the words we need to review. all = all words, otherwise only missed, problematic and new ones.
    virtual void Review(bool change);
    virtual void FinishTest();
    // Save mistakes or success in points too.
    virtual void SaveStats();
    void SaveStatsGlobal(); // Overwrite global statistics.
    virtual void SaveStat(TWordStudyState &stat, bool correct);

    // Returns a possible answer for the current word. write a handler for this.
    // If you know how to get suitable words to fool the user.
    // Returns an index from the local fwords list.
    virtual int GetChoice()
    {
        return fwords->Items[Random(fwords->Count)];
    };
    // Change statistics.
    virtual void Answer(bool correct);

    //__property TStudyPractice practice = { read = get_practice };
    __property TStudyQuestionType question = { read = fquestion };
    __property TStudyAnswerType answertype = { read = fanswer };
    __property int current = { read = fcurrent };
    __property TStudyMethodType method = { read = fmethod };
    __property bool inited = { read = finited };

    // Functions needed by the study window.
    virtual void StartRound(bool firstround) = 0;
    virtual void DrawInfo(TPaintBox *p) = 0;
    // Properties needed by the study window.
    __property bool showmeaning = { read = fshowmeaning };
    __property bool showkanji = { read = fshowkanji };
    __property bool showkana = { read = fshowkana };
    __property bool typeanswer = { read = get_typeanswer }; // Returns whether the current answer is to be typed.
    __property bool choices = { read = get_choices };
    __property bool hidekana = { read = fhidekana };
    __property byte timer = { read = ftimer };
    __property UnicodeString methodname = { read = get_methodname };
    __property int currentindex = { read = get_currentindex };
    __property UnicodeString wordmeanings[int] = { read = get_wordmeanings };
    __property UnicodeString wordkanjis[int] = { read = get_wordkanjis };
    __property UnicodeString wordkanas[int] = { read = get_wordkanas };
};

// Study word group 1 - gradual study
class TWordGroupM1 : public TWordGroupStudy
{
private:
    typedef TWordGroupStudy inherited;

    bool lastanswered; // Used for inner works, don't save!

//--- /settings
    byte finitial; // Number of words to ask at first.
    EM1Order forder; // Order in which the words are shown from the list.
    bool frandomize; // Randomly set up the basic word list, but the presentation still depends on forder.

    byte fwordadd; // Number of words to add in each round.
    byte fselection; // Select words.
    bool fuseglobal; // Use global scores.
//--- settings/

    int fincluded; // Starts out from finitial and grows by adding new words.
    int fpos; // Number of words included from the previous round (so we know what to memorize).
    int fstartpos; // Index where to start the test.

    TIntegerList *fwordstore; // We move the fwords list here so we can give a fake list that can be randomized.
    virtual int get_currentindex();
    int get_lastindex();

    virtual bool DoInit();
protected:
    //virtual void ExportHeader(FILE *f);
    virtual UnicodeString get_methodname()
    {
        return "Word study by gradual inclusion";
    }
    virtual void Step();
public:
    TWordGroupM1(TWordCollection *acollection, UnicodeString aname, TStudyQuestionType aquestion, TStudyAnswerType aanswer, byte atimer, bool ahidekana, byte ainitial, EM1Order aorder, bool arandomize, byte awordadd, byte aselection, bool aglobal);
    TWordGroupM1(TWordCollection *acollection, UnicodeString aname);
    virtual ~TWordGroupM1();

    virtual void Answer(bool correct);

    virtual void Review(bool change);
    virtual void Reset();

    virtual void LoadFromFile(FILE *f, byte ver);
    virtual void SaveToFile(FILE *f);
    void LoadFromFileCustom(FILE *f, byte ver, int ix);
    void SaveToFileCustom(FILE *f, int ix);

    virtual void SaveStat(TWordStudyState &stat, bool correct);

    void StartRound(bool firstround);
    void DrawInfo(TPaintBox *p);
    virtual TModalResult Suspend(bool ashown);
    bool LastWord();
    bool RoundLastWord();

    __property byte initial = { read = finitial };
    __property EM1Order order = { read = forder };
    __property bool randomize = { read = frandomize };
    __property int lastindex = { read = get_lastindex };
    __property byte wordadd = { read = fwordadd };
    __property byte selection = { read = fselection };
    __property bool useglobal = { read = fuseglobal };
};

// Study word group 2 - word refresh
class TWordGroupM2 : public TWordGroupStudy
{
private:
    typedef TWordGroupStudy inherited;

//--- /settings
    int finclusion; // Priority when including new words
    int fnumber; // Number of words to test
//--- settings/

    virtual bool DoInit();
protected:
    virtual UnicodeString get_methodname()
    {
        return "Refresh known words";
    }
    //virtual void ExportHeader(FILE *f);
public:
    TWordGroupM2(TWordCollection *acollection, UnicodeString aname, TStudyQuestionType aquestion, TStudyAnswerType aanswer, byte atimer, bool ahidekana, int ainclusion, int anumber);
    TWordGroupM2(TWordCollection *acollection, UnicodeString aname);
    virtual ~TWordGroupM2();

    virtual void LoadFromFile(FILE *f, byte ver);
    virtual void SaveToFile(FILE *f);
    void LoadFromFileCustom(FILE *f, byte ver, int ix)
    {
    }
    void SaveToFileCustom(FILE *f, int ix)
    {
    }

    void StartRound(bool firstround);
    void DrawInfo(TPaintBox *p);

    __property int inclusion = { read = finclusion };
    __property int number = { read = fnumber };
};

bool WordGroupStudyValid(TStudyQuestionType qtype, TWordIndexList *list, bool showmsg = true); // Returns whether a word group study with the given list of words and question type is valid to start. Invalid study groups are those that not contain enough words or not enough words contain kanji. Set showmsg to false to prevent showing a message box notifying the user of the problem (only when the return value would be false).

#endif
