//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <math.h>

#include "repetition.h"
#include "settings.h"
#include "collection.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(134);
#endif

//---------------------------------------------------------------------------
int day_diff(TDateTime aa, TDateTime bb)
{
    return get_day(aa) - get_day(bb);
}
//---------------------------------------------------------------------------
int get_day(TDateTime dt)
{
    double h = (1.0 / 24.0) * profilesettings.daystart;
    return (int)((double)dt - h);
}
//---------------------------------------------------------------------------
double good_interval(double initdate, double interval)
{
    double r = ((double)(Random(9999) + 1)) / 10000;
    double h = (1.0 / 24.0) * profilesettings.daystart;

    double d;
    if (!interval)
        d = ((get_day(initdate + 1) + h) - initdate) * r;
    else
        d = interval - (initdate - get_day(initdate) - h) + r;

    if (get_day(initdate + d) == get_day(initdate + interval))
        return d;
    return interval;
}
//---------------------------------------------------------------------------
double proportionate(double val, double amax, double amin, double vmax, double vmin, bool limit = false)
{
    amax -= amin;
    val -= amin;
    vmax -= vmin;

    val = ((val / amax) * vmax) + vmin;
    if (limit)
    {
        vmax += vmin;
        if (vmax < vmin)
            swap(vmax, vmin);
        val = min(vmax, max(vmin, val));
    }

    return val;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TTimeStat::addrepeat(byte arepeat)
{
    if (repeatcount)
        memmove(repeat + 1, repeat, sizeof(byte)*min(repeatcount, 100));
    repeatcount = min(101, repeatcount + 1);
    repeat[0] = arepeat;
}
void TTimeStat::grow(int repeatix)
{
    if (alloced <= repeatix)
    {
        TTimeStatItem *tmp = stat;
        stat = new TTimeStatItem[repeatix + 1];
        if (alloced)
        {
            memcpy(stat, tmp, sizeof(TTimeStatItem)*(alloced));
            delete[] tmp;
        }
        memset(stat + alloced, 0, sizeof(TTimeStatItem)*(repeatix + 1 - alloced));
        alloced = repeatix + 1;
    }
}
void TTimeStat::addtime(int repeatix, double atime)
{
    grow(repeatix);
    TTimeStatItem *item = stat + repeatix;
    if (item->timecount)
        memmove(item->time + 1, item->time, sizeof(double)*min(item->timecount, 254));
    item->timecount = min(255, item->timecount + 1);
    item->time[0] = atime;
}
void TTimeStat::removerepeat()
{
    if (!repeatcount)
        return;
    memmove(repeat, repeat + 1, sizeof(byte)*(repeatcount - 1));
    repeatcount--;
}
double TTimeStat::averagerepeat() const
{
    int sum = 0;
    int cnt = min(100, repeatcount);
    if (!cnt)
        return 0;

    for (int ix = 0; ix < cnt; ++ix)
    {
        sum += repeat[ix];
    }
    return (double)sum / cnt;
}
double TTimeStat::averagetime(int repeatix) const
{
    double sum = 0;
    int cnt = stat[repeatix].timecount;
    if (!cnt)
        return 0;

    for (int ix = 0; ix < cnt; ++ix)
        sum += stat[repeatix].time[ix];
    return sum / cnt;
}

//---------------------------------------------------------------------------
TRepetitionCategory::TRepetitionList::TTimeStatList::TTimeStatList() : inherited()
{
}
//---------------------------------------------------------------------------
TRepetitionCategory::TRepetitionList::TTimeStatList::~TTimeStatList()
{
    Clear();
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::TTimeStatList::FreeItem(TTimeStat *item, int ix)
{
    delete[] item->stat;
    delete item;
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::TTimeStatList::SaveToFile(FILE *f)
{
    int c = Count;
    fwrite(&c, sizeof(int), 1, f);
    for (int ix = 0; ix < c; ++ix)
    {
        //fwrite(&Items[ix]->averagerepeat,sizeof(double),1,f);
        fwrite(&Items[ix]->repeatcount, sizeof(byte), 1, f);
        if (Items[ix]->repeatcount)
            fwrite(Items[ix]->repeat, sizeof(byte), Items[ix]->repeatcount, f);

        fwrite(&Items[ix]->alloced, sizeof(byte), 1, f);
        for (int j = 0; j < Items[ix]->alloced; ++j)
        {
            //fwrite(&Items[ix]->stat[j].averagetime,sizeof(double),1,f);
            fwrite(&Items[ix]->stat[j].timecount, sizeof(byte), 1, f);
            if (Items[ix]->stat[j].timecount)
                fwrite(Items[ix]->stat[j].time, sizeof(double), Items[ix]->stat[j].timecount, f);
        }
    }
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::TTimeStatList::LoadFromFile(FILE *f, int version)
{
    double d;
    int i;

    int c;
    fread(&c, sizeof(int), 1, f);
    Capacity = c;
    for (int ix = 0; ix < Capacity; ++ix)
    {
        TTimeStat *item = new TTimeStat;
        memset(item, 0, sizeof(TTimeStat));
        inherited::Add(item);

        if (version >= 11) // Format changed Mar. 25 2011 for v0.577
        {
            fread(&item->repeatcount, sizeof(byte), 1, f);
            if (item->repeatcount)
                fread(item->repeat, sizeof(byte), item->repeatcount, f);
        }
        else
        {
            //fread(&item->averagerepeat,sizeof(double),1,f); // ex ver 10
            fread(&d, sizeof(double), 1, f);
            fread(&i, sizeof(int), 1, f);
            item->repeatcount = min(100, i);
            double fp = fpart(d);
            for (int k = 0; k < item->repeatcount; ++k)
                item->repeat[k] = byte(min(255.001, d)) + (d >= 255 ? 0 : k < double(item->repeatcount) * fp ? 1 : 0);
        }

        fread(&item->alloced, sizeof(byte), 1, f);
        if (item->alloced)
        {
            item->stat = new TTimeStatItem[item->alloced];
            memset(item->stat, 0, sizeof(TTimeStatItem)*item->alloced);
        }
        for (int j = 0; j < item->alloced; ++j)
        {
            if (version >= 11) // Format changed Mar. 25 2011 for v0.577
            {
                fread(&item->stat[j].timecount, sizeof(byte), 1, f);
                if (item->stat[j].timecount)
                    fread(item->stat[j].time, sizeof(double), item->stat[j].timecount, f);
            }
            else
            {
                //fread(&item->stat[j].averagetime,sizeof(double),1,f); // ex ver 10
                fread(&d, sizeof(double), 1, f);
                fread(&i, sizeof(int), 1, f);
                item->stat[j].timecount = min(255, i);
                for (int k = 0; k < item->stat[j].timecount; ++k)
                    item->stat[j].time[k] = d;
            }

        }
    }
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::TTimeStatList::GrowDeck(int deck)
{
    TTimeStat *item;
    while (Count <= deck)
    {
        item = new TTimeStat;
        memset(item, 0, sizeof(TTimeStat));
        inherited::Add(item);
    }
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::TTimeStatList::AddTime(byte adeck, byte arepeat, double aspenttime)
{
    GrowDeck(adeck);
    TTimeStat *item = Items[adeck];
    item->addtime(arepeat, aspenttime);
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::TTimeStatList::AddAverage(byte deck, byte repeat)
{
    undodeck = deck;

    if (Count <= deck)
        THROW(L"Deck must exist when adding average repeat count!");
    TTimeStat *item = Items[deck];

    item->addrepeat(repeat);
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::TTimeStatList::Undo()
{
    TTimeStat *item = Items[undodeck];
    item->removerepeat();
}
//---------------------------------------------------------------------------
double TRepetitionCategory::TRepetitionList::TTimeStatList::estimate(byte deck, byte repeat)
{
    double sec = 86400;
    double timeavg[] = { 25.0 / sec, 25.0 / sec, 20.0 / sec, 20.0 / sec, 20.0 / sec, 15.0 / sec, 15.0 / sec, 15.0 / sec, 10.0 / sec, 10.0 / sec, 10.0 / sec, 10.0 / sec, 10.0 / sec };
    double repeatavg[] = { 2.5, 1.7, 1.2, 0.8, 0.0 };

    int tcnt = sizeof(timeavg) / sizeof(double);
    int rcnt = sizeof(repeatavg) / sizeof(double);

    for (int ix = 0; ix < tcnt; ++ix)
    {
        if (Count > ix && Items[ix]->repeatcount >= 10 && Items[ix]->alloced > 0 && Items[ix]->stat[0].timecount >= 10)
            timeavg[ix] = Items[ix]->averagetime(0); //Items[ix]->stat[0].averagetime;
        else if (ix)
            timeavg[ix] = timeavg[ix - 1] * 0.9;
    }

    double r = 0;
    if (Count <= deck || Items[deck]->repeatcount < 10)
    {
        double d = deck >= tcnt ? timeavg[tcnt - 1] : timeavg[deck];
        double rep = repeat == 255 ? 1 : max(0.0, (deck >= rcnt ? repeatavg[rcnt - 1] : repeatavg[deck]) - repeat) + 1;
        return d*rep;
    }

    TTimeStat *item = Items[deck];
    double avgrep = item->averagerepeat();

    if (repeat == 255)
    {
        if (item->alloced && item->stat[0].timecount >= 10)
            return item->averagetime(0); //item->stat[0].averagetime;
        else
            return deck >= tcnt ? timeavg[tcnt - 1] : timeavg[deck];
    }

    for (int ix = repeat; ix <= min(repeat, (int)avgrep); ++ix)
    {
        if (item->alloced > ix && item->stat[ix].timecount >= 10)
            r += item->averagetime(ix); //item->stat[ix].averagetime;
        else
            r += deck >= tcnt ? timeavg[tcnt - 1] : timeavg[deck];
    }

    if (repeat < item->averagerepeat())
    {
        double d = deck >= tcnt ? timeavg[tcnt - 1] : timeavg[deck];
        if (item->alloced > int(avgrep) + 1)
            d = item->averagetime(int(avgrep) + 1); //item->stat[int(avgrep)+1].averagetime;
        return r + (avgrep - repeat) * d;
    }

    for (int ix = int(avgrep) + 1; ix <= repeat; ++ix)
    {
        if (item->alloced > ix && item->stat[ix].timecount >= 10)
            r += item->averagetime(ix); //item->stat[ix].averagetime;
        else
            r += deck >= tcnt ? timeavg[tcnt - 1] : timeavg[deck];
    }

    return r;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TRepetitionCategory::TRepetitionList::TDayStatList::TDayStatList()
{
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::TDayStatList::SaveToFile(FILE *f)
{
    int c = Count;
    fwrite(&c, sizeof(int), 1, f);
    for (int ix = 0; ix < Count; ++ix)
    {
        fwrite(&Items[ix]->day, sizeof(int), 1, f);

        fwrite(&Items[ix]->length, sizeof(double), 1, f);
        fwrite(&Items[ix]->testcount, sizeof(int), 1, f);
        fwrite(&Items[ix]->testednew, sizeof(int), 1, f);
        fwrite(&Items[ix]->testfailure, sizeof(int), 1, f);
        fwrite(&Items[ix]->testlearned, sizeof(int), 1, f);

        fwrite(&Items[ix]->itemcount, sizeof(int), 1, f);
        fwrite(&Items[ix]->groupcount, sizeof(int), 1, f);
        fwrite(&Items[ix]->itemlearned, sizeof(int), 1, f);
    }
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::TDayStatList::LoadFromFile(FILE *f, int version)
{
    int c;
    fread(&c, sizeof(int), 1, f);
    Capacity = c;
    for (int ix = 0; ix < Capacity; ++ix)
    {
        TDayStat *item = new TDayStat;
        inherited::Add(item);
        fread(&Items[ix]->day, sizeof(int), 1, f);

        fread(&item->length, sizeof(double), 1, f);
        fread(&item->testcount, sizeof(int), 1, f);
        fread(&item->testednew, sizeof(int), 1, f);
        fread(&item->testfailure, sizeof(int), 1, f);
        fread(&item->testlearned, sizeof(int), 1, f);
        //item->testlearned = max(0,item->testlearned);

        fread(&item->itemcount, sizeof(int), 1, f);
        fread(&item->groupcount, sizeof(int), 1, f);
        fread(&item->itemlearned, sizeof(int), 1, f);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TRepetitionCategory::TRepetitionList::TRepetitionList(unsigned int aid) : fid(aid), inherited()
{
    timestats = new TTimeStatList;
    daystats = new TDayStatList;
}
//---------------------------------------------------------------------------
TRepetitionCategory::TRepetitionList::~TRepetitionList()
{
    Clear();
    delete timestats;
    delete daystats;
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::FreeItem(TRepetitionItem *item, int ix)
{
    item->stats->Clear();
    delete item->stats;
    delete item;
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::SaveToFile(FILE *f)
{
    int c = Count;

    fwrite(&c, sizeof(int), 1, f);
    for (int i = 0; i < c; i++)
    {
        int p = (int)Items[i]->pt;
        fwrite(&p, sizeof(int), 1, f);
        double d = Items[i]->testdate;
        fwrite(&d, sizeof(double), 1, f);
        fwrite(&Items[i]->score, sizeof(double), 1, f);
        fwrite(&Items[i]->deck, sizeof(byte), 1, f);
        fwrite(&Items[i]->inclusion, sizeof(word), 1, f);
        fwrite(&Items[i]->interval, sizeof(double), 1, f);

        fwrite(&Items[i]->time, sizeof(double), 1, f);
        fwrite(&Items[i]->repeats, sizeof(byte), 1, f);

        fwrite(&Items[i]->testdeck, sizeof(byte), 1, f); // Added March 25. 2011

        int nextix = -1;
        if (Items[i]->next)
            nextix = IndexOf(Items[i]->next);
        fwrite(&nextix, sizeof(int), 1, f);

        fwrite(&Items[i]->answers.uncertain, sizeof(byte), 1, f);
        fwrite(&Items[i]->answers.good, sizeof(byte), 1, f);
        fwrite(&Items[i]->answers.hard, sizeof(byte), 1, f);
        fwrite(&Items[i]->answers.easy, sizeof(byte), 1, f);
        d = Items[i]->itemdate;
        fwrite(&d, sizeof(double), 1, f);

        p = Items[i]->stats->Count;
        fwrite(&p, sizeof(int), 1, f);
        for (int j = 0; j < p; j++)
        {
            fwrite(&Items[i]->stats->Items[j]->day, sizeof(int), 1, f);
            fwrite(&Items[i]->stats->Items[j]->score, sizeof(byte), 1, f);
            fwrite(&Items[i]->stats->Items[j]->deck, sizeof(byte), 1, f);
        }
    }

    timestats->SaveToFile(f);
    daystats->SaveToFile(f);
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::LoadFromFile(FILE *f, int version)
{
    Clear();

    int c;
    double d;

    fread(&c, sizeof(int), 1, f);
    Capacity = c;

    if (version <= 7)
    {
        bool testing;
        if (version >= 5)
            fread(&testing, sizeof(bool), 1, f);

        if (testing)
        {
            int lastcount;
            fread(&lastcount, sizeof(int), 1, f);
            fread(&d, sizeof(double), 1, f);
        }
    }

    int *nexts = NULL;
    try
    {
        if (version >= 8 && c) // Added Nov 2010, for v0.57.
            nexts = new int[c];

        for (int i = 0; i < c; i++)
        {
            TRepetitionItem *item = new TRepetitionItem;
            item->stats = new TRepStats;
            Add(item);
            int p;
            fread(&p, sizeof(int), 1, f);
            item->pt = (void*)p;
            double d;
            fread(&d, sizeof(double), 1, f);
            item->testdate = d;
            fread(&item->score, sizeof(double), 1, f);
            fread(&item->deck, sizeof(byte), 1, f);
            fread(&item->inclusion, sizeof(word), 1, f);
            fread(&item->interval, sizeof(double), 1, f);

            if (version >= 8) // Added Nov 2010, for v0.57
            {
                fread(&item->time, sizeof(double), 1, f);
                fread(&item->repeats, sizeof(byte), 1, f);
                if (version >= 11)
                    fread(&item->testdeck, sizeof(byte), 1, f); // Added March 25. 2011

                fread(&nexts[i], sizeof(int), 1, f);
            }
            else
            {
                item->time = item->inclusion * (25.0 / 86400.0);
                item->next = NULL;
                item->repeats = 0;
            }

            if (version < 11)
                item->testdeck = item->deck;

            if (version >= 9) // Added 2011 Jan 18, for v0.573
            {
                fread(&item->answers.uncertain, sizeof(byte), 1, f);
                fread(&item->answers.good, sizeof(byte), 1, f);
                fread(&item->answers.hard, sizeof(byte), 1, f);
                fread(&item->answers.easy, sizeof(byte), 1, f);

                fread(&d, sizeof(double), 1, f);
                item->itemdate = d;
            }
            else
            {
                memset(&item->answers, 0, sizeof(TRepAnswerCount));
                item->itemdate = item->testdate;
            }

            fread(&p, sizeof(int), 1, f);
            item->stats->Capacity = p;
            for (int j = 0; j < p; j++)
            {
                TRepStat *rep = new TRepStat;
                item->stats->Add(rep);
                fread(&rep->day, sizeof(int), 1, f);
                fread(&rep->score, sizeof(byte), 1, f);
                fread(&rep->deck, sizeof(byte), 1, f);
            }
        }

        if (version >= 8) //Added Nov 2010, for v0.57
        {
            for (int ix = 0; ix < c; ++ix)
            {
                if (nexts[ix] >= 0)
                    Items[ix]->next = Items[nexts[ix]];
                else
                    Items[ix]->next = NULL;
            }
        }
    }
    catch (...)
    {
        ;
    }
    delete[] nexts;

    if (version >= 8) // Added Nov 2010, for v0.57
    {
        timestats->LoadFromFile(f, version);
        daystats->LoadFromFile(f, version);
    }
    else
    {
        fread(&c, sizeof(int), 1, f);
        TDayStat *tmp = NULL;
        for (int i = 0; i < c; i++)
        {
            bool itemrepeat;
            int itemday;
            fread(&itemrepeat, sizeof(bool), 1, f);
            fread(&itemday, sizeof(int), 1, f);
            if (!tmp || tmp->day != itemday)
            {
                tmp = new TDayStat;
                memset(tmp, 0, sizeof(TDayStat));
                tmp->day = itemday;
                daystats->Add(tmp);

                for (int ix = 0; ix < Count; ++ix)
                {
                    for (int j = Items[ix]->stats->Count - 1; j >= 0; --j)
                    {
                        if (Items[ix]->stats->Items[j]->day <= tmp->day)
                        {
                            if (int(Items[ix]->stats->Items[j]->score) >= 90 && Items[ix]->stats->Items[j]->deck >= 4)
                                tmp->itemlearned++;
                            break;
                        }
                    }
                }
            }

            double tstart, tfinish, tlen;
            fread(&tstart, sizeof(double), 1, f);
            fread(&tfinish, sizeof(double), 1, f);
            if (version >= 5)
                fread(&tlen, sizeof(double), 1, f);
            else
                tlen = tfinish - tstart;
            tmp->length += tlen;

            word wtmp;
            fread(&wtmp, sizeof(word), 1, f);
            tmp->testlearned = wtmp;
            fread(&wtmp, sizeof(word), 1, f);
            fread(&wtmp, sizeof(word), 1, f);
            tmp->testfailure = wtmp;
            fread(&wtmp, sizeof(word), 1, f);
            tmp->testednew = wtmp;

            fread(&wtmp, sizeof(word), 1, f);
            tmp->testcount += wtmp;
            fread(&wtmp, sizeof(word), 1, f);
            tmp->testcount += wtmp;
            fread(&wtmp, sizeof(word), 1, f);
            tmp->testcount += wtmp;

            if (version >= 6) // Added 10.01.27 v0.533
            {
                int itmp;
                fread(&itmp, sizeof(int), 1, f);
                tmp->itemcount = itmp;
                tmp->groupcount = itmp / 2;
            }
            else
            {
                fread(&wtmp, sizeof(word), 1, f);
                tmp->itemcount = wtmp;
                tmp->groupcount = wtmp / 2;
            }
        }
    }

    if (version <= 10 && daystats->Count)
    {
        TDayStat *stat = StatToday(Now());

        int cnt = 0;
        for (int ix = 0; ix < Count; ++ix)
            if (Items[ix]->deck >= 4 && int(Items[ix]->score) >= 90)
                cnt++;
        //if (stat->itemlearned != cnt)
        //MsgBox((UnicodeString)"Old: "+stat->itemlearned+", real: "+cnt,"Error",MB_OK);
        stat->itemlearned = cnt;
    }
}
//---------------------------------------------------------------------------
bool TRepetitionCategory::TRepetitionList::date_passed(int ix, double date)
{
    TRepetitionItem *ri = Items[ix];
    return day_diff(date, ri->testdate + ri->interval) >= 0;
}
//---------------------------------------------------------------------------
int TRepetitionCategory::TRepetitionList::IndexOfData(void *ap)
{
    for (int i = 0; i < Count; i++)
        if (Items[i]->pt == ap)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::Delete(int ix)
{
    inherited::Delete(ix);
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::BeginTest()
{
    roundstart = Now();
}
//---------------------------------------------------------------------------
TDayStat* TRepetitionCategory::TRepetitionList::StatToday(TDateTime when)
{
    if (!daystats->Count || daystats->Items[daystats->Count - 1]->day < get_day(when))
    {
        TDayStat *stat = new TDayStat;

        memset(stat, 0, sizeof(TDayStat));
        stat->day = get_day(when);
        if (daystats->Count) // Copy previous day's statistics.
        {
            stat->itemcount = daystats->Items[daystats->Count - 1]->itemcount;
            stat->groupcount = daystats->Items[daystats->Count - 1]->groupcount;
            stat->itemlearned = daystats->Items[daystats->Count - 1]->itemlearned;
        }
        daystats->Add(stat);
    }

    return daystats->Items[daystats->Count - 1];
}
//---------------------------------------------------------------------------
bool TRepetitionCategory::TRepetitionList::AllowChange()
{
    bool r = true;
    int n = get_day(Now());
    if (daystats->Count && daystats->Items[daystats->Count - 1]->day > n)
    {
        r = false;
        int p = daystats->Count - 1;
        TDayStat *s1, *s2 = daystats->Items[p];
        while (!r && p)
        {
            --p;
            s1 = s2;
            s2 = daystats->Items[p];
            if (s1->length || s1->testcount || s1->testednew || s1->testfailure || s1->itemcount != s2->itemcount || s1->groupcount != s2->groupcount)
                break;
            if (s2->day <= n)
                r = true;
        }

        while (r && daystats->Count - 1 > p)
            daystats->Delete(daystats->Count - 1);
    }

    return r;
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::Step(TRepetitionItem *aitem, TRepetitionUndoItem &undodata, TRepetitionAnswer answer, bool starter, double timeused)
{
    // Set undo data
    undodata.item = aitem;
    undodata.answer = answer;
    undodata.retry.deck = aitem->deck, undodata.retry.interval = aitem->interval, undodata.retry.score = aitem->score, undodata.retry.date = aitem->testdate;
    undodata.starter = starter;
    memcpy(&undodata.hard, &undodata.retry, sizeof(TRepetitionUndoData));
    memcpy(&undodata.good, &undodata.retry, sizeof(TRepetitionUndoData));
    memcpy(&undodata.easy, &undodata.retry, sizeof(TRepetitionUndoData));

    profile->step(roundstart, raUncertain, starter, undodata.retry.deck, undodata.retry.date, undodata.retry.interval, undodata.retry.score, true);
    profile->step(roundstart, raHard, starter, undodata.hard.deck, undodata.hard.date, undodata.hard.interval, undodata.hard.score, true);
    profile->step(roundstart, raGood, starter, undodata.good.deck, undodata.good.date, undodata.good.interval, undodata.good.score, true);
    profile->step(roundstart, raEasy, starter, undodata.easy.deck, undodata.easy.date, undodata.easy.interval, undodata.easy.score, true);
    undodata.retry.interval = deviate_interval(aitem, undodata.retry.interval);
    undodata.hard.interval = deviate_interval(aitem, undodata.hard.interval);
    undodata.good.interval = deviate_interval(aitem, undodata.good.interval);
    undodata.easy.interval = deviate_interval(aitem, undodata.easy.interval);

    // End set undo data

    TDayStat *stat = StatToday(roundstart);
    stat->length += timeused;
    if (aitem->deck == 0)
        stat->testednew++;
    if (starter)
    {
        aitem->testdeck = aitem->deck;
        stat->testcount++;
        if (aitem->inclusion && (answer == raHard || answer == raUncertain))
            stat->testfailure++;
    }

    byte olddeck = aitem->deck;
    int oldscore = aitem->score;

    profile->step(roundstart, answer, starter, aitem->deck, aitem->testdate, aitem->interval, aitem->score, false);

    if (aitem->deck >= 4 && int(aitem->score) >= 90)
    {
        if (starter)
            stat->testlearned++;
        if (olddeck < 4 || oldscore < 90)
            stat->itemlearned++;
    }
    else if (olddeck >= 4 && oldscore >= 90)
        stat->itemlearned--;

    int day = get_day(roundstart);
    TRepStat *rstat;
    if (aitem->stats->Count && aitem->stats->Items[aitem->stats->Count - 1]->day == day)
    {
        rstat = aitem->stats->Items[aitem->stats->Count - 1];
    }
    else
    {
        rstat = new TRepStat;
        aitem->stats->Add(rstat);
        memset(rstat, 0, sizeof(TRepStat));

        memset(&aitem->answers, 0, sizeof(TRepAnswerCount));
    }
    rstat->day = day;
    rstat->score = (int)aitem->score;
    rstat->deck = aitem->deck;
    aitem->time += timeused;

    aitem->interval = deviate_interval(aitem, aitem->interval);
    aitem->itemdate = Now();

    if (answer == raUncertain && aitem->answers.uncertain < 255)
        aitem->answers.uncertain++;
    else if (answer == raHard && aitem->answers.hard < 255)
        aitem->answers.hard++;
    else if (answer == raGood && aitem->answers.good < 255)
        aitem->answers.good++;
    else if (answer == raEasy && aitem->answers.easy < 255)
        aitem->answers.easy++;

    if (starter)
    {
        aitem->inclusion++;
        aitem->repeats = 0;
    }
    else if (aitem->repeats < 255)
        aitem->repeats++;

    if (aitem->inclusion == 1)
    {
        aitem->score = 60;
        rstat->score = 60;
    }

    // Record the time for the item's starting deck and current repeat (starting = the deck before the first question was answered today)
    timestats->AddTime(aitem->testdeck, aitem->repeats, timeused);

    if (answer == raEasy || answer == raGood)
    {
        // Record the average repeat count of the item's starting deck
        timestats->AddAverage(aitem->testdeck, aitem->repeats);
        //aitem->repeats = 0;
    }
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::SimulateStep(TRepetitionItem *aitem, TRepetitionAnswer answer, bool starter, double &newinterval)
{
    newinterval = aitem->interval;
    byte tmpdeck = aitem->deck;
    TDateTime tmpdate = aitem->testdate;
    double tmpscore = aitem->score;
    profile->step(roundstart, answer, starter, tmpdeck, tmpdate, newinterval, tmpscore, true);

    newinterval = deviate_interval(aitem, newinterval);
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::Undo(TRepetitionItem *aitem, TRepetitionAnswer oldanswer, TRepetitionAnswer newanswer, const TRepetitionUndoData &newdata, bool starter)
{
    TRepStat *rstat = aitem->stats->Items[aitem->stats->Count - 1];
    rstat->score = newdata.score;
    rstat->deck = newdata.deck;

    switch (oldanswer)
    {
    case raUncertain:
        aitem->answers.uncertain = max(0, (int)aitem->answers.uncertain - 1);
        break;
    case raHard:
        aitem->answers.hard = max(0, (int)aitem->answers.hard - 1);
        break;
    case raGood:
        aitem->answers.good = max(0, (int)aitem->answers.good - 1);
        break;
    case raEasy:
        aitem->answers.easy = max(0, (int)aitem->answers.easy - 1);
        break;
    }

    switch (newanswer)
    {
    case raUncertain:
        aitem->answers.uncertain = min(255, (int)aitem->answers.uncertain + 1);
        break;
    case raHard:
        aitem->answers.hard = min(255, (int)aitem->answers.hard + 1);
        break;
    case raGood:
        aitem->answers.good = min(255, (int)aitem->answers.good + 1);
        break;
    case raEasy:
        aitem->answers.easy = min(255, (int)aitem->answers.easy + 1);
        break;
    }

    TDayStat *stat = StatToday(roundstart);
    if (int(aitem->score) >= 90 && aitem->deck >= 4 && (int(newdata.score) < 90 || newdata.deck < 4))
    {
        if (starter)
            stat->testlearned--;
        stat->itemlearned--;
    }
    else if ((int(aitem->score) < 90 || aitem->deck < 4) && int(newdata.score) >= 90 && newdata.deck >= 4)
    {
        if (starter)
            stat->testlearned++;
        stat->itemlearned++;
    }

    if (starter)
    {
        if (aitem->inclusion - (starter ? 1 : 0) && (oldanswer == raHard || oldanswer == raUncertain) && newanswer != raHard && newanswer != raUncertain)
            stat->testfailure--;
        else if (aitem->inclusion - (starter ? 1 : 0) && (newanswer == raHard || newanswer == raUncertain) && oldanswer != raHard && oldanswer != raUncertain)
            stat->testfailure++;
    }

    aitem->score = newdata.score;
    aitem->deck = newdata.deck;
    aitem->interval = newdata.interval;

    if ((oldanswer == raUncertain || oldanswer == raHard) && (newanswer == raGood || newanswer == raEasy))
    {
        timestats->AddAverage(aitem->testdeck, aitem->repeats);
    }
    else if ((oldanswer == raGood || oldanswer == raEasy) && (newanswer == raUncertain || newanswer == raHard))
    {
        timestats->Undo();
    }

}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::setdeck(TRepetitionItem *aitem, byte deck, double interval)
{
    aitem->interval = interval;

    double n = Now();
    if ((deck < 4) != (aitem->deck < 4) && int(aitem->score) >= 90)
    {
        TDayStat *stat = StatToday(n);
        if (deck < 4)
            stat->itemlearned--;
        else
            stat->itemlearned++;
    }

    aitem->deck = deck;
    TRepStat *stat;
    if (aitem->stats->Count)
    {
        stat = aitem->stats->Items[aitem->stats->Count - 1];
        stat->deck = deck;
    }
    /*else
    {
        stat = new TRepStat;
        stat->day = get_day(n);
        stat->score = aitem->score;

        aitem->stats->Add(stat);
    }*/

}
//---------------------------------------------------------------------------
double TRepetitionCategory::TRepetitionList::simulate_setdeck(TRepetitionItem *aitem, byte deck)
{
    return deviate_interval(aitem, profile->deckinterval(aitem->score, deck));
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::resetitem(TRepetitionItem *aitem)
{
    if (int(aitem->score) >= 90 && aitem->deck >= 4)
    {
        TDayStat *stat = StatToday(Now());
        stat->itemlearned--;
    }
    aitem->score = 60;
    aitem->deck = 0;
    aitem->testdate = Now();
    aitem->interval = good_interval(aitem->testdate, 0);
    aitem->time = 0;
    aitem->repeats = 0;
    aitem->inclusion = 0;
    aitem->stats->Clear();
    memset(&aitem->answers, 0, sizeof(TRepAnswerCount));
}
//---------------------------------------------------------------------------
double TRepetitionCategory::TRepetitionList::timeestimatenew()
{
    return timestats->estimate(0, 0);
}
//---------------------------------------------------------------------------
double TRepetitionCategory::TRepetitionList::timeestimate(TRepetitionItem *item)
{
    int day = get_day(roundstart);
    byte deck = 0;
    TRepStat *stat = NULL;

    if (item->stats->Count)
        stat = item->stats->Items[item->stats->Count - 1];
    if (stat && stat->day == day)
    {
        if (item->stats->Count > 1)
            stat = item->stats->Items[item->stats->Count - 2];
        else
            stat = NULL;
    }

    if (stat)
        deck = stat->deck;

    return timestats->estimate(deck, item->repeats);
}
//---------------------------------------------------------------------------
double TRepetitionCategory::TRepetitionList::singletimeestimate(byte deck)
{
    return timestats->estimate(deck, 255);
}
//---------------------------------------------------------------------------
double TRepetitionCategory::TRepetitionList::deviate_interval(TRepetitionItem *aitem, double interval)
{
    if (!aitem->next)
        return interval;
    int dev = max(1, interval * 0.1);
    int aday = get_day(aitem->testdate + interval);
    int minx = max(get_day(aitem->testdate) + 1, aday - dev);
    int maxx = aday + max(1, dev);

    int p = aday;
    double devmin = 0;

    for (int ix = minx; ix <= maxx; ++ix)
    {
        TRepetitionItem *curr = aitem->next;

        double devsum = 0;
        bool overlap = false;
        try
        {
            do
            {
                if (abs(get_day(curr->testdate + curr->interval) - aday) <= curr->interval * 0.1)
                {
                    devsum += abs(get_day(curr->testdate + curr->interval) - aday);
                    overlap = true;
                }
                curr = curr->next;
            }
            while (curr != aitem);
        }
        catch (...)
        {
            ;
        }
        if (!overlap && ix == aday && !devsum)
        {
            p = aday;
            break;
        }
        if (devmin <= devsum)
        {
            p = ix;
            devmin = devsum;
        }
    }
    if (aday != p)
        interval += p - aday;

    return interval;
}
//---------------------------------------------------------------------------
TRepetitionItem* TRepetitionCategory::TRepetitionList::AddItem(void *pt, TRepetitionItem *groupitem)
{
    TRepetitionItem *item = new TRepetitionItem;
    memset(item, 0, sizeof(TRepetitionItem));

    item->stats = new TRepStats;

    item->testdate = Now();
    item->interval = good_interval(item->testdate, 0);

    item->pt = pt;

    if (groupitem)
    {
        if (groupitem->next)
            item->next = groupitem->next;
        else
            item->next = groupitem;
        groupitem->next = item;
    }
    else
        item->next = NULL;

    Add(item);

    TDayStat *stat = StatToday(Now());
    stat->itemcount++;
    if (!groupitem)
        stat->groupcount++;

    return item;
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::RemoveItem(void *pt)
{
    int ix = IndexOfData(pt);
    if (ix < 0)
        THROW(L"Bad item data in study! Can't remove!");

    TRepetitionItem *item = Items[ix];

    int n = get_day(Now());
    TDayStat *stat = StatToday(Now());
    stat->itemcount--;

    if (item->next)
    {
        TRepetitionItem *curr = item->next;
        while (curr->next != item)
            curr = curr->next;

        if (item->next != curr)
            curr->next = item->next;
        else
            curr->next = NULL;
    }
    else
        stat->groupcount--;

    if (item->stats->Count && item->stats->Items[item->stats->Count - 1]->deck >= 4 && int(item->stats->Items[item->stats->Count - 1]->score) >= 90)
    {
        stat->itemlearned--;
        if (item->stats->Items[item->stats->Count - 1]->day == n)
            stat->testlearned--;
    }

    Delete(ix);
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::GroupItems(TRepetitionItem *item1, TRepetitionItem *item2)
{
    if (item1 == item2 || !item1 || !item2)
        return;

    TRepetitionItem *tmp = item1;
    while (tmp->next && tmp->next != item1)
    {
        tmp = tmp->next;
        if (tmp == item2)
            return;
    }

    if (!item1->next)
    {
        item1->next = item2->next;
        item2->next = item1;
        if (!item1->next)
            item1->next = item2;
    }
    else if (!item2->next)
    {
        item2->next = item1->next;
        item1->next = item2;
    }
    else
    {
        tmp->next = item2->next;
        item2->next = item1;
    }

    TDayStat *stat = StatToday(Now());
    --stat->groupcount;
}
//---------------------------------------------------------------------------
int TRepetitionCategory::TRepetitionList::DayStatCount()
{
    return daystats->Count;
}
//---------------------------------------------------------------------------
const TDayStat& TRepetitionCategory::TRepetitionList::DayStat(unsigned int statix)
{
    return *daystats->Items[statix];
}
//---------------------------------------------------------------------------
TDayStat* TRepetitionCategory::TRepetitionList::LastDayStat()
{
    return daystats->Count ? daystats->Items[daystats->Count - 1] : NULL;
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::FixDayStats()
{
    TDayStat *stat = LastDayStat();
    if (!stat || stat->itemcount == Count)
        return;

    stat->itemcount = Count;
    stat->itemlearned = 0;
    for (int ix = 0; ix < Count; ++ix)
    {
        if (Items[ix]->score >= 90 && Items[ix]->deck >= 4)
            stat->itemlearned++;
    }

}
//---------------------------------------------------------------------------
int TRepetitionCategory::TRepetitionList::TimeStatCount()
{
    return timestats->Count;
}
//---------------------------------------------------------------------------
const TTimeStat& TRepetitionCategory::TRepetitionList::TimeStat(unsigned int statix)
{
    return *timestats->Items[statix];
}
//---------------------------------------------------------------------------
void TRepetitionCategory::TRepetitionList::swap_daystats(TRepetitionList *rl)
{
    TDayStatList *tmp = rl->daystats;
    rl->daystats = daystats;
    daystats = tmp;
}
//---------------------------------------------------------------------------
int sort_fix_repetition_proc(TRepetitionItem **a, TRepetitionItem **b)
{
    if ((*a)->deck != (*b)->deck)
        return (*b)->deck - (*a)->deck;
    return (*b)->interval - (*a)->interval;
}
void TRepetitionCategory::TRepetitionList::fixintervals()
{
    int n = get_day(Now());
    int today = 0;

    for (int ix = 0; ix < Count; ++ix)
    {
        if (get_day(Items[ix]->testdate + Items[ix]->interval) <= n)
            today++;
        Items[ix]->interval = deviate_interval(Items[ix], profile->deckinterval(Items[ix]->score, Items[ix]->deck));
    }

    TRepetitionItemList *ri = new TRepetitionItemList();
    for (int ix = 0; ix < Count; ++ix)
    {
        if (get_day(Items[ix]->testdate + Items[ix]->interval) <= n)
            ri->Add(Items[ix]);
    }
    int days = min(30, max(0, ri->Count - today) / 15);
    if (days)
    {
        ri->Sort(sort_fix_repetition_proc);
        for (int ix = today; ix < ri->Count; ++ix)
        {
            ri->Items[ix]->interval += (n - get_day(ri->Items[ix]->testdate + ri->Items[ix]->interval)) + ((ix - today) / days);
        }
    }

    delete ri;
}
//---------------------------------------------------------------------------
/*
void TRepetitionCategory::TRepetitionList::Fix()
{
  if (!daystats->Count)
    return;

  TDayStat *stat = StatToday(Now());

  int cnt = 0;
  for (int ix = 0; ix < Count; ++ix)
    if (Items[ix]->deck >= 4 && int(Items[ix]->score) >= 90)
      cnt++;
  if (stat->itemlearned != cnt)
    MsgBox((UnicodeString)"Old: "+stat->itemlearned+", real: "+cnt,"Error",MB_OK);
  stat->itemlearned = cnt;
}
*/
//---------------------------------------------------------------------------
#ifdef HACK_TEST_REMOVE
void TRepetitionCategory::TRepetitionList::RemoveLastTest()
{
    int high = 0;

    for (int ix = 0; ix < Count; ++ix)
        high = max(high, Items[ix]->stats->Items[Items[ix]->stats->Count - 1]->day);

    int nexthigh = 0;
    for (int ix = Count - 1; ix >= 0; --ix)
    {
        TRepetitionItem *rep = Items[ix];
        if (rep->stats->Items[rep->stats->Count - 1]->day != high)
        {
            nexthigh = max(nexthigh, get_day(rep->testdate));
            continue;
        }
        for (int iy = rep->stats->Count - 1; iy >= 0; --iy)
        {
            if (rep->stats->Items[iy]->day != high)
            {
                rep->testdate = rep->stats->Items[iy]->day + 0.5;
                rep->itemdate = rep->testdate;
                rep->score = rep->stats->Items[iy]->score;
                rep->inclusion--;
                rep->deck = rep->stats->Items[iy]->deck;
                nexthigh = max(nexthigh, get_day(rep->itemdate));
                rep->testdeck = 0;
                for (int z = iy - 1; z >= 0; --z)
                {
                    if (rep->stats->Items[z]->day != rep->testdate)
                    {
                        rep->testdeck = rep->stats->Items[z]->deck;
                        break;
                    }
                }
                rep->interval = profile->deckinterval(rep->score, rep->deck);
                break;
            }
            rep->stats->Delete(iy);
        }

        if (rep->stats->Count == 0)
        {
            rep->testdate = 0;
            rep->itemdate = 0;
            rep->score = 0;
            rep->inclusion = 0;
            rep->deck = 0;
            rep->testdeck = 0;
            rep->interval = 0;
            rep->time = 0;
            rep->repeats = 0;
        }
    }

    for (int ix = daystats->Count - 1; ix >= 0; --ix)
    {
        TDayStat *s = daystats->Items[ix];
        if (s->day <= nexthigh)
        {
            roundstart = s->day + 0.5;
            break;
        }
        daystats->Delete(ix);
    }
}
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TRepetitionCategory::TRepetitionCategory()
{
    test_index = -1;
    studies = new TRepetitionLists;
}
//---------------------------------------------------------------------------
TRepetitionCategory::~TRepetitionCategory()
{
    while (studies->Count)
        delete_study(studies->Items[0]->id);
    delete studies;
}
//---------------------------------------------------------------------------
void TRepetitionCategory::SaveToFile(FILE *f)
{
    int c = studies->Count;
    fwrite(&c, sizeof(int), 1, f);

    for (int i = 0; i < c; i++)
    {
        int s = studies->Items[i]->id;
        fwrite(&s, sizeof(int), 1, f);
        studies->Items[i]->SaveToFile(f);
    }
}
//---------------------------------------------------------------------------
void TRepetitionCategory::LoadFromFile(FILE *f, int version)
{
    int c;
    if (version < 3)
        return;
    if (version == 3)
    {
        fread(&c, sizeof(int), 1, f);
        return;
    }

    studies->Clear();

    fread(&c, sizeof(int), 1, f);
    studies->Capacity = c;

    for (int i = 0; i < c; i++)
    {
        int s;
        fread(&s, sizeof(int), 1, f);
        TRepetitionList *rl = new TRepetitionList(s);
        studies->Add(rl);
        rl->LoadFromFile(f, version);
    }
}
//---------------------------------------------------------------------------
int TRepetitionCategory::study_index(unsigned int id)
{
    for (int i = 0; i < studies->Count; i++)
        if (id == studies->Items[i]->id)
            return i;
    return -1;
}
//---------------------------------------------------------------------------
int TRepetitionCategory::new_study()
{
    int sindex;
    do
    {
        sindex = Random(INT32_MAX);
    }
    while (!sindex || study_index(sindex) >= 0);
    TRepetitionList *rl = new TRepetitionList(sindex);
    studies->Add(rl);
    return rl->id;
}
//---------------------------------------------------------------------------
void TRepetitionCategory::delete_study(unsigned int id)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index!");
    TRepetitionList *rl = studies->Items[ix];
    studies->Delete(ix);

    delete rl;
}
//---------------------------------------------------------------------------
void TRepetitionCategory::fetch_all(unsigned int id, TRepetitionItemList *list)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    TRepetitionList *rl = studies->Items[ix];

    for (int i = 0; i < rl->Count; i++)
        list->Add(rl->Items[i]);
}
//---------------------------------------------------------------------------
TRepetitionListBase* TRepetitionCategory::fetch_study(unsigned int id)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    return studies->Items[ix];
}
//---------------------------------------------------------------------------
double TRepetitionCategory::timeestimatenew(unsigned int id)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    return studies->Items[ix]->timeestimatenew();
}
//---------------------------------------------------------------------------
double TRepetitionCategory::timeestimate(unsigned int id, TRepetitionItem *item)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    return studies->Items[ix]->timeestimate(item);
}
//---------------------------------------------------------------------------
double TRepetitionCategory::singletimeestimate(unsigned int id, byte deck)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    return studies->Items[ix]->singletimeestimate(deck);
}
//---------------------------------------------------------------------------
TRepetitionItem* TRepetitionCategory::add_item(unsigned int id, void *pt, TRepetitionItem *groupitem)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't add!");

    TRepetitionList *rl = studies->Items[ix];
    return rl->AddItem(pt, groupitem);
}
//---------------------------------------------------------------------------
void TRepetitionCategory::group_items(unsigned int id, TRepetitionItem *item1, TRepetitionItem *item2)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't add!");

    TRepetitionList *rl = studies->Items[ix];
    return rl->GroupItems(item1, item2);
}
//---------------------------------------------------------------------------
void TRepetitionCategory::remove_item(unsigned int id, void *pt)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    TRepetitionList *rl = studies->Items[ix];
    rl->RemoveItem(pt);
}
//---------------------------------------------------------------------------
bool TRepetitionCategory::allowchange(unsigned int id)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    TRepetitionList *rl = studies->Items[ix];
    return rl->AllowChange();
}
//---------------------------------------------------------------------------
void TRepetitionCategory::BeginTest(unsigned int id)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    test_index = ix;
    memset(&undoitem, 0, sizeof(TRepetitionUndoItem));

    TRepetitionList *rl = studies->Items[ix];
    rl->BeginTest();
}
//---------------------------------------------------------------------------
void TRepetitionCategory::Step(TRepetitionItem *aitem, TRepetitionAnswer answer, bool starter, double timeused)
{
    if (test_index < 0)
        THROW(L"Testing not started! Can't step!");

    TRepetitionList *rl = studies->Items[test_index];
    rl->Step(aitem, undoitem, answer, starter, timeused);
}
//---------------------------------------------------------------------------
void TRepetitionCategory::SimulateStep(TRepetitionItem *aitem, TRepetitionAnswer answer, bool starter, double &newinterval)
{
    if (test_index < 0)
        THROW(L"Testing not started! Can't step!");

    TRepetitionList *rl = studies->Items[test_index];
    rl->SimulateStep(aitem, answer, starter, newinterval);
}
//---------------------------------------------------------------------------
void TRepetitionCategory::Undo(TRepetitionAnswer newanswer)
{
    if (test_index < 0)
        THROW(L"Testing not started! Can't step!");

    TRepetitionList *rl = studies->Items[test_index];
    rl->Undo(undoitem.item, undoitem.answer, newanswer, (newanswer == raUncertain ? undoitem.retry : newanswer == raHard ? undoitem.hard : newanswer == raGood ? undoitem.good : undoitem.easy), undoitem.starter);
    undoitem.answer = newanswer;

}
//---------------------------------------------------------------------------
void TRepetitionCategory::setdeck(unsigned int id, TRepetitionItem *aitem, byte deck, double interval)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    TRepetitionList *rl = studies->Items[ix];
    rl->setdeck(aitem, deck, interval);
}
//---------------------------------------------------------------------------
double TRepetitionCategory::simulate_setdeck(unsigned int id, TRepetitionItem *aitem, byte deck)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    TRepetitionList *rl = studies->Items[ix];
    return rl->simulate_setdeck(aitem, deck);
}
//---------------------------------------------------------------------------
void TRepetitionCategory::resetitem(unsigned int id, TRepetitionItem *aitem)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    TRepetitionList *rl = studies->Items[ix];
    return rl->resetitem(aitem);
}
//---------------------------------------------------------------------------
int TRepetitionCategory::daystatcount(unsigned int id)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    TRepetitionList *rl = studies->Items[ix];
    return rl->DayStatCount();
}
//---------------------------------------------------------------------------
const TDayStat& TRepetitionCategory::daystat(unsigned int id, unsigned int statix)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    TRepetitionList *rl = studies->Items[ix];
    return rl->DayStat(statix);
}
//---------------------------------------------------------------------------
TDayStat* TRepetitionCategory::lastdaystat(unsigned int id)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    TRepetitionList *rl = studies->Items[ix];
    return rl->LastDayStat();
}
//---------------------------------------------------------------------------
void TRepetitionCategory::fixdaystats(unsigned int id)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    TRepetitionList *rl = studies->Items[ix];
    return rl->FixDayStats();
}
//---------------------------------------------------------------------------
int TRepetitionCategory::timestatcount(unsigned int id)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    TRepetitionList *rl = studies->Items[ix];
    return rl->TimeStatCount();
}
//---------------------------------------------------------------------------
const TTimeStat& TRepetitionCategory::timestat(unsigned int id, unsigned int statix)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    TRepetitionList *rl = studies->Items[ix];
    return rl->TimeStat(statix);
}
//---------------------------------------------------------------------------
// Temporary function for loading old style group. added Dec. 2010 for v0.57 -- to be deleted somewhere in the future
void TRepetitionCategory::copy_daystat(unsigned int newid, unsigned int oldid)
{
    int ix = study_index(newid);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    int iy = study_index(oldid);
    if (iy < 0)
        THROW(L"Bad study index! Can't fetch!");

    studies->Items[ix]->swap_daystats(studies->Items[iy]);
}
//---------------------------------------------------------------------------
bool TRepetitionCategory::hasundo()
{
    return undoitem.item != NULL;
}
//---------------------------------------------------------------------------
TRepetitionAnswer TRepetitionCategory::undo_answer()
{
    return undoitem.answer;
}
//---------------------------------------------------------------------------
double TRepetitionCategory::undo_goodinterval()
{
    return undoitem.good.interval;
}
//---------------------------------------------------------------------------
double TRepetitionCategory::undo_easyinterval()
{
    return undoitem.easy.interval;
}
//---------------------------------------------------------------------------
void TRepetitionCategory::fixintervals(unsigned int id)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");

    TRepetitionList *rl = studies->Items[ix];
    rl->fixintervals();
}
//---------------------------------------------------------------------------
#ifdef HACK_TEST_REMOVE
void TRepetitionCategory::RemoveLastTest(unsigned int id)
{
    int ix = study_index(id);
    if (ix < 0)
        THROW(L"Bad study index! Can't fetch!");
    TRepetitionList *rl = studies->Items[ix];
    rl->RemoveLastTest();
}
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TStudentProfile::TStudentProfile() : ffail_rate(8), mul(2.5), inherited()
{
}
//---------------------------------------------------------------------------
TStudentProfile::~TStudentProfile()
{
    Clear();
}
//---------------------------------------------------------------------------
void TStudentProfile::SaveToFile(FILE *f)
{
    fwrite(&ffail_rate, sizeof(byte), 1, f);
    fwrite(&mul, sizeof(double), 1, f);
    int c = Count;
    fwrite(&c, sizeof(int), 1, f);

    for (int i = 0; i < c; i++)
    {
        fwrite(&Items[i]->success, sizeof(double), 1, f);
        Items[i]->answers.SaveToFile(f);
    }
}
//---------------------------------------------------------------------------
void TStudentProfile::LoadFromFile(FILE *f, int version)
{
    fread(&ffail_rate, sizeof(byte), 1, f);
    fread(&mul, sizeof(double), 1, f);
    int c;
    double d;
    fread(&c, sizeof(int), 1, f);

    expand(c);

    for (int i = 0; i < c; i++)
    {
        if (version < 2)
        {
            fread(&d, sizeof(double), 1, f);
        }
        fread(&Items[i]->success, sizeof(double), 1, f);
        if (version >= 2)
            Items[i]->answers.LoadFromFile(f);
    }
    if (version < 2)
        Clear();
}
//---------------------------------------------------------------------------
void TStudentProfile::expand(int ix)
{
    if (Capacity < ix)
        Capacity = ix;

    while (Count < ix)
    {
        TRepetitionState *val = new TRepetitionState;
        val->success = !Count ? 80 - fail_rate : Items[Count - 1]->success;
        Add(val);
    }
}
//---------------------------------------------------------------------------
TRepetitionState* TStudentProfile::get_item(int ix)
{
    expand(ix + 1);
    return inherited::Items[ix];
}
//---------------------------------------------------------------------------
double TStudentProfile::scoremul(double score, TRepetitionAnswer answer)  // Helper function for profile step.
{
    double h = 100 - fail_rate; //h default: 92
    double h2 = h * 0.653; //h2 default: 60.076

    switch (answer)
    {
    case raEasy:
        return (100 - score) * proportionate(score, 0.1, 100.0, 0.6, 0.99);
    case raGood:
        return (100 - score) * proportionate(score, 0.1, h2, 0.1, 0.45); //(score <= h2 ? 0.5 : 0.46);
    case raHard:
        return -score * (score > h2 ? proportionate(score, 100, h2, 0.4, 0.3) : proportionate(score, h2, 0, 0.3, 0.9));
         //(score * (score <= h2 ? 0.4 : score < h ? 0.5 : 0.64))-score;
    case raUncertain:
        return -(score * 0.33);
    default:
        return 0;
    }
}
void TStudentProfile::limitdeck(TRepetitionAnswer answer, byte &elemdeck)
{
    if (answer != raUncertain && answer != raHard)
        return;

    elemdeck = min(elemdeck, answer == raUncertain ? profilesettings.kdecklimitretry : profilesettings.kdecklimithard);
}
void TStudentProfile::step(TDateTime testdate, TRepetitionAnswer answer, bool starter, byte &elemdeck, TDateTime &elemdate, double &eleminterval, double &elemscore, bool simulation) // called after each finalized item test
{
    double passed = testdate - elemdate;

    double tpos = 1;
    if (get_day(elemdate + eleminterval) != get_day(elemdate + passed))
        tpos = max(1.0, (passed / eleminterval));

    elemdate = testdate;

    // Reset item if it's new, got double negative answer or had a bad answer after too much time passed.
    if (!elemdeck || !starter && (answer == raHard || answer == raUncertain) || answer == raHard && tpos > 1.4)
    {
        if (!starter && (answer == raHard || answer == raUncertain))
            elemscore += proportionate(tpos, 2, 1, 0, scoremul(elemscore, answer) / 2, true);
        else if (starter && !elemdeck)
            elemscore = 60;
        else
            elemscore = min(elemscore, 60);

        if (answer == raEasy) // Perfect answer, no need to test for a while, skip to next deck.
        {
            elemdeck = 2;
            elemscore = 88;
            eleminterval = good_interval(testdate, count_interval(elemscore, elemdeck));
        }
        else
        {
            elemdeck = 1;
            eleminterval = good_interval(testdate, 1);
        }
        return;
    }

    if (!starter && answer != raEasy)
        return;

    double scorediff = scoremul(elemscore, answer);

    if (elemdeck >= 4 && (tpos > 1.6 && passed > 60 || tpos > 1.4 && passed > 21)) // 1.6x time passed instead of 1x interval and at least 2 weeks.
    {
        if (answer == raGood)
        {
            elemdeck = max(1, (int)elemdeck - 1);
        }
        else if (answer == raEasy)
        {
            elemscore += (100 - elemscore - scorediff) * proportionate(tpos, 1.6, 100000, 0.6, 1, true) + scorediff;
        }
        else if (answer == raUncertain)
            elemdeck = max(1, (int)elemdeck - 1);

        limitdeck(answer, elemdeck);
    }
    else if (tpos > 1.3 && passed > 14)
    {
        if (answer == raGood)
        {
            ;
        }
        else if (answer == raEasy)
        {
            elemdeck++;
            elemscore += (100 - elemscore - scorediff) * proportionate(tpos, 1.6, 1.3, 0.6, 0.3, true) + scorediff;
        }
        else if (answer == raUncertain)
        {
            elemdeck = max(1, (int)elemdeck - 2);
            elemscore += proportionate(tpos, 1.6, 1.3, 0, scorediff / 2, true);
        }
        else if (answer == raHard)
        {
            elemdeck = max(1, (int)elemdeck - 4);
            elemscore += proportionate(tpos, 1.6, 1.3, 0, scorediff / 2, true);
        }

        limitdeck(answer, elemdeck);
    }
    else
    {
        if (answer == raGood)
        {
            elemdeck++;
            elemscore += (100 - elemscore - scorediff) * proportionate(tpos, 1.3, 1, 0.3, 0, true) + scorediff;
        }
        else if (answer == raEasy)
        {
            if (starter)
                elemdeck += 2;
            else
                elemdeck++;
            elemscore += (100 - elemscore - scorediff) * proportionate(tpos, 1.3, 1, 0.3, 0, true) + scorediff;
        }
        else if (answer == raHard)
        {
            elemdeck = max(1, (int)elemdeck - 3);
            elemscore += proportionate(tpos, 1.3, 1, scorediff / 2, scorediff, true);
        }
        else if (answer == raUncertain)
        {
            elemscore += proportionate(tpos, 1.3, 1, scorediff / 2, scorediff, true);
        }

        limitdeck(answer, elemdeck);

        if (elemdeck > 1 && !simulation && starter && tpos < 1.3) // Change the student profile if item is not new and the result is not just simulated.
        {
            TRepetitionState *item = Items[elemdeck - 2];
            double scorepos = max(0.1, min(1.0, proportionate(tpos, 1.3, 1, 0.1, 1)));
            item->success = item->answers.size() < 20 ? double(item->success * 20 + elemscore * scorepos) / (20 + scorepos) : double((item->answers.percent() + item->success) * item->answers.size() + elemscore * scorepos) / (item->answers.size() * 2 + scorepos);
            //(item->success*item->test_count + elemscore*scorepos) / (item->test_count+scorepos);
            //item->test_count += scorepos;
            item->answers << 1;
            item->answers[0] = answer == raGood || answer == raEasy;
        }
    }

    if (elemdeck <= 0 || elemdeck > 100)
        THROW(L"????");

    if (elemdeck < 2)
        eleminterval = good_interval(testdate, 1);
    else
        eleminterval = MAX(1.0, good_interval(testdate, count_interval(elemscore, elemdeck)));

    elemdeck = min(elemdeck, interval_deck(eleminterval));
}
//---------------------------------------------------------------------------
int TStudentProfile::interval_deck(double interval)
{
    if (3 >= interval)
        return 1;

    double nextinterval = 4.0, previnterval;

    word deck = 2;

    while (nextinterval < interval)
    {
        previnterval = nextinterval;
        nextinterval *= mul;
        deck++;
    }

    if (interval < previnterval + (nextinterval - previnterval)*0.33)
        deck--;

    return deck;
}
//---------------------------------------------------------------------------
double TStudentProfile::deckinterval(double score, char deck)
{
    if (deck <= 1)
        return good_interval(Now(), 1);
    return MAX(1.0, good_interval(Now(), count_interval(score, deck)));
}
//---------------------------------------------------------------------------
double TStudentProfile::count_interval(double score, char deck)
{
    const double base[] = { 60, 88 - fail_rate, 96 - fail_rate, 100 - fail_rate };
    double interval = max(1.6, 4.0 * min(1.5, success_rate(0) / base[0]));
    word step = 1;

    double rnd;

    int randdiff = 2000;
    while (step < deck - 1)
    {
        if (step >= 2)
            randdiff /= 2;
        rnd = ((double)Random(randdiff * 2) - randdiff) / 10000.0;

        interval *= min(1.5, success_rate(step) / base[step < 3 ? step : 3]) * (mul + rnd);
        step++;
    }

    if (score > base[deck < 3 ? deck : 3])
    {
        interval *= 1 + proportionate(score, 100.0, base[deck < 3 ? deck : 3], 0.33, 0);
    }
    else if (score < base[deck < 3 ? deck : 3])
    {
        interval *= 1 - proportionate(score, base[deck < 3 ? deck : 3], 0, 0, 0.5);
    }

    return interval;
}
//---------------------------------------------------------------------------
double TStudentProfile::success_rate(int step)
{
    const double base[] = { 60, 88 - fail_rate, 96 - fail_rate, 100 - fail_rate };
    if (step >= Count)
    {
        if (Count)
            return (success_rate(Count - 1) + base[step > 3 ? 3 : step]) / 2;
        return base[step > 3 ? 3 : step];
    }
    TRepetitionState *item = Items[step];
    if (item->answers.size() < 20)
        return item->success;

    return (item->answers.percent() + item->success) / 2;
}
//---------------------------------------------------------------------------
/*void TStudentProfile::set_rate(byte val)
{
  if (ffail_rate == val || val < 5 || val > 20)
    return;

  CHANGEON
  for (int i = 0; i < Count; i++)
    Items[i]->success = MIN(100.0,Items[i]->success*(100.0-val)/(100.0-ffail_rate));
  ffail_rate = val;
  CHANGEOFF
}
//---------------------------------------------------------------------------
void TStudentProfile::set_mul(double val)
{
  if (mul == val || val < 1.4 || val > 3)
    return;

  CHANGEON
  for (int i = 0; i < Count; i++)
    Items[i]->success = MIN(100.0,Items[i]->success*mul/val);
  mul = val;
  CHANGEOFF
}*/
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(135);
#endif
