//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "words.h"
#include "wordgroupstudy.h"
#include "collection.h"
#include "zkformats.h"
#include "romajizer.h"
#include "genworddata.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(194);
#endif

void delete_word_meanings(int count, TMeaningData* &data)
{
    for (int i = 0; i < count; i++)
        delete[] data[i].meaning;
    delete[] data;
}

void delete_word(TWord *&w)
{
    delete[] w->stats;

    delete[] w->kanji;
    delete[] w->kana;
    delete[] w->romaji;

    delete_word_meanings(w->meaningcnt, w->data);

    while (w->group)
    {
        TWordGroups *g = w->group;
        w->group = g->next;
        delete g;
    }

    delete w;
}

bool MeaningDataMatch(const TMeaningData &a, const TMeaningData &b)
{
    bool dummy = false;
    return MeaningDataMatch(a, b, dummy) && dummy;
}

bool MeaningDataMatch(const TMeaningData &a, const TMeaningData &b, bool &fullmatch)
{
    fullmatch = false;
    if (wcscmp(a.meaning, b.meaning))
        return false;

    fullmatch = WordTypeMatch(a, b);
    return true;
}

int WordCompare(const wchar_t *kanjia, const wchar_t *kanaa, const wchar_t *kanjib, const wchar_t *kanab)
{
    int r = wcscmp(kanjia, kanjib);
    if (!r)
        return wcscmp(kanaa, kanab);
    return r;
}

bool WordTypeMatch(const TMeaningData &a, const TMeaningData &b)
{
    return a.types == b.types && a.notes == b.notes && a.fields == b.fields && a.nametag == b.nametag;
}

//---------------------------------------------------------------------------
wchar_t cdelim[256]; //" 0123456789=\"'[]{}:,./<>?\\~!@#$%^&*()_+|\t\n\r`;";
//---------------------------------------------------------------------------
TWordList::TWordList() : inherited()
{
    ;
}

TWordList::TWordList(TWordList *orig)
{
    Capacity = orig->Count;
    while (Count != orig->Count)
    {
        TWord *w = new TWord;
        TWord *w2 = orig->Items[Count];
        memcpy(w, w2, sizeof(TWord));
        w->group = NULL;
        if (w2->stats)
        {
            w->stats = new TWordStatGlobal[w->meaningcnt];
            memcpy(w->stats, w2->stats, sizeof(TWordStatGlobal) * w->meaningcnt);
        }
        TMeaningData *dat = w->data = new TMeaningData[w->meaningcnt];
        memcpy(dat, w2->data, sizeof(TMeaningData) * w->meaningcnt);
        for (int ix = 0; ix < w->meaningcnt; ++ix)
        {
            dat[ix].meaning = new wchar_t[wcslen(w2->data[ix].meaning) + 1];
            wcscpy(dat[ix].meaning, w2->data[ix].meaning);
        }

        w->kanji = new wchar_t[wcslen(w2->kanji) + 1];
        wcscpy(w->kanji, w2->kanji);
        w->kana = new wchar_t[wcslen(w2->kana) + 1];
        wcscpy(w->kana, w2->kana);
        w->romaji = new wchar_t[wcslen(w2->romaji) + 1];
        wcscpy(w->romaji, w2->romaji);

        Add(w);
    }
}

void TWordList::Export(FILE *f)
{
    UnicodeString z;
    for (int i = 0; i < Count; i++)
    {
        if (i % 100)
            Application->ProcessMessages();
        int llen = wcslen(Items[i]->kanji) + 1;
        fwrite(Items[i]->kanji, sizeof(wchar_t), wcslen(Items[i]->kanji), f);
        fdelim(f);
        llen += wcslen(Items[i]->kana) + 1;
        fwrite(Items[i]->kana, sizeof(wchar_t), wcslen(Items[i]->kana), f);
        fdelim(f);
        llen += z.Length() + 1;
        z = (int)Items[i]->frequency;
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        fdelim(f);

        for (int j = 0; j < Items[i]->meaningcnt; j++)
        {
            int addlen = (j ? 1 : 0) + 3 + wcslen(Items[i]->data[j].meaning) + ((UnicodeString)Items[i]->data[j].types).Length() + ((UnicodeString)Items[i]->data[j].notes).Length() + ((UnicodeString)int(Items[i]->data[j].fields)).Length();
            if (llen + addlen > MAX_EXPORT_LINE_SIZE)
            {
                MsgBox(((UnicodeString)L"Word definition line too long! Skipping to next word! (" + Items[i]->data[j].meaning + ")").c_str(), L"Error", MB_OK);
                break;
            }
            if (j)
                fdelim(f);

            z = Items[i]->data[j].meaning;
            fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
            fdelim(f);
            z = Items[i]->data[j].types;
            fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
            fdelim(f);
            z = Items[i]->data[j].notes;
            fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
            fdelim(f);
            z = int(Items[i]->data[j].fields);
            fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
            fdelim(f);
            z = Items[i]->data[j].nametag;
            fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);

            llen += addlen;
        }
        feol(f);
    }
    feol(f); //empty line after all words
}
//---------------------------------------------------------------------------
UnicodeString TWordList::SQLEscape(wchar_t *c)
{
    UnicodeString s;
    wchar_t *tmp = c;

    int l = wcscspn(tmp, L"|;\\");

    while (*(tmp + l))
    {
        s += UnicodeString(tmp, l);
        s += L"\\";
        s += *(tmp + l);
        tmp += l + 1;
        l = wcscspn(tmp, L"|;\\");
    }
    s += UnicodeString(tmp, l);

    return s;
}
//---------------------------------------------------------------------------
void TWordList::SQLExport(FILE *f, FILE *f2)
{
    UTF8String sep = L"|";
    UTF8String sep2 = L";";
    UTF8String ln = L"\n";
    /*char sep[6];
     int seplen = UnicodeToUtf8(sep,6,L"|",1)-1;
     char sep2[6];
     int seplen2 = UnicodeToUtf8(sep2,6,L";",1)-1;
     char ln[6];
     int lnlen = UnicodeToUtf8(ln,6,L"\n",1)-1;
     */
    //char uc[1024];
    //int uclen;

    UTF8String s;

    for (int i = 0; i < Count; i++)
    {
        fwrite(sep.c_str(), sizeof(char), sep.Length(), f);
        s = i;
        fwrite(s.c_str(), sizeof(char), s.Length(), f);

        fwrite(sep.c_str(), sizeof(char), sep.Length(), f);
        fwrite(sep2.c_str(), sizeof(char), sep2.Length(), f);
        fwrite(sep.c_str(), sizeof(char), sep.Length(), f);

        s = Items[i]->kanji;
        fwrite(s.c_str(), sizeof(char), s.Length(), f);

        fwrite(sep.c_str(), sizeof(char), sep.Length(), f);
        fwrite(sep2.c_str(), sizeof(char), sep2.Length(), f);
        fwrite(sep.c_str(), sizeof(char), sep.Length(), f);

        s = Items[i]->kana;
        fwrite(s.c_str(), sizeof(char), s.Length(), f);

        fwrite(sep.c_str(), sizeof(char), sep.Length(), f);
        fwrite(sep2.c_str(), sizeof(char), sep2.Length(), f);
        fwrite(sep.c_str(), sizeof(char), sep.Length(), f);

        s = Romanize(Items[i]->kana); //romaji;
        fwrite(s.c_str(), sizeof(char), s.Length(), f);

        fwrite(sep.c_str(), sizeof(char), sep.Length(), f);
        fwrite(sep2.c_str(), sizeof(char), sep2.Length(), f);
        fwrite(sep.c_str(), sizeof(char), sep.Length(), f);

        s = Items[i]->frequency;
        fwrite(s.c_str(), sizeof(char), s.Length(), f);

        fwrite(sep.c_str(), sizeof(char), sep.Length(), f);
        fwrite(ln.c_str(), sizeof(char), ln.Length(), f);

        for (int j = 0; j < Items[i]->meaningcnt; j++)
        {
            fwrite(sep.c_str(), sizeof(char), sep.Length(), f2);

            s = i;
            fwrite(s.c_str(), sizeof(char), s.Length(), f2);

            fwrite(sep.c_str(), sizeof(char), sep.Length(), f2);
            fwrite(sep2.c_str(), sizeof(char), sep2.Length(), f2);
            fwrite(sep.c_str(), sizeof(char), sep.Length(), f2);

            s = SQLEscape(Items[i]->data[j].meaning);
            fwrite(s.c_str(), sizeof(char), s.Length(), f2);

            fwrite(sep.c_str(), sizeof(char), sep.Length(), f2);
            fwrite(sep2.c_str(), sizeof(char), sep2.Length(), f2);
            fwrite(sep.c_str(), sizeof(char), sep.Length(), f2);

            s = Items[i]->data[j].types;
            fwrite(s.c_str(), sizeof(char), s.Length(), f2);

            fwrite(sep.c_str(), sizeof(char), sep.Length(), f2);
            fwrite(sep2.c_str(), sizeof(char), sep2.Length(), f2);
            fwrite(sep.c_str(), sizeof(char), sep.Length(), f2);

            s = Items[i]->data[j].notes;
            fwrite(s.c_str(), sizeof(char), s.Length(), f2);

            fwrite(sep.c_str(), sizeof(char), sep.Length(), f2);
            fwrite(sep2.c_str(), sizeof(char), sep2.Length(), f2);
            fwrite(sep.c_str(), sizeof(char), sep.Length(), f2);

            s = int(Items[i]->data[j].fields);
            fwrite(s.c_str(), sizeof(char), s.Length(), f2);

            fwrite(sep.c_str(), sizeof(char), sep.Length(), f2);
            fwrite(ln.c_str(), sizeof(char), ln.Length(), f2);
        }
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordIndexList::TWordIndexList(TWordCollection *acollection/*, bool avocablist*/) : fcollection(acollection)/*,fvocablist(avocablist)*/, inherited()
{
    //freeup = true;
}
//---------------------------------------------------------------------------
TWordIndexList::TWordIndexList(TWordIndexList *alist) : inherited()
{
    Assign(alist);
}
//---------------------------------------------------------------------------
//assign another list to this - make a copy
void TWordIndexList::Assign(TWordIndexList *src)
{
    fcollection = src->fcollection;
    Clear();
    for (int i = 0; i < src->Count; i++)
        Add(src->Items[i]);
}
//---------------------------------------------------------------------------
int TWordIndexList::get_index(int i)
{
    return Items[i]->index;
}
//---------------------------------------------------------------------------
TWord* TWordIndexList::get_word(int i)
{
    if (!Items[i] || Items[i]->index >= fcollection->words->Count)
        THROW(L"whadda");
    return fcollection->words->Items[Items[i]->index];
}
//---------------------------------------------------------------------------
int TWordIndexList::Add(int ix)
{
    return Add(ix, -1);
}
//---------------------------------------------------------------------------
int TWordIndexList::Add(int ix, byte ameaning)
{
    TWordIndex *wi;
    wi = new TWordIndex;
    wi->index = ix;
    wi->inf = NULL;
    wi->meaning = ameaning;
    return inherited::Add(wi);
}
//---------------------------------------------------------------------------
TInfList* CopyInfList(TInfList *src)
{
    if (!src)
        return NULL;
    TInfList *result = new TInfList;
    TInfList *sl = src;
    TInfList *wl = result;
    while (sl)
    {
        wl->inf = sl->inf;
        if (sl->next)
        {
            wl->next = new TInfList;
            wl = wl->next;
        }
        else
            wl->next = NULL;
        sl = sl->next;
    }
    return result;
}
//add an existing word (from another list by assignment, presumably)
//the inflection list is copied, so the original pointer changes
int TWordIndexList::Add(TWordIndex *src)
{
    return AddInflected(src->index, src->meaning, src->inf);
}
//---------------------------------------------------------------------------
int TWordIndexList::AddInflected(int val, char meaning, TInfList *inf)
{
    TWordIndex *wi = new TWordIndex;
    wi->index = val;
    wi->inf = CopyInfList(inf);
    wi->meaning = meaning;
    return inherited::Add(wi);
}
//---------------------------------------------------------------------------
int TWordIndexList::AddInflected(int val, TInfList *inf)
{
    return AddInflected(val, -1, inf);
}
//---------------------------------------------------------------------------

//add the indexes of src with the inflected bit set
void TWordIndexList::AddInflected(TIntegerList *src, TInfList *inf)
{
    for (int i = 0; i < src->Count; i++)
        AddInflected(src->Items[i], -1, inf);
}
//---------------------------------------------------------------------------

//add the indexes of src with the inflected bit set
void TWordIndexList::AddInflected(TWordIndexList *src, TInfList *inf)
{
    for (int i = 0; i < src->Count; i++)
        AddInflected(src->Items[i]->index, src->Items[i]->meaning, inf);
}
//---------------------------------------------------------------------------
void TWordIndexList::FreeItem(TWordIndex *elem, int ix)
{
    //delete inflection data
    TInfList *l;
    while ((l = elem->inf) != 0)
    {
        elem->inf = l->next;
        delete l;
    }

    delete elem;
}
//---------------------------------------------------------------------------
wchar_t* TWordIndexList::get_meaning(int i)
{
    if (Items[i]->meaning < 0)
        return NULL;
    TWord *w = fcollection->words->Items[Items[i]->index];
    return w->data[Items[i]->meaning].meaning;
}
//---------------------------------------------------------------------------
int TWordIndexList::IndexOf(int ix, char ameaning)
{
    for (int i = 0; i < Count; i++)
        if (Items[i]->index == ix && (ameaning < 0 || Items[i]->meaning == ameaning))
            return i;
    return -1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
struct TWordSearchData
{
    int index;
    word freq;

    int len;
    int pos;
    byte mean;
};

int rfreqwordsort(TWordSearchData **aa, TWordSearchData **bb)
{
    if ((*bb)->freq <= 1400 && (*aa)->freq > 1700)
        return -1;
    if ((*bb)->freq > 1700 && (*aa)->freq <= 1400)
        return 1;
    if (abs((*bb)->freq - (*aa)->freq) > 2500)
        return (*bb)->freq - (*aa)->freq;

    return (*bb)->freq - (*aa)->freq + ((*aa)->mean - (*bb)->mean) * 500 + ((*aa)->len - (*bb)->len) * 100 + ((*aa)->pos - (*bb)->pos) * 50;
}

void TWordIndexList::RomajiSort(wchar_t *ch)
{
    int n, m, o;
    int chlen = wcslen(ch);
    wchar_t *a;

    TTemplateList<TWordSearchData, true> *l = new TTemplateList<TWordSearchData, true>;

    l->Capacity = Count;
    while (l->Count < Count)
    {
        TWordSearchData *data = new TWordSearchData;
        TWord *w = collection->words->Items[Indexes[l->Count]];
        data->freq = w->frequency;
        data->index = l->Count;

        data->len = 1000;
        data->pos = 1000;
        data->mean = 255;

        for (int i = 0; i < w->meaningcnt && data->len > chlen; i++)
        {
            a = w->data[i].meaning;

            while (a)
            {
                a = WCSSTRI(a, ch);
                if (!a)
                    break;
                if (a != w->data[i].meaning && !wcschr(cdelim, *(a - 1)))
                {
                    a++;
                    continue;
                }
                n = wcscspn(a, cdelim);

                m = n;
                while (a - (m - n) != w->data[i].meaning && *(a - 1 - (m - n)) != ',')
                    m++;

                o = n - m;
                while (*(a + (m + o)) && *(a + (m + o)) != ',')
                    m++;

                //remove bracket length
                int br = 0;
                while (*(a + o) && *(a + o) != ',')
                {
                    if (*(a + o) == '(')
                        br++;
                    if (br && *(a + o) == ')')
                        br--;
                    if (br)
                        m--;
                    o++;
                }

                if (m < data->len)
                {
                    data->len = m;
                    data->pos = a - w->data[i].meaning;
                    data->mean = i;
                }
                a += n;

                if (!n)
                    break;
            }
        }

        l->Add(data);
    }

    l->Sort(rfreqwordsort);
    TTemplateList<TWordIndex, false> *ltemp = new TTemplateList<TWordIndex, false>;
    ltemp->Capacity = Count;
    while (ltemp->Count < Count)
        ltemp->Add(inherited::Items[ltemp->Count]);

    for (int i = 0; i < Count; i++)
        inherited::Items[i] = ltemp->Items[l->Items[i]->index];
    delete ltemp;

    /*
    while (l->Count)
    {
        delete l->Items[l->Count - 1];
        l->Delete(l->Count - 1);
    }
    */
    delete l;
}
//---------------------------------------------------------------------------
TWordList *sortlist;

//---------------------------------------------------------------------------
int kfreqwordsort(TWordIndex **aa, TWordIndex **ab);
void TWordIndexList::KanaSort()
{
    sortlist = collection->words;
    inherited::Sort(kfreqwordsort);
}
//---------------------------------------------------------------------------
int kjfreqwordsort(TWordIndex **aa, TWordIndex **ab);
void TWordIndexList::KanjiSort()
{
    sortlist = collection->words;
    inherited::Sort(kjfreqwordsort);
}
//---------------------------------------------------------------------------
int kixwordsort(TWordIndex **aa, TWordIndex **ab);
void TWordIndexList::IndexSort()
{
    sortlist = collection->words;
    inherited::Sort(kixwordsort);
}
//---------------------------------------------------------------------------
//sort kana readings
int kfreqwordsort(TWordIndex **aa, TWordIndex **ab)
{
    int a, b;
    int af, bf;

    af = (sortlist->Items[(*aa)->index])->frequency;
    bf = (sortlist->Items[(*ab)->index])->frequency;

    if (abs(bf - af) > 1500 || (abs(bf - af) > 500 && (bf < 1200 || af < 1200)))
        return bf - af;

    a = wcslen(sortlist->Items[(*aa)->index]->kana);
    b = wcslen(sortlist->Items[(*ab)->index]->kana);

    if (a == b)
    {
        //put inflected last
        if ((*aa)->inf && !(*ab)->inf)
            return 1;
        if ((*ab)->inf && !(*aa)->inf)
            return -1;

        return bf - af;
    }

    return a - b;
}
//---------------------------------------------------------------------------
//sort kanji readings
int kjfreqwordsort(TWordIndex **aa, TWordIndex **ab)
{
    int a, b;
    int af, bf;

    af = (sortlist->Items[(*aa)->index])->frequency;
    bf = (sortlist->Items[(*ab)->index])->frequency;

    if (abs(bf - af) > 1500 || (abs(bf - af) > 500 && (bf < 1200 || af < 1200)))
        return bf - af;

    a = wcslen(sortlist->Items[(*aa)->index]->kanji);
    b = wcslen(sortlist->Items[(*ab)->index]->kanji);

    af = Sign((sortlist->Items[(*aa)->index]->data[0].notes & WN_KANAONLY) - (sortlist->Items[(*ab)->index]->data[0].notes & WN_KANAONLY));
    if (af < 0 && a <= b || af > 0 && b <= a)
        return af;

    af = 0;
    bf = 0;

    for (int i = 0; i < a; i++)
        if (KANJI(sortlist->Items[(*aa)->index]->kanji[i]))
            af++;
    for (int i = 0; i < b; i++)
        if (KANJI(sortlist->Items[(*ab)->index]->kanji[i]))
            bf++;

    if (af < bf && a - 1 <= b)
        return -1;
    if (bf < af && b - 1 <= a)
        return 1;

    if (a == b)
    {
        //put inflected last
        if ((*aa)->inf && !(*ab)->inf)
            return 1;
        if ((*ab)->inf && !(*aa)->inf)
            return -1;

        return bf - af;
    }

    return a - b;
}
//---------------------------------------------------------------------------
int kixwordsort(TWordIndex **aa, TWordIndex **ab)
{
    return (*aa)->index - (*ab)->index;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TInfList* TWordInfList::Add(UnicodeString s, byte type, TInfList *inf, byte inftype)
{
    for (int i = 0; i < Count; i++)//find the same one and quit if found
        if (Items[i]->type == type && !wcscmp(s.c_str(), Items[i]->str))
            return NULL;

    //add the inflection to the list
    if (!inf)
    {
        inf = new TInfList;
        inf->next = NULL;
    }
    else
    {
        TInfList *tmp = new TInfList;
        tmp->next = inf;
        inf = tmp;
    }
    inf->inf = inftype;

    TWordInf *w = new TWordInf;
    w->type = type;
    w->inf = inf;
    w->str = new wchar_t[wcslen(s.c_str()) + 1];
    wcscpy(w->str, s.c_str());
    inherited::Add(w);

    return inf;
}
//---------------------------------------------------------------------------
void TWordInfList::FreeItem(TWordInf *item, int ix)
{
    delete[] item->str;
    delete item->inf;

    delete item;
}
//---------------------------------------------------------------------------
//----- DEINFLECT and attached methods --------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void DeAdjectivize(UnicodeString s, int &types, TWordInfList *l);

TInfList* _add_variant(TWordInfList *l, UnicodeString s, byte type, int &types, TInfList *inf, byte inftype)
{
    //return for invalid parameter
    if (s.Length() == 0 || s == L"っ")
        return inf;

    if (!inf && inftype == WI_RARERU && (s == L"できる" || s == L"出来る" || s == L"出來る"))
    {
        inftype = WI_RERU;
    }

    //return for double passive or potential after passive
    if (inf && (inftype == WI_RERU || inftype == WI_SERARERU) && (inf->inf == WI_RERU || inf->inf == WI_RARERU))
        return inf;

    //return for double potential
    if (inftype == WI_ERU)
    {
        if (inf && inf->inf == WI_ERU || s == L"できる" || s == L"出来る" || s == L"出來る")
            return inf;
    }

    //return when we have too many inflections or there is no string left
    if (!s.Length() || l->Count > 50 || inftype == WI_SURU && type == 14)
        return inf;

    //return for bad iru/aru inflections
    if (inftype == WI_NU && (s == L"いる" || s == L"ある") || inftype == WI_NAI && s == L"ある")
        return inf;

    //return for bad NU inflections
    if (inf && inftype == WI_NU && (inf->inf != WI_EBA))
        return inf;

    int lastvar = -1;
    for (int i = l->Count - 1; i >= 0 && lastvar < 0; i--)
        if (l->Items[i]->inf == inf)
            lastvar = (1 << l->Items[i]->type);

    //do nothing for inflection types that must be the first inflection
    if (inf && (inftype == WI_MASU || inftype == WI_MASEN || inftype == WI_MASHITA || inftype == WI_MASHOU || inftype == WI_I || inftype == WI_TARI || inftype == WI_EBA || inftype == WI_E || inftype == WI_KU || inftype == WI_SA || inftype == WI_NA || inftype == WI_KYA || inftype == WI_SURU && inf->inf == WI_SURU || inftype == WI_ZU || inftype == WI_OU || inftype == WI_SOU || inftype == WI_TENASAI || inftype == WI_SURU && lastvar == WT_VERB_U))
        return inf;

    //return if no -u inflection found before tagaru
    if (inf && inftype == WI_TAGARU && lastvar != WT_VERB_U)
        return inf;

    //return if -u inflection found before these inflections
    if ((inftype == WI_RERU || inftype == WI_SERU || inftype == WI_SERARERU || inftype == WI_ERU || inftype == WI_RARERU || inftype == WI_TERU) && lastvar == WT_VERB_U)
        return inf;

    types |= 1 << type;

    if ((inf = l->Add(s, type, inf, inftype)) != 0 && /*type != 4 &&*/(type == 1 || type == 6 || type == 2 || type == 9 || type == 21) && (inftype != WI_I || type != 6))
        DeInflect(l, s, types, inf);

    return inf;
}

//----- DEINFLECT
//type is the same as the types for a word (eg. verb)
//inf is the list of previous inflections. when inf is NULL it means this is the first run
void DeInflect(TWordInfList *l, UnicodeString s, int &type, TInfList *inf)
{
    int slen = s.Length() - (s[s.Length()] == L'+' ? 1 : 0);
    if (!inf)//the first run
                DeAdjectivize(s.SubString(1, slen), type, l);

    //check na-adjective - only for first (not recursed) run
    if (!inf && (s[s.Length()] == L'な' || s[s.Length()] == L'に' || s[s.Length()] == L'で'))//な = 'na'  に = 'ni'  に = 'de'
                _add_variant(l, s.SubString(1, s.Length() - 1), 4, type, inf, (s[s.Length()] == L'な' ? WI_NA : s[s.Length()] == L'に' ? WI_KU : WI_TE));

    //iku-verb

    /*0: kimasu, 1: kimashita, 2: kimasen, 3: kimashou, 4: kanai, 5: saserareru, 6: rareru,
     * 7: saseru, 8: ki, 9: tai, 10: tagaru, 11: sou, 12: te, 13: ta, 14: tara,
     * 15: tari, 16: keba, 17: keru, 18: ke, 19: kou, 20: teru, 21: teiru, 22: nakuya, 23: nakya,
     * 24: chau, 25: naide, 26: zu, 27: nu, 28: sareru, 29: chimau*/
    const wchar_t *iku[] =
    {
        L"きます", L"きました", L"きません", L"きましょう",
        L"かない", L"かせられる", L"かれる", L"かせる",
        L"き", L"きたい", L"きたがる", L"きそう",
        L"って", L"った", L"ったら", L"ったり",
        L"けば", L"ける", L"け", L"こう",
        L"ってる", L"っている", L"かなくや", L"かなきゃ",
        L"っちゃう", L"かないで", L"かず", L"かぬ",
        L"かされる", L"っちまう", L"ってなさい", L"っておく",
        L"っとく"
    };

    const byte iku_inf[] =
    {
        WI_MASU, WI_MASHITA, WI_MASEN, WI_MASHOU,
        WI_NAI, WI_SERARERU, WI_RERU, WI_SERU,
        WI_I, WI_TAI, WI_TAGARU, WI_SOU,
        WI_TE, WI_TA, WI_TARA, WI_TARI,
        WI_EBA, WI_ERU, WI_E, WI_OU,
        WI_TERU, WI_TERU, WI_KYA, WI_KYA,
        WI_CHAU, WI_NAIDE, WI_ZU, WI_NU,
        WI_SERARERU, WI_CHAU, WI_TENASAI, WI_TEOKU,
        WI_TEOKU
    };

    for (int i = 0; i < (int)(sizeof(iku) / sizeof(wchar_t*)); i++)
    { //304f = ku
        if (s.SubString(MAX(1, s.Length() - wcslen(iku[i]) + 1), wcslen(iku[i])) == iku[i])
            _add_variant(l, s.SubString(1, s.Length() - wcslen(iku[i])) + L"く", 18, type, inf, iku_inf[i]);
    }

    /*0: shimasu, 1: simasita, 2: simasen, 3: simasou, 4: sinai, 5: saserareru, 6: sareru,
     * 7: saseru, 8: shi, 9: sitai, 10: shitagaru, 11: sisou, 12: site, 13: sita,
     * 14: sitara, 15: sitari, 16: sureba, 17: siyo, 18: seyo, 19: siyou, 20: suru,
     * 21: teru, 22: teiru, 23: nakuya, 24: nakya, 25: shichau, 26: shinaide, 27: sezu, 28: senu,
     * 29: sareru, 30: chimau*/
    const wchar_t *suru[] =
    {
        L"します", L"しました", L"しません", L"しましょう",
        L"しない", L"させられる", L"される", L"させる",
        L"し", L"したい", L"したがる", L"しそう",
        L"して", L"した", L"したら", L"したり",
        L"すれば", L"しよ", L"せよ", L"しよう",
        L"する", L"してる", L"している", L"しなくや",
        L"しなきゃ", L"しちゃう", L"しないで", L"せず",
        L"せぬ", L"される", L"しちまう", L"してなさい",
        L"しておく", L"しとく", L"すりゃ"
    };

    const byte suru_inf[] =
    {
        WI_MASU, WI_MASHITA, WI_MASEN, WI_MASHOU,
        WI_NAI, WI_SERARERU, WI_RERU, WI_SERU,
        WI_I, WI_TAI, WI_TAGARU, WI_SOU,
        WI_TE, WI_TA, WI_TARA, WI_TARI,
        WI_EBA, WI_E, WI_E, WI_OU,
        WI_SURU, WI_TERU, WI_TERU, WI_KYA,
        WI_KYA, WI_CHAU, WI_NAIDE, WI_ZU,
        WI_NU, WI_SERARERU, WI_CHAU, WI_TENASAI,
        WI_TEOKU, WI_TEOKU, WI_EBA
    };

    for (int i = 0; i < (int)(sizeof(suru) / sizeof(wchar_t*)); i++)
    { //する = suru
        if (s.SubString(MAX(1, s.Length() - wcslen(suru[i]) + 1), wcslen(suru[i])) == suru[i])
        {
            _add_variant(l, s.SubString(1, s.Length() - wcslen(suru[i])) + L"する", 14, type, inf, suru_inf[i]);
            _add_variant(l, s.SubString(1, s.Length() - wcslen(suru[i])), 1, type, inf, suru_inf[i]); //suru + noun
        }
    }

    /*0: masu, 1: mashita, 2: masen, 3: mashou, 4: nai, 5: saserareru, 6: rareru,
     * 7: saseru, 8: tai, 9: tagaru, 10: sou, 11: te, 12: ta, 13: tara,
     * 14: tari, 15: reba, ??? ua mint 5., 16: ro, 17: you, 18: teru, 19: teiru,
     * 20: nakuya, 21: nakya, 22: chau, 23: naide, 24: zu, 25: nu, 27: chimau, 28: eru*/
    const wchar_t *kuru[] =
    {
        L"きます", L"きました", L"きません", L"きましょう",
        L"こない", L"こさせられる", L"こられる", L"こさせる",
        L"きたい", L"きたがる", L"きそう", L"きて",
        L"きた", L"きたら", L"きたり", L"くれば",
        L"こい", L"こよう", L"きてる", L"きている",
        L"こなくや", L"こなきゃ", L"きちゃう", L"こないで",
        L"こず", L"こぬ", L"きちまう", L"これる",
        L"きてなさい", L"きておく", L"きとく", L"くりゃ"
    };

    const byte kuru_inf[] =
    {
        WI_MASU, WI_MASHITA, WI_MASEN, WI_MASHOU,
        WI_NAI, WI_SERARERU, WI_RARERU, WI_SERU,
        WI_TAI, WI_TAGARU, WI_SOU, WI_TE,
        WI_TA, WI_TARA, WI_TARI, WI_EBA,
        WI_E, WI_OU, WI_TERU, WI_TERU,
        WI_KYA, WI_KYA, WI_CHAU, WI_NAIDE,
        WI_ZU, WI_NU, WI_CHAU, WI_ERU,
        WI_TENASAI, WI_TEOKU, WI_TEOKU, WI_EBA
    };

    if (s.Length() > 1)
    {
        for (int i = 0; i < (int)(sizeof(kuru) / sizeof(wchar_t*)); i++)
        { //308b = ru
            int len = wcslen(kuru[i]);
            if (s.Length() - len + 1 > 0 && s.SubString(MAX(1, s.Length() - len + 2), len - 1) == kuru[i] + 1)
            {
                if (s[s.Length() - len + 1] == L'来' || s[s.Length() - len + 1] == L'來')
                    _add_variant(l, s.SubString(1, s.Length() - len + 1) + L"る", 20, type, inf, kuru_inf[i]);
                else if (s[s.Length() - len + 1] == kuru[i][0])
                    _add_variant(l, s.SubString(1, s.Length() - len) + L"くる", 20, type, inf, kuru_inf[i]);
            }
        }
    }

    /*0: kimasu, 1: kimashita, 2: kimasen, 3: kimashou, 4: kanai, 5: saserareru, 6: rareru,
     * 7: saseru, 8: ki, 9: tai, 10: tagaru, 11: sou, 12: te, 13: ta, 14: tara,
     * 15: tari, 16: keba, 17: keru, 18: ke, 19: kou, 20: teru, 21: teiru, 22: nakuya, 23: nakya,
     * 24: chau, 25: naide, 26: wazu, 27: wanu, 28: sareru, 29: chimau*/
    const wchar_t *u[] =
    {
        L"います", L"いました", L"いません", L"いましょう",
        L"わない", L"わせられる", L"われる", L"わせる",
        L"い", L"いたい", L"いたがる", L"いそう",
        L"って", L"った", L"ったら", L"ったり",
        L"えば", L"える", L"え", L"おう",
        L"ってる", L"っている", L"わなくや", L"わなきゃ",
        L"っちゃう", L"わないで", L"わず", L"わぬ",
        L"わされる", L"っちまう", L"ってなさい", L"っておく",
        L"っとく"
    };

    const byte u_inf[] =
    {
        WI_MASU, WI_MASHITA, WI_MASEN, WI_MASHOU,
        WI_NAI, WI_SERARERU, WI_RERU, WI_SERU,
        WI_I, WI_TAI, WI_TAGARU, WI_SOU,
        WI_TE, WI_TA, WI_TARA, WI_TARI,
        WI_EBA, WI_ERU, WI_E, WI_OU,
        WI_TERU, WI_TERU, WI_KYA, WI_KYA,
        WI_CHAU, WI_NAIDE, WI_ZU, WI_NU,
        WI_SERARERU, WI_CHAU, WI_TENASAI, WI_TEOKU,
        WI_TEOKU
    };

    for (int i = 0; i < (int)(sizeof(u) / sizeof(wchar_t*)); i++)
    { //3046 = u
        //if (!wcscmp(u[i],L"い") && type == WT_ADJ) // don't do this for already known adjectives
        //continue; else
        if (s.SubString(MAX(1, s.Length() - wcslen(u[i]) + 1), wcslen(u[i])) == u[i])
            _add_variant(l, s.SubString(1, s.Length() - wcslen(u[i])) + L"う", 2, type, inf, u_inf[i]);
    }

    /*0: kimasu, 1: kimashita, 2: kimasen, 3: kimashou, 4: kanai, 5: saserareru, 6: rareru,
     * 7: saseru, 8: ki, 9: tai, 10: tagaru, 11: sou, 12: te, 13: ta, 14: tara,
     * 15: tari, 16: keba, 17: keru, 18: ke, 19: kou, 20: teru, 21: teiru, 22: nakuya, 23: nakya,
     * 24: chau, 25: naide, 26: kazu, 27: kanu, 28: sareru, 29: chimau*/
    const wchar_t *ku[] =
    {
        L"きます", L"きました", L"きません", L"きましょう",
        L"かない", L"かせられる", L"かれる", L"かせる",
        L"き", L"きたい", L"きたがる", L"きそう",
        L"いて", L"いた", L"いたら", L"いたり",
        L"けば", L"ける", L"け", L"こう",
        L"いてる", L"いている", L"かなくや", L"かなきゃ",
        L"いちゃう", L"かないで", L"かず", L"かぬ",
        L"かされる", L"いちまう", L"いてなさい", L"いておく",
        L"いとく"
    };

    const byte ku_inf[] =
    {
        WI_MASU, WI_MASHITA, WI_MASEN, WI_MASHOU,
        WI_NAI, WI_SERARERU, WI_RERU, WI_SERU,
        WI_I, WI_TAI, WI_TAGARU, WI_SOU,
        WI_TE, WI_TA, WI_TARA, WI_TARI,
        WI_EBA, WI_ERU, WI_E, WI_OU,
        WI_TERU, WI_TERU, WI_KYA, WI_KYA,
        WI_CHAU, WI_NAIDE, WI_ZU, WI_NU,
        WI_SERARERU, WI_CHAU, WI_TENASAI, WI_TEOKU,
        WI_TEOKU
    };

    for (int i = 0; i < (int)(sizeof(ku) / sizeof(wchar_t*)); i++)
    { //304f = ku
        if (s.SubString(MAX(1, s.Length() - wcslen(ku[i]) + 1), wcslen(ku[i])) == ku[i])
            _add_variant(l, s.SubString(1, s.Length() - wcslen(ku[i])) + L"く", 2, type, inf, ku_inf[i]);
    }

    /*0: kimasu, 1: kimashita, 2: kimasen, 3: kimashou, 4: kanai, 5: saserareru, 6: rareru,
     * 7: saseru, 8: ki, 9: tai, 10: tagaru, 11: sou, 12: te, 13: ta, 14: tara,
     * 15: tari, 16: keba, 17: keru, 18: ke, 19: kou, 20: teru, 21: teiru, 22: nakuya, 23: nakya,
     * 24: chau, 25: naide, 26: gazu, 27: ganu, 28: sareru, 29: chimau*/
    const wchar_t *gu[] =
    {
        L"ぎます", L"ぎました", L"ぎません", L"ぎましょう",
        L"がない", L"がせられる", L"がれる", L"がせる",
        L"ぎ", L"ぎたい", L"ぎたがる", L"ぎそう",
        L"いで", L"いだ", L"いだら", L"いだり",
        L"げば", L"げる", L"げ", L"ごう",
        L"いでる", L"いでいる", L"がなくや", L"がなきゃ",
        L"いじゃう", L"がないで", L"がず", L"がぬ",
        L"がされる", L"いじまう", L"いでなさい", L"いでおく",
        L"いどく"
    };

    const byte gu_inf[] =
    {
        WI_MASU, WI_MASHITA, WI_MASEN, WI_MASHOU,
        WI_NAI, WI_SERARERU, WI_RERU, WI_SERU,
        WI_I, WI_TAI, WI_TAGARU, WI_SOU,
        WI_TE, WI_TA, WI_TARA, WI_TARI,
        WI_EBA, WI_ERU, WI_E, WI_OU,
        WI_TERU, WI_TERU, WI_KYA, WI_KYA,
        WI_CHAU, WI_NAIDE, WI_ZU, WI_NU,
        WI_SERARERU, WI_CHAU, WI_TENASAI, WI_TEOKU,
        WI_TEOKU
    };

    for (int i = 0; i < (int)(sizeof(gu) / sizeof(wchar_t*)); i++)
    { //3050 = gu
        if (s.SubString(MAX(1, s.Length() - wcslen(gu[i]) + 1), wcslen(gu[i])) == gu[i])
            _add_variant(l, s.SubString(1, s.Length() - wcslen(gu[i])) + L"ぐ", 2, type, inf, gu_inf[i]);
    }

    /*0: kimasu, 1: kimashita, 2: kimasen, 3: kimashou, 4: kanai, 5: saserareru, 6: rareru,
     * 7: saseru, 8: ki, 9: tai, 10: tagaru, 11: sou, 12: te, 13: ta, 14: tara,
     * 15: tari, 16: keba, 17: keru, 18: ke, 19: kou, 20: teru, 21: teiru, 22: nakuya, 23: nakya,
     * 24: chau, 25: naide, 26: sazu, 27: sanu, 28: chimau*/
    const wchar_t *su[] =
    {
        L"します", L"しました", L"しません", L"しましょう",
        L"さない", L"させられる", L"される", L"させる",
        L"し", L"したい", L"したがる", L"しそう",
        L"して", L"した", L"したら", L"したり",
        L"せば", L"せる", L"せ", L"そう",
        L"してる", L"している", L"さなくや", L"さなきゃ",
        L"しちゃう", L"さないで", L"さず", L"さぬ",
        L"しちまう", L"してなさい", L"しておく", L"しとく"
    };

    const byte su_inf[] =
    {
        WI_MASU, WI_MASHITA, WI_MASEN, WI_MASHOU,
        WI_NAI, WI_SERARERU, WI_RERU, WI_SERU,
        WI_I, WI_TAI, WI_TAGARU, WI_SOU,
        WI_TE, WI_TA, WI_TARA, WI_TARI,
        WI_EBA, WI_ERU, WI_E, WI_OU,
        WI_TERU, WI_TERU, WI_KYA, WI_KYA,
        WI_CHAU, WI_NAIDE, WI_ZU, WI_NU,
        WI_CHAU, WI_TENASAI, WI_TEOKU, WI_TEOKU
    };

    for (int i = 0; i < (int)(sizeof(su) / sizeof(wchar_t*)); i++)
    { //3059 = su
        if (s.SubString(MAX(1, s.Length() - wcslen(su[i]) + 1), wcslen(su[i])) == su[i])
            _add_variant(l, s.SubString(1, s.Length() - wcslen(su[i])) + L"す", 2, type, inf, su_inf[i]);
    }

    /*0: kimasu, 1: kimashita, 2: kimasen, 3: kimashou, 4: kanai, 5: saserareru, 6: rareru,
     * 7: saseru, 8: ki, 9: tai, 10: tagaru, 11: sou, 12: te, 13: ta, 14: tara,
     * 15: tari, 16: keba, 17: keru, 18: ke, 19: kou, 20: teru, 21: teiru, 22: nakuya, 23: nakya,
     * 24: chau, 25: naide, 26: tazu, 27: tanu, 28: sareru, 29: chimau*/
    const wchar_t *tu[] =
    {
        L"ちます", L"ちました", L"ちません", L"ちましょう",
        L"たない", L"たせられる", L"たれる", L"たせる",
        L"ち", L"ちたい", L"ちたがる", L"ちそう",
        L"って", L"った", L"ったら", L"ったり",
        L"てば", L"てる", L"て", L"とう",
        L"ってる", L"っている", L"たなくや", L"たなきゃ",
        L"っちゃう", L"たないで", L"たず", L"たぬ",
        L"たされる", L"っちまう", L"ってなさい", L"っておく",
        L"っとく"
    };

    const byte tu_inf[] =
    {
        WI_MASU, WI_MASHITA, WI_MASEN, WI_MASHOU,
        WI_NAI, WI_SERARERU, WI_RERU, WI_SERU,
        WI_I, WI_TAI, WI_TAGARU, WI_SOU,
        WI_TE, WI_TA, WI_TARA, WI_TARI,
        WI_EBA, WI_ERU, WI_E, WI_OU,
        WI_TERU, WI_TERU, WI_KYA, WI_KYA,
        WI_CHAU, WI_NAIDE, WI_ZU, WI_NU,
        WI_SERARERU, WI_CHAU, WI_TENASAI, WI_TEOKU,
        WI_TEOKU
    };

    for (int i = 0; i < (int)(sizeof(tu) / sizeof(wchar_t*)); i++)
    { //3064 = tu
        if (s.SubString(MAX(1, s.Length() - wcslen(tu[i]) + 1), wcslen(tu[i])) == tu[i])
            _add_variant(l, s.SubString(1, s.Length() - wcslen(tu[i])) + L"つ", 2, type, inf, tu_inf[i]);
    }

    /*0: kimasu, 1: kimashita, 2: kimasen, 3: kimashou, 4: kanai, 5: saserareru, 6: rareru,
     * 7: saseru, 8: ki, 9: tai, 10: tagaru, 11: sou, 12: te, 13: ta, 14: tara,
     * 15: tari, 16: keba, 17: keru, 18: ke, 19: kou, 20: teru, 21: teiru, 22: nakuya, 23: nakya,
     * 24: chau, 25: naide, 26: nazu, 27: nanu, 28: sareru, 29: chimau*/
    const wchar_t *nu[] =
    {
        L"にます", L"にました", L"にません", L"にましょう",
        L"なない", L"なせられる", L"なれる", L"なせる",
        L"に", L"にたい", L"にたがる", L"にそう",
        L"んで", L"んだ", L"んだら", L"んだり",
        L"ねば", L"ねる", L"ね", L"のう",
        L"んでる", L"んでいる", L"ななくや", L"ななきゃ",
        L"んじゃう", L"なないで", L"なず", L"なぬ",
        L"なされる", L"んじまう", L"んでなさい", L"んでおく",
        L"んどく"
    };

    const byte nu_inf[] =
    {
        WI_MASU, WI_MASHITA, WI_MASEN, WI_MASHOU,
        WI_NAI, WI_SERARERU, WI_RERU, WI_SERU,
        WI_I, WI_TAI, WI_TAGARU, WI_SOU,
        WI_TE, WI_TA, WI_TARA, WI_TARI,
        WI_EBA, WI_ERU, WI_E, WI_OU,
        WI_TERU, WI_TERU, WI_KYA, WI_KYA,
        WI_CHAU, WI_NAIDE, WI_ZU, WI_NU,
        WI_SERARERU, WI_CHAU, WI_TENASAI, WI_TEOKU,
        WI_TEOKU
    };

    for (int i = 0; i < (int)(sizeof(nu) / sizeof(wchar_t*)); i++)
    { //306c = nu
        if (s.SubString(MAX(1, s.Length() - wcslen(nu[i]) + 1), wcslen(nu[i])) == nu[i])
            _add_variant(l, s.SubString(1, s.Length() - wcslen(nu[i])) + L"ぬ", 2, type, inf, nu_inf[i]);
    }

    /*0: kimasu, 1: kimashita, 2: kimasen, 3: kimashou, 4: kanai, 5: saserareru, 6: rareru,
     * 7: saseru, 8: ki, 9: tai, 10: tagaru, 11: sou, 12: te, 13: ta, 14: tara,
     * 15: tari, 16: keba, 17: keru, 18: ke, 19: kou, 20: teru, 21: teiru, 22: nakuya, 23: nakya,
     * 24: chau, 25: naide, 26: zu, 27: nu, 28: sareru, 29: chimau*/
    const wchar_t *bu[] =
    {
        L"びます", L"びました", L"びません", L"びましょう",
        L"ばない", L"ばせられる", L"ばれる", L"ばせる",
        L"び", L"びたい", L"びたがる", L"びそう",
        L"んで", L"んだ", L"んだら", L"んだり",
        L"べば", L"べる", L"べ", L"ぼう",
        L"んでる", L"んでいる", L"ばなくや", L"ばなきゃ",
        L"んじゃう", L"ばないで", L"ばず", L"ばぬ",
        L"ばされる", L"んじまう", L"んでなさい", L"んでおく",
        L"んどく"
    };

    const byte bu_inf[] =
    {
        WI_MASU, WI_MASHITA, WI_MASEN, WI_MASHOU,
        WI_NAI, WI_SERARERU, WI_RERU, WI_SERU,
        WI_I, WI_TAI, WI_TAGARU, WI_SOU,
        WI_TE, WI_TA, WI_TARA, WI_TARI,
        WI_EBA, WI_ERU, WI_E, WI_OU,
        WI_TERU, WI_TERU, WI_KYA, WI_KYA,
        WI_CHAU, WI_NAIDE, WI_ZU, WI_NU,
        WI_SERARERU, WI_CHAU, WI_TENASAI, WI_TEOKU,
        WI_TEOKU
    };

    for (int i = 0; i < (int)(sizeof(bu) / sizeof(wchar_t*)); i++)
    { //3076 = bu
        if (s.SubString(MAX(1, s.Length() - wcslen(bu[i]) + 1), wcslen(bu[i])) == bu[i])
            _add_variant(l, s.SubString(1, s.Length() - wcslen(bu[i])) + L"ぶ", 2, type, inf, bu_inf[i]);
    }

    /*0: kimasu, 1: kimashita, 2: kimasen, 3: kimashou, 4: kanai, 5: saserareru, 6: rareru,
     * 7: saseru, 8: ki, 9: tai, 10: tagaru, 11: sou, 12: te, 13: ta, 14: tara,
     * 15: tari, 16: keba, 17: keru, 18: ke, 19: kou, 20: teru, 21: teiru, 22: nakuya, 23: nakya,
     * 24: chau, 25: naide, 26: zu, 27: nu, 28: sareru, 29: chimau*/
    const wchar_t *mu[] =
    {
        L"みます", L"みました", L"みません", L"みましょう",
        L"まない", L"ませられる", L"まれる", L"ませる",
        L"み", L"みたい", L"みたがる", L"みそう",
        L"んで", L"んだ", L"んだら", L"んだり",
        L"めば", L"める", L"め", L"もう",
        L"んでる", L"んでいる", L"まなくや", L"まなきゃ",
        L"んじゃう", L"まないで", L"まず", L"まぬ",
        L"まされる", L"んじまう", L"んでなさい", L"んでおく",
        L"んどく"
    };

    const byte mu_inf[] =
    {
        WI_MASU, WI_MASHITA, WI_MASEN, WI_MASHOU,
        WI_NAI, WI_SERARERU, WI_RERU, WI_SERU,
        WI_I, WI_TAI, WI_TAGARU, WI_SOU,
        WI_TE, WI_TA, WI_TARA, WI_TARI,
        WI_EBA, WI_ERU, WI_E, WI_OU,
        WI_TERU, WI_TERU, WI_KYA, WI_KYA,
        WI_CHAU, WI_NAIDE, WI_ZU, WI_NU,
        WI_SERARERU, WI_CHAU, WI_TENASAI, WI_TEOKU,
        WI_TEOKU
    };

    for (int i = 0; i < (int)(sizeof(mu) / sizeof(wchar_t*)); i++)
    { //3080 = mu
        if (s.SubString(MAX(1, s.Length() - wcslen(mu[i]) + 1), wcslen(mu[i])) == mu[i])
            _add_variant(l, s.SubString(1, s.Length() - wcslen(mu[i])) + L"む", 2, type, inf, mu_inf[i]);
    }

    /*0: kimasu, 1: kimashita, 2: kimasen, 3: kimashou, 4: kanai, 5: saserareru, 6: rareru,
     * 7: saseru, 8: ki, 9: tai, 10: tagaru, 11: sou, 12: te, 13: ta, 14: tara,
     * 15: tari, 16: keba, 17: keru, 18: ke, 19: kou, 20: teru, 21: teiru, 22: nakuya, 23: nakya,
     * 24: chau, 25: naide, 26: zu, 27: nu, 28: sareru, 29: chimau*/
    const wchar_t *r_u[] =
    {
        L"ります", L"りました", L"りません", L"りましょう",
        L"らない", L"らせられる", L"られる", L"らせる",
        L"り", L"りたい", L"りたがる", L"りそう",
        L"って", L"った", L"ったら", L"ったり",
        L"れば", L"れる", L"れ", L"ろう",
        L"ってる", L"っている", L"らなくや", L"らなきゃ",
        L"っちゃう", L"らないで", L"らず", L"らぬ",
        L"らされる", L"っちまう", L"ってなさい", L"っておく",
        L"っとく"
    };

    const byte r_u_inf[] =
    {
        WI_MASU, WI_MASHITA, WI_MASEN, WI_MASHOU,
        WI_NAI, WI_SERARERU, WI_RERU, WI_SERU,
        WI_I, WI_TAI, WI_TAGARU, WI_SOU,
        WI_TE, WI_TA, WI_TARA, WI_TARI,
        WI_EBA, WI_ERU, WI_E, WI_OU,
        WI_TERU, WI_TERU, WI_KYA, WI_KYA,
        WI_CHAU, WI_NAIDE, WI_ZU, WI_NU,
        WI_SERARERU, WI_CHAU, WI_TENASAI, WI_TEOKU,
        WI_TEOKU
    };

    for (int i = 0; i < (int)(sizeof(r_u) / sizeof(wchar_t*)); i++)
    { //308B = r_u
        if (s.SubString(MAX(1, s.Length() - wcslen(r_u[i]) + 1), wcslen(r_u[i])) == r_u[i])
            _add_variant(l, s.SubString(1, s.Length() - wcslen(r_u[i])) + L"る", 2, type, inf, r_u_inf[i]);
    }

    //check ru verb

    /*0: masu, 1: mashita, 2: masen, 3: mashou, 4: nai, 5: saserareru, 6: rareru,
     * 7: saseru, 8: tai, 9: tagaru, 10: sou, 11: te, 12: ta, 13: tara,
     * 14: tari, 15: reba, ??? ua mint 5., 16: ro, 17: yo, 18: you, 19: teru, 20: teiru,
     * 21: nakuya, 22: nakya, 23: chau, 24: naide, 25: zu, 26: nu, 27: reru,
     * 28: chimau*/
    const wchar_t *ru[] =
    {
        L"ます", L"ました", L"ません", L"ましょう",
        L"ない", L"させられる", L"られる", L"させる",
        L"たい", L"たがる", L"そう", L"て",
        L"た", L"たら", L"たり", L"れば",
        L"ろ", L"よ", L"よう", L"てる",
        L"ている", L"なくや", L"なきゃ", L"ちゃう",
        L"ないで", L"ず", L"ぬ", L"れる",
        L"ちまう", L"てなさい", L"ておく", L"とく",
        L"りゃ" };
    const byte ru_inf[] =
    {
        WI_MASU, WI_MASHITA, WI_MASEN, WI_MASHOU,
        WI_NAI, WI_SERARERU, WI_RARERU, WI_SERU,
        WI_TAI, WI_TAGARU, WI_SOU, WI_TE,
        WI_TA, WI_TARA, WI_TARI, WI_EBA,
        WI_E, WI_E, WI_OU, WI_TERU,
        WI_TERU, WI_KYA, WI_KYA, WI_CHAU,
        WI_NAIDE, WI_ZU, WI_NU, WI_ERU,
        WI_CHAU, WI_TENASAI, WI_TEOKU, WI_TEOKU,
        WI_EBA };
    if (!inf)
        _add_variant(l, s + L"る", 6, type, inf, WI_I); //noun base when no ru is present

    for (int i = 0; i < (int)(sizeof(ru) / sizeof(wchar_t*)); i++)
    { //308b = ru
        int len = wcslen(ru[i]);
        if (s.SubString(MAX(1, s.Length() - len + 1), len) == ru[i])
            _add_variant(l, s.SubString(1, s.Length() - len) + L"る", 6, type, inf, ru_inf[i]);
    }
}
//---------------------------------------------------------------------------
//s - the string to have all presumed adjective inflections removed
//types - type of words that can be adjective or verb etc.
//l - the results
void DeAdjectivize(UnicodeString s, int &types, TWordInfList *l)
{
    //ku, kunai, katta, kattara, kattari, kute, karou, kereba, sa, sou
    UnicodeString adjw[] = { L"く", L"くない", L"かった", L"かったら", L"かったり", L"くて", L"かろう", L"ければ", L"けりゃ", L"さ", L"そう" };
    const byte adj_inf[] = { WI_KU, WI_NAI, WI_TA, WI_TARA, WI_TARI, WI_TE, WI_OU, WI_EBA, WI_EBA, WI_SA, WI_SOU };

    int len = s.Length();
    int n;
    bool changed;

    TInfList *inf = NULL, *inf_yoi = NULL;

    do
    {
        changed = false;
        for (int i = 0; i < (int)(sizeof(adjw) / sizeof(UnicodeString)) && len > 1; i++)
        {
            n = adjw[i].Length();
            if (len >= n && s.SubString(len - n + 1, n) == adjw[i])
            {
                len -= n - 1;
                if (len <= 1 || s[len - 1] == L'っ' || s[len - 1] == L'ッ')
                    continue;
                s = s.SubString(1, len - 1) + L'い';
                if (s.Length() >= 5 && s.SubString(s.Length() - 4, 5) == L"かっこいい" || s.Length() >= 4 && s.SubString(s.Length() - 3, 4) == L"かわいい" && adj_inf[i] == WI_SOU || s.Length() >= 5 && s.SubString(s.Length() - 4, 5) == L"きもちいい" || s.Length() >= 6 && s.SubString(s.Length() - 5, 6) == L"ちょうどいい" || s.Length() >= 6 && s.SubString(s.Length() - 5, 6) == L"いごこちいい" || s.Length() >= 5 && s.SubString(s.Length() - 4, 5) == L"あたまいい" || s.Length() >= 5 && s.SubString(s.Length() - 4, 5) == L"かっこいい" || s.Length() > 3 && s.SubString(s.Length() - 2, 3) == L"のいい" || s.Length() > 3 && s.SubString(s.Length() - 2, 3) == L"がいい" || s.Length() == 2 && s.SubString(s.Length() - 1, 2) == L"いい")
                    break;

                if (adj_inf[i] == WI_SOU && s.Length() > 3 && (s.SubString(s.Length() - 2, 3) == L"よさい" || s.SubString(s.Length() - 2, 3) == L"なさい"))
                {
                    bool na = s[s.Length() - 2] == L'な';
                    inf = _add_variant(l, s.SubString(1, s.Length() - 2) + L"い", (na ? WT_COUNT : 9), types, inf, adj_inf[i]);
                    inf_yoi = _add_variant(l, s.SubString(1, s.Length() - 3) + L"いい", 9, types, inf_yoi, adj_inf[i]);
                    s = s.SubString(1, s.Length() - 2) + L"い";
                    len--;
                    changed = true;
                    continue;
                }

                inf = _add_variant(l, s, 9, types, inf, adj_inf[i]);

                if (s.Length() >= 5 && s.SubString(s.Length() - 4, 5) == L"かっこよい" || s.Length() >= 5 && s.SubString(s.Length() - 4, 5) == L"きもちよい" || s.Length() >= 6 && s.SubString(s.Length() - 5, 6) == L"ちょうどよい" || s.Length() >= 6 && s.SubString(s.Length() - 5, 6) == L"いごこちよい" || s.Length() >= 5 && s.SubString(s.Length() - 4, 5) == L"あたまよい" || s.Length() >= 5 && s.SubString(s.Length() - 4, 5) == L"かっこよい" || s.Length() > 3 && s.SubString(s.Length() - 2, 3) == L"のよい" || s.Length() > 3 && s.SubString(s.Length() - 2, 3) == L"がよい" || s.Length() == 2 && s.SubString(s.Length() - 1, 2) == L"よい")
                {
                    inf_yoi = _add_variant(l, s.SubString(1, s.Length() - 2) + L"いい", 9, types, inf_yoi, adj_inf[i]);
                }
                changed = true;
            }
        }
    }
    while (changed);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(195);
#endif

