//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "studyreading.h"
#include "collection.h"
#include "wordobj.h"
#include "kanjiobj.h"
#include "settings.h"
#include "studylist.h"
#include "searchtree.h"
#include "words.h"
#include "kanjiedit.h"
#include "romajizer.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(162);
#endif

TfKStudy *fKStudy;
//---------------------------------------------------------------------------

void TZtringList::TWList::FreeItem(wchar_t *item, int ix)
{
    delete[] item;
}
//---------------------------------------------------------------------------
TZtringList::TZtringList()
{
    flist = new TZtringList::TWList;
}
//---------------------------------------------------------------------------
TZtringList::~TZtringList()
{
    delete flist;
}
//---------------------------------------------------------------------------
int TZtringList::Add(UnicodeString str)
{
    wchar_t *t = new wchar_t[str.Length() + 1];
    wcscpy(t, str.c_str());
    return flist->Add(t);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int TKanjiStudy::TItems::Add(int wordindex, char *meaning, byte kpos)
{
    TKanjiStudy::TItem *i = new TKanjiStudy::TItem;
    i->wordindex = wordindex;
    i->meaning = new char[strlen(meaning) + 1];
    strcpy(i->meaning, meaning);
    i->charpos = kpos;
    return inherited::Add(i);
}
//---------------------------------------------------------------------------
void TKanjiStudy::TItems::SaveToFile(FILE *f)
{
    int c = Count;
    fwrite(&c, sizeof(int), 1, f);
    for (int ix = 0; ix < c; ++ix)
    {
        TKanjiStudy::TItem *i = Items[ix];
        fwrite(&i->wordindex, sizeof(int), 1, f);
        fwrite(&i->charpos, sizeof(byte), 1, f);
        /*word w = strlen(i->meaning);
         fwrite(&w,sizeof(word),1,f);
         fwrite(i->meaning,sizeof(char),w,f);
         */
        write(i->meaning, f, sltWord);
    }
}
//---------------------------------------------------------------------------
void TKanjiStudy::TItems::LoadFromFile(FILE *f, int version, TConvertItem **converts, int convertcount)
{
    int c;
    fread(&c, sizeof(int), 1, f);
    Capacity = c;
    for (int ix = 0; ix < c; ++ix)
    {
        TKanjiStudy::TItem *i = new TKanjiStudy::TItem;
        inherited::Add(i);
        fread(&i->wordindex, sizeof(int), 1, f);

        int min = 0, max = convertcount - 1;
        while (min < max)
        {
            int mid = (min + max) / 2;
            if (converts[mid]->original < i->wordindex)
                min = mid + 1;
            else if (converts[mid]->original > i->wordindex)
                max = mid - 1;
            else
                min = max = mid;
        }
        i->wordindex = converts[min]->changed;

        fread(&i->charpos, sizeof(byte), 1, f);
        /*if (version >= 11)
         read(i->meaning,f,sltWord);
         else {*/
        word w;
        fread(&w, sizeof(word), 1, f);
        i->meaning = new char[w + 1];
        if (w)
            fread(i->meaning, sizeof(char), w, f);
        i->meaning[w] = 0;
        //}
    }

    for (int ix = Count - 1; ix >= 0; ix--)
        if (Items[ix]->wordindex == -1)
            Delete(ix);
}
//---------------------------------------------------------------------------
void TKanjiStudy::TItems::FreeItem(TItem *item, int ix)
{
    delete[] item->meaning;
    delete item;
}
//---------------------------------------------------------------------------
TKanjiStudyList::TKanjiStudyList() : fcurrent(-1), inherited()
{
}
//---------------------------------------------------------------------------
void TKanjiStudyList::FreeItem(TKanjiStudy *item, int ix)
{
    if (fcurrent > ix)
        fcurrent--;

    delete item->words;
    delete item;
}
//---------------------------------------------------------------------------
void TKanjiStudyList::SaveToFile(FILE *f)
{
    int c = Count;
    fwrite(&c, sizeof(int), 1, f);
    fwrite(&fcurrent, sizeof(int), 1, f);

    for (int i = 0; i < c; i++)
    {
        TKanjiStudy *k = Items[i];
        fwrite(&k->kanjiindex, sizeof(int), 1, f);
        fwrite(&k->reading, sizeof(byte), 1, f);
        fwrite(&k->correct, sizeof(bool), 1, f);
        fwrite(&k->tries, sizeof(int), 1, f);
        k->words->SaveToFile(f);
    }
}
//---------------------------------------------------------------------------
void TKanjiStudyList::LoadFromFile(FILE *f, int version, TConvertItem **converts, int convertcount)
{
    if (version < 5)
        return;

    int c;
    fread(&c, sizeof(int), 1, f);
    Capacity = c;
    fread(&fcurrent, sizeof(int), 1, f);

    for (int ix = 0; ix < c; ++ix)
    {
        TKanjiStudy *k = new TKanjiStudy;
        fread(&k->kanjiindex, sizeof(int), 1, f);
        fread(&k->reading, sizeof(byte), 1, f);
        fread(&k->correct, sizeof(bool), 1, f);
        fread(&k->tries, sizeof(int), 1, f);
        k->words = new TKanjiStudy::TItems;
        inherited::Add(k);
        k->words->LoadFromFile(f, version, converts, convertcount);
    }

    for (int ix = Count - 1; ix >= 0; ix--)
    {
        if (!Items[ix]->words->Count)
            Delete(ix);
    }
}
//---------------------------------------------------------------------------
int TKanjiStudyList::IndexOf(int kanjiindex, byte reading, int wordindex, bool &hasword)
{
    hasword = false;
    for (int i = 0; i < Count; i++)
    {
        if (Items[i]->kanjiindex == kanjiindex && Items[i]->reading == reading)
        {
            for (int j = 0; j < Items[i]->words->Count && !hasword; j++)
                if (Items[i]->words->Items[j]->wordindex == wordindex)
                    hasword = true;

            return i;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------
void TKanjiStudyList::Next()
{
    fcurrent++;
}
//---------------------------------------------------------------------------
void TKanjiStudyList::Prev()
{
    fcurrent--;
}
//---------------------------------------------------------------------------
int TKanjiStudyList::Add(TWord *w, char *meaning, int wordindex, int kpos)
{
    int reading = FindReading(kpos, w);
    if (reading <= 0)
        return -1;

    bool hasword;
    int kanjiindex = kanjis->IndexOf(w->kanji[kpos]);
    int ix = IndexOf(kanjiindex, reading, wordindex, hasword);

    if (hasword)
        return ix;

    TKanjiStudy *k;
    if (ix < 0)
    {
        k = new TKanjiStudy;
        memset(k, 0, sizeof(TKanjiStudy));
        k->words = new TKanjiStudy::TItems;
        k->kanjiindex = kanjiindex;
        k->reading = reading;
        ix = inherited::Add(k);
    }
    else
        k = Items[ix];

    k->words->Add(wordindex, meaning, kpos);

    return ix;

}
//---------------------------------------------------------------------------
void TKanjiStudyList::RemoveLine(int windex)
{
    for (int i = Count - 1; i >= 0; i--)
    {
        for (int j = Items[i]->words->Count - 1; j >= 0; j--)
        {
            if (Items[i]->words->Items[j]->wordindex == windex)
                Items[i]->words->Delete(j);
            else if (Items[i]->words->Items[j]->wordindex > windex)
                Items[i]->words->Items[j]->wordindex--;
        }
        if (Items[i]->words->Count == 0)
            Delete(i);
    }
}
//---------------------------------------------------------------------------
int __kanji_sortbykanji(TKanjiStudy **a, TKanjiStudy **b)
{
    return (*a)->kanjiindex - (*b)->kanjiindex;
}
void TKanjiStudyList::Sort()
{
    if (current >= 0)
        return;
    inherited::Sort(__kanji_sortbykanji);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------
__fastcall TfKStudy::TfKStudy(TComponent* Owner) : ffull(false), TBaseForm(Owner)
{
    //RescaleStatusbar(sbInfo);
    EraseBackground = false;

    wobj = new TVirtualWordListObj(pbWords, rowsizes(settings.dictfontsize), (TFuncPtr)NULL, (TPopupWordProc)NULL);

    int cw[2] = { 120 * scaleres, 6250 * scaleres };
    TWordColumnType ct[2] = { wcCustom, wcMeaning };
    wobj->set_cols(NULL, 2, cw, NULL, NULL, ct, NULL, 0, NULL);
    wobj->colresize[1] = false;

    wobj->wordselect = false;
    wobj->meaningproc = meaningproc;
    wobj->showhint = false;
    wobj->prepareproc = prepareproc;
    wobj->drawproc = drawproc;

    kobj = new TKanjiDrawObj(pbKanji, (TDrawStrokeStatePtr)NULL);
    edkanji = new TKanjiEdit(edKanji, false, false, NULL, edfunc);
    edkanji->MaxLength = 8;

    edfunc();

    Constrain(660*scaleres + (Width - ClientWidth), 256*scaleres + (Height - ClientHeight));

    ftries = new TZtringList;

    AppWindow = true;
}
//---------------------------------------------------------------------------
__fastcall TfKStudy::~TfKStudy()
{
    delete ftries;
    delete edkanji;
    delete kobj;
    delete wobj;
}
//---------------------------------------------------------------------------
int TfKStudy::ShowModalEx(TWordStudyList *astudy)
{
    study = astudy;
    coll = study->collection;

    itemsleft = study->readingsdue - 1;
    sbInfo->Panels->Items[1]->Text = itemsleft;
    sbInfo->Panels->Items[3]->Text = "0 /";
    sbInfo->Panels->Items[4]->Text = "0";

    int r;
    try
    {
        updateinfo();
        r = ShowModal();
    }
    catch (...)
    {
        r = mrAbort;
    }

    return r;
}
//---------------------------------------------------------------------------
void TfKStudy::edfunc()
{
    if (edkanji->Text.Length())
        btnAccept->Caption = "Accept";
    else
        btnAccept->Caption = "Reveal";
}
//---------------------------------------------------------------------------
void TfKStudy::updateinfo()
{
    ftries->Clear();
    ffull = false;
    pbTries->Invalidate();

    sbInfo->Panels->Items[1]->Text = itemsleft;

    kobj->kanji = study->readingscharacter;
    wobj->set_virtual(coll, study->readingsword_count, indexproc);
}
//---------------------------------------------------------------------------
UnicodeString TfKStudy::meaningproc(int ix)
{
    return study->readingsword_meaning[ix];
}
//---------------------------------------------------------------------------
int TfKStudy::indexproc(unsigned int ix)
{
    return study->readingsword_index[ix];
}
//---------------------------------------------------------------------------
void TfKStudy::prepareproc(int col, int row, TWordColumnType ctype, TCanvas *c)
{
    if (study->readingswordfailed[row])
    {
        c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x2030ff, 0.95);
    }
    else if (study->readingswordnew[row])
    {
        c->Brush->Color = coloravg(c->Brush->Color, (TColor)0x20ff50, 0.95);
    }

    TColor textcol = c->Font->Color;
    fixtextcolor(textcol, c->Brush->Color);
    c->Font->Color = textcol;
}
//---------------------------------------------------------------------------
void TfKStudy::drawproc(int col, int row, TCanvas *c, const TRect &r)
{
    SetTextAlign(c->Handle, TA_LEFT | TA_BASELINE);

    TSize s;
    byte p = study->readingscharacter_pos[row];
    TWord *w = study->readingsword[row];
    UnicodeString str = w->kanji;

    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = double(wobj->rowsize) * 0.65 / scaleres;
    int y = wobj->rowsize * 0.85 + r.Top - 1;
    TRect r2;

    //first part
    c->Font->Color = clBlack;
    r2 = Rect(r.Left, r.Top, r.Left, r.Bottom);
    int x = 4;
    if (p)
    {
        GetTextExtentPoint32W(c->Handle, str.c_str(), p, &s);
        r2.Right = r2.Left + x + s.cx;
        ExtTextOutW(c->Handle, r2.Left + x, y, ETO_CLIPPED | ETO_OPAQUE, &r2, str.c_str(), p, 0);
        x = 0;
    }

    //second part
    c->Font->Color = (TColor)0x0000d0;
    GetTextExtentPoint32W(c->Handle, str.c_str() + p, 1, &s);
    r2.Left = r2.Right;
    r2.Right = r2.Left + x + s.cx;
    ExtTextOutW(c->Handle, r2.Left + x, y, ETO_CLIPPED | ETO_OPAQUE, &r2, str.c_str() + p, 1, 0);

    //third part
    c->Font->Color = clBlack;
    if (p + 1 < str.Length())
    {
        GetTextExtentPoint32W(c->Handle, str.c_str() + p + 1, str.Length() - p - 1, &s);
        r2.Left = r2.Right;
        r2.Right = r2.Left + s.cx;
        ExtTextOutW(c->Handle, r2.Left, y, ETO_CLIPPED | ETO_OPAQUE, &r2, str.c_str() + p + 1, str.Length() - p - 1, 0);
    }
    c->FillRect(Rect(r2.Right, r.Top, r.Right, r.Bottom));
}
//---------------------------------------------------------------------------
void __fastcall TfKStudy::btnAcceptClick(TObject *Sender)
{
    if (!edkanji->Text.Length())
    {
        ffull = true;
        pbTries->Tag = 0;
        if (!itemsleft)
        {
            lbFResult->Caption = "";
            pFinished->Visible = true;
        }
        else
        {
            lbNResult->Caption = "";
            pNext->Visible = true;
        }
        pbTries->Invalidate();
        if (!ftries->Count)
            sbInfo->Panels->Items[4]->Text = (sbInfo->Panels->Items[4]->Text.ToInt() + 1);
        pAnswer->Visible = false;
        return;
    }

    UnicodeString entered = Hiraganize(edkanji->Text.c_str()), current = Hiraganize(currentreading().c_str());
    if (entered[entered.Length()] == L'っ')
        entered[entered.Length()] == L'つ';
    if (current[current.Length()] == L'っ')
        current[current.Length()] == L'つ';
    bool correct = entered == current;

    if (!correct)
    {
        for (int ix = 0; ix < wobj->count && !correct; ++ix)
        {
            TFuriganaData *dat;
            int cnt = 0;
            int kanjipos = study->readingscharacter_pos[ix];
            try
            {
                TWord *w = study->readingsword[ix];
                cnt = FindFurigana(w, dat);
                int pos;
                for (pos = 0; pos < cnt && kanjipos >= dat[pos].kanji + dat[pos].kanjilen; pos++);
                if (dat[pos].kanjilen == 1 && pos != cnt && dat[pos].kanji == kanjipos)
                {
                    current = Hiraganize(w->kana + dat[pos].kana).SubString(1, dat[pos].kanalen);
                    if (current[current.Length()] == L'っ')
                        current[current.Length()] == L'つ';
                    correct = current == entered;
                }
            }
            catch (...)
            {
                ;
            }
            if (cnt)
                delete[] dat;
        }
    }

    if (correct)
    {
        if (!ftries->Count)
            sbInfo->Panels->Items[3]->Text = UnicodeString(sbInfo->Panels->Items[3]->Text.SubString(1, sbInfo->Panels->Items[3]->Text.Pos(L" ") - 1).ToInt() + 1) + L" /";
        if (!itemsleft)
        {
            lbFResult->Font->Color = clGreen;
            lbFResult->Caption = "Correct!";
            pFinished->Visible = true;
        }
        else
        {
            lbNResult->Font->Color = clGreen;
            lbNResult->Caption = "Correct!";
            pNext->Visible = true;
        }
        ffull = true;
        pbTries->Tag = 1;
        pbTries->Invalidate();
    }
    else
    {
        if (!ftries->Count)
            sbInfo->Panels->Items[4]->Text = (sbInfo->Panels->Items[4]->Text.ToInt() + 1);
        ftries->Add(edkanji->Text);
        ffull = ftries->Count == 5;
        pbTries->Tag = 0;
        pbTries->Invalidate();
        if (ffull)
        {
            if (!itemsleft)
            {
                lbFResult->Font->Color = clMaroon;
                lbFResult->Caption = "Incorrect!";
                pFinished->Visible = true;
            }
            else
            {
                lbNResult->Font->Color = clMaroon;
                lbNResult->Caption = "Incorrect!";
                pNext->Visible = true;
            }
        }
        else
            pAgain->Visible = true;
    }
    pAnswer->Visible = false;
    edkanji->Text = L"";

    edfunc();
}
//---------------------------------------------------------------------------
void __fastcall TfKStudy::FormKeyPress(TObject *Sender, wchar_t &Key)
{
    if (!edkanji->CanInput())
        return;

    if (Key == VK_RETURN)
    {
        if (pAnswer->Visible)
            btnAccept->Click();
        else if (pAgain->Visible && btnAgain->Enabled)
            btnAgain->Click();
        else if (pNext->Visible && btnNext->Enabled)
            btnNext->Click();
        else if (pFinished->Visible && btnFinished->Enabled)
            btnFinished->Click();
    }
    else if (Key == VK_ESCAPE && btnSuspend->Enabled)
        btnSuspend->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfKStudy::btnNextClick(TObject *Sender)
{
    itemsleft--;
    study->next_reading();
    updateinfo();
    pAnswer->Visible = true;
    pNext->Visible = false;
    edKanji->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfKStudy::btnAgainClick(TObject *Sender)
{
    pAnswer->Visible = true;
    pAgain->Visible = false;
    edKanji->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfKStudy::btnFinishedClick(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
UnicodeString TfKStudy::currentreading()
{
    UnicodeString wstr;
    TKanji *k = study->readingskanji;
    try
    {
        byte reading = study->readingsreading;
        if (reading <= k->oncnt)
            wstr = k->on[reading - 1];
        else
            wstr = k->kun[real_index(k, reading) - k->oncnt - 1];
    }
    catch (...)
    {
        wstr = "ERROR";
    }

    if (wstr[1] == L'-')
        wstr = wstr.SubString(2, wstr.Length() - 1);
    else if (wstr.Pos(L'.') > 1)
        wstr = wstr.SubString(1, wstr.Pos(L'.') - 1);

    return wstr;
}
//---------------------------------------------------------------------------
void __fastcall TfKStudy::pbTriesPaint(TObject *Sender)
{
    TCanvas *c = pbTries->Canvas;
    c->Font->Name = L"Arial";
    c->Font->Charset = DEFAULT_CHARSET;
    c->Font->Size = double(12) / scaleres;
    TRect r;

    TSize s;

    if (ffull)
    {
        GetTextExtentPoint32W(c->Handle, L"Correct answer: ", 16, &s);
        r = Rect(0, 0, 10 + s.cx, pbTries->ClientHeight);
        ExtTextOutW(c->Handle, 5, 5, ETO_CLIPPED | ETO_OPAQUE, &r, L"Correct answer: ", 16, NULL);
        UnicodeString wstr = currentreading();

        c->Font->Name = settings.kanafont;
        c->Font->Charset = settings.kanacharset;
        c->Font->Size = double(42) / scaleres;
        c->Font->Color = pbTries->Tag == 1 ? clGreen : clMaroon;
        c->Font->Style = TFontStyles() << fsBold;
        GetTextExtentPoint32W(c->Handle, wstr.c_str(), wstr.Length(), &s);

        r.Left = r.Right;
        r.Right = pbTries->ClientWidth;
        SetTextAlign(c->Handle, TA_LEFT | TA_BASELINE);

        ExtTextOutW(c->Handle, r.Left + 2, r.Bottom*0.75, ETO_CLIPPED | ETO_OPAQUE, &r, wstr.c_str(), wstr.Length(), NULL);

        SetTextAlign(c->Handle, TA_LEFT | TA_TOP);

        c->Font->Color = clBlack;
        c->Font->Style = TFontStyles();
        return;
    }

    GetTextExtentPoint32W(c->Handle, L"Your tries so far: ", 19, &s);

    int h = s.cy + 3;
    r = Rect(0, 0, 10 + s.cx, 5 + h);
    ExtTextOutW(c->Handle, 5, 5, ETO_CLIPPED | ETO_OPAQUE, &r, L"Your tries so far: ", 19, NULL);

    int x = 10 + s.cx, y = 5;
    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = double(16) / scaleres;

    TSize sc;
    GetTextExtentPoint32W(c->Handle, L", ", 2, &sc);

    c->FillRect(Rect(10 + s.cx, 0, pbTries->ClientWidth, 5));

    for (int i = 0; i < ftries->Count; i++)
    {
        GetTextExtentPoint32W(c->Handle, ftries->Items[i].c_str(), ftries->Items[i].Length(), &s);
        if (x + s.cx + sc.cx > pbTries->ClientWidth - 10)
        {
            c->FillRect(Rect(x, y, pbTries->ClientWidth, y + h));
            x = 10;
            y += h;
        }

        r = Rect(x, y, x + s.cx, y + h);
        ExtTextOutW(c->Handle, x, y, ETO_CLIPPED | ETO_OPAQUE, &r, ftries->Items[i].c_str(), ftries->Items[i].Length(), NULL);

        x += s.cx;

        if (i == ftries->Count - 1)
            break; //no coma after last one
        r = Rect(x, y, x + sc.cx, y + h);
        ExtTextOutW(c->Handle, x, y, ETO_CLIPPED | ETO_OPAQUE, &r, L", ", 2, NULL);
        x += sc.cx;
    }
    c->FillRect(Rect(x, y, pbTries->ClientWidth, pbTries->Height));
    c->FillRect(Rect(0, y + h, x, pbTries->Height));

}
//---------------------------------------------------------------------------
void __fastcall TfKStudy::FormPaint(TObject *Sender)
{
    Canvas->FillRect(Rect(0, ChildRect(b0).Top + 1, ClientWidth, ChildRect(b1).Top));
    DrawPillowPanel(Canvas, ChildRect(pButtons, 0, 3, 0, 0));
}
//---------------------------------------------------------------------------
void __fastcall TfKStudy::btnSuspendClick(TObject *Sender)
{
    if (pFinished->Visible)
        btnFinished->Click();
    else
        ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfKStudy::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (pNext->Visible || pFinished->Visible)
        study->next_reading();
}
//---------------------------------------------------------------------------
void __fastcall TfKStudy::pbWordsKeyPress(TObject *Sender, wchar_t &Key)
{
    if ((Key >= L'A' && Key <= L'Z') || (Key >= L'a' && Key <= L'z') || Key == VK_BACK)
    {
        edKanji->SetFocus();
        edKanji->OnKeyPress(Sender, Key);
    }
    if (Key == VK_ESCAPE)
        edKanji->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfKStudy::edKanjiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
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

#ifdef _DEBUG
static TGlobalTest tester2(163);
#endif


