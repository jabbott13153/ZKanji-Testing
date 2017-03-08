//---------------------------------------------------------------------------

#ifndef wordsH
#define wordsH
//---------------------------------------------------------------------------

#include <stdio.h>

#define MAX_EXPORT_LINE_SIZE    4096

//only for reading in words
struct TConvertItem
{
    wchar_t kanji[128];
    wchar_t kana[256];
    int original;
    int changed;
};

struct TWordCollection;
struct TWordStats;
struct TWordFilter;
struct TExampleData;

struct TWordStatGlobal
{
    word included;
    char score; //-/+100 limit
    TDateTime testtime;
};

class TWordList;

struct TMeaningData;
bool MeaningDataMatch(const TMeaningData &a, const TMeaningData &b); // Returns true if the two meaning data exactly match, including the word types.
bool MeaningDataMatch(const TMeaningData &a, const TMeaningData &b, bool &fullmatch);  // Returns true if the meaning in the two meaning data match. Fullmatch is set to false if word types are different.
bool WordTypeMatch(const TMeaningData &a, const TMeaningData &b); // Returns true if the word types of the two meaning data exactly match.
// Dictionary data in a TWord
struct TMeaningData
{
    wchar_t *meaning; // Meaning string for a word
    int types; // See word type defines in zkformats.h
    int notes; // See note defines in zkformats.h
    byte fields; // See word field defines in zkformats.h // changed from int to byte v0.57 Nov. 2010
    word nametag; // See name tag defines in zkformats.h
};

class TWordGroup;
struct TWordGroups
{
    TWordGroup *group; // The group that contains the word.
    char meaning; // Meaning that is in the group. // Changed to char from byte 2013.05.08
    TWordGroups *next; // Used in the linked list.
};
struct TWord
{
    // Changed from int to word v0.57 Nov. 2010
    word frequency; // Word frequency. bigger the better for the word

    wchar_t *kanji;
    wchar_t *kana;
    wchar_t *romaji;

    TMeaningData *data; // Array of meanings and types
    byte meaningcnt; // Number of meanings

    TWordStatGlobal *stats;
    TWordGroups *group; // Linked list of groups this word is in
};

class TWordList : public TTemplateList<TWord, false>
{
private:
    typedef TTemplateList<TWord, false> inherited;
public:
    TWordList();
    TWordList(TWordList *orig);
    virtual ~TWordList()
    {
        Clear();
    }

    void Export(FILE *f);

    //export for mysql
    UnicodeString SQLEscape(wchar_t *c);
    void SQLExport(FILE *f, FILE *f2);
};

struct TInfList
{
    TInfList *next;
    byte inf; // Type of inflection on a word.
};

struct TWordIndex
{
    int index; // Index of word in words list.
    TInfList *inf; // Inflection on word. NULL when not inflected in any way.
    char meaning; // Which meaning is shown in list. -1 when all.
};

class TWordIndexList : public TTemplateList<TWordIndex, true>
{
private:
    typedef TTemplateList<TWordIndex, true> inherited;

    TWordCollection *fcollection; // Which dictionary to use.

    int get_index(int i);
    TWord* get_word(int i);
    wchar_t* get_meaning(int i);
    virtual void FreeItem(TWordIndex *elem, int ix);
public:
    TWordIndexList(TWordCollection *acollection/*, bool avocablist*/);
    TWordIndexList(TWordIndexList *alist);
    virtual ~TWordIndexList()
    {
        Clear();
    };

    int Add(int ix);
    int Add(int ix, byte ameaning);
    int Add(TWordIndex *src);
    void Assign(TWordIndexList *src); // Assign another list to this.
    int AddInflected(int val, char ameaning, TInfList *inf);
    int AddInflected(int val, TInfList *inf);
    void AddInflected(TWordIndexList *src, TInfList *inf); //add the indexes of src with the inflected bit set
    void AddInflected(TIntegerList *src, TInfList *inf); //add the indexes of src with the inflected bit set
    int IndexOf(int ix, char ameaning = -1);

    void KanaSort();
    void KanjiSort();
    void RomajiSort(wchar_t *ch);
    void IndexSort();

    __property int Indexes[int i] = { read = get_index };
    __property TWord* Words[int i] = { read = get_word };
    __property wchar_t* Meanings[int i] = { read = get_meaning }; //only works for vocabulary list. returns NULL otherwise
    __property TWordCollection *collection = { read = fcollection };
};

//--------------------------------------------------------------
//-------- structures and methods for inflection search --------
//--------------------------------------------------------------
struct TWordInf
{
    wchar_t *str;
    TInfList *inf;
    byte type; // Type of word.
};

class TWordInfList : public TTemplateList<TWordInf, true>
{
private:
    typedef TTemplateList<TWordInf, true> inherited;
    void FreeItem(TWordInf *item, int ix);
public:
    virtual ~TWordInfList()
    {
        Clear();
    }
    TInfList* Add(UnicodeString s, byte type, TInfList *inf, byte inftype);
};

// Deletes the character arrays and other data in a word structure, and deletes the word itself.
void delete_word(TWord *&w);
void delete_word_meanings(int count, TMeaningData* &data);

// Deinflection of words, and list type for its use
void DeInflect(TWordInfList *l, UnicodeString s, int &type, TInfList *inf = NULL); //returns all the forms of s while changing the type

extern wchar_t cdelim[]; // Array of characters used as delimiters in the dictionary when searching for latin character definitions. Generated for the current language locale on startup.

#endif
