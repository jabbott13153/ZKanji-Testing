//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "dictupdater.h"
#include "collection.h"
#include "words.h"
#include "wordgroup.h"
#include "kanji.h"
#include "kanjicard.h"
#include "studylist.h"
#include "wordobj.h"
#include "settings.h"
#include "module.h"
#include "kanjiedit.h"
#include "romajizer.h"
#include "gengraph.h"
#include "qsrt.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(218);
#endif

TfDictUpdater *fDictUpdater;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordStateList::TWordStateList() : coll(NULL)
{

}
//---------------------------------------------------------------------------
TWordStateList::~TWordStateList()
{
    Clear();
}
//---------------------------------------------------------------------------
void TWordStateList::FreeItem(TWordState *w, int ix)
{
    delete[] w->groups;
    delete[] w->kanji;
    delete w;
}
//---------------------------------------------------------------------------
int TWordStateList::SortCompare(int aindex, int bindex)
{
    if (aindex == bindex)
        return 0;

    TWord *a = coll->words->Items[aindex];
    TWord *b = coll->words->Items[bindex];

    int r = wcscmp(a->kanji, b->kanji);
    if (!r)
        r = wcscmp(a->kana, b->kana);
    return r;
}
//---------------------------------------------------------------------------
int TWordStateList::FindPosition(int wordix)
{
    int min = 0, max = Count - 1, mid;
    while (min <= max)
    {
        mid = (min + max) / 2;
        int d = SortCompare(wordix, Items[mid]->windex);
        if (d < 0)
            max = mid - 1;
        else if (d > 0)
            min = mid + 1;
        else
        {
            min = mid;
            break;
        }
    }

    return min;
}
//---------------------------------------------------------------------------
int TWordStateList::Add(int wordix, TWordStateExpand exp, int data)
{
    int pos = FindPosition(wordix);
    TWordState *w;
    if (Count <= pos || Items[pos]->windex != wordix)
    {
        w = new TWordState;
        memset(w, 0, sizeof(TWordState));
        w->windex = wordix;
        w->newindex = -1;
        w->matched = false;
        w->action = TWordState::saUpdate;
        w->origix = -1;
        //w->orig = NULL;
        w->longtermix = -1;
        w->meaningredefined = false;
        inherited::Insert(pos, w);
    }
    else
        w = Items[pos];

    switch (exp)
    {
    case wseWordGroup:
    case wseStudyGroup:
        w->groupcnt++;
        break;
    case wseKanji:
        w->kanjicnt++;
        break;
    case wseLongTerm:
        w->longtermix = data;
        break;
    case wseMeaningDef:
        w->meaningredefined = true;
        break;
    case wseOriginal:
        w->origix = data;
        //w->orig = (TOriginalWord*)data;
        break;
    default:;
    }

    return pos;
}
//---------------------------------------------------------------------------
void TWordStateList::Allocate()
{
    for (int ix = 0; ix < Count; ++ix)
    {
        TWordState *w = Items[ix];
        if (w->groupcnt)
        {
            w->groups = new TWordState::TMeaningGroup[w->groupcnt];
            memset(w->groups, 0, sizeof(TWordState::TMeaningGroup) * w->groupcnt);
        }
        if (w->kanjicnt)
        {
            w->kanji = new TWordState::TKanjiExample[w->kanjicnt];
            memset(w->kanji, 0, sizeof(TWordState::TKanjiExample) * w->kanjicnt);
        }

        w->groupcnt = 0;
        w->kanjicnt = 0;
    }
}
//---------------------------------------------------------------------------
void TWordStateList::AddGroup(int windex, int group, int pos, byte meaning, bool study)
{
    int ix = FindPosition(windex);
    TWordState *w = Items[ix];
    TWordState::TMeaningGroup &mg = w->groups[w->groupcnt];
    mg.study = study;
    mg.group = group;
    mg.pos = pos;
    mg.meaning = meaning;
    mg.newmeaning = -1;
    w->groupcnt++;
}
//---------------------------------------------------------------------------
void TWordStateList::AddKanji(int windex, int kindex, int eindex)
{
    int pos = FindPosition(windex);
    TWordState *w = Items[pos];
    w->kanji[w->kanjicnt].kanji = kindex;
    w->kanji[w->kanjicnt].pos = eindex;
    w->kanjicnt++;
}
//---------------------------------------------------------------------------
void TWordStateList::Build(TWordCollection *acoll, TOriginalWordsList *aorig)
{
    coll = acoll;
    orig = aorig;

    // First round:
    // Add all words that are included in groups.
    TWordGroupList *wg = coll->groups;
    for (int ix = 0; ix < wg->Count; ++ix)
    {
        TWordGroup *g = wg->Items[ix];
        for (int iy = 0; iy < g->list->Count; ++iy)
            Add(g->list->Indexes[iy], wseWordGroup, 0);
    }

    // Add all words that are included in study groups.
    wg = coll->studygroups;
    for (int ix = 0; ix < wg->Count; ++ix)
    {
        TWordGroup *g = wg->Items[ix];
        for (int iy = 0; iy < g->list->Count; ++iy)
            Add(g->list->Indexes[iy], wseStudyGroup, 0);
    }

    // Add all words that are included as kanji example.
    for (int ix = 0; ix < kanji_count; ++ix)
    {
        TKanjiData *d = coll->kanjidat + ix;
        if (!d->card)
            continue;
        for (int iy = 0; iy < d->card->ExampleCount; ++iy)
            Add(d->card->Examples[iy]->ix, wseKanji, 0);
    }

    // Add those that are in the long-term study.
    for (int ix = 0; ix < coll->wordstudylist->Count; ++ix)
        Add(coll->wordstudylist->wordindex(ix), wseLongTerm, ix);

    // Add words with redefined meanings on kanji cards.
    for (unsigned int ix = 0; ix < coll->deflist->TrueCount; ++ix)
        Add(coll->deflist->TruePos[ix], wseMeaningDef, 0);

    if (orig) // Add words that were changed by the user, but only for the main dictionary when orig is specified.
    {
        TWordIndexList *il = new TWordIndexList(coll);
        for (int j = 0; j < orig->Count; ++j)
        {
            int ix = FindWordIndex(il, orig->Items[j]->kanji, orig->Items[j]->kana);
            if (ix >= 0)
                Add(ix, wseOriginal, j/*(int)orig->Items[j]*/);
            else
                MsgBox(UnicodeString("Invalid item in the changed word records. Not found: ") + orig->Items[j]->kanji + L", [" + orig->Items[j]->kana + L"]", "Warning", MB_OK);
            il->Clear();
        }
        delete il;
    }

    // Allocate needed space for all words.
    Allocate();

    // Fill list with meaningful data.
    wg = coll->groups;
    for (int ix = 0; ix < wg->Count; ++ix)
    {
        TWordGroup *g = wg->Items[ix];
        for (int iy = 0; iy < g->list->Count; ++iy)
            AddGroup(g->list->Indexes[iy], ix, iy, g->list->Items[iy]->meaning, false);
    }

    wg = coll->studygroups;
    for (int ix = 0; ix < wg->Count; ++ix)
    {
        TWordGroup *g = wg->Items[ix];
        for (int iy = 0; iy < g->list->Count; ++iy)
            AddGroup(g->list->Indexes[iy], ix, iy, g->list->Items[iy]->meaning, true);
    }

    for (int ix = 0; ix < kanji_count; ++ix)
    {
        TKanjiData *d = coll->kanjidat + ix;
        if (!d->card)
            continue;
        for (int iy = 0; iy < d->card->ExampleCount; ++iy)
            AddKanji(d->card->Examples[iy]->ix, ix, iy);
    }
}
//---------------------------------------------------------------------------
TWord* TWordStateList::get_word(int ix)
{
    if (!coll)
        THROW(L"Collection not defined yet! Call 'Build' first!");

    return coll->words->Items[Items[ix]->windex];
}
//---------------------------------------------------------------------------
bool TWordStateList::Convert(TGapList<int> &changes, TWordCollection *adest, UnicodeString updatetext, UnicodeString aborttext)
{
    bool allmatched = true; // Every word matched in the new dictionary.
    dest = adest;

    // Check which word needs interaction. Words that do not need it will have their newindex set to a word index in the new dictionary.
    for (int ix = 0; ix < Count; ++ix)
    {
        TWordState *w = Items[ix];
        w->newindex = changes.ItemSet(w->windex) ? changes.Items[w->windex] : -1;

        bool versiondiff = false; // True when the user changed a word in the dictionary, and the word in the new dictionary and the original unchanged word differs.
        if (w->origix >= 0) // Check whether the word wasn't really updated in the new dictionary, only the user edited it. If the original is the same as the new one, do nothing with it.
        {
            TWord *d = w->newindex >= 0 ? dest->words->Items[w->newindex] : NULL;
            TOriginalWord *o = orig->Items[w->origix];
            if (!d || d->meaningcnt != o->meaningcnt) // The original can't be the same as the new one if they don't have the same number of meanings.
                versiondiff = true;

            for (byte b = 0; !versiondiff && b < d->meaningcnt; ++b)
                if (!MeaningDataMatch(d->data[b], o->data[b])) // Difference found, mark it as different.
                    versiondiff = true;

            if (!versiondiff) // The original word is the same as the one in the updated dictionary. Force copy the user changed word over the new one.
            {
                w->action = TWordState::saCopyOld;
                w->matched = true;
                continue;
            }
        }

        if (w->origix < 0 && w->groupcnt == 0 && w->newindex >= 0) // The word is not in groups, so no need to check meanings. The new dictionary contains an entry with the same kanji and kana, take it as matching even if it does not.
        {
            w->matched = true;
            continue;
        }

        if (w->newindex < 0) // The word was not found in the updated dictionary. If it was added by the user, force copy over the new one and no further handling is necessary.
        {
            if (w->origix >= 0 && orig->Items[w->origix]->change == owcAdded)
            {
                w->action = TWordState::saCopyOld;
                w->matched = true;
            }
            else // The word wasn't added by the user and the new dictionary does not contain it. No need to do anything with it, just remember that there is at least one difference so the dialog must be shown.
                allmatched = false;
            continue;
        }

        // The word is in a word group or was changed by the user so its meanings must be checked, and if a difference with the new data found, mark it as different.
        bool missingmeaning = w->origix >= 0;
        // Check the meaning of words that are in word groups and update the meaning indexes for the new dictionary if a match found.
        for (int iy = 0; iy < w->groupcnt; ++iy)
        {
            // Assume that the meaning index might be the same and check that first.
            if (dest->words->Items[w->newindex]->meaningcnt > w->groups[iy].meaning && MeaningDataMatch(coll->words->Items[w->windex]->data[w->groups[iy].meaning], dest->words->Items[w->newindex]->data[w->groups[iy].meaning]))
            {
                w->groups[iy].newmeaning = w->groups[iy].meaning;
                continue;
            }

            if (coll->words->Items[w->windex]->meaningcnt == 1 && dest->words->Items[w->newindex]->meaningcnt == 1)
            {
                // If we only have a single meaning, pairing the old and the new is easy.
                w->groups[iy].newmeaning = 0;
            }
            else
            {
                // Find an exact meaning with a different index in the new dictionary.
                for (int z = 0; z < dest->words->Items[w->newindex]->meaningcnt; ++z)
                {
                    if (z != w->groups[iy].meaning && MeaningDataMatch(coll->words->Items[w->windex]->data[w->groups[iy].meaning], dest->words->Items[w->newindex]->data[z]))
                    {
                        w->groups[iy].newmeaning = z;
                        break;
                    }
                    else if (z == dest->words->Items[w->newindex]->meaningcnt - 1)
                        missingmeaning = true;
                }
            }
        }

        if (missingmeaning)
        {
            allmatched = false;
            continue;
        }

        w->matched = true;
    }

    // Every word's every meaning had a match in the new dictionary, nothing to do.
    if (allmatched)
        return true;

    if (!updatetext.IsEmpty())
        MsgBox(updatetext, L"Dictionary changed", MB_OK);

    fDictUpdater = new TfDictUpdater(Application);
    int r;
    try
    {
        fDictUpdater->AbortText = aborttext;
        r = fDictUpdater->ShowModalEx(this);
    }
    catch (...)
    {
        r = mrAbort;
    }
    delete fDictUpdater;

    return r == mrOk;
}
//---------------------------------------------------------------------------
int wordstatemeaninggroupcmpproc(TWordState::TMeaningGroup *a, TWordState::TMeaningGroup *b)
{
    if (a->study != b->study)
        return a->study ? 1 : -1;

    if (a->group != b->group)
        return a->group - b->group;

    return a->newmeaning - b->newmeaning;
}
void TWordStateList::Commit()
{
    DISABLEMOD

    // Update collection groups and tests to reflect new indexes.
    for (int ix = 0; ix < Count; ++ix)
    {
        TWordState *s = Items[ix];
        TWord *w = coll->words->Items[s->windex];
        TWord *d;

        if (s->action == TWordState::saCopyOld) // Use the original definition from the old dictionary.
        {
            for (int iy = 0; iy < s->groupcnt; ++iy)
            {
                TWordState::TMeaningGroup *g = s->groups + iy;
                g->newmeaning = g->meaning;
            }
            if (s->newindex < 0) // Must add entirely new word to dictionary.
            {
                if (s->origix >= 0) // User modified word, must be updated in originals list too.
                {
                    TOriginalWord *o = orig->Items[s->origix];
                    o->change = owcAdded;
                    if (o->meaningcnt)
                    {
                        delete_word_meanings(o->meaningcnt, o->data);
                        o->meaningcnt = 0;
                        o->data = NULL;
                    }
                }
                else if (orig) // Word not in the originals list. Add it because dest->AddWord does not do it.
                    s->origix = orig->Add(w, owcAdded);

                s->newindex = dest->AddWord(w, -1, UnicodeString());
            }
            else // Replace word in the new dictionary with the definitions of the old one.
            {
                // Check if the current definition matches the new dictionary.
                d = dest->words->Items[s->newindex];
                bool match = d->meaningcnt == w->meaningcnt;
                for (int iy = 0; iy < d->meaningcnt && match; ++iy)
                    match = MeaningDataMatch(d->data[iy], w->data[iy]);

                if (s->origix >= 0) // User modified word, must be updated in originals list too.
                {
                    TOriginalWord *o = orig->Items[s->origix];

                    if (match)
                        o->change = owcNone;
                    else
                    {
                        delete_word_meanings(o->meaningcnt, o->data);

                        o->meaningcnt = d->meaningcnt;
                        o->data = new TMeaningData[o->meaningcnt];
                        for (int iy = 0; iy < o->meaningcnt; ++iy)
                        {
                            o->data[iy].meaning = new wchar_t[wcslen(d->data[iy].meaning) + 1];
                            wcscpy(o->data[iy].meaning, d->data[iy].meaning);
                        }
                        o->change = owcModified;
                    }

                }
                else if (!match && orig) // Must be added to originals if the words do not match.
                    s->origix = orig->Add(d, owcModified);

                dest->ReplaceWord(s->newindex, w);
            }
        }
        else if (s->origix >= 0) // saUpdate or saRemove. Delete original data as the new dictionary's definition will be used.
            orig->Items[s->origix]->change = owcNone;

        if (s->newindex >= 0 && s->action != TWordState::saRemove)
            d = dest->words->Items[s->newindex];
        else
            d = NULL;

        if (d && w->stats)
        {
            delete[] d->stats;
            d->stats = new TWordStatGlobal[d->meaningcnt];
            memset(d->stats, 0, sizeof(TWordStatGlobal) * d->meaningcnt);
        }
        if (s->groupcnt > 1)
            quicksort(s->groups, 0, s->groupcnt - 1, wordstatemeaninggroupcmpproc);

        //if (!d)
        //{
            while (w->group)
            {
                TWordGroups *tmp = w->group;
                w->group = tmp->next;
                delete tmp;
            }
        //}

        for (int iy = 0; iy < s->groupcnt; ++iy)
        {
            TWordState::TMeaningGroup *g = s->groups + iy;
            TWordGroup *wg = (!g->study ? coll->groups : coll->studygroups)->Items[g->group];
            TWordIndex *wi = wg->list->Items[g->pos];

            // Filling the groups of the destination word.
            // UNTESTED:
            if (d && g->newmeaning != -1)
            {
                // Check whether the same meaning would be added twice for the same group after the update, and remove the excess meaning.
                TWordGroups *tmp = d->group;
                while (tmp)
                {
                    if (tmp->group == wg && tmp->meaning == g->meaning)
                    {
                        g->newmeaning = -1;
                        break;
                    }
                    tmp = tmp->next;
                }

                // tmp is only NULL if the group/meaning pair didn't exist previously.
                if (tmp == NULL)
                {
                    tmp = new TWordGroups;
                    tmp->group = wg;
                    tmp->meaning = g->newmeaning;
                    TWordGroups *old = d->group;
                    d->group = tmp;
                    tmp->next = old;
                }
            }

/*
            if (d)
            {
                TWordGroups *tmp = w->group;
                TWordGroups *prev = NULL;
                while (tmp)
                {
                    if (tmp->group == wg && tmp->meaning == g->meaning)
                    {
                        if (g->newmeaning >= 0)
                            tmp->meaning = g->newmeaning;
                        else
                        {
                            if (!prev)
                                w->group = tmp->next;
                            else
                                prev->next = tmp->next;
                            delete tmp;
                        }
                        break;
                    }
                    prev = tmp;
                    tmp = tmp->next;
                }
            }
*/
            if (!d || g->newmeaning < 0 || (iy && s->groups[iy - 1].study == s->groups[iy].study && s->groups[iy - 1].group == s->groups[iy].group && s->groups[iy - 1].meaning == s->groups[iy].meaning))
            {
                wi->index = -1;
                continue;
            }
            wi->index = s->newindex;
            wi->meaning = g->newmeaning;

            if (w->stats)
                memcpy(&d->stats[g->newmeaning], &w->stats[g->meaning], sizeof(TWordStatGlobal));
        }

/*
        if (d)
        {
            d->group = w->group;
            w->group = NULL;
        }
*/

        for (int iy = 0; iy < s->kanjicnt; ++iy)
        {
            TKanjiData *kd = coll->kanjidat + s->kanji[iy].kanji;
            kd->card->Examples[s->kanji[iy].pos]->ix = d ? s->newindex : -1;
        }

        if (s->longtermix >= 0)
            coll->wordstudylist->Items[s->longtermix]->fwordindex = d ? s->newindex : -1;

        if (d && s->meaningredefined) // Copy every redefined word definition to the destination. The two deflists will be swapped at the end of Commit(), but it's ok because the destination will be deleted.
            dest->deflist->Items[s->newindex] = coll->deflist->Items[s->windex];
    }

    if (orig)
    {
        for (int ix = orig->Count - 1; ix >= 0; --ix)
            if (orig->Items[ix]->change == owcNone)
                orig->Delete(ix);
    }

    for (int ix = 0; ix < coll->groups->Count; ++ix)
        coll->groups->Items[ix]->CleanUp();
    for (int ix = 0; ix < coll->studygroups->Count; ++ix)
        coll->studygroups->Items[ix]->CleanUp();
    for (int ix = 0; ix < kanji_count; ++ix)
    {
        TKanjiCard *c = coll->kanjidat[ix].card;
        if (c)
            c->CleanUp();
    }
    coll->wordstudylist->CleanUp();

    memswap(dest->words, coll->words);
    memswap(dest->rtree, coll->rtree);
    memswap(dest->ktree, coll->ktree);
    memswap(dest->btree, coll->btree);
    memswap(dest->abcde, coll->abcde);
    memswap(dest->aiueo, coll->aiueo);
    memswap(dest->deflist, coll->deflist);
    memswap(dest->abcfilter, coll->abcfilter);
    memswap(dest->setfilters, coll->setfilters);
    memswap(dest->validdat, coll->validdat);
    for (int ix = 0; ix < kanji_count; ++ix)
    {
        memswap(dest->kanjidat[ix].words, coll->kanjidat[ix].words);
        memswap(dest->kanjidat[ix].wordscnt, coll->kanjidat[ix].wordscnt);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/* TfDictUpdater form and helper classes. */

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TfDictUpdater::TWordReplacementList::FreeItem(TWordReplacement *r, int ix)
{
    delete[] r->meanings;
    delete r;
}
//---------------------------------------------------------------------------
int TfDictUpdater::TWordReplacementList::Add(int stateix, int newix)
{
    TWordReplacement *r = new TWordReplacement;
    r->stateindex = stateix;
    r->newindex = newix;
    r->meanings = NULL;
    r->meaningcnt = 0;
    r->action = TWordReplacement::raNormal;
    r->editstate = TWordReplacement::rsUntouched;
    return inherited::Add(r);
}
//---------------------------------------------------------------------------
void TfDictUpdater::TWordReplacementList::AddMeanings(int ix, TIntegerList *meanings)
{
    TWordReplacement *r = Items[ix];
    r->meaningcnt = meanings->Count;
    if (!r->meaningcnt)
        return;
    r->meanings = new TWordReplacement::TMeaningReplacement[meanings->Count];
    for (int ix = 0; ix < meanings->Count; ++ix)
    {
        r->meanings[ix].meaning = meanings->Items[ix];
        r->meanings[ix].newmeaning = 255;
    }
}
//---------------------------------------------------------------------------
void TfDictUpdater::TWordReplacementList::SetNewMeaning(int ix, byte meaning, byte newmeaning)
{
    if (meaning >= Items[ix]->meaningcnt)
        THROW(L"Index for meaning out of bounds. It is not the actual number of meanings for a word but the meanings that are in a group.");
    Items[ix]->meanings[meaning].newmeaning = newmeaning;
}
//---------------------------------------------------------------------------
void TfDictUpdater::TWordReplacementList::UnsetAll(int ix)
{
    for (int iy = 0; iy < Items[ix]->meaningcnt; ++iy)
        SetNewMeaning(ix, iy, 255);
}
//---------------------------------------------------------------------------
bool TfDictUpdater::TWordReplacementList::MeaningSet(int ix, byte meaning)
{
    if (meaning >= Items[ix]->meaningcnt)
        THROW(L"Index for meaning out of bounds. It is not the actual number of meanings for a word but the meanings that are in a group.");
    return Items[ix]->meanings[meaning].newmeaning != 255;
}
//---------------------------------------------------------------------------
int TfDictUpdater::TWordReplacementList::MeaningCount(int ix)
{
    return Items[ix]->meaningcnt;
}
//---------------------------------------------------------------------------
byte TfDictUpdater::TWordReplacementList::OldMeaning(int ix, byte meaning)
{
    if (meaning >= Items[ix]->meaningcnt)
        THROW(L"Index for meaning out of bounds. It is not the actual number of meanings for a word but the meanings that are in a group.");
    return Items[ix]->meanings[meaning].meaning;
}
//---------------------------------------------------------------------------
byte TfDictUpdater::TWordReplacementList::NewMeaning(int ix, byte meaning)
{
    if (meaning >= Items[ix]->meaningcnt)
        THROW(L"Index for meaning out of bounds. It is not the actual number of meanings for a word but the meanings that are in a group.");
    return Items[ix]->meanings[meaning].newmeaning;
}
//---------------------------------------------------------------------------
bool TfDictUpdater::TWordReplacementList::NewMeaningTaken(int ix, byte newmeaning)
{
    for (int z = 0; z < Items[ix]->meaningcnt; ++z)
        if (Items[ix]->meanings[z].newmeaning == newmeaning)
            return true;
    return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TfDictUpdater::TfDictUpdater(TComponent* Owner) : TBaseForm(Owner)
{
    EraseBackground = false;
    aborttext = "";
    usercall = true;
    wobj = new TVirtualWordListObj(wbox, rowsizes(settings.dictfontsize), wcallback, (TPopupWordProc)NULL);
    words = new TWordReplacementList;
    edkanji = new TKanjiEdit(pbKanji, true, true, NULL, ecallback);
    edkanji->EnableRecognizer(sbRecognize);
    dobj = new TWordListObj(pbDict, rowsizes(settings.dictfontsize), dcallback, (TPopupWordProc)NULL);
    msrc = new TVocabListObj(pbMSrc, rowsizes(settings.dictfontsize), mcallbacksrc, (TPopupWordProc)NULL);
    mdest = new TVocabListObj(pbMDest, rowsizes(settings.dictfontsize), mcallbackdest, (TPopupWordProc)NULL);
    listsrc = NULL;
    listdest = NULL;

    wobj->drawproc = wcelldraw;

    lbCopy->Font->Name = SystemFont;

    Constrain(Width - ClientWidth + Label8->Width + Label4->Width + 42 * scaleres, Height - ClientHeight + wbox->Top + (ClientHeight - (wbox->Top + wbox->Height)) + 48 * scaleres);
}
//---------------------------------------------------------------------------
__fastcall TfDictUpdater::~TfDictUpdater()
{
    delete edkanji;
    delete wobj;
    delete dobj;
    delete msrc;
    delete mdest;
    delete listsrc;
    delete listdest;
    delete words;
}
//---------------------------------------------------------------------------
int TfDictUpdater::ShowModalEx(TWordStateList *astates)
{
    states = astates;
    listsrc = new TWordIndexList(states->Collection);
    listdest = new TWordIndexList(states->Destination);
    msrc->wordlist = listsrc;
    mdest->wordlist = listdest;

    TWordCollection *dict = astates->Collection;
    int imgindex = dict->imgindex;
    if (imgindex < 0)
        imgindex = dictionary->Items[0] == dict ? 13 : 11;

    //data->aJtoE->ImageIndex = imgindex;
    //data->aEtoJ->ImageIndex = imgindex + 1;

    // empty 11, 12 - english 13, 14
    btnKanji->Glyph = NULL;
    btnRomaji->Glyph = NULL;
    data->imgMenu->BkColor = clLime;
    data->imgMenu->GetBitmap(imgindex, btnKanji->Glyph);
    data->imgMenu->GetBitmap(imgindex + 1, btnRomaji->Glyph);
    data->imgMenu->BkColor = Graphics::clNone;

    UnicodeString s = dictionary->Strings[dictionary->Index[dict]];
    btnKanji->Hint = L"Japanese to " + s;
    btnRomaji->Hint = s + L" to Japanese";

    TIntegerList *m = new TIntegerList; // Used in filling up meanings list for words. Collects all the meanings that are added to groups without duplicates.
    for (int ix = 0; ix < states->Count; ++ix)
    {
        TWordState *w = states->Items[ix];
        if (!w->matched)
        {
            words->Add(ix, w->newindex);
            for (int iy = 0; iy < w->groupcnt; ++iy)
                m->Add(w->groups[iy].meaning);
            m->Sort();
            for (int iy = m->Count - 1; iy >= 1; --iy)
                if (m->Items[iy] == m->Items[iy - 1])
                    m->Delete(iy);
            words->AddMeanings(words->Count - 1, m);
            m->Clear();

            MatchMeanings(words->Count - 1);
        }
    }
    delete m;

    int cw[4] = { rowsizes(settings.dictfontsize) - 1, 100 * scaleres, 120 * scaleres, 6250 * scaleres };
    TWordColumnType ct[4] = { wcCustom, wcKanji, wcKana, wcMeaning };
    wobj->set_cols(states->Collection, 4, cw, NULL, NULL, ct, NULL, words->Count, windexproc);
    wobj->colresize[0] = false;
    wobj->colresize[3] = false;

    return ShowModal();
}
//---------------------------------------------------------------------------
void TfDictUpdater::wcallback()
{
    TWordReplacement *r = words->Items[wobj->pos];
    TWordState *w = states->Items[r->stateindex];
    btnNext->Enabled = (w->origix < 0 || w->origix >= 0 && r->editstate != TWordReplacement::rsUntouched) && (r->newindex >= 0 || r->action == TWordReplacement::raRemove || r->action == TWordReplacement::raCopyOld);
    UpdateNextText();
    sbCopy->Down = r->action == TWordReplacement::raCopyOld;
    if (sbCopy->Down)
    {
        UpdateDisplay();
        return;
    }
    sbRemove->Down = words->Items[wobj->pos]->action == TWordReplacement::raRemove;
    if (sbRemove->Down)
    {
        UpdateDisplay();
        return;
    }

    if (r->newindex < 0)
    {
        ShowPage(0);
        edkanji->Text = states->Collection->words->Items[w->windex]->kana;
        sbMeanings->Enabled = false;
    }
    else
    {
        UpdateMeanings();
        if (w->origix >= 0 && r->editstate == TWordReplacement::rsUntouched)
        {
            sbMeanings->Enabled = false;
            lbOrig->Visible = true;
        }
        else
            lbOrig->Visible = false;

        if (sbMeanings->Enabled)
            ShowPage(1);
        else
            ShowPage(0);

        edkanji->Text = states->Destination->words->Items[r->newindex]->kanji;
        ecallback();
        for (int ix = 0; ix < dobj->count; ++ix)
            if (dobj->indexes[ix] == r->newindex)
            {
                dobj->pos = ix;
                break;
            }
    }

    lbLongTerm->Caption = w->longtermix >= 0 ? "YES" : "NO";
    UnicodeString str = "";
    for (int ix = 0; ix < w->kanjicnt; ++ix)
        str += kanjis->Kanjis[w->kanji[ix].kanji]->ch;
    if (str.Length() == 0)
        str = "-";
    lbEx->Caption = str;

    UpdateDisplay();
}
//---------------------------------------------------------------------------
int TfDictUpdater::windexproc(unsigned int ix)
{
    return states->Items[words->Items[ix]->stateindex]->windex;
}
//---------------------------------------------------------------------------
void TfDictUpdater::ecallback()
{
    UpdateWords();
}
//---------------------------------------------------------------------------
void TfDictUpdater::dcallback()
{
    btnSelect->Enabled = dobj->pos >= 0;
}
//---------------------------------------------------------------------------
void TfDictUpdater::mcallbacksrc()
{
    if (msrc->pos < 0)
    {
        lbGroups->Caption = "-";
        return;
    }
    int newn = words->Items[wobj->pos]->meanings[msrc->pos].newmeaning;
    mdest->pos = newn;
    TWord *w = states->Collection->words->Items[states->Items[words->Items[wobj->pos]->stateindex]->windex];
    UnicodeString str;
    TWordGroups *s = w->group;
    while (s)
    {
        if (str.Length())
            str += ", ";
        TWordGroup *g = s->group;
        str += g->name;
        s = s->next;
    }

    if (str.Length() == 0)
        str = "-";
    lbGroups->Caption = str;
}
//---------------------------------------------------------------------------
void TfDictUpdater::mcallbackdest()
{
    if (msrc->pos < 0 || mdest->pos < 0)
        return;

    if (words->NewMeaning(wobj->pos, msrc->pos) == mdest->pos)
        return;

    words->SetNewMeaning(wobj->pos, msrc->pos, mdest->pos);
    words->Items[wobj->pos]->editstate = TWordReplacement::rsProgress;
    UpdateNextText();
    wobj->invalidatecell(0, wobj->pos);
}
//---------------------------------------------------------------------------
void TfDictUpdater::UpdateWords()
{
    TWordIndexList *l = new TWordIndexList(states->Destination);

    bool haskanji = false;

    if (pKanji->Visible)
    {
        int i;
        for (i = edkanji->Text.Length(); i > 0 && !haskanji; i--)
            haskanji = KANJI(edkanji->Text[i]) || VALIDCODE(edkanji->Text[i]);
    }

    if (!haskanji)
    {
        if (pKanji->Visible)
            FindWord(TrimRomanized(edkanji->Text).c_str(), l, true, true, false, false, NULL);
        else
            FindWord(edRomaji->Text.LowerCase().Trim().c_str(), l, false, true, false, false, NULL);
    }
    else
        FindKanjiWord(l, edkanji->Text.c_str(), false, NULL);

    dobj->set_by_indexes(l, true);
    delete l;
}
//---------------------------------------------------------------------------
void TfDictUpdater::UpdateMeanings()
{
    listsrc->Clear();
    listdest->Clear();
    TWordReplacement *r = words->Items[wobj->pos];
    for (int ix = 0; ix < r->meaningcnt; ++ix)
        listsrc->Add(wobj->current, r->meanings[ix].meaning);
    for (int ix = 0; ix < states->Destination->words->Items[r->newindex]->meaningcnt; ++ix)
        listdest->Add(r->newindex, ix);

    if (listsrc->Count)
    {
        mdest->Update(r->meanings[0].newmeaning);
        msrc->Update(0);
        sbMeanings->Enabled = true;
    }
    else
    {
        msrc->wordlist = NULL;
        mdest->wordlist = NULL;
        msrc->wordlist = listsrc;
        mdest->wordlist = listdest;
        sbMeanings->Enabled = false;
    }
}
//---------------------------------------------------------------------------
void TfDictUpdater::MatchMeanings(int wordsix)
{
    int windex = states->Items[words->Items[wordsix]->stateindex]->windex;
    int newindex = words->Items[wordsix]->newindex;

    // Only try to match meanings if the word was added to a group where meaning indexes make sense and the word is present in the new dictionary.
    if (newindex >= 0 && words->MeaningCount(wordsix))
    {
        words->UnsetAll(wordsix); // Set 255 for all new meanings to mark them as unset.

        bool allset = true;
        // Try to match meanings from the old dictionary to meanings in the update dictionary. Only exact match counts.
        for (int iy = 0; iy < words->MeaningCount(wordsix); ++iy)
        {
            wchar_t *m1 = states->Collection->words->Items[windex]->data[words->OldMeaning(wordsix, iy)].meaning;
            for (int z = 0; z < states->Destination->words->Items[newindex]->meaningcnt; ++z)
            {
                wchar_t *m2 = states->Destination->words->Items[newindex]->data[z].meaning;
                if (!wcscmp(m1, m2))
                {
                    words->SetNewMeaning(wordsix, iy, z); // Exact match found. Set meaning iy to index z.
                    break;
                }
            }
            if (!words->MeaningSet(wordsix, iy))// One meaning couldn't be found in the new dictionary.
                        allset = false;
        }

        if (!allset)
        { // At least one meaning couldn't be found in the new dictionary.
            for (int iy = 0; iy < words->MeaningCount(wordsix); ++iy)
            {
                if (words->MeaningSet(wordsix, iy))
                    continue;
                // Pair old meaning with new meaning with the same index, if it was not found previously. This is not final, the user will change this in the update dialog.
                if (!words->NewMeaningTaken(wordsix, words->OldMeaning(wordsix, iy)))
                {
                    words->SetNewMeaning(wordsix, iy, words->OldMeaning(wordsix, iy));
                    continue;
                }
                // Old meaning couldn't be paired up with new meaning of the same index. Add a random unmatched meaning from the new dictionary.
                bool meaningset = false;
                for (int z = 0; z < states->Destination->words->Items[newindex]->meaningcnt; ++z)
                {
                    if (words->NewMeaningTaken(wordsix, z))
                        continue;
                    words->SetNewMeaning(wordsix, iy, z);
                    meaningset = true;
                    break;
                }
                // No new meaning was paired with the old one. Set it to index 0 so the user can select the appropriate one later.
                if (!meaningset)
                    words->SetNewMeaning(wordsix, iy, 0);
            }
        }

    }
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::pcSelectorChanging(TObject *Sender, bool &AllowChange)
{
    AllowChange = sbMeanings->Enabled;
    if (!AllowChange)
    {
        if (words->Items[wobj->pos]->newindex < 0)
            MsgBox("Please find a word to replace the old entry and press the 'Select' button.", "Message", MB_OK);
        else
            MsgBox("This word is not in any word group. Meanings can only be selected for words in groups.", "Message", MB_OK);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::btnKanjiClick(TObject *Sender)
{
    if (Sender == btnKanji)
    {
        pKanji->Visible = true;
        pRomaji->Visible = false;
        pbKanji->SetFocus();
    }
    else
    {
        pRomaji->Visible = true;
        pKanji->Visible = false;
        edRomaji->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::btnSelectClick(TObject *Sender)
{
    for (int ix = 0; ix < words->Count; ++ix)
    {
        if (ix == wobj->pos || words->Items[ix]->action == TWordReplacement::raRemove || words->Items[ix]->editstate == TWordReplacement::rsUntouched)
            continue;
        if (words->Items[ix]->newindex == dobj->current)
        {
            MsgBox("This word is alread used as a replacement for another one. Please select a different word or change the other entry first.", "Message", MB_OK);
            return;
        }
    }

    for (int ix = 0; ix < states->Count; ++ix)
    {
        if (!states->Items[ix]->matched)
            continue;
        if (states->Items[ix]->newindex == dobj->current)
        {
            MsgBox("This word is taken by an exact match in the old dictionary and cannot be selected. Please select a different one.", "Message", MB_OK);
            return;
        }
    }

    for (int ix = 0; ix < words->Count; ++ix)
    {
        if (ix == wobj->pos || words->Items[ix]->editstate != TWordReplacement::rsUntouched || words->Items[ix]->newindex != dobj->current)
            continue;
        words->Items[ix]->newindex = -1;
        break;
    }


    words->Items[wobj->pos]->newindex = dobj->current;
    words->Items[wobj->pos]->editstate = TWordReplacement::rsProgress;
    UpdateNextText();
    wobj->invalidatecell(0, wobj->pos);
    btnNext->Enabled = true;

    MatchMeanings(wobj->pos);
    UpdateMeanings();
    UpdateDisplay();

    if (msrc->count)
        ShowPage(1);
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::sbCopyClick(TObject *Sender)
{
    if (usercall)
    {
        if (sbCopy->Down)
        {
            int windex = FindWordIndex(states->Destination, wobj->words[wobj->pos]->kanji, wobj->words[wobj->pos]->kana);
            if (windex >= 0)
            {
                for (int ix = 0; ix < words->Count; ++ix)
                {
                    if (ix == wobj->pos || words->Items[ix]->action == TWordReplacement::raRemove || words->Items[ix]->editstate == TWordReplacement::rsUntouched)
                        continue;
                    if (words->Items[ix]->newindex == windex)
                    {
                        MsgBox("The word with the same kanji and kana is alread used as a replacement for another entry. Change it first in case you would like to copy this entry to the new dictionary.", "Message", MB_OK);
                        sbCopy->Down = false;
                        sbRemove->Down = words->Items[wobj->pos]->action == TWordReplacement::raRemove;
                        return;
                    }
                }

                for (int ix = 0; ix < words->Count; ++ix)
                {
                    if (ix == wobj->pos || words->Items[ix]->editstate != TWordReplacement::rsUntouched || words->Items[ix]->newindex != windex)
                        continue;
                    words->Items[ix]->newindex = -1;
                    break;
                }
            }
            words->Items[wobj->pos]->newindex = windex;

            words->Items[wobj->pos]->editstate = TWordReplacement::rsProgress;
            wobj->invalidatecell(0, wobj->pos);
        }
        else if (words->Items[wobj->pos]->newindex < 0 || states->Items[words->Items[wobj->pos]->stateindex]->origix >= 0)
        {
            words->Items[wobj->pos]->editstate = TWordReplacement::rsUntouched;
            wobj->invalidatecell(0, wobj->pos);
        }
    }

    UpdateDisplay(); // Not just updates display but also sets action to copy.

    if (usercall)
        wcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::sbRemoveClick(TObject *Sender)
{
    if (usercall)
    {
        if (!sbRemove->Down)
        {
            int windex = words->Items[wobj->pos]->newindex >= 0 ? words->Items[wobj->pos]->newindex : FindWordIndex(states->Destination, wobj->words[wobj->pos]->kanji, wobj->words[wobj->pos]->kana);
            for (int ix = 0; ix < words->Count; ++ix)
            {
                if (ix == wobj->pos || words->Items[ix]->action == TWordReplacement::raRemove || words->Items[ix]->editstate == TWordReplacement::rsUntouched)
                    continue;
                if (words->Items[ix]->newindex == windex)
                {
                    MsgBox("The word with the same kanji and kana is alread used as a replacement for another entry. Change it first in case you would not like to remove all data from this one.", "Message", MB_OK);
                    sbRemove->Down = true;
                    sbCopy->Down = words->Items[wobj->pos]->action == TWordReplacement::raCopyOld;
                    return;
                }
            }
            for (int ix = 0; ix < words->Count; ++ix)
            {
                if (ix == wobj->pos || words->Items[ix]->editstate != TWordReplacement::rsUntouched || words->Items[ix]->newindex != windex)
                    continue;
                words->Items[ix]->newindex = -1;
                break;
            }
        }
        words->Items[wobj->pos]->editstate = TWordReplacement::rsProgress;
        wobj->invalidatecell(0, wobj->pos);
    }
    UpdateDisplay(); // Not just updates display but also sets action to remove.

    if (usercall)
        wcallback();
}
//---------------------------------------------------------------------------
void TfDictUpdater::UpdateDisplay()
{
    TWordReplacement *r = words->Items[wobj->pos];
    TWordState *w = states->Items[r->stateindex];

    sbRemove->Enabled = w->origix < 0 || r->editstate != TWordReplacement::rsUntouched && (w->groupcnt || w->kanjicnt || w->longtermix >= 0 || w->meaningredefined);

    usercall = false;
    words->Items[wobj->pos]->action = sbCopy->Down ? TWordReplacement::raCopyOld : sbRemove->Enabled && sbRemove->Down ? TWordReplacement::raRemove : TWordReplacement::raNormal;
    pSelector->Visible = words->Items[wobj->pos]->action == TWordReplacement::raNormal;

    if (pSelector->Visible == lbCopy->Visible)
    {
        TRect r = Rect(0, b1->Top + b1->Height, ClientWidth, ClientHeight);
        InvalidateRect(Handle, &r, false);
    }

    lbCopy->Caption = sbCopy->Down ? L"All meanings of this word will be copied to the new dictionary intact. In case the new dictionary contained a word with the same written and kana form, it will be overwritten." : sbRemove->Enabled && sbRemove->Down ? L"This word will be removed from all groups and tests, with its statistics reset. If the updated dictionary contains this word, the updated definitions will be used." : L"";
    lbCopy->Visible = !pSelector->Visible;
    usercall = true;
}
//---------------------------------------------------------------------------
void TfDictUpdater::UpdateNextText()
{
    bool nodone = false;
    for (int ix = 0; ix < wobj->count && !nodone; ++ix)
        if (ix != wobj->pos && words->Items[ix]->editstate != TWordReplacement::rsDone)
            nodone = true;

    btnNext->Caption = nodone ? L"&Next word >>" : L"&Finish";
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::FormPaint(TObject *Sender)
{
    DrawGrad(this, clBtnFace, COLORDEC(clBtnFace, 0x101010), NULL, b0);
    if (pSelector->Visible)
    {
        DrawGrad(this, COLORDEC(clBtnFace, 0x101010), COLORINC(clBtnFace, 0x101010), b1, b2);
        DrawButtonBar(this, b2, b3, true);
        if (sbMeanings->Down)
        {
            DrawGrad(this, COLORINC(clBtnFace, 0x101010), COLORDEC(clBtnFace, 0x101010), b3, b4);
            DrawGrad(this, COLORINC(clBtnFace, 0x101010), clBtnFace, COLORDEC(clBtnFace, 0x101010), 0.5, b5, b6);
        }
        else
        {
            DrawGrad(this, COLORINC(clBtnFace, 0x101010), COLORDEC(clBtnFace, 0x101010), b3, b9);
        }
        DrawGrad(this, clBtnFace, clBtnFace, b10, b8);
    }
    else
    {
        DrawGrad(Canvas, COLORDEC(clBtnFace, 0x101010), COLORINC(clBtnFace, 0x101010), Rect(0, b1->Top + b1->Height, ClientWidth, lbCopy->Top), true);
        DrawPillowPanel(Canvas, Rect(0, lbCopy->Top, ClientWidth, b8->Top), false, false);
    }
    DrawCloseBar(this, b8, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::FormResize(TObject *Sender)
{
    TRect r;
    if (pSelector->Visible)
    {
        r = Rect(0, b1->Top + b1->Height, ClientWidth, pSelector->Top);
        InvalidateRect(Handle, &r, false);

        r = Rect(0, ChildRect(b10).Bottom, ClientWidth, ClientHeight);
        InvalidateRect(Handle, &r, false);
    }
    else
    {
        r = Rect(0, b1->Top + b1->Height, ClientWidth, ClientHeight);
        InvalidateRect(Handle, &r, false);
    }
}
//---------------------------------------------------------------------------
void TfDictUpdater::wcelldraw(int col, int row, TCanvas *c, const TRect &r)
{
    c->FillRect(r);
    if (words->Items[row]->editstate == TWordReplacement::rsUntouched)
        return;

    if (words->Items[row]->editstate == TWordReplacement::rsProgress)
    {
        ilState->Draw(c, r.Left + (r.Width() - ilState->Width) / 2, r.Top + (r.Height() - ilState->Height) / 2, 3);
        return;
    }

    switch (words->Items[row]->action)
    {
    case TWordReplacement::raCopyOld:
        ilState->Draw(c, r.Left + (r.Width() - ilState->Width) / 2, r.Top + (r.Height() - ilState->Height) / 2, 1);
        break;
    case TWordReplacement::raRemove:
        ilState->Draw(c, r.Left + (r.Width() - ilState->Width) / 2, r.Top + (r.Height() - ilState->Height) / 2, 2);
        break;
    default:
        if (words->Items[row]->newindex < 0)
            return;
        for (int ix = 0; ix < words->MeaningCount(row); ++ix)
            if (!words->MeaningSet(row, ix))
                return;
        ilState->Draw(c, r.Left + (r.Width() - ilState->Width) / 2, r.Top + (r.Height() - ilState->Height) / 2, 0);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::btnNextClick(TObject *Sender)
{
    words->Items[wobj->pos]->editstate = TWordReplacement::rsDone;
    wobj->invalidatecell(0, wobj->pos);
    for (int ix = 0; ix < wobj->count; ++ix)
        if (words->Items[ix]->editstate != TWordReplacement::rsDone)
        {
            wobj->pos = ix;
            break;
        }
    UpdateNextText();

    if (words->Items[wobj->pos]->editstate == TWordReplacement::rsDone) // All done.
        ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::btnAbortClick(TObject *Sender)
{
    if (aborttext.Length())
    {
        if (MsgBox(aborttext, L"Query", MB_YESNO) == IDNO)
            return;
    }
    ModalResult = mrAbort;
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (ModalResult != mrOk)
        return;

    for (int ix = 0; ix < words->Count; ++ix)
    {
        TWordState *s = states->Items[words->Items[ix]->stateindex];
        TWordReplacement *w = words->Items[ix];
        if (w->action != TWordReplacement::raNormal)
        {
            s->action = w->action == TWordReplacement::raCopyOld ? TWordState::saCopyOld : TWordState::saRemove;
            s->newindex = w->action == TWordReplacement::raCopyOld ? w->newindex : -1;
            continue;
        }
        s->action = TWordState::saUpdate;
        s->newindex = w->newindex;
        for (int iy = 0; iy < w->meaningcnt; ++iy)
            for (int z = 0; z < s->groupcnt; ++z)
                if (s->groups[z].meaning == w->meanings[iy].meaning)
                    s->groups[z].newmeaning = w->meanings[iy].newmeaning;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::sbReplacementClick(TObject *Sender)
{
    ShowPage((TSpeedButton*)Sender == sbReplacement ? 0 : 1);
}
//---------------------------------------------------------------------------
void TfDictUpdater::ShowPage(int page)
{
    if (page == 0)
    {
        sbReplacement->Down = true;
        pReplacement->Visible = true;
        pMeanings->Visible = false;
        btnKanji->Click();
    }
    else
    {
        sbMeanings->Down = true;
        pMeanings->Visible = true;
        pReplacement->Visible = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::pbKanjiKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == VK_RETURN &&  btnSelect->Enabled)
    {
        btnSelect->Click();
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::pbKanjiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
        Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfDictUpdater::pbKanjiEnter(TObject *Sender)
{
    btnNext->Default = !pbKanji->Focused() && !edRomaji->Focused();
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(219);
#endif







