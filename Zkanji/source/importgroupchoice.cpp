//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "importgroupchoice.h"
#include "import.h"
#include "gengraph.h"
#include "collection.h"
#include "wordgroup.h"
#include "kanjigroup.h"
#include "wordobj.h"
#include "settings.h"
#include "kanjigrid.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"
TfImportGroupChoice *fImportGroupChoice;
//---------------------------------------------------------------------------
__fastcall TfImportGroupChoice::TfImportGroupChoice(TComponent* Owner) : TBaseForm(Owner), wold(NULL), wnew(NULL), kold(NULL), knew(NULL), sizing(false), splitsize(0.5)
{
    pTop->Align = alNone;
    pBottom->Align = alNone;

    Constrain(pButtons->Width, pButtons->Height * 6);
}
//---------------------------------------------------------------------------
__fastcall TfImportGroupChoice::~TfImportGroupChoice()
{
    delete wold;
    delete wnew;
    delete kold;
    delete knew;
}
//---------------------------------------------------------------------------
int TfImportGroupChoice::ShowModalEx(TWordCollection *acoll, TImportList *alist, bool awordgroups)
{
    list = alist;
    coll = acoll;
    wordgrp = awordgroups;
    Caption = L"Rename or skip imported groups: " + UnicodeString(awordgroups ? L"word" : L"kanji");

    if (awordgroups)
    {
        for (int ix = 0; ix < coll->groups->Count; ++ix)
            cbOld->Items->Add(coll->groups->Names[ix]);
        cbOld->ItemIndex = 0;
        for (int ix = 0; ix < list->WordGroupCount; ++ix)
            cbNew->Items->Add(list->WordGroups[ix]->Name);
        cbNew->CheckAll(cbChecked, false, false);
        cbNew->ItemIndex = 0;

        wold = new TVocabListObj(pbOld, rowsizes(settings.dictfontsize), (TFuncPtr)NULL, (TPopupWordProc)NULL);
        wold->wordselect = false;
        wnew = new TCallbackWordListObj(pbNew, rowsizes(settings.dictfontsize), (TFuncPtr)NULL, (TPopupWordProc)NULL);
        wnew->wordselect = false;
        wnew->NumProc = wnumproc;
        wnew->StringProc = wstrproc;
        wnew->WordTagProc = wtagproc;
    }
    else
    {
        for (int ix = 0; ix < coll->kgroups->Count; ++ix)
            cbOld->Items->Add(coll->kgroups->Names[ix]);
        cbOld->ItemIndex = 0;
        for (int ix = 0; ix < list->KanjiGroupCount; ++ix)
            cbNew->Items->Add(list->KanjiGroups[ix]->Name);
        cbNew->CheckAll(cbChecked, false, false);
        cbNew->ItemIndex = 0;

        kold = new TKanjiGrid(pbOld, 50 * scaleres, false, (TFuncPtr)NULL);
        knew = new TKanjiGrid(pbNew, 50 * scaleres, false, (TFuncPtr)NULL);

        kold->showhint = false;
        knew->showhint = false;
    }

    UpdateButtons();
    cbOldClick(cbOld);
    cbNewClick(cbNew);

    int result = ShowModal();

    return result;
}
//---------------------------------------------------------------------------
void TfImportGroupChoice::SetSplitSize(double siz)
{
    int toph = max(pButtons->Height * 2, bvBottom->Top * siz);
    int bottomh = max(pButtons->Height * 2, bvBottom->Top - toph);
    toph = bvBottom->Top - bottomh;

    if (pTop->Height == toph && pBottom->Height == bottomh && pTop->Width == ClientWidth && pBottom->Width == ClientWidth)
        return;

    HDWP def = BeginDeferWindowPos(2);
    def = DeferWindowPos(def, pTop->Handle, NULL, 0, 0, ClientWidth, toph, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    def = DeferWindowPos(def, pBottom->Handle, NULL, 0, toph, ClientWidth, bottomh, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
    EndDeferWindowPos(def);

    TRect r = ChildRect(pBottomTitle, 0, 0, 0, -1);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);

    r = ChildRect(pAction, 1, 0, -1, 0);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);

    r = ChildRect(cbOld, -2, -2, 2, 2);
    HRGN rgn = CreateRectRgnIndirect(&r);
    InflateRect(&r, -2, -2);
    HRGN rgn2 = CreateRectRgnIndirect(&r);
    CombineRgn(rgn, rgn, rgn2, RGN_DIFF);
    DeleteObject(rgn2);
    RedrawWindow(Handle, NULL, rgn, RDW_INVALIDATE | RDW_ALLCHILDREN);
    DeleteObject(rgn);

    r = ChildRect(cbNew, -2, -2, 2, 2);
    rgn = CreateRectRgnIndirect(&r);
    InflateRect(&r, -2, -2);
    rgn2 = CreateRectRgnIndirect(&r);
    CombineRgn(rgn, rgn, rgn2, RGN_DIFF);
    DeleteObject(rgn2);
    RedrawWindow(Handle, NULL, rgn, RDW_INVALIDATE | RDW_ALLCHILDREN);
    DeleteObject(rgn);
}
//---------------------------------------------------------------------------
void TfImportGroupChoice::UpdateButtons()
{
    btnRename->Enabled = cbNew->ItemIndex >= 0;
    btnChkAll->Enabled = cbNew->Items->Count > 0;
    btnChkNone->Enabled = cbNew->Items->Count > 0;
}
//---------------------------------------------------------------------------
word TfImportGroupChoice::wnumproc(unsigned int ix, TWordNumProcWanted wanted)
{
    if (wanted == wnpwFreq)
        return wordgroup->Items[ix]->Frequency;
    return wordgroup->Items[ix]->DefCount;
}
//---------------------------------------------------------------------------
UnicodeString TfImportGroupChoice::wstrproc(unsigned int ix, byte m, TWordStringProcWanted wanted)
{
    TExportedWord *w = wordgroup->Items[ix];
    if (wanted == wspwMeaning)
        return w->Def[m]->data.meaning;
    if (wanted == wspwKanji)
        return w->Kanji(coll);
    return w->Kana(coll);
}
//---------------------------------------------------------------------------
void TfImportGroupChoice::wtagproc(unsigned int ix, byte m, int &types, int &notes, byte &fields, word &nametag)
{
    TExportedWord *w = wordgroup->Items[ix];
    types = w->Def[m]->data.types;
    notes = w->Def[m]->data.notes;
    fields = w->Def[m]->data.fields;
    nametag = w->Def[m]->data.nametag;
}
//---------------------------------------------------------------------------
TImportWordGroup* TfImportGroupChoice::get_wgroup()
{
    return cbNew->ItemIndex == -1 ? NULL : list->WordGroups[cbNew->ItemIndex];
}
//---------------------------------------------------------------------------
TImportKanjiGroup* TfImportGroupChoice::get_kgroup()
{
    return cbNew->ItemIndex == -1 ? NULL : list->KanjiGroups[cbNew->ItemIndex];
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//- Events ------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::FormPaint(TObject *Sender)
{
    DrawCloseBar(this, bvBottom, NULL);
    TRect r = ChildRect(pTopTitle, 0, 0, 0, -1);
    DrawPillowPanel(Canvas, r, true, false);

    r = ChildRect(pBottomTitle);
    DrawPillowPanel(Canvas, r, true, false);

    r = ChildRect(pAction, 1, 0, -1, 0);
    DrawHorzPillowPanel(Canvas, r, true, false);

    r = ChildRect(cbOld, -2, -2, 2, 2);
    Canvas->Brush->Color = cbOld->Color;
    Canvas->FillRect(r);

    r = ChildRect(cbNew, -2, -2, 2, 2);
    Canvas->Brush->Color = cbNew->Color;
    Canvas->FillRect(r);
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::FormResize(TObject *Sender)
{
    SetSplitSize(splitsize);
    TRect r = Rect(0, bvBottom->Top + bvBottom->Height, ClientWidth, ClientHeight);
    RedrawWindow(Handle, &r, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE);
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::sSizerMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    sizepos = y;
    sizing = Screen->Cursor == crVSplit;
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::sSizerMouseEnter(TObject *Sender)
{
    Screen->Cursor = crVSplit;
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::sSizerMouseLeave(TObject *Sender)
{
    Screen->Cursor = crDefault;
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::sSizerMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    if (!Shift.Contains(ssLeft))
        sizing = false;
    if (sizing)
    {
        splitsize = double(pBottom->Top + (y - sizepos)) / bvBottom->Top;
        SetSplitSize(splitsize);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::sSizerMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    sizing = false;
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::btnAbortClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::btnOkClick(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::btnChkAllClick(TObject *Sender)
{
    cbNew->Items->BeginUpdate();
    cbNew->CheckAll(cbChecked, true, true);
    cbNew->Items->EndUpdate();
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::btnChkNoneClick(TObject *Sender)
{
    cbNew->Items->BeginUpdate();
    cbNew->CheckAll(cbUnchecked, true, true);
    cbNew->Items->EndUpdate();
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::cbNewClickCheck(TObject *Sender)
{
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::btnRenameClick(TObject *Sender)
{
    UnicodeString s = cbNew->Items->Strings[cbNew->ItemIndex];
    if (QueryBox(L"Enter new name for this group:", L"Rename group", s) && s.Trim().Length())
    {
        s = s.Trim();
        if (s.Length() > group_name_length_limit)
            s.SetLength(group_name_length_limit);

        int r = IDOK;
        if (StriIndex(s, cbNew->Items) >= 0)
            r = MsgBox(L"Another group with same name is imported. They will be merged.", L"Message", MB_OKCANCEL);
        if (r == IDOK)
            cbNew->Items->Strings[cbNew->ItemIndex] = s;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::cbOldClick(TObject *Sender)
{
    if (wordgrp)
    {
        wold->wordlist = cbOld->ItemIndex == -1 ? NULL : coll->groups->Lists[cbOld->ItemIndex];
    }
    else
    {
        if (cbOld->ItemIndex >= 0)
            kold->set_items(coll->kgroups->List[cbOld->ItemIndex], coll);
        else
        {
            TKanjiItemList list;
            kold->set_items(NULL, coll);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::cbNewClick(TObject *Sender)
{
    if (wordgrp)
    {
        wnew->count = wordgroup ? wordgroup->Count : 0;
    }
    else
    {
        TKanjiItemList list;
        if (kanjigroup)
        {
            for (int ix = 0; ix < kanjigroup->Count; ++ix)
                list.Add(kanjigroup->Items[ix]->Kanji);
        }
        knew->set_items(&list, coll);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfImportGroupChoice::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (ModalResult != mrOk)
        return;

    if (wordgrp)
    {
        for (int ix = 0; ix < cbNew->Count; ++ix)
        {
            list->WordGroups[ix]->Name = cbNew->Items->Strings[ix].c_str();
            if (!cbNew->Checked[ix])
                list->WordGroups[ix]->Action = giaSkip;
        }
    }
    else
    {
        for (int ix = 0; ix < cbNew->Count; ++ix)
        {
            list->KanjiGroups[ix]->Name = cbNew->Items->Strings[ix].c_str();
            if (!cbNew->Checked[ix])
                list->KanjiGroups[ix]->Action = giaSkip;
        }
    }
}
//---------------------------------------------------------------------------

