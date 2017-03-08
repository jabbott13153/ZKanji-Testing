//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanjicard.h"
#include "kanji.h"
#include "zkformats.h"
#include "collection.h"
#include "words.h"
#include "qsrt.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(72);
#endif

//---------------------------------------------------------------------------
TKanjiCard::TKanjiCard(TKanji *k)
{
    fkanji = k;

    firstex = 0;
    fexcount = 0;

    oncnt = k->oncnt;
    kuncnt = compact_count(k) - oncnt - 1;
    realkuncnt = k->kuncnt;
    fexcounts = new word[kuncnt + oncnt + 1];
    memset(fexcounts, 0, sizeof(word)*(kuncnt + oncnt + 1));
    fcheck.checkall(kuncnt + oncnt);
}
//---------------------------------------------------------------------------
TKanjiCard::~TKanjiCard()
{
    while (firstex)
    {
        TExItem *ex = firstex->next;
        delete firstex->addedon;
        delete firstex->example;
        delete firstex;
        firstex = ex;
    }
    delete[] fexcounts;
}
//---------------------------------------------------------------------------
void TKanjiCard::set_checked(byte ix, bool val)
{
    if (ix > oncnt + kuncnt)
        THROW(L"Out of kanji readings range!");

    fcheck[ix - 1] = val;
  //We should set dictionary modify state, but it is done in TReadingBoxObj to user interaction for now.
}
//---------------------------------------------------------------------------
bool TKanjiCard::get_checked(byte ix)
{
    if (ix > oncnt + kuncnt)
        THROW(L"Kanji reading out of range! (code: 4)");

    return fcheck[ix - 1];
}
//---------------------------------------------------------------------------
TKanjiCardExample* TKanjiCard::get_example(int ix)
{
    TExItem *ex = firstex;
    while (ix && ex)
    {
        ex = ex->next;
        ix--;
    }
    if (ex)
        return ex->example;
    throw ERangeError("Example out of range! (code: 1)");
}
//---------------------------------------------------------------------------
word TKanjiCard::get_examplecount(byte reading)
{
    if (reading > oncnt + kuncnt)
        throw ERangeError("Reading out of range! (code: 2)");

    return fexcounts[reading];
}
//---------------------------------------------------------------------------
int TKanjiCard::get_readingexample(byte reading, word example)
{
    if (reading > oncnt + kuncnt)
        THROW(L"Reading out of range! (code: 3)");
    if (example >= fexcounts[reading])
        THROW(L"Example out of range! (code: 5)");

    TExItem *ex = firstex;
    while (ex && ex->example->reading < reading)
        ex = ex->next;

    while (example-- && ex)
        ex = ex->next;

    if (!ex || ex->example->reading != reading)
        THROW(L"Something went wrong! (code: 6)");

    return ex->example->ix;
}
//---------------------------------------------------------------------------
void TKanjiCard::RemoveLine(int wix)
{
    TExItem *ex = firstex;
    TExItem *prev = NULL;

    MODON
    while (ex)
    {
        if (ex->example->ix == wix)
        {
            if (firstex == ex)
                firstex = ex->next;
            if (prev)
                prev->next = ex->next;
            fexcount--;
            fexcounts[ex->example->reading]--;
            delete ex->addedon;
            delete ex->example;
            delete ex;
            ex = (prev ? prev->next : firstex ? firstex->next : NULL);
            continue;
        }
        else if (ex->example->ix > wix)
            ex->example->ix--;
        prev = ex;
        ex = ex->next;
    }
    MODOFF
}
//---------------------------------------------------------------------------
/*char TKanjiCard::get_examplereading(int wix)
{
  TExItem *ex = firstex;

  while (ex && ex->example->ix != wix)
    ex = ex->next;
  if (ex)
    return ex->example->reading;
  return -1;
}*/
//---------------------------------------------------------------------------
int TKanjiCard::IndexOf(TKanjiCardExample *card)
{
    TExItem *ex = firstex;
    int ix = 0;
    while (ex && ex->example != card)
    {
        ex = ex->next;
        ix++;
    }
    if (ex)
        return ix;
    return -1;
}
//---------------------------------------------------------------------------
void TKanjiCard::SelectExample(TKanjiCardItem *item, int wix)
{
    TExItem *ex = firstex;
    while (ex && ex->example->ix != wix)
        ex = ex->next;

    if (!ex)
        THROW(L"Example not on card to select!");

    if (ex->addedon->IndexOf(item) >= 0)
        THROW(L"Example on card already!");

    MODON
    item->examples->Add(ex->example);
    ex->addedon->Add(item);
    MODOFF
}
//---------------------------------------------------------------------------
void TKanjiCard::DeselectExample(TKanjiCardItem *item, int wix)
{
    TExItem *ex = firstex;
    while (ex && ex->example->ix != wix)
        ex = ex->next;

    if (!ex)
        THROW(L"Example not on card to deselect!");

    int i;
    if ((i = ex->addedon->IndexOf(item)) < 0)
        THROW(L"Example not on card yet!");

    MODON
    item->examples->Remove(ex->example);
    ex->addedon->Delete(i);
    MODOFF
}
//---------------------------------------------------------------------------
bool TKanjiCard::ExampleSelected(TKanjiCardItem *item, int wix)
{
    TExItem *ex = firstex;
    while (ex && ex->example->ix != wix)
        ex = ex->next;

    if (!ex)
        THROW(L"Example not on card to be selected!");

    return ex->addedon->IndexOf(item) >= 0;
}
//---------------------------------------------------------------------------
void TKanjiCard::AddExample(byte reading, int wix, bool duplicatethrows)
{
    if (reading > oncnt + kuncnt)
        THROW(L"Reading out of range! (code: 8)");

    TExItem *ex = firstex;
    TExItem *prev = ex;

    // Each word can only appear for one reading.
    while (prev && prev->example->ix != wix)
        prev = prev->next;
    if (prev)
    {
        if (duplicatethrows)
            THROW(L"This word is already added to the the example list of the kanji!");
        return;
    }

    MODON
    if (!ex)
    {
        firstex = new TExItem;
        firstex->example = new TKanjiCardExample;
        firstex->example->reading = reading;
        firstex->example->ix = wix;
        firstex->next = NULL;
        firstex->addedon = new TKanjiCardItems;

        fexcount = 1;
        fexcounts[reading] = 1;
        return;
    }

    while (ex && ex->example->reading < reading)
    {
        prev = ex;
        ex = ex->next;
    }

    while (ex && ex->example->reading == reading)
    {
        if (ex->example->ix == wix)
            return;
        else
            ex = ex->next;
    }

    ex = new TExItem;
    ex->example = new TKanjiCardExample;
    ex->example->reading = reading;
    ex->example->ix = wix;
    ex->next = (prev ? prev->next : firstex);
    ex->addedon = new TKanjiCardItems;

    if (!prev)
        firstex = ex;
    else
        prev->next = ex;

    fexcount++;
    fexcounts[reading]++;
    MODOFF
}
//---------------------------------------------------------------------------
bool TKanjiCard::RemoveExample(int wix)
{
    TExItem *ex = firstex;
    TExItem *prev = NULL;

    while (ex && ex->example->ix != wix)
    {
        prev = ex;
        ex = ex->next;
    }

    if (!ex)
        return false;

    MODON
    if (prev)
        prev->next = ex->next;
    else
        firstex = ex->next;

    fexcounts[ex->example->reading]--;

    for (int i = 0; i < ex->addedon->Count; i++)
        ex->addedon->Items[i]->examples->Remove(ex->example);

    delete ex->addedon;
    delete ex->example;
    delete ex;

    fexcount--;
    MODOFF

            return true;
}
//---------------------------------------------------------------------------
void TKanjiCard::SwapExamples(byte reading, int wix1, int wix2)
{
    if (wix1 == wix2)
        return;

    if (reading > oncnt + kuncnt)
        THROW(L"Reading out of range! (code: 9)");
    if (!firstex)
        THROW(L"No examples to swap! (code: 1)");

    TExItem *ex = firstex;
    TExItem *prev;
    TExItem *prev2;

    if (wix1 > wix2)
        swap(wix1, wix2);

    if (ex->example->reading < reading || ex->example->ix != wix1)
    {
        while (ex->next && ex->next->example->reading <= reading && ex->next->example->ix != wix1)
            ex = ex->next;
        prev = ex;
    }
    else
    {
        prev = new TExItem;
        prev->next = firstex;
    }

    if (!prev->next || prev->next->example->reading != reading)
        THROW(L"No examples to swap! (code: 2)");

    ex = firstex;
    if (ex->example->reading < reading || ex->example->ix != wix2)
    {
        while (ex->next && ex->next->example->reading <= reading && ex->next->example->ix != wix2)
            ex = ex->next;
        prev2 = ex;
    }
    else
    {
        prev2 = new TExItem;
        prev2->next = firstex;
    }

    if (!prev2->next || prev2->next->example->reading != reading)
        THROW(L"No examples to swap! (code: 2)");

    if (prev2->next == prev)
    {
        prev2->next = prev->next;
        prev->next = prev->next->next;
        prev2->next->next = prev;
        if (firstex == prev2->next->next)
        {
            firstex = prev2->next;
            delete prev2;
        }
    }
    else if (prev->next == prev2)
    {
        prev->next = prev2->next;
        prev2->next = prev2->next->next;
        prev->next->next = prev2;
        if (firstex == prev->next->next)
        {
            firstex = prev->next;
            delete prev;
        }
    }
    else
    {
        swap(prev2->next, prev->next);
        swap(prev2->next->next, prev->next->next);
        if (prev2->next == firstex)
        {
            firstex = prev->next;
            delete prev;
        }
        else if (prev->next == firstex)
        {
            firstex = prev2->next;
            delete prev2;
        }
    }

}
//---------------------------------------------------------------------------
void TKanjiCard::SaveToFile(FILE *f)
{
    fwrite(&fexcount, sizeof(int), 1, f);
    TExItem *ex = firstex;
    while (ex)
    {
        fwrite(&ex->example->ix, sizeof(int), 1, f);
        fwrite(&ex->example->reading, sizeof(byte), 1, f);
        ex = ex->next;
    }
    fcheck.SaveToFile(f);
}
//---------------------------------------------------------------------------
static int cmpexampleitemreading(TKanjiCard::TExItem *a, TKanjiCard::TExItem *b)
{
    return a->example->reading - b->example->reading;
}
//---------------------------------------------------------------------------
TIntegerList* TKanjiCard::LoadFromFile(FILE *f, int version, TKanji *k, TConvertItem *ci, int cnt, /* remove after v17 */ TWordCollection *collection)
{
    fread(&fexcount, sizeof(int), 1, f);

    TIntegerList *converter = NULL;

    TExItem *ex;
    TExItem *prev = NULL;
    for (int i = 0; i < fexcount; ++i)
    {
        ex = new TExItem;
        ex->example = new TKanjiCardExample;
        ex->addedon = new TKanjiCardItems;
        ex->next = 0;

        fread(&ex->example->ix, sizeof(int), 1, f);
        fread(&ex->example->reading, sizeof(byte), 1, f);

        if (version < 17 && cnt)
        {
            int min = 0, max = cnt - 1;
            while (min < max)
            {
                int mid = (min + max) / 2;
                if (ci[mid].original < ex->example->ix)
                    min = mid + 1;
                else if (ci[mid].original > ex->example->ix)
                    max = mid - 1;
                else
                    min = max = mid;
            }

            if (ci[min].original != ex->example->ix)
            {
                delete converter;
                converter = NULL;
                THROW(L"Error in datafile! Kanji example not in dictionary!");
            }

            ex->example->ix = ci[min].changed;
        }

        // Remove only this if line after version 17.
        if (ex->example->ix >= 0)
        {
            // Remove the example reading finding after version 17.
            if (version < 17)
            {
                TWord *w = collection->words->Items[ex->example->ix];
                int klen = wcslen(w->kanji);
                for (int ix = 0; ix < klen; ++ix)
                {
                    if (w->kanji[ix] == k->ch)
                    {
                        ex->example->reading = FindReading(ix, w);
                        break;
                    }
                }
            }

            if (prev)
                prev->next = ex;
            if (!firstex)
                firstex = ex;
            fexcounts[ex->example->reading]++;
            prev = ex;
            if (converter)
                converter->Add(i);
        }
        else // Remove if with else after version 17.
        {
            if (!converter)
            {
                converter = new TIntegerList;
                for (int ix = 0; ix < i; ++ix)
                    converter->Add(ix);
            }
            delete ex->example;
            delete ex->addedon;
            delete ex;
            fexcount--;
            i--;
            if (firstex == ex)
                firstex = NULL;
            converter->Add(-1);
        }
    }

    if (fexcount > 1 && version < 17) // Sort examples by reading because they have been modified.
        mergesort<TExItem>(firstex, &cmpexampleitemreading);

    fcheck.LoadFromFile(f);

    return converter;
}
//---------------------------------------------------------------------------
bool TKanjiCard::HasData()
{
    return (fexcount || !fcheck.equals(true));
}
//---------------------------------------------------------------------------
void TKanjiCard::FillWordList(TWordIndexList *il, byte reading)
{
    int cnt = ReadingExampleCount[reading];
    for (int i = 0; i < cnt; i++)
        il->Add(ReadingExample[reading][cnt - i - 1]);
}
//---------------------------------------------------------------------------
void TKanjiCard::CleanUp()
{
    while (RemoveExample(-1));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TKanjiCardList::TKanjiCardList(TWordCollection *acoll) : fcollection(acoll), TKanjiList(), flist(new TKanjiCardItems)
{
}
//---------------------------------------------------------------------------
TKanjiCardList::~TKanjiCardList()
{
    Clear();
    delete flist;
}
//---------------------------------------------------------------------------
TKanjiSortType __kcisortby;
int kanjisort(TKanji *a, TKanji *b, TKanjiSortType sorttype);
int kanjicarditemsort(TKanjiCardItem **ai, TKanjiCardItem **bi)
{
    return kanjisort((*ai)->kanji, (*bi)->kanji, __kcisortby);
}
//---------------------------------------------------------------------------
void TKanjiCardList::SortBy(TKanjiSortType i)
{
    __kcisortby = i;
    if (i != stUnsorted)
        flist->Sort(kanjicarditemsort);
}
//---------------------------------------------------------------------------
int TKanjiCardList::Add(int kindex)
{
    TKanjiCardItem *item = new TKanjiCardItem;
    TKanji *k = kanjis->Kanjis[kindex];
    int r;
    MODON
    if (!collection->kanjidat[kindex].card)
        collection->kanjidat[kindex].card = new TKanjiCard(k);
    item->examples = new TKanjiCardExamples;
    item->kanji = k;
    r = flist->Add(item);
    fcollection->Modify(false);
    MODOFF
    return r;
}
//---------------------------------------------------------------------------
int TKanjiCardList::Add(wchar_t c)
{
    return Add(kanjis->IndexOf(c));
}
//---------------------------------------------------------------------------
int TKanjiCardList::Add(TKanji *k)
{
    return Add(k->index);
}
//---------------------------------------------------------------------------
int TKanjiCardList::Add(TKanjiCardItem *aitem)
{
    int r;
    MODON
    r = Add(aitem->kanji->index);
    TKanjiCardItem *item = Items[r];
    // Copy examples.
    item->examples = new TKanjiCardExamples;
    for (int i = 0; i < item->examples->Count; i++)
        int j = item->examples->Add(aitem->examples->Items[j]);
    fcollection->Modify(false);
    MODOFF

    return r;
}
//---------------------------------------------------------------------------
void TKanjiCardList::Exchange(int i, int j)
{
    flist->Exchange(i, j);
}
//---------------------------------------------------------------------------
void TKanjiCardList::Delete(int i)
{
    delete Items[i]->examples;
    delete Items[i];
    flist->Delete(i);
}
//---------------------------------------------------------------------------
void TKanjiCardList::AddExample(int kix, int wix)
{
    collection->kanjidat[Kanjis[kix]->index].card->SelectExample(Items[kix], wix);
}
//---------------------------------------------------------------------------
void TKanjiCardList::RemoveExample(int kix, int wix)
{
    collection->kanjidat[Kanjis[kix]->index].card->DeselectExample(Items[kix], wix);
}
//---------------------------------------------------------------------------
bool TKanjiCardList::IsExample(int kix, int wix)
{
    for (int i = 0; i < Items[kix]->examples->Count; i++)
        if (Items[kix]->examples->Items[i]->ix == wix)
            return true;
    return false;
}
//---------------------------------------------------------------------------
void TKanjiCardList::RemoveLine(int ix)
{
    for (int i = 0; i < Count; i++)
        if (IsExample(i, ix))
            RemoveExample(i, ix);
}
//---------------------------------------------------------------------------
void TKanjiCardList::LoadFromFile(FILE *f, int version, TGapList<TIntegerList*> *exampleconvert)
{
    byte b;
    bool l;

    word w;
    int k;
    fread(&w, sizeof(word), 1, f);
    Capacity = w;
    for (int j = 0; j < Capacity; j++)
    {
        fread(&w, sizeof(word), 1, f);
        Add(w);
        fread(&b, sizeof(byte), 1, f);
        Items[j]->examples->Capacity = b;
        for (int i = 0; i < b; i++)
        {
            fread(&k, sizeof(int), 1, f);
            if (version >= 17)
            {
                AddExample(j, collection->kanjidat[Kanjis[j]->index].card->Examples[k]->ix);
            }
            else
            {
                TIntegerList *l = exampleconvert->Items[Kanjis[j]->index];
                k = !l ? k : l->Items[k];
                if (k >= 0)
                    AddExample(j, collection->kanjidat[Kanjis[j]->index].card->Examples[k]->ix);
            }
        }

        if (version <= 3)
        { //skip unused data
            fread(&l, sizeof(bool), 1, f);
            if (!l)
            {
                byte cnt;
                fread(&cnt, sizeof(byte), 1, f);
                for (int i = 0; i < cnt; i++)
                {
                    fread(&b, sizeof(byte), 1, f);
                    enum TCardItemType { ctVisible, ctTested, ctHidden, ctNone };
                    fseek(f, b +sizeof(TCardItemType)*7, SEEK_CUR);
                }
            }
        }

    }
}
//---------------------------------------------------------------------------
void TKanjiCardList::SaveToFile(FILE *f)
{
    word w = Count;
    int k;
    fwrite(&w, sizeof(word), 1, f);
    for (int j = 0; j < Count; j++)
    {
        fwrite(&Items[j]->kanji->index, sizeof(word), 1, f);
        byte b = Items[j]->examples->Count;
        fwrite(&b, sizeof(byte), 1, f);
        for (int i = 0; i < b; i++)
        {
            k = collection->kanjidat[Kanjis[j]->index].card->IndexOf(Items[j]->examples->Items[i]);
            fwrite(&k, sizeof(int), 1, f);
        }
    }
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(73);
#endif
