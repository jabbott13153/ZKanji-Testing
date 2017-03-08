//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <DateUtils.hpp>
#include <Math.h>

#include "wordgroupstudy.h"
#include "words.h"
#include "wordreview.h"
#include "settings.h"
#include "zkformats.h"
#include "collection.h"
#include "studyroom.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#define DRAW_INFO_GAP        20

#ifdef _DEBUG
static TGlobalTest tester1(184);

// Make compiler happy by using variables declared elsewhere:
static bool Dummy_function()
{
    return OneHour * OneMinute * OneSecond * OneMillisecond > 10;
}
#endif

const wchar_t *CStudyPractice[8] = { L"Meaning", L"KanaKanji", L"Kana", L"Kanji", L"Meaning\\Kana", L"Meaning\\Kanji", L"Meaning\\KanaKanji", L"MeaningKana" };
//---------------------------------------------------------------------------

int inflate_score(int score, TDateTime testtime);

//---------------------------------------------------------------------------
void TWordStudyStateList::Answer(int ix, bool correct, bool meaning, bool kanji, bool kana)
{
    Items[ix]->meaningerror = (meaning && !correct ? 1 : !meaning ? 0 : -1);
    Items[ix]->kanjierror = (kanji && !correct ? 1 : !kanji ? 0 : -1);
    Items[ix]->kanaerror = (kana && !correct ? 1 : !kana ? 0 : -1);
    Items[ix]->testtime = Now();
}
//---------------------------------------------------------------------------
void TWordStudyStateList::LoadFromFile(FILE *f, byte ver)
{
    Clear();
    int c;
    fread(&c, sizeof(int), 1, f);
    Capacity = c;
    while (Count < Capacity)
    {
        if (ver > 2)
        {
            TWordStudyState *state = new TWordStudyState;
            fread(&state->correct, sizeof(word), 1, f);
            fread(&state->incorrect, sizeof(word), 1, f);
            fread(&state->chaincorrect, sizeof(word), 1, f);
            fread(&state->lastcorrect, sizeof(bool), 1, f);
            fread(&state->score, sizeof(char), 1, f);

            if (ver < 12) // Changed 2011.04.08 for v0.59.
            {
                int dummy1;
                byte dummy2;
                fread(&dummy1, sizeof(int), 1, f);
                fread(&dummy2, sizeof(byte), 1, f);
            }

            fread(&state->testtime, sizeof(TDateTime), 1, f);
            fread(&state->skip, sizeof(char), 1, f);

            fread(&state->meaningerror, sizeof(char), 1, f);
            fread(&state->kanjierror, sizeof(char), 1, f);
            fread(&state->kanaerror, sizeof(char), 1, f);
            fread(&state->startscore, sizeof(char), 1, f);
            fread(&state->round, sizeof(word), 1, f);

            Add(state);
        }
        else
        {
            //MsgBox(L"zkanji dropped support of old word study groups since version 0.59. Try an older version to convert the data and update with the latest one again.",L"Error",MB_OK);
            THROW(L"zkanji dropped support of old word study groups since version 0.59. Try an older version to convert the data and update with the latest one again.");
        }
    }
}
//---------------------------------------------------------------------------
void TWordStudyStateList::SaveToFile(FILE *f)
{
    int c = Count;
    fwrite(&c, sizeof(int), 1, f);
    for (int i = 0; i < Count; i++)
    {
        fwrite(&Items[i]->correct, sizeof(word), 1, f);
        fwrite(&Items[i]->incorrect, sizeof(word), 1, f);
        fwrite(&Items[i]->chaincorrect, sizeof(word), 1, f);
        fwrite(&Items[i]->lastcorrect, sizeof(bool), 1, f);
        fwrite(&Items[i]->score, sizeof(char), 1, f);

        fwrite(&Items[i]->testtime, sizeof(TDateTime), 1, f);
        fwrite(&Items[i]->skip, sizeof(char), 1, f);

        fwrite(&Items[i]->meaningerror, sizeof(char), 1, f);
        fwrite(&Items[i]->kanjierror, sizeof(char), 1, f);
        fwrite(&Items[i]->kanaerror, sizeof(char), 1, f);
        fwrite(&Items[i]->startscore, sizeof(char), 1, f);
        fwrite(&Items[i]->round, sizeof(word), 1, f);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordGroupStudy::TWordGroupStudy(TWordCollection *acollection, UnicodeString aname, TStudyMethodType amethod, TStudyQuestionType aquestion, TStudyAnswerType aanswer, byte atimer, bool ahidekana) :
        TWordGroup(acollection, aname), fmethod(amethod), fquestion(aquestion), fanswer(aanswer), fhidekana(ahidekana),
        ftimer(atimer), finited(false), fcurrent(-1), fcorrect(0), fwrong(0), fround(0)
{
    fwords = new TIntegerList;
    fanswers = new TWordStudyStateList(words);
}
//---------------------------------------------------------------------------
TWordGroupStudy::TWordGroupStudy(TWordCollection *acollection, UnicodeString aname, TStudyMethodType amethod) : TWordGroup(acollection, aname),
        fmethod(amethod), finited(false), fcurrent(-1), fcorrect(0), fwrong(0), fround(0)
{
    fwords = new TIntegerList;
    fanswers = new TWordStudyStateList(words);
}
//---------------------------------------------------------------------------
TWordGroupStudy::~TWordGroupStudy()
{
    delete fwords;
    delete fanswers;
}
//---------------------------------------------------------------------------
void TWordGroupStudy::DeleteWord(int ix)
{
    if (inited)
        Reset();

    inherited::DeleteWord(ix);
}
//---------------------------------------------------------------------------
void TWordGroupStudy::Reset()
{
    if (fwords)
        fwords->Clear();
    else
        fwords = new TIntegerList;
    finited = false;
    fcurrent = -1;
    fcorrect = 0;
    fwrong = 0;
    fround = 0;
    fanswers->Clear();
}
//---------------------------------------------------------------------------
void TWordGroupStudy::CleanUp()
{
    for (int ix = 0; ix < list->Count; ++ix)
    {
        if (list->Items[ix]->meaning < 0 || list->Items[ix]->index < 0)
        {
            Reset();
            inherited::CleanUp();
        }
    }
}
//---------------------------------------------------------------------------
void TWordGroupStudy::Init()
{
    if (finited)
        return;

    MODON
    Randomize();

    fround = 0;

    DoInit();

    for (int ix = 0; ix < fwords->Count; ++ix)
    {
        int j = fwords->Items[ix];
        TWord *w = flist->Words[j];
        w->stats[flist->Items[j]->meaning].included++;
    }
    MODOFF
}
//---------------------------------------------------------------------------
bool TWordGroupStudy::DoInit()
{
    if (finited)
        return false;

    finited = true;
    fcurrent = -1;

    try
    {

        fanswers->Clear();

        for (int ix = 0; ix < list->Count; ix++)
        {
            TWordStatGlobal *g = WordStats[ix];
            if (!g)
            {
                TWord *w = list->Words[ix];
                w->stats = new TWordStatGlobal[w->meaningcnt];
                memset(w->stats, 0, sizeof(TWordStatGlobal)*w->meaningcnt);

                if (&w->stats[list->Items[ix]->meaning] != WordStats[ix])
                    THROW(L"?");
                g = w->stats;
            }

            TWordStudyState *state = new TWordStudyState;
            memset(state, 0, sizeof(TWordStudyState));
            fanswers->Add(state);

            if ((double)g->testtime == 0)
                continue;

            state->startscore = state->score = inflate_score(g->score, g->testtime);
        }
    }
    catch (...)
    {
        finited = false;
    }

    return finited;
}
//---------------------------------------------------------------------------
bool TWordGroupStudy::get_typeanswer()
{
    return fshowkanji != fshowkana && fshowmeaning &&
            (((answertype == saTypeKana || answertype == saTypeKK) && !fshowkana && (question == sqKana || question == sqMOrKana || question == sqKOrK || question == sqMOrKOrK)) ||
             ((answertype == saTypeKanji || answertype == saTypeKK) && !fshowkanji && (question == sqKanji || question == sqMOrKanji || question == sqKOrK || question == sqMOrKOrK)));
}
//---------------------------------------------------------------------------
bool TWordGroupStudy::get_choices()
{
    return fanswer == sa5Choices;
}
//---------------------------------------------------------------------------
UnicodeString TWordGroupStudy::get_wordmeanings(int ix)
{
    return list->Meanings[ix];
}
//---------------------------------------------------------------------------
UnicodeString TWordGroupStudy::get_wordkanjis(int ix)
{
    return list->Words[ix]->kanji;
}
//---------------------------------------------------------------------------
UnicodeString TWordGroupStudy::get_wordkanas(int ix)
{
    return list->Words[ix]->kana;
}
//---------------------------------------------------------------------------
void TWordGroupStudy::SaveStatsGlobal()
{
    for (int ix = 0; ix < fanswers->Count; ix++)
    {
        WordStats[ix]->score = MAX(-100, MIN(100, inflate_score(WordStats[ix]->score, WordStats[ix]->testtime) + (fanswers->Items[ix]->score - fanswers->Items[ix]->startscore)));
        WordStats[ix]->testtime = fanswers->Items[ix]->testtime;
    }
}
//---------------------------------------------------------------------------
void TWordGroupStudy::SaveStats()
{
    for (int ix = 0; ix < fanswers->Count; ++ix)
    {
        if (!fanswers->Items[ix]->meaningerror && !fanswers->Items[ix]->kanjierror && !fanswers->Items[ix]->kanaerror)
            continue;
        bool correct = fanswers->Items[ix]->meaningerror < 0 || fanswers->Items[ix]->kanjierror < 0 || fanswers->Items[ix]->kanaerror < 0;
        SaveStat(*fanswers->Items[ix]/*, WordStats[ix]->state*/, correct);

        fanswers->Items[ix]->meaningerror = 0;
        fanswers->Items[ix]->kanjierror = 0;
        fanswers->Items[ix]->kanaerror = 0;
    }
}
//---------------------------------------------------------------------------
void TWordGroupStudy::SaveStat(TWordStudyState &stat/*, TLearnedState lstate*/, bool correct)
{
    if (correct)
    {
        stat.correct++;
        stat.score = MIN(100, stat.score + 1);
        if (!stat.lastcorrect)
            stat.chaincorrect = 1;
        else
            stat.chaincorrect++;
    }
    else
    {
        ++stat.incorrect;
        stat.score = MAX(-100, stat.score - /*(lstate == lsLearned ? 6 : lstate == lsProblem ? 1 : 3)*/3);
    }
    stat.lastcorrect = correct;
}
//---------------------------------------------------------------------------
void TWordGroupStudy::Answer(bool correct)
{
    if (correct)
        ++fcorrect;
    else
        ++fwrong;

    fanswers->Answer(currentindex, correct, !fshowmeaning, !fshowkanji, !fshowkana);
}
//---------------------------------------------------------------------------
bool TWordGroupStudy::LastWord()
{
    return fcurrent == fwords->Count - 1;
}
//---------------------------------------------------------------------------
int TWordGroupStudy::get_currentindex()
{
    if (fcurrent < 0)
        return -1;
    return fwords->Items[fcurrent];
}
//---------------------------------------------------------------------------
void TWordGroupStudy::NextWord()
{
    Step();

    int j = fwords->Items[fcurrent];
    TWord *w = flist->Words[j];
    bool haskanji = has_kanji(w->kanji);

    int r;
    switch (fquestion)
    {
    case sqMeaning:
        fshowmeaning = false;
        fshowkanji = true;
        fshowkana = true;
        break;
    case sqKana:
        fshowmeaning = true;
        fshowkanji = true;
        fshowkana = false;
        break;
    case sqKanji:
        fshowmeaning = true;
        fshowkanji = false;
        fshowkana = true;
        break;
    case sqKK:
        fshowmeaning = true;
        fshowkanji = false;
        fshowkana = false;
        break;
    case sqMKana:
        fshowmeaning = false;
        fshowkanji = true;
        fshowkana = false;
        break;
    case sqMOrKana:
        fshowmeaning = haskanji && Random(2);
        fshowkanji = true;
        fshowkana = !fshowmeaning;
        break;
    case sqMOrKanji:
        fshowmeaning = haskanji && Random(2);
        fshowkanji = !fshowmeaning;
        fshowkana = true;
        break;
    case sqMOrKK:
        fshowmeaning = Random(2);
        fshowkanji = !fshowmeaning;
        fshowkana = !fshowmeaning;
        break;
    case sqKOrK:
        fshowmeaning = true;
        fshowkanji = Random(2);
        fshowkana = !fshowkanji;
        break;
    case sqMOrKOrK:
        r = !haskanji ? 0 : Random(3);
        fshowmeaning = r != 0;
        fshowkana = r != 1;
        fshowkanji = r != 2;
        break;
    }
}
//---------------------------------------------------------------------------
void TWordGroupStudy::Step()
{
    fcurrent = MIN(fcurrent + 1, fwords->Count - 1);
}
//---------------------------------------------------------------------------
TModalResult TWordGroupStudy::Suspend(bool ashown)
{
    if (MsgBox("You can always continue with this test later. Just select its\ngroup from the study group, and press \"Continue test\".", "Message", MB_OKCANCEL) == IDCANCEL)
        return mrCancel;

    if (!ashown)
        fcurrent--;
    return mrAbort;
}
//---------------------------------------------------------------------------
void TWordGroupStudy::FillReviewLists(int first, int last, TWordIndexList *l, TWordStudyStateList *sl, TWordStatGlobalList *gl)
{
    l->Capacity = last - first + 1;
    sl->Capacity = last - first + 1;
    gl->Capacity = last - first + 1;
    for (int ix = first; ix <= last; ++ix)
    {
        l->Add(list->Items[fwords->Items[ix]]->index, list->Items[fwords->Items[ix]]->meaning);
        sl->Add(new TWordStudyState(*fanswers->Items[fwords->Items[ix]])); //only adds a copy of everything
        gl->Add(WordStats[ix]);
    }
}
//---------------------------------------------------------------------------
void TWordGroupStudy::RefillReviewLists(int first, int last, TWordStudyStateList *sl)
{
    for (int ix = first; ix <= last; ++ix)
    {
        TWordStudyState *ws = fanswers->Items[fwords->Items[ix]];
        ws->meaningerror = sl->Items[ix - first]->meaningerror;
        ws->kanjierror = sl->Items[ix - first]->kanjierror;
        ws->kanaerror = sl->Items[ix - first]->kanaerror;
        ws->skip = sl->Items[ix - first]->skip;
    }
}
//---------------------------------------------------------------------------
void TWordGroupStudy::Review(bool change)
{
    Review(0, fwords->Count - 1, change, false, false);
    if (change)
        SaveStats();
}
//---------------------------------------------------------------------------
void TWordGroupStudy::Review(int first, int last, bool save, bool remove, int random)
{
    Review(first, last, first, last, save, remove, random);
}
//---------------------------------------------------------------------------
void TWordGroupStudy::Review(int first, int last, int firstwrong, int lastwrong, bool save, bool remove, int random)
{
    TWordIndexList *l = new TWordIndexList(collection);
    TWordStudyStateList *sl = new TWordStudyStateList(words);
    TWordStatGlobalList *gl = new TWordStatGlobalList;
    FillReviewLists(MIN(firstwrong, first), MAX(lastwrong, last), l, sl, gl);
    fWReview = new TfWReview(Application);
    fWReview->ShowModalEx(this, l, sl, gl, first - firstwrong, last - lastwrong + 1, fround, save, remove, random);
    RefillReviewLists(MIN(firstwrong, first), MAX(lastwrong, last), sl);

    delete l;
    delete sl;
    delete gl;
    delete fWReview;
}
//---------------------------------------------------------------------------
void TWordGroupStudy::FinishTest()
{
    Review(true);
    int error_code = 0;
    try
    {
        error_code = 1;
        if (MsgBox("Would you like to save the scores for later tests?", "Question", MB_YESNO) == IDYES)
            SaveStatsGlobal();
        error_code = 2;
        Reset();
    }
    catch (...)
    {
        MsgBox(((UnicodeString)"An error occurred while saving scores! (code: " + IntToStr(error_code) + ")").c_str(), "Error", MB_OK);
    }
}
//---------------------------------------------------------------------------
void TWordGroupStudy::LoadFromFile(FILE *f, byte ver)
{
    bool fchoices;

    if (ver >= 20)
    {
        fread(&fquestion, sizeof(int), 1, f);
        fread(&fanswer, sizeof(int), 1, f);
    }
    else
    {
        TStudyTmpTmp ftmp;
        fread(&ftmp, sizeof(TStudyTmpTmp), 1, f);
        switch (ftmp)
        {
        case epKK:
            fquestion = sqKK;
            break;
        case epKana:
            fquestion = sqKana;
            break;
        case epKanji:
            fquestion = sqKanji;
            break;
        case epMeaningPerKana:
            fquestion = sqMOrKana;
            break;
        case epMeaningPerKanji:
            fquestion = sqMOrKanji;
            break;
        case epMeaningPerKK:
            fquestion = sqMOrKK;
            break;
        case epMeaningKana:
            fquestion = sqMKana;
            break;
        case epMeaning:
        default:
            fquestion = sqMeaning;
        }

        fread(&fchoices, sizeof(bool), 1, f);
    }

    fread(&ftimer, sizeof(byte), 1, f);
    if (ver < 20)
    {
        bool ftypekana;
        fread(&ftypekana, sizeof(bool), 1, f);
        if (ftypekana)
            fanswer = saTypeKana;
        else
        {
            switch (fchoices)
            {
            case false:
                fanswer = saCorrectWrong;
            case true:
            default:
                fanswer = sa5Choices;
            }
        }
    }
    fread(&fhidekana, sizeof(bool), 1, f);
    fread(&finited, sizeof(bool), 1, f);

    fread(&fround, sizeof(word), 1, f);

    fread(&fcurrent, sizeof(int), 1, f);
    fread(&fcorrect, sizeof(int), 1, f);
    fread(&fwrong, sizeof(int), 1, f);

    int c, l;
    fread(&c, sizeof(int), 1, f);

    fwords->Clear();
    fwords->Capacity = c;
    for (int i = 0; i < c; i++)
    {
        fread(&l, sizeof(int), 1, f);
        fwords->Add(l);
    }

    fanswers->LoadFromFile(f, ver);

    if (finited)
    { //global word statistics are not saved and created unless a word was in a finished study list. create global statistics that do not exist yet
        for (int ix = 0; ix < list->Count; ++ix)
        {
            if (!WordStats[ix])
            {
                TWord *w = list->Words[ix];
                w->stats = new TWordStatGlobal[w->meaningcnt];
                memset(w->stats, 0, sizeof(TWordStatGlobal)*w->meaningcnt);

                if (&w->stats[list->Items[ix]->meaning] != WordStats[ix])
                    THROW(L"?");
                w->stats->included = 1;
            }
        }
    }
}
//---------------------------------------------------------------------------
void TWordGroupStudy::SaveToFile(FILE *f)
{
    //fwrite(&fpractice, sizeof(TStudyPractice), 1, f); // Removed 2013 07 04 for v0.731
    //fwrite(&fchoices, sizeof(bool), 1, f); // Removed 2013 07 04 for v0.731
    fwrite(&fquestion, sizeof(int), 1, f); // Added 2013 07 04 for v0.731
    fwrite(&fanswer, sizeof(int), 1, f); // Added 2013 07 04 for v0.731
    fwrite(&ftimer, sizeof(byte), 1, f);
    //fwrite(&ftypekana, sizeof(bool), 1, f); // Removed 2013 07 04 for v0.731
    fwrite(&fhidekana, sizeof(bool), 1, f);
    fwrite(&finited, sizeof(bool), 1, f);
    fwrite(&fround, sizeof(word), 1, f);

    if (fWStudy && fWStudy->ShowingResult)
        fcurrent--;
    fwrite(&fcurrent, sizeof(int), 1, f);
    if (fWStudy && fWStudy->ShowingResult)
        fcurrent++;
    fwrite(&fcorrect, sizeof(int), 1, f);
    fwrite(&fwrong, sizeof(int), 1, f);

    int l = fwords->Count;
    fwrite(&l, sizeof(int), 1, f);
    int c;
    for (int i = 0; i < l; i++)
    {
        c = fwords->Items[i];
        fwrite(&c, sizeof(int), 1, f);
    }

    fanswers->SaveToFile(f);
}
//---------------------------------------------------------------------------
//void TWordGroupStudy::Export(FILE *f)
//{
//    ExportHeader(f);
//    feol(f);
//
//    inherited::Export(f);
//}
//---------------------------------------------------------------------------
//const wchar_t *CEM1Order[4] = { L"Added", L"Backwards", L"Random", L"CNT" };
//void TWordGroupStudy::ExportHeader(FILE *f)
//{
//    UnicodeString z;
//
//    z = L"\tstudy group header:";
//    fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
//    feol(f);
//
//    z = (UnicodeString)methodname + L"\t" + CStudyPractice[fpractice] + L"\t" + (fchoices ? L"choices" : L"nochoices") + L"\t" + (ftypekana ? L"typekana" : L"notypekana") + L"\t" + (fhidekana ? L"hidekana" : L"nohidekana") + L"\t" + (ftimer ? L"timer" : L"notimer");
//    fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
//}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordGroupM1::TWordGroupM1(TWordCollection *acollection, UnicodeString aname, TStudyQuestionType aquestion, TStudyAnswerType aanswer, byte atimer, bool ahidekana, byte ainitial, EM1Order aorder, bool arandomize, byte awordadd, byte aselection, bool aglobal) : inherited(acollection, aname, smGradual, aquestion, aanswer, atimer, ahidekana)
{
    fwordstore = new TIntegerList;
    lastanswered = false;
    fwordadd = awordadd;
    fselection = aselection;
    fuseglobal = aglobal;

    finitial = ainitial;
    fincluded = fstartpos = fpos = 0;
    forder = aorder;
    frandomize = arandomize;
}
//---------------------------------------------------------------------------
TWordGroupM1::TWordGroupM1(TWordCollection *acollection, UnicodeString aname) : TWordGroupStudy(acollection, aname, smGradual)
{
    fwordstore = new TIntegerList;
    lastanswered = false;
}
//---------------------------------------------------------------------------
TWordGroupM1::~TWordGroupM1()
{
    delete fwordstore;
}
//---------------------------------------------------------------------------
void TWordGroupM1::Reset()
{
    delete fwordstore;
    fwordstore = new TIntegerList;
    fincluded = fstartpos = fpos = 0;

    inherited::Reset();
}
//---------------------------------------------------------------------------
int inflate_score(int score, TDateTime testtime)
{
    if (settings.statdays == 0)
        settings.statdays = 60;
    return floor((double)score * MAX(0.0, (settings.statdays - floor(DaySpan(Now(), testtime) + 0.5)) / settings.statdays));
}
//---------------------------------------------------------------------------
bool TWordGroupM1::DoInit()
{
    if (!inherited::DoInit())
        return false;

    int rnd;

    // Copy all the words to an initial not ordered list, so we can remove the ones already in use.
    TIntegerList *l = new TIntegerList;
    for (int ix = 0; ix < flist->Count; ++ix)
    {
        if ((question == sqKanji || question == sqKOrK) && !has_kanji(flist->Words[ix]->kanji))
            continue;
        l->Add(ix);
    }

    if (!frandomize) // Just leave the list as it is.
        fwords->Assign(*l);
    else // Add every item to list in random order.
    {
        while (l->Count)
        {
            rnd = Random(l->Count);
            fwords->Add(l->Items[rnd]);
            l->Delete(rnd);
        }
    }
    delete l;

    if (!fuseglobal)
    {
        for (int ix = 0; ix < fanswers->Count; ++ix)
        {
            TWordStatGlobal *g = WordStats[ix];
            if ((double)g->testtime == 0)
                continue;
            fanswers->Items[ix]->score = 0;
            fanswers->Items[ix]->startscore = 0;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void TWordGroupM1::SaveStat(TWordStudyState &stat/*, TLearnedState lstate*/, bool correct)
{
    if (correct)
        stat.round = fround;
    else
        stat.round++;

    inherited::SaveStat(stat/*,lstate*/, correct);
}
//---------------------------------------------------------------------------
void TWordGroupM1::DrawInfo(TPaintBox *p)
{
    TCanvas *c = p->Canvas;

    TRect r;
    TSize s;

    c->Font->Name = settings.romajifont;
    c->Font->Charset = DEFAULT_CHARSET;
    c->Font->Size = p->Height * 0.4;
    SetTextAlign(c->Handle, TA_TOP | TA_LEFT);

    if (lastanswered)
    {
        if (LastWord())
            GetTextExtentPoint32W(c->Handle, L"Test complete!", 14, &s);
        else
            GetTextExtentPoint32W(c->Handle, L"Round complete!", 15, &s);
        r = p->ClientRect;
        c->Font->Style = TFontStyles() << fsBold;
        if (LastWord())
            ExtTextOutW(c->Handle, (r.Width() - s.cx) / 2 - 1, (r.Height() - s.cy) / 2 - 1, ETO_CLIPPED, &r, L"Test complete!", 14, NULL);
        else
            ExtTextOutW(c->Handle, (r.Width() - s.cx) / 2 - 1, (r.Height() - s.cy) / 2 - 1, ETO_CLIPPED, &r, L"Round complete!", 15, NULL);
        return;
    }

    int w1, w2, w3, w4, w5, w6, h; // Width estimation.
    c->Font->Style = TFontStyles();
    GetTextExtentPoint32W(c->Handle, L"Round #", 7, &s);
    w1 = s.cx;
    h = s.cy;
    c->Font->Style = TFontStyles() << fsBold;
    GetTextExtentPoint32W(c->Handle, L"000", 3, &s);
    w2 = s.cx;
    c->Font->Style = TFontStyles();
    GetTextExtentPoint32W(c->Handle, L"Words: ", 7, &s);
    w3 = s.cx;
    c->Font->Style = TFontStyles() << fsBold;
    GetTextExtentPoint32W(c->Handle, L"000", 3, &s);
    w4 = s.cx;
    c->Font->Style = TFontStyles();
    GetTextExtentPoint32W(c->Handle, L"Current: ", 9, &s);
    w5 = s.cx;
    c->Font->Style = TFontStyles() << fsBold;
    GetTextExtentPoint32W(c->Handle, L"000", 3, &s);
    w6 = s.cx;

    int x = (p->Width - w1 - w2 - DRAW_INFO_GAP - w3 - w4 - DRAW_INFO_GAP - w5 - w6) / 2, y = (p->Height - h) / 2;

    r = Rect(x, y, x + w1, y + h);
    c->Font->Style = TFontStyles();
    ExtTextOutW(c->Handle, x, y, ETO_CLIPPED, &r, L"Round #", 7, NULL);

    x += w1;
    r = Rect(x, y, x + w2, y + h);
    c->Font->Style = TFontStyles() << fsBold;
    SetTextAlign(c->Handle, TA_TOP | TA_CENTER);
    ExtTextOutW(c->Handle, x + w2 / 2, y, ETO_CLIPPED, &r, IntToStr(fround).c_str(), IntToStr(fround).Length(), NULL);
    x += w2 + DRAW_INFO_GAP;

    c->Font->Style = TFontStyles();
    r = Rect(x, y, x + w3, y + h);
    SetTextAlign(c->Handle, TA_TOP | TA_LEFT);
    ExtTextOutW(c->Handle, x, y, ETO_CLIPPED, &r, L"Words: ", 7, NULL);
    x += w3;

    c->Font->Style = TFontStyles() << fsBold;
    r = Rect(x, y, x + w4, y + h);
    SetTextAlign(c->Handle, TA_TOP | TA_CENTER);
    int words = fincluded - fstartpos;
    for (int i = fstartpos; i < fincluded; i++)
        if (fanswers->Items[fwords->Items[i]]->skip != 0)
            words--;
    ExtTextOutW(c->Handle, x + w4 / 2, y, ETO_CLIPPED, &r, IntToStr(words).c_str(), IntToStr(words).Length(), NULL);
    x += w4 + DRAW_INFO_GAP;

    c->Font->Style = TFontStyles();
    r = Rect(x, y, x + w5, y + h);
    SetTextAlign(c->Handle, TA_TOP | TA_LEFT);
    ExtTextOutW(c->Handle, x, y, ETO_CLIPPED, &r, L"Current: ", 9, NULL);
    x += w5;

    c->Font->Style = TFontStyles() << fsBold;
    r = Rect(x, y, x + w6, y + h);
    SetTextAlign(c->Handle, TA_TOP | TA_CENTER);
    words = current + 1;
    for (int i = 0; i < current; i++)
        if (fanswers->Items[fwordstore->Items[i]]->skip != 0)
            words--;
    ExtTextOutW(c->Handle, x + w6 / 2, y, ETO_CLIPPED, &r, IntToStr(words).c_str(), IntToStr(words).Length(), NULL);
#undef DRAW_INFO_GAP
}
//---------------------------------------------------------------------------
void TWordGroupM1::StartRound(bool firstround)
{
    // Don't start a round when we are in the middle of one. Think about suspended tests.
    if (fincluded && current < lastindex)
    {
        Review(false);
        if (!firstround)
            SaveStats();
        return;
    }

    fround++;
    Review(true);

    bool wronged = false;
    for (int i = 0; i < fincluded - fstartpos && !wronged; i++)
        wronged = fanswers->Items[fwordstore->Items[i]]->meaningerror > 0 || fanswers->Items[fwordstore->Items[i]]->kanaerror > 0 || fanswers->Items[fwordstore->Items[i]]->kanjierror > 0;

    lastanswered = false;
    fcurrent = -1;

    if (fincluded < finitial)
        fincluded = finitial;
    else if (!wronged)
    {
        if (fincluded < fwords->Count)
            fincluded = MIN(fincluded + fwordadd, fwords->Count);
        else
            fstartpos = MIN(fstartpos + fwordadd, fwords->Count - finitial);
    }

    fwrong = 0;
    fcorrect = 0;

    fwordstore->Clear();
    EM1Order emo;
    emo = forder;
    int rnd;
    TIntegerList *l;
    switch (emo)
    {
    case em1Added:
        for (int i = fstartpos; i < fincluded; i++)
            fwordstore->Add(fwords->Items[i]);
        break;
    case em1Backwards:
        for (int i = fstartpos; i < fincluded; i++)
            fwordstore->Add(fwords->Items[fincluded - i + fstartpos - 1]);
        break;
    case em1Random:
        l = new TIntegerList;
        for (int i = fstartpos; i < fincluded; i++)
            l->Add(fwords->Items[i]);
        while (l->Count)
        {
            rnd = Random(l->Count);
            fwordstore->Add(l->Items[rnd]);
            l->Delete(rnd);
        }
        delete l;
        break;
    }

    if (!firstround)
        SaveStats();
}
//---------------------------------------------------------------------------
void TWordGroupM1::Review(bool change)
{
    if (fincluded < finitial)
        inherited::Review(0, finitial - 1, false, false, fselection);
    else if (!RoundLastWord())
        inherited::Review(fstartpos, fincluded - 1, false, false, fselection);
    else if (fincluded < fwords->Count)
        inherited::Review(fstartpos, MIN(fincluded + fwordadd, fwords->Count) - 1, fstartpos, fincluded - 1, change, true, fselection);
    else
        inherited::Review(MIN(fstartpos + fwordadd, fwords->Count - finitial), fincluded - 1, fstartpos, fincluded - 1, change, true, fselection);
    fpos = fincluded;

    //if (change)
    //SaveStats();
}
//---------------------------------------------------------------------------
int TWordGroupM1::get_currentindex()
{
    if (fcurrent < 0)
        return -1;
    return fwordstore->Items[fcurrent];
}
//---------------------------------------------------------------------------
void TWordGroupM1::Step()
{
    do
    {
        fcurrent = MIN(fcurrent + 1, fwordstore->Count - 1);
    }
    while (fcurrent < fwordstore->Count - 1 && fanswers->Items[fwordstore->Items[fcurrent]]->skip != 0);
}
//---------------------------------------------------------------------------
TModalResult TWordGroupM1::Suspend(bool ashown)
{
    int r = inherited::Suspend(ashown);
    if (r != mrCancel && !ashown)
        fpos = fstartpos;

    return r;
}
//---------------------------------------------------------------------------
void TWordGroupM1::Answer(bool correct)
{
    if (RoundLastWord())
        lastanswered = true;

    inherited::Answer(correct);
}
//---------------------------------------------------------------------------
bool TWordGroupM1::LastWord()
{
    return !fwrong && fcurrent + fstartpos == fwords->Count - 1 && fstartpos == fincluded - finitial;
}
//---------------------------------------------------------------------------
int TWordGroupM1::get_lastindex()
{
    int tmp = fwordstore->Count - 1;
    while (tmp > 0 && fanswers->Items[fwordstore->Items[tmp]]->skip != 0)
        tmp--;
    return tmp;
}
//---------------------------------------------------------------------------
bool TWordGroupM1::RoundLastWord()
{
    return current == lastindex;
}
//---------------------------------------------------------------------------
void TWordGroupM1::LoadFromFile(FILE *f, byte ver)
{
    inherited::LoadFromFile(f, ver);

    fread(&finitial, sizeof(byte), 1, f);
    fread(&forder, sizeof(EM1Order), 1, f);
    fread(&frandomize, sizeof(bool), 1, f);
    if (ver >= 2)
    {
        fread(&fwordadd, sizeof(byte), 1, f);
        fread(&fselection, sizeof(byte), 1, f);
        fread(&fuseglobal, sizeof(bool), 1, f);
    }
    else
    {
        fwordadd = 1;
        fselection = 0;
        fuseglobal = false;
    }

    fread(&fincluded, sizeof(int), 1, f);
    fread(&fpos, sizeof(int), 1, f);
    fread(&fstartpos, sizeof(int), 1, f);

    int c, j;
    fwordstore->Clear();
    fread(&c, sizeof(int), 1, f);
    fwordstore->Capacity = c;
    for (int i = 0; i < c; i++)
    {
        fread(&j, sizeof(int), 1, f);
        fwordstore->Add(j);
    }
}
//---------------------------------------------------------------------------
void TWordGroupM1::SaveToFile(FILE *f)
{
    inherited::SaveToFile(f);

    fwrite(&finitial, sizeof(byte), 1, f);
    fwrite(&forder, sizeof(EM1Order), 1, f);
    fwrite(&frandomize, sizeof(bool), 1, f);
    fwrite(&fwordadd, sizeof(byte), 1, f);
    fwrite(&fselection, sizeof(byte), 1, f);
    fwrite(&fuseglobal, sizeof(bool), 1, f);

    fwrite(&fincluded, sizeof(int), 1, f);
    fwrite(&fpos, sizeof(int), 1, f);
    fwrite(&fstartpos, sizeof(int), 1, f);

    int c = fwordstore->Count;
    fwrite(&c, sizeof(int), 1, f);
    for (int i = 0; i < fwordstore->Count; i++)
    {
        c = fwordstore->Items[i];
        fwrite(&c, sizeof(int), 1, f);
    }
}
//---------------------------------------------------------------------------
//void TWordGroupM1::ExportHeader(FILE *f)
//{
//    inherited::ExportHeader(f);
//    UnicodeString z;
//
//    z = (UnicodeString)L"\tinitial " + (int)finitial + L"\t" + CEM1Order[forder] + L"\t" + (frandomize ? L"randomize" : L"norandomize") + L"\t" + L"add in round " + (int)fwordadd + L"\tselection " + (int)fselection + L"\t" + (fuseglobal ? L"useglobal" : L"nouseglobal");
//    fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
//}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordGroupM2::TWordGroupM2(TWordCollection *acollection, UnicodeString aname, TStudyQuestionType aquestion, TStudyAnswerType aanswer, byte atimer, bool ahidekana, int ainclusion, int anumber) : inherited(acollection, aname, smRefresh, aquestion, aanswer, atimer, ahidekana)
{
    finclusion = ainclusion;
    fnumber = anumber;
}
//---------------------------------------------------------------------------
TWordGroupM2::TWordGroupM2(TWordCollection *acollection, UnicodeString aname) : inherited(acollection, aname, smRefresh)
{
    ;
}
//---------------------------------------------------------------------------
TWordGroupM2::~TWordGroupM2()
{
}
//---------------------------------------------------------------------------
TWordList *wordlist_of_sort;
int method_of_random_sort;
TWordIndexList *list_of_indexes_to_sort;

int word_sort_for_random(int a, int b)
{
    TWord *w1 = wordlist_of_sort->Items[list_of_indexes_to_sort->Items[a]->index];
    byte m1 = list_of_indexes_to_sort->Items[a]->meaning;
    TWord *w2 = wordlist_of_sort->Items[list_of_indexes_to_sort->Items[b]->index];
    byte m2 = list_of_indexes_to_sort->Items[b]->meaning;

    // "1Number of inclusions", "2Date of last inclusion", "3Global score", "4Word popularity"

    // Get a number between 10 and -10 in every comparison
    double inc;
    if (!w1->stats[m1].included && !w2->stats[m2].included) // Don't divide by zero.
        inc = 0;
    else if (!w2->stats[m2].included)
        inc = 10;
    else if (!w1->stats[m1].included)
        inc = -10;
    else
        inc = (double)((w1->stats[m1].included - w2->stats[m2].included) * 10) / MAX(w1->stats[m1].included, w2->stats[m2].included);

    if ((method_of_random_sort & 0x3) == 0)
        inc *= 1.5;
    else if ((method_of_random_sort & 0x3) == 1)
        inc *= 1.3;
    else if ((method_of_random_sort & 0x3) == 2)
        inc *= 1.2;

    word year1, year2, month1, month2, day1, day2, hour1, hour2, min1, min2, sec1, sec2, msec1, msec2;
    DecodeDateTime(w1->stats[m1].testtime, year1, month1, day1, hour1, min1, sec1, msec1);
    DecodeDateTime(w2->stats[m2].testtime, year2, month2, day2, hour2, min2, sec2, msec2);

    int hourdiff = (((year1 - year2) * 12 + (month1 - month2)) * 30 + (day1 - day2)) * 24 + (hour1 - hour2); // Number of "hours" between two dates.
    double date;
    if (!hourdiff)
        date = 0;
    else if (hourdiff < 7*24)
        date = 10;
    else if (hourdiff > 7 * 24)
        date = -10;
    else
        date = (double)(hourdiff * 10) / (7 * 24);

    if ((method_of_random_sort & 0x12) == 0)
        date *= 1.5;
    else if ((method_of_random_sort & 0x12) == 1)
        date *= 1.3;
    else if ((method_of_random_sort & 0x12) == 2)
        date *= 1.2;

    char r1 = w1->stats[m1].score, r2 = w2->stats[m2].score;
    double mis;
    if (r1 == r2) // One of them was never tested. No need to see this.
        mis = 0;
    else if (r1 > r2)
        mis = 10;
    else if (r2 > r1)
        mis = -10;
    else
        mis = ((double)(r1 - r2) * 10) / MAX(r1, r2);

    if ((method_of_random_sort & 0x48) == 0)
        mis *= 1.5;
    else if ((method_of_random_sort & 0x48) == 1)
        mis *= 1.3;
    else if ((method_of_random_sort & 0x48) == 2)
        mis *= 1.2;

    double pop;
    if (!w1->frequency && !w2->frequency) // Don't divide by zero.
        pop = 0;
    else if (!w2->frequency)
        pop = 10;
    else if (!w1->frequency)
        pop = -10;
    else
        pop = (double)((w1->frequency - w2->frequency) * 10) / MAX(w1->frequency, w2->frequency);

    if ((method_of_random_sort & 0x192) == 0)
        pop *= 1.5;
    else if ((method_of_random_sort & 0x192) == 1)
        pop *= 1.3;
    else if ((method_of_random_sort & 0x192) == 2)
        pop *= 1.2;

    return inc + date + mis + pop;
}
//---------------------------------------------------------------------------
bool TWordGroupM2::DoInit()
{
    if (!inherited::DoInit())
        return false;

    int rnd;

    // Copy all the words to an initial not ordered list, so we can remove the ones already in use.
    TIntegerList *l = new TIntegerList;
    // We might have deleted words.
    fnumber = MIN(fnumber, list->Count);
    for (int ix = 0; ix < list->Count; ix++)
    {
        if ((question == sqKanji || question == sqKOrK) && !has_kanji(list->Words[ix]->kanji))
            continue;
        l->Add(ix);
    }

    // First sort the list, the smaller an index the better.
    wordlist_of_sort = words;
    method_of_random_sort = finclusion;
    list_of_indexes_to_sort = list;
    l->Sort(word_sort_for_random);
    while (fwords->Count != fnumber && l->Count)
    {
        rnd = abs((Random(l->Count) + Random(l->Count) + 2) - l->Count - 1);
        if (l->Count <= rnd)
            continue;
        fwords->Add(l->Items[rnd]);
        l->Delete(rnd);
    }

    delete l;
    return false;
}
//---------------------------------------------------------------------------
void TWordGroupM2::DrawInfo(TPaintBox *p)
{
    TCanvas *c = p->Canvas;

    TRect r;
    TSize s;

    c->Font->Name = settings.romajifont;
    c->Font->Charset = DEFAULT_CHARSET;
    c->Font->Size = p->Height * 0.4;
    SetTextAlign(c->Handle, TA_TOP | TA_LEFT);

    int w1, w2, w3, w4, h; //width estimation
    c->Font->Style = TFontStyles();
    GetTextExtentPoint32A(c->Handle, "Wrong / Correct: ", 17, &s);
    w1 = s.cx;
    h = s.cy;
    c->Font->Style = TFontStyles() << fsBold;
    GetTextExtentPoint32W(c->Handle, L"000 / 000", 9, &s);
    w2 = s.cx;
    c->Font->Style = TFontStyles();
    GetTextExtentPoint32W(c->Handle, L"Remaining: ", 11, &s);
    w3 = s.cx;
    c->Font->Style = TFontStyles() << fsBold;
    GetTextExtentPoint32W(c->Handle, L"000", 3, &s);
    w4 = s.cx;

#define DRAW_INFO_GAP        20
    int x = (p->Width - w1 - w2 - w3 - w4 - DRAW_INFO_GAP) / 2, y = (p->Height - h) / 2;

    r = Rect(x, y, x + w1, y + h);
    c->Font->Style = TFontStyles();
    ExtTextOutW(c->Handle, x, y, ETO_CLIPPED, &r, L"Wrong / Correct: ", 17, NULL);

    x += w1;
    r = Rect(x, y, x + w2, y + h);
    c->Font->Style = TFontStyles() << fsBold;
    SetTextAlign(c->Handle, TA_TOP | TA_CENTER);
    ExtTextOutW(c->Handle, x + w2 / 2, y, ETO_CLIPPED, &r, (IntToStr(fwrong) + " / " + IntToStr(fcorrect)).c_str(), (IntToStr(fwrong) + " / " + IntToStr(fcorrect)).Length(), NULL);
    x += w2 + DRAW_INFO_GAP;

    c->Font->Style = TFontStyles();
    r = Rect(x, y, x + w3, y + h);
    SetTextAlign(c->Handle, TA_TOP | TA_LEFT);
    ExtTextOutW(c->Handle, x, y, ETO_CLIPPED, &r, L"Remaining: ", 11, NULL);
    x += w3;

    c->Font->Style = TFontStyles() << fsBold;
    r = Rect(x, y, x + w4, y + h);
    SetTextAlign(c->Handle, TA_TOP | TA_CENTER);
    ExtTextOutW(c->Handle, x + w4 / 2, y, ETO_CLIPPED, &r, IntToStr(MAX(0, fwords->Count - current - 1)).c_str(), IntToStr(MAX(0, fwords->Count - current - 1)).Length(), NULL);
}
//---------------------------------------------------------------------------
void TWordGroupM2::StartRound(bool firstround)
{
    if (MsgBox("Would you like to review the words?", "Message", MB_YESNO) == IDNO)
    {
        if (!firstround)
            SaveStats();
        return;
    }
    Review(!firstround);
}
//---------------------------------------------------------------------------
void TWordGroupM2::LoadFromFile(FILE *f, byte ver)
{
    inherited::LoadFromFile(f, ver);

    fread(&finclusion, sizeof(int), 1, f);
    fread(&fnumber, sizeof(int), 1, f);
}
//---------------------------------------------------------------------------
void TWordGroupM2::SaveToFile(FILE *f)
{
    inherited::SaveToFile(f);

    fwrite(&finclusion, sizeof(int), 1, f);
    fwrite(&fnumber, sizeof(int), 1, f);
}
//---------------------------------------------------------------------------
//void TWordGroupM2::ExportHeader(FILE *f)
//{
//    inherited::ExportHeader(f);
//    UnicodeString z;
//
//    int finclusion; //priority when including new words
//    int fnumber; //number of words to test
//    z = (UnicodeString)L"\torder " + finclusion + L"\tnumber tested " + fnumber;
//    fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
//}
//---------------------------------------------------------------------------
bool WordGroupStudyValid(TStudyQuestionType qtype, TWordIndexList *list, bool showmsg)
{
    int validcnt = list->Count;
    if (qtype == sqKanji || qtype == sqKOrK)
    {
        for (int ix = 0; ix < list->Count; ++ix)
            if (!has_kanji(list->Words[ix]->kanji))
                --validcnt;
    }
    if (validcnt < 10)
    {
        if (showmsg)
            MsgBox(L"The test cannot start if there are less than 10 usable words added to the study group. When testing your knowledge of kanji, make sure the words contain kanji characters.", L"Message", MB_OK);
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
#undef DRAW_INFO_GAP
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(185);
#endif


