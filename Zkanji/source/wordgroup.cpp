//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "wordgroup.h"
#include "words.h"
#include "collection.h"
#include "wordgroupstudy.h"
#include "zkformats.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(182);
#endif

extern bool freeing_user_data;
void WordGroupChanged(TWordCollection *coll, TWord *w);
//---------------------------------------------------------------------------
TWordGroup::TWordGroup(TWordCollection *acollection, UnicodeString aname)
{
    flist = new TWordIndexList(acollection);
    fname = aname;
    fchecked = false;
}
//---------------------------------------------------------------------------
TWordGroup::~TWordGroup()
{
    for (int i = 0; i < flist->Count; i++)
        FreeWordLink(i);
    delete flist;
}
//---------------------------------------------------------------------------
void TWordGroup::FreeWordLink(int ix)
{
    if (flist->Items[ix]->index < 0)
        return;

    byte m = flist->Items[ix]->meaning;

    TWord *w = flist->Words[ix];
    TWordGroups *tmp, *g = w->group;

    if (g->group == this && g->meaning == m)
    {
        w->group = g->next; // First one it is!
        delete g;
    }
    else
    {
        // Should find it, no checks needed (?)
        while (g->next->group != this || g->next->meaning != m)
            g = g->next;
        tmp = g->next->next;
        delete g->next;
        g->next = tmp;
    }

    // Broadcast that a word with the current meaning was deleted from a group.
    if (!freeing_user_data)
    {
        bool notingroup = true;
        g = w->group;
        while (g && notingroup)
        {
            if (g->meaning == m)
                notingroup = false;
            g = g->next;
        }

        if (notingroup) // The word meaning is no longer in any group. Broadcast it so the red + is removed from word lists.
            WordGroupChanged(collection, w);
    }
}
//---------------------------------------------------------------------------
void TWordGroup::DeleteWord(int ix)
{
    FreeWordLink(ix);
    flist->Delete(ix);
}
//---------------------------------------------------------------------------
void TWordGroup::set_name(UnicodeString aname)
{
    fname = aname;
}
//---------------------------------------------------------------------------
TWordStatGlobal* TWordGroup::get_wordstat(int ix)
{
    return list->Words[ix]->stats ? &(list->Words[ix]->stats[list->Items[ix]->meaning]) : NULL;
}
//---------------------------------------------------------------------------
TWordCollection* TWordGroup::get_collection()
{
    return flist->collection;
}
//---------------------------------------------------------------------------
TWordList* TWordGroup::get_words()
{
    return flist->collection->words;
}
//---------------------------------------------------------------------------
int TWordGroup::AddWord(int aindex, char ameaning)
{
    int i = -1;
    MODON
    if (aindex >= 0)
    {
        TWord *w;

        if (words->Count <= aindex)
            THROW(L"Why?");

        w = words->Items[aindex];

        if (!w->group)
        {
            w->group = new TWordGroups;
            memset(w->group, 0, sizeof(TWordGroups));
            w->group->meaning = ameaning;
            w->group->group = this;
        }
        else
        {
            TWordGroups *g = new TWordGroups;
            memset(g, 0, sizeof(TWordGroups));
            g->next = w->group;
            g->meaning = ameaning;
            g->group = this;
            w->group = g;
        }

        int ingroupcnt = 0;
        TWordGroups *g = w->group;
        while (g && ingroupcnt < 2)
        {
            if (w->group->meaning == ameaning)
                ingroupcnt++;
            g = g->next;
        }

        if (ingroupcnt == 1 && !dictionary->Loading()) // Broadcast that a word was added to a group.
            WordGroupChanged(collection, w);
    }

    i = list->Add(aindex, ameaning);
    collection->Modify(false);
    MODOFF

    return i;
}
//---------------------------------------------------------------------------
//void TWordGroup::Export(FILE *f)
//{
//    UnicodeString z;
//    for (int ix = 0; ix < flist->Count; ++ix)
//    {
//        TWord *w = flist->Words[ix];
//        z = (UnicodeString)fname + L"\t" + w->kanji + L"\t" + w->kana + L"\t" + list->Meanings[ix];
//        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
//        feol(f);
//    }
//}
//---------------------------------------------------------------------------
void TWordGroup::RemoveLine(int windex)
{
    RemoveMeaning(windex, -1);
}
//---------------------------------------------------------------------------
void TWordGroup::RemoveMeaning(int windex, char meaning)
{
    bool removed = false;
    for (int j = list->Count - 1; j >= 0; --j)
    {
        if (!removed && list->Items[j]->index == windex && (meaning < 0 || meaning == list->Items[j]->meaning))
        {
            removed = true;
            DeleteWord(j);
            continue;
        }

        if (meaning < 0)
        {
            if (list->Items[j]->index > windex)
                list->Items[j]->index--;
        }
        else if (list->Indexes[j] == windex && list->Items[j]->meaning > meaning)
            list->Items[j]->meaning--;
    }
}
//---------------------------------------------------------------------------
void TWordGroup::MoveMeaning(int windex, char meaning, char newpos)
{
    for (int j = 0; j < list->Count; ++j)
    {
        if (list->Indexes[j] != windex || list->Items[j]->meaning < min(newpos, meaning) || list->Items[j]->meaning > max(newpos, meaning))
            continue;

        if (list->Items[j]->meaning == meaning)
            list->Items[j]->meaning = newpos;
        else if (list->Items[j]->meaning > meaning)
            --list->Items[j]->meaning;
        else
            ++list->Items[j]->meaning;
    }
}
//---------------------------------------------------------------------------
void TWordGroup::CleanUp()
{
    for (int ix = list->Count - 1; ix >= 0; --ix)
    {
        if (list->Items[ix]->index < 0 || list->Items[ix]->meaning < 0)
            list->Delete(ix);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordGroupList::TWordGroupList(TWordCollection *acollection, bool astudy) : inherited()
{
    fcollection = acollection;
    fstudy = astudy;
};
//---------------------------------------------------------------------------
TWordGroupList::~TWordGroupList()
{
    Clear();
}
//---------------------------------------------------------------------------
int TWordGroupList::Add(const UnicodeString s)
{
    if (fstudy)
        THROW(L"Don't use this simple add with study groups! (bug)");

    TWordGroup *g = new TWordGroup(fcollection, s);
    return inherited::Add(g);
}
//---------------------------------------------------------------------------
void TWordGroupList::set_name(int group, const UnicodeString s)
{
    Items[group]->name = s;
}
//---------------------------------------------------------------------------

UnicodeString TWordGroupList::get_name(int group)
{
    return Items[group]->name;
}
//---------------------------------------------------------------------------
TWordGroupStudy* TWordGroupList::get_test(int ix)
{
    TWordGroupStudy *res = dynamic_cast<TWordGroupStudy*>(Items[ix]);

    if (!res)
        THROW(L"This is not a study group! (bug)");
    return res;

}
//---------------------------------------------------------------------------
TWordIndexList* TWordGroupList::get_list(int group)
{
    return Items[group]->list;
}
//---------------------------------------------------------------------------
void TWordGroupList::DeleteWord(int group, int ix)
{
    Items[group]->DeleteWord(ix);
}
//---------------------------------------------------------------------------
int TWordGroupList::AddWord(int group, int aindex, char ameaning)
{
    return Items[group]->AddWord(aindex, ameaning);
}
//---------------------------------------------------------------------------
bool TWordGroupList::WordInGroup(int group, int aindex, int ameaning)
{
    TWordList *words = Items[group]->collection->words;
    TWordGroups *gl;
    if ((gl = words->Items[aindex]->group) == NULL)
        return false;
    while (gl)
    {
        if (gl->group == Items[group] && gl->meaning == ameaning)//get meaning
                    return true;
        gl = gl->next;
    }

    return false;
}
//---------------------------------------------------------------------------
void TWordGroupList::LoadFromFile(TWordCollection *collection, TConvertItem *ci, int cc, FILE *f, byte version)
{
    int cnt, c;
    byte b;

    int ix;
    byte m;

    Clear();

    UnicodeString newname;

    // Read group names and their data.
    fread(&cnt, sizeof(int), 1, f);
    Capacity = cnt;
    for (int i = 0; i < cnt; i++)
    {

        if (version >= 11) // Added 2011.03.29
            read(newname, f, sltByte);
        else
        {
            char *nameb = NULL;
            fread(&b, sizeof(byte), 1, f);
            if (b)
            {
                nameb = new char[b];
                fread(nameb, sizeof(char), b, f);
            }
            newname = UnicodeString(nameb, b);
            delete[] nameb;
        }
        bool bl;
        fread(&bl, sizeof(bool), 1, f);

        if (fstudy) // We need to know the method before we do anything else.
        {
            fread(&b, sizeof(byte), 1, f);
            TWordGroup *g;
            if (b == 0)
            {
                g = new TWordGroupM1(collection, newname);
                Add(g);
            }
            else if (b == 1)
            {
                g = new TWordGroupM2(collection, newname);
                Add(g);
            }
        }
        else
            Add(newname);

        Items[i]->checked = bl;

        fread(&c, sizeof(int), 1, f);

        bool missing = false; // Only used before version 17.

        for (int j = 0; j < c; j++)
        {
            fread(&ix, sizeof(int), 1, f);
            fread(&m, sizeof(byte), 1, f);

            if (version >= 17)
            {
                AddWord(i, ix, m);
            }
            else
            {
                // Find word with this old index, and add it if possible.
                int min = 0, max = cc - 1;
                while (min < max)
                {
                    int mid = (min + max) / 2;
                    if (ci[mid].original < ix)
                        min = mid + 1;
                    else if (ci[mid].original > ix)
                        max = mid - 1;
                    else
                        min = max = mid;
                }

                if (ci[min].changed < 0 || (collection->words->Items[ci[min].changed])->meaningcnt <= m)
                {
                    missing = true;
                    AddWord(i, -1, 0);
                }
                else
                    AddWord(i, ci[min].changed, m);
            }
        }

        if (fstudy)
            Tests[i]->LoadFromFile(f, version);

        if (missing)
        {
            // Only used before version 17
            if (fstudy)
                Tests[i]->Reset();
            for (int j = Items[i]->list->Count - 1; j >= 0; j--)
                if (Items[i]->list->Items[j]->index < 0)
                    DeleteWord(i, j);

            if (fstudy && Items[i]->list->Count < 10)
            {
                Delete(i);
                i--;
                cnt--;
            }
        }
    }
}
//---------------------------------------------------------------------------
void TWordGroupList::SaveToFile(FILE *f)
{
    int c;
    byte b;

    // Save group names and their data.
    c = Count;
    fwrite(&c, sizeof(int), 1, f);
    for (int i = 0; i < Count; i++)
    {
/*b = Names[i].Length();
    fwrite(&b,sizeof(byte),1,f);
    fwrite(Names[i].c_str(),sizeof(char),b,f);
*/
        write(Names[i], f, sltByte);

        bool bl = Items[i]->checked;
        fwrite(&bl, sizeof(bool), 1, f);

        if (fstudy) // We need to write the method this early.
        {
            b = Tests[i]->method;
            fwrite(&b, sizeof(byte), 1, f);
        }

        c = Lists[i]->Count;
        fwrite(&c, sizeof(int), 1, f);
        for (int j = 0; j < Lists[i]->Count; j++)
        {
            fwrite(&Lists[i]->Items[j]->index, sizeof(int), 1, f);
            fwrite(&Lists[i]->Items[j]->meaning, sizeof(byte), 1, f);
        }

        if (fstudy)
            Tests[i]->SaveToFile(f);
    }
}
//---------------------------------------------------------------------------
bool TWordGroupList::get_checked(int i)
{
    return Items[i]->checked;
}
//---------------------------------------------------------------------------
void TWordGroupList::set_checked(int i, bool val)
{
    Items[i]->checked = val;
}
//---------------------------------------------------------------------------
int TWordGroupList::get_checkedcount()
{
    int r = 0;
    for (int i = 0; i < Count; i++)
        if (Checked[i])
            r++;
    return r;
}
//---------------------------------------------------------------------------
int TWordGroupList::IndexOf(UnicodeString s)
{
    int res = -1;
    s = s.Trim().LowerCase();
    for (int i = 0; i < Count && res < 0; i++)
        if (Items[i]->name.LowerCase() == s)
            res = i;
    return res;
}
//---------------------------------------------------------------------------
//void TWordGroupList::Export(FILE *f)
//{
//    UnicodeString z;
//    z = L"\tword groups:";
//    fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
//    feol(f);
//
//    for (int ix = 0; ix < Count; ++ix)
//        Items[ix]->Export(f);
//}
//---------------------------------------------------------------------------
void TWordGroupList::RemoveLine(int windex)
{
    for (int ix = 0; ix < Count; ++ix)
        Items[ix]->RemoveLine(windex);
}
//---------------------------------------------------------------------------
void TWordGroupList::RemoveMeaning(int windex, char meaning)
{
    for (int ix = 0; ix < Count; ++ix)
        Items[ix]->RemoveMeaning(windex, meaning);
}
//---------------------------------------------------------------------------
void TWordGroupList::MoveMeaning(int windex, char meaning, char newpos)
{
    for (int ix = 0; ix < Count; ++ix)
        Items[ix]->MoveMeaning(windex, meaning, newpos);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(183);
#endif
