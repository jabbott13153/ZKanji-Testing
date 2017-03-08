//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Math.h>
#include <DateUtils.hpp>
#include "studylist.h"
#include "collection.h"
#include "repetition.h"
#include "words.h"
#include "settings.h"
#include "kanjicard.h"
#include "studyroom.h"
#include "studyreading.h"
#include "searchtree.h"
#include "qsrt.h"
#include "romajizer.h"
#include "numinput.h"
#include "msgform.h"
#include "zkformats.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(160);

// Make compiler happy by using variables declared elsewhere:
static bool Dummy_function()
{
    return OneHour * OneMinute * OneSecond * OneMillisecond > 10;
}
#endif

//---------------------------------------------------------------------------
UnicodeString TimeString(double d)
{
    return generate_timestring(d * 86400);
}
//---------------------------------------------------------------------------
UnicodeString generate_timestring(int d)
{
    int y = d / (365.25 * 24 * 60 * 60);
    d -= y * (365.25 * 24 * 60 * 60);
    int dy = d / (24 * 60 * 60);
    d -= dy * (24 * 60 * 60);
    int h = d / (60 * 60);
    d -= h * (60 * 60);
    int m = d / 60;
    d -= m * 60;
    int s = d;

    UnicodeString r = "";
    if (y)
        r += (UnicodeString)y + "yr ";
    if (dy)
        r += (UnicodeString)dy + "d ";
    if (h)
        r += (UnicodeString)h + "h ";
    if (m)
        r += (UnicodeString)m + "m ";
    if (s)
        r += (UnicodeString)s + "s ";
    if (r == "")
        r = "0s";
    return r.Trim();
}
//---------------------------------------------------------------------------
UnicodeString generate_intervalstring(double ival, double showdays)
{
    if (ival < -0.001)
        return "Unknown";

    UnicodeString str;
    if (ival >= max(showdays, 3652.5))
    {
        double d = RoundTo(ival / 3652.5, -1);
        str = (UnicodeString)d + " decade" + (d == 1 ? "" : "s");
    }
    else if (ival >= max(showdays, 365.25))
    {
        double d = RoundTo(ival / 365.25, -1);
        str = (UnicodeString)d + " year" + (d == 1 ? "" : "s");
    }
    else if (ival >= max(showdays, 30.4375))
    {
        double d = RoundTo(ival / 30.4375, -1);
        str = (UnicodeString)d + " month" + (d == 1 ? "" : "s");
    }
    else if (ival >= max(showdays, 7))
    {
        double d = RoundTo(ival / 7, -1);
        str = (UnicodeString)d + " week" + (d == 1 ? "" : "s");
    }
    else if (ival >= 1)
    {
        double d = RoundTo(ival, -1);
        str = (UnicodeString)d + " day" + (d == 1 ? "" : "s");
    }
    else if (ival >= OneHour)
    {
        double d = RoundTo(ival / OneHour, -1);
        str = (UnicodeString)d + " hour" + (d == 1 ? "" : "s");
    }
    else if (ival >= OneMinute)
    {
        double d = RoundTo(ival / OneMinute, -1);
        str = (UnicodeString)d + " min."/*+(d == 1 ? "" : "s")*/;
    }
    else if (ival >= OneSecond)
    {
        double d = RoundTo(ival / OneSecond, -1);
        str = (UnicodeString)d + " sec."/*+(d == 1 ? "" : "s")*/;
    }
    else
        str = "nothing";

    return str;
}
//---------------------------------------------------------------------------
UnicodeString generate_datestring(int d, int n, TDateFormatSetting format)
{
    if (d == n)
        return "Today";
    if (d + 1 == n)
        return "Yesterday";

    return DateString(d, format);
}
//---------------------------------------------------------------------------
/*
 UnicodeString generate_datetimestring(TDateTime date, TDateTime dateNow)
 {
 UnicodeString str = generate_intervalstring(fabs(dateNow-date));
 if (str == "nothing")
 str = "a moment ago";
 else
 str += " ago";
 return str;
 }*/
//---------------------------------------------------------------------------
/*UnicodeString generate_datestring(TDateTime date, TDateTime origin)
 {
 TDateTime a = date, b = origin;

 UnicodeString suffix = "";//" ago";
 if (origin < date) {
 suffix = "";
 swap(a,b);
 }

 int y1 = YearOf(a), y2 = YearOf(b);

 UnicodeString str = " ";
 float span = (y2-y1)/10;
 if (span >= 1) {
 double d = RoundTo(span,-1);
 str = (UnicodeString)d+" decade"+(d == 1 ? "" : "s");
 return str+suffix;
 }

 span = YearSpan(b,a);
 if (span >= 1) {
 double d = RoundTo(span,-1);
 str = (UnicodeString)d+" year"+(d == 1 ? "" : "s");
 return str+suffix;
 }

 span = MonthSpan(b,a);
 if (span >= 1) {
 double d = RoundTo(span,-1);
 str = (UnicodeString)d+" month"+(d == 1 ? "" : "s");
 return str+suffix;
 }

 span = DaySpan(b,a);
 if (span >= 1) {
 double d = RoundTo(span,-1);
 str = (UnicodeString)d+" day"+(d == 1 ? "" : "s");
 return str+suffix;
 }

 span = HourSpan(b,a);
 if (span >= 1) {
 double d = RoundTo(span,-1);
 str = (UnicodeString)d+" hour"+(d == 1 ? "" : "s");
 return str+suffix;
 }

 span = MinuteSpan(b,a);
 if (span >= 1) {
 double d = RoundTo(span,-1);
 str = (UnicodeString)d+" minute"+(d == 1 ? "" : "s");
 return str+suffix;
 }

 span = SecondsBetween(b,a);
 double d = RoundTo(span,-1);
 str = (UnicodeString)d+" second"+(d == 1 ? "" : "s");

 return str+suffix;
 }*/
//---------------------------------------------------------------------------
UnicodeString generate_duestring(TDateTime date, TDateTime dueNow)
{
    double span;

    double d1 = date;
    double d2 = dueNow;

    if (get_day(d2) >= get_day(d1))
        return "Due";
    if (get_day(d2) == get_day(d1) - 1)
        return "Tomorrow";
    if (d1 - d2 < 60.0)
    {
        span = DaySpan(dueNow, date);
        if (span >= 1)
        {
            double d = RoundTo(span, -1);
            if (d - (int)d < 0.5)
                d = (int)d;
            else
                d = Ceil(d);
            return (UnicodeString)d + " day" + (d == 1 ? "" : "s");
        }
        span = HourSpan(dueNow, date);
        if (span >= 1)
        {
            double d = RoundTo(span, -1);
            if (d - (int)d < 0.5)
                d = (int)d;
            else
                d = Ceil(d);
            return (UnicodeString)d + " hour" + (d == 1 ? "" : "s");
        }

        span = MinuteSpan(dueNow, date);
        if (span >= 1)
        {
            double d = RoundTo(span, -1);
            if (d - (int)d < 0.5)
                d = (int)d;
            else
                d = Ceil(d);
            return (UnicodeString)d + " minute" + (d == 1 ? "" : "s");
        }

        span = SecondsBetween(dueNow, date);
        double d = RoundTo(span, -1);
        if (d - (int)d < 0.5)
            d = (int)d;
        else
            d = Ceil(d);
        return (UnicodeString)d + " second" + (d == 1 ? "" : "s");
    }

    return DateString(date);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int wordstudy_duelist_sort_proc(TWordStudyList::TLockedWordStudyItem **a, TWordStudyList::TLockedWordStudyItem **b)
{
    double r = ((*a)->ritem->testdate + (*a)->ritem->interval) - ((*b)->ritem->testdate + (*b)->ritem->interval);
    if (r == 0)
        return (int)(*a) - (int)(*b);
    return r < 0 ? -1 : 1;
}

TWordStudyList *wordstudy_sort_list;
int __fastcall wordstudy_priority_sort_proc(int a, int b)
{
    int r = wordstudy_sort_list->freepriority[a] - wordstudy_sort_list->freepriority[b];
    if (!r)
        return a - b;
    return r;
}

int wordstudy_testlist_sort_proc(TWordStudyList::TTestElem **a, TWordStudyList::TTestElem **b)
{
    double amin = 999999, bmin = 999999;
    for (int j = 0; j < (*a)->itemcount; j++)
        if ((*a)->item[j].score < amin)
            amin = (*a)->item[j].score;
    for (int j = 0; j < (*b)->itemcount; j++)
        if ((*b)->item[j].score < amin)
            return 1;
        else if ((*b)->item[j].score < bmin)
            bmin = (*b)->item[j].score;

    return (int)amin - (int)bmin;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void TWordStudyList::FreeItem(TWordStudyDefinition *item, int ix)
{
    delete[] item->fmeaning;
    delete item;
}
//---------------------------------------------------------------------------
void TWordStudyList::changemeaning(TWordStudyDefinition *def, const UnicodeString &meaning)
{
    if (!meaning.Length() && !def->fmeaning || def->fmeaning && def->fmeaning == meaning)
        return;

    MODON
    delete[] def->fmeaning;
    def->fmeaning = new wchar_t[meaning.Trim().Length() + 1];
    wcscpy(def->fmeaning, meaning.Trim().c_str());

    collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------
bool TWordStudyList::hastype(TWordStudyDefinition *def, TQuestionType qtype)
{
    return def->ftakentype & (1 << qtype);
}
//---------------------------------------------------------------------------
bool TWordStudyList::hastype(int ix, TQuestionType qtype)
{
    return hastype(Items[ix], qtype);
}
//---------------------------------------------------------------------------
const wchar_t* TWordStudyList::wordkanji(TWordStudyDefinition *def)
{
    if (changegap && changegap->ItemSet((int)def))
        return changegap->Items[(int)def].kanji;
    return fcollection->words->Items[def->fwordindex]->kanji;
}
//---------------------------------------------------------------------------
const wchar_t* TWordStudyList::wordkana(TWordStudyDefinition *def)
{
    if (changegap && changegap->ItemSet((int)def))
        return changegap->Items[(int)def].kana;
    return fcollection->words->Items[def->fwordindex]->kana;
}
//---------------------------------------------------------------------------
const wchar_t* TWordStudyList::wordromaji(TWordStudyDefinition *def)
{
    if (changegap && changegap->ItemSet((int)def))
        return Romanize(changegap->Items[(int)def].kana).c_str();
    return Romanize(fcollection->words->Items[def->fwordindex]->kana).c_str(); //romaji;
}
//---------------------------------------------------------------------------
const wchar_t* TWordStudyList::wordmeaning(TWordStudyDefinition *def)
{
    return def->fmeaning;
}
//---------------------------------------------------------------------------
int TWordStudyList::wordindex(TWordStudyDefinition *def)
{
    return def->fwordindex;
}
//---------------------------------------------------------------------------
TDateTime TWordStudyList::wordinclude(TWordStudyDefinition *def)
{
    return def->flastinclude;
}
//---------------------------------------------------------------------------
TWord* TWordStudyList::wordfromdef(TWordStudyDefinition *def)
{
    if (changegap && changegap->ItemSet((int)def))
        return NULL;
    return def->fwordindex >= 0 ? fcollection->words->Items[def->fwordindex] : NULL;
}
//---------------------------------------------------------------------------
const wchar_t* TWordStudyList::wordkanji(int defix)
{
    return wordkanji(Items[defix]);
}
//---------------------------------------------------------------------------
const wchar_t* TWordStudyList::wordkana(int defix)
{
    return wordkana(Items[defix]);
}
//---------------------------------------------------------------------------
const wchar_t* TWordStudyList::wordromaji(int defix)
{
    return wordromaji(Items[defix]);
}
//---------------------------------------------------------------------------
const wchar_t* TWordStudyList::wordmeaning(int defix)
{
    return wordmeaning(Items[defix]);
}
//---------------------------------------------------------------------------
int TWordStudyList::wordindex(int defix)
{
    return wordindex(Items[defix]);
}
//---------------------------------------------------------------------------
TDateTime TWordStudyList::wordinclude(int defix)
{
    return wordinclude(Items[defix]);
}
//---------------------------------------------------------------------------
byte TWordStudyList::wordusage(int defix)
{
    TWordStudyDefinition *def = Items[defix];
    return (int)hastype(def, qtKanji) + (int)hastype(def, qtKana) + (int)hastype(def, qtMeaning);
}
//---------------------------------------------------------------------------
TWord* TWordStudyList::wordfromindex(int defix)
{
    return wordfromdef(Items[defix]);
}
//---------------------------------------------------------------------------
void TWordStudyList::changemeaning(int defix, const UnicodeString &meaning)
{
    changemeaning(Items[defix], meaning);
}
//---------------------------------------------------------------------------
bool TWordStudyList::allowchange()
{
    return category->allowchange(studyid);

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordStudyList::TTestReadingList::TTestReadingList(TWordStudyList *aowner) : owner(aowner)
{
    testlist = new TTestReadings;
    tested = new TTestedReadings;
    undolist = new TReadingsUndoList;
}
//---------------------------------------------------------------------------
TWordStudyList::TTestReadingList::~TTestReadingList()
{
    clear();
    delete undolist;
    delete tested;
    delete testlist;
}
//---------------------------------------------------------------------------
void TWordStudyList::TTestReadingList::clear()
{
    resetundo();
    for (int ix = testlist->Count - 1; ix >= 0; --ix)
    {
        delete testlist->Items[ix]->words;
        testlist->Delete(ix);
    }
    for (int ix = tested->Count - 1; ix >= 0; --ix)
        tested->Delete(ix);
}
//---------------------------------------------------------------------------
void TWordStudyList::TTestReadingList::SaveToFile(FILE *f)
{
    int c = testlist->Count;
    fwrite(&c, sizeof(int), 1, f);
    for (int ix = 0; ix < testlist->Count; ++ix)
    {
        TTestReadingItem *item = testlist->Items[ix];
        fwrite(&item->kanjiindex, sizeof(int), 1, f);
        fwrite(&item->reading, sizeof(byte), 1, f);
        //fwrite(&item->newword,sizeof(bool),1,f);
        //fwrite(&item->failed,sizeof(bool),1,f);

        c = item->words->Count;
        fwrite(&c, sizeof(int), 1, f);
        for (int iy = 0; iy < item->words->Count; ++iy)
        {
            TTestReadingItem::TTestReadingWord *w = item->words->Items[iy];
            fwrite(&w->itemindex, sizeof(int), 1, f);
            fwrite(&w->pos, sizeof(byte), 1, f);

            fwrite(&w->newword, sizeof(bool), 1, f);
            fwrite(&w->failed, sizeof(bool), 1, f);
        }
    }

    c = tested->Count;
    fwrite(&c, sizeof(int), 1, f);
    for (int ix = 0; ix < tested->Count; ++ix)
    {
        TTestedReadingItem *item = tested->Items[ix];
        fwrite(&item->kanjiindex, sizeof(int), 1, f);
        fwrite(&item->reading, sizeof(byte), 1, f);
        fwrite(&item->itemindex, sizeof(int), 1, f);
    }
}
//---------------------------------------------------------------------------
//added Apr 1, 2011 for v0.577 - sort owner and functions only needed to convert to group list ver 11
TWordStudyList *__sort_proc_owner;
int __sort_testword_proc(TWordStudyList::TTestReadingList::TTestReadingItem::TTestReadingWord **a, TWordStudyList::TTestReadingList::TTestReadingItem::TTestReadingWord **b)
{
    return __sort_proc_owner->lockedindex[(*a)->itemindex] - __sort_proc_owner->lockedindex[(*b)->itemindex];
}
int __sort_tested_proc(TWordStudyList::TTestReadingList::TTestedReadingItem **a, TWordStudyList::TTestReadingList::TTestedReadingItem **b)
{
    int r = (*a)->kanjiindex - (*b)->kanjiindex;
    if (r)
        return r;
    r = (*a)->reading - (*b)->reading;
    if (r)
        return r;
    return __sort_proc_owner->lockedindex[(*a)->itemindex] - __sort_proc_owner->lockedindex[(*b)->itemindex];
    /*if (tested->Items[ix]->kanjiindex > item->kanjiindex ||
     tested->Items[ix]->kanjiindex == item->kanjiindex &&
     (tested->Items[ix]->reading > item->reading || tested->Items[ix]->reading == item->reading && owner->lockedindex[tested->Items[ix]->itemindex] > owner->lockedindex[item->itemindex]))
     */
}
void TWordStudyList::TTestReadingList::LoadFromFile(FILE *f, int version)
{
    __sort_proc_owner = owner;

    int c;
    fread(&c, sizeof(int), 1, f);
    testlist->Capacity = c;
    while (testlist->Count < testlist->Capacity)
    {
        TTestReadingItem *item = new TTestReadingItem;
        fread(&item->kanjiindex, sizeof(int), 1, f);
        fread(&item->reading, sizeof(byte), 1, f);

        bool tmpn, tmpf;
        if (version < 13) // Changed 2011.05.01 for v0.59.
        {
            fread(&tmpn, sizeof(bool), 1, f);
            fread(&tmpf, sizeof(bool), 1, f);
            //fread(&item->newword,sizeof(bool),1,f);
            //fread(&item->failed,sizeof(bool),1,f);
        }

        item->words = new TTestReadingItem::TTestReadingWords;

        fread(&c, sizeof(int), 1, f);
        item->words->Capacity = c;
        while (item->words->Count < item->words->Capacity)
        {
            TTestReadingItem::TTestReadingWord *w = new TTestReadingItem::TTestReadingWord;
            fread(&w->itemindex, sizeof(int), 1, f);
            fread(&w->pos, sizeof(byte), 1, f);
            if (version >= 13) // Added 2011.05.01 for v0.59.
            {
                fread(&w->newword, sizeof(bool), 1, f);
                fread(&w->failed, sizeof(bool), 1, f);
            }
            else
            {
                w->newword = tmpn;
                w->failed = tmpf;
            }
            item->words->Add(w);
        }
        testlist->Add(item);
        if (version < 11) // Added Apr 1, 2011 for v0.577.
                    item->words->Sort(&__sort_testword_proc);
    }

    fread(&c, sizeof(int), 1, f);
    tested->Capacity = c;
    while (tested->Count < tested->Capacity)
    {
        TTestedReadingItem *item = new TTestedReadingItem;
        fread(&item->kanjiindex, sizeof(int), 1, f);
        fread(&item->reading, sizeof(byte), 1, f);
        fread(&item->itemindex, sizeof(int), 1, f);

        tested->Add(item);
    }
    if (version < 11)
        tested->Sort(&__sort_tested_proc);
}
//---------------------------------------------------------------------------
void TWordStudyList::TTestReadingList::add_item(int itemindex, bool newword, bool failed)
{
    resetundo();

    try
    {
        TWord *w = owner->collection->words->Items[owner->lockedindex[itemindex]];
        int wlen = wcslen(w->kanji);

        for (int ix = 0; ix < wlen; ++ix)
        {
            if (!KANJI(w->kanji[ix]))
                continue;
            int r = FindReading(ix, w);
            if (r <= 0)
                continue;

            int kindex = kanjis->IndexOf(w->kanji[ix]);
            bool found = false;

            for (int iy = 0; iy < tested->Count && !found; ++iy)
            {
                TTestedReadingItem *item = tested->Items[iy];
                if (item->kanjiindex > kindex || item->kanjiindex == kindex && (item->reading > r || item->reading == r && owner->lockedindex[item->itemindex] > owner->lockedindex[itemindex]))
                    break;
                if (item->kanjiindex == kindex && item->reading == r && owner->lockedindex[item->itemindex] == owner->lockedindex[itemindex])
                    found = true;
            }

            if (found)
                continue;

            int kpos = testlist->Count;
            int ipos = 0;

            for (int iy = 0; iy < kpos && !found; ++iy)
            {
                if (testlist->Items[iy]->kanjiindex > kindex || testlist->Items[iy]->kanjiindex == kindex && testlist->Items[iy]->reading > r)
                    kpos = iy;
                else if (testlist->Items[iy]->kanjiindex == kindex && testlist->Items[iy]->reading == r)
                {
                    kpos = iy;
                    ipos = testlist->Items[iy]->words->Count;
                    for (int iz = 0; iz < ipos && !found; iz++)
                    {
                        if (owner->lockedindex[testlist->Items[iy]->words->Items[iz]->itemindex] == owner->lockedindex[itemindex])
                        {
                            bool undofound = false;
                            for (int j = 0; j < undolist->Count && !undofound; ++j)
                            {
                                if (undolist->Items[j]->word == testlist->Items[iy]->words->Items[iz])
                                    undofound = true;
                            }

                            if (!undofound)
                            {
                                TTestReadingUndoData *undodata = new TTestReadingUndoData;
                                undodata->word = testlist->Items[iy]->words->Items[iz];
                                undodata->failed = testlist->Items[iy]->words->Items[iz]->failed;
                                undolist->Add(undodata);
                            }

                            testlist->Items[iy]->words->Items[iz]->newword |= newword;
                            testlist->Items[iy]->words->Items[iz]->failed |= failed;
                            found = true;
                        }
                        else if (owner->lockedindex[testlist->Items[iy]->words->Items[iz]->itemindex] > owner->lockedindex[itemindex])
                        {
                            ipos = iz;
                        }
                    }
                }
            }

            if (found)
                continue;

            TTestReadingItem *item;
            if (kpos == testlist->Count || testlist->Items[kpos]->kanjiindex != kindex || testlist->Items[kpos]->reading != r)
            {
                item = new TTestReadingItem;
                item->kanjiindex = kindex;
                item->reading = r;
                item->words = new TTestReadingItem::TTestReadingWords;
                testlist->Insert(kpos, item);
            }
            else
                item = testlist->Items[kpos];

            TTestReadingItem::TTestReadingWord *w = new TTestReadingItem::TTestReadingWord;
            w->itemindex = itemindex;
            w->pos = ix;
            w->newword = newword;
            w->failed = failed;
            item->words->Insert(ipos, w);

            TTestReadingUndoData *undodata = new TTestReadingUndoData;
            undodata->word = w;
            undodata->failed = false;
            undolist->Add(undodata);
        }

    }
    catch (...)
    {
        ;
    }
}
//---------------------------------------------------------------------------
void TWordStudyList::TTestReadingList::undo(bool failed)
{
    for (int ix = 0; ix < undolist->Count; ++ix)
        undolist->Items[ix]->word->failed = undolist->Items[ix]->failed || failed;
}
//---------------------------------------------------------------------------
void TWordStudyList::TTestReadingList::resetundo()
{
    undolist->Clear();
}
//---------------------------------------------------------------------------
void TWordStudyList::TTestReadingList::remove_item(int itemindex)
{
    for (int ix = tested->Count - 1; ix >= 0; --ix)
    {
        if (tested->Items[ix]->itemindex == itemindex)
            tested->Delete(ix);
        else if (tested->Items[ix]->itemindex > itemindex)
            tested->Items[ix]->itemindex--;
    }

    for (int ix = testlist->Count - 1; ix >= 0; --ix)
    {
        for (int iy = testlist->Items[ix]->words->Count - 1; iy >= 0; --iy)
        {
            if (testlist->Items[ix]->words->Items[iy]->itemindex == itemindex)
            {
                testlist->Items[ix]->words->Delete(iy);
                if (testlist->Items[ix]->words->Count == 0)
                {
                    delete testlist->Items[ix]->words;
                    testlist->Delete(ix);
                    break;
                }
            }
            else if (testlist->Items[ix]->words->Items[iy]->itemindex > itemindex)
                testlist->Items[ix]->words->Items[iy]->itemindex--;
        }
    }
}
//---------------------------------------------------------------------------
bool TWordStudyList::TTestReadingList::due(int kanjiix)
{
    if (profilesettings.ktestreadings == trNone)
        return false;

    if (kanjiix < 0 || kanjiix >= testlist->Count)
        THROW(L"Index for due test kanji out of bounds!");

    TTestReadingItem *item = testlist->Items[kanjiix];
    TKanji *k = kanjis->Kanjis[item->kanjiindex];
    bool hasnew = false;
    bool hasfailed = false;

    for (int ix = 0; ix < item->words->Count && (!hasnew || !hasfailed); ++ix)
    {
        hasnew |= item->words->Items[ix]->newword;
        hasfailed |= item->words->Items[ix]->failed;
    }

    if (profilesettings.kreadingwords == rwNew && !hasnew || profilesettings.kreadingwords == rwMistakes && !hasfailed || profilesettings.kreadingwords == rwNewAndMistakes && !hasnew && !hasfailed || profilesettings.ktestreadings == trON && item->reading > k->oncnt || profilesettings.ktestreadings == trKun && item->reading <= k->oncnt)
        return false;
    return true;
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestReadingList::firstdue()
{
    if (profilesettings.ktestreadings == trNone)
        return -1;

    int index = -1;
    for (int ix = 0; ix < testlist->Count && index < 0; ++ix)
        if (due(ix))
            index = ix;

    return index;
}
//---------------------------------------------------------------------------
bool TWordStudyList::TTestReadingList::test()
{
    fKStudy = new TfKStudy(Application);
    if (settings.studyreadingwidth > 0 && settings.studyreadingheight > 0)
    {
        fKStudy->Width = settings.studyreadingwidth;
        fKStudy->Height = settings.studyreadingheight;
    }
    int modres = mrCancel;
    try
    {
        modres = fKStudy->ShowModalEx(owner);
    }
    catch (...)
    {
        ;
    }
    settings.studyreadingwidth = fKStudy->Width;
    settings.studyreadingheight = fKStudy->Height;
    delete fKStudy;

    return modres == mrOk;
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestReadingList::testedpos(TTestedReadingItem *item)
{
    int pos = tested->Count;
    for (int ix = 0; ix < tested->Count && pos == tested->Count; ++ix)
        if (tested->Items[ix]->kanjiindex > item->kanjiindex || tested->Items[ix]->kanjiindex == item->kanjiindex && (tested->Items[ix]->reading > item->reading || tested->Items[ix]->reading == item->reading && owner->lockedindex[tested->Items[ix]->itemindex] > owner->lockedindex[item->itemindex]))
            pos = ix;
    return pos;
}
//---------------------------------------------------------------------------
void TWordStudyList::TTestReadingList::next()
{
    int ix = firstdue();
    if (ix < 0)
        THROW(L"No due items to next!");

    for (int iy = testlist->Items[ix]->words->Count - 1; iy >= 0; --iy)
    {
        TTestedReadingItem *item = new TTestedReadingItem;
        item->kanjiindex = testlist->Items[ix]->kanjiindex;
        item->reading = testlist->Items[ix]->reading;
        item->itemindex = testlist->Items[ix]->words->Items[iy]->itemindex;

        tested->Insert(testedpos(item), item);
    }
    delete testlist->Items[ix]->words;
    testlist->Delete(ix);
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestReadingList::duecount()
{
    if (profilesettings.ktestreadings == trNone)
        return 0;

    int cnt = 0;
    int ix = firstdue();
    if (ix < 0)
        return 0;

    for (; ix < testlist->Count; ++ix)
    {
        if (due(ix))
            cnt++;
    }
    return cnt;
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestReadingList::wordcount()
{
    int ix = firstdue();
    if (ix < 0)
        THROW(L"No due reading items to get word count from!");

    int cnt = 0;
    for (int j = 0; j < testlist->Items[ix]->words->Count; ++j)
    {
        if (!j || owner->lockedindex[testlist->Items[ix]->words->Items[j - 1]->itemindex] != owner->lockedindex[testlist->Items[ix]->words->Items[j]->itemindex])
            cnt++;
    }
    return cnt;
}
//---------------------------------------------------------------------------
TWordStudyList::TTestReadingList::TTestReadingItem::TTestReadingWord* TWordStudyList::TTestReadingList::currentword(int wix)
{
    int ix = firstdue();
    if (ix < 0)
        THROW(L"No due reading items to get currentindex!");

    for (int j = 0; j < testlist->Items[ix]->words->Count; ++j)
    {
        if (!j || owner->lockedindex[testlist->Items[ix]->words->Items[j - 1]->itemindex] != owner->lockedindex[testlist->Items[ix]->words->Items[j]->itemindex])
            wix--;
        if (wix < 0)
            return testlist->Items[ix]->words->Items[j];
    }
    return NULL;
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestReadingList::currentindex(int wix)
{
    return currentword(wix)->itemindex;
}
//---------------------------------------------------------------------------
bool TWordStudyList::TTestReadingList::currentwordfailed(int wix)
{
    int ix = firstdue();
    if (ix < 0)
        THROW(L"No due reading items to get currentindex!");

    return currentword(wix)->failed;
}
//---------------------------------------------------------------------------
bool TWordStudyList::TTestReadingList::currentwordnew(int wix)
{
    int ix = firstdue();
    if (ix < 0)
        THROW(L"No due reading items to get currentindex!");

    return currentword(wix)->newword;
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestReadingList::currentkanjipos(int wix)
{
    return currentword(wix)->pos;
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestReadingList::currentkanjiindex()
{
    int ix = firstdue();
    if (ix < 0)
        THROW(L"No due reading items to get current kanji index!");

    return testlist->Items[ix]->kanjiindex;
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestReadingList::currentreading()
{
    int ix = firstdue();
    if (ix < 0)
        THROW(L"No due reading items to get current reading!");

    return testlist->Items[ix]->reading;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TWordStudyList::next_reading()
{
    testreadings->next();
}
//---------------------------------------------------------------------------
int TWordStudyList::get_rdue()
{
    return testreadings->duecount();
}
//---------------------------------------------------------------------------
int TWordStudyList::get_rwordcount()
{
    return testreadings->wordcount();
}
//---------------------------------------------------------------------------
wchar_t TWordStudyList::get_rchar()
{
    TKanji *k = get_rkanji();
    return k->ch;
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::get_rwordmeaning(int ix)
{
    return lockedmeaning[testreadings->currentindex(ix)];
}
//---------------------------------------------------------------------------
int TWordStudyList::get_rwordindex(int ix)
{
    return lockedindex[testreadings->currentindex(ix)];
}
//---------------------------------------------------------------------------
byte TWordStudyList::get_rcharpos(int ix)
{
    return testreadings->currentkanjipos(ix);
}
//---------------------------------------------------------------------------
TWord* TWordStudyList::get_rword(int ix)
{
    return collection->words->Items[lockedindex[testreadings->currentindex(ix)]];
}
//---------------------------------------------------------------------------
bool TWordStudyList::get_rwordfailed(int ix)
{
    return testreadings->currentwordfailed(ix);
}
//---------------------------------------------------------------------------
bool TWordStudyList::get_rwordnew(int ix)
{
    return testreadings->currentwordnew(ix);
}
//---------------------------------------------------------------------------
TKanji* TWordStudyList::get_rkanji()
{
    return kanjis->Kanjis[testreadings->currentkanjiindex()];
}
//---------------------------------------------------------------------------
byte TWordStudyList::get_rreading()
{
    return testreadings->currentreading();
}
//---------------------------------------------------------------------------
bool TWordStudyList::get_hasundo()
{
    return category->hasundo() && (testlist->questionitem().worditem != undoitem);
}
//---------------------------------------------------------------------------
TRepetitionAnswer TWordStudyList::get_lastanswer()
{
    if (!hasundo)
        THROW(L"No undo data to return!");
    return category->undo_answer();
}
//---------------------------------------------------------------------------
double TWordStudyList::get_undogoodint()
{
    if (!hasundo)
        THROW(L"No undo data to return!");
    return category->undo_goodinterval();
}
//---------------------------------------------------------------------------
double TWordStudyList::get_undoeasyint()
{
    if (!hasundo)
        THROW(L"No undo data to return!");
    return category->undo_easyinterval();
}
//---------------------------------------------------------------------------
void TWordStudyList::undo(TRepetitionAnswer answer)
{
    if (!hasundo)
        THROW(L"No undo data to change!");

    TRepetitionAnswer oldanswer = lastanswer;

    if (oldanswer == raEasy || oldanswer == raGood)
    {
        int ix = dueindex(undoitem);
        if (ix < 0)
            THROW(L"Couldn't find test item in due list!");
        duelist->Delete(ix);
        if (answer == raHard || answer == raUncertain)
            failedlist->Add(undoitem);
    }
    else // Failed the last time.
    {
        if (answer == raEasy || answer == raGood)
        {
            int ix = failedlist->IndexOf(undoitem);
            if (ix < 0)
                THROW(L"Couldn't find test item in failed list!");
            failedlist->Delete(ix);
        }
    }

    if (undoitem->type == qtKanji || undoitem->type == qtKana && profilesettings.kkanahint == qtKanji || undoitem->type == qtMeaning && profilesettings.kmeaninghint == qtKanji)
        testreadings->undo(answer == raHard || answer == raUncertain);

    category->Undo(answer);

    if (answer == raEasy || answer == raGood) // Add item to its real place in the sorted duelist.
        duelist->Insert(duepos(undoitem->ritem->testdate + undoitem->ritem->interval), undoitem);

    testlist->undo(answer);

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordStudyList::TItemTree::TItemTree(TWordStudyList *aowner, TWordStudyItemType atype) : fowner(aowner), ftype(atype), inherited(false, false)
{
    list = new TTList;
}
//---------------------------------------------------------------------------
TWordStudyList::TItemTree::~TItemTree()
{
    delete list;
}
//---------------------------------------------------------------------------
void TWordStudyList::TItemTree::SaveToFile(FILE *f)
{
    inherited::SaveToFile(f);
    int c = list->Count;
    fwrite(&c, sizeof(int), 1, f);
    for (int ix = 0; ix < list->Count; ++ix)
    {
        TWordStudyItem *item = list->Items[ix];
        double d = item->added;
        fwrite(&d, sizeof(double), 1, f);
        fwrite(&item->type, sizeof(TQuestionType), 1, f);
        fwrite(&item->hinttype, sizeof(THintType), 1, f);
        int i = fowner->IndexOf(item->def);
        if (i < 0)
            THROW(L"?");
        fwrite(&i, sizeof(int), 1, f);

        if (ftype == witFree)
            fwrite(&((TFreeWordStudyItem*)item)->priority, sizeof(byte), 1, f);
    }

//    c = kanjiwords.TrueCount;
//    fwrite(&c, sizeof(int), 1, f);
//    for (unsigned int ix = 0; ix < kanjiwords.TrueCount; ++ix)
//    {
//        c = kanjiwords.TruePos[ix];
//        fwrite(&c, sizeof(int), 1, f);
//        TIntegerList *l = kanjiwords.TrueItems[ix];
//        c = l->Count;
//        fwrite(&c, sizeof(int), 1, f);
//        for (int iy = 0; iy < l->Count; ++iy)
//        {
//            c = l->Items[iy];
//            fwrite(&c, sizeof(int), 1, f);
//        }
//    }
}
//---------------------------------------------------------------------------
void TWordStudyList::TItemTree::LoadFromFile(FILE *f, int version)
{
    inherited::LoadFromFile(f);
    int c;
    fread(&c, sizeof(int), 1, f);
    list->Capacity = c;
    TWordStudyItem *item;
    for (int ix = 0; ix < list->Capacity; ++ix)
    {
        if (ftype == witFree)
            item = new TFreeWordStudyItem;
        else
            item = new TLockedWordStudyItem;
        list->Add(item);
        double d;
        fread(&d, sizeof(double), 1, f);
        item->added = d;
        fread(&item->type, sizeof(TQuestionType), 1, f);
        if (version >= 14) // Added 2011.05.19
            fread(&item->hinttype, sizeof(THintType), 1, f);

        int i;
        fread(&i, sizeof(int), 1, f);
        item->def = fowner->Items[i];

        if (ftype == witFree)
            fread(&((TFreeWordStudyItem*)item)->priority, sizeof(byte), 1, f);
    }

    if (version <= 13) // Added 2011.05.19
    {
        for (int ix = 0; ix < list->Count; ++ix)
        {
            item = list->Items[ix];
            item->hinttype = htDefault;
            if (fowner->wordindex(item->def) >= 0 && !fowner->hastype(item->def, qtKanji))
            {
                TWord *w = fowner->wordfromdef(item->def);
                if (w)
                {
                    bool kanaonly = true;
                    bool nokanji = true;
                    for (int ix = 0; ix < w->meaningcnt && kanaonly; ++ix)
                        kanaonly = (w->data[ix].notes & WN_KANAONLY) == WN_KANAONLY;
                    for (int ix = wcslen(fowner->wordkanji(item->def)) - 1; ix >= 0 && !kanaonly && nokanji; --ix)
                        nokanji = !KANJI(fowner->wordkanji(item->def)[ix]);

                    if (kanaonly || nokanji)
                        item->hinttype = item->type == qtKana ? htMeaning : htKana;
                }
            }
        }
    }

    if (ftype == witLocked && fowner->studyid)
    {
        TRepetitionListBase* tmplist = fowner->category->fetch_study(fowner->studyid);
        if (list->Count != tmplist->Count)
            THROW(L"Error in database! Bug!");
        for (int ix = 0; ix < tmplist->Count; ++ix)
        {
            tmplist->Items[ix]->pt = list->Items[ix];
            ((TLockedWordStudyItem*)list->Items[ix])->ritem = tmplist->Items[ix];
            if (((TLockedWordStudyItem*)list->Items[ix])->def->repgroup == NULL)
                ((TLockedWordStudyItem*)list->Items[ix])->def->repgroup = ((TLockedWordStudyItem*)list->Items[ix])->ritem;
            else if (version < 18) // Added 2013 May 24. Fixes a bug which caused elements of a single word to be added to different repetition groups.
                fowner->category->group_items(fowner->studyid, ((TLockedWordStudyItem*)list->Items[ix])->def->repgroup, ((TLockedWordStudyItem*)list->Items[ix])->ritem);
        }
    }

    if (version < 19) // Added 2013 07 01 WARNING: don't remove until v0.9!
    {
        int cnt;
        fread(&cnt, sizeof(int), 1, f);
        for (int ix = 0; ix < cnt; ++ix)
        {
            fseek(f, sizeof(int), SEEK_CUR);
            //fread(&c, sizeof(int), 1, f);
            //TIntegerList *l = new TIntegerList;
            //kanjiwords.Items[c] = l;

            fread(&c, sizeof(int), 1, f);
            //l->Capacity = c;
            for (int iy = 0; iy < c /*l->Capacity*/; ++iy)
            {
                fseek(f, sizeof(int), SEEK_CUR);
                //fread(&c, sizeof(int), 1, f);
                //l->Add(c);
            }
        }
    }
}
//---------------------------------------------------------------------------
int TWordStudyList::TItemTree::get_count()
{
    return list->Count;
}
//---------------------------------------------------------------------------
TWordStudyItem* TWordStudyList::TItemTree::get_item(int ix)
{
    return list->Items[ix];
}
//---------------------------------------------------------------------------
int TWordStudyList::TItemTree::Add(TWordStudyItem *item)
{
    Expand(Romanize(owner->wordkana(item->def)).c_str(), list->Count);

//    const wchar_t *k = owner->wordkanji(item->def);
//    int klen = wcslen(k);
//    UnicodeString z;
//    for (int ix = 0; ix < klen; ++ix)
//        if (KANJI(k[ix]) && z.Pos(k[ix]) == 0)
//            z += k[ix];
//
//    for (int ix = 1; ix <= z.Length(); ++ix)
//    {
//        int c = (int)z[ix];
//        TIntegerList *l = kanjiwords.Items[c];
//        if (!l)
//        {
//            l = new TIntegerList;
//            kanjiwords.Items[c] = l;
//        }
//        if (l->IndexOf(list->Count) < 0)
//            l->Add(list->Count);
//    }

    return list->Add(item);
}
//---------------------------------------------------------------------------
void TWordStudyList::TItemTree::Delete(int ix)
{
//    const wchar_t *k = owner->wordkanji(Items[ix]->def);
//
//    int klen = wordkanji.Length();
//    UnicodeString z;
//    for (int zix = 1; zix <= klen; ++zix)
//        if (KANJI(wordkanji[zix]) && z.Pos(wordkanji[zix]) == 0)
//            z += wordkanji[zix];

//    TIntegerList *l;
//    for (int j = 1; j <= z.Length(); ++j)
//    {
//        int c = (int)z[j];
//        l = kanjiwords.Items[c];
//
//        for (int iy = l->Count - 1; iy >= 0; --iy)
//            if (l->Items[iy] == ix)
//            {
//                l->Delete(iy);
//                break;
//            }
//
//        if (!l->Count)
//        {
//            delete l;
//            kanjiwords.ClearIndex(c);
//        }
//    }
//
//    for (unsigned int j = 0; j < kanjiwords.TrueCount; ++j)
//    {
//        l = kanjiwords.TrueItems[j];
//        if (!l || !l->Count)
//            THROW(L"Unknown L!");
//        for (int iy = l->Count - 1; iy >= 0; --iy)
//        {
//            if (l->Items[iy] > ix)
//                l->Items[iy]--;
//        }
//    }

    RemoveLine(ix, true);
    list->Delete(ix);
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::TItemTree::DoGetWord(int ix)
{
    return owner->wordromaji(list->Items[ix]->def);
}
//---------------------------------------------------------------------------
void TWordStudyList::TItemTree::FindWord(const wchar_t *c, TIntegerList *results)
{
    inherited::FindLines(c, false, results);
}
//---------------------------------------------------------------------------
void TWordStudyList::TItemTree::FindKanjiWord(const UnicodeString &c, TIntegerList *results)
{
    //wchar_t k = 0;
    //int mincnt = 99999;

    results->Clear();

    // Find kanji with the least number of words.
    int kd = -1;
    int minw = -1;
    for (int ix = c.Length(); ix > 0; --ix)
    {
        if (!KANJI(c[ix]))
            continue;
        int kix = kanjis->IndexOf(c[ix]);
        int wcnt = fowner->collection->kanjidat[kix].wordscnt;
        if (minw < 0 || wcnt < minw)
        {
            minw = wcnt;
            kd = kix;
        }
    }
    if (kd == -1)
        return;

    UnicodeString s = RemoveUseless(c);
    for (int ix = 0; ix < minw; ++ix)
    {
        for (int z = 0; z < list->Count; ++z)
        {
            if (list->Items[z]->def->fwordindex == fowner->collection->kanjidat[kd].words[ix] && wcsstr(fowner->wordkanji(list->Items[z]->def), s.c_str()))
                results->Add(z);
        }
    }

//    for (int ix = c.Length(); ix > 0; --ix)
//    {
//        if (KANJI(c[ix]) && kanjiwords.Items[c[ix]] && kanjiwords.Items[c[ix]]->Count < mincnt)
//        {
//            mincnt = kanjiwords.Items[c[ix]]->Count;
//            k = c[ix];
//            if (mincnt < 200)
//                break;
//        }
//    }
//
//    if (!k)
//        return;
//
//    UnicodeString s = RemoveUseless(c);
//
//    TIntegerList *l = kanjiwords.Items[k];
//    for (int ix = 0; ix < l->Count; ++ix)
//    {
//        if (wcsstr(fowner->wordkanji(Items[l->Items[ix]]->def), s.c_str()))
//            list->Add(l->Items[ix]);
//    }
}
//---------------------------------------------------------------------------
void TWordStudyList::TItemTree::Filter(const UnicodeString &str, TIntegerList *results)
{
    bool haskanji = false;
    for (int ix = str.Length(); ix > 0 && !haskanji; --ix)
        if (KANJI(str[ix]))
            haskanji = true;

    if (haskanji)
    {
        FindKanjiWord(str, results);
        return;
    }

    UnicodeString s = TrimRomanized(str.c_str());

    if (!s.Length())
        return;

    FindWord(s.c_str(), results);
    for (int ix = results->Count - 1; ix >= 0; --ix)
    {
        if (wcsncmp(fowner->wordromaji(Items[results->Items[ix]]->def), s.c_str(), s.Length()))
            results->Delete(ix);
    }
}
//---------------------------------------------------------------------------
int TWordStudyList::TItemTree::IndexOf(TWordStudyItem *item)
{
    TIntegerList *l = NULL;
    int pos = -1;
    try
    {
        l = new TIntegerList;
        Filter(fowner->wordkanji(item->def), l);
        for (int ix = 0; ix < l->Count && pos < 0; ++ix)
            if (list->Items[l->Items[ix]] == item)
                pos = l->Items[ix];
    }
    catch (...)
    {
        pos = -1;
    }
    delete l;
    return pos;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordStudyList::TWordStudyList(TWordCollection *acoll) : fcollection(acoll), fstudying(false), fstudyid(0), flasttest(0), changegap(NULL), flastinclude(-1)
{
    freelist = new TItemTree(this, witFree);
    lockedlist = new TItemTree(this, witLocked);
    duelist = new TLockedWordStudyItemList;
    failedlist = new TLockedWordStudyItemList;
    //newlist = new TFreeWordStudyItemList;
    category = fcollection->kcategory;
    testreadings = new TTestReadingList(this);

    testlist = NULL;
}
//---------------------------------------------------------------------------
TWordStudyList::~TWordStudyList()
{
    delete testlist;
    delete freelist;
    freelist = NULL;
    delete lockedlist;
    lockedlist = NULL;
    delete duelist;
    duelist = NULL;
    delete failedlist;
    failedlist = NULL;
    //delete newlist;
    //newlist = NULL;
    delete changegap;
    delete testreadings;
    testreadings = NULL;

    Clear();
}
//---------------------------------------------------------------------------
void TWordStudyList::SaveToFile(FILE *f)
{
    fwrite(&fstudyid, sizeof(unsigned int), 1, f);
    fwrite(&flasttest, sizeof(int), 1, f);
    fwrite(&flastinclude, sizeof(int), 1, f);

    int c = Count;
    fwrite(&c, sizeof(int), 1, f);
    for (int ix = 0; ix < Count; ++ix)
    {
        TWordStudyDefinition *item = Items[ix];
        fwrite(&item->fwordindex, sizeof(int), 1, f);
        fwrite(&item->ftakentype, sizeof(byte), 1, f);
        //fwrite(&item->fusage, sizeof(byte), 1, f);
        double d = (double)item->flastinclude;
        fwrite(&d, sizeof(double), 1, f);
        write(item->fmeaning, f, sltWord);
    }

    freelist->SaveToFile(f);
    lockedlist->SaveToFile(f);
    testreadings->SaveToFile(f);

    c = failedlist->Count;
    fwrite(&c, sizeof(int), 1, f);
    for (int ix = 0; ix < failedlist->Count; ++ix)
    {
        c = lockedlist->IndexOf(failedlist->Items[ix]);
        fwrite(&c, sizeof(int), 1, f);
    }

    fwrite(&newcount, sizeof(int), 1, f);

    /*c = newlist->Count;
     fwrite(&c,sizeof(int),1,f);
     for (int ix = 0; ix < newlist->Count; ++ix) {
     c = freelist->IndexOf(newlist->Items[ix]);
     fwrite(&c,sizeof(int),1,f);
     }
     */
}
//---------------------------------------------------------------------------
void TWordStudyList::LoadFromFile(TConvertItem *citem, int ccnt, FILE *f, int version)
{
    fread(&fstudyid, sizeof(unsigned int), 1, f);
    fread(&flasttest, sizeof(int), 1, f);
    fread(&flastinclude, sizeof(int), 1, f);

    int c, c2;
    try
    {
        if (version < 17)
            changegap = new TChangeGapList;
        fread(&c, sizeof(int), 1, f);
        Capacity = c;
        bool changed = false; // Used only below version 17.

        for (int ix = 0; ix < Capacity; ++ix)
        {
            TWordStudyDefinition *item = new TWordStudyDefinition;
            item->repgroup = NULL;
            inherited::Add(item);

            fread(&item->fwordindex, sizeof(int), 1, f);
            fread(&item->ftakentype, sizeof(byte), 1, f);
            if (version < 18)
            {
                byte btmp;
                fread(&btmp, sizeof(byte), 1, f);
            }
            double d;
            fread(&d, sizeof(double), 1, f);
            item->flastinclude = d;

            if (version >= 12) // Added 2011.04.04.
                read(item->fmeaning, f, sltWordAddNULL);
            else if (version == 11) // Added 2011.03.30.
            {
                read(item->fmeaning, f, sltWord);
                if (!item->fmeaning)
                {
                    item->fmeaning = new wchar_t[1];
                    item->fmeaning[0] = 0;
                }
            }
            else
            {
                fread(&c, sizeof(int), 1, f);
                item->fmeaning = new wchar_t[c + 1];
                if (c)
                {
                    char *tmp = new char[c];
                    fread(tmp, sizeof(char), c, f);
                    wcscpy(item->fmeaning, UnicodeString(tmp, c).c_str());
                    delete[] tmp;
                }
                else
                    item->fmeaning[c] = 0;
            }

            if (version < 17) // Added 2012.04 for v0.73.
            {
                int min = 0, max = ccnt - 1;
                while (min < max)
                {
                    int mid = (min + max) / 2;
                    if (citem[mid].original < item->fwordindex)
                        min = mid + 1;
                    else if (citem[mid].original > item->fwordindex)
                        max = mid - 1;
                    else
                        min = max = mid;
                }

                if (citem[min].original != item->fwordindex)
                    THROW(L"Missing data while loading long-term study definitions!");

                if (citem[min].changed < 0) // Missing.
                {
                    changed = true;
                    TChangeItem tmp;
                    tmp.kanji = citem[min].kanji;
                    tmp.kana = citem[min].kana;
                    tmp.index = ix;
                    changegap->Items[(int)item] = tmp;
                }

                item->fwordindex = citem[min].changed;
            }
        }
        if (version < 17 && !changed)
        {
            delete changegap;
            changegap = NULL;
        }
    }
    catch (...)
    {
        if (version < 17)
            delete changegap;
        changegap = NULL;
        throw;
    }

    freelist->LoadFromFile(f, version);
    lockedlist->LoadFromFile(f, version);
    if (version >= 10) // Added 2011.03.07 for v0.575.
        testreadings->LoadFromFile(f, version);

    for (int ix = 0; ix < lockedlist->Count; ++ix)
        duelist->Add((TLockedWordStudyItem*)lockedlist->Items[ix]);

    TIntegerList *tmplist = new TIntegerList;
    fread(&c, sizeof(int), 1, f);
    failedlist->Capacity = c;
    for (int ix = 0; ix < failedlist->Capacity; ++ix)
    {
        fread(&c, sizeof(int), 1, f);
        tmplist->Add(c);
        failedlist->Add((TLockedWordStudyItem*)lockedlist->Items[c]);
    }

    tmplist->Sort();

    for (int ix = tmplist->Count - 1; ix >= 0; --ix)
        duelist->Delete(tmplist->Items[ix]);

    duelist->Sort(wordstudy_duelist_sort_proc);

    delete tmplist;

    if (version >= 16)
    {
        fread(&newcount, sizeof(int), 1, f);
    }
    else
    {
        fread(&c, sizeof(int), 1, f);
        for (int ix = 0; ix < c; ++ix)
            fread(&c2, sizeof(int), 1, f);
    }

    if (changegap) // Not used after version 17.
    {
        for (int ix = lockedlist->Count - 1; ix >= 0; --ix)
            if (wordindex(lockedlist->Items[ix]->def) < 0)
                delete_locked(ix, true);
        for (int ix = freelist->Count - 1; ix >= 0; --ix)
            if (wordindex(freelist->Items[ix]->def) < 0)
                delete_free(ix, true);
        for (int ix = changegap->TrueCount - 1; ix >= 0; --ix)
            Delete(changegap->TrueItems[ix].index);

        delete changegap;
        changegap = NULL;
    }

    if (lockedlist->Count && category->lastdaystat(fstudyid) && category->lastdaystat(fstudyid)->itemcount != lockedlist->Count)
        category->fixdaystats(fstudyid);

    if (version < 13 && lockedlist->Count)
    {
        MsgBox((UnicodeString)L"In previous versions of zkanji, the intervals assigned to items in the long-term study list were too long. To fix this problem, zkanji will assign new intervals to every studied word. This might result in longer tests for the following few days.", L"Update message", MB_OK);
        category->fixintervals(fstudyid);
        duelist->Sort(wordstudy_duelist_sort_proc);
    }

    // Remove unused word definitions (Shouldn't do anything after v0.73 groupfile version 17).
    for (int ix = Count - 1; ix >= 0; --ix)
        if (Items[ix]->ftakentype == 0)
            Delete(ix);
}
//---------------------------------------------------------------------------
void TWordStudyList::RemoveLine(int windex)
{
    for (int ix = Count - 1; ix >= 0; --ix)
    {
        if (Items[ix]->fwordindex > windex)
            Items[ix]->fwordindex--;
        else if (Items[ix]->fwordindex == windex)
        {
            TWordStudyDefinition *def = Items[ix];
            for (int j = lockedlist->Count - 1; j >= 0; --j)
                if (lockedlist->Items[j]->def == def)
                    delete_locked(j, true);
            for (int j = freelist->Count - 1; j >= 0; --j)
                if (freelist->Items[j]->def == def)
                    delete_free(j, true);

            if (ix < Count && Items[ix] == def) // Probably never called if all locked and free items get deleted, but test for it anyway to avoid bugs later if I change this.
                Delete(ix);
        }
    }
}
//---------------------------------------------------------------------------
unsigned int TWordStudyList::get_studyid()
{
    if (!fstudyid && category)
        fstudyid = category->new_study();
    return fstudyid;
}
//---------------------------------------------------------------------------
void TWordStudyList::setdefinition(TWordStudyItem *item, TWordStudyDefinition *def, bool locked)
{
    if (def->ftakentype & (1 << item->type))
        THROW(L"Cannot set item for the given definition, because the test type is already occupied!");

    MODON

    def->ftakentype |= 1 << item->type;
    //++def->fusage;
    item->def = def;
    if (locked && !def->repgroup)
        def->repgroup = ((TLockedWordStudyItem*)item)->ritem;

    collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------
void TWordStudyList::unsetdefinition(TWordStudyItem *item, bool locked, bool deleteempty)
{
    if (!item->def)
        THROW(L"Bug! The item has no associated word definition!");

    if (!(item->def->ftakentype & (1 << item->type)))
        THROW(L"Cannot unset item for the given definition, because the test type wasn't set previously!");

    MODON
    item->def->ftakentype &= ~(1 << item->type);
    //--item->def->fusage;

    if (locked && item->def->repgroup && item->def->repgroup == ((TLockedWordStudyItem*)item)->ritem)
        if (item->def->repgroup->next != item->def->repgroup)
            item->def->repgroup = item->def->repgroup->next;
        else
            item->def->repgroup = NULL;

    // Remove word from list if it has no items associated with it.
    if (deleteempty && item->def->ftakentype == 0)
        Delete(IndexOf(item->def));

    collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------
int TWordStudyList::Add(int windex, const wchar_t *meaning)
{
    for (int ix = 0; ix < Count; ++ix)
        if (Items[ix]->fwordindex == windex)
            THROW(L"Cannot add same word twice!");

    int i = -1;
    MODON
    TWordStudyDefinition *item = new TWordStudyDefinition;
    //item->fusage = 0;
    item->ftakentype = 0;
    item->flastinclude = 0;
    item->repgroup = NULL;
    item->fwordindex = windex;
    item->fmeaning = new wchar_t[wcslen(meaning) + 1];
    wcscpy(item->fmeaning, meaning);

    i = inherited::Add(item);

    collection->Modify(false);
    MODOFF

    if (i >= 0)
        return i;

    THROW(L"Not returning anything here!");
}
//---------------------------------------------------------------------------
int TWordStudyList::additem(int defindex, TQuestionType type, bool haskanji)
{
    int i = -1;
    MODON
    TFreeWordStudyItem *item = new TFreeWordStudyItem;
    item->type = type;
    item->hinttype = htDefault;
    if (type != qtKanji && !haskanji)
    {
        TWord *w = wordfromdef(Items[defindex]);
        bool kanaonly = true;
        bool nokanji = true;
        for (int ix = 0; ix < w->meaningcnt && kanaonly; ++ix)
            kanaonly = (w->data[ix].notes & WN_KANAONLY) == WN_KANAONLY;
        for (int ix = wcslen(w->kanji) - 1; ix >= 0 && !kanaonly && nokanji; --ix)
            nokanji = !KANJI(w->kanji[ix]);
        if (kanaonly || nokanji)
            item->hinttype = type == qtKana ? htMeaning : htKana;
    }
    item->added = Now();
    item->priority = 5;
    setdefinition(item, Items[defindex], false);

    i = freelist->Add(item);

    collection->Modify(false);
    MODOFF
    if (i >= 0)
        return i;

    THROW(L"Not returning anything here!");
}
//---------------------------------------------------------------------------
void TWordStudyList::delete_free(int ix, bool deletedef)
{
    if (ix < 0 || ix >= freelist->Count)
        THROW(L"Index out of bounds in long-term word study list delete operation");

    MODON
    unsetdefinition(freelist->Items[ix], false, deletedef);
    freelist->Delete(ix);
    collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------
void TWordStudyList::real_delete_locked(int ix)
{
    MODON
    TLockedWordStudyItem *item = (TLockedWordStudyItem*)lockedlist->Items[ix];
    int i = dueindex(item);
    if (i >= 0)
        duelist->Delete(i);
    else // Should be in failed list then.
    {
        i = failedlist->IndexOf(item);
        if (i < 0)
            THROW(L"Item not found in duelist nor in failedlist! Database error!");
        failedlist->Delete(i);
    }

    category->remove_item(studyid, item);
    testreadings->remove_item(ix);
    lockedlist->Delete(ix);

    collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------
void TWordStudyList::delete_locked(int ix, bool deletedef)
{
    if (ix < 0 || ix >= lockedlist->Count)
        THROW(L"Index out of bounds in long-term word study list delete operation");

    MODON
    unsetdefinition(lockedlist->Items[ix], true, deletedef);
    real_delete_locked(ix);
    collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------
void TWordStudyList::queue_locked(int ix)
{
    if (ix < 0 || ix >= lockedlist->Count)
        THROW(L"Index out of bounds in long-term word study list queue operation");

    MODON
    TWordStudyItem *freeitem = new TFreeWordStudyItem;
    *freeitem = *lockedlist->Items[ix];
    ((TFreeWordStudyItem*)freeitem)->priority = 5;
    freelist->Add(freeitem);

    if (freeitem->def->repgroup == ((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem)
        if (((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem && ((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem->next != ((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem)
            freeitem->def->repgroup = ((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem->next;
        else
            freeitem->def->repgroup = NULL;

    real_delete_locked(ix);
    collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------
int TWordStudyList::get_freecount()
{
    return freelist->Count;
}
//---------------------------------------------------------------------------
int TWordStudyList::get_freeindex(int freeix)
{
    return wordindex(freelist->Items[freeix]->def);
}
//---------------------------------------------------------------------------
TQuestionType TWordStudyList::get_freetype(int freeix)
{
    return freelist->Items[freeix]->type;
}
//---------------------------------------------------------------------------
THintType TWordStudyList::get_freehint(int freeix)
{
    return freelist->Items[freeix]->hinttype;
}
//---------------------------------------------------------------------------
void TWordStudyList::set_freehint(int freeix, THintType val)
{
    freelist->Items[freeix]->hinttype = val;
}
//---------------------------------------------------------------------------
int TWordStudyList::get_lockedcount()
{
    return lockedlist->Count;
}
//---------------------------------------------------------------------------
int TWordStudyList::get_lockedindex(int lockix)
{
    return wordindex(lockedlist->Items[lockix]->def);
}
//---------------------------------------------------------------------------
TQuestionType TWordStudyList::get_lockedtype(int lockix)
{
    return lockedlist->Items[lockix]->type;
}
//---------------------------------------------------------------------------
THintType TWordStudyList::get_lockedhint(int lockedix)
{
    return lockedlist->Items[lockedix]->hinttype;
}
//---------------------------------------------------------------------------
void TWordStudyList::set_lockedhint(int lockedix, THintType val)
{
    lockedlist->Items[lockedix]->hinttype = val;
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::get_freekanji(int freeix)
{
    return wordkanji(freelist->Items[freeix]->def);
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::get_freekana(int freeix)
{
    return wordkana(freelist->Items[freeix]->def);
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::get_freemeaning(int freeix)
{
    return wordmeaning(freelist->Items[freeix]->def);
}
//---------------------------------------------------------------------------
void TWordStudyList::set_freemeaning(int freeix, UnicodeString val)
{
    changemeaning(freelist->Items[freeix]->def, val);
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::get_lockedkanji(int lockix)
{
    return wordkanji(lockedlist->Items[lockix]->def);
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::get_lockedkana(int lockix)
{
    return wordkana(lockedlist->Items[lockix]->def);
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::get_lockedmeaning(int lockix)
{
    return wordmeaning(lockedlist->Items[lockix]->def);
}
//---------------------------------------------------------------------------
void TWordStudyList::set_lockedmeaning(int lockix, UnicodeString val)
{
    changemeaning(lockedlist->Items[lockix]->def, val);
}
//---------------------------------------------------------------------------
TDateTime TWordStudyList::get_freeadded(int freeix)
{
    return freelist->Items[freeix]->added;
}
//---------------------------------------------------------------------------
byte TWordStudyList::get_freepriority(int freeix)
{
    return ((TFreeWordStudyItem*)freelist->Items[freeix])->priority;
}
//---------------------------------------------------------------------------
void TWordStudyList::set_freepriority(int freeix, byte val)
{
    if (val <= 0 || val > 9 || ((TFreeWordStudyItem*)freelist->Items[freeix])->priority == val)
        return;
    MODON
    ((TFreeWordStudyItem*)freelist->Items[freeix])->priority = val;
    collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::get_freedeftext(int freeix)
{
    return definition_text(freelist->Items[freeix]->def);
}
//---------------------------------------------------------------------------
TDateTime TWordStudyList::get_lockedadded(int lockix)
{
    return lockedlist->Items[lockix]->added;
}
//---------------------------------------------------------------------------
byte TWordStudyList::get_loldscore(int ix)
{
    return lockedstatcount[ix] <= 1 ? 0 : lockedstat[ix][lockedstatcount[ix] - 2].score;
}
//---------------------------------------------------------------------------
byte TWordStudyList::get_lscore(int ix)
{
    return (byte)((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem->score;
}
//---------------------------------------------------------------------------
byte TWordStudyList::get_lolddeck(int ix)
{
    return lockedstatcount[ix] <= 1 ? 0 : lockedstat[ix][lockedstatcount[ix] - 2].deck;
}
//---------------------------------------------------------------------------
byte TWordStudyList::get_ldeck(int ix)
{
    return (byte)((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem->deck;
}
//---------------------------------------------------------------------------
double TWordStudyList::get_deckinterval(int ix, byte deck)
{
    if (deck > 12)
        deck = 12;

    TLockedWordStudyItem *w = ((TLockedWordStudyItem*)lockedlist->Items[ix]);
    return category->simulate_setdeck(studyid, w->ritem, deck);
}
//---------------------------------------------------------------------------
void TWordStudyList::set_deckinterval(int ix, byte deck, double interval)
{
    if (deck > 12 || get_ldeck(ix) == deck)
        return;

    MODON
    TLockedWordStudyItem *w = ((TLockedWordStudyItem*)lockedlist->Items[ix]);

    int index;
    if ((index = failedlist->IndexOf(w)) < 0)
    {
        index = dueindex(w);
        if (index >= 0)
            duelist->Delete(index);
    }
    else
        failedlist->Delete(index);
    if (index < 0)
        THROW(L"Couldn't find item in list!");

    category->setdeck(studyid, w->ritem, deck, interval);
    duelist->Insert(duepos(w->ritem->testdate + w->ritem->interval), w);

    collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------
void TWordStudyList::resetitem(int ix) // Removes study stats for item and resets its last test day to today, giving it a single stat.
{
    MODON
    TLockedWordStudyItem *w = ((TLockedWordStudyItem*)lockedlist->Items[ix]);

    int index;
    if ((index = failedlist->IndexOf(w)) < 0)
    {
        index = dueindex(w);
        if (index >= 0)
            duelist->Delete(index);
    }
    else
        failedlist->Delete(index);
    if (index < 0)
        THROW(L"Couldn't find item in list!");

    category->resetitem(studyid, w->ritem);

    duelist->Insert(duepos(w->ritem->testdate + w->ritem->interval), w);

    collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------
word TWordStudyList::get_ltries(int ix)
{
    return ((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem->inclusion;
}
//---------------------------------------------------------------------------
TDateTime TWordStudyList::get_lfirst(int ix)
{
    return ((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem->stats->Items[0]->day;
}
//---------------------------------------------------------------------------
TDateTime TWordStudyList::get_llasttest(int ix)
{
    return ((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem->testdate;
}
//---------------------------------------------------------------------------
TDateTime TWordStudyList::get_llasttime(int ix)
{
    return ((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem->itemdate;
}
//---------------------------------------------------------------------------
int TWordStudyList::get_llastday(int ix)
{
    if (((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem->stats->Count == 0)
        return 0;
    return ((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem->stats->Items[((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem->stats->Count - 1]->day;
}
//---------------------------------------------------------------------------
const TRepAnswerCount& TWordStudyList::get_lanswers(int ix)
{
    return ((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem->answers;
}
//---------------------------------------------------------------------------
TDateTime TWordStudyList::get_lnext(int ix)
{
    TRepetitionItem *ri = ((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem;

    if ((ix = failedlist->IndexOf(((TLockedWordStudyItem*)lockedlist->Items[ix]))) < 0)
        return ri->testdate + ri->interval;

    return get_day(Now()) - 1;
}
//---------------------------------------------------------------------------
double TWordStudyList::get_linterval(int ix)
{
    TRepetitionItem *ri = ((TLockedWordStudyItem*)lockedlist->Items[ix])->ritem;

    if ((ix = failedlist->IndexOf(((TLockedWordStudyItem*)lockedlist->Items[ix]))) < 0)
        return ri->interval;
    else
        return -1;
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::get_lockeddeftext(int lockix)
{
    return definition_text(lockedlist->Items[lockix]->def);
}
//---------------------------------------------------------------------------
int TWordStudyList::get_lockedstatcount(int lockix)
{
    return lockedlist->Count <= lockix ? 0 : ((TLockedWordStudyItem*)lockedlist->Items[lockix])->ritem->stats->Count;
}
//---------------------------------------------------------------------------
const TRepStat& TWordStudyList::get_lockedstat(int lockix, int statix)
{
    return *((TLockedWordStudyItem*)lockedlist->Items[lockix])->ritem->stats->Items[statix];
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::generate_meaning(int windex)
{
    TWord *w = collection->words->Items[windex];
    UnicodeString str = "";

    for (int ix = 0; ix < w->meaningcnt; ++ix)
    {
        if (ix > 0)
            str += "; ";
        str += w->data[ix].meaning;
    }

    return str;
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::generate_types(int windex)
{
    TWord *w = collection->words->Items[windex];
    UnicodeString str = "";

    int types = 0;
    int notes = 0;
    int fields = 0;
    word nametag = 0;
    for (int ix = 0; ix < w->meaningcnt; ++ix)
    {
        types |= w->data[ix].types;
        notes |= w->data[ix].notes;
        fields |= w->data[ix].fields;
        nametag |= w->data[ix].nametag;
    }

    bool added = false;
    for (int ix = 0; ix < WT_COUNT; ++ix)
    {
        if (types & (1 << ix))
        {
            if (added)
                str += ", ";
            added = true;
            str += wlongtypetext[ix];
        }
    }

    if (added)
        str += "; ";

    added = false;
    for (int ix = 0; ix < WN_COUNT; ++ix)
    {
        if (notes & (1 << ix))
        {
            if (added)
                str += ", ";
            added = true;
            str += wlongnotetext[ix];
        }
    }

    if (added)
        str += "; ";

    added = false;
    for (int ix = 0; ix < WF_COUNT; ++ix)
    {
        if (fields & (1 << ix))
        {
            if (added)
                str += ", ";
            added = true;
            str += wlongfieldtext[ix];
        }
    }

    if (added)
        str += "; ";

    added = false;
    for (int ix = 0; ix < NM_COUNT; ++ix)
    {
        if (nametag & (1 << ix))
        {
            if (added)
                str += ", ";
            added = true;
            str += nlongtagtext[ix];
        }
    }

    return str;
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::definition_text(TWordStudyDefinition *def)
{
    UnicodeString str;
    if (def->ftakentype & (1 << qtKanji))
        str += "[Written] ";
    if (def->ftakentype & (1 << qtKana))
        str += "[Kana] ";
    if (def->ftakentype & (1 << qtMeaning))
        str += "[Meaning] ";

    return str.Trim();
}
//---------------------------------------------------------------------------
void TWordStudyList::filter_free(UnicodeString str, TIntegerList *l)
{
    freelist->Filter(str, l);
}
//---------------------------------------------------------------------------
void TWordStudyList::filter_locked(UnicodeString str, TIntegerList *l)
{
    lockedlist->Filter(str, l);
}
//---------------------------------------------------------------------------
int TWordStudyList::duepos(TDateTime due)
{
    int min = 0, max = duelist->Count - 1;
    int mid;
    while (min < max)
    {
        mid = (min + max) / 2;
        TDateTime next = duelist->Items[mid]->ritem->testdate + duelist->Items[mid]->ritem->interval;
        if (next > due)
            max = mid - 1;
        else if (next < due)
            min = mid + 1;
        else
            min = max = mid;
    }

    while (min < duelist->Count && duelist->Items[min]->ritem->testdate + duelist->Items[min]->ritem->interval <= due)
        ++min;

    return min;
}

//---------------------------------------------------------------------------
int TWordStudyList::dueindex(TLockedWordStudyItem *item)
{
    double t = item->ritem->testdate + item->ritem->interval;
    int ix = min(duepos(t), duelist->Count - 1);

    if (ix < 0)
        return -1;

    while (ix >= 0 && duelist->Items[ix] != item && (double)duelist->Items[ix]->ritem->testdate + duelist->Items[ix]->ritem->interval + 0.01 >= t)
        --ix;

    if (ix < 0 || duelist->Items[ix] != item)
        return -1;

    return ix;
}
//---------------------------------------------------------------------------
double next_day(TDateTime today)
{
    double h = (1.0 / 24.0) * profilesettings.daystart;
    return (get_day(today) + 1.0001 + h);
}
//---------------------------------------------------------------------------
int TWordStudyList::itemsdue()
{
    double n = Now();
    if (flasttest != get_day(n))
        daychanged();

    return duepos(next_day(n)) + failedlist->Count/*+newlist->Count*/;
}
//---------------------------------------------------------------------------
void TWordStudyList::readingstest()
{
    testreadings->test();
}
//---------------------------------------------------------------------------
TModalResult TWordStudyList::begintest()
{
    if (fstudying)
        THROW(L"We are already in the middle of a test!");

    undoitem = NULL;

    int n = get_day(Now());
    if (flasttest < get_day(Now() -int(profilesettings.includeinterval)))
    {
        if (!freelist->Count)
        {
            if (!itemsdue())
            {
                MsgBox("There are no items scheduled for today.", "Message", MB_OK);
                return mrCancel;
            }

            if (MsgBox("There are no new items to include in today's test. Would you like to do the test anyway?", "Query", MB_YESNO) == IDNO)
                return mrCancel;
        }

        if (flastinclude < 0)
            flastinclude = profilesettings.kiteminclude == 0 ? 20 : profilesettings.kiteminclude;
        if (freelist->Count && profilesettings.kiteminclude == 0)
        {
            TGapList<bool> uni;
            int c = 0;
            for (int ix = 0; ix < freelist->Count; ++ix)
            {
                if (!uni.ItemSet(freelist->Items[ix]->def->fwordindex))
                {
                    c++;
                    uni.Items[freelist->Items[ix]->def->fwordindex] = true;
                }
            }
            if (!InputQueryInt("Select the number of new items to test today. Unique words: " + IntToStr(c), "Query", flastinclude, 0, min(999, freelist->Count)))
                return mrCancel;
        }

        if (profilesettings.kiteminclude > 0)
            newcount = min(freelist->Count, profilesettings.kiteminclude);
        else
            newcount = min(freelist->Count, flastinclude);
    }

    if (flasttest != n)
        testreadings->clear();

    if (!itemsdue() && !newcount)
    {
        if (testreadings->firstdue() >= 0)
        {
            if (MsgBox("Would you like to study kanji readings from words tested earlier today?", "Query", MB_YESNO) == IDYES)
                return mrYes;
        }
        else
            MsgBox("There are no items scheduled for today.", "Message", MB_OK);
        return mrCancel;
    }

    flasttest = n;
    fstudying = true;
    try
    {
        generate_testitems();
        category->BeginTest(studyid);
    }
    catch (...)
    {
        fstudying = false;
        throw;
    }
    return mrOk;
}
//---------------------------------------------------------------------------
void TWordStudyList::finishtest()
{
    if (!fstudying)
        THROW(L"We are not in a test!");

    delete testlist;
    testlist = NULL;

    if (testreadings->firstdue() >= 0)
    {
        if (MsgBox("Would you like to study kanji readings from the tested words?", "Query", MB_YESNO) == IDYES)
            readingstest();
    }

    fstudying = false;

    if (flasttest != get_day(Now()))
        daychanged();
}
//---------------------------------------------------------------------------
/*generate_testitems:
 *   Creates a list of items to be tested in the next test of the day. Only works when there are items
 *   due that day.
 *
 * The items list is filled with indexes pointing to failedlist and duelist items. When the added elem of
 * items is negative (X), the elem is taken from the failedlist at the index (-1*X)+1.
 * Positive elems are taken from duelist.
 *
 * (Both the failedlist and duelist point to elements in lockedlist)
 *
 * The newlist is filled with indexes pointing to freelist elements. The freelist contains indexes to
 * those items that are not yet moved to the lockedlist.
 */

//TEMPORARY STRUCTURES:
// * TWordStudyNewItem - holds newly included items, or those that could be included because they are new too
//  usage - number of new items of the same word that could be included in the test
//  needed - number of new items of the same word that must be included in the test
struct TWordStudyNewItem
{
    byte usage;
    byte needed;
    TWordStudyItem *item[3];
};

void TWordStudyList::generate_testitems()
{
    TGapList<int> added(false);

    double n = Now();
    int nd = get_day(n);
    bool showing = false;

    TIntegerList *itmp = NULL;
    try
    {
        int limit = itemsdue();

        if (testlist)
            testlist->Clear();
        else
            testlist = new TTestElemList(this);

        newcount = min(newcount, freelist->Count);

        for (int ix = 0; ix < failedlist->Count; ++ix)
        {
            int li = wordindex(failedlist->Items[ix]->def);
            testlist->addelem(tifFailed, failedlist->Items[ix], !failedlist->Items[ix]->ritem->stats->Count || failedlist->Items[ix]->ritem->stats->Items[failedlist->Items[ix]->ritem->stats->Count - 1]->day < nd);
            added.Items[li]++;

            if (!showing && (double)Now() - n > 0.3 / 86400.0 && ix < (limit + newcount) / 3)
            {
                ShowMessageForm("Please wait...");
                showing = true;
            }

        }

        for (int ix = 0; ix < limit - failedlist->Count; ++ix)
        {
            int li = wordindex(duelist->Items[ix]->def);
            testlist->addelem(tifDue, duelist->Items[ix], true);
            added.Items[li]++;

            if (!showing && (double)Now() - n > 0.3 / 86400.0 && ix + failedlist->Count < (limit + newcount) / 3)
            {
                ShowMessageForm("Please wait...");
                showing = true;
            }
        }

        if (newcount)
        {
            itmp = new TIntegerList;
            for (int ix = 0; ix < freelist->Count; ix++)
                itmp->Add(ix);

            wordstudy_sort_list = this;
            itmp->Sort(wordstudy_priority_sort_proc);

            TGapList<TWordStudyNewItem> newadded;

            int round = 0;
            int addedcnt = 0;
            while (addedcnt < newcount)
            {
                for (int ix = itmp->Count - 1; ix >= 0; --ix)
                {
                    if (!showing && (double)Now() - n > 0.3 / 86400.0 && (int)addedcnt < newcount / 3)
                    {
                        ShowMessageForm("Please wait...");
                        showing = true;
                    }

                    int fi = freeindex[itmp->Items[ix]];

                    if (!newadded.ItemSet(fi))
                    {
                        TWordStudyNewItem item;
                        memset(&item, 0, sizeof(TWordStudyNewItem));
                        newadded.Items[fi] = item;
                    }

                    if (round == 0)
                    {
                        newadded.Items[fi].item[newadded.Items[fi].usage] = freelist->Items[itmp->Items[ix]];
                        newadded.Items[fi].usage++;
                    }

                    if (added.Items[fi] == round && addedcnt < newcount)
                    {
                        added.Items[fi]++;
                        newadded.Items[fi].needed++;
                        addedcnt++;
                        itmp->Delete(ix);
                    }

                }
                round++;
            }

            for (unsigned int ix = 0; ix < newadded.TrueCount; ++ix)
            {
                if (!newadded.TrueItems[ix].needed)
                    continue;

                if (newadded.TrueItems[ix].needed < newadded.TrueItems[ix].usage)
                {
                    for (byte iy = 0; iy < newadded.TrueItems[ix].needed; ++iy)
                        swap(newadded.TrueItems[ix].item[iy], newadded.TrueItems[ix].item[Random(newadded.TrueItems[ix].usage - iy) + iy]);
                }

                for (byte iy = 0; iy < newadded.TrueItems[ix].needed; ++iy)
                    testlist->addelem(tifNew, newadded.TrueItems[ix].item[iy], true);
            }

        }
    }
    catch (...)
    {
        delete testlist;
        testlist = NULL;
        delete itmp;

        if (showing)
            HideMessageForm();

        throw;
    }

    if (showing)
        HideMessageForm();

    delete itmp;
}
//---------------------------------------------------------------------------
void TWordStudyList::startround()
{
    testlist->startround();
    itemstart = Now();
}
//---------------------------------------------------------------------------
void TWordStudyList::suspenditem()
{
    itemstart = (double)Now() - (double)itemstart;
}
//---------------------------------------------------------------------------
void TWordStudyList::resumeitem()
{
    itemstart = double(Now()) -double(itemstart);
}
//---------------------------------------------------------------------------
int TWordStudyList::get_testdue()
{
    return testlist->ItemCount;
}
//---------------------------------------------------------------------------
int TWordStudyList::get_faileddue()
{
    return testlist->FailedCount;
}
//---------------------------------------------------------------------------
int TWordStudyList::get_testnew()
{
    return testlist->NewCount;
}
//---------------------------------------------------------------------------
TQuestionType TWordStudyList::testquestion()
{
    return testlist->questionword()->type;
}
//---------------------------------------------------------------------------
TQuestionType TWordStudyList::testanswer()
{
    THintType ht = testlist->questionword()->hinttype;
    if (ht == htDefault)
    {
        TQuestionType qt = testlist->questionword()->type;
        ht = (THintType)(qt == qtKanji ? profilesettings.kkanjihint : qt == qtKana ? profilesettings.kkanahint : profilesettings.kmeaninghint);
    }

    return (TQuestionType)ht;
}
//---------------------------------------------------------------------------
bool TWordStudyList::newquestion()
{
    return testlist->questionitem().from == tifNew;
}
//---------------------------------------------------------------------------
bool TWordStudyList::failedquestion()
{
    return testlist->questionitem().from == tifFailed;
}
//---------------------------------------------------------------------------
bool TWordStudyList::firstquestion()
{
    return testlist->questionitem().untested;
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::testwordtype()
{
    return generate_types(testlist->questionword()->def->fwordindex);
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::get_testkanji()
{
    return wordkanji(testlist->questionword()->def);
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::get_testkana()
{
    return wordkana(testlist->questionword()->def);
}
//---------------------------------------------------------------------------
UnicodeString TWordStudyList::get_testmeaning()
{
    return wordmeaning(testlist->questionword()->def);
}
//---------------------------------------------------------------------------
int TWordStudyList::get_testwordindex()
{
    return testlist->questionword()->def->fwordindex;
}
//---------------------------------------------------------------------------
bool TWordStudyList::get_testalladded()
{
    if (testlist->questionword()->def->ftakentype == 0x07)
        return true;

    UnicodeString s = testkanji;
    for (int ix = s.Length(); ix >= 1; --ix)
    {
        if (KANJI(s[ix]))
            return false;
    }
    byte types = testlist->questionword()->def->ftakentype;
    return types == 0x03 || types > 0x04; // Any two types are taken.
}
//---------------------------------------------------------------------------
double TWordStudyList::testinterval(TRepetitionAnswer answer)
{
    double r = 0;
    category->SimulateStep(testlist->repetitionitem(), answer, firstquestion()/*testlist->questionfrom() != tifFailed*/, r);
    return r;
}
//---------------------------------------------------------------------------
double TWordStudyList::testestimate()
{
    return testlist->testestimate();
}
//---------------------------------------------------------------------------
void TWordStudyList::answertest(TRepetitionAnswer answer)
{
    MODON

    TTestIndexFrom from = testlist->questionfrom();
    TLockedWordStudyItem *w = NULL;
    int lockedindex = 0;
    if (from == tifNew) // Create new object in locked list and remove old one from freelist.
    {
        TWordStudyItem *olditem = testlist->questionword();
        int ix = freelist->IndexOf(olditem);
        if (ix < 0)
            THROW(L"Couldn't find test item in free list!");

        TDateTime added = olditem->added;
        TWordStudyDefinition *def = olditem->def;
        TQuestionType type = olditem->type;
        THintType htype = olditem->hinttype;

        delete_free(ix, false);

        w = new TLockedWordStudyItem;
        w->type = type;
        w->hinttype = htype;
        w->added = added;
        w->ritem = category->add_item(studyid, w, def->repgroup);
        setdefinition(w, def, true);
        if (def->repgroup == NULL)
            def->repgroup = w->ritem;
        lockedindex = lockedlist->Add(w);

        undoitem = w;

        if (answer == raHard || answer == raUncertain)
            failedlist->Add(w);

        newcount--;
    }
    else if (from == tifDue)
    {
        w = (TLockedWordStudyItem*)testlist->questionword();
        lockedindex = lockedlist->IndexOf(w);

        undoitem = w;

        int ix = dueindex(w);
        if (ix < 0)
            THROW(L"Couldn't find test item in due list!");

        duelist->Delete(ix);
        if (answer == raHard || answer == raUncertain)
            failedlist->Add(w);
    }
    else if (from == tifFailed)
    {
        w = (TLockedWordStudyItem*)testlist->questionword();
        lockedindex = lockedlist->IndexOf(w);

        undoitem = w;

        if (answer == raEasy || answer == raGood)
        {
            int ix = failedlist->IndexOf(w);
            if (ix < 0)
                THROW(L"Couldn't find test item in locked list!");
            failedlist->Delete(ix);
        }
    }

    if (w->type == qtKanji || w->type == qtKana && profilesettings.kkanahint == qtKanji || w->type == qtMeaning && profilesettings.kmeaninghint == qtKanji)
        testreadings->add_item(lockedindex, from == tifNew, answer == raHard || answer == raUncertain);
    else
        testreadings->resetundo();

    double timeused = (double)Now() - (double)itemstart;
    category->Step(w->ritem, answer, firstquestion(), timeused);

    if (answer == raEasy || answer == raGood) // Add item to its real place in the sorted duelist.
        duelist->Insert(duepos(w->ritem->testdate + w->ritem->interval), w);

    testlist->answer(answer, w);
    w->def->flastinclude = Now();

    collection->Modify(false);
    dictionary->ProfileModify();
    MODOFF
}
//---------------------------------------------------------------------------
TWordStudyList::TTestElemList::TTestElemList(TWordStudyList *aowner) : fowner(aowner), fitemcount(0), testitemindex(0), ffailedcount(0), fstep(0), inherited()
{
    dummy = new TRepetitionItem;
    memset(dummy, 0, sizeof(TRepetitionItem));
    dummy->stats = new TRepStats;
    newitems = new TTestedItemIndexList;
    memset(&undoitem, 0, sizeof(TTestedUndoItem));
}
//---------------------------------------------------------------------------
TWordStudyList::TTestElemList::~TTestElemList()
{
    Clear();

    delete newitems;
    newitems = NULL;
    delete dummy->stats;
    delete dummy;
    dummy = NULL;
}
//---------------------------------------------------------------------------
void TWordStudyList::TTestElemList::Clear()
{
    if (!newitems)
        return;

    newitems->Clear();
    fitemcount = 0;
    ffailedcount = 0;
    fstep = 0;

    inherited::Clear();
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestElemList::addelem(TTestIndexFrom from, TWordStudyItem *sitem, bool untested)
{
    int i = indexofelem(sitem);

    if (from == tifFailed && !untested)
        ffailedcount++;

    if (i < 0)
    {
        TTestElem *elem = createelem(from, sitem, untested);
        fitemcount++;
        if (from == tifNew)
        {
            TTestedItemIndex *tmp = new TTestedItemIndex;
            tmp->elem = elem;
            tmp->item = 0;
            newitems->Add(tmp);
        }
        return insertelem(elem);
    }
    if (Items[i]->itemcount == 3)
        THROW(L"Cannot add a fourth item for the given word!");

    fitemcount++;
    Items[i]->item[Items[i]->itemcount].from = from;
    Items[i]->item[Items[i]->itemcount].worditem = sitem;
    Items[i]->item[Items[i]->itemcount].untested = untested;
    if (from == tifNew)
    {
        TTestedItemIndex *tmp = new TTestedItemIndex;
        tmp->elem = Items[i];
        tmp->item = Items[i]->itemcount;
        newitems->Add(tmp);
    }
    Items[i]->itemcount++;
    return i;
}
//---------------------------------------------------------------------------
TWordStudyList::TTestElem* TWordStudyList::TTestElemList::createelem(TTestIndexFrom from, TWordStudyItem *sitem, bool untested)
{
    TTestElem *elem = new TTestElem;
    memset(elem->item, 0, sizeof(TTestedItem)*3);
    elem->testindex = 0;
    elem->itemcount = 1;
    elem->item[0].from = from;
    elem->item[0].worditem = sitem;
    elem->item[0].untested = untested;
    return elem;
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestElemList::listpos(int windex)
{
    int min = 0, max = Count - 1;
    while (min < max)
    {
        int mid = (min + max) / 2;
        if (wordindex(Items[mid]) < windex)
            min = mid + 1;
        else if (wordindex(Items[mid]) > windex)
            max = mid - 1;
        else
            min = max = mid;
    }
    return min;
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestElemList::get_newcount()
{
    return newitems->Count;
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestElemList::indexofelem(TWordStudyItem *sitem)
{
    int windex = wordindex(sitem);
    int ix = listpos(windex);
    if (ix >= Count || wordindex(Items[ix]) != windex)
        return -1;
    return ix;
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestElemList::insertelem(TTestElem* elem)
{
    int ix = listpos(wordindex(elem));
    inherited::Insert(ix, elem);
    return ix;
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestElemList::wordindex(TTestElem *elem)
{
    if (!elem->itemcount)
        return -1;
    return wordindex(elem->item[0].worditem);
}
//---------------------------------------------------------------------------
int TWordStudyList::TTestElemList::wordindex(TWordStudyItem *sitem)
{
    return fowner->wordindex(sitem->def);
}
//---------------------------------------------------------------------------
void TWordStudyList::TTestElemList::startround()
{
    const int repetitionlength = profilesettings.krepetition == kriShort ? 5 : profilesettings.krepetition == kriMedium ? 10 : 20;
    const double elem_averagetime = fowner->category->singletimeestimate(fowner->studyid, 0);
    const double needed_itemtime = repetitionlength * OneMinute; // Number of minutes before a new item from the same word shouldn't be tested.
    const int needed_itemsize = needed_itemtime / elem_averagetime; // Number of items that must be skipped before the same word's item can be asked.

    MODON

    bool testnew = newitems->Count && (profilesettings.knewfirst || newitems->Count == fitemcount || Random((double)min(fitemcount, newitems->Count * 7) / newitems->Count) == 0);

    double n = Now();
    for (int ix = 0; ix < Count; ++ix)
    {
        for (int j = 0; j < Items[ix]->itemcount; ++j)
        {
            if (Items[ix]->item[j].from == tifNew && !testnew || profilesettings.knewfirst && testnew && Items[ix]->item[j].from != tifNew)
            {
                Items[ix]->item[j].score = 999999;
                continue;
            }

            double mul = 0;
            if (Items[ix]->item[j].from == tifDue)
            {
                TRepetitionItem *ri = ((TLockedWordStudyItem*)Items[ix]->item[j].worditem)->ritem;
                mul = (Log2(max(1.0, ri->interval)) + Log2(max(1.0, n - (double)(ri->testdate + ri->interval)))) / 10.0;
            }

            double base = Items[ix]->itemcount * max(1.0, needed_itemsize - mul);
            int space = fitemcount - Items[ix]->itemcount;

            if (Items[ix]->testindex)
                space += max(0, needed_itemsize - (fstep - Items[ix]->testindex));

            double winclude = fowner->wordinclude(Items[ix]->item[j].worditem->def);
            if (space < base && Items[ix]->testindex && fstep - Items[ix]->testindex < needed_itemsize && n - winclude < needed_itemtime)
            {
                space += fstep - Items[ix]->testindex;
                base = (Items[ix]->itemcount + 1) * max(1.0, needed_itemsize - mul);
            }
            else if (Items[ix]->item[j].from == tifDue && n - winclude < needed_itemtime)
            {
                base *= 0.1;
            }

            if (Items[ix]->item[j].from == tifFailed)
            {
                if (Items[ix]->testindex && fstep - Items[ix]->testindex < max(1.0, needed_itemsize - mul) && n - winclude < needed_itemtime)
                {
                    base *= 0.08;
                    if (space <= Items[ix]->itemcount)
                        space += 100000;
                    if (fstep - Items[ix]->testindex <= 1)
                        space += Random(10000) + 5000;

                    space += Random((fstep - Items[ix]->testindex) * 2000);
                }
                else
                    base *= Random(3) + 2;
            }

            if (Items[ix]->item[j].from != tifNew) // Count score of item. the lower the sooner it is tested.
                        Items[ix]->item[j].score = space / base;
            else // Count score of new items. make their score high so they are always the last ones, because they should be randomly added to the test and not by their point.
            {
                if ((double)winclude)
                    Items[ix]->item[j].score = 10000 + max(0, fstep - Items[ix]->testindex) + (n - winclude) / elem_averagetime;
                else
                    Items[ix]->item[j].score = 10000 + max(0, fstep - Items[ix]->testindex) + Random(needed_itemsize * 2);
            }
        }
    }

    if (testnew)
    {
        double newscoremax = 0.0;
        int newitemindex = -1;
        for (int ix = 0; ix < newitems->Count; ++ix)
        {
            TTestedItem *im = &newitems->Items[ix]->elem->item[newitems->Items[ix]->item];
            if (im->from != tifNew || im->score == 0)
            {
                bool found = false;
                for (int iy = 0; iy < Count && !found; ++iy)
                    for (int j = 0; j < Items[iy]->itemcount && !found; ++j)
                    {
                        if (Items[iy]->item[j].worditem == im->worditem)
                            found = true;
                    }
                if (!found)
                    THROW(L"!@!@!@!");
            }
            if (im->score > newscoremax)
            {
                newscoremax = im->score;
                newitemindex = ix;
            }
        }
        if (newitemindex < 0)
            THROW(L"??");
        newitems->Items[newitemindex]->elem->item[newitems->Items[newitemindex]->item].score = 0;
    }

    Sort(wordstudy_testlist_sort_proc);

    testitemindex = questionitemindex();
    fstep++;

    fowner->collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------
byte TWordStudyList::TTestElemList::questionitemindex()
{
    if (!fitemcount)
        THROW(L"No items in test list!");
    TTestElem *elem = Items[0];
    double score = 1999999;
    byte which = 255;
    for (int j = 0; j < elem->itemcount; ++j)
    {
        if (elem->item[j].score < score)
        {
            score = elem->item[j].score;
            which = j;
        }
    }
    return which;
}
//---------------------------------------------------------------------------
TWordStudyList::TTestedItem& TWordStudyList::TTestElemList::questionitem()
{
    return Items[0]->item[questionitemindex()];
}
//---------------------------------------------------------------------------
TWordStudyItem* TWordStudyList::TTestElemList::questionword()
{
    return questionitem().worditem;
}
//---------------------------------------------------------------------------
TRepetitionItem* TWordStudyList::TTestElemList::repetitionitem()
{
    if (!fitemcount)
        return NULL;
    TTestedItem &item = questionitem();

    if (item.from == tifNew)
    {
        dummy->testdate = Now();
        dummy->interval = good_interval(dummy->testdate, 0);

        dummy->score = 0;
        dummy->deck = 0;
        dummy->inclusion = 0;
        return dummy;
    }
    else
        return ((TLockedWordStudyItem*)item.worditem)->ritem;
}
//---------------------------------------------------------------------------
TWordStudyList::TTestIndexFrom TWordStudyList::TTestElemList::questionfrom()
{
    return questionitem().from;
}
//---------------------------------------------------------------------------
double TWordStudyList::TTestElemList::testestimate()
{
    double r = 0;
    for (int ix = 0; ix < Count; ++ix)
    {
        for (int j = 0; j < Items[ix]->itemcount; j++)
        {
            TTestedItem &item = Items[ix]->item[j];
            if (item.from == tifNew)
                r += fowner->category->timeestimatenew(fowner->studyid);
            else
                r += fowner->category->timeestimate(fowner->studyid, ((TLockedWordStudyItem*)item.worditem)->ritem);
        }
    }
    return r;
}
//---------------------------------------------------------------------------
void TWordStudyList::TTestElemList::answer(TRepetitionAnswer answer, TWordStudyItem *w)
{
    TTestedItem &item = questionitem();

    if (item.from == tifNew)
    {
        bool found = false;

        item.worditem = w;
        for (int ix = 0; ix < newitems->Count && !found; ++ix)
        {
            if (newitems->Items[ix]->elem->item[newitems->Items[ix]->item].worditem == item.worditem)
            {
                found = true;
                newitems->Delete(ix);
            }
        }
        if (!found)
            THROW(L"This item should be in the new items list but it is not! Bug!");
    }
    else if (item.worditem != w)
        THROW(L"Presumably tested item and fetched item does not match!");
    Items[0]->testindex = fstep;

    item.untested = false;

    undoitem.answer = answer;
    undoitem.elem = Items[0];
    memcpy(&undoitem.item, &item, sizeof(TTestedItem));

    if (answer == raUncertain || answer == raHard)
    {
        item.from = tifFailed;
        undoitem.index = questionitemindex();
    }
    else
    {
        if (Items[0]->itemcount > 1)
        {
            int qindex = questionitemindex();
            if (qindex < 2)
                memmove(Items[0]->item + qindex, Items[0]->item + qindex + 1, sizeof(TTestedItem)*(2 - qindex));
            Items[0]->itemcount--;
            memset(Items[0]->item + Items[0]->itemcount, 0, sizeof(TTestedItem));

            byte changeditem = 0;
            for (int ix = 0; ix < newitems->Count && changeditem < Items[0]->itemcount - qindex; ++ix)
            {
                if (newitems->Items[ix]->elem == Items[0] && newitems->Items[ix]->item > qindex)
                {
                    newitems->Items[ix]->item--;
                    changeditem++;
                }
            }
        }
        else
        {
            Delete(0);
            undoitem.elem = NULL;
        }
        undoitem.index = -1;

        fitemcount--;
    }
}
//---------------------------------------------------------------------------
void TWordStudyList::TTestElemList::undo(TRepetitionAnswer newanswer)
{

    if (newanswer == raUncertain || newanswer == raHard)
    {
        if (undoitem.answer == raUncertain || undoitem.answer == raHard)
        {
            undoitem.answer = newanswer;
            //undoitem.elem->item[undoitem.index].untested = undoitem.item.untested && newanswer == raUncertain;
        }
        else
        {
            if (undoitem.elem)
            {
                if (undoitem.elem->itemcount == 3)
                    THROW(L"Cannot add a fourth item for the given word! Bug in undo!");
                memcpy(&undoitem.elem->item[undoitem.elem->itemcount], &undoitem.item, sizeof(TTestedItem));
                undoitem.elem->item[undoitem.elem->itemcount].from = tifFailed;
                //undoitem.elem->item[undoitem.elem->itemcount].untested = false; undoitem.item.untested && newanswer == raUncertain;
                undoitem.index = undoitem.elem->itemcount;
                undoitem.elem->itemcount++;
            }
            else
            {
                TTestElem *elem = createelem(tifFailed, undoitem.item.worditem, false/*undoitem.item.untested && newanswer == raUncertain*/);
                elem->testindex = fstep - 1;
                Add(elem);
                undoitem.elem = elem;
                undoitem.index = 0;
            }
            fitemcount++;
        }
    }
    else if (undoitem.answer == raUncertain || undoitem.answer == raHard)
    {
        if (undoitem.elem->itemcount > 1)
        {
            int qindex = undoitem.index;
            if (qindex < 2)
                memmove(undoitem.elem->item + qindex, undoitem.elem->item + qindex + 1, sizeof(TTestedItem)*(2 - qindex));
            undoitem.elem->itemcount--;
            memset(undoitem.elem->item + undoitem.elem->itemcount, 0, sizeof(TTestedItem));

            byte changeditem = 0;
            for (int ix = 0; ix < newitems->Count && changeditem < undoitem.elem->itemcount - qindex; ++ix)
            {
                if (newitems->Items[ix]->elem == undoitem.elem && newitems->Items[ix]->item > qindex)
                {
                    newitems->Items[ix]->item--;
                    changeditem++;
                }
            }
        }
        else
        {
            Delete(IndexOf(undoitem.elem));
            undoitem.elem = NULL;
        }
        undoitem.index = -1;
        fitemcount--;
    }
    else
        ; // Nothing to do in case the original answer and the new answer would similarly remove the same item

    undoitem.answer = newanswer;
}
//---------------------------------------------------------------------------
int TWordStudyList::wordcount()
{
    int c = category->daystatcount(studyid);
    if (!c)
        return 0;

    const TDayStat &stat = category->daystat(studyid, c - 1);

    return stat.groupcount;
}
//---------------------------------------------------------------------------
int TWordStudyList::kanjicount()
{
    TGapList<bool> kanjis(false);
    for (int ix = 0; ix < lockedlist->Count; ++ix)
    {
        if (lockedtype[ix] != qtKanji)
            continue;
        UnicodeString str = lockedkanji[ix];
        for (int iy = str.Length(); iy > 0; --iy)
            if (KANJI(str[iy]))
                kanjis.Items[(int)str[iy]] = true;
    }
    return kanjis.TrueCount;
}
//---------------------------------------------------------------------------
int TWordStudyList::firsttest()
{
    int c = category->daystatcount(studyid);
    if (!c)
        return 0;

    int day = 0;
    for (int ix = 0; ix < c && !day; ++ix)
    {
        const TDayStat &stat = category->daystat(studyid, ix);
        if (stat.length)
            day = stat.day;
    }

    return day;
}
//---------------------------------------------------------------------------
int TWordStudyList::lasttest()
{
    int c = category->daystatcount(studyid);
    if (!c)
        return 0;

    int day = 0;
    for (int ix = c - 1; ix >= 0 && !day; --ix)
    {
        const TDayStat &stat = category->daystat(studyid, ix);
        if (stat.length)
            day = stat.day;
    }

    return day;
}
//---------------------------------------------------------------------------
int TWordStudyList::testdays()
{
    int c = category->daystatcount(studyid);
    if (!c)
        return 0;

    int days = 0;
    for (int ix = 0; ix < c; ++ix)
    {
        const TDayStat &stat = category->daystat(studyid, ix);
        if (stat.length)
            days++;
    }

    return days;
}
//---------------------------------------------------------------------------
int TWordStudyList::skippeddays()
{
    if (lasttest())
    {
        int n = get_day(Now());
        int l = lasttest();
        return l - firsttest() - testdays() + 1 + (n - l - (l < n ? 1 : 0));
    }
    return 0;
}
//---------------------------------------------------------------------------
double TWordStudyList::overalltime()
{
    int c = category->daystatcount(studyid);
    if (!c)
        return 0;

    double len = 0;
    for (int ix = 0; ix < c; ++ix)
        len += category->daystat(studyid, ix).length;
    return len;
}
//---------------------------------------------------------------------------
double TWordStudyList::testaverage()
{
    int c = category->daystatcount(studyid);
    if (!c)
        return 0;

    double len = 0;
    int cnt = 0;
    for (int ix = 0; ix < c; ++ix)
    {
        const TDayStat &stat = category->daystat(studyid, ix);
        if (!stat.length)
            continue;
        len += stat.length;
        cnt++;
    }

    return cnt ? len / cnt : 0;
}
//---------------------------------------------------------------------------
double TWordStudyList::answeraverage()
{
    int c = category->daystatcount(studyid);
    if (!c)
        return 0;

    double len = 0;
    int cnt = 0;
    for (int ix = 0; ix < c; ++ix)
    {
        const TDayStat &stat = category->daystat(studyid, ix);
        if (!stat.length)
            continue;
        len += stat.length;
        cnt += stat.testcount;
    }

    return cnt ? len / cnt : 0;
}
//---------------------------------------------------------------------------
unsigned int TWordStudyList::get_statcount()
{
    return category->daystatcount(studyid);
}
//---------------------------------------------------------------------------
const TDayStat& TWordStudyList::get_daystat(int ix)
{
    return category->daystat(studyid, ix);
}
//---------------------------------------------------------------------------
unsigned int TWordStudyList::get_timestatcount()
{
    return category->timestatcount(studyid);
}
//---------------------------------------------------------------------------
void TWordStudyList::daychanged() // We assume that this is only called when we are not in a test.
{
    //if (!fstudying)
    //newlist->Clear();
}
//---------------------------------------------------------------------------
void TWordStudyList::deckcounts(TGapList<int> &l) // Number of items in a given deck.
{
    for (int ix = 0; ix < lockedlist->Count; ++ix)
        l.Items[lockeddeck[ix]]++;
}
//---------------------------------------------------------------------------
void TWordStudyList::trycounts(TGapList<int> &l) // Number of items that were tested for a given time.
{
    for (int ix = 0; ix < lockedlist->Count; ++ix)
        l.Items[lockedtries[ix]]++;
}
//---------------------------------------------------------------------------
void TWordStudyList::duecounts(TGapList<int> &l) // List of the number of items due on a day.
{
    int n = get_day(Now());
    for (int ix = 0; ix < lockedlist->Count; ++ix)
    {
        int d = get_day(lockednext[ix]);
        l.Items[max(d, n)]++;
    }

    //l.Items[n] += newlist->Count;
}
//---------------------------------------------------------------------------
double TWordStudyList::get_timeaverage(int deckix)
{
    if (deckix >= category->timestatcount(studyid))
        return 0;

    double result = 0;
    const TTimeStat &stat = category->timestat(studyid, deckix);
    double avgrep = stat.averagerepeat();

    for (int j = 0; j <= min(round(avgrep), stat.alloced - 1); ++j)
    {
        double avgtim = stat.averagetime(j);
        result += j > floor(avgrep) ? fpart(avgrep) * avgtim : avgtim;
    }
    return result;
}
//---------------------------------------------------------------------------
double TWordStudyList::get_timeretries(int deckix)
{
    if (deckix >= category->timestatcount(studyid))
        return 0;
    return category->timestat(studyid, deckix).averagerepeat();
}
//---------------------------------------------------------------------------
void TWordStudyList::CleanUp()
{
    for (int ix = freelist->Count - 1; ix >= 0; --ix)
        if (freelist->Items[ix]->def->fwordindex < 0)
            delete_free(ix, true);
    for (int ix = lockedlist->Count - 1; ix >= 0; --ix)
        if (lockedlist->Items[ix]->def->fwordindex < 0)
            delete_locked(ix, true);

    for (int ix = Count - 1; ix >= 0; --ix)
        if (Items[ix]->fwordindex < 0)
            Delete(ix);
}
/*
 void TWordStudyList::decktimeavg(TGapList<double> &l) // average time spent on a deck
 {
 int c = category->timestatcount(studyid);
 for (int ix = 0; ix < c; ++ix) {
 const TTimeStat &stat = category->timestat(studyid,ix);
 for (int j = 0; j <= min(round(stat.averagerepeat),stat.alloced); ++j)
 l.Items[ix] += j > floor(stat.averagerepeat) ? fpart(stat.averagerepeat)*stat.stat[j].averagetime : stat.stat[j].averagetime;
 }
 }
 //---------------------------------------------------------------------------
 void TWordStudyList::decktryavg(TGapList<int> &l) // average number of retries in a deck
 {
 int c = category->timestatcount(studyid);
 for (int ix = 0; ix < c; ++ix)
 l.Items[ix] = category->timestat(studyid,ix).averagerepeat;
 }*/
//---------------------------------------------------------------------------
// To be removed with everything unused below! (first added v0.57 2010.11.03).
//---------------------------------------------------------------------------
int TWordStudyList::addstudyitem(TStudyItem *sitem)
{

    TLockedWordStudyItem *item;
    TRepetitionItem *gitem = NULL;
    for (int ix = 0; ix < lockedlist->Count && !gitem; ++ix)
    {
        item = (TLockedWordStudyItem*)lockedlist->Items[ix];
        if (wordindex(item->def) == sitem->item->wordindex)
            gitem = item->ritem;
    }

    item = new TLockedWordStudyItem;
    TRepetitionItem *r = item->ritem = category->add_item(studyid, item, gitem);
    if (item && item->def && item->def->repgroup == NULL)
        item->def->repgroup = r;
    TRepetitionItem *sr = sitem->ritem;
    r->score = sr->score;
    r->testdate = sr->testdate;
    r->deck = sr->deck;
    r->inclusion = sr->inclusion;
    r->interval = sr->interval;
    r->time = sr->time;
    for (int ix = 0; ix < sr->stats->Count; ++ix)
    {
        TRepStat *stat;
        if (ix && sr->stats->Items[ix]->day == sr->stats->Items[ix - 1]->day)
            stat = r->stats->Items[r->stats->Count - 1];
        else
        {
            stat = new TRepStat;
            r->stats->Add(stat);
        }
        memcpy(stat, sr->stats->Items[ix], sizeof(TRepStat));
    }

    item->type = sitem->direction == sdKanjiToKana ? qtKana : qtKanji;
    item->added = sitem->ritem->stats->Items[0]->day + Random(3600) / 3600;

    int ix = 0;
    for (; ix < Count; ++ix)
    {
        if (wordindex(ix) == sitem->item->wordindex)
        {
            // Because of an unknown bug, some items were added twice to the study list.
            if (wordusage(ix) == 2)
            {
                category->remove_item(studyid, item);
                delete item;
                return -1;
            }

            setdefinition(item, Items[ix], true);
            Items[ix]->flastinclude = max(wordinclude(ix), r->testdate);
            break;
        }
    }
    if (ix == Count)
    {
        delete item;

        THROW(L"Non defined item in study list!");
    }

    return lockedlist->Add(item);
}
//---------------------------------------------------------------------------
void TWordStudyList::finishimport(unsigned int fid)
{
    for (int ix = 0; ix < Count; ++ix)
    {
        if (!wordusage(ix))
        {
            additem(ix, qtKanji, true);
            additem(ix, qtKana, true);
        }
    }
    for (int ix = 0; ix < lockedlist->Count; ++ix)
        duelist->Add((TLockedWordStudyItem*)lockedlist->Items[ix]);

    //wordstudy_sort_list = this;
    duelist->Sort(wordstudy_duelist_sort_proc);

    category->copy_daystat(studyid, fid);
}
//---------------------------------------------------------------------------
//END OF to be removed with everything unused below! (first added v0.57 2010.11.03)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#ifdef HACK_TEST_REMOVE

void TWordStudyList::RemoveLastTest()
{
    if (fstudying)
        return;

    TGapList<TDateTime> gl;

    for (int ix = lockedlist->Count - 1; ix >= 0; --ix)
    {
        TLockedWordStudyItem *dummy = (TLockedWordStudyItem*)lockedlist->Items[ix];
        if (dummy->ritem->stats->Items[0]->day == flasttest) // Must be moved back to the free items list.
            queue_locked(ix);
    }

    category->RemoveLastTest(studyid);

    int highest = 0;
    for (int ix = lockedlist->Count - 1; ix >= 0; --ix)
    {
        TLockedWordStudyItem *dummy = (TLockedWordStudyItem*)lockedlist->Items[ix];
        TRepStat *st = dummy->ritem->stats->Items[dummy->ritem->stats->Count - 1];
        highest = max(highest, st->day);

        if (!gl.ItemSet((int)dummy->def) || gl.Items[(int)dummy->def] < dummy->ritem->itemdate)
            gl.Items[(int)dummy->def] = dummy->ritem->itemdate;
    }
    flasttest = highest;

    for (unsigned int ix = 0; ix < gl.TrueCount; ++ix)
    {
        TWordStudyDefinition *def = (TWordStudyDefinition*)gl.TruePos[ix];
        TDateTime d = gl.TrueItems[ix];
        def->flastinclude = d;
    }

    for (int ix = 0; ix < Count; ++ix)
    {
        TWordStudyDefinition *def = Items[ix];
        if (def->flastinclude >= Now())
        {
            if (!gl.ItemSet((int)def))
                def->flastinclude = 0;
        }
    }

    duelist->Sort(wordstudy_duelist_sort_proc);
}
#endif

//---------------------------------------------------------
//---------------------------------------------------------
//------ Everything below is for group version 7 or less!!!
//---------------------------------------------------------
//---------------------------------------------------------
int TStudyWItem::TItems::IndexOf(TStudyKItem *aitem)
{
    for (int i = 0; i < Count; i++)
        if (Items[i]->kitem == aitem)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
int TStudyKItem::TItems::IndexOf(TStudyWItem *aitem)
{
    for (int i = 0; i < Count; i++)
        if (Items[i]->witem == aitem)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
TStudyList::TTestItemList::TTestItemList(TStudyList *aowner) : owner(aowner), frepeat(false), baselist(new TIntegerList)
{
}
//---------------------------------------------------------------------------
TStudyList::TTestItemList::~TTestItemList()
{
    Clear();
    delete baselist;
    baselist = NULL;
}
//---------------------------------------------------------------------------
TStudyItem* TStudyList::TTestItemList::get_current()
{
    return tests[fcurrent];
}
//---------------------------------------------------------------------------
TStudyItem* TStudyList::TTestItemList::get_item(int ix)
{
    return ix >= 0 && ix < Count ? Items[ix]->test : NULL;
}
//---------------------------------------------------------------------------
TStudyWItem* TStudyList::TTestItemList::get_words(int ix)
{
    TStudyItem *test = tests[ix];
    return test ? test->item : NULL;
}
//---------------------------------------------------------------------------
bool TStudyList::TTestItemList::RemoveLine(int windex)
{
    bool r = false;
    for (int i = Count - 1; i >= 0; i--)
        if (words[i]->wordindex == windex)
        {
            Delete(i);
            r = true;
        }
    return r;
}
//---------------------------------------------------------------------------
void TStudyList::TTestItemList::Delete(int ix)
{
    int p = 0;
    for (int i = 0; i < baselist->Count; i++)
    {
        p += baselist->Items[i];
        if (p + i >= ix)
        {
            int l = -1;
            if (p + i == ix)
            {
                if (firsttests[ix])
                    ffirsts--;
                l = baselist->Items[i];
                baselist->Delete(i);
            }
            if (i < baselist->Count)
                baselist->Items[i] = baselist->Items[i] + l;
            break;
        }
    }
    inherited::Delete(ix);
}
//---------------------------------------------------------------------------
void TStudyList::TTestItemList::FreeItem(TTestItem *item, int ix)
{
    if (fcurrent > ix)
        fcurrent--;

    item->instances--;
    if (!item->instances)
        delete item;
}
//---------------------------------------------------------------------------
void TStudyList::TTestItemList::Clear()
{
    if (!baselist)
        return;
    baselist->Clear();
    Reset();
}
//---------------------------------------------------------------------------
void TStudyList::TTestItemList::BeginUpdate()
{
    frepeat = false;
    baselist->Clear();
    Reset();
}
//---------------------------------------------------------------------------
void TStudyList::TTestItemList::EndUpdate()
{
    if (Count)
    {
        for (int i = 0; i < Count; i++)
            baselist->Add(NULL);
        for (int i = 0; i < Count; i++)
            if (firsttests[i])
                ffirsts++;
    }
}
//---------------------------------------------------------------------------
void TStudyList::TTestItemList::Reset()
{
    fcurrent = -1;

    int bc = basecount;
    if (bc && Count > bc)
    {
        for (int i = 0; i < bc; i++)
        {
            int p = baselist->Items[i];
            for (int j = 0; j < p; j++)
                inherited::Delete(i);
            baselist->Items[i] = NULL;
        }
    }
    while (Count > bc)
        Delete(Count - 1);

    ffirsts = 0;

    if (!bc)
        return;

    frepeat = true;
    for (int i = 0; i < Count; i++)
    {
        Items[i]->mistakes = 0;
        Items[i]->rounds = 0;
    }
}
//---------------------------------------------------------------------------
void TStudyList::TTestItemList::Insert(int apos, TTestItem *aitem)
{
    aitem->instances++;
    int bc = basecount;
    int p = 0;
    for (int i = 0; i < bc; i++)
    {
        p += baselist->Items[i];
        if (apos <= p + i)
        {
            ++baselist->Items[i];
            break;
        }
    }
    inherited::Insert(apos, aitem);
}
//---------------------------------------------------------------------------
int TStudyList::TTestItemList::Add(TRepetitionItem *fitem)
{
    TTestItem *item = new TTestItem;

    item->test = (TStudyItem*)fitem->pt;
    item->instances = 1;
    item->mistakes = 0;
    item->rounds = 0;
    item->included = true;
    return inherited::Add(item);
}
//---------------------------------------------------------------------------
void TStudyList::TTestItemList::Next()
{
    fcurrent++;
}
//---------------------------------------------------------------------------
void TStudyList::TTestItemList::Prev()
{
    fcurrent--;
}
//---------------------------------------------------------------------------
void TStudyList::TTestItemList::Answer(bool correct)
{
    Items[current]->rounds++;
    bool first = firsttests[current] && owner->round == 1;

    if (!correct && Items[current]->rounds < (owner->round == 1 ? 3 : 2) || first && Items[current]->rounds == 1)
    {
        if (first && Items[current]->rounds == 1)
            ffirsts--;
        double mul = (first ? 0.8 : 1) * (Items[current]->rounds > 2 ? 2.5 : Items[current]->rounds > 1 ? 1.5 : 1);
        int dif = Random(5) - 2;
        int d = MIN(Count - current, MAX(4, dif + mul * (profilesettings.krepetition == kriShort ? 8 : profilesettings.krepetition == kriMedium ? 16 : 30)));
        Insert(current + d, Items[current]);
    }

    if (!correct)
        Items[current]->mistakes++;
}
//---------------------------------------------------------------------------
bool TStudyList::TTestItemList::get_firsttest(int ix)
{
    return !Items[ix]->test->ritem->deck;
}
//---------------------------------------------------------------------------
int TStudyList::TTestItemList::get_basecount()
{
    return baselist->Count;
}
//---------------------------------------------------------------------------
int TStudyList::TTestItemList::get_baseindex(int ix)
{
    int pos = ix;
    for (int i = 0; i <= ix; i++)
    {
        pos += baselist->Items[i];
    }
    return pos;
}
//---------------------------------------------------------------------------
int TStudyList::TTestItemList::get_testcount()
{
    return Count + ffirsts;
}
//---------------------------------------------------------------------------
int TStudyList::TTestItemList::get_testcurrent()
{
    return fcurrent + 1;
}
//---------------------------------------------------------------------------
void TStudyList::TTestItemList::LoadFromFile(FILE *f, TRepetitionItemList *plist, bool suspended, int version)
{
    try
    {
        int c;
        fread(&c, sizeof(int), 1, f);

        if (suspended)
        {
            fread(&frepeat, sizeof(bool), 1, f);
            fread(&fcurrent, sizeof(int), 1, f);
            fread(&ffirsts, sizeof(int), 1, f);

            baselist->Capacity = c;
            for (int i = 0; i < c; i++)
            {
                int c2;
                fread(&c2, sizeof(int), 1, f);
                baselist->Add(c2);
            }
            fread(&c, sizeof(int), 1, f);
        }

        Capacity = c;

        if (!suspended)
            BeginUpdate();

        for (int i = 0; i < c; i++)
        {
            word w;
            fread(&w, sizeof(word), 1, f);
            if (w >= plist->Count)
                THROW(L"Error in file!");
            int ix = Add(plist->Items[w]);
            if (suspended)
            {
                fread(&Items[ix]->instances, sizeof(int), 1, f);
                fread(&Items[ix]->mistakes, sizeof(byte), 1, f);
                fread(&Items[ix]->included, sizeof(bool), 1, f);
                fread(&Items[ix]->rounds, sizeof(int), 1, f);
            }
        }

        if (!suspended)
            EndUpdate();

    }
    catch (...)
    {
        ;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TStudyList::TStudyItemList::TStudyItemList(TStudyList *aowner, TTestItemList *atest) : owner(aowner), lastcount(0), ftest(atest), studyid(0), inherited()
{
}
//---------------------------------------------------------------------------
TStudyList::TStudyItemList::~TStudyItemList()
{
    Clear();
    if (studyid)
        category->delete_study(studyid);
}
//---------------------------------------------------------------------------
void TStudyList::TStudyItemList::LoadFromFile(FILE *f, TStudyWordList *wlist, bool suspended, int version)
{
    bool error = false;
    Clear();

    fread(&studyid, sizeof(int), 1, f);

    int c;
    fread(&c, sizeof(int), 1, f);
    Capacity = c;
    if (suspended)
        fread(&lastcount, sizeof(int), 1, f);

    for (int i = 0; i < c; i += 2)
    {
        int wi;
        fread(&wi, sizeof(int), 1, f);
        AddWord(wlist->Items[wi], false);
    }

    TRepetitionItemList *plist = new TRepetitionItemList;
    try
    {
        category->fetch_all(studyid, plist);
        if (plist->Count != Count)
            THROW(L"Count mismatch in study item list and repetition category!");

        for (int i = 0; i < plist->Count; i++)
        {
            plist->Items[i]->pt = Items[i];
            Items[i]->ritem = plist->Items[i];
        }
    }
    catch (...)
    {
        error = true;
        delete plist;
    }

    ftest->Clear();

    if (error)
        THROW(L"Invalid data! Restore backup and restart zkanji!");

    ftest->LoadFromFile(f, plist, suspended, version);

    if (!suspended)
        lastcount = Count;

    delete plist;
}
//---------------------------------------------------------------------------
TRepetitionCategory* TStudyList::TStudyItemList::get_category()
{
    return owner->collection->kcategory;
}
//---------------------------------------------------------------------------
bool TStudyList::TStudyItemList::RemoveLine(int windex)
{
    byte deleted = 0;
    for (int i = Count - 1; i >= 0 && deleted < 2; i--)
    {
        if (Items[i]->item->wordindex == windex)
        {
            Items[i]->item->used = 0;
            Delete(i);
            deleted++;
        }
    }
    return deleted;
}
//---------------------------------------------------------------------------
void TStudyList::TStudyItemList::Delete(int i)
{
    Items[i]->item->used = 0;
    inherited::Delete(i);
}
//---------------------------------------------------------------------------
void TStudyList::TStudyItemList::Delete(TStudyWItem *aitem)
{
    aitem->used--;
    if (aitem->used)
        return;

    for (int i = ftest->Count - 1; i >= 0; i--)
        if (ftest->words[i] == aitem)
            ftest->Delete(i);

    byte deleted = 0;
    for (int i = Count - 1; i >= 0 && deleted < 2; i--)
    {
        if (Items[i]->item == aitem)
        {
            Delete(i);
            deleted++;
        }
    }
}
//---------------------------------------------------------------------------
void TStudyList::TStudyItemList::FreeItem(TStudyItem *item, int ix)
{
    if (lastcount > ix)
        lastcount--;
    category->remove_item(studyid, item);
    delete item;
}
//---------------------------------------------------------------------------
void TStudyList::TStudyItemList::AddWord(TStudyWItem *witem, bool categorize)
{
    TStudyItem *item1 = new TStudyItem;
    TStudyItem *item2 = new TStudyItem;

    item1->item = witem;
    item2->item = witem;
    item1->direction = sdKanaToKanji;
    item2->direction = sdKanjiToKana;

    inherited::Add(item1);
    if (categorize)
        item1->ritem = category->add_item(studyid, item1);
    inherited::Add(item2);
    if (categorize)
        item2->ritem = category->add_item(studyid, item2);
}
//---------------------------------------------------------------------------
void TStudyList::TStudyItemList::AbortTest()
{
    Revert();
}
//---------------------------------------------------------------------------
void TStudyList::TStudyItemList::Revert()
{
    if (!ftest->repeat)
    {
        ftest->Clear();
        while (Count > lastcount)
            Delete(Count - 1);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int TStudyList::TStudyWordList::TStudyWItems::Add(TStudyWItem *aitem)
{
    return inherited::Add(aitem);
}
//---------------------------------------------------------------------------
int TStudyList::TStudyWordList::TStudyWItems::IndexOf(int word_index)
{
    for (int i = 0; i < Count; i++)
        if (Items[i]->wordindex == word_index)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
void TStudyList::TStudyWordList::TStudyWItems::FreeItem(TStudyWItem *item, int ix)
{
    delete[] item->meaning;
    delete item->kanjis;
    delete item;
}
//---------------------------------------------------------------------------
void TStudyList::TStudyWordList::TStudyWItems::DeleteKanji(int ix, TStudyKItem *aitem)
{
    int kix = Items[ix]->kanjis->IndexOf(aitem);
    if (kix < 0)
        THROW(L"Kanji not on item at all!");
    Items[ix]->kanjis->Delete(kix);
    if (!Items[ix]->kanjis->Count)
        Delete(ix);
}
//---------------------------------------------------------------------------
TStudyList::TStudyWordList::TStudyWordList(TStudyList *aowner) : owner(aowner)
{
    witems = new TStudyWItems;
}
//---------------------------------------------------------------------------
TStudyList::TStudyWordList::~TStudyWordList()
{
    delete witems;
}
//---------------------------------------------------------------------------
void TStudyList::TStudyWordList::Assign(TTemplateList<TStudyWItem> *aobj)
{
    witems->Clear();
    for (int i = 0; i < aobj->Count; i++)
        witems->Add(aobj->Items[i]);
}
//---------------------------------------------------------------------------
int TStudyList::TStudyWordList::IndexOf(int word_index)
{
    return witems->IndexOf(word_index);
}
//---------------------------------------------------------------------------
int TStudyList::TStudyWordList::get_count()
{
    return witems->Count;
}
//---------------------------------------------------------------------------
TStudyWItem* TStudyList::TStudyWordList::get_item(int ix)
{
    return witems->Items[ix];
}
//---------------------------------------------------------------------------
bool TStudyList::TStudyWordList::RemoveLine(int windex)
{
    bool r = false;
    for (int i = 0; i < Count; i++)
        if (witems->Items[i]->wordindex == windex)
        {
            for (int j = 0; j < witems->Items[i]->kanjis->Count; j++)
                witems->Items[i]->kanjis->Items[j]->kitem->words->Delete(witems->Items[i]->kanjis->Items[j]->kitem->words->IndexOf(witems->Items[i]));
            witems->Delete(i);
        }
        else if (witems->Items[i]->wordindex > windex)
        {
            witems->Items[i]->wordindex--;
            r = true;
        }
    return r;
}
//---------------------------------------------------------------------------
void TStudyList::TStudyWordList::DeleteKanji(int ix, TStudyKItem *aitem)
{
    witems->DeleteKanji(ix, aitem);
}
//---------------------------------------------------------------------------
void TStudyList::TStudyWordList::Remove(int windex)
{
    TStudyWItem *witem = witems->Items[windex];
    for (int i = 0; i < witem->kanjis->Count; i++)
    {
        TStudyKItem *kitem = witem->kanjis->Items[i]->kitem;
        kitem->words->Delete(kitem->words->IndexOf(witem));
    }
    witems->Delete(windex);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int TStudyList::TStudyKanjiList::TStudyKItems::Add(TStudyKItem *aitem)
{
    return inherited::Add(aitem);
}
//---------------------------------------------------------------------------
void TStudyList::TStudyKanjiList::TStudyKItems::FreeItem(TStudyKItem *item, int ix)
{
    delete item->words;
    delete item;
}
//---------------------------------------------------------------------------
void TStudyList::TStudyKanjiList::TStudyKItems::Exchange(int a, int b)
{
    TStudyKItem *aitem = Items[a];
    Items[a] = Items[b];
    Items[b] = aitem;
}
//---------------------------------------------------------------------------
TStudyList::TStudyKanjiList::TStudyKanjiList(TStudyList *aowner) : owner(aowner), TKanjiList()
{
    kitems = new TStudyKItems;
}
//---------------------------------------------------------------------------
TStudyList::TStudyKanjiList::~TStudyKanjiList()
{
    delete kitems;
}
//---------------------------------------------------------------------------
void TStudyList::TStudyKanjiList::Assign(TTemplateList<TStudyKItem> *aobj)
{
    kitems->Clear();
    for (int i = 0; i < aobj->Count; i++)
        kitems->Add(aobj->Items[i]);
}
//---------------------------------------------------------------------------
/*
 T-List* TStudyList::TStudyKanjiList::get_list()
 {
 return (T-List*)kitems;
 }
 */
//---------------------------------------------------------------------------
void TStudyList::TStudyKanjiList::ClearUp()
{
    for (int i = Count - 1; i >= 0; i--)
        if (!kitems->Items[i]->words->Count)
            owner->DeleteKanji(i);
}
//---------------------------------------------------------------------------
void TStudyList::TStudyKanjiList::Delete(int i)
{
    kitems->Delete(i);
}
//---------------------------------------------------------------------------
TKanji* TStudyList::TStudyKanjiList::get_kanji(int ix)
{
    return kanjis->Kanjis[kitems->Items[ix]->kanjiindex];
}
//---------------------------------------------------------------------------
TStudyKItem* TStudyList::TStudyKanjiList::get_item(int ix)
{
    return kitems->Items[ix];
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TStudyList::TStudyList(TWordCollection *acoll) : fpos(0), fcoll(acoll), lastpos(0)
{
    ftesting = false;
    ffirstday = 0;
    flastday = 0;
    flastrepday = 0;
    flasttest = 0;
    ffirsttest = 0;
    flastrepetition = 0;
    ftest_count = 0;
    fround = 0;
    kstudylist = NULL;

    klist = new TStudyKanjiList(this);
    wlist = new TStudyWordList(this);

    ftest = new TTestItemList(this);
    fstudy = new TStudyItemList(this, ftest);
}
//---------------------------------------------------------------------------
TStudyList::~TStudyList()
{
    Abort();

    delete ftest;
    delete fstudy;

    delete wlist;
    delete klist;
}
//---------------------------------------------------------------------------
int qccfunc(TConvertItem **a)
{
    return (*a)->original;
}
void TStudyList::LoadFromFile(FILE *f, int version)
{
    if (version <= 3)
        return;

    bool error = false;

    bool suspended = false;
    if (version >= 5)
        fread(&suspended, sizeof(bool), 1, f);

    double d;
    fread(&fpos, sizeof(int), 1, f);

    if (suspended)
    {
        fread(&lastpos, sizeof(int), 1, f);
        fread(&fround, sizeof(int), 1, f);
        fread(&startday, sizeof(int), 1, f);
    }

    fread(&ffirstday, sizeof(int), 1, f);
    fread(&flastday, sizeof(int), 1, f);
    fread(&flastrepday, sizeof(int), 1, f);
    fread(&d, sizeof(double), 1, f);
    flasttest = d;
    fread(&d, sizeof(double), 1, f);
    ffirsttest = d;
    fread(&d, sizeof(double), 1, f);
    flastrepetition = d;
    fread(&ftest_count, sizeof(int), 1, f);

    int c;

    TConvertItem **ci = NULL;

    TTemplateList<TStudyKItem> *ktmp = new TTemplateList<TStudyKItem>;
    TTemplateList<TStudyWItem> *wtmp = new TTemplateList<TStudyWItem>;
    try
    {
        fread(&c, sizeof(int), 1, f);
        wtmp->Capacity = c;
        ci = new TConvertItem *[c];
        try
        {
            for (int i = 0; i < c; i++)
            {
                TStudyWItem *witem = new TStudyWItem;
                witem->kanjis = new TStudyWItem::TItems;
                ci[i] = new TConvertItem;
                wtmp->Add(witem);

                byte blen;

                if (version >= 5)
                    fread(&ci[i]->original, sizeof(int), 1, f);

                fread(&blen, sizeof(byte), 1, f);
                fread(ci[i]->kanji, sizeof(wchar_t), blen, f);
                ci[i]->kanji[blen] = 0;
                fread(&blen, sizeof(byte), 1, f);
                fread(ci[i]->kana, sizeof(wchar_t), blen, f);
                ci[i]->kana[blen] = 0;

                witem->wordindex = ci[i]->changed = FindWordIndex(collection, ci[i]->kanji, ci[i]->kana);

                word mlen;
                fread(&mlen, sizeof(word), 1, f);
                witem->meaning = new char[mlen + 1];
                fread(witem->meaning, sizeof(char), mlen, f);
                witem->meaning[mlen] = 0;
                fread(&witem->used, sizeof(word), 1, f);
            }
        }
        catch (...)
        {
            error = true;
        }

        if (error)
            THROW(L"Error occurred in study list reading!");

        fread(&c, sizeof(int), 1, f);
        ktmp->Capacity = c;
        for (int i = 0; i < c; i++)
        {
            TStudyKItem *kitem = new TStudyKItem;
            kitem->words = new TStudyKItem::TItems;
            ktmp->Add(kitem);
            fread(&kitem->kanjiindex, sizeof(int), 1, f);
        }

        for (int i = 0; i < wtmp->Count; i++)
        {
            TStudyWItem *witem = wtmp->Items[i];
            fread(&c, sizeof(int), 1, f);
            witem->kanjis->Capacity = c;
            for (int j = 0; j < c; j++)
            {
                TStudyWItem::TItem *titem = new TStudyWItem::TItem;
                witem->kanjis->Add(titem);
                fread(&titem->reading, sizeof(byte), 1, f);
                int ki;
                fread(&ki, sizeof(int), 1, f);
                TStudyKItem *kitem = ktmp->Items[ki];
                titem->kitem = kitem;
                TStudyKItem::TItem *tkitem = new TStudyKItem::TItem;
                kitem->words->Add(tkitem);
                tkitem->witem = witem;
            }
        }

        wlist->Assign(wtmp);
        klist->Assign(ktmp);
    }
    catch (...)
    {
        if (ci)
        {
            try
            {
                for (int i = 0; i < wtmp->Count; i++)
                    delete ci[i];
            }
            catch (...)
            {
                ;
            }
            delete[] ci;
        }
        error = true;
    }
    delete ktmp;
    delete wtmp;

    if (error)
        THROW(L"Error in data!");

    fstudy->LoadFromFile(f, wlist, suspended, version);

    if (version >= 5 && suspended)
    {
        quicksort/*<TConvertItem*> */(ci, 0, wlist->Count - 1, &qccfunc);
        bool b;
        fread(&b, sizeof(bool), 1, f);
        if (b)
        {
            kstudylist = new TKanjiStudyList;
            kstudylist->LoadFromFile(f, version, ci, wlist->Count);
            if (!kstudylist->Count)
            {
                delete kstudylist;
                kstudylist = 0;
            }
        }
    }

    for (int i = 0; i < wlist->Count; i++)
        delete ci[i];
    delete[] ci;

    for (int i = wlist->Count - 1; i >= 0; i--)
    {
        TStudyWItem *wi = wlist->Items[i];
        if (wi->wordindex == -1)
        {
            wi->used = 1;
            fstudy->Delete(wi);
            wlist->Remove(i);
        }
    }
    klist->ClearUp();

    Abort();
}
//---------------------------------------------------------------------------
void TStudyList::Convert(TWordStudyList *ws)
{
    for (int ix = 0; ix < wlist->Count; ++ix)
    {
        TStudyWItem *w = wlist->Items[ix];
        ws->Add(w->wordindex, UnicodeString(w->meaning).c_str());
    }
    for (int ix = 0; ix < fstudy->Count; ++ix)
    {
        TStudyItem *si = fstudy->Items[ix];
        ws->addstudyitem(si);
    }

    ws->finishimport(fstudy->studyid);
}
//---------------------------------------------------------------------------
bool TStudyList::is_suspended(bool force_abort)
{
    if (!fround || get_day(Now()) != startday)
    {
        if (fround && (force_abort || !ftesting))
        {
            Abort();
            MsgBox("Suspended test was aborted because the day when testing was started is over.", "Message", MB_OK);
        }
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void TStudyList::Abort()
{
    if (!fround)
        return;
    fstudy->AbortTest();
    fpos = lastpos;
    fround = 0;
    if (kstudylist)
        delete kstudylist;
    kstudylist = NULL;
}
//---------------------------------------------------------------------------
void TStudyList::DeleteKanji(int ix)
{
    MODON
    TStudyKItem *ki = klist->Items[ix];
    for (int i = ki->words->Count - 1; i >= 0; i--)
    {
        TStudyWItem *wi = ki->words->Items[i]->witem;

        if (fpos > ix)
            fstudy->Delete(wi);

        int wix = wlist->IndexOf(ki->words->Items[i]->witem->wordindex);
        wlist->DeleteKanji(wix, ki);
    }
    klist->Delete(ix);

    if (fpos > ix)
        fpos--;
    if (lastpos > ix)
        lastpos--;
    if (!fpos)
        flasttest = 0;

    collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(161);
#endif


