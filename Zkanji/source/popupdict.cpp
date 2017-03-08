//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "popupdict.h"
#include "wordlist.h"
#include "kanjiinfo.h"
#include "clipview.h"
#include "dropdown.h"
#include "main.h"
#include "wordobj.h"
#include "collection.h"
#include "sentences.h"
#include "settings.h"
#include "words.h"
#include "module.h"
#include "grouplist.h"
#include "kanji.h"
#include "vocabform.h"
#include "lineobj.h"
#include "mouseevent.h"
#include "kanjihint.h"
#include "kanjilist.h"
#include "modallist.h"
#include "recognizer.h"
#include "gengraph.h"
#include "genworddata.h"
#include "wordfilter.h"
#include "docker.h"
#include "closebutton.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(122);
#endif

TfPopupDict *fPopupDict;
//---------------------------------------------------------------------------

extern bool ThemesAreTurnedOn;

void TfPopupDict::WMNCHitTest(TWMNCHitTest &msg)
{
    if (msg.XPos >= Left && msg.XPos < Left + 4)
    {
        msg.Result = HTLEFT;
        if (msg.YPos >= Top && msg.YPos < Top + 8)
            msg.Result = HTTOPLEFT;
        else if (floating && msg.YPos >= Top + Height - 5 && msg.YPos < Top + Height)
            msg.Result = HTBOTTOMLEFT;
    }
    else if (msg.YPos >= Top && msg.YPos < Top + 4)
    {
        msg.Result = HTTOP;
        if (msg.XPos >= Left && msg.XPos < Left + 8)
            msg.Result = HTTOPLEFT;
        else if (floating && msg.XPos >= Left + Width - 5 && msg.XPos < Left + Width)
            msg.Result = HTTOPRIGHT;
    }
    else if (floating && msg.XPos >= Left + Width - 5 && msg.XPos < Left + Width)
    {
        msg.Result = HTRIGHT;
        if (msg.YPos >= Top + Height - 5 && msg.YPos < Top + Height)
            msg.Result = HTBOTTOMRIGHT;
    }
    else if (floating && msg.YPos >= Top + Height - 5 && msg.YPos < Top + Height)
    {
        msg.Result = HTBOTTOM;
    }
    else if (msg.XPos == Width - 1 || msg.YPos == Height - 1)
        msg.Result = HTBORDER;
    else
        msg.Result = floating ? HTCAPTION : HTCLIENT;
}
//---------------------------------------------------------------------------
void TfPopupDict::WMNCCalcSize(TMessage &msg)
{
    if (!msg.WParam)
    {
        TRect *r = ((TRect*)msg.LParam);
        r->Left += 4;
        r->Top += 4;
        r->Right -= floating ? 4 : 1;
        r->Bottom -= floating ? 4 : 1;
        msg.Result = 0;
    }
    else
    {
        NCCALCSIZE_PARAMS *n = ((NCCALCSIZE_PARAMS*)msg.LParam);
        memmove(n->rgrc + 1, n->rgrc, sizeof(TRect*)*2);
        n->rgrc->left += 4;
        n->rgrc->top += 4;
        n->rgrc->right -= floating ? 4 : 1;
        n->rgrc->bottom -= floating ? 4 : 1;
    }
}
//---------------------------------------------------------------------------
void TfPopupDict::WMNCPaint(TWMNCPaint &msg)
{
    if (!Visible)
        return;

    msg.Result = 0;
    try
    {
        HDC hdc;
        hdc = GetWindowDC(Handle);
        if (!hdc)
            return;

        TRect r;
        HPEN p;
        HPEN pold;

        GetWindowRect(Handle, &r);
        OffsetRect(r, -r.Left, -r.Top);

        TColor pcol;
        int c = (int)ColorToRGB(clBtnFace);
        bool d = (c & 0xff) >= BTN_DARK_TRESH && ((c >> 8) & 0xff) >= BTN_DARK_TRESH && ((c >> 16) & 0xff) >= BTN_DARK_TRESH;

        HBRUSH b;
        HBRUSH bold;
        if (!floating)
        {
            pcol = d ? COLORDEC(clBtnFace, 0x101010) : COLORINC(clBtnFace, 0x101010);
            if (Active)
                pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.2);
            p = CreatePen(PS_SOLID, 0, pcol);
            pold = (HPEN)SelectObject((HDC)hdc, p);

            TPoint pt[4] = { Point(0, r.Height() - 1), Point(3, r.Height() - 1), Point(3, 3), Point(r.Width() - (ClientWidth - bR->Left + 1), 3) };
            Polyline(hdc, pt, 4);

            SelectObject(hdc, pold);
            DeleteObject(p);

            pcol = d ? COLORDEC(clBtnFace, 0x202020) : COLORINC(clBtnFace, 0x202020);
            if (Active)
                pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.4);
            p = CreatePen(PS_SOLID, 0, pcol);
            pold = (HPEN)SelectObject((HDC)hdc, p);

            TPoint pt2[4] = { Point(0, r.Height() - 1), Point(2, r.Height() - 1), Point(2, 2), Point(r.Width() - (ClientWidth - bR->Left + 1), 2) };
            Polyline(hdc, pt2, 4);

            SelectObject(hdc, pold);
            DeleteObject(p);

            pcol = d ? COLORDEC(clBtnFace, 0x404040) : COLORINC(clBtnFace, 0x404040);
            p = CreatePen(PS_SOLID, 0, pcol);
            pold = (HPEN)SelectObject((HDC)hdc, p);
            MoveToEx(hdc, r.Width() - (ClientWidth - bR->Left + 1), 2, NULL);
            LineTo(hdc, r.Width() - 1, 2);
            MoveToEx(hdc, r.Width() - (ClientWidth - bR->Left + 1), 3, NULL);
            LineTo(hdc, r.Width() - 1, 3);

            SelectObject(hdc, pold);
            DeleteObject(p);

            pcol = d ? COLORDEC(clBtnShadow, 0x404040) : COLORINC(clBtnShadow, 0x404040);
            p = CreatePen(PS_SOLID, 0, pcol);
            pold = (HPEN)SelectObject((HDC)hdc, p);
            TPoint pt3[3] = { Point(r.Width() - 1, 0), Point(r.Width() - 1, r.Height() - 1), Point(-1, r.Height() - 1) };
            Polyline(hdc, pt3, 3);

            ExcludeClipRect(hdc, 2, 2, r.Width(), r.Height());
            ExcludeClipRect(hdc, 0, r.Height() - 1, r.Width(), r.Height());
            ExcludeClipRect(hdc, r.Width() - 1, 0, r.Width(), r.Height());
            LOGBRUSH lb;
            lb.lbStyle = BS_SOLID;
            pcol = d ? COLORDEC(clBtnFace, 0x404040) : COLORINC(clBtnFace, 0x404040);
            if (Active)
                pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.5);
            lb.lbColor = pcol;
            b = CreateBrushIndirect(&lb);
            bold = (HBRUSH)SelectObject(hdc, b);
            RoundRect(hdc, 0, 0, r.Width() + 5, r.Height() + 5, 9, 9);
        }
        else
        {
            pcol = d ? COLORDEC(clBtnFace, 0x202020) : COLORINC(clBtnFace, 0x202020);
            if (Active)
                pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.2);
            p = CreatePen(PS_SOLID, 0, pcol);
            pold = (HPEN)SelectObject(hdc, p);

            TPoint pt2[5] = { Point(2, 2), Point(r.Width() - 3, 2), Point(r.Width() - 3, r.Height() - 3), Point(2, r.Height() - 3), Point(2, 2) };
            Polyline(hdc, pt2, 5);
            SelectObject(hdc, pold);
            DeleteObject(p);

            pcol = d ? COLORDEC(clBtnFace, 0x101010) : COLORINC(clBtnFace, 0x101010);
            if (Active)
                pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.4);
            p = CreatePen(PS_SOLID, 0, pcol);
            pold = (HPEN)SelectObject(hdc, p);

            TPoint pt[5] = { Point(3, 3), Point(r.Width() - 4, 3), Point(r.Width() - 4, r.Height() - 4), Point(3, r.Height() - 4), Point(3, 3) };
            Polyline(hdc, pt, 5);
            SelectObject(hdc, pold);
            DeleteObject(p);

            pcol = d ? COLORDEC(clBtnShadow, 0x404040) : COLORINC(clBtnShadow, 0x404040);
            p = CreatePen(PS_SOLID, 0, pcol);
            pold = (HPEN)SelectObject(hdc, p);

            ExcludeClipRect(hdc, 2, 2, r.Width() - 2, r.Height() - 2);
            LOGBRUSH lb;
            lb.lbStyle = BS_SOLID;
            pcol = d ? COLORDEC(clBtnFace, 0x404040) : COLORINC(clBtnFace, 0x404040);
            if (Active)
                pcol = coloravg(pcol, (TColor)colorsettings.activeborder, 0.5);
            lb.lbColor = pcol;
            b = CreateBrushIndirect(&lb);
            bold = (HBRUSH)SelectObject(hdc, b);
            RoundRect(hdc, 0, 0, r.Width(), r.Height(), 9, 9);
        }
        SelectObject(hdc, bold);
        SelectObject(hdc, pold);
        DeleteObject(p);
        DeleteObject(b);
        ReleaseDC(Handle, hdc);

    }
    catch (...)
    {
        ;
    }
}
//---------------------------------------------------------------------------
__fastcall TfPopupDict::TfPopupDict(TComponent* Owner) : kanjichanged(false), kanjisavetime(0), romajichanged(false), romajisavetime(0), lockcnt(0), pinned(false), floating(false), inherited(Owner)
{
    pToolbar->Margins->Right = 20;
    pExamples->Margins->Right = 20;
    pbWords->Margins->Right = 20;
    bR->Left = ClientWidth - 20;

    BorderStyle = Forms::bsNone;
    BorderIcons = TBorderIcons();

    Constrain(320*scaleres, 120*scaleres);
    KeepSize = true;

    EraseBackground = false;
    AppWindow = true;

    OnTop = true;

    //initial setting in case we medled with the form
    if (pKanji->Visible)
        btnKanji->Down = true;
    if (pRomaji->Visible)
        btnRomaji->Down = true;
    if (pJapanese->Visible)
        btnJapanese->Down = true;

    fnoupdate = 0;
    wobj = new TWordListObj(pbWords, rowsizes(settings.popfontsize), wcallback, data->PopupWord);
    wobj->showgroup = true;
    edkanji = new TKanjiEdit(edKanji, true, true, kanjihistory, ecallback);
    edkanji->EnableRecognizer(sbRecognize);
    edjapanese = new TKanjiEdit(edJapanese, false, false, NULL, jcallback);
    lobj = new TLineObj(pbExamples, true, lcallback);

    numJump = new TNumEdit(edExJump, 1, 1, false);
    numJump->callback = &xcallback;
    numJump->exit_triggers = false;

    closebtn = new TMidCloseButton(this, sbpTopRight, 1, 1, &btndrawfunc, &closefunc);
    pinbtn = new TPinButton(this, sbpTopRight, 1, 1, &btndrawfunc, &pinfunc);
    minbtn = new TMinButton(this, sbpTopRight, 19, 1, &btndrawfunc, &minfunc);
    floatbtn = new TFloatButton(this, sbpTopRight, 1, 19, &btndrawfunc, &floatfunc);
    minbtn->Visible = false;

    SettingsChangeEvent(this, settingsChanged, true);

    AppActivateEvent(this, repaintProc, true, true);
    AppActivateEvent(this, repaintProc, false, true);
    AddEvent(this, repaintProc, eltActivate);
    AddEvent(this, repaintProc, eltDeactivate);

    Shadowed = true;
}
//---------------------------------------------------------------------------
void TfPopupDict::settingsChanged(void *caller, TSettingsProcState state)
{
    if (state != spsEnd)
        return;

    wobj->rowsize = rowsizes(settings.popfontsize);
    wobj->headersize = rowsizes(settings.popfontsize);
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::FormDestroy(TObject *Sender)
{
    AppActivateEvent(this, repaintProc, false, false);
    AppActivateEvent(this, repaintProc, true, false);
    RemoveEvents(this);

    SettingsChangeEvent(this, settingsChanged, false);

    delete numJump;
    delete lobj;
    delete edjapanese;
    delete edkanji;
    delete wobj;
    delete closebtn;
    delete pinbtn;
    delete minbtn;
    delete floatbtn;
}
//---------------------------------------------------------------------------
void TfPopupDict::lcallback()
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

        //TWordExampleData *exw = Examples->FindData(lobj->Destination->words->Items[lobj->WordIndex]);
        //TWordExampleData *ex = wobj->pos >= 0 ? Examples->FindData(wobj->words[wobj->pos]) : NULL;

        TStoredWord *exw = store->FindWord(lobj->Collection->words->Items[lobj->WordIndex]);
        TStoredWord *ex = wobj->pos >= 0 ? store->FindWord(wobj->words[wobj->pos]) : NULL;

        if (!ex || !ex->examplecnt || !exw || !exw->examplecnt)
            THROW(L"Fix this when there's a problem");

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
void TfPopupDict::wcallback()
{
    bool setex = false;
    if (wobj->count && Examples)
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
void TfPopupDict::ecallback()
{
    kanjichanged = !edkanji->Text.Trim().IsEmpty();
    kanjisavetime = lasttime;
    UpdateWords(false);
}
//---------------------------------------------------------------------------
void TfPopupDict::jcallback()
{
    if (!pJapanese->Visible)
        return;
    wobj->find_in_continuous(edjapanese->Text.c_str());
}
//---------------------------------------------------------------------------
void TfPopupDict::repaintProc(void *caller)
{
    if (!Visible)
        return;

    SendMessage(Handle, WM_NCPAINT, 0, 0);
    closebtn->Redraw();
    pinbtn->Redraw();
    minbtn->Redraw();
    floatbtn->Redraw();

    if (floating)
    {
        TRect r = Rect(0, 0, ClientWidth, pToolbar->Top);
        RedrawWindow(Handle, &r, NULL, RDW_UPDATENOW | RDW_INVALIDATE | RDW_ALLCHILDREN);
    }
}
//---------------------------------------------------------------------------
void TfPopupDict::WordChanged(byte coll, TChangedWordList *list)
{
    if (coll != settings.dictionary_index)
        return;

    UpdateWords(list->Count != 1 || list->Items[0]->type != wctDelete);
}
//---------------------------------------------------------------------------
void TfPopupDict::UpdateWords(bool holdposition)
{
    if (noupdate)
        return;

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
        return;
    }

    TWordIndexList *l = new TWordIndexList(dictionary->Items[settings.dictionary_index]/*,false*/);

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
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::edRomajiChange(TObject *Sender)
{
    romajichanged = !edRomaji->Text.Trim().IsEmpty();
    romajisavetime = lasttime;
    UpdateWords(false);
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::btnKanjiClick(TObject *Sender)
{
    noupdate = true;
    try
    {
        if (pKanji->Visible)
        {
            if (!has_kanji(edkanji->Text.c_str()))
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
            ActiveControl = edRomaji;
            edRomaji->SelectAll();
        }
        else if (Sender == btnKanji)
        {
            pKanji->Visible = true;
            pRomaji->Visible = false;
            pJapanese->Visible = false;
            ActiveControl = edKanji;
            edkanji->SelectAll();
        }
        else if (Sender == btnJapanese)
        {
            pJapanese->Visible = true;
            pRomaji->Visible = false;
            pKanji->Visible = false;
            ActiveControl = edJapanese;
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
void __fastcall TfPopupDict::edRomajiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
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
void __fastcall TfPopupDict::edRomajiKeyPress(TObject *Sender, wchar_t &Key)
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
void __fastcall TfPopupDict::FormKeyPress(TObject *Sender, wchar_t &Key)
{
    if (!Visible)
        return;

    if (Key == 0x06) // Ctrl-F
    {
        btnFilter->Down = !btnFilter->Down;
        btnFilter->Click();
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (!Visible)
        return;

    bool inf;
    if (!Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt) && !Shift.Contains(ssShift))
    {
        switch (Key)
        {
        case VK_F2:
            btnKanji->Click();
            break;
        case VK_F3:
            btnRomaji->Click();
            break;
        case VK_F4:
            btnJapanese->Click();
            Key = 0;
            break;
        case VK_F5:
            if (sbExamples->Enabled)
            {
                sbExamples->Down = !sbExamples->Down;
                sbExamples->Click();
            }
            break;
        case VK_F6:
            if (pKanji->Visible && sbKanjiStart->Enabled)
            {
                sbKanjiStart->Down = !sbKanjiStart->Down;
                sbKanjiStart->Click();
            }
            break;
        case VK_F7:
            if (pKanji->Visible && sbKanjiEnd->Enabled)
            {
                sbKanjiEnd->Down = !sbKanjiEnd->Down;
                sbKanjiEnd->Click();
            }
            else if (pRomaji->Visible && sbRomajiEnd->Enabled)
            {
                sbRomajiEnd->Down = !sbRomajiEnd->Down;
                sbRomajiEnd->Click();
            }
            break;
        case VK_F8:
            if (pKanji->Visible && sbInfCon->Enabled)
            {
                sbInfCon->Down = !sbInfCon->Down;
                sbInfCon->Click();
            }
            break;
        case VK_ESCAPE:
            inf = InfoWindowVisible();
            if (inf && !edkanji->Focused() && !edjapanese->Focused())
            {
                HideInfoWindows(true);
                Key = 0;
            }
            if (!inf && edkanji->CanInput() && edjapanese->CanInput() && !sbRecognize->Down)
                Popup(false);
        }
    }

    if (Shift.Contains(ssCtrl) && (Key == 'f' || Key == 'F'))
    {
//        btnFilter->Down = !btnFilter->Down;
//        btnFilter->Click();
        Key = 0;
    }

    if (Shift.Contains(ssAlt) && Key >= '1' && Key <= '9' && dictionary->Count > Key - '1')
        fDict->SelectDictionary(Key - '1');
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::edRomajiExit(TObject *Sender)
{
    if (pRomaji->Visible && romajihistory)
    {
        romajihistory->Add(edRomaji->Text.Trim(), NULL);
        romajihistory->FillComboBox(edRomaji);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::FormActivate(TObject *Sender)
{
    edRomajiExit(Sender);
    pbWords->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::FormDeactivate(TObject *Sender)
{
    edRomajiExit(Sender);
    pbWords->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::pbWordsKeyPress(TObject *Sender, wchar_t &Key)
{
    if (!Visible)
        return;

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
void __fastcall TfPopupDict::pbWordsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
//    if (!Visible)
//        return;
//
//    if (pRomaji->Visible)
//    {
//        if (Key == VK_BACK)
//        {
//            edRomaji->SetFocus();
//            edRomaji->SelLength = 0;
//            edRomaji->Text = edRomaji->Text.SubString(1, edRomaji->Text.Length() - 1);
//            edRomaji->SelStart = edRomaji->Text.Length();
//            edRomajiChange(edRomaji);
//        }
//        if (Key == VK_RETURN)
//            edRomaji->SetFocus();
//    }
//    else if (pKanji->Visible || pJapanese->Visible)
//    {
//        TZPaintBox *p = (pKanji->Visible ? edKanji : edJapanese);
//        if (Key >= 'A' && Key <= 'Z')
//        {
//            p->SetFocus();
//            p->OnKeyDown(Sender, Key, Shift);
//        }
//        if (Key == VK_BACK)
//        {
//            p->SetFocus();
//            p->OnKeyDown(Sender, Key, Shift);
//        }
//        if (Key == VK_RETURN)
//            p->SetFocus();
//    }
}
//---------------------------------------------------------------------------
void TfPopupDict::SaveSettings()
{
    settings.popupcolkanji = wobj->colsize[1];
    settings.popupcolkana = wobj->colsize[2];
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::pbWordsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button == mbLeft && Shift.Contains(ssDouble))
    {
        wchar_t c = wobj->KanjiAtPos(x, y);
        if (!c || !KANJI(c))
        {
            if (wobj->IndexAtPos(y) >= 0)
                fGroup->AddVocabulary(dictionary->Items[settings.dictionary_index], wobj->current);
        }
        else
            ShowKanjiInfo(settings.dictionary_index, kanjis->Kanjis[kanjis->IndexOf(c)]);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::sbKanjiStartClick(TObject *Sender)
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
void __fastcall TfPopupDict::sbKanjiEndClick(TObject *Sender)
{
    int i;
    UnicodeString z = edkanji->Text.Trim();
    for (i = 1; i <= z.Length(); i++)
        if (KANJI(z[i]) || VALIDCODE(z[i]))
            break;
    if (i > z.Length() && sbKanjiEnd->Down)
        sbKanjiStart->Down = false;
    sbKanjiStart->Tag = 0;
    sbKanjiEnd->Tag = sbKanjiEnd->Down;
    UpdateWords();
}
//---------------------------------------------------------------------------
void TfPopupDict::set_autosize(bool val)
{
    wobj->autoresize = val;
}
//---------------------------------------------------------------------------
bool TfPopupDict::get_autosize()
{
    return wobj->autoresize;
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::FormPaint(TObject *Sender)
{
    if (!floating)
    {
        if (ThemesAreTurnedOn)
        {
            DrawGrad(Canvas, COLORINC(clBtnFace, 0x181818), COLORDEC(clBtnFace, 0x181818), Rect(0, 0, pToolbar->Width, pToolbar->Height + pToolbar->Top));
            DrawGrad(Canvas, COLORDEC(clBtnFace, 0x0a0a0a), COLORDEC(clBtnFace, 0x404040), Rect(bR->Left + 1, 0, ClientWidth, ClientHeight), false);
        }
    }
    else
    {
        if (ThemesAreTurnedOn)
        {
            DrawCaptionBar(Canvas, Rect(0, 0, pToolbar->Width, pToolbar->Height - 3), Active);
            DrawGrad(Canvas, COLORINC(clBtnFace, 0x181818), COLORDEC(clBtnFace, 0x181818), Rect(0, pToolbar->Top, pToolbar->Width, pToolbar->Height + pToolbar->Top));
        }
        else
        {
            Canvas->Brush->Color = clBtnFace;
            Canvas->FillRect(Rect(0, 0, ClientWidth, pToolbar->Top));
        }
        TRect r = Rect(0, 0, ClientWidth - 40, pToolbar->Top);
        Canvas->Brush->Style = bsClear;
        Canvas->Font->Name = L"Tahoma";
        Canvas->Font->Size = 9;
        Canvas->Font->Style = TFontStyles() << fsBold;
        TSize s;
        GetTextExtentPoint32W(Canvas->Handle, Caption.c_str(), Caption.Length(), &s);
        ExtTextOutW(Canvas->Handle, (r.Width() - s.cx) / 2, 2*scaleres, ETO_CLIPPED, &r, Caption.c_str(), Caption.Length(), NULL);
        Canvas->Brush->Style = bsSolid;
    }

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
void __fastcall TfPopupDict::sbExamplesClick(TObject *Sender)
{
    pExamples->Visible = sbExamples->Down;
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::sbEx1Click(TObject *Sender)
{
    lobj->Mode = lmBoth;
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::sbEx2Click(TObject *Sender)
{
    lobj->Mode = lmJapanese;
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::sbEx3Click(TObject *Sender)
{
    lobj->Mode = lmTranslated;
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::sbExRightMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    lobj->Pos++;
    MouseRepeat(sbExRightMouseDown, Sender, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::sbExLeftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    lobj->Pos--;
    MouseRepeat(sbExLeftMouseDown, Sender, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void TfPopupDict::set_noupdate(bool val)
{
    if (val)
        fnoupdate++;
    else
        fnoupdate--;
    if (fnoupdate < 0)
    {
        fnoupdate = 0;
        THROW(L"Error! Mixed updates!");
    }
}
//---------------------------------------------------------------------------
bool TfPopupDict::get_noupdate()
{
    return fnoupdate;
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::pExPageBtnMouseEnter(TObject *Sender)
{
    ((TPanel*)Sender)->Color = clInfoBk;
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::pExPageBtnMouseLeave(TObject *Sender)
{
    ((TPanel*)Sender)->Color = clBtnFace;
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::pExPageBtnClick(TObject *Sender)
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
void __fastcall TfPopupDict::btnExJumpClick(TObject *Sender)
{
    lobj->Pos = numJump->value - 1;
}
//---------------------------------------------------------------------------
void TfPopupDict::focus_edit()
{
    if (pRomaji->Visible)
    {
        edRomaji->SetFocus();
        edRomaji->SelectAll();
    }
    else if (pKanji->Visible)
    {
        edKanji->SetFocus();
        edkanji->SelectAll();
    }
    else if (pJapanese->Visible)
    {
        edJapanese->SetFocus();
        edjapanese->SelectAll();
    }
}
//---------------------------------------------------------------------------
void TfPopupDict::LockPopup(bool lock)
{
    if (lock)
        lockcnt++;
    else if (lockcnt)
        lockcnt--;
}
//---------------------------------------------------------------------------
void TfPopupDict::Float(bool val)
{
    if (floating == val)
        return;

    if (Visible)
    {
        if (!floating)
        {
            settings.popupwidth = Width;
            settings.popupheight = Height;
        }
        else
            settings.popupfloatrect = BoundsRect;
    }

    SendMessage(Handle, WM_SETREDRAW, FALSE, 0);

    floating = val;

    pExamples->AlignWithMargins = !floating;
    pbWords->AlignWithMargins = !floating;
    pToolbar->Margins->Right = floating ? 0 : 20;
    pToolbar->Margins->Top = floating ? 22 : 0;
    bR->Width = floating ? 999999 : 1;
    bR->Height = floating ? 2 : 999999;
    bR->Shape = floating ? bsTopLine : bsLeftLine;
    bR->Anchors = floating ? (TAnchors() << akLeft << akTop) : (TAnchors() << akRight << akTop);
    bR->Top = floating ? pToolbar->Top - 2 : 0;
    bR->Left = floating ? 0 : pToolbar->Width;
    minbtn->Visible = val;
    floatbtn->Visible = !val;

    if (Visible)
    {
        TRect r = Rect(Left, Top, Left + Width, Top + Height);
        RedrawWindow(DESKTOPHANDLE, &r, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_ERASENOW | RDW_ALLCHILDREN);
        if (!floating)
        {
            r = Rect(0, 0, settings.popuppositionrestore ? settings.popupwidth : 450 * scaleres, settings.popuppositionrestore ? settings.popupheight : 120 * scaleres);
            OffsetRect(&r, Screen->WorkAreaWidth - r.Width() - 2, Screen->WorkAreaHeight - r.Height() - 3);
            SetBounds(r.Left, r.Top, r.Width(), r.Height());
        }
        else
        {
            SetBounds(settings.popuppositionrestore ? settings.popupfloatrect.Left : 50, settings.popuppositionrestore ? settings.popupfloatrect.Top : 50, settings.popuppositionrestore ? settings.popupfloatrect.Width() : 550*scaleres, settings.popuppositionrestore ? settings.popupfloatrect.Height() : 250*scaleres);
        }
    }

    SendMessage(Handle, WM_SETREDRAW, TRUE, 0);
    WINDOWPOS wpos;
    wpos.hwnd = Handle;
    wpos.hwndInsertAfter = NULL;
    wpos.x = Left;
    wpos.y = Top;
    wpos.cx = Width;
    wpos.cy = Height;
    wpos.flags = SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER;
    SendMessage(Handle, WM_WINDOWPOSCHANGED, NULL, (long)&wpos);
    RedrawWindow(Handle, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
    if (closebtn->Visible)
        closebtn->Redraw();
    if (pinbtn->Visible)
        pinbtn->Redraw();
}
//---------------------------------------------------------------------------
void TfPopupDict::Popup(bool show, TDictionaryMode mode)
{
    if (lockcnt)
        return;
    if (!data->Minimized || show == Visible || modallist->Count)
    {
        if (Visible && show)
        {
            SetForegroundWindow(modallist && modallist->Count && modallist->Items[modallist->Count - 1]->Handle ? modallist->Items[modallist->Count - 1]->Handle : Handle);
            if (mode == dmKanji && !pKanji->Visible)
                btnKanji->Click();
            else if (mode == dmRomaji && !pRomaji->Visible)
                btnRomaji->Click();
            else if (mode == dmBrowse && !pJapanese->Visible)
                btnJapanese->Click();
            focus_edit();
        }
        return;
    }

    if (show)
    {
        if (!settings.minimizetotray && fDock && MatchVersion(6, 1))
            PostMessage(fDock->Handle, WM_Z_ENABLE, 0, 0);

        closebtn->Visible = !settings.popupdisappear;
        pinbtn->Visible = settings.popupdisappear;

        AlphaBlend = settings.popuptransparency;
        if (AlphaBlend)
            AlphaBlendValue = 255 - settings.popuptransparency;

        if (!floating)
        {
            TRect r = Rect(0, 0, settings.popuppositionrestore ? settings.popupwidth : 450 * scaleres, settings.popuppositionrestore ? settings.popupheight : 120 * scaleres);
            OffsetRect(&r, Screen->WorkAreaWidth - r.Width() - 2, Screen->WorkAreaHeight - r.Height() - 3);
            SetBounds(r.Left, r.Top, r.Width(), r.Height());
        }
        else
        {
            SetBounds(settings.popuppositionrestore ? settings.popupfloatrect.Left : 50, settings.popuppositionrestore ? settings.popupfloatrect.Top : 50, settings.popuppositionrestore ? settings.popupfloatrect.Width() : 550*scaleres, settings.popuppositionrestore ? settings.popupfloatrect.Height() : 250*scaleres);
        }

        if (settings.popupcolumnreset)
        {
            wobj->colsize[1] = 68 * scaleres;
            wobj->colsize[2] = 88 * scaleres;
        }
        else
        {
            wobj->colsize[1] = settings.popupcolkanji;
            wobj->colsize[2] = settings.popupcolkana;
        }

        bool notupdating = false;

        if (mode == dmKanji)
        {
            if (btnKanji->Down || btnFilter->Down && filterchanged || settings.popupuseclipboard)
            {
                noupdate = true;
                notupdating = true;
            }
            btnKanji->Click();
            if (settings.popupuseclipboard)
            {
                edkanji->SelectAll();
                edkanji->Paste();
                edkanji->SelectAll();
            }
        }
        else if (mode == dmRomaji)
        {
            if (btnRomaji->Down || btnFilter->Down && filterchanged || settings.popupuseclipboard)
            {
                noupdate = true;
                notupdating = true;
            }
            btnRomaji->Click();
            if (settings.popupuseclipboard)
            {
                //edRomaji->SelStart = 0;
                //edRomaji->SelLength = edRomaji->Text.Length();
                edRomaji->SelectAll();
                SendMessage(edRomaji->Handle, WM_PASTE, 0, 0);
                //edRomaji->SelStart = 0;
                //edRomaji->SelLength = edRomaji->Text.Length();
            }
        }
        else
        {
            btnJapanese->Click();
            if (settings.popupuseclipboard)
            {
                edjapanese->SelectAll();
                edjapanese->Paste();
                edjapanese->SelectAll();
            }
        }

        if (notupdating)
            noupdate = false;

        if (btnFilter->Down && filterchanged)
            btnFilter->Click();
        else if (settings.popupuseclipboard)
            UpdateWords();
        filterchanged = false;

        //the first call of SetWindowRgn after the window was shown causes flicker, so make that "flicker" happen now instead of when first changing the window shape for real
        HRGN rgn = CreateRectRgn(0, 0, 0, 0);
        bool err = GetWindowRgn(Handle, rgn) == ERROR;
        if (!err)
        {
            HRGN rgn2 = CreateRectRgn(0, 0, 0, 0);
            SetWindowRgn(Handle, rgn2, false);
        }
        Visible = true;
        if (mode == dmRomaji)
            edRomaji->SelectAll();
        if (!err)
            SetWindowRgn(Handle, rgn, true);
        else
            DeleteObject(rgn);

        SetForegroundWindow(Handle);
    }
    else
    {
        HideKanjiHintNow();
        ReleaseCapture();
        Visible = false;

        pinned = false;
        pinbtn->Down = false;
        settings.popupcolkanji = wobj->colsize[1];
        settings.popupcolkana = wobj->colsize[2];

        if (!floating)
        {
            settings.popupwidth = Width;
            settings.popupheight = Height;
        }
        else
            settings.popupfloatrect = BoundsRect;

        Recreate(false);

        if (!settings.minimizetotray && fDock && fKanji && !fKanji->OnTop && MatchVersion(6, 1))
            PostMessage(fDock->Handle, WM_Z_ENABLE, 1, 0);
    }
}
//---------------------------------------------------------------------------
void TfPopupDict::WMDestroy(TMessage &msg)
{
    if (Visible)
    {
        Popup(false);
    }
    inherited::Dispatch(&msg);
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::FormResize(TObject *Sender)
{
    static TPoint origpos = Point(0, 0);

    HRGN rold = CreateRectRgn(0, 0, 0, 0);
    GetWindowRgn(Handle, rold);
    TRect bold;
    GetRgnBox(rold, &bold);
    HRGN roldbox = CreateRectRgnIndirect(&bold);
    CombineRgn(rold, roldbox, rold, RGN_DIFF);
    DeleteObject(roldbox);

    OffsetRgn(rold, origpos.X, origpos.Y);

    HRGN rgn = CreateRoundRectRgn(0, 0, Width + (floating ? 1 : 5), Height + (floating ? 1 : 5), 9, 9);
    if (!floating)
    {
        HRGN rgn2 = CreateRectRgn(0, 0, Width, Height);
        CombineRgn(rgn, rgn, rgn2, RGN_AND);
        DeleteObject(rgn2);
    }
    OffsetRgn(rgn, Left, Top);

    CombineRgn(rold, rold, rgn, RGN_DIFF);

    OffsetRgn(rgn, -Left, -Top);
    origpos = Point(Left, Top);

    SetWindowRgn(Handle, rgn, false);

    RedrawWindow(DESKTOPHANDLE, NULL, rold, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    DeleteObject(rold);

    TRect r;
    if (floating)
    {
        r = Rect(0, 0, ClientWidth, pToolbar->Height);
        RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }
    else
    {
        r = Rect(bR->Left + bR->Width, 0, ClientWidth, ClientHeight);
        RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }

    if (bold.Width() < Width)
    {
        r = Rect(bold.Right - 8, pToolbar->Top, ClientWidth, ClientHeight);
        RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }

    if (bold.Height() < Height)
    {
        r = Rect(0, bold.Bottom - 8, bold.Width() < Width ? bold.Right - 8 : ClientWidth, ClientHeight);
        RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
    }
}
//---------------------------------------------------------------------------
void TfPopupDict::disable_examples()
{
    if (sbExamples->Down)
    {
        sbExamples->Down = false;
        sbExamplesClick(sbExamples);
    }
    sbExamples->Enabled = false;
    sbExamples->Hint = "Examples data file not found or not a recognized format.";
}
//---------------------------------------------------------------------------
void TfPopupDict::enable_examples()
{
    sbExamples->Enabled = true;
    sbExamples->Hint = "Show / hide word examples [F5]";
}
//---------------------------------------------------------------------------
void TfPopupDict::DictionaryDeleted(byte coll)
{
    if (settings.dictionary_index == coll)
        lobj->set_data(dictionary->Items[0], NULL);
}
//---------------------------------------------------------------------------
void TfPopupDict::SelectDictionary(byte coll)
{
    if (!settings.intray)
        noupdate = true;
    else
        UpdateWords();

    TWordCollection *dict = dictionary->Items[coll];
    int imgindex = dict->imgindex;
    if (imgindex < 0)
    {
        imgindex = coll == 0 ? 13 : 11;
    }

    btnKanji->Glyph = NULL;
    btnRomaji->Glyph = NULL;
    data->imgMenu->BkColor = clLime;
    data->imgMenu->GetBitmap(imgindex, btnKanji->Glyph);
    data->imgMenu->GetBitmap(imgindex + 1, btnRomaji->Glyph);
    data->imgMenu->BkColor = Graphics::clNone;
    edkanji->Collection = coll;

    if (!settings.intray)
        noupdate = false;
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::btnFilterMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button != mbRight || x < 0 || y < 0 || x >= btnFilter->Width || y >= btnFilter->Height)
        return;

    TPoint p = btnFilter->ClientToScreen(Point(0, btnFilter->Height + 2));
    data->PopupFilters(mkDictionary, p.x, p.y);

    /*data->pmFilters->Tag = 1;
     TPoint p = btnFilter->ClientToScreen(Point(0,btnFilter->Height+2));
     data->pmFilters->Popup(p.x,p.y);
     */
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::btnFilterClick(TObject *Sender)
{
    UpdateWords();
}
//---------------------------------------------------------------------------
void TfPopupDict::xcallback()
{
    if (numJump->value_valid && btnExJump->Enabled)
        btnExJump->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfPopupDict::btnKanjiMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button != mbRight || fDock->miDictSelect->Count <= 1)
        return;
    TSpeedButton *btn = (TSpeedButton*)Sender;
    TRect r = btn->ClientRect;
    if (!PtInRect(r, Point(x, y)))
        return;

    TPoint p = btn->ClientToScreen(Point(x, y));
    while (fDock->miDictSelect->Count < fDict->pmDict->Items->Count)
    {
        TMenuItem *mi = fDict->pmDict->Items->Items[fDict->pmDict->Items->Count];
        fDict->pmDict->Items->Remove(mi);
        delete mi;
    }
    while (fDock->miDictSelect->Count > fDict->pmDict->Items->Count)
    {
        TMenuItem *mi = new TMenuItem(fDict->pmDict);
        fDict->pmDict->Items->Add(mi);
    }
    for (int ix = 0; ix < fDict->pmDict->Items->Count; ++ix)
    {
        fDict->pmDict->Items->Items[ix]->Caption = fDock->miDictSelect->Items[ix]->Caption;
        fDict->pmDict->Items->Items[ix]->GroupIndex = fDock->miDictSelect->Items[ix]->GroupIndex;
        fDict->pmDict->Items->Items[ix]->RadioItem = fDock->miDictSelect->Items[ix]->RadioItem;
        fDict->pmDict->Items->Items[ix]->Checked = fDock->miDictSelect->Items[ix]->Checked;
        fDict->pmDict->Items->Items[ix]->OnClick = fDock->miDictSelect->Items[ix]->OnClick;
        fDict->pmDict->Items->Items[ix]->Tag = fDock->miDictSelect->Items[ix]->Tag;
    }
    fDict->pmDict->Popup(p.x, p.y);
}
//---------------------------------------------------------------------------
void TfPopupDict::closefunc()
{
    Popup(false);
}
//---------------------------------------------------------------------------
void TfPopupDict::pinfunc()
{
    pinned = !pinned;
}
//---------------------------------------------------------------------------
void TfPopupDict::minfunc()
{
    Float(false);
}
//---------------------------------------------------------------------------
void TfPopupDict::floatfunc()
{
    Float(true);
}
//---------------------------------------------------------------------------
void TfPopupDict::btndrawfunc(TCanvas *c, TRect const &r)
{
    if (ThemesAreTurnedOn)
    {
        TRect r2 = Rect(-1, -1, r.Width() + 2, r.Height() + 2);
        if (!floating)
        {
            DrawGrad(c, COLORDEC(clBtnFace, 0x0a0a0a), COLORDEC(clBtnFace, 0x404040), r2, false);
        }
        else
        {
            DrawCaptionBar(c, r2, Active);
        }
    }
    else
    {
        c->Brush->Color = clBtnFace;
        c->FillRect(Rect(0, 0, r.Width(), r.Height()));
    }
}
//---------------------------------------------------------------------------
void TfPopupDict::SearchWord(byte coll, TWord *w)
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
void TfPopupDict::Search(byte coll, UnicodeString what)
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
void TfPopupDict::Time(__int64 sec)
{
    lasttime = sec;
    if ((!kanjichanged && !romajichanged) || settings.timedhistsave)
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
static TGlobalTest tester2(123);
#endif




