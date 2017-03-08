//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "listimport.h"
#include "wordobj.h"
#include "module.h"
#include "settings.h"
#include "words.h"
#include "collection.h"
#include "gaplist.h"
#include "studylist.h"
#include "zkformats.h"
#include "msgform.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(98);
#endif


TfListImport *fListImport;

//---------------------------------------------------------------------------
__fastcall TfListImport::TfListImport(TComponent* Owner) : TBaseForm(Owner)
{
    int rows = max(16 * scaleres + 1, rowsizes(settings.dictfontsize));
    wobj = new TWordListObj(pb, rows, wcallback, (TPopupWordProc)NULL);
    wobj->colcount = 3;
    wobj->coltype[0] = wcKanji;
    wobj->coltype[1] = wcKana;
    wobj->coltype[2] = wcMeaning;

    wobj->colsize[0] = 90 + rows;
    wobj->colsize[1] = 120 + rows;
    wobj->colsize[2] = 6250;

    wobj->colresize[0] = true;
    wobj->colresize[1] = true;
    wobj->colresize[2] = false;

    wobj->meaningproc = &meaningproc;
    wobj->prepareproc = &prepareproc;
    wobj->wordselect = false;

    wobj->SetCheckboxHandler(checkproc);

    wobj->checkcolumn[0] = true;
    wobj->checkcolumn[1] = true;
    wobj->checkcolumn[2] = true;

    wobj->selecttype = wlsMultiSelect;
    wobj->multiselection = wlmsContinuousRandom;
    wobj->multicheckcolums = true;

    words = new TAddWordItemTempList;

    EraseBackground = false;
    KeepSize = true;

    Width = settings.ltimportwidth;
    Height = settings.ltimportheight;

    Constrain(537*scaleres, 251*scaleres);
}
//---------------------------------------------------------------------------
__fastcall TfListImport::~TfListImport()
{
    delete wobj;
    for (int ix = 0; ix < words->Count; ++ix)
        deletetemp(words->Items[ix]);
    delete words;
}
//---------------------------------------------------------------------------
void TfListImport::wcallback()
{
    if (wobj->multipleselected)
        edMeaning->Text = "";
    else
        edMeaning->Text = words->Items[wobj->pos]->meaning;
    edMeaning->Enabled = !wobj->multipleselected;
    btnSave->Enabled = !wobj->multipleselected;
    TWordStudyList *study = fcollection->wordstudylist;
    btnReset->Enabled = !wobj->multipleselected && words->Items[wobj->pos]->meaning != study->generate_meaning(wobj->current);

    set_statustext();
}
//---------------------------------------------------------------------------
void TfListImport::set_statustext()
{
    sbInfo->Panels->Items[1]->Text = wobj->pos + 1;
    sbInfo->Panels->Items[3]->Text = IntToStr(wchk[0]) + L"(" + wenb[0] + L")";
    sbInfo->Panels->Items[5]->Text = IntToStr(wchk[1]) + L"(" + wenb[1] + L")";
    sbInfo->Panels->Items[7]->Text = IntToStr(wchk[2]) + L"(" + wenb[2] + L")";
}
//---------------------------------------------------------------------------
UnicodeString TfListImport::meaningproc(int row)
{
    return words->Items[row]->meaning;
}
//---------------------------------------------------------------------------
void TfListImport::prepareproc(int col, int row, TWordColumnType ctype, TCanvas *c)
{
    if (!wobj->checked[row][col])
        c->Font->Color = coloravg((TColor)colorsettings.background, (TColor)colorsettings.maintext, 0.7);
}
//---------------------------------------------------------------------------
void TfListImport::checkproc(unsigned int row, byte ix, bool &checked)
{
    words->Items[row]->qtype[ix] = wobj->checked[row][ix];
    wchk[ix] += wobj->checked[row][ix] ? 1 : -1;
    set_statustext();
}
//---------------------------------------------------------------------------
TAddWordItemTemp* TfListImport::addtemp(int windex)
{
    TAddWordItemTemp *tmp = new TAddWordItemTemp;
    tmp->windex = windex;
    tmp->meaning = "";
    tmp->dindex = -1;
    memset(tmp->qtype, 1, sizeof(bool)*3);

    TWord *w = fcollection->words->Items[windex];
    tmp->nokanji = true;
    for (int ix = wcslen(w->kanji) - 1; ix >= 0 && tmp->nokanji; --ix)
        tmp->nokanji = !KANJI(w->kanji[ix]);

    if (tmp->nokanji)
        tmp->qtype[0] = true;

    tmp->onlykana = tmp->nokanji || (w->data[0].notes & WN_KANAONLY) == WN_KANAONLY;

    words->Add(tmp);
    return tmp;
}
//---------------------------------------------------------------------------
void TfListImport::deletetemp(TAddWordItemTemp *tmp)
{
    delete tmp;
}
//---------------------------------------------------------------------------
int TfListImport::ShowModalEx(TWordIndexList *l)
{
    int r = mrCancel;
    if (!l->Count)
    {
        MsgBox("No items to add to the long-term study list.", "Message", MB_OK);
        return r;
    }

    fcollection = l->collection;
    TWordStudyList *study = l->collection->wordstudylist;
    TWordIndexList *indexes = NULL;

    double n = Now();
    bool showing = false;
    int elemcnt = l->Count * 2 + study->Count;

    TGapList<TAddWordItemTemp *> *useditems = NULL;
    try
    {
        useditems = new TGapList<TAddWordItemTemp *>(false);
        for (int ix = 0; ix < l->Count; ++ix)
        {
            if (!showing && (double)Now() - n > 0.3 / 86400.0 && ix < elemcnt / 3)
            {
                ShowMessageForm("Please wait...");
                showing = true;
            }

            if (!useditems->ItemSet(l->Items[ix]->index))
                useditems->Items[l->Items[ix]->index] = addtemp(l->Items[ix]->index);
        }

        elemcnt -= l->Count;

        for (int ix = 0; ix < study->Count; ++ix)
        {
            if (!showing && (double)Now() - n > 0.3 / 86400.0 && ix < elemcnt / 3)
            {
                ShowMessageForm("Please wait...");
                showing = true;
            }

            int ii = study->wordindex(ix);
            if (!useditems->ItemSet(ii))
                continue;
            if (useditems->Items[ii]->meaning == "")
            {
                useditems->Items[ii]->dindex = ix;
                useditems->Items[ii]->meaning = study->wordmeaning(ix);
            }
            for (int j = 0; j < qtMaxValue; ++j)
                useditems->Items[ii]->qtype[j] = !study->hastype(ix, (TQuestionType)j);
        }

        elemcnt -= study->Count;

        indexes = new TWordIndexList(l->collection);
        for (int ix = 0; ix < words->Count; ++ix)
        {
            if (!showing && (double)Now() - n > 0.3 / 86400.0 && ix < elemcnt / 3)
            {
                ShowMessageForm("Please wait...");
                showing = true;
            }

            TAddWordItemTemp *item = words->Items[ix];
            bool full = true;
            for (int j = 0; j < 3 && full; ++j)
                full = (j == 0 && item->nokanji) || !item->qtype[j];
            if (full)
            {
                deletetemp(item);
                words->Delete(ix);
                ix--;
                continue;
            }
            if (item->meaning == "")
                item->meaning = study->generate_meaning(item->windex);
            indexes->Add(item->windex);
        }

        memset(wchk, 0, sizeof(int)*3);
        memset(wenb, 0, sizeof(int)*3);

        if (indexes->Count)
        {
            sbInfo->Panels->Items[0]->Text = IntToStr(indexes->Count) + "  :";
            wobj->set_by_indexes(indexes);

            for (int iy = 0; iy < words->Count; ++iy)
            {
                for (int ix = 0; ix < 3; ++ix)
                {
                    wobj->checkenabled[iy][ix] = (ix != 0 || !words->Items[iy]->nokanji) && words->Items[iy]->qtype[ix];
                    wenb[ix] += wobj->checkenabled[iy][ix] ? 1 : 0;
                }
                wobj->checked[iy][1] = true;
                wobj->checked[iy][2] = true;
                wobj->checked[iy][0] = !words->Items[iy]->onlykana || !words->Items[iy]->qtype[0];
                if (!wobj->checked[iy][0] && wobj->checkenabled[iy][0])
                    words->Items[iy]->qtype[0] = false;
            }
            set_statustext();
        }
    }
    catch (...)
    {
        if (showing)
            HideMessageForm();

        MsgBox("Error occurred while populating word list.", "Error", MB_OK);
        delete useditems;
        delete indexes;
        return r;
    }
    delete useditems;
    delete indexes;

    if (showing)
        HideMessageForm();

    int addcnt = 0;
    if (wobj->count)
    {
        r = ShowModal();
        SaveFormSize();
        settings.ltimportwidth = SizeInfo.width;
        settings.ltimportheight = SizeInfo.height;
        if (r == mrOk)
        {
            MODON
            bool mod = false;
            for (int ix = 0; ix < words->Count; ++ix)
            {
                bool add = false;
                for (int iy = 0; iy < 3 && !add; ++iy)
                    add = (iy != 0 || !words->Items[ix]->nokanji) && words->Items[ix]->qtype[iy];
                if (!add)
                    continue;
                addcnt++;
                if (words->Items[ix]->dindex >= 0)
                    study->changemeaning(words->Items[ix]->dindex, words->Items[ix]->meaning);
                else
                    words->Items[ix]->dindex = study->Add(words->Items[ix]->windex, words->Items[ix]->meaning.c_str());

                mod = true;

                for (int iy = 0; iy < 3; ++iy)
                {
                    if (iy == 0 && words->Items[ix]->nokanji || !words->Items[ix]->qtype[iy])
                        continue;
                    study->additem(words->Items[ix]->dindex, (TQuestionType)iy, wobj->checked[ix][0]);
                }
            }
            if (!addcnt)
            {
                r = mrCancel;
                MsgBox("No new items were selected for the long-term study list.", "Message", MB_OK);
            }
            if (mod)
                fcollection->Modify(false);
            MODOFF
        }
    }
    else
        MsgBox("No new items to add to long-term study list.", "Message", MB_OK);
    return r;
}
//---------------------------------------------------------------------------
void __fastcall TfListImport::sbTestKanjiClick(TObject *Sender)
{
    bool full = true;
    int ck = ((TSpeedButton*)Sender)->Tag;
    for (int ix = 0; ix < wobj->count && full; ++ix)
        full = !wobj->checkenabled[ix][ck] || wobj->checked[ix][ck];
    for (int ix = 0; ix < wobj->count; ++ix)
        if (wobj->checkenabled[ix][ck] && wobj->checked[ix][ck] != !full)
            wobj->checked[ix][ck] = !full;
}
//---------------------------------------------------------------------------
void __fastcall TfListImport::edMeaningChange(TObject *Sender)
{
    btnSave->Enabled = edMeaning->Text.Trim().Length();
}
//---------------------------------------------------------------------------
void __fastcall TfListImport::btnSaveClick(TObject *Sender)
{
    TWordStudyList *study = fcollection->wordstudylist;
    words->Items[wobj->pos]->meaning = edMeaning->Text.Trim();
    btnReset->Enabled = wobj->count && edMeaning->Text.Trim() != study->generate_meaning(wobj->current);
    wobj->invalidaterow(wobj->pos);
}
//---------------------------------------------------------------------------
void __fastcall TfListImport::btnResetClick(TObject *Sender)
{
    TWordStudyList *study = fcollection->wordstudylist;
    edMeaning->Text = study->generate_meaning(wobj->current);
    btnSave->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfListImport::edMeaningEnter(TObject *Sender)
{
    btnOk->Default = false;
}
//---------------------------------------------------------------------------
void __fastcall TfListImport::edMeaningExit(TObject *Sender)
{
    btnOk->Default = true;
}
//---------------------------------------------------------------------------
void __fastcall TfListImport::edMeaningKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN && btnSave->Enabled)
    {
        btnSave->Click();
        pb->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfListImport::FormPaint(TObject *Sender)
{
    DrawCaptionBar(this, NULL, b1, false);
    DrawPillowPanel(this, b2, b3);
    DrawCloseBar(this, b4, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfListImport::pbKeyPress(TObject *Sender, wchar_t &Key)
{
    ;
}
//---------------------------------------------------------------------------
void __fastcall TfListImport::pbKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_DOWN || Key == VK_UP || Key == VK_LEFT || Key == VK_RIGHT || Key == VK_PRIOR || Key == VK_NEXT || Key == VK_DOWN || Key == VK_HOME || Key == VK_END || Key == VK_SHIFT || Key == VK_CONTROL || Shift.Contains(ssAlt) || Shift.Contains(ssCtrl) || Key == 0 || Key > 254)
        return;

    edMeaning->SetFocus();
    edMeaning->SelStart = edMeaning->Text.Length();

    keybd_event(Key, 0, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfListImport::FormResize(TObject *Sender)
{
    TRect r = Rect(0, b2->Top + b2->Height, ClientWidth, ClientHeight);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE);
}
//---------------------------------------------------------------------------
void __fastcall TfListImport::btnUncheckKanaonlyClick(TObject *Sender)
{
    for (int ix = 0; ix < words->Count; ++ix)
        if (words->Items[ix]->onlykana && wobj->checkenabled[ix][0])
            wobj->checked[ix][0] = false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int ListImport(TWordIndexList *l)
{
    int result;

    fListImport = NULL;

    try
    {
        fListImport = new TfListImport(Application);
        result = fListImport->ShowModalEx(l);
    }
    catch (...)
    {
        result = mrAbort;
    }

    delete fListImport;

    return result;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(99);
#endif


