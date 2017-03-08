//---------------------------------------------------------------------------

#ifndef genworddataH
#define genworddataH
//---------------------------------------------------------------------------

#include "searchtree.h"

struct TExampleData;

// Word data shared among dictionaries.
struct TStoredWord
{
    wchar_t *kanji;
    wchar_t *kana;

    byte jlptnlevel; // 0 when no jlpt level specified.
    word examplecnt;
    TExampleData *examples;
};

// Used in furigana step.
class TWordStoreWords : public TIntegerList
{
private:
    typedef TIntegerList inherited;

    wchar_t* get_kanji(int ix);
    wchar_t* get_kana(int ix);
protected:
    TStoredWord* get_item(int ix);
public:
    TWordStoreWords();

    __property TStoredWord* Items[int index] = { read = get_item };
    __property wchar_t* Kanji[int index] = { read = get_kanji };
    __property wchar_t* Kana[int index] = { read = get_kana };
};

// List of words with shared data in a tree for fast look-up.
class TWordStore : public TTextSearchTreeBase
{
private:
    typedef TTextSearchTreeBase inherited;

    class TStoreList : public TTemplateList<TStoredWord, true>
    {
    private:
        typedef TTemplateList<TStoredWord, true> inherited;
        TWordStore *owner;

        bool InsertIndex(wchar_t *kanji, wchar_t *kana, int limit, int &index); // Sets index to position of data if inserted with kanji and kana. Returns false if such item is already inserted.
    protected:
        virtual void FreeItem(TStoredWord *item, int ix);
    public:
        TStoreList(TWordStore *owner);
        virtual ~TStoreList();

        int Add(wchar_t *kanji, wchar_t *kana, bool sort);
        int Add(TStoredWord *item); // Warning: this Add is never sorted and never checks for duplicates. Sort afterwards if the data is not sorted originally.
        //using inherited::Add;

        int IndexOf(wchar_t *kanji, wchar_t *kana, int limit);
        //TStoredWord* GetWord(wchar_t *kanji, wchar_t *kana);
        //void Sort();
    };

    TStoreList *list;

    TWordCollection *coll;

    int get_cap();
    void set_cap(int cap);
    int count();
    TStoredWord* get_item(int ix);

    int get_index(int ix);
    TWord* get_word(int ix);
    wchar_t* get_kanji(int ix);
    wchar_t* get_kana(int ix);

    void reexpand();
protected:
    virtual UnicodeString DoGetWord(int index);
    virtual void Clear();
public:
    TWordStore();
    virtual ~TWordStore();

    void SaveToFile(FILE *f);
    void LoadFromFile(FILE *f);

    void ClearJLPTData(); // Called to remove any data of jlpt levels before a jlpt level generation. Removes the entire item if it has no example data either.
    void ClearExampleData(); // Called to remove any data of examples before an example import. Removes the entire item if it has no JLPT data either.
    int Add(wchar_t *kanji, wchar_t *kana, bool sort); // Adds a word to the list. If sorted is false, the list must be sorted with Sort() after finished expanding, and before that a limit must be specified when using FindWord, as the new words are added at the end of the list unsorted. Only add sorted if the list was sorted with Sort() or all previous adds were sorted as well.

    void Fill(wchar_t *search, TIntegerList *l); // Fills 'l' with local indexes that match the search string. (The search string should be passed romanized.)
    TStoredWord* FindWord(wchar_t *kanji, wchar_t *kana, int findlimit = -1); // Returns word data for the kanji and kana. Set findlimit to a number less than or equal to Count to prevent including potentially unsorted members at the end of a freshly expanded list. When calling Add with sorted set to false, Sort must be called specifically or a limit used for finding correct words below the index.
    TStoredWord* FindWord(TWord *w, int findlimit = -1);

    void Sort();

    __property int Count = { read = count };
    __property TStoredWord* Items[int index] = { read = get_item };
    __property int Capacity = { read = get_cap, write = set_cap };
    __property TWordCollection *Collection = { read = coll, write = coll };
    __property int Indexes[int ix] = { read = get_index };
    __property TWord* Words[int ix] = { read = get_word };
    __property wchar_t* Kanji[int ix] = { read = get_kanji };
    __property wchar_t* Kana[int ix] = { read = get_kana };
};

extern TWordStore *store;

#endif
