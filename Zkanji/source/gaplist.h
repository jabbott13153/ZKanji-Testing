//---------------------------------------------------------------------------

#ifndef gaplistH
#define gaplistH
//---------------------------------------------------------------------------

#include <stdio.h>

typedef unsigned short word;

extern const unsigned int UINTMAX;

template<typename T>
class TGapList
{
private:
    //unsigned int get_count() const;

    int pos_index(unsigned int ix) const;
    unsigned int data_len(int posindex) const;
    unsigned int data_index(int posindex, unsigned int ix) const;
    unsigned int first_index(int posindex) const;

    unsigned int fcount;
    unsigned int fallocsize;

    unsigned int fused;
    unsigned int falloced;
    T *data;
    T fdefault;
    T fnullelem;
    bool firstempty;

    unsigned int *pos;
    int posalloced;
    int poscount;

    bool fexception;

    void Grow(unsigned int num);
    void GrowPos(int num);

    void Compress();
protected:
    virtual void set_item(unsigned int ix, T const elem);
    T& get_item(unsigned int ix);
    T& get_trueitem(unsigned int ix) const;
    unsigned int get_truepos(unsigned int ix) const;
    unsigned int get_pos(unsigned int ix) const;

    virtual void DeleteProc(T &dat)
    {
    };
    virtual void WriteData(FILE *f, T &dat)
    {
    }
    virtual void ReadData(FILE *f, T &dat)
    {
    }
    virtual void CopyData(T *data, T *orig, unsigned int cnt);
public:
    TGapList(bool ExceptionForMissing = true, int allocsize = 100);
    virtual ~TGapList();

    typedef void (__closure * TGapFunc)(T &);
    typedef void (__closure * TGapIndexFunc)(unsigned int ix);
    typedef void (*TGapFunc2)(T &);
    typedef void (*TGapIndexFunc2)(unsigned int ix);

    template<typename U>
    void Iterate(U);
    template<typename U>
    void IterateIndexes(U);

    void Delete(unsigned int ix); // Remove element and move everything above it one place down.

    TGapList& Assign(const TGapList *l);
    virtual void LoadFromFile(FILE *f);
    virtual void SaveToFile(FILE *f);

    void Clear(); // Removes all items from list.
    void ClearIndex(unsigned int ix); // Removes given item from list without changing index of other elements.
    void Truncate(unsigned int newcount); // Removes all items at and above the given count.
    bool ItemSet(unsigned int ix) const; // Returns true if this item is in the list, false otherwise.

    void SetAll(unsigned int num, T val);

    __property T Items[unsigned int ix] = { read = get_item, write = set_item };
    __property unsigned int Count = { read = fcount };

    __property unsigned int TrueCount = { read = fused };
    __property unsigned int TruePos[unsigned int ix] = { read = get_truepos }; // Virtual position from real array index.
    __property T TrueItems[unsigned int ix] = { read = get_trueitem };
    __property T DefaultValue = { read = fdefault, write = fdefault }; // Set this to give a default value to items in the gaplist that have indices below the count of the list.
    __property T NullElem = { read = fnullelem, write = fnullelem }; // Set this to be returned instead of a 0, when the given index is more than the count of the list. NOT USED FOR INNER ITEMS
    __property unsigned int Pos[unsigned int] = { read = get_pos };

    __property int PosCount = { read = poscount };
};

class TGapStringList : public TGapList<wchar_t*>
{
private:
    typedef TGapList<wchar_t*> inherited;
    bool utf8; //backwards compatible. remove 2011 july    since group ver 11
    byte version;

    virtual void set_item(unsigned int ix, wchar_t* const elem);
protected:
    virtual void DeleteProc(wchar_t* &dat);
    virtual void WriteData(FILE *f, wchar_t* &dat);
    virtual void ReadData(FILE *f, wchar_t* &dat);
    virtual void CopyData(wchar_t **dat, wchar_t **orig, unsigned int cnt);

    virtual void LoadFromFile(FILE *f)
    {
        throw L"Do not call this directly!";
    }
public:
    TGapStringList(bool ExceptionForMissing = true, int allocsize = 500);
    virtual ~TGapStringList();

    void LoadFromFile(FILE *f, bool fileutf8, byte fileversion); //backwards compatible. remove 2011 july     since group ver 11
};

template<typename T>
TGapList<T>::TGapList(bool aexception, int allocsize) : fexception(aexception), fallocsize(allocsize), firstempty(true), falloced(0), fcount(0), data(NULL), pos(NULL), posalloced(0), poscount(0), fused(0)
{
    memset(&fdefault, 0, sizeof(T));
    memset(&fnullelem, 0, sizeof(T));
}

template<typename T>
TGapList<T>::~TGapList()
{
    Clear();
}

template<typename T>
void TGapList<T>::SetAll(unsigned int num, T val)
{
    Clear();
    if (!num || num == UINTMAX)
        return;
    firstempty = false;
    int newalloced = ((num + fallocsize - 1) / fallocsize) * fallocsize;
    data = new T[newalloced];
    falloced = newalloced;
    for (unsigned int i = 0; i < num; i++)
        data[i] = val;
    fused = fcount = num;
}

template<typename T>
void TGapList<T>::Clear()
{
    Iterate<TGapFunc>(DeleteProc);
    if (posalloced)
        delete[] pos;
    if (falloced)
        delete[] data;
    fused = 0;
    falloced = 0;
    firstempty = true;
    posalloced = 0;
    poscount = 0;
    fcount = 0;
}

template<typename T>
void TGapList<T>::Compress()
{
    if (posalloced >= 200 && poscount < posalloced / 2.2)
    {
        unsigned int *tmp = pos;
        pos = new unsigned int[posalloced / 2];
        posalloced /= 2;
        memcpy(pos, tmp, sizeof(unsigned int)*poscount);
        delete[] tmp;
    }

    if (falloced >= fallocsize * 2 && fused < falloced / 2.2)
    {
        T *tmp = data;
        data = new T[falloced / 2];
        falloced /= 2;
        memcpy(data, tmp, sizeof(T)*fused);
        delete[] tmp;
    }
}

template<typename T>
void TGapList<T>::Delete(unsigned int ix)
{
    if (ix == UINTMAX)
        throw L"Gaplist overflow!";
    if (ix >= fcount)
        return;

    if (fused == 1 && ix + 1 == fcount)
    {
        DeleteProc(data[0]);
        poscount = 0;
        posalloced = 0;
        delete[] pos;
        pos = NULL;
        fcount = 0;
        fused = 0;
        firstempty = true;
        return;
    }

    int i = -1;
    while (i + 1 < poscount && pos[i + 1] <= ix)
        i++;

    if (firstempty != ((i % 2) != 0))
    {
        unsigned int fix = first_index(i);

        unsigned int mx = ix + fix - (i >= 0 ? pos[i] : 0);
        DeleteProc(data[mx]);
        if (fused > mx + 1)
            memmove(data + mx, data + mx + 1, sizeof(T)*(fused - mx - 1));

        if (ix + 1 == fcount && i >= 0 && pos[i] == ix)
            poscount -= 2;
        else if (i < poscount - 1 && (ix == 0 || i >= 0 && ix == pos[i]) && ix + 1 == pos[i + 1])
        { //delete it!
            if (i >= 0)
            {
                if (poscount > i + 2)
                    memmove(pos + i, pos + i + 2, sizeof(unsigned int)*(poscount - i - 2));
                poscount -= 2;
            }
            else
            {
                firstempty = true;
                if (poscount > 1)
                    memmove(pos, pos + 1, sizeof(unsigned int)*(poscount - 1));
                poscount--;
                i++;
            }
        }
        else
            i++;
    }
    else
    {
        if (ix == 0 && pos[0] == 1)
        {
            firstempty = false;
            if (poscount > 1)
                memmove(pos, pos + 1, sizeof(unsigned int)*(poscount - 1));
            poscount--;
            i++;
        }
        else if (pos[i + 1] == ix + 1 && i >= 0 && pos[i] == ix)
        {
            if (poscount > 2 + i)
                memmove(pos + i, pos + 2 + i, sizeof(unsigned int)*(poscount - 2 - i));
            poscount -= 2;
        }
        else
            i++;
    }

    for (; i < poscount; i++)
        pos[i]--;

    fcount--;

    Compress();
}

template<typename T>
void TGapList<T>::LoadFromFile(FILE *f)
{
    fread(&firstempty, sizeof(bool), 1, f);
    fread(&fcount, sizeof(unsigned int), 1, f);

    if (posalloced)
    {
        delete[] pos;
        pos = 0;
    }
    fread(&poscount, sizeof(int), 1, f);
    posalloced = 0;
    while (posalloced < poscount)
        posalloced += 100;
    if (posalloced)
    {
        pos = new unsigned int[posalloced];
        fread(pos, sizeof(unsigned int), poscount, f);
    }

    if (falloced)
    {
        delete[] data;
        data = 0;
    }
    falloced = 0;
    fread(&fused, sizeof(unsigned int), 1, f);
    while (falloced < fused)
        falloced += fallocsize;
    if (falloced)
    {
        data = new T[falloced];
        for (unsigned int i = 0; i < fused; i++)
            ReadData(f, data[i]);
    }
}

template<typename T>
void TGapList<T>::SaveToFile(FILE *f)
{
    fwrite(&firstempty, sizeof(bool), 1, f);
    fwrite(&fcount, sizeof(unsigned int), 1, f);

    fwrite(&poscount, sizeof(int), 1, f);
    fwrite(pos, sizeof(unsigned int), poscount, f);

    fwrite(&fused, sizeof(unsigned int), 1, f);
    for (unsigned int i = 0; i < fused; i++)
        WriteData(f, data[i]);
}

template<typename T>
TGapList<T>& TGapList<T>::Assign(const TGapList *other)
{
    if (other == this)
        return *this;

    Clear();

    fallocsize = other->fallocsize;
    firstempty = other->firstempty;
    fcount = other->fcount;

    if (posalloced)
    {
        delete[] pos;
        posalloced = 0;
    }

    poscount = other->poscount;
    while (posalloced < poscount)
        posalloced += 100;
    if (posalloced)
        pos = new unsigned int[posalloced];
    else
        pos = 0;

    if (falloced)
    {
        delete[] data;
        falloced = 0;
    }

    fused = other->fused;
    while (falloced < fused)
        falloced += fallocsize;
    if (falloced)
        data = new T[falloced];
    else
        data = 0;

    if (poscount)
        memcpy(pos, other->pos, sizeof(int)*poscount);

    if (fused)
        CopyData(data, other->data, fused);

    return *this;
}

template<typename T>
void TGapList<T>::CopyData(T *dat, T *orig, unsigned int cnt)
{
    memcpy(dat, orig, sizeof(T)*cnt);
}

template<typename T>
template<typename U> void TGapList<T>::Iterate(U func)
{
    for (unsigned int i = 0; i < fused; i++)
        func(data[i]);
}

template<typename T>
template<typename U> void TGapList<T>::IterateIndexes(U func)
{
    for (int i = -1; i < poscount; i++)
    {
        if (((i % 2) != 0) == firstempty)
            continue;

        unsigned int l = data_len(i);
        for (unsigned int j = 0; j < l; j++)
            func((i >= 0 ? pos[i] : 0) + j);
    }
}

template<typename T>
int TGapList<T>::pos_index(unsigned int ix) const
{
    if (ix >= fcount)
        return (!fcount ? -1 : poscount);

    int min = 0, max = poscount - 1, mid;
    while (min <= max)
    {
        mid = (min + max) / 2;
        if (pos[mid] > ix)
            max = mid - 1;
        else if (pos[mid] <= ix)
            min = mid + 1;
    };

    return min - 1;
}

template<typename T>
unsigned int TGapList<T>::data_len(int posindex) const
{
    if (!poscount && posindex > -1)
        throw L"No positions to measure!";
    if (posindex == poscount)
        throw L"Infinite length position!";

    if (posindex == -1)
        return (poscount ? pos[0] : fcount);

    return (posindex < poscount - 1 ? pos[posindex + 1] : fcount) - pos[posindex];
}

template<typename T>
unsigned int TGapList<T>::first_index(int posindex) const
{
    if (((posindex % 2) != 0) == firstempty)
        throw L"Empty position has no first index!";

    unsigned int p = 0;
    for (int i = -1; i < posindex; i++)
    {
        if (((i % 2) != 0) == firstempty)
            continue;
        p += data_len(i);
    }
    return p;
}

template<typename T>
unsigned int TGapList<T>::data_index(int posindex, unsigned int ix) const
{
    if (ix >= fcount || firstempty == ((posindex % 2) != 0))
        return UINTMAX;

    return ix + first_index(posindex) - (posindex >= 0 ? pos[posindex] : 0);
}

template<typename T>
void TGapList<T>::Grow(unsigned int num)
{
    unsigned int newalloced = falloced;
    while (/*fcount*/fused + num > newalloced)
        newalloced += fallocsize;
    if (newalloced == falloced)
        return;

    T *tmp = data;
    data = new T[newalloced];
    if (falloced)
    {
        memcpy(data, tmp, sizeof(T)*falloced);
        delete[] tmp;
    }
    falloced = newalloced;
}

template<typename T>
void TGapList<T>::GrowPos(int num)
{
    int newalloced = posalloced;
    while (poscount + num > newalloced)
        newalloced += 100;
    if (newalloced == posalloced)
        return;

    unsigned int *tmp = pos;
    pos = new unsigned int[newalloced];
    if (posalloced)
    {
        memcpy(pos, tmp, sizeof(unsigned int)*posalloced);
        delete[] tmp;
    }
    posalloced = newalloced;
}

template<typename T>
void TGapList<T>::set_item(unsigned int ix, T const elem)
{
    if (ix == UINTMAX)
        throw L"Accessing invalid index in gaplist!";

    int p = pos_index(ix);
    unsigned int m = data_index(p, ix);
    if (m != UINTMAX)
    {
        data[m] = elem;
        return;
    }

    bool before; //ix before an alloced position
    bool after; //ix after an alloced position

    before = p < poscount - 1 && pos[p + 1] == ix + 1;
    after = p == poscount && ix == fcount || p < poscount - 1 && p >= 0 && pos[p] == ix;

    Grow(1);

    unsigned int fi = first_index(p + 1);
    if (before && !after)
    {
        pos[p + 1]--;
        if (pos[p + 1] == 0)
        {
            firstempty = false;
            if (poscount > 1)
            {
                memmove(pos, pos + 1, sizeof(unsigned int)*(poscount - 1));
                poscount--;
            }
            else
            {
                poscount = 0;
                posalloced = 0;
                delete[] pos;
                pos = NULL;
            }
        }

    }
    else if (after && !before)
    {
        if (p < poscount)
            pos[p]++;
    }
    else if (before && after)
    {
        if (poscount > p + 2)
            memmove(pos + p, pos + p + 2, sizeof(unsigned int)*(poscount - p - 2));
        poscount -= 2;
        if (!poscount)
        {
            poscount = 0;
            posalloced = 0;
            delete[] pos;
            pos = NULL;
        }
    }
    else
    {
        if (ix > 0 || poscount)
            GrowPos(ix > 0 && (poscount || !firstempty) ? 2 : 1);

        if (poscount > p + 1)
            memmove(pos + p + 1 + (ix > 0 && (poscount || !firstempty) ? 2 : 1), pos + p + 1, sizeof(unsigned int)*(poscount - p - 1));
        if (!poscount && firstempty)
        {
            if (ix)
            {
                poscount++;
                pos[0] = ix;
            }
        }
        else if (ix == 0 && poscount)
        {
            pos[0] = 1;
            poscount++;
        }
        else
        {
            if (p == poscount)
            {
                pos[p] = fcount;
                pos[p + 1] = ix;
            }
            else
            {
                pos[p + 1] = ix;
                pos[p + 2] = ix + 1;
            }
            poscount += 2;
        }
        if (ix == 0)
            firstempty = false;
    }
    if (fused > fi)
        memmove(data + fi + 1, data + fi, sizeof(T)*(fused - fi));

    data[fi] = elem;
    fused++;

    if (fcount < ix + 1)
        fcount = ix + 1;
}

template<typename T>
void TGapList<T>::ClearIndex(unsigned int ix)
{
    if (ix == UINTMAX)
        throw L"Accessing UINTMAX index in gaplist!";

    int p = pos_index(ix);
    unsigned int m = data_index(p, ix);
    if (m == UINTMAX)
        return;

    DeleteProc(data[m]);

    bool first; //first item of position
    bool last; //last item of position

    first = ix == 0 || p >= 0 && pos[p] == ix;
    last = (poscount > p + 1 ? pos[p + 1] : fcount) == ix + 1;

    unsigned int fi = first_index(p) + (ix - (p >= 0 ? pos[p] : 0));
    if (first && !last)
    {
        if (ix == 0)
        {
            GrowPos(1);
            if (poscount)
                memmove(pos + 1, pos, sizeof(unsigned int)*poscount);
            poscount++;
            pos[0] = 0;
            firstempty = true;
            p++;
        }
        pos[p]++;

    }
    else if (last && !first)
    {
        if (p + 1 < poscount)
            pos[p + 1]--;
        else
            fcount--;
    }
    else if (first && last)
    {
        if (poscount == 1)
        {
            poscount = 0;
            posalloced = 0;
            delete[] pos;
            pos = NULL;
            fcount = 0;
            firstempty = true;
        }
        else if (poscount > 1)
        {
            if (ix == 0)
            {
                memmove(pos, pos + 1, sizeof(unsigned int)*(poscount - 1));
                poscount--;
                firstempty = true;
            }
            else
            {
                if (poscount > p + 2)
                    memmove(pos + p, pos + p + 2, sizeof(unsigned int)*(poscount - p - 2));
                else
                    fcount = pos[p - 1];
                poscount -= 2;
            }
        }
    }
    else
    {
        GrowPos(2);

        if (poscount > p + 1)
            memmove(pos + p + 3, pos + p + 1, sizeof(unsigned int)*(poscount - p - 1));
        pos[p + 1] = ix;
        pos[p + 2] = ix + 1;
        poscount += 2;
    }
    if (fused > fi + 1)
        memmove(data + fi, data + fi + 1, sizeof(T)*(fused - fi - 1));
    fused--;
    if (!fused)
    {
        delete[] data;
        falloced = 0;
        data = NULL;
    }

    Compress();
}

template<typename T>
T& TGapList<T>::get_item(unsigned int ix)
{
    if (ix == UINTMAX)
        throw L"Accessing UINTMAX index in gaplist!";

    if (ix >= fcount)
    {
        if (fexception)
            throw L"Index over count in TGapList!";
        return fnullelem;
    }

    int p = pos_index(ix);
    unsigned int m = data_index(p, ix);
    if (m == UINTMAX)
    {
        if (fexception)
            throw L"Accessing non-existing member in gaplist!";
        return fdefault;
    }
    return data[m];
}

template<typename T>
bool TGapList<T>::ItemSet(unsigned int ix) const
{
    if (ix == UINTMAX)
        throw L"Accessing UINTMAX index in gaplist!";

    if (ix >= fcount)
        return false;
    int p = pos_index(ix);
    unsigned int m = data_index(p, ix);
    if (m == UINTMAX)
        return false;
    return true;
}

template<typename T>
T& TGapList<T>::get_trueitem(unsigned int ix) const
{
    if (ix >= fused)
        throw L"Invalid gaplist index for true item!";

    return data[ix];
}

template<typename T>
unsigned int TGapList<T>::get_truepos(unsigned int ix) const
{
    if (ix >= fused)
        throw L"Invalid gaplist index!";

    //if (!firstempty && ix < (poscount > 0 ? pos[0] : fcount))
    //return ix;

    int i;

    for (i = 0; i < poscount; i++)
    {
        if (firstempty != ((i % 2) != 0))
            continue;

        unsigned int g = pos[i] - (!i ? 0 : pos[i - 1]);

        if (ix < g)
            break;

        ix -= g;
    }

    return (!i ? 0 : pos[i - 1]) + ix;
}

template<typename T>
unsigned int TGapList<T>::get_pos(unsigned int ix) const
{
    if (ix == UINTMAX)
        throw L"Accessing UINTMAX index in gaplist!";

    int p = pos_index(ix);
    unsigned int m = data_index(p, ix);
    if (m != UINTMAX)
        return m;
    if (fexception)
        throw L"Invalid gaplist index!";
    else
        return UINTMAX;
}

template<typename T>
void TGapList<T>::Truncate(unsigned int newcount)
{
    throw L"TELL THE PROGRAMMER, THIS IS NOT TESTED YET!";

    if (newcount >= fcount)
        return;

    int p = pos_index(newcount);

    unsigned int ix;
    if (firstempty == ((p % 2) != 0))
    {
        ix = first_index(p + 1);
        fcount = (p >= 0 ? pos[p] : 0);
        poscount = p < 0 ? 0 : p;
    }
    else
    {
        ix = newcount + first_index(p) - (p >= 0 ? pos[p] : 0);

        if (newcount == pos[p])
        {
            fcount = (p <= 0 ? 0 : pos[p - 1]);
            poscount = p - 1 < 0 ? 0 : p - 1;
            firstempty = firstempty || fcount == 0;
        }
        else
        {
            fcount = newcount;
            poscount = p + 1;
        }
    }

    for (unsigned int i = ix; i < fused; i++)
        DeleteProc(data[i]);
    fused = ix;

    Compress();
}

#endif
