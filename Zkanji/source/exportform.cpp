//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "exportform.h"
#include "wordobj.h"
#include "kanjigrid.h"
#include "words.h"
#include "wordgroupstudy.h"
#include "gengraph.h"
#include "settings.h"
#include "collection.h"
#include "wordgroup.h"
#include "kanjigroup.h"
#include "studylist.h"
#include "zkformats.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"
TfExport *fExport;

#ifdef _DEBUG
static TGlobalTest tester1(220);
#endif

/* Global variables here only. */

#ifdef _DEBUG
static TGlobalTest tester2(221);
#endif

bool HasChecked(TCheckListBox *box, int first = 0, int last = -1);

//---------------------------------------------------------------------------
__fastcall TfExport::TfExport(TComponent* Owner) : TBaseForm(Owner), sizing(false)
{
}
//---------------------------------------------------------------------------
__fastcall TfExport::~TfExport()
{
    delete wobj;
    delete kobj;
    delete kwobj;
}
//---------------------------------------------------------------------------
int TfExport::ShowModalEx(TWordCollection *acoll)
{
    coll = acoll;
    pTabs->ActivePage = tabIntro;
    Constrain(Width - pTabs->Width + pSections->Width + 16, Height - pTabs->Height + pSections->Height + 16);
    wobj = NULL;
    kobj = NULL;
    kwobj = NULL;
    if (coll == dictionary->Items[0])
    {
        clbKG->Height = cbKMeaning->Top + cbKMeaning->Height - clbKG->Top;
        cbKMeaning->Visible = false;
        cbKMeaning->Checked = false;
    }

    return ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfExport::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfExport::FormPaint(TObject *Sender)
{
    DrawCloseBar(this, bvBottom, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfExport::FormResize(TObject *Sender)
{
    TRect r(0, bvBottom->Top, ClientWidth, ClientHeight);
    InvalidateRect(Handle, &r, false);
    SizeKanjiPanels(0);
}
//---------------------------------------------------------------------------
void __fastcall TfExport::cbWordsClick(TObject *Sender)
{
    UpdateButtons();
}
//---------------------------------------------------------------------------
void TfExport::UpdateButtons()
{
    TTabSheet *lastpage = cbStudy->Checked ? tabStudy : cbKanji->Checked ? tabKanji : tabWord;
    bool disable = false;
    if (pTabs->ActivePage == lastpage)
    {
        btnNext->Caption = L"&Export...";
        btnNext->Tag = 1;
        bool haswords = (cbWords->Checked && tabWord->Tag == 1 && HasChecked(clbWG));
        bool haskanji = (cbKanji->Checked && tabKanji->Tag == 1 && (HasChecked(clbKG, 1, clbKG->Count - 1) || (clbKG->Checked[0] && (cbKEx->Checked || cbKMeaning->Checked || cbKUser->Checked))));
        bool hasstudy = (cbStudy->Checked && tabStudy->Tag == 1 && HasChecked(clbSG));

        if (!haswords && !haskanji && !hasstudy)
            disable = true;
    }
    else
    {
        btnNext->Caption = L"&Next >>";
        btnNext->Tag = 0;
    }
    btnBack->Enabled = pTabs->ActivePage != tabIntro;
    btnNext->Enabled = (pTabs->ActivePage == tabIntro && (cbWords->Checked || cbKanji->Checked || cbStudy->Checked)) ||
                       (!disable && pTabs->ActivePage != tabIntro);
}
//---------------------------------------------------------------------------
void TfExport::kcallback()
{
    if (kobj->kanji == NULL)
        kwobj->set_by_indexes(NULL);
    else
    {
        TWordIndexList *l = new TWordIndexList(coll);
        for (int ix = 0; ix < kobj->card->examples->Count; ++ix)
            l->Add(kobj->card->examples->Items[ix]->ix);
        kwobj->set_by_indexes(l);
        delete l;
    }
}
//---------------------------------------------------------------------------
void TfExport::SizeKanjiPanels(int h)
{
    if (pbKG->Parent->Tag == 0)
        return;

    int blocksiz = pKG->Height - clbKG->Top;

    if (h != 0)
        sizer = 0;
    else
        h = blocksiz * sizer - sizegap;

    h = max(min(h, blocksiz - sizegap), 50 * scaleres + pbKG->Height - pbKG->ClientHeight);
    if (!sizer)
        sizer = (double)(h + sizegap) / blocksiz;

    if (pKW->Height == blocksiz - h - sizegap && pbKG->Width == pbKG->Parent->ClientWidth - rightgap - pbKG->Left && pbKG->Height == h)
        return;

    HDWP dwp = BeginDeferWindowPos(2);
    dwp = DeferWindowPos(dwp, pbKG->Handle, NULL, 0, 0, pbKG->Parent->ClientWidth - rightgap - pbKG->Left, h, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    dwp = DeferWindowPos(dwp, pKW->Handle, NULL, pKW->Left, pbKG->Top + h + sizegap, pbKG->Parent->ClientWidth - rightgap - pKW->Left, blocksiz - h - sizegap, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
    EndDeferWindowPos(dwp);
}
//---------------------------------------------------------------------------
struct TKanjiGroupPos
{
    TKanjiGroup *group;
    int pos; // Position of kanji in this group.
    TKanjiGroupPos(TKanjiGroup *group, int pos) : group(group), pos(pos) {}
};
struct TKanjiGroupExport
{
    typedef TTemplateList<TKanjiGroupPos, true>   TGroupList;
    TGroupList *groups;
    word index; // Index of kanji in the global kanjis list.

    TKanjiGroupExport(word index) : groups(NULL), index(index) {}
    ~TKanjiGroupExport()
    {
        delete groups;
    }
    void AddGroup(TKanjiGroup *group, int pos)
    {
        if (!groups)
            groups = new TGroupList;
        groups->Add(new TKanjiGroupPos(group, pos));
    }
};

void TfExport::Export(UnicodeString fname)
{
    FILE *f = _wfopen(fname.c_str(), L"wb");
    if (!f)
        THROW(L"Couldn't open the file for writing.");
    try
    {
        wchar_t *intro =
L"\
;zkanji export file for version 0.73 and later.\r\n\
;\r\n\
;The export file must be in UTF-8. Whitespace only refers to the space and the\r\n\
;TAB characters in this description. Lines that start with zero or more\r\n\
;whitespace followed by ; or # are comments. There is no mid-line comment.\r\n\
;\r\n\
;The file consists of sections for different data. Every section starts with a\r\n\
;section name in square brackets like this:\r\n\
;[Section Name]\r\n\
;Only whitespace can be on the same line next to the section name.\r\n\
;\r\n\
;Unrecognized sections are ignored, so older versions of the program can read\r\n\
;newer export formats correctly. The data can contain the same section any\r\n\
;number of times, but if data in entries differ, only the first version is used.\r\n\
;\r\n\
;Within a single section each line has the same format and refers to a separate\r\n\
;entry. The entries can't span over multiple lines. The lines are made up of one\r\n\
;or more tokens which can contain UTF-8 code points between 0x20 and 0xFFFF.\r\n\
;Tokens are made up of function markers (zero or more characters marking the\r\n\
;function of the token) and a variable part. The separator character between\r\n\
;tokens is space, but if a token can contain spaces (depending on its\r\n\
;type), its variable part must start and end with the TAB character (marked with\r\n\
;\\t in the format descriptions). If a token is not recognized, it is ignored\r\n\
;during import, but incorrect tokens (i.e. when a token should be a number and\r\n\
;it is not) can make the whole or part of the line ignored. If a line or its\r\n\
;part is not meaningful without tokens that are missing, the line or that part\r\n\
;is ignored. Tokens must be in the same order as in the format description.\r\n\
;\r\n\
;Lines can contain repeated patterns on two levels separated by the space\r\n\
;character. Top-level patterns are between XX{ ... }XX (braces or curly\r\n\
;brackets), where XX must match on the ends. Secondary patterns within top-level\r\n\
;patterns are between YY( ... )YY (parentheses or round brackets).\r\n\
;\r\n\
;The only exception is the [About] section. If present, it must be the first\r\n\
;one in the file. The only restrictions to its format is that each line must\r\n\
;start with a * or - character, and no line can be longer than 1000 characters.\r\n\
;Lines starting with * belong to a new line, those which start with - are\r\n\
;appended to the previous line during import. On export, the text in this\r\n\
;section is the exact copy of the dictionary information of the source\r\n\
;dictionary (i.e. license text, authors). It is only imported during full\r\n\
;dictionary import.\r\n\
;\r\n\
;The following other sections are accepted starting with zkanji v0.73:\r\n\
;[Words], [Kanji]\r\n\
;\r\n\
;In the following format for the lines, descriptions inside square brackets []\r\n\
;are placeholders for the variable parts of tokens, the text inside the\r\n\
;brackets describes the function of the part they replace.\r\n\
;\r\n\
;Line format for the [Words] section:\r\n\
;Each line describes a word entry in the dictionary. The description can be\r\n\
;partial (i.e. not all meanings are listed). The line's structure is:\r\n\
;[word kanji] [word kana] F[frequency number] M{\\t[word definition]\\t #[meaning number] MT[list of word types separated by comma (see wtypetext in zkformats.cpp)] MN[list of notes (wnotetext)] MF[list of fields (wfieldtext)] NT[list of name tags (ntagtext)] G(\\t[group name]\\t #[entry index])G}M\r\n\
;\r\n\
;The [meaning number] can be missing but if specified, it must be a number\r\n\
;between 0 and 99. If the same number is encountered for a second time for the\r\n\
;same word, that meaning is skipped. The [entry index] when specified refers\r\n\
;to the index of an entry in a group. When several meanings have the same\r\n\
;[entry index] in the same group for the same word, they will be imported to the\r\n\
;same group entry once this is implemented. Until then, they are all added\r\n\
;separately. When the [entry index] is missing, the entries are added to the\r\n\
;group in their order in the export file. Repeated group name+entry index pairs\r\n\
;are ignored and not added to the same group again.\r\n\
;If lines for different words have the same [entry index] for a group, they\r\n\
;can't be merged, so they will be added in their order in the export file.\r\n\
;\r\n\
;Explanation of the [Kanji] section:\r\n\
;[kanji character] D\\t[dictionary kanji meaning]\\t U\\t[user defined kanji meaning]\\t G{\\t[group name]\\t #[entry index]}G W{[word kanji] [word kana] WU\\t[user defined word meaning]\\t}W\r\n\
;\r\n\
;The same kanji character can be repeated in the section if each line lists\r\n\
;different groups, but it is better to have them on a single line. The\r\n\
;[dictionary kanji meaning] is only imported for user dictionaries. The\r\n\
;[entry index] for groups can be missing, but if present, it is used to order\r\n\
;group entries. When it is missing, the entries are added to the group in their\r\n\
;order in the export file.\r\n\
;Word kanji and kana mark kanji example words, with an optional user defined\r\n\
;meaning. The meaning of the selected example words and the definition of kanji\r\n\
;can be edited on the groups panel, but it is usually not specified. It is\r\n\
;unsure whether this feature will be supported in future versions of zkanji.\r\n\
\r\n\
";
        write(intro, f);

        if (!coll->info.IsEmpty())
        {
            write(
L";The following section may contain copyright or licence information of the\r\n\
;dictionary that was originally holding the entries exported to this file. Even\r\n\
;if these entries are only a subset of the original dictionary, the following\r\n\
;still applies:\r\n", f);
            write(L"[About]\r\n", f);

            // When writing the dictionary information, split lines at most 79 characters.
            // "Words" longer than 79 characters are cut in half without mercy.

            UnicodeString str = coll->info;
            int len = str.Length();
            int pos = 1;
            bool newline = true;
            while (pos <= len)
            {
                if (newline)
                    write(L"*", f);
                else
                    write(L"-", f);

                newline = false;
                for (int ix = 0; ix < min(80, len - pos + 1); ++ix)
                {
                    if (str[pos + ix] == L'\r' || str[pos + ix] == L'\n')
                    {
                        if (ix)
                            write(str.c_str() + pos - 1, f, ix);
                        write(L"\r\n", f);
                        newline = true;

                        pos += ix + 1;
                        if (pos <= len && str[pos - 1] == L'\r' && str[pos] == L'\n')
                            ++pos;
                        break;
                    }
                }

                if (newline)
                    continue;

                if (len - pos < 80)
                {
                    write(str.c_str() + pos - 1, f, len - pos + 1);
                    write(L"\r\n", f);
                    break;
                }

                for (int ix = 79; ix >= 0; --ix)
                {
                    if (wcschr(cdelim, str[pos + ix]) != NULL)
                    {
                        write(str.c_str() + pos - 1, f, ix);
                        write(L"\r\n", f);
                        newline = true;
                        pos += ix;
                        //if (pos <= len && str[pos] == L' ')
                        //    ++pos;
                        break;
                    }
                }

                if (newline)
                {
                    newline = false;
                    continue;
                }
                newline = false;

                write(str.c_str() + pos - 1, f, 79);
                write(L"\r\n", f);
                pos += 79;
            }
            write(L"\r\n", f);
        }

        if (cbWords->Checked && HasChecked(clbWG))
        {
            write(L"[Words]\r\n", f);
            bool allwords = clbWG->Checked[0];
            for (int ix = 0; ix < coll->words->Count; ++ix)
            {
                TWord *w = coll->words->Items[ix];
                if (w->group == NULL && !allwords)
                    continue;

                /* Check if word is only in study groups */
                if (!allwords)
                {
                    TWordGroups *g = w->group;
                    while(g && dynamic_cast<TWordGroupStudy*>(g->group) != NULL)
                        g = g->next;
                    if (!g)
                        continue;
                }

                // [kanji]
                write(w->kanji, f);
                write(L" ", f);
                // [kana]
                write(w->kana, f);
                write(L" ", f);
                // F[frequency number]
                write(L"F", f);
                write(IntToStr(w->frequency), f);

                //[word kanji] [word kana] F[frequency number] M{\\t[word definition]\\t #[meaning number] MT[list of word types separated by comma (see wtypetext in zkformats.cpp)] MN[list of notes (wnotetext)] MF[list of fields (wfieldtext)] NT[list of name tags (ntagtext)] G(\\t[group name]\\t #[entry index])G}M
                for (int j = 0; j < w->meaningcnt; ++j)
                {
                    if (!allwords && cbGroupMeaning->Checked) // See if meaning is in a group to export.
                    {
                        TWordGroups *g = w->group;
                        while (g)
                        {
                            if (dynamic_cast<TWordGroupStudy*>(g->group) == NULL && g->meaning == j && clbWG->Checked[coll->groups->IndexOf(g->group) + 1])
                                break;
                            else
                                g = g->next;
                        }
                        if (!g)
                            continue;
                    }

                    // Start of meaning pattern.
                    write(L" M{\t", f);
                    write(w->data[j].meaning, f);
                    write(L"\t #", f);
                    write(IntToStr(j).c_str(), f);

                    // MT[list of word types separated by comma (see wtypetext in zkformats.cpp)]
                    if (w->data[j].types != 0)
                    {
                        write(L" MT", f);
                        bool written = false;
                        for (int k = 0; k < WT_COUNT; ++k)
                        {
                            if ((w->data[j].types & (1 << k)) != 0)
                            {
                                if (written)
                                    write(L",", f);
                                write(wtypetext[k], f);
                                written = true;
                            }
                        }
                    }
                    // MN[list of notes (wnotetext)]
                    if (w->data[j].notes != 0)
                    {
                        write(L" MN", f);
                        bool written = false;
                        for (int k = 0; k < WN_COUNT; ++k)
                        {
                            if ((w->data[j].notes & (1 << k)) != 0)
                            {
                                if (written)
                                    write(L",", f);
                                write(wnotetext[k], f);
                                written = true;
                            }
                        }
                    }
                    // MF[list of fields (wfieldtext)]
                    if (w->data[j].fields != 0)
                    {
                        write(L" MF", f);
                        bool written = false;
                        for (int k = 0; k < WF_COUNT; ++k)
                        {
                            if ((w->data[j].fields & (1 << k)) != 0)
                            {
                                if (written)
                                    write(L",", f);
                                write(wfieldtext[k], f);
                                written = true;
                            }
                        }
                    }
                    // NT[list of name tags (ntagtext)]
                    if (w->data[j].nametag != 0)
                    {
                        write(L" NT", f);
                        bool written = false;
                        for (int k = 0; k < NM_COUNT; ++k)
                        {
                            if ((w->data[j].nametag & (1 << k)) != 0)
                            {
                                if (written)
                                    write(L",", f);
                                write(ntagtext[k], f);
                                written = true;
                            }
                        }
                    }

                    // G(\\t[group name]\\t #[entry index])G
                    TWordGroups *g = w->group;
                    while (g)
                    {
                        if (dynamic_cast<TWordGroupStudy*>(g->group) == NULL && g->meaning == j && clbWG->Checked[coll->groups->IndexOf(g->group) + 1])
                        {
                            write(L" G(\t", f);
                            write(g->group->name.c_str(), f);
                            write(L"\t #", f);
                            write(IntToStr(g->group->list->IndexOf(ix, g->meaning)).c_str(), f);
                            write(L")G", f);
                        }
                        g = g->next;
                    }

                    write(L"}M", f);
                }
                write(L"\r\n", f);
            }
            write(L"\r\n", f);
        }

        if (cbKanji->Checked && HasChecked(clbKG))
        {
            bool allkanji = clbKG->Checked[0];
            bool groupkanji = HasChecked(clbKG, 1, clbKG->Count - 1);
            bool dokanjiexport = groupkanji;
            TTemplateList<TKanjiGroupExport, true> klist;
            if (allkanji)
            {
                for (int ix = 0; ix < kanjis->Count; ++ix)
                {
                    if (((cbKMeaning->Checked && coll->kanjidat[ix].meaning) ||
                        (cbKEx->Checked && coll->kanjidat[ix].card != NULL && coll->kanjidat[ix].card->ExampleCount != 0) ||
                        (cbKUser->Checked && coll->kdeflist->ItemSet(ix))))
                    {
                        dokanjiexport = true;
                        klist.Add(new TKanjiGroupExport(ix));
                    }
                }
            }
            if (groupkanji)
            {
                for (int ix = 0; ix < coll->kgroups->Count; ++ix)
                {
                    if (!clbKG->Checked[ix + 1])
                        continue;

                    TKanjiGroup *kg = coll->kgroups->Items[ix];
                    TKanjiGroupExport *dat;

                    for (int k = 0; k < kg->Count; ++k)
                    {
                        dat = NULL;
                        int kanjiix = kg->Items[k]->kanji->index;
                        for (int j = 0; j < klist.Count && dat == NULL; ++j)
                            if (klist.Items[j]->index == kanjiix)
                                dat = klist.Items[j];
                        if (dat == NULL)
                            klist.Add(dat = new TKanjiGroupExport(kanjiix));
                        dat->AddGroup(kg, k);
                    }
                }
            }

            if (dokanjiexport)
            {
                write(L"[Kanji]\r\n", f);
                //[kanji character] D\\t[dictionary kanji meaning]\\t U\\t[user defined kanji meaning]\\t G{\\t[group name]\\t #[entry index]}G W{[word kanji] [word kana] WU\\t[user defined word meaning]\\t}W
                for (int ix = 0; ix < klist.Count; ++ix)
                {
                    int kanjiix = klist.Items[ix]->index;
                    TKanji *k = kanjis->Kanjis[kanjiix];
                    write(k->ch, f);

                    // D\t[dictionary kanji meaning]\t
                    if (cbKMeaning->Checked && coll->kanjidat[kanjiix].meaning != NULL)
                    {
                        write(L" D\t", f);
                        write(coll->kanjidat[kanjiix].meaning, f);
                        write(L"\t", f);
                    }
                    // U\t[user defined kanji meaning]\t
                    if (cbKUser->Checked && coll->kdeflist->ItemSet(kanjiix))
                    {
                        write(L" U\t", f);
                        write(coll->kdeflist->Items[kanjiix], f);
                        write(L"\t", f);
                    }
                    // G{\\t[group name]\\t #[entry index]}G
                    if (klist.Items[ix]->groups != NULL)
                    {
                        for (int j = 0; j < klist.Items[ix]->groups->Count; ++j)
                        {
                            TKanjiGroupPos *g = klist.Items[ix]->groups->Items[j];
                            write(L" G{\t", f);
                            write(g->group->name.c_str(), f);
                            write(L"\t #", f);
                            write(IntToStr(g->pos).c_str(), f);
                            write(L"}G", f);
                        }
                    }

                    // W{[word kanji] [word kana] WU\\t[user defined word meaning]\\t}W
                    if (cbKEx->Checked && coll->kanjidat[kanjiix].card != NULL && coll->kanjidat[kanjiix].card->ExampleCount > 0)
                    {
                        int cnt = coll->kanjidat[kanjiix].card->ExampleCount;
                        for (int j = 0; j < cnt; ++j)
                        {
                            write(L" W{", f);

                            int wordix = coll->kanjidat[kanjiix].card->Examples[j]->ix;
                            TWord *w = coll->words->Items[wordix];
                            write(w->kanji, f);
                            write(L" ", f);
                            write(w->kana, f);
                            if (cbKUser->Checked && coll->deflist->ItemSet(wordix))
                            {
                                write(L" WU\t", f);
                                write(coll->deflist->Items[wordix], f);
                                write(L"\t", f);
                            }
                            write(L"}W", f);
                        }
                    }

                    write(L"\r\n", f);
                }

                write(L"\r\n", f);
            }
        }
    }
    catch(...)
    {
        fclose(f);
        throw;
    }
    fclose(f);
}
//---------------------------------------------------------------------------
void __fastcall TfExport::btnNextClick(TObject *Sender)
{
    TTabSheet *lastpage = cbStudy->Checked ? tabStudy : cbKanji->Checked ? tabKanji : tabWord;

    if (pTabs->ActivePage == lastpage)
    {
        dlgExport->InitialDir = settings.exportdir;
        UnicodeString fname;

        bool retry = true;
        while (retry)
        {
            try
            {
                if (ExecuteOpenDialog(dlgExport, L"zkanji.export", true, false, fname))
                {
                    settings.exportdir = ExtractFileDir(fname);
                    Export(fname);
                    //SaveExport
                }
                retry = false;
            }
            catch(wchar_t *c)
            {
                retry = MsgBox(c, L"Error", MB_RETRYCANCEL) == IDRETRY;
            }
        }
        ModalResult = mrOk;
        return;
    }

    if (pTabs->ActivePage == tabIntro)
        pTabs->ActivePage = cbWords->Checked ? tabWord : cbKanji->Checked ? tabKanji : tabStudy;
    else if (pTabs->ActivePage == tabWord)
        pTabs->ActivePage = cbKanji->Checked ? tabKanji : tabStudy;
    else if (pTabs->ActivePage == tabKanji)
        pTabs->ActivePage = tabStudy;

    if (pTabs->ActivePage == tabWord && tabWord->Tag != 1)
    {
        tabWord->Tag = 1;
        clbWG->Items->BeginUpdate();
        clbWG->Items->Add(L"[Not in group]");

        for (int ix = 0; ix < coll->groups->Count; ++ix)
        {
            int iy = clbWG->Items->Add(coll->groups->Names[ix]);
            clbWG->ItemEnabled[iy] = coll->groups->Items[ix]->list->Count != 0;
            clbWG->Checked[iy] = clbWG->ItemEnabled[iy];
        }
        clbWG->Items->EndUpdate();

        wobj = new TVocabListObj(pbWG, rowsizes(settings.dictfontsize), (TFuncPtr)NULL, (TPopupWordProc)NULL);

        clbWG->ItemIndex = clbWG->Items->Count != 0 ? 1 : 0;
        clbWGClick(clbWG);
        clbWG->SetFocus();
    }
    else if (pTabs->ActivePage == tabKanji && tabKanji->Tag != 1)
    {
        tabKanji->Tag = 1;
        sizegap = pKW->Top - (pbKG->Top + pbKG->Height);
        sizer = (double)(pbKG->Height + sizegap) / (pKG->Height - clbKG->Top);
        pbKG->Anchors = TAnchors() << akLeft << akTop;
        pKW->Anchors = TAnchors() << akLeft << akTop;
        rightgap = pbKG->Parent->ClientWidth - (pbKG->Left + pbKG->Width);
        SizeKanjiPanels(0);

        clbKG->Items->BeginUpdate();
        clbKG->Items->Add(L"[Not in group]");

        for (int ix = 0; ix < coll->kgroups->Count; ++ix)
        {
            int iy = clbKG->Items->Add(coll->kgroups->Names[ix]);
            clbKG->ItemEnabled[iy] = coll->kgroups->Items[ix]->Count != 0;
            clbKG->Checked[iy] = clbKG->ItemEnabled[iy];
        }
        clbKG->Items->EndUpdate();

        kobj = new TKanjiCardGrid(pbKG, 50 * scaleres, false, &kcallback);
        kwobj = new TWordListObj(pbKEx, rowsizes(settings.dictfontsize), (TFuncPtr)NULL, (TPopupWordProc)NULL);

        clbKG->ItemIndex = clbKG->Items->Count != 0 ? 1 : 0;
        clbKGClick(clbWG);
        clbKG->SetFocus();
    }
    else if (pTabs->ActivePage == tabStudy && tabStudy->Tag != 1)
    {
/*
        tabStudy->Tag = 1;
        clbSG->Items->BeginUpdate();

        for (int ix = 0; ix < coll->wordstudylist->->Count; ++ix)
        {
            clbSG->Items->Add(coll->kgroups->Names[ix]);
            clbSG->Checked[ix] = true;
        }
        clbSG->Items->Add(L"[Not in group]");
        clbSG->Items->EndUpdate();

        kobj = new TKanjiCardGrid(pbKG, 50 * scaleres, false, &kcallback);
        kwobj = new TWordListObj(pbKEx, rowsizes(settings.dictfontsize), (TFuncPtr)NULL, (TPopupWordProc)NULL);

        clbSG->ItemIndex = 0;
        clbSGClick(clbWG);
        clbSG->SetFocus();
*/
    }


    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfExport::btnBackClick(TObject *Sender)
{
    if (pTabs->ActivePage == tabWord)
        pTabs->ActivePage = tabIntro;
    else if (pTabs->ActivePage == tabKanji)
        pTabs->ActivePage = cbWords->Checked ? tabWord : tabIntro;
    else if (pTabs->ActivePage == tabStudy)
        pTabs->ActivePage = cbKanji->Checked ? tabKanji : cbWords->Checked ? tabWord : tabIntro;

    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfExport::clbWGClick(TObject *Sender)
{
    wobj->wordlist = clbWG->ItemIndex > 0 ? coll->groups->Lists[clbWG->ItemIndex - 1] : NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfExport::clbWGClickCheck(TObject *Sender)
{
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfExport::btnWChkAllClick(TObject *Sender)
{
    clbWG->CheckAll(cbChecked, false, false);
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfExport::btnWChkNoneClick(TObject *Sender)
{
    clbWG->CheckAll(cbUnchecked, false, false);
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfExport::btnKChkAllClickClick(TObject *Sender)
{
    clbKG->CheckAll(cbChecked, false, false);
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfExport::btnKChkNoneClickClick(TObject *Sender)
{
    clbKG->CheckAll(cbUnchecked, false, false);
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfExport::clbKGClick(TObject *Sender)
{
    TKanjiGroup *kg = clbKG->ItemIndex > 0 ? coll->kgroups->Items[clbKG->ItemIndex - 1] : NULL;
    kobj->set_items(kg, coll);
}
//---------------------------------------------------------------------------
void __fastcall TfExport::cbKExClick(TObject *Sender)
{
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfExport::clbSGClick(TObject *Sender)
{
    ;
}
//---------------------------------------------------------------------------
void __fastcall TfExport::clbSGClickCheck(TObject *Sender)
{
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfExport::tabKanjiMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    if (!Shift.Contains(ssLeft) || GetCapture() != pbKG->Parent->Handle)
        sizing = false;
    if (sizing || (x >= clbKG->Left + clbKG->Width) && y >= pbKG->Top + pbKG->Height && y < pKW->Top)
        pbKG->Parent->Cursor = crSizeNS;
    else
        pbKG->Parent->Cursor = crDefault;
    if (sizing)
        SizeKanjiPanels(max(1, y - pbKG->Top - grabpos));
}
//---------------------------------------------------------------------------
void __fastcall TfExport::tabKanjiMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button == mbLeft && tabKanji->Cursor == crSizeNS)
    {
        sizing = true;
        grabpos = y - (pbKG->Top + pbKG->Height);
        SizeKanjiPanels(max(1, y - pbKG->Top - grabpos));
    }
}
//---------------------------------------------------------------------------
void __fastcall TfExport::tabKanjiMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    sizing = false;
}
//---------------------------------------------------------------------------

