//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "modallist.h"
#include "kanjiinfo.h"
#include "clipview.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester1(104);
#endif

TModalList *modallist;

//---------------------------------------------------------------------------
int TModalList::Add(TBaseForm *form, const HWND h)
{
    TModalItem *item = new TModalItem;

    item->Handle = h;
    item->info = fInfo && fInfo->Visible;
    item->clip = fClipboard && fClipboard->Visible;
    item->form = form;

    if (item->info)
        fInfo->Hide();
    if (item->clip)
        fClipboard->Hide();

    if (fInfo)
    {
        fInfo->enable_buttons(false);
        fInfo->enable_readings(false);
        item->iKanji = fInfo->kanji;
        item->iColl = fInfo->collection;
    }

    return inherited::Add(item);
}
//---------------------------------------------------------------------------
void TModalList::Update(const HWND hold, const HWND hnew)
{
    int i = IndexOf(hold);
    if (i < 0)
        return;
    Items[i]->Handle = hnew;
}
//---------------------------------------------------------------------------
void TModalList::UpdateTop(const HWND hnew)
{
    if (!Count || Items[Count - 1]->Handle != NULL)
        return;
    Items[Count - 1]->Handle = hnew;
}
//---------------------------------------------------------------------------
int TModalList::IndexOf(const HWND h)
{
    for (int i = 0; i < Count; i++)
        if (Items[i]->Handle == h)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
void TModalList::Delete(const HWND h)
{
    int i = IndexOf(h);
    if (i >= 0)
    {
        while (Count > i + 1)
            Delete(Count - 1);
    }
    else if ((int)h == -1 && Count)
        i = Count - 1;
    else if (!h && Count && Items[Count - 1]->form == NULL)
        i = Count - 1;
    else
        throw ERangeError("Bad index for modal window!");

    TModalItem *item = Items[Count - 1];

    if (fInfo)
    {
        if (Count == 1)
        {
            fInfo->enable_buttons(true);
            fInfo->enable_readings(true);
        }
        fInfo->collection = item->iColl;
        fInfo->kanji = item->iKanji;
        if (item->info)
            fInfo->Show();
        else if (fInfo)
            fInfo->Hide();
    }

    if (fClipboard)
    {
        if (item->clip)
            fClipboard->Show();
        else
            fClipboard->Hide();
    }

    Delete(i);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(105);
#endif
