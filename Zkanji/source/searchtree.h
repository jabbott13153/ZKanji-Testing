//---------------------------------------------------------------------------

#ifndef searchtreeH
#define searchtreeH
//---------------------------------------------------------------------------

#include <stdio.h>

#define NODEFULLCOUNT    300
#define NODETOOMUCHCOUNT 5000

struct TTextNode;
class TWordList;
class TKanjiList;
class TWordCollection;
class TWordIndexList;
struct TWord;
class TWordFilterSelection;

class TTextNodeList : public TTemplateList<TTextNode, true>
{
private:
    typedef TTemplateList<TTextNode, true> inherited;

    TTextNode* get_item(int ix);
    TTextNode *fcontainer; // The container for this list.

    virtual void FreeItem(TTextNode *item, int i);
public:
    TTextNodeList(TTextNode *fcontainer);
    TTextNodeList(TTextNodeList *orig, TTextNode *fcontainer);
    virtual ~TTextNodeList();

    void Sort();
    int Add(TTextNode *node);

    TTextNode* Add(const wchar_t*);
    //virtual void __fastcall Clear();
    //void __fastcall Delete(int i);
    TTextNode* RemoveNode(int i); // Remove and return a node without destroying it.

    TTextNode* nodesearch(const wchar_t *c);
    TTextNode* SearchContainer(const wchar_t *c); // Returns the nearest container of c.
    int RemoveLine(int line, bool linedeleted); // Remove all lines from the sub node and return the number of removed lines.

    //__property TTextNode *Items[int i] = { read = get_item };
    __property TTextNode *container = { read = fcontainer };
};

struct TTextNode
{
    TTextNode *parent;
    wchar_t *label;
    TIntegerList *lines; // Indexes for word list.
    TTextNodeList *nodes; // Nodes inside this one in the tree structure.
    int contained; // Number of words in this and the contained nodes.
};

// Structure holding furigana data. Holds kana indices that are the furigana of one or more kanji.
// kanji - position of kanji, kanjilen - number of kanji from that position.
// kana - position of kana, kanalen - number of kana from that position.
struct TFuriganaData
{
    byte kanji;
    byte kanjilen;
    byte kana;
    byte kanalen;
};

class TTextSearchTreeBase
{
protected:
    enum TWalkType { wtPre, wtPost }; // No inorder for this n-tree.
    typedef void(__closure * TFuncTreeCallback)(TTextNode *, void*);
private:
    TTextNodeList *fnodes; // Nodes for a,b,...,z etc.
    TTextNode *fselected;

    bool hasbase; // Has nodes a to z on top of the nodes list.
    bool blacklisting; // Too many of the same word can be erased and never added again.

    byte skipmin, skipmax; // Min and max length of skipped words (so I don't look for nothing)
    byte skipcnt;
    wchar_t **skipwords; // ords that are blacklisted.

    // Recursive for walkthrough.
    void WalkReq(TTextNode *n, void *data, TFuncTreeCallback afunc, TWalkType wt);
    // Recursive callback functions.
    void SaveNodeReq(TTextNode *node, FILE *f);
    void LoadNodeReq(TTextNode *node, TTextNode *parent, FILE *f, int version);
    void CountReq(TTextNode *n, void *tmp);
    void InsertReq(TTextNode *n, int value);
protected:
    bool blacklisted(UnicodeString str);
    // Find container for string. returns true if container exactly matches search string.
    bool FindContainer(const wchar_t *c);

    virtual UnicodeString DoGetWord(int index) = 0;
    virtual int DoMoveFromFullNode(int index);

    // Walks through the tree, visiting every branch and leaf.
    void Walkthrough(TFuncTreeCallback afunc, void *data, TWalkType wt = wtPre);

    // Returns all lines found under the node closest to the given string. This string should be filtered in derived classes.
    void FindLines(const wchar_t *c, bool exact, TIntegerList *list);

    __property TTextNode *selected = { read = fselected, write = fselected };
    __property TTextNodeList *nodes = { read = fnodes/*, write=fnodes*/ };
public:
    TTextSearchTreeBase(bool createbase, bool blacklist);
    TTextSearchTreeBase(TTextSearchTreeBase *orig);
    virtual ~TTextSearchTreeBase();

    void SaveToFile(FILE *f);
    void LoadFromFile(FILE *f);

    virtual void Clear();

    void AddNode(const wchar_t *c); // Add a new node to the right place, and select the new node when ready.
    void AddLine(int i); // Add a new line number to the selected node's line list.
    void CountWords(); // Walk through the whole tree and count every word.
    void AddSkip(const wchar_t *c); // Add a word that mustn't be found when looking for exact words (because only few would be found).

    void Expand(UnicodeString str, int index, bool insert = false); //Expand the tree with a new word, adding new node if necessary. If insert is true all values that match or above the inserted value are incremented by one.
    void RemoveLine(int line, bool linedeleted = false); // Remove line from all nodes changing the word count. Shift back all higher indices when deleted.
};

class TTextSearchTree : public TTextSearchTreeBase
{
private:
    typedef TTextSearchTreeBase inherited;

    bool freverse;
    bool fkana;
    TWordList *fwords;

    using inherited::Expand;
protected:
    virtual UnicodeString DoGetWord(int index);
    virtual int DoMoveFromFullNode(int index);
public:
    TTextSearchTree(TWordList *awords, bool kana, bool reverse, bool blacklist);
    TTextSearchTree(TWordList *awords, TTextSearchTree *orig);
    void FindWord(const wchar_t *c, TWordIndexList *list, bool exact, bool inflections, TWordFilterSelection *filter = NULL);
    void FindExactWord(const wchar_t *c, TIntegerList *list, unsigned int typ = 0, TWordFilterSelection *filter = NULL);

    void ExpandTree(UnicodeString str, int index);
};

// Return index of word in the given collection, return -1 when no match found.
int FindWordIndex(TWordCollection *collection, const wchar_t *kanji, const wchar_t *kana);
// Return index of word in the collection of the passed list, return -1 when no match found.
int FindWordIndex(TWordIndexList *list, const wchar_t *kanji, const wchar_t *kana);
// Kana can be NULL for english words
void FindWord(const wchar_t *c, TWordIndexList *list, bool kana, bool exact, bool reverse, bool inflections, TWordFilterSelection *filter = NULL);
void FindKanjiWord(TWordIndexList *l, UnicodeString c, bool inflections, TWordFilterSelection *filter = NULL);
int FuReading(const wchar_t *reading, const wchar_t *kana, int kanastart, int kanaend);

// Returns the number of usable items in dat. dat has to be freed by the caller but only when datcnt is not zero.
int FindFurigana(TWord *w, TFuriganaData *&dat);

// Returns true if two words are variants of each other with only okurigana/kanji spelling differences.
// If check_o is true, ignore starting polite o/go as well.
bool SameWord(TWord *w1, TWord *w2, bool check_o = false);

// Returns lenght of string up to but excluding the '.' character.
int kunlen(const wchar_t *kun);

#endif
