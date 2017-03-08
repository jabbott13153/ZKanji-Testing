//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanjidefimport.h"
#include "kanjigrid.h"
#include "wordobj.h"
#include "import.h"
#include "kanji.h"
#include "collection.h"
#include "settings.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"
TfKanjiDefImport *fKanjiDefImport;
//---------------------------------------------------------------------------
__fastcall TfKanjiDefImport::TfKanjiDefImport(TComponent* Owner) : TBaseForm(Owner)
{
    pbKanji->Width = rowsizes(settings.dictfontsize) * 4;
    pbDef->Left = pbKanji->Width;
    pbDef->Width = ClientWidth - pbDef->Left;
    dobj = new TVirtualWordListObj(pbDef, pbKanji->Width / 2, dcallback, (TPopupWordProc)NULL);
    dobj->headersize = rowsizes(settings.dictfontsize);
    dobj->darkenselectedline = false;

    kanjilist = new TIntegerList;
}
//---------------------------------------------------------------------------
__fastcall TfKanjiDefImport::~TfKanjiDefImport()
{
    delete kanjilist;
    delete dobj;
}
//---------------------------------------------------------------------------
int TfKanjiDefImport::ShowModalEx(TImportList *alist)
{
    list = alist;
    int cnt = list->KanjiCount;
    for (int ix = 0; ix < cnt; ++ix)
    {
        TExportedKanji *k = list->Kanji[ix];
        if (k->DictDef != NULL)
        {
            int kix = k->Index;
            if (list->Collection->kanjidat[kix].meaning != NULL && wcscmp(k->DictDef, list->Collection->kanjidat[kix].meaning))
                kanjilist->Add(ix);
        }
    }

    if (kanjilist->Count == 0)
        return mrOk;

    int colw[] = { 6555 };
    TWordColumnType colt[] = { wcCustom };
    dobj->set_cols(list->Collection, 1, colw, colw, NULL, colt, NULL, kanjilist->Count * 2, &returns0);
    dobj->colresizing = false;
    dobj->selecttype = wlsSingleSelect;
    dobj->SetCheckboxHandler(checkchange, getcheck);
    dobj->checkcolumn[0] = true;
    dobj->getheaderproc = getheader;
    dobj->drawproc = drawcell;
    dobj->prepareproc = preparecell;

    //void set_cols(int *acolwidths, int *acoldefaultwidths, bool *acolvisible, TWordColumnType *acoltypes, TAlignment *acolalign, int acount, TVirtualIndexProc aindex);
    int result = ShowModal();

    if (result == mrOk)
    {
        for (int ix = 0; ix < kanjilist->Count; ++ix)
            if (kanjilist->Items[ix] < 0)
                list->Kanji[-1 - kanjilist->Items[ix]]->DictDef = L"";
    }

    return result;
}
//---------------------------------------------------------------------------
int TfKanjiDefImport::returns0(unsigned int ix)
{
    return 0;
}
//---------------------------------------------------------------------------
void TfKanjiDefImport::checkchange(unsigned int row, byte index, bool &checked)
{
    if (!checked)
        return;
    if ((((row % 2) == 0) == checked) == (kanjilist->Items[row / 2] >= 0))
    {
        kanjilist->Items[row / 2] = -1 - kanjilist->Items[row / 2];
        dobj->checked[row + ((row % 2) == 0 ? 1 : -1)][0] = !checked;
        dobj->invalidatecell(0, row + ((row % 2) == 0 ? 1 : -1));
    }
}
//---------------------------------------------------------------------------
bool TfKanjiDefImport::getcheck(unsigned int row, byte index, TListVirtualCheckControlWantedValue wanted)
{
    if (wanted == ccwvBoxColumn)
        return true;
    if (wanted == ccwvEnabled)
        return true;
    return (kanjilist->Items[row / 2] < 0) == (row %2 == 0);
}
//---------------------------------------------------------------------------
void __fastcall TfKanjiDefImport::pbDefVScroll(TObject *Sender, TScrollCode ScrollCode, int &ScrollPos)
{
    pbKanji->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfKanjiDefImport::pbKanjiPaint(TObject *Sender)
{
    int top = (pbDef->VPos * dobj->rowsize) / pbKanji->Width;
    int y = -((pbDef->VPos * dobj->rowsize) % pbKanji->Width) + dobj->headersize;

    TCanvas *c = pbKanji->Canvas;

    c->Brush->Color = clBtnFace;
    c->Pen->Color = clBtnFace;
    TWordListBase::DrawHeadNormal(c, 0, Rect(0, 0, pbKanji->Width, dobj->headersize), "");
    c->Pen->Color = (TColor)colorsettings.gridline;


    c->Font->Name = settings.kanjifont;
    c->Font->Charset = settings.kanjicharset;
    c->Font->Quality = !settings.kanjiantialias ? fqDefault : fqAntialiased;
    c->Font->Color = (TColor)colorsettings.maintext;
    c->Font->Height = pbKanji->Width * 0.8;

    int olda = SetTextAlign(c->Handle, TA_CENTER | TA_BASELINE);

    TRect r;
    while (y < pbKanji->ClientHeight && top < kanjilist->Count)
    {
        if ((top % 2) == 0)
            c->Brush->Color = coloravg((TColor)colorsettings.background, clWhite, 0.97);
        else
            c->Brush->Color = coloravg((TColor)colorsettings.background, clBlack, 0.97);

        if (dobj->selected[top * 2] || dobj->selected[top * 2 + 1])
            c->Brush->Color = pbDef->Focused() ? (TColor)colorsettings.highlight : coloravg((TColor)colorsettings.highlight, (TColor)colorsettings.background, 0.8);


        int ix = kanjilist->Items[top];
        if (ix < 0)
            ix = -1 - ix;

        wchar_t ch = list->Kanji[ix]->Kanji;

        r = Rect(0, max(dobj->headersize, y), pbKanji->Width, y + pbKanji->Width);
        ExtTextOutW(c->Handle, pbKanji->Width / 2, y + pbKanji->Width * 0.8, ETO_CLIPPED | ETO_OPAQUE, &r, &ch, 1, NULL);

        c->MoveTo(r.left, r.bottom - 1);
        c->LineTo(r.right, r.bottom - 1);

        y += pbKanji->Width;
        ++top;
    }

    c->Brush->Color = (TColor)colorsettings.background;

    if (y < pbKanji->ClientHeight)
        c->FillRect(Rect(0, y, pbKanji->ClientWidth, pbKanji->ClientHeight));

    SetTextAlign(c->Handle, olda);
}
//---------------------------------------------------------------------------
UnicodeString TfKanjiDefImport::getheader(int col)
{
    return L"Kanji definition";
}
//---------------------------------------------------------------------------
void TfKanjiDefImport::preparecell(int col, int row, TWordColumnType ctype, TCanvas *c)
{
    int clr = c->Brush->Color; //(dobj->pos != row ? colorsettings.background : colorsettings.highlight);
    if (dobj->selected[row])
        return;

    if (((row / 2) % 2) == 0)
        c->Brush->Color = coloravg((TColor)clr, clWhite, 0.97);
    else
        c->Brush->Color = coloravg((TColor)clr, clBlack, 0.97);
}
//---------------------------------------------------------------------------
void TfKanjiDefImport::drawcell(int col, int row, TCanvas *c, const TRect &r)
{
    int ix = kanjilist->Items[row / 2];
    if (ix < 0)
        ix = -1 - ix;

    bool old = (row % 2) == 0;

    c->FillRect(r);
    c->Font->Height = rowsizes(settings.dictfontsize) * 0.9;
    TFontStyles oldstyle = c->Font->Style;
    c->Font->Style = c->Font->Style << fsBold;

    //c->Brush->Color = clNone;
    c->Brush->Style = bsClear;

    TSize s1;
    GetTextExtentPoint32W(c->Handle, L"Old: ", 5, &s1);
    int skipw = s1.cx;
    TSize s2;
    GetTextExtentPoint32W(c->Handle, L"Imported: ", 10, &s2);
    skipw = max(skipw, (int)s2.cx);

    skipw += scaleres * 10;

    if ((kanjilist->Items[row / 2] < 0) != old)
        c->Font->Color = coloravg(c->Font->Color, c->Brush->Color, 0.3);

    UnicodeString text;
    if (old)
    {
        ExtTextOutW(c->Handle, r.left + skipw - s1.cx, r.top + 2, ETO_CLIPPED, &r, L"Old: ", 5, NULL);
        text = list->Collection->kanjidat[list->Kanji[ix]->Index].meaning;
    }
    else
    {
        ExtTextOutW(c->Handle, r.left + skipw - s2.cx, r.top + 2, ETO_CLIPPED, &r, L"Imported: ", 10, NULL);
        text = list->Kanji[ix]->DictDef;
    }

    c->Font->Style = oldstyle;

    TRect r2 = r;
    r2.left += skipw;
    r2.top += 2;
    r2.right = pbDef->ClientWidth;

    DrawText(c->Handle, text.c_str(), text.Length(), &r2, DT_NOPREFIX | DT_LEFT | DT_TOP | DT_WORDBREAK);

    c->Brush->Style = bsSolid;
}
//---------------------------------------------------------------------------
void TfKanjiDefImport::dcallback()
{
    pbKanji->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfKanjiDefImport::btnNextClick(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfKanjiDefImport::btnAbortClick(TObject *Sender)
{
    ModalResult = mrAbort;
}
//---------------------------------------------------------------------------
void __fastcall TfKanjiDefImport::pbDefResize(TObject *Sender)
{
    if (pbDef->Width != pbDef->Tag)
        pbDef->Invalidate();
    pbDef->Tag = pbDef->Width;
}
//---------------------------------------------------------------------------
void __fastcall TfKanjiDefImport::pbKanjiEnter(TObject *Sender)
{
    pbDef->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfKanjiDefImport::pbDefKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == ' ')
    {
        if (!dobj->checked[dobj->pos][0])
            dobj->checked[dobj->pos][0] = true;
        //else
        //    dobj->checked[dobj->pos + ((dobj->pos % 2) == 0 ? 1 : -1)][0] = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfKanjiDefImport::pbDefEnter(TObject *Sender)
{
    pbKanji->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfKanjiDefImport::pbDefExit(TObject *Sender)
{
    pbKanji->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfKanjiDefImport::FormPaint(TObject *Sender)
{
    DrawCloseBar(this, bvBottom, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfKanjiDefImport::FormResize(TObject *Sender)
{
    TRect r = Rect(0, bvBottom->Top + bvBottom->Height, ClientWidth, ClientHeight);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE);
}
//---------------------------------------------------------------------------

