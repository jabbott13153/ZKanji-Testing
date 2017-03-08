//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "groupadd.h"
#include "gengraph.h"
#include "collection.h"
#include "words.h"
#include "wordgroup.h"
#include "grouplist.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(52);
#endif

TfWordToGroup *fWordToGroup;
//---------------------------------------------------------------------------
__fastcall TfWordToGroup::TfWordToGroup(TComponent* Owner) : TBaseForm(Owner)
{
    cbGroup->MaxLength = group_name_length_limit;
}
//---------------------------------------------------------------------------
void __fastcall TfWordToGroup::FormPaint(TObject *Sender)
{
    DrawPillowPanel(this, NULL, b2, false, false);
    DrawCloseBar(this, b2, NULL);
}
//---------------------------------------------------------------------------
int TfWordToGroup::ShowModalEx(TWordCollection *acoll, int awordindex, int &agroupindex)
{
    TWordIndexList *l = new TWordIndexList(acoll/*,true*/);
    l->Add(awordindex);
    return ShowModalEx(l, agroupindex);
}
//---------------------------------------------------------------------------
int TfWordToGroup::ShowModalEx(TWordIndexList *list, int &agroupindex)
{
    int result;

    fcollection = list->collection;

    if (list->Count == 1)
        for (int ix = 0; ix < fcollection->words->Items[list->Indexes[0]]->meaningcnt; ++ix)
            cbMeaning->Items->Add((UnicodeString)(ix + 1) + L". " + fcollection->words->Items[list->Indexes[0]]->data[ix].meaning);
    cbMeaning->ItemIndex = 0;
    cbMeaning->Enabled = cbMeaning->Items->Count > 1;

    for (int ix = 0; ix < fcollection->groups->Count; ++ix)
        cbGroup->Items->Add(fcollection->groups->Items[ix]->name);

    if (cbGroup->Items->Count > agroupindex && agroupindex >= 0)
        cbGroup->ItemIndex = agroupindex;
    else
        cbGroup->Text = L"New Group";

    result = ShowModal();
    if (result == mrOk)
    {
        MODON
        int coll = dictionary->Index[fcollection];
        int gindex = RealIndex(cbGroup);
        if (gindex < 0)
            gindex = dictionary->AddWordGroup(coll, cbGroup->Text.Trim(), false);

        if (list->Count == 1)
        {
            if (!dictionary->Items[coll]->groups->WordInGroup(gindex, list->Indexes[0], cbMeaning->ItemIndex))
                dictionary->Items[coll]->groups->Items[gindex]->AddWord(list->Indexes[0], cbMeaning->ItemIndex);
        }
        else
        {
            for (int ix = 0; ix < list->Count; ix++)
            {
                for (int j = 0; j < list->Words[ix]->meaningcnt; j++)
                    if (!dictionary->Items[coll]->groups->WordInGroup(gindex, list->Indexes[ix], j))
                        dictionary->Items[coll]->groups->Items[gindex]->AddWord(list->Indexes[ix], j);
            }
        }

        if (RealIndex(cbGroup) < 0)
            fGroup->WordGroupAdded(coll, cbGroup->Text.Trim());

        agroupindex = gindex;
        fcollection->Modify(false);
        MODOFF
    }
    return result;
}
//---------------------------------------------------------------------------
void __fastcall TfWordToGroup::cbGroupChange(TObject *Sender)
{
    btnAdd->Enabled = cbGroup->Text.Trim().Length();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int WordToGroup(TWordCollection *acoll, int awordindex, int &agroupindex)
{
    int res;
    fWordToGroup = NULL;
    try
    {
        fWordToGroup = new TfWordToGroup(Application);
        res = fWordToGroup->ShowModalEx(acoll, awordindex, agroupindex);
    }
    catch (...)
    {
        res = mrAbort;
    }
    delete fWordToGroup;

    return res;
}
//---------------------------------------------------------------------------
int WordToGroup(TWordIndexList *list, int &agroupindex)
{
    int res;
    fWordToGroup = NULL;
    try
    {
        fWordToGroup = new TfWordToGroup(Application);
        res = fWordToGroup->ShowModalEx(list, agroupindex);
    }
    catch (...)
    {
        res = mrAbort;
    }
    delete fWordToGroup;

    return res;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(53);
#endif

