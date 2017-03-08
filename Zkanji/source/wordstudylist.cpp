//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <DateUtils.hpp>
#include "wordstudylist.h"
#include "kanjiedit.h"
#include "wordobj.h"
#include "settings.h"
#include "words.h"
#include "studylist.h"
#include "collection.h"
#include "romajizer.h"
#include "listimport.h"
#include "mouseevent.h"
#include "statobj.h"
#include "repetition.h"
#include "msgform.h"
#include "gengraph.h"
#include "groupadd.h"
#include "module.h"
#include "alphaimagelist.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester1(196);

// Make compiler happy by using variables declared elsewhere:
static bool Dummy_function()
{
    return OneHour * OneMinute * OneSecond * OneMillisecond > 10;
}
#endif

TfStudyList *fStudyList;

#define FREE_NUM       0
#define FREE_ADDED     1
#define FREE_PRIOR     2
#define FREE_STUDIED   3
#define FREE_KANJI     4
#define FREE_KANA      5
#define FREE_MEANING   6

#define LOCK_NUM       0
#define LOCK_ADDED     1
#define LOCK_SCORE     2
#define LOCK_DECK      3
#define LOCK_TRIES     4
#define LOCK_FIRST     5
#define LOCK_LAST      6
#define LOCK_NEXT      7
#define LOCK_INT       8
#define LOCK_STUDIED   9
#define LOCK_KANJI     10
#define LOCK_KANA      11
#define LOCK_MEANING   12

#define TEST_NUM       0
#define TEST_ADDED     1
#define TEST_OLDSCORE  2
#define TEST_SCORE     3
#define TEST_OLDDECK   4
#define TEST_DECK      5
#define TEST_RESULT    6
#define TEST_TIME      7
#define TEST_UNCERTAIN 8
#define TEST_HARD      9
#define TEST_EASY      10
#define TEST_NEXT      11
#define TEST_INT       12
#define TEST_STUDIED   13
#define TEST_KANJI     14
#define TEST_KANA      15
#define TEST_MEANING   16

const wchar_t *freeheaders[] = { L"#", L"Added", L"Priority", L"Studied", L"Written", L"Kana", L"Meaning" };
const wchar_t *lockedheaders[] = { L"#", L"Added", L"Score", L"Deck", L"Tries", L"First", L"Last", L"Next", L"Interval", L"Studied", L"Written", L"Kana", L"Meaning" };
const wchar_t *testedheaders[] = { L"#", L"Added", L"Old score", L"Score", L"Old deck", L"Deck", L"!", L"Time since", L"Postpone", L"Wrong", L"Easy", L"Next", L"Interval", L"Studied", L"Written", L"Kana", L"Meaning" };

bool wsl_scaled_constants = false;
int free_defwidth_small[FREE_HEADING_COUNT] = {
    34/*order*/, 58/*added*/, 46/*priority*/,
    56/*studied*/, 60/*kanji*/, 100/*kana*/,
    6250/*meaning*/
};
int free_defwidth_med[FREE_HEADING_COUNT] = {
    38/*order*/, 60/*added*/, 52/*priority*/,
    62/*studied*/, 80/*kanji*/, 120/*kana*/,
    6250/*meaning*/
};
int free_defwidth_big[FREE_HEADING_COUNT] = {
    41/*order*/, 72/*added*/, 63/*priority*/,
    77/*studied*/, 100/*kanji*/, 140/*kana*/,
    6250/*meaning*/
};

int lock_defwidth_small[LOCK_HEADING_COUNT] = {
    34/*order*/, 58/*added*/, 35/*score*/,
    33/*deck*/, 34/*tries*/, 58/*first*/,
    58/*last*/, 58/*next*/, 74/*interval*/,
    52/*studied*/, 60/*kanji*/, 80/*kana*/,
    6250/*meaning*/
};
int lock_defwidth_med[LOCK_HEADING_COUNT] = {
    38/*order*/, 60/*added*/, 42/*score*/,
    38/*deck*/, 38/*tries*/, 60/*first*/,
    60/*last*/, 60/*next*/, 78/*interval*/,
    62/*studied*/, 80/*kanji*/, 100/*kana*/,
    6250/*meaning*/
};
int lock_defwidth_big[LOCK_HEADING_COUNT] = {
    41/*order*/, 72/*added*/, 48/*score*/,
    41/*deck*/, 41/*tries*/, 72/*first*/,
    72/*last*/, 72/*next*/, 96/*interval*/,
    77/*studied*/, 100/*kanji*/, 120/*kana*/,
    6250/*meaning*/
};

int test_defwidth_small[TEST_HEADING_COUNT] = {
    34/*order*/, 58/*added*/,
    56/*oldscore*/, 35/*score*/, 52/*olddeck*/,
    33/*deck*/, 16/*result*/, 70/*time*/, 57/*uncertain*/,
    42/*hard*/, 32/*easy*/,
    58/*next*/, 74/*interval*/, 52/*studied*/,
    60/*kanji*/, 80/*kana*/, 6250/*meaning*/
};
int test_defwidth_med[TEST_HEADING_COUNT] = {
    38/*order*/, 60/*added*/,
    58/*oldscore*/, 42/*score*/, 54/*olddeck*/,
    38/*deck*/, 18/*result*/, 74/*time*/, 63/*uncertain*/,
    48/*hard*/, 34/*easy*/,
    60/*next*/, 78/*interval*/, 62/*studied*/,
    80/*kanji*/, 100/*kana*/, 6250/*meaning*/
};
int test_defwidth_big[TEST_HEADING_COUNT] = {
    41/*order*/, 72/*added*/,
    74/*oldscore*/, 48/*score*/, 66/*olddeck*/,
    41/*deck*/, 22/*result*/, 90/*time*/, 70/*uncertain*/,
    54/*hard*/, 40/*easy*/,
    72/*next*/, 96/*interval*/, 77/*studied*/,
    100/*kanji*/, 120/*kana*/, 6250/*meaning*/
};

int TChangeArrows::usagecnt = 0;
TAlphaImageList* TChangeArrows::images = NULL;

TChangeArrows::TChangeArrows()
{
    if (!usagecnt)
        images = new TAlphaImageList(data->imgChangeArrows, data->imgChangeArrowsAlpha);
    usagecnt++;
}
TChangeArrows::~TChangeArrows()
{
    usagecnt--;
    if (!usagecnt)
    {
        delete images;
        images = NULL;
    }
}
byte TChangeArrows::get_width()
{
    return images->width;
}
byte TChangeArrows::get_height()
{
    return images->height;
}
void TChangeArrows::Draw(TCanvas *c, int x, int y, TChangeArrowType type)
{
    images->draw(c, x, y, (int)type);
}
void TChangeArrows::Draw(TCanvas *c, int x, int y, TColor bk, TChangeArrowType type)
{
    images->draw(c, x, y, (int)type, bk);
}
//---------------------------------------------------------------------------
__fastcall TfStudyList::TfStudyList(TComponent* Owner) : gindex(-1), fpagetype(ptNone), flisttype(ltNone), savesize(-1), gstatinvalid(false), TBaseForm(Owner)
{
    EraseBackground = false;
    KeepSize = true;

    //RescaleStatusbar(sbItems);

    wobj = new TVirtualWordListObj(pbList, rowsizes(settings.dictfontsize), wcallback, wpopup);
    wobj->headerbutton = true;
    wobj->ordered = true;
    wobj->selecttype = wlsMultiSelect;
    wobj->getheaderproc = &getheaderproc;
    wobj->headerpressproc = &headerpressproc;
    wobj->prepareproc = &prepareproc;
    wobj->drawproc = &drawproc;
    wobj->wordselect = false;
    wobj->showhint = false;
    wobj->showgroup = false;
    wobj->multirightclick = true;
    edFilter = new TKanjiEdit(pbFilter, true, true, NULL, ecallback);
    Constrain(699*scaleres, 419*scaleres);

    Width = settings.studyeditwidth;
    Height = settings.studyeditheight;

    words = new TIntegerList;

    freecolvis.checkall(FREE_HEADING_COUNT - 3, true);
    lockedcolvis.checkall(LOCK_HEADING_COUNT - 3, true);
    testedcolvis.checkall(TEST_HEADING_COUNT - 3, true);

    if (!wsl_scaled_constants)
    {
        wsl_scaled_constants = true;
        for (int ix = 0; ix < FREE_HEADING_COUNT; ++ix)
        {
            free_defwidth_small[ix] *= scaleres;
            free_defwidth_med[ix] *= scaleres;
            free_defwidth_big[ix] *= scaleres;
        }
        for (int ix = 0; ix < LOCK_HEADING_COUNT; ++ix)
        {
            lock_defwidth_small[ix] *= scaleres;
            lock_defwidth_med[ix] *= scaleres;
            lock_defwidth_big[ix] *= scaleres;
        }
        for (int ix = 0; ix < TEST_HEADING_COUNT; ++ix)
        {
            test_defwidth_small[ix] *= scaleres;
            test_defwidth_med[ix] *= scaleres;
            test_defwidth_big[ix] *= scaleres;
        }
    }

    freecolvis = Bits(&settings.ltfreevisible, sizeof(int), 1);
    lockedcolvis = Bits(&settings.ltlockedvisible, sizeof(int), 1);
    testedcolvis = Bits(&settings.lttestedvisible, sizeof(int), 1);

    wsobj = NULL;
    scols = new TStatColumns;

    arrows = new TChangeArrows();

    sh1->Brush->Color = coloravg(COLORDEC(clBtnFace, 0x202020), clSkyBlue, 0.75);

    SettingsChangeEvent(this, settingsChanged, true);
}
//---------------------------------------------------------------------------
__fastcall TfStudyList::~TfStudyList()
{
    SettingsChangeEvent(this, settingsChanged, false);

    delete arrows;

    delete wobj;
    delete edFilter;
    delete scols;
    delete wsobj;

    delete words;
}
//---------------------------------------------------------------------------
void TfStudyList::settingsChanged(void *caller, TSettingsProcState state)
{
    static byte dictfontsize = 0;
    if (state == spsBegin)
    {
        dictfontsize = settings.dictfontsize;
    }

    if (dictfontsize == settings.dictfontsize)
        return;

    wobj->rowsize = rowsizes(settings.dictfontsize);
    int *sizes[] = {
        free_defwidth_big, free_defwidth_med, free_defwidth_small,
        lock_defwidth_big, lock_defwidth_med, lock_defwidth_small,
        test_defwidth_big, test_defwidth_med, test_defwidth_small
    };
    int *w = sizes[int(flisttype) * 3 + settings.dictfontsize];

    wobj->set_cols(NULL, -1, w, w, NULL, NULL, NULL, -1, indexproc);
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::FormClose(TObject *Sender, TCloseAction &Action)
{
    freecolvis.arr_copy(&settings.ltfreevisible, sizeof(int));
    lockedcolvis.arr_copy(&settings.ltlockedvisible, sizeof(int));
    testedcolvis.arr_copy(&settings.lttestedvisible, sizeof(int));
    savesizes();

    settings.studyeditwidth = SizeInfo.width;
    settings.studyeditheight = SizeInfo.height;
}
//---------------------------------------------------------------------------
int TfStudyList::ShowModalEx(byte coll, bool aftertest)
{
#ifdef HACK_TEST_REMOVE
    pHack->Visible = true;
#endif

    fcollection = dictionary->Items[coll];
    study = fcollection->wordstudylist;

    fillstats();

    bool showtested = aftertest && settings.ltshowlast;

    if (settings.ltopentab)
    {
        pWordStats->Height = settings.ltitemstatsize;

        sbWordDeck->Down = settings.ltitemstat == 0;
        sbWordScore->Down = settings.ltitemstat == 1;
        sbWordStats->Down = settings.ltshowitemstat;

        sbG1->Down = settings.ltglobalstat == 0;
        sbG2->Down = settings.ltglobalstat == 1;
        sbG3->Down = settings.ltglobalstat == 2;
        sbG4->Down = settings.ltglobalstat == 3;
        sbG5->Down = settings.ltglobalstat == 4;
        sbG6->Down = settings.ltglobalstat == 5;

        if (!showtested)
        {
            showpage((TPageType)settings.lttab);
            showlist((TListType)settings.ltlist);
        }
    }
    else if (!showtested)
    {
        showpage(ptList);
        showlist(ltFree);
    }

    if (showtested)
    {
        showpage(ptList);
        showlist(ltTested);
    }

    return ShowModal();
}
//---------------------------------------------------------------------------
void TfStudyList::showpage(TPageType apagetype)
{
    if (apagetype != tTabs->TabIndex)
        tTabs->TabIndex = apagetype;

    if (fpagetype == apagetype)
        return;

    fpagetype = apagetype;
    switch (apagetype)
    {
    case ptList:
        pList->Visible = true;
        pStats->Visible = false;
        delete wsobj;
        wsobj = new TStatObj(pbWordStats);
        splitsize(savesize >= 0 ? savesize : pWordStats->Height);
        wsobj->showpopup = true;
        count_wordstat();
        break;
    case ptStats:
        pStats->Visible = true;
        pList->Visible = false;
        delete wsobj;
        wsobj = new TStatObj(pbGlobalStats);
        wsobj->showpopup = true;
        gstatinvalid = true;
        count_globstat();
        break;
    }
    setcaption();
}
//---------------------------------------------------------------------------
void TfStudyList::setcaption()
{
    UnicodeString cap = L"Long-term study list";
    if (fpagetype == ptStats)
        cap += L" (Global statistics)";
    else if (fpagetype == ptList)
    {
        if (flisttype == ltFree)
            cap += L" (Queued items)";
        else if (flisttype == ltLocked)
            cap += L" (Studied items)";
        else if (flisttype == ltTested)
            cap += L" (Last tested items)";
    }

    int n = get_day(Now());
    int day = study->lasttest();
    if (day)
        cap += (UnicodeString)L", Last test: " + generate_datestring(day, n, settings.datefmt);

    int due = study->itemsdue();
    if (due)
        cap += (UnicodeString)L", Items due: " + due;

    Caption = cap;
}
//---------------------------------------------------------------------------
void TfStudyList::wcallback()
{
    sbItems->Panels->Items[1]->Text = wobj->pos + 1;
    sbAddItem->Enabled = wobj->count;
    sbRemoveItem->Enabled = wobj->count;
    edMeaning->Enabled = wobj->count && wobj->sellength == 1;
    sbToQueue->Enabled = wobj->count && (flisttype == ltLocked || flisttype == ltTested);

    miAddItem->Enabled = sbAddItem->Enabled;
    miToGroup->Enabled = wobj->count;
    miRemoveItem->Enabled = sbRemoveItem->Enabled;
    miToQueue->Enabled = sbToQueue->Enabled;
    miStudyOptions->Enabled = sbToQueue->Enabled;

    miDeckInc->Enabled = (flisttype == ltLocked || flisttype == ltTested) && wobj->sellength == 1 && study->lockeddeck[realrow(wobj->pos)] < 12;
    miDeckDec->Enabled = (flisttype == ltLocked || flisttype == ltTested) && wobj->sellength == 1 && study->lockeddeck[realrow(wobj->pos)] > 1;
    miResetData->Enabled = (flisttype == ltLocked || flisttype == ltTested) && wobj->count;

    if (flisttype == ltLocked || flisttype == ltTested)
    {
        if (!miDeckInc->Enabled)
            miDeckInc->Caption = L"Deck+";
        else
        {
            incinterval = study->get_deckinterval(realrow(wobj->pos), study->lockeddeck[realrow(wobj->pos)] + 1);
            miDeckInc->Caption = (UnicodeString)L"Deck+ (interval: " + generate_intervalstring(incinterval) + L")";
        }
        if (!miDeckDec->Enabled)
            miDeckDec->Caption = L"Deck-";
        else
        {
            decinterval = study->get_deckinterval(realrow(wobj->pos), study->lockeddeck[realrow(wobj->pos)] - 1);
            miDeckDec->Caption = (UnicodeString)L"Deck- (interval: " + generate_intervalstring(decinterval) + L")";
        }
    }

    if (!edMeaning->Enabled)
    {
        edMeaning->Text = "";
        btnSave->Enabled = false;
        btnReset->Enabled = false;
    }
    else
    {
        edMeaning->Text = flisttype == ltFree ? study->freemeaning[realrow(wobj->pos)] : study->lockedmeaning[realrow(wobj->pos)];
        edMeaningChange(edMeaning);
    }

    if (wobj->pos >= 0)
        sbItems->Panels->Items[2]->Text = L"Tested: " + (flisttype == ltFree ? study->freedeftext[realrow(wobj->pos)] : study->lockeddeftext[realrow(wobj->pos)]);
    else
        sbItems->Panels->Items[2]->Text = L"Tested: -";

    if (fpagetype == ptList)
        count_wordstat();
    else if (fpagetype == ptStats)
        count_globstat();

    sbItems->Panels->Items[0]->Text = (UnicodeString)wobj->count + L" :";
}
//---------------------------------------------------------------------------
void TfStudyList::wpopup(int x, int y, TWordListObjBase *wobj, TWordColumnType wcc, TWord *w, TKanji *k)
{
    miPriority->Visible = flisttype == ltFree;
    miToQueue->Visible = flisttype == ltLocked || flisttype == ltTested;
    miStudyOptions->Visible = flisttype == ltLocked || flisttype == ltTested;

    if (wobj->pos < 0)
        return;
    if (flisttype == ltFree)
    {
        int pnum = study->freepriority[realrow(wobj->selstart)];
        for (int ix = wobj->selstart + 1; ix < wobj->selstart + wobj->sellength; ++ix)
        {
            if (study->freepriority[realrow(ix)] != pnum)
            {
                pnum = -1;
                break;
            }
        }
        if (pnum != -1)
            miPriority->Items[pnum - 1]->Checked = true;
        else
        { //let the program find the checked item, so I don't have to use a loop
            miPriority->Items[0]->Checked = true;
            miPriority->Items[0]->Checked = false;
        }
    }

    if (wobj->sellength == 1)
    {
        miHint->Enabled = true;
        TQuestionType qt = flisttype == ltFree ? study->freetype[realrow(wobj->selstart)] : study->lockedtype[realrow(wobj->selstart)];
        THintType ht = flisttype == ltFree ? study->freehint[realrow(wobj->selstart)] : study->lockedhint[realrow(wobj->selstart)];
        bool haskanji = qt == qtKanji;
        for (int ix = wcslen(w->kanji) - 1; ix >= 0; --ix && !haskanji)
            haskanji = KANJI(w->kanji[ix]);

        miHintWritten->Visible = (qt != qtKanji) && haskanji;
        miHintKana->Visible = (qt != qtKana);
        miHintMeaning->Visible = (qt != qtMeaning);

        miHintDefault->Checked = ht == htDefault;
        miHintWritten->Checked = ht == htKanji;
        miHintKana->Checked = ht == htKana;
        miHintMeaning->Checked = ht == htMeaning;

        if (!haskanji)
        {
            miHintDefault->Enabled = false;
            miHintWritten->Enabled = false;
            miHintKana->Enabled = false;
            miHintMeaning->Enabled = false;
        }
        else
        {
            miHintDefault->Enabled = true;
            miHintWritten->Enabled = true;
            miHintKana->Enabled = true;
            miHintMeaning->Enabled = true;
        }
    }
    else
        miHint->Enabled = false;

    pmItems->Popup(x, y);
}
//---------------------------------------------------------------------------
char lt_sort_order;
TWordStudyList *lt_sort_studylist;
TWordCollection *lt_sort_collection;

//free list sorting functions
int /*__fastcall*/ lt_sort_freeorderproc(int a, int b)
{
    return (a - b) * lt_sort_order;
}
int /*__fastcall*/ lt_sort_freeaddedproc(int a, int b)
{
    double dif = (double)lt_sort_studylist->freeadded[b] - (double)lt_sort_studylist->freeadded[a];
    if (dif != 0)
    {
        if (dif < 0)
            return lt_sort_order * -1;
        return lt_sort_order;
    }
    return lt_sort_freeorderproc(a, b);
}
int /*__fastcall*/ lt_sort_freepriorityproc(int a, int b)
{
    int res = (lt_sort_studylist->freepriority[b] - lt_sort_studylist->freepriority[a]) * lt_sort_order;
    if (res)
        return res;
    return lt_sort_freeorderproc(a, b);
}
int /*__fastcall*/ lt_sort_freestudyproc(int a, int b)
{
    int res = lt_sort_order * (lt_sort_studylist->freetype[a] - lt_sort_studylist->freetype[b]);
    if (res)
        return res;
    return lt_sort_freeorderproc(a, b);
}
int /*__fastcall*/ lt_sort_freekanaproc(int a, int b)
{
    int res = lt_sort_order * wcscmp(lt_sort_collection->words->Items[lt_sort_studylist->freeindex[a]]->kana, lt_sort_collection->words->Items[lt_sort_studylist->freeindex[b]]->kana);
    if (res)
        return res;
    return lt_sort_freeorderproc(a, b);
}
int /*__fastcall*/ lt_sort_freekanjiproc(int a, int b)
{
    int res = lt_sort_order * wcscmp(lt_sort_collection->words->Items[lt_sort_studylist->freeindex[a]]->kanji, lt_sort_collection->words->Items[lt_sort_studylist->freeindex[b]]->kanji);
    if (res)
        return res;
    return lt_sort_freekanaproc(a, b);
}
//locked list sorting functions
int /*__fastcall*/ lt_sort_lockedorderproc(int a, int b)
{
    return (a - b) * lt_sort_order;
}
int /*__fastcall*/ lt_sort_lockedaddedproc(int a, int b)
{
    double dif = (double)lt_sort_studylist->lockedadded[b] - (double)lt_sort_studylist->lockedadded[a];
    if (dif != 0)
    {
        if (dif < 0)
            return lt_sort_order * -1;
        return lt_sort_order;
    }
    return lt_sort_lockedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_lockedscoreproc(int a, int b)
{
    int res = (lt_sort_studylist->lockedscore[b] - lt_sort_studylist->lockedscore[a]) * lt_sort_order;
    if (res)
        return res;
    return lt_sort_lockedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_lockeddeckproc(int a, int b)
{
    int res = (lt_sort_studylist->lockeddeck[a] - lt_sort_studylist->lockeddeck[b]) * lt_sort_order;
    if (res)
        return res;
    return lt_sort_lockedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_lockedtriesproc(int a, int b)
{
    int res = (lt_sort_studylist->lockedtries[a] - lt_sort_studylist->lockedtries[b]) * lt_sort_order;
    if (res)
        return res;
    return lt_sort_lockedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_lockedfirstproc(int a, int b)
{
    double dif = (double)lt_sort_studylist->lockedfirst[a] - (double)lt_sort_studylist->lockedfirst[b];
    if (dif != 0)
    {
        if (dif < 0)
            return lt_sort_order * -1;
        return lt_sort_order;
    }
    return lt_sort_lockedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_lockedlastproc(int a, int b)
{
    double dif = (double)lt_sort_studylist->lockedlasttest[b] - (double)lt_sort_studylist->lockedlasttest[a];
    if (dif != 0)
    {
        if (dif < 0)
            return lt_sort_order * -1;
        return lt_sort_order;
    }
    return lt_sort_lockedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_lockednextproc(int a, int b)
{
    double dif = (double)lt_sort_studylist->lockednext[a] - (double)lt_sort_studylist->lockednext[b];
    if (dif != 0)
    {
        if (dif < 0)
            return lt_sort_order * -1;
        return lt_sort_order;
    }
    return lt_sort_lockedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_lockedintervalproc(int a, int b)
{
    double dif = (double)lt_sort_studylist->lockedinterval[a] - (double)lt_sort_studylist->lockedinterval[b];
    if (dif != 0)
    {
        if (dif < 0)
            return lt_sort_order * -1;
        return lt_sort_order;
    }
    return lt_sort_lockedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_lockedstudyproc(int a, int b)
{
    int res = lt_sort_order * (lt_sort_studylist->lockedtype[a] - lt_sort_studylist->lockedtype[b]);
    if (res)
        return res;
    return lt_sort_lockedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_lockedkanaproc(int a, int b)
{
    int res = lt_sort_order * wcscmp(lt_sort_collection->words->Items[lt_sort_studylist->lockedindex[a]]->kana, lt_sort_collection->words->Items[lt_sort_studylist->lockedindex[b]]->kana);
    if (res)
        return res;
    return lt_sort_lockedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_lockedkanjiproc(int a, int b)
{
    int res = lt_sort_order * wcscmp(lt_sort_collection->words->Items[lt_sort_studylist->lockedindex[a]]->kanji, lt_sort_collection->words->Items[lt_sort_studylist->lockedindex[b]]->kanji);
    if (res)
        return res;
    return lt_sort_lockedkanaproc(a, b);
}
//tested list sorting functions
int /*__fastcall*/ lt_sort_testedorderproc(int a, int b)
{
    return (a - b) * lt_sort_order;
}
int /*__fastcall*/ lt_sort_testedaddedproc(int a, int b)
{
    double dif = (double)lt_sort_studylist->lockedadded[b] - (double)lt_sort_studylist->lockedadded[a];
    if (dif != 0)
    {
        if (dif < 0)
            return lt_sort_order * -1;
        return lt_sort_order;
    }
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testedscoreproc(int a, int b)
{
    int res = (lt_sort_studylist->lockedscore[b] - lt_sort_studylist->lockedscore[a]) * lt_sort_order;
    if (res)
        return res;
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testeddeckproc(int a, int b)
{
    int res = (lt_sort_studylist->lockeddeck[a] - lt_sort_studylist->lockeddeck[b]) * lt_sort_order;
    if (res)
        return res;
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testedresultproc(int a, int b)
{
    const TRepAnswerCount &ac = lt_sort_studylist->lockedlastanswers[a];
    const TRepAnswerCount &bc = lt_sort_studylist->lockedlastanswers[b];

    int resa = ((ac.easy - ac.hard) * 2 + (!ac.uncertain && ac.good ? 1 : 0)) * lt_sort_order;
    int resb = ((bc.easy - bc.hard) * 2 + (!bc.uncertain && bc.good ? 1 : 0)) * lt_sort_order;
    if (resa != resb)
    {
        if (!resa || !resb)
            return resa ? -1 : 1;
        if ((resa > 0) != (resb > 0))
            return resa > 0 ? -1 : 1;
        return resb - resa;
    }
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testedoldscoreproc(int a, int b)
{
    byte ascore = lt_sort_studylist->lockedoldscore[a];
    byte bscore = lt_sort_studylist->lockedoldscore[b];

    int res = (bscore - ascore) * lt_sort_order;
    if (res)
        return res;
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testedolddeckproc(int a, int b)
{
    byte adeck = lt_sort_studylist->lockedstatcount[a] <= 1 ? 0 : lt_sort_studylist->lockedstat[a][lt_sort_studylist->lockedstatcount[a] - 2].deck;
    byte bdeck = lt_sort_studylist->lockedstatcount[b] <= 1 ? 0 : lt_sort_studylist->lockedstat[b][lt_sort_studylist->lockedstatcount[b] - 2].deck;

    int res = (bdeck - adeck) * lt_sort_order;
    if (res)
        return res;
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testedtimeproc(int a, int b)
{
    double dif = (double)lt_sort_studylist->lockedlasttime[b] - (double)lt_sort_studylist->lockedlasttime[a];
    if (dif != 0)
    {
        if (dif < 0)
            return lt_sort_order * -1;
        return lt_sort_order;
    }
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testeduncertainproc(int a, int b)
{
    int res = (lt_sort_studylist->lockedlastanswers[b].uncertain - lt_sort_studylist->lockedlastanswers[a].uncertain) * lt_sort_order;
    if (res)
        return res;
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testedhardproc(int a, int b)
{
    int res = (lt_sort_studylist->lockedlastanswers[b].hard - lt_sort_studylist->lockedlastanswers[a].hard) * lt_sort_order;
    if (res)
        return res;
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testedeasyproc(int a, int b)
{
    int res = (lt_sort_studylist->lockedlastanswers[b].easy - lt_sort_studylist->lockedlastanswers[a].easy) * lt_sort_order;
    if (res)
        return res;
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testednextproc(int a, int b)
{
    double dif = (double)lt_sort_studylist->lockednext[a] - (double)lt_sort_studylist->lockednext[b];
    if (dif != 0)
    {
        if (dif < 0)
            return lt_sort_order * -1;
        return lt_sort_order;
    }
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testedintervalproc(int a, int b)
{
    double dif = (double)lt_sort_studylist->lockedinterval[a] - (double)lt_sort_studylist->lockedinterval[b];
    if (dif != 0)
    {
        if (dif < 0)
            return lt_sort_order * -1;
        return lt_sort_order;
    }
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testedstudyproc(int a, int b)
{
    int res = lt_sort_order * (lt_sort_studylist->lockedtype[a] - lt_sort_studylist->lockedtype[b]);
    if (res)
        return res;
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testedkanaproc(int a, int b)
{
    int res = lt_sort_order * wcscmp(lt_sort_collection->words->Items[lt_sort_studylist->lockedindex[a]]->kana, lt_sort_collection->words->Items[lt_sort_studylist->lockedindex[b]]->kana);
    if (res)
        return res;
    return lt_sort_testedorderproc(a, b);
}
int /*__fastcall*/ lt_sort_testedkanjiproc(int a, int b)
{
    int res = lt_sort_order * wcscmp(lt_sort_collection->words->Items[lt_sort_studylist->lockedindex[a]]->kanji, lt_sort_collection->words->Items[lt_sort_studylist->lockedindex[b]]->kanji);
    if (res)
        return res;
    return lt_sort_testedkanaproc(a, b);
}
void TfStudyList::reorder()
{
    int order = wobj->colselected;
    lt_sort_order = wobj->colreverseorder ? -1 : 1;
    lt_sort_studylist = study;
    lt_sort_collection = fcollection;

    if (flisttype == ltFree)
    {
        switch (order)
        {
        case FREE_NUM:
            words->Sort(lt_sort_freeorderproc);
            break;
        case FREE_ADDED:
            words->Sort(lt_sort_freeaddedproc);
            break;
        case FREE_PRIOR:
            words->Sort(lt_sort_freepriorityproc);
            break;
        case FREE_STUDIED:
            words->Sort(lt_sort_freestudyproc);
            break;
        case FREE_KANJI:
            words->Sort(lt_sort_freekanjiproc);
            break;
        case FREE_KANA:
            words->Sort(lt_sort_freekanaproc);
            break;
        }
    }
    else if (flisttype == ltLocked)
    {
        switch (order)
        {
        case LOCK_NUM:
            words->Sort(lt_sort_lockedorderproc);
            break;
        case LOCK_ADDED:
            words->Sort(lt_sort_lockedaddedproc);
            break;
        case LOCK_SCORE:
            words->Sort(lt_sort_lockedscoreproc);
            break;
        case LOCK_DECK:
            words->Sort(lt_sort_lockeddeckproc);
            break;
        case LOCK_TRIES:
            words->Sort(lt_sort_lockedtriesproc);
            break;
        case LOCK_FIRST:
            words->Sort(lt_sort_lockedfirstproc);
            break;
        case LOCK_LAST:
            words->Sort(lt_sort_lockedlastproc);
            break;
        case LOCK_NEXT:
            words->Sort(lt_sort_lockednextproc);
            break;
        case LOCK_INT:
            words->Sort(lt_sort_lockedintervalproc);
            break;
        case LOCK_STUDIED:
            words->Sort(lt_sort_lockedstudyproc);
            break;
        case LOCK_KANJI:
            words->Sort(lt_sort_lockedkanjiproc);
            break;
        case LOCK_KANA:
            words->Sort(lt_sort_lockedkanaproc);
            break;
        }
    }
    else if (flisttype == ltTested)
    {
        switch (order)
        {
        case TEST_NUM:
            words->Sort(lt_sort_testedorderproc);
            break;
        case TEST_ADDED:
            words->Sort(lt_sort_testedaddedproc);
            break;
        case TEST_OLDSCORE:
            words->Sort(lt_sort_testedoldscoreproc);
            break;
        case TEST_SCORE:
            words->Sort(lt_sort_testedscoreproc);
            break;
        case TEST_OLDDECK:
            words->Sort(lt_sort_testedolddeckproc);
            break;
        case TEST_DECK:
            words->Sort(lt_sort_testeddeckproc);
            break;
        case TEST_RESULT:
            words->Sort(lt_sort_testedresultproc);
            break;
        case TEST_TIME:
            words->Sort(lt_sort_testedtimeproc);
            break;
        case TEST_UNCERTAIN:
            words->Sort(lt_sort_testeduncertainproc);
            break;
        case TEST_HARD:
            words->Sort(lt_sort_testedhardproc);
            break;
        case TEST_EASY:
            words->Sort(lt_sort_testedeasyproc);
            break;
        case TEST_NEXT:
            words->Sort(lt_sort_testednextproc);
            break;
        case TEST_INT:
            words->Sort(lt_sort_testedintervalproc);
            break;
        case TEST_STUDIED:
            words->Sort(lt_sort_testedstudyproc);
            break;
        case TEST_KANJI:
            words->Sort(lt_sort_testedkanjiproc);
            break;
        case TEST_KANA:
            words->Sort(lt_sort_testedkanaproc);
            break;
        }
    }
}
//---------------------------------------------------------------------------
void TfStudyList::filter()
{
    words->Clear();
    int cnt;

    UnicodeString str = RemoveUseless(edFilter->Text);

    if (str.Length())
    {
        if (flisttype == ltFree)
            study->filter_free(edFilter->Text, words);
        else if (flisttype == ltLocked || flisttype == ltTested)
            study->filter_locked(edFilter->Text, words);
    }
    else
    {
        cnt = flisttype == ltFree ? study->freecount : (flisttype == ltLocked || flisttype == ltTested) ? study->lockedcount : -1;
        for (int ix = 0; ix < cnt; ++ix)
            words->Add(ix);
    }

    if (flisttype == ltTested)
    {
        // Filter out words that were not in the last test.
        int day = study->lasttest();
        for (int ix = words->Count - 1; ix >= 0; --ix)
        {
            if (study->lockedlastday[words->Items[ix]] != day)
                words->Delete(ix);
        }
    }

    for (int ix = words->Count - 1; ix >= 0; --ix)
    {
        TQuestionType itt = flisttype == ltFree ? study->freetype[words->Items[ix]] : study->lockedtype[words->Items[ix]];
        if (itt == qtMeaning && !sbShowMeaning->Down)
            words->Delete(ix);
        if (itt == qtKanji && !sbShowKanji->Down)
            words->Delete(ix);
        if (itt == qtKana && !sbShowKana->Down)
            words->Delete(ix);
    }

    cnt = words->Count;

    reorder();

    wobj->set_virtual(fcollection, cnt, indexproc);
}
//---------------------------------------------------------------------------
void TfStudyList::ecallback()
{
    filter();
    wobj->pos = 0;

    sbItems->Panels->Items[0]->Text = (UnicodeString)wobj->count + " :";
    sbItems->Panels->Items[1]->Text = wobj->pos + 1;
}
//---------------------------------------------------------------------------
int TfStudyList::indexproc(unsigned int ix)
{
    if (flisttype == ltFree)
    {
        if (!words->Count)
            return study->freeindex[ix];
        return study->freeindex[words->Items[ix]];
    }
    else if (flisttype == ltLocked || flisttype == ltTested)
    {
        if (!words->Count)
            return study->lockedindex[ix];
        return study->lockedindex[words->Items[ix]];
    }
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::tTabsChange(TObject *Sender)
{
    settings.lttab = (TPageType)tTabs->TabIndex; //moved here from showlist 2011.feb 18, not to save the last tab when we first open the window and change it programatically
    showpage((TPageType)tTabs->TabIndex);
}
//---------------------------------------------------------------------------
void TfStudyList::savesizes()
{
    if (flisttype == ltFree)
    {
        for (int ix = 0; ix < FREE_HEADING_COUNT; ++ix)
            settings.ltfreesizes[ix] = settings.ltrestoresizes ? -1 : wobj->colsize[ix];
        settings.ltfreeordercolumn = (wobj->colselected + 1) * (wobj->colreverseorder ? -1 : 1);
    }
    else if (flisttype == ltLocked)
    {
        for (int ix = 0; ix < LOCK_HEADING_COUNT; ++ix)
            settings.ltlockedsizes[ix] = settings.ltrestoresizes ? -1 : wobj->colsize[ix];
        settings.ltlockedordercolumn = (wobj->colselected + 1) * (wobj->colreverseorder ? -1 : 1);
    }
    else if (flisttype == ltTested)
    {
        for (int ix = 0; ix < TEST_HEADING_COUNT; ++ix)
            settings.lttestedsizes[ix] = settings.ltrestoresizes ? -1 : wobj->colsize[ix];
        settings.lttestedordercolumn = (wobj->colselected + 1) * (wobj->colreverseorder ? -1 : 1);
    }
}
//---------------------------------------------------------------------------
void TfStudyList::showlist(TListType alisttype)
{
    if (flisttype == alisttype)
        return;

    savesizes();

    switch (alisttype)
    {
    case ltFree:
        sbList1->Down = true;
        sbWordStats->Enabled = false;
        pWordStats->Visible = false;
        break;
    case ltLocked:
        sbList2->Down = true;
        sbWordStats->Enabled = true;
        break;
    case ltTested:
        sbList3->Down = true;
        sbWordStats->Enabled = true;
        break;
    }
    snapshotNow = Now();

    try
    {
        int cw0[FREE_HEADING_COUNT];
        switch (settings.dictfontsize)
        {
        case 0: //big
                    memcpy(cw0, free_defwidth_big, sizeof(int)*FREE_HEADING_COUNT);
            break;
        case 1: //med
                    memcpy(cw0, free_defwidth_med, sizeof(int)*FREE_HEADING_COUNT);
            break;
        default: //small
                    memcpy(cw0, free_defwidth_small, sizeof(int)*FREE_HEADING_COUNT);
            break;
        };
        int cwd0[FREE_HEADING_COUNT];
        memcpy(cwd0, cw0, sizeof(int)*FREE_HEADING_COUNT);

        bool cv0[FREE_HEADING_COUNT] = { true, true, true, true, true, true, true }; //visible
        bool cr0[FREE_HEADING_COUNT] = { true, true, true, true, true, true, false }; //resizable
        bool ch0[FREE_HEADING_COUNT] = { true, true, true, true, true, true, false }; //hoverable
        TWordColumnType ct0[FREE_HEADING_COUNT] = { wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom };
        TAlignment ca0[FREE_HEADING_COUNT] = { taCenter, taRightJustify, taCenter, taCenter, taLeftJustify, taLeftJustify, taLeftJustify };

        int cw1[LOCK_HEADING_COUNT];
        switch (settings.dictfontsize)
        {
        case 0: //big
            memcpy(cw1, lock_defwidth_big, sizeof(int)*LOCK_HEADING_COUNT);
            break;
        case 1: //med
            memcpy(cw1, lock_defwidth_med, sizeof(int)*LOCK_HEADING_COUNT);
            break;
        default: //small
            memcpy(cw1, lock_defwidth_small, sizeof(int)*LOCK_HEADING_COUNT);
            break;
        };

        int cwd1[LOCK_HEADING_COUNT];
        memcpy(cwd1, cw1, sizeof(int)*LOCK_HEADING_COUNT);

        bool cv1[LOCK_HEADING_COUNT] = { true, true, true, true, true, true, true, true, true, true, true, true, true }; //visible
        bool cr1[LOCK_HEADING_COUNT] = { true, true, true, true, true, true, true, true, true, true, true, true, false }; //resizable
        bool ch1[LOCK_HEADING_COUNT] = { true, true, true, true, true, true, true, true, true, true, true, true, false }; //hoverable
        TWordColumnType ct1[LOCK_HEADING_COUNT] = { wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom };
        TAlignment ca1[LOCK_HEADING_COUNT] = { taCenter, taRightJustify, taCenter, taCenter, taCenter, taRightJustify, taRightJustify, taRightJustify, taRightJustify, taCenter, taLeftJustify, taLeftJustify, taLeftJustify };

        int cw2[TEST_HEADING_COUNT];
        switch (settings.dictfontsize)
        {
        case 0: //big
            memcpy(cw2, test_defwidth_big, sizeof(int)*TEST_HEADING_COUNT);
            break;
        case 1: //med
            memcpy(cw2, test_defwidth_med, sizeof(int)*TEST_HEADING_COUNT);
            break;
        default: //small
            memcpy(cw2, test_defwidth_small, sizeof(int)*TEST_HEADING_COUNT);
            break;
        };

        int cwd2[TEST_HEADING_COUNT];
        memcpy(cwd2, cw2, sizeof(int)*TEST_HEADING_COUNT);

        bool cv2[TEST_HEADING_COUNT] = { true, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true }; //visible
        bool cr2[TEST_HEADING_COUNT] = { true, true, true, true, true, true, false, true, true, true, false, true, true, true, true, true, false }; //resizable
        bool ch2[TEST_HEADING_COUNT] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false }; //hoverable
        TWordColumnType ct2[TEST_HEADING_COUNT] = { wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom, wcCustom };
        TAlignment ca2[TEST_HEADING_COUNT] = { taCenter, taRightJustify, taCenter, taCenter, taCenter, taCenter, taCenter, taRightJustify, taRightJustify, taRightJustify, taCenter, taRightJustify, taRightJustify, taCenter, taLeftJustify, taLeftJustify, taLeftJustify };

        words->Clear();

        flisttype = alisttype;

        switch (flisttype)
        {
        case ltFree: //free items not yet studied
            for (int ix = 0; ix < FREE_HEADING_COUNT - 3; ++ix)
                cv0[ix] = freecolvis[ix];
            for (int ix = 0; ix < FREE_HEADING_COUNT; ++ix)
                if (settings.ltfreesizes[ix] > 0)
                    cw0[ix] = settings.ltfreesizes[ix];
            wobj->set_cols(fcollection, FREE_HEADING_COUNT, cw0, cwd0, cv0, ct0, ca0, study->freecount, indexproc);
            for (int ix = 0; ix < FREE_HEADING_COUNT; ++ix)
            {
                wobj->colresize[ix] = cr0[ix];
                wobj->colhover[ix] = ch0[ix];
            }
            wobj->SelectOrderColumn(abs(settings.ltfreeordercolumn) - 1, settings.ltfreeordercolumn < 0);
            break;
        case ltLocked: //locked items under study
            for (int ix = 0; ix < LOCK_HEADING_COUNT - 3; ++ix)
                cv1[ix] = lockedcolvis[ix];
            for (int ix = 0; ix < LOCK_HEADING_COUNT; ++ix)
                if (settings.ltlockedsizes[ix] > 0)
                    cw1[ix] = settings.ltlockedsizes[ix];
            wobj->set_cols(fcollection, LOCK_HEADING_COUNT, cw1, cwd1, cv1, ct1, ca1, study->lockedcount, indexproc);
            for (int ix = 0; ix < LOCK_HEADING_COUNT; ++ix)
            {
                wobj->colresize[ix] = cr1[ix];
                wobj->colhover[ix] = ch1[ix];
            }
            wobj->SelectOrderColumn(abs(settings.ltlockedordercolumn) - 1, settings.ltlockedordercolumn < 0);
            sbWordStats->Click();
            break;
        case ltTested: //tested items under study
            for (int ix = 0; ix < TEST_HEADING_COUNT - 3; ++ix)
                cv2[ix] = testedcolvis[ix];
            for (int ix = 0; ix < TEST_HEADING_COUNT; ++ix)
                if (settings.lttestedsizes[ix] > 0)
                    cw2[ix] = settings.lttestedsizes[ix];

            wobj->set_cols(fcollection, TEST_HEADING_COUNT, cw2, cwd2, cv2, ct2, ca2, 0, indexproc);
            for (int ix = 0; ix < TEST_HEADING_COUNT; ++ix)
            {
                wobj->colresize[ix] = cr2[ix];
                wobj->colhover[ix] = ch2[ix];
            }
            wobj->SelectOrderColumn(abs(settings.lttestedordercolumn) - 1, settings.lttestedordercolumn < 0);
            filter();
            sbWordStats->Click();
            break;
        }

        sbItems->Panels->Items[0]->Text = (UnicodeString)wobj->count + " :";
        generate_popup();
        ecallback();
        wcallback();

        setcaption();
    }
    catch (...)
    {
        throw;
    }
}
//---------------------------------------------------------------------------
void TfStudyList::generate_popup()
{
    TMenuItem *mi;
    while (pmList->Items->Count)
    {
        mi = pmList->Items->Items[0];
        pmList->Items->Delete(0);
        delete mi;
    }

    const wchar_t **strings = flisttype == ltFree ? freeheaders : flisttype == ltLocked ? lockedheaders : testedheaders;
    int cnt = (flisttype == ltFree ? FREE_HEADING_COUNT : flisttype == ltLocked ? LOCK_HEADING_COUNT : TEST_HEADING_COUNT) - 3;
    for (int ix = 0; ix < cnt; ++ix)
    {
        mi = new TMenuItem(pmList);
        mi->Caption = strings[ix];
        mi->AutoCheck = true;
        mi->Checked = (flisttype == ltFree ? freecolvis : flisttype == ltLocked ? lockedcolvis : testedcolvis)[ix];
        mi->OnClick = &colClick;
        mi->Tag = ix;
        pmList->Items->Add(mi);
    }
}
//---------------------------------------------------------------------------
UnicodeString TfStudyList::getheaderproc(int col)
{
    if (flisttype == ltFree)
        return freeheaders[col];
    if (flisttype == ltLocked)
        return lockedheaders[col];
    if (flisttype == ltTested)
        return testedheaders[col];

    return L"???";
}
//---------------------------------------------------------------------------
int TfStudyList::realrow(int row)
{
    if (!words->Count)
        return row;
    return words->Items[row];
}
//---------------------------------------------------------------------------
UnicodeString TfStudyList::meaningproc(int row)
{
    row = realrow(row);

    if (flisttype == ltFree)
        return study->freemeaning[row];
    if (flisttype == ltLocked)
        return study->lockedmeaning[row];
    if (flisttype == ltTested)
        return study->lockedmeaning[row];

    return "???";
}
//---------------------------------------------------------------------------
void TfStudyList::prepareproc(int col, int row, TWordColumnType ctype, TCanvas *c)
{
    row = realrow(row);

    TColor color = wobj->headerbuttoncolor;
    if (wobj->colselected == col)
    {
        c->Brush->Color = coloravg(c->Brush->Color, color, 0.9);
        c->Pen->Color = coloravg(c->Pen->Color, color, 0.9);
    }
    if (col != (flisttype == ltFree ? FREE_KANJI : flisttype == ltLocked ? LOCK_KANJI : TEST_KANJI) && col != (flisttype == ltFree ? FREE_KANA : flisttype == ltLocked ? LOCK_KANA : TEST_KANA) && col != (flisttype == ltFree ? FREE_MEANING : flisttype == ltLocked ? LOCK_MEANING : TEST_MEANING))
        c->Font->Size *= 0.9;

    if (col == (flisttype == ltFree ? FREE_KANJI : flisttype == ltLocked ? LOCK_KANJI : TEST_KANJI) || col == (flisttype == ltFree ? FREE_KANA : flisttype == ltLocked ? LOCK_KANA : TEST_KANA))
    {
        c->Font->Name = settings.kanafont;
        c->Font->Charset = settings.kanacharset;
    }

    if ((flisttype == ltLocked && col == LOCK_NEXT || flisttype == ltTested && col == TEST_NEXT) && get_day(study->lockednext[row]) <= get_day(snapshotNow))
    {
        c->Font->Color = coloravg(c->Font->Color, clRed, 0.35);
        c->Font->Style = TFontStyles() << fsBold;
    }

    TQuestionType itt = flisttype == ltFree ? study->freetype[row] : study->lockedtype[row];
    if (itt == qtMeaning && col == (flisttype == ltFree ? FREE_MEANING : flisttype == ltLocked ? LOCK_MEANING : TEST_MEANING))
    {
        c->Font->Color = coloravg((TColor)colorsettings.maintext, (TColor)0x8090ff, 0.5);
    }
    else if (itt == qtKanji && col == (flisttype == ltFree ? FREE_KANJI : flisttype == ltLocked ? LOCK_KANJI : TEST_KANJI))
    {
        c->Font->Color = coloravg((TColor)colorsettings.maintext, (TColor)0x8090ff, 0.5);
    }
    else if (itt == qtKana && col == (flisttype == ltFree ? FREE_KANA : flisttype == ltLocked ? LOCK_KANA : TEST_KANA))
    {
        c->Font->Color = coloravg((TColor)colorsettings.maintext, (TColor)0x8090ff, 0.5);
    }

    THintType htt = flisttype == ltFree ? study->freehint[row] : study->lockedhint[row];
    if (htt == htDefault)
        htt = (THintType)(itt == qtKanji ? profilesettings.kkanjihint : itt == qtKana ? profilesettings.kkanahint : itt == qtMeaning ? profilesettings.kmeaninghint : htDefault);
    if (htt == htDefault)
        THROW(L"Unknown type set for main word hint!");

    if (htt != htKanji && itt != qtKanji && col == (flisttype == ltFree ? FREE_KANJI : flisttype == ltLocked ? LOCK_KANJI : TEST_KANJI))
        c->Font->Color = coloravg(c->Font->Color, (TColor)colorsettings.background, 0.4);
    if (htt != htKana && itt != qtKana && col == (flisttype == ltFree ? FREE_KANA : flisttype == ltLocked ? LOCK_KANA : TEST_KANA))
        c->Font->Color = coloravg(c->Font->Color, (TColor)colorsettings.background, 0.4);
    if (htt != htMeaning && itt != qtMeaning && col == (flisttype == ltFree ? FREE_MEANING : flisttype == ltLocked ? LOCK_MEANING : TEST_MEANING))
        c->Font->Color = coloravg(c->Font->Color, (TColor)colorsettings.background, 0.4);

    if (flisttype == ltLocked)
    {
        int day = study->lasttest();
        if (study->lockedlastday[row] == day)
        {
            byte a = study->lockedscore[row], b = study->lockedoldscore[row];
            if (a > b && a && b)
            {
                c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x20ff50, 0.95);
            }
            else if (a < b && a && b)
            {
                c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x2030ff, 0.95);
            }
            else
            {
                c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x20f0ff, 0.95);
            }
        }

    }
    else if (flisttype == ltTested)
    {
        byte a, b;
        TRepAnswerCount ac;
        bool changed = false;

        switch (col)
        {
        case TEST_OLDSCORE:
        case TEST_OLDDECK:
            c->Font->Color = coloravg((TColor)colorsettings.maintext, c->Brush->Color, 0.6);
            break;
        case TEST_SCORE:
            a = study->lockedoldscore[row];
            b = study->lockedscore[row];
            if (!a || !b || a == b)
                break;
            c->Font->Style = c->Font->Style << fsBold;
            if (a < b)
            {
                c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x20ff50, 0.83);
                c->Font->Color = coloravg((TColor)colorsettings.maintext, (TColor)0x20ff50, 0.73);
            }
            else if (b < a)
            {
                c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x2030ff, 0.83);
                c->Font->Color = coloravg((TColor)colorsettings.maintext, (TColor)0x2030ff, 0.73);
            }
            else
                c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x20f0ff, 0.83);
            changed = true;
            break;
        case TEST_DECK:
            a = study->lockedolddeck[row];
            b = study->lockeddeck[row];
            if (!a || !b || a == b)
                break;
            c->Font->Style = c->Font->Style << fsBold;
            if (a < b)
            {
                c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x20ff50, 0.83);
                c->Font->Color = coloravg((TColor)colorsettings.maintext, (TColor)0x20ff50, 0.73);
            }
            else if (b < a)
            {
                c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x2030ff, 0.83);
                c->Font->Color = coloravg((TColor)colorsettings.maintext, (TColor)0x2030ff, 0.73);
            }
            else
                c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x20f0ff, 0.83);
            changed = true;
            break;
        case TEST_UNCERTAIN:
            c->Font->Color = coloravg((TColor)colorsettings.maintext, c->Brush->Color, 0.7);
            break;
        case TEST_HARD:
            ac = study->lockedlastanswers[row];
            if (ac.hard == 0)
                c->Font->Color = coloravg((TColor)colorsettings.maintext, c->Brush->Color, 0.7);
            else
            {
                c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x2030ff, 0.83);
                c->Font->Color = coloravg((TColor)colorsettings.maintext, (TColor)0x2030ff, 0.73);
                changed = true;
            }
            break;
        }
        if (!changed)
        {
            ac = study->lockedlastanswers[row];
            int res = (ac.easy - ac.hard) * 2 + (!ac.uncertain && ac.good ? 1 : 0);
            if (ac.easy)
            {
                c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x20ff50, 0.95);
            }
            else if (res < 0)
            {
                c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x2030ff, 0.95);
            }
            else if (!res)
            {
                c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x20f0ff, 0.95);
            }
        }
    }

    TColor cfont = c->Font->Color;
    fixtextcolor(cfont, c->Brush->Color);
    c->Font->Color = cfont;
}
//---------------------------------------------------------------------------
const TRect RectCross(TRect a, TRect b)
{
    return Rect(max(a.Left, b.Left), max(a.Top, b.Top), max(min(a.Right, b.Right), max(a.Left, b.Left)), max(min(a.Bottom, b.Bottom), max(a.Top, b.Top)));
}
//---------------------------------------------------------------------------
void TfStudyList::drawproc(int col, int row, TCanvas *c, const TRect &r)
{
    UnicodeString str = "";

    int nrow = realrow(row);

    TChangeArrowType arrow = atNone;
    bool nospacehide = true;

    TQuestionType qt = flisttype == ltFree ? study->freetype[nrow] : study->lockedtype[nrow];
    THintType ht = flisttype == ltFree ? study->freehint[nrow] : study->lockedhint[nrow];
    bool htchanged = true;
    if (ht == htDefault)
    {
        htchanged = false;
        if (qt == qtKanji)
            ht = (THintType)profilesettings.kkanjihint;
        else if (qt == qtKana)
            ht = (THintType)profilesettings.kkanahint;
        else if (qt == qtMeaning)
            ht = (THintType)profilesettings.kmeaninghint;
        else
            THROW(L"Unknown hint type!");
    }

    if (flisttype == ltFree && col == FREE_STUDIED || flisttype == ltLocked && col == LOCK_STUDIED || flisttype == ltTested && col == TEST_STUDIED)
    {
        TSize s;
        int w;
        int wW;
        int wS;
        int fs = c->Font->Size;
        TColor fc = c->Font->Color;

        GetTextExtentPoint32(c->Handle, L"W", 1, &s);
        wW = s.cx + 1;
        GetTextExtentPoint32(c->Handle, L" ", 1, &s);
        wS = s.cx;

        w = wW * 3 + wS * 2;

        TRect r2 = r;

        TAlignment al = wobj->colalignment[col];
        if (al == taLeftJustify)
        {
            c->FillRect(RectCross(Rect(r2.Left, r2.Top, r2.Left + 3*scaleres, r2.Bottom), r));
            r2.Left += 3 * scaleres;
            c->FillRect(Rect(r2.Left + w, r2.Top, r2.Right, r2.Bottom));
            r2.Right = r2.Left + w;
        }
        else if (al == taRightJustify)
        {
            c->FillRect(RectCross(Rect(r2.Right - 4*scaleres, r2.Top, r2.Right, r2.Bottom), r));
            r2.Right -= 4 * scaleres;
            c->FillRect(RectCross(Rect(r2.Left, r2.Top, r2.Right - w, r2.Bottom), r));
            r2.Left = r2.Right - w;
        }
        else
        {
            c->FillRect(RectCross(Rect(r2.Left, r2.Top, r2.Left + (r.Width() / 2 - w / 2), r2.Bottom), r));
            r2.Left += r.Width() / 2 - w / 2;
            w -= w / 2;
            c->FillRect(RectCross(Rect(r2.Left + w, r2.Top, r2.Right, r2.Bottom), r));
            r2.Right = r2.Left + w;
        }

        SetTextAlign(c->Handle, TA_BASELINE | TA_CENTER);
        c->Font->Size = fs;
        c->Font->Color = fc;

        TRect r3;

        //kanji
        if (qt == qtKanji)
        {
            c->Font->Size = fs;
            c->Font->Color = fc;
        }
        else
        {
            c->Font->Size = fs * 0.7;
            if (ht == htKanji)
                c->Font->Color = coloravg(c->Font->Color, (TColor)0x0000ff, 0.3);
            else
                c->Font->Color = coloravg(c->Font->Color, c->Brush->Color, 0.3);
        }
        r2.Right = r2.Left + wW;
        r3 = RectCross(r2, r);
        ExtTextOut(c->Handle, r2.Left + wW / 2, r2.Top + wobj->rowsize*(c->Font->Charset == DEFAULT_CHARSET ? 0.76 : 0.8), ETO_OPAQUE | ETO_CLIPPED, &r3, L"W", 1, NULL);
        r2.Left = r2.Right;

        //space
        c->Font->Size = fs;
        r2.Right = r2.Left + wS;
        c->FillRect(RectCross(r2, r));
        r2.Left = r2.Right;

        //kana
        if (qt == qtKana)
        {
            c->Font->Size = fs;
            c->Font->Color = fc;
        }
        else
        {
            c->Font->Size = fs * 0.7;
            if (ht == htKana)
                c->Font->Color = coloravg(c->Font->Color, (TColor)0x0000ff, 0.3);
            else
                c->Font->Color = coloravg(c->Font->Color, c->Brush->Color, 0.3);
        }
        r2.Right = r2.Left + wW;
        r3 = RectCross(r2, r);
        ExtTextOut(c->Handle, r2.Left + wW / 2, r2.Top + wobj->rowsize*(c->Font->Charset == DEFAULT_CHARSET ? 0.76 : 0.8), ETO_OPAQUE | ETO_CLIPPED, &r3, L"K", 1, NULL);
        r2.Left = r2.Right;

        //space
        c->Font->Size = fs;
        r2.Right = r2.Left + wS;
        c->FillRect(RectCross(r2, r));
        r2.Left = r2.Right;

        //Meaning
        if (qt == qtMeaning)
        {
            c->Font->Size = fs;
            c->Font->Color = fc;
        }
        else
        {
            c->Font->Size = fs * 0.7;
            if (ht == htMeaning)
                c->Font->Color = coloravg(c->Font->Color, (TColor)0x0000ff, 0.3);
            else
                c->Font->Color = coloravg(c->Font->Color, c->Brush->Color, 0.3);
        }
        r2.Right = r2.Left + wW;
        r3 = RectCross(r2, r);
        ExtTextOut(c->Handle, r2.Left + wW / 2, r2.Top + wobj->rowsize*(c->Font->Charset == DEFAULT_CHARSET ? 0.76 : 0.8), ETO_OPAQUE | ETO_CLIPPED, &r3, L"M", 1, NULL);

        c->Font->Size = fs;
        c->Font->Color = fc;
        return;
    }

    if (flisttype == ltFree)
    {
        switch (col)
        {
        case FREE_KANJI:
            str = study->freekanji[nrow];
            if (qt == qtKanji)
                arrow = atQuestion;
            if (ht == htKanji)
                arrow = htchanged ? atDotSelected : atDotDefault;

            nospacehide = false;
            break;
        case FREE_KANA:
            str = study->freekana[nrow];
            if (qt == qtKana)
                arrow = atQuestion;
            if (ht == htKana)
                arrow = htchanged ? atDotSelected : atDotDefault;

            nospacehide = false;
            break;
        case FREE_MEANING:
            str = study->freemeaning[nrow];
            if (qt == qtMeaning)
                arrow = atQuestion;
            if (ht == htMeaning)
                arrow = htchanged ? atDotSelected : atDotDefault;

            nospacehide = false;
            break;

        case FREE_NUM: //order
                    str = realrow(row) + 1;
            break;
        case FREE_ADDED: //added
                    str = DateString(study->freeadded[nrow], settings.datefmt);
            break;
        case FREE_PRIOR: //priority
                    str = study->freepriority[nrow];
            break;
        }
    }
    else if (flisttype == ltLocked)
    {
        byte a, b;
        int i;
        switch (col)
        {
        case LOCK_KANJI:
            str = study->lockedkanji[nrow];
            if (qt == qtKanji)
                arrow = atQuestion;
            if (ht == htKanji)
                arrow = htchanged ? atDotSelected : atDotDefault;

            nospacehide = false;
            break;
        case LOCK_KANA:
            str = study->lockedkana[nrow];
            if (qt == qtKana)
                arrow = atQuestion;
            if (ht == htKana)
                arrow = htchanged ? atDotSelected : atDotDefault;

            nospacehide = false;
            break;
        case LOCK_MEANING:
            str = study->lockedmeaning[nrow];
            if (qt == qtMeaning)
                arrow = atQuestion;
            if (ht == htMeaning)
                arrow = htchanged ? atDotSelected : atDotDefault;

            nospacehide = false;
            break;

        case LOCK_NUM: //order
                    str = realrow(row) + 1;
            break;
        case LOCK_ADDED: //added
                    str = DateString(study->lockedadded[nrow], settings.datefmt);
            break;
        case LOCK_SCORE: //score
                    b = study->lockedscore[nrow];
            str = b == 0 ? UnicodeString("-") : UnicodeString(int(b));

            if (b != 0)
            {
                if (study->lasttest() != study->lockedlastday[nrow])
                    break;

                a = study->lockedoldscore[nrow];
                if (a == 0)
                    break;

                if (a < b)
                    arrow = atUp;
                else if (a > b)
                    arrow = atDown;
                else
                    arrow = atDot;
            }
            break;
        case LOCK_DECK: //deck
                    b = study->lockeddeck[nrow];
            str = b == 0 ? UnicodeString("-") : UnicodeString(int(b));
            if (b != 0)
            {
                if (study->lasttest() != study->lockedlastday[nrow])
                    break;

                a = study->lockedolddeck[nrow];
                if (a == 0)
                    break;

                if (a < b)
                    arrow = atUp;
                else if (a > b)
                    arrow = atDown;
                else
                    arrow = atDot;
            }
            break;
        case LOCK_TRIES: //tries
                    str = study->lockedtries[nrow];
            break;
        case LOCK_FIRST: //first
                    str = generate_datestring(get_day(study->lockedfirst[nrow]), get_day(snapshotNow), settings.datefmt);
            break;
        case LOCK_LAST: //last
                    i = study->lockedstatcount[nrow];
            if (i > 0)
                str = generate_datestring(study->lockedstat[nrow][i - 1].day, get_day(snapshotNow), settings.datefmt);
            else
                str = "Never";
            break;
        case LOCK_NEXT: //next
                    str = generate_duestring(study->lockednext[nrow], snapshotNow);
            break;
        case LOCK_INT: //interval
                    str = generate_intervalstring(study->lockedinterval[nrow]);
            break;
        }
    }
    else if (flisttype == ltTested)
    {
        byte b;

        if (col == TEST_RESULT)
        {
            const TRepAnswerCount &ac = study->lockedlastanswers[nrow];
            int result = (ac.easy - ac.hard) * 2 + (!ac.uncertain && ac.good ? 1 : 0);
            int paddingsize = (r.Width() - arrows->Width) / 2;
            int atop = r.Top + (r.Height() - arrows->Height) / 2 + 1;
            c->FillRect(r);
            arrows->Draw(c, r.Left + paddingsize, atop, TChangeArrowType(result > 0 ? 0 : result < 0 ? 1 : 2));
            return;
        }
        else if (col == TEST_EASY)
        {
            const TRepAnswerCount &ac = study->lockedlastanswers[nrow];
            if (ac.easy)
            {
                c->FillRect(r);
                int paddingsize = (r.Width() - arrows->Width) / 2;
                int atop = r.Top + (r.Height() - arrows->Height) / 2 + 1;
                arrows->Draw(c, r.Left + paddingsize, atop, TChangeArrowType(2));
                return;
            }
        }

        switch (col)
        {
        case TEST_KANJI:
            str = study->lockedkanji[nrow];
            if (qt == qtKanji)
                arrow = atQuestion;
            if (ht == htKanji)
                arrow = htchanged ? atDotSelected : atDotDefault;

            nospacehide = false;
            break;
        case TEST_KANA:
            str = study->lockedkana[nrow];
            if (qt == qtKana)
                arrow = atQuestion;
            if (ht == htKana)
                arrow = htchanged ? atDotSelected : atDotDefault;

            nospacehide = false;
            break;
        case TEST_MEANING:
            str = study->lockedmeaning[nrow];
            if (qt == qtMeaning)
                arrow = atQuestion;
            if (ht == htMeaning)
                arrow = htchanged ? atDotSelected : atDotDefault;

            nospacehide = false;
            break;

        case TEST_NUM: //order
                    str = realrow(row) + 1;
            break;
        case TEST_ADDED: //added
                    str = DateString(study->lockedadded[nrow], settings.datefmt);
            break;
        case TEST_OLDSCORE: //score
                    b = study->lockedoldscore[nrow];
            str = b == 0 ? UnicodeString("-") : UnicodeString(int(b));
            break;
        case TEST_SCORE: //score
                    b = study->lockedscore[nrow];
            str = b == 0 ? UnicodeString("-") : UnicodeString(int(b));
            break;
        case TEST_OLDDECK: //deck
                    b = study->lockedolddeck[nrow];
            str = b == 0 ? UnicodeString("-") : UnicodeString(int(b));
            break;
        case TEST_DECK: //deck
                    b = study->lockeddeck[nrow];
            str = b == 0 ? UnicodeString("-") : UnicodeString(int(b));
            break;
        case TEST_TIME: //last
                    str = generate_intervalstring(fabs(snapshotNow - study->lockedlasttime[nrow]));
            break;
        case TEST_UNCERTAIN: //score
                    str = int(study->lockedlastanswers[nrow].uncertain);
            break;
        case TEST_HARD: //score
                    str = int(study->lockedlastanswers[nrow].hard);
            break;
        case TEST_NEXT: //next
                    str = generate_duestring(study->lockednext[nrow], snapshotNow);
            break;
        case TEST_INT: //interval
                    str = generate_intervalstring(study->lockedinterval[nrow]);
            break;
        }
    }

    if (arrow != atNone)
    {
        int w = wobj->MeasureCellText(str);
        const float paddingsize = wobj->rowsize * 0.25;

        if (!nospacehide || w + arrows->Width + paddingsize/*+2*scaleres*/ <= r.Width())
        {
            TRect r2 = r;
            TCanvas *c = wobj->Canvas;
            int atop = r.Top + (r.Height() - arrows->Height) / 2 + 1;
            arrows->Draw(c, r.Left + paddingsize, atop, c->Brush->Color, arrow);
            r2.Left += paddingsize + arrows->Width/*+2*scaleres*/;
            c->FillRect(Rect(r.Left, r.Top, r.Left + paddingsize, r.Bottom));
            c->FillRect(Rect(r.Left + paddingsize + arrows->Width, r.Top, r2.Left, r.Bottom));
            c->FillRect(Rect(r.Left + paddingsize, r.Top, r2.Left, atop));
            c->FillRect(Rect(r.Left + paddingsize, atop + arrows->Height, r2.Left, r.Bottom));
            wobj->DrawCellText(r2, str, wobj->colalignment[col]);
            return;
        }
    }

    wobj->DrawCellText(r, str, wobj->colalignment[col]);
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::FormPaint(TObject *Sender)
{
    TRect r;
    if (fpagetype == ptList)
    {
        //toolbar
        DrawButtonBar(Canvas, Rect(0, pList->Top, ClientWidth, pList->Top + b1->Top));
        //meaning definition and word statistics panel
        DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), COLORDEC(clBtnFace, 0x101010), ChildRect(pMeaning, 0, 1, 0, 0));
        if (pWordStats->Visible)
        {
            DrawGrad(Canvas, COLORDEC(clBtnFace, 0x101010), COLORDEC(clBtnFace, 0x202020), ChildRect(pWordStatButtons));
        }
    }
    else if (fpagetype == ptStats)
    {
        //top
        r = ChildRect(pStats);
        r.Bottom = ChildRect(bv4).Top;
        DrawGrad(Canvas, clWhite, clBtnFace, r);
        //Stat text
        r = Rect(0, ChildRect(bv4).Top + 1, ClientWidth, ChildRect(bv5).Top);
        DrawGrad(Canvas, COLORINC(clBtnFace, 0x060606), COLORDEC(clBtnFace, 0x101010), r);
        //above button bar
        r = Rect(0, ChildRect(bv5).Top + 1, ClientWidth, ChildRect(bv6).Top);
        DrawGrad(Canvas, COLORDEC(clBtnFace, 0x101010), COLORDEC(clBtnFace, 0x303030), r);
        //button bar
        DrawButtonBar(this, bv6, bv7);
    }
    //bottom buttons
    DrawCloseBar(this, b2, NULL);
    //topmost bar
    r = ChildRect(pStats);
    DrawTabBar(Canvas, Rect(0, 0, ClientWidth, r.Top - 1));
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::FormResize(TObject *Sender)
{
    if (savesize >= 0)
        splitsize(savesize);

    repaint_panels();
}
//---------------------------------------------------------------------------
void TfStudyList::repaint_panels()
{
    TRect r;
    r = ChildRect(pMeaning);
    r.Top++;
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
    r = Rect(0, b2->Top + 1, ClientWidth, ClientHeight);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE);

    if (pWordStats->Visible)
    {
        r = ChildRect(pWordStatButtons);
        RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::pbListMouseUp(TObject *Sender, TMouseButton button, TShiftState shift, int x, int y)
{
    if (button == mbRight && y < wobj->headersize && y >= 0)
    {
        TPoint p = pbList->ClientToScreen(Point(x, y));
        pmList->Popup(p.x, p.y);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::colClick(TObject *Sender)
{
    TMenuItem *mi = ((TMenuItem*)Sender);
    (flisttype == ltFree ? freecolvis : flisttype == ltLocked ? lockedcolvis : testedcolvis)[mi->Tag] = mi->Checked;
    wobj->colvisible[mi->Tag] = mi->Checked;
}
//---------------------------------------------------------------------------
void TfStudyList::headerpressproc(int oldcol)
{
    if (wobj->count)
    {
        int oldpos = realrow(wobj->pos);
        int oldvpos = pbList->VPos;
        reorder();
        int p = 0;
        for (int ix = 0; ix < words->Count; ++ix)
        {
            if (words->Items[ix] == oldpos)
            {
                p = ix;
                break;
            }
        }

        int rows = (pbList->ClientHeight - wobj->headersize) / wobj->rowsize;
        if ((p > oldvpos || p == oldvpos && !oldvpos) && (p < oldvpos + rows - 1 || p == oldvpos + rows - 1 && oldvpos + rows == wobj->count))
        {
            if (oldvpos != pbList->VPos)
                pbList->VPos = oldvpos;
            else
                pbList->Refresh();
            wobj->pos = p;
            return;
        }
        wobj->pos = p;

        int wvpos = min(p, max(0, p - rows / 2 + 1));
        if (wvpos != pbList->VPos && wvpos <= wobj->count - rows)
            pbList->VPos = wvpos;
        else
            pbList->Refresh();
    }
    else
    {
        if (oldcol >= 0)
            wobj->invalidatecol(oldcol);
        if (wobj->colselected >= 0)
            wobj->invalidatecol(wobj->colselected);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::sbFreeClick(TObject *Sender)
{
    if (((TSpeedButton*)Sender)->Down)
        showlist((TListType)((TSpeedButton*)Sender)->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::sbList1Click(TObject *Sender)
{
    settings.ltlist = (TListType)((TSpeedButton*)Sender)->Tag; //moved here from showlist 2011.feb 18, not to save the last tab when we first open the window and change it programatically
    showlist((TListType)((TSpeedButton*)Sender)->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::pbListKeyPress(TObject *Sender, wchar_t &Key)
{
    if ((Key >= L'A' && Key <= L'Z') || (Key >= L'a' && Key <= L'z'))
    {
        edFilter->SetFocus();
        pbFilter->OnKeyPress(Sender, Key);
    }
    else if (Key == 0x08) // Backspace key.
    {
        edFilter->SetFocus();
        pbFilter->OnKeyPress(Sender, Key);
    }
    else if (Key == 0x0D) // Enter key.
        edFilter->SetFocus();
    else if (Key == 0x1B) // Escape key.
        edFilter->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::pbListKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
//    if (Key >= 'A' && Key <= 'Z' && !Shift.Contains(ssCtrl))
//    {
//        edFilter->SetFocus();
//        pbFilter->OnKeyDown(Sender, Key, Shift);
//    }

    if (flisttype == ltFree && Key >= '1' && Key <= '9' && wobj->count && Shift.Contains(ssShift))
        setpriority(Key - '1' + 1);

//    if (Key == VK_BACK)
//    {
//        edFilter->SetFocus();
//        pbFilter->OnKeyDown(Sender, Key, Shift);
//    }
//    else if (Key == VK_RETURN)
//        edFilter->SetFocus();
//    else if (Key == VK_ESCAPE)
//        edFilter->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::pbFilterKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    HWND h = GetCapture();
    if (h)
        return;

    if (Key == VK_UP || Key == VK_DOWN)
    {
        if (wobj->count)
        {
            pbList->SetFocus();
            pbList->OnKeyDown(Sender, Key, Shift);
        }
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::N11Click(TObject *Sender)
{
    setpriority(((TMenuItem*)Sender)->Tag);
}
//---------------------------------------------------------------------------
void TfStudyList::setpriority(int priority)
{
    if (flisttype != ltFree || !wobj->count)
        return;

    bool changed = false;
    MODON
    for (int ix = wobj->selstart; ix < wobj->selstart + wobj->sellength; ++ix)
    {
        if (study->freepriority[realrow(ix)] != priority)
        {
            study->freepriority[realrow(ix)] = priority;
            changed = true;
        }
    }
    if (changed)
        fcollection->Modify(false);
    MODOFF

    if (changed)
    {
        int oldpos = realrow(wobj->pos);
        reorder();
        if (wobj->colselected == 2)
        {
            int p = 0;

            if (wobj->sellength == 1)
            {
                for (int ix = 0; ix < words->Count; ++ix)
                {
                    if (words->Items[ix] == oldpos)
                    {
                        p = ix;
                        break;
                    }
                }
            }

            if (p != wobj->pos)
            {
                wobj->pos = p;
                int wvpos = min(p, max(0, p - (pbList->ClientHeight - wobj->headersize) / wobj->rowsize / 2 + 1));
                if (wvpos != pbList->VPos)
                    pbList->VPos = wvpos;
                else
                    pbList->Invalidate();
            }
            else
                wobj->invalidateselected();
        }
        else
            wobj->invalidateselected();
    }
}
//---------------------------------------------------------------------------
void TfStudyList::sethint(THintType hint)
{
    if (wobj->sellength != 1 || flisttype == ltFree && study->freehint[realrow(wobj->selstart)] == hint || flisttype != ltFree && study->lockedhint[realrow(wobj->selstart)] == hint)
        return;

    MODON
    if (flisttype == ltFree)
        study->freehint[realrow(wobj->selstart)] = hint;
    else
        study->lockedhint[realrow(wobj->selstart)] = hint;
    fcollection->Modify(false);
    MODOFF

    wobj->invalidaterow(wobj->selstart);
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::sbShowKanjiClick(TObject *Sender)
{
    ecallback();
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::sbAddItemClick(TObject *Sender)
{
    TWordIndexList *l = NULL;
    int res = mrCancel;
    try
    {
        l = new TWordIndexList(fcollection/*,false*/);
        for (int ix = 0; ix < wobj->sellength; ++ix)
        {
            int r = flisttype == ltFree ? study->freeindex[realrow(wobj->selstart + ix)] : study->lockedindex[realrow(wobj->selstart + ix)];
            l->Add(r);
        }

        res = ListImport(l);
    }
    catch (...)
    {
        ;
    }
    delete l;
    if (res == mrOk)
    {
        if (flisttype == ltFree)
        {
            int oldpos = realrow(wobj->pos);
            filter();
            int p = 0;
            for (int ix = 0; ix < words->Count; ++ix)
            {
                if (words->Items[ix] == oldpos)
                {
                    p = ix;
                    break;
                }
            }

            wobj->pos = p;
            int wvpos = min(p, max(0, p - (pbList->ClientHeight - wobj->headersize) / wobj->rowsize / 2 + 1));
            if (wvpos != pbList->VPos)
                pbList->VPos = wvpos;
            else
                pbList->Invalidate();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::sbRemoveItemMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft || !sbRemoveItem->Enabled)
        return;

    if (flisttype == ltFree)
    {
        MouseRepeat(sbRemoveItemMouseDown, Sender, Button, Shift, X, Y);
        removeitem();
    }
}
//---------------------------------------------------------------------------
void TfStudyList::removeitem()
{
    double n = Now();
    bool showing = false;

    int ss = wobj->selstart;
    TIntegerList *l = new TIntegerList;
    for (int ix = 0; ix < wobj->sellength; ++ix)
        l->Add(realrow(ix + ss));
    l->Sort(false);

    for (int ix = l->Count - 1; ix >= 0; --ix)
    {
        if (!showing && (double)Now() - n > 0.3 / 86400.0 && ix > 2 * l->Count / 3)
        {
            ShowMessageForm("Please wait...");
            showing = true;
        }

        int r = l->Items[ix];
        if (flisttype == ltFree)
            study->delete_free(r, true);
        else if (flisttype == ltLocked || flisttype == ltTested)
            study->delete_locked(r, true);
        words->Delete(ss);
    }
    for (int ix = 0; ix < words->Count; ++ix)
    {
        if (!showing && (double)Now() - n > 0.3 / 86400.0 && ix < words->Count / 3)
        {
            ShowMessageForm("Please wait...");
            showing = true;
        }

        int i = words->Items[ix];
        int p = 0;
        while (p < l->Count && l->Items[p] < i)
            p++;
        words->Items[ix] = i - p;
    }

    if (showing)
        HideMessageForm();

    delete l;

    wobj->set_virtual(fcollection, words->Count, indexproc);
    wobj->pos = min(ss, wobj->count - 1);
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::sbRemoveItemClick(TObject *Sender)
{
    if (flisttype == ltLocked || flisttype == ltTested)
    {
        if (!allowchange())
            return;
        if (MsgBox("The selected items' study data will be lost. Do you want to remove them from the list?", "Warning", MB_YESNO) == IDNO)
            return;
        removeitem();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::sbToQueueClick(TObject *Sender)
{
    if (!allowchange())
        return;

    if (MsgBox("The selected items' study data will be lost. Do you want to move them back to the queue?", "Warning", MB_YESNO) == IDNO)
        return;

    double n = Now();
    bool showing = false;

    int ss = wobj->selstart;
    TIntegerList *l = new TIntegerList;
    for (int ix = 0; ix < wobj->sellength; ++ix)
        l->Add(realrow(ix + ss));
    l->Sort(false);

    for (int ix = l->Count - 1; ix >= 0; --ix)
    {
        if (!showing && (double)Now() - n > 0.3 / 86400.0 && ix > 2 * l->Count / 3)
        {
            ShowMessageForm("Please wait...");
            showing = true;
        }

        study->queue_locked(l->Items[ix]);
        words->Delete(ss);
    }
    for (int ix = 0; ix < words->Count; ++ix)
    {
        int i = words->Items[ix];
        int p = 0;
        while (p < l->Count && l->Items[p] < i)
            p++;
        words->Items[ix] = i - p;
    }

    delete l;

    if (showing)
        HideMessageForm();

    wobj->set_virtual(fcollection, words->Count, indexproc);
    wobj->pos = min(ss, wobj->count - 1);
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::miAddItemClick(TObject *Sender)
{
    if (sbAddItem->Enabled)
        sbAddItem->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::miRemoveItemClick(TObject *Sender)
{
    if (sbRemoveItem->Enabled)
    {
        if (flisttype == ltFree)
            removeitem();
        else
            sbRemoveItem->Click();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::miToQueueClick(TObject *Sender)
{
    if (sbToQueue->Enabled)
        sbToQueue->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::edMeaningChange(TObject *Sender)
{
    if (wobj->count)
    {
        UnicodeString current = flisttype == ltFree ? study->freemeaning[realrow(wobj->pos)] : study->lockedmeaning[realrow(wobj->pos)];
        btnSave->Enabled = edMeaning->Enabled && edMeaning->Text.Trim().Length() && edMeaning->Text.Trim() != current;
        btnReset->Enabled = edMeaning->Enabled && current != study->generate_meaning(flisttype == ltFree ? study->freeindex[realrow(wobj->pos)] : study->lockedindex[realrow(wobj->pos)]);
    }
    else
    {
        btnSave->Enabled = false;
        btnReset->Enabled = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::btnResetClick(TObject *Sender)
{
    edMeaning->Text = study->generate_meaning(flisttype == ltFree ? study->freeindex[realrow(wobj->pos)] : study->lockedindex[realrow(wobj->pos)]);
    btnSave->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::btnSaveClick(TObject *Sender)
{
    if (flisttype == ltFree)
        study->freemeaning[realrow(wobj->pos)] = edMeaning->Text.Trim();
    else if (flisttype == ltLocked || flisttype == ltTested)
        study->lockedmeaning[realrow(wobj->pos)] = edMeaning->Text.Trim();
    edMeaningChange(edMeaning);
    pbList->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::sbWordStatsClick(TObject *Sender)
{
    pWordStats->Visible = sbWordStats->Down;
    settings.ltshowitemstat = sbWordStats->Down;
    FormResize(this);

    if (sbWordStats->Down && fpagetype == ptList)
    {
        count_wordstat();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::splitWordStatsMouseEnter(TObject *Sender)
{
    if (!sbWordStats->Down)
        return;

    Screen->Cursor = crVSplit;
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::splitWordStatsMouseLeave(TObject *Sender)
{
    if (!sbWordStats->Down)
        return;

    Screen->Cursor = crDefault;
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::splitWordStatsMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (!sbWordStats->Down)
        return;

    mousesizing = false;
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::splitWordStatsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (!sbWordStats->Down)
        return;

    TPoint p;
    GetCursorPos(&p);
    mousey = p.y;
    mousesize = pWordStats->Height;
    mousesizing = Screen->Cursor == crVSplit;
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::splitWordStatsMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    if (!sbWordStats->Down)
        return;

    if (!Shift.Contains(ssLeft))
        mousesizing = false;
    if (mousesizing)
    {
        TPoint p;
        GetCursorPos(&p);

        splitsize(mousesize + (int)(mousey - p.y));
        savesize = pWordStats->Height;
    }
}
//---------------------------------------------------------------------------
void TfStudyList::splitsize(int size)
{
    if (savesize < 0)
        savesize = size;

    size = min(max(size, 150 * scaleres), pList->Height - 150 * scaleres);

    if (size == pWordStats->Height)
        return;

    settings.ltitemstatsize = pWordStats->Height;

    pWordStats->Height = size;
    repaint_panels();
}
//---------------------------------------------------------------------------
void TfStudyList::count_globstat(TGlobalStatPage page)
{
    if (fpagetype == ptList || !gstatinvalid)
        return;

    gstatinvalid = false;

    if (page == gspAuto)
    {
        TGlobalStatPage sp[] = { gspResults, gspItems, gspTime, gstpDecks, gspTries, gspAvg };
        TSpeedButton *sb[] = { sbG1, sbG2, sbG3, sbG4, sbG5, sbG6 };
        for (unsigned int ix = 0; ix < sizeof(sb) / sizeof(TSpeedButton*) && page == gspAuto; ++ix)
        {
            if (sb[ix]->Down)
                page = sp[ix];
        }
    }

    settings.ltglobalstat = page;
    fstatpage = page;

    int maxval = 5;
    int bkey = 1;
    int keyinc = 1;

    int pos = 0;
    UnicodeString valstr;
    UnicodeString colstr;
    int last = -1;
    int *v;
    int redline = -1;
    int numdiv = 10;
    unsigned int n = get_day(Now());
    unsigned int c;
    TGapList<int> li(false);
    TGapList<double> ld(false);
    switch (page)
    {
    case gspResults:
        valstr = "Tested items";
        colstr = "Date";
        maxval = 20;
        bkey = n;
        c = study->statcount;
        if (!c)
            break;
        for (unsigned int ix = 0; ix < c; ++ix)
        {
            const TDayStat &stat = study->daystat[ix];
            if (!stat.testcount && last < 0)
            {
                last = stat.day - 1;
                continue;
            }
            if (last >= 0)
            {
                for (int iy = last + 1; iy < stat.day; ++iy)
                    scols->AddEmptyKey(iy);
            }
            v = scols->Add(3, stat.day, true);
            last = stat.day;
            v[0] = stat.testcount - stat.testfailure - stat.testlearned;
            v[1] = stat.testlearned;
            v[2] = stat.testfailure;

            maxval = max(maxval, stat.testcount);
        }

        pos = scols->Count * wsobj->column_size - wsobj->drawarea.Width() / 2;

        for (unsigned int ix = last + 1; ix <= n; ++ix)
            scols->AddEmptyKey(ix);

        break;
    case gspItems:
        valstr = "Number of items";
        colstr = "Date";
        maxval = 20;
        bkey = n;
        c = study->statcount;
        if (!c)
            break;
        for (unsigned int ix = 0; ix < c; ++ix)
        {
            const TDayStat &stat = study->daystat[ix ? ix - 1 : 0];
            last = stat.day;
            if (!ix)
            {
                v = scols->Add(4, stat.day, false);
                v[0] = stat.itemcount;
                v[1] = stat.itemlearned;
                v[2] = stat.testcount;
                v[3] = stat.testednew;

                maxval = max(maxval, stat.itemcount);
                continue;
            }

            const TDayStat &stat2 = study->daystat[ix];
            for (int iy = last + 1; iy < stat2.day; ++iy)
            {
                v = scols->Add(2, iy, false);
                v[0] = stat.itemcount;
                v[1] = stat.itemlearned;
            }

            v = scols->Add(4, stat2.day, false);
            v[0] = stat2.itemcount;
            v[1] = stat2.itemlearned;
            v[2] = stat2.testcount;
            v[3] = stat2.testednew;

            maxval = max(maxval, stat2.itemcount);
        }
        for (unsigned int ix = study->daystat[c - 1].day + 1; ix < n; ++ix)
        {
            v = scols->Add(2, ix, false);
            v[0] = study->daystat[c - 1].itemcount;
            v[1] = study->daystat[c - 1].itemlearned;
        }
        redline = scols->Count - 1;

        study->duecounts(li);
        for (unsigned int ix = n; ix < li.Count; ++ix)
        {
            if (!li.Items[ix] && ix > n)
                scols->AddEmptyKey(ix);
            else if (li.Items[ix])
            {
                v = scols->Add(1, ix, false);
                v[0] = li.Items[ix];
                maxval = max(maxval, v[0]);
            }
        }
        pos = redline < 0 ? scols->Count * wsobj->column_size : (redline + 1) * wsobj->column_size - wsobj->drawarea.Width() / 2;
        break;
    case gspTime:
        valstr = "Time spent on testing";
        colstr = "Date";
        maxval = 10;
        numdiv = 300;
        bkey = n;
        c = study->statcount;
        if (!c)
            break;
        for (unsigned int ix = 0; ix < c; ++ix)
        {
            const TDayStat &stat = study->daystat[ix];
            if (!stat.testcount && last < 0)
            {
                last = stat.day - 1;
                continue;
            }

            if (last >= 0)
            {
                for (int iy = last + 1; iy < stat.day; ++iy)
                    scols->AddEmptyKey(iy);
            }

            v = scols->Add(1, ix, false);
            last = stat.day;
            v[0] = stat.length * 86400;
            maxval = max(maxval, v[0]);
        }

        pos = scols->Count * wsobj->column_size - wsobj->drawarea.Width() / 2;

        for (unsigned int ix = last + 1; ix <= n; ++ix)
            scols->AddEmptyKey(ix);
        break;
    case gstpDecks:
        valstr = "Items in deck";
        colstr = "Decks";
        maxval = 10;
        study->deckcounts(li);
        for (unsigned int ix = 1; ix < li.Count; ++ix)
        {
            if (!li.Items[ix])
                scols->AddEmptyKey(ix);
            else
            {
                v = scols->Add(1, ix, false);
                v[0] = li.Items[ix];
                maxval = max(maxval, v[0]);
            }
        }
        break;
    case gspTries:
        valstr = "Number of items";
        colstr = "Number of inclusions";
        maxval = 10;
        study->trycounts(li);
        for (unsigned int ix = 1; ix < li.Count; ++ix)
        {
            if (!li.Items[ix])
                scols->AddEmptyKey(ix);
            else
            {
                v = scols->Add(1, ix, false);
                v[0] = li.Items[ix];
                maxval = max(maxval, v[0]);
            }
        }
        break;
    case gspAvg:
        valstr = "Avg. time for items";
        colstr = "Deck";
        maxval = 10;
        numdiv = 300;
        bkey = 0;
        c = study->timestatcount;
        for (unsigned int ix = 0; ix < c; ++ix)
        {
            v = scols->Add(1, ix, false);
            v[0] = study->timeaverage[ix] * 86400;
            maxval = max(maxval, v[0]);
        }
        break;
    }

    wsobj->basekey = bkey;
    wsobj->keyincrement = keyinc;
    wsobj->set_items(scols, colstr, valstr, maxval, numdiv, redline, numproc, keyproc, hintproc);
    wsobj->pos = pos;
    scols->Clear();
}
//---------------------------------------------------------------------------
void TfStudyList::count_wordstat(TWordStatPage page)
{
    if (fpagetype == ptStats)
        return;

    if (page == wspAuto)
        page = (TWordStatPage)(sbWordDeck->Down ? sbWordDeck->Tag : sbWordScore->Tag);

    settings.ltitemstat = page;
    fwordpage = page;

    byte maxval = 5;
    int bkey = get_day(Now());

    UnicodeString valstr;
    if (page == wspDeck)
    {
        valstr = "Deck";
        maxval = 10;
    }
    else if (page == wspScore)
    {
        valstr = "Score";
        maxval = 100;
    }

    if (wobj->sellength == 1)
    {
        int itemrow = realrow(wobj->pos);
        int statcnt = study->lockedstatcount[itemrow];
        for (int ix = 0; ix < statcnt; ++ix)
        {
            const TRepStat &stat = study->lockedstat[itemrow][ix];
            int *v = scols->Add(1, stat.day, false);
            if (page == wspDeck)
            {
                v[0] = stat.deck;
                maxval = max(maxval, v[0]);
            }
            else if (page == wspScore)
            {
                v[0] = stat.score;
            }
        }
    }

    wsobj->basekey = bkey;
    wsobj->set_items(scols, "Day of test", valstr, maxval, 10, -1, numproc, keyproc, hintproc);
    wsobj->pos = wsobj->count * wsobj->column_size;
    scols->Clear();
}
//---------------------------------------------------------------------------
UnicodeString TfStudyList::numproc(int val)
{
    if (fpagetype == ptList || fpagetype == ptStats && fstatpage != gspTime && fstatpage != gspAvg)
        return val;
    else
        return generate_timestring(val);
}
//---------------------------------------------------------------------------
UnicodeString TfStudyList::keyproc(int ix, int key)
{
    if (fpagetype == ptList || fpagetype == ptStats && (fstatpage == gspResults || fstatpage == gspItems))
        return DateString(key);
    else if (fpagetype == ptStats && fstatpage == gspTime)
    {
        if ((unsigned int)key >= study->statcount)
            return DateString(key);
        return DateString(study->daystat[key].day);
    }
    else
    {
        if (key == 0)
            return "New";
        return (UnicodeString)key + ".";
    }
}
//---------------------------------------------------------------------------
UnicodeString TfStudyList::hintproc(int ix, const TStatColumn &column)
{
    UnicodeString result = "";
    if (fpagetype == ptList)
    {
        if (wobj->sellength == 1)
        {
            int itemrow = realrow(wobj->pos);
            const TRepStat &stat = study->lockedstat[itemrow][ix];
            result = (UnicodeString)"Deck: " + stat.deck + "\nScore: " + stat.score;
        }
    }
    else
    {
        int c = study->statcount;
        int val1, val2;
        const TStatColumn *col;
        if (!c || !column.valcnt)
            return result;
        switch (fstatpage)
        {
        case gspResults:
            if (column.values[0] + column.values[1] + column.values[2])
            {
                val1 = int(((double)column.values[1] / (column.values[0] + column.values[1] + column.values[2])) * 100);
                val2 = int(((double)column.values[2] / (column.values[0] + column.values[1] + column.values[2])) * 100);
                result = (UnicodeString)"Tested: " + (column.values[0] + column.values[1] + column.values[2]) + "\nLearned: " + column.values[1] + " (" + val1 + "%)" + "\nIncorrect: " + column.values[2] + " (" + val2 + "%)";
            }
            break;
        case gspItems:
            if (ix <= wsobj->redline)
            {
                if (column.valcnt == 1)
                {
                    if (ix)
                    {
                        col = wsobj->columns[ix - 1];
                        val1 = column.values[0] - col->values[0];
                    }
                    else
                        val1 = column.values[0];
                    if (val1 < 0)
                        result = (UnicodeString)"Removed: " + (-val1);
                    else if (val1 > 0)
                        result = (UnicodeString)"New items: " + val1;
                    else
                        result = "Nothing learned today";
                }
                else if (column.valcnt == 2)
                {
                    if (ix)
                    {
                        col = wsobj->columns[ix - 1];
                        val1 = column.values[0] - col->values[0];
                    }
                    else
                        val1 = column.values[0];
                    if (column.values[1])
                    {
                        if (column.values[0])
                            c = int(((double)column.values[1] / column.values[0]) * 100);
                        else
                            c = 0;
                        if (result.Length())
                            result += "\n";
                        result += (UnicodeString)"Learned: " + column.values[1] + " (" + c + "%)";
                    }
                    if (val1 < 0)
                    {
                        if (result.Length())
                            result += "\n";
                        result += (UnicodeString)"Removed: " + (-val1);
                    }
                    if (result.Length())
                        result += "\n";
                    result += "Not studied...";
                }
                else
                {
                    if (ix)
                    {
                        col = wsobj->columns[ix - 1];
                        val1 = column.values[0] - col->values[0];
                    }
                    else
                        val1 = column.values[0];
                    if (column.values[1])
                    {
                        if (column.values[0])
                            c = int(((double)column.values[1] / column.values[0]) * 100);
                        else
                            c = 0;

                        if (result.Length())
                            result += "\n";
                        result += (UnicodeString)"Learned: " + column.values[1] + " (" + c + "%)";
                    }
                    if (column.values[2])
                    {
                        if (result.Length())
                            result += "\n";
                        result += (UnicodeString)"Tested: " + column.values[2];
                    }
                    if (column.values[3])
                    {
                        if (result.Length())
                            result += "\n";
                        result += (UnicodeString)"New: " + column.values[3];
                    }
                    if (val1 < 0)
                    {
                        if (result.Length())
                            result += "\n";
                        result += (UnicodeString)"Removed: " + (-val1);
                    }
                    if (column.values[2] == 0)
                    {
                        if (result.Length())
                            result += "\n";
                        result += "Not studied...";
                    }
                }
            }
            else
            {
                if (!column.valcnt)
                    result = "Nothing scheduled.";
                result = (UnicodeString)"Scheduled: " + column.values[0];
            }
            break;
        case gspTime:
            if ((unsigned int)column.key < study->statcount)
            {
                result = (UnicodeString)study->daystat[column.key].testcount + " items tested";
            }
            break;
        case gstpDecks:
            break;
        case gspTries:
            break;
        case gspAvg:
            c = study->timeretries[column.key] * 100;
            result = (UnicodeString)"Retried " + (c / 100) + (c == 100 || c == 0 ? (UnicodeString)"" : (UnicodeString)"." + (c -int(c / 100) * 100)) + " time" + (c == 100 ? "" : "s") + "\nin average";
            break;
        }
    }
    return result;
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::sbWordDeckClick(TObject *Sender)
{
    count_wordstat((TWordStatPage)((TSpeedButton*)Sender)->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::btnTestClick(TObject *Sender)
{
    if (!allowchange())
        return;
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
bool TfStudyList::allowchange()
{
    if (!study->allowchange())
    {
        MsgBox("The system's time is set to a day which is earlier than the day of the last change in the long-term study list. Either set the date back or wait until the given day arrives.", "Error", MB_OK);
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void TfStudyList::fillstats()
{
    lbITotal->Caption = study->lockedcount;
    lbIWords->Caption = study->wordcount();
    lbIKanji->Caption = study->kanjicount();
    lbIDue->Caption = study->itemsdue();
    if (lbIDue->Caption != '0')
        lbIDue->Font->Style = TFontStyles() << fsBold;
    else
        lbIDue->Font->Style = TFontStyles();

    sh1->Visible = lbIDue->Caption != '0';
    if (sh1->Visible)
    {
        lbIDueTitle->Color = sh1->Brush->Color;
        lbIDue->Color = sh1->Brush->Color;
    }
    else
    {
        lbIDueTitle->ParentColor = true;
        lbIDue->ParentColor = true;
    }

    int n = get_day(Now());
    int day = study->firsttest();
    lbTFirst->Caption = day ? generate_datestring(day, n, settings.datefmt) : (UnicodeString)"-";
    day = study->lasttest();
    lbTLast->Caption = day ? generate_datestring(day, n, settings.datefmt) : (UnicodeString)"-";
    lbTDays->Caption = study->testdays();
    lbTSkipped->Caption = study->skippeddays();

    lbTOverall->Caption = TimeString(study->overalltime());
    lbTAverage->Caption = TimeString(study->testaverage());
    lbTAAverage->Caption = TimeString(study->answeraverage());
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::sbG1Click(TObject *Sender)
{
    gstatinvalid = true;
    count_globstat((TGlobalStatPage)((TSpeedButton*)Sender)->Tag);
}
//---------------------------------------------------------------------------
void TfStudyList::z_daychange(TMessage &msg)
{
    snapshotNow = Now();

    study->daychanged();

    fillstats();
    if (fpagetype == ptList)
    {
        count_wordstat();
        pbList->Invalidate();
    }
    else if (fpagetype == ptStats)
    {
        gstatinvalid = true;
        count_globstat();
    }

    setcaption();
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::btnSettingsClick(TObject *Sender)
{
    if (ShowSettings(L"Long-term study"))
        PostMessage(Handle, WM_Z_DAYCHANGED, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::miDeckIncClick(TObject *Sender)
{
    if (!allowchange())
        return;

    int change = ((TMenuItem*)Sender)->Tag;

    study->set_deckinterval(realrow(wobj->pos), study->lockeddeck[realrow(wobj->pos)] + change, change > 0 ? incinterval : decinterval);

    headerpressproc(wobj->colselected);
    wcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::miResetDataClick(TObject *Sender)
{
    if (!allowchange())
        return;

    if (MsgBox("All selected words will be put back to deck 0 with minimal interval, and their long-term study statistics will be removed.\nPlease consider using the \"Remove\" and \"Back to queue\" options if you would like to remove the selected words from the list.\n\nDo you want to continue?", "Query", MB_YESNO) == IDNO)
        return;

    for (int ix = 0; ix < wobj->sellength; ++ix)
        study->resetitem(realrow(wobj->selstart + ix));

    headerpressproc(wobj->colselected);
    wcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::miToGroupClick(TObject *Sender)
{
    if (!wobj->sellength)
        return;

    if (wobj->sellength == 1)
    {
        int wix;
        if (flisttype == ltFree)
        {
            wix = study->freeindex[realrow(wobj->pos)];
        }
        else if (flisttype == ltLocked || flisttype == ltTested)
        {
            wix = study->lockedindex[realrow(wobj->pos)];
        }

        MODON
        WordToGroup(fcollection, wix, gindex);
        fcollection->Modify(false);
        MODOFF

                return;
    }

    TWordIndexList *l = NULL;
    l = new TWordIndexList(fcollection/*,false*/);
    for (int ix = 0; ix < wobj->sellength; ix++)
    {
        int wix;
        if (flisttype == ltFree)
        {
            wix = study->freeindex[realrow(wobj->selstart + ix)];
        }
        else if (flisttype == ltLocked || flisttype == ltTested)
        {
            wix = study->lockedindex[realrow(wobj->selstart + ix)];
        }
        l->Add(wix);
    }

    MODON
    WordToGroup(l, gindex);

    if (l->Count)
        fcollection->Modify(false);
    MODOFF

            delete l;
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    TSpeedButton *sbtn[] = { sbG1, sbG2, sbG3, sbG4, sbG5, sbG6 };
    TSpeedButton *lbtn[] = { sbList1, sbList2, sbList3 };

    if (!dynamic_cast<TEdit *>(ActiveControl) || Shift.Contains(ssCtrl))
    {
        if (fpagetype == ptStats)
        {
            if (Key >= '1' && Key <= '6' && Shift.Empty())
            {
                sbtn[Key - '1']->Down = true;
                sbtn[Key - '1']->Click();
            }
        }
        else if (fpagetype == ptList)
        {
            if (Key >= '1' && Key <= '3' && Shift.Empty())
            {
                lbtn[Key - '1']->Down = true;
                lbtn[Key - '1']->Click();
            }
        }
    }

    if (Key == VK_TAB && Shift.Contains(ssCtrl))
    {
        if (!Shift.Contains(ssShift))
        {
            if (tTabs->TabIndex == tTabs->Tabs->Count - 1)
                tTabs->TabIndex = 0;
            else
                tTabs->TabIndex++;
        }
        else
        {
            if (tTabs->TabIndex == 0)
                tTabs->TabIndex = tTabs->Tabs->Count - 1;
            else
                tTabs->TabIndex--;
        }
        tTabsChange(tTabs);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::miHintDefaultClick(TObject *Sender)
{
    sethint((THintType)((TMenuItem*)Sender)->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TfStudyList::btnHackClick(TObject *Sender)
{
#ifdef HACK_TEST_REMOVE
    if (MsgBox(L"Removing the last test is irreversible! Back up your data files before you continue! You've been warned!\n\nAre you sure?", L"Query", MB_YESNO) == IDNO)
        return;

    study->RemoveLastTest();
    TMessage dummymsg = { };
    filter();
    z_daychange(dummymsg);
#endif
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(197);
#endif

