//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanjilist.h"
#include "wordlist.h"
#include "main.h"
#include "romajizer.h"
#include "kanjiinfo.h"
#include "clipview.h"
#include "grouplist.h"
#include "settings.h"
#include "kanji.h"
#include "zmenu.h"
#include "collection.h"
#include "kanjiobj.h"
#include "kanjigrid.h"
#include "module.h"
#include "popupdict.h"
#include "kanjihint.h"
#include "zrads.h"
#include "modallist.h"
#include "gengraph.h"
#include "docker.h"
#include "closebutton.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(90);
#endif

TfKanji *fKanji;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/*
bool TfKanji::ChangeLimit(int width)
{
    int l = (minlimit + fboxsize * 3) / fboxsize;
    int diff = pbGrid->Width - pbGrid->ClientWidth;
    int extra = (!pWhole->Visible && pWhole->Align == alLeft) ? fboxsize / 2 - 1 : 0;

    return (pWhole->Align == alLeft && width <= l * fboxsize + diff + extra) | (pWhole->Align == alTop && width > l * fboxsize + diff + extra);
}
*/
//---------------------------------------------------------------------------
int TfKanji::get_changewidth()
{
    return fboxsize * 9 + GetSystemMetrics(SM_CXVSCROLL);
}
//---------------------------------------------------------------------------
int TfKanji::get_minlim()
{
    return fboxsize * 5;
}
//---------------------------------------------------------------------------
void TfKanji::do_sizing(TRect r, bool forced)
{
    if (scaleres != 1)
        cbCustom->Width = edCustom->Left - 4 * scaleres - cbCustom->Left;

    if ((forced || pWhole->Align == alLeft) && r.Width() < changewidth)
    {
        try
        {
            pbGrid->AlignWithMargins = false;
            pWhole->Visible = true;
            pFilterTop->Visible = true;
            pWhole->Align = alTop;
            pInfo->Visible = false;
            ArrangeFilters();
            pHide->Visible = false;
            InvalidateEmptyArea(pSort, true);
            lbS->Invalidate();
        }
        catch (...)
        {
            ;
        }
    }
    else if ((forced || pWhole->Align == alTop) && r.Width() >= changewidth)
    {
        try
        {
            pbGrid->AlignWithMargins = btnHide->Tag == 0;
            pWhole->Visible = btnHide->Tag == 0;
            pFilterTop->Visible = pWhole->Visible;
            bvHideLeft->Visible = btnHide->Tag == 0;
            pWhole->Align = alLeft;
            pInfo->Width = fboxsize * 6 - 1;
            pInfo->Visible = true;
            pbReadings->Width = pInfo->Width;
            pbMeanings->Width = pInfo->Width - pbMeanings->Left;
            pHide->Visible = true;
            pWhole->Width = fboxsize * 6 - 1;
            TPoint p = ScreenToClient(pSort->ClientToScreen(Point(0, 0)));
            TRect r = Rect(p.x, p.y, p.x + pSort->Width, p.y + pSort->Height);
            RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_VALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
            ArrangeFilters();
        }
        catch (...)
        {
            ;
        }
    }
}
//---------------------------------------------------------------------------
void TfKanji::WMSizing(TWMSizing &msg)
{
    ConstraintSizing(msg, Width - ClientWidth + GetSystemMetrics(SM_CXVSCROLL) + fboxsize * 5, Height - pbGrid->ClientHeight + fboxsize * (pWhole->Align == alLeft ? 5 : 1), 200000, 200000, /*Width - ClientWidth + pbGrid->Width - pbGrid->ClientWidth, fboxsize,*/ 0, 0, 0, 0);

    int mw = GetSystemMetrics(SM_CXMAXTRACK);
    if (mw < msg.lprc->Width())
    {
        int d = (mw - (Width - pbGrid->ClientWidth)) % fboxsize;
        switch (msg.fwSide)
        {
        case WMSZ_LEFT:
        case WMSZ_TOP:
        case WMSZ_BOTTOMLEFT:
            msg.lprc->left = msg.lprc->right - mw + d;
            break;
        case WMSZ_RIGHT:
        case WMSZ_TOPRIGHT:
        case WMSZ_BOTTOMRIGHT:
            msg.lprc->right = msg.lprc->left + mw - d;
            break;
        }
    }
}
//---------------------------------------------------------------------------
__fastcall TfKanji::TfKanji(TComponent* Owner) : rads2(NULL), clipviewing(false), clipnext(NULL), fontop(false), TBaseForm(Owner)
{
    //RescaleStatusbar(sbInfo);

    kbox = new TKanjiDrawObj(pbKanji);
    fboxsize = 50 * scaleres;

    //fminlimit = fboxsize * 5 + 23 * scaleres;

    nofilter = true;
    fkobj = new TKanjiGrid(pbGrid, fboxsize, true, &kcallback);
    fkobj->allowcopy = true;
    memset(rads, 0, sizeof(bool)*RAD_COUNT);
    radselected = 0;
    edreading = new TKanjiEdit(edReading, false, false, NULL, &ecallback);
    kcallback();

    sbInfo->Tag = -1;

    StartClipviewing();
    SettingsChangeEvent(this, settingsChanged, true);
    AppActivateEvent(this, appDeactivate, false, true);

    RestoreOldActive = true;
    KeepSize = true;

    sbF1->Flat = !MatchVersion(6, 0);
    sbF2->Flat = sbF1->Flat;
    sbF3->Flat = sbF1->Flat;
    sbF4->Flat = sbF1->Flat;
    sbF5->Flat = sbF1->Flat;
    sbF6->Flat = sbF1->Flat;
    sbFAll->Flat = sbF1->Flat;
    sbReset->Flat = sbF1->Flat;

    // Building menus.
    gridmenu = new TZMenuRows(3);
    gridmenuextra = new TZMenuRows(3);
    gridmenucombined = new TZMenuRows(3);

    wchar_t *gridnames[] =
    { L"Show", L"Kanji Info", L"Translate", L"Clpb: Kanji", L"Copy", L"Append", L"Clpb: All", L"Copy", L"Append", L"Kanji", L"To Group...", L"Collect Words..." };
    TZMenuItemType gridtypes[] =
    { itTitle, itSimple, itSimple, itAltTitle, itSimple, itSimple, itAltTitle, itSimple, itSimple, itTitle, itSimple, itSimple };
    TZPopupCommand gridcmd[] =
    { pcNone, pcKanjiInfo, pcTranslate, pcNone, pcCopy1, pcAppend1, pcNone, pcCopy2, pcAppend2, pcNone, pcToGroup, pcCollect };

    wchar_t *gridnamesextra[] =
    { L"Definition", L"Edit...", L"" };
    TZMenuItemType gridtypesextra[] =
    { itTitle, itSimple, itEmpty };
    TZPopupCommand gridcmdextra[] =
    { pcNone, pcEditKanjiDef, pcNone };

    for (unsigned int ix = 0; ix < (sizeof(gridnames) / sizeof(wchar_t*)) / 3; ++ix)
    {
        TZMenuRow *row = gridmenu->AddRow();
        row->Setup(gridnames + (ix * 3), gridtypes + (ix * 3), (void**)(gridcmd + (ix*3)));
    }
    for (unsigned int ix = 0; ix < (sizeof(gridnamesextra) / sizeof(wchar_t*)) / 3; ++ix)
    {
        TZMenuRow *row = gridmenuextra->AddRow();
        row->Setup(gridnamesextra + (ix * 3), gridtypesextra + (ix * 3), (void**)(gridcmdextra + (ix*3)));
    }

    gridmenucombined->AppendRows(gridmenu);
    gridmenucombined->AppendRows(gridmenuextra);

    closebtn = new TMidCloseButton(this, sbpTopRight, 2, 2, &bgdraw, &unpop);
    closebtn->Visible = false;

    AppWindow = true;

    AppActivateEvent(this, repaintProc, true, true);
    AppActivateEvent(this, repaintProc, false, true);
    AddEvent(this, repaintProc, eltActivate);
    AddEvent(this, repaintProc, eltDeactivate);
}
//---------------------------------------------------------------------------
__fastcall TfKanji::~TfKanji()
{
    AppActivateEvent(this, repaintProc, false, false);
    AppActivateEvent(this, repaintProc, true, false);
    RemoveEvent(this, repaintProc, eltDeactivate);
    RemoveEvent(this, repaintProc, eltActivate);

    delete closebtn;

    SettingsChangeEvent(this, settingsChanged, false);
    AppActivateEvent(this, appDeactivate, false, false);
    StopClipviewing();

    delete gridmenucombined;
    delete gridmenuextra;
    delete gridmenu;

    //if (options)
    //FreeOptions();

    delete kbox;
    delete fkobj;
    delete edreading;
}
//---------------------------------------------------------------------------
void TfKanji::StartClipviewing()
{
    if (clipviewing)
        return;
    clipnext = SetClipboardViewer(Handle);
    clipviewing = true;
}
//---------------------------------------------------------------------------
void TfKanji::StopClipviewing()
{
    if (!clipviewing)
        return;
    ChangeClipboardChain(Handle, clipnext);
    clipnext = NULL;
    clipviewing = false;
}
//---------------------------------------------------------------------------
void TfKanji::settingsChanged(void *caller, TSettingsProcState state)
{
    if (state != spsEnd)
        return;

    if (Visible && pInfo->Visible)
    {
        pbReadings->Refresh();
        pbKanji->Refresh();
        pbMeanings->Refresh();
    }
}
//---------------------------------------------------------------------------
void TfKanji::ecallback()
{
    if (edReading->Focused() && edreading->Text != L"")
        cbReading->Checked = true;
    FilterKanji();
}
//---------------------------------------------------------------------------
/*void __fastcall TfKanji::SaveOptions()
 {
 if (options)
 FreeOptions();

 options = new TSearchOptions;
 memset(options,0,sizeof(TSearchOptions));

 options->usemeaning = cbMeaning->Checked;
 options->usereading = cbReading->Checked;
 options->usestrokes = cbStrokes->Checked;
 options->usejouyou = cbJouyou->Checked;
 options->useJLPT = cbJLPT->Checked;
 if (edMeaning->Text.Trim().Length()) {
 options->meaning = new wchar_t[edMeaning->Text.Trim().Length()+1];
 wcscpy(options->meaning,edMeaning->Text.Trim().c_str());
 }
 if (edStrokes->Text.Trim().Length()) {
 options->strokes = new wchar_t[edStrokes->Text.Trim().Length()+1];
 wcscpy(options->strokes,edStrokes->Text.Trim().c_str());
 }
 if (edreading->Text.Trim().Length()) {
 options->reading = new wchar_t[edreading->Text.Trim().Length()+1];
 wcscpy(options->reading,edreading->Text.Trim().c_str());
 }
 options->on = cbOn->Checked;
 options->kun = cbKun->Checked;
 options->oku = cbOku->Checked;
 if (edJouyou->Text.Trim().Length()) {
 options->jouyou = new wchar_t[edJouyou->Text.Trim().Length()+1];
 wcscpy(options->jouyou,edJouyou->Text.Trim().c_str());
 }
 if (edJLPT->Text.Trim().Length()) {
 options->JLPT = new wchar_t[edJLPT->Text.Trim().Length()+1];
 wcscpy(options->JLPT,edJLPT->Text.Trim().c_str());
 }
 options->radtype = fradtype;
 if (radselected) {
 if (fradtype < 2) {
 options->radcnt = 0;
 for (int i = 0; i < RAD_COUNT; i++)
 if (rads[i])
 options->radcnt++;
 options->rads = new byte[options->radcnt];
 int n = 0;
 for (int i = 0; i < RAD_COUNT && n < options->radcnt; i++)
 if (rads[i]) options->rads[n++] = i;
 } else {
 if (rads2) {
 options->rads2 = new T-List;
 for (int i = 0; i < rads2->Count; i++) {
 T-List *l = new T-List;
 for (int j = 0; j < ((T-List*)rads2->Items[i])->Count; j++)
 l->Add(((T-List*)rads2->Items[i])->Items[j]);
 options->rads2->Add(l);
 }
 }
 }
 }
 options->useskip = cbSKIP->Checked;
 if (options->useskip) {
 options->SKIP[0] = cbSKIP->Tag;
 options->SKIP[1] = (edS2->Text.Trim().Length() ? edS2->Text.Trim().ToInt() : -1);
 options->SKIP[2] = (edS3->Text.Trim().Length() ? edS3->Text.Trim().ToInt() : -1);
 }
 options->use4c = cb4C->Checked;
 if (options->use4c) {
 options->C4[0] = (edC1->Text.Trim().Length() ? edC1->Text.Trim().ToInt() : -1);
 options->C4[1] = (edC2->Text.Trim().Length() ? edC2->Text.Trim().ToInt() : -1);
 options->C4[2] = (edC3->Text.Trim().Length() ? edC3->Text.Trim().ToInt() : -1);
 options->C4[3] = (edC4->Text.Trim().Length() ? edC4->Text.Trim().ToInt() : -1);
 options->C4[4] = (edC5->Text.Trim().Length() ? edC5->Text.Trim().ToInt() : -1);
 }
 options->custom_type = cbCustom->ItemIndex;
 if (edCustom->Text.Trim().Length()) {
 options->custom = new wchar_t[edCustom->Text.Trim().Length()+1];
 wcscpy(options->custom,edCustom->Text.Trim().c_str());
 }

 if (fkobj->pos >= 0)
 options->kindex = Kanjis[fkobj->pos]->index;
 }
 //---------------------------------------------------------------------------
 void __fastcall TfKanji::RestoreOptions()
 {
 if (!options)
 return;

 int k = options->kindex;

 nofilter = true;
 try {
 cbMeaning->Checked = options->usemeaning;
 cbReading->Checked = options->usereading;
 cbStrokes->Checked = options->usestrokes;
 cbJouyou->Checked = options->usejouyou;
 cbJLPT->Checked = options->useJLPT;
 edMeaning->Text = options->meaning ? options->meaning : L"";
 edStrokes->Text = options->strokes ? options->strokes : L"";
 edreading->Text = options->reading ? options->reading : L"";
 cbOn->Checked = options->on;
 cbKun->Checked = options->kun;
 cbOku->Checked = options->oku;
 edJouyou->Text = options->jouyou ? options->jouyou : L"";
 edJouyou->Text = options->JLPT ? options->JLPT : L"";
 fradtype = options->radtype;
 memset(rads,0,sizeof(bool)*RAD_COUNT);
 radselected = (options->radtype < 2 && options->rads) || (options->radtype == 2 && options->rads2);
 if (radselected) {
 if (options->radtype < 2) {
 for (int i = 0; i < options->radcnt; i++)
 rads[options->rads[i]] = true;
 btnRads->Enabled = true;
 } else if (options->rads2) {
 clearrads2(rads2);
 rads2 = options->rads2;
 options->rads2 = NULL;
 }
 } else
 btnRads->Enabled = false;
 if (options->useskip) {
 cbSKIP->Tag = options->SKIP[0];
 TSpeedButton *sbs[] = { sbS1, sbS2, sbS3, sbS4 };
 sbs[cbSKIP->Tag-1]->Down = true;
 sbs[cbSKIP->Tag-1]->Click();
 edS2->Text = options->SKIP[1] <= 0 ? (UnicodeString)L"" : (UnicodeString)((int)options->SKIP[1]);
 edS3->Text = options->SKIP[2] <= 0 ? (UnicodeString)L"" : (UnicodeString)((int)options->SKIP[2]);
 } else {
 sbS1->Down = true;
 cbSKIP->Tag = 1;
 edS2->Text = L"";
 edS3->Text = L"";
 }
 cbSKIP->Checked = options->useskip;

 TEdit2 *eds[] = { edC1, edC2, edC3, edC4, edC5 };
 for (int i = 0; i < (int)(sizeof(eds)/sizeof(TEdit2*)); i++)
 eds[i]->Text = options->C4[i] < 0 || !options->use4c ? (UnicodeString)L"" : (UnicodeString)((int)options->C4[i]);
 cb4C->Checked = options->use4c;

 cbCustom->ItemIndex = options->custom_type;
 edCustom->Text = options->custom ? options->custom : L"";

 pbRadicals->Refresh();
 FreeOptions();
 } catch(...) {
 }

 nofilter = false;


 FilterKanji();
 fkobj->kanji = kanjis->Kanjis[k];
 }
 //---------------------------------------------------------------------------
 void __fastcall TfKanji::FreeOptions()
 {
 if (!options)
 return;

 delete[] options->meaning;
 delete[] options->strokes;
 delete[] options->reading;
 delete[] options->jouyou;
 delete[] options->JLPT;
 delete[] options->rads;
 delete[] options->custom;
 clearrads2(options->rads2);
 delete options;
 options = NULL;
 }*/
//---------------------------------------------------------------------------
void TfKanji::kcallback()
{
    if (fInfo && (Screen->ActiveForm == fKanji || fDock && Screen->ActiveForm == (TForm*)fDock))
    {
        TWinControl *ctrl = Screen->ActiveForm->ActiveControl;
        while (ctrl && ctrl->Parent && ctrl != fKanji)
            ctrl = ctrl->Parent;
        if (ctrl == fKanji)
        {
            fInfo->collection = settings.dictionary_index;
            fInfo->kanji = fkobj->kanji;
        }
    }

    kbox->kanji = (fkobj->kanji ? fkobj->kanji->ch : NULL);
    UpdateData();

    fGroup->UpdateKButtons();
    fDict->UpdateKButtons();

    if (fDock)
    {
        fDock->miKCopy->Enabled = fkobj->count;
        fDock->miKAppend->Enabled = fkobj->count;
        fDock->miKCopyAll->Enabled = fkobj->count;
        fDock->miKAppendAll->Enabled = fkobj->count;
        fDock->miKKanjiDef->Enabled = fkobj->count && dictionary->Index[collection];
    }

    if (!fkobj->count || !kanji)
    {
        sbInfo->Panels->Items[0]->Text = L" :";
        sbInfo->Panels->Items[1]->Text = L"";
        sbInfo->Panels->Items[2]->Text = L"U: -";
        sbInfo->Panels->Items[3]->Text = L"G: -";
        sbInfo->Panels->Items[4]->Text = L"F: -";
        sbInfo->Panels->Items[5]->Text = L"J: -";
        sbInfo->Invalidate();
        if (fDock)
        {
            fDock->miKTranslate->Enabled = false;
            fDock->miKInfo->Enabled = false;
            fDock->miKGenerate->Enabled = fkobj->count;
        }
        return;
    }

    if (fDock)
    {
        fDock->miKTranslate->Enabled = true;
        fDock->miKInfo->Enabled = true;
        fDock->miKGenerate->Enabled = true;
    }

    sbInfo->Panels->Items[0]->Text = IntToStr(fkobj->count) + L" :";
    sbInfo->Panels->Items[1]->Text = IntToStr(fkobj->pos + 1);
    if (sbInfo->Panels->Items[2]->Text.Length() != 7)
        sbInfo->Panels->Items[2]->Text.SetLength(7);
    sbInfo->Panels->Items[2]->Text = (UnicodeString)L"U: " + wtoAS(fkobj->kanji->_uni);
    sbInfo->Panels->Items[2]->Text.SetLength(7);
    if (fkobj->kanji->jouyou < 9)
        sbInfo->Panels->Items[3]->Text = (UnicodeString)L"G: " + (fkobj->kanji->jouyou > 0 ? IntToStr(fkobj->kanji->jouyou).c_str() : L"-");
    else
        sbInfo->Panels->Items[3]->Text = (UnicodeString)L"G: 名" +int(fkobj->kanji->jouyou);
    sbInfo->Panels->Items[4]->Text = (UnicodeString)L"F: " + (fkobj->kanji->frequency);
    sbInfo->Panels->Items[5]->Text = (UnicodeString)L"J: " + (fkobj->kanji->NJLPT ? (L"N" + IntToStr(fkobj->kanji->NJLPT)).c_str() : fkobj->kanji->JLPT >= 1 && fkobj->kanji->JLPT <= 4 ? IntToStr(fkobj->kanji->JLPT).c_str() : L"-");

    sbInfo->Refresh();
}
//---------------------------------------------------------------------------

void TfKanji::FilterKanji()
{
    if (nofilter)
        return;

    UnicodeString readingtext = L"";
    if (cbReading->Checked)
        readingtext = Romanize(edreading->Text.c_str());

    if ((!cbMeaning->Checked || !edMeaning->Text.Length()) && (!cbReading->Checked || !readingtext.Length()) && (!cbStrokes->Checked || !edStrokes->Text.Length()) && !cbSKIP->Checked && (!cbJouyou->Checked || !edJouyou->Text.Length()) && (!cbJLPT->Checked || !edJLPT->Text.Length()) && !radselected && !fRadicals && !cb4C->Checked && !edCustom->Text.Trim().Length() && !cbList->ItemIndex)
    {

        fkobj->set_items(kanjis, fkobj->collection, fkobj->pos >= 0 ? fkobj->kanjis[fkobj->pos] : NULL);
        return;
    }

    byte fclass = (fRadicals ? fRadicals->tcSelect->TabIndex : fradtype);
    bool *sel = (fRadicals ? fRadicals->selected : rads);
    TRadGroup *rads2t = (fRadicals ? radlist->Groups : rads2);

    UnicodeString z;
    if (cbList->ItemIndex == 1)
        GetClipText(z, 0);

    TKanjiItemList *results = new TKanjiItemList();
    int n;
    UnicodeString s;
    if (cbMeaning->Checked)
        s = edMeaning->Text.LowerCase().Trim();
    UnicodeString t;

    //radicals
    if (!fclass && (!fRadicals && radselected || fRadicals && fRadicals->selcount))
    {
        int c = 9999;
        //find radical with the least number of results
        for (int i = 0; i < RAD_COUNT; i++)
            if (sel[i] && partlist[i][0] < c)
            {
                c = partlist[i][0];
                n = i;
            }
        for (int i = 1; i <= partlist[n][0]; i++)
            results->Add(kanjis->Kanjis[partlist[n][i]]);

        for (int k = results->Count - 1; k >= 0; k--)
        {
            bool found = true;
            for (int i = 0; i < RAD_COUNT && found; i = (i == n - 1 ? i + 2 : i + 1))
            {
                found = !sel[i];
                if (!found)
                    for (int j = 1; j <= partlist[i][0] && !found; j++)
                        found = (partlist[i][j] == results->Kanjis[k]->index);
            }
            if (!found)
                results->Delete(k);
        }
    }
    else
        results->Assign(kanjis);

    //meaning
    if (s.Length())
        for (int i = results->Count - 1; i >= 0; i--)
        {
            bool found = false;

            t = dictionary->KanjiMeaning(dictionary->Index[collection], results->Kanjis[i]->index);
            wchar_t *c = t.c_str();

            while (c && !found)
            {
                c = WCSSTRI(c, s.c_str());
                if (c && (c == results->Kanjis[i]->meaning || wcschr(cdelim, *(c - 1))))
                    found = true;
                else if (c)
                    c++;
            }
            if (!found)
                results->Delete(i);
        }

    bool found;
    if (fclass == 1 && (!fRadicals && radselected || fRadicals && fRadicals->selcount))
    {
        for (int i = results->Count - 1; i >= 0; i--)
        {
            found = false;
            for (int j = 0; j < 214 && !found; j++)
                if (sel[j])
                    found = (results->Kanjis[i]->radical - 1 == j);
            if (!found)
                results->Delete(i);
        }
    }
    else if (fclass == 2 && (rads2t || fRadicals && fRadicals->selcount))
    {
        if (fRadicals && fRadicals->selcount)
        {
            TIntegerList *list = new TIntegerList;
            if (!rads2t)
                rads2t = new TRadGroup;
            rads2t->Add(list);
            fRadicals->AddSelected2(list);
        }

        for (int i = results->Count - 1; i >= 0; i--)
            if (!radlist->MatchRadicals(results->Kanjis[i]->ch, rads2t))
                results->Delete(i);

        if (fRadicals && fRadicals->selcount)
        {
            delete rads2t->Items[rads2t->Count - 1];
            rads2t->Delete(rads2t->Count - 1);
            if (!rads2t->Count)
                delete rads2t;
        }
    }

    if (cbList->ItemIndex > 0)
    {
        for (int i = results->Count - 1; i >= 0; i--)
        {
            switch (cbList->ItemIndex)
            {
            case 1: //clipboard
                if (!wcschr(z.c_str(), results->Kanjis[i]->ch))
                    results->Delete(i);
                break;
            case 2: //common
                if (!results->Kanjis[i]->frequency)
                    results->Delete(i);
                break;
            case 3: //jouyou
                if (results->Kanjis[i]->jouyou > 9)
                    results->Delete(i);
                break;
            case 4: //JLPT
                if (!results->Kanjis[i]->JLPT)
                    results->Delete(i);
                break;
            case 5: //gakken
                if (!results->Kanjis[i]->gakken)
                    results->Delete(i);
                break;
            case 6: //halpern
                if (!results->Kanjis[i]->halpern)
                    results->Delete(i);
                break;
            case 7: //heisig
                if (!results->Kanjis[i]->heisig)
                    results->Delete(i);
                break;
            case 8: //henshall
                if (!results->Kanjis[i]->henshall)
                    results->Delete(i);
                break;
            case 9: //nelson
                if (!results->Kanjis[i]->nelson)
                    results->Delete(i);
                break;
            case 10: //KnK
                if (!results->Kanjis[i]->knk)
                    results->Delete(i);
                break;
            case 11: //KnK
                if (!results->Kanjis[i]->knk || results->Kanjis[i]->knk > 1945)
                    results->Delete(i);
                break;
            case 12: //SnH
                if (results->Kanjis[i]->SnH[0] == 'x')
                    results->Delete(i);
                break;
            }
        }
    }

    if (readingtext.Length())
    {
        bool found;
        s = readingtext;
        for (int i = results->Count - 1; i >= 0; i--)
        {
            found = false;
            if (cbOn->Checked)
                for (int j = 0; j < results->Kanjis[i]->oncnt && !found; j++)
                    if (!wcscmp(Romanize(results->Kanjis[i]->on[j]).c_str(), s.c_str()))
                        found = true;

            if (cbKun->Checked)
                for (int j = 0; j < results->Kanjis[i]->kuncnt && !found; j++)
                {
                    UnicodeString r = results->Kanjis[i]->kun[j];
                    if (cbOku->Checked && readingtext == Romanize(r.c_str()))
                        found = true;
                    else if ((n = r.Pos(L".")) > 0)
                    {
                        if (readingtext == Romanize(r.SubString(1, (n ? n - 1 : r.Length())).c_str()))
                            found = true;
                    }
                }

            if (!found)
                results->Delete(i);
        }
    }

    try
    {
        if (cbStrokes->Checked && edStrokes->Text.Length() && edStrokes->Text.ToInt() > 0)
        {
            int min, max;
            n = edStrokes->Text.Pos('-');
            if (!n)
                min = max = edStrokes->Text.ToInt();
            else
            {
                if (n == 1)
                    min = 0;
                else
                    min = edStrokes->Text.SubString(1, n - 1).ToInt();
                if (n == edStrokes->Text.Length())
                    max = 25;
                else
                    max = edStrokes->Text.SubString(n + 1, 3).ToInt();
            }
            for (int i = results->Count - 1; i >= 0; i--)
                if (results->Kanjis[i]->strokes < min || results->Kanjis[i]->strokes > max)
                    results->Delete(i);
        }
    }
    catch (EConvertError *e)
    {
        //do nothing, just skip this part
    }

    try
    {
        if (cbJouyou->Checked && edJouyou->Text.Length())
        {
            int min, max;
            n = edJouyou->Text.Pos('-');
            if (!n)
                min = max = edJouyou->Text.ToInt();
            else
            {
                if (n == 1)
                    min = 0;
                else
                    min = edJouyou->Text.SubString(1, n - 1).ToInt();
                if (n == edJouyou->Text.Length())
                    max = 25;
                else
                    max = edJouyou->Text.SubString(n + 1, 3).ToInt();

                if (min > 0 && max < 25 && max < min)
                    swap(max, min);
            }
            for (int i = results->Count - 1; i >= 0; i--)
                if (results->Kanjis[i]->jouyou < min || results->Kanjis[i]->jouyou > max)
                    results->Delete(i);
        }
    }
    catch (EConvertError *e)
    {
        //do nothing, just skip this part
    }

    try
    {
        if (cbJLPT->Checked && edJLPT->Text.Length())
        {
            int min, max;
            n = edJLPT->Text.Pos('-');
            if (!n)
                min = max = edJLPT->Text.ToInt();
            else
            {
                if (n == 1)
                    min = 0;
                else
                    min = edJLPT->Text.SubString(1, n - 1).ToInt();
                if (n == edJLPT->Text.Length())
                    max = 25;
                else
                    max = edJLPT->Text.SubString(n + 1, 3).ToInt();

                if (min > 0 && max < 25 && max < min)
                    swap(max, min);
            }
            for (int i = results->Count - 1; i >= 0; i--)
                if (results->Kanjis[i]->NJLPT < 1 && (results->Kanjis[i]->JLPT < min || results->Kanjis[i]->JLPT > max) || results->Kanjis[i]->NJLPT >= 1 && (results->Kanjis[i]->NJLPT < min || results->Kanjis[i]->NJLPT > max))
                    results->Delete(i);
        }
    }
    catch (EConvertError *e)
    {
        //do nothing, just skip this part
    }

    if (cbSKIP->Checked)
    {
        int S1, S2 = -1, S3 = -1;
        S1 = cbSKIP->Tag;
        try
        {
            if (edS2->Text.Length())
                S2 = edS2->Text.ToInt();
            try
            {
                if (edS3->Text.Length())
                    S3 = edS3->Text.ToInt();
            }
            catch (EConvertError *e)
            {
                //do nothing, just skip this part
            }
        }
        catch (EConvertError *e)
        {
            //do nothing, just skip this part
        }
        if (!S2)
            S2 = -1;
        if (!S3)
            S3 = -1;
        for (int i = results->Count - 1; i >= 0; i--)
            if (results->Kanjis[i]->SKIP1 != S1 || S2 >= 0 && results->Kanjis[i]->SKIP2 != S2 || S3 >= 0 && results->Kanjis[i]->SKIP3 != S3)
                results->Delete(i);
    }

    if (cb4C->Checked)
    {
        char c1 = ' ';
        char c2 = ' ';
        char c3 = ' ';
        char c4 = ' ';
        char c5 = ' ';

        if (edC1->Text.Length() && edC1->Text[1] >= L'0' && edC1->Text[1] <= L'9')
            c1 = edC1->Text[1];
        if (edC2->Text.Length() && edC2->Text[1] >= L'0' && edC2->Text[1] <= L'9')
            c2 = edC2->Text[1];
        if (edC3->Text.Length() && edC3->Text[1] >= L'0' && edC3->Text[1] <= L'9')
            c3 = edC3->Text[1];
        if (edC4->Text.Length() && edC4->Text[1] >= L'0' && edC4->Text[1] <= L'9')
            c4 = edC4->Text[1];
        if (edC5->Text.Length() && edC5->Text[1] >= L'0' && edC5->Text[1] <= L'9')
            c5 = edC5->Text[1];

        for (int i = results->Count - 1; i >= 0; i--)
            if (c1 != ' ' && results->Kanjis[i]->c4[0] != c1 || c2 != ' ' && results->Kanjis[i]->c4[1] != c2 || c3 != ' ' && results->Kanjis[i]->c4[2] != c3 || c4 != ' ' && results->Kanjis[i]->c4[3] != c4 || c5 != ' ' && results->Kanjis[i]->c4b != c5)
                results->Delete(i);
    }

    if (edCustom->Text.Trim().Length())
    {
        UnicodeString custom = edCustom->Text.Trim().LowerCase();
        UnicodeString checker = L"";

        int cl, cp;

        for (int i = results->Count - 1; i >= 0; i--)
        {
            switch (cbCustom->ItemIndex)
            {
            case siGakken: //Gakken Kanji Dictionary
                if (IntToStr(results->Kanjis[i]->gakken) != custom)
                    results->Delete(i);
                break;
            case siHalpern: //Halpern Index
                if (IntToStr(results->Kanjis[i]->halpern) != custom)
                    results->Delete(i);
                break;
            case siHeisig: //Heisig Index
                if (IntToStr(results->Kanjis[i]->heisig) != custom)
                    results->Delete(i);
                break;
            case siHenshall: //Henshall Index
                if (IntToStr(results->Kanjis[i]->henshall) != custom)
                    results->Delete(i);
                break;
            case siKnK: //S&H Kanji&Kana
                if (IntToStr(results->Kanjis[i]->knk) != custom)
                    results->Delete(i);
                break;
            case siNelson: //Nelson Index
                if (IntToStr(results->Kanjis[i]->nelson) != custom)
                    results->Delete(i);
                break;
            case siSnH: //Spahn&Hadamatsky The Kanji Dictionary
                if (((UnicodeString)results->Kanjis[i]->SnH).SubString(1, 8).LowerCase().Pos(custom.SubString(1, 8)) != 1)
                    results->Delete(i);
                break;
            case siUNICODE: //Unicode
                        checker = wtoAS(results->Kanjis[i]->_uni);
                if (checker.Pos(custom.SubString(1, 4)) != 1)
                    results->Delete(i);
                break;
            case siJIS: //JIS code
                        checker = wtoAS(results->Kanjis[i]->_jis);
                if (checker.Pos(custom.SubString(1, 4)) != 1)
                    results->Delete(i);
                break;
            case siSJIS: //Shift-JIS code
                        checker = wtoAS(JIStoShiftJIS(results->Kanjis[i]->_jis));
                if (checker.Pos(custom.SubString(1, 4)) != 1)
                    results->Delete(i);
                break;
            case siEUC: //EUC-JP code
                        checker = wtoAS(JIStoEUC(results->Kanjis[i]->_jis));
                if (checker.Pos(custom.SubString(1, 4)) != 1)
                    results->Delete(i);
                break;
            case siKuten: //Kuten code point
                        cl = custom.Length();
                cp = custom.Pos(L"-");

                if (!cp && cl > 2 || cp > 3 || cl > cp + 2)
                {
                    results->Delete(i);
                    break;
                }
                if (!cl || cp == 1 && cl == 1)
                    break;

                checker = JIStoKuten(results->Kanjis[i]->_jis);

                if (cp == 0 || cp > 1)
                {
                    if (custom.SubString(1, cp ? cp - 1 : cl) != checker.SubString(1, checker.Pos(L"-") - 1))
                    {
                        results->Delete(i);
                        break;
                    }
                }
                if (cp == 0 || cp == cl)
                    break;

                if (custom.SubString(cp + 1, cl - cp) != checker.SubString(checker.Pos(L"-") + 1, 5))
                {
                    results->Delete(i);
                    break;
                }
                break;
            }
        }
    }

    fkobj->set_items(results, fkobj->collection, fkobj->pos >= 0 ? fkobj->kanjis[fkobj->pos] : NULL);

    delete results;
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caNone;
    fMain->Close();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::cbSortlistChange(TObject *Sender)
{
    fkobj->sort_by = (TKanjiSortType)(cbSortlist->ItemIndex + 1);
    fkobj->Redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::edChange(TObject *Sender)
{
    FilterKanji();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::pbRadicalsPaint(TObject *Sender)
{
    TCanvas *c = pbRadicals->Canvas;
    c->Brush->Color = clWhite;
    c->Pen->Color = clBtnShadow;
    if (pbRadicals->Tag)
        DARKEN(c->Brush->Color, 0xfafaf0);
    TFont *f = pbRadicals->Canvas->Font;
    TRect r = pbRadicals->ClientRect;
    TPoint p[] = { Point(0, 0), Point(r.right - 1, 0), Point(r.right - 1, r.bottom - 1), Point(0, r.bottom - 1), Point(0, 0) };
    c->Polyline(p, 4);

    if (!radselected)
    {
        SetTextAlign(c->Handle, TA_BASELINE | TA_CENTER);
        InflateRect(&r, -1, -1);
        f->Name = SystemFont;
        f->Size = 8;
        f->Style = TFontStyles() << fsBold;
        f->Charset = DEFAULT_CHARSET;
        ExtTextOutW(c->Handle, pbRadicals->ClientWidth / 2, pbRadicals->ClientHeight*0.75, ETO_OPAQUE | ETO_CLIPPED, &r, L"Radicals: Click here!", 21, 0);
        return;
    }

    SetRadicalFont(f);
    f->Size = 14;
    f->Style = TFontStyles();
    if (fradtype < 2)
    {
        int x = 1;
        SetTextAlign(c->Handle, TA_BASELINE | TA_LEFT);

        TSize s;
        wchar_t rad;
        for (int i = 0; i < RAD_COUNT; i++)
        {
            if (rads[i] && fradtype <= 1)
                rad = (fradtype == 1 ? ::radicals[i] : parts[i]);
            else
                continue;
            GetTextExtentPoint32W(c->Handle, &rad, 1, &s);
            if (x + s.cx > pbRadicals->ClientWidth)
                break;
            r = Rect(x, 1, x + s.cx, pbRadicals->ClientHeight - 1);
            ExtTextOutW(c->Handle, x + 1, pbRadicals->ClientHeight*0.9 - 1, ETO_OPAQUE | ETO_CLIPPED, &r, &rad, 1, 0);
            x += s.cx;
        }
        c->FillRect(Rect(x, 1, pbRadicals->ClientWidth - 1, pbRadicals->ClientHeight - 1));
    }
    else
    {

        InflateRect(&r, -1, -1);

        UnicodeString s;
        for (int i = 0; i < rads2->Count; i++)
        {
            if (s.Length())
                s += L"+";
            TIntegerList *l = rads2->Items[i];
            for (int j = 0; j < l->Count; j++)
                s += radlist->Items[l->Items[j]]->ch;
        }

        SetTextAlign(c->Handle, TA_BASELINE | TA_LEFT);
        ExtTextOutW(c->Handle, 2, 1 + r.Height()*0.88, ETO_CLIPPED | ETO_OPAQUE, &r, s.c_str(), s.Length(), NULL);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::FormResize(TObject *Sender)
{
    if (Visible)
        sbInfo->Invalidate();

    do_sizing(ClientRect, false);

    // Make panel edges pretty (no fullrepaint!)
    if (Visible)
    {
        TRect r;
        if (pWhole->Align == alTop)
        {
            TPanel *p[] = { pFilterTop, pFilter1, pFilter2, pFilter3, pFilter4, pFilter5, pFilter6 };
            for (int i = 0; i < (int)(sizeof(p) / sizeof(TPanel*)); i++)
            {
                r = p[i]->ClientRect;
                r.left = r.right - 1;
                InvalidateRect(p[i]->Handle, &r, false);
            }
        }
        if (pInfo->Visible)
        {
            InvalidateEmptyArea(pWhole, false);
            InvalidateEmptyArea(pInfo, false);
        }
    }

    if (ontop)
    {
        TRect rold;
        GetWindowRgnBox(Handle, &rold);
        TPoint p = ClientToScreen(TPoint(0, 0));
        OffsetRect(rold, p.x - 4, p.y - 4);

        HRGN rgn = CreateRoundRectRgn(0, 0, Width + 1, Height + 1, 9, 9);
        SetWindowRgn(Handle, rgn, false);

        if (rold.Height() < Height)
        {
            TRect r = Rect(-4, Height - (Height - rold.Height()) - 4, Width + 4, Height);
            RedrawWindow(Handle, &r, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
        }
        else if (rold.Height() > Height)
        {
            rold.top = rold.Bottom - 4;
            TRect r1 = Rect(rold.left, rold.top, rold.left + 4, rold.bottom);
            RedrawWindow(DESKTOPHANDLE, &r1, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
            TRect r2 = Rect(rold.right - 4, rold.top, rold.right, rold.bottom);
            RedrawWindow(DESKTOPHANDLE, &r2, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
            r1 = Rect(Left, Top + Height - 4, Left + 4, Top + Height);
            RedrawWindow(DESKTOPHANDLE, &r1, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
            r2 = Rect(Left + Width - 4, Top + Height - 4, Left + Width, Top + Height);
            RedrawWindow(DESKTOPHANDLE, &r2, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::sbResetClick(TObject *Sender)
{
    nofilter = true;
    try
    {
        edMeaning->Text = L"";
        edStrokes->Text = L"";
        edreading->Text = L"";
        edJouyou->Text = L"";
        edJLPT->Text = L"";
        edS2->Text = L"";
        edS3->Text = L"";
        cbMeaning->Checked = false;
        cbReading->Checked = false;
        cbStrokes->Checked = false;
        cbJouyou->Checked = false;
        cbJLPT->Checked = false;
        sbS1->Down = true;
        cbSKIP->Tag = sbS1->Tag;
        cbSKIP->Checked = false;
        cbOn->Checked = true;
        cbKun->Checked = true;
        cbOku->Checked = true;

        edC1->Text = L"";
        edC2->Text = L"";
        edC3->Text = L"";
        edC4->Text = L"";
        edC5->Text = L"";
        cb4C->Checked = false;

        edCustom->Text = L"";

        radselected = false;
        memset(rads, 0, sizeof(bool)*RAD_COUNT);
        btnRads->Enabled = false;
        clearrads2(rads2);

        if (Visible)
        {
            fkobj->Redraw();
            if (!settings.intray || Focused())
                pbGrid->SetFocus();
            pbRadicals->Invalidate();
        }
    }
    catch (...)
    {
    }
    nofilter = false;
    FilterKanji();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    /*if (Shift.Contains(ssCtrl) && Key == VK_TAB) {
     ChangeFocus(Shift.Contains(ssShift));
     return;
     }
     */
    if (ontop)
    {
        switch (Key)
        {
        case VK_ESCAPE:
            if (!HideInfoWindows(true))
                Popup(false);
            break;
        case VK_F2:
        case VK_F3:
        case VK_F4:
            fPopupDict->Popup(true, Key == VK_F2 ? dmKanji : Key == VK_F3 ? dmRomaji : dmBrowse);
            break;
        default:
            if (Key >= '1' && Key <= '9')
                fDict->SelectDictionary(Key - '1');
        }
    }
    else
    {
        switch (Key)
        {
        case VK_ESCAPE:
            HideInfoWindows(true);
            break;
        }
    }
}
//---------------------------------------------------------------------------
void TfKanji::set_kanji(TKanji *k)
{
    if (fkobj->kanji == k)
        return;

    fkobj->kanji = k;
    if (fkobj->kanji != k)
    {
        sbResetClick(sbReset);
        fkobj->kanji = k;
        if (fkobj->kanji != k)
        {
            cbList->ItemIndex = 0;
            FilterKanji();
            fkobj->kanji = k;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::miInfoClick(TObject *Sender)
{
    if (!fkobj->kanji)
        return;
    ShowKanjiInfo(settings.dictionary_index, fkobj->kanji);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::miTranslateClick(TObject *Sender)
{
    if (fkobj->kanji)
    {
        fDict->btnKanji->Click();
        fDict->edkanji->Insert(fkobj->kanji->ch);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::miCopyClick(TObject *Sender)
{
    ClipReplace(fkobj->selstring);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::miAppendClick(TObject *Sender)
{
    ClipAppend(fkobj->selstring);
}
//---------------------------------------------------------------------------
UnicodeString TfKanji::KanjiString()
{
    return fkobj->kanjistring;
}
//---------------------------------------------------------------------------
UnicodeString TfKanji::SelString()
{
    return fkobj->selstring;
}
//---------------------------------------------------------------------------
void TfKanji::FillKanjiList(TKanjiItemList *kl, bool aselected)
{
    for (int i = 0; i < (aselected ? fkobj->selcount : fkobj->count); i++)
        kl->Add(Kanjis[(aselected ? fkobj->selpos[i] : i)]->index);
}
//---------------------------------------------------------------------------
void TfKanji::popupcallback(TZMenuRows *rows, int col, int row)
{
    TZMenuRow *r = rows->Rows[row];
    TZPopupCommand cmd = (TZPopupCommand)r->Data[col];

    if (cmd == pcNone)
        return;

    TKanjiItemList *kl = NULL;

    try
    {
        switch (cmd)
        {
        case pcKanjiInfo:
            ShowKanjiInfo(settings.dictionary_index, kanji);
            break;
        case pcTranslate:
            if (!fKanji->ontop)
            {
                fDict->btnKanji->Click();
                fDict->edkanji->Insert(fkobj->kanji->ch);
            }
            else
            {
                fPopupDict->Popup(true, dmKanji);
                fPopupDict->btnKanji->Click();
                fPopupDict->edkanji->Insert(fkobj->kanji->ch);
            }
            break;
        case pcCopy1:
            ClipReplace(fkobj->selstring);
            break;
        case pcAppend1:
            ClipAppend(fkobj->selstring);
            break;
        case pcCopy2:
            ClipReplace(fkobj->kanjistring);
            break;
        case pcAppend2:
            ClipAppend(fkobj->kanjistring);
            break;
        case pcToGroup:
            kl = new TKanjiItemList;
            FillKanjiList(kl, true);
            KanjiToGroup(kl);
            break;
        case pcCollect:
            kl = new TKanjiItemList;
            FillKanjiList(kl, true);
            ::GenerateWordGroup(kl);
            break;
        case pcEditKanjiDef:
            kl = new TKanjiItemList;
            if (fkobj->selcount > 1)
                FillKanjiList(kl, true);
            else
            {
                for (int i = fkobj->pos; i < fkobj->count; i++)
                    kl->Add(fkobj->kanjis[i]);
            }
            ChangeDef(settings.dictionary_index, kl, 0);
            break;
        }
    }
    catch (...)
    {
        ;
    }
    delete kl;
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::pbGridMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (!fkobj->count || Button != mbRight || !fkobj->KanjiAtPos(Point(x, y)))
        return;

    TPoint p = pbGrid->ClientToScreen(Point(x, y));
    if (!settings.dictionary_index)
    {
        TZMenuRow *row = gridmenu->Rows[0];
        row->Enabled[1] = fkobj->selcount <= 1;
        row->Enabled[2] = fkobj->selcount <= 1;
        menu->Popup(gridmenu, p.x, p.y, &popupcallback);
    }
    else
    {
        TZMenuRow *row = gridmenu->Rows[0];
        row->Enabled[1] = kanji != NULL;
        row->Enabled[2] = kanji != NULL;

        row = gridmenuextra->Rows[0];
        if (fkobj->selcount == 1)
            row->Titles[1] = L"Add...";
        else
            row->Titles[1] = L"Edit...";

        menu->Popup(gridmenucombined, p.x, p.y, &popupcallback);
    }
}
//---------------------------------------------------------------------------
UnicodeString TfKanji::get_radicals()
{
    UnicodeString s;
    for (int i = 0; i < RAD_COUNT; i++)
        if (rads[i])
            s += IntToStr(i) + L",";
    return s;
}
//---------------------------------------------------------------------------

void TfKanji::set_radicals(UnicodeString s)
{
    memset(rads, 0, sizeof(bool)*RAD_COUNT);
    radselected = false;

    while (s.Length())
    {
        rads[s.SubString(1, s.Pos(L",") - 1).ToInt()] = true;
        radselected = true;
        s = s.SubString(s.Pos(L",") + 1, s.Length());
    }

    btnRads->Enabled = radselected;
}
//---------------------------------------------------------------------------

void __fastcall TfKanji::sbS1Click(TObject *Sender)
{
    cbSKIP->Tag = ((TSpeedButton*)Sender)->Tag;
    cbSKIP->Checked = true;
    edChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfKanji::N91MeasureItem(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height)
{
    Width = 25;
}
//---------------------------------------------------------------------------

void __fastcall TfKanji::N91Click(TObject *Sender)
{
    TEdit2 *e = (edC1->Focused() ? edC1 : (edC2->Focused() ? edC2 : (edC3->Focused() ? edC3 : (edC4->Focused() ? edC4 : edC5))));
    TMenuItem *mi = (TMenuItem*)Sender;
    if (mi->Tag >= 0)
        e->Text = IntToStr(mi->Tag);
    else
        e->Text = L"";
}
//---------------------------------------------------------------------------

void __fastcall TfKanji::edC1Enter(TObject *Sender)
{
    TEdit2 *e = (TEdit2*)Sender;
    e->SelStart = 0;
    e->SelLength = e->Text.Length();
}
//---------------------------------------------------------------------------

void __fastcall TfKanji::edC5Change(TObject *Sender)
{
    edC1Enter(Sender);
    TEdit2 *e = (TEdit2*)Sender;

    if (Visible && e->Text.Length())
    {
        if (Sender == edC1)
            edC2->SetFocus();
        else if (Sender == edC2)
            edC3->SetFocus();
        else if (Sender == edC3)
            edC4->SetFocus();
        else if (Sender == edC4)
            edC5->SetFocus();
        cb4C->Checked = true;
    }

    edChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfKanji::edS2Change(TObject *Sender)
{
    cbSKIP->Checked = true;
    edChange(Sender);
}
//---------------------------------------------------------------------------
void TfKanji::ArrangeFilters(int which)
{
    TPanel *pp[] = { pFilter1, pFilter2, pFilter3, pFilter4, pFilter5, pFilter6 };
    if (Visible)
        DisableAlign();

    for (unsigned int ix = 0; ix < sizeof(pp) / sizeof(TPanel*); ++ix)
        pp[ix]->Width = pp[ix]->Parent->Width;

    try
    {
        int top = 1;
        for (unsigned int ix = 0; ix < sizeof(pp) / sizeof(TPanel*); ++ix)
        {
            if (pp[ix]->Visible)
            {
                pp[ix]->Top = top;
                top += pp[ix]->Height;
                if (which == (int)ix)
                    pp[ix]->Invalidate();
            }
        }

        if (pInfo->Visible)
        {
            pInfo->Top = top;
            pInfo->Height = sbInfo->Top - top - 1;
            pInfo->Width = pWhole->Width;
            top += pInfo->Height;
        }

        pWhole->Height = top;

        if (pWhole->Align == alTop && ClientHeight < top + fboxsize + sbInfo->Height + pSort->Height + 1)
            ClientHeight = top + fboxsize + sbInfo->Height + pSort->Height + 1;

        if (pInfo->Visible)
        {
            TRect r = Rect(0, pInfo->Height - 1, pInfo->Width, pInfo->Height);
            InvalidateRect(pInfo->Handle, &r, false);
        }
    }
    catch (...)
    {
        ;
    }

    if (Visible)
        EnableAlign();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::sbF1Click(TObject *Sender)
{
    TPanel *pp[] = { pFilter1, pFilter2, pFilter3, pFilter4, pFilter5, pFilter6 };
    TSpeedButton *sb[] = { sbF1, sbF2, sbF3, sbF4, sbF5, sbF6 };
    int t = ((TSpeedButton*)Sender)->Tag;
    pp[t]->Visible = sb[t]->Down;
    ArrangeFilters(t);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::sbFAllClick(TObject *Sender)
{
    TPanel *pp[] = { pFilter1, pFilter2, pFilter3, pFilter4, pFilter5, pFilter6 };
    TSpeedButton *sbs[] = { sbF1, sbF2, sbF3, sbF4, sbF5, sbF6 };
    int l = sizeof(sbs) / sizeof(TSpeedButton*);

    //SendMessage(Handle,WM_SETREDRAW,0,0);

    bool onoff = false;
    try
    {
        for (int i = 0; i < l && !onoff; i++)
            if (!sbs[i]->Down)
                onoff = true;

        for (int i = 0; i < l; i++)
            if (sbs[i]->Down != onoff)
            {
                sbs[i]->Down = onoff;
                pp[i]->Visible = onoff;
            }
        ArrangeFilters();
    }
    catch (...)
    {
        ;
    }

    //SendMessage(Handle,WM_SETREDRAW,1,0);

    ::Repaint(this);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::FormShow(TObject *Sender)
{
    static bool firsttime = true;

    if (firsttime && !fontop)
    {
        Width = 300 * scaleres;
        firsttime = false;
        nofilter = false;
        FilterKanji();
        //SetWindowPos(Handle, GetDesktopWindow(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    }

    do_sizing(ClientRect, true);

    // The VCL has trouble positioning elements in some cases, i.e. when the window is not visible or has 0 width or height.
    // Position the aligned controls one by one here to help VCL not screw up.
    int l = cbMeaning->Left + cbMeaning->Width;
    edMeaning->SetBounds(l * scaleres, 3 * scaleres, pWhole->Width - (93 + l) * scaleres, edMeaning->Height);
    l = cbReading->Left + cbReading->Width;
    edReading->SetBounds(l * scaleres, 27 * scaleres, pWhole->Width - (125 + l) * scaleres, 20 * scaleres);
    cbStrokes->Anchors = TAnchors() << akTop << akLeft;
    cbStrokes->SetBounds(pWhole->Width - cbStrokes->Width - 29 * scaleres, 4 * scaleres, cbStrokes->Width, cbStrokes->Height);
    cbStrokes->Anchors = TAnchors() << akTop << akRight;
    edStrokes->Anchors = TAnchors() << akTop << akLeft;
    edStrokes->SetBounds(pWhole->Width - edStrokes->Width - 5 * scaleres, 3 * scaleres, edStrokes->Width, edStrokes->Height);
    edStrokes->Anchors = TAnchors() << akTop << akRight;

    cbOn->Anchors = TAnchors() << akTop << akLeft;
    cbOn->SetBounds(pWhole->Width - cbOn->Width - 87 * scaleres, 28 * scaleres, cbOn->Width, cbOn->Height);
    cbOn->Anchors = TAnchors() << akTop << akRight;
    cbKun->Anchors = TAnchors() << akTop << akLeft;
    cbKun->SetBounds(pWhole->Width - cbKun->Width - 48 * scaleres, 28 * scaleres, cbKun->Width, cbKun->Height);
    cbKun->Anchors = TAnchors() << akTop << akRight;
    cbOku->Anchors = TAnchors() << akTop << akLeft;
    cbOku->SetBounds(pWhole->Width - cbOku->Width - 7 * scaleres, 28 * scaleres, cbOku->Width, cbOku->Height);
    cbOku->Anchors = TAnchors() << akTop << akRight;

    pbRadicals->SetBounds(45 * scaleres, 3 * scaleres, pWhole->Width - (6 + 45) * scaleres, 21 * scaleres);

    edCustom->SetBounds(218 * scaleres, 3 * scaleres, pWhole->Width - (5 + 218) * scaleres, edCustom->Height);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::cbCustomChange(TObject *Sender)
{
    if (edCustom->Text.Trim().Length())
        edChange(edCustom);
}
//---------------------------------------------------------------------------
void TfKanji::UpdateData()
{
    TCanvas *c = pbReadings->Canvas;
    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = 11;

    TSize s;
    GetTextExtentPointW(c->Handle, L"\x3044", 1, &s);
    fcolsize = s.cx + 6 * scaleres;
    frowsize = s.cy + 2 * scaleres;

    if (kanji)
    {
        pbReadings->HMax = (kanji->oncnt + kanji->kuncnt) * fcolsize + (kanji->oncnt ? 18 : 0) + (kanji->kuncnt ? 18 : 0);
        pbReadings->HPos = pbReadings->HMax;

        int maxx = 0;
        for (int i = 0; i < kanji->kuncnt; i++)
            maxx = MAX(maxx, (int)wcslen(kanji->kun[i]) - (wcschr(kanji->kun[i], L'.') ? 1 : 0));
        for (int i = 0; i < kanji->oncnt; i++)
            maxx = MAX(maxx, (int)wcslen(kanji->on[i]) - (wcschr(kanji->on[i], L'.') ? 1 : 0));
        maxx = MAX(maxx, 2);

        int sz = GetSystemMetrics(SM_CYHSCROLL);
        if (pbReadings->Height != 4 + maxx * frowsize + sz)
        {
            pbReadings->Height = 4 + maxx * frowsize + sz;
            bvRBottom->Top = pbReadings->Top + pbReadings->Height + 1;
        }
    }
    else
    {
        pbReadings->HMax = 5;
    }
    pbReadings->Invalidate();
    pbMeanings->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfKanji::pbReadingsPaint(TObject *Sender)
{
    TCanvas *c = pbReadings->Canvas;
    if (!kanji)
    {
        c->FillRect(pbReadings->ClientRect);
        return;
    }

    c->Font->Size = 11;
    TRect r = Rect(0, 0, fcolsize, frowsize);
    r.left += (pbReadings->HMax - fcolsize - pbReadings->HPos);
    r.right += (pbReadings->HMax - fcolsize - pbReadings->HPos);
    if (r.left + fcolsize < pbReadings->Width)
    {
        r.left = pbReadings->Width - fcolsize;
        r.right = pbReadings->Width;
    }

    if (kanji->oncnt)
    {
        r.left += fcolsize - 18 * scaleres;
        r.right += fcolsize - 18 * scaleres;
        c->Font->Style = TFontStyles() << fsBold;
        c->Font->Name = L"Arial";
        c->Font->Charset = DEFAULT_CHARSET;
        c->Font->Size = 10;
        c->Brush->Color = clBlack;
        c->Font->Color = clWhite;
        AngleTextOutEx(c, 270, r.right - 3*scaleres, 2*scaleres, Rect(r.left, 0, r.left + 18*scaleres, pbReadings->Height), L"on");
        r.left -= 18 * scaleres;
        r.right -= 18 * scaleres;
        c->Font->Style = TFontStyles();
        c->Font->Size = 11;
        c->Brush->Color = clWhite;
        c->Font->Color = clBlack;
    }

    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;

    for (int i = 0; i < kanji->oncnt; i++)
    {
        r.top = 0;
        r.bottom = frowsize;
        for (int j = 0; j < (int)wcslen(kanji->on[i]); j++)
        {
            if (kanji->on[i][j] != L'.')
            {
                ExtTextOutW(c->Handle, r.left + (r.Width() - fcolsize) / 2 + 1, r.top, ETO_CLIPPED | ETO_OPAQUE, &r, kanji->on[i] + j, 1, NULL);
                r.top = r.bottom;
                r.bottom = r.top + frowsize;
            }
        }
        c->FillRect(Rect(r.left, r.top, r.right, pbReadings->Height));
        r.left -= fcolsize;
        r.right -= fcolsize;
    }

    if (kanji->kuncnt)
    {
        r.left += fcolsize - 18 * scaleres;
        r.right += fcolsize - 18 * scaleres;
        c->Font->Style = TFontStyles() << fsBold;
        c->Font->Name = L"Arial";
        c->Font->Charset = DEFAULT_CHARSET;
        c->Font->Size = 10;
        c->Brush->Color = clBlack;
        c->Font->Color = clWhite;
        AngleTextOutEx(c, 270, r.right - 3*scaleres, 2*scaleres, Rect(r.left, 0, r.left + 18*scaleres, pbReadings->Height), L"kun");
        r.left -= 18 * scaleres;
        r.right -= 18 * scaleres;
        c->Font->Style = TFontStyles();
        c->Font->Size = 11;
        c->Brush->Color = clWhite;
        c->Font->Color = clBlack;
    }
    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;

    for (int i = 0; i < kanji->kuncnt; i++)
    {
        r.top = 0;
        r.bottom = frowsize;
        for (int j = 0; j < (int)wcslen(kanji->kun[i]); j++)
        {
            if (kanji->kun[i][j] != L'.')
            {
                ExtTextOutW(c->Handle, r.left + (r.Width() - fcolsize) / 2 + 1, r.top, ETO_CLIPPED | ETO_OPAQUE, &r, kanji->kun[i] + j, 1, NULL);
                if (j > 0 && kanji->kun[i][j - 1] == L'.')
                {
                    c->Brush->Color = clBlack;
                    c->FillRect(Rect(r.right - 21*scaleres, r.top - 1, r.right - 4*scaleres, r.top));
                    c->Brush->Color = clWhite;
                }
                r.top = r.bottom;
                r.bottom = r.top + frowsize;
            }
        }
        c->FillRect(Rect(r.left, r.top, r.right, pbReadings->Height));
        r.left -= fcolsize;
        r.right -= fcolsize;
    }

    c->FillRect(Rect(0, 0, r.right, pbReadings->Height));
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::pbMeaningsPaint(TObject *Sender)
{
    TCanvas *c = pbMeanings->Canvas;
    c->Brush->Color = clWhite;
    c->FillRect(pbMeanings->ClientRect);
    if (!kanji)
        return;
    c->Font->Size = 10;
    TRect r = pbMeanings->ClientRect;
    wchar_t *ch = fkobj->collection->kanjidat[kanji->index].meaning ? fkobj->collection->kanjidat[kanji->index].meaning : kanji->meaning;

    r.left += 2 * scaleres;
    r.top += 2 * scaleres;
    r.bottom -= 2 * scaleres;
    r.right -= 2 * scaleres;
    DrawTextW(c->Handle, ch, wcslen(ch), &r, DT_LEFT | DT_TOP | DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT);

    int d = pbMeanings->ClientHeight - 4 * scaleres - r.bottom;
    r.top += d / 2;
    r.bottom += d / 2;
    d = pbMeanings->ClientWidth - 4 * scaleres - r.right;
    r.left += d / 2;
    r.right += d / 2;
    DrawTextW(c->Handle, ch, wcslen(ch), &r, DT_LEFT | DT_BOTTOM | DT_NOPREFIX | DT_WORDBREAK);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::btnHideClick(TObject *Sender)
{
    if (!btnHide->Tag)
    {
        pbGrid->AlignWithMargins = false;
        btnHide->Tag = 1;
        btnHide->Caption = L">>";
        pWhole->Visible = false;
        pFilterTop->Visible = false;
        bvHideLeft->Visible = false;
    }
    else
    {
        pbGrid->AlignWithMargins = true;
        btnHide->Tag = 0;
        btnHide->Caption = L"<<";
        pWhole->Visible = true;
        pFilterTop->Visible = true;
        bvHideLeft->Visible = true;
    }
    if (WindowState != wsMaximized)
    {
        TRect r = Rect(0, 0, Width, Height);
        SendMessage(Handle, WM_SIZING, WMSZ_RIGHT, (int)(&r));
        Width = r.Width();
    }
}
//---------------------------------------------------------------------------
void TfKanji::WMChangeCBChain(TWMChangeCBChain &msg)
{
    if (msg.Remove == clipnext)
        clipnext == msg.Next;
    else if (clipnext)
        SendMessage(clipnext, WM_CHANGECBCHAIN, (int)msg.Remove, (int)msg.Next);
    msg.Result = 0;
}
//---------------------------------------------------------------------------
void TfKanji::WMDrawClipboard(TMessage &msg)
{
    if (clipnext)
        SendMessage(clipnext, WM_DRAWCLIPBOARD, msg.WParam, msg.LParam);
    if (cbList && cbList->ItemIndex == 1)
        FilterKanji();
}
//---------------------------------------------------------------------------

void __fastcall TfKanji::btnRadsClick(TObject *Sender)
{
    if (!radselected)
    {
        pbRadicalsClick(pbRadicals);
        return;
    }

    memset(rads, 0, sizeof(bool)*RAD_COUNT);
    clearrads2(rads2);

    radselected = false;
    btnRads->Enabled = false;
    pbRadicals->Invalidate();
    FilterKanji();
}
//---------------------------------------------------------------------------
void TfKanji::clearrads2(TRadGroup *&l)
{
    //if (l)
    //{
        //for (int i = 0; i < l->Count; i++)
        //    delete l->Items[i];
        delete l;
        l = NULL;
    //}
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::FormActivate(TObject *Sender)
{
    //if (!settings.docked && SizeInfo.maxed && !ontop && WindowState != wsMaximized)
    //SendMessage(Handle,WM_SYSCOMMAND,SC_MAXIMIZE,-1);

    //if (!settings.docked && !settings.intray)
    //FormActivated(this);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::English1Click(TObject *Sender)
{
    fDict->miClick(Sender);
}
//---------------------------------------------------------------------------
TKanji* TfKanji::get_kanji()
{
    if (fkobj->selcount <= 1)
        return fkobj->kanji;
    return NULL;
}
//---------------------------------------------------------------------------
void TfKanji::set_collection(TWordCollection *acollection)
{
    fkobj->collection = acollection;
    pbMeanings->Invalidate();
}
//---------------------------------------------------------------------------
TWordCollection* TfKanji::get_collection()
{
    return fkobj->collection;
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::pbGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN && fkobj->count)
        ShowKanjiInfo(settings.dictionary_index, fkobj->kanji);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::miNullMeasureItem(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height)
{
    Width = 1;
}
//---------------------------------------------------------------------------
TKanji* TfKanji::get_kanjis(int ix)
{
    return fkobj->kanjis[ix];
}
//---------------------------------------------------------------------------
int TfKanji::get_count()
{
    return fkobj->count;
}
//---------------------------------------------------------------------------
void TfKanji::DefChanged()
{
    fkobj->Redraw();
    if (edMeaning->Text.Trim().Length())
        FilterKanji();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::miCopyAllClick(TObject *Sender)
{
    ClipReplace(fkobj->kanjistring);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::miAppendAllClick(TObject *Sender)
{
    ClipAppend(fkobj->kanjistring);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::miKanjiDefClick(TObject *Sender)
{
    TKanjiItemList *kl = new TKanjiItemList;
    try
    {
        if (fkobj->selcount > 1)
            FillKanjiList(kl, true);
        else
            for (int i = fkobj->pos; i < fkobj->count; i++)
                kl->Add(fkobj->kanjis[i]);
        ChangeDef(settings.dictionary_index, kl, 0);
    }
    catch (...)
    {
        ;
    }
    delete kl;
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::FormPaint(TObject *Sender)
{
    TRect rft = ChildRect(pFilterTop);
    rft.Right = pWhole->Width;
    if (!ontop)
        DrawPillowPanel(Canvas, rft);
    else
        DrawCaptionBar(Canvas, rft, Active);

    if (pWhole->Align == alTop)
        DrawPillowPanel(Canvas, ChildRect(pSortInner), false, true);
    else
        DrawPillowPanel(Canvas, ChildRect(pSort));

    TPanel *pp[] = { pFilter1, pFilter2, pFilter3, pFilter4, pFilter5, pFilter6 };

    for (unsigned int ix = 0; ix < sizeof(pp) / sizeof(TPanel*); ++ix)
    {
        if (!pp[ix]->Visible)
            continue;

        TRect r = ChildRect(pp[ix]);

        DrawPillowPanel(Canvas, r, true);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::pbRadicalsMouseEnter(TObject *Sender)
{
    pbRadicals->Tag = 1;
    pbRadicals->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TfKanji::pbRadicalsMouseLeave(TObject *Sender)
{
    pbRadicals->Tag = 0;
    pbRadicals->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TfKanji::pbRadicalsResize(TObject *Sender)
{
    pbRadicals->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::edStrokesChange(TObject *Sender)
{
    if (edStrokes->Focused() && edStrokes->Text != L"")
        cbStrokes->Checked = true;

    FilterKanji();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::edMeaningChange(TObject *Sender)
{
    if (edMeaning->Focused() && edMeaning->Text != L"")
        cbMeaning->Checked = true;
    FilterKanji();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::edJouyouChange(TObject *Sender)
{
    if (edJouyou->Focused() && edJouyou->Text != L"")
        cbJouyou->Checked = true;
    FilterKanji();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::edJLPTChange(TObject *Sender)
{
    if (edJLPT->Focused() && edJLPT->Text != L"")
        cbJLPT->Checked = true;
    FilterKanji();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::cbMeaningClick(TObject *Sender)
{
    if (cbMeaning->Checked && Visible)
        edMeaning->SetFocus();
    if (edMeaning->Text.Length())
        FilterKanji();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::cbReadingClick(TObject *Sender)
{
    if (cbReading->Checked && Visible && (!settings.intray || Focused()))
        edReading->SetFocus();
    if (edreading->Text.Length())
        FilterKanji();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::cbStrokesClick(TObject *Sender)
{
    if (cbStrokes->Checked && Visible)
        edStrokes->SetFocus();
    if (edStrokes->Text.Length())
        FilterKanji();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::cbJouyouClick(TObject *Sender)
{
    if (cbJouyou->Checked && Visible)
        edJouyou->SetFocus();
    if (edJouyou->Text.Length())
        FilterKanji();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::cbJLPTClick(TObject *Sender)
{
    if (cbJLPT->Checked && Visible)
        edJLPT->SetFocus();
    if (edJLPT->Text.Length())
        FilterKanji();
}
//---------------------------------------------------------------------------
void TfKanji::Popup(bool show, bool activate)
{
    if (show == ontop || modallist->Count)
    {
        if (modallist->Count)
            SetForegroundWindow(modallist->Items[modallist->Count - 1]->Handle);
        return;
    }

    if (show)
    {
        if (!settings.minimizetotray && fDock && MatchVersion(6, 1))
            PostMessage(fDock->Handle, WM_Z_ENABLE, 0, 0);

        SaveFormSize(true);
        KeepSize = false;

        Shadowed = true;

        RestoreOldActive = false;
        if (settings.kanjireset)
            sbReset->Click();
        BorderStyle = Forms::bsNone;
        BorderIcons = TBorderIcons() << biSystemMenu;
        pFilterTop->Color = COLORINC(clBtnFace, 0x00202020);
        closebtn->Visible = true;
        int fw = 8 + fboxsize * 5 + GetSystemMetrics(SM_CXVSCROLL);
        int fh = 8 + fboxsize * 3 + /*1 +*/sbInfo->Height + pFilterTop->Height + pFilter1->Height + pFilter2->Height + pFilter3->Height + pFilter4->Height + pFilter5->Height + pFilter6->Height + pSort->Height;
        Recreate(false);
        fontop = true;
        HRGN rgn = CreateRoundRectRgn(0, 0, fw + 1, fh + 1, 9, 9);
        SetWindowRgn(Handle, rgn, false);
        data->miKanji->Caption = L"Hide kanji list";

        SetWindowPos(Handle, HWND_TOPMOST, 5, 5, fw, fh, SWP_NOACTIVATE);

        ShowWindow(Handle, SW_SHOWNA);
        SetBounds(5, 5, fw, fh);

        //the first call of SetWindowRgn after the window was shown causes flicker, so make that "flicker" happen now instead of when first changing the window shape for real
        rgn = CreateRectRgn(0, 0, 0, 0);
        int err = GetWindowRgn(Handle, rgn) == ERROR;
        HRGN rgn2 = CreateRectRgn(0, 0, 0, 0);
        SetWindowRgn(Handle, rgn2, false);
        Visible = true;
        SetWindowRgn(Handle, err ? NULL : rgn, true);
        if (err)
            DeleteObject(rgn);

        sbInfo->Cursor = crSizeNS;
        SendMessage(Handle, WM_NCPAINT, 1, 0);
        if (activate)
            SetForegroundWindow(Handle);

        fInfo->OnTop = true;
        fClipboard->OnTop = true;
    }
    else
    {
        HideKanjiHint();
        if (!fPopupDict->Visible)
        {
            if (fInfo->Visible)
                fInfo->Hide();
            if (fClipboard->Visible)
                fClipboard->Hide();
        }
        Visible = false;
        Shadowed = false;

        SetWindowPos(Handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

        closebtn->Visible = false;
        BorderStyle = bsSizeable;
        BorderIcons = TBorderIcons() << biSystemMenu << biMinimize << biMaximize;
        pFilterTop->Color = clBtnFace;
        fontop = false;
        Recreate(false);
        SetWindowRgn(Handle, 0, true);
        WindowState = wsNormal;
        sbInfo->Cursor = crDefault;
        sbInfo->Tag = -1;
        SetBounds(SizeInfo.left, SizeInfo.top, SizeInfo.width, SizeInfo.height);

        KeepSize = true;

        data->miKanji->Caption = L"Show kanji list";
        RestoreOldActive = true;

        HWND fgnd = GetForegroundWindow();
        if (fInfo && settings.intray && fgnd != fPopupDict->Handle && fgnd != fInfo->Handle && fgnd != fClipboard->Handle)
        {
            fInfo->OnTop = false;
            fClipboard->OnTop = false;
        }

        if (!settings.minimizetotray && fDock && fKanji && !fKanji->OnTop && MatchVersion(6, 1))
            PostMessage(fDock->Handle, WM_Z_ENABLE, 1, 0);
    }
}
//---------------------------------------------------------------------------
void TfKanji::WMDestroy(TMessage &msg)
{
    StopClipviewing();
    if (fontop)
    { //window being closed from vista/win7 task bar
        Popup(false);
    }
    inherited::Dispatch(&msg);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::CreateWnd()
{
    inherited::CreateWnd();
    StartClipviewing();
}
//---------------------------------------------------------------------------
void TfKanji::WMNCCalcSize(TMessage &msg)
{
    if (!ontop)
    {
        msg.Result = DefWindowProc(Handle, msg.Msg, msg.WParam, msg.LParam);
        return;
    }

    if (!msg.WParam)
    {
        TRect *r = ((TRect*)msg.LParam);
        InflateRect(r, -4, -4);
        msg.Result = 0;
    }
    else
    {
        NCCALCSIZE_PARAMS *n = ((NCCALCSIZE_PARAMS*)msg.LParam);
        memmove(n->rgrc + 1, n->rgrc, sizeof(TRect*)*2);
        InflateRect(n->rgrc, -4, -4);
    }
}
//---------------------------------------------------------------------------
void TfKanji::WMNCHitTest(TMessage &msg)
{
    if (!ontop)
    {
        msg.Result = DefWindowProc(Handle, msg.Msg, msg.WParam, msg.LParam);
        return;
    }

    msg.Result = HTCLIENT;

    TPoint p = ScreenToClient(Point(GET_X_LPARAM(msg.LParam), GET_Y_LPARAM(msg.LParam)));

    if (p.y < 0)
        msg.Result = HTCAPTION;
    else if (p.y > ClientHeight)
        msg.Result = HTBOTTOM;
}
//---------------------------------------------------------------------------
void TfKanji::WMNCPaint(TMessage &msg)
{
    if (!ontop)
    {
        msg.Result = DefWindowProc(Handle, msg.Msg, msg.WParam, msg.LParam);
        return;
    }

    HDC hdc;
    hdc = GetWindowDC(Handle);
    if (!hdc)
        return;

    TRect r;
    HPEN p;
    HPEN pold;
    int c = (int)ColorToRGB(clBtnFace);
    bool d = (c & 0xff) >= BTN_DARK_TRESH && ((c >> 8) & 0xff) >= BTN_DARK_TRESH && ((c >> 16) & 0xff) >= BTN_DARK_TRESH;

    GetWindowRect(Handle, &r);
    OffsetRect(r, -r.Left, -r.Top);
    ExcludeClipRect(hdc, 4, 4, r.Width() - 4, r.Height() - 4);

    TColor pcol;

    pcol = d ? COLORDEC(clBtnFace, 0x101010) : COLORINC(clBtnFace, 0x101010);
    if (Active)
        pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.2);
    p = CreatePen(PS_SOLID, 0, pcol);
    pold = (HPEN)SelectObject((HDC)hdc, p);

    TPoint pts[5] = { Point(3, r.Height() - 4), Point(3, 3), Point(r.Width() - 4, 3), Point(r.Width() - 4, r.Height() - 4), Point(3, r.Height() - 4) };
    Polyline(hdc, pts, 5);
    SelectObject(hdc, pold);
    DeleteObject(p);

    pcol = d ? COLORDEC(clBtnFace, 0x202020) : COLORINC(clBtnFace, 0x202020);
    if (Active)
        pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.4);
    p = CreatePen(PS_SOLID, 0, pcol);
    pold = (HPEN)SelectObject((HDC)hdc, p);

    TPoint pts2[5] = { Point(2, r.Height() - 3), Point(2, 2), Point(r.Width() - 3, 2), Point(r.Width() - 3, r.Height() - 3), Point(2, r.Height() - 3) };
    Polyline(hdc, pts2, 5);
    SelectObject(hdc, pold);
    DeleteObject(p);

    pcol = d ? COLORDEC(clBtnShadow, 0x404040) : COLORINC(clBtnShadow, 0x404040);
    p = CreatePen(PS_SOLID, 0, pcol);
    pold = (HPEN)SelectObject((HDC)hdc, p);

    LOGBRUSH lb;
    lb.lbStyle = BS_SOLID;
    pcol = d ? COLORDEC(clBtnFace, 0x404040) : COLORINC(clBtnFace, 0x404040);
    if (Active)
        pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.5);
    lb.lbColor = pcol;

    HBRUSH b = CreateBrushIndirect(&lb);
    HBRUSH bold = (HBRUSH)SelectObject((HDC)hdc, b);

    ExcludeClipRect(hdc, 2, 2, r.Width() - 2, r.Height() - 2);
    RoundRect(hdc, 0, 0, r.Width(), r.Height(), 9, 9);
    SelectObject(hdc, bold);
    SelectObject(hdc, pold);
    DeleteObject(p);
    DeleteObject(b);
    ReleaseDC(Handle, hdc);

    /*HDC hdc;
     hdc = GetWindowDC(Handle);
     if (!hdc)
     return;

     TRect r;
     GetWindowRect(Handle,&r);
     OffsetRect(r,-r.Left,-r.Top);
     int c = (int)ColorToRGB(clBtnFace);

     bool d = ((c & 0xff) < BTN_DARK_TRESH || ((c >> 8) & 0xff) < BTN_DARK_TRESH || ((c >> 16) & 0xff) < BTN_DARK_TRESH) ? false : true;

     ExcludeClipRect(hdc,4,4,r.Width()-4,r.Height()-4);
     HPEN p = CreatePen(PS_SOLID,0,d ? COLORDEC(ColorToRGB(clBtnShadow),0x404040) : COLORINC(ColorToRGB(clBtnShadow),0x404040));
     HPEN pold = (HPEN)SelectObject((HDC)hdc,p);
     TPoint pts[5] = { Point(3,r.Height()-4), Point(3,3),Point(r.Width()-4,3), Point(r.Width()-4,r.Height()-4), Point(3,r.Height()-4) };
     Polyline(hdc,pts,5);
     ExcludeClipRect(hdc,3,3,r.Width()-3,r.Height()-3);  // width -4 height -4, changed for higher dpi but might be bad for normal

     LOGBRUSH lb;
     lb.lbStyle = BS_SOLID;
     lb.lbColor = ColorToRGB(d ? COLORDEC(ColorToRGB(clBtnFace),0x404040) : COLORINC(ColorToRGB(clBtnFace),0x404040));

     HBRUSH b = CreateBrushIndirect(&lb);
     HBRUSH bold = (HBRUSH)SelectObject((HDC)hdc,b);
     RoundRect(hdc,0,0,r.Width(),r.Height(),9,9);
     SelectObject(hdc,bold);
     SelectObject(hdc,pold);
     DeleteObject(p);
     DeleteObject(b);
     ReleaseDC(Handle, hdc);
     */
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::pFilterTopMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (!ontop)
        return;
    ReleaseCapture();
    DefWindowProc(Handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::pbRadicalsClick(TObject *Sender)
{
    fRadicals = new TfRadicals(Application);
    fRadicals->tcSelect->TabIndex = fradtype;
    fRadicals->btnGroup->Down = settings.classic2group;
    fRadicals->btnExact->Down = settings.classic2exact;
    fRadicals->btnNames->Down = settings.classic2hidenames;

    fRadicals->tcSelectClick(fRadicals->tcSelect);

    if (fradtype < 2)
    {
        for (int i = 0; i < RAD_COUNT; i++)
            if (rads[i])
                fRadicals->select(i, true);
    }
    else
        radlist->SetRadicals(rads2);

    if (fRadicals->ShowModal() == mrOk)
    {
        fradtype = fRadicals->tcSelect->TabIndex;
        settings.classic2group = fRadicals->btnGroup->Down;
        settings.classic2exact = fRadicals->btnExact->Down;
        settings.classic2hidenames = fRadicals->btnNames->Down;

        if (fradtype == 2)
            memset(rads, 0, sizeof(bool)*RAD_COUNT);
        else
            memcpy(rads, fRadicals->selected, sizeof(bool)*RAD_COUNT);
        radselected = false;

        if (fradtype < 2)
        {
            for (int i = 0; i < RAD_COUNT && !radselected; i++)
                radselected = rads[i];
        }
        else if ((radselected = radlist->GroupCount) != false)
            radlist->GetRadicals(rads2);

        pbRadicals->Invalidate();
    }
    delete fRadicals;
    fRadicals = NULL;
    btnRads->Enabled = radselected;
    FilterKanji();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::miGenerateClick(TObject *Sender)
{
    GenerateWordGroup();
}
//---------------------------------------------------------------------------
void TfKanji::GenerateWordGroup()
{
    if (fDock && fDock->ActiveControl && fDock->ActiveControl->Owner == fGroup)
    {
        fGroup->GenerateWordGroup();
        return;
    }

    if (!fDock->miKGenerate->Enabled)
        return;

    TKanjiItemList *kl = new TKanjiItemList;
    try
    {
        FillKanjiList(kl, true);
        ::GenerateWordGroup(kl);
    }
    catch (...)
    {
        ;
    }
    delete kl;
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::sbInfoMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (sbInfo->Cursor == crDefault)
        return;

    sbInfo->Tag = Y;
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::sbInfoMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (sbInfo->Tag < 0)
        return;

    sbInfoMouseMove(Sender, Shift, X, Y);
    sbInfo->Tag = -1;
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::sbInfoMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    if (sbInfo->Tag < 0)
        return;

    int y = Y;

    Height = Height + MAX(y - sbInfo->Tag, fboxsize - pbGrid->Height);
}
//---------------------------------------------------------------------------
void TfKanji::appDeactivate(void *caller)
{
    sbInfo->Tag = -1;
}
//---------------------------------------------------------------------------
void TfKanji::repaintProc(void *caller)
{
    if (!Visible)
        return;

    if (ontop)
    {
        SendMessage(Handle, WM_NCPAINT, 0, 0);
        pFilterTop->Invalidate();
        TRect r = Rect(0, 0, ClientWidth, pFilterTop->Height);
        //InvalidateRect(Handle,&r,false);
        closebtn->Redraw();
        RedrawWindow(Handle, &r, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_ALLCHILDREN);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::pbGridMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button == mbLeft && Shift.Contains(ssDouble))
    {
        if (!fkobj->KanjiAtPos(Point(X, Y)))
            return;

        if (fkobj->kanji)
            ShowKanjiInfo(settings.dictionary_index, fkobj->kanji);
    }
}
//---------------------------------------------------------------------------
void TfKanji::WordChanged(byte coll, TChangedWordList *list)
{
    if (coll != settings.dictionary_index)
        return;
    fkobj->Redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::miNewWordClick(TObject *Sender)
{
    fDict->miNewWordClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::miDeleteWordClick(TObject *Sender)
{
    fDict->miDeleteWordClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfKanji::edReadingResize(TObject *Sender)
{
    if (edReading->Left + edReading->Width + 3 > cbOn->Left)
    {
        edReading->Width = edReading->Width;
    }
}
//---------------------------------------------------------------------------
void TfKanji::bgdraw(TCanvas *c, TRect const &r)
{
    TRect rft = Rect(0, -2, r.Width(), pFilterTop->Height - 2);
    DrawCaptionBar(c, rft, Active);
}
//---------------------------------------------------------------------------
void TfKanji::unpop()
{
    Popup(false);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(91);
#endif

