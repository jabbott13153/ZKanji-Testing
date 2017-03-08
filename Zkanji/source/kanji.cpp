//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanji.h"

#include "collection.h"
#include "kanjicard.h"
#include "words.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef DEBUG
static TGlobalTest tester1(70);
#endif

int kanjisort(TKanji *a, TKanji *b, TKanjiSortType sorttype)
{
    bool sorted;
    switch (sorttype)
    {
    case stJouyou: // Jouyou grade
        sorted = a->jouyou != b->jouyou;
        if (sorted)
            return (!a->jouyou ? 1 : !b->jouyou ? -1 : a->jouyou - b->jouyou);
    case stNJLPT: // NEW JLPT grade
        sorted = a->NJLPT != b->NJLPT;
        if (sorted)
            return b->NJLPT - a->NJLPT;
    case stJLPTold: // Old JLPT grade
        sorted = a->JLPT != b->JLPT;
        if (sorted)
            return b->JLPT - a->JLPT;
    case stRadical: // Radical
        sorted = a->radical != b->radical;
        if (sorted)
            return a->radical - b->radical;
    case stStrokes: // Number of strokes
        sorted = a->strokes != b->strokes;
        if (sorted)
            return a->strokes - b->strokes;
    case stWordFrequency: // Word frequency
        sorted = a->word_freq != b->word_freq;
        if (sorted)
            return b->word_freq - a->word_freq;
    case stWords: // Words of kanji
        sorted = dictionary->Items[0]->kanjidat[a->index].wordscnt != dictionary->Items[0]->kanjidat[b->index].wordscnt;
        if (sorted)
            return dictionary->Items[0]->kanjidat[b->index].wordscnt - dictionary->Items[0]->kanjidat[a->index].wordscnt;
    case stFrequency: // Frequency
        if (a->frequency && b->frequency)
            return a->frequency - b->frequency;
        else if (a->frequency)
            return -1;
        else if (b->frequency)
            return 1;
    case stUnicode: // Unicode
        return a->ch - b->ch;
    default:
        return a->index - b->index;
    }
}

//---------------------------------------------------------------------------
TKanjiList::TKanjiList()
{
}
//---------------------------------------------------------------------------
TKanjiList::~TKanjiList()
{
}
//---------------------------------------------------------------------------
int TKanjiList::IndexOf(int ix)
{
    for (int i = 0; i < Count; i++)
        if (Kanjis[i]->index == ix)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
void TKanjiList::Clear()
{
    for (int i = Count - 1; i >= 0; i--)
        Delete(i);
}
//---------------------------------------------------------------------------
UnicodeString TKanjiList::get_string()
{
    UnicodeString r = "";
    for (int i = 0; i < Count; i++)
        r += Kanjis[i]->ch;
    return r;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TKanjiItemList::TKanjiItemList(bool allow_duplicate) : flist(new TKanjiItems), fduplicate(allow_duplicate), TKanjiList()
{
    ;
}
//---------------------------------------------------------------------------
TKanjiItemList::~TKanjiItemList()
{
    Clear();
    delete flist;
}
//---------------------------------------------------------------------------
/*
TKanjiItems* TKanjiItemList::get_list()
{
  return flist;
}
*/
//---------------------------------------------------------------------------
TKanji* TKanjiItemList::get_kanji(int ix)
{
    if (ix < 0 || ix >= Count)
        THROW(L"Out of range for kanji in kanji item list!");

    if (!Items[ix])
        return NULL;
    return Items[ix]->kanji;
}
//---------------------------------------------------------------------------
TKanjiItem* TKanjiItemList::get_item(int i)
{
    if (i < 0 || i >= flist->Count)
        return NULL;
    return flist->Items[i];
}
//---------------------------------------------------------------------------
int TKanjiItemList::AddNew(TKanji* fkanji)
{
    if (!fduplicate && IndexOf(fkanji->ch) >= 0)
        return -1;
    TKanjiItem *ki = new TKanjiItem;
    ki->kanji = fkanji;
    ki->instances = 1;
    return flist->Add(ki);
}
//---------------------------------------------------------------------------
void TKanjiItemList::Assign(TKanjiList *alist)
{
    if (alist == this)
        return;
    Clear();
    if (!alist)
        return;
    TKanjiItemList *li = dynamic_cast<TKanjiItemList *>(alist);
    if (li)
        fduplicate = li->fduplicate;
    for (int i = 0; i < alist->Count; i++)
        Add(alist->Kanjis[i]->index);
}
//---------------------------------------------------------------------------
void TKanjiItemList::Add(UnicodeString str)
{
    for (int ix = 1; ix <= str.Length(); ++ix)
        if (KANJI(str[ix]))
            Add(kanjis->IndexOf(str[ix]));
}
//---------------------------------------------------------------------------
int TKanjiItemList::Add(wchar_t c)
{
    return Add(kanjis->IndexOf(c));
}
//---------------------------------------------------------------------------
int TKanjiItemList::Add(int kindex)
{
    if (kindex < 0 || kindex >= kanjis->Count || !fduplicate && IndexOf(kindex) >= 0)
        return -1;

    int r;
    TKanjiItem *ki = kanjis->Items[kindex];
    ki->instances++;
    r = flist->Add(ki);
    return r;
}
//---------------------------------------------------------------------------
int TKanjiItemList::Add(TKanji *k)
{
    return Add(k->index);
}
//---------------------------------------------------------------------------
int TKanjiItemList::IndexOf(const wchar_t c)
{
    for (int i = 0; i < Count; i++)
        if (Kanjis[i]->ch == c)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
void TKanjiItemList::Delete(int i)
{
    Items[i]->instances--;

    if (!Items[i]->instances)
        delete Items[i];
    flist->Delete(i);
}
//---------------------------------------------------------------------------
TKanjiSortType __ksortby;
int kanjiitemsort(TKanjiItem **ai, TKanjiItem **bi)
{
    return kanjisort((*ai)->kanji, (*bi)->kanji, __ksortby);
}
//---------------------------------------------------------------------------
void TKanjiItemList::SortBy(TKanjiSortType i)
{
    __ksortby = i;
    if (i != stUnsorted)
        flist->Sort(kanjiitemsort);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int compact_count(TKanji *k)
{
    if (!k->kuncnt)
        return 1 + k->oncnt;

    int cnt = 1;
    wchar_t *w = k->kun[0];
    wchar_t *p = wcschr(w, L'.');
    if (!p)
        p = wcschr(w, 0);
    for (int i = 1; i < k->kuncnt; i++)
    {
        wchar_t *kun = k->kun[i];
        wchar_t *kp = wcschr(kun, L'.');
        if (!kp || p - w != kp - kun || wcsncmp(w, kun, p - w))
        {
            p = (kp ? kp : wcschr(kun, 0));
            w = kun;
            cnt++;
        }
    }
    return 1 + k->oncnt + cnt;
}
//---------------------------------------------------------------------------
int real_index(TKanji *k, int compactix)
{
    if (!k)
        return -1;

    if (compactix <= k->oncnt)
        return compactix;

    int c = compactix - 1 - k->oncnt;

    wchar_t *w = k->kun[0];
    wchar_t *p = wcschr(w, L'.');
    if (!p)
        p = wcschr(w, 0);
    int i = 1;

    while (c)
    {
        if (i >= k->kuncnt)
            THROW(L"Overindexing in kun reading count!");

        wchar_t *kun = k->kun[i++];
        wchar_t *kp = wcschr(kun, L'.');
        if (!kp || p - w != kp - kun || wcsncmp(w, kun, p - w))
        {
            p = (kp ? kp : wcschr(kun, 0));
            w = kun;
            c--;
            continue;
        }
        compactix++;
    }

    return compactix;
}
//---------------------------------------------------------------------------
int first_index(TKanji *k, int realix)
{
    if (!k)
        return -1;

    if (realix <= k->oncnt + 1)
        return realix;

    int i = realix - 2 - k->oncnt;
    if (i + 1 >= k->kuncnt)
        THROW(L"Overindexing in kun reading count!");

    wchar_t *w = k->kun[i + 1];
    wchar_t *p = wcschr(w, L'.');
    if (!p)
        p = wcschr(w, 0);

    do
    {
        wchar_t *kun = k->kun[i];
        wchar_t *kp = wcschr(kun, L'.');
        if (!kp)
            kp = wcschr(kun, 0);
        if (p - w != kp - kun || wcsncmp(w, kun, p - w))
            break;
        i--;
    }
    while (i >= k->oncnt);

    return i + 2 + k->oncnt;
}
//---------------------------------------------------------------------------
int compact_index(TKanji *k, int realix)
{
    if (!k)
        return -1;

    if (realix >= 1 + k->oncnt + k->kuncnt)
        THROW(L"Overindexing in kun reading count!");

    if (realix <= k->oncnt + 1)
        return realix;

    realix = first_index(k, realix) - k->oncnt - 1;

    int i = 1;
    int cnt = 1;

    wchar_t *w = k->kun[0];
    wchar_t *p = wcschr(w, L'.');
    if (!p)
        p = wcschr(w, 0);

    while (i <= realix)
    {
        wchar_t *kun = k->kun[i++];
        wchar_t *kp = wcschr(kun, L'.');
        if (!kp)
            kp = wcschr(kun, 0);

        if (p - w != kp - kun || wcsncmp(w, kun, p - w))
        {
            cnt++;
            w = kun;
            p = kp;
        }
    };

    return cnt + k->oncnt;
}
//---------------------------------------------------------------------------
word JIStoShiftJIS(word w)
{
    word res = (((w + 0x100) >> 1) & 0xff00) + 0x7000;
    if (res > 0x9f00)
        res += 0x4000;
    if (res & 0x100)
        res |= (w & 0xff) + 0x1f + ((w & 0xff) >= 0x60 ? 1 : 0);
    else
        res |= (w & 0xff) + 0x7e;
    return res;
}
//---------------------------------------------------------------------------
word JIStoEUC(word w)
{
    return (w & 0x7f7f) + 0x8080;
}
//---------------------------------------------------------------------------
UnicodeString JIStoKuten(word w)
{
    return UnicodeString(((w & 0xff00) >> 8) - 0x20) + L"-" + ((w & 0xff) - 0x20);
}
//---------------------------------------------------------------------------

#ifdef DEBUG
static TGlobalTest tester2(71);
#endif
