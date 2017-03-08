//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <IniFiles.hpp>
#include <Registry.hpp>
#include <Themes.hpp>

#include "settingswindow.h"
#include "collection.h"
#include "myfonts.h"
#include "numedit.h"
#include "wordlist.h"
#include "popupdict.h"
#include "main.h"
#include "grouplist.h"
#include "kanjilist.h"
#include "docker.h"
#include "kanji.h"
#include "romajizer.h"
#include "kanjiinfo.h"
#include "clipview.h"
#include "repetition.h"
#include "kanapractice.h"
#include "module.h"
#include "elementsettings.h"
#include "recognizer.h"
#include "localeinfo.h"
#include "mouseevent.h"
#include "sitemanager.h"
#include "studylist.h"
#include "gengraph.h"
#include "wordfilter.h"
#include "dropdown.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(144);
#endif

TfSettings *fSettings;
//---------------------------------------------------------------------------

extern TSiteList *tmpsitelist;

extern bool ThemesAreTurnedOn;


//---------------------------------------------------------------------------
__fastcall TfSettings::TfSettings(TComponent* Owner) : TBaseForm(Owner)
{
    olddecor = settings.windowdecoration;

    int pageix = -1;

    if (!settings.settingspage.IsEmpty())
    {
        UnicodeString page = settings.settingspage.LowerCase();
        for (int ix = 0; ix < pages->PageCount && pageix == -1; ++ix)
            if (pages->Pages[ix]->Caption.LowerCase() == page)
                pageix = ix;
    }

    if (pageix != -1)
        pages->ActivePage = pages->Pages[pageix];
    else
        pages->ActivePage = tabGlobal;

    numAutoSave = new TNumEdit(edAutoSave, 1, 60);
    numKanjiHintTime = new TNumEdit(edKanjiHintTime, 1, 10);
    numKWarnThres = new TNumEdit(edKWarnThres, 5, 999);
    numDayStart = new TNumEdit(edDayStart, 1, 12);
    numKWarnItems = new TNumEdit(edKWarnItems, 1, 999);
    numKWarnAfter = new TNumEdit(edKWarnAfter, 1, 180);
    numKItemInclude = new TNumEdit(edKItemInclude, 0);
    numKRetryLimit = new TNumEdit(edKRetryLimit, 1, 10);
    numKHardLimit = new TNumEdit(edKHardLimit, 1, 10);
    numBckCount = new TNumEdit(edBckCount, 1, 999);
    numBckSpacing = new TNumEdit(edBckSpacing, 1, 99);
    numHistCnt = new TNumEdit(edHistCnt, 10, 1000);
    numHistDelay = new TNumEdit(edHistDelay, 1, 99);

    cbFreq->ItemHeight *= scaleres;

    Kfont = 0;

    cbKanji->Items->Assign(jfonts);
    cbKanji->ItemIndex = cbKanji->Items->IndexOf(settings.kanjifont);
    if (cbKanji->ItemIndex < 0 && cbKanji->Items->Count)
        cbKanji->ItemIndex = 0;
    cbKanjiAntialias->Checked = settings.kanjiantialias;

    cbKana->Items->Assign(jfonts);
    cbKana->ItemIndex = cbKana->Items->IndexOf(settings.kanafont);
    if (cbKana->ItemIndex < 0 && cbKana->Items->Count)
        cbKana->ItemIndex = 0;
    cbRomaji->Items->Assign(rfonts);
    cbRomaji->ItemIndex = cbRomaji->Items->IndexOf(settings.romajifont);
    if (cbRomaji->ItemIndex < 0 && cbRomaji->Items->Count)
        cbRomaji->ItemIndex = 0;
    cbSmallRomaji->Items->Assign(rfonts);
    cbSmallRomaji->ItemIndex = cbSmallRomaji->Items->IndexOf(settings.smallromajifont);
    if (cbSmallRomaji->ItemIndex < 0 && cbSmallRomaji->Items->Count)
        cbSmallRomaji->ItemIndex = 0;

    cbSearchSave->Checked = settings.searchsave;
    cbPositionRestore->Checked = settings.positionrestore;
    cbKanjiRestore->Checked = settings.kanjirestore;

    cbAutoSave->Checked = settings.autosave;
    edAutoSave->Text = settings.autosavedelay;
    edAutoSave->Enabled = cbAutoSave->Checked;

    cbWinDecor->Checked = settings.windowdecoration;
    cbOldMenu->Checked = settings.oldstylepopup;
    cbShadow->Checked = settings.toolshadow;

    cbMinTray->Checked = settings.minimizetotray;

    cbShowClipboardCopy->Checked = settings.showclipcopy;
    cbShowClipboardAppend->Checked = settings.showclipappend;

    cbShowNonKanji->Checked = settings.shownonkanjielements;
    cbShowNonKanji->Enabled = hasstrokes;

    cbColumnRestore->Checked = settings.columnreset;
    cbPopupColumnRestore->Checked = settings.popupcolumnreset;
    cbPopupPositionRestore->Checked = settings.popuppositionrestore;
    cbPopupDisappear->Checked = settings.popupdisappear;
    cbPopupUseClipboard->Checked = settings.popupuseclipboard;
    cbKanjiReset->Checked = settings.kanjireset;

    tbTransparency->Position = settings.popuptransparency;

    cbHWRememberSize->Checked = !fRecognize->resetsize;
    cbHWRememberPosition->Checked = !fRecognize->resetpos;

    cbRememberTray->Checked = settings.remembertray;
    cbStartInTray->Checked = settings.startintray;
    cbStartInTrayClick(cbStartInTray);

    cbCheckUpdate->Checked = settings.autocheck;
    cbCheckInterval->ItemIndex = settings.checkinterval;

    FillKRef();
    for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
    {
        if (settings.reference_order[ix] == settings.reference_index)
        {
            cbInfo->ItemIndex = ix;
            break;
        }
    }

    cbDictKey->Checked = settings.syskeys;
    cbDictKeyClick(cbDictKey);
    cbJShift->Checked = settings.JShift;
    cbJKeys->ItemIndex = settings.JKeys;
    cbJChar->ItemIndex = cbJChar->Items->IndexOf((char)settings.JChar);

    cbEShift->Checked = settings.EShift;
    cbEKeys->ItemIndex = settings.EKeys;
    cbEChar->ItemIndex = cbEChar->Items->IndexOf((char)settings.EChar);

    cbKShift->Checked = settings.KShift;
    cbKKeys->ItemIndex = settings.KKeys;
    cbKChar->ItemIndex = cbKChar->Items->IndexOf((char)settings.KChar);

    cbShowInflectionText->Checked = settings.showinftext;
    cbShowInflectionAlways->Checked = settings.showinfalways;

    cbInGroup->Checked = settings.showingroup;
    cbInJLPT->Checked = settings.showinJLPT;

    cbFontSize->ItemIndex = settings.dictfontsize;
    cbPopFontSize->ItemIndex = settings.popfontsize;

    cbAutosize->Checked = fDict->Autosize;

    cbABC->ItemIndex = settings.alphaorder;
    cbFreq->ItemIndex = settings.freqsymbols;

    //vocabulary printing
    cbVDictFonts->Checked = settings.vdictfonts;
    cbVKana->Items->Assign(jfonts);
    cbVKana->ItemIndex = cbVKana->Items->IndexOf(settings.vkanafont);
    if (cbVKana->ItemIndex < 0 && cbVKana->Items->Count)
        cbVKana->ItemIndex = 0;
    cbVRomaji->Items->Assign(rfonts);
    cbVRomaji->ItemIndex = cbVRomaji->Items->IndexOf(settings.vromajifont);
    if (cbVRomaji->ItemIndex < 0 && cbVRomaji->Items->Count)
        cbVRomaji->ItemIndex = 0;
    if (cbVDictFonts->Checked)
        cbVDictFontsClick(cbVDictFonts);

    cbVHeight->ItemIndex = settings.vheight;
    cbVLeft->ItemIndex = settings.vleft;
    cbVRight->ItemIndex = settings.vright;
    cbVSeparate->Checked = settings.vseparate;
    cbVRightChange(cbVRight);
    cbVSep->Checked = settings.vlines;
    cbVShade->Checked = settings.vshade;
    cbVDouble->Checked = settings.vdouble;
    cbVNumber->Checked = settings.vnumber;
    cbVFurigana->Checked = settings.vfurigana;

    cbKanjiHint->Checked = settings.show_kanji_hint;
    cbKanjiHintTime->Checked = settings.hide_kanji_hint;
    edKanjiHintTime->Text = settings.kanji_hint_time;
    cbKanjiHintClick(cbKanjiHint);

    cbStatDate->ItemIndex = (int)settings.datefmt;

    edDayStart->Text = profilesettings.daystart;
    cbWarnTime->ItemIndex = (int)profilesettings.kwarntime;
    numKWarnThres->value = profilesettings.kwarnthres;
    cbIncludeInterval->ItemIndex = profilesettings.includeinterval;
    numKWarnItems->value = profilesettings.kwarnitemscount;

    cbKWarnAfter->Checked = profilesettings.kwarnafter;
    numKWarnAfter->value = profilesettings.kwarnaftertime;
    numKItemInclude->value = profilesettings.kiteminclude;
    cbKWarnAfterClick(cbKWarnAfter);

    numKRetryLimit->value = profilesettings.kdecklimitretry;
    numKHardLimit->value = profilesettings.kdecklimithard;

    cbKWarnItems->Checked = profilesettings.kwarnitems;

    cbRepetitionInterval->ItemIndex = profilesettings.krepetition;
    cbLabelInterval->Checked = profilesettings.klabelinterval;
    cbKNewFirst->Checked = profilesettings.knewfirst;

    cbKReadings->ItemIndex = profilesettings.ktestreadings;
    cbKReadingWords->ItemIndex = profilesettings.kreadingwords;
    cbKReadingsChange(cbKReadings);

    cbKTypeKanji->Checked = profilesettings.ktypekanji;
    cbKTypeKana->Checked = profilesettings.ktypekana;
    cbKTypeMeaning->Checked = profilesettings.ktypemeaning;

    if (profilesettings.kkanjihint == qtKana)
        rbKanjiKana->Checked = true;
    if (profilesettings.kkanjihint == qtMeaning)
        rbKanjiMeaning->Checked = true;
    if (profilesettings.kkanahint == qtKanji)
        rbKanaKanji->Checked = true;
    if (profilesettings.kkanahint == qtMeaning)
        rbKanaMeaning->Checked = true;
    if (profilesettings.kmeaninghint == qtKanji)
        rbMeaningKanji->Checked = true;
    if (profilesettings.kmeaninghint == qtKana)
        rbMeaningKana->Checked = true;

    cbWarnTimeChange(cbWarnTime);
    cbKWarnItemsClick(cbKWarnItems);

    cbSuspendAfter->ItemIndex = int(profilesettings.ksuspendafter);

    cbText->Selected = (TColor)colorsettings.maintext;
    cbKnO->Selected = (TColor)colorsettings.kanausage;
    cbBg->Selected = (TColor)colorsettings.background;
    cbHl->Selected = (TColor)colorsettings.highlight;
    cbGrid->Selected = (TColor)colorsettings.gridline;
    cbInfCon->Selected = (TColor)colorsettings.wordinf;
    cbTp->Selected = (TColor)colorsettings.wordtype;
    cbUsg->Selected = (TColor)colorsettings.wordusage;
    cbFld->Selected = (TColor)colorsettings.wordfield;
    cbNoWd->Selected = (TColor)colorsettings.kanjinoword;
    cbUns->Selected = (TColor)colorsettings.kanjiunsorted;
    cbN1->Selected = (TColor)colorsettings.N[0];
    cbN2->Selected = (TColor)colorsettings.N[1];
    cbN3->Selected = (TColor)colorsettings.N[2];
    cbN4->Selected = (TColor)colorsettings.N[3];
    cbN5->Selected = (TColor)colorsettings.N[4];

    freq1 = (TColor)colorsettings.wordpop;
    freq2 = (TColor)colorsettings.wordavg;
    freq3 = (TColor)colorsettings.wordunpop;

    cbLTOpenTab->Checked = settings.ltopentab;
    cbLTRestoreSizes->Checked = settings.ltrestoresizes;

    cbLTShowLast->Checked = settings.ltshowlast;

    tmpsitelist = new TSiteList;
    tmpsitelist->Assign(*sitelist);
    SetSiteText();

    cbBckCreate->Checked = settings.backupdata;
    numBckCount->value = settings.bckcount;
    numBckSpacing->value = settings.bckspacing;
    numHistCnt->value = settings.histcnt;
    numHistDelay->value = settings.histdelay;

    cbTimedHistSave->Checked = settings.timedhistsave;

    edBckPath->Text = settings.bckpath;

    TRegistry *reg = NULL;
    try
    {
        reg = new TRegistry;
        reg->Access = KEY_READ;
        reg->RootKey = HKEY_CURRENT_USER;
        if (reg->OpenKey(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", false))
        {
            wchar_t c[1024];
            GetModuleFileName(0, c, 1024);
            cbAutoStart->Checked = reg->ValueExists(L"zkanji") && reg->ReadString(L"zkanji") == c;
        }
        else
            THROW(L"Error at auto-start reading");
    }
    catch (...)
    {
        cbAutoStart->Enabled = false;
        lbAutoStart->Enabled = false;
        //MsgBox(L"An error occurred while reading the auto-start setting!",L"Error",MB_OK);
    }
    delete reg;
}
//---------------------------------------------------------------------------
__fastcall TfSettings::~TfSettings()
{
    delete numAutoSave;
    delete numKanjiHintTime;
    delete numKWarnItems;
    delete numKWarnThres;
    delete numDayStart;
    delete numKWarnAfter;
    delete numKItemInclude;
    delete numKRetryLimit;
    delete numKHardLimit;
    delete numBckCount;
    delete numBckSpacing;
    delete numHistCnt;
    delete numHistDelay;
    tmpsitelist->Clear();
    delete tmpsitelist;

    if (Kfont)
        DeleteObject(Kfont);
    Kfont = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::pbKanjiPaint(TObject *Sender)
{
    TCanvas *c = pbKanji->Canvas;
    TFont *f = c->Font;
    int rix = RealIndex(cbKanji);
    if (rix >= 0)
    {
        f->Name = cbKanji->Items->Strings[rix];
        f->Charset = (int)jfonts->Objects[rix];
    }
    else
        SetSystemFont(f);

    f->Size = double(pbKanji->ClientHeight) * 0.7 / scaleres;

    TSize s;
    GetTextExtentPoint32W(c->Handle, L"\x6f22", 1, &s);
    if (pbKanji->ClientHeight < s.cy)
        f->Size = double(pbKanji->ClientHeight) * 0.65 / scaleres;

    TRect r = pbKanji->ClientRect;
    SetTextAlign(c->Handle, VTA_BASELINE | TA_CENTER);

    HFONT oldf = NULL;
    if (cbKanjiAntialias->Checked)
    {
        oldf = f->Handle;
        if (!Kfont)
            Kfont = create_localfont(f);
        SelectObject(c->Handle, Kfont);
    }
    ExtTextOutW(c->Handle, r.Width() / 2, r.Height()*0.8, ETO_CLIPPED | ETO_OPAQUE, &r, L"\x6f22", 1, 0);
    if (oldf)
        SelectObject(c->Handle, oldf);
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbKanjiChange(TObject *Sender)
{
    if (Kfont)
        DeleteObject(Kfont);
    Kfont = 0;
    pbKanji->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbKanaChange(TObject *Sender)
{
    pbText1->Refresh();
    pbText2->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::pbText1Paint(TObject *Sender)
{
    int tag = ((TZPaintBox*)Sender)->Tag;
    TZPaintBox *pb = tag == 0 ? pbText1 : pbText2;

    TCanvas *c = pb->Canvas;
    TFont *f = c->Font;
    int oldcharset = f->Charset;
    TRect r = pb->ClientRect;
    SetTextAlign(c->Handle, TA_LEFT | TA_BASELINE);
    c->FillRect(r);

    //draw kana line
    int rix = RealIndex(cbKana);
    if (rix >= 0)
    {
        f->Name = cbKana->Items->Strings[rix];
        f->Charset = (int)jfonts->Objects[rix];
    }
    else
        SetSystemFont(f);

    f->Size = double(rowsizes((tag == 0 ? cbFontSize->ItemIndex : cbPopFontSize->ItemIndex)) * 0.66) / scaleres;

    TRect r2 = r;
    r2.top = 2;
    r2.bottom = r2.top + rowsizes(1);
    ExtTextOutW(c->Handle, 6, r2.Bottom - 2, ETO_CLIPPED | ETO_OPAQUE, &r2, L"\x3072\x3089\x304C\x306A\x30AB\x30BF\x30AB\x30CA\x5e79\x4e8b\x611f\x3058\x6f22\x5b57\x76e3\x4e8b\x51a0\x8f9e\x5b98\x4e8b", 20, 0);

    c->FillRect(Rect(0, 0, pb->ClientWidth, r2.top));

    //draw romaji line
    rix = RealIndex(cbRomaji);
    if (rix >= 0)
        f->Name = cbRomaji->Items->Strings[rix];
    else
        f->Name = L"Tahoma";
    f->Charset = DEFAULT_CHARSET;
    f->Size = double(rowsizes((tag == 0 ? cbFontSize->ItemIndex : cbPopFontSize->ItemIndex))) * 0.625 / scaleres;

    r2.top = r2.bottom;
    r2.bottom = r2.top + rowsizes(1) + 4;
    ExtTextOutW(c->Handle, 6, r2.Bottom - 5, ETO_CLIPPED | ETO_OPAQUE, &r2, L"The lazy dog jumps over the quick brown fox.", 44, 0);

    //draw comment line
    rix = RealIndex(cbSmallRomaji);
    if (rix >= 0)
        f->Name = cbSmallRomaji->Items->Strings[rix];
    else
        f->Name = L"Tahoma";
    f->Charset = DEFAULT_CHARSET;
    f->Size = 7;

    r2.top = r2.bottom;
    r2.bottom = r2.top + rowsizes(1);
    ExtTextOutW(c->Handle, 6, r2.Bottom - 6, ETO_CLIPPED | ETO_OPAQUE, &r2, L"The lazy dog jumps over the quick brown fox.", 44, 0);

    c->FillRect(Rect(0, r2.bottom, pb->ClientWidth, pb->ClientHeight));

    f->Charset = oldcharset;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::FormClose(TObject *Sender, TCloseAction &Action)
{
    settings.settingspage = pages->ActivePage->Caption;

    if (ModalResult == mrOk)
    {
        SettingsChangeBegin();

        if (RealIndex(cbKanji) >= 0)
        {
            settings.kanjifont = cbKanji->Items->Strings[RealIndex(cbKanji)];
            settings.kanjicharset = (int)jfonts->Objects[jfonts->IndexOf(settings.kanjifont)];
        }
        settings.kanjiantialias = cbKanjiAntialias->Checked;

        if (RealIndex(cbKana) >= 0)
        {
            settings.kanafont = cbKana->Items->Strings[RealIndex(cbKana)];
            settings.kanacharset = (int)jfonts->Objects[jfonts->IndexOf(settings.kanafont)];
        }
        if (RealIndex(cbRomaji) >= 0)
            settings.romajifont = cbRomaji->Items->Strings[RealIndex(cbRomaji)];
        if (RealIndex(cbSmallRomaji) >= 0)
            settings.smallromajifont = cbSmallRomaji->Items->Strings[RealIndex(cbSmallRomaji)];

        settings.searchsave = cbSearchSave->Checked;
        settings.positionrestore = cbPositionRestore->Checked;
        settings.kanjirestore = cbKanjiRestore->Checked;

        settings.autosavedelay = edAutoSave->Text.ToInt();
        settings.autosave = cbAutoSave->Checked;
        if (settings.autosave)
            fMain->StartAutosave(settings.autosavedelay);
        else
            fMain->StopAutosave();

        settings.windowdecoration = cbWinDecor->Checked;
        settings.oldstylepopup = cbOldMenu->Checked;
        settings.toolshadow = cbShadow->Checked;

        settings.minimizetotray = cbMinTray->Checked;

        settings.showclipcopy = cbShowClipboardCopy->Checked;
        settings.showclipappend = cbShowClipboardAppend->Checked;
        settings.shownonkanjielements = cbShowNonKanji->Checked;

        settings.remembertray = cbRememberTray->Checked;
        settings.startintray = cbStartInTray->Checked;

        settings.autocheck = cbCheckUpdate->Checked;
        settings.checkinterval = cbCheckInterval->ItemIndex;

        for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
        {
            settings.reference_order[ix] = (byte)cbKRef->Items->Objects[ix];
            settings.referenceshow[settings.reference_order[ix]] = cbKRef->Checked[ix];
        }
        settings.reference_index = settings.reference_order[cbInfo->ItemIndex];

        settings.columnreset = cbColumnRestore->Checked;
        settings.popupcolumnreset = cbPopupColumnRestore->Checked;
        settings.popuppositionrestore = cbPopupPositionRestore->Checked;
        settings.popupdisappear = cbPopupDisappear->Checked;
        settings.popupuseclipboard = cbPopupUseClipboard->Checked;
        settings.kanjireset = cbKanjiReset->Checked;
        settings.popuptransparency = tbTransparency->Position;

        fRecognize->resetsize = !cbHWRememberSize->Checked;
        fRecognize->resetpos = !cbHWRememberPosition->Checked;

        settings.syskeys = cbDictKey->Checked;
        settings.JShift = cbJShift->Checked;
        settings.JKeys = cbJKeys->ItemIndex;
        settings.JChar = (int)cbJChar->Text[1];
        settings.EShift = cbEShift->Checked;
        settings.EKeys = cbEKeys->ItemIndex;
        settings.EChar = (int)cbEChar->Text[1];
        settings.KShift = cbKShift->Checked;
        settings.KKeys = cbKKeys->ItemIndex;
        settings.KChar = (int)cbKChar->Text[1];

        settings.showinftext = cbShowInflectionText->Checked;
        settings.showinfalways = cbShowInflectionAlways->Checked;
        settings.alphaorder = cbABC->ItemIndex;
        if (settings.freqsymbols != cbFreq->ItemIndex)
            dictionary->ClearABCFilters();
        settings.freqsymbols = cbFreq->ItemIndex;

        settings.showingroup = cbInGroup->Checked;
        settings.showinJLPT = cbInJLPT->Checked;

        if (settings.dictfontsize != cbFontSize->ItemIndex)
        {
            for (int ix = 0; ix < FREE_HEADING_COUNT; ++ix)
                settings.ltfreesizes[ix] = -1;
            for (int ix = 0; ix < LOCK_HEADING_COUNT; ++ix)
                settings.ltlockedsizes[ix] = -1;
            for (int ix = 0; ix < TEST_HEADING_COUNT; ++ix)
                settings.lttestedsizes[ix] = -1;
        }

        settings.dictfontsize = cbFontSize->ItemIndex;
        settings.popfontsize = cbPopFontSize->ItemIndex;

        fDict->Autosize = cbAutosize->Checked;
        fPopupDict->Autosize = fDict->Autosize; //cbPopupAutosize->Checked;

        settings.vdictfonts = cbVDictFonts->Checked;
        if (RealIndex(cbVKana) >= 0)
            settings.vkanafont = cbVKana->Items->Strings[RealIndex(cbVKana)];
        if (RealIndex(cbVRomaji) >= 0)
            settings.vromajifont = cbVRomaji->Items->Strings[RealIndex(cbVRomaji)];

        settings.vheight = cbVHeight->ItemIndex;
        settings.vleft = (TVPrintItem)cbVLeft->ItemIndex;
        settings.vright = (TVPrintItem)cbVRight->ItemIndex;
        settings.vseparate = cbVSeparate->Checked && cbVSeparate->Enabled;
        settings.vlines = cbVSep->Checked;
        settings.vshade = cbVShade->Checked;
        settings.vdouble = cbVDouble->Checked;
        settings.vnumber = cbVNumber->Checked;
        settings.vfurigana = cbVFurigana->Checked;

        settings.show_kanji_hint = cbKanjiHint->Checked;
        settings.kanji_hint_time = edKanjiHintTime->Text.ToInt();
        settings.hide_kanji_hint = cbKanjiHintTime->Checked;

        settings.datefmt = (TDateFormatSetting)cbStatDate->ItemIndex;

        profilesettings.daystart = edDayStart->Text.ToInt();
        profilesettings.kwarntime = (TKWarnTime)cbWarnTime->ItemIndex;
        profilesettings.includeinterval = (TKInterval)cbIncludeInterval->ItemIndex;
        profilesettings.kwarnthres = edKWarnThres->Text.ToInt();
        profilesettings.kwarnitemscount = edKWarnItems->Text.ToInt();
        profilesettings.kwarnitems = cbKWarnItems->Checked;

        profilesettings.krepetition = (TKRInterval)cbRepetitionInterval->ItemIndex;
        profilesettings.klabelinterval = cbLabelInterval->Checked;
        profilesettings.knewfirst = cbKNewFirst->Checked;

        profilesettings.ktestreadings = (TKTestReadings)cbKReadings->ItemIndex;
        profilesettings.kreadingwords = (TKReadingWords)cbKReadingWords->ItemIndex;

        profilesettings.kwarnafter = cbKWarnAfter->Checked;
        profilesettings.kwarnaftertime = numKWarnAfter->value;
        profilesettings.kiteminclude = numKItemInclude->value;

        profilesettings.kdecklimitretry = numKRetryLimit->value;
        profilesettings.kdecklimithard = numKHardLimit->value;

        profilesettings.kkanjihint = rbKanjiKana->Checked ? qtKana : qtMeaning;
        profilesettings.kkanahint = rbKanaKanji->Checked ? qtKanji : qtMeaning;
        profilesettings.kmeaninghint = rbMeaningKanji->Checked ? qtKanji : qtKana;

        profilesettings.ktypekanji = cbKTypeKanji->Checked;
        profilesettings.ktypekana = cbKTypeKana->Checked;
        profilesettings.ktypemeaning = cbKTypeMeaning->Checked;

        profilesettings.ksuspendafter = (TKSuspendTime)cbSuspendAfter->ItemIndex;

        colorsettings.maintext = (int)cbText->Selected;
        colorsettings.kanausage = (int)cbKnO->Selected;
        colorsettings.background = (int)cbBg->Selected;
        colorsettings.highlight = (int)cbHl->Selected;
        colorsettings.gridline = (int)cbGrid->Selected;
        colorsettings.wordinf = (int)cbInfCon->Selected;
        colorsettings.wordtype = (int)cbTp->Selected;
        colorsettings.wordusage = (int)cbUsg->Selected;
        colorsettings.wordfield = (int)cbFld->Selected;
        colorsettings.kanjinoword = (int)cbNoWd->Selected;
        colorsettings.kanjiunsorted = (int)cbUns->Selected;
        colorsettings.N[0] = (int)cbN1->Selected;
        colorsettings.N[1] = (int)cbN2->Selected;
        colorsettings.N[2] = (int)cbN3->Selected;
        colorsettings.N[3] = (int)cbN4->Selected;
        colorsettings.N[4] = (int)cbN5->Selected;

        colorsettings.wordpop = freq1;
        colorsettings.wordavg = freq2;
        colorsettings.wordunpop = freq3;
        fGroup->pGroupBtn->Color = cbBg->Selected;

        settings.ltopentab = cbLTOpenTab->Checked;
        settings.ltrestoresizes = cbLTRestoreSizes->Checked;
        settings.ltshowlast = cbLTShowLast->Checked;

        if (olddecor != settings.windowdecoration)
            SendMessage(fMain->Handle, WM_THEMECHANGED, 0, 0);

        settings.backupdata = cbBckCreate->Checked;
        settings.bckcount = numBckCount->value;
        settings.bckspacing = numBckSpacing->value;

        settings.histcnt = numHistCnt->value;
        settings.histdelay = numHistDelay->value;
        settings.timedhistsave = cbTimedHistSave->Checked;

        romajihistory->Limit();
        kanjihistory->Limit();

        dictionary->DeleteBackup(!settings.backupdata ? 0 : settings.bckcount);

        UnicodeString bckpath = FullPathFromBackupPath(edBckPath->Text);
        if (edBckPath->Text.Pos(L"<>") != 1 && edBckPath->Text.LowerCase().Pos(paths.inipath.LowerCase()) == 1)
            edBckPath->Text = L"<>" + edBckPath->Text.SubString(paths.inipath.Length(), edBckPath->Text.Length() - paths.inipath.Length());

        bool checkbck = false;
        UnicodeString oldpath = FullPathFromBackupPath(settings.bckpath);
        if (bckpath.LowerCase() != oldpath.LowerCase() && PathsDifferent(bckpath, oldpath))
        {
            checkbck = true;
            if (dictionary->BackupCount() && MsgBox(L"You have changed the path for backup folders. Would you like to move existing backups to the new location?", L"Query", MB_YESNO) == IDYES)
                dictionary->MoveBackup(bckpath);
        }


        settings.bckpath = edBckPath->Text;

        if (checkbck)
            dictionary->ReadBackup();

        SettingsChangeEnd();

        sitelist->Assign(*tmpsitelist);

        if (cbAutoStart->Enabled)
        {
            TRegistry *reg = NULL;
            try
            {
                reg = new TRegistry;
                reg->Access = KEY_WRITE;
                reg->RootKey = HKEY_CURRENT_USER;
                if (reg->OpenKey(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", false))
                {
                    if (cbAutoStart->Checked)
                    {
                        wchar_t c[1024];
                        GetModuleFileName(0, c, 1024);
                        reg->WriteString(L"zkanji", c);
                    }
                    else
                    {
                        reg->DeleteValue(L"zkanji");
                    }
                }
                else
                    THROW(L"Couldn't open registry key!");
            }
            catch (...)
            {
                MsgBox(L"An error occurred while changing the auto-start setting.", L"Error", MB_OK);
            }
            delete reg;
        }

        SaveSettings();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if (ModalResult != mrOk)
        return;

    if (RealIndex(cbKanji) < 0 || RealIndex(cbKana) < 0 || RealIndex(cbRomaji) < 0 || RealIndex(cbSmallRomaji) < 0 || RealIndex(cbVRomaji) < 0 || RealIndex(cbVKana) < 0)
    {
        if (MsgBox(L"One or more selected fonts were not found in the system and won't be saved in the settings. Do you still want to continue?", L"Warning", MB_YESNO) == IDNO)
        {
            CanClose = false;
            return;
        }
    }
    if (cbDictKey->Checked)
    {
        if (!RegisterHotKey(Handle, 0, (cbJKeys->ItemIndex == 0 || cbJKeys->ItemIndex == 2 ? MOD_CONTROL : 0) | (cbJKeys->ItemIndex == 0 || cbJKeys->ItemIndex == 1 ? MOD_ALT : 0) | (cbJShift->Checked ? MOD_SHIFT : 0), (int)cbJChar->Text[1]))
        {
            MsgBox(L"The shortcut specified for the Japanese-English popup dictionary is already taken in your system. Choose another or disable system shortcut keys.", L"Error", MB_OK);
            CanClose = false;
            return;
        }
        UnregisterHotKey(Handle, 0);

        if (!RegisterHotKey(Handle, 1, (cbEKeys->ItemIndex == 0 || cbEKeys->ItemIndex == 2 ? MOD_CONTROL : 0) | (cbEKeys->ItemIndex == 0 || cbEKeys->ItemIndex == 1 ? MOD_ALT : 0) | (cbEShift->Checked ? MOD_SHIFT : 0), (int)cbEChar->Text[1]))
        {
            MsgBox(L"The shortcut specified for the English-Japanese popup dictionary is already taken in your system. Choose another or disable system shortcut keys.", L"Error", MB_OK);
            CanClose = false;
            return;
        }
        UnregisterHotKey(Handle, 1);

        if (!RegisterHotKey(Handle, 2, (cbKKeys->ItemIndex == 0 || cbKKeys->ItemIndex == 2 ? MOD_CONTROL : 0) | (cbKKeys->ItemIndex == 0 || cbKKeys->ItemIndex == 1 ? MOD_ALT : 0) | (cbKShift->Checked ? MOD_SHIFT : 0), (int)cbKChar->Text[1]))
        {
            MsgBox(L"The shortcut specified for the Kanji popup list is already taken in your system. Choose another or disable system shortcut keys.", L"Error", MB_OK);
            CanClose = false;
            return;
        }
        UnregisterHotKey(Handle, 2);
    }

    if (edBckPath->Text.IsEmpty())
    {
        MsgBox(L"Specify a valid and accessible backup folder.", L"Path missing", MB_OK);
        CanClose = false;
        return;
    }

    edBckPath->Text = edBckPath->Text.Trim();
    while (!edBckPath->Text.IsEmpty() && edBckPath->Text[edBckPath->Text.Length()] == L'\\')
        edBckPath->Text = edBckPath->Text.SetLength(edBckPath->Text.Length() - 1);

    if (edBckPath->Text.LowerCase() != settings.bckpath.LowerCase())
    {
        UnicodeString testpath = FullPathFromBackupPath(edBckPath->Text);
        if (!pathaccess(testpath))
        {
            MsgBox(L"The path specified as backup folder parent is not accessible or invalid. Choose another folder or reset to default.", L"Error", MB_OK);
            CanClose = false;
            return;
        }
        if (FileExists(testpath + L"\\backup"))
        {
            if (MsgBox(L"A file called \"backup\" is in the specified backup parent folder and will be deleted. Do you want to continue?", L"Query", MB_YESNO) != IDYES)
            {
                CanClose = false;
                return;
            }
            if (!DeleteFile(testpath + L"\\backup") && FileExists(testpath + L"\\backup"))
            {
                MsgBox(L"Cannot delete \"backup\" file from the specified backup folder. Remove the file manually or choose a different folder.", L"Message", MB_OK);
                CanClose = false;
                return;
            }
        }

        if (!DirectoryExists(testpath + L"\\backup") && !CreateDir(testpath + L"\\backup"))
        {
            MsgBox(L"Couldn't create backup folder. Choose a different parent path.", L"Message", MB_OK);
            CanClose = false;
            return;
        }

    }
}
//---------------------------------------------------------------------------
TMemIniFile* LoadIni(UnicodeString name)
{
    FILE *f = FileExists(name) ? _wfopen(name.c_str(), L"rb") : 0;
    if (!f)
        return new TMemIniFile(name, TEncoding::UTF8);

    char c;
    fread(&c, sizeof(char), 1, f);
    fclose(f);
    if (c == '[' || c == ' ' || c == '\r' || c == '\n')
        return new TMemIniFile(name);
    return new TMemIniFile(name, TEncoding::UTF8);
}

void __fastcall LoadSettings()
{
    UnicodeString s;
    //int n;
    TMemIniFile *ini = LoadIni(paths.inipath + L"zkanji.ini");

    colorsettings.activeborder = 0xffffff;

    error_code = 0;
    try
    {
        error_code = 5099;

        /*open settings for profile first!*/
        profilesettings.daystart = ini->ReadInteger(L"long-term study", L"day start", 4);
        profilesettings.kwarntime = (TKWarnTime)ini->ReadInteger(L"long-term study", L"warn time", 0);
        profilesettings.kwarnthres = ini->ReadInteger(L"long-term study", L"kanji warn threshold", 10);
        profilesettings.includeinterval = (TKInterval)ini->ReadInteger(L"long-term study", L"include interval", 0);
        profilesettings.kiteminclude = max(0, ini->ReadInteger(L"long-term study", L"include count", 20));

        //remove after 2011. Jun. or sooner!
        settings.datefmt = (TDateFormatSetting)ini->ReadInteger(L"long-term study", L"date format", 0);

        profilesettings.krepetition = (TKRInterval)ini->ReadInteger(L"long-term study", L"kanji repetition interval", 1);
        profilesettings.klabelinterval = ini->ReadBool(L"long-term study", L"label repetition interval", true);

        profilesettings.ktestreadings = (TKTestReadings)ini->ReadInteger(L"long-term study", L"kanji tested readings", 1);
        profilesettings.kreadingwords = (TKReadingWords)ini->ReadInteger(L"long-term study", L"kanji reading words", 2);

        profilesettings.knewfirst = ini->ReadBool(L"long-term study", L"new items first", true);

        profilesettings.kwarnitems = ini->ReadBool(L"long-term study", L"warn items", true);
        profilesettings.kwarnitemscount = ini->ReadInteger(L"long-term study", L"warn items count", 60);

        //int kltmp = ini->ReadInteger(L"long-term study", L"last warning", 0);
        //profilesettings.klastwarning = *(float*) &kltmp;
        profilesettings.klastwarning = IniReadDouble(ini, L"long-term study", L"last warning", 0);

        profilesettings.kwarnafter = ini->ReadBool(L"long-term study", L"warn after", true);
        profilesettings.kwarnaftertime = ini->ReadInteger(L"long-term study", L"warn after length", 10);

        profilesettings.kdecklimitretry = ini->ReadInteger(L"long-term study", L"deck limit retry", 4);
        profilesettings.kdecklimithard = ini->ReadInteger(L"long-term study", L"deck limit hard", 3);

        profilesettings.ktypekanji = ini->ReadBool(L"long-term study", L"type kanji", true);
        profilesettings.ktypekana = ini->ReadBool(L"long-term study", L"type kana", true);
        profilesettings.ktypemeaning = ini->ReadBool(L"long-term study", L"type meaning", false);

        profilesettings.ksuspendafter = (TKSuspendTime)ini->ReadInteger(L"long-term study", L"suspend after", 0);

        profilesettings.kkanjihint = (TQuestionType)ini->ReadInteger(L"long-term study", L"hint for kanji", qtKana);
        profilesettings.kkanahint = (TQuestionType)ini->ReadInteger(L"long-term study", L"hint for kana", qtKanji);
        profilesettings.kmeaninghint = (TQuestionType)ini->ReadInteger(L"long-term study", L"hint for meaning", qtKanji);

        error_code = 64;

#ifdef _DEBUG
        settings.debug = true;
#else
        settings.debug = ini->ReadBool(L"Debug", L"debug", false);
#endif

        settings.firstrun = ini->ReadBool(L"general", L"first run", true);
        settings.exportdir = ini->ReadString(L"general", L"export folder", paths.inipath);

        settings.datefmt = (TDateFormatSetting)ini->ReadInteger(L"general", L"date format", settings.datefmt);

        ini->WriteBool(L"general", L"first run", false);
        error_code = 1;

        /*settings.baselayout*/fDock->layout = (TDockLayout)ini->ReadInteger(L"view", L"layout", dlUndecided/*(int)(Screen->Width > 700 ? wlExpandKanji : wlExpandDict)*/);
        //settings.docked = Screen->Width <= 824 || ini->ReadBool(L"view",L"docked",true);

        if (wordfilters)
        {
            wordfilters->Clear();
            dictionaryfilters->Clear();
            kanjiwordfilters->Clear();
        }

        UnicodeString filtername;
        int filterindex = 0;
        while ((filtername = ini->ReadString(L"word filters", L"filter #" + IntToStr(filterindex) + L"name", L"")).Length())
        {
            UnicodeString fident = L"filter #" + IntToStr(filterindex);
            TWordFilter filter;
            filter.types = ini->ReadInteger(L"word filters", fident + L"types", (unsigned int) - 1);
            filter.notes = ini->ReadInteger(L"word filters", fident + L"notes", (unsigned int) - 1);
            filter.fields = ini->ReadInteger(L"word filters", fident + L"fields", (unsigned int) - 1);
            filter.typeinclusion = (TWordTypeInclusion)ini->ReadInteger(L"word filters", fident + L"all types", 0);
            filter.allusages = ini->ReadBool(L"word filters", fident + L"all usages", false);
            for (int ix = 0; ix <= 5; ++ix)
                filter.JLPT[ix] = ini->ReadBool(L"word filters", fident + " " + (!ix ? L"Non-JLPT" : (L"JLPT N" + IntToStr(ix)).c_str()), (unsigned int) - 1);
            wordfilters->Add(filtername, filter);
            filterindex++;
        }

        filterindex = 0;
        while ((filtername = ini->ReadString(L"dictionary filters", L"filter #" + IntToStr(filterindex), L"")) != L"")
        {
            bool exclude = ini->ReadBool(L"dictionary filters", L"exclude filter #" + IntToStr(filterindex), false);
            int fix;
            if ((fix = wordfilters->IndexOf(filtername)) >= 0)
                dictionaryfilters->Add(wordfilters->Id[fix] * (exclude ? -1 : 1));
            filterindex++;
        }

        filterindex = 0;
        while ((filtername = ini->ReadString(L"kanji word filters", L"filter #" + IntToStr(filterindex), L"")) != L"")
        {
            bool exclude = ini->ReadBool(L"kanji word filters", L"exclude filter #" + IntToStr(filterindex), false);
            int fix;
            if ((fix = wordfilters->IndexOf(filtername)) >= 0)
                kanjiwordfilters->Add(wordfilters->Id[fix] * (exclude ? -1 : 1));
            filterindex++;
        }

        filterindex = 0;
        while ((filtername = ini->ReadString(L"word group filters", L"filter #" + IntToStr(filterindex), L"")) != L"")
        {
            bool exclude = ini->ReadBool(L"word group filters", L"exclude filter #" + IntToStr(filterindex), false);
            int fix;
            if ((fix = wordfilters->IndexOf(filtername)) >= 0)
                wordgroupfilters->Add(wordfilters->Id[fix] * (exclude ? -1 : 1));
            filterindex++;
        }

        //WordFiltersChanged();

        settings.vocabcollection = ini->ReadString(L"new vocabulary", L"collection", L"");
        settings.vocabgroup = ini->ReadString(L"new vocabulary", L"group", L"");
        error_code = 8;

        settings.kanjicollection = ini->ReadString(L"kanji transfer", L"collection", L"");
        settings.kanjigroup = ini->ReadString(L"kanji transfer", L"group", L"");
        error_code = 9;

        settings.dictionary_index = 0;
        /*
         // SelectDictionary only works when settings.dictionary_index is different from the set value, so +1
         settings.dictionary_index = MIN(dictionary->Count-1,ini->ReadInteger(L"dictionary",L"index",0))+1;
         fDict->SelectDictionary(settings.dictionary_index-1);
         error_code = 10;
         */
        settings.kanjiantialias = ini->ReadBool(L"fonts", L"classic antialias", false);
        settings.kanjifont = ini->ReadString(L"fonts", L"kanji", L"MS Mincho");
        int fontindex = jfonts->IndexOf(settings.kanjifont);
        if (fontindex < 0)
        {
            fontindex = jfonts->IndexOf("MS Mincho");
            if (fontindex < 0)
                fontindex = systemfontindex;
            settings.kanjifont = fontindex >= 0 ? jfonts->Strings[fontindex].c_str() : L"Arial";
        }
        if (fontindex >= 0)
            settings.kanjicharset = (int)jfonts->Objects[fontindex];
        else
            settings.kanjicharset = DEFAULT_CHARSET;
        error_code = 11;

        elementsettings.kanjifont = settings.kanjifont;
        elementsettings.kanjicharset = settings.kanjicharset;

        settings.kanafont = ini->ReadString(L"fonts", L"kana", L"MS Gothic");
        fontindex = jfonts->IndexOf(settings.kanafont);
        if (fontindex < 0)
        {
            fontindex = jfonts->IndexOf("MS Gothic");
            if (fontindex < 0)
                fontindex = systemfontindex;
            settings.kanafont = fontindex >= 0 ? jfonts->Strings[fontindex].c_str() : L"Arial";
        }
        if (fontindex >= 0)
            settings.kanacharset = (int)jfonts->Objects[fontindex];
        else
            settings.kanacharset = DEFAULT_CHARSET;
        error_code = 12;

        settings.romajifont = ini->ReadString(L"fonts", L"romaji", L"Tahoma");
        fontindex = rfonts->IndexOf(settings.romajifont);
        if (fontindex < 0)
        {
            fontindex = rfonts->IndexOf("Segoe UI");
            if (fontindex < 0)
                fontindex = rfonts->IndexOf("Tahoma");
            settings.kanafont = fontindex >= 0 ? rfonts->Strings[fontindex].c_str() : L"Arial";
        }
        error_code = 13;

        settings.smallromajifont = ini->ReadString(L"fonts", L"smallromaji", L"Arial");
        fontindex = rfonts->IndexOf(settings.smallromajifont);
        if (fontindex < 0)
        {
            fontindex = rfonts->IndexOf("Segoe UI");
            settings.smallromajifont = fontindex >= 0 ? rfonts->Strings[fontindex].c_str() : L"Arial";
        }
        error_code = 14;

        settings.autosave = ini->ReadBool(L"userdata", L"autosave", true);
        settings.autosavedelay = ini->ReadInteger(L"userdata", L"autosavedelay", 2);
        error_code = 15;

        settings.minimizetotray = ini->ReadBool(L"general", L"minimize to tray", !MatchVersion(6, 1));

        settings.windowdecoration = ini->ReadBool(L"windows", L"window decoration", true);
        TThemeServices *t = ThemeServices();
        ThemesAreTurnedOn = settings.windowdecoration && t && t->ThemesAvailable && t->ThemesEnabled;

        settings.oldstylepopup = ini->ReadBool(L"general", L"old style popup", false);
        settings.toolshadow = ini->ReadBool(L"general", L"tool window shadow", false);

        settings.searchsave = ini->ReadBool(L"exit", L"searchsave", true);
        settings.positionrestore = ini->ReadBool(L"run", L"positionrestore", true);
        settings.kanjirestore = ini->ReadBool(L"run", L"kanjirestore", true);
        error_code = 16;

        settings.showclipcopy = ini->ReadBool(L"clipboard", L"showoncopy", false);
        settings.showclipappend = ini->ReadBool(L"clipboard", L"showonappend", true);
        error_code = 17;

        settings.remembertray = ini->ReadBool(L"run", L"remembertray", false);
        settings.startintray = ini->ReadBool(L"run", L"startintray", false);
        settings.intray = settings.startintray || settings.remembertray && ini->ReadBool(L"run", L"intray", false);
        error_code = 18;

        settings.autocheck = ini->ReadBool(L"update", L"autocheck", false);
        settings.checkinterval = ini->ReadInteger(L"update", L"checkinterval", -1);
        if (settings.checkinterval > 3)
            settings.checkinterval = 1;
        settings.lastcheck = ini->ReadInteger(L"update", L"lastcheck day", (int)Now() - 365);
        error_code = 185;

        settings.columnreset = ini->ReadBool(L"run", L"columnreset", true);
        settings.popupcolumnreset = ini->ReadBool(L"popup", L"columnreset", true);
        settings.popuppositionrestore = ini->ReadBool(L"popup", L"positionrestore", true);
        settings.popupdisappear = ini->ReadBool(L"popup", L"disappear", false);
        settings.popupuseclipboard = ini->ReadBool(L"popup", L"useclipboard", true);
        settings.kanjireset = ini->ReadBool(L"popup", L"kanjireset", false);
        error_code = 20;

        settings.colkanji = ini->ReadInteger(L"dictionary", L"colkanji", 100);
        settings.colkana = ini->ReadInteger(L"dictionary", L"colkana", 120);
        settings.popupcolkanji = ini->ReadInteger(L"popup", L"colkanji", 68);
        settings.popupcolkana = ini->ReadInteger(L"popup", L"colkana", 88);
        settings.popupwidth = ini->ReadInteger(L"popup", L"width", 450);
        settings.popupheight = ini->ReadInteger(L"popup", L"height", 120);
        settings.popupfloatrect = Rect(ini->ReadInteger(L"popup", "floatleft", 50), ini->ReadInteger(L"popup", "floattop", 50), ini->ReadInteger(L"popup", "floatwidth", 550), ini->ReadInteger(L"popup", "floatheight", 250));
        settings.popupfloatrect.Right += settings.popupfloatrect.Left;
        settings.popupfloatrect.Bottom += settings.popupfloatrect.Top;
        fPopupDict->Float(ini->ReadBool(L"popup", L"floating", false));

        settings.studycolkanji = ini->ReadInteger(L"study", L"colkanji", 100);
        settings.studycolkana = ini->ReadInteger(L"study", L"colkana", 120);
        error_code = 21;

        settings.reference_index = ini->ReadInteger(L"info", L"reference index", 0);
        try
        {
            TGapList<bool> refchk(false);
            for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
            {
                settings.reference_order[ix] = ini->ReadInteger(L"info", (UnicodeString)L"Ref. Order" + ix, ix);
                if (settings.reference_order[ix] >= KANJI_REFERENCE_COUNT || refchk.ItemSet(settings.reference_order[ix]))
                    THROW(L"Error! Bad kanji reference order!");
                refchk.Items[settings.reference_order[ix]] = true;
                settings.referenceshow[ix] = ini->ReadBool(L"info", (UnicodeString)L"Ref. Show" + ix, true);
            }
        }
        catch (...)
        {
            for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
            {
                settings.reference_order[ix] = ix;
                settings.referenceshow[ix] = true;
            }
        }

        settings.shownonkanjielements = ini->ReadBool(L"info", L"show non-kanji elements", false);

        settings.syskeys = ini->ReadBool(L"popup", L"syskeys", true);
        settings.JShift = ini->ReadBool(L"popup", L"jshift", false);
        settings.JKeys = ini->ReadInteger(L"popup", L"jkeys", 0);
        settings.JChar = ini->ReadInteger(L"popup", L"jchar", (int)'J');
        error_code = 23;

        settings.EShift = ini->ReadBool(L"popup", L"eshift", false);
        settings.EKeys = ini->ReadInteger(L"popup", L"ekeys", 0);
        settings.EChar = ini->ReadInteger(L"popup", L"echar", (int)'E');
        error_code = 24;

        settings.KShift = ini->ReadBool(L"popup", L"kshift", false);
        settings.KKeys = ini->ReadInteger(L"popup", L"kkeys", 0);
        settings.KChar = ini->ReadInteger(L"popup", L"kchar", (int)'K');
        error_code = 25;

        settings.popuptransparency = ini->ReadInteger(L"popup", "transparency", 0);
        error_code = 251;

        UnicodeString setpage = ini->ReadString(L"setup", L"settings page", L"Global");

        //settings.settingspage = ini->ReadInteger(L"setup", L"settingspage", 0);
        settings.settingspage = ini->ReadString(L"setup", L"settingspage", L"");

        settings.showinftext = ini->ReadBool(L"dictionary", L"showinftext", true);
        settings.showinfalways = ini->ReadBool(L"dictionary", L"showinfalways", false);
        settings.alphaorder = ini->ReadInteger(L"dictionary", L"alpha order", 0);
        settings.freqsymbols = ini->ReadInteger(L"dictionary", L"frequency symbols", 0);

        settings.showingroup = ini->ReadBool(L"dictionary", L"showingroup", false);
        settings.showinJLPT = ini->ReadBool(L"dictionary", L"indicate JLPT", true);

        settings.dictfontsize = ini->ReadInteger(L"dictionary", L"size", 1);
        settings.popfontsize = ini->ReadInteger(L"popupdictionary", L"size", 2);
        error_code = 26;

        settings.histcnt = ini->ReadInteger(L"search history", L"count", 100);
        settings.timedhistsave = ini->ReadBool(L"search history", L"timed save", true);
        settings.histdelay = ini->ReadInteger(L"search history", L"delay", 4);
        error_code = 30;

        settings.classic2group = ini->ReadBool(L"kanji", L"classic2 group", true);
        settings.classic2exact = ini->ReadBool(L"kanji", L"classic2 exact", false);
        settings.classic2hidenames = ini->ReadBool(L"kanji", L"classic2 hide names", false);
        error_code = 48;

        settings.M1Num = ini->ReadInteger(L"wordstudy", L"M1Num", 0);
        settings.M1Order = ini->ReadInteger(L"wordstudy", L"M1Order", 0);
        settings.M1Rand = ini->ReadBool(L"wordstudy", L"M1Rand", false);
        settings.M1Add = ini->ReadInteger(L"wordstudy", L"M1Add", 1);
        settings.M1Selection = ini->ReadInteger(L"wordstudy", L"M1Selection", 0);
        settings.M1Global = ini->ReadBool(L"wordstudy", L"M1Global", false);
        settings.statdays = ini->ReadInteger(L"wordstudy", L"stat deflation days", 60);
        if (!settings.statdays)
            settings.statdays = 60;
        error_code = 57;

        settings.M2Num = ini->ReadInteger(L"wordstudy", L"M2Num", 0);
        settings.M2Inc = ini->ReadInteger(L"wordstudy", L"M2Inc", 0);

        settings.M0Practice = ini->ReadInteger(L"wordstudy", L"M0Practice", 255); // Old word group study setting before v0.731. Removed 2013 07 04.
        if (settings.M0Practice != 255)
        {
            settings.M0Choices = ini->ReadInteger(L"wordstudy", L"M0Choices", 255); // Old word group study setting before v0.731. Removed 2013 07 04.
            settings.M0Type = ini->ReadBool(L"wordstudy", L"M0Type", false); // Old word group study setting before v0.731. Removed 2013 07 04.
            ini->DeleteKey(L"wordstudy", L"M0Practice");
            ini->DeleteKey(L"wordstudy", L"M0Choices");
            ini->DeleteKey(L"wordstudy", L"M0Type");
        }
        else
        {
            settings.M0Question = ini->ReadInteger(L"wordstudy", L"M0Question", 0); // Added 2013 07 04 for v0.731.
            settings.M0Answer = ini->ReadInteger(L"wordstudy", L"M0Answer", 0); // Added 2013 07 04 for v0.731.
        }

        settings.M0Timer = ini->ReadInteger(L"wordstudy", L"M0Timer", 0);


        settings.M0HideKana = ini->ReadBool(L"wordstudy", L"M0HideKana", false);
        settings.compactreview = ini->ReadBool(L"wordstudy", L"compactreview", false);
        error_code = 58;

        //settings.topwindow  = ini->ReadInteger(L"windows",L"topwindow",WindowList->IndexOf(fDict));
        //if (settings.topwindow < 0)
        //settings.topwindow = 0;

        settings.save_gen = ini->ReadBool(L"word list generation", L"save", true);
        settings.gen_src = ini->ReadInteger(L"word list generation", L"source", 0);
        settings.gen_dest = ini->ReadInteger(L"word list generation", L"destination", 0);
        settings.gen_linked = ini->ReadBool(L"word list generation", L"link dictionaries", true);
        settings.gen_group = ini->ReadString(L"word list generation", L"dest group", L"");
        settings.gen_len = ini->ReadInteger(L"word list generation", L"kana length", 0);
        settings.gen_pop = ini->ReadInteger(L"word list generation", L"min frequency", 0);
        settings.gen_pos = ini->ReadInteger(L"word list generation", L"kanji position", 0);
        settings.gen_only = ini->ReadBool(L"word list generation", L"only from list", false);
        settings.gen_grad = ini->ReadInteger(L"word list generation", L"max grade", 0);
        settings.gen_num = ini->ReadInteger(L"word list generation", L"max kanji count", 0);
        error_code = 59;

        settings.vdictfonts = ini->ReadBool(L"vocabulary printing", L"dictionary fonts", true);
        settings.vkanafont = ini->ReadString(L"vocabulary printing", L"kana", L"MS Gothic");

        fontindex = jfonts->IndexOf(settings.vkanafont);
        if (fontindex < 0)
        {
            fontindex = jfonts->IndexOf("MS Mincho");
            if (fontindex < 0)
                fontindex = systemfontindex;
            settings.vkanafont = fontindex >= 0 ? jfonts->Strings[fontindex].c_str() : L"Arial";
        }
        if (fontindex >= 0)
            settings.vkanacharset = (int)jfonts->Objects[fontindex];
        else
            settings.vkanacharset = DEFAULT_CHARSET;

        settings.vromajifont = ini->ReadString(L"vocabulary printing", L"romaji", L"Tahoma");
        fontindex = rfonts->IndexOf(settings.vromajifont);
        if (fontindex < 0)
        {
            fontindex = rfonts->IndexOf("Tahoma");
            settings.vromajifont = fontindex >= 0 ? rfonts->Strings[fontindex].c_str() : L"Arial";
        }

        settings.vheight = ini->ReadInteger(L"vocabulary printing", L"row height", 0);
        settings.vleft = (TVPrintItem)ini->ReadInteger(L"vocabulary printing", L"left", 0);
        settings.vright = (TVPrintItem)ini->ReadInteger(L"vocabulary printing", L"right", 1);
        settings.vseparate = ini->ReadBool(L"vocabulary printing", L"separate", false);
        settings.vlines = ini->ReadBool(L"vocabulary printing", L"lines", false);
        settings.vshade = ini->ReadBool(L"vocabulary printing", L"shade", false);
        settings.vdouble = ini->ReadBool(L"vocabulary printing", L"double", false);
        settings.vnumber = ini->ReadBool(L"vocabulary printing", L"number pages", false);
        settings.vfurigana = ini->ReadBool(L"vocabulary printing", L"furigana", false);
        error_code = 60;

        settings.printerindex = ini->ReadInteger(L"vocabulary printing", L"printer index", 0);
        settings.vportrait = ini->ReadBool(L"vocabulary printing", L"orientation portrait", true);
        settings.vtofile = ini->ReadBool(L"vocabulary printing", L"print to file", false);
        error_code = 61;

        settings.show_kanji_hint = ini->ReadBool(L"kanji", L"show kanji hint", true);
        settings.hide_kanji_hint = ini->ReadBool(L"kanji", L"hide kanji hint", true);
        settings.kanji_hint_time = ini->ReadInteger(L"kanji", L"kanji hint time", 5);
        error_code = 62;

        settings.kanatests = false;

        error_code = 65;

        colorsettings.maintext = ini->ReadInteger(L"colors", L"main text", (int)clWindowText);
        colorsettings.kanausage = ini->ReadInteger(L"colors", L"kana usage", (int)0xa0b090);
        colorsettings.background = ini->ReadInteger(L"colors", L"background", (int)clWindow);
        colorsettings.highlight = ini->ReadInteger(L"colors", L"highlight", 0xe0e0e0);
        colorsettings.gridline = ini->ReadInteger(L"colors", L"grid line", 0xcacaca);
        colorsettings.wordinf = ini->ReadInteger(L"colors", L"word inf/conj bg.", 0xeaffff);
        colorsettings.wordtype = ini->ReadInteger(L"colors", L"word type", 0x308040);
        colorsettings.wordusage = ini->ReadInteger(L"colors", L"word usage", 0x204090);
        colorsettings.wordfield = ini->ReadInteger(L"colors", L"word field", 0x804030);
        colorsettings.kanjinoword = ini->ReadInteger(L"colors", L"kanji no words", (int)clSilver);
        colorsettings.kanjiunsorted = ini->ReadInteger(L"colors", L"unsorted kanji", 0xf5fdfd);
        colorsettings.wordpop = ini->ReadInteger(L"colors", L"word frequency popular", 0x0000a0f0);
        colorsettings.wordavg = ini->ReadInteger(L"colors", L"word frequency average", 0x00d0d0d0);
        colorsettings.wordunpop = ini->ReadInteger(L"colors", L"word frequency unpopular", 0x000000f0);
        colorsettings.N[0] = ini->ReadInteger(L"colors", L"N1 color", 0x00000000);
        colorsettings.N[1] = ini->ReadInteger(L"colors", L"N2 color", 0x00C84D91);
        colorsettings.N[2] = ini->ReadInteger(L"colors", L"N3 color", 0x0099A007);
        colorsettings.N[3] = ini->ReadInteger(L"colors", L"N4 color", 0x000E7BB6);
        colorsettings.N[4] = ini->ReadInteger(L"colors", L"N5 color", 0x002222FF);
        error_code = 66;

        settings.ltopentab = ini->ReadBool(L"long-term study", L"open last tab", true);
        settings.ltrestoresizes = ini->ReadBool(L"long-term study", L"restore sizes", false);
        settings.lttab = settings.ltopentab ? ini->ReadInteger(L"long-term study", L"active tab", 0) : 0;
        settings.ltlist = settings.ltopentab ? ini->ReadInteger(L"long-term study", L"active list", 0) : 0;
        settings.ltshowlast = ini->ReadBool(L"long-term study", L"show last test", true);

        settings.ltitemstat = settings.ltopentab ? ini->ReadInteger(L"long-term study", L"item stat", 0) : 0;
        settings.ltglobalstat = settings.ltopentab ? ini->ReadInteger(L"long-term study", L"global stat", 0) : 0;
        settings.ltitemstatsize = settings.ltopentab ? ini->ReadInteger(L"long-term study", L"item stat size", 150) : 150;
        settings.ltshowitemstat = settings.ltopentab ? ini->ReadBool(L"long-term study", L"show item stat", false) : false;

        settings.ltfreevisible = ini->ReadInteger(L"long-term study", L"free visible columns", 0x1f);
        settings.ltlockedvisible = ini->ReadInteger(L"long-term study", L"locked visible columns", 0x28c);
        settings.lttestedvisible = ini->ReadInteger(L"long-term study", L"tested visible columns", 0x277C);
        for (int ix = 0; ix < FREE_HEADING_COUNT; ix++)
            settings.ltfreesizes[ix] = settings.ltrestoresizes ? -1 : ini->ReadInteger(L"long-term study", (UnicodeString)L"free column size" + ix, -1);
        for (int ix = 0; ix < LOCK_HEADING_COUNT; ix++)
            settings.ltlockedsizes[ix] = settings.ltrestoresizes ? -1 : ini->ReadInteger(L"long-term study", (UnicodeString)L"locked column size" + ix, -1);
        for (int ix = 0; ix < TEST_HEADING_COUNT; ix++)
            settings.lttestedsizes[ix] = settings.ltrestoresizes ? -1 : ini->ReadInteger(L"long-term study", (UnicodeString)L"tested column size" + ix, -1);
        settings.ltfreeordercolumn = ini->ReadInteger(L"long-term study", L"free order column", 1);
        settings.ltlockedordercolumn = ini->ReadInteger(L"long-term study", L"locked order column", 6);
        settings.lttestedordercolumn = ini->ReadInteger(L"long-term study", L"tested order column", 8);

        settings.ltsodnumber = ini->ReadBool(L"long-term study", L"sod numbers", true);

        error_code = 662;

        settings.ltimportwidth = ini->ReadInteger(L"long-term word import", L"width", 650);
        settings.ltimportheight = ini->ReadInteger(L"long-term word import", L"height", 550);

        error_code = 663;

        sitelist->Clear();
        int n = ini->ReadInteger(L"dictionary sites", L"count", sitelist->Count);
        for (int ix = 0; ix < n; ++ix)
        {
            UnicodeString tmpname = ini->ReadString(L"dictionary sites", (UnicodeString)L"name" + (ix + 1), L"");
            UnicodeString tmpaddress = ini->ReadString(L"dictionary sites", (UnicodeString)L"address" + (ix + 1), L"");
            int tmppos = ini->ReadInteger(L"dictionary sites", (UnicodeString)L"insert" + (ix + 1), -1);
            if (tmpname == "" || tmpaddress == "" || tmppos < 0)
                continue;
            sitelist->Add(tmpname, tmpaddress, tmppos);
        }

        error_code = 661;

        settings.example_cache = MIN(1024 * 1024 * 5, MAX(1024 * 512, ini->ReadInteger(L"example sentences", L"cache size", 1024 * 1024)));

        settings.backupdata = ini->ReadBool(L"backup", L"create", true);
        settings.bckcount = ini->ReadInteger(L"backup", L"count", 2);
        settings.bckspacing = ini->ReadInteger(L"backup", L"spacing", 7);
        settings.bckpath = ini->ReadString(L"backup", L"path", L"<>\\data");
        //settings.lastbackup = IniReadDouble(ini, L"backup", L"last", 0);
    }
    catch (...)
    {
        MsgBox(((UnicodeString)L"An error occurred while reading settings from main ini file! (code: " + IntToStr(error_code) + ")").c_str(), L"Error", MB_OK);
        error_code = 0;
    }
    if (settings.firstrun && error_code)
        ini->UpdateFile();
    delete ini;

    if (error_code != 0)
    {
        ini = LoadIni(paths.inipath + L"kanatest.ini");

        try
        {
            error_code = 661;

            int *k;
            int size = ini->ReadInteger(L"Hiragana", L"size", 0);
            if (size)
            {
                k = new int[size];
                memset(k, 0, sizeof(int)*size);
                try
                {
                    for (int i = 0; i < size; i++)
                        k[i] = ini->ReadInteger(L"Hiragana", (UnicodeString)L"set" + i, 0);
                    settings.bitshira = Bits(k, sizeof(int), size);
                }
                catch (...)
                {
                    ;
                }
                delete[] k;
            }

            error_code = 67;

            size = ini->ReadInteger(L"Katakana", L"size", 0);

            if (size)
            {
                k = new int[size];
                memset(k, 0, sizeof(int)*size);
                try
                {
                    for (int i = 0; i < size; i++)
                        k[i] = ini->ReadInteger(L"Katakana", (UnicodeString)L"set" + i, 0);
                    settings.bitskata = Bits(k, sizeof(int), size);
                }
                catch (...)
                {
                    ;
                }
                delete[] k;
            }

        }
        catch (...)
        {
            MsgBox(((UnicodeString)L"An error occurred while reading settings from kanatest ini file! (code: " + IntToStr(error_code) + ")").c_str(), L"Error", MB_OK);
            error_code = 0;
        }
        delete ini;
    }

    if (error_code != 0)
    {
        ini = LoadIni(paths.inipath + L"elements.ini");

        try
        {
            elementsettings.lastfilename = ini->ReadString(L"files", L"last", L"");
            if (!FileExists(elementsettings.lastfilename))
                elementsettings.lastfilename = L"";
            elementsettings.kanjibackground = ini->ReadBool(L"view", L"show font background", false);
            elementsettings.strokenumbers = ini->ReadBool(L"view", L"show stroke order", true);
            elementsettings.forcecentre = ini->ReadBool(L"fonts", L"background font force centered", false);
            elementsettings.kanjifont = ini->ReadString(L"fonts", L"background font name", settings.kanjifont);
            elementsettings.kanjicharset = ini->ReadInteger(L"fonts", L"background font charset", settings.kanjicharset);
            elementsettings.kanjicolor = (TColor)ini->ReadInteger(L"fonts", L"background font color", (TColor)0xeaeaea);
            UnicodeString tmpfontsize = ini->ReadString(L"fonts", L"background font size", 0.75);

            elementsettings.kanjisize = _wtof(tmpfontsize.c_str());
        }
        catch (...)
        {
            MsgBox(((UnicodeString)L"An error occurred while reading settings from element ini file! (code: " + IntToStr(error_code) + ")").c_str(), L"Error", MB_OK);
            error_code = 0;
        }
        delete ini;
    }
    error_code = 0;
}
//---------------------------------------------------------------------------
void __fastcall LoadUISettings()
{
    UnicodeString s;
    int n;
    TMemIniFile *ini = LoadIni(paths.inipath + L"zkanji.ini");

    error_code = 0;
    try
    {
        error_code = 5099;

        fDict->btnFilter->Down = ini->ReadBool(L"dictionary", L"use filter", false);
        if (!ini->ReadBool(L"dictionary", L"example page selector", true))
            fDict->pExPageBtnClick(fDict->pExPageBtn);
        error_code = 6;

        if (!ini->ReadBool(L"dictionary", L"example type selector", true))
            fDict->pExPageBtnClick(fDict->pExTypeBtn);
        error_code = 7;

        //SelectDictionary only works when settings.dictionary_index is different from the set value, so +1
        settings.dictionary_index = MIN(dictionary->Count - 1, ini->ReadInteger(L"dictionary", L"index", 0)) + 1;
        fDict->SelectDictionary(settings.dictionary_index - 1);
        error_code = 10;

        fPopupDict->btnFilter->Down = ini->ReadBool(L"popup", L"use filter", false);
        if (!ini->ReadBool(L"popup", L"example page selector", false))
            fPopupDict->pExPageBtnClick(fPopupDict->pExPageBtn);
        if (!ini->ReadBool(L"popup", L"example type selector", false))
            fPopupDict->pExPageBtnClick(fPopupDict->pExTypeBtn);
        error_code = 19;

        fClipboard->cbAuto->Checked = ini->ReadBool(L"clipboard", L"autotranslate", false);
        error_code = 22;

        fGroup->btnKFilter->Down = ini->ReadBool(L"groups", L"use kanji word filter", false);
        fGroup->btnWFilter->Down = ini->ReadBool(L"groups", L"use word group filter", false);
        error_code = 23;

        fDict->settingsChanged(fDict, spsEnd);
        fGroup->settingsChanged(fGroup, spsEnd);
        fPopupDict->settingsChanged(fPopupDict, spsEnd);
        error_code = 27;

        switch (ini->ReadInteger(L"dictionary", L"mode", false))
        {
        case 0:
            fDict->btnKanji->Click();
            break;
        case 1:
            fDict->btnRomaji->Click();
            break;
        case 2:
            fDict->btnJapanese->Click();
            break;
        }
        error_code = 28;

        fDict->sbRomajiEnd->Down = ini->ReadBool(L"dictionary", L"romajiend", true);
        fDict->sbKanjiEnd->Down = ini->ReadBool(L"dictionary", L"kanjiend", true);
        fDict->sbKanjiStart->Down = !fDict->sbKanjiEnd->Down && ini->ReadBool(L"dictionary", L"kanjistart", false);
        fDict->sbExamples->Down = ini->ReadBool(L"dictionary", L"show examples", false);
        error_code = 29;

        if (fDict->sbExamples->Down)
            fDict->sbExamplesClick(fDict->sbExamples);
        n = ini->ReadInteger(L"dictionary", L"example mode", 0);
        switch (n)
        {
        case 0:
            fDict->sbEx1->Down = true;
            fDict->sbEx1->Click();
            break;
        case 1:
            fDict->sbEx2->Down = true;
            fDict->sbEx2->Click();
            break;
        case 2:
            fDict->sbEx3->Down = true;
            fDict->sbEx3->Click();
            break;
        }
        error_code = 30;

        fPopupDict->sbRomajiEnd->Down = ini->ReadBool(L"popupdictionary", L"romajiend", true);
        fPopupDict->sbKanjiEnd->Down = ini->ReadBool(L"popupdictionary", L"kanjiend", true);
        fPopupDict->sbKanjiStart->Down = !fPopupDict->sbKanjiEnd->Down && ini->ReadBool(L"popupdictionary", L"kanjistart", false);
        fPopupDict->sbExamples->Down = ini->ReadBool(L"popupdictionary", L"show examples", false);
        error_code = 31;

        if (fPopupDict->sbExamples->Down)
            fPopupDict->sbExamplesClick(fDict->sbExamples);
        error_code = 32;

        n = ini->ReadInteger(L"popupdictionary", L"example mode", 0);
        switch (n)
        {
        case 0:
            fPopupDict->sbEx1->Down = true;
            fPopupDict->sbEx1->Click();
            break;
        case 1:
            fPopupDict->sbEx2->Down = true;
            fPopupDict->sbEx2->Click();
            break;
        case 2:
            fPopupDict->sbEx3->Down = true;
            fPopupDict->sbEx3->Click();
            break;
        }
        error_code = 33;

        fDict->sbInfCon->Down = ini->ReadBool(L"dictionary", L"showinflections", true);
        fPopupDict->sbInfCon->Down = ini->ReadBool(L"popupdictionary", L"showinflections", true);
        fDict->Autosize = ini->ReadBool(L"dictionary", L"autosize", false);
        fPopupDict->Autosize = fDict->Autosize; //ini->ReadBool(L"popupdictionary",L"autosize",false);
        error_code = 34;

        fRecognize->resetsize = ini->ReadBool(L"handwriting recognition", L"reset size", true);
        fRecognize->resetpos = ini->ReadBool(L"handwriting recognition", L"reset position", true);
        fRecognize->btnGrid->Down = ini->ReadBool(L"handwriting recognition", L"grid", false);
        fRecognize->btnGen->Down = ini->ReadBool(L"handwriting recognition", L"recognize general characters", true);
        fRecognize->btnGrid->Click();
        fRecognize->btnGen->Click();
        error_code = 341;

        fGroup->Pages->TabIndex = ini->ReadInteger(L"groups", L"activepageindex", 0);

        error_code = 35;
        fGroup->SelectDictionary(MIN(ini->ReadInteger(L"groups", L"selected dictionary", 0), fGroup->cbDictionary->Items->Count - 1));
        error_code = 66;

        fGroup->PagesChange(NULL);
        error_code = 36;

        bool vocabdown = ini->ReadBool(L"groups", L"vocabulary view", true);
        fGroup->sbVocab->Down = vocabdown;
        fGroup->sbStudy->Down = !vocabdown;
        fGroup->sbVocab->Click();
        error_code = 37;

        fGroup->lbWGroups->ItemIndex = ini->ReadInteger(L"groups", L"selected group", -1);
        if (fGroup->lbWGroups->ItemIndex >= 0)
            fGroup->lbWGroupsClick(fGroup->lbWGroups);
        error_code = 38;

        fGroup->cbKunGroup->Checked = ini->ReadBool(L"groups", L"group kun readings", false);
        fGroup->cbKunGroupClick(fGroup->cbKunGroup);
        error_code = 39;

        if (settings.positionrestore)
        {
            TRect dr = Screen->DesktopRect;
            int caph = GetSystemMetrics(SM_CYCAPTION);
            for (int i = 0; i < WINDOW_SIZE_COUNT; i++)
            {
                s = L"window" + IntToStr(i) + "left";
                fsi[i].left = ini->ReadInteger(L"sizes", s, 0);
                s = L"window" + IntToStr(i) + "top";
                fsi[i].top = ini->ReadInteger(L"sizes", s, 0);
                s = L"window" + IntToStr(i) + "width";
                fsi[i].width = ini->ReadInteger(L"sizes", s, 0);
                s = L"window" + IntToStr(i) + "height";
                fsi[i].height = ini->ReadInteger(L"sizes", s, 0);
                s = L"window" + IntToStr(i) + "maximized";
                fsi[i].maxed = ini->ReadBool(L"sizes", s, false);

                if (fsi[i].left > dr.Right)
                    fsi[i].left = dr.Right - fsi[i].width;
                if (fsi[i].left + fsi[i].width < dr.Left)
                    fsi[i].left = dr.Left;
                if (fsi[i].top > dr.Bottom)
                    fsi[i].top = dr.Bottom - fsi[i].height;
                if (fsi[i].top < dr.Top - caph)
                    fsi[i].top = dr.Top;
            }
            fGroup->SetSplitSize(ini->ReadInteger(L"sizes", L"example box", (Screen->DesktopHeight > 600 ? 115 : 85)));

            settings.vocabheight = ini->ReadInteger(L"sizes", L"vocab height", 359);
            settings.vocabwidth = ini->ReadInteger(L"sizes", L"vocab width", 598);

            error_code = 391;
            settings.studyeditwidth = ini->ReadInteger(L"long-term study", L"width", 785);
            settings.studyeditheight = ini->ReadInteger(L"long-term study", L"height", 560);
            fDock->splitwidth = ini->ReadInteger(L"sizes", L"kanji split width", 1);

            fDock->heightrate = IniReadDouble(ini, L"sizes", L"group split height", 0);

            settings.studyreadingwidth = ini->ReadInteger(L"sizes", L"reading study width", 0);
            settings.studyreadingheight = ini->ReadInteger(L"sizes", L"reading study height", 0);
            if (Screen->Width <= 800 && fDock->splitwidth > 300)
                fDock->splitwidth = 1;

            error_code = 392;

            fRecognize->SetDimensions(ini->ReadInteger(L"handwriting recognition", L"left", -1), ini->ReadInteger(L"handwriting recognition", L"top", -1), ini->ReadInteger(L"handwriting recognition", L"width", -1));
            error_code = 393;

            settings.filterwindowwidth = ini->ReadInteger(L"sizes", L"filter editor width", -1);
            settings.filterwindowheight = ini->ReadInteger(L"sizes", L"filter editor height", -1);

            error_code = 394;
        }
        else
        {
            fGroup->SetSplitSize(Screen->DesktopHeight > 600 ? 115 : 85);

            settings.vocabheight = 359;
            settings.vocabwidth = 598;

            settings.studyeditwidth = 785;
            settings.studyeditheight = 560;
            settings.studyeditmainsplit = 220;
            settings.studyeditwordsplit = 170;
            fDock->splitwidth = 1;
            fDock->heightrate = 0.6;
            settings.studyreadingwidth = 0;
            settings.studyreadingheight = 0;

            settings.filterwindowwidth = -1;
            settings.filterwindowheight = -1;
        }
        error_code = 40;

        fGroup->btnKCard->Down = ini->ReadBool(L"groups", L"card controls", false);
        if (fGroup->btnKCard->Down)
        {
            switch (ini->ReadInteger(L"groups", L"card page", 0))
            {
            case 0:
                fGroup->sbKWords->Down = true;
                break;
            case 1:
            default:
                fGroup->sbKDef->Down = true;
                break;
            }
        }
        fGroup->cbGroupEx->ItemIndex = ini->ReadInteger(L"groups", L"group examples", 0);
        fGroup->btnKCardClick(fGroup->btnKCard);
        fGroup->sbKWordsClick(fGroup->sbKDef);
        fGroup->cbGroupExChange(fGroup->cbGroupEx);
        error_code = 41;

        fGroup->sbFilter->Down = ini->ReadBool(L"groups", L"filter words", true);
        error_code = 411;

        fKanji->cbSortlist->ItemIndex = ini->ReadInteger(L"kanji", L"sort", 0);
        fKanji->cbSortlistChange(fKanji->cbSortlist);
        error_code = 42;

        n = kanjis->IndexOf(ini->ReadInteger(L"kanji", L"kanji", 0x4e00));
        fKanji->kanji = kanjis->Kanjis[n >= 0 ? n : kanjis->IndexOf((wchar_t)0x4e00)];
        error_code = 43;

        fKanji->sbF1->Down = ini->ReadBool(L"kanji", L"search1", true);
        fKanji->sbF2->Down = ini->ReadBool(L"kanji", L"search2", true);
        fKanji->sbF3->Down = ini->ReadBool(L"kanji", L"search3", true);
        fKanji->sbF4->Down = ini->ReadBool(L"kanji", L"search4", true);
        fKanji->sbF5->Down = ini->ReadBool(L"kanji", L"search5", true);
        fKanji->sbF6->Down = ini->ReadBool(L"kanji", L"search6", true);
        error_code = 44;

        fKanji->sbF1Click(fKanji->sbF1);
        fKanji->sbF1Click(fKanji->sbF2);
        fKanji->sbF1Click(fKanji->sbF3);
        fKanji->sbF1Click(fKanji->sbF4);
        fKanji->sbF1Click(fKanji->sbF5);
        fKanji->sbF1Click(fKanji->sbF6);
        error_code = 45;

        fKanji->btnHide->Tag = (int)ini->ReadBool(L"kanji", L"hidefilters", false);
        if (fKanji->pHide->Visible && fKanji->btnHide->Tag)
        {
            fKanji->btnHide->Tag = 0;
            fKanji->btnHide->Click();
        }
        error_code = 46;

        fKanji->cbList->ItemIndex = ini->ReadInteger(L"kanji", L"list", 0);
        fKanji->radtype = ini->ReadInteger(L"kanji", L"classical", 0);
        error_code = 47;

        if (settings.kanjirestore)
        {
            error_code = 49;
            fKanji->radicals = ini->ReadString(L"kanji", L"radicals", L"");
            fKanji->cbMeaning->Checked = ini->ReadBool(L"kanji", L"use meaning", false);
            fKanji->cbReading->Checked = ini->ReadBool(L"kanji", L"use reading", false);
            fKanji->cbStrokes->Checked = ini->ReadBool(L"kanji", L"use strokes", false);
            fKanji->cbJouyou->Checked = ini->ReadBool(L"kanji", L"use jouyou", false);
            fKanji->cbJLPT->Checked = ini->ReadBool(L"kanji", L"use JLPT", false);
            fKanji->edMeaning->Text = ini->ReadString(L"kanji", L"meaning", L"");
            fKanji->edStrokes->Text = ini->ReadString(L"kanji", L"strokes", L"");
            fKanji->edJouyou->Text = ini->ReadString(L"kanji", L"jouyou", L"");
            fKanji->edJLPT->Text = ini->ReadString(L"kanji", L"JLPT", L"");
            fKanji->cbOn->Checked = ini->ReadBool(L"kanji", L"on", true);
            fKanji->cbKun->Checked = ini->ReadBool(L"kanji", L"kun", true);
            fKanji->cbOku->Checked = ini->ReadBool(L"kanji", L"oku", true);
            error_code = 50;
            fKanji->cbSKIP->Checked = ini->ReadBool(L"kanji", L"SKIP", false);
            fKanji->cbSKIP->Tag = ini->ReadInteger(L"kanji", L"s1", 0);
            fKanji->sbS1->Down = fKanji->cbSKIP->Tag == 1;
            fKanji->sbS2->Down = fKanji->cbSKIP->Tag == 2;
            fKanji->sbS3->Down = fKanji->cbSKIP->Tag == 3;
            fKanji->sbS4->Down = fKanji->cbSKIP->Tag == 4;
            error_code = 51;
            fKanji->cb4C->Checked = ini->ReadBool(L"kanji", L"4Corner", false);
            fKanji->edC1->Text = ini->ReadString(L"kanji", L"4CornerA", L"");
            fKanji->edC2->Text = ini->ReadString(L"kanji", L"4CornerB", L"");
            fKanji->edC3->Text = ini->ReadString(L"kanji", L"4CornerC", L"");
            fKanji->edC4->Text = ini->ReadString(L"kanji", L"4CornerD", L"");
            fKanji->edC5->Text = ini->ReadString(L"kanji", L"4CornerE", L"");
            error_code = 52;
            fKanji->edS2->Text = ini->ReadString(L"kanji", L"s2", L"");
            fKanji->edS3->Text = ini->ReadString(L"kanji", L"s3", L"");
            fKanji->edreading->Text = ToKana(ini->ReadString(L"kanji", L"reading", L""), n);
            error_code = 53;
            fKanji->cbCustom->ItemIndex = ini->ReadInteger(L"kanji", L"customindex", 0);
            fKanji->edCustom->Text = ini->ReadString(L"kanji", L"custom", L"");
        }

        fInfo->sbRef->Down = ini->ReadBool(L"info", L"show reference", false);
        fInfo->sbParents->Down = ini->ReadBool(L"info", L"show parents", false);
        fInfo->sbParts->Down = ini->ReadBool(L"info", L"show parts", false);
        fInfo->sbSimilar->Down = ini->ReadBool(L"info", L"show similar", false);
        fInfo->strokeorder = ini->ReadBool(L"info", L"show stroke order", false);
        error_code = 54;

        fInfo->sbShadow->Down = ini->ReadBool(L"info", L"stroke shadow", true);
        fInfo->sbNumbers->Down = ini->ReadBool(L"info", L"stroke numbers", false);
        fInfo->sbDirection->Down = ini->ReadBool(L"info", L"stroke direction", false);
        if (fInfo->sbShadow->Down)
            fInfo->sbShadow->Click();
        if (fInfo->sbNumbers->Down)
            fInfo->sbNumbers->Click();
        if (fInfo->sbDirection->Down)
            fInfo->sbDirection->Click();
        error_code = 55;

        switch (ini->ReadInteger(L"info", L"stroke animation speed", 1))
        {
        case 0:
            fInfo->sbSpeedS1->Down = true;
            fInfo->sbSpeed1->Click();
            break;
        case 1:
            fInfo->sbSpeed2->Down = true;
            fInfo->sbSpeedS2->Down = true;
            fInfo->sbSpeed2->Click();
            break;
        case 2:
            fInfo->sbSpeed3->Down = true;
            fInfo->sbSpeedS3->Down = true;
            fInfo->sbSpeed3->Click();
            break;
        }
        fInfo->cbGrid->Checked = ini->ReadBool(L"info", L"grid", false);
        error_code = 56;

        fInfo->miRadical->Checked = ini->ReadBool(L"info", L"show radical", false);
        fInfo->miRadicalClick(fInfo->miRadical);
        error_code = 565;

        fGroup->pGroupBtn->Color = (TColor)colorsettings.background;

    }
    catch (...)
    {
        MsgBox(((UnicodeString)L"An error occurred while reading settings from main ini file! (code: " + IntToStr(error_code) + ")").c_str(), L"Error", MB_OK);
        error_code = 0;
    }
    error_code = 0;

    delete ini;
}
//---------------------------------------------------------------------------
void __fastcall SaveSettings()
{
    if (settings.positionrestore)
    {
        fClipboard->SaveFormSize();
        error_code = 32;

        fInfo->SaveFormSize();
        error_code = 33;

        fsi[2] = fClipboard->SizeInfo;
        error_code = 37;

        fsi[3] = fInfo->SizeInfo;
        error_code = 38;
    }

    fDict->SaveSettings();
    error_code = 47;

    fPopupDict->SaveSettings();
    error_code = 48;

    fGroup->SaveSettings();
    error_code = 49;

    TMemIniFile *ini = new TMemIniFile(paths.inipath + L"zkanji.ini", TEncoding::UTF8);

    bool error = false;

    try
    {
        error_code = 300;
        ini->WriteInteger(L"dictionary", L"index", settings.dictionary_index);

        error_code = 301;

        UnicodeString filtername;

        ini->EraseSection(L"word filters");

        for (int j = 0; j < wordfilters->Count; ++j)
        {
            UnicodeString fident = L"filter #" + IntToStr(j);
            ini->WriteString(L"word filters", fident + L"name", wordfilters->Names[j]);
            const TWordFilter &filter = wordfilters->Filters[j];
            ini->WriteInteger(L"word filters", fident + L"types", filter.types);
            ini->WriteInteger(L"word filters", fident + L"notes", filter.notes);
            ini->WriteInteger(L"word filters", fident + L"fields", filter.fields);
            ini->WriteInteger(L"word filters", fident + L"all types", (int)filter.typeinclusion);
            ini->WriteBool(L"word filters", fident + L"all usages", filter.allusages);
            for (int ix = 0; ix <= 5; ++ix)
                ini->WriteBool(L"word filters", fident + " " + (!ix ? L"Non-JLPT" : (L"JLPT N" + IntToStr(ix)).c_str()), filter.JLPT[ix]);
        }

        ini->EraseSection(L"dictionary filters");
        for (int j = 0; j < dictionaryfilters->Count; ++j)
        {
            ini->WriteString(L"dictionary filters", L"filter #" + IntToStr(j), dictionaryfilters->FilterName[j]);
            ini->WriteBool(L"dictionary filters", L"exclude filter #" + IntToStr(j), dictionaryfilters->Exclude[j]);
        }

        ini->EraseSection(L"kanji word filters");
        for (int j = 0; j < kanjiwordfilters->Count; ++j)
        {
            ini->WriteString(L"kanji word filters", L"filter #" + IntToStr(j), kanjiwordfilters->FilterName[j]);
            ini->WriteBool(L"kanji word filters", L"exclude filter #" + IntToStr(j), kanjiwordfilters->Exclude[j]);
        }

        ini->EraseSection(L"word group filters");
        for (int j = 0; j < wordgroupfilters->Count; ++j)
        {
            ini->WriteString(L"word group filters", L"filter #" + IntToStr(j), wordgroupfilters->FilterName[j]);
            ini->WriteBool(L"word group filters", L"exclude filter #" + IntToStr(j), wordgroupfilters->Exclude[j]);
        }

        error_code = 301;
        ini->WriteBool(L"dictionary", L"use filter", fDict->btnFilter->Down);
        ini->WriteBool(L"dictionary", L"example page selector", fDict->pExPage->Tag == 0);
        ini->WriteBool(L"dictionary", L"example type selector", fDict->pExType->Tag == 0);
        error_code = 302;

        ini->WriteInteger(L"view", L"layout", (int)fDock->layout/*settings.baselayout*/);
        //ini->WriteBool(L"view",L"docked",settings.docked);

        ini->WriteString(L"new vocabulary", L"collection", settings.vocabcollection);
        ini->WriteString(L"new vocabulary", L"group", settings.vocabgroup);

        ini->WriteString(L"kanji transfer", L"collection", settings.kanjicollection);
        ini->WriteString(L"kanji transfer", L"group", settings.kanjigroup);

        ini->WriteBool(L"fonts", L"classic antialias", settings.kanjiantialias);
        ini->WriteString(L"fonts", L"kanji", settings.kanjifont);
        ini->WriteString(L"fonts", L"kana", settings.kanafont);
        ini->WriteString(L"fonts", L"romaji", settings.romajifont);
        ini->WriteString(L"fonts", L"smallromaji", settings.smallromajifont);

        ini->WriteBool(L"userdata", L"autosave", settings.autosave);
        ini->WriteInteger(L"userdata", L"autosavedelay", settings.autosavedelay);

        ini->WriteBool(L"general", L"minimize to tray", settings.minimizetotray);

        ini->WriteBool(L"windows", L"window decoration", settings.windowdecoration);
        ini->WriteBool(L"general", L"old style popup", settings.oldstylepopup);
        ini->WriteBool(L"general", L"tool window shadow", settings.toolshadow);

        ini->WriteBool(L"exit", L"searchsave", settings.searchsave);
        ini->WriteBool(L"run", L"positionrestore", settings.positionrestore);
        ini->WriteBool(L"run", L"kanjirestore", settings.kanjirestore);

        ini->WriteBool(L"clipboard", L"showoncopy", settings.showclipcopy);
        ini->WriteBool(L"clipboard", L"showonappend", settings.showclipappend);

        ini->WriteBool(L"run", L"remembertray", settings.remembertray);
        ini->WriteBool(L"run", L"startintray", settings.startintray);
        ini->WriteBool(L"run", L"intray", settings.intray && settings.remembertray || settings.startintray);

        ini->WriteBool(L"update", L"autocheck", settings.autocheck);
        ini->WriteInteger(L"update", L"checkinterval", settings.checkinterval);
        ini->WriteInteger(L"update", L"lastcheck day", settings.lastcheck);

        ini->WriteInteger(L"info", L"reference index", settings.reference_index);
        error_code = 304;
        for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
        {
            ini->WriteInteger(L"info", (UnicodeString)L"Ref. Order" + ix, settings.reference_order[ix]);
            ini->WriteBool(L"info", (UnicodeString)L"Ref. Show" + ix, settings.referenceshow[ix]);
        }
        error_code = 305;

        ini->WriteBool(L"info", L"show non-kanji elements", settings.shownonkanjielements);
        error_code = 315;

        ini->WriteBool(L"popup", L"use filter", fPopupDict->btnFilter->Down);
        ini->WriteBool(L"popup", L"example page selector", fPopupDict->pExPage->Tag == 0);
        ini->WriteBool(L"popup", L"example type selector", fPopupDict->pExType->Tag == 0);
        error_code = 306;

        ini->WriteBool(L"run", L"columnreset", settings.columnreset);
        ini->WriteBool(L"popup", L"columnreset", settings.popupcolumnreset);
        ini->WriteBool(L"popup", L"positionrestore", settings.popuppositionrestore);
        ini->WriteBool(L"popup", L"disappear", settings.popupdisappear);
        ini->WriteBool(L"popup", L"useclipboard", settings.popupuseclipboard);
        ini->WriteBool(L"popup", L"kanjireset", settings.kanjireset);

        ini->WriteInteger(L"dictionary", L"colkanji", settings.colkanji);
        ini->WriteInteger(L"dictionary", L"colkana", settings.colkana);
        ini->WriteInteger(L"popup", L"colkanji", settings.popupcolkanji);
        ini->WriteInteger(L"popup", L"colkana", settings.popupcolkana);
        ini->WriteInteger(L"popup", L"width", settings.popupwidth);
        ini->WriteInteger(L"popup", L"height", settings.popupheight);
        ini->WriteInteger(L"popup", "floatleft", settings.popupfloatrect.Left);
        ini->WriteInteger(L"popup", "floattop", settings.popupfloatrect.Top);
        ini->WriteInteger(L"popup", "floatwidth", settings.popupfloatrect.Width());
        ini->WriteInteger(L"popup", "floatheight", settings.popupfloatrect.Height());
        ini->WriteBool(L"popup", L"floating", fPopupDict->Floating);

        ini->WriteInteger(L"study", L"colkanji", settings.studycolkanji);
        ini->WriteInteger(L"study", L"colkana", settings.studycolkana);

        error_code = 307;
        ini->WriteBool(L"clipboard", L"autotranslate", fClipboard->cbAuto->Checked);
        error_code = 308;

        ini->WriteBool(L"popup", L"syskeys", settings.syskeys);
        ini->WriteBool(L"popup", L"jshift", settings.JShift);
        ini->WriteInteger(L"popup", L"jkeys", settings.JKeys);
        ini->WriteInteger(L"popup", L"jchar", settings.JChar);
        ini->WriteBool(L"popup", L"eshift", settings.EShift);
        ini->WriteInteger(L"popup", L"ekeys", settings.EKeys);
        ini->WriteInteger(L"popup", L"echar", settings.EChar);
        ini->WriteBool(L"popup", L"kshift", settings.KShift);
        ini->WriteInteger(L"popup", L"kkeys", settings.KKeys);
        ini->WriteInteger(L"popup", L"kchar", settings.KChar);

        ini->WriteInteger(L"popup", "transparency", settings.popuptransparency);

        ini->WriteString(L"setup", L"settingspage", settings.settingspage);

        ini->WriteBool(L"dictionary", L"showinftext", settings.showinftext);
        ini->WriteBool(L"dictionary", L"showinfalways", settings.showinfalways);
        ini->WriteInteger(L"dictionary", L"alpha order", settings.alphaorder);
        ini->WriteInteger(L"dictionary", L"frequency symbols", settings.freqsymbols);

        ini->WriteBool(L"dictionary", L"showingroup", settings.showingroup);
        ini->WriteBool(L"dictionary", L"indicate JLPT", settings.showinJLPT);

        ini->WriteInteger(L"dictionary", L"size", settings.dictfontsize);
        ini->WriteInteger(L"popupdictionary", L"size", settings.popfontsize);
        error_code = 309;

        ini->WriteInteger(L"search history", L"count", settings.histcnt);
        ini->WriteBool(L"search history", L"timed save", settings.timedhistsave);
        ini->WriteInteger(L"search history", L"delay", settings.histdelay);
        error_code = 3000;

        ini->WriteBool(L"dictionary", L"autosize", fDict->Autosize);
        error_code = 310;

        ini->WriteBool(L"handwriting recognition", L"reset size", fRecognize->resetsize);
        ini->WriteBool(L"handwriting recognition", L"reset position", fRecognize->resetpos);
        error_code = 311;
        ini->WriteBool(L"handwriting recognition", L"recognize general characters", fRecognize->btnGen->Down);
        ini->WriteBool(L"handwriting recognition", L"grid", fRecognize->btnGrid->Down);
        error_code = 312;

        ini->WriteInteger(L"dictionary", L"mode", (fDict->pKanji->Visible ? 0 : fDict->pRomaji->Visible ? 1 : 2));
        error_code = 313;
        ini->WriteBool(L"dictionary", L"romajiend", fDict->sbRomajiEnd->Down);
        ini->WriteBool(L"dictionary", L"kanjistart", fDict->sbKanjiStart->Down);
        ini->WriteBool(L"dictionary", L"kanjiend", fDict->sbKanjiEnd->Down);
        ini->WriteBool(L"dictionary", L"show examples", fDict->sbExamples->Down);
        ini->WriteBool(L"popupdictionary", L"romajiend", fPopupDict->sbRomajiEnd->Down);
        ini->WriteBool(L"popupdictionary", L"kanjistart", fPopupDict->sbKanjiStart->Down);
        ini->WriteBool(L"popupdictionary", L"kanjiend", fPopupDict->sbKanjiEnd->Down);
        ini->WriteBool(L"popupdictionary", L"show examples", fPopupDict->sbExamples->Down);
        error_code = 314;

        int n = (fDict->sbEx1->Down ? 0 : fDict->sbEx2->Down ? 1 : 2);
        ini->WriteInteger(L"dictionary", L"example mode", n);
        n = (fPopupDict->sbEx1->Down ? 0 : fPopupDict->sbEx2->Down ? 1 : 2);
        ini->WriteInteger(L"popupdictionary", L"example mode", n);
        error_code = 315;

        ini->WriteBool(L"dictionary", L"showinflections", fDict->sbInfCon->Down);
        error_code = 316;
        ini->WriteBool(L"popupdictionary", L"showinflections", fPopupDict->sbInfCon->Down);
        error_code = 317;

        ini->WriteInteger(L"groups", L"activepageindex", fGroup->Pages->TabIndex);
        ini->WriteInteger(L"groups", L"selected dictionary", fGroup->cbDictionary->ItemIndex);
        ini->WriteBool(L"groups", L"vocabulary view", fGroup->sbVocab->Down);
        ini->WriteInteger(L"groups", L"selected group", fGroup->lbWGroups->ItemIndex);

        ini->WriteBool(L"groups", L"use kanji word filter", fGroup->btnKFilter->Down);
        ini->WriteBool(L"groups", L"use word group filter", fGroup->btnWFilter->Down);

        ini->WriteBool(L"groups", L"group kun readings", fGroup->cbKunGroup->Checked);
        error_code = 318;

        UnicodeString s;
        //int h1;
        //int h2;
        //__int64 hr;
        //double dhr;
        if (settings.positionrestore)
        {
            for (int i = 0; i < WINDOW_SIZE_COUNT; i++)
            {
                s = L"window" + IntToStr(i) + "left";
                ini->WriteInteger(L"sizes", s, fsi[i].left);
                s = L"window" + IntToStr(i) + "top";
                ini->WriteInteger(L"sizes", s, fsi[i].top);
                s = L"window" + IntToStr(i) + "width";
                ini->WriteInteger(L"sizes", s, fsi[i].width);
                s = L"window" + IntToStr(i) + "height";
                ini->WriteInteger(L"sizes", s, fsi[i].height);
                s = L"window" + IntToStr(i) + "maximized";
                ini->WriteBool(L"sizes", s, fsi[i].maxed);
            }
            error_code = 319;
            ini->WriteInteger(L"sizes", L"example box", fGroup->pbKWords->Height);

            ini->WriteInteger(L"sizes", L"vocab height", settings.vocabheight);
            ini->WriteInteger(L"sizes", L"vocab width", settings.vocabwidth);

            ini->WriteInteger(L"long-term study", L"width", settings.studyeditwidth);
            ini->WriteInteger(L"long-term study", L"height", settings.studyeditheight);

            ini->WriteInteger(L"sizes", L"kanji split width", fDock->splitwidth);

            IniWriteDouble(ini, L"sizes", L"group split height", fDock->heightrate);
            //dhr = fDock->heightrate;
            //hr = *((__int64*) &dhr);
            //h1 = hr & 0xFFFFFFFF;
            //h2 = (hr >> 32) & 0xFFFFFFFF;
            //ini->WriteInteger(L"sizes", L"group split height part1", h1);
            //ini->WriteInteger(L"sizes", L"group split height part2", h2);

            ini->WriteInteger(L"sizes", L"reading study width", settings.studyreadingwidth);
            ini->WriteInteger(L"sizes", L"reading study height", settings.studyreadingheight);

            error_code = 320;
            ini->WriteInteger(L"handwriting recognition", L"left", fRecognize->SavedLeft);
            ini->WriteInteger(L"handwriting recognition", L"top", fRecognize->SavedTop);
            ini->WriteInteger(L"handwriting recognition", L"width", fRecognize->Width);

            error_code = 3201;

            ini->WriteInteger(L"sizes", L"filter editor width", settings.filterwindowwidth);
            ini->WriteInteger(L"sizes", L"filter editor height", settings.filterwindowheight);

            error_code = 3202;
        }
        error_code = 321;

        ini->WriteBool(L"info", L"show reference", fInfo->sbRef->Down);
        ini->WriteBool(L"info", L"show parents", fInfo->sbParents->Down);
        ini->WriteBool(L"info", L"show parts", fInfo->sbParts->Down);
        ini->WriteBool(L"info", L"show similar", fInfo->sbSimilar->Down);
        ini->WriteBool(L"info", L"show stroke order", fInfo->strokeorder);
        error_code = 322;

        ini->WriteBool(L"info", L"stroke shadow", fInfo->sbShadow->Down);
        ini->WriteBool(L"info", L"stroke numbers", fInfo->sbNumbers->Down);
        ini->WriteBool(L"info", L"stroke direction", fInfo->sbDirection->Down);
        ini->WriteInteger(L"info", L"stroke animation speed", (fInfo->sbSpeed1->Down ? 0 : fInfo->sbSpeed2->Down ? 1 : 2));
        error_code = 323;

        ini->WriteBool(L"info", L"grid", fInfo->cbGrid->Checked);
        error_code = 324;
        ini->WriteBool(L"info", L"show radical", fInfo->miRadical->Checked);
        error_code = 325;

        ini->WriteBool(L"groups", L"card controls", fGroup->btnKCard->Down);
        ini->WriteInteger(L"groups", L"card page", fGroup->sbKWords->Down ? 0 : fGroup->sbKDef->Down ? 1 : 2);
        ini->WriteInteger(L"groups", L"group examples", fGroup->cbGroupEx->ItemIndex);
        error_code = 326;

        ini->WriteBool(L"groups", L"filter words", fGroup->sbFilter->Down);
        error_code = 327;

        ini->WriteBool(L"kanji", L"hidefilters", (bool)fKanji->btnHide->Tag);
        ini->WriteInteger(L"kanji", L"sort", fKanji->cbSortlist->ItemIndex);
        ini->WriteInteger(L"kanji", L"kanji", (fKanji->kanji ? fKanji->kanji->ch : -1));

        ini->WriteInteger(L"kanji", L"list", fKanji->cbList->ItemIndex);
        error_code = 328;

        ini->WriteBool(L"kanji", L"search1", fKanji->sbF1->Down);
        ini->WriteBool(L"kanji", L"search2", fKanji->sbF2->Down);
        ini->WriteBool(L"kanji", L"search3", fKanji->sbF3->Down);
        ini->WriteBool(L"kanji", L"search4", fKanji->sbF4->Down);
        ini->WriteBool(L"kanji", L"search5", fKanji->sbF5->Down);
        ini->WriteBool(L"kanji", L"search6", fKanji->sbF6->Down);
        error_code = 329;

        ini->WriteInteger(L"kanji", L"classical", fKanji->radtype);
        error_code = 330;
        ini->WriteBool(L"kanji", L"classic2 group", settings.classic2group);
        ini->WriteBool(L"kanji", L"classic2 exact", settings.classic2exact);
        ini->WriteBool(L"kanji", L"classic2 hide names", settings.classic2hidenames);

        error_code = 331;
        if (settings.kanjirestore)
        {

            ini->WriteBool(L"kanji", L"use meaning", fKanji->cbMeaning->Checked);
            ini->WriteBool(L"kanji", L"use reading", fKanji->cbReading->Checked);
            ini->WriteBool(L"kanji", L"use strokes", fKanji->cbStrokes->Checked);
            ini->WriteBool(L"kanji", L"use jouyou", fKanji->cbJouyou->Checked);
            ini->WriteBool(L"kanji", L"use JLPT", fKanji->cbJLPT->Checked);

            ini->WriteString(L"kanji", L"radicals", fKanji->radicals);
            ini->WriteString(L"kanji", L"meaning", fKanji->edMeaning->Text);
            ini->WriteString(L"kanji", L"strokes", fKanji->edStrokes->Text);
            ini->WriteString(L"kanji", L"jouyou", fKanji->edJouyou->Text);
            ini->WriteString(L"kanji", L"JLPT", fKanji->edJLPT->Text);
            ini->WriteBool(L"kanji", L"on", fKanji->cbOn->Checked);
            ini->WriteBool(L"kanji", L"kun", fKanji->cbKun->Checked);
            ini->WriteBool(L"kanji", L"oku", fKanji->cbOku->Checked);

            ini->WriteBool(L"kanji", L"SKIP", fKanji->cbSKIP->Checked);
            ini->WriteInteger(L"kanji", L"s1", fKanji->cbSKIP->Tag);
            ini->WriteString(L"kanji", L"s2", fKanji->edS2->Text);
            ini->WriteString(L"kanji", L"s3", fKanji->edS3->Text);
            error_code = 332;

            ini->WriteString(L"kanji", L"reading", Romanize(fKanji->edreading->Text.c_str()));
            error_code = 333;

            ini->WriteBool(L"kanji", L"4Corner", fKanji->cb4C->Checked);
            ini->WriteString(L"kanji", L"4CornerA", fKanji->edC1->Text);
            ini->WriteString(L"kanji", L"4CornerB", fKanji->edC2->Text);
            ini->WriteString(L"kanji", L"4CornerC", fKanji->edC3->Text);
            ini->WriteString(L"kanji", L"4CornerD", fKanji->edC4->Text);
            ini->WriteString(L"kanji", L"4CornerE", fKanji->edC5->Text);

            ini->WriteInteger(L"kanji", L"customindex", fKanji->cbCustom->ItemIndex);
            ini->WriteString(L"kanji", L"custom", fKanji->edCustom->Text);
        }
        error_code = 334;

        ini->WriteInteger(L"wordstudy", L"M1Num", settings.M1Num);
        ini->WriteInteger(L"wordstudy", L"M1Order", settings.M1Order);
        ini->WriteBool(L"wordstudy", L"M1Rand", settings.M1Rand);
        ini->WriteInteger(L"wordstudy", L"M1Add", settings.M1Add);
        ini->WriteInteger(L"wordstudy", L"M1Selection", settings.M1Selection);
        ini->WriteBool(L"wordstudy", L"M1Global", settings.M1Global);

        ini->WriteInteger(L"wordstudy", L"stat deflation days", settings.statdays);

        ini->WriteInteger(L"wordstudy", L"M2Num", settings.M2Num);
        ini->WriteInteger(L"wordstudy", L"M2Inc", settings.M2Inc);
        ini->WriteInteger(L"wordstudy", L"M0Question", settings.M0Question); // Added 2013 07 04 for v0.731.
        ini->WriteInteger(L"wordstudy", L"M0Answer", settings.M0Answer); // Added 2013 07 04 for v0.731.

        ini->WriteInteger(L"wordstudy", L"M0Timer", settings.M0Timer);
        ini->WriteBool(L"wordstudy", L"M0HideKana", settings.M0HideKana);
        ini->WriteBool(L"wordstudy", L"compactreview", settings.compactreview);

        //ini->WriteInteger(L"windows",L"topwindow",settings.topwindow);

        ini->WriteString(L"general", L"export folder", settings.exportdir);
        ini->WriteInteger(L"general", L"date format", (int)settings.datefmt);

        ini->WriteBool(L"word list generation", L"save", settings.save_gen);
        ini->WriteInteger(L"word list generation", L"source", settings.gen_src);
        ini->WriteInteger(L"word list generation", L"destination", settings.gen_dest);
        ini->WriteBool(L"word list generation", L"link dictionaries", settings.gen_linked);
        ini->WriteString(L"word list generation", L"dest group", settings.gen_group);
        ini->WriteInteger(L"word list generation", L"kana length", settings.gen_len);
        ini->WriteInteger(L"word list generation", L"min frequency", settings.gen_pop);
        ini->WriteInteger(L"word list generation", L"kanji position", settings.gen_pos);
        ini->WriteBool(L"word list generation", L"only from list", settings.gen_only);
        ini->WriteInteger(L"word list generation", L"max grade", settings.gen_grad);
        ini->WriteInteger(L"word list generation", L"max kanji count", settings.gen_num);

        ini->WriteBool(L"vocabulary printing", L"dictionary fonts", settings.vdictfonts);
        ini->WriteString(L"vocabulary printing", L"kana", settings.vkanafont);
        ini->WriteString(L"vocabulary printing", L"romaji", settings.vromajifont);
        ini->WriteInteger(L"vocabulary printing", L"row height", settings.vheight);
        ini->WriteInteger(L"vocabulary printing", L"left", settings.vleft);
        ini->WriteInteger(L"vocabulary printing", L"right", settings.vright);
        ini->WriteBool(L"vocabulary printing", L"separate", settings.vseparate);
        ini->WriteBool(L"vocabulary printing", L"lines", settings.vlines);
        ini->WriteBool(L"vocabulary printing", L"shade", settings.vshade);
        ini->WriteBool(L"vocabulary printing", L"double", settings.vdouble);
        ini->WriteBool(L"vocabulary printing", L"number pages", settings.vnumber);
        ini->WriteBool(L"vocabulary printing", L"furigana", settings.vfurigana);

        ini->WriteInteger(L"vocabulary printing", L"printer index", settings.printerindex);
        ini->WriteBool(L"vocabulary printing", L"orientation portrait", settings.vportrait);
        ini->WriteBool(L"vocabulary printing", L"print to file", settings.vtofile);

        ini->WriteBool(L"kanji", L"show kanji hint", settings.show_kanji_hint);
        ini->WriteBool(L"kanji", L"hide kanji hint", settings.hide_kanji_hint);
        ini->WriteInteger(L"kanji", L"kanji hint time", settings.kanji_hint_time);

        ini->EraseSection("kanjistudy");

        ini->WriteInteger(L"long-term study", L"day start", profilesettings.daystart);
        ini->WriteInteger(L"long-term study", L"warn time", (int)profilesettings.kwarntime);
        ini->WriteInteger(L"long-term study", L"include interval", profilesettings.includeinterval);
        ini->WriteInteger(L"long-term study", L"include count", profilesettings.kiteminclude);
        ini->WriteInteger(L"long-term study", L"kanji warn threshold", profilesettings.kwarnthres);

        //remove after 2011. Jun. or sooner!
        ini->DeleteKey("long-term study", L"date format");

        ini->WriteInteger(L"long-term study", L"kanji repetition interval", profilesettings.krepetition);
        ini->WriteBool(L"long-term study", L"label repetition interval", profilesettings.klabelinterval);
        ini->WriteBool(L"long-term study", L"new items first", profilesettings.knewfirst);

        ini->WriteInteger(L"long-term study", L"kanji tested readings", profilesettings.ktestreadings);
        ini->WriteInteger(L"long-term study", L"kanji reading words", profilesettings.kreadingwords);

        ini->WriteBool(L"long-term study", L"warn items", profilesettings.kwarnitems);
        ini->WriteInteger(L"long-term study", L"warn items count", profilesettings.kwarnitemscount);

        error_code = 335;
        //float kltmp = profilesettings.klastwarning;
        //ini->WriteInteger(L"long-term study", L"last warning", *(int*)&kltmp);
        IniWriteDouble(ini, L"long-term study", L"last warning", profilesettings.klastwarning);
        error_code = 336;

        ini->WriteBool(L"long-term study", L"warn after", profilesettings.kwarnafter);
        ini->WriteInteger(L"long-term study", L"warn after length", profilesettings.kwarnaftertime);
        ini->WriteBool(L"long-term study", L"type kanji", profilesettings.ktypekanji);
        ini->WriteBool(L"long-term study", L"type kana", profilesettings.ktypekana);
        ini->WriteBool(L"long-term study", L"type meaning", profilesettings.ktypemeaning);

        ini->WriteInteger(L"long-term study", L"deck limit hard", profilesettings.kdecklimithard);
        ini->WriteInteger(L"long-term study", L"deck limit retry", profilesettings.kdecklimitretry);

        ini->WriteInteger(L"long-term study", L"suspend after", profilesettings.ksuspendafter);

        ini->WriteInteger(L"long-term study", L"hint for kanji", profilesettings.kkanjihint);
        ini->WriteInteger(L"long-term study", L"hint for kana", profilesettings.kkanahint);
        ini->WriteInteger(L"long-term study", L"hint for meaning", profilesettings.kmeaninghint);

        ini->WriteInteger(L"colors", L"main text", colorsettings.maintext);
        ini->WriteInteger(L"colors", L"kana usage", colorsettings.kanausage);
        ini->WriteInteger(L"colors", L"background", colorsettings.background);
        ini->WriteInteger(L"colors", L"highlight", colorsettings.highlight);
        ini->WriteInteger(L"colors", L"grid line", colorsettings.gridline);
        ini->WriteInteger(L"colors", L"word inf/conj bg.", colorsettings.wordinf);
        ini->WriteInteger(L"colors", L"word type", colorsettings.wordtype);
        ini->WriteInteger(L"colors", L"word usage", colorsettings.wordusage);
        ini->WriteInteger(L"colors", L"word field", colorsettings.wordfield);
        ini->WriteInteger(L"colors", L"kanji no words", colorsettings.kanjinoword);
        ini->WriteInteger(L"colors", L"unsorted kanji", colorsettings.kanjiunsorted);
        ini->WriteInteger(L"colors", L"word frequency popular", colorsettings.wordpop);
        ini->WriteInteger(L"colors", L"word frequency average", colorsettings.wordavg);
        ini->WriteInteger(L"colors", L"word frequency unpopular", colorsettings.wordunpop);
        ini->WriteInteger(L"colors", L"N1 color", colorsettings.N[0]);
        ini->WriteInteger(L"colors", L"N2 color", colorsettings.N[1]);
        ini->WriteInteger(L"colors", L"N3 color", colorsettings.N[2]);
        ini->WriteInteger(L"colors", L"N4 color", colorsettings.N[3]);
        ini->WriteInteger(L"colors", L"N5 color", colorsettings.N[4]);

        ini->WriteBool(L"long-term study", L"open last tab", settings.ltopentab);
        ini->WriteBool(L"long-term study", L"restore sizes", settings.ltrestoresizes);
        ini->WriteInteger(L"long-term study", L"active tab", settings.lttab);
        ini->WriteInteger(L"long-term study", L"active list", settings.ltlist);
        ini->WriteBool(L"long-term study", L"show last test", settings.ltshowlast);

        ini->WriteInteger(L"long-term study", L"item stat", settings.ltitemstat);
        ini->WriteInteger(L"long-term study", L"global stat", settings.ltglobalstat);
        ini->WriteInteger(L"long-term study", L"item stat size", settings.ltitemstatsize);
        ini->WriteBool(L"long-term study", L"show item stat", settings.ltshowitemstat);

        ini->WriteInteger(L"long-term study", L"free visible columns", settings.ltfreevisible);
        ini->WriteInteger(L"long-term study", L"locked visible columns", settings.ltlockedvisible);
        ini->WriteInteger(L"long-term study", L"tested visible columns", settings.lttestedvisible);
        error_code = 337;
        for (int ix = 0; ix < FREE_HEADING_COUNT; ix++)
            ini->WriteInteger(L"long-term study", (UnicodeString)L"free column size" + ix, settings.ltrestoresizes ? -1 : settings.ltfreesizes[ix]);
        for (int ix = 0; ix < LOCK_HEADING_COUNT; ix++)
            ini->WriteInteger(L"long-term study", (UnicodeString)L"locked column size" + ix, settings.ltrestoresizes ? -1 : settings.ltlockedsizes[ix]);
        for (int ix = 0; ix < TEST_HEADING_COUNT; ix++)
            ini->WriteInteger(L"long-term study", (UnicodeString)L"tested column size" + ix, settings.ltrestoresizes ? -1 : settings.lttestedsizes[ix]);
        error_code = 338;
        ini->WriteInteger(L"long-term study", L"free order column", settings.ltfreeordercolumn);
        ini->WriteInteger(L"long-term study", L"locked order column", settings.ltlockedordercolumn);
        ini->WriteInteger(L"long-term study", L"tested order column", settings.lttestedordercolumn);

        ini->WriteBool(L"long-term study", L"sod numbers", settings.ltsodnumber);

        ini->WriteInteger(L"long-term word import", L"width", settings.ltimportwidth);
        ini->WriteInteger(L"long-term word import", L"height", settings.ltimportheight);

        ini->WriteInteger(L"example sentences", L"cache size", settings.example_cache);

        error_code = 339;
        ini->EraseSection(L"dictionary sites");
        error_code = 340;

        ini->WriteInteger(L"dictionary sites", L"count", sitelist->Count);
        error_code = 341;
        for (int ix = 0; ix < sitelist->Count; ++ix)
        {
            ini->WriteString(L"dictionary sites", (UnicodeString)L"name" + (ix + 1), sitelist->Items[ix]->name);
            ini->WriteString(L"dictionary sites", (UnicodeString)L"address" + (ix + 1), sitelist->Items[ix]->address);
            ini->WriteInteger(L"dictionary sites", (UnicodeString)L"insert" + (ix + 1), sitelist->Items[ix]->pos);
        }
        error_code = 342;

        ini->WriteBool(L"backup", L"create", settings.backupdata);
        ini->WriteInteger(L"backup", L"count", settings.bckcount);
        ini->WriteInteger(L"backup", L"spacing", settings.bckspacing);
        ini->WriteString(L"backup", L"path", settings.bckpath);
        //IniWriteDouble(ini, L"backup", L"last", settings.lastbackup);

        ini->UpdateFile();
    }
    catch (...)
    {
        error = true;
    }

    delete ini;

    if (!error && settings.kanatests)
    {
        error_code = 350;
        ini = new TMemIniFile(paths.inipath + "kanatest.ini", TEncoding::UTF8);
        try
        {

            error_code = 351;
            int size = settings.bitshira.arr_size(sizeof(int));
            int *k = new int[size];
            settings.bitshira.arr_copy(k, sizeof(int));
            error_code = 352;

            ini->WriteInteger(L"Hiragana", L"size", size);
            for (int i = 0; i < size; i++)
                ini->WriteInteger(L"Hiragana", (UnicodeString)L"set" + i, k[i]);
            error_code = 353;

            delete[] k;

            size = settings.bitskata.arr_size(sizeof(int));
            k = new int[size];
            settings.bitskata.arr_copy(k, sizeof(int));
            error_code = 354;

            ini->WriteInteger(L"Katakana", L"size", size);
            for (int i = 0; i < size; i++)
                ini->WriteInteger(L"Katakana", (UnicodeString)L"set" + i, k[i]);
            error_code = 355;

            delete[] k;
            error_code = 356;

            ini->UpdateFile();
        }
        catch (...)
        {
            error = true;
        }
        delete ini;
    }

    if (error)
        THROW(L"Error occurred while saving settings.");
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbDictKeyClick(TObject *Sender)
{
    for (int i = 0; i < pJShortcut->ControlCount; i++)
        pJShortcut->Controls[i]->Enabled = cbDictKey->Checked;
    for (int i = 0; i < pEShortcut->ControlCount; i++)
        pEShortcut->Controls[i]->Enabled = cbDictKey->Checked;
    for (int i = 0; i < pKShortcut->ControlCount; i++)
        pKShortcut->Controls[i]->Enabled = cbDictKey->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbAutoSaveClick(TObject *Sender)
{
    edAutoSave->Enabled = cbAutoSave->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbStartInTrayClick(TObject *Sender)
{
    cbRememberTray->Enabled = !cbStartInTray->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbKWarnAfterClick(TObject *Sender)
{
    edKWarnAfter->Enabled = cbKWarnAfter->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::FormPaint(TObject *Sender)
{
    DrawCloseBar(this, bvBottom, NULL);
    DrawTabBar(Canvas, Rect(0, 0, ClientWidth, 22));
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbVDictFontsClick(TObject *Sender)
{
    cbVKana->Enabled = !cbVDictFonts->Checked;
    cbVRomaji->Enabled = !cbVDictFonts->Checked;
    pbVText->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::pbVTextPaint(TObject *Sender)
{
    TCanvas *c = pbVText->Canvas;
    SetTextAlign(c->Handle, TA_BASELINE | TA_LEFT);

    TComboBox *cb = cbVDictFonts->Checked ? cbKana : cbVKana;

    int rix = RealIndex(cb);
    if (rix >= 0)
    {
        c->Font->Name = cb->Items->Strings[rix];
        c->Font->Charset = (int)jfonts->Objects[rix];
    }
    else
        SetSystemFont(c->Font);

    c->Font->Size = double(pbVText->ClientHeight) * 0.30 / scaleres;
    TRect r = Rect(0, 0, pbVText->ClientWidth, pbVText->ClientHeight / 2);
    HFONT newfont = create_localfont(c->Font);
    HFONT oldfont = (HFONT)SelectObject(c->Handle, newfont);
    ExtTextOutW(c->Handle, 2, pbVText->ClientHeight / 2*0.8, ETO_CLIPPED | ETO_OPAQUE, &r, L"かなカナ漢字感じ幹事", 10, 0);
    SelectObject(c->Handle, oldfont);
    DeleteObject(newfont);

    cb = cbVDictFonts->Checked ? cbRomaji : cbVRomaji;
    rix = RealIndex(cb);
    if (rix >= 0)
        c->Font->Name = cb->Items->Strings[rix];
    else
        c->Font->Name = L"Tahoma";
    c->Font->Charset = DEFAULT_CHARSET;
    r = Rect(0, pbVText->ClientHeight / 2, pbVText->ClientWidth, pbVText->ClientHeight);
    newfont = create_localfont(c->Font);
    oldfont = (HFONT)SelectObject(c->Handle, newfont);
    ExtTextOutW(c->Handle, 2, pbVText->ClientHeight / 2 + pbVText->ClientHeight / 2*0.8, ETO_CLIPPED | ETO_OPAQUE, &r, L"This is a sample text", 21, 0);
    SelectObject(c->Handle, oldfont);
    DeleteObject(newfont);
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbVKanaChange(TObject *Sender)
{
    pbVText->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbVRightChange(TObject *Sender)
{
    cbVSeparate->Enabled = (cbVRight->ItemIndex != (int)viNothing);
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbKanjiHintTimeClick(TObject *Sender)
{
    edKanjiHintTime->Enabled = cbKanjiHintTime->Checked && cbKanjiHintTime->Enabled;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbKanjiHintClick(TObject *Sender)
{
    cbKanjiHintTime->Enabled = cbKanjiHint->Checked;
    cbKanjiHintTimeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbKWarnItemsClick(TObject *Sender)
{
    edKWarnItems->Enabled = cbKWarnItems->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbWarnTimeChange(TObject *Sender)
{
    lbKWarnThres->Enabled = cbWarnTime->ItemIndex;
    edKWarnThres->Enabled = cbWarnTime->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::btnRestoreClick(TObject *Sender)
{
    cbText->Selected = clWindowText;
    cbKnO->Selected = (TColor)0xa0b090;
    cbBg->Selected = clWindow;
    cbHl->Selected = (TColor)0xe0e0e0;
    cbGrid->Selected = (TColor)0xcacaca;
    cbInfCon->Selected = (TColor)0xeaffff;
    cbTp->Selected = (TColor)0x308040;
    cbUsg->Selected = (TColor)0x204090;
    cbFld->Selected = (TColor)0x804030;
    cbNoWd->Selected = clSilver;
    cbUns->Selected = (TColor)0xf5fdfd;
    cbN1->Selected = (TColor)0x00000000;
    cbN2->Selected = (TColor)0x00C84D91;
    cbN3->Selected = (TColor)0x0099A007;
    cbN4->Selected = (TColor)0x000E7BB6;
    cbN5->Selected = (TColor)0x002222FF;
    freq1 = (TColor)0x0000a0f0;
    freq2 = (TColor)0x00d0d0d0;
    freq3 = (TColor)0x000000f0;
    pbFreq1->Invalidate();
    pbFreq2->Invalidate();
    pbFreq3->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbFreqDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State)
{
    UnicodeString f1, f2, f3;
    UnicodeString fname;
    switch (Index)
    {
    case 0:
        f1 = 'q';
        f2 = '4';
        f3 = 'r';
        fname = L"Webdings";
        break;
    case 1:
        f1 = 'q';
        f2 = 'q';
        f3 = 'q';
        fname = L"Webdings";
        break;
    case 2:
        f1 = 'J';
        f2 = 'K';
        f3 = 'L';
        fname = L"Wingdings";
        break;
    };

    TCanvas *c = ((TComboBox*)Control)->Canvas;
    if (State.Contains(odFocused) || State.Contains(odSelected))
    {
        c->Brush->Color = cbHl->Selected;
    }
    else
    {
        c->Brush->Color = cbBg->Selected;
    }
    c->FillRect(Rect);
    c->Font->Size = double(Rect.Height()) * 0.65 / scaleres;
    //TSize s;
    int x = Rect.Left + 1, y = Rect.Top + Rect.Height() * 0.76;
    SetTextAlign(c->Handle, TA_LEFT | TA_BASELINE);
    for (int ix = 0; ix < 3; ++ix)
    {
        UnicodeString str = (ix == 0 ? f1 : ix == 1 ? f2 : f3);
        c->Font->Color = ix == 0 ? freq1 : ix == 1 ? freq2 : freq3;
        c->Font->Name = fname;

        ExtTextOutW(c->Handle, x, y, ETO_CLIPPED, &Rect, str.c_str(), 1, NULL);
        x += 2 + Rect.Height() - 3;

        if (ix < 2)
        {
            c->Font->Color = cbGrid->Selected;
            c->Font->Name = L"Tahoma";
            ExtTextOutW(c->Handle, x, y, ETO_CLIPPED, &Rect, L"|", 1, NULL);
            x += Rect.Height() - 9;
        }
    }
    c->Font->Color = clWindowText;
    c->Brush->Color = clWindow;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::pbFreq1Paint(TObject *Sender)
{
    TZPaintBox *box = (TZPaintBox*)Sender;
    UnicodeString f1, f2, f3;
    UnicodeString fname;
    switch (cbFreq->ItemIndex)
    {
    case 0:
        f1 = 'q';
        f2 = '4';
        f3 = 'r';
        fname = L"Webdings";
        break;
    case 1:
        f1 = 'q';
        f2 = 'q';
        f3 = 'q';
        fname = L"Webdings";
        break;
    case 2:
        f1 = 'J';
        f2 = 'K';
        f3 = 'L';
        fname = L"Wingdings";
        break;
    };

    TCanvas *c = box->Canvas;
    TColor col = box->Tag == 1 ? freq1 : box->Tag == 2 ? freq2 : freq3;
    UnicodeString str = box->Tag == 1 ? f1 : box->Tag == 2 ? f2 : f3;

    SetTextAlign(c->Handle, TA_CENTER | TA_BASELINE);
    c->Font->Size = double(box->ClientHeight) * 0.65 / scaleres;
    TRect r = box->ClientRect;
    c->Font->Color = col;
    c->Brush->Color = cbBg->Selected;
    c->Font->Name = fname;

    ExtTextOut(c->Handle, r.Width() / 2, r.Height()*0.76, ETO_CLIPPED | ETO_OPAQUE, &r, str.c_str(), 1, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbBgChange(TObject *Sender)
{
    pbFreq1->Invalidate();
    pbFreq2->Invalidate();
    pbFreq3->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::btnFreq1Click(TObject *Sender)
{
    int tag = ((TBitBtn*)Sender)->Tag;
    dColor->CustomColors->Clear();
    dColor->CustomColors->Add((UnicodeString)L"ColorA=" + IntToHex(freq1, 6));
    dColor->CustomColors->Add((UnicodeString)L"ColorB=" + IntToHex(freq2, 6));
    dColor->CustomColors->Add((UnicodeString)L"ColorC=" + IntToHex(freq3, 6));
    switch (tag)
    {
    case 1:
        dColor->Color = freq1;
        break;
    case 2:
        dColor->Color = freq2;
        break;
    case 3:
        dColor->Color = freq3;
        break;
    }


    if (ExecuteDialog(dColor))
    {
        switch (tag)
        {
        case 1:
            freq1 = dColor->Color;
            pbFreq1->Invalidate();
            break;
        case 2:
            freq2 = dColor->Color;
            pbFreq2->Invalidate();
            break;
        case 3:
            freq3 = dColor->Color;
            pbFreq3->Invalidate();
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::dColorShow(TObject *Sender)
{
    data->ModalBegin(NULL, ((TCommonDialog*)Sender)->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::dColorClose(TObject *Sender)
{
    data->ModalEnd(((TCommonDialog*)Sender)->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbKanjiAntialiasClick(TObject *Sender)
{
    if (Kfont)
        DeleteObject(Kfont);
    Kfont = 0;

    pbKanji->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::btnManageSitesClick(TObject *Sender)
{
    fSites = new TfSites(Application);
    try
    {
        if (fSites->ShowModal() == mrOk)
            SetSiteText();
    }
    catch (...)
    {
        ;
    }
    delete fSites;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::SetSiteText()
{
    if (tmpsitelist->Count == 0)
    {
        lbSite1->Caption = L"There are";
        lbSiteCnt->Caption = L"no";
        lbSite2->Caption = L"sites added.";
    }
    else if (tmpsitelist->Count == 1)
    {
        lbSite1->Caption = L"There is";
        lbSiteCnt->Caption = L"1   ";
        lbSite2->Caption = L"site added.";
    }
    else
    {
        lbSite1->Caption = L"There are";
        lbSiteCnt->Caption = tmpsitelist->Count;
        lbSite2->Caption = L"sites added.";
    }
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbKRefClick(TObject *Sender)
{
    sbKRefUp->Enabled = cbKRef->ItemIndex > 0;
    sbKRefDown->Enabled = cbKRef->ItemIndex < cbKRef->Items->Count - 1;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::sbKRefUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    MouseRepeat(sbKRefUpMouseDown, Sender, Button, Shift, x, y);

    int iix = cbInfo->ItemIndex;

    int dif = ((TSpeedButton*)Sender)->Tag;
    int newix = cbKRef->ItemIndex + dif;
    cbInfo->Items->Move(cbKRef->ItemIndex, newix);
    if (iix == cbKRef->ItemIndex)
        cbInfo->ItemIndex = newix;
    else if (iix == newix)
        cbInfo->ItemIndex = cbKRef->ItemIndex;
    cbKRef->Items->Move(cbKRef->ItemIndex, newix);
    cbKRef->ItemIndex = newix;

    sbKRefUp->Enabled = newix > 0;
    sbKRefDown->Enabled = newix < cbKRef->Items->Count - 1;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::sbKRefAllClick(TObject *Sender)
{
    cbKRef->Items->BeginUpdate();
    for (int ix = 0; ix < cbKRef->Items->Count; ++ix)
        cbKRef->Checked[ix] = ((TSpeedButton*)Sender)->Tag == 0;
    cbKRef->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::btnKRefClick(TObject *Sender)
{
    byte tmpref[KANJI_REFERENCE_COUNT];
    memcpy(tmpref, settings.reference_order, sizeof(byte)*KANJI_REFERENCE_COUNT);

    int iix = settings.reference_order[cbInfo->ItemIndex];
    for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
        settings.reference_order[ix] = ix;
    FillKRef();
    cbInfo->ItemIndex = iix;

    memcpy(settings.reference_order, tmpref, sizeof(byte)*KANJI_REFERENCE_COUNT);
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::FillKRef()
{
    cbInfo->Items->BeginUpdate();
    cbKRef->Items->BeginUpdate();
    cbInfo->Items->Clear();
    cbKRef->Items->Clear();
    for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
    {
        cbInfo->Items->Add(refstr[settings.reference_order[ix]]);
        cbKRef->Items->Add(refstr[settings.reference_order[ix]]);
        cbKRef->Items->Objects[ix] = (TObject*)settings.reference_order[ix];
        cbKRef->Checked[ix] = settings.referenceshow[settings.reference_order[ix]];
    }
    cbInfo->Items->EndUpdate();
    cbKRef->Items->EndUpdate();
    cbKRef->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbKReadingsChange(TObject *Sender)
{
    cbKReadingWords->Enabled = cbKReadings->ItemIndex != 0;
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::btnBckResetClick(TObject *Sender)
{
    edBckPath->Text = L"<>\\data";
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::cbBckCreateClick(TObject *Sender)
{
    int cnt = dictionary->BackupCount();
    lbBck->Visible = (!cbBckCreate->Checked && cnt) || (cbBckCreate->Checked && cnt > numBckCount->value);
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::edBckPathExit(TObject *Sender)
{
    for (int ix = edBckPath->Text.Length(); ix != 0; --ix)
        if (edBckPath->Text[ix] == L'/')
            edBckPath->Text[ix] = L'\\';
}
//---------------------------------------------------------------------------
void __fastcall TfSettings::btnBckBrowseClick(TObject *Sender)
{
    UnicodeString path = FullPathFromBackupPath(edBckPath->Text);
    if (BrowseForFolder(L"Choose root folder for backups", path, true, true, true, false, false, this))
    {
        if (path.Pos(L"<>") != 1 && path.LowerCase().Pos(paths.inipath.LowerCase()) == 1)
            path = L"<>" + path.SubString(paths.inipath.Length(), path.Length() - paths.inipath.Length() + 1);
        edBckPath->Text = path;
    }
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(145);
#endif





