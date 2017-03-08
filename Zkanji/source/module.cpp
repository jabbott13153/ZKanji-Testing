//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

//#include <FileCtrl.hpp>
#include <StdActns.hpp>

#include "module.h"
#include "main.h"
#include "popupdict.h"
#include "settings.h"
#include "vocabform.h"
#include "kanjiinfo.h"
#include "clipview.h"
#include "wordlist.h"
#include "grouplist.h"
#include "kanji.h"
#include "collection.h"
#include "wordobj.h"
#include "words.h"
#include "zmenu.h"
#include "kanjihint.h"
#include "kanjilist.h"
#include "modallist.h"
#include "kanjiedit.h"
#include "docker.h"
#include "stats.h"
#include "elementedit.h"
#include "writetest.h"
#include "sitemanager.h"
#include "update.h"
#include "jlpteditor.h"
#include "wordfilter.h"
#include "listimport.h"
#include "zkformats.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(106);
#endif

Tdata *data;
int ignore_esc_char = 0;

#define WM_Z_TRAYMSG   (WM_APP+99)

/*
 TWordListObjBase *popupobj = NULL;
 TKanji *popkanji = NULL;
 UnicodeString poptext;
 byte popcollection = 0;
 bool popwholeword;
 */

struct zappevent;
struct zsettingsevent;
struct zwordgroupevent;
extern TTemplateList<zappevent, true> *appevents;
extern TTemplateList<zsettingsevent, true> *settingevents;
extern TTemplateList<zwordgroupevent, true> *wordgroupevents;

TKanjiEdit *popedit;

UnicodeString msgboxtext = "__NOMESSAGEBOX__";

//---------------------------------------------------------------------------
__fastcall Tdata::Tdata(TComponent* Owner) : TDataModule(Owner)
{
    myprocessid = GetCurrentProcessId();

    fminimized = false;
    ftrayhandle = AllocateHWnd(TrayWnd);

    editrows = new TZMenuRows(1);
    TZMenuRow *row = editrows->AddRow();
    row->Titles[0] = L"Edit filters...";
    row->Types[0] = itSimple;
    row->Callbacks[0] = &wordFiltersEdit;

    filterrows = NULL;
    siterows = new TZMenuRows(1);

    // Build popup menu rows
    firstrows = new TZMenuRows(3);
    TZMenuRow* firstrow = firstrows->AddRow();

    kfirstrows = new TZMenuRows(3);
    TZMenuRow* kfirstrow = kfirstrows->AddRow();

    secondrows = new TZMenuRows(3);
    TZMenuRow* secondrow = secondrows->AddRow();

    thirdrows = new TZMenuRows(3);
    TZMenuRow* thirdrow = thirdrows->AddRow();

    kextrarows = new TZMenuRows(3);
    TZMenuRow* kextrarow = kextrarows->AddRow();

    kextrauserrows = new TZMenuRows(3);
    TZMenuRow* kextrauserrow = kextrauserrows->AddRow();

    wextrarows = new TZMenuRows(3);
    TZMenuRow* wextrarow = wextrarows->AddRow();

    selrows = new TZMenuRows(3);
    TZMenuRow* selrow = selrows->AddRow();

    wextrauserrows = new TZMenuRows(3);
    TZMenuRow* wextrauserrow = wextrauserrows->AddRow();

    wextrauserrows2 = new TZMenuRows(3);
    TZMenuRow* wextrauserrow2 = wextrauserrows2->AddRow();

    pwunion = new TZMenuRows(3);

    TZMenuItemType types[] = { itTitle, itSimple, itSimple };
    TZMenuItemType types2[] = { itTitle, itEmpty, itSimple };

    TZMenuItemType typesalt[] = { itAltTitle, itSimple, itSimple };
    TZMenuItemType typesalt2[] = { itAltTitle, itSimple, itEmpty };

    wchar_t *firstnames[] = { L"?", L"To Vocab...", L"Translate" };
    TZPopupCommand firstcmd[] = { pcNone, pcToVocab, pcTranslate };
    firstrow->Setup(firstnames, types, (void**)firstcmd);

    wchar_t *firstnameskanji[] = { L"Kanji", L"Kanji Info", L"Translate" };
    TZPopupCommand kfirstcmd[] = { pcNone, pcKanjiInfo, pcTranslate };
    kfirstrow->Setup(firstnameskanji, types, (void**)kfirstcmd);

    wchar_t *secondnames[] = { L"?", L"Copy", L"Append" };
    TZPopupCommand secondcmd[] = { pcNone, pcCopy1, pcAppend1 };
    secondrow->Setup(secondnames, typesalt, (void**)secondcmd);

    wchar_t *thirdnames[] = { L"Clpb: Kanji", L"Copy", L"Append" };
    TZPopupCommand thirdcmd[] = { pcNone, pcCopy2, pcAppend2 };
    thirdrow->Setup(thirdnames, typesalt, (void**)thirdcmd);

    wchar_t *kextranames[] = { L"Kanji", L"To Group...", L"Collect Words..." };
    TZPopupCommand kextracmd[] = { pcNone, pcToGroup, pcCollect };
    kextrarow->Setup(kextranames, types, (void**)kextracmd);

    wchar_t *kextrausernames[] = { L"Kanji Def.", L"Edit...", L"" };
    TZPopupCommand kextrausercmd[] = { pcNone, pcEditKanjiDef, pcNone };
    kextrauserrow->Setup(kextrausernames, typesalt2, (void**)kextrausercmd);

    wchar_t *wextranames[] = { L"Word", L"To Long-term...", L"Search Online" };
    TZPopupCommand wextracmd[] = { pcNone, pcToLongTerm, pcNone };
    wextrarow->Setup(wextranames, types, (void**)wextracmd);

    wextrarow->Subrows[2] = siterows;

    wchar_t *selnames[] = { L"Selected Text", L"", L"Search Online" };
    TZPopupCommand selcmd[] = { pcNone, pcNone, pcNone };
    selrow->Setup(selnames, types2, (void**)selcmd);

    selrow->Subrows[2] = siterows;


    wchar_t *wextrausernames[] = { L"Word Entry", L"Edit...", L"Delete..." };
    TZPopupCommand wextrausercmd[] = { pcNone, pcEditEntry, pcDeleteEntry };
    wextrauserrow->Setup(wextrausernames, typesalt, (void**)wextrausercmd);

    wchar_t *wextrausernames2[] = { L"Word Entry", L"Edit...", L"Revert..." };
    TZPopupCommand wextrausercmd2[] = { pcNone, pcEditEntry, pcRevertEntry };
    wextrauserrow2->Setup(wextrausernames2, typesalt, (void**)wextrausercmd2);

    pwunion->AppendRows(firstrows, false);
    pwunion->AppendRows(kfirstrows, false);
    pwunion->AppendRows(secondrows, false);
    pwunion->AppendRows(thirdrows, false);
    pwunion->AppendRows(kextrarows);
    pwunion->AppendRows(kextrauserrows, false);
    pwunion->AppendRows(wextrarows);
    pwunion->AppendRows(selrows);
    pwunion->AppendRows(wextrauserrows);
    pwunion->AppendRows(wextrauserrows2);

}
//---------------------------------------------------------------------------
__fastcall Tdata::~Tdata()
{
    delete firstrows;
    delete kfirstrows;
    delete secondrows;
    delete thirdrows;
    delete kextrarows;
    delete kextrauserrows;
    delete wextrarows;
    delete selrows;
    delete wextrauserrows;
    delete wextrauserrows2;
    delete pwunion;

    delete siterows;

    delete editrows;
    delete filterrows;

    if (fminimized)
        RemoveIcon();
    DeallocateHWnd(ftrayhandle);
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aExitExecute(TObject *Sender)
{
    fPopupDict->Popup(false);
    RemoveIcon();
    if (fKanji->ontop)
        fKanji->Popup(false);
    PostMessage(fMain->Handle, WM_SYSCOMMAND, SC_CLOSE, 0);
}
//---------------------------------------------------------------------------
void __fastcall Tdata::miRestoreClick(TObject *Sender)
{
    Restore();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::TrayWnd(TMessage &msg)
{
    if (msg.Msg == WM_Z_TRAYMSG)
    {
        ReleaseCapture();
        if (modallist->Count && (msg.LParam == WM_LBUTTONUP || msg.LParam == WM_RBUTTONUP))
            SetForegroundWindow(modallist->Items[modallist->Count - 1]->Handle);
        else if (msg.LParam == WM_RBUTTONUP)
        {
            TPoint p;
            GetCursorPos(&p);
            pmTray->WindowHandle = ftrayhandle;
            SetFocus(ftrayhandle);
            pmTray->Popup(p.x, p.y);
        }
        else if (msg.LParam == WM_LBUTTONUP/*WM_LBUTTONDBLCLK*/)
        {
            Restore();
        }
    }
    else
        msg.Result = DefWindowProc(ftrayhandle, msg.Msg, msg.WParam, msg.LParam);
}
//---------------------------------------------------------------------------
void Tdata::AddIcon()
{
    if (!ftrayhandle || !settings.minimizetotray)
        return;

    ::_NOTIFYICONDATAW d;
    d.cbSize = sizeof(TNotifyIconData);
    d.hWnd = ftrayhandle;
    d.uID = 100;
    d.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    d.uCallbackMessage = WM_Z_TRAYMSG;
    d.hIcon = fDict->Icon->Handle;
    wcscpy(d.szTip, L"zkanji - Right-click for instant dictionary!");
    Shell_NotifyIconW(NIM_ADD, &d);
}
//---------------------------------------------------------------------------
void Tdata::RemoveIcon()
{
    if (!ftrayhandle || !settings.minimizetotray)
        return;

    ::_NOTIFYICONDATAW d;
    d.cbSize = sizeof(TNotifyIconData);
    d.hWnd = ftrayhandle;
    d.uID = 100;
    d.uFlags = NIF_MESSAGE;
    d.uCallbackMessage = WM_Z_TRAYMSG;
    d.hIcon = 0;
    d.szTip[0] = 0;
    Shell_NotifyIconW(NIM_DELETE, &d);
}
//---------------------------------------------------------------------------
void Tdata::Minimize()
{
    if (fminimized)
        return;
    fminimized = true;

    AddIcon();
}
//---------------------------------------------------------------------------
void Tdata::Restore()
{
    if (fminimized && !modallist->Count)
    {
        fPopupDict->Popup(false);
        RemoveIcon();

        if (fKanji->ontop)
            fKanji->Popup(false);

        fMain->Restore();
        fminimized = false;
    }

    Application->BringToFront();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::miToJapClick(TObject *Sender)
{
    fPopupDict->Popup(true, dmRomaji);
}
//---------------------------------------------------------------------------
void __fastcall Tdata::miFromJapClick(TObject *Sender)
{
    fPopupDict->Popup(true, dmKanji);
}
//---------------------------------------------------------------------------
struct zappevent
{
    TCPPNotifyEvent event;
    void *caller;
    bool activate;
};
void AppActivateEvent(void *caller, TCPPNotifyEvent fprc, bool activate, bool add)
{
    static int cnt = 0;
    if (add)
    {
        cnt++;
        zappevent *e = new zappevent;
        e->activate = activate;
        e->event = fprc;
        e->caller = caller;
        appevents->Add(e);
    }
    else
    {
        for (int ix = 0; ix < appevents->Count; ix++)
        {
            zappevent *e = appevents->Items[ix];
            if (e->event == fprc && e->activate == activate && e->caller == caller)
            {
                cnt--;
                //delete e;
                appevents->Delete(ix);
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
struct zsettingsevent
{
    TSettingsProc event;
    void *caller;
};
void SettingsChangeEvent(void *caller, TSettingsProc fprc, bool add)
{
    if (add)
    {
        if (!settingevents)
            settingevents = new TTemplateList<zsettingsevent, true>;
        zsettingsevent *e = new zsettingsevent;
        e->event = fprc;
        e->caller = caller;
        settingevents->Add(e);
    }
    else
    {
        if (!settingevents)
            return;
        for (int ix = 0; ix < settingevents->Count; ++ix)
        {
            zsettingsevent *e = settingevents->Items[ix];
            if (e->event == fprc && e->caller == caller)
            {
                settingevents->Delete(ix);
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
struct zwordgroupevent
{
    TWordGroupFuncPtr event;
    void *caller;
};
void WordGroupEvent(void *caller, TWordGroupFuncPtr fprc)
{
    zwordgroupevent *e = NULL;
    int ix = 0;
    for (; ix < wordgroupevents->Count; ++ix)
    {
        if (wordgroupevents->Items[ix]->caller == caller)
        {
            e = wordgroupevents->Items[ix];
            break;
        }
    }

    if (fprc)
    {
        if (!e)
        {
            e = new zwordgroupevent;
            e->caller = caller;
            wordgroupevents->Add(e);
        }
        e->event = fprc;
    }
    else if (e)
        wordgroupevents->Delete(ix);
}
//---------------------------------------------------------------------------
void WordGroupChanged(TWordCollection *coll, TWord *w)
{
    if (!wordgroupevents)
        return;
    for (int ix = 0; ix < wordgroupevents->Count; ++ix)
    {
        zwordgroupevent *e = wordgroupevents->Items[ix];
        e->event(e->caller, coll, w);
    }
}
//---------------------------------------------------------------------------
void SettingsChange(TSettingsProcState state)
{
    if (!settingevents)
        return;
    error_code = 0;
    for (int ix = 0; ix < settingevents->Count; ++ix)
    {
        zsettingsevent *e = settingevents->Items[ix];
        e->event(e->caller, state);
    }

}
//---------------------------------------------------------------------------
void SettingsChangeBegin()
{
    SettingsChange(spsBegin);
}
//---------------------------------------------------------------------------
void SettingsChangeEnd()
{
    SettingsChange(spsEnd);
}
//---------------------------------------------------------------------------
void __fastcall Tdata::appEventsActivate(TObject *Sender)
{
    if (!activelist || !appevents)
        return;
    for (int ix = 0; ix < appevents->Count; ++ix)
    {
#ifdef _DEBUG
        try
        {
#endif
            zappevent *e = appevents->Items[ix];
            if (e->activate)
                e->event(e->caller);
#ifdef _DEBUG
        }
        catch (...)
        {
            throw;
        }
#endif
    }
}
//---------------------------------------------------------------------------
void __fastcall Tdata::appEventsDeactivate(TObject *Sender)
{
    if (!activelist || AppActive)
        return;

    HideKanjiHint();

    try
    {
        if (appevents)
            for (int ix = 0; ix < appevents->Count; ++ix)
            {
                zappevent *e = appevents->Items[ix];
                if (!e->activate)
                    e->event(e->caller);
            }
    }
    catch (...)
    {
        ;
    }

    if (settings.popupdisappear && fPopupDict->Visible && !fPopupDict->Pinned)
    {
        if (fAddVocab)
            return;
        fPopupDict->Popup(false);
    }

    if (settings.intray)
    {
        if (fInfo->Visible && !fKanji->Visible)
            fInfo->Close();
        else if (fKanji->Visible && !modallist->Count)
        {
            /*HDWP hd = BeginDeferWindowPos(2);
             try {
             hd = DeferWindowPos(hd,fKanji->Handle,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
             if (fInfo->Visible)
             hd = DeferWindowPos(hd,fInfo->Handle,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
             } catch(...) {
            ;
             }
             if (hd)
             EndDeferWindowPos(hd);
             */
        }
        if (fClipboard->Visible)
            fClipboard->Close();

    }
    else
    {
        ;
    }
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aAboutExecute(TObject *Sender)
{
    fMain->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aSettingsExecute(TObject *Sender)
{
    ShowSettings();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aClipboardExecute(TObject *Sender)
{
    ShowClipboard();
}
//---------------------------------------------------------------------------
void Tdata::popupwordcallback(TZMenuRows *rows, int col, int row)
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
        case pcToVocab:
            fGroup->AddVocabulary(popcol, popwordindex);
            break;
        case pcKanjiInfo:
            ShowKanjiInfo(popcolindex, popkanji);
            break;
        case pcTranslate:
            Translate(poptranslate);
            break;
        case pcCopy1:
            ClipReplace(clip1);
            break;
        case pcAppend1:
            ClipAppend(clip1);
            break;
        case pcCopy2:
            ClipReplace(clip2);
            break;
        case pcAppend2:
            ClipAppend(clip2);
            break;
        case pcToGroup:
            kl = new TKanjiItemList(false);
            kl->Add(popkanjistr);
            KanjiToGroup(kl);
            break;
        case pcCollect:
            kl = new TKanjiItemList(false);
            kl->Add(popkanjistr);
            GenerateWordGroup(kl);
            break;
        case pcEditKanjiDef:
            ChangeDef(popcolindex, popkanji->ch, 0);
            break;
        case pcToLongTerm:
            wl = new TWordIndexList(popcol);
            wl->Add(popwordindex);
            ListImport(wl);
            break;
        case pcEditEntry:
            EditWordDef(popcol, popwordindex, popwordmeaning);
            break;
        case pcDeleteEntry:
            DeleteWordEntry(popcolindex, popwordindex, popwordmeaning);
            break;
        case pcRevertEntry:
            RevertWordEntry(popwordindex);
            break;
        case pcSearchOnline:
            LaunchSiteSearch(popsearchonline, row, sitelist);
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
void Tdata::PopupWord(int x, int y, TWordListObjBase *wobj, TWordColumnType wcc, TWord *w, TKanji *k)
{
    TZMenuRow* firstrow = firstrows->Rows[0];
    TZMenuRow* kfirstrow = kfirstrows->Rows[0];
    TZMenuRow* secondrow = secondrows->Rows[0];
    TZMenuRow* thirdrow = thirdrows->Rows[0];
    TZMenuRow* kextrarow = kextrarows->Rows[0];
    TZMenuRow* kextrauserrow = kextrauserrows->Rows[0];
    TZMenuRow* wextrarow = wextrarows->Rows[0];
    TZMenuRow* selrow = selrows->Rows[0];
    TZMenuRow* wextrauserrow = wextrauserrows->Rows[0];
    TZMenuRow* wextrauserrow2 = wextrauserrows2->Rows[0];

    bool sel = !wobj->selecting == ((int)wcslen(wcc == wcKanji ? w->kanji : w->kana) == wobj->selstring.Length());

    if (!k && !sel) // Not single kanji, nor selecting.
    {
        switch (wcc)
        {
        case wcKanji:
            firstrow->Titles[0] = L"Written";
            secondrow->Titles[0] = L"Clpb: Written";
            firstrow->Enabled[2] = true;
            clip1 = w->kanji;
            popsearchonline = w->kanji;
            break;
        case wcKana:
            firstrow->Titles[0] = L"Kana";
            secondrow->Titles[0] = L"Clpb: Kana";
            firstrow->Enabled[2] = true;
            clip1 = w->kana;
            popsearchonline = w->kana;
            break;
        default:
            secondrow->Titles[0] = L"Clpb: Meaning";
            firstrow->Titles[0] = L"Word";
            firstrow->Enabled[2] = false;
            clip1 = wobj->meaningtext[wobj->pos];
            popsearchonline = (w->data[0].notes & WN_KANAONLY) ? w->kana : w->kanji;
            break;
        }
        firstrow->Visible = true;
        kfirstrow->Visible = false;
        secondrow->Visible = (wcc == wcMeaning || wcc == wcKanji || wcc == wcKana);
        thirdrow->Visible = false;
        kextrarow->Visible = has_kanji(w->kanji);
        kextrarow->Titles[0] = L"Word's Kanji";
        kextrauserrow->Visible = false;

        selrow->Visible = false;
        wextrarow->Visible = true;
    }
    else if (sel) // Selection.
    {
        clip1 = wobj->selstring;
        popsearchonline = clip1;

        if (!k)
        {
            firstrow->Visible = true;
            firstrow->Titles[0] = L"Selection";
            firstrow->Enabled[2] = true;
            kfirstrow->Visible = false;
        }
        else
        {
            firstrow->Visible = false;
            kfirstrow->Visible = true;
        }
        secondrow->Titles[0] = L"Clpb: Sel.";
        secondrow->Visible = true;
        thirdrow->Visible = false;
        kextrarow->Visible = has_kanji(wobj->selstring.c_str());
        kextrarow->Titles[0] = L"Selected Kanji";
        kextrauserrow->Visible = false;

        wextrarow->Visible = false;
        selrow->Visible = sitelist->Count > 0;
    }
    else // Single kanji.
    {
        clip1 = w->kanji;
        popsearchonline = clip1;
        clip2 = k->ch;

        firstrow->Visible = false;
        kfirstrow->Visible = true;
        secondrow->Titles[0] = L"Clpb: Written";
        secondrow->Visible = true;
        thirdrow->Visible = true;
        kextrarow->Visible = true;
        kextrarow->Titles[0] = L"Single Kanji";
        kextrauserrow->Visible = wobj->collection != dictionary->Items[0];

        selrow->Visible = false;
        wextrarow->Visible = true;
    }

    if (wextrarow->Visible)
        wextrarow->Types[2] = sitelist->Count > 0 ? itSimple : itEmpty;
    if (selrow->Visible)
        selrow->Types[2] = sitelist->Count > 0 ? itSimple : itEmpty;

    wextrauserrow2->Visible = /*(wextrarow->Visible || selrow->Visible) &&*/ wobj->collection == dictionary->Items[0] && dictionary->Originals->IsModified(w) && wobj->meaningindexes[wobj->pos] == -1;

    wextrauserrow->Visible = !wextrauserrow2->Visible /*&& (wextrarow->Visible || selrow->Visible)*/;
    wextrauserrow->Enabled[2] = wobj->collection != dictionary->Items[0] || dictionary->Originals->IsAdded(w);

    siterows->Clear();
    for (int ix = 0; ix < sitelist->Count; ++ix)
    {
        TZMenuRow *r = siterows->AddRow();
        r->Titles[0] = sitelist->Items[ix]->name;
        r->Types[0] = itSimple;
        r->Data[0] = (void*)pcSearchOnline;
    }

    popcol = wobj->collection;
    popcolindex = dictionary->Index[wobj->collection];
    popkanji = k;
    popwordindex = wobj->current;
    popwordmeaning = wobj->meaningindexes[wobj->pos];

    if (sel && wcc == wcKanji)
    {
        popkanjistr = clip1;
        poptranslate = wobj->selstring;
    }
    else if (k)
    {
        popkanjistr = clip2;
        poptranslate = w->kanji;
    }
    else
    {
        popkanjistr = UnicodeString(w->kanji);
        poptranslate = clip1;
    }

    menu->Popup(pwunion, x, y, &popupwordcallback);
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aJtoEExecute(TObject *Sender)
{
    fDict->SetFocus();
    if (!fDict->btnKanji->Down)
        fDict->btnKanji->Click();
    fDict->edKanji->SetFocus();
    fDict->edkanji->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aEtoJExecute(TObject *Sender)
{
    fDict->SetFocus();
    if (!fDict->btnRomaji->Down)
        fDict->btnRomaji->Click();
    fDict->edRomaji->SetFocus();
    fDict->edRomaji->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aJExecute(TObject *Sender)
{
    fDict->SetFocus();
    if (!fDict->btnJapanese->Down)
        fDict->btnJapanese->Click();
    fDict->edJapanese->SetFocus();
    fDict->edjapanese->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::miESep1MeasureItem(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height)
{
    Height = 4;
}
//---------------------------------------------------------------------------
void __fastcall Tdata::miESep1DrawItem(TObject *Sender, TCanvas *ACanvas, TRect &ARect, bool Selected)
{
    ACanvas->Brush->Color = clMenu;
    ACanvas->FillRect(Rect(ARect.left, ARect.top, ARect.right, ARect.Bottom));
    ACanvas->Pen->Color = (TColor)0xe0e0e0;
    ACanvas->MoveTo(ARect.left, ARect.Bottom - 1);
    ACanvas->LineTo(ARect.right, ARect.Bottom - 1);
}
//---------------------------------------------------------------------------
void __fastcall Tdata::miKanjiClick(TObject *Sender)
{
    fKanji->Popup(!fKanji->ontop);
}
//---------------------------------------------------------------------------
void __fastcall Tdata::appEventsSettingChange(TObject *Sender, int Flag, const UnicodeString Section, int &Result)
{
    appEvents->OnActivate(Sender);
}
//---------------------------------------------------------------------------
void Tdata::ModalBegin(TBaseForm *form, const HWND hwnd)
{
    HideKanjiHint();
    if (modallist)
        modallist->Add(form, hwnd);
    if (Screen->ActiveForm && Screen->ActiveForm->Visible)
        SendMessage(Screen->ActiveForm->Handle, CM_DEACTIVATE, 0, 0);

    PostMessage(fMain->Handle, WM_Z_MODALBEGIN, 0, 0);
}
//---------------------------------------------------------------------------
void Tdata::ModalEnd(const HWND hwnd)
{
    if (settings.intray && fPopupDict && fKanji && fInfo && fClipboard && modallist && modallist->Count == 1)
    {
        //SetWindowPos(fPopupDict->Handle,HWND_TOPMOST,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        fPopupDict->OnTop = true;
        if (fKanji->Visible)
            SetWindowPos(fKanji->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        //fInfo->OnTop = true;
        //fClipboard->OnTop = true;
    }

    if (modallist)
        modallist->Delete(hwnd);
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aKanaPracticeExecute(TObject *Sender)
{
    KanaPractice();
}
//---------------------------------------------------------------------------
void Tdata::PopupKanji(int x, int y, TKanjiEdit *eobj, wchar_t kanji)
{
    popedit = eobj;

    UnicodeString sel = eobj->SelText;

    miCut->Enabled = sel.Length();
    miCopy->Enabled = sel.Length();
    miDelete->Enabled = sel.Length();

    miInfo->Visible = (int)kanji != 0 && modallist->Count == 0;
    if (miInfo->Visible)
    {
        miInfo->Tag = (int)kanji;
        pmKanji->Tag = eobj->Collection;
    }

    if (sitelist->Count && eobj->Text.Length() && modallist->Count == 0)
    {
        miSites->Visible = true;
        while (miSites->Count)
        {
            TMenuItem *mtmp = miSites->Items[0];
            miSites->Delete(0);
            delete mtmp;
        }
        for (int ix = 0; ix < sitelist->Count; ++ix)
        {
            TMenuItem *mi = new TMenuItem(miSites);
            mi->Caption = sitelist->Items[ix]->name;
            mi->Tag = ix;
            mi->OnClick = siteClick;
            SiteSearchString = eobj->Text;
            miSites->Add(mi);
        }
    }

    pmKanji->Popup(x, y);
}
//---------------------------------------------------------------------------
void __fastcall Tdata::siteClick(TObject *Sender)
{
    TMenuItem *mi = ((TMenuItem*)Sender);
    LaunchSiteSearch(SiteSearchString, mi->Tag, sitelist);
}
//---------------------------------------------------------------------------
void __fastcall Tdata::miInfoClick(TObject *Sender)
{
    ShowKanjiInfo(((TMenuItem*)Sender)->GetParentMenu()->Tag, kanjis->Kanjis[kanjis->IndexOf((wchar_t)((TMenuItem*)Sender)->Tag)]);
}
//---------------------------------------------------------------------------
void __fastcall Tdata::miCutClick(TObject *Sender)
{
    popedit->Copy();
    popedit->Delete();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::miCopyClick(TObject *Sender)
{
    popedit->Copy();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::miPasteClick(TObject *Sender)
{
    popedit->Paste();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::miDeleteClick(TObject *Sender)
{
    popedit->Delete();
}
//---------------------------------------------------------------------------

void __fastcall Tdata::miSelectAllClick(TObject *Sender)
{
    popedit->SelectAll();
}
//---------------------------------------------------------------------------
int MsgBox(UnicodeString message, UnicodeString caption, int buttons)
{
    msgboxtext = caption;
    data->ModalBegin(NULL, NULL);
    int r = 0;
    try
    {
        r = Application->MessageBox(message.c_str(), caption.c_str(), buttons);
    }
    catch (...)
    {
        ;
    }
    data->ModalEnd(NULL);
    msgboxtext = "__NOMESSAGEBOX__";
    return r;
}
//---------------------------------------------------------------------------
bool QueryBox(UnicodeString message, UnicodeString caption, UnicodeString &str)
{
    msgboxtext = caption;
    data->ModalBegin(NULL, NULL);
    bool r = false;
    try
    {
        r = InputQuery(caption, message, str);
    }
    catch (...)
    {
        ;
    }
    data->ModalEnd(NULL);
    msgboxtext = "__NOMESSAGEBOX__";
    return r;
}
//---------------------------------------------------------------------------
bool ExecuteDialog(TCommonDialog *dlg)
{
    data->ModalBegin(NULL, NULL);
    bool r = false;
    try
    {
        r = dlg->Execute();
    }
    catch (...)
    {
        ;
    }
    data->ModalEnd(NULL);
    return r;
}
//---------------------------------------------------------------------------
void __fastcall Tdata::dlgFolderCallback(TBrowseForFolder *Sender, HWND Wnd, DWORD uMsg, int lParam, int lpData, int &Result)
{
    ;
}
//---------------------------------------------------------------------------
void __fastcall Tdata::dlgFolderAccept(TObject *Sender)
{
    bool *b = (bool*)data->dlgFolder->Tag;
    *b = true;
}
//---------------------------------------------------------------------------
void __fastcall Tdata::dlgFolderCancel(TObject *Sender)
{
    bool *b = (bool*)data->dlgFolder->Tag;
    *b = false;
}
//---------------------------------------------------------------------------
bool BrowseForFolder(UnicodeString caption, UnicodeString &path, bool newfolderbtn, bool folderedit, bool showshares, bool showfiles, bool validatedir, TControl *parent)
{
    data->dlgFolder->Folder = path;
    data->dlgFolder->DialogCaption = caption;
    data->dlgFolder->RootDir = UnicodeString();
    data->dlgFolder->BrowseOptions = TBrowseOptions() << bifNewDialogStyle << bifReturnFSAncestors << bifReturnOnlyFSDirs;
    if (!newfolderbtn)
        data->dlgFolder->BrowseOptions << bifNoNewFolderButton;
    if (folderedit)
        data->dlgFolder->BrowseOptions << bifEditBox;
    if (showshares)
        data->dlgFolder->BrowseOptions << bifShareable;
    if (showfiles)
        data->dlgFolder->BrowseOptions << bifBrowseIncludeFiles;
    if (folderedit && validatedir)
        data->dlgFolder->BrowseOptions << bifValidate;

    msgboxtext = caption;
    data->ModalBegin(NULL, NULL);
    bool r = false;
    try
    {
        data->dlgFolder->Tag = (int)&r;
        data->dlgFolder->Execute();
        if (r)
            path = data->dlgFolder->Folder;
    }
    catch (...)
    {
        ;
    }
    data->ModalEnd(NULL);
    msgboxtext = "__NOMESSAGEBOX__";

    return r;
}
//---------------------------------------------------------------------------
BOOL __stdcall enum_FindMessageBoxHandle(HWND hwnd, long param)
{
    HWND *h = (HWND*)param;
    wchar_t s[256];
    GetWindowTextW(hwnd, s, 255);
    if (msgboxtext == s)
    {
        *h = hwnd;
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
HWND MessageBoxHandle()
{
    HWND h = 0;
    EnumThreadWindows(GetCurrentThreadId(), &enum_FindMessageBoxHandle, (long)&h);

    return h;
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aStatsExecute(TObject *Sender)
{
    fStats = new TfStats(Application);
    fStats->ShowModalEx(/*!settings.docked && fGroup->Active || settings.docked &&*/fDock->ActiveControl && fDock->ActiveControl->Owner == fGroup ? fGroup->cbDictionary->ItemIndex : settings.dictionary_index);
    fStats = NULL; // Form deletes itself. Don't delete here or thread handling will be broken.
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aStrokeEditExecute(TObject *Sender)
{
    TKanji *tmp = fInfo->kanji;
    fInfo->kanji = NULL;
    TKanjiItemList *il = NULL;
    try
    {
        bool av = elements->Count > 0;
        fEditElem = new TfEditElem(Application);
        try
        {
            il = new TKanjiItemList();
            fKanji->FillKanjiList(il, true);

            for (int ix = 0; ix < il->Count; ++ix)
                if (il->Kanjis[ix]->element < 0)
                    fEditElem->AddUNICODE(il->Kanjis[ix]);

            fEditElem->ShowModalEx(il->Count == 1 ? max(0, il->Kanjis[0]->element) : 0);
        }
        catch (...)
        {
            ;
        }
        delete fEditElem;
        fEditElem = 0;

        if (av != (elements->Count > 0))
            fInfo->ElementsChanged(elements->Count);
    }
    catch (...)
    {
        ;
    }
    delete il;
    fInfo->kanji = tmp;
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aShowHideExExecute(TObject *Sender)
{
    if (!fDict->sbExamples->Enabled)
        return;

    fDict->sbExamples->Down = !fDict->sbExamples->Down;
    fDict->sbExamples->Click();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aFilterWordsExecute(TObject *Sender)
{
    if (!fDict->btnFilter->Enabled)
        return;

    fDict->btnFilter->Down = !fDict->btnFilter->Down;
    fDict->btnFilter->Click();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aInfConjExecute(TObject *Sender)
{
    if (!fDict->pKanji->Visible || !fDict->sbInfCon->Enabled)
        return;

    fDict->sbInfCon->Down = !fDict->sbInfCon->Down;
    fDict->sbInfCon->Click();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aWordStartExecute(TObject *Sender)
{
    if (!fDict->pKanji->Visible || !fDict->sbKanjiStart->Enabled)
        return;
    fDict->sbKanjiStart->Down = !fDict->sbKanjiStart->Down;
    fDict->sbKanjiStart->Click();
}
//---------------------------------------------------------------------------

void __fastcall Tdata::aWordEndExecute(TObject *Sender)
{
    if (fDict->pKanji->Visible && fDict->sbKanjiEnd->Enabled)
    {
        fDict->sbKanjiEnd->Down = !fDict->sbKanjiEnd->Down;
        fDict->sbKanjiEnd->Click();
    }
    else if (fDict->pRomaji->Visible && fDict->sbRomajiEnd->Enabled)
    {
        fDict->sbRomajiEnd->Down = !fDict->sbRomajiEnd->Down;
        fDict->sbRomajiEnd->Click();
    }
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aBetaExecute(TObject *Sender)
{
    if (!fKanji->kanji || fKanji->kanji->element < 0)
        return;
    fWriter = new TfWriter(Application);
    try
    {
        fWriter->ShowModalEx(fKanji->kanji->element,
#ifdef _DEBUG
            true
#else
            false
#endif
                );
    }
    catch (...)
    {
        ;
    }
    delete fWriter;
    fWriter = NULL;
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aUpdateExecute(TObject *Sender)
{
    CheckForUpdates();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aStudyListExecute(TObject *Sender)
{
    fGroup->btnKEditStudy->Click();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aJLPTEditExecute(TObject *Sender)
{
    frmJLPTEdit = NULL;
    try
    {
        frmJLPTEdit = new TfrmJLPTEdit(Application);
        frmJLPTEdit->ShowModal();
    }
    catch (...)
    {
        ;
    }

    frmJLPTEdit->Free();
}
//---------------------------------------------------------------------------
void __fastcall Tdata::aEditFiltersExecute(TObject *Sender)
{
    SetFilter();
}
//---------------------------------------------------------------------------
//word filter menu item callback functions
void Tdata::wordFiltersEdit(TZMenuRows *menu, int col, int row)
{
    SetFilter();
}
//---------------------------------------------------------------------------
void Tdata::wordFiltersCheck(TZMenuRows *menu, int col, int row)
{
    TWordFilterSelection *selection;
    switch (menu->Tag)
    {
    case mkDictionary:
        selection = dictionaryfilters;
        break;
    case mkKanjiWords:
        selection = kanjiwordfilters;
        break;
    case mkWordGroups:
        selection = wordgroupfilters;
        break;
    default:
        THROW(L"Set something already!");
    }

    bool exclude = col == 2;
    int ix = (int)menu->Rows[row]->Data[col];

    bool change = false;
    if (menu->Rows[row]->Checked[col])
    {
        if (selection->IndexOf(wordfilters->Id[ix] * (exclude ? -1 : 1)) < 0)
        {
            selection->Remove(wordfilters->Id[ix] * (exclude ? 1 : -1));
            selection->Add(wordfilters->Id[ix] * (exclude ? -1 : 1));
            change = true;
        }
    }
    else
    {
        if ((ix = selection->IndexOf(wordfilters->Id[ix] * (exclude ? -1 : 1))) >= 0)
        {
            selection->Delete(ix);
            change = true;
        }
    }

    if (change)
    {
        switch (menu->Tag)
        {
        case mkDictionary:
            if (!fDict->Visible)
                fDict->filterchanged = true;
            else
            {
                fDict->btnFilter->Down = true;
                fDict->btnFilter->Click();
            }
            if (!fPopupDict->Visible)
                fPopupDict->filterchanged = true;
            else
            {
                fPopupDict->btnFilter->Down = true;
                fPopupDict->btnFilter->Click();
            }
            break;
        case mkKanjiWords:
            fGroup->btnKFilter->Down = true;
            fGroup->btnKFilter->Click();
            break;
        case mkWordGroups:
            fGroup->btnWFilter->Down = true;
            fGroup->btnWFilter->Click();
            break;
        }
    }
}
//---------------------------------------------------------------------------
void Tdata::PopupFilters(TWordFilterMenuKind kind, int x, int y)
{
    if (!wordfilters->Count)
    {
        menu->Popup(editrows, x, y);
        return;
    }

    wchar_t *names[] = { L"Edit Filters...", L"Include", L"Exclude" };
    TZMenuItemType types[] = { itSimple, itTitle, itTitle };

    TWordFilterSelection *selection;
    switch (kind)
    {
    case mkDictionary:
        selection = dictionaryfilters;
        break;
    case mkKanjiWords:
        selection = kanjiwordfilters;
        break;
    case mkWordGroups:
        selection = wordgroupfilters;
        break;
    default:
        THROW(L"Set something already!");
    }

    delete filterrows;

    filterrows = new TZMenuRows(3);
    TZMenuRow *row = filterrows->AddRow();
    row->Setup(names, types);
    row->Callbacks[0] = &wordFiltersEdit;
    filterrows->AddSpan();
    for (int ix = 0; ix < wordfilters->Count; ++ix)
    {
        row = filterrows->AddRow();
        row->Titles[0] = wordfilters->Names[ix] + L" filter:";
        row->Types[0] = itAltTitle;
        row->Types[1] = itCheckAuto;
        row->Types[2] = itCheckAuto;
        row->Callbacks[1] = &wordFiltersCheck;
        row->Callbacks[2] = &wordFiltersCheck;
        row->Data[1] = (void*)ix;
        row->Data[2] = (void*)ix;

        row->Checked[1] = selection->IndexOf(wordfilters->Id[ix]) >= 0;
        row->Checked[2] = selection->IndexOf(-wordfilters->Id[ix]) >= 0;
    }
    filterrows->Tag = kind;

    menu->Popup(filterrows, x, y);
}
//---------------------------------------------------------------------------
void __fastcall Tdata::appEventsMessage(tagMSG &Msg, bool &Handled)
{
    if (Msg.message == WM_MOUSEWHEEL || Msg.message == WM_MOUSEHWHEEL)
    {
        HWND wnd = WindowFromPoint(Msg.pt);
        if (wnd != 0)
        {
            DWORD procid = 0;
            if (GetWindowThreadProcessId(wnd, &procid) != 0 && procid == myprocessid)
                Msg.hwnd = wnd;
        }
    }
    else if (Msg.message == WM_CHAR && (wchar_t)Msg.wParam == VK_ESCAPE && ignore_esc_char)
    {
        --ignore_esc_char;
        Msg.wParam = 0;
        Handled = true;
    }
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(107);
#endif



