//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "intlist.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//static TGlobalTest tester1(62);

//---------------------------------------------------------------------------
int numsort(int a, int b)
{
    return a - b;
}
//---------------------------------------------------------------------------
int numsortrev(int a, int b)
{
    return b - a;
}
//---------------------------------------------------------------------------
/*
__fastcall TIntegerList::TIntegerList() : T-List()
{

}
//---------------------------------------------------------------------------
int __fastcall TIntegerList::Add(int val)
{
    return inherited::Add((void*)val);
}
//---------------------------------------------------------------------------
void __fastcall TIntegerList::Insert(int index, int val)
{
    inherited::Insert(index, (void*)val);
}
//---------------------------------------------------------------------------
int __fastcall TIntegerList::get_item(int ix)
{
    return (int)inherited::Items[ix];
}
//---------------------------------------------------------------------------
void __fastcall TIntegerList::set_item(int ix, int val)
{
    inherited::Items[ix] = (void*)val;
}
//---------------------------------------------------------------------------
int __fastcall TIntegerList::IndexOf(int val)
{
    return inherited::IndexOf((void*)val);
}
//---------------------------------------------------------------------------
void __fastcall TIntegerList::Remove(int val)
{
    inherited::Remove((void*)val);
}
//---------------------------------------------------------------------------
*/
void TIntegerList::Sort(bool reverse)
{
    if (!reverse)
        Sort(numsort);
    else
        Sort(numsortrev);
}
//---------------------------------------------------------------------------
static SortIntegerListByFunc sortfunc;
static SortIntegerListByFunc2 sortfunc2;

int __sortthelistwithfunc(int *a, int *b)
{
    return sortfunc(*a, *b);
}

int __sortthelistwithfunc2(int *a, int *b)
{
    return sortfunc2(*a, *b);
}

void TIntegerList::Sort(SortIntegerListByFunc func)
{
    sortfunc = func;
    inherited::Sort(__sortthelistwithfunc);
}
//---------------------------------------------------------------------------
void TIntegerList::Sort(SortIntegerListByFunc2 func)
{
    sortfunc2 = func;
    inherited::Sort(__sortthelistwithfunc2);
}
//---------------------------------------------------------------------------

//static TGlobalTest tester2(63);

