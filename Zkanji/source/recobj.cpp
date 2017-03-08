//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Math.h>
#include "recobj.h"
#include "elementpoints.h"
#include "angle.h"
#include "bits.h"
#include "qsrt.h"
#include "kanjielement.h"
#include "zkformats.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(126);
#endif

extern TStrokeModels *models = NULL;
extern TStrokeModels *cmodels = NULL;

//---------------------------------------------------------------------------
void TRecPointList::FreeItem(TRecPointData *item, int ix)
{
    delete item->points;
    delete item->original;
    delete[] item->pos;
    delete[] item->data;
    delete[] item->cdata;
    delete item;
}
//---------------------------------------------------------------------------
void TRecPointList::Clear()
{
    dimensions = Rect(0, 0, 0, 0);
    inherited::Clear();
}
//---------------------------------------------------------------------------
void TRecPointList::remove(int ix)
{
    Delete(ix);

    if (Count)
        dimensions = Rect(Items[0]->original->Dimension.Left, Items[0]->original->Dimension.Top, Items[0]->original->Dimension.Right + 1, Items[0]->original->Dimension.Bottom + 1);
    else
        dimensions = Rect(0, 0, 0, 0);
    for (int iy = 1; iy < Count; ++iy)
        UnionRect(dimensions, TRect(dimensions), Rect(Items[iy]->original->Dimension.Left, Items[iy]->original->Dimension.Top, Items[iy]->original->Dimension.Right + 1, Items[iy]->original->Dimension.Bottom + 1));

    for (int ix = 0; ix < Count; ++ix)
    {
        TRecPointData *item = Items[ix];
        Bits *tmp = item->pos;
        item->pos = new Bits[Count - 1];
        delete[] tmp;
    }

    for (int ix = 0; ix < Count; ++ix)
    {
        TRecPointData *item = Items[ix];
        for (int iy = 0; iy < Count; ++iy)
        {
            if (ix == iy)
                continue;
            TRecPointData *item2 = Items[iy];
            ProcessDimensions(item->pos[iy > ix ? iy - 1 : iy], item->original->Dimension, item2->original->Dimension, Rect(0, 0, max(width, 6500), max(height, 6500)));
            ProcessDimensions(item2->pos[ix > iy ? ix - 1 : ix], item2->original->Dimension, item->original->Dimension, Rect(0, 0, max(width, 6500), max(height, 6500)));
        }
    }

}
//---------------------------------------------------------------------------
//int __rec_data_sort(TCompareData const & d) {
//return d.distance;
//}
int TRecPointList::add(TPointListEx *pt)
{
    TRecPointData *data = new TRecPointData;
    memset(data, 0, sizeof(TRecPointData));
    data->points = new TPointListEx;
    data->original = new TPointListEx(pt);

    try
    {
        ProcessStroke(data->original, data->original, true);
    }
    catch (...)
    {
        delete data->points;
        delete data;
        return -1;
    }

    TRect orect = data->original->Dimension;
    if (!Count)
        dimensions = Rect(data->original->Dimension.Left, data->original->Dimension.Top, data->original->Dimension.Right + 1, data->original->Dimension.Bottom + 1);
    else
        UnionRect(dimensions, TRect(dimensions), Rect(data->original->Dimension.Left, data->original->Dimension.Top, data->original->Dimension.Right + 1, data->original->Dimension.Bottom + 1));

    data->original->resize(10000, 10000);
    DissectStroke(data->original, data->points);
    data->original->resize(DoubleRect(orect));

    data->data = new TCompareData[models->Count];
    data->cdata = new TCompareData[cmodels->Count];
    TPointListEx *ptresized = NULL;
    try
    {
        ptresized = new TPointListEx(data->points);
        for (int ix = 0; ix < models->Count; ++ix)
        {
            data->data[ix].distance = models->Items[ix]->compare(ptresized);
            data->data[ix].index = ix;
        }
        for (int ix = 0; ix < cmodels->Count; ++ix)
        {
            data->cdata[ix].distance = cmodels->Items[ix]->compare(ptresized);
            data->cdata[ix].index = ix;
        }

        //quicksort(data->data,0,models->Count-1,&__rec_data_sort);
        //quicksort(data->cdata,0,cmodels->Count-1,&__rec_data_sort);

        Add(data);

        for (int ix = 0; ix < Count; ++ix)
        {
            TRecPointData *item = Items[ix];
            Bits *tmp = item->pos;
            item->pos = new Bits[Count - 1];

            delete[] tmp;
        }

        for (int ix = 0; ix < Count; ++ix)
        {
            TRecPointData *item = Items[ix];
            for (int iy = 0; iy < Count; ++iy)
            {
                if (ix == iy)
                    continue;
                TRecPointData *item2 = Items[iy];
                ProcessDimensions(item->pos[iy > ix ? iy - 1 : iy], item->original->Dimension, item2->original->Dimension, Rect(0, 0, max(width, 6500), max(height, 6500)));
                ProcessDimensions(item2->pos[ix > iy ? ix - 1 : ix], item2->original->Dimension, item->original->Dimension, Rect(0, 0, max(width, 6500), max(height, 6500)));
            }
        }

    }
    catch (...)
    {
        ;
    }
    delete ptresized;

    return Count - 1;
}
//---------------------------------------------------------------------------
word TRecPointList::get_width()
{
    return dimensions.Width();
}
//---------------------------------------------------------------------------
word TRecPointList::get_height()
{
    return dimensions.Height();
}
//---------------------------------------------------------------------------
TPointListEx* TRecPointList::get_point(int ix)
{
    return Items[ix]->points;
}
//---------------------------------------------------------------------------
TPointListEx* TRecPointList::get_orig(int ix)
{
    return Items[ix]->original;
}
//---------------------------------------------------------------------------
int __rec_compare_int(TCompareData *a)
{
    return a->distance;
}

//#define COMPARE_DEBUG    1
#ifdef COMPARE_DEBUG
struct _TStrokeDistance
{
    int stroke;
    int distance;

    int model;

    TGapList<int> posdist;

    _TStrokeDistance(int stroke, int distance) : stroke(stroke), distance(distance), posdist(false)
    {
        ;
    }
};
typedef TTemplateList<_TStrokeDistance, true> _TDistList;

struct _TElemDistance
{
    int elem;
    int extra;
    int s1, s2, s3;
    int distance;

    int strokecount;

    _TDistList *dist;

    _TElemDistance(int elem) : elem(elem), distance(0), extra(0), s1(0), s2(0), s3(0)
    {
        dist = new _TDistList;
    }
    ~_TElemDistance()
    {
        delete dist;
    };
};

#endif
void TRecPointList::compare(int *results, int &cnt, int swplimit, int cntlimit, bool kanji, bool general)
{
    int hasrec = 0;
    int charrec = 0;
    for (int ix = 0; ix < elements->Count; ++ix)
    {
        TKanjiElement *e = elements->Items[ix];
        if (kanji && e->recdata && e->owner < NO_OWNER)
            hasrec++;
        else if (e->recdata && e->owner > NO_OWNER && general)
            charrec++;
    }

    cnt = min(cnt, hasrec + charrec);

#ifdef COMPARE_DEBUG
    TGapList<_TElemDistance*> distances(false);
#endif

    TCompareData *value = new TCompareData[hasrec + charrec];
    for (int ix = 0; ix < hasrec + charrec; ++ix)
    {
        value[ix].index = -1;
        value[ix].distance = 2147483647;
    }

    int pos = 0;
    int lowest = 999999;

    int used[255];
    try
    {
        for (int ix = 0; ix < elements->Count; ++ix)
        {
            TKanjiElement *e = elements->Items[ix];

            if (!e->recdata || e->owner < NO_OWNER && !kanji || (cntlimit >= 0 && abs(e->variants[0].strokecount - Count) > cntlimit || !cntlimit && e->variants[0].strokecount < max(1, min(Count - 3, Count / 2))) || e->owner > NO_OWNER && !general)
                continue;

#ifdef COMPARE_DEBUG
            distances.Items[ix] = new _TElemDistance(ix);
            _TElemDistance *_elem = distances.Items[ix];
            _TDistList* _distlist = _elem->dist;

            _elem->strokecount = e->variants[0].strokecount;
            _elem->s1 = max(0, Count - e->variants[0].strokecount) * 40000;
#endif

            memset(used, -1, sizeof(int)*255);
            value[pos].distance = max(0, Count - e->variants[0].strokecount) * 40000;
            value[pos].index = ix;
            for (int iy = 0; iy < min(e->variants[0].strokecount + swplimit, Count) && value[pos].distance < max(10000, lowest) * 1.5; ++iy)
            {
                int distmin = -1;
                int sindex;
                for (int k = iy - swplimit; k < iy + swplimit + 1; ++k)
                {
                    if (k < 0 || k >= e->variants[0].strokecount || used[k] >= 0 && (k <= 0 || k != iy || used[k - 1] >= 0))
                        continue;
                    double dval;
                    if (e->recdata[k].data.index >= models->Count)
                        dval = Items[iy]->cdata[e->recdata[k].data.index - models->Count].distance / 2;
                    else
                        dval = Items[iy]->data[e->recdata[k].data.index].distance / 2;

                    dval += abs(k - iy) * 300;

                    if (distmin < 0 || distmin > dval)
                    {
                        distmin = dval;
                        sindex = k;

#ifdef COMPARE_DEBUG
                        while (_distlist->Count <= k)
                            _distlist->Add(new _TStrokeDistance(_distlist->Count, k == _distlist->Count ? dval : 0));
                        _distlist->Items[k]->distance = dval;
                        _distlist->Items[k]->stroke = k;
                        _distlist->Items[k]->model = (e->recdata[k].data.index >= models->Count) ? -(e->recdata[k].data.index - models->Count) : e->recdata[k].data.index;
#endif
                    }
                }
                if (distmin < 0)
                    distmin = 0;
                else
                {
                    if (used[sindex] >= 0 && used[sindex - 1] < 0)
                        used[sindex - 1] = sindex - 1;
                    else if (used[sindex] >= 0)
                        THROW(L"???");
                    used[sindex] = iy;
                }
                value[pos].distance += min(100000, distmin);
            }

            int compdist = max(3000, value[pos].distance);

            int n = min(Count, e->variants[0].strokecount);
            double posw;
            for (int iy = 0; iy < n && value[pos].distance < max(10000, lowest) * 1.5; ++iy)
            {
                int c = used[iy];
#ifdef COMPARE_DEBUG
                while (c >= 0 && _distlist->Count <= c)
                    _distlist->Add(new _TStrokeDistance(_distlist->Count, 0));
                _TStrokeDistance *_dist;
                if (c < 0)
                {
                    _elem->extra += (double)compdist * 0.2 * 196 / n;
                }
                else
                {
                    _dist = _distlist->Items[c];
                }

#endif
                if (c < 0)
                {
                    value[pos].distance += (double)compdist * 0.2 * 196 / n; //maximum pos difference. this should be changed if difference changes
                    continue;
                }

                int c2;
                double dval;

                for (int iz = iy + 1; iz < min(n, iy + 3); ++iz)
                {
                    c2 = used[iz];
                    if (c2 >= 0)
                    {
                        posw = iz - iy == 1 ? 0.09 : 0.03;

                        dval = ((double)(pos_difference(e->recdata[iy].pos[iz - 1], Items[c]->pos[c2 - (c2 > c ? 1 : 0)])) * ((double)compdist * posw)) / n;
                        if (swplimit > 0 && iz == iy + 1 && c == iy && c2 == iz && e->recdata[iy].data.index == e->recdata[iz].data.index)
                        {
                            double dtmp = ((double)(pos_difference(e->recdata[iy].pos[iz - 1], Items[c2]->pos[c])) * ((double)compdist * posw)) / n;
                            if (dtmp < dval)
                            {
                                dval = dtmp;
                                int k = c;
                                c = used[iy] = c2;
                                used[iz] = k;
                            }
                        }
                        value[pos].distance += dval;
#ifdef COMPARE_DEBUG
                        _dist->posdist.Items[c2] += dval;
#endif
                    }
                }
                for (int iz = iy - 1; iz >= max(0, iy - 2); --iz)
                {
                    c2 = used[iz];
                    if (c2 >= 0)
                    {
                        posw = iy - iz == 1 ? 0.09 : 0.03;

                        dval = ((double)(pos_difference(e->recdata[iy].pos[iz], Items[c]->pos[c2 - (c2 > c ? 1 : 0)])) * ((double)compdist * posw)) / n;
                        value[pos].distance += dval;
#ifdef COMPARE_DEBUG
                        _dist->posdist.Items[c2] += dval;
#endif
                    }
                }

            }

#ifdef COMPARE_DEBUG
            int lastdistance = value[pos].distance;
#endif
            if (e->variants[0].width < 5000 && e->variants[0].height < 5000)
            {
                if (width < 3500 && height < 3500)
                    value[pos].distance = max(0.0, value[pos].distance - compdist * 0.05);
                else if (width > 5000 || height > 5000)
                    value[pos].distance += compdist * 0.05;
            }
            else if (e->variants[0].width > 5000 && e->variants[0].height > 5000)
            {
                if (width < 3500 && height < 3500)
                    value[pos].distance += compdist * 0.05;
                else if (width > 5000 || height > 5000)
                    value[pos].distance = max(0.0, value[pos].distance - compdist * 0.05);
            }
#ifdef COMPARE_DEBUG
            _elem->extra += (value[pos].distance - lastdistance);
            lastdistance = value[pos].distance;
#endif

            if (e->variants[0].strokecount > Count)
            {
                int d = min(4, e->variants[0].strokecount - Count);
                value[pos].distance += d * 2500 + min((d - 1) * 3333, 10000);
            }

            if (value[pos].distance >= max(10000, lowest) * 1.5)
            {

                if (lowest > value[pos].distance)
                {
                    lowest = value[pos].distance;
#ifdef COMPARE_DEBUG
                    _elem->s2 += (value[pos].distance - lastdistance);
#endif
                }
                else
                {
                    value[pos].index = -1;
                    value[pos].distance = 2147483647;
                }
            }
            else
            {

#ifdef COMPARE_DEBUG
                _elem->s3 += (value[pos].distance - lastdistance);
#endif
                if (lowest > value[pos].distance)
                    lowest = value[pos].distance;

#ifdef COMPARE_DEBUG
                _elem->distance = value[pos].distance;
#endif

                pos++;
            }

        }
    }
    catch (...)
    {
        ;
    }
    quicksort(value, 0, hasrec - 1 + charrec, &__rec_compare_int);
    for (int ix = 0; ix < cnt; ++ix)
    {
        if (value[ix].index == (unsigned short) - 1)
        {
            cnt = ix;
            break;
        }
        results[ix] = value[ix].index;
    }

    delete[] value;

#ifdef COMPARE_DEBUG
#define FormatString UnicodeString
#define FormatWrite fputs
    FILE *f = fopen("D:\\strokedebug.txt", "wb");
    FormatString str;
    try
    {
        for (unsigned int ix = 0; ix < distances.TrueCount; ++ix)
        {
            _TElemDistance *etmp = distances.TrueItems[ix];
            if (f)
            {
                str = FormatString("Elem: ") + etmp->elem + ", distance: " + etmp->distance + "\r\nextra: " + etmp->extra + "\r\n";
                str += FormatString("s1: ") + etmp->s1 + ", s2: " + etmp->s2 + ", s3: " + etmp->s3 + "\r\n\r\nStrokes\r\n";

                for (int iy = 0; iy < etmp->dist->Count; ++iy)
                {
                    _TStrokeDistance *d = etmp->dist->Items[iy];
                    str += FormatString("stroke number: ") + d->stroke + ", distance: " + d->distance + ", model: " + d->model + "\r\npositions\r\n";
                    for (int z = max(0, d->stroke - 1); z <= min(d->stroke + 2, etmp->strokecount); ++z)
                    {
                        int dist = d->posdist.Items[z];
                        str += FormatString(d->stroke) + " -> " + z + ": " + dist + "\r\n";
                    }
                }

                str += "\r\n\r\n";

                FormatWrite(str.c_str(), f);
            }
            delete etmp;
        }
    }
    catch (...)
    {
        ;
    }
    if (f)
        fclose(f);
#endif
}
//---------------------------------------------------------------------------
int TRecPointList::pos_difference(const Bits &p1, const Bits &p2)
{
    int diff = 0;

    byte vert[] = { 0, 4, 8, 12, 15, 11, 7, 3 };
    byte horz[] = { 1, 5, 9, 13, 14, 10, 6, 2 };

    byte minv = 255, minh = 255, minv1 = 255, minh1 = 255;

    for (int ix = 0; ix < 8; ++ix)
    {
        if (p1[vert[ix]])
        {
            minv1 = ix;
            for (int iy = 0; iy < 8; ++iy)
            {
                if (p2[vert[iy]])
                {
                    minv = iy;
                    diff += pow(abs(ix - iy), 2);
                    break;
                }
            }
            break;
        }
    }

    for (int ix = 7; ix >= minv1; --ix)
    {
        if (p1[vert[ix]])
        {
            for (int iy = 7; iy >= minv; --iy)
            {
                if (p2[vert[iy]])
                {
                    diff += pow(abs(ix - iy), 2);
                    break;
                }
            }
            break;
        }
    }

    for (int ix = 0; ix < 8; ++ix)
    {
        if (p1[horz[ix]])
        {
            minh1 = ix;
            for (int iy = 0; iy < 8; ++iy)
            {
                if (p2[horz[iy]])
                {
                    minh = iy;
                    diff += pow(abs(ix - iy), 2);
                    break;
                }
            }
            break;
        }
    }
    for (int ix = 7; ix >= minh1; --ix)
    {
        if (p1[horz[ix]])
        {
            for (int iy = 7; iy >= minh; --iy)
            {
                if (p2[horz[iy]])
                {
                    diff += pow(abs(ix - iy), 2);
                    break;
                }
            }
            break;
        }
    }

    return diff;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void TStrokeModels::ReadModels(UnicodeString fname)
{
    FILE *f = _wfopen(fname.c_str(), L"rb");
    if (!f)
        THROW(L"Error!!!");
    try
    {
        wchar_t line[1024];
        bool bigendian = false;
        while (getlinew(line, 1024, f, bigendian))
        {
            if (line[0] == L'#' || line[0] == 0 || wcsspn(line, L" \n\r\t\0") == wcslen(line))
                continue;

            TPointListEx *list = new TPointListEx;
            wcstok(line, L":");
            wchar_t *c;
            while ((c = wcstok(NULL, L";")) != NULL)
            {
                UnicodeString str = c;

                int x = str.SubString(1, str.Pos(L',') - 1).ToInt();
                int y = str.SubString(str.Pos(L',') + 1, str.Length()).ToInt();
                list->add(x, y);
            }
            try
            {
                list->resize(10000, 10000);
                ProcessStroke(list, list, false);
                DissectStroke(list, list);
                Add(list);
            }
            catch (...)
            {
                delete list;
                list = 0;
            }
        }

    }
    catch (...)
    {
        ;
    }
    fclose(f);
}
//---------------------------------------------------------------------------
void TStrokeModels::SaveToFile(FILE *f)
{
    word c = Count;
    fwrite(&c, sizeof(word), 1, f);
    for (int ix = 0; ix < Count; ++ix)
    {
        TPointListEx *item = Items[ix];
        c = item->Count;
        fwrite(&c, sizeof(word), 1, f);
        for (int iy = 0; iy < item->Count; ++iy)
        {
            TPoint p = *item->Items[iy];
            fwrite(&p.x, sizeof(long), 1, f);
            fwrite(&p.y, sizeof(long), 1, f);
        }
    }
}
//---------------------------------------------------------------------------
void TStrokeModels::LoadFromFile(FILE *f, int version, bool main)
{
    //int change[3][2] = { { -450, -420 }, { -920, -818 }, { -1400, -1200 } };

    Clear();

    word c;
    fread(&c, sizeof(word), 1, f);
    Capacity = c;

    while (Count < Capacity)
    {
        //bool doit = main && (Count == 11 || Count == 18 || Count == 22 || Count == 23 || Count == 24 || Count == 25 || Count == 26 || Count == 34);
        //bool there = false;
        //TPoint ex = Point(-1000, -1000);
        //int step = 0;

        TPointListEx *item = new TPointListEx;
        fread(&c, sizeof(word), 1, f);
        item->Capacity = c;
        while (item->Count < item->Capacity)
        {
            TPoint p;

            fread(&p.x, sizeof(long), 1, f);
            fread(&p.y, sizeof(long), 1, f);

            /*if (step < 3) {
             if (doit) {
             there = there || p.x < ex.x && p.y < ex.y;
             if (!there)
             ex = p;
             else {
             p = Point(ex.x + change[step][0], ex.y + change[step][1]);
             step++;
             }
             }
             */
            item->add(p);
            //}
        }
        Add(item);
    }
}
//---------------------------------------------------------------------------
void TStrokeModels::FreeItem(TPointListEx *item, int ix)
{
    delete item;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(127);
#endif

