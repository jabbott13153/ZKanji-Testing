//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanjidef.h"
#include "collection.h"
#include "kanji.h"
#include "settings.h"
#include "main.h"
#include "clipview.h"
#include "kanjiinfo.h"
#include "words.h"
#include "wordobj.h"
#include "module.h"
#include "zmenu.h"
#include "gengraph.h"
#include "kanjiobj.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(74);
#endif

TfDef *fDef;
//---------------------------------------------------------------------------

/*void __fastcall TfDef::WMNCActivate(TMessage &msg)
 {
 msg.Result = DefWindowProc(Handle,WM_NCACTIVATE,(AppActive ? 1 : 0),msg.LParam);
 }*/
//---------------------------------------------------------------------------
extern wchar_t *infostr[KANJI_REFERENCE_COUNT];
__fastcall TfDef::TfDef(TComponent* Owner) : changed(false), TBaseForm(Owner)
{
    //RescaleStatusbar(sbInfo);

    EraseBackground = false;
    Constrain(492*scaleres + (Width - ClientWidth), 192*scaleres + (Height - ClientHeight));

    int ri = settings.reference_index;

    TSize s;
    HDC hdc = GetDC(pUC->Handle);
    SelectObject(hdc, pUC->Font->Handle);
    GetTextExtentPoint32W(hdc, ((UnicodeString)L" " + infostr[ri]).c_str(), wcslen(infostr[ri]) + 1, &s);
    ReleaseDC(pUC->Handle, hdc);

    pUC->Caption = (UnicodeString)" " + infostr[ri];
    lbUC->Left = s.cx + 4;
    lbUC->Width = pUC->Width - 2 - lbUC->Left;

    kobj = new TKanjiDrawObj(pbKanji);

    for (int i = 0; i < dictionary->Count; i++)
        cbDictionary->Items->Add(dictionary->Strings[i]);
    cbDictionary->ItemIndex = 0;

    wobj = new TWordListObj(pbWords, rowsizes(settings.dictfontsize), wcallback, (TPopupWordProc)NULL);
    wobj->wordselect = false;

    AppActivateEvent(this, appActivate, true, true);
    AppActivateEvent(this, appDeactivate, false, true);

    history = new TIntegerList;

    //popup menu
    menurows = new TZMenuRows(3);
    wordmenurows = new TZMenuRows(3);

    wchar_t *names[] = {
        L"Kanji", L"Kanji Info", L"",
        L"Clpb: Word", L"Copy", L"Append",
        L"Clpb: Kanji", L"Copy", L"Append"
    };
    TZMenuItemType types[] = {
        itTitle, itSimple, itSimple,
        itAltTitle, itSimple, itSimple,
        itAltTitle, itSimple, itSimple
    };
    TZPopupCommand cmd[] = {
        pcNone, pcKanjiInfo, pcNone,
        pcNone, pcCopy1, pcAppend1,
        pcNone, pcCopy2, pcAppend2
    };

    wchar_t *wordnames[] = {
        L"Clipboard", L"Copy", L"Append"
    };
    TZMenuItemType wordtypes[] = {
        itTitle, itSimple, itSimple
    };
    TZPopupCommand wordcmd[] = {
        pcNone, pcCopy1, pcAppend1
    };

    for (unsigned int ix = 0; ix < (sizeof(names) / sizeof(wchar_t*)) / 3; ++ix)
    {
        TZMenuRow *row = menurows->AddRow();
        row->Setup(names + (ix*3), types + (ix*3), (void**)(cmd + (ix*3)));
    }

    for (unsigned int ix = 0; ix < (sizeof(wordnames) / sizeof(wchar_t*)) / 3; ++ix)
    {
        TZMenuRow *row = wordmenurows->AddRow();
        row->Setup(wordnames + (ix*3), wordtypes + (ix*3), (void**)(wordcmd + (ix*3)));
    }
}
//---------------------------------------------------------------------------
__fastcall TfDef::~TfDef()
{
    delete menurows;
    delete wordmenurows;

    delete history;

    AppActivateEvent(this, appActivate, true, false);
    AppActivateEvent(this, appDeactivate, false, false);

    delete wobj;
    delete kobj;
}
//---------------------------------------------------------------------------
void TfDef::appActivate(void *caller)
{
    if (!Visible)
        return;
    PostMessage(Handle, WM_NCACTIVATE, 1, 0);
}
//---------------------------------------------------------------------------
void TfDef::appDeactivate(void *caller)
{
    if (!Visible)
        return;
    PostMessage(Handle, WM_NCACTIVATE, 0, 0);
}
//---------------------------------------------------------------------------
void TfDef::wcallback()
{

}
//---------------------------------------------------------------------------
int TfDef::ShowModalEx(byte collection, TKanjiList *klist, int index)
{
    ix = index;
    list = klist;
    coll = collection;
    kdef = 0;
    for (int i = 0; i < kanjis->Count; i++)
        if (dictionary->Items[coll]->kanjidat[i].meaning)
            kdef++;

    SetCaptions();

    return ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfDef::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfDef::btnOkClick(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfDef::edMeaningChange(TObject *Sender)
{
    btnOk->Caption = (edMeaning->Text.Trim().Length() > 0 ? "Ok" : "Reset");
}
//---------------------------------------------------------------------------
void TfDef::SetCaptions()
{
    ch = list->Kanjis[ix]->ch;

    switch (settings.reference_index)
    {
    case kdiJouyou:
        lbUC->Caption = list->Kanjis[ix]->jouyou <= 10 ? (list->Kanjis[ix]->jouyou < 9 ? IntToStr(list->Kanjis[ix]->jouyou) : (UnicodeString)L"名" + (list->Kanjis[ix]->jouyou)) : (UnicodeString)L"-";
        break;
    case kdiJLPTold:
        lbUC->Caption = list->Kanjis[ix]->JLPT;
        break;
    case kdiSnH:
        lbUC->Caption = (list->Kanjis[ix]->SnH[0] != 'x' ? list->Kanjis[ix]->SnH : 0);
        break;
    case kdiKnK:
        lbUC->Caption = list->Kanjis[ix]->knk;
        break;
    case kdiNelson:
        lbUC->Caption = list->Kanjis[ix]->nelson;
        break;
    case kdiHalpern:
        lbUC->Caption = list->Kanjis[ix]->halpern;
        break;
    case kdiHeisig:
        lbUC->Caption = list->Kanjis[ix]->heisig;
        break;
    case kdiGakken:
        lbUC->Caption = list->Kanjis[ix]->gakken;
        break;
    case kdiHenshall:
        lbUC->Caption = list->Kanjis[ix]->henshall;
        break;
    case kdiSKIP:
        lbUC->Caption = (UnicodeString)list->Kanjis[ix]->SKIP1 + "-" + list->Kanjis[ix]->SKIP2 + "-" + list->Kanjis[ix]->SKIP3;
        break;
    case kdi4C:
        lbUC->Caption = (list->Kanjis[ix]->c4[0] == 'x' ? (UnicodeString)"-" : (UnicodeString)list->Kanjis[ix]->c4 + "." + list->Kanjis[ix]->c4b);
        break;
    case kdiUnicode:
        lbUC->Caption = wtoAS(list->Kanjis[ix]->_uni).UpperCase();
        break;
    case kdiJIS:
        lbUC->Caption = wtoAS(list->Kanjis[ix]->_jis).UpperCase();
        break;
    case kdiSJIS:
        lbUC->Caption = wtoAS(JIStoShiftJIS(list->Kanjis[ix]->_jis)).UpperCase();
        break;
    case kdiEUC:
        lbUC->Caption = wtoAS(JIStoEUC(list->Kanjis[ix]->_jis)).UpperCase();
        break;
    case kdiFrequency:
        lbUC->Caption = int(list->Kanjis[ix]->frequency);
        break;
    case kdiKuten:
        lbUC->Caption = JIStoKuten(list->Kanjis[ix]->_jis);
        break;
    case kdiNJLPT:
        lbUC->Caption = list->Kanjis[ix]->NJLPT ? UnicodeString(L"N") +int(list->Kanjis[ix]->NJLPT) : UnicodeString(L"-");
        break;
    }
    /*switch(settings.reference_index) {
     case 0: lbUC->Caption = wtoAS(list->Kanjis[ix]->_uni).UpperCase(); break;
     case 1: lbUC->Caption = wtoAS(list->Kanjis[ix]->_jis).UpperCase(); break;
     case 2: lbUC->Caption = (list->Kanjis[ix]->SnH[0] == 'x' ? (UnicodeString)"-" : (UnicodeString)list->Kanjis[ix]->SnH); break;
     case 3: lbUC->Caption = (list->Kanjis[ix]->knk == 0 ? (UnicodeString)"-" : IntToStr(list->Kanjis[ix]->knk)); break;
     case 4: lbUC->Caption = (list->Kanjis[ix]->nelson == 0 ? (UnicodeString)"-" : IntToStr(list->Kanjis[ix]->nelson)); break;
     case 5: lbUC->Caption = (list->Kanjis[ix]->halpern == 0 ? (UnicodeString)"-" : IntToStr(list->Kanjis[ix]->halpern)); break;
     case 6: lbUC->Caption = (list->Kanjis[ix]->heisig == 0 ? (UnicodeString)"-" : IntToStr(list->Kanjis[ix]->heisig)); break;
     case 7: lbUC->Caption = (list->Kanjis[ix]->gakken == 0 ? (UnicodeString)"-" : IntToStr(list->Kanjis[ix]->gakken)); break;
     case 8: lbUC->Caption = (list->Kanjis[ix]->henshall == 0 ? (UnicodeString)"-" : IntToStr(list->Kanjis[ix]->henshall)); break;
     case 9: lbUC->Caption = IntToStr(list->Kanjis[ix]->SKIP1)+"-"+IntToStr(list->Kanjis[ix]->SKIP2)+"-"+IntToStr(list->Kanjis[ix]->SKIP3); break;
     case 10: lbUC->Caption = (list->Kanjis[ix]->c4[0] == 'x' ? (UnicodeString)"-" : list->Kanjis[ix]->c4+(UnicodeString)"."+list->Kanjis[ix]->c4b); break;
     case 11: lbUC->Caption = (list->Kanjis[ix]->JLPT == 0  ? (UnicodeString)"-" : IntToStr(list->Kanjis[ix]->JLPT)); break;
     case 12: lbUC->Caption = wtoAS(JIStoShiftJIS(list->Kanjis[ix]->_jis)).UpperCase(); break;
     case 13: lbUC->Caption = wtoAS(JIStoEUC(list->Kanjis[ix]->_jis)).UpperCase(); break;
     case 14: lbUC->Caption = list->Kanjis[ix]->jouyou <= 10 ? (list->Kanjis[ix]->jouyou < 9 ? IntToStr(list->Kanjis[ix]->jouyou) : (UnicodeString)L"名"+(list->Kanjis[ix]->jouyou)) : (UnicodeString)L"-"; break;
     case 15: lbUC->Caption = IntToStr(list->Kanjis[ix]->frequency).UpperCase(); break;
     }
     */
    edMeaning->Text = dictionary->KanjiMeaning(coll, list->Kanjis[ix]->index);
    btnNext->Enabled = ix < list->Count - 1;
    if (!btnNext->Enabled)
    {
        btnOk->Default = true;
        btnNext->Default = false;
    }
    kobj->kanji = ch;

    sbInfo->Panels->Items[1]->Text = IntToStr(list->Count);
    sbInfo->Panels->Items[3]->Text = IntToStr(ix + 1);
    sbInfo->Panels->Items[5]->Text = IntToStr(kdef);

    cbDictionaryChange(cbDictionary);
}
//---------------------------------------------------------------------------
void TfDef::SetDef()
{
    dictionary->ModifyKanjiMeaning(coll, list->Kanjis[ix]->index, edMeaning->Text.Trim());
}
//---------------------------------------------------------------------------
void TfDef::SetChanged()
{
    changed |= edMeaning->Text.Trim() != dictionary->KanjiMeaning(coll, list->Kanjis[ix]->index);
}
//---------------------------------------------------------------------------
void __fastcall TfDef::btnNextClick(TObject *Sender)
{
    bool canset = (!dictionary->Items[coll]->kanjidat[list->Kanjis[ix]->index].meaning);
    SetChanged();
    SetDef();
    if (canset == (bool)(dictionary->Items[coll]->kanjidat[list->Kanjis[ix]->index].meaning))
        kdef += (canset ? 1 : -1);

    history->Add(ix);

    do
    {
        ix++;
    }
    while (cbSkip->Checked && ix < list->Count && dictionary->Items[coll]->kanjidat[list->Kanjis[ix]->index].meaning);

    if (ix == list->Count)
    {
        Close();
        return;
    }

    btnBack->Enabled = true;
    SetCaptions();
    edMeaning->SetFocus();
    edMeaning->SelStart = 0;
    edMeaning->SelLength = edMeaning->Text.Length();
}
//---------------------------------------------------------------------------
void __fastcall TfDef::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (ModalResult == mrOk)
    {
        SetChanged();
        SetDef();
    }
    if (changed)
        ModalResult = mrOk;
    else
        ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfDef::cbDictionaryChange(TObject *Sender)
{
    TWordIndexList *l = new TWordIndexList(dictionary->Items[cbDictionary->ItemIndex]);
    lbOrig->Caption = (dictionary->Items[cbDictionary->ItemIndex]->kanjidat[list->Kanjis[ix]->index].meaning ? dictionary->Items[cbDictionary->ItemIndex]->kanjidat[list->Kanjis[ix]->index].meaning : kanjis->Kanjis[list->Kanjis[ix]->index]->meaning);
    if (KANJI(ch))
        fMain->SearchK(l, true, true, false, ch);
    else
        fMain->SearchJapan(l, true, true, false, UnicodeString(ch));
    wobj->set_by_indexes(l, true);
    delete l;
}
//---------------------------------------------------------------------------
void __fastcall TfDef::pbWordsEnter(TObject *Sender)
{
    btnNext->Default = false;
}
//---------------------------------------------------------------------------
void __fastcall TfDef::pbWordsExit(TObject *Sender)
{
    btnNext->Default = true;
}
//---------------------------------------------------------------------------
void __fastcall TfDef::pbWordsKeyPress(TObject *Sender, wchar_t &Key)
{
    if (!wcschr(cdelim, Key) && Key != VK_BACK && Key != VK_RETURN)
    {
        edMeaning->SetFocus();
        edMeaning->SelLength = 0;
        edMeaning->Text = edMeaning->Text + Key;
        edMeaning->SelStart = edMeaning->Text.Length();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDef::pbWordsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_BACK)
    {
        edMeaning->SetFocus();
        edMeaning->SelLength = 0;
        edMeaning->Text = edMeaning->Text.SubString(1, edMeaning->Text.Length() - 1);
        edMeaning->SelStart = edMeaning->Text.Length();
    }
    if (Key == VK_RETURN)
        edMeaning->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfDef::edMeaningKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if ((Key == VK_UP || Key == VK_DOWN) && wobj->count)
    {
        pbWords->SetFocus();
        pbWords->OnKeyDown(Sender, Key, Shift);
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDef::btnBackClick(TObject *Sender)
{
    btnBack->Enabled = history->Count > 1;
    ix = history->Items[history->Count - 1];
    history->Delete(history->Count - 1);
    SetCaptions();

    edMeaning->SetFocus();
    edMeaning->SelStart = 0;
    edMeaning->SelLength = edMeaning->Text.Length();
}
//---------------------------------------------------------------------------

void __fastcall TfDef::pbWordsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button == mbLeft && Shift.Contains(ssDouble))
    {
        wchar_t c = wobj->KanjiAtPos(x, y);

        if (!c || !KANJI(c))
            return;
        ShowKanjiInfo(coll, kanjis->Kanjis[kanjis->IndexOf(c)]);
    }
}
//---------------------------------------------------------------------------
void TfDef::popupcallback(TZMenuRows *rows, int col, int row)
{
    TZMenuRow *r = rows->Rows[row];
    TZPopupCommand cmd = (TZPopupCommand)r->Data[col];

    if (cmd == pcNone)
        return;

    switch (cmd)
    {
    case pcKanjiInfo:
        ShowKanjiInfo(dictionary->Index[wobj->collection], popkanji);
        break;
    case pcCopy1:
        ClipReplace(poptext);
        break;
    case pcAppend1:
        ClipAppend(poptext);
        break;
    case pcCopy2:
        ClipReplace(popkanji->ch);
        break;
    case pcAppend2:
        ClipAppend(popkanji->ch);
        break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDef::pbWordsMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button == mbLeft)
        return;

    TPoint p, p2;
    GetCursorPos(&p);
    p2 = pbWords->ScreenToClient(p);

    int ix;
    char m;
    if (!wobj->WordAtPos(p2.y, ix, m))
        return;

    wchar_t c;
    if ((c = wobj->KanjiAtPos(p2.x, p2.y)) != 0)
        popkanji = kanjis->Kanjis[kanjis->IndexOf(c)];
    else
        popkanji = NULL;

    poptext = (p2.x < wobj->colsize[0] + wobj->colsize[1] ? wobj->words[wobj->pos]->kanji : wobj->words[wobj->pos]->kana);

    if (popkanji)
        menu->Popup(menurows, p.x, p.y, &popupcallback);
    else
        menu->Popup(wordmenurows, p.x, p.y, &popupcallback);

}
//---------------------------------------------------------------------------
void __fastcall TfDef::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE)
    {
        if (fInfo->Visible)
        {
            fInfo->Close();
            Key = 0;
        }
        else
            btnCancelClick(btnCancel);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDef::FormPaint(TObject *Sender)
{
    TCanvas *c = Canvas;

    DrawPillowPanel(Canvas, Rect(bvleft->Left + 2, 0, ClientWidth, bv1->Top));
    DrawPillowPanel(this, bv2, bv3, true);
    DrawPillowPanel(this, bv4, bv5, true);
    DrawCloseBar(this, bv7, NULL);

    c->Brush->Color = clWhite;
    c->FillRect(Rect(bvleft->Left + 2, bv1->Top + 2, ClientWidth, bv2->Top));
    c->FillRect(Rect(bvleft->Left + 2, bv3->Top + 2, ClientWidth, bv4->Top));
}
//---------------------------------------------------------------------------
void __fastcall TfDef::FormResize(TObject *Sender)
{
    TRect r = Rect(0, bv7->Top + bv7->Height, ClientWidth, ClientHeight);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE | RDW_NOCHILDREN);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(75);
#endif

