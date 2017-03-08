//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "importquery.h"
#include "settings.h"
#include "collection.h"
#include "words.h"
#include "wordobj.h"
#include "zkformats.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(58);
#endif

TfImportQuery *fImportQuery;
//---------------------------------------------------------------------------

__fastcall TfImportQuery::TfImportQuery(TComponent* Owner) : TBaseForm(Owner)
{
    wobj = new TVocabListObj(pb, rowsizes(settings.dictfontsize), (TFuncPtr)NULL, (TPopupWordProc)NULL);
    wobj->wordselect = false;
    fsel = -1;
}
//---------------------------------------------------------------------------
__fastcall TfImportQuery::~TfImportQuery()
{
    delete wobj;
}
//---------------------------------------------------------------------------
int TfImportQuery::ShowModalEx(byte acoll, int wordix, const wchar_t *mm, word freq, int atypes, int anotes, byte afields, word anametag)
{
    int r;
    TWordIndexList *l = new TWordIndexList(dictionary->Items[acoll]/*,true*/);
    TWord *w = dictionary->Items[acoll]->words->Items[wordix];
    for (int i = 0; i < w->meaningcnt; i++)
        l->Add(wordix, i);
    lbFreqOri->Caption = (UnicodeString) + "Frequency: " + w->frequency;
    lbFreqNew->Caption = (UnicodeString) + "Frequency: " + freq;
    lbMeaning->Caption = (UnicodeString) + "Meaning: " + mm;
    lbLabels->Caption = "Labels: ";

    for (int i = 0; i < WT_COUNT; i++)
        if (atypes & (1 << i))
            lbLabels->Caption = lbLabels->Caption + wtypetext[i] + ", ";
    for (int i = 0; i < WN_COUNT; i++)
        if (anotes & (1 << i))
            lbLabels->Caption = lbLabels->Caption + wnotetext[i] + ", ";
    for (int i = 0; i < WF_COUNT; i++)
        if (afields & (1 << i))
            lbLabels->Caption = lbLabels->Caption + wfieldtext[i] + ", ";
    for (int i = 0; i < NM_COUNT; i++)
        if (anametag & (1 << i))
            lbLabels->Caption = lbLabels->Caption + ntagtext[i] + ", ";
    if (lbLabels->Caption.Length() > 8)
        lbLabels->Caption = lbLabels->Caption.SubString(1, lbLabels->Caption.Length() - 2);

    wobj->wordlist = l;
    r = ShowModal();
    delete l;

    return r;
}
//---------------------------------------------------------------------------
void __fastcall TfImportQuery::BitBtn3Click(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfImportQuery::BitBtn2Click(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfImportQuery::BitBtn1Click(TObject *Sender)
{
    fsel = wobj->pos;
    ModalResult = mrYes;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(59);
#endif
