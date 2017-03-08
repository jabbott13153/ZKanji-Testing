//---------------------------------------------------------------------------

#ifndef intlistH
#define intlistH
//---------------------------------------------------------------------------

#include "templatelist.h"

typedef int (*SortIntegerListByFunc)(int a, int b);
typedef int __fastcall (*SortIntegerListByFunc2)(int a, int b);
class TIntegerList : public TTemplateVector<int>
{
private:
    typedef TTemplateVector<int>    inherited;
public:
    using inherited::Sort;
    void Sort(bool reverse = false);
    void Sort(SortIntegerListByFunc func);
    void Sort(SortIntegerListByFunc2 func);
};

/*
typedef int __fastcall (*SortIntegerListByFunc)(int a, int b);
class TIntegerList : public T-List
{
private:
    typedef T-List inherited;
    int __fastcall get_item(int ix);
    void __fastcall set_item(int ix, int val);
public:
    __fastcall TIntegerList();
    int __fastcall Add(int val);
    int __fastcall IndexOf(int val);
    void __fastcall Remove(int val);
    void __fastcall Insert(int index, int val);

    using inherited::Sort;
    void __fastcall Sort(bool reverse = false);
    void __fastcall Sort(SortIntegerListByFunc func);

    __property int Items[int] = { read = get_item, write = set_item };
};
*/
#endif
