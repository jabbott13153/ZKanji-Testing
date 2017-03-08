//---------------------------------------------------------------------------
#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <string.h>
#include <Zlib.hpp>

#include "collection.h"
#include "words.h"
#include "main.h"
#include "kanji.h"
#include "kanjilist.h"
#include "wordlist.h"
#include "grouplist.h"
#include "popupdict.h"
#include "zkformats.h"
#include "settings.h"
#include "repetition.h"
#include "studylist.h"
#include "searchtree.h"
#include "wordgroupstudy.h"
#include "romajizer.h"
#include "kanjigroup.h"
#include "sentences.h"
#include "kanjiobj.h"
#include "kanjigrid.h"
#include "kanjicard.h"
#include "msgform.h"
#include "fastbuffer.h"
#include "module.h"
#include "genworddata.h"
#include "wordfilter.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester1(18);
#endif

//extern wchar_t Dictionary_Export_Version_String[];
//extern wchar_t Dictionary_Group_Export_Version_Number[];
extern wchar_t Version_String[];

int error_code = 0;

int TWordCollectionList::schanging = 0;
int TWordCollectionList::schangesave = -1;
int TWordCollectionList::sdisabled = 0;
bool TWordCollectionList::ssaving = false;
bool TWordCollectionList::sloading = false;
bool TWordCollectionList::sprofchanged = false;
bool TWordCollectionList::sbackingup = false;
TWordCollectionList* TWordCollectionList::instance = NULL;

//---------------------------------------------------------------------------
//List for words that were changed in the original English dictionary by the user.
//---------------------------------------------------------------------------
TOriginalWordsList::~TOriginalWordsList()
{
    Clear();
}
//---------------------------------------------------------------------------
void TOriginalWordsList::FreeItem(TOriginalWord *elem, int ix)
{
    delete[] elem->kanji;
    delete[] elem->kana;
    delete_word_meanings(elem->meaningcnt, elem->data);
    delete elem;
}
//---------------------------------------------------------------------------
bool TOriginalWordsList::SkipInFile(FILE *f)
{
    int c = 0;
    try
    {
        if (fread(&c, sizeof(int), 1, f) != 1)
            THROW(L"Couldn't read count of elements");
    }
    catch (...)
    {
        return false;
    }
    return c == 0;
}
//---------------------------------------------------------------------------
void TOriginalWordsList::LoadFromFile(FILE *f)
{
    //if (!FileExists(fname))
    //return;

    Clear();

    //FILE *f = _wfopen(fname.c_str(),L"rb");

    //if (!f) // error
    //THROW(L"Error opening originals word file!";

    //int version;
    //TWordIndexList *il = NULL;

    try
    {
        /*
         char tmp[9];
         tmp[8] = 0;
         fread(tmp,sizeof(char),8,f);
         if (strncmp("zorig",tmp,5))
         THROW(L"Bad file format for original words list! Quitting...";
         //version = strtol(tmp+5,0,10); - For future use
         */

        int c;
        fread(&c, sizeof(int), 1, f);
        Capacity = c;
        while (Count != Capacity)
        {
            TOriginalWord *w = new TOriginalWord;
            fread(&c, sizeof(int), 1, f);
            w->change = (TOriginalWordChange)c;
            read(w->kanji, f, sltByteAddNULL);
            read(w->kana, f, sltByteAddNULL);
            fread(&w->meaningcnt, sizeof(byte), 1, f);
            w->data = new TMeaningData[w->meaningcnt];

            for (byte b = 0; b < w->meaningcnt; ++b)
            {
                read(w->data[b].meaning, f, sltWordAddNULL);
                fread(&w->data[b].types, sizeof(int), 1, f);
                fread(&w->data[b].notes, sizeof(int), 1, f);
                fread(&w->data[b].fields, sizeof(byte), 1, f);
                fread(&w->data[b].nametag, sizeof(word), 1, f);
            }
            inherited::Add(w);
        }
        /*
         int fs = 0;
         fread(&fs,sizeof(int),1,f);
         if (fs != ftell(f))
         THROW(L"Corrupt file. No size indicator at end.";

         il = new TWordIndexList(dictionary->Items[0]);
         for (int i = Count - 1; i >= 0; --i) {
         if (FindWordIndex(il, Items[i]->kanji, Items[i]->kana) < 0)
         Delete(i);
         il->Clear();
         }
         */
    }
    catch (...)
    {
        ;
    }
    //fclose(f);
    //delete il;
}
//---------------------------------------------------------------------------
void TOriginalWordsList::SaveToFile(FILE *f)
{
    //if (!Count && !FileExists(fname))
    //return;
    //FILE *f = _wfopen(fname.c_str(),L"wb");

    //if (!f) // error
    //THROW(L"Error opening originals word file!";

    try
    {
        /*char verstr[9];
         sprintf(verstr, "zorig%03i", ORIGINAL_WORDS_VERSION);
         fwrite(verstr,sizeof(char),8,f);
         */
        int c = Count;
        fwrite(&c, sizeof(int), 1, f);

        for (int ix = 0; ix < Count; ++ix)
        {
            TOriginalWord *w = Items[ix];
            c = w->change;
            fwrite(&c, sizeof(int), 1, f);
            write(w->kanji, f, sltByteAddNULL);
            write(w->kana, f, sltByteAddNULL);
            fwrite(&w->meaningcnt, sizeof(byte), 1, f);

            for (byte b = 0; b < w->meaningcnt; ++b)
            {
                write(w->data[b].meaning, f, sltWordAddNULL);
                fwrite(&w->data[b].types, sizeof(int), 1, f);
                fwrite(&w->data[b].notes, sizeof(int), 1, f);
                fwrite(&w->data[b].fields, sizeof(byte), 1, f);
                fwrite(&w->data[b].nametag, sizeof(word), 1, f);
            }
        }

        //int s = ftell(f) + sizeof(int);
        //fwrite(&s,sizeof(int),1,f);
    }
    catch (...)
    {
        ;
    }
    //fclose(f);
}
//---------------------------------------------------------------------------
int TOriginalWordsList::Add(TWord *w, TOriginalWordChange change)
{
    if (change == owcNone)
        THROW(L"Cannot add word with no changes at all to the originals list! This is a bug.");
    int ix = IndexOf(w);
    if ((ix < 0 || Items[ix]->change != owcAdded) && change == owcDeleted)
        THROW(L"Cannot remove item when it was not added by the user!");

    if (ix >= 0 && w)
    {
        if (Items[ix]->change == owcAdded)
        {
            if (change == owcDeleted)
                Delete(ix);
            return ix;
        }

        if (change == owcModified)
        {
            Items[ix]->change = change;
            return ix;
        }

        THROW(L"Invalid record when comparing to the original dictionary! This is a bug!");
    }

    TOriginalWord *ow = new TOriginalWord;
    memset(ow, 0, sizeof(TOriginalWord));

    ow->kanji = new wchar_t[wcslen(w->kanji) + 1];
    wcscpy(ow->kanji, w->kanji);
    ow->kana = new wchar_t[wcslen(w->kana) + 1];
    wcscpy(ow->kana, w->kana);
    ow->change = change;
    ow->meaningcnt = change == owcAdded ? 0 : w->meaningcnt;
    if (ow->meaningcnt)
    {
        ow->data = new TMeaningData[ow->meaningcnt];
        memcpy(ow->data, w->data, sizeof(TMeaningData) * w->meaningcnt);
        for (byte b = 0; b < ow->meaningcnt; ++b)
        {
            ow->data[b].meaning = new wchar_t[wcslen(w->data[b].meaning) + 1];
            wcscpy(ow->data[b].meaning, w->data[b].meaning);
        }
    }

    inherited::Insert(-ix - 1, ow);
    return -ix - 1;
}
//---------------------------------------------------------------------------
void TOriginalWordsList::Revert(int wordindex)
{
    TWord *w = dictionary->Items[0]->words->Items[wordindex];

    int ix = IndexOf(w->kanji, w->kana);
    if (ix < 0)
        return;
    TOriginalWord *orig = Items[ix];

    TGapList<int> pos;
    TGapList<int> posorig;

    if (Revertable(wordindex, pos, posorig) == 1) // The word can be safely reverted if the matching meanings are moved back to their original position.
    {
        int cnt = posorig.TrueCount;
        for (int ix = 0; ix < cnt; ++ix)
        {
            int p = posorig.TruePos[ix];
            int p2 = posorig.TrueItems[ix];
            if (p != p2)
            {
                dictionary->MoveWordMeaning(0, wordindex, p2, p); // This call changes this originals list, but because we haven't changed anything yet, it's ok.
                for (int iy = ix + 1; iy < cnt; ++iy)
                {
                    int p3 = posorig.TruePos[iy];
                    int p4 = posorig.TrueItems[iy];
                    if (p4 > p2 && p4 <= p)
                        --posorig.Items[p3];
                    else if (p4 >= p && p4 < p2)
                        ++posorig.Items[p3];
                }
            }
        }
    }

    delete_word_meanings(w->meaningcnt, w->data);
    w->data = orig->data;
    w->meaningcnt = orig->meaningcnt;
    orig->meaningcnt = 0;
    orig->data = NULL;

    Delete(ix);
}
//---------------------------------------------------------------------------
int TOriginalWordsList::Revertable(int wordindex, TGapList<int> &pos, TGapList<int> &posorig)
{
    TWord *w = dictionary->Items[0]->words->Items[wordindex];

    TOriginalWord *orig = Find(w);
    if (!orig)
        return -1;

    bool missing = false;
    bool badpos = false;


    TWordGroups *g = w->group;
    while (g && !missing)
    {
        missing = g->meaning >= orig->meaningcnt;

        if (!badpos && !missing)
        {
            if (!pos.ItemSet(g->meaning))
            {
                if (posorig.ItemSet(g->meaning) || wcscmp(orig->data[g->meaning].meaning, w->data[g->meaning].meaning))
                {
                    bool found = false;
                    for (int ix = 0; !found && ix < orig->meaningcnt; ++ix)
                    {
                        if (posorig.ItemSet(ix) || ix == g->meaning)
                            continue;

                        if (!wcscmp(orig->data[ix].meaning, w->data[g->meaning].meaning))
                        {
                            found = true;
                            pos.Items[g->meaning] = ix;
                            posorig.Items[ix] = g->meaning;
                        }
                    }
                    if (!found)
                        badpos = true;
                }
                else
                {
                    pos.Items[g->meaning] = g->meaning;
                    posorig.Items[g->meaning] = g->meaning;
                }
            }
        }

        g = g->next;
    }

    if (missing)
        return -1;
    if (badpos)
        return 0;
    return 1;
}
//---------------------------------------------------------------------------
int TOriginalWordsList::IndexOf(TWord *w)
{
    return IndexOf(w->kanji, w->kana);
}
//---------------------------------------------------------------------------
TOriginalWord* TOriginalWordsList::Find(TWord *w)
{
    return Find(w->kanji, w->kana);
}
//---------------------------------------------------------------------------
TOriginalWord* TOriginalWordsList::Find(wchar_t *kanji, wchar_t *kana)
{
    int ix = IndexOf(kanji, kana);
    if (ix < 0)
        return NULL;
    return Items[ix];
}
//---------------------------------------------------------------------------
bool TOriginalWordsList::IsAdded(TWord *w)
{
    TOriginalWord *word = Find(w);
    return word && word->change == owcAdded;
}
//---------------------------------------------------------------------------
bool TOriginalWordsList::IsModified(TWord *w)
{
    TOriginalWord *word = Find(w);
    return word && word->change == owcModified;
}
//---------------------------------------------------------------------------
int TOriginalWordsList::SortCompare(wchar_t *kanji, wchar_t *kana, int comppos)
{
    int r = wcscmp(kanji, Items[comppos]->kanji);
    if (!r)
        r = wcscmp(kana, Items[comppos]->kana);
    return r;
}
//---------------------------------------------------------------------------
int TOriginalWordsList::IndexOf(wchar_t *kanji, wchar_t *kana)
{
    int min = 0, max = Count - 1, mid;
    while (min <= max)
    {
        mid = (min + max) / 2;
        int d = SortCompare(kanji, kana, mid);
        if (d < 0)
            max = mid - 1;
        else if (d > 0)
            min = mid + 1;
        else
        {
            min = mid;
            break;
        }
    }

    if (min >= 0 && min < Count && !wcscmp(Items[min]->kanji, kanji) && !wcscmp(Items[min]->kana, kana))
        return min;
    return -min - 1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//List containing words being changed during a single operation.
//---------------------------------------------------------------------------
TChangedWordList::TChangedWordList(byte acollection) : fcollection(acollection), inherited()
{
}
//---------------------------------------------------------------------------
TChangedWordList::~TChangedWordList()
{
    Clear();
}
//---------------------------------------------------------------------------
void TChangedWordList::FreeItem(TChangedWord *elem, int ix)
{
    delete[] elem->kanji;
    delete[] elem->kana;
    delete elem;
}
//---------------------------------------------------------------------------
int TChangedWordList::Add(const wchar_t *ckanji, const wchar_t *ckana, const TWordChangeType ctype)
{
    TChangedWord *cw = new TChangedWord;
    cw->kanji = new wchar_t[wcslen(ckanji) + 1];
    wcscpy(cw->kanji, ckanji);
    cw->kana = new wchar_t[wcslen(ckana) + 1];
    wcscpy(cw->kana, ckana);
    cw->type = ctype;
    return Add(cw);
}
typedef int(*TWordPosFunc)(bool reset, const wchar_t *kanjia, const wchar_t *kanaa, const wchar_t *kanjib, const wchar_t *kanab);

wchar_t *sort_romanized_w = NULL;

int abccmp(bool reset, const wchar_t *kanjia, const wchar_t *kanaa, const wchar_t *kanjib, const wchar_t *kanab)
{
    //static UnicodeString sc;
    int result;
    UnicodeString dc;
    if (reset)
    {
        UnicodeString sc = RomanizeEx(kanaa);
        delete[] sort_romanized_w;
        sort_romanized_w = new wchar_t[sc.Length() + 1];
        wcscpy(sort_romanized_w, sc.c_str());
    }

    dc = RomanizeEx(kanab);

    result = wcscmp(sort_romanized_w, dc.c_str());
    if (!result)
    {
        int len = wcslen(kanaa);
        for (int ix = 0; ix < len && !result; ++ix)
        {
            bool ka = KATAKANA(kanaa[ix]) || DASH(kanaa[ix]);
            bool kb = KATAKANA(kanab[ix]) || DASH(kanab[ix]);
            if (!ka && kb)
                result = -1;
            else if (ka && !kb)
                result = 1;
        }
    }

    if (kanjia)
    {
        if (!result)
            result = wcslen(kanjia) - wcslen(kanjib);
        if (!result)
            result = wcscmp(kanjia, kanjib);
    }
    return result;
}
//---------------------------------------------------------------------------
int japcmp(bool reset, const wchar_t *kanjia, const wchar_t *kanaa, const wchar_t *kanjib, const wchar_t *kanab)
{
    UnicodeString dc;
    if (reset)
    {
        UnicodeString sc = Hiraganize(kanaa);
        delete[] sort_romanized_w;
        sort_romanized_w = new wchar_t[sc.Length() + 1];
        wcscpy(sort_romanized_w, sc.c_str());
    }
    dc = Hiraganize(kanab);

    int result = wcscmp(sort_romanized_w, dc.c_str());
    if (!result)
    {
        int len = wcslen(kanaa);
        for (int ix = 0; ix < len && !result; ++ix)
        {
            bool ka = KATAKANA(kanaa[ix]) || DASH(kanaa[ix]);
            bool kb = KATAKANA(kanab[ix]) || DASH(kanab[ix]);
            if (!ka && kb)
                result = -1;
            else if (ka && !kb)
                result = 1;
        }
    }

    if (kanjia)
    {
        if (!result)
            result = wcslen(kanjia) - wcslen(kanjib);
        if (!result)
            result = wcscmp(kanjia, kanjib);
    }
    return result;
}
//---------------------------------------------------------------------------
TWordCollection *abcfilter_cmp_collection = NULL;
int abcfilter_prev_ix = -1;
int /*__fastcall*/ abcfilter_cmp_proc(int a, int b)
{
    TWord *wa = abcfilter_cmp_collection->words->Items[a];
    TWord *wb = abcfilter_cmp_collection->words->Items[b];

    TWordPosFunc orderfunc = settings.alphaorder == woAlpha ? abccmp : japcmp;
    int r = orderfunc(a != abcfilter_prev_ix, wa->kanji, wa->kana, wb->kanji, wb->kana);

    abcfilter_prev_ix = a;

    return r;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordCollection::TWordCollection() : modification(wccNone)
{
    memset(&lastwritetime, 0, sizeof(SYSTEMTIME));
    words = new TWordList();
    rtree = new TTextSearchTree(words, false, false, true);
    ktree = new TTextSearchTree(words, true, false, false);
    btree = new TTextSearchTree(words, true, true, false);
    groups = new TWordGroupList(this, false);
    studygroups = new TWordGroupList(this, true);
    abcde = new TIntegerList;
    aiueo = new TIntegerList;
    abcfilter = NULL;
    setfilters = new TWordFilterCopyList;
    deflist = new TGapStringList(false);
    kdeflist = new TGapStringList(false);

    kgroups = new TKanjiGroupList(this);

    kanjidat = new TKanjiData[kanji_count];
    memset(kanjidat, 0, sizeof(TKanjiData)*kanji_count);

    validdat = new TValidData[validcode_count];
    memset(validdat, 0, sizeof(TValidData)*validcode_count);

    kcategory = new TRepetitionCategory;
    wordstudylist = new TWordStudyList(this);

    imgindex = -1;
}
//---------------------------------------------------------------------------
/*
 TWordCollection::TWordCollection(const TWordCollection *orig, bool full) : modification(wccNone)
 {
 lastwritetime = orig->lastwritetime;
 words = new TWordList(orig->words);
 rtree = new TTextSearchTree(this, orig->rtree);
 ktree = new TTextSearchTree(this, orig->ktree);
 btree = new TTextSearchTree(this, orig->btree);
 groups = new TWordGroupList(this, false);
 studygroups = new TWordGroupList(this, true);

 abcde = new TIntegerList;
 aiueo = new TIntegerList;
 abcde->Capacity = orig->abcde->Count;
 while(abcde->Count != orig->abcde->Count)
 abcde->Add(orig->abcde->Items[abcde->Count]);
 aiueo->Capacity = orig->aiueo->Count;
 while(aiueo->Count != orig->aiueo->Count)
 aiueo->Add(orig->aiueo->Items[aiueo->Count]);

 abcfilter = NULL;
 setfilters = new TWordFilterCopyList;
 deflist = new TGapStringList(false);
 kdeflist = new TGapStringList(false);

 while(deflist->TrueCount != orig->deflist->TrueCount)
 deflist->Items[orig->deflist->TruePos[deflist->TrueCount]] = orig->deflist->TrueItems[deflist->TrueCount];
 while(kdeflist->TrueCount != orig->kdeflist->TrueCount)
 kdeflist->Items[orig->kdeflist->TruePos[kdeflist->TrueCount]] = orig->kdeflist->TrueItems[kdeflist->TrueCount];

 kgroups = new TKanjiGroupList(this);

 if (!kanji_count)
 THROW(L"Cannot copy data with no kanji_count loaded.";

 kanjidat = new TKanjiData[kanji_count];
 memset(kanjidat,0,sizeof(TKanjiData)*kanji_count);
 for (int ix = 0; ix < kanji_count; ++ix)
 {
 kanjidat[ix].wordscnt = orig->kanjidat[ix].wordscnt;
 kanjidat[ix].words = new int[kanjidat[ix].wordscnt];
 memcpy(kanjidat[ix].words,orig->kanjidat[ix].words, sizeof(int)*kanjidat[ix].wordscnt);
 }

 validdat = new TValidData[validcode_count];
 memset(validdat,0,sizeof(TValidData)*validcode_count);
 for (int ix = 0; ix < validcode_count; ix++)
 {
 validdat[ix].wordscnt = orig->validdat[ix].wordscnt;
 validdat[ix].words = new int[validdat[ix].wordscnt];
 memcpy(validdat[ix].words,orig->validdat[ix].words, sizeof(int)*validdat[ix].wordscnt);
 }

 kcategory = new TRepetitionCategory;
 wordstudylist = new TWordStudyList(this);

 imgindex = -1;
 }
 */
//---------------------------------------------------------------------------
TWordCollection::~TWordCollection()
{
    // Remove group references of words before freeing them.
    delete groups;
    delete studygroups;

    delete kgroups;
    delete aiueo;
    delete abcde;
    delete abcfilter;
    delete setfilters;

    TWord *w;

    for (int i = words->Count - 1; i >= 0; i--)
    {
        w = words->Items[i];
        delete_word(w);
    }

    words->Clear();

    delete words;
    delete rtree;
    delete ktree;
    delete btree;

    if (kanji_count && kanjidat)
    {
        for (int i = 0; i < kanji_count; i++)
        {
            delete[] kanjidat[i].words;
            delete[] kanjidat[i].meaning;
            delete kanjidat[i].card;
        }
        delete[] kanjidat;
    }

    for (int i = 0; i < validcode_count; ++i)
        delete[] validdat[i].words;
    delete[] validdat;

    delete deflist;
    delete kdeflist;
    delete wordstudylist;
    delete kcategory;
}
//---------------------------------------------------------------------------
void TWordCollection::Modify(bool main)
{
    if (modification == wccMain || modification == wccStartupMain || modification == wccStartupUser)
        return;

    modification = main ? wccMain : wccUser;
}
//---------------------------------------------------------------------------
void TWordCollection::StartupModify(bool main)
{
    if (modification == wccStartupMain)
        return;

    modification = main ? wccStartupMain : wccStartupUser;
}
//---------------------------------------------------------------------------
// Return position in the alphabetic order. Or "-1 - would_be_position" when not present.
int TWordCollection::WordPosition(const wchar_t *kanji, const wchar_t *kana, TWordOrder order)
{
    TIntegerList *l;

    if (order == woFiltered)
        l = abcfilter;
    else if (order == woSelect)
        l = settings.alphaorder == woAlpha ? abcde : aiueo;
    else
        l = order == woAlpha ? abcde : aiueo;

    int mid, min = 0, max = l->Count - 1;
    if (max < 0)
        return -1;

    TWordPosFunc orderfunc = (order == woSelect || order == woFiltered ? settings.alphaorder : order) == woAlpha ? abccmp : japcmp;
    int r;
    TWord *w;
    bool reset = true;

    while (min <= max)
    {
        mid = (max + min) / 2;
        w = words->Items[l->Items[mid]];
        r = orderfunc(reset, kanji, kana, w->kanji, w->kana);
        reset = false;

        if (!r && kanji)
        {
            r = wcscmp(kanji, w->kanji);
        }
        if (!r && mid > min)
        {
            w = words->Items[l->Items[mid - 1]];
            r = orderfunc(false, kanji, kana, w->kanji, w->kana);
            if (!r)
                r = -1;
            else
                r = 0;
        }

        if (r < 0)
            max = mid - 1;
        else if (r > 0)
            min = mid + 1;
        else
            break;
    }

    if (r)
    {
        mid = min;

        if (mid < l->Count)
        {
            w = words->Items[l->Items[mid]];

            if (wcsncmp(Hiraganize(w->kana).c_str(), Hiraganize(kana).c_str(), Hiraganize(kana).Length()))
                mid = -1 - mid;
        }
        else
            mid = -1 - mid;
    }

    return mid;
}
//---------------------------------------------------------------------------
void TWordCollection::ExpandRomajiTree(int wordindex, wchar_t *meaning)
{
    wchar_t *c = meaning;
    wchar_t replace;
    int len = wcslen(c);
    for (int ix = 0; ix <= len; ++ix)
    {
        if (ix == len || wcschr(cdelim, c[ix]))
        {
            replace = c[ix];
            c[ix] = 0;
            if (wcslen(c))
                rtree->ExpandTree(((UnicodeString)c).LowerCase().c_str(), wordindex);
            c[ix] = replace;
            c += ix + 1;
            len -= ix + 1;
            ix = -1;
        }
    }
}
//---------------------------------------------------------------------------
int TWordCollection::AddWord(const wchar_t *kanji, const wchar_t *kana, const wchar_t *meaning, const int type, const int note, const byte field, const word nametag, const word frequency)
{
    int index = -1;
    int wp;
    TWord *w = NULL;
    try
    {
        w = new TWord;
        memset(w, 0, sizeof(TWord));

        w->frequency = frequency;
        w->meaningcnt = 1;
        w->data = new TMeaningData[1];

        w->data[0].meaning = new wchar_t[wcslen(meaning) + 1];
        wcscpy(w->data[0].meaning, meaning);

        int klen = wcslen(kanji);
        if (klen > 255)
            THROW(L"Cannot add word with kanji longer than 255 characters to the dictionary.");
        w->kanji = new wchar_t[klen + 1];
        wcscpy(w->kanji, kanji);
        klen = wcslen(kana);
        if (klen > 255)
            THROW(L"Cannot add word with kanji longer than 255 characters to the dictionary.");
        w->kana = new wchar_t[klen + 1];
        wcscpy(w->kana, kana);
        UnicodeString romaji = Romanize(kana);
        w->romaji = new wchar_t[romaji.Length() + 1];
        wcscpy(w->romaji, romaji.c_str());

        w->data[0].types = type;
        w->data[0].notes = note;
        w->data[0].fields = field;
        w->data[0].nametag = nametag;

        MODON

        index = words->Add(w);

        // Expand trees so they find this word too.
        ExpandRomajiTree(index, w->data[0].meaning);

        int len = wcslen(kanji);

        // Add kanji words.
        int dat;
        for (int i = 0; i < len; i++)
        {
            if (KANJI(kanji[i]))
            {
                dat = kanjis->IndexOf(kanji[i]);
                add_word_line(kanjidat[dat], index);
            }
            else if (VALIDCODE(kanji[i]))
            {
                dat = validindex(kanji[i]);
                add_valid_word_line(validdat[dat], index);
            }
        }

        ktree->ExpandTree(w->romaji, index);
        btree->ExpandTree(w->romaji, index);

        wp = WordPosition(kanji, kana, woAlpha);
        abcde->Insert((wp < 0 ? -1 - wp : wp), words->Count - 1);
        wp = WordPosition(kanji, kana, woAiueo);
        aiueo->Insert((wp < 0 ? -1 - wp : wp), words->Count - 1);
        if (abcfilter && dictionaryfilters->match(words->Items[words->Count - 1]))
        {
            wp = WordPosition(kanji, kana, woFiltered);
            abcfilter->Insert((wp < 0 ? -1 - wp : wp), words->Count - 1);
        }
        Modify(true);
        MODOFF

    }
    catch (...)
    {
        if (w->data)
            delete[] w->data[0].meaning;
        delete[] w->data;
        delete[] w->kanji;
        delete[] w->kana;
        delete[] w->romaji;

        if (index >= 0)
        {
            ktree->RemoveLine(index, false);
            btree->RemoveLine(index, false);
        }

        throw;
    }

    return index;
}
//---------------------------------------------------------------------------
int TWordCollection::AddWord(TWord *w)
{
    int index = -1;

    MODON
    index = words->Add(w);

    // Expand trees so they find this word too.
    for (int ix = 0; ix < w->meaningcnt; ++ix)
        ExpandRomajiTree(index, w->data[ix].meaning);

    int len = wcslen(w->kanji);

    // Add kanji words.
    int dat;
    for (int i = 0; i < len; i++)
    {
        if (KANJI(w->kanji[i]))
        {
            dat = kanjis->IndexOf(w->kanji[i]);
            add_word_line(kanjidat[dat], index);
        }
        else if (VALIDCODE(w->kanji[i]))
        {
            dat = validindex(w->kanji[i]);
            add_valid_word_line(validdat[dat], index);
        }
    }

    ktree->ExpandTree(w->romaji, index);
    btree->ExpandTree(w->romaji, index);

    int wp = WordPosition(w->kanji, w->kana, woAlpha);
    abcde->Insert((wp < 0 ? -1 - wp : wp), words->Count - 1);
    wp = WordPosition(w->kanji, w->kana, woAiueo);
    aiueo->Insert((wp < 0 ? -1 - wp : wp), words->Count - 1);
    if (abcfilter && dictionaryfilters->match(words->Items[words->Count - 1]))
    {
        wp = WordPosition(w->kanji, w->kana, woFiltered);
        abcfilter->Insert((wp < 0 ? -1 - wp : wp), words->Count - 1);
    }
    Modify(true);
    MODOFF

    return index;
}
//---------------------------------------------------------------------------
int TWordCollection::AddWord(TWord *w, char meaning, UnicodeString str)
{
    if (meaning >= 0)
        return AddWord(w->kanji, w->kana, (str.Length() ? str.c_str() : w->data[meaning].meaning), w->data[meaning].types, w->data[meaning].notes, w->data[meaning].fields, w->data[meaning].nametag, w->frequency);

    // Create a copy of w to add to the dictionary.
    int index = -1;
    int wp;
    TWord *d = NULL;
    try
    {
        d = new TWord;
        memset(d, 0, sizeof(TWord));

        d->frequency = w->frequency;
        d->meaningcnt = w->meaningcnt;
        d->data = new TMeaningData[d->meaningcnt];

        d->kanji = new wchar_t[wcslen(w->kanji) + 1];
        wcscpy(d->kanji, w->kanji);
        d->kana = new wchar_t[wcslen(w->kana) + 1];
        wcscpy(d->kana, w->kana);
        d->romaji = new wchar_t[wcslen(w->romaji) + 1];
        wcscpy(d->romaji, w->romaji);

        for (int ix = 0; ix < d->meaningcnt; ++ix)
        {
            d->data[ix].meaning = new wchar_t[wcslen(w->data[ix].meaning) + 1];
            wcscpy(d->data[ix].meaning, w->data[ix].meaning);

            d->data[ix].types = w->data[ix].types;
            d->data[ix].notes = w->data[ix].notes;
            d->data[ix].fields = w->data[ix].fields;
            d->data[ix].nametag = w->data[ix].nametag;
        }
        MODON

        index = words->Add(d);

        // Expand trees so they find this word too.
        for (int ix = 0; ix < d->meaningcnt; ++ix)
            ExpandRomajiTree(index, d->data[ix].meaning);

        int len = wcslen(d->kanji);

        // Add kanji words.
        int dat;
        for (int i = 0; i < len; i++)
        {
            if (KANJI(d->kanji[i]))
            {
                dat = kanjis->IndexOf(d->kanji[i]);
                add_word_line(kanjidat[dat], index);
            }
            else if (VALIDCODE(d->kanji[i]))
            {
                dat = validindex(d->kanji[i]);
                add_valid_word_line(validdat[dat], index);
            }
        }

        ktree->ExpandTree(d->romaji, index);
        btree->ExpandTree(d->romaji, index);

        wp = WordPosition(d->kanji, d->kana, woAlpha);
        abcde->Insert((wp < 0 ? -1 - wp : wp), words->Count - 1);
        wp = WordPosition(d->kanji, d->kana, woAiueo);
        aiueo->Insert((wp < 0 ? -1 - wp : wp), words->Count - 1);
        if (abcfilter && dictionaryfilters->match(words->Items[words->Count - 1]))
        {
            wp = WordPosition(d->kanji, d->kana, woFiltered);
            abcfilter->Insert((wp < 0 ? -1 - wp : wp), words->Count - 1);
        }
        Modify(true);
        MODOFF

    }
    catch (...)
    {
        if (d->data)
            delete[] d->data[0].meaning;
        delete[] d->data;
        delete[] d->kanji;
        delete[] d->kana;
        delete[] d->romaji;

        if (index >= 0)
        {
            ktree->RemoveLine(index, false);
            btree->RemoveLine(index, false);
        }

        throw;
    }

    return index;
}
//---------------------------------------------------------------------------
void TWordCollection::ReplaceWord(int windex, TWord *w, byte *replacecnt, byte **replacers) // USED IN: entry import
{
    TWord *d = words->Items[windex];
    int wp;

    MODON

    if (abcfilter && dictionaryfilters->match(d))
    {
        wp = WordPosition(d->kanji, d->kana, woFiltered);
        if (wp >= 0)
            abcfilter->Delete(wp);
    }
    if (wcscmp(d->kanji, w->kanji) || wcscmp(d->kana, w->kana))
    {
        wp = WordPosition(d->kanji, d->kana, woAlpha);
        abcde->Delete(wp);
        wp = WordPosition(d->kanji, d->kana, woAiueo);
        aiueo->Delete(wp);
        ktree->RemoveLine(windex, false);
        btree->RemoveLine(windex, false);

        ktree->ExpandTree(w->romaji, windex);
        btree->ExpandTree(w->romaji, windex);
        wp = WordPosition(w->kanji, w->kana, woAlpha);
        abcde->Insert((wp < 0 ? -1 - wp : wp), windex);
        wp = WordPosition(w->kanji, w->kana, woAiueo);
        aiueo->Insert((wp < 0 ? -1 - wp : wp), windex);

        TGapList<char> charmatch;
        for (int ix = wcslen(d->kanji) - 1; ix >= 0; --ix)
            if (KANJI(d->kanji[ix]))
                charmatch.Items[(int)d->kanji[ix]] = 1;
            else if (VALIDCODE(d->kanji[ix]))
                charmatch.Items[(int)d->kanji[ix]] = -1;
        for (int ix = wcslen(w->kanji) - 1; ix >= 0; --ix)
        {
            if (KANJI(w->kanji[ix]) || VALIDCODE(w->kanji[ix]))
            {
                if (charmatch.ItemSet((int)w->kanji[ix]))
                    charmatch.Items[(int)w->kanji[ix]] = 0; // Don't clear index, or we might overwrite a perfectly good character.
                else
                    charmatch.Items[(int)w->kanji[ix]] = KANJI(w->kanji[ix]) ? 2 : -2;
            }
        }
        for (unsigned int ix = 0; ix < charmatch.TrueCount; ++ix)
        {
            char match = charmatch.TrueItems[ix];
            wchar_t ch = (wchar_t)charmatch.TruePos[ix];
            if (match == 1) // Must delete word from kanji and its cards.
            {
                int kix = kanjis->IndexOf(ch);
                delete_word_line(kanjidat[kix], windex);
                if (kanjidat[kix].card)
                    kanjidat[kix].card->RemoveLine(windex);
            }
            else if (match == -1) // Must delete word from valid characters list
                delete_valid_word_line(validdat[validindex(ch)], windex);
            else if (match == 2) // Must add word to kanji.
                add_word_line(kanjidat[kanjis->IndexOf(ch)], windex);
            else if (match == -2) // Must add word to valid codes.
                add_valid_word_line(validdat[validindex(ch)], windex);
        }
    }
    rtree->RemoveLine(windex, false);

    for (int ix = 0; ix < w->meaningcnt; ix++)
        ExpandRomajiTree(windex, w->data[ix].meaning);

    w->group = d->group;
    d->group = NULL;
    delete_word(d);
    words->Items[windex] = w;

    bool *matched = new bool[w->meaningcnt];
    memset(matched, 0, sizeof(bool) * w->meaningcnt);

    TWordGroups *g = w->group;
    TWordGroups *prev = NULL;
    while (g)
    {
        bool remove = true;
        for (int ix = 0; ix < replacecnt[g->meaning] && remove; ++ix)
            if (!matched[replacers[g->meaning][ix]])
                remove = false;

        if (remove) // Meaning was deleted in the import. Remove it from all groups. Don't bother lowering the value of other meaning indexes, as they will be replaced.
        {
            if (dynamic_cast<TWordGroupStudy*>(g->group) != NULL)
                ((TWordGroupStudy*)g->group)->Reset();

            int ix = g->group->list->IndexOf(windex, g->meaning);
            g->group->list->Delete(ix);
            if (g == w->group)
                w->group = g->next;
            else
                prev->next = g->next;
            delete g;
            if (prev)
                g = prev->next;
            else
                g = w->group;
            continue;
        }

        bool first = true;
        TWordGroups *nextprev = g;
        TWordGroups *next = g->next;
        for (int ix = 0; ix < replacecnt[g->meaning]; ++ix)
        {
            if (!matched[replacers[g->meaning][ix]])
            {
                if (first)
                {
                    int li = g->group->list->IndexOf(windex, g->meaning);
                    g->meaning = replacers[g->meaning][ix];
                    g->group->list->Items[li]->meaning = -1 - g->meaning;
                    first = false;
                }
                else
                {
                    if (dynamic_cast<TWordGroupStudy*>(g->group) != NULL)
                        ((TWordGroupStudy*)g->group)->Reset();

                    int li = g->group->list->IndexOf(windex, g->meaning);
                    TWordGroups *g2 = new TWordGroups;
                    g2->next = g->next;
                    g2->group = g->group;
                    g2->meaning = replacers[g->meaning][ix];
                    g->next = g2;
                    TWordIndex *i = new TWordIndex;
                    i->index = windex;
                    i->meaning = -1 - g2->meaning;
                    i->inf = NULL;
                    g->group->list->Insert(li + 1, i);

                    nextprev = g2;
                    next = g2->next;
                }
                matched[replacers[g->meaning][ix]] = true;
            }
        }
        for (int ix = 0; ix < g->group->list->Count; ++ix)
            if (g->group->list->Items[ix]->meaning < 0)
                g->group->list->Items[ix]->meaning = -1 - g->group->list->Items[ix]->meaning;

        prev = nextprev;
        g = next;
    }

    delete[] matched;

    if (abcfilter && dictionaryfilters->match(w))
    {
        wp = WordPosition(w->kanji, w->kana, woFiltered);
        abcfilter->Insert((wp < 0 ? -1 - wp : wp), windex);
    }

    MODOFF
}
//---------------------------------------------------------------------------
// "meaning" is the index of meaning in the original word. not needed when you add an entirely new meaning.
byte TWordCollection::CopyWordMeaning(int wix, TWord *w, byte meaning)
{
    return AddWordMeaning(wix, w->data[meaning].meaning, w->data[meaning].types, w->data[meaning].notes, w->data[meaning].fields, w->data[meaning].nametag);
}
//---------------------------------------------------------------------------
byte TWordCollection::AddWordMeaning(int wix, const wchar_t *c, int types, int notes, byte fields, word nametag)
{
    TWord *d = words->Items[wix];

    MODON
    if (d->stats)
    {
        TWordStatGlobal *stmp = d->stats;
        d->stats = new TWordStatGlobal[d->meaningcnt + 1];
        memcpy(d->stats, stmp, sizeof(TWordStatGlobal) * d->meaningcnt);
        memset(d->stats + d->meaningcnt, 0, sizeof(TWordStatGlobal));
        delete[] stmp;
    }

    TMeaningData *dtmp = d->data;
    d->data = new TMeaningData[d->meaningcnt + 1];
    memcpy(d->data, dtmp, sizeof(TMeaningData) * d->meaningcnt);
    delete[] dtmp;

    d->data[d->meaningcnt].meaning = new wchar_t[wcslen(c) + 1];
    wcscpy(d->data[d->meaningcnt].meaning, c);
    d->data[d->meaningcnt].types = types;
    d->data[d->meaningcnt].notes = notes;
    d->data[d->meaningcnt].fields = fields;
    d->data[d->meaningcnt].nametag = nametag;

    ExpandRomajiTree(wix, d->data[d->meaningcnt].meaning);
    d->meaningcnt++;

    Modify(true);
    MODOFF

    return d->meaningcnt - 1;
}
//---------------------------------------------------------------------------
char TWordCollection::WordMeaningIndex(int wordix, const wchar_t *meaning)
{
    TWord *d = words->Items[wordix];
    for (int ix = 0; ix < d->meaningcnt; ++ix)
        if (!wcscmp(meaning, d->data[ix].meaning))
            return ix;
    return -1;
}
//---------------------------------------------------------------------------
void TWordCollection::ReplaceWord(int wordix, TWord *w) // USED IN: dictionary update
{
    TWord *d = words->Items[wordix];
    if (wcscmp(w->kanji, d->kanji) || wcscmp(w->kana, d->kana))
        THROW(L"Can't replace old word with one with different written form and kana.");

    rtree->RemoveLine(wordix, false);
    for (int ix = 0; ix < d->meaningcnt; ++ix)
        delete[] d->data[ix].meaning;
    delete[] d->data;
    delete[] d->stats;
    d->stats = NULL;

    d->meaningcnt = w->meaningcnt;
    d->data = new TMeaningData[w->meaningcnt];
    memcpy(d->data, w->data, sizeof(TMeaningData) * w->meaningcnt);
    /*
     if (w->stats) {
     d->stats = new TWordStatGlobal[w->meaningcnt];
     memcpy(d->stats, w->stats, sizeof(TWordStatGlobal) * w->meaningcnt);
     }
     */
    for (int ix = 0; ix < w->meaningcnt; ix++)
    {
        d->data[ix].meaning = new wchar_t[wcslen(w->data[ix].meaning) + 1];
        wcscpy(d->data[ix].meaning, w->data[ix].meaning);
        ExpandRomajiTree(wordix, w->data[ix].meaning);
    }
    d->frequency = w->frequency;
}
//---------------------------------------------------------------------------
void TWordCollection::DeleteWordMeaning(int wordindex, byte meaning)
{
    TWord *w = words->Items[wordindex];
    if (w->meaningcnt <= 1)
        THROW(L"I shouldn't delete non-existing meanings! (bug)");

    MODON
    // Remove all traces of the meaning from groups.
    groups->RemoveMeaning(wordindex, meaning);
    studygroups->RemoveMeaning(wordindex, meaning);

    TWordGroups* g = w->group;
    while (g)
    {
        if (g->meaning > meaning)
            g->meaning--;
        g = g->next;
    }

    TMeaningData *tmpdata = w->data;
    w->data = new TMeaningData[w->meaningcnt - 1];
    memcpy(w->data, tmpdata, sizeof(TMeaningData)*meaning);
    if (meaning < w->meaningcnt - 1)
        memcpy(w->data + meaning, tmpdata + meaning + 1, sizeof(TMeaningData)*(w->meaningcnt - meaning - 1));
    delete[] tmpdata[meaning].meaning;
    delete[] tmpdata;

    if (w->stats)
    {
        TWordStatGlobal *tmpstats = w->stats;
        w->stats = new TWordStatGlobal[w->meaningcnt - 1];
        memcpy(w->stats, tmpstats, sizeof(TWordStatGlobal)*meaning);
        if (meaning < w->meaningcnt - 1)
            memcpy(w->stats + meaning, tmpstats + meaning + 1, sizeof(TWordStatGlobal)*(w->meaningcnt - meaning - 1));
        delete[] tmpstats;
    }

    w->meaningcnt--;

    rtree->RemoveLine(wordindex);
    for (int ix = 0; ix < w->meaningcnt; ix++)
        ExpandRomajiTree(wordindex, w->data[ix].meaning);

    Modify(true);
    MODOFF
}
//---------------------------------------------------------------------------
void TWordCollection::MoveWordMeaning(int wordindex, byte meaning, byte newpos)
{
    if (meaning == newpos)
        return;

    TWord *w = words->Items[wordindex];
    if (newpos >= w->meaningcnt)
        THROW(L"Moving meaning outside range! (bug)");

    MODON
    groups->MoveMeaning(wordindex, meaning, newpos);
    studygroups->MoveMeaning(wordindex, meaning, newpos);

    TWordGroups* g = w->group;
    while (g)
    {
        if (g->meaning == meaning)
            g->meaning = newpos;
        else if (g->meaning > meaning && g->meaning <= newpos)
            --g->meaning;
        else if (g->meaning < meaning && g->meaning >= newpos)
            ++g->meaning;
        g = g->next;
    }

    TMeaningData tmp = w->data[meaning];
    int cnt = max(meaning, newpos) - min(meaning, newpos);
    memmove(w->data + min(meaning, newpos) + (meaning > newpos ? 1 : 0), w->data + min(meaning, newpos) + (meaning < newpos ? 1 : 0), sizeof(TMeaningData) * cnt);
    w->data[newpos] = tmp;

    if (w->stats)
    {
        TWordStatGlobal tmp2 = w->stats[meaning];
        memmove(w->stats + min(meaning, newpos) + (meaning > newpos ? 1 : 0), w->stats + min(meaning, newpos) + (meaning < newpos ? 1 : 0), sizeof(TWordStatGlobal) * cnt);
        w->stats[newpos] = tmp2;
    }

    Modify(true);
    MODOFF
}
//---------------------------------------------------------------------------
void TWordCollection::DeleteWord(int wordindex)
{
    TWord *w = words->Items[wordindex];

    MODON
    groups->RemoveLine(wordindex);
    studygroups->RemoveLine(wordindex);

    wordstudylist->RemoveLine(wordindex);

    // ------------------------------------------------------------------------------
    // Do anything above this line that requres data from the word in the words list.
    // ------------------------------------------------------------------------------

    words->Delete(wordindex);
    delete_word(w);

    rtree->RemoveLine(wordindex, true);
    ktree->RemoveLine(wordindex, true);
    btree->RemoveLine(wordindex, true);

    for (int ix = abcde->Count - 1; ix >= 0; --ix)
    {
        if (abcde->Items[ix] == wordindex)
            abcde->Delete(ix);
        else if (abcde->Items[ix] > wordindex)
            abcde->Items[ix]--;

        if (aiueo->Items[ix] == wordindex)
            aiueo->Delete(ix);
        else if (aiueo->Items[ix] > wordindex)
            aiueo->Items[ix]--;
    }

    if (abcfilter)
    {
        for (int ix = abcfilter->Count - 1; ix >= 0; --ix)
        {
            if (abcfilter->Items[ix] == wordindex)
                abcfilter->Delete(ix);
            else if (abcfilter->Items[ix] > wordindex)
                abcfilter->Items[ix]--;
        }
    }

    deflist->Delete(wordindex);

    for (int ix = 0; ix < kgroups->Count; ix++)
        kgroups->Items[ix]->RemoveLine(wordindex);

    // Set words found by kanji.
    for (int ix = 0; ix < kanji_count; ++ix)
    {
        for (int j = kanjidat[ix].wordscnt - 1; j >= 0; --j)
        {
            if (kanjidat[ix].words[j] > wordindex)
                kanjidat[ix].words[j]--;
            else if (kanjidat[ix].words[j] == wordindex)
            {
                if (kanjidat[ix].wordscnt - 1 > 0)
                {
                    int *tmpwords = kanjidat[ix].words;
                    kanjidat[ix].words = new int[kanjidat[ix].wordscnt - 1];
                    memcpy(kanjidat[ix].words, tmpwords, sizeof(int)*j);
                    if (j < kanjidat[ix].wordscnt - 1)
                        memcpy(kanjidat[ix].words + j, tmpwords + j + 1, sizeof(int)*(kanjidat[ix].wordscnt - j - 1));
                    delete[] tmpwords;
                }
                else
                {
                    delete[] kanjidat[ix].words;
                    kanjidat[ix].words = NULL;
                }
                kanjidat[ix].wordscnt--;
            }
        }
        if (kanjidat[ix].card)
            kanjidat[ix].card->RemoveLine(wordindex);
    }

    for (int ix = 0; ix < validcode_count; ++ix)
    {
        for (int j = validdat[ix].wordscnt - 1; j >= 0; --j)
        {
            if (validdat[ix].words[j] > wordindex)
                validdat[ix].words[j]--;
            else if (validdat[ix].words[j] == wordindex)
            {
                if (validdat[ix].wordscnt - 1 > 0)
                {
                    int *tmpwords = validdat[ix].words;
                    validdat[ix].words = new int[validdat[ix].wordscnt - 1];
                    memcpy(validdat[ix].words, tmpwords, sizeof(int)*j);
                    if (j < validdat[ix].wordscnt - 1)
                        memcpy(validdat[ix].words + j, tmpwords + j + 1, sizeof(int)*(validdat[ix].wordscnt - j - 1));
                    delete[] tmpwords;
                }
                else
                {
                    delete[] validdat[ix].words;
                    validdat[ix].words = NULL;
                }
                validdat[ix].wordscnt--;
            }
        }
    }

    Modify(true);
    MODOFF
}
//---------------------------------------------------------------------------
void TWordCollection::ChangeWordMeaning(int wordindex, byte meaning, wchar_t *str, int wtype, int wnote, byte wfield, word ntag, word wfrequency)
{
    TWord *w = words->Items[wordindex];
    MODON
    if (str && wcscmp(w->data[meaning].meaning, str) != 0)
    {
        delete[] w->data[meaning].meaning;
        w->data[meaning].meaning = new wchar_t[wcslen(str) + 1];
        wcscpy(w->data[meaning].meaning, str);
        rtree->RemoveLine(wordindex);
        for (int ix = 0; ix < w->meaningcnt; ++ix)
            ExpandRomajiTree(wordindex, w->data[ix].meaning);
    }

    w->data[meaning].types = wtype;
    w->data[meaning].notes = wnote;
    w->data[meaning].fields = wfield;
    w->data[meaning].nametag = ntag;
    if (wfrequency < 10001)
        w->frequency = wfrequency;

    Modify(true);
    MODOFF
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordCollectionList* TWordCollectionList::Create()
{
    if (!instance)
        instance = new TWordCollectionList();
    return instance;
}
//---------------------------------------------------------------------------
void TWordCollectionList::FreeUp()
{
    if (!instance)
        return;
    delete instance;
    instance = NULL;
}
//---------------------------------------------------------------------------
TWordCollectionList::TWordCollectionList() : /*lastsavetime(0),*/ changelist(NULL), backups(NULL)
{
    backups = new TStringList();
    originals = new TOriginalWordsList();
    SettingsChangeEvent(this, settingsChanged, true);
}
//---------------------------------------------------------------------------
__fastcall TWordCollectionList::~TWordCollectionList()
{
    SettingsChangeEvent(this, settingsChanged, false);
    delete backups;
    backups = NULL;
    delete changelist;
    delete originals;
    Clear();
}
//---------------------------------------------------------------------------
void __fastcall TWordCollectionList::Clear()
{
    while (Count)
        Delete(Count - 1);
}
//---------------------------------------------------------------------------
void __fastcall TWordCollectionList::Delete(int ix)
{
    MODON
    delete Items[ix];
    TStringList::Delete(ix);
    MODOFF
}
//---------------------------------------------------------------------------
int __fastcall TWordCollectionList::Add(const UnicodeString aname)
{
    if (IndexOf(aname) >= 0)
        return -1;

    TWordCollection *obj = new TWordCollection();

    int i;

    MODON
    i = TStringList::AddObject(aname, (TObject*)obj);
    MODOFF

    return i;
}
//---------------------------------------------------------------------------
int __fastcall TWordCollectionList::IndexOf(const System::UnicodeString s)
{
    UnicodeString str = s.LowerCase();
    for (int ix = 0; ix < Count; ++ix)
        if (Strings[ix].LowerCase() == str)
            return ix;
    return -1;
}
//---------------------------------------------------------------------------
bool TWordCollectionList::Rename(byte ix, const UnicodeString aname)
{
    int aix = IndexOf(aname);
    if (aix >= 0 && aix != ix)
        return false;

    Strings[ix] = aname;
    return true;
}
//---------------------------------------------------------------------------
TWordCollection* TWordCollectionList::get_items(byte ix)
{
    return (TWordCollection*)Objects[ix];
}
//---------------------------------------------------------------------------
TWordList* TWordCollectionList::get_words(byte ix)
{
    return Items[ix]->words;
}
//---------------------------------------------------------------------------
int TWordCollectionList::WordIndex(byte ix, const wchar_t *kanji, const wchar_t *kana)
{
    int res = -1;
    TWordCollection *wc = Items[ix];
    TWordIndexList *l = new TWordIndexList(wc/*,false*/);
    wc->ktree->FindWord(Romanize(kana).c_str(), l, true, false);
    for (int i = 0; i < l->Count; i++)
        if (!wcscmp(wc->words->Items[l->Indexes[i]]->kanji, kanji) && !wcscmp(wc->words->Items[l->Indexes[i]]->kana, kana))
        { //only the same when the kana and kanji is the same too
            res = l->Indexes[i];
            break;
        }
    delete l;
    return res;
}
//---------------------------------------------------------------------------
/*void TWordCollectionList::LoadOriginals()
 {
 if (originals)
 return;
 originals = new TOriginalWordsList();
 originals->LoadFromFile(datapath + L"English.zko");
 }
 //---------------------------------------------------------------------------
 TOriginalWordsList* TWordCollectionList::get_orig()
 {
 LoadOriginals();
 return originals;
 }*/
//---------------------------------------------------------------------------
int TWordCollectionList::AddWord(byte coll, const wchar_t *kanji, const wchar_t *kana, const wchar_t *meaning, const int type, const int note, const byte field, const word nametag, const word frequency)
{
    if (!changelist)
        THROW(L"No initialization before adding word to dictionary!");
    else if (changelist->CollectionIndex != coll)
        THROW(L"Bad initialization for new word!");

    //if (!coll)
    //LoadOriginals();

    int index = Items[coll]->AddWord(kanji, kana, meaning, type, note, field, nametag, frequency);

    // IMPORTANT: dictupdater.cpp TWordStateList::Commit emulates originals change and other stuff. Make changes there if this code changes.
    if (!coll)
        originals->Add(Items[coll]->words->Items[index], owcAdded);

    /*
     TWordCollection *wc = Items[coll];
     TWord *d = new TWord;

     memset(d,0,sizeof(TWord));
     d->frequency = frequency;
     d->meaningcnt = 1;
     d->data = new TMeaningData[1];

     d->data[0].meaning = new wchar_t[wcslen(meaning)+1];
     wcscpy(d->data[0].meaning,meaning);

     d->kanji = new wchar_t[wcslen(kanji)+1];
     wcscpy(d->kanji,kanji);
     d->kana = new wchar_t[wcslen(kana)+1];
     wcscpy(d->kana,kana);
     d->romaji = new wchar_t[Romanize(kana).Length()+1];
     wcscpy(d->romaji,Romanize(kana).c_str());

     d->data[0].types = type;
     d->data[0].notes = note;
     d->data[0].fields = field;
     d->data[0].nametag = nametag;

     int index;
     CHANGEON
     index = wc->words->Add(d);

     // expand trees so they find this word too
     ExpandRomajiTree(coll,index,d->data[0].meaning);

     int len = wcslen(kanji);

     // add kanji words
     int dat;
     for (int i = 0; i < len; i++)
     if (KANJI(kanji[i])) {
     dat = kanjis->IndexOf(kanji[i]);
     add_word_line(wc->kanjidat[dat],index);
     } else if (VALIDCODE(kanji[i])) {
     dat = validindex(kanji[i]);
     add_valid_word_line(wc->validdat[dat],index);
     }

     wc->ktree->ExpandTree(d->romaji,index);
     wc->btree->ExpandTree(d->romaji,index);

     int wp;
     wp = WordPosition(coll,kanji,kana,woAlpha);
     wc->abcde->Insert((wp < 0 ? -1-wp : wp) ,wc->words->Count-1);
     wp = WordPosition(coll,kanji,kana,woAiueo);
     wc->aiueo->Insert((wp < 0 ? -1-wp : wp) ,wc->words->Count-1);
     if (wc->abcfilter && dictionaryfilters->match(wc->words->Items[wc->words->Count-1])) {
     wp = WordPosition(coll,kanji,kana,woFiltered);
     wc->abcfilter->Insert((wp < 0 ? -1-wp : wp) ,wc->words->Count-1);
     }
     CHANGEOFF
     */
    changelist->Add(kanji, kana, wctAdd);

    return index;
}
//---------------------------------------------------------------------------
int TWordCollectionList::AddWord(byte coll, TWord *w, char meaning, UnicodeString str)
{
    if (!changelist)
        THROW(L"No initialization before adding word to dictionary!");
    else if (changelist->CollectionIndex != coll)
        THROW(L"Bad initialization for new word!");

    int index = Items[coll]->AddWord(w, meaning, str);

    // IMPORTANT: dictupdater.cpp TWordStateList::Commit emulates originals change and other stuff. Make changes there if this code changes.
    if (!coll)
        originals->Add(Items[coll]->words->Items[index], owcAdded);

    //return AddWord(ix, w->kanji, w->kana, (str.Length() ? str.c_str() : w->data[meaning].meaning), w->data[meaning].types, w->data[meaning].notes, w->data[meaning].fields, w->data[meaning].nametag, w->frequency);

    changelist->Add(w->kanji, w->kana, wctAdd);

    return index;
}
//---------------------------------------------------------------------------
int TWordCollectionList::AddWord(byte coll, TWord *w)
{
    if (!changelist)
        THROW(L"No initialization before adding word to dictionary!");
    else if (changelist->CollectionIndex != coll)
        THROW(L"Bad initialization for new word!");

    int index = Items[coll]->AddWord(w);

    // IMPORTANT: dictupdater.cpp TWordStateList::Commit emulates originals change and other stuff. Make changes there if this code changes.
    if (!coll)
        originals->Add(Items[coll]->words->Items[index], owcAdded);

    changelist->Add(w->kanji, w->kana, wctAdd);
    return index;
}
//---------------------------------------------------------------------------
void TWordCollectionList::ReplaceWord(byte coll, int windex, TWord *w, byte *replacecnt, byte **replacers)
{
    if (!changelist)
        THROW(L"No initialization before replacing word to dictionary!");
    else if (changelist->CollectionIndex != coll)
        THROW(L"Bad initialization for replacing word!");
    else if (!coll)
    {
        TWord *d = Items[coll]->words->Items[windex];
        if (wcscmp(d->kanji, w->kanji) || wcscmp(d->kana, w->kana))
            THROW(L"Can't replace word in the base dictionary, because deleting base words is not supported.");
    }

    Items[coll]->ReplaceWord(windex, w, replacecnt, replacers);

    // IMPORTANT: dictupdater.cpp TWordStateList::Commit emulates originals change and other stuff. Make changes there if this code changes.
    if (!coll)
        originals->Add(Items[coll]->words->Items[windex], owcModified);

    changelist->Add(w->kanji, w->kana, wctChange);
}
//---------------------------------------------------------------------------
byte TWordCollectionList::CopyWordMeaning(byte coll, int wix, TWord *w, byte meaning)
{
    if (!changelist)
        THROW(L"No initialization before adding word meaning to dictionary!");
    else if (changelist->CollectionIndex != coll)
        THROW(L"Bad initialization for new word meaning!");

    TWordCollection *wc = Items[coll];
    TWord *d = wc->words->Items[wix];

    if (!coll)
        originals->Add(d, owcModified);

    byte mindex = wc->CopyWordMeaning(wix, w, meaning);

    changelist->Add(d->kanji, d->kana, wctAddMeaning);

    return mindex;
}
//---------------------------------------------------------------------------
byte TWordCollectionList::AddWordMeaning(byte coll, int wix, const wchar_t *c, int types, int notes, byte fields, word nametag)
{
    if (!changelist)
        THROW(L"No initialization before adding word meaning to dictionary!");
    else if (changelist->CollectionIndex != coll)
        THROW(L"Bad initialization for new word meaning!");

    TWordCollection *wc = Items[coll];
    TWord *d = wc->words->Items[wix];

    if (!coll)
        originals->Add(d, owcModified);

    byte mindex = wc->AddWordMeaning(wix, c, types, notes, fields, nametag);

    changelist->Add(d->kanji, d->kana, wctAddMeaning);

    return mindex;
}
//---------------------------------------------------------------------------
char TWordCollectionList::WordMeaningIndex(byte coll, int wordix, const wchar_t *meaning)
{
    TWordCollection *wc = Items[coll];
    return wc->WordMeaningIndex(wordix, meaning);
}
//---------------------------------------------------------------------------
void find_kanji_words(TWordCollection *coll, int *kanjiindex)
{
    //if (dictionary->Count)
    //THROW(L"Can't change kanji count when dictionaries are already in the program!";

    if (kanji_count && kanji_count != kanjis->Count)
        THROW(L"Different kanji count for dictionary!");

    coll->kanjidat = new TKanjiData[kanji_count];
    memset(coll->kanjidat, 0, sizeof(TKanjiData)*kanji_count);

    TWord *w;

    for (int i = 0; i < coll->words->Count; i++)
    {
        w = coll->words->Items[i];
        for (unsigned int j = 0; j < wcslen(w->kanji); j++)
        {
            if (KANJI(w->kanji[j]))
                add_word_line(coll->kanjidat[kanjiindex[w->kanji[j] - 0x4e00]], i);
            if (VALIDCODE(w->kanji[j]))
                add_valid_word_line(coll->validdat[validindex(w->kanji[j])], i);
        }
    }
    delete[] kanjiindex;
}
//---------------------------------------------------------------------------
void TWordCollectionList::DeleteWordMeaning(byte coll, int wordindex, byte meaning)
{
    if (!changelist)
        THROW(L"No initialization before deleting word meaning!");
    else if (changelist->CollectionIndex != coll)
        THROW(L"Bad initialization for deleting word meaning!");

    TWordCollection *wc = Items[coll];
    TWord *d = wc->words->Items[wordindex];

    if (!coll)
    {
        //LoadOriginals();
        originals->Add(d, owcModified);
    }

    wc->DeleteWordMeaning(wordindex, meaning);

    /*
     if (w->meaningcnt <= 1)
     THROW(L"I shouldn't delete non-existing meanings! (bug)";

     int wix;
     CHANGEON
     // remove all traces of the meaning from sublists
     Items[coll]->groups->RemoveMeaning(wordindex, meaning);
     Items[coll]->studygroups->RemoveMeaning(wordindex, meaning);

     TWordGroups* g = w->group;
     while (g) {
     if (g->meaning > meaning) g->meaning--;
     g = g->next;
     }

     TMeaningData *tmpdata = w->data;
     w->data = new TMeaningData[w->meaningcnt-1];
     memcpy(w->data,tmpdata,sizeof(TMeaningData)*meaning);
     if (meaning < w->meaningcnt-1)
     memcpy(w->data+meaning,tmpdata+meaning+1,sizeof(TMeaningData)*(w->meaningcnt-meaning-1));
     delete[]  tmpdata[meaning].meaning;
     delete[]  tmpdata;

     if (w->stats) {
     TWordStatGlobal *tmpstats = w->stats;
     w->stats = new TWordStatGlobal[w->meaningcnt-1];
     memcpy(w->stats,tmpstats,sizeof(TWordStatGlobal)*meaning);
     if (meaning < w->meaningcnt-1)
     memcpy(w->stats+meaning,tmpstats+meaning+1,sizeof(TWordStatGlobal)*(w->meaningcnt-meaning-1));
     delete[]  tmpstats;
     }

     w->meaningcnt--;

     Items[coll]->rtree->RemoveLine(wordindex);
     for (int ix = 0; ix < w->meaningcnt; ix++)
     Items[coll]->ExpandRomajiTree(wordindex,w->data[ix].meaning);

     CHANGEOFF
     */

    changelist->Add(d->kanji, d->kana, wctDeleteMeaning);

}
//---------------------------------------------------------------------------
void TWordCollectionList::MoveWordMeaning(byte coll, int wordindex, byte meaning, byte newpos)
{
    if (!changelist)
        THROW(L"No initialization before moving word meaning!");
    else if (changelist->CollectionIndex != coll)
        THROW(L"Bad initialization for moving word meaning!");

    if (meaning == newpos)
        return;

    TWordCollection *wc = Items[coll];
    TWord *d = wc->words->Items[wordindex];

    // IMPORTANT: dictupdater.cpp TWordStateList::Commit emulates originals change and other stuff. Make changes there if this code changes.
    if (!coll)
        originals->Add(d, owcModified);

    wc->MoveWordMeaning(wordindex, meaning, newpos);

    changelist->Add(d->kanji, d->kana, wctMoveMeaning);

}
//---------------------------------------------------------------------------
void TWordCollectionList::DeleteWord(byte coll, int wordindex)
{
    if (!changelist)
    {
        THROW(L"No initialization before deleting word!");
    }
    else if (changelist->CollectionIndex != coll)
        THROW(L"Bad initialization for deleting word!");

    TWord *w = Items[coll]->words->Items[wordindex];
    UnicodeString kj = w->kanji;
    UnicodeString kk = w->kana;

    // IMPORTANT: dictupdater.cpp TWordStateList::Commit emulates originals change and other stuff. Make changes there if this code changes.
    if (!coll)
    {
        //LoadOriginals();
        originals->Add(w, owcDeleted);
    }

    Items[coll]->DeleteWord(wordindex);

    changelist->Add(kj.c_str(), kk.c_str(), wctDelete);
    /*
     int wix;

     CHANGEON
     Items[coll]->groups->RemoveLine(wordindex);
     Items[coll]->studygroups->RemoveLine(wordindex);

     Items[coll]->wordstudylist->RemoveLine(wordindex);
     // Do anything above this line that requres data from the word in the words list!
     Items[coll]->words->Delete(wordindex);

     changelist->Add(w->kanji, w->kana, wctDelete);
     delete_word(w);

     Items[coll]->rtree->RemoveLine(wordindex,true);
     Items[coll]->ktree->RemoveLine(wordindex,true);
     Items[coll]->btree->RemoveLine(wordindex,true);

     for (int ix = Items[coll]->abcde->Count-1; ix >= 0; --ix) {
     if ((int)Items[coll]->abcde->Items[ix] == wordindex) {
     Items[coll]->abcde->Delete(ix);
     } else if ((int)Items[coll]->abcde->Items[ix] > wordindex) {
     Items[coll]->abcde->Items[ix]--;
     }

     if ((int)Items[coll]->aiueo->Items[ix] == wordindex) {
     Items[coll]->aiueo->Delete(ix);
     } else if ((int)Items[coll]->aiueo->Items[ix] > wordindex) {
     Items[coll]->aiueo->Items[ix]--;
     }
     }

     if (Items[coll]->abcfilter) {
     for (int ix = Items[coll]->abcfilter->Count-1; ix >= 0; --ix) {
     if ((int)Items[coll]->abcfilter->Items[ix] == wordindex) {
     Items[coll]->abcfilter->Delete(ix);
     } else if ((int)Items[coll]->abcfilter->Items[ix] > wordindex) {
     Items[coll]->abcfilter->Items[ix]--;
     }
     }
     }

     Items[coll]->deflist->Delete(wordindex);


     for (int ix = 0; ix < Items[coll]->kgroups->Count; ix++)
     Items[coll]->kgroups->Items[ix]->RemoveLine(wordindex);

     // set words found by kanji
     for (int ix = 0; ix < kanji_count; ++ix) {
     for (int j = Items[coll]->kanjidat[ix].wordscnt-1; j >= 0; --j) {
     if (Items[coll]->kanjidat[ix].words[j] > wordindex)
     Items[coll]->kanjidat[ix].words[j]--;
     else if (Items[coll]->kanjidat[ix].words[j] == wordindex) {
     if (Items[coll]->kanjidat[ix].wordscnt-1 > 0) {
     int *tmpwords = Items[coll]->kanjidat[ix].words;
     Items[coll]->kanjidat[ix].words = new int[Items[coll]->kanjidat[ix].wordscnt-1];
     memcpy(Items[coll]->kanjidat[ix].words,tmpwords,sizeof(int)*j);
     if (j < Items[coll]->kanjidat[ix].wordscnt-1)
     memcpy(Items[coll]->kanjidat[ix].words+j,tmpwords+j+1,sizeof(int)*(Items[coll]->kanjidat[ix].wordscnt-j-1));
     delete[]  tmpwords;
     } else {
     delete[]  Items[coll]->kanjidat[ix].words;
     Items[coll]->kanjidat[ix].words = NULL;
     }
     Items[coll]->kanjidat[ix].wordscnt--;
     }
     }
     if (Items[coll]->kanjidat[ix].card)
     Items[coll]->kanjidat[ix].card->RemoveLine(wordindex);
     }

     for (int ix = 0; ix < validcode_count; ++ix) {
     for (int j = Items[coll]->validdat[ix].wordscnt-1; j >= 0; --j) {
     if (Items[coll]->validdat[ix].words[j] > wordindex)
     Items[coll]->validdat[ix].words[j]--;
     else if (Items[coll]->validdat[ix].words[j] == wordindex) {
     if (Items[coll]->validdat[ix].wordscnt-1 > 0) {
     int *tmpwords = Items[coll]->validdat[ix].words;
     Items[coll]->validdat[ix].words = new int[Items[coll]->validdat[ix].wordscnt-1];
     memcpy(Items[coll]->validdat[ix].words,tmpwords,sizeof(int)*j);
     if (j < Items[coll]->validdat[ix].wordscnt-1)
     memcpy(Items[coll]->validdat[ix].words+j,tmpwords+j+1,sizeof(int)*(Items[coll]->validdat[ix].wordscnt-j-1));
     delete[]  tmpwords;
     } else {
     delete[]  Items[coll]->validdat[ix].words;
     Items[coll]->validdat[ix].words = NULL;
     }
     Items[coll]->validdat[ix].wordscnt--;
     }
     }
     }

     CHANGEOFF
     */
}
//---------------------------------------------------------------------------
/*void TWordCollectionList::ExpandRomajiTree(byte coll, int wordindex, wchar_t *meaning)
 {
 wchar_t *c = meaning;
 wchar_t replace;
 int len = wcslen(c);
 for (int ix = 0; ix <= len; ++ix) {
 if (ix == len || wcschr(cdelim,c[ix])) {
 replace = c[ix];
 c[ix] = 0;
 if (wcslen(c))
 Items[coll]->rtree->ExpandTree(((UnicodeString)c).LowerCase().c_str(),wordindex);
 c[ix] = replace;
 c += ix+1;
 len -= ix+1;
 ix = -1;
 }
 }
 }*/
//---------------------------------------------------------------------------
/*
bool BackupNeeded(const UnicodeString &ddate) // Check last write date of existing data files and return true if there is some difference, or a file is missing. ddate is the folder name in the backup to check.
{
    UnicodeString basepath = FullPathFromBackupPath(settings.bckpath) + L"\\backup\\" + ddate;
    if (!DirectoryExists(basepath))
        return true;
    HANDLE fhandle = 0;
    FILETIME time;

    basepath += L"\\";
    if (!FileExists(basepath + L"zkanji.zsp")
        return true;
    for (int ix = 0; ix < dictionary->Count; ++ix)
    {
        UnicodeString dname = basepath + dictionary->Strings[ix] + L".zkd";
        SetLastError(0);
        if ((fhandle = CreateFile(dname.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) != INVALID_HANDLE_VALUE)
        {
            GetFileTime(fhandle, &time, NULL, NULL);
            CloseHandle(fhandle);
            fhandle = 0;
        }
        else if (GetLastError() == ERROR_FILE_NOT_FOUND)
            return false;

        UnicodeString gname = basepath + dictionary->Strings[ix] + L".zkg";
        SetLastError(0);
        if ((fhandle = CreateFile(gname.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) != INVALID_HANDLE_VALUE)
        {
            GetFileTime(fhandle, &time, NULL, NULL);
            CloseHandle(fhandle);
            fhandle = 0;
        }
        else if (GetLastError() == ERROR_FILE_NOT_FOUND)
            return false;
    }
}
*/
//---------------------------------------------------------------------------
/*
struct TFolderSizeData
{
    UnicodeString name;
    DWORD size; // Set to MAXUINT in case the size of the folder would be over that limit.
    TFolderSizeData(wchar_t *name, DWORD size) : name(name), size(size) {}
};
typedef TTemplateList<TFolderSizeData, true> TFolderSizeDataList;
//---------------------------------------------------------------------------
static int _sort_backup_folders_by_name(TFolderSizeData **a, TFolderSizeData **b)
{
    return wcscmp((*a)->name.c_str(), (*b)->name.c_str());
}
//---------------------------------------------------------------------------
// Sets size to the sum of file sizes inside path. Returns false on error.
static bool BackupFolderSize(const UnicodeString &path, DWORD &size)
{
    size = 0;
    WIN32_FIND_DATA dat;
    HANDLE h = FindFirstFile(path.c_str(), &dat);
    if (h == INVALID_HANDLE_VALUE)
        return GetLastError() == ERROR_NO_MORE_FILES;

    try
    {
        do
        {
            if (size == MAXUINT || (dat.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) == FILE_ATTRIBUTE_REPARSE_POINT)
                continue;
            if ((dat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
            {
                DWORD dsiz;
                if (BackupFolderSize(path + dat.cFileName, dsiz))
                {
                    if (dsiz == MAXUINT || size + dsiz < size || size + dsiz >= MAXUINT) // Lazy check for overflow.
                        size = MAXUINT;
                    else
                        size += dsiz;
                }
                continue;
            }

            if (dat.nFileSizeHigh != 0 || size + dat.nFileSizeLow < size || size + dat.nFileSizeLow >= MAXUINT) // Lazy check for overflow.
                size = MAXUINT;
            else
                size += dat.nFileSizeLow;
        }
        while(FindNextFile(h, &dat) != FALSE);
    }
    catch(...)
    {
        FindClose(h);
        return false;
    }

    int err = GetLastError();
    FindClose(h);

    return err == ERROR_NO_MORE_FILES;
}
//---------------------------------------------------------------------------
// Fills the list with folders and the size they take up. Returns false on error.
static bool ListBackupFolderSizes(const UnicodeString &path, TFolderSizeDataList &list)
{
    WIN32_FIND_DATA dat;
    HANDLE h = FindFirstFile(path.c_str(), &dat);
    if (h == INVALID_HANDLE_VALUE)
        return GetLastError() == ERROR_NO_MORE_FILES;
    try
    {
        do
        {
            if ((dat.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) == FILE_ATTRIBUTE_REPARSE_POINT ||
                (dat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY ||
                wcslen(dat.cFileName) != 8)
                continue;

            list.Add(new TFolderSizeData(dat.cFileName, dat.nFileSizeHigh != 0 ? MAXUINT : dat.nFileSizeLow));
        }
        while(FindNextFile(h, &dat) != FALSE);
    }
    catch(...)
    {
        FindClose(h);
        return false;
    }
    int err = GetLastError();
    FindClose(h);

    if (err == ERROR_NO_MORE_FILES)
    {
        for (int ix = list.Count - 1; ix >= 0; --ix)
        {
            if (!BackupFolderSize(ExtractFilePath(path) + list[ix]->name, list[ix]->size))
                list.Delete(ix);
        }
    }

    return err == ERROR_NO_MORE_FILES;
}
//---------------------------------------------------------------------------
static DWORD BackupFileSize(const UnicodeString &file) // Returns MAXUINT on error.
{
    LARGE_INTEGER dsiz;
    memset(&dsiz, 0, sizeof(LARGE_INTEGER));
    HANDLE fhandle;
    SetLastError(0);
    if ((fhandle = CreateFile(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) != INVALID_HANDLE_VALUE)
    {
        GetFileSizeEx(fhandle, &dsiz);
        CloseHandle(fhandle);
    }
    else if (GetLastError() == ERROR_FILE_NOT_FOUND)
        return MAXUINT;

    if (dsiz.HighPart != 0)
        return MAXUINT;
    return dsiz.LowPart;
}
//---------------------------------------------------------------------------
static void BackupDeleteFolder(const UnicodeString &folder)
{
    SHFILEOPSTRUCT op;
    memset(&op, 0, sizeof(SHFILEOPSTRUCT));
    op.wFunc = FO_DELETE;
    UnicodeString dnullfile = folder;
    dnullfile.SetLength(dnullfile.Length() + 2);
    dnullfile[dnullfile.Length()] = 0;
    dnullfile[dnullfile.Length() - 1] = 0;
    op.pFrom = dnullfile.c_str();
    op.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI | FOF_SILENT;

    SHFileOperation(&op);
}
*/
//---------------------------------------------------------------------------
int __fastcall _custom_sort_backup_folders_proc(TStringList *list, int a, int b)
{
    return wcscmp(list->Strings[a].c_str(), list->Strings[b].c_str());
}
//---------------------------------------------------------------------------
void TWordCollectionList::ReadBackup()
{
    if (backups == NULL)
        return;

    UnicodeString path = FullPathFromBackupPath(settings.bckpath) + L"\\backup\\*";
    backups->Clear();

    WIN32_FIND_DATA dat;
    HANDLE h = FindFirstFile(path.c_str(), &dat);
    if (h == INVALID_HANDLE_VALUE)
        return;
    try
    {
        do
        {
            if ((dat.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) == FILE_ATTRIBUTE_REPARSE_POINT ||
                (dat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY ||
                wcslen(dat.cFileName) != 8)
                continue;

            backups->Add(dat.cFileName);
        }
        while(FindNextFile(h, &dat) != FALSE);
    }
    catch(...)
    {
        ;
    }
    FindClose(h);

    backups->CustomSort(&_custom_sort_backup_folders_proc);

    for (int ix = backups->Count - 1; ix != -1; --ix)
    {
        bool remove = false;
        try
        {
            int k = StrToInt(backups->Strings[ix]);
            int year = k / 10000;
            int month = (k - year * 10000) / 100;
            int day = (k - year * 10000) - (month * 100);

            if (year < 1900 || year > 2300 || month > 12 || month < 1 || day > 31 || day < 1)
                remove = true;

            TDateTime(year, month, day); // Incorrect conversion causes an exception, disqualifying the given folder name as an incorrect date.
        }
        catch(...)
        {
            remove = true;
        }
        if (remove)
            backups->Delete(ix);
    }

    lastbackup = 0;

    if (backups->Count != 0)
    {
        int k = StrToInt(backups->Strings[backups->Count - 1]);
        int year = k / 10000;
        int month = (k - year * 10000) / 100;
        int day = (k - year * 10000) - (month * 100);
        lastbackup = TDateTime(year, month, day);
    }
}
//---------------------------------------------------------------------------
void TWordCollectionList::CreateBackup(TDateTime now, bool onload)
{
    if (!settings.backupdata || sbackingup)
        return;

    sbackingup = true;

    if (int(now) - int(lastbackup) < settings.bckspacing || (!onload && (double((fMain->RunningTime() / 60) / 60) / 24) < 2)) // Don't backup if zkanji is not running for 2 days and we are not called while loading.
    {
        sbackingup = false;
        return;
    }

    UnicodeString path = FullPathFromBackupPath(settings.bckpath) + L"\\backup";
    if (!DirectoryExists(path) && !CreateDir(path))
    {
        sbackingup = false;
        return;
    }

    path += L"\\";

    UnicodeString newbackup = now.FormatString(L"yyyymmdd");
    UnicodeString newbackuppath = path + newbackup;
    if (!DirectoryExists(newbackuppath) && !CreateDir(newbackuppath))
    {
        sbackingup = false;
        return;
    }

    newbackuppath += L"\\";

    try
    {
        DeleteBackup(settings.bckcount - 1);

        // Copy files to new backup folder.
        for (int ix = 0; ix < Count; ++ix)
        {
            CopyFile((paths.datapath + Strings[ix] + L".zkd").c_str(), (newbackuppath + Strings[ix] + L".zkd").c_str(), FALSE);
            CopyFile((paths.datapath + Strings[ix] + L".zkg").c_str(), (newbackuppath + Strings[ix] + L".zkg").c_str(), FALSE);
        }
        CopyFile((paths.datapath + "zkanji.zsp").c_str(), (newbackuppath + "zkanji.zsp").c_str(), FALSE);

        backups->Add(newbackup);
        lastbackup = now;
    }
    catch(...)
    {
        ;
    }

    sbackingup = false;
}
//---------------------------------------------------------------------------
void TWordCollectionList::DeleteBackup(int keep)
{
    UnicodeString path = FullPathFromBackupPath(settings.bckpath) + L"\\backup\\";

    while (backups->Count > keep)
    {
        SHFILEOPSTRUCT op;
        memset(&op, 0, sizeof(SHFILEOPSTRUCT));
        op.wFunc = FO_DELETE;
        UnicodeString dnullfile = path + backups->Strings[0];
        dnullfile.SetLength(dnullfile.Length() + 2);
        dnullfile[dnullfile.Length()] = 0;
        dnullfile[dnullfile.Length() - 1] = 0;
        op.pFrom = dnullfile.c_str();
        op.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI | FOF_SILENT;
        SHFileOperation(&op);

        backups->Delete(0);
    }
}
//---------------------------------------------------------------------------
int TWordCollectionList::BackupCount()
{
    return backups->Count;
}
//---------------------------------------------------------------------------
static void _MoveFolder(UnicodeString oldpath, UnicodeString newpath)
{
    WIN32_FIND_DATA dat;

    SetLastError(0);
    if (!DirectoryExists(newpath) && !CreateDir(newpath))
        return;

    HANDLE f = FindFirstFile((oldpath + L"\\*").c_str(), &dat);

    if (f != INVALID_HANDLE_VALUE)
    {
        TStringList *names = new TStringList;
        try
        {
            do
            {
                if ((dat.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) == FILE_ATTRIBUTE_REPARSE_POINT ||
                    (dat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY ||
                    (dat.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) == FILE_ATTRIBUTE_DEVICE ||
                    (dat.dwFileAttributes & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY ||
                    (dat.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) == FILE_ATTRIBUTE_SYSTEM)
                    continue;
                names->Add(dat.cFileName);
            }
            while (FindNextFile(f, &dat));
            FindClose(f);

            for (int ix = 0; ix < names->Count; ++ix)
                MoveFile((oldpath + L"\\" + names->Strings[ix]).c_str(), (newpath + L"\\" + names->Strings[ix]).c_str());

        }
        catch(...)
        {
            ;
        }
        delete names;
    }

    RemoveDir(oldpath);
}
//---------------------------------------------------------------------------
void TWordCollectionList::MoveBackup(UnicodeString newpath)
{
    UnicodeString oldpath = FullPathFromBackupPath(settings.bckpath);

    int ending = 0;
    while (oldpath.Length() > ending && oldpath[oldpath.Length() - ending] == L'\\')
        ++ending;
    if (ending)
        oldpath = oldpath.SubString(1, oldpath.Length() - ending);
    while (newpath.Length() > ending && newpath[newpath.Length() - ending] == L'\\')
        ++ending;
    if (ending)
        newpath = newpath.SubString(1, newpath.Length() - ending);

    if (!DirectoryExists(oldpath) || !DirectoryExists(oldpath + L"\\backup") || (!DirectoryExists(newpath) && !CreateDir(newpath)) || (!DirectoryExists(newpath + L"\\backup") && !CreateDir(newpath + L"\\backup")))
        return;

    oldpath += L"\\backup";
    newpath += L"\\backup";

    for (int ix = 0; ix < backups->Count; ++ix)
        _MoveFolder(oldpath + L"\\" + backups->Strings[ix], newpath + L"\\" + backups->Strings[ix]);

    RemoveDir(oldpath);
}
//---------------------------------------------------------------------------
extern bool interminating;
void TWordCollectionList::Save()
{
    if (save_in_progress || !Modified)
        return;

    if (Changing || sdisabled)
    {
        if (!interminating)
            PostMessage(fMain->Handle, WM_TIMER, 0, 0);
        return;
    }

    ssaving = true;
    try
    {
        UnicodeString ddate;
        UnicodeString basepath;

        for (int ix = 0; ix < Count; ++ix)
        {
            if (Items[ix]->modification != wccMain && Items[ix]->modification != wccStartupMain)
                continue;
            if (!save_dictionary(paths.datapath + Strings[ix] + L".zkd", Items[ix], ix == 0))
            {
                MsgBox((UnicodeString)L"An error occurred while saving dictionary! Restoring backup! (code: " + IntToStr(error_code) + ")", L"Error", MB_OK);
                error_code = 0;
                RestoreBackup(paths.datapath + Strings[ix] + L".zkd");
            }
        }

        for (int ix = 0; ix < Count; ++ix)
        {
            if (Items[ix]->modification == wccNone)
                continue;
            if (!save_groups(paths.datapath + Strings[ix] + L".zkg", Items[ix]))
            {
                MsgBox((UnicodeString)L"An error occurred while saving groups! Restoring backup! (code: " + IntToStr(error_code) + ")", L"Error", MB_OK);
                error_code = 0;
                RestoreBackup(paths.datapath + Strings[ix] + L".zkg");
            }
        }

        if (sprofchanged && !save_profile(paths.datapath + "zkanji.zsp"))
        {
            MsgBox(L"An error occurred while saving long-term study profile data! Restoring backup!", L"Error", MB_OK);
            error_code = 0;
            RestoreBackup(paths.datapath + L"zkanji.zsp");
        }

        for (int ix = 0; ix < Count; ++ix)
            Items[ix]->modification = wccNone;
    }
    catch (const wchar_t *c)
    {
        MsgBox(c, L"ERROR", MB_OK);
    }
    catch (...)
    {
        ;
    }
    sprofchanged = false;
    ssaving = false;
}
//---------------------------------------------------------------------------
void TWordCollectionList::Load()
{
    if (save_in_progress)
    {
        ssaving = false;
        Change(true);
        MsgBox(L"An error occurred while loading database! Restart zkanji! (bug)", L"Error!", MB_OK);
        Change(false);
        return;
    }

    MODON

    LoadingDictionaryGuard __loadguard;

    if (FileExists(paths.datapath + L"zkanji.zsp"))
    {
        load_profile(paths.datapath + L"zkanji.zsp");
        //Backup(paths.datapath + L"zkanji.zsp");
    }

    if (FileExists(paths.datapath + L"English.zkg"))
    {
        try
        {
            load_groups(paths.datapath + L"English.zkg", Items[0]);
            //Backup(paths.datapath + L"English.zkg"); //backup successfully loaded file on startup
        }
        catch (...)
        {
            MsgBox(UnicodeString(L"An error occurred while opening \"English.zkg\" group file! (Code: ") + error_code + L")", L"Error", MB_OK);
        }
    }
    if (Application->Terminated)
        return;

    void *h;
    _WIN32_FIND_DATAW data;
    h = FindFirstFile((paths.datapath + L"*.zkd").c_str(), &data);
    if (h != INVALID_HANDLE_VALUE)
    {
        UnicodeString name;
        while (GetLastError() != ERROR_NO_MORE_FILES)
        {
            if (data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
                continue;
            name = ExtractFileName(data.cFileName).SubString(1, ExtractFileName(data.cFileName).Length() - 4);
            if (name.LowerCase() == L"english")
            {
                FindNextFileW(h, &data);
                continue;
            }
            if (!load_dictionary(paths.datapath + name + L".zkd", Items[Add(name)], false, false))
            {
                MsgBox(UnicodeString(L"Couldn't load user dictionary file: ") + name + L".zkd" + L"\nTry restarting the program.", L"Error", MB_OK);
                return;
            }
            //Backup(paths.datapath + name + L".zkd");

            if (FileExists(paths.datapath + name + L".zkg"))
            {
                try
                {
                    load_groups(paths.datapath + name + L".zkg", Items[Count - 1]);
                    //Backup(paths.datapath + name + L".zkg");
                }
                catch (...)
                {
                    MsgBox(UnicodeString(L"An error occurred while opening group file: ") + name + L".zkg (Code: " + error_code + L")", L"Error", MB_OK);
                }
            }
            if (Application->Terminated)
                return;

            if (FileExists(paths.datapath + name + L".bmp"))
            {
                TBmp *flags = NULL;

                TBmp *flag = NULL;
                TBmp *flagalpha = NULL;

                int added = 0;
                try
                {
                    flags = new TBmp;
                    flags->LoadFromFile(paths.datapath + name + L".bmp");
                    if (flags->Width == 32 && flags->Height == 16)
                    {
                        TColor bg = flags->Canvas->Pixels[0][15];
                        flag = new TBmp;
                        flagalpha = new TBmp;
                        flagalpha->PixelFormat = pf1bit;
                        flag->SetSize(16, 16);
                        flagalpha->SetSize(16, 16);

                        BitBlt(flag->Canvas->Handle, 0, 0, 16, 16, flags->Canvas->Handle, 0, 0, SRCCOPY);

                        for (int ix = 0; ix < 16 * 16; ++ix)
                            flagalpha->Canvas->Pixels[ix % 16][ix / 16] = (TColor)(flag->Canvas->Pixels[ix % 16][ix / 16] == bg ? clWhite : clBlack);

                        if (::data->imgMenu->Add(flag, flagalpha) > 0)
                            added++;

                        BitBlt(flag->Canvas->Handle, 0, 0, 16, 16, flags->Canvas->Handle, 16, 0, SRCCOPY);

                        for (int ix = 0; ix < 16 * 16; ++ix)
                            flagalpha->Canvas->Pixels[ix % 16][ix / 16] = (TColor)(flag->Canvas->Pixels[ix % 16][ix / 16] == bg ? clWhite : clBlack);

                        if (::data->imgMenu->Add(flag, flagalpha) > 0)
                            added++;

                        Items[Count - 1]->imgindex = ::data->imgMenu->Count - 2;

                    }
                }
                catch (...)
                {
                    Items[Count - 1]->imgindex = -1;
                    for (int ix = 0; ix < added; ++ix)
                    {
                        ::data->imgMenu->Delete(::data->imgMenu->Count - 1);
                    }
                }
                delete flags;
                delete flag;
                delete flagalpha;
            }

            FindNextFileW(h, &data);
        }
        FindClose(h);
    }

    for (int ix = 0; ix < Count; ++ix)
        if (Items[ix]->modification != wccStartupMain && Items[ix]->modification != wccStartupUser)
            Items[ix]->modification = wccNone;

    CreateBackup(Now(), true);

    MODOFF
}
//---------------------------------------------------------------------------
bool TWordCollectionList::Reload(byte coll)
{
    MODON

    LoadingDictionaryGuard __loadguard;

    UnicodeString dictname = Strings[coll];
    Delete(coll);

    TWordCollection *c = new TWordCollection();
    int result = IDOK;
    do
    {
        bool error;
        try
        {
            error = !load_dictionary(paths.datapath + dictname + L".zkd", c, coll == 0, false);
        }
        catch(...)
        {
            error = true;
        }
        if (error)
            result = MsgBox(L"Reloading the dictionary was unsuccessful. Make sure no other program has the file open and try again.", L"Error", MB_RETRYCANCEL);
    }
    while (result == IDRETRY);

    if (result == IDCANCEL)
    {
        DisableSave();
        MsgBox(L"Dictionary data saving has been disabled. Please restart the program.", L"Message", MB_OK);
        return false;
    }

    InsertObject(coll, dictname, (TObject*)(c));

    if (FileExists(paths.datapath + dictname + L".zkg"))
    {
        result = IDOK;
        do
        {
            try
            {
                load_groups(paths.datapath + dictname + L".zkg", Items[coll]);
            }
            catch (...)
            {
                result = MsgBox(UnicodeString(L"An error occurred while opening group file: ") + dictname + L".zkg (Code: " + error_code + L")", L"Error", MB_RETRYCANCEL);
            }
        }
        while (result == IDRETRY);

        if (result == IDCANCEL)
        {
            DisableSave();
            MsgBox(L"Dictionary data saving has been disabled. Please restart the program.", L"Message", MB_OK);
            return false;
        }
    }

    MODOFF

    return true;
}
//---------------------------------------------------------------------------
bool TWordCollectionList::Loading()
{
    return sloading;
}
//---------------------------------------------------------------------------
bool TWordCollectionList::get_changing()
{
    return schanging > 0;
}
//---------------------------------------------------------------------------
bool TWordCollectionList::get_disabled()
{
    return sdisabled > 0;
}
//---------------------------------------------------------------------------
bool TWordCollectionList::get_modified()
{
    if (sprofchanged)
        return true;

    for (int ix = 0; ix < Count; ++ix)
        if (Items[ix]->modification != wccNone)
            return true;

    return false;
}
//---------------------------------------------------------------------------
void TWordCollectionList::ChangeWordMeaning(byte coll, int wordindex, byte meaning, wchar_t *str, int wtype, int wnote, byte wfield, word ntag, word wfrequency)
{
    if (!changelist)
    {
        THROW(L"No initialization before setting word fields!");
    }
    else if (changelist->CollectionIndex != coll)
        THROW(L"Bad initialization for setting word fields!");

    TWord *w = Items[coll]->words->Items[wordindex];

    // IMPORTANT: dictupdater.cpp TWordStateList::Commit emulates originals change and other stuff. Make changes there if this code changes.
    if (!coll)
    {
        //LoadOriginals();
        originals->Add(w, owcModified);
    }

    Items[coll]->ChangeWordMeaning(wordindex, meaning, str, wtype, wnote, wfield, ntag, wfrequency);
    /*
     CHANGEON
     w->data[meaning].types = wtype;
     w->data[meaning].notes = wnote;
     w->data[meaning].fields = wfield;
     w->data[meaning].nametag = ntag;
     if (wfrequency < 10001)
     w->frequency = wfrequency;
     CHANGEOFF
     */
    changelist->Add(w->kanji, w->kana, wctChange);
}
//---------------------------------------------------------------------------
void TWordCollectionList::RevertWord(int wordindex)
{
    if (!changelist)
    {
        THROW(L"No initialization before setting word fields!");
    }
    else if (changelist->CollectionIndex != 0)
        THROW(L"Bad initialization for setting word fields!");

    TWord *w = Items[0]->words->Items[wordindex];

    originals->Revert(wordindex);

    changelist->Add(w->kanji, w->kana, wctChange);
}
//---------------------------------------------------------------------------
int TWordCollectionList::Revertable(int wordindex)
{
    TGapList<int> pos;
    TGapList<int> posorig;
    return originals->Revertable(wordindex, pos, posorig);
}
//---------------------------------------------------------------------------
void TWordCollectionList::Change(bool add)
{
    if (ssaving)
        THROW(L"Cannot alter database while saving!");

    if (!schanging && !add)
        THROW(L"Change called when no changing was in progress.");

    schanging = max(0, schanging + (add ? 1 : -1));
}
//---------------------------------------------------------------------------
void TWordCollectionList::ResetChange()
{
    if (ssaving)
        THROW(L"We are in the middle of saving!");

    schangesave = schanging;
    schanging = 0;
}
//---------------------------------------------------------------------------
void TWordCollectionList::RestoreChange()
{
    if (schangesave < 0)
        THROW(L"Nothing to restore!");

    schanging = schangesave;
    schangesave = -1;
}
//---------------------------------------------------------------------------
void TWordCollectionList::DisableSave()
{
    ++sdisabled;
}
//---------------------------------------------------------------------------
void TWordCollectionList::EnableSave()
{
    sdisabled = max(0, sdisabled - 1);
}
//---------------------------------------------------------------------------
void TWordCollectionList::ProfileModify()
{
    if (!schanging)
        THROW(L"ProfileModify called when no changing was in progress.");

    sprofchanged = true;
}
//---------------------------------------------------------------------------
int TWordCollectionList::get_index(TWordCollection *wc)
{
    return IndexOfObject((TObject*)wc);
}
//---------------------------------------------------------------------------
int TWordCollectionList::AddWordGroup(byte ix, UnicodeString aname, bool astudy)
{
    int i;
    MODON
    if (!astudy)
        i = Items[ix]->groups->Add(aname);
    else
        i = Items[ix]->studygroups->Add(aname);
    Items[ix]->Modify(false);
    MODOFF

    return i;
}
//---------------------------------------------------------------------------
int TWordCollectionList::AddKanjiGroup(byte ix, UnicodeString aname)
{
    int i;
    MODON
    i = Items[ix]->kgroups->Add(aname);
    Items[ix]->Modify(false);
    MODOFF

    return i;
}
//---------------------------------------------------------------------------
//returns custom meaning of kanji or an empty string when no custom meaning is present
UnicodeString TWordCollectionList::CustomKanjiMeaning(byte ix, word kindex)
{
    if (!Items[ix]->kanjidat[kindex].meaning)
        return "";
    return Items[ix]->kanjidat[kindex].meaning;
}
//---------------------------------------------------------------------------
//returns custom meaning of kanji or the original meaning when no custom meaning is present
UnicodeString TWordCollectionList::KanjiMeaning(byte ix, word kindex)
{
    if (!Items[ix]->kanjidat[kindex].meaning)
        return kanjis->Kanjis[kindex]->meaning;
    return Items[ix]->kanjidat[kindex].meaning;
}
//---------------------------------------------------------------------------
bool TWordCollectionList::ModifyKanjiMeaning(byte ix, word kindex, UnicodeString meaning)
{
    if ((Items[ix]->kanjidat[kindex].meaning && meaning == Items[ix]->kanjidat[kindex].meaning) || (!Items[ix]->kanjidat[kindex].meaning && (!meaning.Length() || meaning == kanjis->Kanjis[kindex]->meaning)))
        return false;

    MODON
    delete[] Items[ix]->kanjidat[kindex].meaning;
    Items[ix]->kanjidat[kindex].meaning = NULL;

    if (meaning.Length() && meaning != KanjiMeaning(ix, kindex))
    {
        Items[ix]->kanjidat[kindex].meaning = new wchar_t[meaning.Length() + 1];
        wcscpy(Items[ix]->kanjidat[kindex].meaning, meaning.c_str());
    }
    Items[ix]->Modify(false);
    MODOFF

    return true;
}
//---------------------------------------------------------------------------
/*
void TWordCollectionList::Export(UnicodeString fname, byte ix)
{
    error_code = 0;

    FILE *f = _wfopen(fname.c_str(), L"wb");
    if (!f)
        THROW(L"Couldn't open file for dictionary export!");

    try
    {
        TWordCollection *col = Items[ix];

        UnicodeString z;
        error_code = 1;

        funicode(f);

        z = (UnicodeString)L"Version: " + Dictionary_Export_Version_String;
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);

        z = L"# zkanji version used for export: ";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        z = (UnicodeString)L"v" + Version_String;
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);
        z = L"# Exported dictionary: ";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        z = Strings[ix];
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);

        z = L"# Date of export: ";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        wchar_t c[1024];
        GetDateFormatW(0x0409, DATE_SHORTDATE, NULL, NULL, c, 1023);
        z = (UnicodeString)c + L", ";
        GetTimeFormatW(0x0409, TIME_NOSECONDS, NULL, NULL, c, 1023);
        z += c;
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);
        z = L"# File format: (items are separated by the tab character, not space)";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);
        z = L"# kanji\tkana\tword_frequency\t[meaning\ttypes\tnotes\tfields\tname_tag]";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);
        z = L"# The values in brackets are repeated for each meaning, but the brackets are not written.";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);
        z = L"# Fields, types and notes are written in decimal numbers. Each bit represents a different value.";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);
        z = L"# Lines starting with # are comments. # in the middle of a line doesn't count as comment.";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);
        z = L"# Send me an e-mail if you need further information.";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);
        feol(f);
        error_code = 2;
        col->words->Export(f);
        error_code = 0;
    }
    catch (...)
    {
        ;
    }
    fclose(f);
}
//---------------------------------------------------------------------------
void TWordCollectionList::SQLExport(UnicodeString fname, byte ix)
{
    TWordCollection *col = Items[ix];

    FILE *f1 = NULL;
    FILE *f2 = NULL;

    error_code = 0;
    f1 = _wfopen(fname.c_str(), L"wb");
    if (f1)
        f2 = _wfopen((ExtractFilePath(fname) + "b" + ExtractFileName(fname)).c_str(), L"wb");
    if (!f1 || !f2)
    {
        if (f1)
            fclose(f1);
        THROW(L"Error opening files for dictionary export!");
    }

    try
    {

        col->words->SQLExport(f1, f2);

    }
    catch (...)
    {

    }

    fclose(f2);
    fclose(f1);

}
//---------------------------------------------------------------------------
void TWordCollectionList::ExportGroups(UnicodeString fname, byte coll, bool words, bool wstudy, bool kanji, bool kexamples)
{
    error_code = 0;

    FILE *f = _wfopen(fname.c_str(), L"wb");
    if (!f)
        THROW(L"Couldn't open file for dictionary export!");
    try
    {
        TWordCollection *col = Items[coll];

        UnicodeString z;

        error_code = 1;

        funicode(f);

        z = (UnicodeString)L"# zkanji groups export";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);
        z = (UnicodeString)L"# Version: " + Dictionary_Group_Export_Version_Number;
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);
        z = L"# zkanji version used for export: ";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        z = (UnicodeString)L"v" + Version_String;
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);
        z = L"# Exported dictionary: ";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        z = Strings[coll];
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);

        z = L"# Date of export: ";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        wchar_t c[1024];
        GetDateFormatW(0x0409, DATE_SHORTDATE, NULL, NULL, c, 1023);
        z = (UnicodeString)c + L", ";
        GetTimeFormatW(0x0409, TIME_NOSECONDS, NULL, NULL, c, 1023);
        z += c;
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);
        error_code = 2;

        if (words)
            col->groups->Export(f);
        error_code = 3;
        if (wstudy)
            col->studygroups->Export(f);
        error_code = 4;

        /*if (wstats)
         WordStatisticExport(f, coll);
         error_code = 5;*-/

        if (kanji)
            col->kgroups->Export(f);
        error_code = 6;
        if (kexamples)
            KanjiExampleExport(f, coll);
        error_code = 0;
    }
    catch (...)
    {
        ;
    }
    fclose(f);
}
//---------------------------------------------------------------------------
void TWordCollectionList::KanjiExampleExport(FILE *f, byte coll)
{
    UnicodeString z;

    z = L"\texamples for kanji:";
    fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
    feol(f);

    TWordCollection *col = Items[coll];

    for (int ix = 0; ix < kanjis->Count; ++ix)
    {
        if (!col->kanjidat[ix].card)
            continue;

        for (int iy = 0; iy < col->kanjidat[ix].card->ExampleCount; ++iy)
        {
            TWord *w = col->words->Items[col->kanjidat[ix].card->Examples[iy]->ix];

            z = (UnicodeString)kanjis->Kanjis[ix]->ch + L"\t" + w->kanji + L"\t" + w->kana;
            fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
            feol(f);
        }
    }
}
//---------------------------------------------------------------------------
void TWordCollectionList::WordStatisticExport(FILE *f, byte coll)
{
    UnicodeString z;

    z = L"\tword test statistics:";
    fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
    feol(f);

    TWordCollection *col = Items[coll];
    for (int ix = 0; ix < col->words->Count; ++ix)
    {
        TWord *w = col->words->Items[ix];
        for (int iy = 0; iy < w->meaningcnt; ++iy)
        {
            if (!w->stats || !w->stats[iy].included)
                continue;

            z = (UnicodeString)w->kanji + L"\t" + w->kana + L"\t" + w->data[iy].meaning + L"\tincluded " + (int)w->stats[iy].included + L"\tscore " + (int)w->stats[iy].score + "\t" + FormatDateTime("dd/mm/yy hh:nn:ss", w->stats[iy].testtime) + L"\t"/*+(w->stats[iy].state == lsNormal ? L"normal" : w->stats[iy].state == lsLearned ? L"learned" : L"problematic")*-/;
            fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
            feol(f);
        }
    }
}
*/
//---------------------------------------------------------------------------
/*
 // return position in the alphabetic order. Or "-1 - would_be_position" when not present.
 int TWordCollectionList::WordPosition(byte coll, const wchar_t *kanji, const wchar_t *kana, TWordOrder order)
 {
 T-List *l;

 if (order == woFiltered) {
 l = Items[coll]->abcfilter;
 } else if (order == woSelect)
 l = settings.alphaorder == woAlpha ? Items[coll]->abcde : Items[coll]->aiueo;
 else
 l = order == woAlpha ? Items[coll]->abcde : Items[coll]->aiueo;

 int mid, min = 0, max = l->Count-1;
 if (max < 0)
 return -1;

 TWordPosFunc orderfunc = (order == woSelect || order == woFiltered ? settings.alphaorder : order) == woAlpha ? abccmp : japcmp;
 int r;
 TWord *w;
 bool reset = true;

 while (min <= max) {
 mid = (max+min) / 2;
 w = Items[coll]->words->Items[(int)l->Items[mid]];
 r = orderfunc(reset,kanji,kana,w->kanji,w->kana);
 reset = false;

 if (!r && kanji) {
 r = wcscmp(kanji,w->kanji);
 } if (!r && mid > min) {
 w = Items[coll]->words->Items[(int)l->Items[mid-1]];
 r = orderfunc(false,kanji,kana,w->kanji,w->kana);
 if (!r) r = -1;
 else r = 0;
 }

 if (r < 0)
 max = mid-1;
 else if (r > 0)
 min = mid+1;
 else
 break;
 }

 if (r) {
 mid = min;

 if (mid < l->Count) {
 w = Items[coll]->words->Items[(int)l->Items[mid]];

 if (wcsncmp(Hiraganize(w->kana).c_str(),Hiraganize(kana).c_str(), Hiraganize(kana).Length()))
 mid = -1 - mid;
 } else
 mid = -1 - mid;
 }

 return mid;
 }
 */
//---------------------------------------------------------------------------
void TWordCollectionList::SortFilters()
{
    for (int ix = 0; ix < Count; ++ix)
    {
        TWordCollection *c = Items[ix];
        if (!c->abcfilter)
            continue;
        abcfilter_cmp_collection = c;
        abcfilter_prev_ix = -1;
        c->abcfilter->Sort(abcfilter_cmp_proc);
    }
}
//---------------------------------------------------------------------------
void TWordCollectionList::settingsChanged(void *caller, TSettingsProcState state)
{
    if (state == spsBegin)
    {
        savedsortorder = settings.alphaorder;
        return;
    }

    if (state != spsEnd || savedsortorder == settings.alphaorder)
        return;

    SortFilters();
}
//---------------------------------------------------------------------------
void TWordCollectionList::SetABCFilter(byte coll)
{
    TWordCollection *c = Items[coll];

    if (c->abcfilter && dictionaryfilters->filters_match(c->setfilters))
        return;

    double n = Now();
    bool msgform = false;
    delete c->abcfilter;

    try
    {
        c->abcfilter = new TIntegerList;
        dictionaryfilters->create_copy(c->setfilters);

        TIntegerList *l = settings.alphaorder == 0 ? Items[coll]->abcde : Items[coll]->aiueo;

        for (int ix = 0; ix < l->Count; ix++)
        {
            if (dictionaryfilters->match(c->words->Items[l->Items[ix]]))
                c->abcfilter->Add(l->Items[ix]);

            if (!msgform && (double)Now() - n > 0.3 / 86400.0 && ix < l->Count / 2)
            {
                ShowMessageForm("Populating filtered dictionary. Please wait...");
                msgform = true;
            }
        }

    }
    catch (...)
    {
        delete c->abcfilter;
        c->abcfilter = NULL;
    }

    if (msgform)
        HideMessageForm();
}
//---------------------------------------------------------------------------
void TWordCollectionList::ClearABCFilters()
{
    for (int ix = 0; ix < Count; ix++)
    {
        delete Items[ix]->abcfilter;
        Items[ix]->abcfilter = NULL;
    }
}
//---------------------------------------------------------------------------
int TWordCollectionList::WordIndex(byte coll, int index, TWordOrder order)
{
    return order == woFiltered ? Items[coll]->abcfilter->Items[index] : (order == woSelect ? settings.alphaorder : order) == woAlpha ? Items[coll]->abcde->Items[index] : Items[coll]->aiueo->Items[index];
}
//---------------------------------------------------------------------------
int TWordCollectionList::ListCount(byte coll, TWordOrder order)
{
    return order == woFiltered ? Items[coll]->abcfilter->Count : (order == woSelect ? settings.alphaorder : order) == woAlpha ? Items[coll]->abcde->Count : Items[coll]->aiueo->Count;
}
//---------------------------------------------------------------------------
int FindReading(byte kanjipos, TWord *w)
{
    int r = 0;
    TFuriganaData *dat;
    int cnt = FindFurigana(w, dat);
    if (cnt <= 0)
        return -1;
    try
    {
        int pos;
        for (pos = 0; pos < cnt && kanjipos >= dat[pos].kanji + dat[pos].kanjilen; pos++)
            ;
        if (pos == cnt || dat[pos].kanji + dat[pos].kanjilen <= kanjipos)
            pos = -1;

        TKanji *k = kanjis->Kanjis[kanjis->IndexOf(w->kanji[kanjipos])];
        int ccnt = compact_count(k);
        if (pos >= 0)
        {
            bool fullmatch = false; // Exact match.
            int lastmatch = -1;
            int matchval = 9999;
            int val;
            wchar_t *c;
            int lastkana = -1;
            int kanaval = 9999;

            UnicodeString kanatmp = Hiraganize(w->kana);
            const wchar_t *kana = kanatmp.c_str(); // This is valid through the whole function.

            if (dat[pos].kanjilen == 1)
            {
                for (int i = 0; i < k->oncnt; i++)
                {
                    c = k->on[i];
                    if (c[0] == L'-')
                        c++;
                    if (kunlen(c) != dat[pos].kanalen)
                        continue;

                    if (!wcsncmp(kana + dat[pos].kana, Hiraganize(c).c_str(), dat[pos].kanalen))
                    {
                        fullmatch = true;
                        lastmatch = i;
                        matchval = 1;
                        break;
                    }
                    if (lastmatch >= 0 && matchval == 1)
                        continue; // Don't care about other matches that are not exact either.

                    if ((val = FuReading(c, kana, dat[pos].kana, dat[pos].kana + dat[pos].kanalen - (pos < cnt - 1 ? 0 : 1))) != 0 && val < matchval)
                    {
                        lastmatch = i;
                        matchval = val;
                    }
                }
                bool afterkana = lastmatch < 0 || KANA(w->kanji[dat[pos].kanji + dat[pos].kanjilen]);
                if (fullmatch && !afterkana)
                    r = lastmatch + 1; // Found the on-reading. no need to go further.
                else if (fullmatch)
                {
                    // Found on-reading but it could be kun too.
                    UnicodeString on = Hiraganize(c);
                    for (int i = 1 + k->oncnt; i < ccnt && !r; i++)
                    {
                        int kk = real_index(k, i) - 1 - k->oncnt;
                        if (!wcsncmp(k->kun[kk], c, kunlen(k->kun[kk])))
                            r = i;
                    }
                    if (!r)
                        r = lastmatch + 1;
                }
                else
                {
                    // No perfect match or no match at all.
                    for (int i = 1 + k->oncnt; i < ccnt; i++)
                    {
                        int kk = real_index(k, i) - 1 - k->oncnt;
                        if (kunlen(k->kun[kk]) != dat[pos].kanalen)
                            continue;
                        if (!wcsncmp(kana + dat[pos].kana, k->kun[kk], dat[pos].kanalen))
                        {
                            r = i;
                            break;
                        }

                        if (!afterkana && lastmatch >= 0 && matchval == 1 || lastkana >= 0 && kanaval == 1)
                            continue;

                        if ((val = FuReading(k->kun[kk], kana, dat[pos].kana, dat[pos].kana + dat[pos].kanalen - (pos < cnt - 1 ? 0 : 1))) != 0 && val < kanaval)
                        {
                            lastkana = i;
                            kanaval = val;
                        }
                    }
                    if (r == 0)
                        r = (lastkana >= 0 ? lastkana : lastmatch + 1);
                }
            }
            else
            {
                enum TKanjiWordPos { wpStart, wpEnd, wpMid } wp;
                if (dat[pos].kanji == kanjipos)
                    wp = wpStart;
                else if (dat[pos].kanji + dat[pos].kanjilen - 1 == kanjipos)
                    wp = wpEnd;
                else
                    wp = wpMid;
                bool afterkana = wp == wpEnd && KANA(w->kanji[dat[pos].kanji + dat[pos].kanjilen]);

                for (int i = 0; i < ccnt - 1; i++)
                {
                    int kk;
                    if (i >= k->oncnt)
                        kk = real_index(k, i + 1);
                    c = i < k->oncnt ? k->on[i] : k->kun[kk - 1 - k->oncnt];
                    if (c[0] == L'-')
                        c++;
                    int len = kunlen(c);

                    if (len > dat[pos].kanalen - 1 - (wp == wpMid ? 1 : 0))
                        continue;

                    if (wp != wpMid)
                    {
                        if (i < k->oncnt && !fullmatch && !wcsncmp(kana + dat[pos].kana + (wp == wpStart ? 0 : dat[pos].kanalen - len), Hiraganize(c).c_str(), len))
                        {
                            fullmatch = true;
                            lastmatch = i;
                            matchval = 1;
                            if (!afterkana)
                                break;
                            i = k->oncnt - 1; // Found exact match, skip other on readings.
                            continue;
                        }

                        if (i >= k->oncnt && (lastmatch < 0 || afterkana) && !wcsncmp(kana + dat[pos].kana + (wp == wpStart ? 0 : dat[pos].kanalen - len), c, len))
                        {
                            lastkana = i + 1;
                            break;
                        }

                        if (i < k->oncnt && lastmatch >= 0 && matchval == 1 || i >= k->oncnt && kanaval == 1)
                            continue;

                        int s = dat[pos].kana + (wp == wpStart ? 0 : dat[pos].kanalen - len);
                        if ((val = FuReading(c, kana, s, s + len - 1)) != 0)
                        {
                            if (i < k->oncnt && val < matchval)
                            {
                                lastmatch = i;
                                matchval = val;
                            }
                            else if (i >= k->oncnt && val < kanaval)
                            {
                                lastkana = i + 1;
                                kanaval = val;
                            }
                        }

                    }
                    else
                    {
                        int start = dat[pos].kana + (kanjipos - dat[pos].kanji);
                        int end = dat[pos].kana + dat[pos].kanalen - len - (dat[pos].kanji + dat[pos].kanjilen - 1 - kanjipos);

                        if (i >= k->oncnt || !fullmatch)
                        {
                            for (int pp = start; pp <= end; pp++)
                            {
                                if (!wcsncmp(kana + pp, Hiraganize(c).c_str(), len))
                                {
                                    if (i < k->oncnt)
                                    {
                                        lastmatch = i;
                                        matchval = 1;
                                        fullmatch = true;
                                        break;
                                    }
                                    else
                                    {
                                        lastkana = i + 1;
                                        kanaval = 1;
                                        fullmatch = true;
                                        break;
                                    }
                                }

                                if (i < k->oncnt && lastmatch >= 0 || i >= k->oncnt && lastkana >= 0)
                                    continue;

                                if ((val = FuReading(c, kana, start, start + len - 1)) != 0)
                                    if (i < k->oncnt && val < matchval)
                                    {
                                        lastmatch = i;
                                        matchval = val;
                                    }
                                    else if (i >= k->oncnt && val < kanaval)
                                    {
                                        lastkana = i + 1;
                                        kanaval = val;
                                    }
                            }
                            if (fullmatch)
                                break;
                        }
                    }

                }

                r = (lastkana >= 0 ? lastkana : lastmatch + 1);
            }

        }
    }
    catch (...)
    {
        ;
    }

    if (cnt)
        delete[] dat;

    return r;
}
//---------------------------------------------------------------------------
bool IsReading(TKanji *k, TWord *w, byte readingpos)
{
    bool r = false;

    TFuriganaData *dat;
    int cnt = FindFurigana(w, dat);
    if (cnt <= 0)
        return false;
    try
    {
        int len = 0;
        int len2;

        wchar_t *reading = (!readingpos ? NULL : readingpos <= k->oncnt ? k->on[readingpos - 1] : k->kun[readingpos - k->oncnt - 1]);

        if (reading)
        {
            len = kunlen(reading);
            if (reading[0] == L'-')
            {
                len--;
                reading++;
            }
        }

        if (!reading && cnt < (int)wcslen(w->kanji))
            len = 0;

        enum TKanjiWordPos { wpNone, wpStart, wpEnd, wpMid } wp = wpNone;
        for (int i = 0; i < cnt && !r; i++)
        {
            if (dat[i].kanjilen > 1)
            {
                if (!reading)
                { //looking for irregular kanji reading
                    for (int j = 0; j < dat[i].kanjilen && !r; j++)
                    {
                        if (w->kanji[dat[i].kanji + j] == k->ch)
                        {
                            r = true;

                            //match all other readings before jumping to conclusion
                            for (int kp = 0; kp < k->oncnt + k->kuncnt && r; kp++)
                            {
                                reading = kp < k->oncnt ? k->on[kp] : k->kun[kp - k->oncnt];
                                if (reading[0] == L'-')
                                    reading++;
                                len = kunlen(reading);
                                for (int j = 0; j < dat[i].kanjilen && r; j++)
                                {
                                    if (w->kanji[dat[i].kanji + j] == k->ch)
                                        wp = (j == 0 ? wpStart : j == dat[i].kanjilen - 1 ? wpEnd : wpMid);
                                    if (wp == wpStart && FuReading(reading, w->kana, dat[i].kana, dat[i].kana + dat[i].kanalen - (i < cnt - 1 ? 0 : 1)))
                                        r = false;
                                    else if (wp == wpEnd && FuReading(reading, w->kana, dat[i].kana + dat[i].kanalen - len, dat[i].kana + dat[i].kanalen - (i < cnt - 1 ? 0 : 1)))
                                        r = false;
                                    else if (wp == wpMid)
                                    {
                                        for (int k = j; k < dat[i].kanalen - len + 1 - (dat[i].kanjilen - j - 1) && !r; ++k)
                                            if (FuReading(reading, w->kana, dat[i].kana + k, dat[i].kana + dat[i].kanalen - (i < cnt - 1 ? 0 : 1)))
                                                r = false;
                                    }
                                }
                            }
                            reading = NULL;
                        }
                    }
                }
                else
                { //try to guess the reading of the kanji
                    for (int j = 0; j < dat[i].kanjilen && !r; j++)
                    {
                        if (w->kanji[dat[i].kanji + j] == k->ch)
                            wp = (j == 0 ? wpStart : j == dat[i].kanjilen - 1 ? wpEnd : wpMid);
                        if (wp == wpStart && FuReading(reading, w->kana, dat[i].kana, dat[i].kana + dat[i].kanalen - (i < cnt - 1 ? 0 : 1)))
                            r = true;
                        else if (wp == wpEnd && FuReading(reading, w->kana, dat[i].kana + dat[i].kanalen - len, dat[i].kana + dat[i].kanalen - (i < cnt - 1 ? 0 : 1)))
                            r = true;
                        else if (wp == wpMid)
                        {
                            for (int k = j; k < dat[i].kanalen - len + 1 - (dat[i].kanjilen - j - 1) && !r; ++k)
                                if (FuReading(reading, w->kana, dat[i].kana + k, dat[i].kana + dat[i].kanalen - (i < cnt - 1 ? 0 : 1)))
                                    r = true;
                        }
                    }
                }
                continue;
            }

            if (w->kanji[dat[i].kanji] == k->ch)
            {
                if (reading && len == dat[i].kanalen && FuReading(reading, w->kana, dat[i].kana, dat[i].kana + dat[i].kanalen - (i < cnt - 1 ? 0 : 1)))
                    r = true;
                else if (!reading)
                {
                    for (int j = 0; j < k->oncnt && !r; j++)
                    {
                        len2 = wcslen(k->on[j]);
                        if (k->on[j][0] == L'-')
                            len2--;
                        if (len2 == dat[i].kanalen && FuReading(k->on[j] + (k->on[j][0] == L'-' ? 1 : 0), w->kana, dat[i].kana, dat[i].kana + dat[i].kanalen - (i < cnt - 1 ? 0 : 1)))
                            r = true;
                    }
                    for (int j = 0; j < k->kuncnt && !r; j++)
                    {
                        if (kunlen(k->kun[j]) == dat[i].kanalen && FuReading(k->kun[j], w->kana, dat[i].kana, dat[i].kana + dat[i].kanalen - (i < cnt - 1 ? 0 : 1)))
                            r = true;
                    }
                    r = !r;
                }
            }
        }

    }
    catch (...)
    {
    }

    if (cnt)
        delete[] dat;

    return r;
}
//---------------------------------------------------------------------------
int TWordCollectionList::BaseIndex(TWordCollection *col, int wordindex)
{
    if (col == Items[0])
        return wordindex;

    TWordIndexList *l = NULL;
    try
    {
        l = new TWordIndexList(Items[0]);
        TWord *w = col->words->Items[wordindex];
        FindWord(Romanize(w->kana).c_str(), l, true, true, false, false, NULL);
        while (l->Count > 0 && (wcscmp(l->Words[l->Count - 1]->kana, w->kana) || wcscmp(l->Words[l->Count - 1]->kanji, w->kanji)))
            l->Delete(l->Count - 1);

        if (!l->Count)
            wordindex = -1;
        else
            wordindex = l->Indexes[l->Count - 1];
    }
    catch (...)
    {
        wordindex = -1;
    }
    delete l;

    return wordindex;
}
//---------------------------------------------------------------------------
int TWordCollectionList::BaseIndex(byte ix, int wordindex)
{
    return BaseIndex(Items[ix], wordindex);
}
//---------------------------------------------------------------------------
int TWordCollectionList::ConvertIndex(TWordCollection *src, int wordindex, TWordCollection *dest)
{
    if (src == NULL)
        src = Items[0];

    if (src == dest)
        return wordindex;

    TWordIndexList *l = NULL;
    try
    {
        l = new TWordIndexList(dest);
        TWord *w = src->words->Items[wordindex];
        FindWord(Romanize(w->kana).c_str(), l, true, true, false, false, NULL);
        while (l->Count > 0 && (wcscmp(l->Words[l->Count - 1]->kana, w->kana) || wcscmp(l->Words[l->Count - 1]->kanji, w->kanji)))
            l->Delete(l->Count - 1);

        if (!l->Count)
            wordindex = -1;
        else
            wordindex = l->Indexes[l->Count - 1];
    }
    catch (...)
    {
        wordindex = -1;
    }
    delete l;

    return wordindex;
}
//---------------------------------------------------------------------------
void TWordCollectionList::BeginWordChange(byte ix)
{
    if (changelist)
        THROW(L"Double initialization while dictionary is changing!");
    changelist = new TChangedWordList(ix);
}
//---------------------------------------------------------------------------
void TWordCollectionList::EndWordChange()
{
    if (!changelist)
        THROW(L"No initialization while dictionary is changing!");

    if (changelist->Count)
    {
        fDict->WordChanged(changelist->CollectionIndex, changelist);
        fKanji->WordChanged(changelist->CollectionIndex, changelist);
        fGroup->WordChanged(changelist->CollectionIndex, changelist);
        fPopupDict->WordChanged(changelist->CollectionIndex, changelist);
    }

    delete changelist;
    changelist = NULL;
}
//---------------------------------------------------------------------------
void add_word_line(TKanjiData &k, int i)
{
    // Only add new.
    if (k.wordscnt)
    {
        for (int ix = 0; ix < k.wordscnt; ++ix)
            if (k.words[ix] == i)
                return;

        int *tmp = k.words;
        ++k.wordscnt;
        k.words = new int[k.wordscnt];
        memcpy(k.words, tmp, sizeof(int) * (k.wordscnt - 1));
        delete[] tmp;
    }
    else
    {
        ++k.wordscnt;
        k.words = new int[1];
    }
    k.words[k.wordscnt - 1] = i;
}
//---------------------------------------------------------------------------
void delete_word_line(TKanjiData &k, int i)
{
    for (int ix = 0; ix < k.wordscnt; ++ix)
    {
        if (k.words[ix] != i)
            continue;
        int *tmp = k.words;
        if (k.wordscnt > 1)
        {
            k.words = new int[k.wordscnt - 1];
            if (ix > 0)
                memcpy(k.words, tmp, sizeof(int) * ix);
            if (ix != k.wordscnt - 1)
                memcpy(k.words + ix, tmp + ix + 1, sizeof(int) * (k.wordscnt - ix - 1));
        }
        else
            k.words = NULL;
        --k.wordscnt;
        delete[] tmp;
    }
}
//---------------------------------------------------------------------------
void add_valid_word_line(TValidData &d, int i)
{
    //add only new words
    if (d.wordscnt)
    {
        for (int ix = 0; ix < d.wordscnt; ++ix)
            if (d.words[ix] == i)
                return;
        int *tmp = d.words;
        ++d.wordscnt;
        d.words = new int[d.wordscnt];
        memcpy(d.words, tmp, sizeof(int) * (d.wordscnt - 1));
        delete[] tmp;
    }
    else
    {
        ++d.wordscnt;
        d.words = new int[1];
    }
    d.words[d.wordscnt - 1] = i;
}
//---------------------------------------------------------------------------
void delete_valid_word_line(TValidData &d, int i)
{
    for (int ix = 0; ix < d.wordscnt; ++ix)
    {
        if (d.words[ix] != i)
            continue;
        int *tmp = d.words;
        if (d.wordscnt > 1)
        {
            d.words = new int[d.wordscnt - 1];
            if (ix > 0)
                memcpy(d.words, tmp, sizeof(int) * ix);
            if (ix != d.wordscnt - 1)
                memcpy(d.words + ix, tmp + ix + 1, sizeof(int) * (d.wordscnt - ix - 1));
        }
        else
            d.words = NULL;
        --d.wordscnt;
        delete[] tmp;
    }
}
//---------------------------------------------------------------------------
void Translate(UnicodeString txt)
{
    if (settings.intray)
    {
        if (!fPopupDict->Visible)
            fPopupDict->Popup(true, dmKanji);
        else if (!fPopupDict->pKanji->Visible)
            fPopupDict->btnKanji->Click();
        fPopupDict->edkanji->Text = txt;
        fPopupDict->SetFocus();
    }
    else
    {
        if (!fDict->pKanji->Visible)
            fDict->btnKanji->Click();
        fDict->edkanji->Text = txt;
        fDict->SetFocus();
    }
}
//---------------------------------------------------------------------------
void Translate(byte coll, UnicodeString kanji, UnicodeString kana)
{
    if (settings.intray)
    {
        if (!fPopupDict->Visible)
            fPopupDict->Popup(true, dmKanji);
        else if (!fPopupDict->pKanji->Visible)
            fPopupDict->btnKanji->Click();
    }
    else
    {
        if (!fDict->pKanji->Visible)
            fDict->btnKanji->Click();
    }

    fDict->SelectDictionary(coll);
    int ix = FindWordIndex(dictionary->Items[coll], kanji.c_str(), kana.c_str());
    if (ix < 0)
    {
        Translate(kana);
        return;
    }

    if (settings.intray)
    {
        fPopupDict->SearchWord(coll, dictionary->Items[coll]->words->Items[ix]);
        fPopupDict->SetFocus();
    }
    else
    {
        fDict->SearchWord(coll, dictionary->Items[coll]->words->Items[ix]);
        fDict->SetFocus();
    }

}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(19);
#endif
