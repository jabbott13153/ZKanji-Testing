//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "searchtree.h"
#include "romajizer.h"
#include "words.h"
#include "collection.h"
#include "zkformats.h"
#include "kanji.h"
#include "wordfilter.h"
#include "genworddata.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

#define SEARCHTREE_VERSION   5

#ifdef _DEBUG
static TGlobalTest tester1(138);
#endif

//---------------------------------------------------------------------------
int FindWordIndex(TWordIndexList *list, const wchar_t *kanji, const wchar_t *kana);
int FindWordIndex(TWordCollection *collection, const wchar_t *kanji, const wchar_t *kana)
{
    TWordIndexList *list = new TWordIndexList(collection);
    int r = FindWordIndex(list, kanji, kana);
    delete list;

    return r;
}
int FindWordIndex(TWordIndexList *list, const wchar_t *kanji, const wchar_t *kana)
{
    int r = -1;

    try
    {
        FindWord(Romanize(kana).c_str(), list, true, true, false, false);
        for (int i = list->Count - 1; i >= 0; i--)
        {
            TWord *w = list->Words[i];
            if (wcscmp(w->kanji, kanji) || wcscmp(w->kana, kana))
                list->Delete(i);
        }
        if (list->Count == 1)
            r = list->Indexes[0];
    }
    catch (...)
    {
       ;
    }

    return r;
}
//---------------------------------------------------------------------------
void FindWord(const wchar_t *c, TWordIndexList *list, bool kana, bool exact, bool reverse, bool inflections, TWordFilterSelection *filter)
{
    if (kana)
        (reverse ? list->collection->btree : list->collection->ktree)->FindWord(c, list, exact, inflections, filter);
    else
        list->collection->rtree->FindWord(c, list, exact, inflections, filter);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TTextNodeList::TTextNodeList(TTextNode *acontainer) : inherited()
{
    fcontainer = acontainer;
}
//---------------------------------------------------------------------------
TTextNodeList::TTextNodeList(TTextNodeList *orig, TTextNode *acontainer) : fcontainer(acontainer)
{
    Capacity = orig->Count;
    while (Count != orig->Count)
    {
        TTextNode *n = new TTextNode;
        n->contained = orig->Items[Count]->contained;
        n->lines = new TIntegerList;
        n->lines->Capacity = orig->Items[Count]->lines->Count;
        while (n->lines->Count != orig->Items[Count]->lines->Count)
            n->lines->Add(orig->Items[Count]->lines->Items[n->lines->Count]);
        if (n->label != orig->Items[Count]->label)
        {
            n->label = new wchar_t[wcslen(orig->Items[Count]->label) + 1];
            wcscpy(n->label, orig->Items[Count]->label);
        }
        else
            n->label = NULL;
        n->parent = fcontainer;
        n->nodes = new TTextNodeList(orig->Items[Count]->nodes, n);
        inherited::Add(n);
    }
    /*
     TTextNode *n = new TTextNode;
     n->contained = 0;
     n->parent = container;
     n->nodes = new TTextNodeList(n);
     n->lines = new TIntegerList;
     if (str) {
     n->label = new wchar_t[wcslen(str)+1];
     GenLower(str,n->label);
     } else
     n->label = NULL;
     return Items[inherited::Add(n)];
     */
}
//---------------------------------------------------------------------------
TTextNodeList::~TTextNodeList()
{
    Clear();
}
//---------------------------------------------------------------------------
/*
TTextNode* TTextNodeList::get_item(int ix)
{
    return (TTextNode*) inherited::Items[ix];
}
*/
//---------------------------------------------------------------------------
int TTextNodeList::Add(TTextNode *node)
{
    node->parent = container;
    return inherited::Add(node);
}
//---------------------------------------------------------------------------
int nodesort(TTextNode **a, TTextNode **b)
{
    return GenCompareI((*a)->label, (*b)->label);
}
//---------------------------------------------------------------------------
void TTextNodeList::Sort()
{
    inherited::Sort(nodesort);
}
//---------------------------------------------------------------------------
TTextNode* TTextNodeList::Add(const wchar_t *str)
{
    TTextNode *n = new TTextNode;
    n->contained = 0;
    n->parent = container;
    n->nodes = new TTextNodeList(n);
    n->lines = new TIntegerList;
    if (str)
    {
        n->label = new wchar_t[wcslen(str) + 1];
        GenLower(str, n->label);
    }
    else
        n->label = NULL;
    return Items[inherited::Add(n)];
}
//---------------------------------------------------------------------------
/*
void TTextNodeList::Clear()
{
    for (int i = Count - 1; i >= 0; i--)
        FreeItem(i);
    inherited::Clear();
}
*/
//---------------------------------------------------------------------------
void TTextNodeList::FreeItem(TTextNode *item, int i)
{
    delete item->lines;
    delete item->nodes;
    delete[] item->label;

    delete item;
}
//---------------------------------------------------------------------------
/*
void TTextNodeList::Delete(int i)
{
    FreeItem(i);
    inherited::Delete(i);
}
*/
//---------------------------------------------------------------------------
TTextNode* TTextNodeList::RemoveNode(int i)
{
    TTextNode *n = Items[i];
    Items[i] = NULL;
    inherited::Delete(i);
    return n;
}
//---------------------------------------------------------------------------
TTextNode* TTextNodeList::nodesearch(const wchar_t *c)
{
    int mid, n;
    int min = 0;
    int max = Count - 1;

    while (max >= min)
    {
        mid = (min + max) / 2;

        n = GenCompareIN(Items[mid]->label, c, wcslen(Items[mid]->label));
        if (n > 0)
            max = mid - 1;
        else if (n < 0)
            min = mid + 1;
        else
            break;
    }

    if (min > max)
        return NULL;

    return Items[mid];
}
//---------------------------------------------------------------------------
//returns the nearest(!) container of c
TTextNode* TTextNodeList::SearchContainer(const wchar_t *c)
{
    TTextNode *m, *n = NULL;

    m = nodesearch(c);
    while (m)
    {
        n = m;
        m = m->nodes->SearchContainer(c);
    }

    return n;
}
//---------------------------------------------------------------------------
//removes a line from all sub nodes and returns the number removed
int TTextNodeList::RemoveLine(int line, bool linedeleted)
{
    int removed = 0;
    for (int i = Count - 1; i >= 0; i--)
    {
        removed += Items[i]->nodes->RemoveLine(line, linedeleted);
        if (!Items[i]->lines->Count && !Items[i]->nodes->Count && container)
            Delete(i);
    }
    if (container)
    {
        int ix = container->lines->IndexOf(line);
        if (ix >= 0)
        {
            removed++;
            container->lines->Delete(ix);
        }
        if (linedeleted)
        {
            for (int i = container->lines->Count - 1; i >= 0; i--)
                if (container->lines->Items[i] > line)
                {
                    container->lines->Items[i]--;
                }
        }
        container->contained -= removed;
    }
    return removed;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TTextSearchTreeBase::TTextSearchTreeBase(bool createbase, bool blacklist) : hasbase(createbase), blacklisting(blacklist)
{
    skipmin = 255;
    skipmax = 0; // Min and max length of skipped words (so I don't look for nothing).
    skipcnt = 0;
    skipwords = 0;

    fnodes = new TTextNodeList(NULL);
    fselected = NULL;

    wchar_t base[2];
    base[1] = 0;

    //create base a-z array
    if (hasbase)
        for (base[0] = L'a'; base[0] <= L'z'; base[0]++)
            nodes->Add(base);
}
//---------------------------------------------------------------------------
/*
 TTextSearchTreeBase::TTextSearchTreeBase(TTextSearchTreeBase *orig)
 {
 hasbase = orig->hasbase;
 skipmin = orig->skipmin;
 skipmax = orig->skipmax;
 skipcnt = orig->skipcnt;
 blacklisting = orig->blacklisting;
 if (blacklisting && skipcnt)
 skipwords = new wchar_t*[skipcnt];
 else
 skipwords = NULL;
 for (int ix = 0; ix < skipcnt; ++ix) {
 skipwords[ix] = new wchar_t[wcslen(orig->skipwords[ix])+1];
 wcscpy(skipwords[ix], orig->skipwords[ix]);
 }

 fselected = NULL;
 fnodes = new TTextNodeList(orig->fnodes, NULL);
 }
 */
//---------------------------------------------------------------------------
TTextSearchTreeBase::~TTextSearchTreeBase()
{
    Clear();

    delete nodes;
}
//---------------------------------------------------------------------------
void TTextSearchTreeBase::Clear()
{
    nodes->Clear();
    for (int i = 0; i < skipcnt; i++)
        delete[] skipwords[i];
    skipcnt = 0;
    delete[] skipwords;
    skipwords = NULL;
}
//---------------------------------------------------------------------------
// Finds container for string. returns true if container exactly matches search string.
// Sets selected node to a parent node if no exact match found.
bool TTextSearchTreeBase::FindContainer(const wchar_t *c)
{
    int i = wcslen(c);
    TTextNode *n;

    if (!i) // Error
        THROW(L"Empty node name!");

    if (!selected || selected && selected->label[0] != c[0])
    {
        // First select the node from a-z.
        if (hasbase && *c >= L'a' && *c <= L'z')
        {
            fselected = nodes->Items[c[0] - 'a'];
        }
        else
        {
            // Maybe some other character.
            fselected = NULL;
            for (int k = (hasbase ? L'z' - L'a' + 1 : 0); k < nodes->Count; k++)
                if (nodes->Items[k]->label[0] == c[0])
                {
                    fselected = nodes->Items[k];
                    break;
                }
            if (!selected)
            {
                // Create a new node at the base.
                wchar_t base[2] = L"a";
                base[0] = c[0];
                fselected = nodes->Add(base);
                return false;
            }
        }
        if (i == 1)
            return true; //exact match
    }

    i = GenCompareIN(c, selected->label, wcslen(selected->label));
    // Get out of selected node while it's not a container of the searched one.
    while (i)
    {
        fselected = selected->parent;
        i = GenCompareIN(c, selected->label, wcslen(selected->label));
        if (!i && !GenCompareI(c, selected->label))
            return true; //found
    }

    n = selected->nodes->SearchContainer(c);
    if (n)
        fselected = n;

    return !GenCompareI(selected->label, c);
}
//---------------------------------------------------------------------------
void TTextSearchTreeBase::AddNode(const wchar_t* c)
{
    if (!FindContainer(c))
        fselected = selected->nodes->Add(c);
}
//---------------------------------------------------------------------------
void TTextSearchTreeBase::AddLine(int i)
{
    if (!selected)
        THROW(L"No selected node in tree!");

    if (!selected->lines)
        selected->lines = new TIntegerList;
    selected->lines->Add(i);
}
//---------------------------------------------------------------------------
void TTextSearchTreeBase::RemoveLine(int line, bool linedeleted)
{
    fselected = NULL;
    nodes->RemoveLine(line, linedeleted);
}
//---------------------------------------------------------------------------
void TTextSearchTreeBase::CountReq(TTextNode *n, void *tmp)
{
    n->contained += n->lines->Count;
    if (n->parent)
        n->parent->contained += n->contained;
}

void TTextSearchTreeBase::CountWords()
{
    Walkthrough(&CountReq, 0, wtPost);
}
//---------------------------------------------------------------------------
void TTextSearchTreeBase::InsertReq(TTextNode *n, int value)
{
    for (int ix = 0; ix < n->lines->Count; ++ix)
        if (n->lines->Items[ix] >= value)
            n->lines->Items[ix]++;
}
//---------------------------------------------------------------------------
/*A small documentation about the search tree of the dictionary.
 * -------------------------------------------------------------------------------
 * First a short description of other objects in the dictionary:
 * TWord - this structure has the meaning/kanji-kana of a single entry. Kanji and
 *     kana are wchar_t* strings without space. Meaning is wchar_t*, has spaces and a
 *     lot of other non-alphanumeric characters like commas.
 *         eg. "new, fresh, novel".
 *     Thus the meaning must be indexed for every separate real-meaning. A word
 *     may have more than one meaning. They are stored in an array of character
 *     strings. (Not really, but make this simple.)
 * TWordList - A list holding TWord structures
 * TTextSearchTreeBase - What this documentation is about. This is a tree to make
 *     searches faster. Despite its name, it is really a forest. I'll only write
 *     about a single tree from now on. This tree has nodes like real trees, and
 *     can use them to search for strings (described soon).
 * TTextNodeList - A list type holding TTextNode objects.
 * TTextNode - A node in the TTextSearchTreeBase. Here things get complicated.
 *     An instance of this structure has the following:
 *         label - wchar_t*. As the name says this is the label for the node. The
 *             label defines which words can be stored in the node. (The indices
 *             of words, but it's almost the same.) It's all in lower case.
 *         lines - the indices to the words this node has. It's a simple TIntegerList
 *             with integers as its elements.
 *         nodes - instance of the TTextNodeList class. It holds the sub-nodes of
 *             this node. All sub-nodes have longer labels than this one, but
 *             their labels begin with this node's label.
 *         contained - sum of word indices contained in this node and sub-nodes.
 *         parent - can be NULL for top level nodes, otherwise the node that holds
 *             this one in their TTextNodeList nodes.
 * -------------------------------------------------------------------------------
 * How words are stored, and how the search works:
 * The TTextSearchTreeBase has a TTextNodeList (nodelist for short) object. The nodes
 * stored in the nodelist all have 1 character long labels. A word can be found
 * starting out from the node with the same character as the first character in
 * the word. A node has a TTextNodeList object too (the one called nodes). We
 * track down words starting from the first node, and going to sub-nodes by their
 * labels.
 * eg. for the word "analphabeta" we start from "a" -> "an" -> ... -> "analph" etc.
 * (The "a" node may not have a sub-node with the label "an". It can have something
 * like "ana" too!!)
 * When the node with the longest label is reached (which is still less or the same
 * as the word we are after), we stop. The lines list has the indices for words
 * that start with the node's label. The word we need must be one of them, or none,
 * but that means such word does not exist in the database.
 * -------------------------------------------------------------------------------
 * How the Expand works to build the custom dictionary. This is important, because
 * badly built trees won't be expandable!
 *
 * First there are no nodes, or only 1 character long nodes. The new word is added
 * to the index list of the node, which has the same label as the 1st character in
 * the word.
 * 300 words can be added to a single node before the node needs to "grow"
 * sub-nodes. When the limit is reached, sub-nodes are created with labels that
 * are 1 character longer than their parent node's. These sub-nodes are created
 * for the first 2 characters of words in the base node. Every word's index is
 * moved to the sub-nodes.
 * IMPORTANT: there are separate TTextSearchTreeBase objects for the kana and the
 * meaning of a word. When the tree is for the meaning, we have to be careful! The
 * meaning has many real meanings in it (separated by commas or space or anything
 * that is not a letter). The same word can be in many nodes for each meaning.
 * Some real meanings may be part of other real meanings like "a" is part of "an".
 * When this happens, the word will be in both the node labeled as "a" and the one
 * labeled as "an". When there is no "an" node yet (first 300), the word is only
 * in the base node. Instead of moving the word from "a" to "an", we have to
 * duplicate the entry = Add its index to the new node too, and don't remove from
 * the base node.
 * When new words are added, we add them for all(!) their meaning and their single
 * kana to the appropriate TTextNodeList. When a 2 character long labeled node
 * exists that can hold the word, we add it there. If it reached the 300 limit, we
 * have to create 3 characters long labeled nodes etc.
 *
 * There is a blacklist for the meanings when they appear in more than 5000 words.
 * For example "the" may be in many words. "Thesaurus" is not "the" in this case.
 * (BTW every label is in lowercase!)
 * A blacklisted meaning doesn't get added to any nodes, and the node with the
 * blacklisted label is deleted and is never to be created again.
 *
 * Important points to note: Everything is tested in lower case. Don't create a
 * node when it's not absolutely necessary (eg. its future base haven't reached
 * 300 in word count). A word may be both in a base node and a sub-node when it
 * has a meaning that must appear in the base and another that is in the sub-node.
 * Every node keeps the sum of words appearing in it and its sub-nodes. When a
 * new word is added, we have to update the parent node's word count too.
 * Base nodes cannot be blacklisted! When adding a new word that requires the
 * creation of sub-nodes, we have to check if the sub-node is blacklisted or not,
 * before creating it. We can create nodes with longer than one character extra in
 * this case.
 * The word delimiter characters for the words in the meaning string are in an
 * array called cdelim.
 * -----------------------------------------------------------------------------*/
void TTextSearchTreeBase::Expand(UnicodeString str, int index, bool insert)
{
    if (!str.Length())
        THROW(L"Why are we here? (bug)");

    if (insert)
        Walkthrough((TFuncTreeCallback)&InsertReq, (void*)index);

    if (blacklisted(str)) // If a word is blacklisted, it should not be included in search trees.
        return;

    wchar_t *c = new wchar_t[str.Length() + 1];
    wcscpy(c, str.c_str());
    AnsiStrLower(c);

    int cntdiff = 0;

    UnicodeString s;
    TTextNode *n, *sel;

    FindContainer(c);

    if (selected->lines->IndexOf(index) >= 0)
    {
        delete[] c;
        return;
    }

    unsigned int slen = wcslen(selected->label);
    bool match = !wcscmp(c, selected->label);
    //nodes->Count > 0 means that the selected node is already split
    if (match || selected->lines->Count < NODEFULLCOUNT && !selected->nodes->Count)
    {
        if (selected->parent && selected->lines->Count >= NODETOOMUCHCOUNT) // Can only happen on match, blacklist!
        {
            AddSkip(c);
            cntdiff = -selected->lines->Count;
            while (selected->nodes->Count)
                selected->parent->nodes->Add(selected->nodes->RemoveNode(0));
            selected->parent->nodes->Sort(); // Sort nodes in alphabetical order, so they can be "searched".
            sel = selected->parent;
            selected->parent->nodes->Delete(selected->parent->nodes->IndexOf(selected));
            delete[] c;
            c = NULL;
            selected = sel;
        }
        else
        {
            selected->lines->Add(index);
            cntdiff = 1;
        }
    }
    else if (!selected->nodes->Count) // The node is full, move everything to a new level, do everything else only after that.
    {
        for (int ix = selected->lines->Count - 1; ix >= 0; --ix)
            cntdiff += DoMoveFromFullNode(ix);
    }

    if (!match && selected->nodes->Count) // We have to create a new node for this word only.
    {

        unsigned int len = slen + 1;
        while (len <= wcslen(c) && blacklisted(((UnicodeString)c).SubString(1, len)))
            len++;
        if (len <= wcslen(c))
        {
            s = ((UnicodeString)c).SubString(1, len);
            n = selected->nodes->SearchContainer(s.c_str());
            if (!n || wcscmp(n->label, s.c_str()))
            {
                n = selected->nodes->Add(s.c_str());
                selected->nodes->Sort();
            }
            n->lines->Add(index);
            n->contained++;

            cntdiff++;
        }
    }

    do
    {
        selected->contained += cntdiff;
        selected = selected->parent;
    }
    while (selected);

    delete[] c;
}
//---------------------------------------------------------------------------
bool TTextSearchTreeBase::blacklisted(UnicodeString str)
{
    if (!blacklisting)
        return false;
    int blen = str.Length();
    if (blen >= skipmin && blen <= skipmax)
        for (int i = 0; i < skipcnt; i++)
            if (!wcscmp(str.c_str(), skipwords[i]))
                return true;
    return false;
}
//---------------------------------------------------------------------------
int TTextSearchTreeBase::DoMoveFromFullNode(int index)
{
    UnicodeString k = AnsiLowerCase(DoGetWord(selected->lines->Items[index]));

    int slen = wcslen(selected->label);
    UnicodeString s;
    TTextNode *n;

    int klen = k.Length();
    if (klen == slen) // Don't do anything.
                return 0;

    int len = slen + 1;
    while (len <= klen && blacklisted(k.SubString(1, len)))
        len++;
    if (len > klen)
        return 0;

    s = k.SubString(1, len);
    n = selected->nodes->SearchContainer(s.c_str());
    if (!n || wcscmp(n->label, s.c_str()))
    {
        n = selected->nodes->Add(s.c_str());
        selected->nodes->Sort();
    }
    n->lines->Add(selected->lines->Items[index]);
    n->contained++;

    selected->lines->Delete(index);

    return 0;
}
//---------------------------------------------------------------------------
// Returns all lines found under the node closest to the given string.
// This string should be filtered in derived classes.
// WARNING: c MUST be lowercase, list is emptied.
void TTextSearchTreeBase::FindLines(const wchar_t *c, bool exact, TIntegerList *list)
{
    try
    {
        list->Clear();
    }
    catch (...)
    {
        THROW(L"List must be allocated! Should be empty as well. BUG!");
    }

    TIntegerList *l = new TIntegerList;

    TTextNode *n;

    int blen = wcslen(c);

    // Find node with least number of words.
    FindContainer(c);

    list->Add((int)selected);

    // Recursively add everything from selected that matches the c string.
    while (list->Count)
    {
        n = (TTextNode*)list->Items[0];

        for (int ix = 0; ix < n->lines->Count; ++ix)
            l->Add(n->lines->Items[ix]);

        list->Delete(0);

        if (!exact/*|| wcslen(n->label) < blen*/)
            for (int ix = 0; ix < n->nodes->Count; ++ix)
                if (!GenCompareIN(n->nodes->Items[ix]->label, c, blen))
                    list->Add((int)n->nodes->Items[ix]);
    }
    // Instead of laOr-ing the lists, sort and remove duplicates.
    if (!exact)
        l->Sort(true);

    int iii = 0;
    while (iii < l->Count)
    {
        while (iii < l->Count - 1 && l->Items[iii] == l->Items[iii + 1])
            iii++;
        list->Add(l->Items[iii++]);
    }

    delete l;
}
//---------------------------------------------------------------------------
void TTextSearchTreeBase::SaveNodeReq(TTextNode *node, FILE *f)
{
    int l;
    int cc;

    write(node->label, f, sltByte);

    l = node->lines->Count;
    fwrite(&l, sizeof(int), 1, f);
    for (int j = 0; j < node->lines->Count; j++)
    {
        l = node->lines->Items[j];
        fwrite(&l, sizeof(int), 1, f);
    }
    if (node->nodes->Count > 256*256 - 1)
        THROW(L"Too big number!");
    cc = node->nodes->Count;
    fwrite(&cc, sizeof(int), 1, f);
}
//---------------------------------------------------------------------------
void TTextSearchTreeBase::SaveToFile(FILE *f)
{
    char verstr[8];
    sprintf(verstr, "zsct%03i", SEARCHTREE_VERSION);
    fwrite(verstr, sizeof(char), 7, f);

    int nodecnt = nodes->Count;
    fwrite(&nodecnt, sizeof(int), 1, f);
    Walkthrough((TFuncTreeCallback)&SaveNodeReq, (void*)f);

    fwrite(&skipcnt, sizeof(byte), 1, f);

    for (int i = 0; i < skipcnt; i++)
        write(skipwords[i], f, sltByte);
}
//---------------------------------------------------------------------------
void TTextSearchTreeBase::LoadNodeReq(TTextNode *node, TTextNode *parent, FILE *f, int version)
{
    int l;
    byte b;
    int cc;

    node->parent = parent;

    if (version >= 5)
    {
        if (node->label)
            delete[] node->label;
        read(node->label, f, sltByteAddNULL);
    }
    else
    {
        fread(&b, sizeof(byte), 1, f);
        char *tmp = new char[b + 1];
        if (b)
            fread(tmp, sizeof(char), b, f);
        tmp[b] = 0;

        if (node->label)
            delete[] node->label;

        int b2 = MultiByteToWideChar(CP_ACP, 0, tmp, b + 1, node->label, 0);
        node->label = new wchar_t[b2];

        MultiByteToWideChar(CP_ACP, 0, tmp, b + 1, node->label, b2);
        delete[] tmp;
    }

    if (parent)
        node->lines = new TIntegerList;
    fread(&node->contained, sizeof(int), 1, f);
    for (int j = 0; j < node->contained; j++)
    {
        fread(&l, sizeof(int), 1, f);
        node->lines->Add(l);
    }
    if (parent)
        node->nodes = new TTextNodeList(node);
    if (version >= 4) // Added 10.01.27 v0.533
        fread(&cc, sizeof(int), 1, f);
    else
    {
        word tmp;
        fread(&tmp, sizeof(word), 1, f);
        cc = tmp;
    }
    node->nodes->Capacity = node->nodes->Count + cc + 2;
    for (int j = 0; j < cc; j++)
    {
        TTextNode *n = new TTextNode;
        n->label = NULL;
        node->nodes->Add(n);
        LoadNodeReq(n, node, f, version);
        node->contained += n->contained;
    }
    if (version <= 4)
        node->nodes->Sort();
}

void TTextSearchTreeBase::LoadFromFile(FILE *f)
{
    fselected = NULL;
    int nodecnt;

    int version = 4;
    char cstr[6];
    cstr[4] = 0;
    fread(cstr, sizeof(byte), 4, f);
    if (!strcmp(cstr, "zsct")) // Version above 4 - added 2011.04.04
    {
        fread(cstr, sizeof(byte), 3, f);
        cstr[3] = 0;
        version = strtol(cstr, 0, 10);

        fread(&nodecnt, sizeof(int), 1, f);
    }
    else
        nodecnt = (cstr[3] << 24) | (cstr[2] << 16) | (cstr[1] << 8) | cstr[0];

    for (int i = 0; i < nodecnt; i++)
    {
        if (i == nodes->Count) // Add every saved extra node that's not part of the list.
            nodes->Add((wchar_t*)NULL);
        LoadNodeReq(nodes->Items[i], NULL, f, version);
    }
    //if (version <= 4)
    //nodes->Sort();

    fread(&skipcnt, sizeof(byte), 1, f);
    byte b;

    if (skipcnt)
        skipwords = new wchar_t*[skipcnt];
    else
        skipwords = NULL;

    if (!blacklisting && skipcnt)
        THROW(L"Cannot have skipped words in a non-blacklisting tree!");
    for (int i = 0; i < skipcnt; i++)
    {
        if (version >= 5) // Added 2011.04.04
        {
            read(skipwords[i], f, sltByteAddNULL);
        }
        else
        {
            fread(&b, sizeof(byte), 1, f);
            char *tmp = new char[b + 1];
            if (b)
                fread(tmp, sizeof(char), b, f);
            tmp[b] = 0;
            skipwords[i] = new wchar_t[b + 1];
            MultiByteToWideChar(CP_ACP, 0, tmp, b + 1, skipwords[i], b + 1);
            delete[] tmp;
        }

        skipmin = MIN(skipmin, b);
        skipmax = MAX(skipmax, b);
    }
}
//---------------------------------------------------------------------------
void TTextSearchTreeBase::WalkReq(TTextNode *n, void *data, TFuncTreeCallback afunc, TWalkType wt)
{
    if (wt == wtPre)
        afunc(n, data);
    for (int i = 0; i < n->nodes->Count; i++)
        WalkReq(n->nodes->Items[i], data, afunc, wt);
    if (wt == wtPost)
        afunc(n, data);
}
//---------------------------------------------------------------------------
void TTextSearchTreeBase::Walkthrough(TFuncTreeCallback afunc, void *data, TWalkType wt)
{
    for (int i = 0; i < nodes->Count; i++)
        WalkReq(nodes->Items[i], data, afunc, wt);
}
//---------------------------------------------------------------------------
void TTextSearchTreeBase::AddSkip(const wchar_t *c)
{
    if (!blacklisting)
        return;

    int clen = wcslen(c);
    skipmin = MIN(clen, skipmin);
    skipmax = MAX(clen, skipmax);
    if (!skipwords)
    {
        skipwords = new wchar_t*[1];
    }
    else
    {
        wchar_t **tmp = skipwords;
        skipwords = new wchar_t*[skipcnt + 1];
        memcpy(skipwords, tmp, sizeof(char*)*skipcnt);
        delete[] tmp;
    }
    skipwords[skipcnt] = new wchar_t[clen + 1];
    wcscpy(skipwords[skipcnt], c);
    AnsiStrLower(skipwords[skipcnt]);
    skipcnt++;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TTextSearchTree::TTextSearchTree(TWordList *awords, bool kana, bool reverse, bool blacklisting) : fwords(awords), fkana(kana), freverse(reverse), inherited(true, blacklisting)
{
    wchar_t base[2];
    base[1] = 0;

    // Only a reverse list can start with a kana N.
    if (kana && reverse)
    {
        base[0] = '\'';
        nodes->Add(base);
    }
}
//---------------------------------------------------------------------------
TTextSearchTree::TTextSearchTree(TWordList *awords, TTextSearchTree *orig) : fwords(awords), inherited(orig, NULL)
{
    fkana = orig->fkana;
    freverse = orig->freverse;
}
//---------------------------------------------------------------------------
UnicodeString Reverse(UnicodeString str)
{
    UnicodeString s = "";
    int len = str.Length();
    s.SetLength(len);
    for (int ix = len; ix >= 1; --ix)
        s[len - ix + 1] = str[ix];
    return s;
}
//---------------------------------------------------------------------------
UnicodeString TTextSearchTree::DoGetWord(int index)
{
    UnicodeString s = Romanize(fwords->Items[index]->kana); //romaji;
    if (freverse)
        return Reverse(s);
    return s;
}
//---------------------------------------------------------------------------
void TTextSearchTree::ExpandTree(UnicodeString str, int index)
{
    if (freverse)
        str = Reverse(str);

    Expand(str, index);
}
//---------------------------------------------------------------------------
void TTextSearchTree::FindWord(const wchar_t *c, TWordIndexList *list, bool exact, bool inflections, TWordFilterSelection *filter)
{
    if (list->collection->words != fwords)
        THROW(L"Invalid call to FindWord!");

    TWord *w;
    bool what;
    TTextNode *n;
    int x, y;
    wchar_t line[1024];
    wchar_t cc[1024];

    int blen = min(wcslen(c), 1023);

    wchar_t *bc = new wchar_t[blen + 1];

    // Create a character array that is backwards.
    // Reverse ONLY WORKS FOR KANA at the moment.
    // We are looking for reversed words, so reverse the search string too.
    if (freverse)
    {
        bc[blen] = 0;
        for (int i = 0; i < blen; i++)
            bc[blen - i - 1] = c[i];
    }
    else
    {
        wcsncpy(bc, c, blen);
        bc[blen] = 0;
    }
    AnsiStrLower(bc);

    if (exact && blacklisted(bc))
    {
        delete[] bc;
        return;
    }

    // Find the first suitable word in the search and save everything about it.
    wcscpy(line, bc);
    wchar_t *str = wcstok(line, L" ");
    while (str && !fkana && wcschr(cdelim, *str) && (!freverse || *str != L'\''))
        str = wcstok(NULL, L" ");
    if (!str)
    {
        delete[] bc;
        return;
    }

    // Find node with least number of words from the "good" search words.
    FindContainer(str); // Find node with first word.

    n = selected;
    wcscpy(cc, str); // Remember word,

    str = wcstok(NULL, L" ");
    while (str && !fkana && wcschr(cdelim, *str) && (!freverse || *str != L'\''))
        str = wcstok(NULL, L" ");

    while (str)
    {
        FindContainer(str); // Find node with least number of words.
        if (selected->contained < n->contained)
        {
            // We got a winner.
            n = selected;
            wcscpy(cc, str);
        }

        str = wcstok(NULL, L" ");
        while (str && !fkana && wcschr(cdelim, *str) && (!freverse || *str != L'\''))
            str = wcstok(NULL, L" ");
    }
    blen = wcslen(cc);

    selected = n; // Bring back winner node and continue search from there.

    TIntegerList *l = new TIntegerList;

    inherited::FindLines(cc, exact, l);

    // Search for the bc expression in all the words found.
    if (fwords)
    {
        for (int i = 0; i < l->Count; i++)
        {
            w = fwords->Items[l->Items[i]];

            if (filter && !filter->match(w))
            {
                l->Delete(i);
                i--;
                continue;
            }

            // Meaning search.
            if (!fkana)
            {
                what = true;
                for (int j = 0; j < w->meaningcnt; j++)
                {
                    x = wcslen(bc);
                    str = w->data[j].meaning;
                    y = wcslen(str);
                    while (what && y >= x)
                    {

                        if (str == w->data[j].meaning || wcschr(cdelim, *(str - 1)))
                        {
                            if (exact && y != x && !wcschr(cdelim, *(str + x)))
                                what = true;
                            else
                                what = GenCompareIN(str, bc, x);
                        }
                        str++;
                        y--;
                    }
                    if (!what)
                        break;
                }
                if (what)
                {
                    l->Delete(i);
                    i--;
                }
            }
            else // Kana search
            {
                int wlen = wcslen(w->romaji);
                if (wlen < blen || ((!exact || freverse) && GenCompareIN(w->romaji + (freverse ? wlen - blen : 0), c, blen) || exact && !freverse && (_wcsnicmp(w->romaji, c, blen) || w->romaji[blen] != 0)))
                {
                    l->Delete(i);
                    i--;
                }
            }
        }
    }

    for (int i = 0; i < l->Count; i++)
        list->Add(l->Items[i]);

    delete l;

    if (fwords)
    {
        // Find inflections and sort when needed.
        if (!fkana)
        {
            list->RomajiSort(bc);
        }
        else
        {
            // Find inflected forms.
            l = new TIntegerList;
            int dummy = 0;
            UnicodeString str = ToKana(UnicodeString(c).SetLength(blen), dummy);
            dummy = 0;

            if (inflections)
            {
                TWordInfList *inflist = new TWordInfList;
                if (str.Length())
                    DeInflect(inflist, str, dummy);

                if (freverse)
                {
                    TWordIndexList *ll = new TWordIndexList(list->collection);
                    for (int k = 0; k < inflist->Count; k++)
                    {
                        if (wcslen(inflist->Items[k]->str) <= 1)
                            continue;
                        FindWord(Romanize(inflist->Items[k]->str).c_str(), ll, exact, false, filter);

                        for (int i = ll->Count - 1; i >= 0; i--)
                        {
                            bool found = false;
                            int mcnt = fwords->Items[ll->Items[i]->index]->meaningcnt;
                            for (int j = 0; j < mcnt && !found; ++j)
                            {
                                if (fwords->Items[ll->Items[i]->index]->data[j].types & (1 << inflist->Items[k]->type))
                                    found = true;
                                else if (inflist->Items[k]->type == WT_ADJ_BASE && fwords->Items[ll->Items[i]->index]->data[j].types & WT_ADJ_AUX) // Find aux adj. for type adjective
                                    found = true;
                            }

                            if (!found)
                                ll->Delete(i);
                        }

                        list->AddInflected(ll, inflist->Items[k]->inf);

                        ll->Clear();
                    }
                    delete ll;
                }
                else
                {
                    TWordIndexList *ll = NULL;
                    if (!exact)
                        ll = new TWordIndexList(list->collection);
                    for (int k = 0; k < inflist->Count; k++)
                    {
                        //if (exact)
                        //{
                            FindExactWord(Romanize(inflist->Items[k]->str).c_str(), l, (1 << inflist->Items[k]->type), filter);
                            list->AddInflected(l, inflist->Items[k]->inf);
                            l->Clear();
                        //}
                        /*
                        else
                        {
                            FindWord(Romanize(inflist->Items[k]->str).c_str(), ll, exact, false, filter);

                            for (int i = ll->Count - 1; i >= 0; i--)
                            {
                                bool found = false;
                                int mcnt = fwords->Items[ll->Items[i]->index]->meaningcnt;
                                for (int j = 0; j < mcnt && !found; ++j)
                                {
                                    if (fwords->Items[ll->Items[i]->index]->data[j].types & (1 << inflist->Items[k]->type))
                                        found = true;
                                    else if (inflist->Items[k]->type == WT_ADJ_BASE && fwords->Items[ll->Items[i]->index]->data[j].types & WT_ADJ_AUX) // Find aux adj. for type adjective
                                        found = true;
                                }

                                if (!found)
                                    ll->Delete(i);
                            }

                            list->AddInflected(ll, inflist->Items[k]->inf);

                            ll->Clear();
                        }
                        */
                    }
                    delete ll;
                }
                delete inflist;

            }

            list->KanaSort();

            delete l;
        }
    }
    delete[] bc;
}
//---------------------------------------------------------------------------
int TTextSearchTree::DoMoveFromFullNode(int index)
{
    if (fkana)
        return inherited::DoMoveFromFullNode(index);

    int result = 0;
    wchar_t *k;
    int slen = wcslen(selected->label);
    wchar_t *l;
    UnicodeString s;
    TTextNode *n;

    TWord *w = fwords->Items[(selected->lines->Items[index])];

    //same as above, but we have to separate the meanings and test for every single part
    TStringList *ms = new TStringList; //add every word to this first
    bool fullmove = true;
    for (int j = 0; j < w->meaningcnt; j++)
    {
        k = new wchar_t[wcslen(w->data[j].meaning) + 1];
        wcscpy(k, w->data[j].meaning);
        AnsiStrLower(k);
        l = wcstok(k, cdelim);
        if (l)
            do
            {
                if (!wcsncmp(selected->label, l, slen))
                {
                    if ((int)wcslen(l) == slen) //fits this node well
                                fullmove = false;
                    else if (!blacklisted(l))
                        ms->Add(l);
                }
            }
            while ((l = wcstok(NULL, cdelim)) != NULL);

        delete[] k;
    }
    ms->Sort();
    for (int j = 0; j < ms->Count; j++)
    {
        //find length of new node to create
        int len = slen + 1;
        while (len <= ms->Strings[j].Length() && blacklisted(ms->Strings[j].SubString(1, len)))
            len++;
        //if blacklisted, delete
        if (len > ms->Strings[j].Length())
            ms->Delete(j);
        else
        { //delete everything that would go to the same node
            while (ms->Count > j + 1 && !wcsncmp(ms->Strings[j].c_str(), ms->Strings[j + 1].c_str(), len))
                ms->Delete(j + 1);

            s = ms->Strings[j].SubString(1, len);
            n = selected->nodes->SearchContainer(s.c_str());
            if (!n || wcsncmp(n->label, s.c_str(), len))
            {
                n = selected->nodes->Add(s.c_str());
                selected->nodes->Sort();
            }
            n->lines->Add(selected->lines->Items[index]);
            result++;
            n->contained++;
        }
    }

    if (fullmove)
    { //delete line from original node
        selected->lines->Remove(selected->lines->Items[index]);
        result--;
    }

    delete ms;

    return result;
}
//---------------------------------------------------------------------------
void TTextSearchTree::FindExactWord(const wchar_t *c, TIntegerList *list, unsigned int typ, TWordFilterSelection *filter)
{
    try
    {
        FindContainer(c);
    }
    catch (...)
    {
        return;
    }

    TWord *w;
    for (int i = 0; i < selected->lines->Count; i++)
    { //look for exact match only
        w = fwords->Items[selected->lines->Items[i]];
        if ((!filter || filter->match(w)) && !wcscmp(w->romaji, c) && list->IndexOf(selected->lines->Items[i]) < 0)
        {
            int mcnt = w->meaningcnt;
            bool found = !typ;
            for (int j = 0; j < mcnt && !found; ++j)
                found = (w->data[j].types & typ) == typ || typ == WT_ADJ && (w->data[j].types & WT_ADJ_AUX);
            if (found)
                list->Add(selected->lines->Items[i]);
        }
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int kunlen(const wchar_t *kun)
{
    return (int)wcscspn(kun, L".\0");
}

int onlen(const wchar_t *on)
{
    return wcslen(on) - (on[0] == L'-' ? 1 : 0);
}

int FuReading(const wchar_t *reading, const wchar_t *kana, int kanastart, int kanaend)
{
    int len = kunlen(reading);
    int val = 0;

    if (len > kanaend - kanastart + 1)
        return 0;
    int i;
    wchar_t kch2;
    for (i = 0; i < len && kanastart + i <= kanaend; i++)
    {
        if (KanaMatch(reading, i, kana, kanastart + i))
            continue;
        wchar_t och = HiraganaCh(reading, i), kch = HiraganaCh(kana, kanastart + i);

        if (i == len - 1)
        {
            if (kch == L'っ')
            {
                if (och == L'ち')
                {
                    val += 2;
                    continue;
                }
                if (och == L'つ')
                {
                    val += 1;
                    continue;
                }
            }
            if (och == L'つ' && kch == L'ち')
            {
                val += 2;
                continue;
            }
            if (och == L'ち' && kch == L'じ')
            {
                val += 2;
                continue;
            }
            if (och == L'た' && kch == L'だ')
            {
                val += 1;
                continue;
            }
        }

        if (i == 0)
        {
            if (och == L'ち' && kch == L'じ' || och == L'つ' && kch == L'ず')
            {
                val += 2;
                continue;
            }
            if (kch == och + 1 && (och >= 0x304b && och < 0x3062 && (och % 2) || och >= 0x3064 && och < 0x3069 && !(och % 2)))
            {
                val += 1;
                continue;
            }
            if (och >= 0x306f && och < 0x307d && (kch == och + 1 || kch == och + 2) && !(och % 3))
            {
                val += 1;
                continue;
            }
        }

        if (och == L'ヶ' && kch == L'か' || och == L'ヵ' && kch == L'か' ||

            och == L'よ' && kch == L'ょ' || och == L'ゆ' && kch == L'ゅ' || och == L'や' && kch == L'ゃ' || //h row matches

            och == L'あ' && kch == L'わ')
        {
            val++;
            continue;
        }

        if (kanastart + i + 1 <= kanaend && i == len - 1 && kch == L'っ' && och - 0x3041 >= 0 && och - 0x3041 < 84)
        {
            kch2 = HiraganaCh(kana, kanastart + i + 1);
            if (kch2 - 0x3041 >= 0 && kch2 - 0x3041 < 84 && consonantcolumn[och - 0x3041] == consonantcolumn[kch2 - 0x3041] && consonantcolumn[och - 0x3041] > 0)
                val += 2;
            continue;
        }
        break;
    }
    return i == len ? MAX(val, 1) : 0;
}

int KunLenSort(int a, int b)
{
    return b - a;
}
int KanLenSort(int a, int b)
{
    return wcslen(store->Kanji[a]) - wcslen(store->Kanji[b]);
}

wchar_t *zero0[] = { L"ぜろ", L"れい", L"まる" };
wchar_t *ichi1[] = { L"ひと", L"いち", L"じゅう", L"せん", L"まん", L"おく", L"いっせん", L"いちまん", L"いちおく", L"ワン" };
wchar_t *ni2[] = { L"に", L"ふた", L"ふつ", L"はた", L"はつ", L"じ", L"ツー", L"にじゅう", L"にひゃく", L"にせん", L"にまん", L"におく" };
wchar_t *san3[] = { L"さん", L"ぞう", L"み", L"みっ", L"スリー", L"さんじゅう", L"さんびゃく", L"さんぜん", L"さんまん", L"さんおく" };
wchar_t *yon4[] = { L"よ", L"よっ", L"よん", L"し", L"フォー", L"しじゅう", L"しひゃく", L"よんじゅう", L"よんひゃく", L"よんせん", L"よんまん", L"よんおく" };
wchar_t *go5[] = { L"ご", L"い", L"いつ", L"ファイブ", L"ごじゅう", L"ごひゃく", L"ごせん", L"ごまん", L"ごおく" };
wchar_t *roku6[] = { L"む", L"むい", L"むっ", L"ろく", L"りく", L"シックス", L"ろくじゅう", L"ろっぴゃく", L"ろくせん", L"ろくまん", L"ろくおく" };
wchar_t *nana7[] = { L"なな", L"なの", L"しち", L"セベン", L"セブン", L"しちじゅう", L"しちひゃく", L"しちせん", L"しちまん", L"しちおく", L"ななじゅう", L"ななひゃく", L"ななせん", L"ななまん", L"ななおく" };
wchar_t *hachi8[] = { L"や", L"やっ", L"よう", L"はち", L"エイト", L"はちじゅう", L"はっぴゃく", L"はっせん", L"はちまん", L"はちおく" };
wchar_t *kyuu9[] = { L"ここの", L"きゅう", L"く", L"ナイン", L"きゅうじゅう", L"くじゅう", L"きゅうひゃく", L"くひゃく", L"きゅうせん", L"きゅうまん", L"きゅうおく" };
wchar_t *shimekun[] = { L"しめ", L"じめ" };

wchar_t *onajikun[] = { L"おなじ", L"おなじく" };
wchar_t *marukun[] = { L"まる", L"かんすうじゼロ", L"じゅう" };
wchar_t *andokun[] = { L"アンド" };
wchar_t *attokun[] = { L"ｱｯﾄ" };
wchar_t *purasukun[] = { L"プラス" };

wchar_t *lettera[] = { L"エー" };
wchar_t *letterb[] = { L"ビー" };
wchar_t *letterc[] = { L"シー" };
wchar_t *letterd[] = { L"ディー" };
wchar_t *lettere[] = { L"イー" };
wchar_t *letterf[] = { L"エフ" };
wchar_t *letterg[] = { L"ジー" };
wchar_t *letterh[] = { L"エイチ", L"エッチ", L"ハー" };
wchar_t *letteri[] = { L"アイ", L"イ" };
wchar_t *letterj[] = { L"ジェー" };
wchar_t *letterk[] = { L"ケー", L"キー" };
wchar_t *letterl[] = { L"エル" };
wchar_t *letterm[] = { L"エム" };
wchar_t *lettern[] = { L"エヌ", L"ン" };
wchar_t *lettero[] = { L"オ", L"ウ" };
wchar_t *letterp[] = { L"ピー", L"ペー" };
wchar_t *letterq[] = { L"キュー", L"クー" };
wchar_t *letterr[] = { L"アール", L"エル", L"レ" };
wchar_t *letters[] = { L"エス", L"ス" };
wchar_t *lettert[] = { L"ティー" };
wchar_t *letteru[] = { L"ユー" };
wchar_t *letterv[] = { L"ヴィー", L"ブイ" };
wchar_t *letterw[] = { L"ダブルヴィー", L"ダブルブイー" };
wchar_t *letterx[] = { L"エックス" };
wchar_t *lettery[] = { L"ワイ" };
wchar_t *letterz[] = { L"ゼィー", L"ゼッド" };

int ValidReadings(wchar_t vchar, wchar_t** &readings)
{
    switch (vchar)
    {
    case L'０':
        readings = zero0;
        return sizeof(zero0) / sizeof(wchar_t*);
    case L'１':
        readings = ichi1;
        return sizeof(ichi1) / sizeof(wchar_t*);
    case L'２':
        readings = ni2;
        return sizeof(ni2) / sizeof(wchar_t*);
    case L'３':
        readings = san3;
        return sizeof(san3) / sizeof(wchar_t*);
    case L'４':
        readings = yon4;
        return sizeof(yon4) / sizeof(wchar_t*);
    case L'５':
        readings = go5;
        return sizeof(go5) / sizeof(wchar_t*);
    case L'６':
        readings = roku6;
        return sizeof(roku6) / sizeof(wchar_t*);
    case L'７':
        readings = nana7;
        return sizeof(nana7) / sizeof(wchar_t*);
    case L'８':
        readings = hachi8;
        return sizeof(hachi8) / sizeof(wchar_t*);
    case L'９':
        readings = kyuu9;
        return sizeof(kyuu9) / sizeof(wchar_t*);
    case L'〆':
        readings = shimekun;
        return sizeof(shimekun) / sizeof(wchar_t*);
    case L'Ａ':
    case L'ａ':
        readings = lettera;
        return sizeof(lettera) / sizeof(wchar_t*);
    case L'Ｂ':
    case L'ｂ':
        readings = letterb;
        return sizeof(letterb) / sizeof(wchar_t*);
    case L'Ｃ':
    case L'ｃ':
        readings = letterc;
        return sizeof(letterc) / sizeof(wchar_t*);
    case L'Ｄ':
    case L'ｄ':
        readings = letterd;
        return sizeof(letterd) / sizeof(wchar_t*);
    case L'Ｅ':
    case L'ｅ':
        readings = lettere;
        return sizeof(lettere) / sizeof(wchar_t*);
    case L'Ｆ':
    case L'ｆ':
        readings = letterf;
        return sizeof(letterf) / sizeof(wchar_t*);
    case L'Ｇ':
    case L'ｇ':
        readings = letterg;
        return sizeof(letterg) / sizeof(wchar_t*);
    case L'Ｈ':
    case L'ｈ':
        readings = letterh;
        return sizeof(letterh) / sizeof(wchar_t*);
    case L'Ｉ':
    case L'ｉ':
        readings = letteri;
        return sizeof(letteri) / sizeof(wchar_t*);
    case L'Ｊ':
    case L'ｊ':
        readings = letterj;
        return sizeof(letterj) / sizeof(wchar_t*);
    case L'Ｋ':
    case L'ｋ':
        readings = letterk;
        return sizeof(letterk) / sizeof(wchar_t*);
    case L'Ｌ':
    case L'ｌ':
        readings = letterl;
        return sizeof(letterl) / sizeof(wchar_t*);
    case L'Ｍ':
    case L'ｍ':
        readings = letterm;
        return sizeof(letterm) / sizeof(wchar_t*);
    case L'Ｎ':
    case L'ｎ':
        readings = lettern;
        return sizeof(lettern) / sizeof(wchar_t*);
    case L'Ｏ':
    case L'ｏ':
        readings = lettero;
        return sizeof(lettero) / sizeof(wchar_t*);
    case L'Ｐ':
    case L'ｐ':
        readings = letterp;
        return sizeof(letterp) / sizeof(wchar_t*);
    case L'Ｑ':
    case L'ｑ':
        readings = letterq;
        return sizeof(letterq) / sizeof(wchar_t*);
    case L'Ｒ':
    case L'ｒ':
        readings = letterr;
        return sizeof(letterr) / sizeof(wchar_t*);
    case L'Ｓ':
    case L'ｓ':
        readings = letters;
        return sizeof(letters) / sizeof(wchar_t*);
    case L'Ｔ':
    case L'ｔ':
        readings = lettert;
        return sizeof(lettert) / sizeof(wchar_t*);
    case L'Ｕ':
    case L'ｕ':
        readings = letteru;
        return sizeof(letteru) / sizeof(wchar_t*);
    case L'Ｖ':
    case L'ｖ':
        readings = letterv;
        return sizeof(letterv) / sizeof(wchar_t*);
    case L'Ｗ':
    case L'ｗ':
        readings = letterw;
        return sizeof(letterw) / sizeof(wchar_t*);
    case L'Ｘ':
    case L'ｘ':
        readings = letterx;
        return sizeof(letterx) / sizeof(wchar_t*);
    case L'Ｙ':
    case L'ｙ':
        readings = lettery;
        return sizeof(lettery) / sizeof(wchar_t*);
    case L'Ｚ':
    case L'ｚ':
        readings = letterz;
        return sizeof(letterz) / sizeof(wchar_t*);
    case L'〃':
        readings = onajikun;
        return sizeof(onajikun) / sizeof(wchar_t*);
    case L'〇':
        readings = marukun;
        return sizeof(marukun) / sizeof(wchar_t*);
    case L'＆':
        readings = andokun;
        return sizeof(andokun) / sizeof(wchar_t*);
    case L'＠':
        readings = attokun;
        return sizeof(attokun) / sizeof(wchar_t*);
    case L'＋':
        readings = purasukun;
        return sizeof(purasukun) / sizeof(wchar_t*);
    default:
        readings = NULL;
        return 0;
    }
}
bool FuriganaStep(int step, const wchar_t *kanji, const wchar_t *kana, int &kanjistart, int &kanjiend, int &kanastart, int &kanaend, TFuriganaData *dat, int &datpos)
{
    TIntegerList *l;
    TKanji *k;
    int ix, iy;
    bool res = true;
    wchar_t och, kch, tmp;

    switch (step)
    {
    case 1: // Remove kana from begining and ending of word
        ix = 0;
        while (kanjistart <= kanjiend && kanastart <= kanaend && !KANJI(kanji[kanjistart]) && !VALIDCODE(kanji[kanjistart]) && KanaMatch(kanji, kanjistart, kana, kanastart))
            kanjistart++ , kanastart++ , ix++;

        while (kanjistart <= kanjiend && kanastart <= kanaend && kanji[kanjistart] == L'０' && (kanastart == kanaend || HiraganaCh(kana, kanastart) != L'ぜ' || kanastart < kanaend && HiraganaCh(kana, kanastart + 1) != L'ろ'))
            kanjistart++ , /*kanastart++,*/ix++;
        while (kanjistart <= kanjiend && kanastart <= kanaend && (kanji[kanjistart] == L'・' || kanji[kanjistart] == L'．' || kanji[kanjistart] == L'／'))
            kanjistart++ , /*kanastart++,*/ix++;

        if (!kanjistart && !kanjiend && kanastart <= kanaend && datpos == 0)
        {
            dat[datpos].kanji = kanjistart;
            dat[datpos].kanjilen = 1;
            dat[datpos].kana = kanastart;
            dat[datpos].kanalen = kanaend - kanastart + 1;
            kanjistart++;
            kanastart = kanaend + 1;
            datpos++;
            res = true;
            break;
        }

        if (kanjistart <= kanjiend && kanastart <= kanaend)
        {
            res = FuriganaStep(2, kanji, kana, kanjistart, kanjiend, kanastart, kanaend, dat, datpos);
            if (!res && !datpos && ix == kanjistart + ((int)wcslen(kana) - 1 - kanaend) && kanjiend - kanjistart + 1 <= 3)
            {
                res = true;
                for (int i = kanjistart; i <= kanjiend && res; i++)
                    if (!KANJI(kanji[i]) && !VALIDCODE(kanji[i]))
                        res = false;
                if (res)
                {
                    dat[datpos].kanji = kanjistart;
                    dat[datpos].kanjilen = kanjiend - kanjistart + 1;
                    dat[datpos].kana = kanastart;
                    dat[datpos].kanalen = kanaend - kanastart + 1;
                    kanastart = kanaend + 1;
                    kanjistart = kanjiend + 1;
                    datpos++;
                }
            }
        }
        else if (kanjistart <= kanjiend || kanastart <= kanaend)
            res = false;
        break;
    case 2: // Find possible readings for next round.
        ix = -1;
        if ((!VALIDCODE(kanji[kanjistart]) || kanji[kanjistart] == L'々') && (ix = kanjis->IndexOf(kanji[kanjistart - (kanjistart != 0 && kanji[kanjistart] == L'々' ? 1 : 0)])) < 0)
            return false;
        k = ix >= 0 ? kanjis->Kanjis[ix] : NULL;
        l = new TIntegerList;
        try
        {
            byte readingcount = k ? k->oncnt : 0;
            wchar_t **readings = k ? k->on : NULL;
            if (!k)
                readingcount = ValidReadings(kanji[kanjistart], readings);

            for (int i = 0; i < readingcount; i++)
                if (FuReading((readings[i] + (readings[i][0] == L'-' ? 1 : 0)), kana, kanastart, kanaend))
                    if (l->IndexOf(onlen(readings[i])) < 0)
                        l->Add(onlen(readings[i]));

            if (k)
            {
                for (int i = 0; i < k->kuncnt; i++)
                    if (FuReading(k->kun[i], kana, kanastart, kanaend) && l->IndexOf(kunlen(k->kun[i])) < 0)
                        l->Add(kunlen(k->kun[i]));

                for (int i = 0; i < k->irregcnt; i++)
                    if (FuReading(k->irreg[i], kana, kanastart, kanaend) && l->IndexOf(wcslen(k->irreg[i])) < 0)
                        l->Add(wcslen(k->irreg[i]));
            }

            if (l->Count == 1)
            {
                // Ideal case.
                ix = abs(l->Items[0]);

                dat[datpos].kanji = kanjistart;
                dat[datpos].kanjilen = 1;
                dat[datpos].kana = kanastart;
                dat[datpos].kanalen = ix;

                kanastart += ix;
                kanjistart++;
                datpos++;
                if (!(res = FuriganaStep(1, kanji, kana, kanjistart, kanjiend, kanastart, kanaend, dat, datpos)))
                {
                    kanastart -= ix;
                    kanjistart--;
                    datpos--;
                }
            }
            else if (l->Count)
            {
                l->Sort(KunLenSort);
                for (int i = 0; i < l->Count; i++)
                {
                    ix = abs(l->Items[i]);

                    dat[datpos].kanji = kanjistart;
                    dat[datpos].kanjilen = 1;
                    dat[datpos].kana = kanastart;
                    dat[datpos].kanalen = ix;

                    kanastart += ix;
                    kanjistart++;
                    datpos++;
                    if ((res = FuriganaStep(1, kanji, kana, kanjistart, kanjiend, kanastart, kanaend, dat, datpos)) == true)
                        break;
                    kanjistart--;
                    kanastart -= ix;
                    datpos--;
                }
            }
            else
                res = false;
        }
        catch (...)
        {
            ;
        }
        delete l;

        if (!res)
        {
            // Not found. see custom readings.
            for (iy = kanjistart; iy <= kanjiend; iy++)
                if (!KANJI(kanji[iy]) && kanji[iy] != KURIKAESHI)
                    break;

            TWordStoreWords *il = new TWordStoreWords();
            wchar_t *c = new wchar_t[kanaend - kanastart + 2];
            wcsncpy(c, kana + kanastart, kanaend - kanastart + 1);
            c[kanaend - kanastart + 1] = 0;
            try
            {
                for (int i = 0; i < kanaend - kanastart + 1 && !res; i++)
                {
                    tmp = c[i + 1];
                    c[i + 1] = 0;

                    //FindWord(Romanize(c).c_str(),il,true,true,false,false,NULL);
                    store->Fill(Romanize(c).c_str(), il);

                    och = HiraganaCh(c, 0);
                    kch = c[0];
                    if (och >= 0x304b && och < 0x3062 && !(och % 2) || och >= 0x3064 && och < 0x3069 && (och % 2))
                    {
                        (*c)--;
                        store->Fill(Romanize(c).c_str(), il);
                        //FindWord(Romanize(c).c_str(),il,true,true,false,false,NULL);
                    }
                    else if (och >= 0x306f && och < 0x307d && (och % 3))
                    {
                        (*c) -= (och % 3);
                        store->Fill(Romanize(c).c_str(), il);
                        //FindWord(Romanize(c).c_str(),il,true,true,false,false,NULL);
                    }
                    c[0] = kch;

                    c[i + 1] = tmp;
                    //KanLenList = il->collection->words;
                    il->Sort(KanLenSort);
                    for (int j = 0; j < il->Count; j++)
                    {
                        wchar_t *ilkanji = il->Kanji[j];
                        //wchar_t *ilkana = il->Kana[j];

                        if (wcsncmp(ilkanji, kanji + kanjistart, MIN((int)wcslen(ilkanji), kanjiend - kanjistart + 1)))
                            continue;

                        for (ix = iy - kanjistart; ix < MIN((int)wcslen(ilkanji), kanjiend - kanjistart + 1); ++ix)
                            if (KANJI(ilkanji[ix]) || ilkanji[ix] == KURIKAESHI)
                                break;
                        if (ix < MIN((int)wcslen(ilkanji), kanjiend - kanjistart + 1))
                            continue;

                        dat[datpos].kanji = kanjistart;
                        dat[datpos].kanjilen = MIN((int)wcslen(ilkanji), iy - kanjistart);
                        dat[datpos].kana = kanastart;
                        dat[datpos].kanalen = i + 1 - (wcslen(ilkanji) - dat[datpos].kanjilen);

                        kanastart += dat[datpos].kanalen;
                        kanjistart += dat[datpos].kanjilen;
                        datpos++;

                        if ((res = FuriganaStep(1, kanji, kana, kanjistart, kanjiend, kanastart, kanaend, dat, datpos)) == true)
                            break;

                        datpos--;
                        kanjistart -= dat[datpos].kanjilen;
                        kanastart -= dat[datpos].kanalen;
                    }
                    il->Clear();
                }
            }
            catch (...)
            {
                ;
            }
            delete[] c;
            delete il;

        }

        break;
    }
    return res;
}
//---------------------------------------------------------------------------
int FindFurigana(TWord *w, TFuriganaData *&dat)
{
    UnicodeString kanji = w->kanji;
    UnicodeString kana = w->kana;

    int klen = kanji.Length();
    int i;
    for (i = 0; i < klen; ++i)
        if (KANJI(kanji[i + 1]))
            break;

    int datpos = 0;
    if (i == kanji.Length()) // No kanji was found, so no furigana can be shown.
        return 0;

    bool ok;

    int kanjistart = 0;
    int kanastart = 0;

    int kanjiend = klen - 1;
    int kanaend = kana.Length() - 1;

    dat = new TFuriganaData[kanjiend + 1];

    try
    {
        ok = FuriganaStep(1, kanji.c_str(), kana.c_str(), kanjistart, kanjiend, kanastart, kanaend, dat, datpos) && kanjiend < kanjistart; // Remove kana from begining and ending of word.
        if (!ok && FileExists(paths.installdatapath + L"furigana.txt"))
            ok = import_one_furigana((paths.installdatapath + L"furigana.txt").c_str(), kanji.c_str(), kana.c_str(), dat, datpos);
    }
    catch (...)
    {
        ok = false;
    }
    if (!ok)
    {
        delete[] dat;
        dat = NULL;
        datpos = -1;
    }

    return datpos;
}
//---------------------------------------------------------------------------
bool SameWord(TWord *w1, TWord *w2, bool check_o)
{
    if (w1 == w2)
        return true;

    wchar_t *kana1 = w1->kana, *kana2 = w2->kana, *kanji1 = w1->kanji, *kanji2 = w2->kanji;
    if (check_o)
    {
        if ((kana1[0] == L'お' || kana1[0] == L'ご') && KANJI(kanji1[1]) && (kanji1[0] == L'御' || kanji1[0] == L'お' || kanji1[0] == L'ご'))
            kana1++ , kanji1++;
        if ((kana2[0] == L'お' || kana2[0] == L'ご') && KANJI(kanji2[1]) && (kanji2[0] == L'御' || kanji2[0] == L'お' || kanji2[0] == L'ご'))
            kana2++ , kanji2++;
    }

    if (wcscmp(kana1, kana2))
        return false;

    if (!wcscmp(kanji1, kanji2))
        return true;

    wchar_t *k1 = kanji1, *k2 = kanji2;
    bool hask1 = false, hask2 = false;

    while (*k1 || *k2)
    {
        bool K1 = KANJI(*k1), K2 = KANJI(*k2);
        hask1 = hask1 || K1;
        hask2 = hask2 || K2;
        if (K1 && K2)
        {
            if (*k1 != *k2)
                return false;
            k1++;
            k2++;
            continue;
        }

        if (*k1)
            do
            {
                k1++;
            }
            while (*k1 && !KANJI(*k1));

        if (*k2)
            do
            {
                k2++;
            }
            while (*k2 && !KANJI(*k2));
    }
    if ((*k1 == 0) != (*k2 == 0))
        return false;

    if (!hask1 || !hask2)
    {
        if (w1->meaningcnt != w2->meaningcnt)
            return false;

        for (int ix = 0; ix < w1->meaningcnt; ++ix)
        {
            if (wcscmp(w1->data[ix].meaning, w2->data[ix].meaning))
                return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(139);
#endif
