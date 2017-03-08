//---------------------------------------------------------------------------

#ifndef repetitionH
#define repetitionH
//---------------------------------------------------------------------------

#include <stdio.h>

class TWordCollection;
enum TRepetitionAnswer { raUncertain, raHard, raGood, raEasy };

// Single test data for an item
struct TRepStat
{
    int day; // Test day of the repetition. this can be different to the real day because a new test day can start anywhere between midnight and noon, and we might still be in the middle of a previous one.
    byte score; // Rounded to 1-100.
    byte deck; // Deck of card after answering the question.
};
typedef TTemplateList<TRepStat, true> TRepStats;

// Number of times a given answer was given for a given item.
struct TRepAnswerCount
{
    byte uncertain;
    byte hard;
    byte good;
    byte easy;
};

// Past test statistics and current data for an item.
struct TRepetitionItem
{
    TRepStats *stats;

    void *pt; // Data identifying the item.

    TDateTime testdate; // Date of the start of the last test.
    TDateTime itemdate; // Exact date and time when the item was tested the last time.
    TRepAnswerCount answers; // Record of last answers given on this item.

    double score; // Current score of item.
    byte deck; // Current deck of item that determines interval.
    word inclusion; // Number of times this item was included in a test and not repeated.
    double interval; // Time till next test, 1.0 = 1 day.

    byte repeats; // Number of times the item got repeated in the last test so far. only 255 repeats are stored and nothing above this is measured.
    byte testdeck; // Deck of item when it was first included in today's test.

    double time; // Sum of all time spent on this item.

    TRepetitionItem *next; // Used for grouping repetition items to avoid spacing two items on the same day. For saving space, there is no other pointer for the first or last or the previous element. Instead all elements are linked in a loop. Check where you started out.
};

struct TRepetitionUndoData
{
    byte deck;
    double score;
    double interval;
    TDateTime date;
};
struct TRepetitionUndoItem
{
    TRepetitionItem *item;
    TRepetitionAnswer answer; // Last answer given to the last answered item.
    bool starter;

    TRepetitionUndoData retry; // Precomputed deck, score, interval for repetition item depending on last answer.
    TRepetitionUndoData hard;
    TRepetitionUndoData good;
    TRepetitionUndoData easy;
};

struct TTimeStatItem
{
    //double averagetime;
    double time[255];
    byte timecount;
};

struct TTimeStat
{
    TTimeStatItem *stat;
    byte alloced;

    //double averagerepeat;
    byte repeat[101]; // 100 + 1 for undo.
    byte repeatcount;

    void removerepeat();

    void addrepeat(byte arepeat);
    void addtime(int repeatix, double atime);
    void grow(int repeatix);

    double averagerepeat() const;
    double averagetime(int repeatix) const;
};

// Added Nov. 2010 for v0.57
struct TDayStat
{
    int day; // Integer part of TDateTime of the day of the stat data.

    double length; // Length of test in the fraction of days.
    int testcount; // Count of items that were tested (not the ones that were added to the test but not tested because there was not enough time for it).
    int testednew; // Items that were newly added to the test at a given day.
    int testfailure; // Number of items that were to be repeated for the first time on a given day.
    int testlearned; // Number of items learned on a given day.

    int itemcount; // Count of all items in the repetition list.
    int groupcount; // Count of groups of items (with their *next either set to NULL or not pointing to any other separate element that was counted).
    int itemlearned; // Number of items that had score and deck measuring as learned on a given day.
};

typedef TTemplateList<TRepetitionItem, true> TRepetitionListBase;
typedef TTemplateList<TRepetitionItem, false> TRepetitionItemList;
class TRepetitionCategory
{
private:
    class TRepetitionList;
    typedef TTemplateList<TRepetitionList> TRepetitionLists;

    class TRepetitionList : public TRepetitionListBase
    {
    private:
        typedef TRepetitionListBase inherited;
        unsigned int fid;

        TDateTime roundstart; // Date and time when testing was started on a day.

        class TTimeStatList : public TTemplateList<TTimeStat, true>
        {
        private:
            typedef TTemplateList<TTimeStat, true> inherited;
            void FreeItem(TTimeStat *item, int ix);

            byte undodeck;

            void GrowDeck(int deck);
        public:
            TTimeStatList();
            virtual ~TTimeStatList();

            void SaveToFile(FILE *f);
            void LoadFromFile(FILE *f, int version);

            // Record the time for the item's starting deck and current repeat (starting = the deck before the first question is answered today).
            void AddTime(byte deck, byte repeat, double spenttime);
            // Record the average repeat count of the item's starting deck.
            void AddAverage(byte deck, byte repeat);

            void Undo(); // Only undo average repeat count, time count stays whatever the result was.

            double estimate(byte deck, byte repeat);
        };
        TTimeStatList *timestats; // Statistics to determine ETA during test and to measure how much time is spent in each deck.

        class TDayStatList : public TTemplateList<TDayStat, true>
        {
        private:
            typedef TTemplateList<TDayStat, true> inherited;
        public:
            TDayStatList();
            void SaveToFile(FILE *f);
            void LoadFromFile(FILE *f, int version);
        };

        TDayStatList *daystats;

        TDayStat* StatToday(TDateTime when);
    protected:
        virtual void FreeItem(TRepetitionItem *item, int ix);
    public:
        TRepetitionList(unsigned int aid);
        virtual ~TRepetitionList();

        void SaveToFile(FILE *f);
        void LoadFromFile(FILE *f, int version);

#ifdef HACK_TEST_REMOVE
        void RemoveLastTest();
#endif

        bool date_passed(int ix, double date);
        int IndexOfData(void *ap);
        using inherited::IndexOf;
        virtual void Delete(int ix);
        virtual void DeleteRange(int ix, int cnt) { THROW(L"Not implemented"); }

        void BeginTest();
        void Step(TRepetitionItem *aitem, TRepetitionUndoItem &undoitem, TRepetitionAnswer answer, bool starter, double timeused);
        void SimulateStep(TRepetitionItem *aitem, TRepetitionAnswer answer, bool starter, double &newinterval);
        void TRepetitionList::Undo(TRepetitionItem *aitem, TRepetitionAnswer oldanswer, TRepetitionAnswer newanswer, const TRepetitionUndoData &newdata, bool starter);

        TRepetitionItem* AddItem(void *pt, TRepetitionItem *groupitem);
        void RemoveItem(void *pt);
        void GroupItems(TRepetitionItem *item1, TRepetitionItem *item2);

        bool AllowChange();

        double deviate_interval(TRepetitionItem *aitem, double interval);

        double timeestimatenew();
        double timeestimate(TRepetitionItem *item);
        double singletimeestimate(byte deck);

        void setdeck(TRepetitionItem *aitem, byte deck, double interval);
        double simulate_setdeck(TRepetitionItem *aitem, byte deck);
        void resetitem(TRepetitionItem *aitem);

        int DayStatCount();
        const TDayStat& DayStat(unsigned int statix);
        TDayStat* LastDayStat();
        void FixDayStats(); // Recounts day statistics for the last day.
        int TimeStatCount();
        const TTimeStat& TimeStat(unsigned int statix);

        __property unsigned int id = { read = fid };

        // Temporary function for loading old style group. added Dec. 2010 for v0.57.
        void swap_daystats(TRepetitionList *rl);

        void fixintervals(); // Must be called when interval computing was changed and we have to count new intervals for every item.
    };

    TRepetitionLists *studies;

    int study_index(unsigned int id);
    int test_index; // Index of currently tested study list.

    TRepetitionUndoItem undoitem;
public:
    TRepetitionCategory();
    virtual ~TRepetitionCategory();

    virtual void SaveToFile(FILE *f);
    virtual void LoadFromFile(FILE *f, int version);

    int new_study(); // Create new study list.
    void delete_study(unsigned int id); // Deletes study list.
    void fetch_all(unsigned int id, TRepetitionItemList *list); // Kept for compatibility with versions 7 or older, superseded in Now. 2010, v0.57.
    TRepetitionListBase* fetch_study(unsigned int id);

    double timeestimatenew(unsigned int id);
    double timeestimate(unsigned int id, TRepetitionItem *item);
    double singletimeestimate(unsigned int id, byte deck);

    void BeginTest(unsigned int id);
    void Step(TRepetitionItem *aitem, TRepetitionAnswer answer, bool starter, double timeused);
    void SimulateStep(TRepetitionItem *aitem, TRepetitionAnswer answer, bool starter, double &newinterval);
    void Undo(TRepetitionAnswer newanswer);

    // Removes all traces of last recorded test data.
#ifdef HACK_TEST_REMOVE
    void RemoveLastTest(unsigned int id);
#endif

    TRepetitionItem* add_item(unsigned int id, void *pt, TRepetitionItem *groupitem = NULL);
    void remove_item(unsigned int id, void *pt);
    void group_items(unsigned int id, TRepetitionItem *item1, TRepetitionItem *item2); // Updates the list by grouping two or more items. Does nothing if item1 and item2 are already in a group (can be reached from each other via the "next" member).

    bool allowchange(unsigned int id); // Returns true if the last statistics is older or same as today's date.

    void setdeck(unsigned int id, TRepetitionItem *aitem, byte deck, double interval);
    double simulate_setdeck(unsigned int id, TRepetitionItem *aitem, byte deck);
    void resetitem(unsigned int id, TRepetitionItem *aitem);

    int daystatcount(unsigned int id);
    const TDayStat& daystat(unsigned int id, unsigned int statix);
    TDayStat* lastdaystat(unsigned int id);
    void fixdaystats(unsigned int id);
    int timestatcount(unsigned int id);
    const TTimeStat& timestat(unsigned int id, unsigned int statix);

    // Functions for undo, generally returning data.
    bool hasundo();
    TRepetitionAnswer undo_answer();
    double undo_goodinterval();
    double undo_easyinterval();

    // Temporary function for loading old style group. added Dec. 2010 for v0.57.
    void copy_daystat(unsigned int newid, unsigned int oldid);

    void fixintervals(unsigned int id); // Must be called when interval computing was changed and we have to count new intervals for every item.

};

struct TRepetitionState
{
    //double test_count;
    double success; // Percent of success [0-100]

    Bits answers;
};

// Object responsible for determining the performance of studied items. it is also a list of multipliers at a given step
class TStudentProfile : public TTemplateList<TRepetitionState, true>
{
private:
    typedef TTemplateList<TRepetitionState, true> inherited;

    TRepetitionState* get_item(int ix);
    void set_item(int ix, TRepetitionState *val);

    byte ffail_rate; // Acceptable percent of forgotten items [5-20], default: 8
    double mul; // Base multiplier of intervals. default: 2.5 (4 for first time)

    double count_interval(double score, char deck);
    int interval_deck(double interval);
    //void set_rate(byte val);
    //void set_mul(double val);
    double scoremul(double score, TRepetitionAnswer answer);
    double success_rate(int step);

    void expand(int ix);
    void limitdeck(TRepetitionAnswer answer, byte &elemdeck);

public:
    TStudentProfile();
    virtual ~TStudentProfile();

    void SaveToFile(FILE *f);
    void LoadFromFile(FILE *f, int version);
    void step(TDateTime testdate, TRepetitionAnswer answer, bool starter, byte &elemdeck, TDateTime &elemdate, double &eleminterval, double &elemscore, bool simulation);

    double deckinterval(double score, char deck);

    __property byte fail_rate = { read = ffail_rate/*, write=set_rate*/ };
    __property double multiplier = { read = mul/*, write=set_mul*/ };

    __property TRepetitionState *Items[int] = { read = get_item };
};

extern TStudentProfile *profile;

int day_diff(TDateTime a, TDateTime b);
int get_day(TDateTime dt);
double good_interval(double initdate, double interval);

#endif
