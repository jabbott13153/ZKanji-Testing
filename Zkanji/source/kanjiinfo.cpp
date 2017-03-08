//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanjiinfo.h"
#include "dropdown.h"
#include "main.h"
#include "kanjilist.h"
#include "settings.h"
#include "collection.h"
#include "kanji.h"
#include "kanjiobj.h"
#include "kanjielement.h"
#include "mouseevent.h"
#include "modallist.h"
#include "popupdict.h"
#include "closebutton.h"
#include "gengraph.h"
#include "kanjihint.h"
#include "mouseovers.h"

#include "docker.h"
#include "wordlist.h"
#include "popupdict.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"

#define SMALL_STROKEWIDTH   6
#define LARGE_STROKEWIDTH   11

#ifdef _DEBUG
static TGlobalTest tester1(88);
#endif

TfInfo *fInfo;

const wchar_t *refstr[KANJI_REFERENCE_COUNT] = {
    L"Jouyou", L"JLPT (old)", L"S & H Dict.", L"Kanji & Kana",
    L"Nelson#", L"Halpern#", L"Heisig#", L"Gakken#",
    L"Henshall#", L"SKIP code", L"4-Corner", L"Unicode", L"JIS X0208",
    L"Shift-JIS", L"EUC-JP", L"Frequency", L"Kuten", L"JLPT"
};

const wchar_t *fullrefstr[KANJI_REFERENCE_COUNT] = {
    L"Jouyou grade", L"JLPT level (old)", L"S & H Kanji Dictionary",
    L"Kanji and Kana", L"Nelson Character Index", L"Halpern Character Index",
    L"Heisig Character Index", L"(Gakken) Character Index",
    L"Henshall Character Index", L"SKIP code", L"4-Corner Code", L"Unicode",
    L"Double-byte JIS X0208", L"Shift-JIS Character Code", L"EUC-JP Character Code",
    L"Frequency of use", L"Kuten Code Point", L"JLPT N level"
};

wchar_t *infostr[KANJI_REFERENCE_COUNT] = {
    L"Jouyou:", L"old JLPT:", L"S&H:",
    L"K&K:", L"Nelson:", L"Halp.:",
    L"Heis.:", L"Gakken:",
    L"Hensh.:", L"SKIP:", L"4C:", L"Uni.:",
    L"JIS:", L"S-JIS:", L"EUC-JP:",
    L"Freq:", L"Kuten:", L"JLPT:"
};

//---------------------------------------------------------------------------
UnicodeString __fastcall EscapeLabel(const wchar_t *c)
{
    UnicodeString str = L"";
    int len = wcslen(c);
    for (int i = 0; i < len; ++i)
    {
        str += c[i];
        if (c[i] == L'&')
            str += c[i];
    }
    return str;
}
//---------------------------------------------------------------------------
void ShowKanjiInfo(int collection, TKanji *k)
{
    fInfo->kanji = k;
    fInfo->collection = collection;
    if (!k || fInfo->Visible)
        return;

    //fInfo->Recreate(true);
    fInfo->Show();
}
//---------------------------------------------------------------------------
void TfInfo::WMNCCalcSize(TMessage &msg)
{
    if (!msg.WParam)
    {
        TRect *r = ((TRect*)msg.LParam);
        InflateRect(r, -4, -4);
        msg.Result = 0;
    }
    else
    {
        NCCALCSIZE_PARAMS *n = ((NCCALCSIZE_PARAMS*)msg.LParam);
        memmove(n->rgrc + 1, n->rgrc, sizeof(tagRECT)*2);
        InflateRect(n->rgrc, -4, -4);
        msg.Result = WVR_VALIDRECTS;
    }
}
//---------------------------------------------------------------------------
void TfInfo::WMNCHitTest(TWMNCHitTest &msg)
{
    TMessage tmp = *reinterpret_cast<TMessage *>(&msg);
    msg.Result = DefWindowProc(Handle, tmp.Msg, tmp.WParam, tmp.LParam);
    msg.Result = HTCAPTION;
}
//---------------------------------------------------------------------------
void TfInfo::WMNCPaint(TMessage &msg)
{
    HDC hdc;
    hdc = GetWindowDC(Handle);
    if (!hdc)
        return;

    TRect r;
    HPEN p;
    HPEN pold;
    int c = (int)ColorToRGB(clBtnFace);
    bool d = (c & 0xff) >= BTN_DARK_TRESH && ((c >> 8) & 0xff) >= BTN_DARK_TRESH && ((c >> 16) & 0xff) >= BTN_DARK_TRESH;

    GetWindowRect(Handle, &r);
    OffsetRect(r, -r.Left, -r.Top);
    ExcludeClipRect(hdc, 4, 4, r.Width() - 4, r.Height() - 4);

    TColor pcol;

    pcol = d ? COLORDEC(clBtnFace, 0x101010) : COLORINC(clBtnFace, 0x101010);
    if (Active)
        pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.2);
    p = CreatePen(PS_SOLID, 0, pcol);
    pold = (HPEN)SelectObject((HDC)hdc, p);

    TPoint pts[5] = { Point(3, r.Height() - 4), Point(3, 3), Point(r.Width() - 4, 3), Point(r.Width() - 4, r.Height() - 4), Point(3, r.Height() - 4) };
    Polyline(hdc, pts, 5);
    SelectObject(hdc, pold);
    DeleteObject(p);

    pcol = d ? COLORDEC(clBtnFace, 0x202020) : COLORINC(clBtnFace, 0x202020);
    if (Active)
        pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.4);
    p = CreatePen(PS_SOLID, 0, pcol);
    pold = (HPEN)SelectObject((HDC)hdc, p);

    TPoint pts2[5] = { Point(2, r.Height() - 3), Point(2, 2), Point(r.Width() - 3, 2), Point(r.Width() - 3, r.Height() - 3), Point(2, r.Height() - 3) };
    Polyline(hdc, pts2, 5);
    SelectObject(hdc, pold);
    DeleteObject(p);

    pcol = d ? COLORDEC(clBtnShadow, 0x404040) : COLORINC(clBtnShadow, 0x404040);
    p = CreatePen(PS_SOLID, 0, pcol);
    pold = (HPEN)SelectObject((HDC)hdc, p);

    LOGBRUSH lb;
    lb.lbStyle = BS_SOLID;
    pcol = d ? COLORDEC(clBtnFace, 0x404040) : COLORINC(clBtnFace, 0x404040);
    if (Active)
        pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.5);
    lb.lbColor = pcol;

    HBRUSH b = CreateBrushIndirect(&lb);
    HBRUSH bold = (HBRUSH)SelectObject((HDC)hdc, b);

    ExcludeClipRect(hdc, 2, 2, r.Width() - 2, r.Height() - 2);
    RoundRect(hdc, 0, 0, r.Width(), r.Height(), 9, 9);
    SelectObject(hdc, bold);
    SelectObject(hdc, pold);
    DeleteObject(p);
    DeleteObject(b);
    ReleaseDC(Handle, hdc);
}
//---------------------------------------------------------------------------
__fastcall TfInfo::TfInfo(TComponent* Owner) : modaldisabled(false), mmousept(-9999, -9999), fcollection(-1),
        ctrlaction(ctrlHidden), fkanji(NULL), felement(-1), fstrokeorder(false), TBaseForm(Owner)
{
    history = new TDropDownList(true, true);

    KeepSize = true;
    ActivateOnShow = false;
    OnTop = true;

    TRect rsod = ChildRect(pSOD, 0, sbOptionS->Top, 0, sbOptionS->Top);
    TRect rctrl = ChildRect(pControl);
    OffsetRect(&rctrl, 0, rsod.Top - rctrl.Bottom);
    ctrltop = rctrl.Top;
    ctrlheight = rctrl.Height();
    pControl->Top = ctrltop + ctrlheight - 1;
    ctrlShape1->SetBounds(0, 0, pControl->Width, pControl->Height);
    pControl->Height = 1;
    pControl->Visible = false;
    pButtons->Top = pbKanji->Top + max(pbReadings->Height, pbKanji->Height);

    bm2 = new TBmp;
    kobj = new TKanjiDrawObj(pbKanji, playcallback);
    kobj->strokewidth = SMALL_STROKEWIDTH;
    posimilar = new TKanjiPanelObj(pbSimilar, p1callback, scolor, sdraw);
    poparts = new TKanjiPanelObj(pbParts, p2callback, pcolor, pdraw);
    poparents = new TKanjiPanelObj(pbParents, p3callback);
    posimilar->bgcolor = (TColor)0x00fffcf2;
    poparts->bgcolor = (TColor)0x00f8fffc;
    poparents->bgcolor = (TColor)0x00f2f8ff;
    poparts->partselectable = true;

    mobjON = new TMouseOvers(pbReadings, doMPrepare, doMONMeasure, doMONDraw, doMChange);
    mobjON->Canvas = bm2->Canvas;

    mobjKun = new TMouseOvers(pbReadings, doMPrepare, doMKunMeasure, doMKunDraw, doMChange);
    mobjKun->Canvas = bm2->Canvas;

    titles = new TLabel *[KANJI_REFERENCE_COUNT];
    refs = new TLabel *[KANJI_REFERENCE_COUNT];
    for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
    {
        titles[ix] = new TLabel(this);
        titles[ix]->Font->Style = TFontStyles() << fsBold;
        titles[ix]->Visible = false;
        refs[ix] = new TLabel(this);
        refs[ix]->Visible = false;
        titles[ix]->Parent = pRef;
        refs[ix]->Parent = pRef;

        refs[ix]->Alignment = taRightJustify;

        titles[ix]->OnMouseDown = &pbKanjiMouseDown;
        refs[ix]->OnMouseDown = &pbKanjiMouseDown;
        titles[ix]->OnMouseMove = &pbKanjiMouseMove;
        refs[ix]->OnMouseMove = &pbKanjiMouseMove;
        titles[ix]->OnMouseUp = &pbKanjiMouseUp;
        refs[ix]->OnMouseUp = &pbKanjiMouseUp;
    }

    AppActivateEvent(this, repaintProc, true, true);
    AppActivateEvent(this, repaintProc, false, true);
    AppActivateEvent(this, appDeactivate, false, true);
    SettingsChangeEvent(this, settingsChanged, true);

    AddEvent(this, repaintProc, eltActivate);
    AddEvent(this, repaintProc, eltDeactivate);

    closebtn = new TCloseButton(this, sbpTopRight, 0, 0, &cbgptr);
    Shadowed = true;
}
//---------------------------------------------------------------------------
__fastcall TfInfo::~TfInfo()
{
    AppActivateEvent(this, repaintProc, true, false);
    AppActivateEvent(this, repaintProc, false, false);
    RemoveEvent(this, repaintProc, eltActivate);
    RemoveEvent(this, repaintProc, eltDeactivate);

    delete closebtn;

    delete mobjON;
    delete mobjKun;

    delete kobj;
    delete bm2;

    delete posimilar;
    delete poparts;
    delete poparents;
    clear_history();
    delete history;

    for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
    {
        delete titles[ix];
        delete refs[ix];
    }
    delete[] titles;
    delete[] refs;

    SettingsChangeEvent(this, settingsChanged, false);
    AppActivateEvent(this, appDeactivate, false, false);

}
//---------------------------------------------------------------------------
void TfInfo::settingsChanged(void *caller, TSettingsProcState state)
{
    if (state != spsEnd)
        return;

    if (miRefView->Count == KANJI_REFERENCE_COUNT + 2)
        build_refmenu();

    refview_setall();
    if (fkanji || felement < 0 || !settings.shownonkanjielements || !hasstrokes)
        set_kanji_data(fkanji);
    else
        set_elem_data(felement);

    if (Visible)
    {
        pbReadings->Invalidate();
        pbSimilar->Invalidate();
    }

    if (fkanji || felement >= 0)
    {
        kobj->Update();
    }
    else if (Visible)
        pbKanji->Invalidate();

    update_references();
}
//---------------------------------------------------------------------------
void TfInfo::appDeactivate(void *caller)
{
    stop_moving();
}
//---------------------------------------------------------------------------
void TfInfo::repaintProc(void *caller)
{
    SendMessage(Handle, WM_NCPAINT, 0, 0);
    TRect r = Rect(0, 0, ClientWidth, b0->Top);

    /*InvalidateRect(Handle,&r,false);
     pTitle->Invalidate();*/

    closebtn->Redraw();
    RedrawWindow(Handle, &r, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_ALLCHILDREN);

}
//---------------------------------------------------------------------------
void __fastcall TfInfo::FormDeactivate(TObject *Sender)
{
    stop_moving();
}
//---------------------------------------------------------------------------
void TfInfo::clear_history()
{
    history->Clear();
    btnBack->Enabled = false;
    btnHistory->Enabled = false;
}
//---------------------------------------------------------------------------
void TfInfo::add_history(TKanji *k)
{
    if (!k)
        return;
    history->Add(UnicodeString(k->ch), (void*)-1);
    btnBack->Enabled = true;
    btnHistory->Enabled = true;
}
//---------------------------------------------------------------------------
void TfInfo::add_history(int elem)
{
    if (elem < 0)
        return;
    history->Add("*", (void*)elem);
    btnBack->Enabled = true;
    btnHistory->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::FormKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == 0x1B) // Escape key.
        Hide();
    else if (Key >= '0' && Key <= '9')
    {
        switch (Key)
        {
        case '1':
            sbRef->Down = !sbRef->Down;
            aRefPanel->Checked = sbRef->Down;
            sbRef->Click();
            break;
        case '2':
            sbSimilar->Down = !sbSimilar->Down;
            aSimilarPanel->Checked = sbSimilar->Down;
            sbSimilar->Click();
            break;
        case '3':
            sbParts->Down = !sbParts->Down;
            aPartsPanel->Checked = sbParts->Down;
            sbParts->Click();
            break;
        case '4':
            sbParents->Down = !sbParents->Down;
            aParentsPanel->Checked = sbParents->Down;
            sbParents->Click();
            break;
        }
        Key = 0;
    }
    else if (Key == L' ' && sbPlay->Enabled)
    {
        if (sbPlay->Down)
            sbStop->Click();
        else
            sbPlay->Click();
    }
    else if (Key == L'g' || Key == L'G')
    {
        cbGrid->Checked = !cbGrid->Checked;
        cbGridClick(cbGrid);
    }
    else if (Key == L'd' || Key == L'D' && sbOrder->Enabled)
    {
        sbOrder->Down = !sbOrder->Down;
        sbOrder->Click();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
/*    if (Key == VK_ESCAPE)
        Hide();
    else*/ if (Key >= '1' && Key <= '9' && Shift.Contains(ssAlt))
    {
        int coll = Key - '1';
        if (coll < dictionary->Count && settings.dictionary_index != coll)
        {
            fDict->SelectDictionary(coll);
            collection = coll;
        }
        Key = 0;
    }
//    else if (Key == VK_SPACE && sbPlay->Enabled)
//    {
//        if (sbPlay->Down)
//            sbStop->Click();
//        else
//            sbPlay->Click();
//    }
//    else if (Key == L'g' || Key == L'G')
//    {
//        cbGrid->Checked = !cbGrid->Checked;
//        cbGridClick(cbGrid);
//    }
//    else if (Key == L'd' || Key == L'D' && sbOrder->Enabled)
//    {
//        sbOrder->Down = !sbOrder->Down;
//        sbOrder->Click();
//    }
    else if (!modallist || !modallist->Count)
    {
        switch (Key)
        {
        case VK_F2:
            if (!settings.intray)
            {
                fDock->SetFocus();
                fDict->btnKanji->Click();
            }
            else
            {
                fPopupDict->SetFocus();
                fPopupDict->btnKanji->Click();
            }
            break;
        case VK_F3:
            if (!settings.intray)
            {
                fDock->SetFocus();
                fDict->btnRomaji->Click();
            }
            else
            {
                fPopupDict->SetFocus();
                fPopupDict->btnRomaji->Click();
            }
            break;
        case VK_F4:
            if (!settings.intray)
            {
                fDock->SetFocus();
                fDict->btnJapanese->Click();
            }
            else
            {
                fPopupDict->SetFocus();
                fPopupDict->btnJapanese->Click();
            }
            break;
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TfInfo::cbGridClick(TObject *Sender)
{
    kobj->grid = cbGrid->Checked;
}
//---------------------------------------------------------------------------
void TfInfo::set_kanji(TKanji *k)
{
    if (k == fkanji)
        return;

    clear_history();
    fkanji = k;
    felement = -1;

    set_kanji_data(k);
}
//---------------------------------------------------------------------------
void TfInfo::set_elem(int elem)
{
    if (!fkanji && felement == elem)
        return;

    clear_history();
    felement = elem;
    fkanji = NULL;

    set_elem_data(felement);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::miRefViewClick(TObject *Sender)
{
    int tag = ((TMenuItem*)Sender)->Tag;
    settings.referenceshow[tag] = ((TMenuItem*)Sender)->Checked;

    update_references();
}
//---------------------------------------------------------------------------
void TfInfo::refview_setall()
{
    bool all = true;
    for (int ix = 0; ix < KANJI_REFERENCE_COUNT && all; ++ix)
        all = settings.referenceshow[ix];
    if (all)
    {
        miRefAll->Tag = 1;
        miRefAll->Caption = "Hide All";
    }
    else
    {
        miRefAll->Tag = 0;
        miRefAll->Caption = "Show All";
    }
}
//---------------------------------------------------------------------------
int /*__fastcall*/ __sortkanjiparents(int a, int b)
{
    unsigned short af = kanjis->Kanjis[a]->frequency;
    unsigned short bf = kanjis->Kanjis[b]->frequency;
    if (!af)
        af = 9999;
    if (!bf)
        bf = 9999;
    return af - bf;
}
//---------------------------------------------------------------------------
void TfInfo::set_data(TKanji *k, int elem)
{
    if (k)
        kobj->kanji = k->ch;
    else if (elem >= 0)
    {
        kobj->element = elem;
        kobj->kanji = 0;
    }
    else
    {
        kobj->element = -1;
        kobj->kanji = 0;
    }

    if (pbReadings->VPos)
    {
        pbReadings->VertScrollBar->Enabled = true;
        pbReadings->VPos = 0;
    }

    UnicodeString s;

    fill_references();
    aCopyRef->Enabled = fkanji != NULL;
    aAppendRef->Enabled = fkanji != NULL;
    aCopyKanji->Enabled = fkanji != NULL;
    aAppendKanji->Enabled = fkanji != NULL;
    aTranslateKanji->Enabled = fkanji != NULL;

    if (k)
        elem = k->element;

    if (k || elem >= 0)
    {
        btnFind->Enabled = k && !modaldisabled;

        TIntegerList *ls = new TIntegerList;
        try
        {
            //fill similar list
            if (k)
            {
                for (int i = 0; i < fkanji->sim1 + fkanji->sim2; i++)
                    ls->Add(kanjis->IndexOf(fkanji->similar[i]));
            }
            posimilar->set_items(ls);

            //fill parts list
            ls->Clear();
            if (elem >= 0)
            {
                int epos = -1;
                TKanjiElement *e = elements->Items[elem];
                do
                {
                    if (epos >= 0)
                    {
                        if (ls->Items[epos] < 0)
                        {
                            e = elements->Items[-ls->Items[epos]];
                        }
                        else
                        {
                            e = elements->Items[kanjis->Kanjis[ls->Items[epos]]->element];
                        }
                    }
                    epos++;
                    for (int i = 0; i < 4 && e->parts[i] >= 0; i++)
                    {
                        int what;
                        if (elements->Items[e->parts[i]]->owner < NO_OWNER)
                            what = elements->Items[e->parts[i]]->owner;
                        else
                            what = -e->parts[i];

                        if (ls->IndexOf(what) < 0)
                            ls->Add(what);
                    }
                }
                while (epos < ls->Count);
                for (int i = ls->Count - 1; i >= 0; i--)
                    if (ls->Items[i] < 0 && elements->Items[-ls->Items[i]]->variants[0].strokecount == 0 || !(settings.shownonkanjielements && hasstrokes) && ls->Items[i] < 0)
                        ls->Delete(i);
            }
            poparts->set_items(ls);

            //fill parents list
            ls->Clear();
            if (elem >= 0)
            {
                int epos = -1;
                TKanjiElement *e = elements->Items[elem];
                do
                {
                    if (epos >= 0)
                    {
                        if ((int)ls->Items[epos] < 0)
                            e = elements->Items[-(int)ls->Items[epos]];
                        else
                            e = elements->Items[kanjis->Kanjis[ls->Items[epos]]->element];
                    }
                    epos++;
                    for (int i = 0; i < e->parentcnt; i++)
                    {
                        int what = (elements->Items[e->parents[i]]->owner < NO_OWNER ? elements->Items[e->parents[i]]->owner : -e->parents[i]);
                        if (ls->IndexOf(what) < 0)
                            ls->Add(what);
                    }
                }
                while (epos < ls->Count);
                for (int i = ls->Count - 1; i >= 0; i--)
                    if (ls->Items[i] < 0)
                        ls->Delete(i);
            }
            ls->Sort(&__sortkanjiparents);
            poparents->set_items(ls);
        }
        catch (...)
        {
            ;
        }
        if (ls)
            delete ls;
    }
    else
    {
        btnFind->Enabled = false;
        posimilar->set_items(NULL);
        poparts->set_items(NULL);
        poparents->set_items(NULL);
    }

    mobjON->reset(k ? k->oncnt : 0);
    mobjKun->reset(k ? k->kuncnt : 0);
    pbReadings->Invalidate();
    enable_player();

    lbStroke->Caption = fkanji && fkanji->element >= 0 || elem >= 0 ? ((kobj->strokeposition < 9 ? "0" : "") + IntToStr(kobj->strokeposition + 1)).c_str() : L"N/A";
    lbStrokeS->Caption = lbStroke->Caption;

    update_similar_buttons();
    update_parts_buttons();
    update_parents_buttons();
}
//---------------------------------------------------------------------------
void TfInfo::set_kanji_data(TKanji *k)
{
    set_data(k, -1);
}
//---------------------------------------------------------------------------
void TfInfo::set_elem_data(int elem)
{
    set_data(NULL, elem);
}
//---------------------------------------------------------------------------
void TfInfo::enable_player()
{
    if (sbPlayS->Down)
        sbStopS->Click();

    sbOrder->Enabled = felement < 0;
    sbOrder->Down = felement >= 0 || fstrokeorder;
    kobj->strokeorder = !fkanji && felement >= 0 || sbOrder->Enabled && sbOrder->Down;

    sbStop->Enabled = sbOrder->Down;
    sbPlay->Enabled = sbOrder->Down && (fkanji && fkanji->element >= 0 || felement >= 0);
    sbPause->Enabled = sbPlay->Enabled;
    sbRew->Enabled = sbPlay->Enabled;
    sbFF->Enabled = sbPlay->Enabled;
    sbBack->Enabled = sbPlay->Enabled;
    sbFore->Enabled = sbPlay->Enabled;
    lbStroke->Enabled = sbOrder->Down;

    sbStopS->Enabled = sbOrder->Down;
    sbPlayS->Enabled = sbPlay->Enabled;
    sbPauseS->Enabled = sbPause->Enabled;
    sbRewS->Enabled = sbRew->Enabled;
    sbFFS->Enabled = sbFF->Enabled;
    sbBackS->Enabled = sbBack->Enabled;
    sbForeS->Enabled = sbFore->Enabled;
    lbStrokeS->Enabled = lbStroke->Enabled;
}
//---------------------------------------------------------------------------
extern bool ThemesAreTurnedOn;
void __fastcall TfInfo::pbReadingsPaint(TObject *Sender)
{
    TCanvas *c = bm2->Canvas;
    TSize s;
    TRect r;
    int x, y = 2 * scaleres - pbReadings->VPos;
    bm2->Width = pbReadings->ClientWidth;
    bm2->Height = pbReadings->ClientHeight;
    if (ThemesAreTurnedOn)
    {
        r = pbReadings->ClientRect;
        c->Brush->Style = bsSolid;
        c->FillRect(Rect(r.Left, r.Top, r.Right, r.Top + r.Height() / 8*7));
        DrawGrad(c, clWhite, COLORDEC(clWhite, 0x181818), Rect(r.Left, r.Top + r.Height() / 8*7, r.Right, r.Bottom));
        c->Brush->Style = bsClear;
    }
    else
    {
        c->Brush->Style = bsSolid;
        c->FillRect(pbReadings->ClientRect);
    }
    if (!kanji)
    {
        if (felement >= 0)
        {
            c->Font->Name = SystemFont;
            c->Font->Charset = DEFAULT_CHARSET;
            c->Font->Size = 12;
            c->Font->Style = TFontStyles();
            c->Font->Color = clBlack;
            UnicodeString str = L"This element is not a kanji by itself.";
            DrawMultilineString(bm2, str, y);
        }

        pbReadings->Canvas->Draw(0, 0, bm2);
        return;
    }

    if (miRadical->Checked && fkanji)
    {
        c->Font->Name = SystemFont;
        c->Font->Charset = DEFAULT_CHARSET;
        c->Font->Size = 10;
        c->Font->Color = clBlack;
        c->Font->Style = TFontStyles() << fsBold;
        GetTextExtentPoint32A(c->Handle, "Radical:", 8, &s);
        ExtTextOutA(c->Handle, 2*scaleres, y, 0, &r, "Radical:", 8, 0);

        c->Font->Style = TFontStyles();
        SetRadicalFont(c->Font);
        c->Font->Size = 10;
        TSize s2;
        UINT oldalign = SetTextAlign(c->Handle, TA_LEFT | TA_BASELINE);
        UnicodeString wtr = (UnicodeString)L"　" + radicals[kanji->radical - 1] + L" ";
        GetTextExtentPoint32W(c->Handle, wtr.c_str(), wtr.Length(), &s2);
        ExtTextOutW(c->Handle, s.cx, y + s.cy*0.8, 0, &r, wtr.c_str(), wtr.Length(), 0);
        SetTextAlign(c->Handle, oldalign);

        c->Font->Name = settings.romajifont;
        c->Font->Charset = DEFAULT_CHARSET;
        c->Font->Size = 10;
        UnicodeString str = (UnicodeString)int(kanji->radical);
        ExtTextOutW(c->Handle, s.cx + s2.cx, y, 0, &r, str.c_str(), str.Length(), 0);

        c->MoveTo(int(2*scaleres)*2, y + s.cy +int(2*scaleres)*2);
        c->LineTo(pbReadings->ClientWidth -int(2*scaleres)*2, y + s.cy +int(2*scaleres)*2);

        y += s.cy + 8 * scaleres;
    }

    c->Font->Name = SystemFont;
    c->Font->Charset = DEFAULT_CHARSET;
    c->Font->Size = 10;
    c->Font->Color = clBlack;
    c->Font->Style = TFontStyles() << fsBold;
    GetTextExtentPoint32W(c->Handle, L"ON readings:", 12, &s);
    ExtTextOutW(c->Handle, 2, y, 0, &r, L"ON readings:", 12, 0);
    x = 2 * scaleres;
    y = y + s.cy + 3 * scaleres;

    mobjON->draw(x, y);

    y = y + s.cy + 3 * scaleres +int(2 * scaleres) * 2;
    c->Font->Name = SystemFont;
    c->Font->Charset = DEFAULT_CHARSET;
    c->Font->Size = 10;
    c->Font->Style = TFontStyles() << fsBold;
    GetTextExtentPoint32A(c->Handle, "KUN readings:", 13, &s);
    ExtTextOutA(c->Handle, 2*scaleres, y, 0, &r, "KUN readings:", 13, 0);
    x = 2 * scaleres;
    y = y + s.cy + 2 * scaleres;

    mobjKun->draw(x, y);

    y = y + s.cy + 3 * scaleres +int(2 * scaleres) * 2;
    c->Font->Name = SystemFont;
    c->Font->Charset = DEFAULT_CHARSET;
    c->Font->Size = 10;
    c->Font->Style = TFontStyles() << fsBold;
    GetTextExtentPoint32A(c->Handle, "Meaning:", 8, &s);
    ExtTextOutA(c->Handle, 2*scaleres, y, 0, &r, "Meaning:", 8, 0);
    y = y + s.cy + 2 * scaleres;

    c->Font->Name = SystemFont;
    c->Font->Charset = DEFAULT_CHARSET;
    c->Font->Size = 12;
    c->Font->Style = TFontStyles();

    UnicodeString str = dictionary->KanjiMeaning(fcollection, kanji->index);
    if (fcollection > 0 && !dictionary->Items[fcollection]->kanjidat[kanji->index].meaning)
        c->Font->Color = (TColor)0x00d08040;
    DrawMultilineString(bm2, str, y);

    pbReadings->Canvas->Draw(0, 0, bm2);
}
//---------------------------------------------------------------------------
void TfInfo::DrawMultilineString(TBmp *bmp, UnicodeString str, int &top)
{
    TCanvas *c = bmp->Canvas;
    TRect r = Rect(2 * scaleres, 0, bmp->Width - 2 * scaleres, bmp->Height - 2 * scaleres);
    DrawTextW(c->Handle, str.c_str(), str.Length(), &r, DT_CALCRECT | DT_LEFT | DT_WORDBREAK);
    if (pbReadings->VMax != MAX(pbReadings->ClientHeight + 1, top + (int)r.bottom +int(2*scaleres)*2 + pbReadings->VPos))
        pbReadings->VertScrollBar->Enabled = true;
    pbReadings->VMax = MAX(1, top + (int)r.bottom +int(2 * scaleres) * 2 + pbReadings->VPos - 1);
    pbReadings->VertScrollBar->ThumbSize = pbReadings->ClientHeight;
    if (pbReadings->ClientHeight + 1 > top + r.bottom +int(2*scaleres)*2 + pbReadings->VPos)
        pbReadings->VertScrollBar->Enabled = false;
    r = Rect(2 * scaleres, top, bmp->Width - 2 * scaleres, top + r.bottom);
    DrawTextW(c->Handle, str.c_str(), str.Length(), &r, DT_LEFT | DT_WORDBREAK);
    top += r.bottom +int(2 * scaleres) * 2;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::btnFindClick(TObject *Sender)
{
    if (kanji)
    {
        fKanji->kanji = kanji;
        if (settings.intray && !fKanji->Visible)
            fKanji->Popup(true, false);
    }
}
//---------------------------------------------------------------------------
bool TfInfo::same_ON(TKanji *a, TKanji *b)
{
    bool result = false;
    for (int i = 0; i < a->oncnt && !result; i++)
        for (int j = 0; j < b->oncnt && !result; j++)
            result = !wcscmp(a->on[i], b->on[j]);

    return result;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::FormHide(TObject *Sender)
{
    clear_history();
}
//---------------------------------------------------------------------------

TKanji* TfInfo::get_kanji()
{
    if (!fkanji)
        return NULL;
    return fkanji;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::btnBackClick(TObject *Sender)
{
    if (history->Strings[0][1] != L'*')
    {
        fkanji = kanjis->Kanjis[kanjis->IndexOf(history->Strings[0][1])];
        felement = -1;
        set_kanji_data(fkanji);
    }
    else
    {
        felement = (int)history->Data[0];
        fkanji = NULL;
        set_elem_data(felement);
    }

    history->Delete(0);
    btnBack->Enabled = history->Count;
    btnHistory->Enabled = history->Count;
}
//---------------------------------------------------------------------------
void TfInfo::set_collection(byte ix)
{
    if (fcollection == ix)
        return;
    fcollection = ix;
    posimilar->collection = dictionary->Items[fcollection];
    poparts->collection = dictionary->Items[fcollection];
    poparents->collection = dictionary->Items[fcollection];
    pbReadings->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbOpenKanjiClick(TObject *Sender)
{
    show_option(ioEnlarge);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbCloseKanjiClick(TObject *Sender)
{
    show_option(ioShrink);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::miOrderClick(TObject *Sender)
{
    strokeorder = sbOrder->Down;
    enable_player();
}
//---------------------------------------------------------------------------
void TfInfo::show_option(TInfoOption opt)
{
    kobj->Stop();

    int oh = ClientHeight;

    int fh = max(pbKanji->Height, pbReadings->Height) + pButtons->Height + b0->Height + b0->Top + (pOrder->Visible ? pOrder->Height : 0);

    int baseheight = fh;

    fh += opt != ioEnlarge && pSimilar->Visible ? pSimilar->Height : 0;
    fh += opt != ioEnlarge && pParts->Visible ? pParts->Height : 0;
    fh += opt != ioEnlarge && pParents->Visible ? pParents->Height : 0;
    fh += (pRef->Visible ? pRef->Height : 0);

    int fw = ClientWidth;
    switch (opt)
    {
    case ioEnlarge:
        fh += ClientWidth - pbKanji->Height - pSOD->Height + pOrder->Height;
        baseheight += ClientWidth - pbKanji->Height - pSOD->Height + pOrder->Height;
        pSOD->Visible = false;
        pOrder->Visible = true;
        remove_controlpanel();
        break;
    case ioShrink:
        pOrder->Visible = false;
        pSOD->Visible = true;
        fh -= ClientWidth - (pbReadings->Left - 1) - pSOD->Height + pOrder->Height;
        baseheight -= ClientWidth - (pbReadings->Left - 1) - pSOD->Height + pOrder->Height;
        if (!modallist->Count)
            fh += (sbSimilar->Down ? pSimilar->Height : 0) + (sbParts->Down ? pParts->Height : 0) + (sbParents->Down ? pParents->Height : 0);
        break;
    case ioSimilar:
        fh += (sbSimilar->Down && sbSimilar->Enabled ? 1 : -1) * pSimilar->Height;
        break;
    case ioParts:
        fh += (sbParts->Down && sbParts->Enabled ? 1 : -1) * pParts->Height;
        break;
    case ioParents:
        fh += (sbParents->Down && sbParents->Enabled ? 1 : -1) * pParents->Height;
        break;
    case ioRef:
        fh += (sbRef->Down && sbRef->Enabled ? 1 : -1)*(pRef->Height);
        break;
    }

    bool vis;
    TRect r;

    switch (opt)
    {
    case ioEnlarge:
    case ioShrink:
        if (opt == ioEnlarge)
        {
            sbCloseKanji->Visible = true;
            sbOpenKanji->Visible = false;
            pbKanji->Width = ClientWidth;
            pbKanji->Height = pbKanji->Width;
            sbSimilar->Enabled = false;
            sbParts->Enabled = false;
            sbParents->Enabled = false;
            aSimilarPanel->Enabled = false;
            aPartsPanel->Enabled = false;
            aParentsPanel->Enabled = false;
            kobj->strokewidth = LARGE_STROKEWIDTH;
            pOrder->Top = pbKanji->Height + pbKanji->Top;
        }
        else if (opt == ioShrink)
        {
            sbOpenKanji->Visible = true;
            sbCloseKanji->Visible = false;
            pbKanji->Width = pbReadings->Left - 1;
            pbKanji->Height = pbKanji->Width;
            sbSimilar->Enabled = !modallist->Count;
            sbParts->Enabled = !modallist->Count;
            sbParents->Enabled = !modallist->Count;
            aSimilarPanel->Enabled = !modallist->Count;
            aPartsPanel->Enabled = !modallist->Count;
            aParentsPanel->Enabled = !modallist->Count;
            kobj->strokewidth = SMALL_STROKEWIDTH;
        }
        kobj->Update();
        pButtons->Top = pbKanji->Top + max(pbReadings->Height, pbKanji->Height) + (pOrder->Visible ? pOrder->Height : 0);
    case ioNone:
    case ioRef:
        pRef->Top = baseheight;
        if (!elements->Count)
            sbOrder->Down = false;
        pSimilar->Top = pRef->Top + (sbRef->Down ? pRef->Height : 0);
        pRef->Visible = sbRef->Down;
    case ioSimilar:
        vis = sbSimilar->Down && sbSimilar->Enabled;
        pParts->Top = pSimilar->Top + (vis ? pSimilar->Height : 0);
        pSimilar->Visible = vis;
    case ioParts:
        if (!elements->Count)
            sbParts->Down = false;
        vis = sbParts->Down && sbParts->Enabled;
        pParents->Top = pParts->Top + (vis ? pParts->Height : 0);
        pParts->Visible = vis;
    case ioParents:
        if (!elements->Count)
            sbParents->Down = false;
        vis = sbParents->Down && sbParents->Enabled;
        pParents->Visible = vis;
    }

    ClientHeight = fh;
    roundbounds(fw, fh);

    enable_player();

    if (closebtn)
        closebtn->Redraw();

    r = Rect(-4, min(fh, oh), ClientWidth + 4, min(fh, oh) + 4);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_FRAME | RDW_ALLCHILDREN);

    if (fh < oh)
    {
        GetWindowRect(Handle, &r);
        r.Bottom = r.Top + oh;
        r.Top = r.Bottom - 4;
        TRect r1 = Rect(r.Left, r.Top, r.Left + 4, r.Bottom);
        RedrawWindow(DESKTOPHANDLE, &r1, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
        TRect r2 = Rect(r.Right - 4, r.Top, r.Right, r.Bottom);
        RedrawWindow(DESKTOPHANDLE, &r2, 0, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::pbSimilarMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (!fkanji && felement < 0 || Button != mbLeft)
        return;

    if (!settings.shownonkanjielements || !hasstrokes)
    {
        int elem = (Sender == pbSimilar ? posimilar : Sender == pbParts ? poparts : poparents)->ElementAtPos(x);
        if (elem >= 0 && elements->Items[elem]->owner < NO_OWNER)
            elem = elements->Items[elem]->owner;
        else
            elem = (Sender == pbSimilar ? posimilar : Sender == pbParts ? poparts : poparents)->KanjiAtPos(x);

        if (elem >= 0)
        {
            add_history(fkanji);
            fkanji = kanjis->Kanjis[elem];
            felement = -1;
            set_kanji_data(fkanji);
        }
    }
    else
    {
        int elem = (Sender == pbSimilar ? posimilar : Sender == pbParts ? poparts : poparents)->ElementAtPos(x);
        if (elem >= 0)
        {
            if (elements->Items[elem]->owner < NO_OWNER)
            {
                if (fkanji)
                    add_history(fkanji);
                else if (felement >= 0)
                    add_history(felement);
                fkanji = kanjis->Kanjis[elements->Items[elem]->owner];
                felement = -1;
                set_kanji_data(fkanji);
            }
            else
            {
                if (fkanji)
                    add_history(fkanji);
                else if (felement >= 0)
                    add_history(felement);
                fkanji = NULL;
                felement = elem;
                set_elem_data(felement);
            }
        }
    }

}
//---------------------------------------------------------------------------
void __fastcall TfInfo::miHideClick(TObject *Sender)
{
    if (pmHide->PopupComponent == pbSimilar || pmHide->PopupComponent == lbSimilar || pmHide->PopupComponent == pSimilar || pmHide->PopupComponent == btnSimilarBack || pmHide->PopupComponent == btnSimilarFore)
    {
        sbSimilar->Down = false;
        aSimilarPanel->Checked = false;
        sbSimilar->Click();
    }
    else if (pmHide->PopupComponent == pbParts || pmHide->PopupComponent == lbParts || pmHide->PopupComponent == pParts || pmHide->PopupComponent == btnPartsBack || pmHide->PopupComponent == btnPartsFore)
    {
        sbParts->Down = false;
        aPartsPanel->Checked = false;
        sbParts->Click();
    }
    else if (pmHide->PopupComponent == pbParents || pmHide->PopupComponent == lbParents || pmHide->PopupComponent == pParents || pmHide->PopupComponent == btnParentsBack || pmHide->PopupComponent == btnParentsFore)
    {
        sbParents->Down = false;
        aParentsPanel->Checked = false;
        sbParents->Click();
    }
    else if (pmHide->PopupComponent == pOrder || pmHide->PopupComponent == lbStroke)
    {
        sbOrder->Down = false;
        sbOrder->Click();
    }
}
//---------------------------------------------------------------------------
void TfInfo::enable_buttons(bool enable)
{
    modaldisabled = !enable;
    btnFind->Enabled = enable && fkanji;

    if (pbKanji->Width == ClientWidth)
        enable = false;

    sbSimilar->Enabled = enable;
    sbParts->Enabled = enable;
    sbParents->Enabled = enable;
    aSimilarPanel->Enabled = enable;
    aPartsPanel->Enabled = enable;
    aParentsPanel->Enabled = enable;
    pSimilar->Visible = enable && sbSimilar->Down;
    pParts->Visible = enable && sbParts->Down;
    pParents->Visible = enable && sbParents->Down;
    BorderIcons = (enable ? TBorderIcons() << biSystemMenu : TBorderIcons());

    show_option(ioNone);
}
//---------------------------------------------------------------------------
void TfInfo::enable_readings(bool enable)
{
    mobjON->Enabled = enable;
    mobjKun->Enabled = enable;
}
//---------------------------------------------------------------------------
void TfInfo::Show()
{
    if (Visible)
        return;
    Recreate(true);

    pRef->Visible = sbRef->Enabled && sbRef->Down;
    pOrder->Visible = pbKanji->Width == ClientWidth;
    pSimilar->Visible = sbSimilar->Enabled && sbSimilar->Down;
    pParts->Visible = sbParts->Enabled && sbParts->Down;
    pParents->Visible = sbParents->Enabled && sbParents->Down;

    show_option(ioNone);

    //the first call of SetWindowRgn after the window was shown causes flicker, so make that "flicker" happen now instead of when first changing the window shape for real
    HRGN rgn = CreateRectRgn(0, 0, 0, 0);
    int err = GetWindowRgn(Handle, rgn) == ERROR;
    HRGN rgn2 = CreateRectRgn(0, 0, 0, 0);
    SetWindowRgn(Handle, rgn2, false);
    Visible = true;
    SetWindowRgn(Handle, err ? NULL : rgn, true);
    if (err)
        DeleteObject(rgn);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::FormClose(TObject *Sender, TCloseAction &Action)
{
    kobj->Stop();
}
//---------------------------------------------------------------------------
void TfInfo::playcallback(TDrawStrokeState state)
{
    switch (state)
    {
    case dssStopped:
        sbStop->Down = true;
        sbStopS->Down = true;
        sbPause->GroupIndex = 0;
        sbPauseS->GroupIndex = 0;
        lbStroke->Caption = (kobj->strokeposition < 9 ? "0" : "") + IntToStr(kobj->strokeposition + 1);
        lbStrokeS->Caption = lbStroke->Caption;
        break;
    case dssStarted:
        sbPause->GroupIndex = 1;
        sbPlay->Down = true;
        sbPauseS->GroupIndex = 3;
        sbPlayS->Down = true;
        break;
    case dssStroke:
        lbStroke->Caption = (kobj->strokeposition < 9 ? "0" : "") + IntToStr(kobj->strokeposition + 1);
        lbStrokeS->Caption = lbStroke->Caption;

        if (!sbStop->Down)
            break;
    case dssPaused:
        sbPause->GroupIndex = 1;
        sbPause->Down = true;
        sbPauseS->GroupIndex = 3;
        sbPauseS->Down = true;
        break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbPlayClick(TObject *Sender)
{
    if (fkanji && fkanji->element >= 0 || felement >= 0)
        kobj->Play();
}
//---------------------------------------------------------------------------
void TfInfo::p1callback()
{
    update_similar_buttons();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::btnSimilarBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    MouseRepeat(btnSimilarBackMouseDown, Sender, Button, Shift, x, y);
    posimilar->pos--;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::btnSimilarForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    MouseRepeat(btnSimilarForeMouseDown, Sender, Button, Shift, x, y);
    posimilar->pos++;
}
//---------------------------------------------------------------------------
void TfInfo::p2callback()
{
    update_parts_buttons();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::btnPartsBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    MouseRepeat(btnPartsBackMouseDown, Sender, Button, Shift, x, y);
    poparts->pos--;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::btnPartsForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    MouseRepeat(btnPartsForeMouseDown, Sender, Button, Shift, x, y);
    poparts->pos++;
}
//---------------------------------------------------------------------------
void TfInfo::p3callback()
{
    update_parents_buttons();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::btnParentsBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    MouseRepeat(btnParentsBackMouseDown, Sender, Button, Shift, x, y);
    poparents->pos--;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::btnParentsForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    MouseRepeat(btnParentsForeMouseDown, Sender, Button, Shift, x, y);
    poparents->pos++;
}
//---------------------------------------------------------------------------
void TfInfo::update_similar_buttons()
{
    btnSimilarFore->Enabled = (fkanji || felement >= 0) && (pbSimilar->Width / pbSimilar->Height + posimilar->pos) < posimilar->count;
    btnSimilarBack->Enabled = (fkanji || felement >= 0) && posimilar->pos;
}
//---------------------------------------------------------------------------
void TfInfo::update_parts_buttons()
{
    btnPartsFore->Enabled = (fkanji || felement >= 0) && (pbParts->Width / pbParts->Height + poparts->pos) < poparts->count;
    btnPartsBack->Enabled = (fkanji || felement >= 0) && poparts->pos;
}
//---------------------------------------------------------------------------
void TfInfo::update_parents_buttons()
{
    btnParentsFore->Enabled = (fkanji || felement >= 0) && (pbParents->Width / pbParents->Height + poparents->pos) < poparents->count;
    btnParentsBack->Enabled = (fkanji || felement >= 0) && poparents->pos;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbStopClick(TObject *Sender)
{
    kobj->Stop();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbShadowClick(TObject *Sender)
{
    set_shadow(((TSpeedButton*)Sender)->Down);
}
//---------------------------------------------------------------------------
void TfInfo::set_shadow(bool show)
{
    sbShadow->Down = show;
    miShadow->Checked = show;
    sbShadowS->Down = show;
    kobj->strokeshadow = show;
}
//---------------------------------------------------------------------------
void TfInfo::set_numbers(bool show)
{
    sbNumbers->Down = show;
    sbNumbersS->Down = show;
    miNumbers->Checked = show;
    kobj->strokenumbers = show;
}
//---------------------------------------------------------------------------
void TfInfo::set_dots(bool show)
{
    sbDirection->Down = show;
    sbDirectionS->Down = show;
    miDirection->Checked = show;
    kobj->strokedots = show;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbPauseClick(TObject *Sender)
{
    kobj->Pause();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft)
        return;
    MouseRepeat(sbBackMouseDown, Sender, Button, Shift, X, Y);
    kobj->StrokeBack();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft)
        return;
    MouseRepeat(sbForeMouseDown, Sender, Button, Shift, X, Y);
    kobj->StrokeForward();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbFFClick(TObject *Sender)
{
    kobj->StrokeFastForward();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbRewClick(TObject *Sender)
{
    kobj->StrokeRewind();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbSpeed1Click(TObject *Sender)
{
    set_speed((TDrawSpeed)((TSpeedButton*)Sender)->Tag);
}
//---------------------------------------------------------------------------
void TfInfo::set_speed(TDrawSpeed speed)
{
    if (kobj->strokespeed == speed)
        return;

    switch (speed)
    {
    case dsNormalSpeed:
        kobj->strokespeed = dsNormalSpeed;
        sbSpeed1->Down = true;
        sbSpeedS1->Down = true;
        miSpeed1->Checked = true;
        break;
    case dsFast:
        kobj->strokespeed = dsFast;
        sbSpeed2->Down = true;
        sbSpeedS2->Down = true;
        miSpeed2->Checked = true;
        break;
    case dsFastest:
        kobj->strokespeed = dsFastest;
        sbSpeed3->Down = true;
        sbSpeedS3->Down = true;
        miSpeed3->Checked = true;
        break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbNumbersClick(TObject *Sender)
{
    set_numbers(((TSpeedButton*)Sender)->Down);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbDirectionClick(TObject *Sender)
{
    set_dots(((TSpeedButton*)Sender)->Down);
}
//---------------------------------------------------------------------------
void TfInfo::scolor(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected, TColor &background, TColor &fontcolor)
{
    if (fkanji && index >= fkanji->sim1)
        fontcolor = (TColor)0x00ff6000;
}
//---------------------------------------------------------------------------
void TfInfo::sdraw(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected)
{
}
//---------------------------------------------------------------------------
void TfInfo::pcolor(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected, TColor &background, TColor &fontcolor)
{
    if (elem && elem->owner >= NO_OWNER)
    {
        if (!settings.shownonkanjielements || !hasstrokes)
        {
            background = COLORDEC(background, 0x0a0a0a);
            fontcolor = COLORDEC(background, 0x504857);
        }
        else
        {
            background = coloravg(background, (TColor)0x5555ee, 0.97);
            fontcolor = coloravg(fontcolor, (TColor)0x5555ee, 0.4);
        }
    }
}
//---------------------------------------------------------------------------
void TfInfo::pdraw(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected)
{
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::btnHistoryClick(TObject *Sender)
{
    TRect r = Rect(btnBack->Left, btnBack->Top, btnHistory->Left + btnHistory->Width, btnBack->Top + btnBack->Height);
    TPoint p = Point(-btnBack->Left, -btnBack->Top);
    p = btnBack->ClientToScreen(p);
    OffsetRect(&r, p.x, p.y);
    InflateRect(&r, 0, -3);
    history->Popup(r, histfunc, this, false);

    history->Capture();
}
//---------------------------------------------------------------------------
void TfInfo::histfunc(int index, UnicodeString w, void *data)
{
    while (index >= 0)
    {
        history->Delete(0);
        index--;
    }

    if ((int)data < 0)
    {
        fkanji = kanjis->Kanjis[kanjis->IndexOf(w[1])];
        felement = -1;
        set_kanji_data(fkanji);
    }
    else
    {
        felement = (int)data;
        fkanji = NULL;
        set_elem_data(felement);
    }
    btnBack->Enabled = history->Count;
    btnHistory->Enabled = history->Count;
}
//---------------------------------------------------------------------------
void TfInfo::roundbounds(int fw, int fh)
{
    fw += 8;
    fh += 8;
    HRGN rgn = CreateRoundRectRgn(0, 0, fw + 1, fh + 1, 9, 9);
    SetWindowRgn(Handle, rgn, Visible);
}
//---------------------------------------------------------------------------
extern bool ThemesAreTurnedOn;
void TfInfo::cbgptr(TCanvas *c, TRect const &r)
{
    if (ThemesAreTurnedOn)
    {
        TRect r2 = Rect(pTitle->Left, 0, ClientWidth, b0->Top);
        OffsetRect(&r2, -r.Left, -r.Top);
        DrawCaptionBar(c, r2, Active);
    }
    else
        c->FillRect(Rect(0, 0, r.Width(), r.Height()));
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::FormPaint(TObject *Sender)
{
    if (ThemesAreTurnedOn)
    {
        DrawCaptionBar(Canvas, Rect(pTitle->Left, 0, ClientWidth, b0->Top), Active);
        DrawPillowPanel(Canvas, Rect(pSize->Left + pSize->Width, 0, pTitle->Left, b0->Top));
        if (pRef->Visible)
            DrawPillowPanel(Canvas, ChildRect(pRef));
        DrawButtonBar(Canvas, ChildRect(pButtons, 0, 2, 0, 0));
        DrawGrad(Canvas, clWhite, clBtnFace, ChildRect(pSOD));
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::pbKanjiMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    GetCursorPos(&mmousept);
    if (pbKanji->Width == ClientWidth)
    {
        HideKanjiHint(false);
    }
}
//---------------------------------------------------------------------------
void TfInfo::start_moving()
{
    mmoving = true;

    ReleaseCapture();
    DefWindowProc(Handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}
//---------------------------------------------------------------------------
void TfInfo::stop_moving()
{
    mmousept = Point(-9999, -9999);

    if (!mmoving)
        return;

    mmoving = false;

    DefWindowProc(Handle, WM_NCLBUTTONUP, HTCAPTION, 0);
    ReleaseCapture();
}
//---------------------------------------------------------------------------
void TfInfo::ElementsChanged(bool available)
{
    if (!available)
    {
        if (sbOrder->Down)
            sbOrder->Down = false;
        if (sbParts->Down)
            sbParts->Down = false;
        if (sbParents->Down)
            sbParents->Down = false;
        sbOrder->Visible = false;
        sbParts->Visible = false;
        sbParents->Visible = false;
    }
    else
    {
        sbOrder->Visible = true;
        sbParts->Visible = true;
        sbParents->Visible = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::pmHidePopup(TObject *Sender)
{
    bool vis = false;
    if (pmHide->PopupComponent == pbSimilar || pmHide->PopupComponent == pbParts || pmHide->PopupComponent == pbParents)
    {
        if (pmHide->PopupComponent == pbSimilar)
            vis = posimilar->KanjiAtPos(pbSimilar->ScreenToClient(pmHide->PopupPoint).x) >= 0;
        if (pmHide->PopupComponent == pbParts)
            vis = poparts->KanjiAtPos(pbParts->ScreenToClient(pmHide->PopupPoint).x) >= 0;
        if (pmHide->PopupComponent == pbParents)
            vis = poparents->KanjiAtPos(pbParents->ScreenToClient(pmHide->PopupPoint).x) >= 0;
    }
    miCopy->Visible = vis;
    miAppend->Visible = vis;
}
//---------------------------------------------------------------------------

void __fastcall TfInfo::miCopyClick(TObject *Sender)
{
    int k = -1;
    if (pmHide->PopupComponent == pbSimilar)
        k = posimilar->KanjiAtPos(pbSimilar->ScreenToClient(pmHide->PopupPoint).x);
    if (pmHide->PopupComponent == pbParts)
        k = poparts->KanjiAtPos(pbParts->ScreenToClient(pmHide->PopupPoint).x);
    if (pmHide->PopupComponent == pbParents)
        k = poparents->KanjiAtPos(pbParents->ScreenToClient(pmHide->PopupPoint).x);
    if (k < 0)
        return;

    ClipReplace(kanjis->Kanjis[k]->ch);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::miAppendClick(TObject *Sender)
{
    int k = -1;
    if (pmHide->PopupComponent == pbSimilar)
        k = posimilar->KanjiAtPos(pbSimilar->ScreenToClient(pmHide->PopupPoint).x);
    if (pmHide->PopupComponent == pbParts)
        k = poparts->KanjiAtPos(pbParts->ScreenToClient(pmHide->PopupPoint).x);
    if (pmHide->PopupComponent == pbParents)
        k = poparents->KanjiAtPos(pbParents->ScreenToClient(pmHide->PopupPoint).x);
    if (k < 0)
        return;

    ClipAppend(kanjis->Kanjis[k]->ch);
}
//---------------------------------------------------------------------------
UnicodeString TfInfo::kanjidata()
{
    if (!fkanji)
        return "";

    UnicodeString s;

    s += (UnicodeString)L"Kanji information for the kanji: " + fkanji->ch + "\n\n";

    if (fkanji->oncnt)
    {
        s += (UnicodeString)L"ON readings:\n";
        for (byte ix = 0; ix < fkanji->oncnt; ++ix)
        {
            if (ix)
                s += L"、";
            s += fkanji->on[ix];
        }
        s += "\n\n";
    }

    if (fkanji->kuncnt)
    {
        s += (UnicodeString)L"Kun readings:\n";
        for (byte ix = 0; ix < fkanji->kuncnt; ++ix)
        {
            if (ix)
                s += L"、";
            s += fkanji->kun[ix];
        }
        s += "\n\n";
    }

    s += L"Meaning:\n";
    if (fcollection == 0 || !dictionary->Items[fcollection]->kanjidat[fkanji->index].meaning)
    {
        s += fkanji->meaning;
    }
    else
    {
        s += dictionary->Items[fcollection]->kanjidat[fkanji->index].meaning;
    }
    s += L"\n\n";

    s += (UnicodeString)L"Stroke count: " +int(fkanji->strokes) + "\n\n";

    s += L"Data and reference numbers:\n";

    s += (UnicodeString)L"Radical: " + radicals[fkanji->radical - 1] + " #" +int(fkanji->radical) + "\n";

    for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
    {
        UnicodeString tmps = reference_number((TKanjiDataItem)settings.reference_order[ix]);

        if (tmps != "-")
            s += (UnicodeString)fullrefstr[settings.reference_order[ix]] + ": " + tmps + "\n";
    }

    return s;
}
//---------------------------------------------------------------------------
UnicodeString TfInfo::reference_number(TKanjiDataItem item)
{
    UnicodeString result;

    if (!fkanji)
        return result;

    switch (item)
    {
    case kdiUnicode:
        result = "0x" + wtoAS(fkanji->_uni).UpperCase();
        break;
    case kdiJIS:
        result = "0x" + wtoAS(fkanji->_jis).UpperCase();
        break;
    case kdiSJIS:
        result = "0x" + wtoAS(JIStoShiftJIS(fkanji->_jis)).UpperCase();
        break;
    case kdiEUC:
        result = "0x" + wtoAS(JIStoEUC(fkanji->_jis)).UpperCase();
        break;
    case kdiFrequency:
        result = fkanji->frequency ? UnicodeString(int(fkanji->frequency)) : UnicodeString("-");
        break;
    case kdiKuten:
        result = JIStoKuten(fkanji->_jis);
        break;
    case kdiJouyou:
        if (fkanji->jouyou > 0 && fkanji->jouyou <= 10)
        {
            if (fkanji->jouyou < 9)
                result = int(fkanji->jouyou);
            else
                result = (UnicodeString)L"名" +int(fkanji->jouyou);
        }
        else
            result = "-";
        break;
    case kdiSnH:
        if (fkanji->SnH[0] != 'x')
            result = fkanji->SnH;
        else
            result = "-";
        break;
    case kdiKnK:
        if (fkanji->knk > 0)
            result = int(fkanji->knk);
        else
            result = "-";
        break;
    case kdiNelson:
        if (fkanji->nelson > 0)
            result = int(fkanji->nelson);
        else
            result = "-";
        break;
    case kdiHalpern:
        if (fkanji->halpern > 0)
            result = int(fkanji->halpern);
        else
            result = "-";
        break;
    case kdiHeisig:
        if (fkanji->heisig > 0)
            result = int(fkanji->heisig);
        else
            result = "-";
        break;
    case kdiGakken:
        if (fkanji->gakken > 0)
            result = int(fkanji->gakken);
        else
            result = "-";
        break;
    case kdiHenshall:
        if (fkanji->henshall > 0)
            result = int(fkanji->henshall);
        else
            result = "-";
        break;
    case kdiSKIP:
        result = (UnicodeString)int(fkanji->SKIP1) + "-" +int(fkanji->SKIP2) + "-" +int(fkanji->SKIP3);
        break;
    case kdi4C:
        if (fkanji->c4[0] != 'x')
            result = fkanji->c4 + (UnicodeString)"." + fkanji->c4b;
        else
            result = "-";
        break;
    case kdiJLPTold:
        if (fkanji->JLPT)
            result = int(fkanji->JLPT);
        else
            result = "-";
        break;
    case kdiNJLPT:
        if (fkanji->NJLPT)
            result = UnicodeString(L"N") +int(fkanji->NJLPT);
        else
            result = "-";
        break;
    }
    return result;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::FormShow(TObject *Sender)
{
    static bool firsttime = true;

    if (!firsttime)
        return;
    firsttime = false;

    sbRef->Down = sbRef->Down;
    sbOrder->Down = sbOrder->Down;
    sbOrder->Click();
    sbSimilar->Down = sbSimilar->Down;
    sbParts->Down = sbParts->Down;
    sbParents->Down = sbParents->Down;
    aRefPanel->Checked = sbRef->Down;
    aSimilarPanel->Checked = sbSimilar->Down;
    aPartsPanel->Checked = sbParts->Down;
    aParentsPanel->Checked = sbParents->Down;

    build_refmenu();
    refview_setall();
    update_references();
}
//---------------------------------------------------------------------------
void TfInfo::build_refmenu()
{
    while (miRefView->Count > 2)
    {
        TMenuItem *mi = miRefView->Items[2];
        miRefView->Delete(2);
        delete mi;
    }

    for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
    {
        TMenuItem *mi = new TMenuItem(pmRef);
        mi->Caption = EscapeLabel(refstr[settings.reference_order[ix]]);
        mi->AutoCheck = true;
        mi->Checked = settings.referenceshow[settings.reference_order[ix]];
        mi->Tag = settings.reference_order[ix];
        mi->OnClick = miRefViewClick;
        miRefView->Add(mi);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::miRefAllClick(TObject *Sender)
{
    for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
    {
        settings.referenceshow[settings.reference_order[ix]] = !((TMenuItem*)Sender)->Tag;
        miRefView->Items[ix + 2]->Checked = settings.referenceshow[settings.reference_order[ix]];
    }

    update_references();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::edRefMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    pbKanji->SetFocus();
}
//---------------------------------------------------------------------------
void TfInfo::update_references()//computations to set the position of reference data labels
{
    refview_setall();

    pRef->Width = ClientWidth;
    pRef->Left = 0;
    int linespacing = ClientWidth / 143 / scaleres;
    int paddingtop = 2 * ((double)linespacing / 3) * scaleres * scaleres;

    int cnt = 0;
    for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
        if (settings.referenceshow[ix])
            cnt++;
    int columns = cnt == 1 ? 1 : cnt <= 6 ? 2 : 3;
    int lines = max(1, (cnt + columns - 1) / columns);
    pRef->Height = 2 + paddingtop + (titles[0]->Height + linespacing * scaleres) * lines;
    if (columns <= 2)
    {
        lStrokesTitle->Caption = "Stroke count:";
        lStrokes->AutoSize = true;
        lStrokes->Font->Size = 10;
        lStrokes->AutoSize = false;
        bX->Left = ClientWidth / 3.5454;
        bX->Height = lines * (titles[0]->Height + linespacing) - linespacing;
        bX->Top = 2 + (pRef->Height - 2 - bX->Height) / 2;
        lStrokesTitle->Top = bX->Top + (bX->Height - lStrokesTitle->Height) / 2;
        lStrokes->Top = lStrokesTitle->Top - 2;
        lStrokes->Width = bX->Left / 4.84;
        lStrokes->Left = bX->Left * 0.7107;
        lStrokesTitle->Left = (lStrokes->Left - lStrokesTitle->Width) - lStrokes->Width * 0.24;
    }
    else
    {
        lStrokesTitle->Caption = "Strokes:";
        lStrokes->AutoSize = true;
        lStrokes->Font->Size = lines == 2 ? 10 : 24;
        lStrokes->AutoSize = false;
        bX->Left = ClientWidth / 8.25;
        bX->Height = lines * (titles[0]->Height + linespacing) - linespacing;
        bX->Top = 2 + paddingtop + (pRef->Height - 2 - linespacing - bX->Height) / 2;
        lStrokes->Width = bX->Left * .84615;
        lStrokes->Left = (bX->Left - lStrokes->Width) / 2 + 1;
        lStrokesTitle->Left = (bX->Left - lStrokesTitle->Width) / 2 + 1;
        lStrokesTitle->Top = bX->Top + (bX->Height - (lStrokes->Height + lStrokesTitle->Height)) / 2;
        lStrokes->Top = lStrokesTitle->Top + lStrokesTitle->Height;
    }

    if (cnt == 1)
    {
        refs[0]->Left = ClientWidth * 0.734265 - refs[0]->Width;
        titles[0]->Left = ClientWidth / 3.3;
    }
    else if (columns == 2)
    {
        int ct1 = ClientWidth / 3, c1 = ClientWidth * 0.58042, ct2 = ClientWidth * 0.6806, c2 = ct2 + c1 - ct1;
        for (int ix = 0; ix < cnt; ++ix)
        {
            titles[ix]->Left = !(ix % 2) ? ct1 : ct2;
            refs[ix]->Left = (!(ix % 2) ? c1 : c2) - refs[ix]->Width;
            titles[ix]->Top = 2 + paddingtop + (ix / 2) * (refs[0]->Height + linespacing);
            refs[ix]->Top = 2 + paddingtop + (ix / 2) * (refs[0]->Height + linespacing);
        }
    }
    else
    {
        int ct1 = ClientWidth * 0.13519, c1 = ClientWidth * 0.37296, ct2 = ClientWidth * 0.42657, c2 = ct2 + c1 - ct1, ct3 = 2 * ct2 - ct1, c3 = ct3 + c1 - ct1;
        for (int ix = 0; ix < cnt; ++ix)
        {
            titles[ix]->Left = !(ix % 3) ? ct1 : !((ix - 1) % 3) ? ct2 : ct3;
            refs[ix]->Left = (!(ix % 3) ? c1 : !((ix - 1) % 3) ? c2 : c3) - refs[ix]->Width;
            titles[ix]->Top = 2 + paddingtop + (ix / 3) * (refs[0]->Height + linespacing);
            refs[ix]->Top = 2 + paddingtop + (ix / 3) * (refs[0]->Height + linespacing);
        }
    }

    for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
    {
        titles[ix]->Visible = ix < cnt;
        refs[ix]->Visible = ix < cnt;
    }

    fill_references();
    show_option(ioNone);
}
//---------------------------------------------------------------------------
void TfInfo::fill_references()//set text of reference data labels
{
    if (fkanji)
        lStrokes->Caption = int(fkanji->strokes);
    else if (felement >= 0)
        lStrokes->Caption = elements->Items[felement]->variants[0].strokecount;
    else
        lStrokes->Caption = "-";

    int cnt = 0;
    for (int ix = 0; ix < KANJI_REFERENCE_COUNT; ++ix)
        if (settings.referenceshow[ix])
            cnt++;

    UnicodeString s;
    int index = -1;
    TColor specialcol = (TColor)0x0b13a4;
    fixtextcolor(specialcol, clBtnFace);
    for (int ix = 0; ix < cnt; ++ix)
    {
        do
        {
            index++;
        }
        while (!settings.referenceshow[settings.reference_order[index]]);
        bool special = settings.reference_index == settings.reference_order[index];
        titles[ix]->Caption = EscapeLabel(cnt == 1 ? fullrefstr[settings.reference_order[index]] : refstr[settings.reference_order[index]]) + ":";
        refs[ix]->Caption = "";

        titles[ix]->Font->Color = special ? specialcol : clWindowText;
        refs[ix]->Font->Color = special ? specialcol : clWindowText;

        if (!fkanji)
        {
            refs[ix]->Caption = "-";
            continue;
        }

        refs[ix]->Caption = reference_number((TKanjiDataItem)settings.reference_order[index]);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::miHideRefClick(TObject *Sender)
{
    sbRef->Down = false;
    sbRef->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::pmRefPopup(TObject *Sender)
{
    TRect r = ChildRect(pRef);
    TPoint p = ClientToScreen(Point(0, 0));
    OffsetRect(&r, p.x, p.y);
    miHideRef->Visible = pRef->Visible && PtInRect(&r, pmRef->PopupPoint);
    aTranslateKanji->Visible = modallist->Count == 0;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::pmSODPopup(TObject *Sender)
{
    aTranslateKanji->Visible = modallist->Count == 0;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::miShadowClick(TObject *Sender)
{
    set_shadow(miShadow->Checked);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::miNumbersClick(TObject *Sender)
{
    set_numbers(miNumbers->Checked);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::miDirectionClick(TObject *Sender)
{
    set_dots(miDirection->Checked);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::miSpeed1Click(TObject *Sender)
{
    set_speed(dsNormalSpeed);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::miSpeed2Click(TObject *Sender)
{
    set_speed(dsFast);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::miSpeed3Click(TObject *Sender)
{
    set_speed(dsFastest);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbOptionSClick(TObject *Sender)
{
    TRect r = ChildRect(sbOptionS);
    TPoint p = ClientToScreen(Point(r.Left, r.Bottom + 2));
    pmSOD->Popup(p.x, p.y);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::cbGridMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    pbKanji->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::miRadicalClick(TObject *Sender)
{
    mobjON->reset(fkanji ? fkanji->oncnt : 0);
    mobjKun->reset(fkanji ? fkanji->kuncnt : 0);

    if (pbReadings->VPos != 0)
        pbReadings->VPos = 0;
    else
        pbReadings->Invalidate();
    miRadical->ImageIndex = miRadical->Checked ? -1 : 5;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::pTitleDblClick(TObject *Sender)
{
    if (sbOpenKanji->Visible)
        sbOpenKanji->Click();
    else
        sbCloseKanji->Click();
    Invalidate();
    Application->ProcessMessages();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::pbKanjiMouseLeave(TObject *Sender)
{
    if (pbKanji->Width == ClientWidth)
        HideKanjiHint();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::pbKanjiMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    if (settings.show_kanji_hint && fkanji && mmousept.x == -9999 && mmousept.y == -9999 && Sender == pbKanji && pbKanji->Width == ClientWidth)
        ShowKanjiHint(pbKanji, x, y, fkanji->ch, dictionary->Items[fcollection]);
    else if (mmousept.x != -9999 && mmousept.y != -9999)
    {
        TPoint p;
        GetCursorPos(&p);
        int d = max(abs(mmousept.x - p.x), abs(mmousept.y - p.y));
        if (d > 4)
        {
            Left = Left + (p.x - mmousept.x);
            Top = Top + (p.y - mmousept.y);

            mmousept = Point(-9999, -9999);

            start_moving();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbRefClick(TObject *Sender)
{
    if (sbRef->Enabled)
    {
        aRefPanel->Checked = sbRef->Down;
        show_option(ioRef);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbSimilarClick(TObject *Sender)
{
    if (sbSimilar->Enabled)
    {
        aSimilarPanel->Checked = sbSimilar->Down;
        show_option(ioSimilar);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbPartsClick(TObject *Sender)
{
    if (sbParts->Enabled)
    {
        aPartsPanel->Checked = sbParts->Down;
        show_option(ioParts);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbParentsClick(TObject *Sender)
{
    if (sbParents->Enabled)
    {
        aParentsPanel->Checked = sbParents->Down;
        show_option(ioParents);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::aCopyKanjiExecute(TObject *Sender)
{
    if (!fkanji)
        THROW(L"?");
    ClipReplace(fkanji->ch);
}
//---------------------------------------------------------------------------

void __fastcall TfInfo::aAppendKanjiExecute(TObject *Sender)
{
    if (!fkanji)
        THROW(L"?");
    ClipAppend(fkanji->ch);
}
//---------------------------------------------------------------------------

void __fastcall TfInfo::aCopyRefExecute(TObject *Sender)
{
    if (!fkanji)
        THROW(L"?");
    ClipReplace(kanjidata());
}
//---------------------------------------------------------------------------

void __fastcall TfInfo::aAppendRefExecute(TObject *Sender)
{
    if (!fkanji)
        THROW(L"?");
    ClipAppend(kanjidata());
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::aRefPanelExecute(TObject *Sender)
{
    sbRef->Down = !sbRef->Down;
    sbRef->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::aSimilarPanelExecute(TObject *Sender)
{
    sbSimilar->Down = !sbSimilar->Down;
    sbSimilar->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::aPartsPanelExecute(TObject *Sender)
{
    sbParts->Down = !sbParts->Down;
    sbParts->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::aParentsPanelExecute(TObject *Sender)
{
    sbParents->Down = !sbParents->Down;
    sbParents->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::pbReadingsVScroll(TObject *Sender, TScrollCode ScrollCode, int &ScrollPos)
{
    mobjON->scroll(0, ScrollPos);
    mobjKun->scroll(0, ScrollPos);
}
//---------------------------------------------------------------------------
void TfInfo::doMPrepare(TCanvas *c, int index, bool selected)
{
    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = 12;
    c->Font->Style = TFontStyles();
    c->Font->Color = selected ? clRed : clBlack;
    c->Brush->Color = clWhite;
    c->Brush->Style = bsClear;
}
//---------------------------------------------------------------------------
void TfInfo::doMChange(const TRect &r, int index, int data, bool selected)
{
    BitBlt(pbReadings->Canvas->Handle, r.Left, r.Top, r.Width(), r.Height(), bm2->Canvas->Handle, r.Left, r.Top, SRCCOPY);
}
//---------------------------------------------------------------------------
void TfInfo::doMONMeasure(TCanvas *c, int index, int &x, int &y, TRect &rect, int &data, bool &active)
{
    TSize s;
    GetTextExtentPoint32W(c->Handle, kanji->on[index], wcslen(kanji->on[index]), &s);

    rect = Rect(x, y, x +int(2 * scaleres) * 2 + s.cx, y +int(2 * scaleres) * 2 + s.cy);
    data = index;

    if (index == kanji->oncnt - 1)
        return;
    GetTextExtentPoint32W(c->Handle, kanji->on[index + 1], wcslen(kanji->on[index + 1]), &s);
    if (x != 2 * scaleres && x + rect.Width() + 12 * scaleres + s.cx > pbReadings->ClientWidth)
    {
        x = 2 * scaleres;
        y += s.cy +int(2 * scaleres) * 2;
    }
    else
        x += rect.Width() + 12 * scaleres;
}
//---------------------------------------------------------------------------
void TfInfo::doMONDraw(TCanvas *c, const TRect &r, int index, int data, bool selected)
{
    ExtTextOutW(c->Handle, r.Left + 2*scaleres, r.Top + 2*scaleres, ETO_CLIPPED, &r, kanji->on[index], wcslen(kanji->on[index]), NULL);
    if (index < kanji->oncnt - 1)
    {
        TRect r2 = Rect(r.Right, r.Top, r.Right + 12 * scaleres, r.Bottom);
        c->Font->Color = clBlack;
        ExtTextOutW(c->Handle, r2.Left, r2.Top + 2*scaleres, ETO_CLIPPED, &r2, L"、", 1, NULL);
    }
}
//---------------------------------------------------------------------------
void TfInfo::doMKunMeasure(TCanvas *c, int index, int &x, int &y, TRect &rect, int &data, bool &active)
{
    TSize s;
    UnicodeString kun = kanji->kun[index];
    int ppos;
    if ((ppos = kun.Pos(L".")) > 0)
        kun = kun.SubString(1, ppos - 1) + kun.SubString(ppos + 1, kun.Length());
    GetTextExtentPoint32W(c->Handle, kun.c_str(), kun.Length(), &s);

    rect = Rect(x, y, x +int(2 * scaleres) * 2 + s.cx, y +int(2 * scaleres) * 2 + s.cy);
    data = index;

    if (index == kanji->kuncnt - 1)
        return;

    kun = kanji->kun[index + 1];
    ppos;
    if ((ppos = kun.Pos(L".")) > 0)
        kun = kun.SubString(1, ppos - 1) + kun.SubString(ppos + 1, kun.Length());
    GetTextExtentPoint32W(c->Handle, kun.c_str(), kun.Length(), &s);
    if (x != 2 * scaleres && x + rect.Width() + 12 * scaleres + s.cx > pbReadings->ClientWidth)
    {
        x = 2 * scaleres;
        y += s.cy +int(2 * scaleres) * 2;
    }
    else
        x += rect.Width() + 12 * scaleres;
}
//---------------------------------------------------------------------------
void TfInfo::doMKunDraw(TCanvas *c, const TRect &r, int index, int data, bool selected)
{
    int ppos;
    UnicodeString kun = kanji->kun[index];
    if ((ppos = kun.Pos(L".")) > 0)
        kun = kun.SubString(1, ppos - 1) + kun.SubString(ppos + 1, kun.Length());

    ExtTextOutW(c->Handle, r.Left + 2*scaleres, r.Top + 2*scaleres, ETO_CLIPPED, &r, kun.c_str(), ppos && !selected ? ppos - 1 : kun.Length(), NULL);
    if (ppos && !selected)
    {
        TSize s;
        GetTextExtentPoint32W(c->Handle, kun.c_str(), ppos - 1, &s);
        c->Font->Color = (TColor)0xffb000;
        ExtTextOutW(c->Handle, r.Left + 2*scaleres + s.cx, r.Top + 2*scaleres, ETO_CLIPPED, &r, kun.c_str() + ppos - 1, kun.Length() - ppos + 1, NULL);
    }
    if (index < kanji->kuncnt - 1)
    {
        TRect r2 = Rect(r.Right, r.Top, r.Right + 12 * scaleres, r.Bottom);
        c->Font->Color = clBlack;
        ExtTextOutW(c->Handle, r2.Left, r2.Top + 2*scaleres, ETO_CLIPPED, &r2, L"、", 1, NULL);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::pbReadingsClick(TObject *Sender)
{
    if (fkanji && mobjON->Position >= 0)
    {
        TKanji *k = kanji;
        fKanji->sbReset->Click();
        bool doreset = settings.kanjireset;
        settings.kanjireset = false;

        fKanji->kanji = k;
        fKanji->edreading->Text = k->on[mobjON->Position];
        fKanji->cbKun->Checked = false;
        fKanji->cbOku->Checked = false;
        fKanji->cbReading->Checked = true;

        if (settings.intray && !fKanji->Visible)
            fKanji->Popup(true, false);
        settings.kanjireset = doreset;
    }
    if (fkanji && mobjKun->Position >= 0)
    {
        TKanji *k = kanji;
        UnicodeString kn = k->kun[mobjKun->Position];
        UnicodeString kj = kn;
        int p = kn.Pos(L".");
        if (p)
        {
            kn = kn.SubString(1, p - 1) + kn.SubString(p + 1, kn.Length());
            kj = UnicodeString(k->ch) + kj.SubString(p + 1, kj.Length());
        }
        else
            kj = k->ch;
        Translate(collection, kj, kn);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::pbKanjiMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    mmousept = Point(-9999, -9999);
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::pControlMouseEnter(TObject *Sender)
{
    show_controlpanel();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::pControlMouseLeave(TObject *Sender)
{
    hide_controlpanel();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbOptionSMouseEnter(TObject *Sender)
{
    show_controlpanel();
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::sbOptionSMouseLeave(TObject *Sender)
{
    hide_controlpanel();
}
//---------------------------------------------------------------------------
void TfInfo::show_controlpanel()
{
    if (ctrlaction == ctrlVisible)
        return;
    ctrlaction = ctrlShow;
    tControl->Enabled = true;
}
//---------------------------------------------------------------------------
void TfInfo::hide_controlpanel()
{
    if (ctrlaction == ctrlHidden)
        return;
    ctrlaction = ctrlaction == ctrlShow ? ctrlHideAfterShow : ctrlHide;
    tControl->Enabled = true;
}
//---------------------------------------------------------------------------
void TfInfo::remove_controlpanel()
{
    ctrlaction = ctrlHidden;
    pControl->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::tControlTimer(TObject *Sender)
{
    const int speeds[] = { 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 5 };

    int dif;
    switch (ctrlaction)
    {
    case ctrlHidden:
        tControl->Enabled = false;
        pControl->Visible = false;
        pControl->Top = ctrltop + ctrlheight - 1;
        pControl->Height = 1;
        break;
    case ctrlVisible:
        tControl->Enabled = false;
        break;
    case ctrlHideAfterShow:
    case ctrlShow:
        if (!pControl->Visible)
        {
            pControl->Visible = true;
            break;
        }

        if (pControl->Height == ctrlheight)
        {
            if (ctrlaction == ctrlHideAfterShow)
                ctrlaction = ctrlHide;
            else
            {
                tControl->Enabled = false;
                ctrlaction = ctrlVisible;
            }
            break;
        }
        dif = pControl->Height <= 10 ? speeds[pControl->Height] : ctrlheight - pControl->Height <= 10 ? speeds[ctrlheight - pControl->Height] : 6;
        dif = min(dif, ctrlheight - pControl->Height);
        pControl->SetBounds(pControl->Left, pControl->Top - dif, pControl->Width, pControl->Height + dif);
        break;
    case ctrlHide:
        if (pControl->Height == 1)
        {
            tControl->Enabled = false;
            ctrlaction = ctrlHidden;
            pControl->Visible = false;
            break;
        }
        dif = pControl->Height <= 10 ? speeds[pControl->Height] : ctrlheight - pControl->Height <= 10 ? speeds[ctrlheight - pControl->Height] : 6;
        dif = min(dif, pControl->Height - 1);
        pControl->SetBounds(pControl->Left, pControl->Top + dif, pControl->Width, pControl->Height - dif);
        break;
    }
}
//---------------------------------------------------------------------------
void TfInfo::set_strokeorder(bool val)
{
    if (val == fstrokeorder)
        return;
    fstrokeorder = val;
    if (fkanji || felement < 0)
    {
        sbOrder->Down = fstrokeorder;
        sbOrder->Click();
    }
}
//---------------------------------------------------------------------------
void TfInfo::WMMenuChar(TMessage &msg)
{
    msg.Result = MNC_CLOSE << 16;
}
//---------------------------------------------------------------------------
void __fastcall TfInfo::aTranslateKanjiExecute(TObject *Sender)
{
    if (!fkanji)
        THROW(L"?");
    if (!settings.intray)
        fDict->Search(fcollection, fkanji->ch);
    else
    {
        if (!fPopupDict->Visible)
            fPopupDict->Popup(true);
        fPopupDict->Search(fcollection, fkanji->ch);
    }
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(89);
#endif


