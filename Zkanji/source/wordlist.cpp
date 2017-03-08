//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "wordlist.h"
#include "wordfilter.h"
#include "dropdown.h"
#include "main.h"
#include "clipview.h"
#include "sentences.h"
#include "wordobj.h"
#include "collection.h"
#include "settings.h"
#include "words.h"
#include "grouplist.h"
#include "kanjilist.h"
#include "kanji.h"
#include "module.h"
#include "popupdict.h"
#include "kanjiinfo.h"
#include "searchtree.h"
#include "zkformats.h"
#include "lineobj.h"
#include "mouseevent.h"
#include "kanjiobj.h"
#include "recognizer.h"
#include "docker.h"
#include "gengraph.h"
#include "newword.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(186);
#endif

TfDict *fDict;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*void __fastcall TfDict::WMSysCommand(TWMSysCommand &msg)
 {
 msg.Result = 0;

 if ((msg.CmdType & 0xFFF0) == SC_MINIMIZE || (msg.CmdType & 0xFFF0) == SC_ICON) {
 fMain->Minimize();
 return;
 }

 if ((msg.CmdType & 0xFFF0) == SC_MAXIMIZE || (msg.CmdType & 0xFFF0) == SC_ZOOM) {
 SizeInfo.maxed = true;
 miThisWindow->Visible = true;
 data->aTileKanji->Visible = false;
 data->aTileDict->Visible = false;
 fMain->Maximize();
 ShowWindow(Handle,SW_SHOWMAXIMIZED);
 return;
 } else if ((msg.CmdType & 0xFFF0) == SC_RESTORE) {
 miThisWindow->Visible = false;
 data->aTileKanji->Visible = true;
 data->aTileDict->Visible = true;
 SizeInfo.maxed = false;

 if (btnFilter->Down && filterchanged)
 btnFilter->Click();
 filterchanged = false;

 fMain->Restore();
 ShowWindow(Handle,SW_RESTORE);
 SetWindowPos(Handle,NULL,SizeInfo.left,SizeInfo.top,SizeInfo.width,SizeInfo.height, SWP_NOACTIVATE | SWP_NOZORDER);
 return;
 }

 msg.Result = DefWindowProc(Handle,msg.Msg,(*(TMessage*)&msg).WParam,(*(TMessage*)&msg).LParam);
 }*/
//---------------------------------------------------------------------------
int *irreg = NULL;
int irregcnt = 0;
__fastcall TfDict::TfDict(TComponent* Owner) : kanjichanged(false), kanjisavetime(0), romajichanged(false), romajisavetime(0), inherited(Owner)
{
    //RescaleStatusbar(sbInfo);

    if (settings.debug)
    {
        irreg = new int[5000];
        irregcnt = 0;
        miCheck->Visible = true;
    }

    //initial setting in case we medled with the form
    if (pKanji->Visible)
        btnKanji->Down = true;
    if (pRomaji->Visible)
        btnRomaji->Down = true;
    if (pJapanese->Visible)
        btnJapanese->Down = true;

    //Constrain(302*scaleres+(Width-ClientWidth),106*scaleres+(Height-ClientHeight));

    fnoupdate = 0;
    wobj = new TWordListObj(pbWords, rowsizes(settings.dictfontsize), wcallback, data->PopupWord);
    wobj->showgroup = true;
    edkanji = new TKanjiEdit(edKanji, true, true, kanjihistory, ecallback);
    edkanji->EnableRecognizer(sbRecognize);
    edjapanese = new TKanjiEdit(edJapanese, false, false, NULL, jcallback);
    lobj = new TLineObj(pbExamples, true, lcallback);

    numJump = new TNumEdit(edExJump, 1, 1, false);
    numJump->callback = &xcallback;
    numJump->exit_triggers = false;

    SettingsChangeEvent(this, settingsChanged, true);

    AddEvent(this, DoActivate, eltActivate);
    AddEvent(this, DoDeactivate, eltDeactivate);

    RestoreOldActive = true;
}
//---------------------------------------------------------------------------
__fastcall TfDict::~TfDict()
{
    RemoveEvents(this);

    SettingsChangeEvent(this, settingsChanged, false);

    if (settings.debug)
        delete[] irreg;

    delete numJump;
    delete lobj;
    delete wobj;
    delete edkanji;
    delete edjapanese;
}
//---------------------------------------------------------------------------
void TfDict::settingsChanged(void *caller, TSettingsProcState state)
{
    if (state != spsEnd)
        return;

    wobj->rowsize = rowsizes(settings.dictfontsize);
    wobj->headersize = rowsizes(settings.dictfontsize);
}
//---------------------------------------------------------------------------
void __fastcall TfDict::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caNone;
    fMain->Close();
}
//---------------------------------------------------------------------------
void __fastcall TfDict::edRomajiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    HWND h = GetCapture();
    if (h)
        return;

    if ((Key == VK_UP || Key == VK_DOWN) && !Shift.Contains(ssCtrl))
    {
        if (wobj->count)
        {
            pbWords->SetFocus();
            pbWords->OnKeyDown(Sender, Key, Shift);
        }
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDict::edRomajiKeyPress(TObject *Sender, wchar_t &Key)
{
    HWND h = GetCapture();
    if (h)
        return;

    if (Key == VK_RETURN && edRomaji->Text.Trim().Length() > 0)
    {
        if (wobj->count && romajihistory)
        {
            romajihistory->Add(edRomaji->Text.Trim(), NULL);
            romajihistory->FillComboBox(edRomaji);
        }
        edRomaji->Text = "";
        edRomajiChange(Sender);
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void TfDict::WordChanged(byte coll, TChangedWordList *list)
{
    if (coll != settings.dictionary_index)
        return;
    UpdateWords(list->Count != 1 || list->Items[0]->type != wctDelete);
    wcallback();
}
//---------------------------------------------------------------------------
void TfDict::UpdateWords(bool holdposition)
{
    //dictionary->filtered = btnFilter->Down;
    if (pJapanese->Visible)
    {
        if (btnFilter->Down)
        {
            if (!dictionary->Items[settings.dictionary_index]->abcfilter || !dictionaryfilters->filters_match(dictionary->Items[settings.dictionary_index]->setfilters))
            {
                wobj->set_to_continuous(dictionary->Items[settings.dictionary_index], edjapanese->Text.c_str(), false);
                dictionary->SetABCFilter(settings.dictionary_index);
            }
        }

        wobj->set_to_continuous(dictionary->Items[settings.dictionary_index], edjapanese->Text.c_str(), btnFilter->Down);
        update_info();
        return;
    }

    TWordIndexList *l = new TWordIndexList(dictionary->Items[settings.dictionary_index]);

    bool haskanji = false;

    if (pKanji->Visible)
    {
        int i;
        for (i = edkanji->Text.Length(); i > 0 && !haskanji; i--)
            haskanji = KANJI(edkanji->Text[i]) || VALIDCODE(edkanji->Text[i]);
        if (!haskanji)
            if (sbKanjiStart->Down && sbKanjiEnd->Down)
            {
                sbKanjiStart->Down = sbKanjiStart->Tag == 0;
                sbKanjiEnd->Down = sbKanjiEnd->Tag == 0;
            }
    }

    if (!haskanji)
    {
        if (pKanji->Visible)
            fMain->SearchJapan(l, sbKanjiStart->Down, sbKanjiEnd->Down, sbInfCon->Down, edkanji->Text, (btnFilter->Down ? dictionaryfilters : NULL));
        else
            fMain->SearchLatin(l, false, sbRomajiEnd->Down, sbInfCon->Down, edRomaji->Text, (btnFilter->Down ? dictionaryfilters : NULL));
    }
    else
        fMain->SearchK(l, sbKanjiStart->Down, sbKanjiEnd->Down, sbInfCon->Down, edkanji->Text, (btnFilter->Down ? dictionaryfilters : NULL));

    wobj->set_by_indexes(l, holdposition);
    delete l;
    update_info();
}
//---------------------------------------------------------------------------
void __fastcall TfDict::btnKanjiClick(TObject *Sender)
{
    noupdate = true;
    try
    {
        if (pKanji->Visible)
        {
            if (only_kana(edkanji->Text.c_str()))
                edjapanese->Text = edkanji->Text;
            else if (wobj->pos >= 0)
                edjapanese->Text = wobj->currentword->kana;
        }
        else if (pJapanese->Visible)
            edkanji->Text = edjapanese->Text;

        ((TSpeedButton*)Sender)->Down = true;
        if (Sender == btnRomaji)
        {
            pRomaji->Visible = true;
            pKanji->Visible = false;
            pJapanese->Visible = false;
            data->aWordStart->Enabled = false;
            data->aWordEnd->Enabled = true;
            if (Visible)
                edRomaji->SetFocus();
            edRomaji->SelectAll();
        }
        else if (Sender == btnKanji)
        {
            pKanji->Visible = true;
            pRomaji->Visible = false;
            pJapanese->Visible = false;
            data->aWordStart->Enabled = true;
            data->aWordEnd->Enabled = true;
            if (Visible)
                edKanji->SetFocus();
            edkanji->SelectAll();
        }
        else if (Sender == btnJapanese)
        {
            pJapanese->Visible = true;
            pKanji->Visible = false;
            pRomaji->Visible = false;
            data->aWordStart->Enabled = false;
            data->aWordEnd->Enabled = false;
            if (Visible)
                edJapanese->SetFocus();
            edjapanese->SelectAll();
        }

    }
    catch (...)
    {
        ;
    }
    noupdate = false;

    UpdateWords();
}
//---------------------------------------------------------------------------
void TfDict::update_info()
{
    sbInfo->Panels->Items[0]->Text = IntToStr(wobj->count) + " :";
    sbInfo->Panels->Items[1]->Text = IntToStr(wobj->pos + 1);
    sbInfo->Panels->Items[2]->Text = (wobj->pos >= 0 ? wobj->wordnotes : (UnicodeString)" ");
}
//---------------------------------------------------------------------------
void TfDict::wcallback()
{
    update_info();

    if (fDock)
    {
        fDock->miWRevertWord->Enabled = wobj->pos >= 0 && settings.dictionary_index == 0 && dictionary->Originals->IsModified(wobj->currentword);
        fDock->miWDeleteWord->Enabled = !fDock->miWRevertWord->Enabled && wobj->pos >= 0 && (settings.dictionary_index > 0 || dictionary->Originals->IsAdded(wobj->currentword));

        fDock->miWEditWord->Enabled = /*settings.dictionary_index > 0 &&*/wobj->pos >= 0;
        fDock->miWToVocab->Enabled = wobj->pos >= 0;
        fDock->miWAppendKanji->Enabled = wobj->pos >= 0;
        fDock->miWAppendKana->Enabled = wobj->pos >= 0;
        fDock->miWCopyKanji->Enabled = wobj->pos >= 0;
        fDock->miWCopyKana->Enabled = wobj->pos >= 0;
    }

    bool setex = false;
    if (wobj->count && wobj->pos >= 0 && Examples)
    {
        TStoredWord *ex = wobj->pos >= 0 ? store->FindWord(wobj->words[wobj->pos]) : NULL;
        if (ex && ex->examplecnt)
        {
            lobj->set_data(wobj->collection, ex);
            setex = true;
        }
    }
    if (!setex)
        lobj->set_data(wobj->collection, NULL);

    if (pJapanese->Visible && !edJapanese->Focused() && wobj->pos >= 0)
    {
        noupdate = true;
        try
        {
            edjapanese->Text = wobj->words[wobj->pos]->kana;
        }
        catch (...)
        {
            ;
        }
        noupdate = false;
    }
}
//---------------------------------------------------------------------------
void TfDict::lcallback()
{
    static bool insidethis = false;
    if (insidethis)
        return;

    if (lobj->WordIndex >= 0)
    {
        lobj->Lock(true);
        if (btnFilter->Down)
            btnFilter->Down = false;

        insidethis = true;

        TWord *w = lobj->Collection->words->Items[lobj->WordIndex];
        TStoredWord *exw = store->FindWord(lobj->Collection->words->Items[lobj->WordIndex]);
        TStoredWord *ex = wobj->pos >= 0 ? store->FindWord(wobj->words[wobj->pos]) : NULL;
        if (!ex || !ex->examplecnt || !exw || !exw->examplecnt)
            THROW(L"fix");

        int oblock = ex->examples[lobj->Pos].block;
        int onum = ex->examples[lobj->Pos].num;
        int ohl = lobj->DrawObj->Highlighted;
        int opos = lobj->DrawObj->Positions[ohl];

        if (btnRomaji->Down)
            btnKanji->Click();
        else if (btnKanji->Down)
            edkanji->ToHistory();

        if (btnJapanese->Down)
            edjapanese->Text = w->kana;
        else
            edkanji->Text = w->kana;

        int bix = dictionary->ConvertIndex(lobj->Collection, lobj->WordIndex, wobj->collection);
        if (bix >= 0)
        {
            w = wobj->collection->words->Items[bix];
            for (int i = wobj->pos; i < wobj->count; i++)
                if (wobj->words[i] == w)
                {
                    wobj->pos = i;
                    break;
                }
        }

        insidethis = false;
        lobj->Lock(false);

        for (int i = 0; i < exw->examplecnt; i++)
            if (exw->examples[i].block == oblock && exw->examples[i].num == onum && Examples->sentences[oblock][onum]->words[exw->examples[i].wordnum].position == opos)
            {
                lobj->set_all(wobj->collection, exw, i);
                break;
            }
        return;
    }

    sbExLeft->Enabled = lobj->Pos > 0;
    sbExRight->Enabled = lobj->Pos < lobj->Count - 1;

    lbExMax->Caption = lobj->Count;
    lbExNum->Caption = (UnicodeString)": " + IntToStr(MIN(lobj->Count, lobj->Pos + 1));
    edExJump->Text = lobj->Pos + 1;
    btnExJump->Enabled = lobj->Count > 1;
    numJump->max = lobj->Count;
}
//---------------------------------------------------------------------------
void TfDict::ecallback()
{
    kanjichanged = !edkanji->Text.Trim().IsEmpty();
    kanjisavetime = lasttime;
    UpdateWords(false);
}
//---------------------------------------------------------------------------
void TfDict::jcallback()
{
    wobj->find_in_continuous(edjapanese->Text.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TfDict::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (!Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt) && !Shift.Contains(ssShift))
    {
        switch (Key)
        {
        case VK_F2:
            btnKanji->Click();
            Key = 0;
            break;
        case VK_F3:
            btnRomaji->Click();
            Key = 0;
            break;
        case VK_F4:
            btnJapanese->Click();
            Key = 0;
            break;
        case VK_ESCAPE:
            if (InfoWindowVisible() && !edKanji->Focused() && !edJapanese->Focused())
            {
                HideInfoWindows(true);
                Key = 0;
            }
            break;
        case VK_ADD:
            if (pKanji->Visible)
            {
                sbAddKanji->Click();
                Key = 0;
            }
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDict::sbAddKanjiClick(TObject *Sender)
{
    edkanji->Insert(fKanji->SelString());
}
//---------------------------------------------------------------------------
int TfDict::get_current()
{
    return wobj->current;
}
//---------------------------------------------------------------------------
void __fastcall TfDict::pbWordsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button == mbLeft && Shift.Contains(ssDouble))
    {
        wchar_t c = wobj->KanjiAtPos(x, y);
        if (!c || !KANJI(c))
        {
            if (wobj->IndexAtPos(y) >= 0 && fDock)
                miToVocabClick(fDock->miWToVocab);
        }
        else
            ShowKanjiInfo(settings.dictionary_index, kanjis->Kanjis[kanjis->IndexOf(c)]);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDict::edRomajiChange(TObject *Sender)
{
    romajichanged = !edRomaji->Text.Trim().IsEmpty();
    romajisavetime = lasttime;
    UpdateWords(false);
}
//---------------------------------------------------------------------------
void __fastcall TfDict::pbWordsKeyPress(TObject *Sender, wchar_t &Key)
{
    if (pRomaji->Visible)
    {
        if (Key == VK_BACK)
        {
            edRomaji->SetFocus();
            edRomaji->SelLength = 0;
            edRomaji->Text = edRomaji->Text.SubString(1, edRomaji->Text.Length() - 1);
            edRomaji->SelStart = edRomaji->Text.Length();
            edRomajiChange(edRomaji);
        }
        else if (Key == VK_RETURN)
            edRomaji->SetFocus();
        else if (!wcschr(cdelim, Key) && Key != VK_ESCAPE)
        {
            edRomaji->SetFocus();
            edRomaji->SelLength = 0;
            edRomaji->Text = edRomaji->Text + Key;
            edRomaji->SelStart = edRomaji->Text.Length();
            edRomajiChange(edRomaji);
        }
    }
    else if (pKanji->Visible || pJapanese->Visible)
    {
        TZPaintBox *p = (pKanji->Visible ? edKanji : edJapanese);
        if ((Key >= L'A' && Key <= L'Z') || (Key >= L'a' && Key <= L'z') || Key == VK_BACK || Key == L' ')
        {
            p->SetFocus();
            p->OnKeyPress(Sender, Key);
        }
        else if (Key == VK_RETURN)
            p->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDict::pbWordsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
/*
    if (pRomaji->Visible)
    {
        if (Key == VK_BACK)
        {
            edRomaji->SetFocus();
            edRomaji->SelLength = 0;
            edRomaji->Text = edRomaji->Text.SubString(1, edRomaji->Text.Length() - 1);
            edRomaji->SelStart = edRomaji->Text.Length();
            UpdateWords();
        }
        if (Key == VK_RETURN)
            edRomaji->SetFocus();
    }
    else if (pKanji->Visible || pJapanese->Visible)
    {
        TZPaintBox *p = (pKanji->Visible ? edKanji : edJapanese);
        if (Key >= 'A' && Key <= 'Z')
        {
            p->SetFocus();
            p->OnKeyDown(Sender, Key, Shift);
        }
        if (Key == VK_BACK)
        {
            p->SetFocus();
            p->OnKeyDown(Sender, Key, Shift);
        }
        if (Key == VK_RETURN)
            p->SetFocus();
    }
    */
    if (Key == VK_ESCAPE)
    {
        if (pRomaji->Visible)
            edRomaji->SetFocus();
        else if (pKanji->Visible)
            edKanji->SetFocus();
        else if (pJapanese->Visible)
            edJapanese->SetFocus();
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDict::edRomajiExit(TObject *Sender)
{
    if (pRomaji->Visible && romajihistory)
    {
        romajihistory->Add(edRomaji->Text.Trim(), NULL);
        romajihistory->FillComboBox(edRomaji);
    }
}
//---------------------------------------------------------------------------
void TfDict::DoDeactivate(void *caller)
{
    edRomajiExit(edRomaji);
    pbWords->Refresh();
}
//---------------------------------------------------------------------------
void TfDict::DoActivate(void *caller)
{
    edRomajiExit(edRomaji);
}
//---------------------------------------------------------------------------
void TfDict::SaveSettings()
{
    settings.colkanji = wobj->colsize[1];
    settings.colkana = wobj->colsize[2];
}
//---------------------------------------------------------------------------
void __fastcall TfDict::FormShow(TObject *Sender)
{
    static bool firsttime = true;
    if (firsttime)
    {
        firsttime = false;
        if (!settings.columnreset)
        {
            wobj->colsize[1] = settings.colkanji;
            wobj->colsize[2] = settings.colkana;
        }
        fDock->miDictSelect->Items[settings.dictionary_index]->Click();
    }

    if (noupdate)
    {
        noupdate = false;
        UpdateWords();
    }

    if (btnFilter->Down && filterchanged)
        btnFilter->Click();
    filterchanged = false;
}
//---------------------------------------------------------------------------
void __fastcall TfDict::sbKanjiStartClick(TObject *Sender)
{
    int i;
    UnicodeString z = edkanji->Text.Trim();
    for (i = 1; i <= z.Length(); i++)
        if (KANJI(z[i]) || VALIDCODE(z[i]))
            break;
    if (i > z.Length())//no kanji
                sbKanjiEnd->Down = false;
    sbKanjiStart->Tag = sbKanjiStart->Down;
    sbKanjiEnd->Tag = 0;
    UpdateWords();
}
//---------------------------------------------------------------------------
void __fastcall TfDict::sbKanjiEndClick(TObject *Sender)
{
    int i;
    UnicodeString z = edkanji->Text.Trim();
    for (i = 1; i <= z.Length(); i++)
        if (KANJI(z[i]) || VALIDCODE(z[i]))
            break;
    if (i > z.Length() && sbKanjiEnd->Down)//no kanji
                sbKanjiStart->Down = false;
    sbKanjiStart->Tag = 0;
    sbKanjiEnd->Tag = sbKanjiEnd->Down;
    UpdateWords();
}
//---------------------------------------------------------------------------
void __fastcall TfDict::miClick(TObject *Sender)
{
    SelectDictionary(((TMenuItem*)Sender)->Tag);
}
//---------------------------------------------------------------------------
void TfDict::DeselectDictionary()
{
    settings.dictionary_index = -1;
    lobj->set_data(NULL, NULL);
    wobj->set_by_indexes(NULL, false);
}
//---------------------------------------------------------------------------
void TfDict::SelectDictionary(byte coll)
{
    if (coll >= dictionary->Count || settings.dictionary_index == coll)
        return;
    settings.dictionary_index = coll;

    fMain->SetTaskbarTasks();

//    if (fDock)
//        fDock->miWNewWord->Enabled = settings.dictionary_index > 0;

    fDock->miDictSelect->Items[coll]->Checked = true;

    if (settings.intray)
        noupdate = true;
    else
        UpdateWords();

    SetCaptions(dictionary->Strings[coll]);

    TWordCollection *dict = dictionary->Items[coll];
    int imgindex = dict->imgindex;
    if (imgindex < 0)
    {
        imgindex = coll == 0 ? 13 : 11;
    }

    data->aJtoE->ImageIndex = imgindex;
    data->aEtoJ->ImageIndex = imgindex + 1;

    //empty 11, 12 - english 13, 14
    btnKanji->Glyph = NULL;
    btnRomaji->Glyph = NULL;
    data->imgMenu->BkColor = clLime;
    data->imgMenu->GetBitmap(imgindex, btnKanji->Glyph);
    data->imgMenu->GetBitmap(imgindex + 1, btnRomaji->Glyph);
    data->imgMenu->BkColor = Graphics::clNone;

    fPopupDict->SelectDictionary(coll);

    fKanji->collection = dictionary->Items[coll];

    if (fKanji->edMeaning->Text.Trim().Length())
        fKanji->FilterKanji();

    if (fInfo && (Screen->ActiveForm == this || fDock && Screen->ActiveForm == (TForm*)fDock || Screen->ActiveForm == fPopupDict))
        fInfo->collection = settings.dictionary_index;
    edkanji->Collection = coll;
}
//---------------------------------------------------------------------------
void TfDict::SearchWord(byte coll, TWord *w)
{
    if (!btnKanji->Down)
        btnKanji->Down = true;
    btnKanji->Click();
    if (settings.dictionary_index != coll)
        SelectDictionary(coll);
    edkanji->Text = w->kana;

    for (int ix = 0; ix < wobj->count; ++ix)
    {
        if (wobj->words[ix] == w)
        {
            wobj->pos = ix;
            break;
        }
    }
}
//---------------------------------------------------------------------------
void TfDict::Search(byte coll, UnicodeString what)
{
    if (has_jpchar(what.c_str()))
    {
        if (!btnKanji->Down)
        {
            btnKanji->Down = true;
            btnKanji->Click();
        }
    }
    else if (!btnRomaji->Down)
    {
        btnRomaji->Down = true;
        btnRomaji->Click();
    }

    if (settings.dictionary_index != coll)
        SelectDictionary(coll);
    if (btnKanji->Down)
        edkanji->Text = what;
    else
        edRomaji->Text = what;
}
//---------------------------------------------------------------------------
void TfDict::SetCaptions(UnicodeString s)
{
    data->aJtoE->Caption = L"Japanese to " + s;
    data->aEtoJ->Caption = s + L" to Japanese";
    btnKanji->Hint = data->aJtoE->Caption + L" [F2]";
    btnRomaji->Hint = data->aEtoJ->Caption + L" [F3]";
    fPopupDict->btnKanji->Hint = btnKanji->Hint;
    fPopupDict->btnRomaji->Hint = btnRomaji->Hint;
    data->miFromJap->Caption = L"Japanese to " + s;
    data->miToJap->Caption = s + L" to Japanese";
    Caption = L"Dictionary - " + s;
    if (fDock)
        fDock->SetCaption();
}
//---------------------------------------------------------------------------
void TfDict::DictionaryRenamed(byte coll, UnicodeString s)
{
    fDock->miDictSelect->Items[coll]->Caption = s;

    if (coll == settings.dictionary_index)
        SetCaptions(s);
}
//---------------------------------------------------------------------------
void __fastcall TfDict::miEditWordClick(TObject *Sender)
{
    EditWordDef(dictionary->Items[settings.dictionary_index], wobj->current, wobj->currentmeaning);
}
//---------------------------------------------------------------------------
void __fastcall TfDict::miDeleteWordClick(TObject *Sender)
{
    DeleteWordEntry(settings.dictionary_index, wobj->current, wobj->currentmeaning);
}
//---------------------------------------------------------------------------
void __fastcall TfDict::miRevertWordClick(TObject *Sender)
{
    RevertWordEntry(wobj->current);
}
//---------------------------------------------------------------------------
void __fastcall TfDict::miToVocabClick(TObject *Sender)
{
    fGroup->AddVocabulary(dictionary->Items[settings.dictionary_index], wobj->current);
}
//---------------------------------------------------------------------------
void __fastcall TfDict::miAppendKanjiClick(TObject *Sender)
{
    ClipAppend(wobj->currentword->kanji);
}
//---------------------------------------------------------------------------
void __fastcall TfDict::miAppendKanaClick(TObject *Sender)
{
    ClipAppend(wobj->currentword->kana);
}
//---------------------------------------------------------------------------
void __fastcall TfDict::miCopyKanjiClick(TObject *Sender)
{
    ClipReplace(wobj->currentword->kanji);
}
//---------------------------------------------------------------------------
void __fastcall TfDict::miCopyKanaClick(TObject *Sender)
{
    ClipReplace(wobj->currentword->kana);
}
//---------------------------------------------------------------------------
void TfDict::set_autosize(bool val)
{
    wobj->autoresize = val;
}
//---------------------------------------------------------------------------
bool TfDict::get_autosize()
{
    return wobj->autoresize;
}
//---------------------------------------------------------------------------
void TfDict::DictionaryDeleted(byte coll)
{
    fPopupDict->DictionaryDeleted(coll);
    if (coll == settings.dictionary_index)
    {
        lobj->set_data(dictionary->Items[0], NULL);
        SelectDictionary(0);
    }
    else if (coll < settings.dictionary_index)
        settings.dictionary_index--;
}
//---------------------------------------------------------------------------
void __fastcall TfDict::tempsave1Click(TObject *Sender)
{
#ifdef _DEBUG
    if (!pKanji->Visible)
        return;

    TWordIndexList *l = new TWordIndexList(dictionary->Items[settings.dictionary_index]/*,false*/);

    int i;
    bool haskanji = false;

    for (i = edkanji->Text.Length(); i > 0 && !haskanji; i--)
        haskanji = KANJI(edkanji->Text[i]);
    if (!haskanji)
        if (sbKanjiStart->Down && sbKanjiEnd->Down)
        {
            sbKanjiStart->Down = sbKanjiStart->Tag == 0;
            sbKanjiEnd->Down = sbKanjiEnd->Tag == 0;
        }

    if (!haskanji)
    {
        if (pKanji->Visible)
            fMain->SearchJapan(l, sbKanjiStart->Down, sbKanjiEnd->Down, sbInfCon->Down, edkanji->Text);
        else
            fMain->SearchLatin(l, false, sbRomajiEnd->Down, sbInfCon->Down, edRomaji->Text);
    }
    else
        fMain->SearchK(l, sbKanjiStart->Down, sbKanjiEnd->Down, sbInfCon->Down, edkanji->Text);

    FILE *f;
    wchar_t cc[2];
    f = _wfopen(L"c:\\results.txt", L"wb");
    try
    {
        for (i = 0; i < l->Count; i++)
        {
            TWord *w = l->Words[i];
            fwrite(w->kanji, sizeof(wchar_t), wcslen(w->kanji), f);
            cc[0] = 9;
            fwrite(cc, sizeof(wchar_t), 1, f);
            fwrite(w->kana, sizeof(wchar_t), wcslen(w->kana), f);
            cc[0] = 0xd;
            cc[1] = 0xa;
            fwrite(cc, sizeof(wchar_t), 2, f);
        }
    }
    catch (...)
    {
    }
    fclose(f);
    delete l;
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfDict::btnFilterClick(TObject *Sender)
{
    data->aFilterWords->Checked = btnFilter->Down;
    UpdateWords();
}
//---------------------------------------------------------------------------
void __fastcall TfDict::btnFilterMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button != mbRight || x < 0 || y < 0 || x >= btnFilter->Width || y >= btnFilter->Height)
        return;

    TPoint p = btnFilter->ClientToScreen(Point(0, btnFilter->Height + 2));
    data->PopupFilters(mkDictionary, p.x, p.y);
    /*data->pmFilters->Tag = 1;
     data->pmFilters->Popup(p.x,p.y);
     */
}
//---------------------------------------------------------------------------
void __fastcall TfDict::miCheckClick(TObject *Sender)
{
    /*UnicodeString w;
     bool match;

     for (int i = 0; i < kanjis->Count; i++) {
     bool kj = false;
     if (kanjis->Kanjis[i]->irregcnt) {
     TKanji *k = kanjis->Kanjis[i];
     for (int j = 0; j < k->irregcnt; j++) {
     for (int l = 0; l < k->oncnt; l++) {
     UnicodeString on = k->on[l] + (k->on[l][0] == L'-' ? 1 : 0);
     if (on.Length() != (int)wcslen(k->irreg[j]))
     continue;
     match = true;
     for (int m = 0; m < on.Length() && match; m++) {
     if (HiraganaCh(on.c_str(),m) != HiraganaCh(k->irreg[j],m) )
     match = false;
     }
     if (!kj && match) {
     w = w+",  "+k->ch+"  ";
     kj = true;
     }
     if (match) w = w+","+on;
     }
     for (int l = 0; l < k->kuncnt; l++) {
     UnicodeString kun = k->kun[l];
     if (kun.Length() != (int)wcslen(k->irreg[j]))
     continue;
     match = true;
     for (int m = 0; m < kun.Length() && match; m++) {
     if (HiraganaCh(kun.c_str(),m) != HiraganaCh(k->irreg[j],m) )
     match = false;
     }
     if (!kj && match) {
     w = w+",  "+k->ch+"  ";
     kj = true;
     }
     if (match) w = w+","+kun;
     }
     }
     }
     }*/

    FILE *f = _wfopen(L"d:\\linetests.txt", L"wb");
    bool good;
    UnicodeString z;
    sbInfo->Panels->Items[0]->Text = IntToStr(dictionary->Items[0]->words->Count);
    sbInfo->Panels->Items[1]->Text = "0";

    //bool useirreg = irregcnt;

    //if (useirreg)
    reimportirreg1Click(NULL);

    //to trick this bastard. delete
    bool useirreg = false;
    int irregcnt = 0;
    int irreg[1];
    //delete end;

    for (int i = 0; i < (useirreg ? irregcnt : dictionary->Items[0]->words->Count); i++)
    {
        int ix = useirreg ? irreg[i] : i;
        TFuriganaData *dat;
        int datcnt;
        try
        {
            datcnt = FindFurigana(dictionary->Items[0]->words->Items[ix], dat);
            if (datcnt < 0)
                good = false;
        }
        catch (...)
        {
            good = false;
            datcnt = 0;
        }
        if (datcnt > 0)
            delete[] dat;
        if (!good)
        {
            z = (UnicodeString)IntToStr(ix) + L"\r\n" + dictionary->Items[0]->words->Items[ix]->kanji + L" ||| " + dictionary->Items[0]->words->Items[ix]->kana + L"\r\n";
            fwrite(z.c_str(), sizeof(wchar_t), z.Length(), f);
            sbInfo->Panels->Items[1]->Text = IntToStr(StrToInt(sbInfo->Panels->Items[1]->Text) + 1);
            Application->ProcessMessages();
        }
        sbInfo->Panels->Items[0]->Text = IntToStr((useirreg ? irregcnt : dictionary->Items[0]->words->Count)) - i;
        if (i % 500)
            Application->ProcessMessages();
        if (useirreg && good)
        {
            memmove(irreg + i, irreg + i + 1, (irregcnt - i - 1)*sizeof(int));
            irregcnt--;
            i--;
        } /*else if (!useirreg && !good) {
         irreg[irregcnt] = ix;
         irregcnt++;
         }*/
    }
    fclose(f);
}
//---------------------------------------------------------------------------
void __fastcall TfDict::reimportirreg1Click(TObject *Sender)
{
#ifdef _DEBUG
    for (int i = 0; i < kanjis->Count; i++)
        if (kanjis->Kanjis[i]->irregcnt)
        {
            delete[] kanjis->Kanjis[i]->irreg;
            kanjis->Kanjis[i]->irregcnt = 0;
        }
    if (FileExists(paths.installdatapath + L"irregulars.txt"))
        import_irregulars((paths.installdatapath + L"irregulars.txt").c_str());
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfDict::sbExamplesClick(TObject *Sender)
{
    pExamples->Visible = sbExamples->Down;
}
//---------------------------------------------------------------------------
void __fastcall TfDict::sbExLeftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    lobj->Pos--;
    MouseRepeat(sbExLeftMouseDown, Sender, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TfDict::sbExRightMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    lobj->Pos++;
    MouseRepeat(sbExRightMouseDown, Sender, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TfDict::sbEx2Click(TObject *Sender)
{
    lobj->Mode = lmJapanese;
}
//---------------------------------------------------------------------------
void __fastcall TfDict::sbEx3Click(TObject *Sender)
{
    lobj->Mode = lmTranslated;
}
//---------------------------------------------------------------------------
void __fastcall TfDict::sbEx1Click(TObject *Sender)
{
    lobj->Mode = lmBoth;
}
//---------------------------------------------------------------------------
void __fastcall TfDict::btnExJumpClick(TObject *Sender)
{
    lobj->Pos = numJump->value - 1;
}
//---------------------------------------------------------------------------
void TfDict::set_noupdate(bool val)
{
    if (val)
        fnoupdate++;
    else
        fnoupdate--;
    if (fnoupdate < 0)
        THROW(L"Error! Mixed updates!");
}
//---------------------------------------------------------------------------
bool TfDict::get_noupdate()
{
    return fnoupdate;
}
//---------------------------------------------------------------------------
void __fastcall TfDict::pExPageBtnClick(TObject *Sender)
{
    TPanel *pExBtn = (TPanel*)Sender;
    TPanel *pEx = ((TPanel*)pExBtn->Parent);
    if (pEx->Width > 7)
    {
        pEx->Tag = pEx->Width;
        pEx->Width = 7;
        if (Visible)
        {
            TRect r = Rect(pExamples->Left, pExamples->Top, pExArrows->Left + pExArrows->Width, pExamples->Top + pExamples->Height);
            RedrawWindow(Handle, &r, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_NOFRAME | RDW_ALLCHILDREN);
        }
    }
    else
    {
        pEx->Width = pEx->Tag;
        pEx->Tag = 0;
        if (Visible)
        {
            TRect r = Rect(pExamples->Left, pExamples->Top, pExArrows->Left + pExArrows->Width, pExamples->Top + pExamples->Height);
            RedrawWindow(Handle, &r, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_NOFRAME | RDW_ALLCHILDREN);
        }
    }
    (pEx == pExType ? bvExType : bvExPage)->Visible = pEx->Width > 7;
}
//---------------------------------------------------------------------------
void __fastcall TfDict::pExPageBtnMouseEnter(TObject *Sender)
{
    ((TPanel*)Sender)->Color = clInfoBk;
}
//---------------------------------------------------------------------------
void __fastcall TfDict::pExPageBtnMouseLeave(TObject *Sender)
{
    ((TPanel*)Sender)->Color = clBtnFace;
}
//---------------------------------------------------------------------------
void __fastcall TfDict::FormPaint(TObject *Sender)
{
    DrawGrad(Canvas, COLORINC(clBtnFace, 0x181818), COLORDEC(clBtnFace, 0x181818), Rect(0, 0, ClientWidth, pToolbar->Height + pToolbar->Top - 1));
    if (sbExamples->Down)
    {
        TRect r = pExArrows->ClientRect;
        TPoint p = Point(0, 0);
        p = ScreenToClient(pExArrows->ClientToScreen(p));
        OffsetRect(r, p.x, p.y);
        DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), COLORDEC(clBtnFace, 0x101010), r, false);

        r = pExPage->ClientRect;
        p = Point(-1, 0);
        p = ScreenToClient(pExPage->ClientToScreen(p));
        OffsetRect(r, p.x, p.y);
        DrawGrad(Canvas, COLORINC(clBtnFace, 0x0a0a0a), COLORDEC(clBtnFace, 0x0a0a0a), r);

        r = pExType->ClientRect;
        p = Point(0, 0);
        p = ScreenToClient(pExType->ClientToScreen(p));
        OffsetRect(r, p.x, p.y);
        DrawGrad(Canvas, COLORINC(clBtnFace, 0x0a0a0a), COLORDEC(clBtnFace, 0x0a0a0a), r);
    }
}
//---------------------------------------------------------------------------
void TfDict::disable_examples()
{
    if (sbExamples->Down)
    {
        sbExamples->Down = false;
        sbExamplesClick(sbExamples);
    }
    sbExamples->Enabled = false;
    data->aShowHideEx->Enabled = false;
    sbExamples->Hint = "Examples data file not found or not a recognized format.";
}
//---------------------------------------------------------------------------
void TfDict::enable_examples()
{
    sbExamples->Enabled = true;
    data->aShowHideEx->Enabled = true;
    sbExamples->Hint = "Show / hide word examples [F5]";
}
//---------------------------------------------------------------------------
void __fastcall TfDict::miNewWordClick(TObject *Sender)
{
    AddNewWord(settings.dictionary_index);
}
//---------------------------------------------------------------------------
void TfDict::xcallback()
{
    if (numJump->value_valid && btnExJump->Enabled)
        btnExJump->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfDict::btnKanjiMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button != mbRight || fDock->miDictSelect->Count <= 1)
        return;
    TSpeedButton *btn = (TSpeedButton*)Sender;
    TRect r = btn->ClientRect;
    if (!PtInRect(r, Point(x, y)))
        return;

    TPoint p = btn->ClientToScreen(Point(x, y));
    while (fDock->miDictSelect->Count < pmDict->Items->Count)
    {
        TMenuItem *mi = pmDict->Items->Items[pmDict->Items->Count];
        pmDict->Items->Remove(mi);
        delete mi;
    }
    while (fDock->miDictSelect->Count > pmDict->Items->Count)
    {
        TMenuItem *mi = new TMenuItem(pmDict);
        pmDict->Items->Add(mi);
    }
    for (int ix = 0; ix < pmDict->Items->Count; ++ix)
    {
        pmDict->Items->Items[ix]->Caption = fDock->miDictSelect->Items[ix]->Caption;
        pmDict->Items->Items[ix]->GroupIndex = fDock->miDictSelect->Items[ix]->GroupIndex;
        pmDict->Items->Items[ix]->RadioItem = fDock->miDictSelect->Items[ix]->RadioItem;
        pmDict->Items->Items[ix]->Checked = fDock->miDictSelect->Items[ix]->Checked;
        pmDict->Items->Items[ix]->OnClick = fDock->miDictSelect->Items[ix]->OnClick;
        pmDict->Items->Items[ix]->Tag = fDock->miDictSelect->Items[ix]->Tag;
    }
    pmDict->Popup(p.x, p.y);
}
//---------------------------------------------------------------------------
void TfDict::UpdateKButtons()
{
    sbAddKanji->Enabled = fKanji && fKanji->Count;
}
//---------------------------------------------------------------------------
void TfDict::Time(__int64 sec)
{
    lasttime = sec;
    if ((!kanjichanged && !romajichanged) || !settings.timedhistsave)
        return;
    if (kanjichanged && sec - kanjisavetime >= settings.histdelay)
    {
        kanjichanged = false;
        kanjisavetime = lasttime;
        kanjihistory->Add(edkanji->Text, NULL);
    }
    if (romajichanged && sec - romajisavetime >= settings.histdelay)
    {
        romajichanged = false;
        romajisavetime = lasttime;
        romajihistory->Add(edRomaji->Text.Trim(), NULL);
        romajihistory->FillComboBox(edRomaji);
    }
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(187);
#endif


