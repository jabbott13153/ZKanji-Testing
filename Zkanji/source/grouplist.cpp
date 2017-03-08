//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Themes.hpp>

#include "grouplist.h"
#include "main.h"
#include "kanjigrid.h"
#include "words.h"
#include "wordobj.h"
#include "boxobj.h"
#include "collection.h"
#include "kanjiinfo.h"
#include "kanjilist.h"
#include "wordlist.h"
#include "settings.h"
#include "wordgroupstudy.h"
#include "vocabform.h"
#include "studygroup.h"
#include "studyroom.h"
#include "module.h"
#include "zmenu.h"
#include "mouseevent.h"
#include "vocabprint.h"
#include "studylist.h"
#include "kanjigroup.h"
#include "msgform.h"
//#include "groupexport.h"
#include "docker.h"
#include "words.h"
#include "listimport.h"
#include "wordstudylist.h"
#include "wordstudytest.h"
#include "gengraph.h"
#include "romajizer.h"
#include "warningbubble.h"
#include "genworddata.h"
#include "wordfilter.h"
#include "dictquery.h"
#include "newword.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester1(56);
#endif

TfGroup *fGroup;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TfGroup::TfGroup(TComponent* Owner) : TBaseForm(Owner), fcoll(NULL)
{
    Constrain(516*scaleres + (Width - ClientWidth), 322*scaleres + (Height - ClientHeight));

    EraseBackground = false;
    needexamples = true;

    kresults = new TIntegerList;

    kobj = new TKanjiCardGrid(pbGrid, 50 * scaleres, true, &kcallback);
    kobj->allowcopy = true;
    kwobj = new TWordListObj(pbKWords, rowsizes(settings.dictfontsize), kwcallback, &data->PopupWord);
    kwobj->showgroup = true;

    kbox = new TReadingBoxObj(pbReadings, rowsizes(settings.dictfontsize), bcallback);
    kbox->header = false;
    ewobj = new TVirtualWordListObj(pbEWords, rowsizes(settings.dictfontsize), ewcallback, &data->PopupWord);
    ewobj->Tag = -1;
    ewobj->meaningproc = ewmeaning;
    ewobj->prepareproc = ewprepare;

    int cw[3] = { 80 * scaleres, 120 * scaleres, 6250 * scaleres };
    TWordColumnType ct[3] = { wcKanji, wcKana, wcMeaning };

    ewobj->set_cols(NULL, 3, cw, NULL, NULL, ct, NULL, 0, NULL);
    ewobj->colresize[2] = false;

    wobj = new TVocabListObj(pbWords, rowsizes(settings.dictfontsize), &wcallback, &data->PopupWord);
    wobj->selecttype = wlsMultiSelect;
    wobj->multiselection = wlmsContinuousRandom;
    kwsize = -1;

    edKFilter = new TKanjiEdit(pbKFilter, false, false, NULL, &kfcallback);
    edWFilter = new TKanjiEdit(pbWFilter, false, false, NULL, &wfcallback);

    feditstate = mesUnchanged;
    fdeditstate = mesUnchanged;

    for (int i = 0; i < pExBar->ControlCount; i++)
        if (pExBar->Controls[i]->ClassType() == __classid(TSpeedButton))
            pExBar->Controls[i]->Cursor = crArrow;

    infoh = 19 * scaleres;
    pKBack->Margins->Bottom = infoh;
    pbWords->Margins->Bottom = infoh;
    pbReadings->Margins->Top = pGroupBtn->Height;
    pGroupBtn->SetBounds(pbReadings->Left - 1, 0, ClientWidth - pbReadings->Left + 1, pGroupBtn->Height);
    sbInfo->Anchors = TAnchors() << akLeft << akTop;
    sbInfo->BringToFront();
    pKWords->Align = alNone;
    pKDef->Align = alNone;
    pbKWords->Align = alNone;
    pExBar->Align = alNone;
    pKanjiTop->Align = alNone;
    SettingsChangeEvent(this, settingsChanged, true);
    RestoreOldActive = true;

    // Building menus.
    gridmenu = new TZMenuRows(3);
    gridmenuextra = new TZMenuRows(3);
    gridmenucombined = new TZMenuRows(3);

    wchar_t *gridnames[] = { L"Show", L"Kanji Info", L"Translate", L"Clpb: Kanji", L"Copy", L"Append", L"Clpb: All", L"Copy", L"Append", L"Kanji", L"To Group...", L"Collect Words..." };
    TZMenuItemType gridtypes[] = { itTitle, itSimple, itSimple, itAltTitle, itSimple, itSimple, itAltTitle, itSimple, itSimple, itTitle, itSimple, itSimple };
    TZPopupCommand gridcmd[] = { pcNone, pcKanjiInfo, pcTranslate, pcNone, pcCopy1, pcAppend1, pcNone, pcCopy2, pcAppend2, pcNone, pcToGroup, pcCollect };

    wchar_t *gridnamesextra[] = { L"Definition", L"Edit...", L"" };
    TZMenuItemType gridtypesextra[] = { itTitle, itSimple, itEmpty };
    TZPopupCommand gridcmdextra[] = { pcNone, pcEditKanjiDef, pcNone };
    for (unsigned int ix = 0; ix < (sizeof(gridnames) / sizeof(wchar_t*)) / 3; ++ix)
    {
        TZMenuRow *row = gridmenu->AddRow();
        row->Setup(gridnames + (ix*3), gridtypes + (ix*3), (void**)(gridcmd + (ix*3)));
    }
    for (unsigned int ix = 0; ix < (sizeof(gridnamesextra) / sizeof(wchar_t*)) / 3; ++ix)
    {
        TZMenuRow *row = gridmenuextra->AddRow();
        row->Setup(gridnamesextra + (ix*3), gridtypesextra + (ix*3), (void**)(gridcmdextra + (ix*3)));
    }

    gridmenucombined->AppendRows(gridmenu);
    gridmenucombined->AppendRows(gridmenuextra);
}
//---------------------------------------------------------------------------
__fastcall TfGroup::~TfGroup()
{
    SettingsChangeEvent(this, settingsChanged, false);

    delete gridmenucombined;
    delete gridmenuextra;
    delete gridmenu;

    delete edKFilter;
    delete edWFilter;
    delete ewobj;
    delete kobj;
    delete kwobj;
    delete kbox;
    delete wobj;

    delete kresults;
}
//---------------------------------------------------------------------------
void TfGroup::settingsChanged(void *caller, TSettingsProcState state)
{
    if (state != spsEnd)
        return;

    wobj->rowsize = rowsizes(settings.dictfontsize);
    wobj->headersize = rowsizes(settings.dictfontsize);

    ewobj->rowsize = rowsizes(settings.dictfontsize);
    ewobj->headersize = rowsizes(settings.dictfontsize);

    kwobj->rowsize = rowsizes(settings.dictfontsize);
    kwobj->headersize = rowsizes(settings.dictfontsize);

    kbox->rowsize = rowsizes(settings.dictfontsize);
    kbox->headersize = rowsizes(settings.dictfontsize);

    kobj->Redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caNone;
    fMain->Close();
}
//---------------------------------------------------------------------------
void TfGroup::kcallback()
{
    sbMatch->Down = false;

    if (fInfo && pbGrid->Focused() && Pages->TabIndex == 0 && (Screen->ActiveForm == this || fDock && Screen->ActiveForm == (TForm*)fDock))
    {
        fInfo->collection = cbDictionary->ItemIndex;
        if (kobj->kanji)
            fInfo->kanji = kobj->kanji;
    }

    btnKLeft->Enabled = kobj->count && kobj->simplesel && !kobj->selected[0];
    btnKRight->Enabled = kobj->count && kobj->simplesel && !kobj->selected[kobj->count - 1];
    btnKRemove->Enabled = kobj->count && kobj->pos >= 0;
    cbGroupEx->Enabled = kobj->count && kobj->pos >= 0;
    btnKAddStudy->Enabled = kobj->selcount > 0;

    if (fDock)
    {
        fDock->miGKKanjiInfo->Enabled = Pages->TabIndex == 0 && kobj->count && kobj->pos >= 0 && !kobj->hasselected;
        fDock->miGKTranslateKanji->Enabled = kobj->count && kobj->pos >= 0 && !kobj->hasselected;
        fDock->miGKKanjiDef->Enabled = Pages->TabIndex == 0 && kobj->count && kobj->pos >= 0 && cbDictionary->ItemIndex > 0;
    }

    if (Pages->TabIndex == 0 && fDock)
    {
        fDock->miGKGenerate->Enabled = kobj->count && kobj->pos >= 0;
        fDock->miGKCopyKanji->Enabled = kobj->count && kobj->pos >= 0;
        fDock->miGKAppendKanji->Enabled = kobj->count && kobj->pos >= 0;
        fDock->miGKCopyAll->Enabled = kobj->count;
        fDock->miGKAppendAll->Enabled = kobj->count;
    }

    fdeditstate = mesUnchanged;
    SetMeaningMemoText();

    if (kbox->collection != kobj->collection)
        kbox->collection = kobj->collection;
    kbox->kanji = kobj->kanji;

    if (edKFilter->Text.Length())
        edKFilter->Text = L"";

    collect_examples();

    set_kinfo();
}
//---------------------------------------------------------------------------
void TfGroup::kwcallback()
{
    set_info();

    if (kwobj->pos < 0)
    {
        sbAddWord->Enabled = false;
        add_del(true);
        sbAddWord->Visible = true;
        sbAddToReading->Visible = false;
        return;
    }

    if (kwobj->rowcolors[kwobj->pos] == wcUncolored)
    {
        if (can_add_word())
        {
            sbAddWord->Enabled = true;
            add_del(true);
            sbAddWord->Visible = true;
            sbAddToReading->Visible = false;
        }
        else
        {
            sbAddToReading->Visible = true;
            sbAddWord->Visible = false;
        }
    }
    else
    {
        sbAddWord->Enabled = true;
        add_del(false);
        sbAddWord->Visible = true;
        sbAddToReading->Visible = false;
    }
}
//---------------------------------------------------------------------------
int TfGroup::ewindex(unsigned int ix)
{
    int re = (cbKunGroup->Checked ? kbox->pos : compact_index(kbox->kanji, kbox->pos));

    TKanjiCard *kc = Collection->kanjidat[kbox->kanji->index].card;
    return kc->ReadingExample[re][kc->ReadingExampleCount[re] - ix - 1];
}
//---------------------------------------------------------------------------
UnicodeString TfGroup::ewmeaning(int ix)
{
    int ewix = ewindex(ix);
    if (Collection->deflist->ItemSet(ewix))
    {
        UnicodeString r = Collection->deflist->Items[ewix];
        if (r.Length())
            return r;
    }
    return ewobj->meaningtext[ix];
}
//---------------------------------------------------------------------------
void TfGroup::ewprepare(int col, int row, TWordColumnType ctype, TCanvas *c)
{
    if (ctype == wcMeaning && Collection->deflist->ItemSet(ewindex(row)))
        c->Font->Style = c->Font->Style << fsBold;
}
//---------------------------------------------------------------------------
bool TfGroup::can_add_word()
{
    if (!kbox->kanji || kwobj->pos < 0)
        return false;

    if (!kbox->kanji || kwobj->pos < 0)
        return false;

    TWord *w = kwobj->currentword;
    if (!w)
        return false;

    int len = wcslen(w->kanji);
    bool r = false;
    int sel = (cbKunGroup->Checked ? kbox->pos : compact_index(kbox->kanji, kbox->pos));
    for (int i = 0; i < len && !r; i++)
        if (w->kanji[i] == kbox->kanji->ch)
            r = max(0, FindReading(i, w)) == sel;

    return r;
}
//---------------------------------------------------------------------------
void TfGroup::bcallback()
{
    if (sbMatch->Down || cbGroupEx->ItemIndex == 4)
        collect_examples();
    else
        color_examples();

    if (!btnKCard->Down)
        return;

    if (kbox->kanji)
    {
        int re = cbKunGroup->Checked ? kbox->pos : compact_index(kbox->kanji, kbox->pos);
        int rc = Collection->kanjidat[kbox->kanji->index].card->ReadingExampleCount[re];
        ewobj->Tag = kobj->pos;
        ewobj->set_virtual(Collection, rc, ewindex);
        ewcolorize();
    }
    else
    {
        ewobj->Tag = -1;
        ewobj->set_virtual(Collection, 0, NULL);
    }

    set_info();
}
//---------------------------------------------------------------------------
void TfGroup::ewcolorize()
{
    for (int i = 0; i < ewobj->count; i++)
        if (kobj->isexample[kobj->pos][ewobj->indexes[i]])
            ewobj->rowcolors[i] = wcRed;
        else
            ewobj->rowcolors[i] = wcUncolored;
}
//---------------------------------------------------------------------------
void TfGroup::FillKList()
{
    lbKGroups->Items->BeginUpdate();
    try
    {
        lbKGroups->Items->Clear();
        for (int i = 0; i < klist->Count; i++)
        {
            lbKGroups->Items->Add(klist->Names[i]);
            lbKGroups->Checked[i] = klist->Checked[i];
        }
    }
    catch (...)
    {
        ;
    }
    lbKGroups->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void TfGroup::UpdateKList()
{
    FillKList();
    lbKGroups->ItemIndex = 0;
    lbKGroupsClick(lbKGroups);
}
//---------------------------------------------------------------------------
void TfGroup::UpdateKButtons()
{
    if (!Collection)
        return;
    btnKUp->Enabled = lbKGroups->ItemIndex > 0;
    btnKDown->Enabled = lbKGroups->ItemIndex < lbKGroups->Count - 1;
    btnKChkAll->Enabled = lbKGroups->Count;
    btnKChkNone->Enabled = lbKGroups->Count;

    btnKDelete->Enabled = lbKGroups->ItemIndex >= 0;
    btnKRename->Enabled = lbKGroups->ItemIndex >= 0;

    btnKAdd->Enabled = lbKGroups->ItemIndex >= 0 && fKanji->Count;
    btnKRemove->Enabled = kobj->count;
}
//---------------------------------------------------------------------------
int TfGroup::AddKanjiGroup(byte coll, UnicodeString s)
{
    int i = -1;
    if (cbDictionary->ItemIndex != coll)
    {
        cbDictionary->ItemIndex = coll;
        cbDictionaryChange(cbDictionary);
    }

    s = s.Trim();
    if (s.Length() > group_name_length_limit)
        s.SetLength(group_name_length_limit);

    MODON
    i = klist->IndexOf(s);

    if (i < 0)
    {
        i = klist->Add(s);
        dictionary->Items[coll]->Modify(false);
    }
    KanjiGroupAdded(s);
    MODOFF return i;
}
//---------------------------------------------------------------------------
void TfGroup::KanjiGroupAdded(UnicodeString s)
{
    lbKGroups->Items->BeginUpdate();
    try
    {
        FillKList();
        lbKGroups->ItemIndex = StriIndex(s, lbKGroups->Items);
    }
    catch (...)
    {
        ;
    }
    lbKGroups->Items->EndUpdate();

    lbKGroupsClick(lbKGroups);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKNewClick(TObject *Sender)
{
    UnicodeString s;
    if (QueryBox(L"Enter name for the new kanji group:", L"New group", s) && s.Trim().Length())
    {
        AddKanjiGroup(cbDictionary->ItemIndex, s);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKRenameClick(TObject *Sender)
{
    UnicodeString s = klist->Names[kgroup];
    if (QueryBox(L"Enter new name for this group:", L"Rename group", s) && s.Trim().Length())
    {
        s = s.Trim();
        if (s.Length() > group_name_length_limit)
            s.SetLength(group_name_length_limit);

        if (StriIndex(s, lbKGroups->Items) < 0)
        {
            MODON
            int i = kgroup;
            klist->Names[i] = s;
            lbKGroups->Items->Strings[i] = s;
            lbKGroups->ItemIndex = i;
            Collection->Modify(false);
            MODOFF
        }
        else if (s.LowerCase() != lbKGroups->Items->Strings[lbKGroups->ItemIndex].LowerCase())
            MsgBox(L"Group with this name already exists.", L"Message", MB_OK);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKDeleteClick(TObject *Sender)
{
    if (!klist->Items[kgroup]->Count || MsgBox(L"Do you really want to delete this group?", L"Query", MB_YESNO) == IDYES)
    {
        MODON
        int i = kgroup;
        klist->Delete(i);
        lbKGroups->Items->BeginUpdate();
        try
        {
            FillKList();
            lbKGroups->ItemIndex = MIN(lbKGroups->Count - 1, i);
        }
        catch (...)
        {
            ;
        }
        lbKGroups->Items->EndUpdate();
        lbKGroupsClick(lbKGroups);
        Collection->Modify(false);
        MODOFF
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::lbKGroupsClick(TObject *Sender)
{
    kobj->set_items((kgroup >= 0 ? klist->Items[kgroup] : NULL), Collection);
    UpdateKButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::FormKeyPress(TObject *Sender, wchar_t &Key)
{
    ;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    /*if (Shift.Contains(ssCtrl) && Key == VK_TAB) {
     ChangeFocus(Shift.Contains(ssShift));
     return;
     }
     */
    if (Key == VK_ESCAPE && InfoWindowVisible())
    {
        HideInfoWindows(true);
        Key = 0;
    }
    if (Key >= '1' && Key <= '9' && Shift.Contains(ssAlt) && cbDictionary->ItemIndex != Key - '1' && cbDictionary->Items->Count > Key - '1')
    {
        cbDictionary->ItemIndex = Key - '1';
        cbDictionaryChange(cbDictionary);
    }
}
//---------------------------------------------------------------------------
bool TfGroup::get_modified()
{
    return dictionary->Modified;
}
//---------------------------------------------------------------------------
void TfGroup::SaveSettings()
{
    settings.studycolkanji = kwobj->colsize[1];
    settings.studycolkana = kwobj->colsize[2];
}
//---------------------------------------------------------------------------
void TfGroup::LoadSettings()
{
    if (!settings.columnreset)
    {
        kwobj->colsize[1] = settings.studycolkanji;
        kwobj->colsize[2] = settings.studycolkana;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::FormShow(TObject *Sender)
{
    static bool firsttime = true;
    if (firsttime)
    {
        firsttime = false;
        //fcollection = dictionary->Items[MAX(0,cbDictionary->ItemIndex)];
        LoadSettings();

        int w = GetSystemMetrics(SM_CXVSCROLL);

        pGroupBtn->Width -= w - 16 - 1;
    }
    //if (!settings.debug)
    //    dlgSave->Filter = L"zKanji export (*.zex)|*.zex";

    SetCaption();

    if (needexamples)
        do_collect_examples();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKChkAllClick(TObject *Sender)
{
    MODON
    bool mod = false;
    for (int i = 0; i < lbKGroups->Count; i++)
    {
        if (!klist->Checked[i])
            mod = true;
        klist->Checked[i] = true;
        lbKGroups->Checked[i] = true;
    }
    if (mod)
        Collection->Modify(false);
    MODOFF
    if (cbGroupEx->ItemIndex == 3)
        collect_examples();
    UpdateKButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKChkNoneClick(TObject *Sender)
{
    MODON
    bool mod = false;
    for (int i = 0; i < lbKGroups->Count; i++)
    {
        if (klist->Checked[i])
            mod = true;
        klist->Checked[i] = false;
        lbKGroups->Checked[i] = false;
    }
    if (mod)
        Collection->Modify(false);
    MODOFF
    if (cbGroupEx->ItemIndex == 3)
        collect_examples();
    UpdateKButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::lbKGroupsClickCheck(TObject *Sender)
{
    MODON
    bool mod = false;
    for (int i = 0; i < lbKGroups->Count; i++)
    {
        if (klist->Checked[i] != lbKGroups->Checked[i])
        {
            mod = true;
            klist->Checked[i] = lbKGroups->Checked[i];
        }
    }
    if (mod)
        Collection->Modify(false);
    MODOFF

    UpdateKButtons();
}
//---------------------------------------------------------------------------
int TfGroup::AddWordGroup(byte coll, UnicodeString s)
{
    if (cbDictionary->ItemIndex != coll)
    {
        cbDictionary->ItemIndex = coll;
        cbDictionaryChange(cbDictionary);
    }
    int i;
    s = s.Trim();
    if (s.Length() > group_name_length_limit)
        s.SetLength(group_name_length_limit);
    if ((i = StriIndex(s, lbWGroups->Items)) < 0)
    {
        MODON
        wlist->Add(s);
        WordGroupAdded(s);
        dictionary->Items[coll]->Modify(false);
        MODOFF
    }
    else
    {
        lbWGroups->ItemIndex = i;
        lbWGroupsClick(lbWGroups);
    }
    return i;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnWNewClick(TObject *Sender)
{
    UnicodeString s;
    if (QueryBox(L"Enter name for the new word group:", "New group", s) && s.Trim().Length())
        AddWordGroup(cbDictionary->ItemIndex, s);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnWDeleteClick(TObject *Sender)
{
    int i = wgroup;
    int j = cbToGroup->ItemIndex;
    if (!wlist->Lists[i]->Count || MsgBox(L"Do you really want to delete this group?", "Question", MB_YESNO) == IDYES)
    {
        MODON
        wlist->Delete(i);

        lbWGroups->Items->BeginUpdate();
        lbWGroups->Items->Delete(i);
        lbWGroups->ItemIndex = MIN(lbWGroups->Items->Count - 1, i);
        lbWGroups->Items->EndUpdate();

        if (cbToGroup->Items->Count)
        {
            cbToGroup->Items->BeginUpdate();
            cbToGroup->Items->Delete(i);
            cbToGroup->ItemIndex = MIN(cbToGroup->Items->Count - 1, j);
            cbToGroup->Items->EndUpdate();
        }

        lbWGroupsClick(lbWGroups);

        Collection->Modify(false);
        MODOFF
    }
}
//---------------------------------------------------------------------------
void TfGroup::FillWList()
{
    lbWGroups->Items->BeginUpdate();
    try
    {
        lbWGroups->Items->Clear();
        for (int i = 0; i < wlist->Count; i++)
        {
            lbWGroups->Items->Add(wlist->Names[i]);
            lbWGroups->Checked[i] = wlist->Checked[i];
        }
    }
    catch (...)
    {
    }
    lbWGroups->Items->EndUpdate();

    if (sbVocab->Down)
    {
        cbToGroup->Items->Assign(lbWGroups->Items);
        if (cbToGroup->ItemIndex < 0 && cbToGroup->Items->Count)
            cbToGroup->ItemIndex = 0;
    }
    else
        cbToGroup->Items->Clear();
}
//---------------------------------------------------------------------------
void TfGroup::UpdateWList()
{
    FillWList();
    lbWGroups->ItemIndex = 0;
    lbWGroupsClick(lbWGroups);
}
//---------------------------------------------------------------------------
void TfGroup::UpdateWButtons()
{
    btnWUp->Enabled = wgroup > 0;
    btnWDown->Enabled = wgroup < lbWGroups->Items->Count - 1 && wgroup >= 0;
    btnWDelete->Enabled = wgroup >= 0;
    btnWRename->Enabled = wgroup >= 0;

    if (fDock)
    {
        fDock->miGWAppendKana->Enabled = Pages->TabIndex == 1 && wobj->pos >= 0;
        fDock->miGWCopyKana->Enabled = Pages->TabIndex == 1 && wobj->pos >= 0;
        fDock->miGWCopyWord->Enabled = Pages->TabIndex == 1 && wobj->pos >= 0;
        fDock->miGWAppendWord->Enabled = Pages->TabIndex == 1 && wobj->pos >= 0;
    }

    btnWNew->Enabled = sbVocab->Down;

    btnWChkAll->Enabled = lbWGroups->Count > 0;
    btnWChkNone->Enabled = lbWGroups->Count > 0;

    btnWordDelete->Enabled = wobj->count && (sbVocab->Down || wobj->count - wobj->sellength >= 10);

    btnWStudyList->Enabled = (sbVocab->Down && lbWGroups->Items->Count > 0 || sbStudy->Down && lbWGroups->ItemIndex >= 0 && Collection->studygroups->Items[lbWGroups->ItemIndex]->list->Count >= 10);
    btnWStudyList->Caption = (sbStudy->Down && lbWGroups->ItemIndex >= 0 ? (wstudygroups->Tests[lbWGroups->ItemIndex]->inited ? L"Continue test" : L"Start the test") : L"Study this group");
    btnWStudyList->Hint = (sbStudy->Down ? L"Start studying words or continue a suspended test" : L"Create new study group from checked groups");
    btnWReset->Enabled = (sbStudy->Down && lbWGroups->ItemIndex >= 0);

    bool filtered = lbWGroups->ItemIndex >= 0 && wlist->Lists[lbWGroups->ItemIndex]->Count != wobj->count;
    btnWordUp->Enabled = wobj->selstart > 0 && !filtered && wobj->selseqcount < 2;
    btnWordDown->Enabled = wobj->selstart >= 0 && wobj->selstart + wobj->sellength < wobj->count && !filtered && wobj->selseqcount < 2;
    btnWAddStudy->Enabled = wobj->sellength > 0;

    //miTranslateWritten->Enabled = wobj->pos >= 0 && wobj->sellength == 1;
    //miTranslateKana->Enabled = wobj->pos >= 0 && wobj->sellength == 1;

    if (fDock)
    {
        fDock->miGWEditWord->Enabled = /*cbDictionary->ItemIndex > 0 &&*/wobj->pos >= 0 && Pages->TabIndex == 1 && wobj->sellength == 1;
        //fDock->miGKDeleteWord->Enabled = cbDictionary->ItemIndex > 0 && wobj->pos >= 0 && Pages->TabIndex == 1 && wobj->sellength == 1;
        fDock->miGWToVocab->Enabled = wobj->pos >= 0 && Pages->TabIndex == 1 && wobj->sellength == 1;
    }

    if (Pages->TabIndex == 1)
    {
        bool haskanji = false;
        if (wobj->pos >= 0)
        {
            TWord *w = wobj->currentword;
            for (unsigned int i = 0; i < wcslen(w->kanji) && !haskanji; i++)
                haskanji = KANJI(w->kanji[i]);
        }

        if (fDock)
        {
            fDock->miGKGenerate->Enabled = haskanji;
            fDock->miGKCopyKanji->Enabled = wobj->count;
            fDock->miGKAppendKanji->Enabled = wobj->count;
        }
    }
    if (fDock)
        fDock->miGWPrintVocab->Enabled = Pages->TabIndex == 1;

    cbToGroup->Enabled = sbVocab->Down && wobj->pos >= 0 && cbToGroup->Items->Count > 0;
    btnToGroup->Enabled = cbToGroup->Enabled && cbToGroup->ItemIndex != lbWGroups->ItemIndex && cbToGroup->ItemIndex >= 0;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnWRenameClick(TObject *Sender)
{
    int i = wgroup;
    int j = cbToGroup->ItemIndex;
    UnicodeString s = wlist->Names[wgroup];
    if (QueryBox(L"Enter new name for this group:", L"Rename group", s) && s.Trim().Length())
    {
        s = s.Trim();
        if (s.Length() > group_name_length_limit)
            s.SetLength(group_name_length_limit);

        if (StriIndex(s, lbWGroups->Items) < 0)
        {
            MODON
            wlist->Names[i] = s.Trim();

            lbWGroups->Items->BeginUpdate();
            lbWGroups->Items->Strings[i] = s;
            lbWGroups->ItemIndex = i;
            lbWGroups->Items->EndUpdate();

            if (cbToGroup->Items->Count)
            {
                cbToGroup->Items->BeginUpdate();
                cbToGroup->Items->Strings[i] = s;
                cbToGroup->ItemIndex = j;
                cbToGroup->Items->EndUpdate();
            }

            Collection->Modify(false);
            MODOFF
        }
        else if (s.LowerCase() != lbWGroups->Items->Strings[i].LowerCase())
            MsgBox(L"Group with the same name already exists.", L"Error", MB_OK);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnWChkAllClick(TObject *Sender)
{
    MODON
    bool mod = false;
    for (int i = 0; i < lbWGroups->Count; i++)
    {
        if (!wlist->Checked[i])
            mod = true;
        wlist->Checked[i] = true;
        lbWGroups->Checked[i] = true;
    }
    if (mod)
        Collection->Modify(false);
    MODOFF

            UpdateWButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnWChkNoneClick(TObject *Sender)
{
    MODON
    bool mod = false;
    for (int i = 0; i < lbWGroups->Count; i++)
    {
        if (wlist->Checked[i])
            mod = true;
        wlist->Checked[i] = false;
        lbWGroups->Checked[i] = false;
    }
    if (mod)
        Collection->Modify(false);
    MODOFF

            UpdateWButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::sbVocabClick(TObject *Sender)
{
    UpdateWList();
}
//---------------------------------------------------------------------------
bool TfGroup::AddVocabulary(TWordCollection *collection, int index)
{
    int res;
    fAddVocab = new TfAddVocab(Application);

    try
    {
        if ((res = fAddVocab->ShowModalEx(collection, index)) == mrOk)
        {
            int coll = fAddVocab->CollectionIndex + fAddVocab->cbCollection->Tag;
            if (fAddVocab->CollectionIndex == -1) // New dictionary.
                DictionaryAdded(fAddVocab->cbCollection->Text);
            else
            {
                WordGroupAdded(coll, fAddVocab->cbGroups->Text);
                wobj->Update(wobj->FindWord(fAddVocab->index, fAddVocab->meaning, true));
            }
        }
    }
    catch (...)
    {
        ;
    }

    delete fAddVocab;
    fAddVocab = NULL;
    return res == mrOk;
}
//---------------------------------------------------------------------------
void TfGroup::WordGroupAdded(UnicodeString s)
{
    if (!sbVocab->Down)
        return;

    lbWGroups->Items->BeginUpdate();
    lbWGroups->Items->Add(s);
    lbWGroups->ItemIndex = lbWGroups->Items->Count - 1;
    lbWGroups->Items->EndUpdate();

    int j = cbToGroup->ItemIndex;
    cbToGroup->Items->BeginUpdate();
    cbToGroup->Items->Add(s);
    cbToGroup->ItemIndex = j;
    cbToGroup->Items->EndUpdate();

    lbWGroupsClick(lbWGroups);
}
//---------------------------------------------------------------------------
void TfGroup::WordGroupAdded(byte coll, UnicodeString s)
{
    if (Pages->TabIndex != 1 && cbDictionary->ItemIndex != coll)
        return;

    int gindex = StriIndex(s, lbWGroups->Items);

    if (cbDictionary->ItemIndex != coll)
    {
        cbDictionary->ItemIndex = coll;
        SelectDictionary(cbDictionary->ItemIndex);
    }
    else
    {
        if (gindex < 0 && s.Length())
            WordGroupAdded(s);
        WordChanged(coll, NULL);
    }

    if (!s.Length())
        return;

    if (!sbVocab->Down)
    {
        sbVocab->Down = true;
        //if (Visible && Pages->TabIndex == 1)
        //sbVocab->SetFocus();
        FillWList();
    }
    lbWGroups->ItemIndex = (gindex < 0 ? lbWGroups->Count - 1 : gindex);
    UpdateWButtons();
    wobj->wordlist = (lbWGroups->ItemIndex >= 0 ? wlist->Lists[lbWGroups->ItemIndex] : NULL);
}
//---------------------------------------------------------------------------
void TfGroup::WordChanged(byte coll, TChangedWordList *list)
{
    if (coll != cbDictionary->ItemIndex)
        return;

    if (Pages->TabIndex == 0)
    {
        kobj->Redraw();
        kcallback();
    }
    else
        wobj->Update(wobj->pos);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::lbWGroupsClick(TObject *Sender)
{
    wobj->wordlist = (lbWGroups->ItemIndex >= 0 ? wlist->Lists[lbWGroups->ItemIndex] : NULL);
    UpdateWButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::lbWGroupsClickCheck(TObject *Sender)
{
    MODON
    bool mod = false;
    for (int i = 0; i < lbWGroups->Count; i++)
    {
        if (wlist->Checked[i] != lbWGroups->Checked[i])
        {
            mod = true;
            wlist->Checked[i] = lbWGroups->Checked[i];
        }
    }
    if (mod)
        Collection->Modify(false);
    MODOFF

    UpdateWButtons();
}
//---------------------------------------------------------------------------
void TfGroup::set_kinfo()
{
    if (kbox->kanji)
    {
        lbKInfo1->Caption = IntToStr(kobj->count) + L" :";
        lbKInfo2->Caption = IntToStr(kobj->pos + 1);
        if (!kobj->count)
            lbKInfo3->Caption = L"";
        else
            lbKInfo3->Caption = dictionary->KanjiMeaning(max(0, cbDictionary->ItemIndex), kbox->kanji->index);
    }
    else
    {
        lbKInfo1->Caption = IntToStr(kobj->count) + L" :";
        lbKInfo2->Caption = !kobj->count ? L"0" : L"-";
        lbKInfo3->Caption = L"";
    }

}
//---------------------------------------------------------------------------
void TfGroup::set_info()
{
    if (!Collection || (Pages->TabIndex == 0 && btnKCard->Down && !sbKWords->Down))
    {
        sbInfo->Panels->Items[0]->Text = L"";
        sbInfo->Panels->Items[1]->Text = L"";
        sbInfo->Panels->Items[2]->Text = L"";
        return;
    }

    TWordListObjBase *obj = !KanjiTab->Visible ? (TWordListObjBase*)wobj : btnKCard->Down ? (TWordListObjBase*)ewobj : (TWordListObjBase*)kwobj;
    sbInfo->Panels->Items[0]->Text = IntToStr(obj->count) + L" :";
    sbInfo->Panels->Items[1]->Text = obj->pos + 1;
    if (obj->pos < 0)
        sbInfo->Panels->Items[2]->Text = L" ";
    else
    {
        UnicodeString str;
        if (obj != ewobj || !Collection->deflist->ItemSet(obj->indexes[obj->pos]))
            str = obj->wordnotes;
        else
        {
            for (int i = 0; i < ewobj->words[ewobj->pos]->meaningcnt; i++)
            {
                if (i)
                    str += L" ";
                if (ewobj->words[ewobj->pos]->meaningcnt > 1)
                    str += IntToStr(i) + L") ";
                str += ewobj->words[ewobj->pos]->data[i].meaning;
            }
        }

        sbInfo->Panels->Items[2]->Text = str;
    }
}
//---------------------------------------------------------------------------
void TfGroup::wcallback()
{
    UpdateWButtons();
    set_info();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnWordDeleteClick(TObject *Sender)
{
    int ix = wobj->selstart;

    MODON
    if (sbStudy->Down && wlist->Tests[lbWGroups->ItemIndex]->inited)
    {
        if (MsgBox(L"The test is suspended and it will be reset!\nDo you wish to continue?", L"Warning", MB_YESNO) == IDNO)
            return;
        wlist->Tests[lbWGroups->ItemIndex]->Reset();
    }

    for (int ix = wobj->selseqcount - 1; ix >= 0; --ix)
    {
        for (int iy = wobj->selseq[ix].len - 1; iy >= 0; --iy)
            wlist->DeleteWord(lbWGroups->ItemIndex, wobj->order[wobj->selseq[ix].pos + iy]);
    }

    Collection->Modify(false);
    MODOFF

    wobj->Update(ix);
    UpdateWButtons();
}
//---------------------------------------------------------------------------
int SortWordIndexMeaning(TWordIndex **a, TWordIndex **b)
{
    if ((*a)->index != (*b)->index)
        return (*b)->index - (*a)->index;
    return (*a)->meaning - (*b)->meaning;
}
//---------------------------------------------------------------------------
void TfGroup::AddCheckedWords(TWordIndexList *l, int selected, bool studygroup)
{
    TWordGroupList *g = (studygroup ? Collection->studygroups : Collection->groups);
    int k = g->CheckedCount;
    TTemplateList<TWordIndex, false> *ll = new TTemplateList<TWordIndex, false>;
    try
    {
        for (int j = 0; j < g->Count; j++)
        {
            if (k && !g->Checked[j] || !k && selected != j || !k && selected < 0)
                continue;
            for (int i = 0; i < g->Lists[j]->Count; i++)
            {
                l->Add(g->Lists[j]->Items[i]);
                ll->Add(l->Items[l->Count - 1]);
            }
        }

        // Remove duplicate words with sorting.
        ll->Sort(SortWordIndexMeaning);
        for (int i = ll->Count - 1; i > 0; i--)
            if (ll->Items[i]->index == ll->Items[i - 1]->index && ll->Items[i]->meaning == ll->Items[i - 1]->meaning)
                ll->Items[i]->index = -1;
    }
    catch (...)
    {
        ;
    }
    delete ll;
    for (int i = l->Count - 1; i >= 0; i--)
        if (l->Items[i]->index < 0)
            l->Delete(i);
}
//---------------------------------------------------------------------------
bool IsInt(UnicodeString str)
{
    return UnicodeString(str.ToInt()) == str;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnWStudyListClick(TObject *Sender)
{
    bool named = true;
    bool startstudy = false;
    TWordGroupStudy *g;

    if (!sbStudy->Down)
    {
        UnicodeString groupname = L"Study " + wgroups->Items[lbWGroups->ItemIndex]->name;
        int k = 1;
        for (int i = 0; i < wstudygroups->Count; i++)
        {
            if (wstudygroups->Items[i]->name.SubString(1, 12) == L"Study group " && IsInt(wstudygroups->Items[i]->name.SubString(13, 99)))
                k = MAX(k, wstudygroups->Items[i]->name.SubString(12, 99).ToInt() + 1);
            if (wstudygroups->Items[i]->name == groupname)
                named = false;
        }
        if (!named)
            groupname = L"Study group " + IntToStr(k);

        TWordIndexList *l = new TWordIndexList(dictionary->Items[cbDictionary->ItemIndex]);
        AddCheckedWords(l, lbWGroups->ItemIndex, false);

        fWordStudy = new TfWordStudy(Application);
        try
        {
            // Set study group attributes and rules
            // mrOk - new study group, mrYes - start studying afterwards
            if ((g = fWordStudy->ShowModalEx(Collection, l, groupname)) == NULL)
            {
                delete l;
                delete fWordStudy;
                return;
            }

            if (wstudygroups->IndexOf(fWordStudy->groupname) >= 0)
            {
                MsgBox(L"Study group with same name already exists. Reverting name to default.", L"Message", MB_OK);
                g->name = groupname;
            }

            MODON
            wstudygroups->Insert(0, g);
            for (int i = 0; i < l->Count; i++)
                g->AddWord(l->Items[i]->index, l->Items[i]->meaning);

            startstudy = fWordStudy->ModalResult == mrYes;

            Collection->Modify(false);
            MODOFF

            delete l;
            sbStudy->Down = true;
            sbStudy->Click();
        }
        catch (...)
        {
            ;
        }
        delete fWordStudy;
        lbWGroups->ItemIndex = 0;
    }
    else
        startstudy = lbWGroups->ItemIndex >= 0;

    if (startstudy) // Start our study right away!
    {
        g = wstudygroups->Tests[lbWGroups->ItemIndex];
        if (!WordGroupStudyValid(g->question, g->list))
            return;

        g->Init();

        fWStudy = NULL;
        try
        {
            fWStudy = new TfWStudy(Application);
            HideAllWindows();
            fWStudy->ShowModalEx(g);
        }
        catch(...)
        {
            ;
        }
        ShowAllWindows();
        delete fWStudy;
        fWStudy = NULL;

        UpdateWButtons();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnWResetClick(TObject *Sender)
{
    if (lbWGroups->ItemIndex < 0)
        return;

    bool startstudy;

    fWordStudy = new TfWordStudy(Application);
    TWordGroupStudy *tmp = wstudygroups->Tests[lbWGroups->ItemIndex];

    MODON
    TWordGroupStudy *result = fWordStudy->ShowModalEx2(tmp);
    if (result && result != tmp)
    {
        tmp->Reset();
        wstudygroups->Items[lbWGroups->ItemIndex] = result;
        wobj->wordlist = result->list;
        delete tmp;
    }

    startstudy = (fWordStudy->ModalResult == mrYes);
    MODOFF

            delete fWordStudy;

    if (startstudy)
        btnWStudyListClick(btnWReset);
    else
        UpdateWButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pbKWordsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    //int n;
    if (Button == mbLeft && Shift.Contains(ssDouble))
    {
        wchar_t c = kwobj->KanjiAtPos(x, y);

        if (!c || !KANJI(c))
        {
            int ix = kwobj->IndexAtPos(y);
            if (ix >= 0)
                AddVocabulary(dictionary->Items[cbDictionary->ItemIndex], ix);
        }
        else
            ShowKanjiInfo(cbDictionary->ItemIndex, kanjis->Kanjis[kanjis->IndexOf(c)]);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pbWordsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button == mbLeft && Shift.Contains(ssDouble))
    {
        TPoint p;
        GetCursorPos(&p);
        p = pbWords->ScreenToClient(p);
        int ix;
        char m;
        if (!wobj->WordAtPos(p.y, ix, m))
            return;

        wchar_t c = wobj->KanjiAtPos(x, y);

        if (!c || !KANJI(c))
        {
            if (wobj->IndexAtPos(y) >= 0)
            {
                if (cbDictionary->ItemIndex == 0)
                    fDict->SearchWord(cbDictionary->ItemIndex, wobj->currentword);
                else
                    EditWordDef(dictionary->Items[cbDictionary->ItemIndex], wobj->current, wobj->currentmeaning);
            }

            return;
        }
        ShowKanjiInfo(cbDictionary->ItemIndex, kanjis->Kanjis[kanjis->IndexOf(c)]);
    }
}
//---------------------------------------------------------------------------
TWordGroupList* TfGroup::get_wordgrouplist()
{
    return (sbVocab->Down ? Collection->groups : Collection->studygroups);
}
//---------------------------------------------------------------------------
TKanjiGroupList* TfGroup::get_kanjigrouplist()
{
    return Collection->kgroups;
}
//---------------------------------------------------------------------------
TWordGroupList* TfGroup::get_wstudygroups()
{
    return Collection->studygroups;
}
//---------------------------------------------------------------------------
TWordGroupList* TfGroup::get_wgroups()
{
    return Collection->groups;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::cbDictionaryChange(TObject *Sender)
{
    SelectDictionary(MAX(0, cbDictionary->ItemIndex));
}
//---------------------------------------------------------------------------
void TfGroup::DeselectDictionary()
{
    fcoll = NULL;
    lbKGroups->Items->Clear();
    lbWGroups->Items->Clear();
    kwobj->set_by_indexes(NULL, false);
    kbox->collection = NULL;
    kobj->set_items(NULL, NULL, NULL);
    wobj->wordlist = NULL;
}
//---------------------------------------------------------------------------
void TfGroup::SelectDictionary(byte coll)
{
    if (coll >= cbDictionary->Items->Count || fcoll == dictionary->Items[coll])
        return;

    fcoll = dictionary->Items[coll];
    cbDictionary->ItemIndex = coll;
    if (cbDictionary->ItemIndex < 0)
        cbDictionary->ItemIndex = 0;

    btnRenameDict->Enabled = cbDictionary->ItemIndex > 0;
    btnDeleteDict->Enabled = cbDictionary->ItemIndex > 0;

    //if (fDock)
    //fDock->miGExport->Enabled = (settings.debug || coll > 0);

    FillKList(); // To have the correct kanji groups listed in the listbox before collecting words that happen when changing anything in the kobj.

    UpdateWList();
    UpdateKList();

    SetCaption();

    if (fInfo && (Screen->ActiveForm == this || fDock && Screen->ActiveForm == (TForm*)fDock))
        fInfo->collection = cbDictionary->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnNewDictClick(TObject *Sender)
{
    if (dictionary->Count == 255)
    {
        MsgBox(L"I don't know how you managed it, but you already\nhave the maximum number of dictionaries.", L"Message", MB_OK);
        return;
    }

    UnicodeString s, i;

    if (DictionaryQuery(L"New dictionary", s, i))
    {
        s = s.Trim();
        if (!s.Length())
            return;

        if (s.Length() > dictionary_name_length_limit)
            s.SetLength(dictionary_name_length_limit);

        MODON
        if (dictionary->Add(s) < 0)
        {
            MsgBox(L"Dictionary with the same name already exists.", L"Error", MB_OK);
            return;
        }

        dictionary->Items[dictionary->Count - 1]->info = i;
        dictionary->Items[dictionary->Count - 1]->Modify(true);
        MODOFF

        DictionaryAdded(s);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnRenameDictClick(TObject *Sender)
{
    UnicodeString oldname = dictionary->Strings[cbDictionary->ItemIndex];
    UnicodeString s = oldname;
    UnicodeString i = dictionary->Items[cbDictionary->ItemIndex]->info;
    if (DictionaryQuery(L"Rename dictionary", s, i))
    {
        if (s == oldname && i == dictionary->Items[cbDictionary->ItemIndex]->info)
            return;

        s = s.Trim();
        if (!s.Length())
            return;

        if (s.Length() > dictionary_name_length_limit)
            s.SetLength(dictionary_name_length_limit);

        MODON
        if (!dictionary->Rename(cbDictionary->ItemIndex, s))
        {
            MsgBox(L"Dictionary with the same name already exists.", "Error", MB_OK);
            return;
        }
        else
        {
            int ix = cbDictionary->ItemIndex;
            if (dictionary->Items[ix]->info != i)
            {
                dictionary->Items[ix]->info = i;
                dictionary->Items[ix]->Modify(true);
            }

            if (s != oldname)
            {
                cbDictionary->Items->Strings[ix] = s;
                cbDictionary->ItemIndex = ix;

                fDict->DictionaryRenamed(ix, s);

                if (FileExists(paths.datapath + oldname + L".zkd"))
                    RenameFile(paths.datapath + oldname + L".zkd", paths.datapath + s + L".zkd");
                if (FileExists(paths.datapath + oldname + L".zkg"))
                    RenameFile(paths.datapath + oldname + L".zkg", paths.datapath + s + L".zkg");
            }
        }
        MODOFF
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnDeleteDictClick(TObject *Sender)
{
    int ix = cbDictionary->ItemIndex;
    UnicodeString s = dictionary->Strings[ix];
    if (MsgBox(L"The dictionary with all its groups and added word definitions is to be removed!\nDo you want to continue?", L"Warning", MB_YESNO) == IDYES)
    {
        MODON
        if (ix == cbDictionary->ItemIndex)
            cbDictionary->ItemIndex = 0;

        cbDictionary->Items->Delete(ix);
        DictionaryDeleted(ix);

        dictionary->Delete(ix);
        if (FileExists(paths.datapath + s + L".zkd"))
        {
            DeleteFile(paths.datapath + s + L".zkd");
            if (FileExists(paths.datapath + s + L".zkg"))
                DeleteFile(paths.datapath + s + L".zkg");
        }
        else
        {
            DeleteFile(paths.datapath + s + L".zkk"); // Some very old format. These lines will be killed in the next version.
            DeleteFile(paths.datapath + s + L".zkw");
        }

        SelectDictionary(0);
        MODOFF
    }
}
//---------------------------------------------------------------------------
TWordList* TfGroup::get_words()
{
    return dictionary->Items[cbDictionary->ItemIndex]->words;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miCopyKanjiClick(TObject *Sender)
{
    ClipReplace(kobj->selstring);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miAppendKanjiClick(TObject *Sender)
{
    ClipAppend(kobj->selstring);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miCopyWordClick(TObject *Sender)
{
    ClipReplace(wobj->currentword->kanji);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miAppendWordClick(TObject *Sender)
{
    ClipAppend(wobj->currentword->kanji);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miCopyKanaClick(TObject *Sender)
{
    ClipReplace(wobj->currentword->kana);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miAppendKanaClick(TObject *Sender)
{
    ClipAppend(wobj->currentword->kana);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miToVocabClick(TObject *Sender)
{
    AddVocabulary(dictionary->Items[cbDictionary->ItemIndex], wobj->current);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miEditWordClick(TObject *Sender)
{
    EditWordDef(dictionary->Items[cbDictionary->ItemIndex], wobj->current, wobj->currentmeaning);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miDeleteWordClick(TObject *Sender)
{
    DeleteWordEntry(cbDictionary->ItemIndex, wobj->current, wobj->currentmeaning);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::PagesChange(TObject *Sender)
{
    if (fDock)
    {
        fDock->miGKKanjiDef->Enabled = Pages->TabIndex == 0;
        fDock->miGKKanjiInfo->Enabled = Pages->TabIndex == 0;
        fDock->miGKTranslateKanji->Visible = Pages->TabIndex == 0;
        if (Pages->TabIndex == 0)
            fDock->miGKTranslateKanji->ShortCut = ShortCut('T', TShiftState() << ssCtrl);
        else
            fDock->miGKTranslateKanji->ShortCut = TShortCut();
        fDock->miGWTranslateWritten->Visible = Pages->TabIndex == 1;
        fDock->miGWTranslateKana->Visible = Pages->TabIndex == 1;
        fDock->miGWCopyKana->Visible = Pages->TabIndex == 1;
        fDock->miGWAppendKana->Visible = Pages->TabIndex == 1;
        fDock->miGWCopyWord->Visible = Pages->TabIndex == 1;
        fDock->miGWAppendWord->Visible = Pages->TabIndex == 1;

        fDock->miGWEditWord->Enabled = /*cbDictionary->ItemIndex > 0 &&*/wobj->pos >= 0 && Pages->TabIndex == 1 && wobj->sellength == 1;
        fDock->miGWToVocab->Enabled = wobj->pos >= 0 && Pages->TabIndex == 1 && wobj->sellength == 1;

        fDock->miGKAppendKanji->Enabled = (wobj->pos >= 0 && Pages->TabIndex == 1) || (Pages->TabIndex == 0 && kobj->count);
        fDock->miGKCopyKanji->Enabled = (wobj->pos >= 0 && Pages->TabIndex == 1) || (Pages->TabIndex == 0 && kobj->count);
        fDock->miGKCopyKanji->Caption = (Pages->TabIndex == 0 ? L"Copy kanji" : L"Copy word");
        fDock->miGKAppendKanji->Caption = (Pages->TabIndex == 0 ? L"Append kanji" : L"Append word");
        fDock->miGWCopyKana->Enabled = Pages->TabIndex == 1 && wobj->pos >= 0;
        fDock->miGWAppendKana->Enabled = Pages->TabIndex == 1 && wobj->pos >= 0;
        fDock->miGWCopyWord->Enabled = Pages->TabIndex == 1 && wobj->pos >= 0;
        fDock->miGWAppendWord->Enabled = Pages->TabIndex == 1 && wobj->pos >= 0;
        fDock->miGWPrintVocab->Enabled = Pages->TabIndex == 1;
    }

    switch (Pages->TabIndex)
    {
    case 0:
        kcallback();
        KanjiTab->Visible = true;
        pGroupBtn->Visible = true;
        WordTab->Visible = false;

        if (fDock)
        {
            fDock->miKanjiGroups->Visible = true;
            fDock->miWordGroups->Visible = false;
        }

        break;
    case 1:
        lbWGroupsClick(lbWGroups);
        pGroupBtn->Visible = false;
        WordTab->Visible = true;
        KanjiTab->Visible = false;

        if (fDock)
        {
            fDock->miKanjiGroups->Visible = false;
            fDock->miWordGroups->Visible = true;
        }
        break;
    }

    SetSplitSize(pbKWords->Height);
    sbInfo->BringToFront();

    set_info();

    if (Sender == Pages)
    {
        if (Pages->TabIndex == 0)
        {
            if (kobj->count)
                pbGrid->SetFocus();
            else
                lbKGroups->SetFocus();
        }
        else if (Pages->TabIndex == 1)
        {
            if (wobj->count)
                pbWords->SetFocus();
            else
                lbWGroups->SetFocus();
        }
    }

    SetCaption();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::FormResize(TObject *Sender)
{
    if (Pages->TabIndex == 0)
    {
        TRect r = pKanjiButtons->ClientRect;
        r.Top = pKTButtons->Height + pKTButtons->Top;
        r.Bottom = pKBButtons->Top;
        --r.Right;
        InvalidateRect(pKanjiButtons->Handle, &r, true);
        sbInfo->SetBounds(pKBack->Left, ClientHeight - infoh, ClientWidth - pKBack->Left, infoh);
    }
    else
    {
        TRect r = pWordButtons->ClientRect;
        r.Top = pWTButtons->Height + pWTButtons->Top;
        r.Bottom = pWSButtons->Top;
        --r.Right;
        InvalidateRect(pWordButtons->Handle, &r, true);
        sbInfo->SetBounds(pbWords->Left, ClientHeight - infoh, ClientWidth - pbWords->Left, infoh);
        pWordFilterControls->SetBounds(pbWords->Left, 1, pbWords->Width, pbWords->Top - 1);
    }
}
//---------------------------------------------------------------------------
void TfGroup::KanjiToGroup(byte coll, word group, TKanjiList *kl)
{
    bool changed = false;
    if (cbDictionary->ItemIndex != coll)
    {
        cbDictionary->ItemIndex = coll;
        changed = true;
    }

    lbKGroups->Items->BeginUpdate();
    if (changed)
        cbDictionaryChange(cbDictionary);

    if (group != lbKGroups->ItemIndex)
        lbKGroups->ItemIndex = group;
    lbKGroups->Items->EndUpdate();

    int l = -1;

    MODON
    bool mod = false;
    for (int i = 0; i < kl->Count; i++)
    {
        int ix = klist->Items[group]->IndexOf(kl->Kanjis[i]->index);
        if (ix < 0)
        {
            l = klist->AddKanji(group, kl->Kanjis[i]->index);
            mod = true;
        }
        else
            l = MAX(l, ix);
    }
    if (mod)
        Collection->Modify(false);
    MODOFF

    lbKGroupsClick(lbKGroups);

    kobj->pos = l;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKAddClick(TObject *Sender)
{
    TKanjiItemList *kl = new TKanjiItemList;
    try
    {
        fKanji->FillKanjiList(kl, true);
        KanjiToGroup(cbDictionary->ItemIndex, kgroup, kl);
    }
    catch (...)
    {
    }
    delete kl;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKRemoveClick(TObject *Sender)
{
    if (MsgBox(L"Are you sure you want to remove the selected kanji from the group?", L"Query", MB_YESNO) == IDNO)
        return;

    MODON
    for (int i = kobj->selcount - 1; i >= 0; i--)
        klist->Items[kgroup]->Delete(kobj->selpos[i]);

    Collection->Modify(false);
    MODOFF

    lbKGroupsClick(lbKGroups);
}
//---------------------------------------------------------------------------
void TfGroup::collect_examples()
{
    needexamples = true;
    if (!Visible)
        return;

    do_collect_examples();
}
//---------------------------------------------------------------------------
void TfGroup::do_collect_examples()
{
    static bool collecting = false;

    if (btnKCard->Down || kbox->kanji != kobj->kanji)
        return;

    if (collecting)
        return;

    collecting = true;

    kresults->Clear();
    needexamples = false;

    if (kobj->hasselected || kobj->pos < 0 || kbox->pos < 0 && (sbMatch->Down || cbGroupEx->ItemIndex == 4))
    {
        kwobj->set_by_indexes(NULL);
        collecting = false;
        return;
    }

    if (cbGroupEx->ItemIndex == 4)
    {
        TKanjiCard *kc = Collection->kanjidat[kobj->kanji->index].card;
        TWordIndexList *wl = new TWordIndexList(Collection);
        try
        {
            for (int i = kc->ExampleCount - 1; i >= 0; i--)
                kresults->Add(kc->Examples[i]->ix);
            FilterKResults();
        }
        catch (...)
        {
            ;
        }
        delete wl;
        color_examples();
        collecting = false;
        return;
    }

    TWord *w;
    int j, n;

    double nw = Now();
    bool showing = false;

    try
    {
        for (j = 0; j < dictionary->Items[cbDictionary->ItemIndex]->kanjidat[kobj->kanji->index].wordscnt; j++)
        {
            n = dictionary->Items[cbDictionary->ItemIndex]->kanjidat[kobj->kanji->index].words[j];

            if (btnKFilter->Down)
            {
                if (!showing && (double)Now() - nw > 0.3 / 86400.0 && j > dictionary->Items[cbDictionary->ItemIndex]->kanjidat[kobj->kanji->index].wordscnt / 3)
                {
                    ShowMessageForm(L"Please wait...");
                    TRect r = ClientRect;
                    RedrawWindow(Handle, &r, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN);
                    Application->ProcessMessages();
                    showing = true;
                }
                if (!kanjiwordfilters->match(words->Items[n]))
                    continue;
            }
            kresults->Add(n);
        }

        if (cbGroupEx->ItemIndex)
        {
            bool good;
            UnicodeString z;

            switch (cbGroupEx->ItemIndex)
            {
            case 1:
                z = kobj->kanjistring;
                break;
            case 2:
                z = kobj->kanjistring.SubString(1, kobj->pos + 1);
                break;
            case 3:
                z = kobj->kanji->ch;
                for (int i = 0; i < lbKGroups->Count; i++)
                {
                    if (lbKGroups->Checked[i])
                        z += klist->Items[i]->KanjiString;
                }
                break;
            }

            for (int ix = kresults->Count - 1; ix >= 0; --ix)
            {
                if (!showing && (double)Now() - nw > 0.3 / 86400.0 && ix > kresults->Count / 3)
                {
                    ShowMessageForm(L"Please wait...");
                    TRect r = ClientRect;
                    RedrawWindow(Handle, &r, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN);
                    Application->ProcessMessages();
                    showing = true;
                }
                good = true;
                w = words->Items[kresults->Items[ix]];
                for (j = wcslen(w->kanji) - 1; j >= 0 && good; --j)
                    if (KANJI(w->kanji[j]) && !z.Pos(w->kanji[j]))
                        good = false;
                if (!good)
                    kresults->Delete(ix);
            }

        }

        int sel = (cbKunGroup->Checked ? real_index(kbox->kanji, kbox->pos) : kbox->pos);
        int selb = (cbKunGroup->Checked ? kbox->pos : compact_index(kbox->kanji, kbox->pos));

        for (int ix = kresults->Count - 1; ix >= 0; --ix)
        {
            if (!showing && (double)Now() - nw > 0.3 / 86400.0 && ix > 2 * kresults->Count / 3)
            {
                ShowMessageForm(L"Please wait...");
                TRect r = ClientRect;
                RedrawWindow(Handle, &r, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN);
                Application->ProcessMessages();
                showing = true;
            }
            w = words->Items[kresults->Items[ix]];
            bool good = true;
            if (sbFilter->Down && w->frequency < 1500)
                good = false;
            else if (sbMatch->Down && !IsReading(kobj->kanji, w, sel))
                good = false;
            else if (sbMatch->Down)
            {
                int len = wcslen(w->kanji);
                bool r = false;
                for (int i = 0; i < len && good; i++)
                {
                    if (!r && w->kanji[i] == kbox->kanji->ch)
                        r = max(0, FindReading(i, w)) == selb;
                }
                good = good && r;
            }

            if (!good)
                kresults->Delete(ix);
        }

        FilterKResults();

    }
    catch (...)
    {
        ;
    }

    if (showing)
        HideMessageForm();

    color_examples();
    collecting = false;
}
//---------------------------------------------------------------------------
void TfGroup::color_examples()
{
    if (btnKCard->Down)
        return;

    kwobj->RemoveColor(false);

    if (!kbox->kanji || kbox->pos < 0 || !kwobj->count)
    {
        pbKWords->Invalidate();
        kwcallback();
        return;
    }

    TKanjiCard *kc = Collection->kanjidat[kobj->kanji->index].card;
    int sel = (!cbKunGroup->Checked ? compact_index(kobj->kanji, kbox->pos) : kbox->pos);

    for (int j = 0; j < compact_count(kobj->kanji); j++)
    {
        if (j == sel)
            continue;
        for (int i = kc->ReadingExampleCount[j] - 1; i >= 0; i--)
        {
            int ix = kwobj->IndexOf(kc->ReadingExample[j][i]);
            if (ix >= 0)
                kwobj->rowcolors[ix] = wcRed;
        }
    }

    if (!kc || !kc->ReadingExampleCount[sel])
    {
        pbKWords->Invalidate();
        kwcallback();
        return;
    }

    for (int i = kc->ReadingExampleCount[sel] - 1; i >= 0; i--)
    {
        int ix = kwobj->IndexOf(kc->ReadingExample[sel][i]);
        if (ix >= 0)
            kwobj->rowcolors[ix] = wcYellow;
    }

    pbKWords->Invalidate();
    kwcallback();
}
//---------------------------------------------------------------------------
void TfGroup::add_del(bool add)
{
    if (cbGroupEx->ItemIndex == 4)
        add = false;
    if (sbAddWord->Tag == add)
        return;
    sbAddWord->Tag = add;
    TBmp *bmp = new TBmp;
    try
    {
        data->imgBtn->GetBitmap((add ? 0 : 1), bmp);
        sbAddWord->Glyph->Assign(bmp);
        sbAddWord->Hint = (add ? L"Add selected word to the examples of the kanji" : L"Remove selected word from the examples of the kanji");
    }
    catch (...)
    {
        ;
    }
    delete bmp;

}
//---------------------------------------------------------------------------
void TfGroup::DefChanged()
{
    kobj->Redraw();
    set_info();
    set_kinfo();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::cbGroupExChange(TObject *Sender)
{
    sbMatch->Down = false;
    sbMatch->Enabled = cbGroupEx->ItemIndex != 4;
    sbFilter->Enabled = sbMatch->Enabled;
    btnKFilter->Enabled = sbMatch->Enabled;
    if (!btnKCard->Down)
        collect_examples();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKCardClick(TObject *Sender)
{
    sbMatch->Down = false;
    if (btnKCard->Down)
    {
        pKControls1->Visible = false;
        pKControls2->Visible = true;
        pKWords->Visible = sbKWords->Down;
        pKDef->Visible = sbKDef->Down;
        pbKWords->Visible = false;
    }
    else
    {
        pKControls2->Visible = false;
        pKControls1->Visible = true;
        pbKWords->Visible = true;
        pKDef->Visible = false;
        pKWords->Visible = false;
    }

    collect_examples();
    if (btnKCard->Down)
        bcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pExBarMouseEnter(TObject *Sender)
{
    Screen->Cursor = crVSplit;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pExBarMouseLeave(TObject *Sender)
{
    Screen->Cursor = crDefault;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pExBarMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    mousesizing = false;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pExBarMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    TPoint p;
    GetCursorPos(&p);
    mousey = p.y;
    mousesize = pKWords->Height;
    mousesizing = Screen->Cursor == crVSplit;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pExBarMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    if (!Shift.Contains(ssLeft))
        mousesizing = false;
    if (mousesizing)
    {
        TPoint p;
        GetCursorPos(&p);
        SetSplitSize(mousesize + (mousey - p.y));
        kwsize = pKWords->Height;
    }
}
//---------------------------------------------------------------------------
/*
 void gcallback1(TMenuItem *mi, int ix)
 {
 TKanjiItemList *kl;

 switch (ix) {
 case 4: // Info
 ShowKanjiInfo(fGroup->cbDictionary->ItemIndex,fGroup->kobj->kanji);
 break;
 case 5: // Replace kanji
 ClipReplace(fGroup->kobj->selstring);
 break;
 case 6: // Replace all -- put every kanji in the clipboard
 ClipReplace(fGroup->kobj->kanjistring);
 break;
 case 7: // To group
 kl = new TKanjiItemList();
 try {
 UnicodeString w = fGroup->kobj->selstring;
 for (int i = 1; i <= w.Length(); i++)
 if (kl->IndexOf(w[i]) < 0)
 kl->Add(w[i]);

 KanjiToGroup(kl);
 } catch(...) {
;
 }
 delete kl;
 break;
 case 8: // Translate
 fDict->btnKanji->Click();
 fDict->edkanji->Insert(fGroup->kobj->kanji->ch);
 break;
 case 9: // Append kanji
 ClipAppend(fGroup->kobj->selstring);
 break;
 case 10: // Append all
 ClipAppend(fGroup->kobj->kanjistring);
 break;
 case 11: // generate word list
 fGroup->GenerateWordGroup();
 break;
 }
 }
 //---------------------------------------------------------------------------
 void gcallback2(TMenuItem *mi, int ix)
 {
 TKanjiItemList *kl;
 UnicodeString s;
 switch (ix) {
 case 5: // Info
 ShowKanjiInfo(fGroup->cbDictionary->ItemIndex, fGroup->kobj->kanji);
 break;
 case 6: // Replace kanji
 ClipReplace(fGroup->kobj->selstring);
 break;
 case 7: // Replace all -- put every kanji in the clipboard
 ClipReplace(fGroup->kobj->kanjistring);
 break;
 case 8: // To group
 kl = new TKanjiItemList;
 try {
 UnicodeString w = fGroup->kobj->selstring;
 for (int i = 1; i <= w.Length(); i++)
 if (kl->IndexOf(w[i]) < 0)
 kl->Add(w[i]);

 KanjiToGroup(kl);
 } catch(...) {
;
 }
 delete kl;
 break;
 case 9: // Add/Edit kanji definition
 kl = new TKanjiItemList;
 try {
 for (int i = fGroup->kobj->pos; i < fGroup->kobj->count; i++)
 kl->Add(fGroup->kobj->kanjis[i]);
 ChangeDef(fGroup->cbDictionary->ItemIndex,kl,0);
 } catch(...) {
;
 }
 delete kl;
 break;
 case 10: // Translate
 fDict->btnKanji->Click();
 fDict->edkanji->Insert(fGroup->kobj->kanji->ch);
 break;
 case 11: // append kanji
 ClipAppend(fGroup->kobj->selstring);
 break;
 case 12: // append all
 ClipAppend(fGroup->kobj->kanjistring);
 break;
 case 13: // generate word list
 fGroup->GenerateWordGroup();
 break;
 }
 }
 */
//---------------------------------------------------------------------------
void TfGroup::popupcallback(TZMenuRows *rows, int col, int row)
{
    TZMenuRow *r = rows->Rows[row];
    TZPopupCommand cmd = (TZPopupCommand)r->Data[col];

    if (cmd == pcNone)
        return;

    TKanjiItemList *kl = NULL;
    TWordIndexList *wl = NULL;

    try
    {
        switch (cmd)
        {
        case pcKanjiInfo:
            ShowKanjiInfo(cbDictionary->ItemIndex, kobj->kanji);
            break;
        case pcTranslate:
            fDict->btnKanji->Click();
            fDict->edkanji->Insert(kobj->kanji->ch);
            break;
        case pcCopy1:
            ClipReplace(kobj->selstring);
            break;
        case pcAppend1:
            ClipAppend(kobj->selstring);
            break;
        case pcCopy2:
            ClipReplace(kobj->kanjistring);
            break;
        case pcAppend2:
            ClipAppend(kobj->kanjistring);
            break;
        case pcToGroup:
            kl = new TKanjiItemList(false);
            kl->Add(kobj->selstring);
            ::KanjiToGroup(kl);
            break;
        case pcCollect:
            kl = new TKanjiItemList(false);
            kl->Add(kobj->selstring);
            ::GenerateWordGroup(kl);
            break;
        case pcEditKanjiDef:
            kl = new TKanjiItemList(false);
            kl->Add(kobj->selstring);
            ChangeDef(cbDictionary->ItemIndex, kl, 0);
            break;
        }
    }
    catch (...)
    {
        ;
    }

    delete kl;
    delete wl;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pbGridMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button != mbRight || !kobj->KanjiAtPos(Point(x, y)))
        return;

    TPoint p = pbGrid->ClientToScreen(Point(x, y));
    if (!cbDictionary->ItemIndex)
    {
        TZMenuRow *row = gridmenu->Rows[0];
        row->Enabled[1] = !kobj->hasselected;
        row->Enabled[2] = !kobj->hasselected;
        menu->Popup(gridmenu, p.x, p.y, &popupcallback);
    }
    else
    {
        TZMenuRow *row = gridmenu->Rows[0];
        row->Enabled[1] = !kobj->hasselected;
        row->Enabled[2] = !kobj->hasselected;

        row = gridmenuextra->Rows[0];
        if (kobj->selcount == 1 && !dictionary->Items[cbDictionary->ItemIndex]->kanjidat[kobj->kanji->index].meaning)
            row->Titles[1] = L"Add...";
        else
            row->Titles[1] = L"Edit...";

        menu->Popup(gridmenucombined, p.x, p.y, &popupcallback);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miCopyAllClick(TObject *Sender)
{
    ClipReplace(kobj->kanjistring);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miAppendAllClick(TObject *Sender)
{
    ClipAppend(kobj->kanjistring);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miKanjiInfoClick(TObject *Sender)
{
    if (!kobj->kanji)
        return;
    ShowKanjiInfo(cbDictionary->ItemIndex, kobj->kanji);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miTranslateKanjiClick(TObject *Sender)
{
    if (kobj->kanji)
    {
        fDict->btnKanji->Click();
        fDict->edkanji->Text = kobj->kanji->ch;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miKanjiDefClick(TObject *Sender)
{
    TKanjiItemList *kl = new TKanjiItemList();
    try
    {
        for (int i = kobj->pos; i < kobj->count; i++)
            kl->Add(kobj->kanjis[i]);
        ChangeDef(cbDictionary->ItemIndex, kl, 0);
    }
    catch (...)
    {
        ;
    }
    delete kl;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnWordUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (wobj->selseqcount >= 2)
        return;

    bool up = Sender == btnWordUp;
    TSpeedButton *btn = (TSpeedButton*)Sender;
    if (up && !wobj->selstart || !up && wobj->selstart + wobj->sellength == wobj->count || Button != mbLeft)
        return;

    TWordGroupStudy *ws;
    if (sbStudy->Down && (ws = wlist->Tests[lbWGroups->ItemIndex])->inited)
    {
        int r = MsgBox(L"The test is suspended and it will be reset!\nDo you wish to continue?", L"Warning", MB_YESNO);
        PostMessage(btn->Parent->Handle, WM_LBUTTONUP, 0, (x + btn->Left) | ((y + btn->Top) << 16));
        if (r == IDNO)
            return;

        ws->Reset();
    }
    else
        MouseRepeat(btnWordUpMouseDown, Sender, Button, Shift, x, y);

    MODON
    if (up)
        for (int i = 0; i < wobj->sellength; ++i)
            wlist->Lists[lbWGroups->ItemIndex]->Exchange(wobj->selstart + i - 1, wobj->selstart + i);
    else
        for (int i = wobj->sellength; i > 0; --i)
            wlist->Lists[lbWGroups->ItemIndex]->Exchange(wobj->selstart + i - 1, wobj->selstart + i);

    int selstart;
    int sellength;
    if (up)
    {
        selstart = wobj->selstart - 1;
        sellength = wobj->sellength;
    }
    else
    {
        selstart = wobj->selstart + wobj->sellength;
        sellength = -wobj->sellength;
    }
    wobj->Update(selstart, sellength);

    Collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKLeftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    MouseRepeat(btnKLeftMouseDown, Sender, Button, Shift, x, y);

    bool left = Sender == btnKLeft;
    MODON
    if (left)
        for (int i = 0; i < kobj->selcount; i++)
            klist->Items[kgroup]->Exchange(kobj->selpos[i], kobj->selpos[i] - 1);
    else
        for (int i = kobj->selcount - 1; i >= 0; i--)
            klist->Items[kgroup]->Exchange(kobj->selpos[i] + 1, kobj->selpos[i]);
    kobj->set_items(klist->Items[kgroup], Collection, kobj->selpos[0] + (left ? -1 : 1), kobj->selcount);
    Collection->Modify(false);
    MODOFF
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::FormPaint(TObject *Sender)
{
    TRect r;
    int t;

    // Top bar.
    DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, Rect(0, 0, ClientWidth, pDict->Height + pDict->Top));
    if (Pages->TabIndex == 0) // Kanji groups.
    {
        // Kanji group control buttons bg.
        r = ChildRect(blKTop);
        DrawGrad(Canvas, clWhite, clBtnFace, Rect(0, KanjiTab->Top, pKanjiButtons->Width, r.Top));

        // Around the kanji groups list.
        t = r.Top + blKTop->Height;
        r = ChildRect(blKBottom);
        Canvas->Brush->Color = lbWGroups->Color;
        Canvas->FillRect(Rect(0, t, pKanjiButtons->Width, r.Top));

        Canvas->Brush->Color = clBtnFace;

        // Size bar bottom.
        r = pExBar->ClientRect;
        TPoint p = ScreenToClient(pExBar->ClientToScreen(Point(0, 0)));
        OffsetRect(&r, p.x, p.y);
        DrawGrad(Canvas, clBtnFace, COLORDEC(clBtnFace, 0x101010), r);

        // Size bar top.
        r = ChildRect(pKInfo);
        DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, r);

        // Divider line between the kanji in groups and the readings box.
        p = ScreenToClient(pKTop->ClientToScreen(Point(0, 0)));
        Canvas->Pen->Color = clBtnShadow;
        Canvas->MoveTo(p.x + pKTop->Width, p.y);
        Canvas->LineTo(p.x + pKTop->Width, p.y + pKTop->Height);

        // Control buttons for positioning or removing kanji.
        r = ChildRect(pKControls);
        DrawGrad(Canvas, clBtnFace, COLORDEC(clBtnFace, 0x080808), r, false);

        // Long-term study buttons top light.
        r = ChildRect(blKBottom, 0, blKBottom->Height, 0, 0);
        DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, Rect(r.Left, r.Top, r.Right, r.Bottom + 17*scaleres));

        // Long-term study buttons body bg.
        r.Bottom = r.Top + pKBButtons->Height - blKBottom->Height;
        r.Top += 17 * scaleres;
        DrawGrad(Canvas, clBtnFace, COLORDEC(clBtnFace, 0x060606), r);

        // Bg for readings bar with checkbox.
        DrawButtonBar(Canvas, ChildRect(pGroupBtn));
    }
    else // Word groups.
    {
        // Kanji group control buttons bg.
        r = ChildRect(blWTop);
        DrawGrad(Canvas, clWhite, clBtnFace, Rect(0, WordTab->Top, pWordButtons->Width, r.Top));

        // Around the word groups list.
        t = r.Top + blWTop->Height;
        r = ChildRect(blWBottom);
        Canvas->Brush->Color = lbWGroups->Color;
        Canvas->FillRect(Rect(0, t, pWordButtons->Width, r.Top));

        // Bg for controls for managing words in groups.
        r = ChildRect(pWordFilterControls);
        DrawGrad(Canvas, clBtnFace, COLORDEC(clBtnFace, 0x080808), r);

        // Long-term study buttons top light.
        r = ChildRect(blWBottom2, 0, blWBottom2->Height, 0, 0);
        DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, Rect(r.Left, r.Top, r.Right, r.Bottom + 17*scaleres));

        // Long-term study buttons body bg.
        r.Bottom = r.Top + pWBButtons->Height - blWBottom2->Height;
        r.Top += 17 * scaleres;
        DrawGrad(Canvas, clBtnFace, COLORDEC(clBtnFace, 0x060606), r);

        // Study group buttons bg.
        r = ChildRect(pWSButtons);
        DrawButtonBar(Canvas, r);
    }
}
//---------------------------------------------------------------------------
void TfGroup::GenerateWordGroup()
{
    if (fDock && fDock->ActiveControl && fDock->ActiveControl->Owner == fKanji)
    {
        fKanji->GenerateWordGroup();
        return;
    }

    if (!fDock->miGKGenerate->Enabled)
        return;

    //TKanjiCardList *kl = new TKanjiCardList(Collection);
    TKanjiItemList *kl = new TKanjiItemList(Collection);

    try
    {
        if (Pages->TabIndex == 0)
        {
            UnicodeString w = kobj->selstring;
            for (int i = 1; i <= w.Length(); i++)
                kl->Add(w[i]);
        }
        else
        {
            UnicodeString w;
            for (int i = 0; i < wobj->sellength; i++)
                w += wobj->words[i + wobj->selstart]->kanji;

            for (int i = 1; i <= w.Length(); i++)
                if (KANJI(w[i]) && kl->IndexOf(w[i]) < 0)
                    kl->Add(w[i]);

        }
        if (kl->Count)
            ::GenerateWordGroup(kl);
    }
    catch (...)
    {
        ;
    }
    delete kl;
}
//---------------------------------------------------------------------------
void TfGroup::WordsToGroup(byte coll, UnicodeString agroup, TWordIndexList *wl)
{
    // Used for example when creating word group from kanji.

    int g;
    agroup = agroup.Trim();
    if ((g = dictionary->Items[coll]->groups->IndexOf(agroup)) < 0)
        g = dictionary->Items[coll]->groups->Add(agroup);

    int x;

    dictionary->BeginWordChange(coll);
    try
    {
        for (int i = 0; i < wl->Count; i++)
        {
            if (dictionary->Items[coll] == wl->collection)
                x = wl->Indexes[i];
            else
            {
                x = dictionary->WordIndex(coll, wl->Words[i]->kanji, wl->Words[i]->kana);
                if (x < 0)
                {
                    for (int j = 0; j < wl->Words[i]->meaningcnt; j++)
                    {
                        if (x < 0)
                            x = dictionary->AddWord(coll, wl->Words[i], 0, L"");
                        else
                            dictionary->CopyWordMeaning(coll, x, wl->Words[i], j);
                    }
                }
            }

            for (int j = 0; j < dictionary->Items[coll]->words->Items[x]->meaningcnt; j++)
            {
                if (!dictionary->Items[coll]->groups->WordInGroup(g, x, j))
                    dictionary->Items[coll]->groups->Items[g]->AddWord(x, j);
            }
        }
    }
    catch (...)
    {
        ;
    }
    dictionary->EndWordChange();

    if (coll != cbDictionary->ItemIndex)
    {
        cbDictionary->ItemIndex = coll;
        cbDictionaryChange(cbDictionary);
    }

    if (!sbVocab->Down)
    {
        sbVocab->Down = true;
        //if (Pages->TabIndex == 1)
        //sbVocab->SetFocus();
    }
    FillWList();
    lbWGroupsClick(lbWGroups);

    if (Pages->TabIndex != 1)
    {
        Pages->TabIndex = 1;
        PagesChange(Pages);
    }

    lbWGroups->ItemIndex = g;
    UpdateWButtons();
    wobj->wordlist = dictionary->Items[coll]->groups->Items[g]->list;
    pbWords->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miPrintVocabClick(TObject *Sender)
{
    fVocabPrint = new TfVocabPrint(Application);
    try
    {
        TWordIndexList *l = new TWordIndexList(dictionary->Items[cbDictionary->ItemIndex]/*,true*/);
        try
        {
            AddCheckedWords(l, lbWGroups->ItemIndex, sbStudy->Down);
            fVocabPrint->ShowModalEx(l);
        }
        catch (...)
        {
            ;
        }
        delete l;
    }
    catch (...)
    {
        ;
    }
    delete fVocabPrint;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::sbMatchClick(TObject *Sender)
{
    collect_examples();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pbWordsKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == L' ')
    {
        if (btnWAddStudy->Enabled)
            btnWAddStudy->Click();
    }
    else if ((Key >= L'A' && Key <= L'Z') || (Key >= L'a' && Key <= L'z'))
    {
        edWFilter->SetFocus();
        pbWFilter->OnKeyPress(Sender, Key);
    }
    else if (Key == 0x08) // Backspace key.
    {
        edWFilter->SetFocus();
        pbWFilter->OnKeyPress(Sender, Key);
    }
    else if (Key == 0x0D) // Enter key.
        edWFilter->SetFocus();
    else if (Key == 0x1B) // Escape key.
        edWFilter->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pbWordsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_DELETE)
    {
        if (btnWordDelete->Enabled)
            btnWordDeleteClick(NULL);
    }
//    else if (Key == VK_SPACE)
//    {
//        if (btnWAddStudy->Enabled)
//            btnWAddStudy->Click();
//    }
//    else if (Key >= 'A' && Key <= 'Z')
//    {
//        edWFilter->SetFocus();
//        pbWFilter->OnKeyDown(Sender, Key, Shift);
//    }
//    else if (Key == VK_BACK)
//    {
//        edWFilter->SetFocus();
//        pbWFilter->OnKeyDown(Sender, Key, Shift);
//    }
//    else if (Key == VK_RETURN)
//        edWFilter->SetFocus();
//    else if (Key == VK_ESCAPE)
//        edWFilter->SetFocus();

}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pbGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    switch (Key)
    {
    case VK_DELETE:
        if (btnKRemove->Enabled)
            btnKRemoveClick(NULL);
        break;
    case VK_RETURN:
        if (kobj->kanji)
            ShowKanjiInfo(cbDictionary->ItemIndex, kobj->kanji);
        break;
    case VK_SPACE:
        if (btnKAddStudy->Enabled)
            btnKAddStudy->Click();
        break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::cbToGroupChange(TObject *Sender)
{
    UpdateWButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnToGroupClick(TObject *Sender)
{
    if (!sbVocab->Down)
        THROW(L"Error! Can't move to study lists.");

    int cnt = 0;
    for (int ix = 0; ix < wobj->selseqcount; ++ix)
    {
        for (int iy = 0; iy < wobj->selseq[ix].len; ++iy)
        {
            if (!wlist->WordInGroup(cbToGroup->ItemIndex, wobj->indexes[wobj->selseq[ix].pos + iy], wobj->meaningindexes[wobj->selseq[ix].pos + iy]))
            {
                wlist->Items[cbToGroup->ItemIndex]->AddWord(wobj->indexes[wobj->selseq[ix].pos + iy], wobj->meaningindexes[wobj->selseq[ix].pos + iy]);
                ++cnt;
            }
        }
    }

    MsgBox((L"Words copied to the group: " + IntToStr(cnt)).c_str(), L"Message", MB_OK);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::cbKunGroupClick(TObject *Sender)
{
    kbox->grouping = cbKunGroup->Checked;
    if (kbox->kanji && Visible)
        pbReadings->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::dlgOpenShow(TObject *Sender)
{
    data->ModalBegin(NULL, ((TCommonDialog*)Sender)->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::dlgOpenClose(TObject *Sender)
{
    data->ModalEnd(((TCommonDialog*)Sender)->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miExportClick(TObject *Sender)
{
/*
    dlgSave->InitialDir = settings.exportdir;

    MODON
    if (ExecuteDialog(dlgSave))
    {
        settings.exportdir = ExtractFileDir(dlgSave->FileName);
        ShowMessageForm(L"Exporting dictionary, please wait.");
        try
        {
            if (settings.debug && ExtractFileExt(dlgSave->FileName) == L".csv")
                dictionary->SQLExport(dlgSave->FileName, cbDictionary->ItemIndex);
            else
                dictionary->Export(dlgSave->FileName, cbDictionary->ItemIndex);
            if (error_code)
                THROW(((UnicodeString)L"Error occurred during dictionary export! (code: " + error_code + L")").c_str());
            HideMessageForm();
        }
        catch (const wchar_t *c)
        {
            HideMessageForm();
            MsgBox(c, L"Error", MB_OK);
        }
        catch (Exception &e)
        {
            HideMessageForm();
            MsgBox(e.Message.c_str(), L"Error", MB_OK);
        }
        catch (...)
        {
            HideMessageForm();
            MsgBox(L"An error occurred during dictionary export.", L"Error", MB_OK);
        }
        error_code = 0;
    }
    MODOFF
*/
}
//---------------------------------------------------------------------------
//void ImportDictionary(byte coll, bool wakangroups = false); //defined at importwords.cpp
void __fastcall TfGroup::miImportClick(TObject *Sender)
{
/*
    MODON
    try
    {
        ImportDictionary(max(1, cbDictionary->ItemIndex));
        if (error_code)
            THROW(((UnicodeString)L"Error occurred during dictionary import! (code: " + error_code + L")").c_str());
    }
    catch (const wchar_t *c)
    {
        fMain->StopAutosave();
        error_code = 1;
        MsgBox(((UnicodeString)c + L" Exiting to protect data! Please restart zkanji.").c_str(), L"Error", MB_OK);
    }
    catch (...)
    {
        fMain->StopAutosave();
        error_code = 1;
        MsgBox(L"An unexpected error occurred during dictionary import. Exiting to protect data! Please restart zkanji.", L"Error", MB_OK);
    }
    if (error_code)
    {
        dictionary->DisableSave();
        fMain->Close();
    }
    MODOFF
*/
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miGExportClick(TObject *Sender)
{
/*
    dlgGExport->InitialDir = settings.exportdir;

    error_code = 0;
    fGExport = new TfGExport(Application);
    if (fGExport->ShowModalEx(cbDictionary->ItemIndex) == mrOk)
    {
        if (dlgGExport->Execute())
        {
            settings.exportdir = ExtractFileDir(dlgGExport->FileName);
            ShowMessageForm(L"Exporting groups, please wait...");
            try
            {
                dictionary->ExportGroups(dlgGExport->FileName, fGExport->cbDict->ItemIndex, fGExport->cbWord->Checked, fGExport->cbWordStudy->Checked, fGExport->cbKanji->Checked, fGExport->cbKanjiExample->Checked);
                if (error_code)
                    THROW(((UnicodeString)L"Error occurred during dictionary export! (code: " + error_code + L")").c_str());
            }
            catch (const wchar_t *c)
            {
                MsgBox(c, L"Error", MB_OK);
            }
            catch (Exception &e)
            {
                MsgBox(e.Message.c_str(), L"Error", MB_OK);
            }
            catch (...)
            {
                MsgBox(L"An error occurred during group export.", L"Error", MB_OK);
            }
            HideMessageForm();
            error_code = 0;
        }
    }
    delete fGExport;
*/
}
//---------------------------------------------------------------------------
void TfGroup::SetSplitSize(int s)
{
    bool grow = false;

    if (kwsize >= 0) // kwsize is < 0 only for the very first time.
    {
        s = MAX(MIN(s, KanjiTab->Height - 170 * scaleres), 0);
        grow = pKWords->Height <= s;
    }
    else
    {
        kwsize = s;
    }

    if (s == pKWords->Height && pKWords->Width == pKWords->Parent->Width && pKWords->Top == pKWords->Parent->Height - pKWords->Height)
        return;

    //pKWords->Height = s;

    HDWP dp = BeginDeferWindowPos(5);
    dp = DeferWindowPos(dp, pKWords->Handle, NULL, 0, pKBack->Height - s, pKBack->Width, s, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    dp = DeferWindowPos(dp, pKDef->Handle, NULL, 0, pKBack->Height - s, pKBack->Width, s, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    dp = DeferWindowPos(dp, pbKWords->Handle, NULL, 0, pKBack->Height - s, pKBack->Width, s, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    dp = DeferWindowPos(dp, pExBar->Handle, NULL, 0, pKBack->Height - s - pExBar->Height, pKBack->Width, pExBar->Height, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    dp = DeferWindowPos(dp, pKanjiTop->Handle, NULL, 0, 0, pKBack->Width, pKBack->Height - s - pExBar->Height, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    //dp = DeferWindowPos(dp,pKControls->Handle,NULL,0,pKBack->Height-s-pExBar->Height-pKControls->Height,0,0,SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOSIZE);
    EndDeferWindowPos(dp);

    if (Pages->TabIndex == 0 && Visible)
    {
        InvalidateEmptyArea(pKanjiTop);
        InvalidateEmptyArea(pExBar);
        if ((sbKWords->Down || !btnKCard->Down) && grow)
            UpdateWindow((btnKCard->Down ? pbEWords : pbKWords)->Handle);
        if (!grow)
        {
            UpdateWindow(pbReadings->Handle);
            UpdateWindow(pbGrid->Handle);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::sbKRemoveMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft)
        return;
    MouseRepeat(sbKRemoveMouseDown, Sender, Button, Shift, X, Y);

    int p = ewobj->pos;
    MODON
    Collection->kanjidat[kobj->kanji->index].card->RemoveExample(ewobj->current);
    Collection->Modify(false);
    MODOFF

    kbox->invalidate();
    bcallback();
    ewobj->pos = p;

    if (!pbEWords->Focused())
        pbEWords->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::sbKRemoveMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    pbEWords->SetFocus();
}
//---------------------------------------------------------------------------
void TfGroup::ewcallback()
{
    sbKRemove->Enabled = ewobj->count;

    sbKUp->Enabled = ewobj->pos > 0;
    sbKDown->Enabled = ewobj->pos < ewobj->count - 1;

    if (!ewobj->count)
    {
        edKMeaning->Text = L"";
        edKMeaning->Enabled = false;
        lbKMeaning->Enabled = false;
        btnKSave->Enabled = false;
        btnKClear->Enabled = false;
        sbKMember->GroupIndex = 0;
        sbKMember->Down = false;
        sbKMember->Enabled = false;
    }
    else
    {
        edKMeaning->Enabled = true;
        lbKMeaning->Enabled = true;

        bool sel = kobj->isexample[kobj->pos][ewobj->current];
        sbKMember->Enabled = sel || kobj->cards[kobj->pos]->examples->Count < EXAMPLE_SEL_MAX;
        if (sbKMember->Enabled)
        {
            if (sel)
            {
                sbKMember->GroupIndex = 1;
                sbKMember->Down = true;
            }
            else
            {
                sbKMember->GroupIndex = 0;
                sbKMember->Down = false;
            }
        }

        if (!Collection->deflist->ItemSet(ewobj->indexes[ewobj->pos]))
        {
            edKMeaning->Font->Color = clGrayText;
            edKMeaning->Text = ewobj->meaningtext[ewobj->pos];
            btnKClear->Enabled = false;
            feditstate = mesUnchanged;
        }
        else
        {
            edKMeaning->Font->Color = clWindowText;
            edKMeaning->Text = Collection->deflist->Items[ewobj->indexes[ewobj->pos]];
            btnKClear->Enabled = true;
            feditstate = mesUserDef;
        }
        btnKSave->Enabled = feditstate == mesModified;
    }

    set_info();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::edKMeaningEnter(TObject *Sender)
{
    edKMeaning->ShowHint = false;
    if (feditstate == mesUnchanged) // Gray old meaning.
    {
        edKMeaning->Text = L"";
        edKMeaning->Font->Color = clWindowText;
    }

}
//---------------------------------------------------------------------------
void __fastcall TfGroup::edKMeaningExit(TObject *Sender)
{
    edKMeaning->ParentShowHint = true;
    if (feditstate == mesUnchanged)
    {
        edKMeaning->Text = ewobj->meaningtext[ewobj->pos];
        edKMeaning->Font->Color = clGrayText;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::edKMeaningChange(TObject *Sender)
{
    if (edKMeaning->Modified)
    {
        feditstate = mesModified;
        btnKSave->Enabled = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKSaveClick(TObject *Sender)
{
    if (edKMeaning->Text.Trim().Length())
    {
        MODON
        Collection->deflist->Items[ewobj->indexes[ewobj->pos]] = edKMeaning->Text.Trim().c_str();
        Collection->Modify(false);
        MODOFF

        btnKClear->Enabled = true;
        feditstate = mesUserDef;
    }
    else
    {
        MODON
        Collection->deflist->Items[ewobj->indexes[ewobj->pos]] = NULL;
        Collection->Modify(false);
        MODOFF

        feditstate = mesUnchanged;
        btnKClear->Enabled = false;
        edKMeaningExit(Sender);
    }
    pbEWords->SetFocus();
    btnKSave->Enabled = false;
    set_info();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::edKMeaningKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    HWND h = GetCapture();
    if (h)
        return;

    if (Key == VK_UP || Key == VK_DOWN)
    {
        if (ewobj->count)
        {
            pbEWords->SetFocus();
            pbEWords->OnKeyDown(Sender, Key, Shift);
        }
        Key = 0;
    }
    else if (Key == VK_RETURN && btnKSave->Enabled)
    {
        btnKSave->Click();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pbEWordsKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key != VK_BACK && Key != VK_RETURN && Key != VK_ESCAPE && Key != VK_SPACE)
    {
        edKMeaning->SetFocus();
        edKMeaning->SelLength = 0;
        edKMeaning->Text = edKMeaning->Text + Key;
        edKMeaning->SelStart = edKMeaning->Text.Length();
        feditstate = mesModified;
        btnKSave->Enabled = true;
    }

    if (Key == VK_SPACE)
    {
        if (sbKMember->Enabled)
        {
            sbKMember->Click();
        }
    }

    if (Key == VK_BACK)
    {
        edKMeaning->SetFocus();
        edKMeaning->SelLength = 0;
        if (edKMeaning->Text.Length())
        {
            edKMeaning->Text = edKMeaning->Text.SubString(1, edKMeaning->Text.Length() - 1);
            feditstate = mesModified;
            btnKSave->Enabled = true;
        }
        edKMeaning->SelStart = edKMeaning->Text.Length();
    }
    if (Key == VK_RETURN || Key == VK_ESCAPE)
        edKMeaning->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::sbKWordsClick(TObject *Sender)
{
    if (sbKWords->Down)
    {
        pKWords->Visible = true;
        pKDef->Visible = false;
    }
    else if (sbKDef->Down)
    {
        pKDef->Visible = true;
        pKWords->Visible = false;
    }
    else
        THROW(L"How did I get here?");
    set_info();
}
//---------------------------------------------------------------------------
void TfGroup::SetMeaningMemoText()
{
    if (!kobj->kanji)
    {
        mDefinition->Text = L"";
        mDefinition->Enabled = false;
        return;
    }
    mDefinition->Enabled = true;

    if (fdeditstate == mesModified)
        return;

    if (Collection->kdeflist->ItemSet(kobj->kanji->index))
    {
        mDefinition->Text = Collection->kdeflist->Items[kobj->kanji->index];
        mDefinition->Font->Color = clWindowText;
        btnKDSave->Enabled = false;
        btnKDReset->Enabled = false;
        btnKDClear->Enabled = true;
        fdeditstate = mesUserDef;
    }
    else
    {
        fdeditstate = mesUnchanged;
        if (mDefinition->Focused())
        {
            mDefinition->Font->Color = clWindowText;
        }
        else
        {
            mDefinition->Font->Color = clGrayText;
            if (cbDictionary->ItemIndex && Collection->kanjidat[kobj->kanji->index].meaning)
            {
                mDefinition->Text = Collection->kanjidat[kobj->kanji->index].meaning;
            }
            else
            {
                mDefinition->Text = kobj->kanji->meaning;
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::mDefinitionChange(TObject *Sender)
{
    if (mDefinition->Modified)
    {
        btnKDSave->Enabled = true;
        btnKDReset->Enabled = Collection->kdeflist->ItemSet(kobj->kanji->index);
        btnKDClear->Enabled = true;
        fdeditstate = mesModified;
    }
    else
    {
        btnKDSave->Enabled = false;
        btnKDReset->Enabled = false;
        btnKDClear->Enabled = fdeditstate == mesUserDef;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKDSaveClick(TObject *Sender)
{
    if (mDefinition->Text.Trim().Length())
    {
        MODON
        Collection->kdeflist->Items[kobj->kanji->index] = mDefinition->Text.Trim().c_str();
        Collection->Modify(true);
        MODOFF

        fdeditstate = mesUserDef;
        mDefinition->SetFocus();
        mDefinition->SelLength = 0;
        mDefinition->SelStart = mDefinition->Text.Length();
    }
    else
    {
        MODON
        Collection->kdeflist->ClearIndex(kobj->kanji->index);
        Collection->Modify(true);
        MODOFF

        fdeditstate = mesUnchanged;
        if (mDefinition->Focused())
            pbGrid->SetFocus();
    }
    SetMeaningMemoText();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::mDefinitionEnter(TObject *Sender)
{
    if (fdeditstate == mesUnchanged)
    {
        mDefinition->Text = L"";
        mDefinition->Font->Color = clWindowText;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::mDefinitionExit(TObject *Sender)
{
    if (fdeditstate == mesUnchanged)
        SetMeaningMemoText();
    else if (fdeditstate == mesModified)
        mDefinition->Text = mDefinition->Text.Trim();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKDResetClick(TObject *Sender)
{
    fdeditstate = mesUnchanged;
    SetMeaningMemoText();
    if (fdeditstate != mesUnchanged)
    {
        mDefinition->SetFocus();
        mDefinition->SelLength = 0;
        mDefinition->SelStart = mDefinition->Text.Length();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKDClearClick(TObject *Sender)
{
    MODON
    Collection->kdeflist->ClearIndex(kobj->kanji->index);
    Collection->Modify(false);
    MODOFF

    fdeditstate = mesUnchanged;
    if (mDefinition->Focused())
        pbGrid->SetFocus();
    SetMeaningMemoText();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKClearClick(TObject *Sender)
{
    btnKSave->Enabled = false;
    btnKClear->Enabled = false;
    MODON
    Collection->deflist->ClearIndex(ewobj->indexes[ewobj->pos]);
    Collection->Modify(false);
    MODOFF

    ewobj->invalidaterow(ewobj->pos);
    pbEWords->SetFocus();
    feditstate = mesUnchanged;
    edKMeaningExit(Sender);
    set_info();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::sbKUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button != mbLeft)
        return;

    int delta = (Sender == sbKUp ? -1 : +1);
    MouseRepeat(sbKUpMouseDown, Sender, Button, Shift, x, y);

    int re = (cbKunGroup->Checked ? kbox->pos : compact_index(kbox->kanji, kbox->pos));

    TKanjiCard *kc = Collection->kanjidat[kbox->kanji->index].card;
    MODON
    kc->SwapExamples(re, ewobj->indexes[ewobj->pos], ewobj->indexes[ewobj->pos + delta]);
    Collection->Modify(false);
    MODOFF

            int n = ewobj->pos;

    ewobj->Update(n + delta);

    ewcolorize();

    if (!pbEWords->Focused())
        pbEWords->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miTranslateWrittenClick(TObject *Sender)
{
    if (!((TMenuItem*)Sender)->Parent->Visible)
        return;

    if (wobj->pos >= 0)
    {
        TWord *w = wobj->currentword;
        fDict->btnKanji->Click();
        fDict->edkanji->Text = w->kanji;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miTranslateKanaClick(TObject *Sender)
{
    if (!((TMenuItem*)Sender)->Visible || !((TMenuItem*)Sender)->Parent->Visible)
        return;

    if (wobj->pos >= 0)
    {
        TWord *w = wobj->currentword;
        fDict->btnKanji->Click();
        fDict->edkanji->Text = w->kana;
    }
}
//---------------------------------------------------------------------------
//TWordCollection* TfGroup::get_collection()
//{
//    return dictionary->Items[max(0, cbDictionary->ItemIndex)];
//}
//---------------------------------------------------------------------------
int TfGroup::get_kanji_group()
{
    return lbKGroups->ItemIndex;
}
//---------------------------------------------------------------------------
int TfGroup::get_word_group()
{
    return lbWGroups->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::sbKMemberClick(TObject *Sender)
{
    if (!kobj->isexample[kobj->pos][ewobj->current])
    {
        MODON
        Collection->kanjidat[kobj->kanji->index].card->SelectExample(kobj->card, ewobj->current);
        Collection->Modify(false);
        MODOFF

        sbKMember->GroupIndex = 1;
        sbKMember->Down = true;
    }
    else
    {
        sbKMember->GroupIndex = 0;
        sbKMember->Down = false;
        MODON
        Collection->kanjidat[kobj->kanji->index].card->DeselectExample(kobj->card, ewobj->current);
        Collection->Modify(false);
        MODOFF
    }
    ewobj->rowcolors[ewobj->pos] = !kobj->isexample[kobj->pos][ewobj->current] ? wcUncolored : wcRed;
    ewobj->invalidaterow(ewobj->pos);
    if (!pbEWords->Focused())
        pbEWords->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    MouseRepeat(btnKUpMouseDown, Sender, Button, Shift, X, Y);

    int delta = Sender == btnKUp ? -1 : 1;

    int i = lbKGroups->ItemIndex + delta;
    MODON
    klist->Exchange(kgroup + delta, kgroup);
    Collection->Modify(false);
    MODOFF

    lbKGroups->Items->Exchange(i, i - delta);
    lbKGroups->ItemIndex = i;
    UpdateKButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::lbKGroupsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    switch (Key)
    {
    case VK_DOWN:
        if (lbKGroups->Count > lbKGroups->ItemIndex + 1)
        {
            if (lbKGroups->Header[lbKGroups->ItemIndex + 1])
                lbKGroups->ItemIndex += 2;
            else
                lbKGroups->ItemIndex++;
            lbKGroupsClick(lbKGroups);
        }
        Key = 0;
        break;
    case VK_UP:
        if (lbKGroups->ItemIndex > 0)
        {
            if (lbKGroups->Header[lbKGroups->ItemIndex - 1])
                lbKGroups->ItemIndex -= 2;
            else
                lbKGroups->ItemIndex--;
            lbKGroupsClick(lbKGroups);
        }
        Key = 0;
        break;
    case VK_PRIOR:
        if (lbKGroups->ItemIndex > 0)
        {
            int j = MAX(0, lbKGroups->ItemIndex - MAX(1, lbKGroups->Height / lbKGroups->ItemHeight - 1));
            if (lbKGroups->Header[j])
                j--;
            lbKGroups->ItemIndex = j;
            lbKGroupsClick(lbKGroups);
        }
        Key = 0;
        break;
    case VK_NEXT:
        if (lbKGroups->Count > lbKGroups->ItemIndex + 1)
        {
            int j = MIN(lbKGroups->Count - 1, lbKGroups->ItemIndex + MAX(1, lbKGroups->Height / lbKGroups->ItemHeight - 1));
            if (lbKGroups->Header[j])
                j++;
            lbKGroups->ItemIndex = j;
            lbKGroupsClick(lbKGroups);
        }
        Key = 0;
        break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKAddStudyClick(TObject *Sender)
{
    TWordIndexList *l = NULL;
    try
    {
        l = new TWordIndexList(Collection);

        for (int i = 0; i < kobj->selcount; ++i)
            for (int iy = 0; iy < Collection->kanjidat[kobj->kanjis[kobj->selpos[i]]->index].card->ExampleCount; ++iy)
                l->Add(Collection->kanjidat[kobj->kanjis[kobj->selpos[i]]->index].card->Examples[iy]->ix);
        ListImport(l);
    }
    catch (...)
    {
        ;
    }
    delete l;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnWAddStudyClick(TObject *Sender)
{

    TWordIndexList *l = NULL;
    try
    {
        l = new TWordIndexList(Collection);
        for (int ix = 0; ix < wobj->selseqcount; ++ix)
        {
            for (int iy = 0; iy < wobj->selseq[ix].len; ++iy)
                l->Add(wobj->indexes[wobj->selseq[ix].pos + iy]);
        }

        ListImport(l);
    }
    catch (...)
    {
        ;
    }
    delete l;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKEditStudyClick(TObject *Sender)
{
    bool aftertest = false;
    do
    {
        fStudyList = new TfStudyList(Application);
        int r;
        try
        {
            r = fStudyList->ShowModalEx(cbDictionary->ItemIndex, aftertest);
        }
        catch (...)
        {
            r = mrCancel;
            MsgBox(L"An error occurred while managing the long-term study list.", L"Error", MB_OK);
        }
        delete fStudyList;
        fStudyList = NULL;

        if (r != mrOk)
            break;

        TWordStudyList *wg = Collection->wordstudylist;

        TModalResult res = wg->begintest();
        if (res == mrOk)
        {
            HideAllWindows();
            fTest = NULL;
            int r;
            try
            {
                fTest = new TfTest(Application);
                r = fTest->ShowModalEx(wg);
            }
            catch (...)
            {
                ;
            }
            aftertest = fTest->AnswerGiven;

            delete fTest;
            fTest = NULL;

            wg->finishtest();
            ShowAllWindows();

            if (r == mrOk)
                MsgBox(L"Test finished for today. Well done!", L"Message", MB_OK);
        }
        else if (res == mrYes)
        {
            HideAllWindows();
            wg->readingstest();
            ShowAllWindows();
        }
    }
    while (true);

    SetCaption();
}
//---------------------------------------------------------------------------
void TfGroup::SetCaption()
{
    UnicodeString str = cbDictionary->Text + L" Study groups";
    Caption = str;

    int d = Collection->wordstudylist->itemsdue();
    if (d)
    {
        btnKEditStudy->Caption = (UnicodeString)L"  Due: " + d;
        btnWEditStudy->Caption = (UnicodeString)L"  Due: " + d;
        btnKEditStudy->Font->Style = TFontStyles() << fsBold;
        btnWEditStudy->Font->Style = TFontStyles() << fsBold;
    }
    else
    {
        btnKEditStudy->Caption = L"Long-term study";
        btnWEditStudy->Caption = L"Long-term study";
        btnKEditStudy->Font->Style = TFontStyles();
        btnWEditStudy->Font->Style = TFontStyles();
    }

    if (fDock)
        fDock->SetCaption();
}
//---------------------------------------------------------------------------
void TfGroup::z_daychange(TMessage &msg)
{
    if (fStudyList || fTest)
        return;

    Collection->wordstudylist->daychanged();
    SetCaption();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::sbAddWordMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft)
        return;

    if (!kobj->kanji)
        return;
    if (!Collection->kanjidat[kobj->kanji->index].card)
        THROW(L"No card created! Bad code! 01");

    if (sbAddWord->Tag == 1)
    {
        MODON
        Collection->kanjidat[kobj->kanji->index].card->AddExample(kbox->compact_index(), kwobj->current);
        kbox->invalidate();
        kwobj->rowcolors[kwobj->pos] = wcYellow;
        add_del(false);

        if (!kobj->isexample[kobj->pos][kwobj->current] && kobj->cards[kobj->pos]->examples->Count < EXAMPLE_SEL_MAX)
            Collection->kanjidat[kobj->kanji->index].card->SelectExample(kobj->card, kwobj->current);
        Collection->Modify(false);
        MODOFF
    }
    else
    {
        MODON
        Collection->kanjidat[kobj->kanji->index].card->RemoveExample(kwobj->current);
        Collection->Modify(false);
        MODOFF

        kbox->invalidate();
        if (cbGroupEx->ItemIndex == 4)
        {
            int p = kwobj->pos;
            collect_examples();
            kwobj->pos = p;
            MouseRepeat(sbAddWordMouseDown, Sender, Button, Shift, X, Y);
            return;
        }
        else
            kwobj->rowcolors[kwobj->pos] = wcUncolored;
        add_del(true);
    }

    kwcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::sbAddWordMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    pbKWords->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::sbAddToReadingClick(TObject *Sender)
{
    if (kwobj->pos < 0)
        return;
    int pos;

    int len = wcslen(kwobj->currentword->kanji);
    for (pos = 0; pos < len - 1; ++pos)
        if (kwobj->currentword->kanji[pos] == kbox->kanji->ch)
            break;

    int i = FindReading(pos, kwobj->currentword);
    if (i < 0)
    {
        i = 0;
        //THROW(L"Don't remove this line or the compiler compiles this function wrong.";
    }
    kbox->pos = cbKunGroup->Checked ? i : real_index(kbox->kanji, i);

    sbAddWordMouseDown(sbAddWord, mbLeft, TShiftState(), 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pbGridMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button == mbLeft && Shift.Contains(ssDouble))
    {
        if (!kobj->KanjiAtPos(Point(X, Y)))
            return;

        if (kobj->kanji)
            ShowKanjiInfo(cbDictionary->ItemIndex, kobj->kanji);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pKBackResize(TObject *Sender)
{
    SetSplitSize(MIN(MAX(kwsize, pKWords->Height), KanjiTab->Height - 160));
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miNewWordClick(TObject *Sender)
{
    AddNewWord(fGroup->cbDictionary->ItemIndex);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miGenerateClick(TObject *Sender)
{
    GenerateWordGroup();
}
//---------------------------------------------------------------------------
void TfGroup::kfcallback()
{
    FilterKResults();
}
//---------------------------------------------------------------------------
void TfGroup::FilterKResults()
{
    TWordCollection *col = dictionary->Items[cbDictionary->ItemIndex];

    TWordIndexList *ll = new TWordIndexList(col);

    UnicodeString filterstr = Hiraganize(RemoveUseless(edKFilter->Text).c_str());

    ll->Capacity = kresults->Count;
    if (!filterstr.Length())
    {
        for (int ix = 0; ix < kresults->Count; ++ix)
            ll->Add(kresults->Items[ix]);
    }
    else
    {
        for (int ix = 0; ix < kresults->Count; ++ix)
            if (!wcsncmp(Hiraganize(col->words->Items[kresults->Items[ix]]->kana).c_str(), filterstr.c_str(), filterstr.Length()))
                ll->Add(kresults->Items[ix]);
    }

    ll->KanjiSort();
    kwobj->set_by_indexes(ll, true);
    color_examples();

    set_info();
    delete ll;
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pbKFilterKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_UP || Key == VK_DOWN)
    {
        if (kwobj->count)
        {
            pbKWords->SetFocus();
            pbKWords->OnKeyDown(Sender, Key, Shift);
        }
        Key = 0;
    }
    if (Key == VK_ESCAPE || Key == VK_RETURN)
        edKFilter->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pbKWordsKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == L' ')
    {
        if (sbAddWord->Enabled && sbAddWord->Visible)
            sbAddWordMouseDown(sbAddWord, mbLeft, TShiftState(), 0, 0);
        else if (sbAddToReading->Enabled && sbAddToReading->Visible)
            sbAddToReading->Click();
    }
    else if ((Key >= L'A' && Key <= L'Z') || (Key >= L'a' && Key <= L'z'))
    {
        edKFilter->SetFocus();
        pbKFilter->OnKeyPress(Sender, Key);
    }
    else if (Key == 0x08) // Backspace key.
    {
        edKFilter->SetFocus();
        pbKFilter->OnKeyPress(Sender, Key);
    }
    else if (Key == 0x1D) // Enter key.
        edKFilter->SetFocus();
    else if (Key == 0x1B) // Escape key.
        edKFilter->SetFocus();

}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pbKWordsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
//    if (Key >= 'A' && Key <= 'Z')
//    {
//        edKFilter->SetFocus();
//        pbKFilter->OnKeyDown(Sender, Key, Shift);
//    }
//    if (Key == VK_BACK)
//    {
//        edKFilter->SetFocus();
//        pbKFilter->OnKeyDown(Sender, Key, Shift);
//    }
//    if (Key == VK_RETURN)
//        edKFilter->SetFocus();
//
//    if (Key == VK_ESCAPE)
//        edKFilter->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::miImportWakanClick(TObject *Sender)
{
/*
    MODON
    try
    {
        ImportDictionary(max(1, cbDictionary->ItemIndex), true);
        if (error_code)
            THROW(((UnicodeString)L"Error occurred during dictionary import! (code: " + error_code + L")").c_str());
    }
    catch (const wchar_t *c)
    {
        fMain->StopAutosave();
        error_code = 1;
        MsgBox(((UnicodeString)c + L" Exiting to protect data! Please restart zkanji.").c_str(), L"Error", MB_OK);
    }
    catch (...)
    {
        fMain->StopAutosave();
        error_code = 1;
        MsgBox(L"An unexpected error occurred during dictionary import. Exiting to protect data! Please restart zkanji.", L"Error", MB_OK);
    }
    if (error_code)
    {
        dictionary->DisableSave();
        fMain->Close();
    }
    MODOFF
*/
}
//---------------------------------------------------------------------------

void __fastcall TfGroup::btnWUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    MouseRepeat(btnWUpMouseDown, Sender, Button, Shift, X, Y);

    char dif = (Sender == btnWUp ? 1 : -1);
    int i = wgroup - dif;
    UnicodeString s = cbToGroup->Text;

    MODON
    wlist->Exchange(i, i + dif);
    Collection->Modify(false);
    MODOFF

            lbWGroups->Items->BeginUpdate();
    lbWGroups->Items->Exchange(i, i + dif);
    lbWGroups->ItemIndex = i;
    lbWGroups->Items->EndUpdate();

    if (cbToGroup->Items->Count)
    {
        cbToGroup->Items->BeginUpdate();
        cbToGroup->Items->Exchange(i, i + dif);
        cbToGroup->ItemIndex = cbToGroup->Items->IndexOf(s);
        cbToGroup->Items->EndUpdate();
    }

    UpdateWButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnKFilterMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (!btnKFilter->Enabled || Button != mbRight || x < 0 || y < 0 || x >= btnKFilter->Width || y >= btnKFilter->Height)
        return;

    TPoint p = btnKFilter->ClientToScreen(Point(0, btnKFilter->Height + 2));
    data->PopupFilters(mkKanjiWords, p.x, p.y);
}
//---------------------------------------------------------------------------
void TfGroup::FilterWResults()
{
    wobj->set_filter(RemoveUseless(edWFilter->Text), btnWFilter->Down ? wordgroupfilters : NULL);
    UpdateWButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnWFilterMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (!btnWFilter->Enabled || Button != mbRight || x < 0 || y < 0 || x >= btnWFilter->Width || y >= btnWFilter->Height)
        return;

    TPoint p = btnWFilter->ClientToScreen(Point(0, btnWFilter->Height + 2));
    data->PopupFilters(mkWordGroups, p.x, p.y);
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::pbWFilterKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_UP || Key == VK_DOWN)
    {
        if (wobj->count)
        {
            pbWords->SetFocus();
            pbWords->OnKeyDown(Sender, Key, Shift);
        }
        Key = 0;
    }
    if (Key == VK_ESCAPE || Key == VK_RETURN)
        edWFilter->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TfGroup::btnWFilterClick(TObject *Sender)
{
    FilterWResults();
}
//---------------------------------------------------------------------------
void TfGroup::wfcallback()
{
    FilterWResults();
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(57);
#endif

