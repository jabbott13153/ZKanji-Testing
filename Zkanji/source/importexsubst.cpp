//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "importexsubst.h"
#include "gengraph.h"
#include "import.h"
#include "wordobj.h"
#include "settings.h"
#include "kanjiedit.h"
#include "searchtree.h"
#include "romajizer.h"
#include "collection.h"
#include "words.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "edit2"
#pragma link "ZPaintBox"
#pragma resource "*.dfm"
TfExampleSub *fExampleSub;
//---------------------------------------------------------------------------
__fastcall TfExampleSub::TfExampleSub(TComponent* Owner) : TBaseForm(Owner)
{
    wobj = new TCallbackWordListObj(wbox, rowsizes(settings.dictfontsize), &wcallback, (TPopupWordProc)NULL);
    wobj->wordselect = false;
    wobj->NumProc = &wnumproc;
    wobj->StringProc = &wstrproc;
    wobj->WordTagProc = &wtagproc;
    wobj->drawproc = wdraw;
    edobj = new TWordListObj(pbDict, rowsizes(settings.dictfontsize), &edcallback, (TPopupWordProc)NULL );
    edobj->wordselect = false;

    int colw[] = { 48 * scaleres, 100 * scaleres, 120 * scaleres, 6555 * scaleres };
    TWordColumnType colt[] = { wcCustom, wcKanji, wcKana, wcCustom };
    wobj->set_cols(4, colw, colw, NULL, colt, NULL, 0);
    wobj->colresize[0] = false;
    wobj->SetCheckboxHandler(wcheck);
    wobj->checkcolumn[0] = true;

    edkanji = new TKanjiEdit(pbKanji, true, true, NULL, &kcallback);
    edkanji->EnableRecognizer(sbRecognize);
}
//---------------------------------------------------------------------------
__fastcall TfExampleSub::~TfExampleSub()
{
    delete edobj;
    delete edkanji;
    delete wobj;
}
//---------------------------------------------------------------------------
int TfExampleSub::ShowModalEx(TImportList *alist)
{
    list = alist;
    cix = dictionary->Index[list->Collection];

    int cnt = list->KanjiCount;
    for (int ix = 0; ix < cnt; ++ix)
    {
        int kcnt = list->Kanji[ix]->ExampleCount;
        for (int iy = 0; iy < kcnt; ++iy)
        {
            if (list->Kanji[ix]->Example[iy]->Index < 0)
                words.Add(new TExampleWordData(ix, list->Kanji[ix]->Example[iy]));
        }
    }

    if (words.Count == 0)
        return mrOk;

    wobj->count = words.Count;
    wobj->pos = 0;

    int result = ShowModal();

    for (int ix = 0; ix < words.Count; ++ix)
        if (!wobj->checked[ix][0])
            words[ix]->example->Index = -1;

    return result;
}
//---------------------------------------------------------------------------
void TfExampleSub::wcallback()
{
    btnNext->Caption = wobj->pos == wobj->count - 1 ? L"&Done" : L"Next";

    edkanji->Text = "";
    edRomaji->Text = "";
    UpdateWords();
}
//---------------------------------------------------------------------------
void TfExampleSub::edcallback()
{
    btnSelect->Enabled = edobj->pos >= 0;
}
//---------------------------------------------------------------------------
void TfExampleSub::wcheck(unsigned int row, byte index, bool &checked)
{
    wobj->invalidatecell(3, row);
}
//---------------------------------------------------------------------------
word TfExampleSub::wnumproc(unsigned int ix, TWordNumProcWanted wanted)
{
    return 0;
}
//---------------------------------------------------------------------------
UnicodeString TfExampleSub::wstrproc(unsigned int ix, byte m, TWordStringProcWanted wanted)
{
    switch (wanted)
    {
    case wspwKanji:
        return words[ix]->example->Kanji(list->Collection);
    case wspwKana:
        return words[ix]->example->Kana(list->Collection);
    default:
        return UnicodeString();
    }
}
//---------------------------------------------------------------------------
void TfExampleSub::wtagproc(unsigned int i, byte m, int &types, int &notes, byte &fields, word &nametag)
{
    ;
}
//---------------------------------------------------------------------------
void TfExampleSub::wdraw(int col, int row, TCanvas *c, const TRect &r)
{
    int al = SetTextAlign(c->Handle, TA_BASELINE | TA_LEFT);

    if (col == 0)
    {
        wchar_t ch = list->Kanji[words[row]->kindex]->Kanji;

        c->Font->Name = settings.kanafont;
        c->Font->Charset = settings.kanacharset;
        c->Font->Size = double(wobj->rowsize) * 0.66 / scaleres;

        ExtTextOutW(c->Handle, r.left + 3 * scaleres, r.Top + wobj->rowsize * 0.80, ETO_OPAQUE | ETO_CLIPPED, &r, &ch, 1, 0);
    }
    if (col == 3)
    {
        if (words[row]->example->Index < 0)
        {
            c->FillRect(r);
            return;
        }

        TRect r2 = r;

        if (!wobj->checked[row][0])
            c->Font->Color = coloravg(c->Font->Color, (TColor)colorsettings.background, 0.3);

        UnicodeString str;
        c->Font->Style = TFontStyles() << fsBold;
        TSize s;
        GetTextExtentPoint32W(c->Handle, L"Selected: ", 10, &s);
        r2.Right = r2.Left + s.cx + 6 * scaleres + s.cx;
        ExtTextOutW(c->Handle, r2.left + 4 * scaleres, r2.Top + wobj->rowsize * 0.8, ETO_OPAQUE | ETO_CLIPPED, &r2, L"Selected: ", 10, 0);

        r2 = r;

        r2.left += 4 * scaleres + s.cx + 2 * scaleres;
        c->Font->Style = TFontStyles();
        c->Font->Name = settings.kanafont;
        c->Font->Charset = settings.kanacharset;
        c->Font->Size = double(wobj->rowsize) * 0.66 / scaleres;
        TWord *w = dictionary->Words[cix]->Items[words[row]->example->Index];
        str = UnicodeString(w->kanji) + L" (" + UnicodeString(w->kana) + L")";

        ExtTextOutW(c->Handle, r2.left, r2.Top + wobj->rowsize * 0.80, ETO_OPAQUE | ETO_CLIPPED, &r2, str.c_str(), str.Length(), 0);
    }

    SetTextAlign(c->Handle, al);
}
//---------------------------------------------------------------------------
void TfExampleSub::kcallback()
{
    UpdateWords();
}
//---------------------------------------------------------------------------
void TfExampleSub::UpdateWords()
{
    TWordIndexList *l = new TWordIndexList(list->Collection);

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

    for (int ix = l->Count - 1; ix >= 0; --ix)
    {
        if (wcschr(l->Words[ix]->kanji, list->Kanji[words[wobj->pos]->kindex]->Kanji) == NULL)
            l->Delete(ix);
    }

    edobj->set_by_indexes(l, true);
    delete l;
}
//---------------------------------------------------------------------------
void __fastcall TfExampleSub::FormPaint(TObject *Sender)
{
    DrawCloseBar(this, bvBottom, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfExampleSub::FormResize(TObject *Sender)
{
    TRect r = Rect(0, bvBottom->Top + bvBottom->Height, ClientWidth, ClientHeight);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE);
}
//---------------------------------------------------------------------------
void __fastcall TfExampleSub::btnNextClick(TObject *Sender)
{
    if (wobj->pos == wobj->count - 1)
        ModalResult = mrOk;
    else
        ++wobj->pos;
}
//---------------------------------------------------------------------------
void __fastcall TfExampleSub::btnAbortClick(TObject *Sender)
{
    ModalResult = mrAbort;
}
//---------------------------------------------------------------------------
void __fastcall TfExampleSub::btnKanjiClick(TObject *Sender)
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
    UpdateWords();
}
//---------------------------------------------------------------------------
void __fastcall TfExampleSub::edRomajiChange(TObject *Sender)
{
    UpdateWords();
}
//---------------------------------------------------------------------------
void __fastcall TfExampleSub::btnSelectClick(TObject *Sender)
{
    words[wobj->pos]->example->Index = edobj->current;
    wobj->checked[wobj->pos][0] = true;
    wobj->invalidatecell(3, wobj->pos);
    if (wobj->pos < wobj->count - 1)
        ++wobj->pos;
}
//---------------------------------------------------------------------------
void __fastcall TfExampleSub::pbKanjiKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == 13)
    {
        if (btnSelect->Enabled)
            btnSelect->Click();
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfExampleSub::pbKanjiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN && btnSelect->Enabled)
        Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfExampleSub::pbKanjiEnter(TObject *Sender)
{
    btnNext->Default = !pbKanji->Focused() && !edRomaji->Focused() && !btnSelect->Focused() && !btnAbort->Focused();
}
//---------------------------------------------------------------------------


