//---------------------------------------------------------------------------

#ifndef templatelistH
#define templatelistH

#include "qsrt.h"
//---------------------------------------------------------------------------

// Vector containing items of type T.

template<typename T>
class TTemplateVector
{
public:
    typedef T Elem;
private:
    typedef TTemplateVector<T> selftype;

    Elem *items;
    int count;
    int capacity;

    void set_item(int ix, const Elem& val);
    Elem get_item(int ix);

    int get_count() const;
    int get_capacity() const;
    void set_capacity(int cap, int insertpos = -1);
protected:
    void CheckIndex(int ix, bool endinclusive = false) const;
    virtual void DeleteNotify(T& item, int ix) {}
public:
    TTemplateVector();
    virtual ~TTemplateVector();

    virtual void Clear();
    virtual void Delete(int ix);
    virtual void DeleteRange(int ix, int cnt);
    virtual int Remove(const Elem& val);
    virtual void Exchange(int s1, int s2);
    int IndexOf(const Elem& val) const;
    int Add(const Elem& val);
    void Insert(int ix, const Elem& val);
    void Reverse();
    Elem& Last();
    const Elem& Last() const;
    Elem& First();
    const Elem& First() const;
    void Assign(const selftype &list);
    void Sort(int(*func)(Elem*, Elem*));
    void Move(int pos, int newpos);

    Elem& operator[](int ix);
    const Elem& operator[](int ix) const;

    __property Elem Items[int] = { read = get_item, write = set_item };
    __property int Count = { read = get_count };
    __property int Capacity = { read = get_capacity, write = set_capacity };
    __property Elem* List = { read = items };
};

//---------------------------------------------------------------------------
template<typename T>
TTemplateVector<T>::TTemplateVector() : items(NULL), count(0), capacity(0)/*, ftag(0)*/
{
    ;
}
//---------------------------------------------------------------------------
template<typename T>
TTemplateVector<T>::~TTemplateVector()
{
    Clear();
}
//---------------------------------------------------------------------------
template<typename T>
void TTemplateVector<T>::CheckIndex(int ix, bool endinclusive) const
{
    if (ix < 0 || ((!endinclusive && ix >= count) || (endinclusive && ix > count)))
        throw L"Index out of bounds.";
}
//---------------------------------------------------------------------------
template<typename T>
int TTemplateVector<T>::get_count() const
{
    return count;
}
//---------------------------------------------------------------------------
template<typename T>
int TTemplateVector<T>::get_capacity()  const
{
    return capacity;
}
//---------------------------------------------------------------------------
template<typename T>
void TTemplateVector<T>::set_capacity(int cap, int insertpos)
{
    cap = max(0, cap);
    if (capacity == cap)
        return;

    if (cap < count)
    {
        for (int ix = cap; ix < count; ++ix)
            DeleteNotify(items[ix], ix);
        count = cap;
    }

    Elem* temp = items;
    if (cap)
        items = new Elem[cap];
    else
        items = NULL;

    if (cap && count)
    {
        if (insertpos == -1)
            memcpy(items, temp, sizeof(Elem) * min(count, cap));
        else
        {
            if (insertpos)
                memcpy(items, temp, sizeof(Elem) * insertpos);
            if (insertpos < count)
                memcpy(items + insertpos + 1, temp + insertpos, sizeof(Elem) * (count - insertpos));
        }
    }

    delete[] temp;
    capacity = cap;
}
//---------------------------------------------------------------------------
template<typename T>
void TTemplateVector<T>::Clear()
{
    set_capacity(0);
}
//---------------------------------------------------------------------------
template<typename T>
int TTemplateVector<T>::Remove(const Elem &val)
{
    int ix = IndexOf(val);
    if (ix >= 0)
        Delete(ix);
    return ix;
}
//---------------------------------------------------------------------------
template<typename T>
void TTemplateVector<T>::Delete(int ix)
{
    CheckIndex(ix);

    DeleteNotify(items[ix], ix);

    if (capacity > 1024 && count + capacity / 4 - 1 < capacity / 2)
    {
        Elem* temp = items;
        capacity /= 2;
        items = new Elem[capacity];
        if (ix)
            memcpy(items, temp, sizeof(Elem) * ix);
        if (ix + 1 != count)
            memcpy(items + ix, temp + ix + 1, sizeof(Elem) * (count - ix - 1));
        delete[] temp;
    }
    else
        memmove(items + ix, items + ix + 1, sizeof(Elem) * (count - ix - 1));

    --count;
}
//---------------------------------------------------------------------------
template<typename T>
void TTemplateVector<T>::DeleteRange(int ix, int cnt)
{
    if (cnt < 0)
        throw L"Cannot delete negative count.";

    CheckIndex(ix);
    CheckIndex(ix + cnt, true);

    for (int iy = cnt - 1; iy >= 0; --iy)
        DeleteNotify(items[ix + iy], ix + iy);

    if (capacity > 1024 && count + capacity / 4 - cnt < capacity / 2)
    {
        Elem* temp = items;
        while (capacity > 1024 && count + capacity / 4 - cnt < capacity / 2)
            capacity /= 2;
        items = new Elem[capacity];
        if (ix)
            memcpy(items, temp, sizeof(Elem) * ix);
        if (ix + cnt != count)
            memcpy(items + ix, temp + ix + cnt, sizeof(Elem) * (count - ix - cnt));
        delete[] temp;
    }
    else
        memmove(items + ix, items + ix + cnt, sizeof(Elem) * (count - ix - cnt));

    count -= cnt;
}
//---------------------------------------------------------------------------
template<typename T>
void TTemplateVector<T>::Sort(int(*func)(T*, T*))
{
    if (count > 1)
        quicksort(items, 0, count - 1, func);
}
//---------------------------------------------------------------------------
template<typename T>
void TTemplateVector<T>::Exchange(int s1, int s2)
{
    CheckIndex(s1);
    CheckIndex(s2);

    Elem tmp = items[s1];
    items[s1] = items[s2];
    items[s2] = tmp;
}
//---------------------------------------------------------------------------
template<typename T>
void TTemplateVector<T>::Reverse()
{
    for (int ix = 0; ix < count / 2; ++ix)
        Exchange(ix, count - ix - 1);
}
//---------------------------------------------------------------------------
template<typename T>
void TTemplateVector<T>::set_item(int ix, const Elem& val)
{
    CheckIndex(ix);

    items[ix] = val;
}
//---------------------------------------------------------------------------
template<typename T>
TTemplateVector<T>::Elem TTemplateVector<T>::get_item(int ix)
{
    CheckIndex(ix);
    return items[ix];
}
//---------------------------------------------------------------------------
template<typename T>
TTemplateVector<T>::Elem& TTemplateVector<T>::operator[](int ix)
{
    CheckIndex(ix);
    return items[ix];
}
//---------------------------------------------------------------------------
template<typename T>
const TTemplateVector<T>::Elem& TTemplateVector<T>::operator[](int ix) const
{
    CheckIndex(ix);
    return items[ix];
}
//---------------------------------------------------------------------------
template<typename T>
int TTemplateVector<T>::Add(const Elem& val)
{
    if (count == capacity)
        set_capacity(count + (count < 4 ? 2 : count < 8 ? 4 : count < 32 ? 8 : count < 64 ? 16 : capacity / 4));

    items[count] = val;
    ++count;

    return count - 1;
}
//---------------------------------------------------------------------------
template<typename T>
void TTemplateVector<T>::Insert(int ix, const Elem& val)
{
    if (ix > count)
        ix = count;
    CheckIndex(ix, true);

    if (count == capacity)
        set_capacity(count + (count < 4 ? 2 : count < 8 ? 4 : count < 32 ? 8 : count < 64 ? 16 : capacity / 4), ix);
    else
        memmove(items + ix + 1, items + ix, sizeof(Elem) * (count - ix));
    items[ix] = val;
    ++count;
}
//---------------------------------------------------------------------------
template<typename T>
TTemplateVector<T>::Elem& TTemplateVector<T>::Last()
{
    if (!count)
        throw L"No items in list.";

    return items[count - 1];
}
//---------------------------------------------------------------------------
template<typename T>
TTemplateVector<T>::Elem& TTemplateVector<T>::First()
{
    if (!count)
        throw L"No items in list.";
    return items[0];
}
//---------------------------------------------------------------------------
template<typename T>
const TTemplateVector<T>::Elem& TTemplateVector<T>::Last() const
{
    if (!count)
        throw L"No items in list.";

    return items[count - 1];
}
//---------------------------------------------------------------------------
template<typename T>
const TTemplateVector<T>::Elem& TTemplateVector<T>::First() const
{
    if (!count)
        throw L"No items in list.";
    return items[0];
}
//---------------------------------------------------------------------------
template<typename T>
int TTemplateVector<T>::IndexOf(const Elem& val) const
{
    for (int ix = 0; ix < count; ++ix)
        if (items[ix] == val)
            return ix;

    return -1;
}
//---------------------------------------------------------------------------
template<typename T>
void TTemplateVector<T>::Assign(const selftype &list)
{
    if (list.items == items)
        return;

    Clear();

    set_capacity(list.capacity);
    count = list.count;
    memcpy(items, list.items, sizeof(Elem) * count);
}
//---------------------------------------------------------------------------
template<typename T>
void TTemplateVector<T>::Move(int pos, int newpos)
{
    CheckIndex(pos);
    CheckIndex(newpos);

    if (pos == newpos)
        return;
    if (pos < newpos)
    {
        Elem e = items[pos];
        memmove(items + pos, items + pos + 1, sizeof(Elem) * (newpos - pos));
        items[newpos] = e;
    }
    else
    {
        Elem e = items[pos];
        memmove(items + newpos + 1, items + newpos, sizeof(Elem) * (pos - newpos));
        items[newpos] = e;
    }
}
//---------------------------------------------------------------------------

// Vector containing pointers to type T. When AFreeUp is true, every element delete also calls FreeItem which disposes of the memory. FreeItem can be overriden. Set ArrayType to true to delete items with delete[] instead of delete.
template<class T, bool AFreeUp = false, bool ArrayType = false>
class TTemplateList : public TTemplateVector<T*>
{
public:
    typedef T* Elem;
private:
    typedef TTemplateVector<T*> inherited;
    typedef TTemplateList<T, AFreeUp, ArrayType> selftype;

    virtual void DeleteNotify(Elem &item, int ix);
protected:
    virtual void FreeItem(Elem item, int ix);
    virtual void CopyElem(Elem dest, Elem src) {}
public:
    TTemplateList()
    {
        ;
    }
    virtual ~TTemplateList();

    void Assign(const selftype &list);
    Elem Extract(int ix);
};

#pragma warn -8008
#pragma warn -8066
//---------------------------------------------------------------------------
template<class T, bool A, bool F>
TTemplateList<T, A, F>::~TTemplateList()
{
    Clear();
}
//---------------------------------------------------------------------------
template<class T, bool A, bool F>
void TTemplateList<T, A, F>::DeleteNotify(Elem& item, int ix)
{
    if (A && item)
        FreeItem(item, ix);
}
//---------------------------------------------------------------------------
template<class T, bool A, bool F>
void TTemplateList<T, A, F>::FreeItem(Elem item, int ix)
{
    if (!F)
        delete item;
    else
        delete[] item;
}
//---------------------------------------------------------------------------
template<class T, bool A, bool F>
TTemplateList<T, A, F>::Elem TTemplateList<T, A, F>::Extract(int ix)
{
    Elem item = items[ix];
    items[ix] = NULL;
    Delete(ix);

    return item;
}
//---------------------------------------------------------------------------
template<class T, bool A, bool F>
void TTemplateList<T, A, F>::Assign(const TTemplateList<T, A, F>& list)
{
    if (&list == this)
        return;

    Clear();
    while (Count < list.Count)
    {
        Elem elem = new T;
        CopyElem(elem, list[Count]);
        Add(elem);
    }
}
//---------------------------------------------------------------------------

#pragma warn .8008
#pragma warn .8066

#endif
