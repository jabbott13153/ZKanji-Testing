//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <ZLib.hpp>

#include "genworddata.h"
#include "collection.h"
#include "words.h"
#include "sentences.h"
#include "romajizer.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#define GENTREE_VERSION       1

#ifdef _DEBUG
static TGlobalTest tester1(50);
#endif

//---------------------------------------------------------------------------
TWordStore *store = NULL;

extern int exampleheadersize;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordStoreWords::TWordStoreWords() : inherited()
{

}
//---------------------------------------------------------------------------
TStoredWord* TWordStoreWords::get_item(int ix)
{
    return store->Items[inherited::Items[ix]];
}
//---------------------------------------------------------------------------
wchar_t* TWordStoreWords::get_kanji(int ix)
{
    return store->Kanji[inherited::Items[ix]];
}
//---------------------------------------------------------------------------
wchar_t* TWordStoreWords::get_kana(int ix)
{
    return store->Kana[inherited::Items[ix]];
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

int StoredWordSortCompare(TStoredWord **a, TStoredWord **b)
{
    return WordCompare((*a)->kanji, (*a)->kana, (*b)->kanji, (*b)->kana);
/*
    TWord *w1 = store->Collection->words->Items[a->wordindex];
    TWord *w2 = store->Collection->words->Items[b->wordindex];

    int r = wcscmp(a->kanji, b->kanji);
    if (!r)
      r = wcscmp(a->kana, b->kana);
    return r;
*/
}

//---------------------------------------------------------------------------

TWordStore::TStoreList::TStoreList(TWordStore *owner) : owner(owner)
{
}
//---------------------------------------------------------------------------
TWordStore::TStoreList::~TStoreList()
{
    Clear();
}
//---------------------------------------------------------------------------
void TWordStore::TStoreList::FreeItem(TStoredWord *item, int ix)
{
    delete[] item->kanji;
    delete[] item->kana;

    delete[] item->examples;
    delete item;
}
//---------------------------------------------------------------------------
/*
void TWordStore::TStoreList::Sort()
{
  inherited::Sort(&StoredWordSortCompare);
}
*/
//---------------------------------------------------------------------------
bool TWordStore::TStoreList::InsertIndex(wchar_t *kanji, wchar_t *kana, int limit, int &index)
{
    int min = 0, max = limit, mid;
    TStoredWord *w;
    while (min <= max)
    {
        mid = (min + max) / 2;
        w = Items[mid];
        int d = WordCompare(kanji, kana, w->kanji, w->kana);
        if (d < 0)
            max = mid - 1;
        else if (d > 0)
            min = mid + 1;
        else
        {
            min = mid;
            index = min;
            return false;
        }
    }

    if (min >= 0 && min <= limit)
    {
        index = min;
        if (min < limit)
        {
            TStoredWord *w = Items[min];
            if (!wcscmp(w->kanji, kanji) && !wcscmp(w->kana, kana))
                return false;
        }
        return true;
    }
    if (min > limit && min <= Count)
    {
        index = min;
        return true;
    }

    THROW(L"Position invalid");
}
//---------------------------------------------------------------------------
int TWordStore::TStoreList::IndexOf(wchar_t *kanji, wchar_t *kana, int limit)
{
    int ix;
    if (Count && !InsertIndex(kanji, kana, limit < 0 ? Count - 1 : limit, ix))
        return ix;
    return -1;
}
//---------------------------------------------------------------------------
int TWordStore::TStoreList::Add(wchar_t *kanji, wchar_t *kana, bool sort)
{
    int ix = -1;
    if (sort)
    {
        if (!Count)
            ix = 0;
        else if (!InsertIndex(kanji, kana, Count - 1, ix))
            return ix;
    }
    TStoredWord *w = new TStoredWord;
    memset(w, 0, sizeof(TStoredWord));
    w->kanji = new wchar_t[wcslen(kanji) + 1];
    wcscpy(w->kanji, kanji);
    w->kana = new wchar_t[wcslen(kana) + 1];
    wcscpy(w->kana, kana);

    if (sort)
    {
        inherited::Insert(ix, w);
        return ix;
    }
    return inherited::Add(w);
}
//---------------------------------------------------------------------------
int TWordStore::TStoreList::Add(TStoredWord *item)
{
    return inherited::Add(item);
}
//---------------------------------------------------------------------------
/*
int TWordStore::TStoreList::SortedIndexOf(wchar_t *kanji, wchar_t *kana)
{
  int min = 0, max = Count - 1, mid;
  TStoredWord *w;
  while (min <= max) {
    mid = (min + max) / 2;
    w = Items[mid];
    int d = WordCompare(kanji, kana, w->kanji, w->kana);
    if (d < 0)
      max = mid - 1;
    else if (d > 0)
      min = mid + 1;
    else {
      min = mid;
      break;
    }
  }

  if (min >= 0 && min < Count) {
    TStoredWord *w = Items[min];
    if (!wcscmp(w->kanji, kanji) && !wcscmp(w->kana, kana))
      return min;
  }
  return -1; //-min - 1;
}
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordStore::TWordStore() : coll(NULL), inherited(false, false)
{
    list = new TStoreList(this);
}
//---------------------------------------------------------------------------
TWordStore::~TWordStore()
{
    Clear();
    delete list;
}
//---------------------------------------------------------------------------
void TWordStore::SaveToFile(FILE *f)
{
    //inherited::SaveToFile(f);
    int c = list->Count;
    int cnt = 0;
    for (int ix = 0; ix < c; ++ix)
    {
        TStoredWord *w = list->Items[ix];
        if (w->jlptnlevel == 0)
            continue;
        else
            ++cnt;
    }

    fwrite(&cnt, sizeof(int), 1, f);
    for (int ix = 0; ix < c; ++ix)
    {
        TStoredWord *w = list->Items[ix];
        if (w->jlptnlevel == 0)
            continue;
        write(w->kanji, f, sltByteAddNULL);
        write(w->kana, f, sltByteAddNULL);
        //fwrite(&w->wordindex, sizeof(int), 1, f);
        fwrite(&w->jlptnlevel, sizeof(byte), 1, f);
    }
}
//---------------------------------------------------------------------------
void TWordStore::LoadFromFile(FILE *f)
{
    bool reload = list->Count != 0; // Word store can reload jlpt data because when the dictionary is updated from the program, there is already data in it.
    if (reload)
        ClearJLPTData();

    int c;
    fread(&c, sizeof(int), 1, f);
    if (!reload)
        list->Capacity = c;

    int cnt = 0;

    wchar_t *kanji = NULL;
    wchar_t *kana = NULL;

    try
    {
        while ((!reload && list->Count != list->Capacity) || (reload && cnt != c))
        {
            TStoredWord *w;

            if (!reload)
            {
                w = new TStoredWord;
                memset(w, 0, sizeof(TStoredWord));
                read(w->kanji, f, sltByteAddNULL);
                read(w->kana, f, sltByteAddNULL);

                fread(&w->jlptnlevel, sizeof(byte), 1, f);
                list->Add(w);
            }
            else
            {
                read(kanji, f, sltByteAddNULL);
                read(kana, f, sltByteAddNULL);
                w = Items[Add(kanji, kana, true)];
                delete[] kanji;
                delete[] kana;
                kanji = NULL;
                kana = NULL;

                fread(&w->jlptnlevel, sizeof(byte), 1, f);

                ++cnt;
            }
        }

        reexpand();
    }
    catch(...)
    {
        delete[] kanji;
        delete[] kana;
        Clear();
        throw;
    }
}
//---------------------------------------------------------------------------
void TWordStore::reexpand()
{
    inherited::Clear();
    for (int ix = 0; ix < list->Count; ++ix)
        Expand(Romanize(list->Items[ix]->kana), ix, false);
}
//---------------------------------------------------------------------------
void TWordStore::ClearJLPTData()
{
    int cnt = list->Count;
    for (int ix = cnt - 1; ix >= 0; --ix)
    {
        TStoredWord *w = list->Items[ix];
        if (w->jlptnlevel == 0)
            continue;
        if (w->examplecnt == 0)
        {
            list->Delete(ix);
            //RemoveLine(ix, ix != cnt - 1);
            //--cnt;
        }
        else
            w->jlptnlevel = 0;
    }
    reexpand();
}
//---------------------------------------------------------------------------
void TWordStore::ClearExampleData()
{
    int cnt = list->Count;
    for (int ix = cnt - 1; ix >= 0; --ix)
    {
        TStoredWord *w = list->Items[ix];
        if (w->examplecnt == 0)
            continue;
        if (w->jlptnlevel == 0)
        {
            list->Delete(ix);
            //RemoveLine(ix, ix != cnt - 1);
            //--cnt;
        }
        else
            w->examplecnt = 0;
    }
    reexpand();
}
//---------------------------------------------------------------------------
UnicodeString TWordStore::DoGetWord(int index)
{
    //TWord *w = Words[list->Items[index]->wordindex];
    //return Romanize(w->kana);
    return Romanize(list->Items[index]->kana);
}
//---------------------------------------------------------------------------
void TWordStore::Clear()
{
    list->Clear();
    inherited::Clear();
}
//---------------------------------------------------------------------------
int TWordStore::count()
{
    return list->Count;
}
//---------------------------------------------------------------------------
int TWordStore::get_cap()
{
    return list->Capacity;
}
//---------------------------------------------------------------------------
void TWordStore::set_cap(int cap)
{
    list->Capacity = cap;
}
//---------------------------------------------------------------------------
TStoredWord* TWordStore::get_item(int ix)
{
    return list->Items[ix];
}
//---------------------------------------------------------------------------
/*
void TWordStore::FromWords(TWordList *words)
{
  for (int ix = 0; ix < words->Count; ++ix) {
//    TStoredWord *sw = new TStoredWord;
//    memset(sw, 0, sizeof(TStoredWord));
//    sw->wordindex = ix;
//    list->Add(sw);
    TWord *w = words->Words[ix];
    list->Add(w->kanji, w->kana);
  }

//  list->Sort();
//  list->Sort(&StoredWordSortCompare);

  for (int ix = 0; ix < Count; ++ix) {
    TStoredWord *sw = list->Items[ix];
    Expand(Romanize(coll->words->Items[sw->wordindex]->kana), ix, true);
  }
}
*/
//---------------------------------------------------------------------------
int TWordStore::Add(wchar_t *kanji, wchar_t *kana, bool sort)
{
    int ix = list->Add(kanji, kana, sort);
    if (sort)
        Expand(Romanize(kana), ix, false);
    return ix;
}
//---------------------------------------------------------------------------
void TWordStore::Fill(wchar_t *search, TIntegerList *l)
{
    if (!search || search[0] == 0)
        return;

    FindLines(search, true, l);
    for (int ix = l->Count - 1; ix >= 0; --ix)
        if (Romanize(Kana[l->Items[ix]]) != search)
            l->Delete(ix);
}
//---------------------------------------------------------------------------
void TWordStore::Sort()
{
    list->Sort(&StoredWordSortCompare);
    reexpand();
}
//---------------------------------------------------------------------------
TStoredWord* TWordStore::FindWord(wchar_t *kanji, wchar_t *kana, int findlimit)
{
    int ix = list->IndexOf(kanji, kana, findlimit);
    if (ix >= 0)
        return Items[ix];
    return NULL;
}
//---------------------------------------------------------------------------
/*
TStoredWord* TWordStore::FindWord(wchar_t *kanji, wchar_t *kana)
{
  TStoredWord *result = NULL;
  TIntegerList *l = new TIntegerList;
  UnicodeString search = Romanize(kana);
  Fill(search.c_str(), l);
  for (int ix = l->Count - 1; ix >= 0 && result == NULL; --ix)
  {
    TStoredWord *w = list->Items[l->Items[ix]];
    if (!wcscmp(kana, w->kana) && !wcscmp(kanji, w->kanji))
      result = w;
  }
  delete l;
  return result;
}
*/
//---------------------------------------------------------------------------
TStoredWord* TWordStore::FindWord(TWord *w, int findlimit)
{
    return FindWord(w->kanji, w->kana, findlimit);
}
//---------------------------------------------------------------------------
int TWordStore::get_index(int ix)
{
    TStoredWord *w = list->Items[ix];
    return FindWordIndex(coll, w->kanji, w->kana);
//return Items[ix]->wordindex;
}
//---------------------------------------------------------------------------
TWord* TWordStore::get_word(int ix)
{
    ix = Indexes[ix];
    if (ix < 0)
        return NULL;
    return coll->words->Items[ix];
//return coll->words->Items[Items[ix]->wordindex];
}
//---------------------------------------------------------------------------
wchar_t* TWordStore::get_kanji(int ix)
{
    return list->Items[ix]->kanji;
//return coll->words->Items[Items[ix]->wordindex]->kanji;
}
//---------------------------------------------------------------------------
wchar_t* TWordStore::get_kana(int ix)
{
    return list->Items[ix]->kana;
//return coll->words->Items[Items[ix]->wordindex]->kana;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(51);
#endif

