//---------------------------------------------------------------------------

#ifndef kanjiH
#define kanjiH
//---------------------------------------------------------------------------

#define EXAMPLE_SEL_MAX   10

class TKanjiList;
struct TWordCollection;
struct TTWordTemp;

enum TKanjiSortType { stUnsorted, stJouyou, stNJLPT, stFrequency, stWords, stWordFrequency, stStrokes, stRadical, stUnicode, stJLPTold };

enum TKanjiDataItem
{
        kdiJouyou, kdiJLPTold, kdiSnH, kdiKnK, kdiNelson, kdiHalpern, kdiHeisig, kdiGakken, kdiHenshall,
        kdiSKIP, kdi4C, kdiUnicode, kdiJIS, kdiSJIS, kdiEUC, kdiFrequency, kdiKuten, kdiNJLPT
};

struct TKanji
{
    word index; // Index of kanji in "kanjis" list.

    // "numeric" data.
    wchar_t ch; //  the kanji itself.
    word _jis; // jis lookup.
    word _uni; // U  unicode lookup.
    byte radical; // B  radical of the kanji.
    byte jouyou; // G  jouyou grade.
    byte JLPT; // JLPT level.

    byte NJLPT; // Guessed N kanji level of the JLPT from 2010.

    byte strokes; // S  stroke number for the kanji.
    word frequency; // F  frequency number from 1 to 2501, lower the better.
    short element; // Element number in the kanji stroke order database.

    word knk; // IN  index in "Kanji and kana" for lookup.
    byte SKIP1; // P  first part of skip code.
    byte SKIP2; // Second part of skip code.
    byte SKIP3; // Third part of skip code.
    word nelson; // N  Modern Reader's Japanese-English Character Dictionary.
    word halpern; // H   New Japanese-English Character Dictionary.
    word heisig; // L   Remembering The Kanji.
    word gakken; // K  Gakken Kanji Dictionary - A New Dictionary of Kanji Usage.
    word henshall; // E  A Guide To Remembering Japanese Characters.
    char SnH[8]; // I  Spahn & Hadamitsky dictionary.
    char c4[5]; // Q  Four-Corner code.
    char c4b;

    int word_freq; // Sum of the frequency of all words this kanji is in.

    // Character arrays
    wchar_t **on; // Array for on readings.
    byte oncnt;
    wchar_t **kun; // Array for kun readings.
    byte kuncnt;
    wchar_t **irreg; // Array for irregular readings.
    byte irregcnt;

    wchar_t *meaning;

    wchar_t *similar; // Length of similar is sim1+sim2.
    int sim1;
    int sim2;
};

class TKanjiList
{
private:
    UnicodeString get_string();
protected:
    //virtual T-List* get_list() = 0;
    virtual TKanji* get_kanji(int ix) = 0;
    virtual int get_count() = 0; //{ return list ? list->Count : 0; };
    virtual void set_capacity(int cap) = 0; //{ list->Capacity = cap; }
    virtual int get_capacity() = 0; //{ return list->Capacity; }

    //__property T-List *list = { read=get_list };
public:
    TKanjiList();
    virtual ~TKanjiList();

    virtual void Clear();
    virtual void Delete(int i) = 0;

    int IndexOf(int kindex);

    __property int Count = { read = get_count };
    __property UnicodeString KanjiString = { read = get_string };
    __property int Capacity = { read = get_capacity, write = set_capacity };
    __property TKanji *Kanjis[int ix] = { read = get_kanji };
};

struct TKanjiItem
{
    TKanji *kanji;
    int instances;
};

class TKanjiItemList : public TKanjiList
{
private:
    typedef TTemplateList<TKanjiItem> TKanjiItems;

    TKanjiItems *flist;
    bool fduplicate;

    //virtual TKanjiItems* get_list();
    TKanji* get_kanji(int ix);
    TKanjiItem* get_item(int i);
protected:
    __property TKanjiItem *Items[int i] = { read = get_item };
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
    TKanjiItemList(bool allow_duplicate = true);
    virtual ~TKanjiItemList();

    void Assign(TKanjiList *alist);
    void SortBy(TKanjiSortType i);
    using TKanjiList::IndexOf;
    int IndexOf(const wchar_t c);

    virtual void Add(UnicodeString str);
    virtual int Add(int kindex);
    virtual int Add(wchar_t wchar);
    virtual int Add(TKanji *k);
    int AddNew(TKanji* fkanji);
    virtual void Delete(int i);
};

extern word *partlist[252]; // Array containing the kanjis with the first index'th part. first elem is always the count in the array.
extern TKanjiItemList *kanjis; // List of every kanji recognized by the program. See also: validkanji[].
extern wchar_t cdelim[]; // Array of characters used as delimiters in the dictionary when searching for latin character definitions. Generated for the current language locale on startup.

int compact_count(TKanji *k); // Returns count of readings excluding double kun readings with.
int real_index(TKanji *k, int compactix); // Returns real index in compact kun reading list that has no double kun readings.
int first_index(TKanji *k, int realix); // First kun or other reading that is not a duplicate.
int compact_index(TKanji *k, int realix); // Index in compact list counted from real index.

word JIStoShiftJIS(word jis); // Converts JIS X 0208 index to Shift-JIS.
word JIStoEUC(word jis); // Converts JIS X 0208 index to EUC-JP.
UnicodeString JIStoKuten(word jis); // Converts JIS X 0208 to its Kuten code point.

#endif
