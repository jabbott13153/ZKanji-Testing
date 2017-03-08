//---------------------------------------------------------------------------

#ifndef kanjicardH
#define kanjicardH
//---------------------------------------------------------------------------

#include "kanji.h"

struct TKanji;
class TExampleList;
struct TConvertItem;
class TKanjiCard;
class TWordIndexList;

struct TKanjiCardExample
{
    byte reading; // Reading of kanji (from ON or KUN)
    int ix; // Index of word.
};
typedef TTemplateList<TKanjiCardExample, false> TKanjiCardExamples;

struct TKanjiCardItem
{
    TKanjiCardExamples *examples;
    TKanji *kanji;
};
typedef TTemplateList<TKanjiCardItem, false> TKanjiCardItems;

class TKanjiCard
{
private:
    struct TExItem
    {
        TExItem *next;
        TKanjiCardExample *example;
        TKanjiCardItems *addedon;
    };
    friend int cmpexampleitemreading(TExItem *a, TExItem *b);

    TKanji *fkanji;

    Bits fcheck;
    byte oncnt;
    byte kuncnt;
    byte realkuncnt;

    TExItem *firstex;
    int fexcount;
    word *fexcounts; // Count of examples for each reading.

    void set_checked(byte ix, bool val);
    bool get_checked(byte ix);

    TKanjiCardExample* get_example(int);
    word get_examplecount(byte reading);
    int get_readingexample(byte reading, word example);
    bool get_examplesel(int ix);
    void set_examplesel(int ix, bool val);

    void CleanUp(); // Only used during dictionary update to remove items with a negative word index.
    friend class TWordStateList;
public:
    TKanjiCard(TKanji *k);
    virtual ~TKanjiCard();

    // Returns true when the card contains useful data (eg. to save).
    bool HasData();

    void SaveToFile(FILE *f);
    TIntegerList* LoadFromFile(FILE *f, int version, TKanji *k, TConvertItem *ci, int cnt, /* remove after v17 */ TWordCollection *collection);

    int IndexOf(TKanjiCardExample *card);
    void AddExample(byte reading, int wix, bool duplicatethrows = true);
    bool RemoveExample(int wix); // Returns true if an item was removed.
    void RemoveLine(int wix); // wix word was removed from the dictionary, remove it from card examples
    void SwapExamples(byte reading, int wix1, int wix2);
    void SelectExample(TKanjiCardItem *item, int wix);
    void DeselectExample(TKanjiCardItem *item, int wix);
    bool ExampleSelected(TKanjiCardItem *item, int wix);

    void FillWordList(TWordIndexList *il, byte reading);

    __property TKanji *kanji = { read = fkanji };
    __property bool Checked[byte] = { read = get_checked, write = set_checked };
    __property int ExampleCount = { read = fexcount };
    __property TKanjiCardExample* Examples[int ix] = { read = get_example };

    __property word ReadingExampleCount[byte reading] = { read = get_examplecount };
    __property int ReadingExample[byte reading][word ix] = { read = get_readingexample };
};

class TKanjiCardList : public TKanjiList
{
private:
    virtual TKanji* get_kanji(int ix)
    {
        if (!Items[ix])
            return NULL;
        return Items[ix]->kanji;
    }

    virtual TKanjiCardItem* get_item(int i)
    {
        return flist->Items[i];
    }
    TWordCollection *fcollection;

    TKanjiCardItems *flist;
    //virtual TKanjiCardItems* get_list() { return flist; }
protected:
    virtual int get_count()
    {
        return flist ? flist->Count : 0;
    }
    virtual void set_capacity(int cap)
    {
        flist->Capacity = cap;
    }
    virtual int get_capacity()
    {
        return flist->Capacity;
    }
public:
    TKanjiCardList(TWordCollection *acoll);
    virtual ~TKanjiCardList();

    virtual void SortBy(TKanjiSortType i);

    void AddExample(int local_index, int wix);
    void RemoveExample(int local_index, int wix);
    bool IsExample(int kix, int wix);
    virtual int Add(int kindex);
    virtual int Add(wchar_t wchar);
    virtual int Add(TKanji *k);
    virtual int Add(TKanjiCardItem *item);
    virtual void Delete(int i);
    void Exchange(int i, int j);

    void RemoveLine(int ix);

    void LoadFromFile(FILE *f, int version, TGapList<TIntegerList*> *exampleconvert);
    void SaveToFile(FILE *f);

    __property TKanjiCardItem *Items[int i] = { read = get_item };
    __property TWordCollection *collection = { read = fcollection };
};

#endif
