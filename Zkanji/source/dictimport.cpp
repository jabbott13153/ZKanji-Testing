//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "dictimport.h"
#include "import.h"
#include "wordobj.h"
#include "settings.h"
#include "collection.h"
#include "words.h"
#include "kanjiedit.h"
#include "searchtree.h"
#include "module.h"
#include "romajizer.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"
TfDictImport *fDictImport;
//---------------------------------------------------------------------------

__fastcall TfDictImport::TfDictImport(TComponent* Owner) : TBaseForm(Owner), ddefs(NULL), dupdating(false)
{
    edkanji = new TKanjiEdit(pbKanji, true, true, NULL, ecallback);
    edobj = new TWordListObj(pbDict, rowsizes(settings.dictfontsize), edcallback, (TPopupWordProc)NULL );
    edobj->wordselect = false;
    edkanji->EnableRecognizer(sbRecognize);

    wobj = new TCallbackWordListObj(wbox, rowsizes(settings.dictfontsize), wcallback, (TPopupWordProc)NULL );
    wobj->wordselect = false;
    wobj->NumProc = wnumproc;
    wobj->StringProc = wstrproc;
    wobj->WordTagProc = wtagproc;

    iobj = new TCallbackWordListObj(ibox, rowsizes(settings.dictfontsize), icallback, (TPopupWordProc)NULL );
    iobj->wordselect = false;
    iobj->NumProc = inumproc;
    iobj->StringProc = istrproc;
    iobj->WordTagProc = itagproc;
    iobj->colresizing = false;
    iobj->drawproc = idraw;

    dobj = new TVocabListObj(dbox, rowsizes(settings.dictfontsize), dcallback, (TPopupWordProc)NULL );
    dobj->wordselect = false;
    dobj->allownopos = true;
    dobj->colresizing = false;
    dobj->selecttype = wlsMultiSelect;
    dobj->multiselection = wlmsRandom;
    dobj->prepareproc = dprep;
    dobj->drawproc = ddraw;

    Constrain(ChildRect(btnWordSel).right + 8 * scaleres + Width - ClientWidth, Height - wbox->Height + wbox->Top + rowsizes(settings.dictfontsize) * 3);
}
//---------------------------------------------------------------------------
__fastcall TfDictImport::~TfDictImport()
{
    delete edkanji;
    delete edobj;
    delete wobj;
    delete iobj;
    delete dobj;
    delete ddefs;
}
//---------------------------------------------------------------------------
TImportedWord* TfDictImport::get_iword(int ix)
{
    if (groupimport)
        THROW(L"Only valid when managing entry import.");
    return list->ImportWord[words[ix]];
}
//---------------------------------------------------------------------------
TReplacedWord* TfDictImport::get_rword(int ix)
{
    if (!groupimport)
        THROW(L"Only valid when managing groups.");
    return list->ReplaceWord[words[ix]];
}
//---------------------------------------------------------------------------
TExportedWord* TfDictImport::get_eword(int ix)
{
    if (!groupimport)
        return list->ImportWord[words[ix]]->data;
    return list->ReplaceWord[words[ix]]->data;
}
//---------------------------------------------------------------------------
int TfDictImport::getecnt()
{
    if (!groupimport)
    {
        TImportedWord *w = iwords[wobj->pos];
        return w->defs.Count;
    }
    else
    {
        TReplacedWord *r = rwords[wobj->pos];
        return r->defs.Count;
    }
}
//---------------------------------------------------------------------------
int TfDictImport::ShowModalEx(TImportList *alist, TImportOptions *aoptions, bool agroup)
{
    list = alist;
    options = aoptions;
    groupimport = agroup;

    if (groupimport)
    {
        Caption = L"Word group import substitute finder";
        lbOrig1->Caption = L"This word was not found in the dictionary. Select a word to be used as a replacement. The word won't be imported to groups if no replacement is selected.";
        Label6->Caption = L"Select a word to be used as a replacement:";
        Label3->Caption = L"Meanings used as a replacement for the selected meaning:";
        Label5->Left = Label3->Left + Label3->Width + 20 * scaleres;
    }

    TWordCollection *coll = list->Collection;
    int imgindex = coll->imgindex;
    if (imgindex < 0)
        imgindex = dictionary->Items[0] == coll ? 13 : 11;
    btnKanji->Glyph = NULL;
    btnRomaji->Glyph = NULL;
    data->imgMenu->BkColor = clLime;
    data->imgMenu->GetBitmap(imgindex, btnKanji->Glyph);
    data->imgMenu->GetBitmap(imgindex + 1, btnRomaji->Glyph);
    data->imgMenu->BkColor = Graphics::clNone;

    UnicodeString s = dictionary->Strings[dictionary->Index[coll]];
    btnKanji->Hint = L"Japanese to " + s;
    btnRomaji->Hint = s + L" to Japanese";

    if (!groupimport)
    {
        for (int ix = 0; ix < list->ImportCount; ++ix)
        {
            TImportedWord *iw = list->ImportWord[ix];
            if (iw->action != wiaSkip && iw->data->Index >= 0)
                wordstaken.Items[iw->data->Index] = (iw->action != wiaCheck ? -1 : words.Count);

            if (iw->action != wiaCheck)
                continue;
            //iw->action = wiaApply;
            words.Add(ix);
        }
    }
    else
    {
        for (int ix = 0; ix < list->ReplaceCount; ++ix)
        {
            TReplacedWord *rw = list->ReplaceWord[ix];
            bool added = false;
            for (int iy = 0; iy < rw->defs.Count; ++iy)
            {
                if (rw->defs[iy]->defcnt == 0)
                {
                    words.Add(ix);
                    added = true;
                    break;
                }
            }
            if (rw->data->Index >= 0)
                wordstaken.Items[rw->data->Index] = !added ? -1 : words.Count - 1;
        }
    }
    if (!words.Count)
        return mrOk;

    cbDelUnmatched->Visible = !options->wordgroups;

    ddefs = new TWordIndexList(alist->Collection);
    dupdating = true; // Start of callback block
    dobj->wordlist = ddefs;

    wobj->SetCheckboxHandler(wcheck, wgetcheck);

    int wcolwidths[] = { rowsizes(settings.dictfontsize) + 4 * scaleres, rowsizes(settings.dictfontsize) - 1, 100 * scaleres, 120 * scaleres, 6250 * scaleres };
    TWordColumnType wcoltypes[] = { wcCustom, wcPop, wcKanji, wcKana, wcMeaning };
    wobj->set_cols(5, wcolwidths, wcolwidths, NULL, wcoltypes, NULL, words.Count);
    wobj->colresize[1] = false;
    wobj->colresize[3] = true;
    wobj->colresize[4] = false;

    iobj->SetCheckboxHandler(icheck, igetcheck);
    int icolwidths[] = { rowsizes(settings.dictfontsize) + 4 * scaleres, 30 * scaleres, 6250 * scaleres };
    TWordColumnType icoltypes[] = { wcCustom, wcCustom, wcMeaning };
    iobj->set_cols(3, icolwidths, icolwidths, NULL, icoltypes, NULL, 0);

    int dcolwidths[] = { 30 * scaleres, rowsizes(settings.dictfontsize) - 1, 6250 * scaleres };
    TWordColumnType dcoltypes[] = { wcCustom, wcPop, wcMeaning };
    dobj->set_cols(3, dcolwidths, dcolwidths, NULL, dcoltypes, NULL, 0);
    dupdating = false; // End of callback block

    wobj->pos = 0;

    int result = ShowModal();

    return result;
}
//---------------------------------------------------------------------------
word TfDictImport::wnumproc(unsigned int ix, TWordNumProcWanted wanted)
{
    if (wanted == wnpwFreq)
        return max(0, ewords[ix]->Frequency);
    return ewords[ix]->DefCount;
}
//---------------------------------------------------------------------------
UnicodeString TfDictImport::wstrproc(unsigned int ix, byte m, TWordStringProcWanted wanted)
{
    if (wanted == wspwKanji)
        return ewords[ix]->Kanji(list->Collection);
    if (wanted == wspwKana)
        return ewords[ix]->Kana(list->Collection);

    return ewords[ix]->Def[m]->data.meaning;
}
//---------------------------------------------------------------------------
void TfDictImport::wtagproc(unsigned int ix, byte m, int &types, int &notes, byte &fields, word &nametag)
{
     TMeaningData &data = ewords[ix]->Def[m]->data;

     types = data.types;
     notes = data.notes;
     fields = data.fields;
     nametag = data.nametag;
}
//---------------------------------------------------------------------------
void TfDictImport::wcheck(unsigned int row, byte index, bool &checked)
{
    if (((!groupimport && iwords[row]->action == wiaSkip) || (groupimport && rwords[row]->skip)) && checked)
    {
        int wix = ewords[row]->Index;
        if (wix >= 0)
        {
            int ix = wordstaken.ItemSet(wix) ? wordstaken.Items[wix] : -2;
            if (ix != -2 && ix != (int)row)
            {
                MsgBox(L"The dictionary word for this imported entry is already reserved. Please change the other entry first.", L"Message", MB_OK);
                checked = false;
                return;
            }
        }

        if (!groupimport)
            iwords[row]->action = wiaCheck;
        else
            rwords[row]->skip = false;
        if (wix >= 0)
            wordstaken.Items[wix] = row;
    }
    else if (!checked)
    {
        int wix = ewords[row]->Index;
        if (wix >= 0)
            wordstaken.ClearIndex(wix);
        if (!groupimport)
            iwords[row]->action = wiaSkip;
        else
            rwords[row]->skip = true;
    }
}
//---------------------------------------------------------------------------
bool TfDictImport::wgetcheck(unsigned int row, byte index, TListVirtualCheckControlWantedValue wanted)
{
    if (wanted == ccwvBoxColumn)
        return index == 0;
    if (wanted == ccwvChecked)
        return (!groupimport && iwords[row]->action != wiaSkip) || (groupimport && !rwords[row]->skip);
    return true;
}
//---------------------------------------------------------------------------
void TfDictImport::wcallback()
{
    btnNext->Caption = wobj->pos == wobj->count - 1 ? L"&Import" : L"&Next";

    TImportedWord *w = !groupimport ? iwords[wobj->pos] : NULL;
    TExportedWord *e = ewords[wobj->pos];
    TReplacedWord *r = groupimport ? rwords[wobj->pos] : NULL;

    if (e->Index >= 0 && ((!groupimport && w->defs.Count == 0 && (w->matchdefs.Count == 0 || w->matchdefs.Count == list->Collection->words->Items[e->Index]->meaningcnt)) || (groupimport && r->defs.Count == 0)))
    {
        btnWordSel->Visible = true;
        pOrig->Visible = false;
        lbOrig1->Visible = false;
        lbOrig2->Visible = false;
        lbOrig3->Visible = true;
        pcSelector->ActivePage = tabWords;
    }
    else if (/*(groupimport || (w->modtype & wiitNew) == wiitNew) &&*/ e->Index < 0)
    {
        btnWordSel->Visible = false;
        lbOrig3->Visible = false;
        lbOrig1->Visible = groupimport || dictionary->Index[list->Collection] > 0;
        if (lbOrig1->Visible)
        {
            lbOrig2->Visible = false;
            pOrig->Visible = true;
            btnKanji->Down = true;
            edkanji->Text = e->Kana(list->Collection);
            ecallback();
        }
        else
            lbOrig2->Visible = true;
        pcSelector->ActivePage = tabWords;
    }
    else
    {
        dupdating = true;
        ddefs->Clear();

        cbDelUnmatched->Checked = !groupimport && !options->wordgroups && (w->modtype & 0x1000) == 0x1000;

        TWord *ww = list->Collection->words->Items[e->Index];
        for (int ix = 0; ix < ww->meaningcnt; ++ix)
        {
            bool skip = false;
            if (!groupimport)
            {
                for (int iy = 0; iy < w->matchdefs.Count && !skip; ++iy)
                    if (w->matchdefs[iy]->dictdefs[0] == ix)
                        skip = true;
            }
            else
            {
                for (int iy = 0; iy < r->matchdefs.Count && !skip; ++iy)
                    if (r->matchdefs[iy]->dictdefs[0] == ix)
                        skip = true;
            }
            if (!skip)
                ddefs->Add(e->Index, ix);
        }

        pIMeanings->Visible = groupimport || edefcnt; //groupimport || ((w->modtype & wiitMatch) != wiitMatch) || ww->meaningcnt < e->DefCount;
        pDMeanings->Visible = groupimport || ddefs->Count; //groupimport || ((w->modtype & wiitMatch) != wiitMatch) || ww->meaningcnt > e->DefCount;

        Label3->Visible = pIMeanings->Visible;
        Label5->Visible = pIMeanings->Visible;
        if (!Label3->Visible)
            cbDelUnmatched->Left = Label3->Left;
        else
            cbDelUnmatched->Left = Label5->Left + Label5->Width + 24 * scaleres;

        if (dobj->pos < 0)
            dbox->Invalidate();
        dobj->Update(-1);

        dupdating = false;
        iobj->count = edefcnt;
        iobj->Update(0);
        pcSelector->ActivePage = tabMeanings;
        btnWordSel->Visible = (!groupimport && w->newword) || (groupimport && r->newword);
    }
}
//---------------------------------------------------------------------------
word TfDictImport::inumproc(unsigned int ix, TWordNumProcWanted wanted)
{
    if (wanted == wnpwFreq)
        return max(0, ewords[wobj->pos]->Frequency);
    return 1;
}
//---------------------------------------------------------------------------
UnicodeString TfDictImport::istrproc(unsigned int ix, byte m, TWordStringProcWanted wanted)
{
    if (wanted == wspwKanji)
        return ewords[wobj->pos]->Kanji(list->Collection);
    if (wanted == wspwKana)
        return ewords[wobj->pos]->Kana(list->Collection);

    if (!groupimport)
    {
        TImportedWord *w = iwords[wobj->pos];
        int p = w->defs[ix]->pos;
        if (p < 0)
            p = -1 - p;
        return w->data->Def[p]->data.meaning;
    }
    else
    {
        TReplacedWord *r = rwords[wobj->pos];
        int p = r->defs[ix]->pos;
        if (p < 0)
            p = -1 - p;
        return r->data->Def[p]->data.meaning;
    }
}
//---------------------------------------------------------------------------
void TfDictImport::itagproc(unsigned int ix, byte m, int &types, int &notes, byte &fields, word &nametag)
{
    TExportedWord *e = ewords[wobj->pos];
    int p;

    if (!groupimport)
    {
        TImportedWord *w = iwords[wobj->pos];
        p = w->defs[ix]->pos;
        if (p < 0)
            p = -1 - p;
    }
    else
    {
        TReplacedWord *r = rwords[wobj->pos];
        p = r->defs[ix]->pos;
        if (p < 0)
            p = -1 - p;
    }

    TMeaningData &data = e->Def[p]->data;
    types = data.types;
    notes = data.notes;
    fields = data.fields;
    nametag = data.nametag;
}
//---------------------------------------------------------------------------
void TfDictImport::icheck(unsigned int row, byte index, bool &checked)
{
    if (!groupimport)
    {
        TImportedWord *w = iwords[wobj->pos];
        w->defs[row]->pos = -1 - w->defs[row]->pos;
    }
    else
    {
        TReplacedWord *r = rwords[wobj->pos];
        r->defs[row]->pos = -1 - r->defs[row]->pos;
    }
}
//---------------------------------------------------------------------------
bool TfDictImport::igetcheck(unsigned int row, byte index, TListVirtualCheckControlWantedValue wanted)
{
    if (wanted == ccwvBoxColumn)
        return index == 0;
    if (wanted == ccwvChecked)
        return !groupimport ? (iwords[wobj->pos]->defs[row]->pos >= 0) : (rwords[wobj->pos]->defs[row]->pos >= 0);
    return true;
}
//---------------------------------------------------------------------------
void TfDictImport::icallback()
{
    if (iobj->pos < 0)
        return;

    dupdating = true;
    TImportedWord *w = !groupimport ? iwords[wobj->pos] : NULL;
    TReplacedWord *r = groupimport ? rwords[wobj->pos] : NULL;
    TImportedDefinition *def = w ? w->defs[iobj->pos] : r->defs[iobj->pos];
    dobj->DeselectAll();
    for (int ix = 0; ix < def->defcnt; ++ix)
    {
        for (int iy = 0; iy < dobj->count; ++iy)
        {
            if (dobj->meaningindexes[iy] == def->dictdefs[ix])
            {
                dobj->selected[iy] = true;
                break;
            }
        }
    }
    dupdating = false;
}
//---------------------------------------------------------------------------
void TfDictImport::dcallback()
{
    if (dupdating || iobj->pos < 0)
    {
        if (iobj->pos < 0)
        {
            for (int ix = 0; ix < dobj->count; ++ix)
            {
                if (dobj->selected[ix])
                    dobj->selected[ix] = false;
                break;
            }
        }
        return;
    }

    TImportedDefinition *def;
    if (!groupimport)
    {

        TImportedWord *w = iwords[wobj->pos];
        def = w->defs[iobj->pos];

        for (int ix = 0; ix < dobj->count; ++ix)
        {
            if (dobj->selected[ix])
            {
                if (def->defcnt != 0)
                {
                    bool matched = false;
                    for (int z = 0; z < def->defcnt; ++z)
                    {
                        if (def->dictdefs[z] == dobj->meaningindexes[ix])
                            matched = true;
                        else if (dmeaningcnt(def->dictdefs[z]) > 1)
                        {
                            dobj->selected[ix] = false;
                            return;
                        }
                    }

                    if (!matched && dmeaningcnt(dobj->meaningindexes[ix]) > 0)
                    {
                        dobj->selected[ix] = false;
                        return;
                    }
                }

                for (int iy = 0; iy < w->defs.Count; ++iy)
                {
                    if (iy == iobj->pos || imeaningcnt(w->defs[iy]->pos) <= 1)
                        continue;

                    for (int z = 0; z < w->defs[iy]->defcnt; ++z)
                    {
                        if (w->defs[iy]->dictdefs[z] == dobj->meaningindexes[ix])
                        {
                            dobj->selected[ix] = false;
                            return;
                        }
                    }
                }
            }
        }
    }
    else
    {
        TReplacedWord *r = rwords[wobj->pos];
        def = r->defs[iobj->pos];

        for (int ix = 0; ix < dobj->count; ++ix)
        {
            if (dobj->selected[ix])
            {
                bool matched = false;
                for (int z = 0; z < r->defs.Count; ++z)
                {
                    if (z == iobj->pos)
                        continue;
                    for (int iy = 0; iy < r->defs[z]->defcnt && !matched; ++iy)
                    {
                        if (r->defs[z]->dictdefs[iy] == dobj->meaningindexes[ix])
                            matched = true;
                    }
                }
                if (matched)
                {
                    dobj->selected[ix] = false;
                    return;
                }
            }
        }
    }

    delete[] def->dictdefs;
    def->dictdefs = NULL;
    def->defcnt = 0;
    for (int ix = 0; ix < dobj->count; ++ix)
        if (dobj->selected[ix])
            ++def->defcnt;

    if (def->defcnt)
    {
        def->dictdefs = new byte[def->defcnt];
        int pos = 0;
        for (int ix = 0; ix < dobj->count; ++ix)
        {
            if (dobj->selected[ix])
                def->dictdefs[pos++] = dobj->meaningindexes[ix];
        }
    }

    iobj->invalidatecell(1, iobj->pos);
    dobj->invalidaterows(0, dobj->count);
}
//---------------------------------------------------------------------------
void TfDictImport::UpdateWords()
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

    edobj->set_by_indexes(l, true);
    delete l;
}
//---------------------------------------------------------------------------
void TfDictImport::ecallback()
{
    UpdateWords();
}
//---------------------------------------------------------------------------
void TfDictImport::edcallback()
{
    btnSelect->Enabled = edobj->pos >= 0;
}
//---------------------------------------------------------------------------
void TfDictImport::idraw(int col, int row, TCanvas * c, const TRect & r)
{
    if (col == 0)
        c->FillRect(r);
    if (col != 1)
        return;

    TColor textcol = c->Font->Color;
    if (c->Brush->Color != (TColor)colorsettings.background)
        fixtextcolor(textcol, c->Brush->Color);
    c->Font->Color = textcol;
    c->Font->Size = 7;

    UnicodeString str = IntToStr(imeaningcnt(!groupimport ? iwords[wobj->pos]->defs[row]->pos : rwords[wobj->pos]->defs[row]->pos));
    TSize s;

    HFONT of = (HFONT)SelectObject(c->Handle, c->Font->Handle);
    GetTextExtentPoint32(c->Handle, str.c_str(), str.Length(), &s);
    ExtTextOut(c->Handle, r.Left + (r.Width() - s.cx) / 2, r.Top + (r.Height() - s.cy) / 2, ETO_CLIPPED | ETO_OPAQUE, &r, str.c_str(), str.Length(), NULL);
    SelectObject(c->Handle, of);
}
//---------------------------------------------------------------------------
void TfDictImport::dprep(int col, int row, TWordColumnType ctype, TCanvas *c)
{
    if (dobj->selected[row] || iobj->pos < 0)
        return;

    bool dimrow;

    int dcnt = dmeaningcnt(dobj->meaningindexes[row]);

    if (!groupimport)
    {
        TImportedWord *w = iwords[wobj->pos];
        TImportedDefinition *def = w->defs[iobj->pos];
        int icnt = def->defcnt;

        dimrow = icnt != 0 && dcnt != 0;
        if (!dimrow && dcnt != 0)
        {
            bool match = false;
            for (int ix = 0; ix < w->defs.Count && !match; ++ix)
            {
                if (ix == iobj->pos || w->defs[ix]->defcnt <= 1)
                    continue;
                for (int iy = 0; iy < w->defs[ix]->defcnt && !match; ++iy)
                    if (w->defs[ix]->dictdefs[iy] == dobj->meaningindexes[row])
                        match = true;
            }
            dimrow = match;
        }

        if (!dimrow && !icnt && dcnt > 1)
        {
            bool match = false;
            for (int ix = 0; ix < dobj->count && !match; ++ix)
            {
                if (ix == row || dmeaningcnt(dobj->meaningindexes[ix]) <= 1)
                    continue;
                match = true;
            }
            dimrow = match;
        }
    }
    else
    {
        dimrow = dcnt != 0;
        TReplacedWord *r = rwords[wobj->pos];
        TImportedDefinition *def = r->defs[iobj->pos];
        if (dimrow)
        {
            for (int ix = 0; ix < def->defcnt && dimrow; ++ix)
                if (def->dictdefs[ix] == dobj->meaningindexes[row])
                    dimrow = false;
        }
    }

    if (dimrow)
        c->Font->Color = coloravg(c->Font->Color, c->Brush->Color, 0.4);
}
//---------------------------------------------------------------------------
void TfDictImport::ddraw(int col, int row, TCanvas * c, const TRect & r)
{
    if (col != 0)
        return;

    TColor textcol = c->Font->Color;
    if (c->Brush->Color != (TColor)colorsettings.background)
        fixtextcolor(textcol, c->Brush->Color);
    c->Font->Color = textcol;
    c->Font->Size = 7;

    UnicodeString str = IntToStr(dmeaningcnt(dobj->meaningindexes[row]));
    TSize s;

    HFONT of = (HFONT)SelectObject(c->Handle, c->Font->Handle);
    GetTextExtentPoint32(c->Handle, str.c_str(), str.Length(), &s);
    ExtTextOut(c->Handle, r.Left + (r.Width() - s.cx) / 2, r.Top + (r.Height() - s.cy) / 2, ETO_CLIPPED | ETO_OPAQUE, &r, str.c_str(), str.Length(), NULL);
    SelectObject(c->Handle, of);
}
//---------------------------------------------------------------------------
int TfDictImport::imeaningcnt(int pos)
{
    if (!groupimport)
    {
        TImportedWord *w = iwords[wobj->pos];
        for (int ix = 0; ix < w->defs.Count; ++ix)
            if (w->defs[ix]->pos == pos || w->defs[ix]->pos == -1 - pos)
                return w->defs[ix]->defcnt;
    }
    else
    {
        TReplacedWord *r = rwords[wobj->pos];
        for (int ix = 0; ix < r->defs.Count; ++ix)
            if (r->defs[ix]->pos == pos || r->defs[ix]->pos == -1 - pos)
                return r->defs[ix]->defcnt;
    }
    return 0;
}
//---------------------------------------------------------------------------
int TfDictImport::dmeaningcnt(int meaning)
{
    int j = 0;
    if (!groupimport)
    {
        TImportedWord *w = iwords[wobj->pos];
        for (int ix = 0; ix < w->defs.Count; ++ix)
            for (int z = 0; z < w->defs[ix]->defcnt; ++z)
                if (w->defs[ix]->dictdefs[z] == meaning)
                    ++j;
    }
    else
    {
        TReplacedWord *r = rwords[wobj->pos];
        for (int ix = 0; ix < r->defs.Count; ++ix)
            for (int z = 0; z < r->defs[ix]->defcnt; ++z)
                if (r->defs[ix]->dictdefs[z] == meaning)
                    ++j;
    }
    return j;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//-  Event Handlers  --------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfDictImport::btnAbortClick(TObject *Sender)
{
    ModalResult = mrAbort;
}
//---------------------------------------------------------------------------
void __fastcall TfDictImport::btnKanjiClick(TObject *Sender)
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
void __fastcall TfDictImport::btnSelectClick(TObject *Sender)
{
    int wix = edobj->indexes[edobj->pos];
    if (wordstaken.ItemSet(wix))
    {
        if (wordstaken.Items[wix] != ewords[wobj->pos]->Index)
        {
            MsgBox(L"This word was selected as a match for another imported entry. Please select a different one, or uncheck the word to avoid adding a new entry to the dictionary.", L"Message", MB_OK);
            return;
        }
        wcallback();
        return;
    }

    if (!groupimport)
    {
        TImportedWord *w = iwords[wobj->pos];
        w->matchdefs.Clear();
        w->defs.Clear();
        if (w->data->Index >= 0)
            wordstaken.ClearIndex(w->data->Index);
        w->modtype = TWordImportImportType(w->modtype & ~0x1000);
        TWordImportAction oldaction = w->action; // Action must be saved because FillImportWord can set this to skip if every meanings match.
        list->FillImportWord(w, *options);
        w->action = oldaction;
    }
    else
    {
        TReplacedWord *r = rwords[wobj->pos];
        r->matchdefs.Clear();
        r->defs.Clear();
        if (r->data->Index >= 0)
            wordstaken.ClearIndex(r->data->Index);
        list->FillReplaceWord(r);
    }

    ewords[wobj->pos]->Index = wix;
    wordstaken.Items[wix] = wobj->pos;

    wcallback();

}
//---------------------------------------------------------------------------
void __fastcall TfDictImport::edRomajiChange(TObject *Sender)
{
    UpdateWords();
}
//---------------------------------------------------------------------------
void __fastcall TfDictImport::btnWordSelClick(TObject *Sender)
{
    if (!groupimport)
    {
        TImportedWord *w = iwords[wobj->pos];
        w->matchdefs.Clear();
        w->defs.Clear();
        wordstaken.ClearIndex(w->data->Index);
        w->data->Index = -1;
        w->modtype = TWordImportImportType(w->modtype & ~0x1000);
        list->FillImportWord(w, *options);
    }
    else
    {
        TReplacedWord *r = rwords[wobj->pos];
        r->matchdefs.Clear();
        r->defs.Clear();
        wordstaken.ClearIndex(r->data->Index);
        r->data->Index = -1;
        list->FillReplaceWord(r);
    }

    wcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfDictImport::cbDelUnmatchedClick(TObject *Sender)
{
    if (options->wordgroups)
        return;

    TImportedWord *w = iwords[wobj->pos];
    if (cbDelUnmatched->Checked)
        w->modtype = TWordImportImportType(w->modtype | 0x1000);
    else
        w->modtype = TWordImportImportType(w->modtype & ~0x1000);
}
//---------------------------------------------------------------------------
void __fastcall TfDictImport::btnNextClick(TObject *Sender)
{
    if (wobj->pos < wobj->count - 1)
        ++wobj->pos;
    else
        ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfDictImport::FormPaint(TObject *Sender)
{
    DrawCloseBar(this, bvBottom, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfDictImport::FormResize(TObject *Sender)
{
    TRect r = Rect(0, bvBottom->Top + bvBottom->Height, ClientWidth, ClientHeight);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE);
}
//---------------------------------------------------------------------------
void __fastcall TfDictImport::edRomajiEnter(TObject *Sender)
{
    btnNext->Default = !edRomaji->Focused() && !pbKanji->Focused() && !btnSelect->Focused() && !btnAbort->Focused();
}
//---------------------------------------------------------------------------
void __fastcall TfDictImport::pbKanjiKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == VK_RETURN && btnSelect->Enabled)
        btnSelect->Click();
}
//---------------------------------------------------------------------------

