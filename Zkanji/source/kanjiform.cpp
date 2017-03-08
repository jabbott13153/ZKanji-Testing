//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanjiform.h"
#include "settings.h"
#include "collection.h"
#include "kanji.h"
#include "kanjiobj.h"
#include "kanjigrid.h"
#include "mouseevent.h"
#include "kanjigroup.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(80);
#endif

TfKToGroup *fKToGroup;
//---------------------------------------------------------------------------

__fastcall TfKToGroup::TfKToGroup(TComponent* Owner) : TBaseForm(Owner)
{
    cbGroup->MaxLength = group_name_length_limit;

    blist = new TKanjiItemList();
    cellsize = 50 * scaleres;
    kobj = new TKanjiGrid(pbGrid, cellsize, false, (TFuncPtr)NULL);
    EraseBackground = false;
    Constrain(max(Width - pbGrid->ClientWidth + cellsize*8, (int)ChildRect(btnAdd).Right + 3), Height - pbGrid->ClientHeight + 3*cellsize, 99999, 99999, Width - pbGrid->ClientWidth, cellsize, Height - pbGrid->ClientHeight, cellsize);
}
//---------------------------------------------------------------------------
__fastcall TfKToGroup::~TfKToGroup()
{
    delete kobj;
    delete blist;
}
//---------------------------------------------------------------------------
int TfKToGroup::ShowModalEx(TKanjiItemList *alist)
{
    list = alist;
    blist->Assign(list);
    kobj->set_items(blist, kobj->collection);
    for (int i = 0; i < dictionary->Count; i++)
        cbDictionary->Items->Add(dictionary->Strings[i]);

    if (cbDictionary->Items->IndexOf(settings.kanjicollection) >= 0)
    {
        cbDictionary->ItemIndex = cbDictionary->Items->IndexOf(settings.kanjicollection);
        cbDictionaryChange(cbDictionary);

        cbGroup->Items->BeginUpdate();
        try
        {
            cbGroup->Items->Clear();
            for (int i = 0; i < dictionary->Items[cbDictionary->ItemIndex]->kgroups->Count; i++)
                cbGroup->Items->Add(dictionary->Items[cbDictionary->ItemIndex]->kgroups->Items[i]->name);

            if (cbGroup->Items->IndexOf(settings.kanjigroup) >= 0)
                cbGroup->ItemIndex = cbGroup->Items->IndexOf(settings.kanjigroup);
            else if (cbGroup->Items->Count > 0)
                cbGroup->ItemIndex = 0;
        }
        catch (...)
        {
            ;
        }
        cbGroup->Items->EndUpdate();
    }
    else
    {
        cbDictionary->ItemIndex = 0;
        cbDictionaryChange(cbDictionary);
    }
    UpdateButtons();
    ActiveControl = (!btnOk->Enabled ? (TWinControl*)cbGroup : (TWinControl*)btnOk);
    return ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfKToGroup::cbDictionaryChange(TObject *Sender)
{
    cbGroup->Items->BeginUpdate();
    try
    {
        cbGroup->Items->Clear();
        for (int i = 0; i < dictionary->Items[cbDictionary->ItemIndex]->kgroups->Count; i++)
            cbGroup->Items->Add(dictionary->Items[cbDictionary->ItemIndex]->kgroups->Items[i]->name);
        cbGroup->ItemIndex = 0;
    }
    catch (...)
    {
        ;
    }
    cbGroup->Items->EndUpdate();

    if (Visible)
    {
        cbGroup->SetFocus();
        cbGroup->SelectAll();
    }
    kobj->collection = dictionary->Items[cbDictionary->ItemIndex];
    cbGroupChange(cbGroup);
}
//---------------------------------------------------------------------------
void __fastcall TfKToGroup::cbGroupChange(TObject *Sender)
{
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfKToGroup::btnAddClick(TObject *Sender)
{
    blist->Assign(list);
    kobj->set_items(blist, kobj->collection, kobj->pos >= 0 ? kobj->kanjis[kobj->pos] : NULL);
    UpdateButtons();
}
//---------------------------------------------------------------------------
void TfKToGroup::UpdateButtons()
{
    btnOk->Enabled = cbGroup->Text.Trim().Length();
    btnAdd->Enabled = list->Count != kobj->count;
    btnDel->Enabled = kobj->count > 1;
}
//---------------------------------------------------------------------------
void __fastcall TfKToGroup::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (ModalResult == mrOk)
    {
        settings.kanjicollection = cbDictionary->Items->Strings[RealIndex(cbDictionary)];
        if (RealIndex(cbGroup) >= 0)
            settings.kanjigroup = cbGroup->Items->Strings[RealIndex(cbGroup)];
        else
            settings.kanjigroup = cbGroup->Text.Trim();
        list->Clear();
        for (int i = 0; i < kobj->count; i++)
            list->Add(kobj->kanjis[i]->index);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfKToGroup::btnDelMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    blist->Delete(kobj->pos);
    kobj->set_items(blist, kobj->collection, kobj->pos, 1);
    UpdateButtons();

    MouseRepeat(btnDelMouseDown, Sender, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TfKToGroup::pbGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_DELETE)
        btnDelMouseDown(btnDel, mbLeft, Shift, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfKToGroup::FormPaint(TObject *Sender)
{
    DrawPillowPanel(this, NULL, b0);
    //DrawPillowPanel(this,b1,b2,true,true);
    Canvas->FillRect(Rect(0, b1->Top + 1, ClientWidth, b2->Top));
    DrawCloseBar(this, b2, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfKToGroup::FormResize(TObject *Sender)
{
    //TRect r = Rect(0,0,ClientWidth,b0->Top);
    //RedrawWindow(Handle,&r,NULL, RDW_INVALIDATE/* | RDW_ALLCHILDREN*/);
    TRect r = Rect(0, b1->Top + 1, ClientWidth, ClientHeight);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE/*| RDW_ALLCHILDREN*/);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(81);
#endif

