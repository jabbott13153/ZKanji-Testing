//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "zrads.h"
#include "romajizer.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(212);
#endif

//---------------------------------------------------------------------------
TRadicalList::TRadicalSearchTree::TRadicalSearchTree(TRadicalList *aowner) : fowner(aowner), inherited(false, false)
{
}
//---------------------------------------------------------------------------
UnicodeString TRadicalList::TRadicalSearchTree::DoGetWord(int index)
{
    byte name = (index & 0xff000000) >> 24;
    index = index & 0xffffff;
    TKanjiRadical *rad = fowner->Items[index];

    int len = wcslen(rad->names);
    wchar_t *cc = NULL;
    int pos = 0;
    try
    {
        cc = new wchar_t[len + 1];
        memcpy(cc, rad->names, len*sizeof(wchar_t));
        cc[len] = 0;
        int j = 0;
        for (int ix = 0; ix <= len; ++ix)
        {
            if (ix == len || cc[ix] == L' ')
            {
                cc[ix] = 0;
                if (len - pos)
                {
                    if (j == name)
                    {
                        delete[] cc;
                        return Romanize(cc + pos);
                    }
                    j++;
                }
                if (ix < len)
                    cc[ix] = ' ';
                pos = ix + 1;
            }
        }
    }
    catch (...)
    {
        ;
    }
    delete[] cc;

    THROW(L"We shouldn't be here!");
}
//---------------------------------------------------------------------------
void TRadicalList::TRadicalSearchTree::ExpandTree(UnicodeString str, int index, byte name)
{
    index = index | (name << 24);
    inherited::Expand(str, index);
}
//---------------------------------------------------------------------------
//returns the element number in list under the nod, not the elements themselves.
//WARNING: c MUST be lowercase or the function won't find anything!
void TRadicalList::TRadicalSearchTree::FindElement(const wchar_t *c, TIntegerList *list)
{
    FindLines(c, false, list);
    for (int ix = 0; ix < list->Count; ++ix)
        list->Items[ix] = list->Items[ix] & 0xffffff;
    list->Sort();
    for (int ix = list->Count - 1; ix > 0; --ix)
        if (list->Items[ix] == list->Items[ix - 1])
            list->Delete(ix);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TRadicalList::TRadicalList() : inherited(), fgroups(NULL)
{
    ntree = new TRadicalSearchTree(this);
}
//---------------------------------------------------------------------------
TRadicalList::~TRadicalList()
{
    delete ntree;
    Clear();
    Reset();
}
//---------------------------------------------------------------------------
void TRadicalList::FreeItem(TKanjiRadical *rad, int ix)
{
    if (rad->names)
        delete[] rad->names;
    if (rad->list)
        delete[] rad->list;
    delete rad;
}
//---------------------------------------------------------------------------
/*
TKanjiRadical* TRadicalList::get_item(int ix)
{
    return ((TKanjiRadical*)T-List::Items[ix]);
}
*/
//---------------------------------------------------------------------------
void TRadicalList::ImportAdd(wchar_t *w)
{
    wchar_t *c = wcstok(w, L" ");
    TKanjiRadical *rad = new TKanjiRadical;

    rad->radical = StrToInt(c);
    c = wcstok(NULL, L" ");
    rad->ch = c[0];
    c = wcstok(NULL, L" ");
    rad->strokes = StrToInt(c);
    c += wcslen(c) + 1;
    rad->list = NULL;

    int len = wcslen(c);
    wchar_t *cc = new wchar_t[len + 1];
    int pos = 0;
    try
    {
        memcpy(cc, c, len * sizeof(wchar_t));
        cc[len] = 0;
        int j = 0;
        for (int ix = 0; ix <= len; ++ix)
        {
            if (ix == len || cc[ix] == L' ')
            {
                cc[ix] = 0;
                if (len - pos)
                {
                    ntree->ExpandTree(Romanize(cc + pos).c_str(), Count, j);
                    j++;
                }
                if (ix < len)
                    cc[ix] = ' ';
                pos = ix + 1;
            }
        }
    }
    catch (...)
    {
        ;
    }
    delete[] cc;

    rad->names = new wchar_t[len + 1];
    memcpy(rad->names, c, len*sizeof(wchar_t));
    rad->names[len] = 0;

    Add(rad);
}
//---------------------------------------------------------------------------
//l recieves data! Create it before calling this function!
void TRadicalList::Filter(UnicodeString s, byte strokes, TIntegerList *l, bool exact)
{
    bool del;

    if (s.Length())
        ntree->FindElement(Romanize(s.c_str()).c_str(), l);
    else if (strokes)
        for (int i = 0; i < Count; i++)
            l->Add(i);

    for (int i = l->Count - 1; i >= 0; i--)
    {
        TKanjiRadical *r = Items[l->Items[i]];

        del = (strokes && r->strokes != strokes);

        if (!del && s.Length())
        {
            wchar_t *c;
            int slen = s.Length();
            int nlen = wcslen(r->names);
            c = wcsstr(r->names, s.c_str());
            while (c && (c != r->names && *(c - 1) != L' ' || exact && ((c - r->names) + slen > nlen || (c - r->names) + slen < nlen && r->names[(c - r->names) + slen] != L' ')))
                c = wcsstr(c + 1, s.c_str());
            del = !c;
        }

        //del is changed inside previous IF, don't add ELSE
        if (del)
            l->Delete(i);
    }
}
//---------------------------------------------------------------------------
void TRadicalList::SaveToFile(FILE *f)
{
    word w = Count;
    fwrite(&w, sizeof(w), 1, f);

    for (int i = 0; i < Count; i++)
    {
        TKanjiRadical *r = Items[i];
        fwrite(&r->ch, sizeof(r->ch), 1, f);
        w = wcslen(r->list);
        fwrite(&w, sizeof(w), 1, f);
        fwrite(r->list, sizeof(wchar_t), w, f);
        fwrite(&r->strokes, sizeof(r->strokes), 1, f);
        fwrite(&r->radical, sizeof(r->radical), 1, f);
        w = wcslen(r->names);
        fwrite(&w, sizeof(w), 1, f);
        fwrite(r->names, sizeof(wchar_t), w, f);
    }

    ntree->SaveToFile(f);
}
//---------------------------------------------------------------------------
void TRadicalList::LoadFromFile(FILE *f, int version)
{
    Clear();

    word cnt;
    fread(&cnt, sizeof(cnt), 1, f);
    word w;
    while (Count < cnt)
    {
        TKanjiRadical *r = new TKanjiRadical;
        fread(&r->ch, sizeof(r->ch), 1, f);
        fread(&w, sizeof(w), 1, f);
        r->list = new wchar_t[w + 1];
        r->list[w] = 0;
        fread(r->list, sizeof(wchar_t), w, f);
        fread(&r->strokes, sizeof(r->strokes), 1, f);
        fread(&r->radical, sizeof(r->radical), 1, f);
        fread(&w, sizeof(w), 1, f);
        r->names = new wchar_t[w + 1];
        fread(r->names, sizeof(wchar_t), w, f);
        r->names[w] = 0;
        Add(r);
    }

    if (version <= 4) // DICTIONARY_VERSION, added Nov 2010, for v0.57
    {
        bool dummy;
        fread(&dummy, sizeof(bool), 1, f);
    }

    ntree->LoadFromFile(f);
}
//---------------------------------------------------------------------------
void TRadicalList::Reset()
{
    if (!fgroups)
        return;

    delete fgroups;
    fgroups = NULL;
}
//---------------------------------------------------------------------------
void TRadicalList::BeginAdd()
{
    if (!fgroups)
        fgroups = new TRadGroup;
    TIntegerList *l = new TIntegerList;
    fgroups->Add(l);
}
//---------------------------------------------------------------------------
void TRadicalList::EndAdd()
{
    if (!fgroups || !fgroups->Count)
        THROW(L"Error, supposedly bad call for radical selection add!");

    if (!fgroups->Items[fgroups->Count - 1]->Count)
        fgroups->Delete(fgroups->Count - 1);
}
//---------------------------------------------------------------------------
void TRadicalList::AddRad(word ix, bool group)
{
    if (!fgroups || !fgroups->Count)
        THROW(L"Error, supposedly bad call for radical selection add!");

    TIntegerList *l = fgroups->Items[fgroups->Count - 1];

    l->Add(ix);
    if (group)
    {
        while (Count > ix + 1 && Items[ix]->radical == Items[ix + 1]->radical)
            l->Add(++ix);
    }
}
//---------------------------------------------------------------------------
int TRadicalList::get_group_count()
{
    return (fgroups ? fgroups->Count : 0);
}
//---------------------------------------------------------------------------
UnicodeString TRadicalList::get_group_string(int ix)
{
    if (!fgroups || ix < 0 || ix >= fgroups->Count)
        THROW(L"Error, bad index for radical group string!");

    TIntegerList *l = fgroups->Items[ix];
    UnicodeString result;

    for (int i = 0; i < l->Count; i++)
        result += Items[l->Items[i]]->ch;

    return result;
}
//---------------------------------------------------------------------------
void TRadicalList::SetRadicals(TRadGroup *l)
{
    Reset();
    if (!l)
        return;

    fgroups = new TRadGroup;
    for (int ix = 0; ix < l->Count; ix++)
    {
        TIntegerList *l2 = new TIntegerList;
        l2->Assign(*l->Items[ix]);
        //for (int j = 0; j < l->Items[ix]->Count; j++)
        //    l2->Add(l->Items[ix]->Items[j]);
        fgroups->Add(l2);
    }
}
//---------------------------------------------------------------------------
void TRadicalList::GetRadicals(TRadGroup* &l)
{
    TRadGroup *tmp = fgroups;
    fgroups = NULL;
    SetRadicals(tmp);
    l = fgroups;
    fgroups = tmp;
}
//---------------------------------------------------------------------------
bool TRadicalList::MatchRadicals(wchar_t kanji, TRadGroup *l)
{
    if (!l)
        return true;

    bool found = true;
    for (int i = 0; i < l->Count && found; i++)
    {
        TIntegerList *ll = l->Items[i];
        found = false;
        for (int j = 0; j < ll->Count && !found; j++)
        {
            if (Items[ll->Items[j]]->list)
                found = wcschr(Items[ll->Items[j]]->list, kanji) != NULL;
        }

    }
    return found;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(213);
#endif

