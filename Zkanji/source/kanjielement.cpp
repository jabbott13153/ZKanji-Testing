//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <stdio.h>
#include <Math.h>

#include "kanjielement.h"
#include "kanji.h"
#include "recobj.h"
#include "bits.h"
#include "recobj.h"
#include "elementpoints.h"
#include "angle.h"
#include "gdipbmp.h"
#include "romajizer.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(78);
#endif

//v 7: names for elements can be set
#define ELEMENTS_VERSION     7
const word NO_OWNER = 0x2000;
extern bool hasrecdata;

TElementVariant ElementVariant(int aelem, byte avar)
{
    TElementVariant item;
    item.element = aelem;
    item.variant = avar;
    return item;
}
bool operator == (TElementVariant const & a, TElementVariant const & b)
{
    return a.element == b.element && a.variant == b.variant;
}
bool operator != (TElementVariant const & a, TElementVariant const & b)
{
    return !(a == b);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TRepository::TRepository(UnicodeString aname) : fname(aname), inherited()
{
}
//---------------------------------------------------------------------------
int TRepository::IndexOf(word aelem, byte avar)
{
    for (int ix = 0; ix < Count; ++ix)
        if (Items[ix]->element == aelem && Items[ix]->variant == avar)
            return ix;
    return -1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int TRepositoryList::Add(UnicodeString aname)
{
    aname.SetLength(min(aname.Length(), 32));
    if (IndexOf(aname) >= 0)
        THROW(L"Repositories cannot have conflicting names! (add)");

    return inherited::Add(new TRepository(aname));
}
//---------------------------------------------------------------------------
int TRepositoryList::IndexOf(UnicodeString aname)
{
    for (int ix = 0; ix < Count; ++ix)
        if (Items[ix]->Name == aname)
            return ix;
    return -1;
}
//---------------------------------------------------------------------------
UnicodeString TRepositoryList::get_name(int ix)
{
    return Items[ix]->Name;
}
//---------------------------------------------------------------------------
void TRepositoryList::set_name(int ix, UnicodeString val)
{
    val.SetLength(min(val.Length(), 32));
    if (IndexOf(val) != ix)
        THROW(L"Repositories cannot have conflicting names! (rename)");

    Items[ix]->Name = val;
}
//---------------------------------------------------------------------------
int TRepositoryList::get_cnt(int ix)
{
    return Items[ix]->Count;
}
//---------------------------------------------------------------------------
int TRepositoryList::Add(int arepos, word aelem, byte avar)
{
    int i;
    if ((i = Items[arepos]->IndexOf(aelem, avar)) >= 0)
        return i;

    i = Items[arepos]->Add(new TElementVariant);
    TElementVariant *item = Items[arepos]->Items[i];
    item->element = aelem;
    item->variant = avar;
    return i;
}
//---------------------------------------------------------------------------
TElementVariant TRepositoryList::get_repitem(int rep, int ix)
{
    return *Items[rep]->Items[ix];
}
//---------------------------------------------------------------------------
void TRepositoryList::RemoveItem(int rep, int ix)
{
    Items[rep]->Delete(ix);
}
//---------------------------------------------------------------------------
void TRepositoryList::VariantRemoved(short aelement, byte avar)
{
    for (int ix = 0; ix < Count; ++ix)
    {
        for (int iy = Items[ix]->Count - 1; iy >= 0; --iy)
        {
            if (Items[ix]->Items[iy]->element == aelement)
            {
                if (Items[ix]->Items[iy]->variant == avar)
                    Items[ix]->Delete(iy);
                else if (Items[ix]->Items[iy]->variant > avar)
                    --Items[ix]->Items[iy]->variant;
            }
        }
    }
}
//---------------------------------------------------------------------------
void TRepositoryList::ElementRemoved(short aelement)
{
    for (int ix = 0; ix < Count; ++ix)
    {
        for (int iy = Items[ix]->Count - 1; iy >= 0; --iy)
        {
            if (Items[ix]->Items[iy]->element == aelement)
                Items[ix]->Delete(iy);
            else if (Items[ix]->Items[iy]->element > aelement)
                --Items[ix]->Items[iy]->element;
        }
    }
}
//---------------------------------------------------------------------------
void TRepositoryList::VariantChanged(short srcelem, byte srcvar, short destelem, byte destvar)
{
    for (int ix = 0; ix < Count; ++ix)
    {
        for (int iy = Items[ix]->Count - 1; iy >= 0; --iy)
        {
            if (Items[ix]->Items[iy]->element == srcelem && Items[ix]->Items[iy]->variant == srcvar)
                Items[ix]->Items[iy]->element = destelem, Items[ix]->Items[iy]->variant = destvar;
        }
    }
}
//---------------------------------------------------------------------------
void TRepositoryList::SaveToFile(FILE *f)
{
    //byte b;
    word w = Count;
    fwrite(&w, sizeof(word), 1, f);
    for (int ix = 0; ix < Count; ++ix)
    {
/*
        b = Items[ix]->Name.Length();
        fwrite(&b,sizeof(byte),1,f);
        fwrite(Items[ix]->Name.c_str(),sizeof(char),b,f);
*/
        write(Items[ix]->Name, f, sltByte);

        w = Items[ix]->Count;
        fwrite(&w, sizeof(word), 1, f);
        for (int iy = 0; iy < Items[ix]->Count; ++iy)
        {
            TElementVariant *v = Items[ix]->Items[iy];
            fwrite(&v->element, sizeof(int), 1, f);
            fwrite(&v->variant, sizeof(byte), 1, f);
        }
    }
}
//---------------------------------------------------------------------------
void TRepositoryList::LoadFromFile(FILE *f, int version)
{
    Clear();

    byte b;
    word w;
    fread(&w, sizeof(word), 1, f);
    Capacity = w;
    UnicodeString newname;
    while (Count < Capacity)
    {
        try
        {
            if (version >= 6)
                read(newname, f, sltByte);
            else
            {
                fread(&b, sizeof(byte), 1, f);
                char *c = NULL;
                if (b)
                {
                    c = new char[b];
                    fread(c, sizeof(char), b, f);
                }
                newname = UnicodeString(c, b);
                delete[] c;
            }
            Add(newname);
        }
        catch (...)
        {
            Clear();
            throw;
        }

        fread(&w, sizeof(word), 1, f);
        Items[Count - 1]->Capacity = w;
        while (Items[Count - 1]->Count < Items[Count - 1]->Capacity)
        {
            TElementVariant *v = new TElementVariant;
            Items[Count - 1]->Add(v);
            fread(&v->element, sizeof(int), 1, f);
            fread(&v->variant, sizeof(byte), 1, f);
        }
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void elem_free(TKanjiElement* &aelem)
{
    if (!aelem)
        return;

    delete[] aelem->parents;
    if (aelem->recdata)
    {
        for (int ix = 0; ix < aelem->variants[0].strokecount; ++ix)
            delete[] aelem->recdata[ix].pos;
        delete[] aelem->recdata;
    }

    for (int ix = 0; ix < aelem->variantcount; ++ix)
    {
        if (aelem->variants[ix].standalone)
        {
            for (int j = 0; j < aelem->variants[ix].strokecount; ++j)
                delete[] aelem->variants[ix].strokes[j].points;
            delete[] aelem->variants[ix].strokes;
        }
        else
            delete[] aelem->variants[ix].positions;
    }
    delete[] aelem->variants;
    delete aelem;
    aelem = 0;
}
//---------------------------------------------------------------------------
TKanjiElement* elem_copy(const TKanjiElement *aelem, bool copy_rec)
{
    TKanjiElement *elem = new TKanjiElement;

    memcpy(elem, aelem, sizeof(TKanjiElement));
    if (!copy_rec)
        elem->recdata = NULL;

    if (elem->parentcnt)
    {
        elem->parents = new word[elem->parentcnt];
        memcpy(elem->parents, aelem->parents, sizeof(word)*elem->parentcnt);
    }

    if (elem->variantcount)
    {
        elem->variants = new TKanjiVariant[elem->variantcount];
        memcpy(elem->variants, aelem->variants, sizeof(TKanjiVariant)*elem->variantcount);

        if (elem->recdata && elem->variants[0].strokecount && elem->owner)
        {
            elem->recdata = new TRecognizeData[elem->variants[0].strokecount];
            memcpy(elem->recdata, aelem->recdata, sizeof(TRecognizeData)*elem->variants[0].strokecount);

            for (int ix = 0; ix < elem->variants[0].strokecount; ++ix)
            {
                elem->recdata[ix].pos = new Bits[elem->variants[0].strokecount - 1];
                for (int iy = 0; iy < elem->variants[0].strokecount - 1; iy++)
                    elem->recdata[ix].pos[iy] = aelem->recdata[ix].pos[iy];
            }
        }
    }
    for (int ix = 0; ix < elem->variantcount; ++ix)
    {
        if (elem->variants[ix].standalone)
        {
            elem->variants[ix].strokes = new TElementStroke[elem->variants[ix].strokecount];
            memcpy(elem->variants[ix].strokes, aelem->variants[ix].strokes, sizeof(TElementStroke)*elem->variants[ix].strokecount);
            for (int j = 0; j < elem->variants[ix].strokecount; ++j)
            {
                elem->variants[ix].strokes[j].points = new TElementPoint[elem->variants[ix].strokes[j].pointcount];
                memcpy(elem->variants[ix].strokes[j].points, aelem->variants[ix].strokes[j].points, sizeof(TElementPoint)*elem->variants[ix].strokes[j].pointcount);
            }
        }
        else
        {
            elem->variants[ix].positions = new TPartPosition[4];
            memcpy(elem->variants[ix].positions, aelem->variants[ix].positions, sizeof(TPartPosition)*4);
        }
    }

    return elem;
}
//---------------------------------------------------------------------------
void position_elem(TPartPosition &part, TElemPattern pattern, int ix, int count)
{
    if (pattern == epUndef || pattern == epC || pattern == epB || pattern == epHira || pattern == epKata || pattern == epW)
    {
        if (count == 1)
        {
            part.x = 1200;
            part.y = 800;
            part.width = 7600;
            part.height = 8400;
        }
        else if (count == 2)
        {
            part.x = 500 + ix * 4600;
            part.y = 800;
            part.width = 4400;
            part.height = 8400;
        }
        else if (count == 3)
        {
            part.x = 500;
            part.y = 800 + ix * 2866;
            part.width = 9000;
            part.height = 2666;
        }
        else
        {
            part.x = 500 + (ix % 2) * 4600;
            part.y = 800 + (ix / 2) * 4300;
            part.width = 4400;
            part.height = 4100;
        }
    }
    else if (ix > 1 && (pattern == epEbl || pattern == epEblt || pattern == epElt || pattern == epEltr || pattern == epEtr || pattern == epErbl || pattern == epEw || pattern == epH2 || pattern == epV2 || pattern == epElr) || ix > 2 && (pattern == epH3 || pattern == epV3 || pattern == epT))
    {
        part.x = 500 + (ix % 2) * 4600;
        part.y = 800 + (ix / 2) * 4300;
        part.width = 4400;
        part.height = 4000;
    }
    else
    {
        switch (pattern)
        {
        case epEbl:
            if (ix == 0)
            {
                part.x = 500;
                part.y = 1400;
                part.width = 9000;
                part.height = 7800;
            }
            else
            {
                part.x = 3000;
                part.y = 800;
                part.width = 6000;
                part.height = 7000;
            }
            break;
        case epEblt:
            if (ix == 0)
            {
                part.x = 500;
                part.y = 800;
                part.width = 9000;
                part.height = 8400;
            }
            else
            {
                part.x = 3000;
                part.y = 2200;
                part.width = 6000;
                part.height = 5600;
            }
            break;
        case epElt:
            if (ix == 0)
            {
                part.x = 500;
                part.y = 800;
                part.width = 9000;
                part.height = 7800;
            }
            else
            {
                part.x = 3000;
                part.y = 2200;
                part.width = 6000;
                part.height = 7000;
            }
            break;
        case epEltr:
            if (ix == 0)
            {
                part.x = 500;
                part.y = 800;
                part.width = 9000;
                part.height = 8000;
            }
            else
            {
                part.x = 2500;
                part.y = 3000;
                part.width = 5000;
                part.height = 6000;
            }
            break;
        case epEtr:
            if (ix == 0)
            {
                part.x = 500;
                part.y = 800;
                part.width = 9000;
                part.height = 8400;
            }
            else
            {
                part.x = 1500;
                part.y = 3400;
                part.width = 5000;
                part.height = 5400;
            }
            break;
        case epErbl:
            if (ix == 0)
            {
                part.x = 500;
                part.y = 2300;
                part.width = 9000;
                part.height = 6900;
            }
            else
            {
                part.x = 2500;
                part.y = 1000;
                part.width = 5000;
                part.height = 6500;
            }
            break;
        case epEw:
            if (ix == 0)
            {
                part.x = 1000;
                part.y = 800;
                part.width = 8000;
                part.height = 8400;
            }
            else
            {
                part.x = 2000;
                part.y = 2000;
                part.width = 6000;
                part.height = 6000;
            }
            break;
        case epElr:
            if (ix == 0)
            {
                part.x = 1000;
                part.y = 800;
                part.width = 7000;
                part.height = 8400;
            }
            else
            {
                part.x = 3500;
                part.y = 800;
                part.width = 3000;
                part.height = 8400;
            }
            break;
        case epH2:
            if (ix == 0)
            {
                part.x = 500;
                part.y = 800;
                part.width = 4300;
                part.height = 8400;
            }
            else
            {
                part.x = 5200;
                part.y = 800;
                part.width = 4300;
                part.height = 8400;
            }
            break;
        case epH3:
            if (ix == 0)
            {
                part.x = 500;
                part.y = 800;
                part.width = 2800;
                part.height = 8400;
            }
            else if (ix == 1)
            {
                part.x = 3600;
                part.y = 800;
                part.width = 2800;
                part.height = 8400;
            }
            else
            {
                part.x = 6700;
                part.y = 800;
                part.width = 2800;
                part.height = 8400;
            }
            break;
        case epV2:
            if (ix == 0)
            {
                part.x = 600;
                part.y = 800;
                part.width = 8800;
                part.height = 4000;
            }
            else
            {
                part.x = 600;
                part.y = 5200;
                part.width = 8800;
                part.height = 4000;
            }
            break;
        case epV3:
            if (ix == 0)
            {
                part.x = 600;
                part.y = 800;
                part.width = 8800;
                part.height = 2533;
            }
            else if (ix == 1)
            {
                part.x = 600;
                part.y = 3733;
                part.width = 8800;
                part.height = 2533;
            }
            else
            {
                part.x = 600;
                part.y = 6666;
                part.width = 8800;
                part.height = 2533;
            }
            break;
        case epV4:
            if (ix == 0)
            {
                part.x = 600;
                part.y = 800;
                part.width = 8800;
                part.height = 1900;
            }
            else if (ix == 1)
            {
                part.x = 600;
                part.y = 2966;
                part.width = 8800;
                part.height = 1900;
            }
            else if (ix == 2)
            {
                part.x = 600;
                part.y = 5132;
                part.width = 8800;
                part.height = 1900;
            }
            else
            {
                part.x = 600;
                part.y = 7298;
                part.width = 8800;
                part.height = 1900;
            }
            break;
        case epT:
            if (ix == 0)
            {
                part.x = 2000;
                part.y = 700;
                part.width = 6000;
                part.height = 4200;
            }
            else if (ix == 1)
            {
                part.x = 700;
                part.y = 5200;
                part.width = 4200;
                part.height = 4100;
            }
            else
            {
                part.x = 5100;
                part.y = 5200;
                part.width = 4200;
                part.height = 4100;
            }
            break;
        }
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TElementList *elements = NULL;
word elementcount = 0;

//---------------------------------------------------------------------------
TElementList::TElementList(word count) : version(ELEMENTS_VERSION), inherited()
{
    while (count > Count)
    {
        memset(Items[Add(new TKanjiElement)], 0, sizeof(TKanjiElement));
        Items[Count - 1]->owner = -1;
        for (int ix = 0; ix < 4; ++ix)
            Items[Count - 1]->parts[ix] = -1;
    }

    repositories = new TRepositoryList;
}
//---------------------------------------------------------------------------
TElementList::TElementList(UnicodeString fname) : version(ELEMENTS_VERSION), inherited()
{
    repositories = new TRepositoryList;
    LoadFromFile(fname);
}
//---------------------------------------------------------------------------
TElementList::~TElementList()
{
    Clear();
    delete repositories;
}
//---------------------------------------------------------------------------
void TElementList::FreeItem(TKanjiElement *e, int ix)
{
    if (e->owner < NO_OWNER)
    {
        try
        {
            kanjis->Kanjis[e->owner]->element = -1;
        }
        catch (...)
        {
            ;
        }
    }
    elem_free(e);
}
//---------------------------------------------------------------------------
void TElementList::Update(short element, TKanjiElement *elem)
{
    for (int ix = 0; ix < 4 && Items[element]->parts[ix] >= 0; ++ix)
        RemoveParent(Items[element]->parts[ix], element);
    for (int ix = 0; ix < 4 && elem->parts[ix] >= 0; ++ix)
        AddParent(elem->parts[ix], element);

    DeleteVariantNames(element);

    int sc = Items[element]->variantcount ? Items[element]->variants[0].strokecount : 0;
    TRecognizeData *tmprec = Items[element]->recdata;
    Items[element]->recdata = NULL;
    TKanjiElement *e = elem_copy(elem, false);
    if (sc != (e->variantcount ? e->variants[0].strokecount : 0))
        DeleteRecognizeData(element);
    else
        e->recdata = tmprec;
    Delete(element);
    Insert(element, e);
    if (e->owner < NO_OWNER)
        kanjis->Kanjis[e->owner]->element = element;
}
//---------------------------------------------------------------------------
void TElementList::PositionUpdate(short element, byte avar)
{
    TGapList<int> gl(false);
    __Reposition(element, avar, gl, false);
}
//---------------------------------------------------------------------------
void TElementList::ChangeVariant(short element, byte avar, byte newvar)
{
    TKanjiElement *elem = Items[element];

    for (int ix = 0; ix < elem->parentcnt; ++ix)
    {
        TKanjiElement *e = Items[elem->parents[ix]];
        for (int j = 0; j < 4 && e->parts[j] >= 0; ++j)
            if (e->parts[j] == element)
                for (int k = 0; k < e->variantcount; ++k)
                    if (!e->variants[k].standalone && e->variants[k].positions[j].variant == avar)
                    {
                        e->variants[k].positions[j].variant = newvar;
                        PositionUpdate(elem->parents[ix], k);
                    }
    }
}
//---------------------------------------------------------------------------
void TElementList::RemoveVariant(short element, byte avar)
{
  /*when called, the given variant must not be used by any parent!*/

    TKanjiElement *elem = Items[element];
    if (elem->variantcount <= 1 || elem->variantcount <= avar)
        THROW(L"Removing invalid variant!");

    RemoveVariantName(element, avar);

    for (int ix = 0; ix < elem->parentcnt; ++ix)
    {
        TKanjiElement *e = Items[elem->parents[ix]];
        for (int j = 0; j < 4 && e->parts[j] >= 0; ++j)
            if (e->parts[j] == element)
                for (int k = 0; k < e->variantcount; ++k)
                    if (!e->variants[k].standalone && e->variants[k].positions[j].variant > avar)
                        e->variants[k].positions[j].variant--;
            /*will be removed after making sure this never happens*/
                    else if (!e->variants[k].standalone && e->variants[k].positions[j].variant == avar)
                        THROW(L"Error, variant shouldn't be here!");
    }

    TKanjiVariant *tmp = elem->variants;
    elem->variants = new TKanjiVariant[elem->variantcount - 1];
    if (avar)
        memcpy(elem->variants, tmp, sizeof(TKanjiVariant)*avar);
    if (avar < elem->variantcount - 1)
        memcpy(elem->variants + avar, tmp + avar + 1, sizeof(TKanjiVariant)*(elem->variantcount - avar - 1));

    TKanjiVariant *v = &tmp[avar];
    if (v->standalone)
    {
        for (int ix = 0; ix < v->strokecount; ++ix)
            delete[] v->strokes[ix].points;
        delete[] v->strokes;
    }
    else
    {
        delete[] v->positions;
    }
    delete[] tmp;

    --elem->variantcount;

    if (avar == 0)//recdata became invalid when first variant is removed
                PositionUpdate(element, 0);

    repositories->VariantRemoved(element, avar);
}
//---------------------------------------------------------------------------
void TElementList::DuplicateVariant(short aelement, byte avar)
{
    TKanjiElement *elem = Items[aelement];
    if (elem->variantcount >= 32)
        THROW(L"Cannot create more than 32 variants for an element!");
    if (elem->variantcount <= avar)
        THROW(L"Duplicating invalid element!");

    TKanjiVariant *tmp = elem->variants;
    try
    {
        elem->variants = new TKanjiVariant[elem->variantcount + 1];
        if (elem->variantcount)
            memcpy(elem->variants, tmp, sizeof(TKanjiVariant)*elem->variantcount);
        if (tmp)
            memcpy(elem->variants + elem->variantcount, tmp + avar, sizeof(TKanjiVariant));

        TKanjiVariant *v = &elem->variants[elem->variantcount];
        TKanjiVariant *oldv = &elem->variants[avar];
        if (v->standalone)
        {
            v->strokes = new TElementStroke[v->strokecount];
            for (int ix = 0; ix < v->strokecount; ++ix)
            {
                v->strokes[ix].pointcount = oldv->strokes[ix].pointcount;
                v->strokes[ix].tips = oldv->strokes[ix].tips;
                v->strokes[ix].points = new TElementPoint[v->strokes[ix].pointcount];
                memcpy(v->strokes[ix].points, oldv->strokes[ix].points, sizeof(TElementPoint)*v->strokes[ix].pointcount);
            }
        }
        else
        {
            v->positions = new TPartPosition[4];
            memcpy(v->positions, oldv->positions, sizeof(TPartPosition)*4);
        }
    }
    catch (...)
    {
        TKanjiVariant *bak = elem->variants;
        elem->variants = tmp;
        delete[] bak;
        throw;
    }
    delete[] tmp;

    ++elem->variantcount;
}
//---------------------------------------------------------------------------
void TElementList::AddKanjiModel(TKanjiElement *elem, byte variant, byte stroke)
{
    TPointListEx *pt = new TPointListEx;
    TRect r = Rect(0, 0, 10000, 10000);
    GetElementStrokePoints(r, elem, variant, stroke, pt);
    ProcessStroke(pt, pt, false);
    pt->resize(10000, 10000);
    DissectStroke(pt, pt, 3, false);

    models->Add(pt);
}
//---------------------------------------------------------------------------
void TElementList::AddCharacterModel(TKanjiElement *elem, byte variant, byte stroke)
{
    TPointListEx *pt = new TPointListEx;
    TRect r = Rect(0, 0, 10000, 10000);
    GetElementStrokePoints(r, elem, variant, stroke, pt);
    ProcessStroke(pt, pt, false);
    pt->resize(10000, 10000);
    DissectStroke(pt, pt, 3, false);

    cmodels->Add(pt);
}
//---------------------------------------------------------------------------
void TElementList::RemoveKanjiModel(int kindex)
{
    models->Delete(kindex);
    for (int ix = 0; ix < elements->Count; ++ix)
    {
        TKanjiElement *e = elements->Items[ix];
        if (!e->recdata)
            continue;
        for (int iy = 0; iy < e->variants[0].strokecount; ++iy)
        {
            if (e->recdata[iy].data.index == kindex)
            {
                DeleteRecognizeData(ix);
                GenerateRecognizeData(ix);
                break;
            }
            else if (e->recdata[iy].data.index > kindex)
                e->recdata[iy].data.index--;
        }
    }
}
//---------------------------------------------------------------------------
void TElementList::RemoveCharacterModel(int cindex)
{
    cmodels->Delete(cindex);
    for (int ix = 0; ix < elements->Count; ++ix)
    {
        TKanjiElement *e = elements->Items[ix];
        if (!e->recdata)
            continue;
        for (int iy = 0; iy < e->variants[0].strokecount; ++iy)
        {
            if (e->recdata[iy].data.index == models->Count + cindex)
            {
                DeleteRecognizeData(ix);
                GenerateRecognizeData(ix);
                break;
            }
            else if (e->recdata[iy].data.index > models->Count + cindex)
                e->recdata[iy].data.index--;
        }
    }
}
//---------------------------------------------------------------------------
void TElementList::DeleteRecognizeData(int element)
{
    TKanjiElement *e = elements->Items[element];

    if (e->recdata)
    {
        for (int ix = 0; ix < e->variants[0].strokecount; ++ix)
            delete[] e->recdata[ix].pos;
        delete[] e->recdata;
        e->recdata = NULL;
    }
}
//---------------------------------------------------------------------------
void TElementList::GenerateRecognizeData(int element)
{
    //int m;
    TPointListEx *tmp = NULL;
    TRect *dims = NULL;
    TRect r = Rect(0, 0, 10000, 10000);
    try
    {
        tmp = new TPointListEx;
        TKanjiElement *e = elements->Items[element];

        if (e->recdata)
            THROW(L"No recognition data should be present at the generation!");

        if (!e->variantcount || !e->variants[0].strokecount || e->owner == NO_OWNER)
            return;

        bool genchar = e->owner > NO_OWNER; //non kanji unicode character

        e->recdata = new TRecognizeData[e->variants[0].strokecount];
        for (int ix = 0; ix < e->variants[0].strokecount; ++ix)
            e->recdata[ix].pos = new Bits[e->variants[0].strokecount - 1];

        dims = new TRect[e->variants[0].strokecount];

//if (element == 4530)
//m = 1;

        for (int ix = 0; ix < e->variants[0].strokecount; ++ix)
        {
            GetElementStrokePoints(r, e, 0, ix, tmp);
            ProcessStroke(tmp, tmp, false);

            dims[ix] = tmp->Dimension;

            tmp->resize(10000, 10000);
            DissectStroke(tmp, tmp);
            e->recdata[ix].data.distance = -1;
            e->recdata[ix].data.index = -1;

            if (genchar)
            {
                for (int iy = 0; iy < cmodels->Count; ++iy)
                {
                    double d = cmodels->Items[iy]->compare(tmp);
                    if (e->recdata[ix].data.distance < 0 || d < e->recdata[ix].data.distance)
                    {
                        e->recdata[ix].data.distance = d;
                        e->recdata[ix].data.index = iy + models->Count; //index for general char data starts from models->Count
                    }
                }
            }

            for (int iy = 0; iy < models->Count; ++iy)
            {
                double d = models->Items[iy]->compare(tmp);
                if (e->recdata[ix].data.distance < 0 || d < e->recdata[ix].data.distance)
                {
                    e->recdata[ix].data.distance = d;
                    e->recdata[ix].data.index = iy;
                }
            }
            tmp->clear();
        }

        for (int ix = 0; ix < e->variants[0].strokecount; ++ix)
            for (int iy = 0; iy < e->variants[0].strokecount; ++iy)
                if (ix != iy)
                    ProcessDimensions(e->recdata[ix].pos[iy - (iy > ix ? 1 : 0)], dims[ix], dims[iy], Rect(0, 0, e->variants[0].width, e->variants[0].height));
    }
    catch (...)
    {
        ;
    }
    delete tmp;
    delete[] dims;
}
//---------------------------------------------------------------------------
void TElementList::RegenerateRecognizeData(int element)
{
    DeleteRecognizeData(element);
    GenerateRecognizeData(element);
}
//---------------------------------------------------------------------------
int TElementList::ParentPos(short aelement, short aparent)
{
    TKanjiElement *elem = Items[aelement];
    for (int ix = 0; ix < elem->parentcnt; ++ix)
        if (elem->parents[ix] == aparent)
            return ix;
    return -1;
}
//---------------------------------------------------------------------------
void TElementList::RemoveParent(short aelement, short aparent)
{
    int ppos = ParentPos(aelement, aparent);
    if (ppos < 0)
        return;

    TKanjiElement *elem = Items[aelement];
    word *tmp = elem->parents;

    if (elem->parentcnt > 1)
    {
        elem->parents = new word[elem->parentcnt - 1];
        if (ppos)
            memcpy(elem->parents, tmp, sizeof(word)*ppos);
        if (ppos < elem->parentcnt - 1)
            memcpy(elem->parents + ppos, tmp + ppos + 1, sizeof(word)*(elem->parentcnt - ppos - 1));
    }
    else
        elem->parents = NULL;
    --elem->parentcnt;

    delete[] tmp;
}
//---------------------------------------------------------------------------
void TElementList::AddParent(short aelement, short aparent)
{
    if (ParentPos(aelement, aparent) >= 0)
        return;

    TKanjiElement *elem = Items[aelement];
    word *tmp = elem->parents;
    int ppos = elem->parentcnt;
    for (int ix = 0; ix < elem->parentcnt; ++ix)
        if (aparent < elem->parents[ix])
        {
            ppos = ix;
            break;
        }
    elem->parents = new word[elem->parentcnt + 1];
    if (ppos > 0)
        memcpy(elem->parents, tmp, sizeof(word)*ppos);
    if (ppos < elem->parentcnt)
        memcpy(elem->parents + ppos + 1, tmp + ppos, sizeof(word)*(elem->parentcnt - ppos));
    elem->parents[ppos] = aparent;

    ++elem->parentcnt;
    delete[] tmp;
}
//---------------------------------------------------------------------------
TKanjiVariant* TElementList::get_variant(int ix, byte var)
{
    if (var >= Items[ix]->variantcount)
        throw ERangeError("Invalid index for element variant!");
    return &Items[ix]->variants[var];
}
//---------------------------------------------------------------------------
byte TElementList::get_stroke_count(int ix, byte var)
{
    return Variants[ix][var]->strokecount;
}
//---------------------------------------------------------------------------
int TElementList::get_repcount()
{
    return repositories->Count;
}
//---------------------------------------------------------------------------
int TElementList::get_repscount(int ix)
{
    return repositories->Counts[ix];
}
//---------------------------------------------------------------------------
UnicodeString TElementList::get_repname(int ix)
{
    return repositories->Names[ix];
}
//---------------------------------------------------------------------------
void TElementList::set_repname(int ix, UnicodeString val)
{
    repositories->Names[ix] = val;
}
//---------------------------------------------------------------------------
TElementVariant TElementList::get_repitem(int repix, int ix)
{
    return repositories->RepItem[repix][ix];
}
//---------------------------------------------------------------------------
extern bool hasrecdata; //stroke recognition data loaded
void TElementList::SaveToFile(UnicodeString fname)
{
    //byte b;
    word w;
    FILE *f = _wfopen(fname.c_str(), L"wb");
    if (!f)
        THROW(L"Cannot create stroke element file!");
    try
    {
        version = ELEMENTS_VERSION;

        char verstr[9];
        sprintf(verstr, "zksod%03i", version);
        fwrite(verstr, sizeof(char), 8, f);

        w = Count;
        fwrite(&w, sizeof(word), 1, f);

        for (int i = 0; i < Count; i++)
        {
            TKanjiElement *e = Items[i];

            fwrite(&e->owner, sizeof(word), 1, f); //changed from short dec 2010 v0.57
            fwrite(&e->pattern, sizeof(TElemPattern), 1, f);
            fwrite(&e->parentcnt, sizeof(word), 1, f);
            if (e->parentcnt)
                fwrite(e->parents, sizeof(word), e->parentcnt, f);
            fwrite(e->parts, sizeof(short), 4, f);
            fwrite(&e->variantcount, sizeof(byte), 1, f);

            for (int j = 0; j < e->variantcount; j++)
            {
                TKanjiVariant *v = e->variants + j;
                fwrite(&v->strokecount, sizeof(byte), 1, f);
                fwrite(&v->width, sizeof(word), 1, f);
                fwrite(&v->height, sizeof(word), 1, f);
                fwrite(&v->x, sizeof(short), 1, f);
                fwrite(&v->y, sizeof(short), 1, f);
                fwrite(&v->centrepoint, sizeof(byte), 1, f);
                fwrite(&v->standalone, sizeof(bool), 1, f);

                if (!v->standalone)
                {
                    for (int k = 0; k < 4; ++k)
                    {
                        TPartPosition *p = v->positions + k;
                        fwrite(&p->variant, sizeof(byte), 1, f);
                        fwrite(&p->width, sizeof(word), 1, f);
                        fwrite(&p->height, sizeof(word), 1, f);
                        fwrite(&p->x, sizeof(short), 1, f);
                        fwrite(&p->y, sizeof(short), 1, f);
                    }
                }
                else
                {
                    for (int k = 0; k < v->strokecount; ++k)
                    {
                        TElementStroke *t = v->strokes + k;
                        fwrite(&t->pointcount, sizeof(byte), 1, f);
                        for (int l = 0; l < t->pointcount; l++)
                        {
                            TElementPoint *ep = t->points + l;
                            fwrite(&ep->x, sizeof(short), 1, f);
                            fwrite(&ep->y, sizeof(short), 1, f);
                            fwrite(&ep->c1x, sizeof(short), 1, f);
                            fwrite(&ep->c1y, sizeof(short), 1, f);
                            fwrite(&ep->c2x, sizeof(short), 1, f);
                            fwrite(&ep->c2y, sizeof(short), 1, f);
                            fwrite(&ep->type, sizeof(TPointType), 1, f);
                        }
                        fwrite(&t->tips, sizeof(byte), 1, f);
                    }
                }
            }

            if (e->recdata)
            {
                for (int ix = 0; ix < e->variants[0].strokecount; ++ix)
                {
                    fwrite(&e->recdata[ix].data.index, sizeof(word), 1, f);
                    fwrite(&e->recdata[ix].data.distance, sizeof(int), 1, f);

                    for (int iy = 0; iy < e->variants[0].strokecount - 1; ++iy)
                        e->recdata[ix].pos[iy].SaveToFile(f);
                }
            }
        }

        models->SaveToFile(f);
        cmodels->SaveToFile(f);

        repositories->SaveToFile(f);

    //v7, 2011.11.08
        w = elementnames.TrueCount;
        fwrite(&w, sizeof(word), 1, f);
        for (unsigned int ix = 0; ix < elementnames.TrueCount; ++ix)
        {
            const TElementNames &name = elementnames.TrueItems[ix];

            unsigned int code = elementnames.TruePos[ix];
            fwrite(&code, sizeof(unsigned int), 1, f);

            byte c = name.count;
            wchar_t *pos = name.names;
            while (c)
            {
                pos = wcschr(pos, 0) + 1;
                c--;
            }
            w = pos - name.names;
            fwrite(&w, sizeof(word), 1, f);
            fwrite(name.names, sizeof(wchar_t), w, f);
        }
    }
    catch (...)
    {
        ;
    }
    fclose(f);
}
//---------------------------------------------------------------------------
void TElementList::LoadFromFile(UnicodeString fname)
{
    Clear();

    if (!models)
        THROW(L"You need to initialize models first you moron!");
    models->Clear();

    //byte b;
    word w;
    FILE *f = _wfopen(fname.c_str(), L"rb");
    if (!f)
        THROW(L"Cannot open stroke element file!");
    try
    {
        char verstr[9];
        fread(verstr, sizeof(char), 8, f);
        verstr[8] = 0;

        if (!strncmp("zksod", verstr, 5))
            version = strtol(verstr + 5, 0, 10);

        if (version < 2)
            THROW(L"Invalid stroke order datafile! Please update your zdict.zks!");

        fread(&w, sizeof(word), 1, f);
        Capacity = w;

        while (Count < Capacity)
            memset(Items[Add(new TKanjiElement)], 0, sizeof(TKanjiElement));

        for (int i = 0; i < Count; i++)
        {
            TKanjiElement *e = Items[i];

            if (version >= 4)
            { //added DEC 2010 for v0.57
                fread(&e->owner, sizeof(word), 1, f);

                if (e->owner < NO_OWNER)
                {
/*
        // 碵 4739 鬥 6168 - included after 2011 Jan 29, v0.57
          if (e->owner >= 4739)
            e->owner++;
          if (e->owner >= 6168)
            e->owner++;*/
                    kanjis->Kanjis[e->owner]->element = i;
                }
            }
            else
            {
                short stmp;
                fread(&stmp, sizeof(short), 1, f);

                if (stmp >= 0)
                {
                    e->owner = stmp;
                    kanjis->Kanjis[e->owner]->element = i;
                }
                else
                    e->owner = NO_OWNER;
            }

            fread(&e->pattern, sizeof(TElemPattern), 1, f);
            fread(&e->parentcnt, sizeof(word), 1, f);
            if (e->parentcnt)
            {
                e->parents = new word[e->parentcnt];
                fread(e->parents, sizeof(word), e->parentcnt, f);
            }
            fread(e->parts, sizeof(short), 4, f);
            fread(&e->variantcount, sizeof(byte), 1, f);
            if (e->variantcount)
            {
                e->variants = new TKanjiVariant[e->variantcount];
                memset(e->variants, 0, sizeof(TKanjiVariant)*e->variantcount);
            }

            for (int j = 0; j < e->variantcount; j++)
            {
                TKanjiVariant *v = e->variants + j;
                fread(&v->strokecount, sizeof(byte), 1, f);
                fread(&v->width, sizeof(word), 1, f);
                fread(&v->height, sizeof(word), 1, f);
                if (version >= 1)
                {
                    fread(&v->x, sizeof(short), 1, f);
                    fread(&v->y, sizeof(short), 1, f);
                }
                fread(&v->centrepoint, sizeof(byte), 1, f);
                fread(&v->standalone, sizeof(bool), 1, f);

                if (!v->standalone)
                {
                    v->positions = new TPartPosition[4];
                    memset(v->positions, 0, sizeof(TPartPosition)*4);
                    for (int k = 0; k < 4; ++k)
                    {
                        TPartPosition *p = v->positions + k;
                        fread(&p->variant, sizeof(byte), 1, f);
                        fread(&p->width, sizeof(word), 1, f);
                        fread(&p->height, sizeof(word), 1, f);
                        fread(&p->x, sizeof(short), 1, f);
                        fread(&p->y, sizeof(short), 1, f);
                    }
                }
                else
                {
                    if (v->strokecount)
                    {
                        v->strokes = new TElementStroke[v->strokecount];
                        memset(v->strokes, 0, sizeof(TElementStroke)*v->strokecount);
                        for (int k = 0; k < v->strokecount; ++k)
                        {
                            TElementStroke *t = v->strokes + k;
                            fread(&t->pointcount, sizeof(byte), 1, f);
                            t->points = new TElementPoint[t->pointcount];
                            memset(t->points, 0, sizeof(TElementPoint)*t->pointcount);
                            for (int l = 0; l < t->pointcount; l++)
                            {
                                TElementPoint *ep = t->points + l;
                                fread(&ep->x, sizeof(short), 1, f);
                                fread(&ep->y, sizeof(short), 1, f);
                                fread(&ep->c1x, sizeof(short), 1, f);
                                fread(&ep->c1y, sizeof(short), 1, f);
                                fread(&ep->c2x, sizeof(short), 1, f);
                                fread(&ep->c2y, sizeof(short), 1, f);
                                fread(&ep->type, sizeof(TPointType), 1, f);
                            }
                            if (version >= 5)
                                fread(&t->tips, sizeof(byte), 1, f);
                        }
                    }
                }
            }
            if ((version >= 2 && e->owner < NO_OWNER || version >= 4 && e->owner != NO_OWNER) && e->variantcount && e->variants[0].strokecount)
            {
                e->recdata = new TRecognizeData[e->variants[0].strokecount];
                for (int ix = 0; ix < e->variants[0].strokecount; ++ix)
                    e->recdata[ix].pos = new Bits[e->variants[0].strokecount - 1];
            }

            if (version >= 2 && e->recdata)
            {
                for (int ix = 0; ix < e->variants[0].strokecount; ++ix)
                {
                    fread(&e->recdata[ix].data.index, sizeof(word), 1, f);
                    fread(&e->recdata[ix].data.distance, sizeof(int), 1, f);

                    for (int iy = 0; iy < e->variants[0].strokecount - 1; ++iy)
                        e->recdata[ix].pos[iy].LoadFromFile(f);
                }
            }
        }

        if (version >= 2)
        {
            models->LoadFromFile(f, version, true);
            EnableRecognize();
        }
        if (version >= 4)
            cmodels->LoadFromFile(f, version, false);

        if (version >= 3)
            repositories->LoadFromFile(f, version);

        if (version >= 7)
        {
            fread(&w, sizeof(word), 1, f);
            for (int ix = 0; ix < w; ++ix)
            {
                unsigned int code;
                fread(&code, sizeof(unsigned int), 1, f);

                word clen;
                fread(&clen, sizeof(word), 1, f);
                TElementNames elem;
                elem.names = new wchar_t[clen]; //clen includes 0
                fread(elem.names, sizeof(wchar_t), clen, f);
                elem.count = 0;
                for (int k = 0; k < clen; ++k)
                {
                    if (elem.names[k] == 0)
                        elem.count++;
                }

                elementnames.Items[code] = elem;
            }
        }
    }
    catch (...)
    {
        fclose(f);
        Clear();
        throw;
    }
    fclose(f);

    if (!models->Count)
    {
        DisableRecognize();
        if (!models->Count && FileExists(ExtractFileDir(ParamStr(0)) + "\\data\\models.zsm"))
        {
            try
            {
                models->ReadModels(ExtractFileDir(ParamStr(0)) + "\\data\\models.zsm");
            }
            catch (...)
            {
                models->Clear();
            }
        }
    }
    if (models->Count)//{
                EnableRecognize();
//fversion = currentversion;
//}
}
//---------------------------------------------------------------------------
int TElementList::MeasureBit(TRect r, double width, short element, char variant, char stroke, int speed, TDoublePoint *startpoint, TDrawBitDirection *dir)
{
    if (element < 0 || element >= Count)
        THROW(L"Element out of range!");

    if (speed < 0)
        speed = (r.Width() + r.Height()) / 40;

    if (startpoint)
        * startpoint = DoublePoint(-1, -1);
    if (dir)
        * dir = dbdUnset;

    return DrawElement(r, NULL, width, 0, element, variant, stroke, stroke, true, false, speed, -1, startpoint, dir);
}
//---------------------------------------------------------------------------
int TElementList::DrawBit(TRect r, HDC handle, double width, int color, short element, char variant, char stroke, int bit, int speed, TDoublePoint *startpoint, TDrawBitDirection *dir, TRect *bitrect)
{
    if (speed < 0)
        speed = (r.Width() + r.Height()) / 40;

    if (startpoint)
        * startpoint = Point(-1, -1);
    if (dir)
        * dir = dbdUnset;
    if (bitrect)
        * bitrect = Rect(-1, -1, -1, -1);

    return DrawElement(r, handle, width, color, element, variant, stroke, stroke, true, true, speed, bit, startpoint, dir, bitrect);
}
//---------------------------------------------------------------------------
void TElementList::Draw(TRect r, HDC handle, double width, int color, short element, char variant, char from, char to, TDoublePoint *startpoint, TDrawBitDirection *dir)
{
    if (to < from)
        return;

    if (element < 0 || element >= Count || variant < 0 || variant >= Items[element]->variantcount)
        THROW(L"Bad variant index for element drawing!");

    TKanjiVariant *v = Variants[element][variant];

    TRect bak = r;

    if (from < 0)
        from = 0;
    else if (from >= v->strokecount)
        from = v->strokecount - 1;

    if (to < 0)
        to = v->strokecount - 1;
    else if (to >= v->strokecount)
        to = v->strokecount - 1;

    if (startpoint)
        * startpoint = Point(-1, -1);
    if (dir)
        * dir = dbdUnset;

    DrawElement(r, handle, width, color, element, variant, from, to, startpoint, dir);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*TRect zline(TCanvas *c, TDoublePoint p1, TDoublePoint p2, int bits, int bit)
    {
      TRect r = Rect(-1,-1,-1,-1);
      TDoublePoint pp1,pp2;

//      TDoublePoint origin;

      if (bits == 1) {

        c->MoveTo(round(p1.x),round(p1.y));
        c->LineTo(round(p2.x),round(p2.y));

        pp1 = p1;
        pp2 = p2;
      } else {
        bit = MAX(0,MIN(bits,bit));
        double dx = (p2.x-p1.x)/(double)bits;
        double dy = (p2.y-p1.y)/(double)bits;

        pp1 = Point(round(p1.x+dx*bit),round(p1.y+dy*bit));
        pp2 = Point(round(dx < 0 ? MAX(p1.x+dx*(bit+1),p2.x) : MIN(p1.x+dx*(bit+1),p2.x)),
                    round(dy < 0 ? MAX(p1.y+dy*(bit+1),p2.y) : MIN(p1.y+dy*(bit+1),p2.y)));
        c->MoveTo(round(pp1.x),round(pp1.y));
        c->LineTo(round(pp2.x),round(pp2.y));

      }

      r = Rect(round(MIN(pp1.x,pp2.x)),round(MIN(pp1.y,pp2.y)),round(MAX(pp1.x,pp2.x)),round(MAX(pp1.y,pp2.y)));
      InflateRect(&r,c->Pen->Width/2,c->Pen->Width/2);
      return r;
    }*/
int TElementList::DrawElement(TRect r, HDC handle, double width, int color, short element, char variant, char from, char to, bool bits, bool draw, int speed, int bit, TDoublePoint *startpoint, TDrawBitDirection *dir, TRect *bitrect)
{
    if (element < 0 || element >= Count)
        THROW(L"Element out of range!");

    if (to < from)
        return 0;

    if (bits && speed < 0)
        THROW(L"Bad parameter for kanji bit drawing!");
    if (bits)
        to = from;

    TKanjiElement *e = Items[element];
    TKanjiVariant *v = Variants[element][variant];

    if (from < 0 || from >= v->strokecount || to >= v->strokecount)
        throw ERangeError("Out of range for variant stroke range!");

    TDoublePoint pp;
    TRect r2;
    double wdelta = (double)r.Width() / 10000.0;
    double hdelta = (double)r.Height() / 10000.0;

    int bitcnt = 0;

    if (!v->standalone)
    {
        int strokepos = 0;
        for (int ix = 0; ix < 4 && e->parts[ix] >= 0 && strokepos <= to; ++ix)
        {
            TPartPosition *ppos = &v->positions[ix];
            TKanjiElement *epart = Items[e->parts[ix]];
            while (ppos->variant >= epart->variantcount)
                --ppos->variant;

            TKanjiVariant *vpart = &epart->variants[ppos->variant];
            if (!vpart->width || !vpart->height)
                Reposition(e->parts[ix], false);

            bool nocenter = (e->pattern < epEbl || e->pattern > epElr);
            strokepos += (vpart->centrepoint && !nocenter ? vpart->centrepoint - 1 : vpart->strokecount);

            if (strokepos < from)
                continue;

            r2.Left = (double)r.Left + ((double)ppos->x - (double)vpart->x * ppos->width / vpart->width) * r.Width() / 10000.0;
            r2.Top = (double)r.Top + ((double)ppos->y - (double)vpart->y * ppos->height / vpart->height) * r.Height() / 10000.0;
            r2.Right = (double)r2.Left + (double)ppos->width * r.Width() / vpart->width;
            r2.Bottom = (double)r2.Top + (double)ppos->height * r.Height() / vpart->height;

            int s = (vpart->centrepoint && !nocenter ? vpart->centrepoint - 1 : vpart->strokecount);
            if (s)
                bitcnt += DrawElement(r2, handle, width, color, e->parts[ix], v->positions[ix].variant, MAX(0, from - (strokepos - s)), MIN(s - 1, s - (strokepos - to)), bits, draw, speed, bit, startpoint, dir, bitrect);
        }

        if (e->pattern >= epEbl && e->pattern <= epElr)//only use centrepoint when element has any of these patterns
            for (int ix = 3; ix >= 0 && strokepos <= to; ix--)
            {
                if (e->parts[ix] < 0)
                    continue;

                TPartPosition *ppos = &v->positions[ix];
                TKanjiElement *epart = Items[e->parts[ix]];
                TKanjiVariant *vpart = &epart->variants[ppos->variant];
                if (!vpart->centrepoint)
                    continue;

                strokepos += vpart->strokecount - vpart->centrepoint + 1;

                if (strokepos < from)
                    continue;

                r2.Left = (double)r.Left + ((double)ppos->x - (double)vpart->x * ppos->width / vpart->width) * r.Width() / 10000.0;
                r2.Top = (double)r.Top + ((double)ppos->y - (double)vpart->y * ppos->height / vpart->height) * r.Height() / 10000.0;
                r2.Right = (double)r2.Left + (double)ppos->width * r.Width() / vpart->width;
                r2.Bottom = (double)r2.Top + (double)ppos->height * r.Height() / vpart->height;

                int s = vpart->strokecount; //-vpart->centrepoint+1;
                bitcnt += DrawElement(r2, handle, width, color, e->parts[ix], v->positions[ix].variant, MAX(0, from - (strokepos - s)), MIN(s - 1, s - (strokepos - to)), bits, draw, speed, bit, startpoint, dir, bitrect);
            }
    }
    else
    {
        for (int ix = from; ix <= to; ++ix)
        {
            for (int k = 0; k < v->strokes[ix].pointcount; ++k)
            {
                TElementPoint *p = &v->strokes[ix].points[k];
                TDoublePoint pt, ptc1, ptc2;

                TZStrokeType starttip = stNormalTip;
                TZStrokeType endtip = stNormalTip;
                if (k == 1)
                {
                    if (v->strokes[ix].tips == (byte)stSingleDot)
                        starttip = stSingleDot;
                    else if (v->strokes[ix].tips == (byte)stStartPointed || v->strokes[ix].tips == (byte)stStartPointedEndPointed || v->strokes[ix].tips == (byte)stStartPointedEndThin)
                        starttip = stStartPointed;
                    else if (v->strokes[ix].tips == (byte)stStartThin || v->strokes[ix].tips == (byte)stStartThinEndPointed || v->strokes[ix].tips == (byte)stStartThinEndThin)
                        starttip = stStartThin;
                }
                if (k == v->strokes[ix].pointcount - 1)
                {
                    if (v->strokes[ix].tips == (byte)stSingleDot);
                    else if (v->strokes[ix].tips == (byte)stEndPointed || v->strokes[ix].tips == (byte)stStartThinEndPointed || v->strokes[ix].tips == (byte)stStartPointedEndPointed)
                        endtip = stEndPointed;
                    else if (v->strokes[ix].tips == (byte)stEndThin || v->strokes[ix].tips == (byte)stStartThinEndThin || v->strokes[ix].tips == (byte)stStartPointedEndThin)
                        endtip = stEndThin;
                }

                pt.x = wdelta * (double)p->x + r.left;
                pt.y = hdelta * (double)p->y + r.top;
                ptc1.x = wdelta * (double)p->c1x + r.left;
                ptc1.y = hdelta * (double)p->c1y + r.top;
                ptc2.x = wdelta * (double)p->c2x + r.left;
                ptc2.y = hdelta * (double)p->c2y + r.top;

                if (startpoint && fnear(startpoint->x, -1) && fnear(startpoint->y, -1))
                    * startpoint = DoublePoint(pt.x, pt.y);

                double dirx, diry, dirxv, diryv;
                TDoublePoint dirp;
                switch (p->type)
                {
                case ptMoveTo:
        //c->MoveTo(round(pt.x),round(pt.y));

                            pp = pt;
                    break;
                case ptLineTo:
                    if (dir && *dir == dbdUnset)
                    {
                        dirx = (pt.x - pp.x);
                        diry = (pt.y - pp.y);
                        dirxv = fabs(diry ? dirx / diry : dirx);
                        diryv = fabs(dirx ? diry / dirx : diry);
                        if (dirxv < 0.06)
                        {
                            *dir = (diry < 0 ? dbdUp : dbdDown);
                        }
                        else if (diryv < 0.1)
                        {
                            *dir = (dirx < 0 ? dbdLeft : dbdRight);
                        }
                        else
                        {
                            *dir = (diry < 0 ? (dirx < 0 ? dbdUpLeft : dbdUpRight) : (dirx < 0 ? dbdDownLeft : dbdDownRight));
                        }
                    }
                    if (bits)
                    {
                        int ln = MAX(1.0, zpointdistance(pp, pt) / (double)speed);
                        if (draw && bit >= bitcnt && bit < bitcnt + ln)
                        {
                            TRect tmp = zline(handle, width, color, pp, pt, starttip, endtip, ln, bit - bitcnt);
                            if (bitrect)
                            {
                                if (*bitrect == Rect(-1, -1, -1, -1))
                                    * bitrect = tmp;
                                else
                                    *bitrect = Rect(MIN(bitrect->Left, tmp.Left), MIN(bitrect->Top, tmp.Top), MAX(bitrect->Right, tmp.Right), MAX(bitrect->Bottom, tmp.Bottom));
                            }
                        }
                        bitcnt += ln;

                    }
                    else
                    {
//c->LineTo(round(pt.x),round(pt.y));
                        zline(handle, width, color, pp, pt, starttip, endtip);
                    }
                    pp = pt;
                    break;
                case ptCurve:
                    if (dir && *dir == dbdUnset)
                    {
                        dirp = DoublePoint((ptc1.x + ptc2.x) / 2.0, (ptc1.y + ptc2.y) / 2.0);

                        dirx = (dirp.x - pp.x);
                        diry = (dirp.y - pp.y);
                        dirxv = fabs(diry ? dirx / diry : dirx);
                        diryv = fabs(dirx ? diry / dirx : diry);
                        if (dirxv < 0.06)
                        {
                            *dir = (diry < 0 ? dbdUp : dbdDown);
                        }
                        else if (diryv < 0.1)
                        {
                            *dir = (dirx < 0 ? dbdLeft : dbdRight);
                        }
                        else
                        {
                            *dir = (diry < 0 ? (dirx < 0 ? dbdUpLeft : dbdUpRight) : (dirx < 0 ? dbdDownLeft : dbdDownRight));
                        }
                    }

                    if (bits)
                    {
                        int ln = MAX(1.0, zbezierlen(pp, ptc1, ptc2, pt) / (double)speed);
                        if (draw && bit >= bitcnt && bit < bitcnt + ln)
                        {
                            TRect tmp = zbezier(handle, width, color, pp, ptc1, ptc2, pt, starttip, endtip, ln, bit - bitcnt);
                            if (bitrect)
                            {
                                if (*bitrect == Rect(-1, -1, -1, -1) || tmp == Rect(-1, -1, -1, -1))
                                    * bitrect = tmp;
                                else
                                    *bitrect = Rect(MIN(bitrect->Left, tmp.Left), MIN(bitrect->Top, tmp.Top), MAX(bitrect->Right, tmp.Right), MAX(bitrect->Bottom, tmp.Bottom));
                            }
                        }
                        bitcnt += ln;
                    }
                    else
                    {
                        zbezier(handle, width, color, pp, ptc1, ptc2, pt, starttip, endtip);
                    }
                    pp = pt;
                    break;
                }
            }
        }
    }

    return bitcnt;
}
//---------------------------------------------------------------------------

struct element_parentitem
{
    short aelem;
    byte avariant;
    int steps;
};
int sort_parents_by_steps_element(element_parentitem **a, element_parentitem **b)
{
    return (*a)->steps - (*b)->steps;
}

void TElementList::__Reposition(short aelement, byte avariant, TGapList<int> &gl, bool reparts)
{
    if (gl.Items[aelement] & (1 << avariant))
        return;

    gl.Items[aelement] |= (1 << avariant);

    TKanjiElement *ke = Items[aelement];

    TRect dim;
    try
    {
        if (!avariant)
            DeleteRecognizeData(aelement);

        TKanjiVariant *kv = &ke->variants[avariant];

        long &l = dim.Left;
        long &r = dim.Right;
        long &t = dim.Top;
        long &b = dim.Bottom;
        l = 99999, r = 0, t = 99999, b = 0;

        if (kv->standalone)
        {
            for (int j = 0; j < kv->strokecount; ++j)
            {
                TElementStroke *es = &kv->strokes[j];
                for (int k = 0; k < es->pointcount; ++k)
                {
                    if (es->points[k].type == ptMoveTo || es->points[k].type == ptLineTo)
                    {
                        l = min(l, (long)es->points[k].x);
                        t = min(t, (long)es->points[k].y);
                        r = max(l + 1, max(r, (long)es->points[k].x));
                        b = max(t + 1, max(b, (long)es->points[k].y));
                    }
                    else if (es->points[k].type == ptCurve)
                    {
                        TDoubleRect dr = zbezierdimensions(DoublePoint(es->points[k - 1].x, es->points[k - 1].y), DoublePoint(es->points[k].c1x, es->points[k].c1y), DoublePoint(es->points[k].c2x, es->points[k].c2y), DoublePoint(es->points[k].x, es->points[k].y));
                        l = min(l, (long)dr.Left);
                        t = min(t, (long)dr.Top);
                        r = max(l + 1, max(r, (long)dr.Right));
                        b = max(t + 1, max(b, (long)dr.Bottom));
                    }
                    else
                        THROW(L"We are not ready for this!");

                }
            }
/*if (l != 99999) {
        int w = MAX(3000.0,(((double)(r-l)+999.0)/1000.0)*1000.0);
        int dl = (w-(r-l))/2;
        l -= dl;
        r = l+w;

        int h = MAX(3000.0,(((double)(b-t)+999.0)/1000.0)*1000.0);
        int dr = (h-(b-t))/2;
        t -= dr;
        b = t+h;
      }*/
        }
        else
        { //!standalone
            for (int j = 0; j < 4 && ke->parts[j] >= 0; ++j)
            {
                l = MIN(l, (long) kv->positions[j].x);
                t = MIN(t, (long) kv->positions[j].y);
                r = MAX(r, (long) kv->positions[j].x + kv->positions[j].width/*-1*/);
                b = MAX(b, (long) kv->positions[j].y + kv->positions[j].height/*-1*/);
            }

            kv->strokecount = 0;
            for (int j = 0; j < 4 && ke->parts[j] >= 0; ++j)
                kv->strokecount += Items[ke->parts[j]]->variants[kv->positions[j].variant].strokecount;
        }

        if (avariant == 0)
            GenerateRecognizeData(aelement);

        if (reparts)
        {
            for (int ix = ke->parentcnt - 1; ix >= 0; --ix)
            {
                TKanjiElement *parent = Items[ke->parents[ix]];

                for (int j = 0; j < parent->variantcount; ++j)
                {
                    TKanjiVariant *parentkv = &parent->variants[j];
                    if (parentkv->standalone)
                        continue;
                    for (int k = 0; k < 4 && parent->parts[k] >= 0; ++k)
                    {
                        if (parent->parts[k] != aelement || parentkv->positions[k].variant != avariant)//dim[parentkv->positions[k].variant].Left == 99999 )
                                    continue;

                        parentkv->positions[k].x += (double)(dim.Left - kv->x) * parentkv->positions[k].width / kv->width;
                        parentkv->positions[k].y += (double)(dim.Top - kv->y) * parentkv->positions[k].height / kv->height;
                        parentkv->positions[k].width *= (double)dim.Width() / kv->width;
                        parentkv->positions[k].height *= (double)dim.Height() / kv->height;
                    }
                }
            }
        }

        kv->x = dim.Left;
        kv->y = dim.Top;
        kv->width = dim.Width();
        kv->height = dim.Height();

        TTemplateList<element_parentitem, true> *parents = new TTemplateList<element_parentitem, true>;
        try
        {
            for (int ix = 0; ix < ke->parentcnt; ++ix)
            {
                for (int j = 0; j < elements->Items[ke->parents[ix]]->variantcount; ++j)
                {
                    int k = StepsRequired(ke->parents[ix], j, aelement, avariant);
                    if (k)
                    {
                        element_parentitem *pi = new element_parentitem;
                        pi->steps = k;
                        pi->aelem = ke->parents[ix];
                        pi->avariant = j;
                        parents->Add(pi);
                    }
                }
            }

            parents->Sort(sort_parents_by_steps_element);

            while (parents->Count)
            {
                element_parentitem* pi = parents->Items[0];
                __Reposition(pi->aelem, pi->avariant, gl, reparts);
                parents->Delete(0);
                //delete pi;
            }
        }
        catch (...)
        {
            ;
        }
        delete parents;
    }
    catch (...)
    {
        ;
    }
}
//---------------------------------------------------------------------------
int TElementList::StepsRequired(short parentelement, byte parentvariant, short toelement, short tovariant)
{
    int steps = 0;

    TKanjiElement *ke = Items[parentelement];
    TKanjiVariant *kv = &ke->variants[parentvariant];

    if (kv->standalone)
        return 0;

    for (int ix = 0; ix < 4 && ke->parts[ix] >= 0; ++ix)
    {
        if (ke->parts[ix] == toelement && (tovariant < 0 || kv->positions[ix].variant == tovariant))
        {
            steps = MAX(steps, 1);
            continue;
        }

        int sr = StepsRequired(ke->parts[ix], kv->positions[ix].variant, toelement, tovariant);
        if (sr)
            steps = MAX(steps, sr + 1);
    }

    return steps;
}
//---------------------------------------------------------------------------
bool TElementList::HasPart(short element, short part)
{
    TKanjiElement *e = Items[element];
    bool has = false;
    for (int ix = 0; ix < 4 && e->parts[ix] >= 0 && !has; ++ix)
    {
        if (e->parts[ix] == part)
            has = true;
        else
            has = HasPart(e->parts[ix], part);
    }
    return has;
}
//---------------------------------------------------------------------------
void TElementList::Reposition(short aelement, bool reparts)
{
    TGapList<int> glist(false);
    glist.SetAll(Count, 0);

    for (int ix = 0; ix < Items[aelement]->variantcount; ++ix)
        __Reposition(aelement, ix, glist, reparts);
}
//---------------------------------------------------------------------------
void TElementList::__RepositionAllHelper(int aelement, TGapList<int> &gl)
{

    TKanjiElement *ke = elements->Items[aelement];

    for (int ix = 0; ix < 4 && ke->parts[ix] >= 0; ++ix)
        __RepositionAllHelper(ke->parts[ix], gl);

    for (int ix = 0; ix < ke->variantcount; ++ix)
        if (ke->variants[ix].standalone)
            __Reposition(aelement, ix, gl, true);
}

void TElementList::RepositionAll()
{
    TGapList<int> glist(false);
    glist.SetAll(Count, 0);

    for (int ix = 0; ix < Count; ++ix)
        __RepositionAllHelper(ix, glist);
}
//---------------------------------------------------------------------------
void TElementList::DeleteElement(short element)
{
    if (Items[element]->parentcnt > 0)
        THROW(L"Cannot delete element that is part of other elements!");

    for (int ix = 0; ix < 4 && Items[element]->parts[ix] >= 0; ++ix)
        RemoveParent(Items[element]->parts[ix], element);

    DeleteVariantNames(element);

    Delete(element);
    DecrementElement(element);
}
//---------------------------------------------------------------------------
void TElementList::DecrementElement(short aelement)
{
    ElementRemoved(aelement);

    for (int ix = 0; ix < kanjis->Count; ++ix)
    {
        if (kanjis->Kanjis[ix]->element > aelement)
            --kanjis->Kanjis[ix]->element;
    }

    for (int ix = 0; ix < Count; ++ix)
    {
        TKanjiElement *e = Items[ix];
        for (int j = 0; j < e->parentcnt; ++j)
        {
            if (e->parents[j] > aelement)
                --e->parents[j];
        }

        for (int j = 0; j < 4 && e->parts[j] >= 0; ++j)
        {
            if (e->parts[j] > aelement)
                --e->parts[j];
        }
    }

}
//---------------------------------------------------------------------------
int TElementList::AddRepository(UnicodeString aname)
{
    return repositories->Add(aname);
}
//---------------------------------------------------------------------------
int TElementList::ToRepository(int arepos, word aelem, byte avar)
{
    return repositories->Add(arepos, aelem, avar);
}
//---------------------------------------------------------------------------
void TElementList::DeleteRepository(int arepos)
{
    repositories->Delete(arepos);
}
//---------------------------------------------------------------------------
void TElementList::RemoveFromRepository(int arepos, int pos)
{
    repositories->RemoveItem(arepos, pos);
}
//---------------------------------------------------------------------------
void TElementList::ElementRemoved(short aelement)
{
    repositories->ElementRemoved(aelement);
}
//---------------------------------------------------------------------------
void TElementList::Clear()
{
    inherited::Clear();

    repositories->Clear();

    for (unsigned int ix = 0; ix < elementnames.TrueCount; ++ix)
        delete[] elementnames.TrueItems[ix].names;
    elementnames.Clear();
}
//---------------------------------------------------------------------------
void TElementList::Merge(short srcelem, short destelem)
{
    for (int ix = 0; ix < 4 && (Items[srcelem]->parts[ix] >= 0 || Items[destelem]->parts[ix] >= 0); ++ix)
    {
        if (Items[srcelem]->parts[ix] != Items[destelem]->parts[ix])
            THROW(L"Error! Parts do not match!");

        RemoveParent(Items[srcelem]->parts[ix], srcelem);
    }

    TKanjiElement *src = Items[srcelem];
    TKanjiElement *dest = Items[destelem];

    TKanjiVariant *tmp = dest->variants;
    try
    {
        dest->variants = new TKanjiVariant[dest->variantcount + src->variantcount];
        memcpy(dest->variants, tmp, sizeof(TKanjiVariant)*dest->variantcount);
        memcpy(dest->variants + dest->variantcount, src->variants, sizeof(TKanjiVariant)*src->variantcount);
    }
    catch (...)
    {
        TKanjiVariant *bak = dest->variants;
        dest->variants = tmp;
        delete[] bak;
        throw;
    }
    delete[] tmp;

    for (int ix = 0; ix < src->parentcnt; ++ix)
    {
        AddParent(destelem, src->parents[ix]);
        TKanjiElement *parent = Items[src->parents[ix]];
        for (int iy = 0; iy < 4 && parent->parts[iy] >= 0; ++iy)
        {
            if (parent->parts[iy] == srcelem)
            {
                parent->parts[iy] = destelem;
                for (int z = 0; z < parent->variantcount; ++z)
                    parent->variants[z].positions[iy].variant += dest->variantcount;
            }
        }
    }
    for (int ix = 0; ix < src->variantcount; ++ix)
        repositories->VariantChanged(srcelem, ix, destelem, ix + dest->variantcount);

    dest->variantcount += src->variantcount;

    src->variantcount = 0;

    Delete(srcelem);
    DecrementElement(srcelem);
}
//---------------------------------------------------------------------------
void TElementList::Duplicate(short element)
{
    TKanjiElement *e = elem_copy(Items[element], false);
    e->owner = NO_OWNER;
    delete[] e->parents;
    e->parentcnt = 0;
    e->parents = NULL;

    Add(e);
}
//---------------------------------------------------------------------------
UnicodeString TElementList::VariantName(short aelement, byte avar, bool global_accepted)
{
    if (avar >= Items[aelement]->variantcount)
        THROW(L"Variant index out of range");

    if (Items[aelement]->owner == NO_OWNER)
        return L"";
    wchar_t which = Items[aelement]->owner < NO_OWNER ? kanjis->Kanjis[Items[aelement]->owner]->ch : Items[aelement]->owner;
    if (!elementnames.ItemSet(which) || !elementnames.Items[which].count || elementnames.Items[which].count <= avar && !global_accepted)
        return L"";

    const wchar_t *name = elementnames.Items[which].names;
    if (elementnames.Items[which].count <= avar)
        return name;

    while (avar)
    {
        name = wcschr(name, 0) + 1;
        avar--;
    }
    return name;
}
//---------------------------------------------------------------------------
void TElementList::RemoveVariantName(short aelement, byte avar)
{
    if (avar >= Items[aelement]->variantcount)
        THROW(L"Variant index out of range");

    if (Items[aelement]->owner == NO_OWNER)
        return;

    wchar_t which = Items[aelement]->owner < NO_OWNER ? kanjis->Kanjis[Items[aelement]->owner]->ch : Items[aelement]->owner;
    if (!elementnames.ItemSet(which) || elementnames.Items[which].count <= avar)
        return;

    if (elementnames.Items[which].count == 1)
    {
        DeleteVariantNames(aelement);
        return;
    }

    wchar_t *tmp = elementnames.Items[which].names;
    wchar_t *vname = VariantNameStart(aelement, avar);
    int vlen = wcslen(vname);
    int len = VariantNamesLength(aelement);
    wchar_t *names = new wchar_t[len - vlen];
    memcpy(names, tmp, sizeof(wchar_t)*(vname - tmp));
    memcpy(names + (vname - tmp), vname + vlen, sizeof(wchar_t)*(len - (vname - tmp) - vlen));

    delete[] tmp;
    elementnames.Items[which].names = names;
    elementnames.Items[which].count--;
}
//---------------------------------------------------------------------------
void TElementList::SetVariantName(short aelement, byte avar, UnicodeString newname)
{
    if (avar >= Items[aelement]->variantcount)
        THROW(L"Variant index out of range");

    if (Items[aelement]->owner == NO_OWNER)
        return;

    wchar_t which = Items[aelement]->owner < NO_OWNER ? kanjis->Kanjis[Items[aelement]->owner]->ch : Items[aelement]->owner;

    if (!elementnames.ItemSet(which))
    {
        if (!newname.Length())
            return;

        TElementNames n;
        n.count = 0;
        n.names = NULL;
        elementnames.Items[which] = n;
    }

    if (elementnames.Items[which].count <= avar)
    {
        if (!newname.Length())
            return;

        wchar_t *tmp = elementnames.Items[which].names;
        int len = VariantNamesLength(aelement);
        int skip = avar - elementnames.Items[which].count;
        wchar_t *name = new wchar_t[len + skip + newname.Length() + 1];
        if (len)
            memcpy(name, tmp, sizeof(wchar_t)*len);
        for (int ix = 0; ix < skip; ++ix)
            name[len + ix] = 0;
        memcpy(name + len + skip, newname.c_str(), sizeof(wchar_t)*(newname.Length() + 1));

        elementnames.Items[which].names = name;
        elementnames.Items[which].count += skip + 1;

        delete[] tmp;
        return;
    }

    if (!newname.Length() && avar == elementnames.Items[which].count - 1)
    {
        elementnames.Items[which].count--;
        while (elementnames.Items[which].count && !wcslen(VariantNameStart(aelement, elementnames.Items[which].count - 1)))
            elementnames.Items[which].count--;
        if (!elementnames.Items[which].count)
        {
            delete[] elementnames.Items[which].names;
            elementnames.ClearIndex(which);
        }
        return;
    }

    int len = VariantNamesLength(aelement);
    wchar_t *vname = VariantNameStart(aelement, avar);
    int vlen = wcslen(vname);
    wchar_t *tmp = elementnames.Items[which].names;
    wchar_t *name = new wchar_t[len - vlen + newname.Length()];
    memcpy(name, tmp, vname - tmp);
    memcpy(name + (vname - tmp), newname.c_str(), sizeof(wchar_t)*(newname.Length()));
    memcpy(name + (vname - tmp) + newname.Length(), vname + vlen, sizeof(wchar_t)*(len - (vname - tmp) - vlen));

    elementnames.Items[which].names = name;

    delete[] tmp;
}
//---------------------------------------------------------------------------
void TElementList::DeleteVariantNames(short aelement)
{
    if (Items[aelement]->owner == NO_OWNER)
        return;

    wchar_t which = Items[aelement]->owner < NO_OWNER ? kanjis->Kanjis[Items[aelement]->owner]->ch : Items[aelement]->owner;

    if (elementnames.ItemSet(which))
        delete[] elementnames.Items[which].names;

    elementnames.ClearIndex(which);
}
//---------------------------------------------------------------------------
int TElementList::VariantNamesLength(int element)
{
    if (Items[element]->owner == NO_OWNER)
        return 0;

    wchar_t which = Items[element]->owner < NO_OWNER ? kanjis->Kanjis[Items[element]->owner]->ch : Items[element]->owner;

    if (!elementnames.ItemSet(which) || !elementnames.Items[which].count)
        return 0;

    byte c = elementnames.Items[which].count;
    const wchar_t *name = elementnames.Items[which].names;
    while (c)
    {
        name = wcschr(name, 0) + 1;
        c--;
    }
    return name - elementnames.Items[which].names;
}
//---------------------------------------------------------------------------
wchar_t* TElementList::VariantNameStart(int element, byte avar)
{
    if (avar >= Items[element]->variantcount)
        THROW(L"Variant index out of range");

    if (Items[element]->owner == NO_OWNER)
        return 0;

    wchar_t which = Items[element]->owner < NO_OWNER ? kanjis->Kanjis[Items[element]->owner]->ch : Items[element]->owner;

    if (!elementnames.ItemSet(which) || elementnames.Items[which].count <= avar)
        return NULL;
    wchar_t *name = elementnames.Items[which].names;
    while (avar)
    {
        name = wcschr(name, 0) + 1;
        avar--;
    }
    return name;
}
//---------------------------------------------------------------------------
int TElementList::IndexOfChar(wchar_t ch)
{
    for (int ix = 0; ix < Count; ++ix)
    {
        if (Items[ix]->owner == ch)
            return ix;
    }
    return -1;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(79);
#endif
