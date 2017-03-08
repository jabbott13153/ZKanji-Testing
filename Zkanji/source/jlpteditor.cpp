//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <IniFiles.hpp>
#include "jlpteditor.h"
#include "zkformats.h"
#include "searchtree.h"
#include "words.h"
#include "collection.h"
#include "romajizer.h"
#include "wordobj.h"
#include "settings.h"
#include "gengraph.h"
#include "kanjiedit.h"
#include "kanji.h"
#include "mouseevent.h"
#include "module.h"
#include "wordgroup.h"
#include "grouplist.h"
#include "romajizer.h"
#include "sentences.h"
#include "numinput.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#define JLPTLIST_VERSION     2

#ifdef _DEBUG
static TGlobalTest tester1(64);
#endif

TfrmJLPTEdit *frmJLPTEdit;

//---------------------------------------------------------------------------
TTempJLPTItemList::TTempJLPTItemList() : fcnt(0), defcnt(0)
{
    //defs = new TGapStringList(false);
}
//---------------------------------------------------------------------------
TTempJLPTItemList::~TTempJLPTItemList()
{
    Clear();
    //delete defs;
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::Clear()
{
    inherited::Clear();
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::ResetKanji()
{
    for (int ix = 0; ix < kanjis->Count; ++ix)
        kanjis->Kanjis[ix]->NJLPT = kanjis->Kanjis[ix]->JLPT + (kanjis->Kanjis[ix]->JLPT >= 3 ? 1 : 0);
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::Delete(int ix)
{
    delete[] Items[ix]->meaningorig;
    if (Items[ix]->next == Items[ix])
    {
        if (Items[ix]->definition)
            defcnt--;
        delete[] Items[ix]->definition;
    }
    else
    {
        TTempJLPTItem *item = Items[ix];
        while (item->next != Items[ix])
            item = item->next;
        item->next = Items[ix]->next;
    }

    inherited::Delete(ix);
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::Preload()
{
    Clear();
    ResetKanji();
    for (int ix = 5; ix >= 1; --ix)
        LoadN(ix);
    /*
     wchar_t line[1024];
     bool big = false;
     FILE *f = _wfopen((paths->installdatapath+L"JLPT\\1kyu.txt").c_str(),L"rb");

     TStringList *sl = new TStringList;
     while (getlinew(line,1024,f,big)) {
     if (!wcslen(line))
     continue;

     int tc = 2;
     int p = 0;
     while (tc && p < 1024) {
     p++;
     if (line[p] == L'\t')
     tc--;
     }
     if (p == 1024) {
     p = 0;
     continue;
     }
     line[p] = 0;
     sl->Add(line);
     }

     fclose(f);

     f = _wfopen((paths->installdatapath+L"JLPT\\excel1kyu.txt").c_str(),L"rb");
     FILE *f2 = _wfopen((paths->installdatapath+L"JLPT\\1kyu2.txt").c_str(),L"ab");
     while (getlinew(line,1024,f,big)) {
     if (!wcslen(line))
     continue;

     UnicodeString s = line;

     int tc = 2;
     int p = 0;
     while (tc) {
     p++;
     if (line[p] == L'\t')
     tc--;
     }
     line[p] = 0;

     bool found = false;
     for (int ix = 0; ix < sl->Count && !found; ++ix)
     found = sl->Strings[ix] == line;
     if (found)
     continue;

     fwrite(s.c_str(),2,s.Length(),f2);
     fwrite(L"\r\n",2,2,f2);
     }
     fclose(f);
     fclose(f2);

     delete sl;
     */
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::LoadN(int n)
{
    FILE *f = _wfopen((paths.installdatapath + "JLPT\\N" + n + ".txt").c_str(), L"rb");
    if (!f)
        THROW(L"JLPT base data not found!");

    wchar_t line[1024];
    bool big = false;

    wchar_t *kanji = NULL, *kana = NULL;

    TWordIndexList *l = NULL;

    /*static TGapList<int> N5kanji(false);
     static TGapList<int> N4kanji(false);
     static TGapList<int> N3kanji(false);
     static TGapList<int> N2kanji(false);
     static TGapList<int> N1kanji(false);

     TGapList<int> *Nkanjis[6] = { NULL, &N1kanji, &N2kanji, &N3kanji, &N4kanji, &N5kanji };

     static TGapList<bool> Nkanji;

     TGapList<int> N1extra;
     */
    try
    {
        l = new TWordIndexList(dictionary->Items[0]);
        TTempJLPTArea area = jaFixed;
        big = false;
        while (getlinew(line, 1024, f, big))
        {
            if (wcscmp(line, L"---") == 0)
            {
                area = TTempJLPTArea(area + 1);
                continue;
            }

            wchar_t *ln = line;

            TTempJLPTItem *item = new TTempJLPTItem;
            item->definition = NULL;
            item->Nlevel = n;
            item->Nlevel_orig = n;
            item->use = true;
            item->windex = -1;
            item->kanaonly = line[0] == L'(';
            item->usekana = true;
            item->mark = jmNone;
            if (item->kanaonly)
                ln++;

            wchar_t *tpos = wcschr(ln, L'\t');

            if (!tpos)
                THROW(L"bad line");
            if (item->kanaonly)
                * (tpos - 1) = 0;
            else
                *tpos = 0;

            kanji = new wchar_t[wcslen(ln) + 1];
            wcscpy(kanji, ln);
            item->haskanji = false;

            for (int ix = wcslen(ln) - 1; ix >= 0 && !item->haskanji; --ix)
            { // /*&& !item->haskanji*/; --ix)
                if (KANJI(ln[ix]))
                {
                    item->haskanji = true;
                    /*Nkanji.Items[ln[ix]] = true;
                     TKanji *k = kanjis->Kanjis[kanjis->IndexOf(ln[ix])];

                     if (n == 1)
                     N1extra.Items[ln[ix]] = k->JLPT;


                     if (!item->kanaonly && item->Nlevel-(item->Nlevel == 3 ? 1 : 0) == k->JLPT+(k->JLPT > 2 ? 1 : 0 )  ) {
                     if ((*Nkanjis[item->Nlevel-(item->Nlevel == 3 ? 1 : 0)]).Items[ln[ix]] <= 0)
                     (*Nkanjis[item->Nlevel-(item->Nlevel == 3 ? 1 : 0)]).Items[ln[ix]] = 1;
                     } else if (!item->kanaonly && k->JLPT > 0 && k->JLPT <= 4 &&
                     !(*Nkanjis[k->JLPT+(k->JLPT > 2 ? 1 : 0 )]).ItemSet(ln[ix]))
                     (*Nkanjis[k->JLPT+(k->JLPT > 2 ? 1 : 0 )]).Items[ln[ix]] = -item->Nlevel;
                     */
                }
            }
            if (!item->haskanji)
                item->kanaonly = true;

            ln = tpos + 1;
            tpos = wcschr(ln, L'\t');
            if (!tpos)
                THROW(L"bad line");
            *tpos = 0;
            kana = new wchar_t[wcslen(ln) + 1];
            wcscpy(kana, ln);

            ln = tpos + 1;
            UnicodeString s = ln;
            for (int ix = s.Length(); ix > 1; ix--)
            {
                if (s[ix] != L' ' && (s[ix - 1] == L',' || s[ix - 1] == L'、'))
                {
                    s = s.SubString(1, ix - 2) + ", " + s.SubString(ix, s.Length());
                }
            }
            item->meaningorig = new wchar_t[s.Length() + 1];
            wcscpy(item->meaningorig, s.c_str());

            item->area = area;

            FindWord(Romanize(kana).c_str(), l, true, true, false, false, NULL);
            for (int ix = l->Count - 1; ix >= 0; --ix)
            {
                if (wcscmp(l->Words[ix]->kanji, kanji) || wcscmp(l->Words[ix]->kana, kana))
                    l->Delete(ix);
            }

            if (n == 1)
            {
                ; //
            }

            if (l->Count != 1)
                THROW(L"Word not found...");

            delete[] kanji;
            kanji = NULL;
            delete[] kana;
            kana = NULL;

            item->windex = l->Indexes[0];
            item->next = item;
            fcnt++;

            for (int ix = Count - 1; ix >= 0; --ix)
            {
                if (Items[ix]->windex == item->windex)
                {
                    item->next = Items[ix]->next;
                    Items[ix]->next = item;
                    item->definition = Items[ix]->definition;
                    fcnt--;

                    /*for (int iy = 0; iy <= ix; ++iy) {
                     if (Items[iy]->windex == item->windex) {
                     item->next = Items[iy];
                     item->definition = Items[iy]->definition;
                     break;
                     }
                     }*/

                    break;
                }
            }

            l->Clear();

            inherited::Add(item);
        }

    }
    catch (...)
    {
        delete[] kanji;
        delete[] kana;
        delete l;
        fclose(f);
        throw;
    }

    if (n == 1)
    {
        /*UnicodeString str;

         int tc = Nkanji.TrueCount;
         int n5k = 0, n4k = 0, n3k = 0, n2k = 0, n1k = 0;
         int n5x = 0, n4x = 0, n3x = 0, n2x = 0, n1x = 0;

         int nonset = 0;

         UnicodeString m1,m2,m4,m5;
         for (int ix = 0; ix < kanjis->Count; ++ix) {
         TKanji *k = kanjis->Kanjis[ix];
         switch (k->JLPT) {
         case 1:
         m1 += k->ch;
         break;
         case 2:
         m2 += k->ch;
         break;
         case 3:
         m4 += k->ch;
         break;
         case 4:
         m5 += k->ch;
         break;
         }
         }


         for (int ix = 0; ix < tc; ++ix) {
         wchar_t k = (wchar_t)Nkanji.TruePos[ix];
         if (N5kanji.Items[k] > 0)
         n5k++;
         else if (N5kanji.Items[k] < 0)
         n5x++;
         if (N4kanji.Items[k] > 0)
         n4k++;
         else if (N4kanji.Items[k] < 0)
         n4x++;
         if (N3kanji.Items[k] > 0)
         n3k++;
         else if (N3kanji.Items[k] < 0)
         n3x++;
         if (N2kanji.Items[k] > 0)
         n2k++;
         else if (N2kanji.Items[k] < 0)
         n2x++;
         if (N1kanji.Items[k] > 0)
         n1k++;
         else if (N1kanji.Items[k] < 0)
         n1x++;
         if (!N1kanji.Items[k] && !N2kanji.Items[k] && !N3kanji.Items[k] && !N4kanji.Items[k] && !N5kanji.Items[k])
         str += k;

         if (N5kanji.Items[k]) {
         int p = m5.Pos(k);
         if (p)
         m5[p] = L' ';
         }
         if (N4kanji.Items[k]) {
         int p = m4.Pos(k);
         if (p)
         m4[p] = L' ';
         }
         if (N2kanji.Items[k]) {
         int p = m2.Pos(k);
         if (p)
         m2[p] = L' ';
         }
         if (N1kanji.Items[k]) {
         int p = m1.Pos(k);
         if (p)
         m1[p] = L' ';
         }





         if (!N1extra.ItemSet(k))
         nonset++;
         }
         int n5f = n5k+n5x;
         int n4f = n4k+n4x;
         int n3f = n3k+n3x;
         int n2f = n2k+n2x;
         int n1f = n1k+n1x;

         int n1e1 = 0;
         int n1e2 = 0;
         //    int n1e3 = 0;
         int n1e4 = 0;
         int n1e5 = 0;
         int n1e0 = 0;
         for (int ix = 0; ix < N1extra.TrueCount; ++ix) {
         wchar_t k = (wchar_t)N1extra.TruePos[ix];
         if (N1extra.Items[k] == 1)
         n1e1++;
         else if (N1extra.Items[k] == 2)
         n1e2++;
         else if (N1extra.Items[k] == 3)
         n1e4++;
         else if (N1extra.Items[k] == 4)
         n1e5++;
         else
         n1e0++;
         }

         int nsum = n5k+n4k+n3k+n2k+n1k + str.Length();
         */

        /*
         FILE *f1 = NULL;
         UnicodeString kyustr;
         f1 = _wfopen((paths.installdatapath+"JLPT\\excel2kyu.txt").c_str(),L"rb");

         FILE *f2 = NULL;
         f2 = _wfopen((paths.installdatapath+"JLPT\\N2extra.txt").c_str(),L"wb");
         FILE *f3 = NULL;
         f3 = _wfopen((paths.installdatapath+"JLPT\\N2missing.txt").c_str(),L"wb");
         wchar_t bom = 0xFEFF;
         fwrite(&bom,2,1,f2);
         fwrite(&bom,2,1,f3);

         while (getlinew(line,1024,f1,big)) {
         l->Clear();
         wchar_t *kanji = line;
         wchar_t *kana = wcschr(line,L'\t');
         *kana = NULL;
         kana++;
         wchar_t *def = wcschr(kana,L'\t');
         *def = NULL;
         def++;

         *wcschr(def,L'\t') = NULL;

         while (*def == L'(') {
         wchar_t *br = def;
         while (*br) {
         br++;
         if (*br == L')') {
         br += 2;
         break;
         }
         }
         if (*br)
         def = br;
         }


         FindWord(Romanize(kana).c_str(),l,true,true,false,false,NULL);
         for (int ix = l->Count-1; ix >= 0; --ix) {
         if (wcscmp(l->Words[ix]->kanji,kanji) || wcscmp(l->Words[ix]->kana,kana))
         l->Delete(ix);
         }
         if (l->Count != 1) {
         UnicodeString s = UnicodeString(kanji)+L"\t"+kana+L"\t"+def;
         fwrite(s.c_str(),2,s.Length(),f3);
         fwrite(L"\r\n",2,2,f3);
         continue;
         }

         bool found = false;
         for (int ix = 0; ix < Count && !found; ++ix) {
         TWord *w = Words[ix];
         found = !wcscmp(w->kanji,kanji) && !wcscmp(w->kana,kana);
         }
         if (!found) {
         UnicodeString s = UnicodeString(kanji)+L"\t"+kana+L"\t"+def;
         fwrite(s.c_str(),2,s.Length(),f2);
         fwrite(L"\r\n",2,2,f2);
         }
         }

         fclose(f1);
         fclose(f2);
         fclose(f3);
         */
    }

    delete l;
    fclose(f);
}
//---------------------------------------------------------------------------
TWord* TTempJLPTItemList::get_word(unsigned int ix)
{
    if (ix >= (unsigned int)Count)
        THROW(L"Index out of range!");
    return dictionary->Items[0]->words->Items[Items[ix]->windex];
}
//---------------------------------------------------------------------------
UnicodeString TTempJLPTItemList::get_area(unsigned int ix)
{
    if (ix >= (unsigned int)Count)
        THROW(L"Index out of range!");
    switch (Items[ix]->area)
    {
    case jaFixed:
        return "X";
    case jaFound:
        return "F";
    case jaMatch:
        return "M";
    default:
        return "?";
    }
}
//---------------------------------------------------------------------------
UnicodeString TTempJLPTItemList::get_meaning(unsigned int ix)
{
    if (ix >= (unsigned int)Count)
        THROW(L"Index out of range!");
    if (Items[ix]->definition)
        return Items[ix]->definition;
    return Items[ix]->meaningorig;
}
//---------------------------------------------------------------------------
UnicodeString TTempJLPTItemList::get_cmeaning(unsigned int ix)
{
    if (ix >= (unsigned int)Count)
        THROW(L"Index out of range!");

    TStringList *l = new TStringList;

    UnicodeString c;
    try
    {
        TTempJLPTItem *item = Items[ix];
        do
        {
            if (Items[ix]->definition)
                l->Add(Items[ix]->definition);
            else
                l->Add(item->meaningorig);
            item = item->next;
        }
        while (item != Items[ix]);

        for (int ix = 0; ix < l->Count; ++ix)
        {
            if (!c.Length())
                c += l->Strings[ix];
            else
            {
                bool found = false;
                for (int iy = 0; iy < ix && !found; ++iy)
                {
                    if (l->Strings[iy] == l->Strings[ix])
                        found = true;
                }
                if (!found)
                    c += " | " + l->Strings[ix];
            }
        }
    }
    catch (...)
    {
        ;
    }
    delete l;

    return c;
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::set_meaning(unsigned int ix, UnicodeString val)
{
    if (ix >= (unsigned int)Count)
        THROW(L"Index out of range!");
    val = val.Trim();

    if (Items[ix]->definition)
    {
        defcnt--;
        delete[] Items[ix]->definition;
    }

    if (!val.Length())
        Items[ix]->definition = NULL;
    else
    {
        Items[ix]->definition = new wchar_t[val.Length() + 1];
        wcscpy(Items[ix]->definition, val.c_str());
        defcnt++;
    }
    TTempJLPTItem *item = Items[ix];
    while (item->next != Items[ix])
    {
        item->next->definition = item->definition;
        item = item->next;
    }
}
//---------------------------------------------------------------------------
bool TTempJLPTItemList::get_own(unsigned int ix)
{
    if (ix >= (unsigned int)Count)
        THROW(L"Index out of range!");
    return Items[ix]->definition != NULL;
}
//---------------------------------------------------------------------------
UnicodeString TTempJLPTItemList::get_dictmeaning(unsigned int ix)
{
    TWord *w = Words[ix];
    UnicodeString str = "";

    for (int ix = 0; ix < w->meaningcnt; ++ix)
    {
        if (ix > 0)
            str += "; ";
        str += w->data[ix].meaning;
    }

    return str;

}
//---------------------------------------------------------------------------
bool TTempJLPTItemList::get_valid(unsigned int ix)
{
    TTempJLPTItem *item = Items[ix];
    if (item == item->next)
        return true;

    bool valid = true;
    int kanjiat = 0;
    int kanaat = 99;
    bool levels[5];
    memset(levels, 0, sizeof(bool)*5);
    do
    {
        if (item->use)
        {
            if (item->kanaonly && kanjiat > item->Nlevel || !item->kanaonly && kanaat < item->Nlevel)
                valid = false;
            if (levels[5 - item->Nlevel])
                valid = false;
            levels[5 - item->Nlevel] = true;
            if (!item->kanaonly)
                kanjiat = max(item->Nlevel, kanjiat);
            else
                kanaat = min(item->Nlevel, kanaat);
        }
        item = item->next;
    }
    while (item != Items[ix] && valid);
    return valid;
}
//---------------------------------------------------------------------------
bool TTempJLPTItemList::get_npresent(unsigned int ix, byte n)
{
    TTempJLPTItem *i = Items[ix];
    do
    {
        if (i->Nlevel == n)
            return true;
        i = i->next;
    }
    while (i != Items[ix]);
    return false;
}
//---------------------------------------------------------------------------
bool TTempJLPTItemList::get_nused(unsigned int ix, byte n)
{
    TTempJLPTItem *i = Items[ix];
    do
    {
        if (i->Nlevel == n && i->use)
            return true;
        i = i->next;
    }
    while (i != Items[ix]);
    return false;
}
//---------------------------------------------------------------------------
int TTempJLPTItemList::get_nlvl(unsigned int ix)
{
    bool n[5];
    memset(n, 0, sizeof(bool)*5);
    int cnt = 0;
    TTempJLPTItem *i = Items[ix];
    do
    {
        if (!n[i->Nlevel - 1] && i->use)
        {
            n[i->Nlevel - 1] = true;
            cnt++;
        }
        i = i->next;
    }
    while (i != Items[ix]);
    return cnt;
}
//---------------------------------------------------------------------------
int TTempJLPTItemList::get_klvl(unsigned int ix)
{
    if (!Items[ix]->haskanji)
        return 0;
    int maxN = 6;
    TWord *w = Words[ix];
    for (int ix = wcslen(w->kanji) - 1; ix >= 0; --ix)
    {
        if (!KANJI(w->kanji[ix]))
            continue;
        int jlpt = kanjis->Kanjis[kanjis->IndexOf(w->kanji[ix])]->NJLPT;
        if (!jlpt)
        {
            jlpt = kanjis->Kanjis[kanjis->IndexOf(w->kanji[ix])]->JLPT;
            if (jlpt >= 3)
                jlpt++;
        }
        maxN = min(jlpt, maxN);
    }
    return maxN;
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::ChangeIndex(unsigned int ix, int windex)
{
    TTempJLPTItem *it = Items[ix];
    if (it->next == it)
    {
        it->windex = windex;
        if (it->definition)
        {
            defcnt--;
            delete[] it->definition;
            it->definition = NULL;
        }
    }
    else
    {
        TTempJLPTItem *oit = it->next;
        while (oit->next != it)
            oit = oit->next;
        oit->next = it->next;
        it->next = it;
        it->windex = windex;
        it->definition = NULL;
    }

    it->haskanji = false;
    for (int ix = wcslen(Words[ix]->kanji) - 1; ix >= 0 && !it->haskanji; --ix)
        it->haskanji = KANJI(Words[ix]->kanji[ix]);

    if (!it->haskanji)
        it->kanaonly = true;
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::AddToBunch(unsigned int ix, int bix)
{
    TTempJLPTItem *it = Items[ix];
    if (it->next == it)
    {
        if (it->definition)
        {
            defcnt--;
            delete[] it->definition;
        }
    }
    else
    {
        TTempJLPTItem *oit = it->next;
        while (oit->next != it)
            oit = oit->next;
        oit->next = it->next;
    }
    it->next = Items[bix]->next;
    Items[bix]->next = it;
    it->windex = Items[bix]->windex;
    it->definition = Items[bix]->definition;

    it->haskanji = false;
    for (int ix = wcslen(Words[ix]->kanji) - 1; ix >= 0 && !it->haskanji; --ix)
        it->haskanji = KANJI(Words[ix]->kanji[ix]);

    if (!it->haskanji)
        it->kanaonly = true;
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::AutoFix(unsigned int ix)
{
    TTempJLPTItem *i = Items[ix];

    do
    {
        if (i->mark == jmFixed)
            return;
        i = i->next;
    }
    while (i != Items[ix]);

    do
    {
        if (i->use)
            break;
        i = i->next;
    }
    while (i != Items[ix]);
    if (!i->use)//skip word if no used present
                return;

    TTempJLPTItem *c = i->next;
    int itemcnt = 1;
    while (c != i)
    {
        if (c->use)
        {
            itemcnt++;
            if (c->Nlevel > i->Nlevel)//return if used item's level is not the lowest
                        return;
        }
        c = c->next;
    }

    TWord *w = Words[ix];
    int maxN = 5;

    for (int j = wcslen(w->kanji) - 1; j >= 0; --j)
    {
        if (!KANJI(w->kanji[j]))
            continue;

        TKanji *k = kanjis->Kanjis[kanjis->IndexOf(w->kanji[j])];
        maxN = min(maxN, (k->NJLPT ? k->NJLPT : (k->JLPT >= 3 ? k->JLPT + 1 : k->JLPT)));
    }

    if (itemcnt == 1)
    {
        if (!i->haskanji || i->kanaonly || i->Nlevel <= maxN)
            return;

        if (maxN == 0)
        {
            i->kanaonly = true;
            return;
        }

        i->kanaonly = true;
        TTempJLPTItem *item = new TTempJLPTItem;
        item->use = true;
        item->kanaonly = false;
        item->usekana = true;
        item->haskanji = true;
        item->meaningorig = new wchar_t[wcslen(i->meaningorig) + 1];
        wcscpy(item->meaningorig, i->meaningorig);
        item->windex = i->windex;

        item->Nlevel = maxN;
        item->Nlevel_orig = maxN;
        item->area = jaFixed;
        item->next = i;
        item->definition = i->definition;

        c = i;
        while (c->next != i)
            c = c->next;
        c->next = item;

        Add(item);
        return;
    }

    TTempJLPTItem *i2 = i->next;
    if (itemcnt > 2)
    {
        while (!i2->use || i2 != i && i2->Nlevel > maxN)
            i2 = i2->next;

        if (i2 == i)
        {
            do
            {
                i2 = i2->next;
            }
            while (!i2->use || i2 == i);
        }
        c = i;
        do
        {
            if (c != i2 && c != i)
                c->use = false;
            c = c->next;
        }
        while (c != i);
    }
    else
    { //itemcnt == 2, only one other useable item left
        while (!i2->use)
            i2 = i2->next;
    }

    if (i->Nlevel <= maxN || !i->haskanji || maxN == 0)
    {
        i2->use = maxN > 0 && i->haskanji && i2->Nlevel < i->Nlevel && i->kanaonly;

        if (i->haskanji)
        {
            if (maxN == 0)
                i->kanaonly = true;
            else if (i->kanaonly && i2->use)
                i2->kanaonly = false;
        }
        return;
    }

    i->kanaonly = true;
    i2->kanaonly = false;

    i2->Nlevel = min(i2->Nlevel, maxN);
    i2->Nlevel_orig = i2->Nlevel;
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::AutoFixKatakana(unsigned int ix)
{
    if (Items[ix]->definition || Items[ix]->haskanji && !Items[ix]->kanaonly)
        return;

    bool haskata = false;
    for (int j = wcslen(Words[ix]->kana) - 1; j >= 0 && !haskata; --j)
        haskata = KATAKANA(Words[ix]->kana[j]);

    if (!haskata)
        return;

    UnicodeString s;
    for (int j = 0; j < Words[ix]->meaningcnt; ++j)
    {
        if (j)
            s += "; ";
        s += Words[ix]->data[j].meaning;
    }
    Items[ix]->definition = new wchar_t[s.Length() + 1];
    wcscpy(Items[ix]->definition, s.c_str());
    defcnt++;

    TTempJLPTItem *it = Items[ix]->next;
    while (!it->definition)
    {
        it->definition = Items[ix]->definition;
    }
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::AutoDefineWords(unsigned int ix, int max_word_len, int min_def_count)
{
    if (Items[ix]->definition)
        return;

    int defcnt = 0;

    UnicodeString s;
    for (int j = 0; j < Words[ix]->meaningcnt; ++j)
    {
        if (j)
            s += "; ";
        s += Words[ix]->data[j].meaning;
        defcnt++;
    }

    UnicodeString s2 = s;
    while (defcnt <= min_def_count)
    {
        if (s2.Pos(L','))
        {
            defcnt++;
            s2 = s2.SubString(s2.Pos(L',') + 1, s2.Length());
        }
        else
            break;
    }

    if (s.Length() > max_word_len && defcnt > min_def_count)
        return;

    UnicodeString tmp = s;
    do
    {
        int p = tmp.Pos(L"(");
        if (p > 0)
        {
            tmp = tmp.SubString(p + 1, tmp.Length());
            p = tmp.Pos(L")");

            if (p > 0)
            {
                tmp = tmp.SubString(p + 1, tmp.Length());
                UnicodeString br = tmp.SubString(1, p - 1);
                try
                {
                    int k = _wtoi(br.c_str());
                    if (UnicodeString(k) == br)
                        return;
                }
                catch (...)
                {
                    ;
                }
            }

        }
        else
            break;
    }
    while (tmp.Length());

    Items[ix]->definition = new wchar_t[s.Length() + 1];
    wcscpy(Items[ix]->definition, s.c_str());
    //defcnt++;

    TTempJLPTItem *it = Items[ix]->next;
    while (!it->definition)
    {
        it->definition = Items[ix]->definition;
    }
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::SaveToFile(UnicodeString filename)
{
    FILE *f = _wfopen(filename.c_str(), L"wb");
    if (!f)
        THROW(L"Couldn't open file for writing!");

    TGapList<TTempJLPTItem*> items;
    try
    {
        char verstr[7];
        sprintf(verstr, "zjv%03i", JLPTLIST_VERSION);
        fwrite(verstr, 1, 6, f);
        int cnt = Count;
        fwrite(&cnt, sizeof(int), 1, f);

        for (int ix = 0; ix < Count; ++ix)
        {
            TTempJLPTItem *item = Items[ix];
            fwrite(&item->kanaonly, sizeof(bool), 1, f);
            fwrite(&item->usekana, sizeof(bool), 1, f); //2011.10.25 v2
            fwrite(&item->haskanji, sizeof(bool), 1, f);
            write(item->meaningorig, f, sltWordAddNULL);
            fwrite(&item->windex, sizeof(int), 1, f);
            if (!items.ItemSet(item->windex))
                items.Items[item->windex] = item;
            fwrite(&item->Nlevel, sizeof(byte), 1, f);
            fwrite(&item->Nlevel_orig, sizeof(byte), 1, f);
            fwrite(&item->area, sizeof(TTempJLPTArea), 1, f);
            fwrite(&item->use, sizeof(bool), 1, f);
            fwrite(&item->mark, sizeof(TTempJLPTMark), 1, f);
        }

        cnt = items.TrueCount;
        fwrite(&cnt, sizeof(int), 1, f);
        for (int ix = 0; ix < cnt; ++ix)
        {
            int p = items.TruePos[ix];
            TWord *w = dictionary->Items[0]->words->Items[p];
            fwrite(&p, sizeof(int), 1, f);
            write(w->kanji, f, sltByteAddNULL);
            write(w->kana, f, sltByteAddNULL);
            write(items.TrueItems[ix]->definition, f, sltWordAddNULL, (items.TrueItems[ix]->definition ? -1 : 0));
        }

        for (int ix = 0; ix < kanjis->Count; ++ix)
            fwrite(&kanjis->Kanjis[ix]->NJLPT, sizeof(byte), 1, f);
    }
    catch (...)
    {
        fclose(f);
        throw;
    }
    fclose(f);
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::LoadFromFile(UnicodeString filename)
{
    FILE *f = _wfopen(filename.c_str(), L"rb");
    if (!f)
        THROW(L"Couldn't open file for reading!");

    TGapList<TTempJLPTItem*> items;
    TWordIndexList *l = NULL;
    Clear();
    ResetKanji();

    fcnt = 0;

    int version;

    try
    {
        char tmp[7];
        fread(tmp, 1, 6, f);
        if (strncmp(tmp, "zjv", 3))
            THROW(L"Invalid file!");
        tmp[6] = 0;
        version = strtol(tmp + 3, 0, 10);

        int cnt;
        fread(&cnt, sizeof(int), 1, f);
        Capacity = cnt;
        while (Count < Capacity)
        {
            TTempJLPTItem *item = new TTempJLPTItem;
            item->next = item;
            item->definition = NULL;
            fread(&item->kanaonly, sizeof(bool), 1, f);
            if (version >= 2)
                fread(&item->usekana, sizeof(bool), 1, f);
            else
                item->usekana = true;

            fread(&item->haskanji, sizeof(bool), 1, f);
            read(item->meaningorig, f, sltWordAddNULL);
            fread(&item->windex, sizeof(int), 1, f);
            if (!items.ItemSet(item->windex))
            {
                items.Items[item->windex] = item;
                fcnt++;
            }
            else
            {
                item->next = items.Items[item->windex]->next;
                items.Items[item->windex]->next = item;
                /*TTempJLPTItem *t;
                 TTempJLPTItem *orig;
                 item->next = orig = t = items.Items[item->windex];
                 while (t->next != orig)
                 t = t->next;
                 t->next = item;
                 */
            }
            fread(&item->Nlevel, sizeof(byte), 1, f);
            fread(&item->Nlevel_orig, sizeof(byte), 1, f);
            fread(&item->area, sizeof(TTempJLPTArea), 1, f);
            fread(&item->use, sizeof(bool), 1, f);
            fread(&item->mark, sizeof(TTempJLPTMark), 1, f);

            inherited::Add(item);
        }

        // The word indexes will get updated in the next run to accomodate dictionary changes.
        // First set unchanged index to negative to tell apart changed and unchanged.
        for (int ix = 0; ix < Count; ++ix)
            Items[ix]->windex = -1 - Items[ix]->windex;

        l = new TWordIndexList(dictionary->Items[0]);
        fread(&cnt, sizeof(int), 1, f);
        for (int ix = 0; ix < cnt; ++ix)
        {
            int wix;
            fread(&wix, sizeof(int), 1, f);
            wchar_t *kanji, *kana, *def;
            read(kanji, f, sltByteAddNULL);
            read(kana, f, sltByteAddNULL);

            //if (wcscmp(kana, L"うれしい") == 0) // Probably debug line.
            //    def = 0;

            int iix = Count - 1;
            for ( ; iix >= 0 && Items[iix]->windex != -1 - wix; --iix)
                ;

            if (iix < 0)
                MsgBox("Error in data! Word not found!", "Error", MB_OK);

            int dlen = read(def, f, sltWordAddNULL);
            if (!dlen || iix < 0)
            {
                delete[] def;
                def = NULL;
            }
            else if (iix >= 0)
            {
                TTempJLPTItem *it = Items[iix];
                defcnt++;
                do
                {
                    it->definition = def;
                    it = it->next;
                }
                while (it != Items[iix]);
            }

            int newwindex = -1 - Items[iix]->windex;
            //bool mismatch = false;
            if (iix >= 0 && (dictionary->Items[0]->words->Count <= wix || wcscmp(dictionary->Items[0]->words->Items[wix]->kana, kana) || wcscmp(dictionary->Items[0]->words->Items[wix]->kanji, kanji)))
            {
                //mismatch = true;
                l->Clear();
                FindWord(Romanize(kana).c_str(), l, true, true, false, false, NULL);
                for (int iy = l->Count - 1; iy >= 0; --iy)
                    if (wcscmp(l->Words[iy]->kanji, kanji) || wcscmp(l->Words[iy]->kana, kana))
                        l->Delete(iy);

                if (l->Count == 0) // Try to find the word manually.
                {
                    UnicodeString value = UnicodeString(kanji) + L" " + UnicodeString(kana);
                    do
                    {
                        QueryBox(L"Enter correct kanji and kana for word:\r\n" + value, L"Word not found", value);
                        value = value.Trim();

                        int p = value.Pos(L" ");
                        if (p != 0)
                        {
                            delete[] kanji;
                            delete[] kana;
                            kanji = new wchar_t[p];
                            wcscpy(kanji, value.SubString(1, p - 1).c_str());
                            value = value.SubString(p + 1, value.Length() - p).Trim();
                            kana = new wchar_t[value.Length() + 1];
                            wcscpy(kana, value.c_str());

                            FindWord(Romanize(kana).c_str(), l, true, true, false, false, NULL);
                            for (int iy = l->Count - 1; iy >= 0; --iy)
                                if (wcscmp(l->Words[iy]->kanji, kanji) || wcscmp(l->Words[iy]->kana, kana))
                                    l->Delete(iy);
                        }
                    }
                    while (l->Count == 0 && value != L"");
                }

                if (l->Count == 1)
                    newwindex = l->Indexes[0];
            }

            if (newwindex >= 0) // Match with existing items in case a merge is needed.
            {
                int iindex = -1;

                for (int iy = 0; iy < Count && iindex == -1; ++iy)
                    if (Items[iy]->windex == newwindex)
                        iindex = iy;

                if (iindex == -1) // Not found another item with same windex.
                {
                    ;
                }
                else // Merge
                {
                    TTempJLPTItem *it1 = Items[iindex];
                    TTempJLPTItem *it2 = Items[iix];

                    while (it1->next != Items[iindex])
                        it1 = it1->next;
                    while (it2->next != Items[iix])
                        it2 = it2->next;

                    it2->next = Items[iindex];
                    it1->next = Items[iix];

                    if (it1->definition && it2->definition)
                    {
                        int newnum = 1;
                        InputQueryInt(L"Enter number:\r\n1 " + UnicodeString(it1->definition) + L"\r\n2" + UnicodeString(it2->definition), L"Pick definition to use", newnum, 1, 2);

                        if (newnum == 2)
                        {
                            delete[] it1->definition;
                            it1->definition = NULL;
                        }
                        else
                        {
                            delete[] it2->definition;
                            it2->definition = NULL;
                        }
                        --defcnt;
                    }

                    if ((it1->definition && !it2->definition) || (it2->definition && !it1->definition))
                    {
                        TTempJLPTItem *it = Items[iix];
                        do
                        {
                            it->definition = (it1->definition != NULL ? it1->definition : it2->definition);
                            it = it->next;
                        }
                        while (it != Items[iix]);
                    }

                }

                TTempJLPTItem *it = Items[iix];
                do
                {
                    it->windex = newwindex;
                    it = it->next;
                }
                while (it != Items[iix]);
            }
            else
            {
                for (; iix >= 0; --iix)
                {
                    bool canbreak = Items[iix]->next == Items[iix];
                    Delete(iix);
                    if (canbreak)
                        break;
                }
            }

            delete[] kanji;
            delete[] kana;
        }

        for (int ix = 0; ix < kanjis->Count; ++ix)
            fread(&kanjis->Kanjis[ix]->NJLPT, sizeof(byte), 1, f);

        // Set unchanged indexes that were in the dictionary in exact form back to positive.
        //for (int ix = Count - 1; ix >= 0; --ix)
        //    if (Items[ix]->windex < 0)
        //        Items[ix]->windex = -1 - Items[ix]->windex;

    }
    catch (...)
    {
        fclose(f);
        delete l;
        throw;
    }
    fclose(f);
    delete l;
}
//---------------------------------------------------------------------------
void TTempJLPTItemList::Export(UnicodeString filename)
{
    FILE *f = _wfopen(filename.c_str(), L"wb");
    if (!f)
        THROW(L"Couldn't open file for reading!");

    try
    {
        UnicodeString s;
        for (int ix = 0; ix < Count; ++ix)
        {
            if (!Items[ix]->use)
                continue;

            int minlvl = Items[ix]->Nlevel;
            bool usekanji = Items[ix]->haskanji && !Items[ix]->kanaonly;
            TTempJLPTItem *it = Items[ix]->next;
            while (it != Items[ix] && minlvl == Items[ix]->Nlevel)
            {
                if (it->use)
                {
                    minlvl = max(minlvl, it->Nlevel);
                    usekanji = usekanji || it->haskanji && !it->kanaonly;
                }
                it = it->next;
            }

            if (minlvl != Items[ix]->Nlevel)
                continue;

            s = UnicodeString(Words[ix]->kanji) + L"\t" + Words[ix]->kana + L"\t" + minlvl + L"\t" + (usekanji ? "1" : "0") + L"\r\n";
            fwrite(s.c_str(), sizeof(wchar_t), s.Length(), f);
        }

        fwrite(L"\r\n", sizeof(wchar_t), 2, f);

        for (int ix = 0; ix < kanjis->Count; ++ix)
        {
            s = UnicodeString(kanjis->Kanjis[ix]->ch) + kanjis->Kanjis[ix]->NJLPT + L"\r\n";
            fwrite(s.c_str(), sizeof(wchar_t), s.Length(), f);
        }

    }
    catch (...)
    {
        fclose(f);
        throw;
    }

    fclose(f);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool jpe_scaled_constants = false;

#define JLPT_COL_USE       0
#define JLPT_COL_NLEVEL    1
#define JLPT_COL_KANJI     2
#define JLPT_COL_KANA      3
#define JLPT_COL_AREA      4
#define JLPT_COL_MEANING   5

#define JLPT_HEADING_COUNT    7
int jlpt_defwidth_small[JLPT_HEADING_COUNT] = {
    58/*use*/, 25/*Nlevel*/, 60/*kanji*/,
    100/*kana*/, 34/*area*/, 6250/*meaning*/
};
int jlpt_defwidth_med[JLPT_HEADING_COUNT] = {
    60/*use*/, 25/*Nlevel*/, 80/*kanji*/,
    120/*kana*/, 38/*area*/, 6250/*meaning*/
};
int jlpt_defwidth_big[JLPT_HEADING_COUNT] = {
    72/*use*/, 28/*Nlevel*/, 100/*kanji*/,
    140/*kana*/, 41/*area*/, 6250/*meaning*/
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TfrmJLPTEdit::TfrmJLPTEdit(TComponent* Owner) : TBaseForm(Owner), items(NULL), wobj(NULL), view(NULL), freordering(false), edkanji(NULL), filename(""), fmodified(false)
{
    Constrain(550, 300);
    KeepSize = true;

    edkanji = new TKanjiEdit(edKanji, true, false, NULL, ecallback);
    items = new TTempJLPTItemList;

    TMemIniFile *ini = new TMemIniFile(paths.inipath + L"jlptedit.ini");
    try
    {
        filename = ini->ReadString(L"File", "Last open", "");
        if (!FileExists(filename))
            filename = "";
        else
            items->LoadFromFile(filename);

        int w = ini->ReadInteger(L"Window", "Width", 0);
        int h = ini->ReadInteger(L"Window", "Height", 0);
        if (w && h)
        {
            Width = w;
            Height = h;
        }

    }
    catch (...)
    {
        items->Clear();
        items->ResetKanji();
        filename = "";
    }
    delete ini;

    try
    {
        if (filename == "")
            items->Preload();
    }
    catch (...)
    {
        delete items;
        items = new TTempJLPTItemList;
        dlgOpen->InitialDir = paths.installdatapath;
        if (ExecuteDialog(dlgOpen))
        {
            filename = dlgOpen->FileName;
            try
            {
                items->LoadFromFile(filename);
            }
            catch (...)
            {
                filename = "";
            }
        }

        if (!filename.Length())
        {
            delete items;
            items = NULL;
            PostMessage(Handle, WM_CLOSE, 0, 0);
            return;
        }
    }
    count_ncount();

    view = new TIntegerList;
    for (int ix = 0; ix < items->Count; ++ix)
        view->Add(ix);

    if (!jpe_scaled_constants)
    {
        jpe_scaled_constants = true;
        for (int ix = 0; ix < JLPT_HEADING_COUNT; ++ix)
        {
            jlpt_defwidth_small[ix] *= scaleres;
            jlpt_defwidth_med[ix] *= scaleres;
            jlpt_defwidth_big[ix] *= scaleres;
        }
    }

    int cw[JLPT_HEADING_COUNT];
    int cwd[JLPT_HEADING_COUNT];
    switch (settings.dictfontsize)
    {
    case 0: //big
                memcpy(cw, jlpt_defwidth_big, sizeof(int)*JLPT_HEADING_COUNT);
        break;
    case 1: //med
                memcpy(cw, jlpt_defwidth_med, sizeof(int)*JLPT_HEADING_COUNT);
        break;
    default: //small
                memcpy(cw, jlpt_defwidth_small, sizeof(int)*JLPT_HEADING_COUNT);
        break;
    };
    memcpy(cwd, cw, sizeof(int)*JLPT_HEADING_COUNT);

    bool cv[JLPT_HEADING_COUNT] = { true, true, true, true, true, true }; //visible
    bool cr[JLPT_HEADING_COUNT] = { false, false, true, true, true, false }; //resizable
    bool ch[JLPT_HEADING_COUNT] = { true, true, true, true, true, false }; //hover button
    TWordColumnType ct[JLPT_HEADING_COUNT] = { wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom };
    TAlignment ca[JLPT_HEADING_COUNT] = { taCenter, taCenter, taLeftJustify, taLeftJustify, taCenter, taLeftJustify };

    wobj = new TVirtualWordListObj(pb, rowsizes(settings.dictfontsize), &wcallback, (TPopupWordProc)NULL);
    wobj->set_cols(dictionary->Items[0], JLPT_HEADING_COUNT, cw, cwd, cv, ct, ca, items->Count, windex);
    wobj->headerbutton = true;
    wobj->ordered = true;
    //wobj->multiselect = false;
    wobj->getheaderproc = &getheaderproc;
    //wobj->meaningproc = (TWordListMeaningProc)NULL;
    //wobj->headerselectproc = (TWordListColumnChangeProc)NULL;
    wobj->headerpressproc = &headerpressproc;
    wobj->prepareproc = &prepareproc;
    wobj->drawproc = &drawproc;
    wobj->wordselect = false;
    wobj->showhint = false;
    wobj->showgroup = false;
    wobj->multirightclick = false;

    wobj->SetCheckboxHandler(chkcallback);
    wobj->checkcolumn[0] = true;
    wobj->checkcolumn[2] = true;
    wobj->checkcolumn[3] = true;

    for (int ix = 0; ix < JLPT_HEADING_COUNT; ++ix)
    {
        wobj->colresize[ix] = cr[ix];
        wobj->colhover[ix] = ch[ix];
    }

    for (int ix = 0; ix < dictionary->Items[0]->groups->Count; ++ix)
    {
        TMenuItem *mi = new TMenuItem(miGroups);
        mi->Name = L"miGroup" + UnicodeString(ix);
        mi->Caption = dictionary->Items[0]->groups->Items[ix]->name;
        miGroups->Add(mi);

        TMenuItem *miA = new TMenuItem(mi);
        TMenuItem *miB = new TMenuItem(mi);
        TMenuItem *miC = new TMenuItem(mi);
        TMenuItem *miD = new TMenuItem(mi);
        TMenuItem *miE = new TMenuItem(mi);
        TMenuItem *miF = new TMenuItem(mi);
        miA->Name = L"miGroupFilterA" + UnicodeString(ix);
        miB->Name = L"miGroupFilterB" + UnicodeString(ix);
        miC->Name = L"miCountN2Kanji" + UnicodeString(ix);
        miD->Name = L"miMarkN3" + UnicodeString(ix);
        miE->Name = L"miMarkN3Kana" + UnicodeString(ix);
        miF->Name = L"miAddN3Words" + UnicodeString(ix);
        miA->Caption = L"Filter for N2 kanji";
        miB->Caption = L"Filter for N2 kana only";
        miC->Caption = L"Count N2 kanji";
        miD->Caption = L"Mark as N3";
        miE->Caption = L"Mark as N3 only kana";
        miF->Caption = L"Add all words to N3";
        miA->OnClick = &miFilterAClick;
        miB->OnClick = &miFilterBClick;
        miC->OnClick = &miCountN2KanjiClick;
        miD->OnClick = &miMarkN3Click;
        miE->OnClick = &miMarkN3KanaClick;
        miF->OnClick = &miAddN3WordsClick;
        miA->Tag = ix;
        miB->Tag = ix;
        miC->Tag = ix;
        miD->Tag = ix;
        miE->Tag = ix;
        miF->Tag = ix;

        mi->Add(miA);
        mi->Add(miB);
        mi->Add(miC);
        mi->Add(miD);
        mi->Add(miE);
        mi->Add(miF);
    }

    reorder(0, false);
    updatenlabels();
    updatecaption();
}
//---------------------------------------------------------------------------
__fastcall TfrmJLPTEdit::~TfrmJLPTEdit()
{
    delete wobj;
    delete items;
    delete view;
    delete edkanji;
    clearalt();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::btnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
int jlpted_sort_column;
char jlpted_sort_order;
TTempJLPTItemList *jlpted_sort_items;

int /*__fastcall*/ jlpted_sort_kanaorderproc(int a, int b);
int /*__fastcall*/ jlpted_sort_Norderproc(int a, int b);
int /*__fastcall*/ jlpted_sort_areaorderproc(int a, int b);

int /*__fastcall*/ jlpted_sort_orderproc(int a, int b)
{
    return (a - b) * jlpted_sort_order;
}
int /*__fastcall*/ jlpted_sort_kanjiorderproc(int a, int b)
{
    if (SameWord(jlpted_sort_items->Words[a], jlpted_sort_items->Words[b]))
        return 0;

    int diff = wcscmp(jlpted_sort_items->Words[a]->kanji, jlpted_sort_items->Words[b]->kanji);
    if (diff)
        return diff * jlpted_sort_order;
    if (jlpted_sort_column == JLPT_COL_KANJI && jlpted_sort_column != JLPT_COL_KANA)
        return jlpted_sort_kanaorderproc(a, b);
    if (jlpted_sort_column == JLPT_COL_NLEVEL)
        return jlpted_sort_areaorderproc(a, b);
    return jlpted_sort_Norderproc(a, b);
}
int /*__fastcall*/ jlpted_sort_kanaorderproc(int a, int b)
{
    int diff = wcscmp(jlpted_sort_items->Words[a]->kana, jlpted_sort_items->Words[b]->kana);
    if (diff)
        return diff * jlpted_sort_order;
    if (jlpted_sort_column != JLPT_COL_KANJI)
        return jlpted_sort_kanjiorderproc(a, b);
    if (jlpted_sort_column == JLPT_COL_NLEVEL)
        return jlpted_sort_areaorderproc(a, b);
    return jlpted_sort_Norderproc(a, b);
}
int /*__fastcall*/ jlpted_sort_Norderproc(int a, int b)
{
    int diff = jlpted_sort_items->Items[b]->Nlevel - jlpted_sort_items->Items[a]->Nlevel;
    if (diff)
        return diff * jlpted_sort_order;

    if (jlpted_sort_items->Words[a] == jlpted_sort_items->Words[b])
        return 0;

    if (jlpted_sort_column == JLPT_COL_NLEVEL || jlpted_sort_column == JLPT_COL_AREA)
        return jlpted_sort_kanaorderproc(a, b);
    return jlpted_sort_areaorderproc(a, b);
}
int /*__fastcall*/ jlpted_sort_areaorderproc(int a, int b)
{
    if (jlpted_sort_items->Words[a] == jlpted_sort_items->Words[b])
        return 0;
    int diff = (int)jlpted_sort_items->Items[a]->area - (int)jlpted_sort_items->Items[b]->area;
    if (diff)
        return diff;

    if (jlpted_sort_items->Words[a] == jlpted_sort_items->Words[b])
        return 0;

    if (jlpted_sort_column == JLPT_COL_NLEVEL || jlpted_sort_column == JLPT_COL_AREA)
        return jlpted_sort_kanaorderproc(a, b);
    return jlpted_sort_Norderproc(a, b);
}

void TfrmJLPTEdit::reorder(int col, bool rev)
{
    jlpted_sort_column = col < 0 ? wobj->colselected : col;
    jlpted_sort_order = (col < 0 ? wobj->colreverseorder : rev) ? -1 : 1;
    jlpted_sort_items = items;
    switch (jlpted_sort_column)
    {
    case JLPT_COL_USE:
        view->Sort(jlpted_sort_orderproc);
        break;
    case JLPT_COL_KANJI:
        view->Sort(jlpted_sort_kanjiorderproc);
        break;
    case JLPT_COL_KANA:
        view->Sort(jlpted_sort_kanaorderproc);
        break;
    case JLPT_COL_NLEVEL:
        view->Sort(jlpted_sort_Norderproc);
        break;
    case JLPT_COL_AREA:
        view->Sort(jlpted_sort_areaorderproc);
        break;
    }

    freordering = true;
    try
    {
        for (int ix = 0; ix < view->Count; ++ix)
        {
            wobj->checked[ix][0] = items->Items[realrow(ix)]->use;
            wobj->checked[ix][2] = !items->Items[realrow(ix)]->kanaonly;
            wobj->checked[ix][3] = items->Items[realrow(ix)]->usekana;
            wobj->checkenabled[ix][2] = wobj->checked[ix][0] && items->Items[realrow(ix)]->haskanji;
            wobj->checkenabled[ix][3] = wobj->checked[ix][0];
        }
    }
    catch (...)
    {
        ;
    }
    freordering = false;
    wobj->SelectOrderColumn(jlpted_sort_column, wobj->colreverseorder);
}
//---------------------------------------------------------------------------
int TfrmJLPTEdit::windex(unsigned int ix)
{
    return items->Items[ix]->windex;
}
//---------------------------------------------------------------------------
int TfrmJLPTEdit::realrow(int row)
{
    if (!view || !view->Count)
        return row;
    return view->Items[row];
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::wcallback()
{
    static int lastpos = -1;

    sbInfo->Panels->Items[0]->Text = view->Count;
    sbInfo->Panels->Items[1]->Text = view->Count ? UnicodeString(":  ") + (wobj->pos + 1) : UnicodeString("0");

    sbInfo->Panels->Items[2]->Text = view->Count ? items->CollectedMeanings[realrow(wobj->pos)] : UnicodeString(L"-");

    lbMeaning->Enabled = wobj->pos >= 0;
    edMeaning->Enabled = wobj->pos >= 0;
    cbN->Enabled = wobj->pos >= 0;

    if (wobj->pos < 0)
    {
        lastpos = -1;
        edMeaning->Text = "";
    }
    else if (lastpos != realrow(wobj->pos))
    {
        edMeaning->Text = items->Meanings[realrow(wobj->pos)];
        lastpos = realrow(wobj->pos);
        cbN->ItemIndex = 5 - items->Items[realrow(wobj->pos)]->Nlevel;
    }

    UnicodeString m = edMeaning->Text.Trim();

    btnSave->Enabled = lastpos >= 0 && (5 - cbN->ItemIndex != items->Items[lastpos]->Nlevel || m.Length() && (!items->OwnMeaning[lastpos] || m != items->Meanings[lastpos]));

    btnOrig->Enabled = lastpos >= 0 && m != items->Items[lastpos]->meaningorig;
    btnDict->Enabled = lastpos >= 0 && m != items->DictionaryMeanings[lastpos];

    btnL->Enabled = lastpos >= 0;
    miL->Enabled = btnL->Enabled;
    btnK->Enabled = lastpos >= 0;
    miK->Enabled = btnK->Enabled;
    btnF->Enabled = lastpos >= 0;
    miF->Enabled = btnF->Enabled;

    if (lastpos >= 0)
    {
        btnL->Down = items->Items[lastpos]->mark == jmLevel;
        miL->Checked = btnL->Down;
        btnK->Down = items->Items[lastpos]->mark == jmKanji;
        miK->Checked = btnK->Down;
        btnF->Down = items->Items[lastpos]->mark == jmFixed;
        miF->Checked = btnF->Down;

        TWordIndexList *l = NULL;
        try
        {
            l = new TWordIndexList(dictionary->Items[btnDefinition->Tag]);
            FindWord(Romanize(items->Words[lastpos]->kana).c_str(), l, true, true, false, false, NULL);
            for (int ix = l->Count - 1; ix >= 0; --ix)
            {
                if (wcscmp(l->Words[ix]->kana, items->Words[lastpos]->kana) || wcscmp(l->Words[ix]->kanji, items->Words[lastpos]->kanji))
                    l->Delete(ix);
            }

            if (l->Count == 1)
            {
                UnicodeString s;
                for (int ix = 0; ix < l->Words[0]->meaningcnt; ++ix)
                {
                    if (ix)
                        s += "; ";
                    s += l->Words[0]->data[ix].meaning;
                }
                lbDefinition->Caption = s;
            }
            else
                lbDefinition->Caption = "***Not Found***";
        }
        catch (...)
        {
            ;
        }
        delete l;

    }
    else
        lbDefinition->Caption = "-";

    btnDelMeaning->Enabled = lastpos >= 0 && items->OwnMeaning[lastpos];

    sbPrev->Enabled = view->Count && items->Items[lastpos]->next != items->Items[lastpos];
    sbNext->Enabled = sbPrev->Enabled;

    TLabel *lb[] = { lb1, lb2, lb3, lb4, lb5 };
    for (int ix = 0; ix < 5; ++ix)
    {
        lb[ix]->Transparent = wobj->pos < 0 || !items->NPresent[lastpos][ix + 1];
        lb[ix]->ParentColor = wobj->pos < 0 || !items->NPresent[lastpos][ix + 1];

        if (wobj->pos >= 0 && items->NPresent[lastpos][ix + 1])
        {
            lb[ix]->Color = items->NUsed[lastpos][ix + 1] ? (TColor)0x6688ff : (TColor)0xc8c0d0;
        }

    }

    updateklabels();

    clearalt();
    if (wobj->pos >= 0)
    {
        TWordIndexList *l = NULL;
        try
        {
            l = new TWordIndexList(dictionary->Items[0]);
            TWord *w = items->Words[lastpos];
            FindWord(Romanize(w->kana).c_str(), l, true, true, false, false, NULL);

            FindKanjiWord(l, w->kanji, false, NULL);
            l->IndexSort();

            for (int ix = l->Count - 1; ix >= 0; --ix)
                if (l->Indexes[ix] == items->Items[lastpos]->windex || ix && l->Indexes[ix] == l->Indexes[ix - 1])
                    l->Delete(ix);

            for (int ix = 0; ix < l->Count; ++ix)
            {
                TMenuItem *mi = new TMenuItem(pmAlt);
                mi->Tag = -1;
                for (int iy = 0; iy < items->Count && mi->Tag < 0; ++iy)
                    if (items->Items[iy]->windex == l->Indexes[ix])
                        mi->Tag = iy;

                UnicodeString s;
                for (int iy = 0; iy < l->Words[ix]->meaningcnt; ++iy)
                {
                    if (iy)
                        s += ", ";
                    s += l->Words[ix]->data[iy].meaning;
                }
                if (s.Length() > 45)
                {
                    s.SetLength(42);
                    s += "...";
                }

                mi->Caption = UnicodeString(l->Words[ix]->kanji) + L"(" + l->Words[ix]->kana + L") " + s;
                if (mi->Tag >= 0)
                    mi->ImageIndex = 1;
                else
                {
                    mi->Tag = -l->Indexes[ix];
                    mi->Caption = UnicodeString(l->Words[ix]->frequency) + " - " + mi->Caption;
                }

                mi->OnClick = &miAltClick;

                pmAlt->Items->Add(mi);
            }
        }
        catch (...)
        {
            ;
        }
        delete l;
    }

    btnAlt->Enabled = pmAlt->Items->Count;
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::clearalt()
{
    for (int ix = pmAlt->Items->Count - 1; ix >= 0; --ix)
    {
        TMenuItem *mi = pmAlt->Items->Items[ix];
        pmAlt->Items->Delete(ix);
        delete mi;
    }
}
//---------------------------------------------------------------------------
UnicodeString TfrmJLPTEdit::getheaderproc(int col)
{
    switch (col)
    {
    case JLPT_COL_USE: //use
                return "#";
    case JLPT_COL_NLEVEL: //N level
                return "N";
    case JLPT_COL_KANJI: //written
                return "Written";
    case JLPT_COL_KANA: //kana
                return "Kana";
    case JLPT_COL_AREA: //area
                return "Area";
    case JLPT_COL_MEANING: //meaning
                return "Meaning";
    default:
        return "undef";
    }
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::_reorder(int oldpos)
{
    int oldvpos = pb->VPos;
    reorder();
    int p = 0;
    for (int ix = 0; ix < view->Count; ++ix)
    {
        if (view->Items[ix] == oldpos)
        {
            p = ix;
            break;
        }
    }

    int rows = (pb->ClientHeight - wobj->headersize) / wobj->rowsize;
    if ((p > oldvpos || p == oldvpos && !oldvpos) && (p < oldvpos + rows - 1 || p == oldvpos + rows - 1 && oldvpos + rows == wobj->count))
    {
        if (oldvpos != pb->VPos)
            pb->VPos = oldvpos;
        else
            pb->Refresh();
        wobj->pos = p;
        return;
    }
    wobj->pos = p;

    int wvpos = min(p, max(0, p - rows / 2 + 1));
    if (wvpos != pb->VPos && wvpos <= wobj->count - rows)
        pb->VPos = wvpos;
    else
        pb->Refresh();
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::headerpressproc(int oldcol)
{
    if (wobj->count)
    {
        _reorder(realrow(wobj->pos));
    }
    else
    {
        if (oldcol >= 0)
            wobj->invalidatecol(oldcol);
        if (wobj->colselected >= 0)
            wobj->invalidatecol(wobj->colselected);
    }
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::prepareproc(int col, int row, TWordColumnType ctype, TCanvas *c)
{
    row = realrow(row);

    TColor color;
    if (col == JLPT_COL_NLEVEL)
    {
        if (items->Items[row]->Nlevel == 2 && items->KanjiLevel[row] > 2)
        {
            c->Font->Color = (TColor)0x559922;
            c->Font->Style = c->Font->Style << fsUnderline;
        }

        /*switch (items->KanjiLevel[row]) {
         case 1:
         color = (TColor)0xffbb66;
         break;
         case 2:
         color = (TColor)0xaaff88;
         break;
         case 3:
         color = (TColor)0x88aaff;
         break;
         case 4:
         color = (TColor)0x66eeee;
         break;
         case 5:
         color = (TColor)0x998866;
         break;
         default:
         color = c->Brush->Color;
         }
         c->Brush->Color = coloravg(c->Brush->Color,color,0.9);
         */
    }

    if (items->Items[row]->mark != jmNone)
    {
        color = (TColor)(items->Items[row]->mark == jmLevel ? 0xffbb66 : items->Items[row]->mark == jmKanji ? 0x99ff66 : 0x0088bb);
        c->Brush->Color = coloravg(c->Brush->Color, color, 0.9);
    }

    color = wobj->headerbuttoncolor;
    if (wobj->colselected == col)
    {
        c->Brush->Color = coloravg(c->Brush->Color, color, 0.9);
        c->Pen->Color = coloravg(c->Pen->Color, color, 0.9);
    }

    if (col == JLPT_COL_KANJI || col == JLPT_COL_KANA)
    { //written / kana
        c->Font->Name = settings.kanafont;
        c->Font->Charset = settings.kanacharset;
    }
    else
        c->Font->Name = settings.romajifont;
    if (!items->Items[row]->use)
        c->Font->Color = coloravg(c->Brush->Color, c->Font->Color, 0.7);
    else if (col == JLPT_COL_KANJI && items->Items[row]->kanaonly)
        c->Font->Color = coloravg((TColor)colorsettings.background, (TColor)colorsettings.maintext, 0.7);
    else if (col == JLPT_COL_MEANING && !items->ValidItem[row])
    {
        TColor cl = (TColor)0x2000df;
        fixtextcolor(cl, c->Brush->Color);
        c->Font->Color = cl;
    }
    else if (col == JLPT_COL_MEANING && !items->OwnMeaning[row])
    {
        TColor cl = (TColor)0xffa020;
        fixtextcolor(cl, c->Brush->Color);
        c->Font->Color = cl;
    }

    if (col == JLPT_COL_NLEVEL && items->NLevelCount[row] > 2)
    {
        TColor cl = coloravg(c->Brush->Color, (TColor)0x4030ff, 0.7);
        c->Brush->Color = cl;
        cl = c->Font->Color;
        fixtextcolor(cl, c->Brush->Color);
        c->Font->Color = cl;
    }

    if (col == JLPT_COL_USE || col == JLPT_COL_NLEVEL)
        c->Font->Size = c->Font->Size * 0.9;
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::drawproc(int col, int row, TCanvas *c, const TRect &r)
{
    UnicodeString str;
    switch (col)
    {
    case JLPT_COL_USE: //use
                str = realrow(row);
        break;
    case JLPT_COL_KANJI: //written
                str = items->Words[view->Items[row]]->kanji;
        break;
    case JLPT_COL_KANA: //kana
                str = items->Words[view->Items[row]]->kana;
        break;
    case JLPT_COL_NLEVEL: //N level
                str = items->Items[view->Items[row]]->Nlevel;
        if (items->Items[view->Items[row]]->Nlevel != items->Items[view->Items[row]]->Nlevel_orig)
            str = "*" + str;
        break;
    case JLPT_COL_AREA: //area
                str = items->Areas[view->Items[row]];
        break;
    case JLPT_COL_MEANING: //meaning
                str = items->Meanings[view->Items[row]];
        break;
    default:
        str = "undef";
    }

    wobj->DrawCellText(r, str, wobj->colalignment[col]);
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::chkcallback(unsigned int row, byte index, bool &checked)
{
    if (freordering)
        return;

    switch (index)
    {
    case 0:
        if (items->Items[realrow(row)]->use != wobj->checked[row][0])
        {
            if (!wobj->checked[row][0])
                NCount[items->Items[realrow(row)]->Nlevel - 1]--;
            else
                NCount[items->Items[realrow(row)]->Nlevel - 1]++;
            items->Items[realrow(row)]->use = wobj->checked[row][0];
            updatenlabels();
            wcallback();
        }
        wobj->checkenabled[row][2] = wobj->checked[row][0] && items->Items[realrow(row)]->haskanji;
        wobj->checkenabled[row][3] = wobj->checked[row][0];
        break;
    case 2:
        items->Items[realrow(row)]->kanaonly = !wobj->checked[row][2];
        break;
    case 3:
        items->Items[realrow(row)]->usekana = wobj->checked[row][3];
        break;
    }
    invalidate_lines(items->Items[realrow(row)]->windex);
    modified = true;
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::invalidate_lines(unsigned int windex)
{
    for (int ix = 0; ix < view->Count; ++ix)
        if (items->Items[realrow(ix)]->windex == (int)windex)
            wobj->invalidaterow(ix);
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::save(UnicodeString file)
{
    try
    {
        items->SaveToFile(file);
        filename = file;
        fmodified = false;

        updatecaption();
    }
    catch (...)
    {
        MsgBox(L"Error saving file!", L"Error", MB_OK);
        return;
    }
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::open(UnicodeString file)
{
    try
    {
        items->LoadFromFile(file);
        filename = file;
        fmodified = false;

        count_ncount();
        memset(NCount, 0, sizeof(NCount));
        for (int ix = 0; ix < items->Count; ++ix)
        {
            if (items->Items[ix]->use)
                NCount[items->Items[ix]->Nlevel - 1]++;
        }

        updatecaption();
        updatenlabels();
    }
    catch (...)
    {
        items->Preload();
        MsgBox(L"Error opening file!", L"Error", MB_OK);
    }
    wobj->count = 0;
    Filter();
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::updatecaption()
{
    UnicodeString cap = UnicodeString(L"JLPT vocabulary list editor. Words: ") + items->WordCount + L", Defined: " + items->DefinitionCount + L" (" +int(items->DefinitionCount * 100 / items->WordCount) + L"%)";

    if (filename.Length())
        cap = ExtractFileName(filename) + L" - " + cap;
    if (fmodified)
        cap = L"*" + cap;

    Caption = cap;
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::updatenlabels()
{
    TLabel *lb[5] = { lbN1, lbN2, lbN3, lbN4, lbN5 };
    int cnt = 0;
    for (int ix = 4; ix >= 0; --ix)
    {
        if (cbSum->Checked)
            cnt += NCount[ix];
        else
            cnt = NCount[ix];
        lb[ix]->Caption = cnt;
    }
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::set_mod(bool val)
{
    if (fmodified == val)
        return;
    fmodified = val;
    updatecaption();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::FormPaint(TObject *Sender)
{
    DrawCloseBar(this, b1, NULL);
    DrawPillowPanel(this, 0, bt, false, false);
    DrawPillowPanel(Canvas, ChildRect(pButtons));
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::FormResize(TObject *Sender)
{
    FormPaint(Sender);
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::ecallback()
{
    if (edkanji->CanInput())
        Filter();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::sbN5Click(TObject *Sender)
{
    Filter();
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::Filter()
{
    TSpeedButton *nbuttons[5] = { sbN1, sbN2, sbN3, sbN4, sbN5 };
    TSpeedButton *abuttons[3] = { sbAreaFixed, sbAreaFound, sbAreaMatch };

    int pos = wobj->pos >= 0 ? realrow(wobj->pos) : 0;

    static unsigned int maxlen = 0;
    try
    {
        int tmp = edKLen->Text.Length() ? edKLen->Text.ToInt() : 0;
        maxlen = tmp;
    }
    catch (...)
    {
        ;
    }

    view->Clear();
    for (int ix = 0; ix < items->Count; ++ix)
    {
        if (maxlen > 0 && wcslen(items->Words[ix]->kanji) > maxlen)
            continue;

        if (!nbuttons[items->Items[ix]->Nlevel - 1]->Down)
            continue;
        if (!abuttons[(int)items->Items[ix]->area]->Down)
            continue;

        bool haskanji = false;
        for (int ix = edkanji->Text.Length(); ix >= 1 && !haskanji; --ix)
            haskanji = KANJI(edkanji->Text[ix]);

        if (haskanji)
        {
            if (wcsncmp(edkanji->CleanText.c_str(), items->Words[ix]->kanji, edkanji->CleanText.Length()))
                continue;
        }
        else
        {

            if (wcsncmp(RomanizeEx(edkanji->CleanText.c_str()).c_str(), RomanizeEx(items->Words[ix]->kana).c_str(), RomanizeEx(edkanji->CleanText.c_str()).Length()))
                continue;
        }

        view->Add(ix);
    }

    if (view->Count != wobj->count)
        wobj->count = view->Count;
    else
        pb->Invalidate();

    _reorder(pos);

    /*int newpos = 0;
     for (int ix = 0; ix < view->Count; ++ix) {
     if (view->Items[ix] == pos) {
     newpos = ix;
     break;
     }
     }
     wobj->pos = newpos;
     */
    wcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::SpeedButton1Click(TObject *Sender)
{
    TSpeedButton *btns[8] = { sbN1, sbN2, sbN3, sbN4, sbN5, sbAreaFixed, sbAreaFound, sbAreaMatch };
    bool changed = false;
    for (int ix = 0; (unsigned)ix < sizeof(btns) / sizeof(TSpeedButton*); ++ix)
    {
        if (!btns[ix]->Down)
        {
            changed = true;
            btns[ix]->Down = true;
        }
    }

    edKLen->Text = "";
    if (edkanji->Text.Length())
        edkanji->Text = "";
    else if (changed)
        Filter();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::btnSaveClick(TObject *Sender)
{
    int rix = realrow(wobj->pos);
    items->Meanings[rix] = edMeaning->Text.Trim();

    /*bool nchange = false;
     if (items->Items[rix]->Nlevel != 5-cbN->ItemIndex) {
     if (items->Items[rix]->use) {
     NCount[items->Items[rix]->Nlevel-1]--;
     NCount[4-cbN->ItemIndex]++;
     }
     items->Items[rix]->Nlevel = 5-cbN->ItemIndex;
     nchange = true;
     }
     */
    modified = true;

    /*if (nchange && wobj->colselected != JLPT_COL_USE)
     _reorder(rix);
     else*/
    invalidate_lines(items->Items[rix]->windex);

    /*if (nchange)
     updatenlabels();
     */
    updatecaption();
    wcallback();

    pb->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::btnOrigClick(TObject *Sender)
{
    edMeaning->Text = items->Items[realrow(wobj->pos)]->meaningorig;
    wcallback();
    focusmeaningbox();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::btnDictClick(TObject *Sender)
{
    edMeaning->Text = items->DictionaryMeanings[realrow(wobj->pos)];
    wcallback();
    focusmeaningbox();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::btnDelMeaningClick(TObject *Sender)
{
    items->Meanings[realrow(wobj->pos)] = "";
    invalidate_lines(items->Items[realrow(wobj->pos)]->windex);

    modified = true;

    updatecaption();
    wcallback();
    edMeaning->Text = items->Meanings[realrow(wobj->pos)];
    focusmeaningbox();
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::focusmeaningbox()
{
    edMeaning->SetFocus();
    edMeaning->SelStart = 0;
    edMeaning->SelLength = edMeaning->Text.Length();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::edMeaningChange(TObject *Sender)
{
    wcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::edMeaningKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == VK_RETURN && btnSave->Enabled)
        btnSave->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::cbNChange(TObject *Sender)
{
    int rix = realrow(wobj->pos);

    TTempJLPTItem *i = items->Items[rix];
    if (i->Nlevel == 5 - cbN->ItemIndex)
        return;

    int ncnt = i->use;
    TTempJLPTItem *c = i->next;
    while (c != i)
    {
        if (c->use)
            ncnt++;
        c = c->next;
    }

    int r = mrNo;
    if (ncnt == 1 && i->use && i->haskanji)
        r = MsgBox("Would you like to add a new item for this word?", "Query", MB_YESNOCANCEL);

    if (r == IDCANCEL)
        return;

    if (r == IDYES)
    {
        TTempJLPTItem *i2 = new TTempJLPTItem;
        memset(i2, 0, sizeof(TTempJLPTItem));
        i2->use = true;
        i2->Nlevel = 5 - cbN->ItemIndex;
        i2->definition = i->definition;
        i2->meaningorig = new wchar_t[wcslen(i->meaningorig) + 1];
        wcscpy(i2->meaningorig, i->meaningorig);
        i2->haskanji = i->haskanji;
        i2->windex = i->windex;
        i2->area = i->area;
        i2->mark = jmNone;
        i2->Nlevel_orig = i2->Nlevel;
        i2->next = i->next;
        i2->usekana = true;
        i->next = i2;

        i2->kanaonly = i2->Nlevel > i->Nlevel;
        i->kanaonly = !i2->kanaonly;

        items->Add(i2);

        Filter();

        NCount[i2->Nlevel - 1]++;
        modified = true;
        if (wobj->colselected != JLPT_COL_USE)
            _reorder(rix);
        else
            invalidate_lines(i->windex);

        updatenlabels();

        wcallback();
        return;
    }

    if (i->use)
    {
        NCount[i->Nlevel - 1]--;
        NCount[4 - cbN->ItemIndex]++;
    }
    i->Nlevel = 5 - cbN->ItemIndex;

    modified = true;

    if (wobj->colselected != JLPT_COL_USE)
        _reorder(rix);
    else
        invalidate_lines(i->windex);

    updatenlabels();

    wcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::sbNextMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    MouseRepeat(&sbNextMouseDown, Sender, Button, Shift, X, Y);
    TSpeedButton *sb = (TSpeedButton*)Sender;

    TTempJLPTItem *item = items->Items[realrow(wobj->pos)];
    if (sb->Tag > 0)
        item = item->next;
    else
    {
        TTempJLPTItem *tmp = item;
        while (item->next != tmp)
            item = item->next;
    }

    for (int ix = 0; ix < view->Count; ++ix)
    {
        if (items->Items[view->Items[ix]] == item)
        {
            wobj->pos = ix;
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::sbLinkClick(TObject *Sender)
{
    wcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miSaveClick(TObject *Sender)
{
    bool unsaved = !filename.Length() || !DirectoryExists(ExtractFilePath(filename), true);
    if (unsaved)
    {
        miSaveAs->Click();
        return;
    }

    save(filename);
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miSaveAsClick(TObject *Sender)
{
    if (!filename.Length() || filename.Length() && !DirectoryExists(ExtractFilePath(filename), true))
        dlgSave->InitialDir = paths.installdatapath;
    else
        dlgSave->InitialDir = ExtractFilePath(filename);

    if (ExecuteDialog(dlgSave))
        save(dlgSave->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miOpenClick(TObject *Sender)
{
    if (fmodified)
    {
        int r = MsgBox("The list was modified! Would you like to save the changes?", "Message", MB_YESNOCANCEL);
        if (r == IDCANCEL)
            return;

        if (r == IDYES)
        {
            miSave->Click();
            if (fmodified == true)
                return;
        }
    }

    if (!filename.Length() || filename.Length() && !DirectoryExists(ExtractFilePath(filename), true))
        dlgOpen->InitialDir = paths.installdatapath;
    else
        dlgOpen->InitialDir = ExtractFilePath(filename);

    if (ExecuteDialog(dlgOpen))
        open(dlgOpen->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if (fmodified)
    {
        int r = MsgBox("The list was modified! Would you like to save the changes?", "Message", MB_YESNOCANCEL);
        if (r == IDCANCEL)
        {
            CanClose = false;
            return;
        }

        if (r == IDYES)
        {
            miSave->Click();
            CanClose = fmodified == false;
        }
    }

    TMemIniFile *ini = new TMemIniFile(paths.inipath + L"jlptedit.ini");
    try
    {
        ini->WriteString(L"File", "Last open", filename);

        ini->WriteInteger(L"Window", "Width", SizeInfo.width);
        ini->WriteInteger(L"Window", "Height", SizeInfo.height);

        ini->UpdateFile();
    }
    catch (...)
    {
        ;
    }
    delete ini;

}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::edKanjiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_UP || Key == VK_DOWN)
    {
        if (wobj->count)
        {
            pb->SetFocus();
            pb->OnKeyDown(Sender, Key, Shift);
        }
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::btnAltClick(TObject *Sender)
{
    TRect r = ChildRect(btnAlt);
    TPoint p = ClientToScreen(TPoint(r.Left, r.Bottom + 2));
    pmAlt->Popup(p.x, p.y);
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miAltClick(TObject *Sender)
{
    /*Menu item for alternative word pressed. When item's tag is negative,
     * it's the windex of a word not found in the list yet. Positive numbers are
     * the order index of the first found word of the same index already in the
     * list.
     */

    TMenuItem *mi = (TMenuItem*)Sender;
    int rix = realrow(wobj->pos);
    int oldindex = items->Items[rix]->windex;

    if (mi->Tag < 0)
        items->ChangeIndex(rix, -mi->Tag);
    else
        items->AddToBunch(rix, mi->Tag);

    invalidate_lines(oldindex);
    invalidate_lines(items->Items[rix]->windex);

    modified = true;
    Filter();
    wcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::cbSumClick(TObject *Sender)
{
    updatenlabels();
}
//---------------------------------------------------------------------------
struct TTempJLPTKanjiCollection
{
    TIntegerList *indexes;
    TKanji *k;
    int JLPTwordfreq;
    int examplefreq;
    int wordfreq;
    int kanjifreq;
    int wordcntfreq;

    int frequency;
};

int tmpJLPT_sort_wfreq(TTempJLPTKanjiCollection **a, TTempJLPTKanjiCollection **b);
int tmpJLPT_sort_kfreq(TTempJLPTKanjiCollection **a, TTempJLPTKanjiCollection **b);

int tmpJLPT_sort_jfreq(TTempJLPTKanjiCollection **a, TTempJLPTKanjiCollection **b)
{
    int v = (*b)->JLPTwordfreq - (*a)->JLPTwordfreq;
    if (!v)
        return tmpJLPT_sort_wfreq(a, b);
    return v;
}
int tmpJLPT_sort_excnt(TTempJLPTKanjiCollection **a, TTempJLPTKanjiCollection **b)
{
    int v = (*b)->examplefreq - (*a)->examplefreq;
    if (!v)
        return tmpJLPT_sort_wfreq(a, b);
    return v;
}
int tmpJLPT_sort_wcnt(TTempJLPTKanjiCollection  **a, TTempJLPTKanjiCollection **b)
{
    int v = dictionary->Items[0]->kanjidat[(*b)->k->index].wordscnt - dictionary->Items[0]->kanjidat[(*a)->k->index].wordscnt;
    if (!v)
        return tmpJLPT_sort_wfreq(a, b);
    return v;
}
int tmpJLPT_sort_wfreq(TTempJLPTKanjiCollection **a, TTempJLPTKanjiCollection **b)
{
    int v = (*b)->k->word_freq - (*a)->k->word_freq;
    if (!v)
        return tmpJLPT_sort_kfreq(a, b);
    return v;
}
int tmpJLPT_sort_kfreq(TTempJLPTKanjiCollection **a, TTempJLPTKanjiCollection **b)
{
    int v = (*a)->k->frequency - (*b)->k->frequency;
    if (!v)
        return (*a)->k->index - (*b)->k->index;
    return v;
}
int tmpJLPT_sort_final(TTempJLPTKanjiCollection **a, TTempJLPTKanjiCollection **b)
{
    int av = (*a)->JLPTwordfreq * frmJLPTEdit->edKwrd->Text.ToInt() + (*a)->examplefreq * frmJLPTEdit->edKex->Text.ToInt() + (*a)->wordfreq * frmJLPTEdit->edKKwf->Text.ToInt() + (*a)->kanjifreq * frmJLPTEdit->edKk->Text.ToInt() + (*a)->wordcntfreq * frmJLPTEdit->edKwcnt->Text.ToInt();
    int bv = (*b)->JLPTwordfreq * frmJLPTEdit->edKwrd->Text.ToInt() + (*b)->examplefreq * frmJLPTEdit->edKex->Text.ToInt() + (*b)->wordfreq * frmJLPTEdit->edKKwf->Text.ToInt() + (*b)->kanjifreq * frmJLPTEdit->edKk->Text.ToInt() + (*b)->wordcntfreq * frmJLPTEdit->edKwcnt->Text.ToInt();
    int v = av - bv;

    if (!v)
        return (*a)->k->index - (*b)->k->index;
    return v;
}

struct TTempJLPTWordCollection
{
    int index;

    int kanjifreq;
    int kanjiJLPT;

    int wordfreq;
    int examplefreq;
};

int tmpJLPT_sortw_wfreq(TTempJLPTWordCollection **a, TTempJLPTWordCollection **b);

int tmpJLPT_sortw_kfreq(TTempJLPTWordCollection **a, TTempJLPTWordCollection **b)
{
    int v = (*a)->kanjifreq - (*b)->kanjifreq;
    if (!v)
        return tmpJLPT_sortw_wfreq(a, b);
    return v;
}
int tmpJLPT_sortw_kjlpt(TTempJLPTWordCollection **a, TTempJLPTWordCollection **b)
{
    int v = (*b)->kanjiJLPT - (*a)->kanjiJLPT;
    if (!v)
        return tmpJLPT_sortw_wfreq(a, b);
    return v;
}
int tmpJLPT_sortw_efreq(TTempJLPTWordCollection **a, TTempJLPTWordCollection **b)
{
    int v = (*b)->examplefreq - (*a)->examplefreq;
    if (!v)
        return tmpJLPT_sortw_wfreq(a, b);
    return v;
}
int tmpJLPT_sortw_wfreq(TTempJLPTWordCollection **a, TTempJLPTWordCollection **b)
{
    int v = (*b)->wordfreq - (*a)->wordfreq;
    if (!v)
        return (*a)->index - (*b)->index;
    return v;
}
int tmpJLPT_sortw_final(TTempJLPTWordCollection **a, TTempJLPTWordCollection **b)
{
    int av = (*a)->kanjiJLPT * frmJLPTEdit->edNkj->Text.ToInt() + ((*a)->kanjifreq < 0 || (*b)->kanjifreq < 0 ? 0 : (*a)->kanjifreq * frmJLPTEdit->edNkf->Text.ToInt()) + ((*a)->kanjifreq < 0 || (*b)->kanjifreq < 0 ? 8 : 1) * (*a)->wordfreq * frmJLPTEdit->edNwf->Text.ToInt() + (*a)->examplefreq * frmJLPTEdit->edNef->Text.ToInt();
    int bv = (*b)->kanjiJLPT * frmJLPTEdit->edNkj->Text.ToInt() + ((*a)->kanjifreq < 0 || (*b)->kanjifreq < 0 ? 0 : (*b)->kanjifreq * frmJLPTEdit->edNkf->Text.ToInt()) + ((*a)->kanjifreq < 0 || (*b)->kanjifreq < 0 ? 8 : 1) * (*b)->wordfreq * frmJLPTEdit->edNwf->Text.ToInt() + (*b)->examplefreq * frmJLPTEdit->edNef->Text.ToInt();
    int v = av - bv;

    if (!v)
        return (*a)->index - (*b)->index;
    return v;
}

void __fastcall TfrmJLPTEdit::btnAutoClick(TObject *Sender)
{
    TGapList<TTempJLPTKanjiCollection *> kl;
    TGapList<TTempJLPTWordCollection *> wl;
    TTemplateList<TTempJLPTKanjiCollection, false> *tl = new TTemplateList<TTempJLPTKanjiCollection, false>;
    TTemplateList<TTempJLPTWordCollection, false> *twl = new TTemplateList<TTempJLPTWordCollection, false>;
    for (int ix = 0; ix < items->Count; ++ix)
    {
        TTempJLPTItem *i = items->Items[ix];
        if (i->Nlevel != 3 && i->Nlevel != 2 || !i->use || i->kanaonly && cbExclude->Checked)
            continue;

        TWord *w = items->Words[ix];

        for (int j = wcslen(w->kanji) - 1; j >= 0; --j)
        {
            if (!KANJI(w->kanji[j]))
                continue;

            if (!kl.ItemSet((int)w->kanji[j]))
            {
                TTempJLPTKanjiCollection *ti = new TTempJLPTKanjiCollection;
                memset(ti, 0, sizeof(TTempJLPTKanjiCollection));
                tl->Add(ti);
                ti->indexes = new TIntegerList;
                ti->k = kanjis->Kanjis[kanjis->IndexOf(w->kanji[j])];
                kl.Items[(int)w->kanji[j]] = ti;
            }
            TIntegerList *il = kl.Items[(int)w->kanji[j]]->indexes;
            bool found = false;
            for (int k = 0; k < il->Count && !found; ++k)
                found = il->Items[k] == ix;
            if (!found)
            {
                if (!wl.ItemSet(ix))
                {
                    TTempJLPTWordCollection *wti = new TTempJLPTWordCollection;
                    memset(wti, 0, sizeof(TTempJLPTWordCollection));
                    twl->Add(wti);
                    wl.Items[ix] = wti;
                    wti->index = ix;
                }

                il->Add(ix);
                kl.Items[(int)w->kanji[j]]->JLPTwordfreq += w->frequency;

                //TWordExampleData *ex = Examples->FindData(items->Words[ix]);
                TStoredWord *sw = store->FindWord(items->Words[ix]);
                if (sw)
                    kl.Items[(int)w->kanji[j]]->examplefreq += sw->examplecnt;
            }
        }
    }

    tl->Sort(tmpJLPT_sort_jfreq);
    for (int ix = 0; ix < tl->Count; ++ix)
        tl->Items[ix]->JLPTwordfreq = ix;

    tl->Sort(tmpJLPT_sort_excnt);
    for (int ix = 0; ix < tl->Count; ++ix)
        tl->Items[ix]->examplefreq = ix;

    tl->Sort(tmpJLPT_sort_wcnt);
    for (int ix = 0; ix < tl->Count; ++ix)
        tl->Items[ix]->wordcntfreq = ix;

    tl->Sort(tmpJLPT_sort_wfreq);
    for (int ix = 0; ix < tl->Count; ++ix)
        tl->Items[ix]->wordfreq = ix;

    tl->Sort(tmpJLPT_sort_kfreq);
    for (int ix = 0; ix < tl->Count; ++ix)
        tl->Items[ix]->kanjifreq = ix;

    tl->Sort(tmpJLPT_sort_final);

    int f = 0;
    for (int ix = tl->Count - 1; ix >= 0; --ix)
    {
        if (tl->Items[ix]->k->JLPT == 1)
            tl->Items[ix]->frequency = 0;
        else
            tl->Items[ix]->frequency = ++f;
    }

    for (unsigned int ix = 0; ix < wl.TrueCount; ++ix)
    {
        TWord *w = items->Words[wl.TruePos[ix]];
        TTempJLPTWordCollection *wi = wl.TrueItems[ix];
        wi->wordfreq = w->frequency;

        //TWordExampleData *ex = Examples->FindData(items->Words[wl.TruePos[ix]]);
        TStoredWord *sw = store->FindWord(items->Words[wl.TruePos[ix]]);
        if (sw)
            wi->examplefreq = sw->examplecnt;

        int kanjicnt = 0;

        if ((w->data[0].notes | (1 << 10)) != 0 || !items->Items[wl.TruePos[ix]]->kanaonly)
        {
            for (int j = wcslen(w->kanji) - 1; j >= 0; --j)
            {
                if (KANJI(w->kanji[j]))
                {
                    kanjicnt++;
                    wi->kanjifreq += (!kl.Items[w->kanji[j]]->frequency ? tl->Count + 200 : kl.Items[w->kanji[j]]->frequency)*20;
                    wi->kanjiJLPT += (!kl.Items[w->kanji[j]]->k->JLPT ? 0 : kl.Items[w->kanji[j]]->k->JLPT)*20;
                }
            }
        }
        if (!kanjicnt)
        {
            wi->kanjifreq = -20;
            wi->kanjiJLPT = 70;
        }
        else
        {
            wi->kanjifreq /= kanjicnt;
            wi->kanjiJLPT /= kanjicnt;
        }
    }

    for (int ix = 0; ix < kanjis->Count; ++ix)
        kanjis->Kanjis[ix]->NJLPT = kanjis->Kanjis[ix]->JLPT + (kanjis->Kanjis[ix]->JLPT >= 3 ? 1 : 0);

    twl->Sort(tmpJLPT_sortw_kfreq);
    for (int ix = 0; ix < twl->Count; ++ix)
        twl->Items[ix]->kanjifreq = ix;

    twl->Sort(tmpJLPT_sortw_kjlpt);
    for (int ix = 0; ix < twl->Count; ++ix)
        twl->Items[ix]->kanjiJLPT = ix;

    twl->Sort(tmpJLPT_sortw_efreq);
    for (int ix = 0; ix < twl->Count; ++ix)
        twl->Items[ix]->examplefreq = ix;

    twl->Sort(tmpJLPT_sortw_wfreq);
    for (int ix = 0; ix < twl->Count; ++ix)
        twl->Items[ix]->wordfreq = ix;

    twl->Sort(tmpJLPT_sortw_final);

    TGapList<bool> usedk;
    for (int ix = 0; ix < items->Count; ++ix)
    {
        TTempJLPTItem *it = items->Items[ix];
        if (!it->haskanji || it->kanaonly || !it->use || it->Nlevel == 1 || it->Nlevel > 3)
            continue;
        if (it->Nlevel == 2 && it->mark != jmKanji)
            continue;
        if (it->Nlevel == 3 && it->mark != jmFixed)
            continue;

        TWord *w = items->Words[ix];

        for (int j = wcslen(w->kanji); j >= 0; --j)
        {
            if (!KANJI(w->kanji[j]))
                continue;
            TKanji *k = kanjis->Kanjis[kanjis->IndexOf(w->kanji[j])];
            if (k->JLPT == 2)
            {
                k->NJLPT = 3;
                usedk.Items[w->kanji[j]] = true;
            }
        }
    }

    int wc = 0;
    TIntegerList *ktmp = new TIntegerList;
    for (int ix = 0; ix < twl->Count; ++ix)
    {
        TWord *w = items->Words[twl->Items[ix]->index];
        TTempJLPTItem *it = items->Items[twl->Items[ix]->index];

        /*if (items->Items[twl->Items[ix]->index]->Nlevel == 3 && items->Items[twl->Items[ix]->index]->Nlevel_orig != 3 && (int)usedk.TrueCount < edKN3->Text.ToInt()) {
         for (int j = wcslen(w->kanji)-1; j >= 0; --j) {
         if (!KANJI(w->kanji[j]))
         continue;
         TKanji *k = kanjis->Kanjis[kanjis->IndexOf(w->kanji[j])];
         if (k->JLPT == 2) {
         k->NJLPT = 3;
         usedk.Items[w->kanji[j]] = true;
         }
         }
         continue;
         }
         */
        if (it->Nlevel != 2 && it->Nlevel != 3 || it->mark == jmFixed)
            continue;

        if (it->mark != jmNone && it->mark != jmFixed)
        {
            bool changed = false;
            if (!it->use)
            {
                it->mark = jmNone;
                continue;
            }
            TTempJLPTItem *tmp = it->next;

            while (tmp != it)
            {
                if (tmp->use && tmp->Nlevel >= 3 && (tmp->haskanji && !tmp->kanaonly || tmp->kanaonly && it->mark == jmKanji))
                {
                    it->mark = jmFixed;
                    break;
                }
                tmp = tmp->next;
            }
            tmp = it->next;

            if (it->Nlevel == 3)
            {
                if (it->mark == jmLevel || it->mark == jmKanji && !it->haskanji)
                {
                    while (tmp != it)
                    {
                        if (tmp->Nlevel == 2)
                        {
                            it->use = false;
                            NCount[2]--;
                            if (!tmp->use)
                            {
                                tmp->use = true;
                                NCount[1]++;
                            }
                            tmp->mark = jmFixed;
                            if (tmp->haskanji && tmp->kanaonly && !it->kanaonly)
                                tmp->kanaonly = false;
                            changed = true;
                            break;
                        }
                        tmp = tmp->next;
                    }
                    if (!changed)
                    {
                        NCount[2]--;
                        NCount[1]++;
                        it->mark = jmFixed;
                        it->Nlevel == 2;

                        changed = true;
                    }

                }
                else if (it->mark == jmKanji)
                {
                    while (tmp != it)
                    {
                        if (tmp->Nlevel == 2)
                        {
                            if (!tmp->use)
                            {
                                NCount[1]++;
                                tmp->use = true;
                            }
                            tmp->kanaonly = false;
                            it->kanaonly = true;
                            it->mark = jmFixed;
                            tmp->mark = jmFixed;
                            changed = true;
                            break;
                        }
                        tmp = tmp->next;
                    }
                    if (!changed)
                    {
                        TTempJLPTItem *i2 = new TTempJLPTItem;
                        memset(i2, 0, sizeof(TTempJLPTItem));
                        i2->use = true;
                        i2->Nlevel = 2;
                        i2->definition = it->definition;
                        i2->meaningorig = new wchar_t[wcslen(it->meaningorig) + 1];
                        wcscpy(i2->meaningorig, it->meaningorig);
                        i2->haskanji = it->haskanji;
                        i2->windex = it->windex;
                        i2->area = it->area;
                        i2->mark = jmFixed;
                        it->mark = jmFixed;
                        i2->Nlevel_orig = 2;
                        i2->next = it->next;
                        it->next = i2;
                        i2->usekana = true;

                        i2->kanaonly = false;
                        it->kanaonly = true;

                        items->Add(i2);

                        NCount[1]++;
                        changed = true;
                    }
                }
            }
            else
            {
                if (it->mark == jmLevel || it->mark == jmKanji && !it->haskanji)
                {
                    while (tmp != it)
                    {
                        if (tmp->Nlevel == 3)
                        {
                            tmp->kanaonly = it->kanaonly;
                            if (!tmp->use)
                            {
                                tmp->use = true;
                                NCount[2]++;
                            }
                            NCount[1]--;
                            it->use = false;
                            tmp->mark = jmFixed;
                            changed = true;
                            break;
                        }
                        tmp = tmp->next;
                    }

                    if (!changed)
                    {
                        it->mark = jmFixed;
                        NCount[1]--;
                        NCount[2]++;
                        it->Nlevel = 3;
                        changed = true;
                    }
                }
                else if (it->mark == jmKanji)
                {
                    while (tmp != it)
                    {
                        if (tmp->Nlevel == 3)
                        {
                            if (!tmp->use)
                            {
                                tmp->use = true;
                                NCount[2]++;
                            }
                            tmp->mark = jmFixed;
                            it->mark = jmFixed;
                            tmp->kanaonly = true;
                            it->kanaonly = false;
                            changed = true;
                            break;
                        }
                        tmp = tmp->next;
                    }

                    if (!changed)
                    {
                        TTempJLPTItem *i2 = new TTempJLPTItem;
                        memset(i2, 0, sizeof(TTempJLPTItem));
                        i2->use = true;
                        i2->Nlevel = 3;
                        i2->definition = it->definition;
                        i2->meaningorig = new wchar_t[wcslen(it->meaningorig) + 1];
                        wcscpy(i2->meaningorig, it->meaningorig);
                        i2->haskanji = it->haskanji;
                        i2->windex = it->windex;
                        i2->area = it->area;
                        i2->mark = jmFixed;
                        it->mark = jmFixed;
                        i2->Nlevel_orig = 3;
                        i2->next = it->next;
                        it->next = i2;
                        i2->usekana = true;

                        i2->kanaonly = true;
                        it->kanaonly = false;

                        items->Add(i2);

                        NCount[2]++;
                        changed = true;
                    }
                }
            }
            if (changed)
                continue;
        }

        bool good = false;
        bool wordmatched = false;

        ktmp->Clear();

        for (int j = wcslen(w->kanji) - 1; j >= 0; --j)
        {
            if (!KANJI(w->kanji[j]))
                continue;

            if ((int)usedk.TrueCount < edKN3->Text.ToInt() || wordmatched)
            {

                if (kanjis->Kanjis[kanjis->IndexOf(w->kanji[j])]->JLPT == 2 && !usedk.ItemSet(w->kanji[j]))
                {
                    ktmp->Add(w->kanji[j]);
                    usedk.Items[w->kanji[j]] = true;
                    kanjis->Kanjis[kanjis->IndexOf(w->kanji[j])]->NJLPT = 3;
                }
                wordmatched = good = true;
            }
            else
            {
                good = (usedk.ItemSet(w->kanji[j]) || kanjis->Kanjis[kanjis->IndexOf(w->kanji[j])]->JLPT > 2) && w->frequency >= edNminf->Text.ToInt();
                if (!good)
                    break;
            }
        }

        if (good/*&& wc < 1850*/)
        {
            TTempJLPTItem *temp = items->Items[twl->Items[ix]->index];
            bool h2 = temp->Nlevel == 2, h3 = temp->Nlevel == 3, h45 = temp->Nlevel > 3, h1 = temp->Nlevel == 1;
            TTempJLPTItem *c = temp->next;
            while (c != temp/*&& !(h2 && h3)*/)
            {
                if (c->use)
                {
                    h1 = h1 || c->Nlevel == 1;
                    h2 = h2 || c->Nlevel == 2;
                    h3 = h3 || c->Nlevel == 3;
                    h45 = h45 || c->Nlevel > 3;
                }
                c = c->next;
            }
            if ((!h2 || !h3) && (!h1 || h45))
            {
                wc++;

                if (!h3 && items->Items[twl->Items[ix]->index]->Nlevel == 2)
                {
                    NCount[1]--;
                    NCount[2]++;

                    items->Items[twl->Items[ix]->index]->Nlevel = 3;
                    items->Items[twl->Items[ix]->index]->Nlevel_orig = 3;
                }
            }
            else
            {
                if (!h2 && items->Items[twl->Items[ix]->index]->Nlevel == 3)
                {
                    NCount[1]++;
                    NCount[2]--;

                    //for (int ix = 0; ix < ktmp->Count; ++ix)
                    //usedk.Delete(ktmp->Items[ix]);

                    items->Items[twl->Items[ix]->index]->Nlevel = 2;
                    items->Items[twl->Items[ix]->index]->Nlevel_orig = 2;
                }
            }
        }
        else
        {
            TTempJLPTItem *temp = items->Items[twl->Items[ix]->index];
            bool h2 = temp->Nlevel == 2;
            TTempJLPTItem *c = temp->next;
            while (c != temp && !h2)
            {
                if (c->use)
                    h2 = c->Nlevel == 2;
                c = c->next;
            }

            if (!h2 && items->Items[twl->Items[ix]->index]->Nlevel == 3)
            {
                NCount[1]++;
                NCount[2]--;

                //for (int ix = 0; ix < ktmp->Count; ++ix)
                //usedk.Delete(ktmp->Items[ix]);

                items->Items[twl->Items[ix]->index]->Nlevel = 2;
                items->Items[twl->Items[ix]->index]->Nlevel_orig = 2;
            }
        }

        if (items->Items[twl->Items[ix]->index]->Nlevel != 3 || cbExclude->Checked && items->Items[twl->Items[ix]->index]->kanaonly)
            for (int ix = 0; ix < ktmp->Count; ++ix)
            {
                usedk.ClearIndex(ktmp->Items[ix]);
                kanjis->Kanjis[kanjis->IndexOf((wchar_t)ktmp->Items[ix])]->NJLPT = 2;
            }
    }

    delete ktmp;

    /*UnicodeString order;
     for (int ix = 0; ix < tl->Count; ++ix) {
     if (tl->items[ix]->k->JLPT == 2)
     order += tl->Items[ix]->k->ch;
     }
     */

    delete tl;
    for (unsigned int ix = 0; ix < kl.TrueCount; ++ix)
    {
        delete kl.TrueItems[ix]->indexes;
        TTempJLPTKanjiCollection *ti = kl.TrueItems[ix];
        delete ti;
    }

    for (unsigned int ix = 0; ix < wl.TrueCount; ++ix)
    {
        TTempJLPTWordCollection *wi = wl.TrueItems[ix];
        delete wi;
    }

    delete twl;

    //fix katakana words that have jmLevel mark
    for (int ix = 0; ix < items->Count; ++ix)
    {
        TTempJLPTItem *it = items->Items[ix];
        if (it->mark == jmNone || it->mark == jmFixed || it->haskanji || it->Nlevel == 1 || it->Nlevel > 3)
            continue;
        if (it->Nlevel == 3)
        {
            it->Nlevel = 2;
            NCount[2]--;
            NCount[1]++;
            it->mark = jmFixed;
        }
        else if (it->Nlevel == 2)
        {
            it->Nlevel = 3;
            NCount[1]--;
            NCount[2]++;
            it->mark = jmFixed;
        }

    }

    updatenlabels();
    /*
     for(int ix = 0; ix < items->Count; ++ix)
     items->Items[ix]->Nlevel_orig = items->Items[ix]->Nlevel;
     */

    updatenlabels();
    updateklabels();
    Filter();
    wcallback();
    pb->Invalidate();

    modified = true;
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::count_ncount()
{
    memset(NCount, 0, sizeof(int)*5);
    for (int ix = 0; ix < items->Count; ++ix)
    {
        if (items->Items[ix]->use)
            NCount[items->Items[ix]->Nlevel - 1]++;
    }
    updatenlabels();
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::autofix()
{
    for (int ix = 0; ix < items->Count; ++ix)
        items->AutoFix(ix);
    count_ncount();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miAutoN3Click(TObject *Sender)
{
    if (miAutoN3->Checked == pAuto->Visible)
        return;

    pAuto->Visible = miAutoN3->Checked;
    if (!pAuto->Visible)
    {
        pb->Top -= pAuto->Height;
        pb->Height += pAuto->Height;
    }
    else
    {
        pb->Height -= pAuto->Height;
        pb->Top += pAuto->Height;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::AutoFix1Click(TObject *Sender)
{
    autofix();
    updatenlabels();
    updatecaption();
    Filter();
    wcallback();
    pb->Invalidate();

    modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::AllN3toN21Click(TObject *Sender)
{
    NCount[1] += NCount[2];
    NCount[2] = 0;
    for (int ix = 0; ix < items->Count; ++ix)
        if (items->Items[ix]->Nlevel == 3)
        {
            items->Items[ix]->Nlevel = 2;
            items->Items[ix]->Nlevel_orig = 2;
        }
    updatenlabels();
    updatecaption();
    Filter();
    wcallback();
    pb->Invalidate();

    modified = true;
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::updateklabels()
{
    TLabel *lb[5] = { lbK1, lbK2, lbK3, lbK4, lbK5 };
    TWord *w;
    int lix = 0;
    if (wobj->pos >= 0)
    {
        w = items->Words[realrow(wobj->pos)];
        for (unsigned int ix = 0; ix < wcslen(w->kanji) && lix < 5; ++ix)
        {
            if (!KANJI(w->kanji[ix]))
                continue;

            lb[lix]->Caption = UnicodeString(w->kanji[ix]) + "(" + kanjis->Kanjis[kanjis->IndexOf(w->kanji[ix])]->NJLPT + ")";
            lix++;
        }
    }

    for (; lix < 5; ++lix)
        lb[lix]->Caption = "-";
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::pbKeyPress(TObject *Sender, wchar_t &Key)
{
    if (wobj->pos < 0)
        return;

    if (Key == VK_SPACE)
    {
        if (btnL->Down)
        {
            btnK->Down = true;
            btnK->Click();
        }
        else if (btnK->Down)
        {
            btnF->Down = true;
            btnF->Click();
        }
        else if (btnF->Down)
        {
            btnF->Down = false;
            btnF->Click();
        }
        else
        {
            btnL->Down = true;
            btnL->Click();
        }
    }
    else if (Key == VK_RETURN)
    {
        edMeaning->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::btnLClick(TObject *Sender)
{
    int row = realrow(wobj->pos);
    if (btnL->Down)
    {
        items->Items[row]->mark = jmLevel;
        miL->Checked = true;
        miK->Checked = false;
        miF->Checked = false;
    }
    else if (btnK->Down)
    {
        items->Items[row]->mark = jmKanji;
        miK->Checked = true;
        miL->Checked = false;
        miF->Checked = false;
    }
    else if (btnF->Down)
    {
        items->Items[row]->mark = jmFixed;
        miF->Checked = true;
        miL->Checked = false;
        miK->Checked = false;
    }
    else
    {
        items->Items[row]->mark = jmNone;
        miL->Checked = false;
        miK->Checked = false;
        miF->Checked = false;
    }
    modified = true;
    wobj->invalidateselected();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miLClick(TObject *Sender)
{
    btnL->Down = !btnL->Down;
    btnL->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miKClick(TObject *Sender)
{
    btnK->Down = !btnK->Down;
    btnK->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miFClick(TObject *Sender)
{
    btnL->Down = !btnL->Down;
    btnL->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (btnSave->Enabled && Shift.Contains(ssCtrl) && Key == VK_RETURN)
    {
        btnSave->Click();
    }
    else if (Key == VK_F2)
        edKanji->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::btnDefinitionClick(TObject *Sender)
{
    btnDefinition->Tag++;
    if (btnDefinition->Tag >= dictionary->Count)
        btnDefinition->Tag = 0;
    btnDefinition->Caption = dictionary->Strings[btnDefinition->Tag];

    wcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::Katakanaautodef1Click(TObject *Sender)
{
    autofixkatakana();
    updatenlabels();
    updatecaption();
    Filter();
    wcallback();
    pb->Invalidate();

    modified = true;
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::autofixkatakana()
{
    for (int ix = 0; ix < items->Count; ++ix)
        items->AutoFixKatakana(ix);
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::Autodefshortword1Click(TObject *Sender)
{
    autodefinewords(45);
    updatenlabels();
    updatecaption();
    Filter();
    wcallback();
    pb->Invalidate();

    modified = true;
}
//---------------------------------------------------------------------------
void TfrmJLPTEdit::autodefinewords(int max_meaning_length)
{
    for (int ix = 0; ix < items->Count; ++ix)
        items->AutoDefineWords(ix, max_meaning_length, 4);
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miFilterAClick(TObject *Sender)//throws out words from group that are non N2 and were defined as kanji words in any previous N level
{
    bool anymatch = MsgBox("Throw out words on any match?", "Query", MB_YESNO) == IDYES;

    TWordGroup *g = dictionary->Items[0]->groups->Items[((TMenuItem*)Sender)->Tag];
    TWord *w;
    for (int ix = g->list->Count - 1; ix >= 0; --ix)
    {
        w = g->list->Words[ix];

        bool good = true;
        for (int j = wcslen(w->kanji) - 1; j >= 0 && good && !anymatch; --j)
        {
            if (!KANJI(w->kanji[j]))
                continue;

            if (kanjis->Kanjis[kanjis->IndexOf(w->kanji[j])]->JLPT == 1)//the word shouldn't be here with jlpt 1 kanji, leave it alone and handle in the final import process later
                        good = false;
        }
        if (!good)
            continue;

        for (int iy = 0; iy < items->Count && good; ++iy)
        {
            TTempJLPTItem *it = items->Items[iy];
            if (!it->use || it->windex != g->list->Indexes[ix] && !SameWord(items->Words[iy], w, true))
                continue;

            if (anymatch || it->Nlevel == 1 && it->kanaonly)
                good = false;
            else if ((!it->haskanji || it->haskanji && !it->kanaonly) && (it->Nlevel == 1 || it->Nlevel > 3))
                good = false;

            break;
        }

        if (!good)
            g->DeleteWord(ix);
    }

    fGroup->lbWGroupsClick(fGroup->lbWGroups);
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miFilterBClick(TObject *Sender)//throws out words from group that are non N2 and were defined in any previous N level
{
    bool anymatch = MsgBox("Throw out words on any match?", "Query", MB_YESNO) == IDYES;

    TWordGroup *g = dictionary->Items[0]->groups->Items[((TMenuItem*)Sender)->Tag];
    for (int ix = g->list->Count - 1; ix >= 0; --ix)
    {
        bool good = true;

        for (int iy = 0; iy < items->Count && good; ++iy)
        {
            TTempJLPTItem *it = items->Items[iy];
            if (!it->use || it->windex != g->list->Indexes[ix] && !SameWord(items->Words[iy], g->list->Words[ix], true))
                continue;

            if (anymatch || it->Nlevel != 2 && it->Nlevel != 3/*&& it->kanaonly*/)
                good = false;

            break;
        }

        if (!good)
            g->DeleteWord(ix);
    }

    fGroup->lbWGroupsClick(fGroup->lbWGroups);
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miCountN2KanjiClick(TObject *Sender)
{
    TGapList<bool> nkanji;
    TWordGroup *g = dictionary->Items[0]->groups->Items[((TMenuItem*)Sender)->Tag];
    TWord *w;
    for (int ix = g->list->Count - 1; ix >= 0; --ix)
    {
        w = g->list->Words[ix];

        bool good = false;
        for (int j = 0; j < items->Count && !good; ++j)
        {
            TTempJLPTItem *it = items->Items[j];
            good = it->windex == g->list->Indexes[ix] && (it->Nlevel == 2 || it->Nlevel == 3) && it->use;
        }

        for (int j = wcslen(w->kanji) - 1; j >= 0 && good; --j)
        {
            if (!KANJI(w->kanji[j]))
                continue;

            if (kanjis->Kanjis[kanjis->IndexOf(w->kanji[j])]->JLPT == 1)//the word shouldn't be here with jlpt 1 kanji, leave it alone and handle in the final import process later
                        good = false;
        }
        if (!good)
            continue;

        for (int j = wcslen(w->kanji) - 1; j >= 0 && good; --j)
        {
            if (!KANJI(w->kanji[j]))
                continue;

            TKanji *k = kanjis->Kanjis[kanjis->IndexOf(w->kanji[j])];
            if (k->JLPT == 2 && !nkanji.ItemSet(int(k->ch)))
                nkanji.Items[int(k->ch)] = true;

        }
    }
    MsgBox(nkanji.TrueCount, "Possible N2 kanji count in group", MB_OK);
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miMarkN3Click(TObject *Sender)
{
    TWordGroup *g = dictionary->Items[0]->groups->Items[((TMenuItem*)Sender)->Tag];
    TWord *w;
    for (int ix = g->list->Count - 1; ix >= 0; --ix)
    {
        w = g->list->Words[ix];

        bool good = false;
        TTempJLPTItem *it = NULL;
        for (int j = 0; j < items->Count && !good; ++j)
        {
            it = items->Items[j];
            good = it->windex == g->list->Indexes[ix] && it->use && (it->Nlevel == 2 || it->Nlevel == 3);
        }

        if (good && it->mark != jmNone)
        {
            g->DeleteWord(ix);
            good = false;
        }

        if (!good)
            continue;

        for (int j = wcslen(w->kanji) - 1; j >= 0 && good; --j)
        {
            if (!KANJI(w->kanji[j]))
                continue;

            if (kanjis->Kanjis[kanjis->IndexOf(w->kanji[j])]->JLPT == 1)//the word shouldn't be here with jlpt 1 kanji, leave it alone and handle in the final import process later
                        good = false;
        }

        if (it->Nlevel != 3 && it->Nlevel != 2 && good)
            THROW(L"?");

        if (it->Nlevel != 2)
        {
            NCount[it->Nlevel - 1]--;
            NCount[1]++;
        }

        it->Nlevel = 2;
        if (good)
        {
            it->mark = jmLevel;
        }
        else
        {
            if (it->kanaonly)
                it->mark = jmLevel;
            else
                it->mark = jmKanji;
        }

        g->DeleteWord(ix);
    }
    modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miMarkN3KanaClick(TObject *Sender)
{
    TWordGroup *g = dictionary->Items[0]->groups->Items[((TMenuItem*)Sender)->Tag];
    for (int ix = g->list->Count - 1; ix >= 0; --ix)
    {
        bool good = false;
        TTempJLPTItem *it = NULL;
        for (int j = 0; j < items->Count && !good; ++j)
        {
            it = items->Items[j];
            good = it->windex == g->list->Indexes[ix] && it->use && (it->Nlevel == 2 || it->Nlevel == 3);
        }

        if (good && it->mark != jmNone)
        {
            g->DeleteWord(ix);
            good = false;
        }

        if (!good)
            continue;

        if (it->Nlevel != 3 && it->Nlevel != 2 && good)
            THROW(L"?");

        if (it->Nlevel != 2)
        {
            NCount[it->Nlevel - 1]--;
            NCount[1]++;
        }

        it->Nlevel = 2;
        if (it->kanaonly)
            it->mark = jmLevel;
        else
            it->mark = jmKanji;

        g->DeleteWord(ix);
    }
    modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::miAddN3WordsClick(TObject *Sender)
{
    if (MsgBox("Only use this function if words don't have to be checked for duplication! Do you want to continue?", "Query", MB_YESNO) == IDNO)
        return;

    TWordGroup *g = dictionary->Items[0]->groups->Items[((TMenuItem*)Sender)->Tag];
    for (int ix = g->list->Count - 1; ix >= 0; --ix)
    {
        TWord *w = g->list->Words[ix];

        bool match = false;
        for (int k = 0; k < items->Count && !match; ++k)
            match = g->list->Indexes[ix] == items->Items[k]->windex || SameWord(items->Words[k], w);
        if (match)
            continue;

        int mink = 3;
        bool nokanji = true;
        for (int j = wcslen(w->kanji); j >= 0 && mink; --j)
        {
            if (!KANJI(w->kanji[j]))
                continue;

            nokanji = false;
            int n = kanjis->Kanjis[kanjis->IndexOf(w->kanji[j])]->NJLPT;
            if (!n)
                n = kanjis->Kanjis[kanjis->IndexOf(w->kanji[j])]->JLPT;

            mink = min(n, mink);
        }

        bool wkanaonly = (w->data[0].notes & WN_KANAONLY) == WN_KANAONLY;

        TTempJLPTItem *it = new TTempJLPTItem;
        memset(it, 0, sizeof(TTempJLPTItem));
        it->windex = g->list->Indexes[ix];
        it->haskanji = !nokanji;
        it->kanaonly = wkanaonly || mink < 3 || nokanji;
        it->usekana = true;
        it->meaningorig = new wchar_t[2];
        wsprintf(it->meaningorig, L"\*");
        it->Nlevel = 3;
        it->Nlevel_orig = 3;
        it->use = true;
        it->next = it;
        it->definition = 0;
        it->mark = jmFixed;

        NCount[2]++;
        items->Add(it);

        if (!wkanaonly && mink < 3 && mink && !nokanji)
        {
            TTempJLPTItem *it2 = new TTempJLPTItem;
            memset(it2, 0, sizeof(TTempJLPTItem));
            it2->windex = g->list->Indexes[ix];
            it2->haskanji = true;
            it2->kanaonly = false;
            it2->usekana = true;
            it2->meaningorig = new wchar_t[2];
            wsprintf(it2->meaningorig, L"?");
            it2->Nlevel = mink;
            it2->Nlevel_orig = mink;
            it2->use = true;
            it2->next = it;
            it->next = it2;
            it2->definition = 0;
            it2->mark = jmFixed;

            NCount[mink - 1]++;
            items->Add(it2);
        }
    }

    updatenlabels();
    updatecaption();
    Filter();
    wcallback();
    pb->Invalidate();
    modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::JumptoSame1Click(TObject *Sender)
{
    int a = wobj->pos, b = -1;
    do
    {
        if (b >= 0)
        {
            if (items->Items[realrow(a)]->windex != items->Items[realrow(b)]->windex && SameWord(items->Words[realrow(a)], items->Words[realrow(b)]))
            {
                wobj->pos = a;
                return;
            }
        }
        b = a;
        do
        {
            a++;
        }
        while (a < view->Count && !items->Items[realrow(a)]->use);
    }
    while (a < view->Count);

    MsgBox(L"Finished searching the word list.", L"Message", MB_OK);
}
//---------------------------------------------------------------------------
void __fastcall TfrmJLPTEdit::Export1Click(TObject *Sender)
{
    items->Export(paths.installdatapath + "JLPTNData.txt");
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(65);
#endif

