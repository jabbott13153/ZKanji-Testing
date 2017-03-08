//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanjigroup.h"
#include "kanji.h"
#include "collection.h"
#include "kanjicard.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(84);
#endif

void feol(FILE *f);

//---------------------------------------------------------------------------
TKanjiGroup::TKanjiGroup(TKanjiGroupList *aowner, UnicodeString aname) : fowner(aowner), fname(aname), fchecked(false), TKanjiCardList(aowner->collection)
{
}
//---------------------------------------------------------------------------
TKanjiGroup::TKanjiGroup(TKanjiGroupList *aowner, UnicodeString aname, FILE *f, int version, TGapList<TIntegerList*> *exampleconvert) : fowner(aowner), fname(aname), fchecked(false), TKanjiCardList(aowner->collection)
{
    LoadFromFile(f, version, exampleconvert);
}
//---------------------------------------------------------------------------
void TKanjiGroup::LoadFromFile(FILE *f, int version, TGapList<TIntegerList*> *exampleconvert)
{
    if (version >= 4)
        fread(&fchecked, sizeof(bool), 1, f);
    TKanjiCardList::LoadFromFile(f, version, exampleconvert);
}
//---------------------------------------------------------------------------
void TKanjiGroup::SaveToFile(FILE *f)
{
    fwrite(&fchecked, sizeof(bool), 1, f);
    TKanjiCardList::SaveToFile(f);
}
//---------------------------------------------------------------------------
void TKanjiGroup::Export(FILE *f)
{
    UnicodeString z;

    for (int ix = 0; ix < Count; ++ix)
    {
        z = fname + L"\t" + Kanjis[ix]->ch + L"";
        fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
        feol(f);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TKanjiGroupList::TKanjiGroupList(TWordCollection *acoll) : fcollection(acoll), inherited()
{
}
//---------------------------------------------------------------------------
TKanjiGroupList::~TKanjiGroupList()
{
    Clear();
}
//---------------------------------------------------------------------------
int TKanjiGroupList::Add(TKanjiGroup *g)
{
    return inherited::Add(g);
}
//---------------------------------------------------------------------------
int TKanjiGroupList::Add(UnicodeString s)
{
    return inherited::Add(new TKanjiGroup(this, s));
}
//---------------------------------------------------------------------------
void TKanjiGroupList::set_name(int group, const UnicodeString s)
{
    Items[group]->name = s;
}
//---------------------------------------------------------------------------
UnicodeString TKanjiGroupList::get_name(int group)
{
    return Items[group]->name;
}
//---------------------------------------------------------------------------
int TKanjiGroupList::AddKanji(int group, int kindex)
{
    return Items[group]->Add(kindex);
}
//---------------------------------------------------------------------------
void TKanjiGroupList::RemoveKanji(int group, int ix)
{
    Items[group]->Delete(ix);
}
//---------------------------------------------------------------------------
void TKanjiGroupList::LoadFromFile(FILE *f, int version, TGapList<TIntegerList*> *exampleconvert)
{
    word w;
    byte b;
    char *c;
    UnicodeString newname;

    Clear();

    fread(&w, sizeof(word), 1, f);
    Capacity = w;
    for (int i = 0; i < Capacity; i++)
    {
        if (version >= 11)
        { //added 2011.03.30
            if (version >= 12)//added 2011.04.04
                        read(newname, f, sltByteAddNULL);
            else
                read(newname, f, sltByte);
        }
        else
        {
            c = NULL;
            fread(&b, sizeof(byte), 1, f);
            if (b)
            {
                c = new char[b];
                fread(c, sizeof(char), b, f);
            }
            newname = UnicodeString(c, b);
            delete[] c;
        }
        Add(new TKanjiGroup(this, newname, f, version, exampleconvert)); //"add" loads group data from file
    }
}
//---------------------------------------------------------------------------
void TKanjiGroupList::SaveToFile(FILE *f)
{
    word w;

    w = Count;
    fwrite(&w, sizeof(word), 1, f);
    for (int i = 0; i < Count; i++)
    {
        write(Items[i]->name, f, sltByte);
        Items[i]->SaveToFile(f);
    }
}
//---------------------------------------------------------------------------
bool TKanjiGroupList::get_checked(int i)
{
    return Items[i]->checked;
}
//---------------------------------------------------------------------------
void TKanjiGroupList::set_checked(int i, bool val)
{
    Items[i]->checked = val;
}
//---------------------------------------------------------------------------
int TKanjiGroupList::CountChecked()
{
    int r = 0;
    for (int i = 0; i < Count; i++)
        if (Checked[i])
            r++;
    return r;
}
//---------------------------------------------------------------------------
int TKanjiGroupList::IndexOf(UnicodeString &groupname)
{
    int i = -1;
    groupname = groupname.Trim();
    for (int ix = 0; ix < Count && i < 0; ++ix)
        if (Names[ix].LowerCase() == groupname.LowerCase())
        {
            i = ix;
            groupname = Names[ix];
        }
    return i;
}
//---------------------------------------------------------------------------
void TKanjiGroupList::Export(FILE *f)
{
    UnicodeString z;

    z = L"\tkanji groups:";
    fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
    feol(f);

    for (int ix = 0; ix < Count; ++ix)
        Items[ix]->Export(f);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(85);
#endif

