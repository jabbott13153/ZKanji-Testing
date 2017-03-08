//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "vocabform.h"
#include "main.h"
#include "clipview.h"
#include "wordobj.h"
#include "collection.h"
#include "settings.h"
#include "words.h"
#include "wordgroup.h"
#include "module.h"
#include "zmenu.h"
#include "searchtree.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(170);
#endif

TfAddVocab *fAddVocab;
//---------------------------------------------------------------------------

void TfAddVocab::WMNCHitTest(TWMNCHitTest& msg)
{
    TMessage *msgo = (TMessage*) &msg;
    msg.Result = DefWindowProc(Handle, msg.Msg, msgo->WParam, msgo->LParam);

    if (ClientHeight > pbMeanings->Top + sbInfo->Height)
        return;

    if (msg.Result == HTLEFT || msg.Result == HTRIGHT || msg.Result == HTTOPLEFT || msg.Result == HTTOPRIGHT || msg.Result == HTTOP || msg.Result == HTBOTTOM || msg.Result == HTBOTTOMLEFT || msg.Result == HTBOTTOMRIGHT)
        msg.Result = HTBORDER;

    TPoint p = ClientToScreen(Point(0, 0));
    if (msg.XPos - p.x >= ClientWidth)
        msg.Result = HTRIGHT;
    else if (msg.XPos - p.x < 0)
        msg.Result = HTLEFT;
}

//---------------------------------------------------------------------------
__fastcall TfAddVocab::TfAddVocab(TComponent* Owner) : TBaseForm(Owner)
{
    //RescaleStatusbar(sbInfo);
    cbCollection->MaxLength = dictionary_name_length_limit;
    cbGroups->MaxLength = group_name_length_limit;

    cbCollection->Items->Assign(dictionary);
    EraseBackground = false;

    p1 = new TStatusPanel(NULL);
    p2 = new TStatusPanel(NULL);
    p1->Assign(sbInfo->Panels->Items[0]);
    p2->Assign(sbInfo->Panels->Items[1]);
    pgroups = sbInfo->Panels->Items[3];

    ShowHidePanels(false);
    ClientHeight = pbMeanings->Top + sbInfo->Height;

    wobj = new TWordListObj(pbMeanings, rowsizes(settings.dictfontsize), wcallback, (TPopupWordProc)NULL);
    wobj->wordselect = false;

    Constrain(Width, Height);

    Width = settings.vocabwidth;

    // Popup menu.
    menurows = new TZMenuRows(3);

    wchar_t *names[] = {
        L"Clipboard", L"Copy", L"Append",
        L"Entry", L"Edit...", L"Delete"
    };
    TZMenuItemType types[] = {
        itTitle, itSimple, itSimple,
        itAltTitle, itSimple, itSimple
    };
    TZPopupCommand cmd[] = {
        pcNone, pcCopy1, pcAppend1,
        pcNone, pcEditEntry, pcDeleteEntry
    };

    for (unsigned int ix = 0; ix < (sizeof(names) / sizeof(wchar_t*)) / 3; ++ix)
    {
        TZMenuRow *row = menurows->AddRow();
        row->Setup(names + (ix*3), types + (ix*3), (void**)(cmd + (ix*3)));
    }
}
//---------------------------------------------------------------------------
__fastcall TfAddVocab::~TfAddVocab()
{
    delete menurows;

    delete p1;
    delete p2;
    delete wobj;

    settings.vocabwidth = Width;
}
//---------------------------------------------------------------------------
void TfAddVocab::ShowHidePanels(bool show)
{
    int cnt = sbInfo->Panels->Count;
    if (cnt == (show ? 4 : 2))
        return;
    sbInfo->Panels->BeginUpdate();
    if (!show)
    {
        sbInfo->Panels->Delete(0);
        sbInfo->Panels->Delete(0);
    }
    else
    {
        sbInfo->Panels->Insert(0);
        sbInfo->Panels->Insert(0);
        sbInfo->Panels->Items[0]->Assign(p1);
        sbInfo->Panels->Items[1]->Assign(p2);
    }
    sbInfo->Panels->EndUpdate();
}
//---------------------------------------------------------------------------
void TfAddVocab::wcallback()
{
    UnicodeString s = "";
    TWordGroups *g = NULL;
    int ix;
    if (wobj->count && wobj->pos >= 0)
    {
        g = wobj->words[wobj->pos]->group;
        ix = wobj->pos;
    }
    else if (RealIndex(cbCollection) == srcindex)
    {
        g = words->Items[findex]->group;
        ix = cbMeaning->ItemIndex;
    }
    while (g)
    {
        if (g->meaning == ix)
            s = g->group->name + (s.Length() ? ", " : "") + s;
        g = g->next;
    }
    if (!s.Length())
        s = "The selected meaning is not in any group.";

    pgroups->Text = s;
    if (sbInfo->Panels->Count == 4)
        sbInfo->Panels->Items[1]->Text = wobj->pos + 1;
}
//---------------------------------------------------------------------------
int TfAddVocab::ShowModalEx(TWordCollection *collection, int aindex)
{
    srcindex = dictionary->Index[collection];

    findex = aindex;
    words = collection->words;

    if (srcindex < 0)
        THROW(L"Error adding to vocabulary! No source!");

/*
    if (srcindex)
    {
        cbCollection->Items->Delete(0);
        cbCollection->Tag = 1;
    }
*/

    cbCollection->ItemIndex = dictionary->IndexOf(settings.vocabcollection) /*- cbCollection->Tag*/;
    if (cbCollection->ItemIndex < 0)
        cbCollection->ItemIndex = srcindex /*- cbCollection->Tag*/;

    cbCollectionChange(cbCollection);

    cbGroups->ItemIndex = cbGroups->Items->IndexOf(settings.vocabgroup);

    if (RealIndex(cbGroups) < 0 && RealIndex(cbCollection) == srcindex /*- cbCollection->Tag*/)
    {
        ActiveControl = cbGroups;
        cbGroups->Text = "New Group";
        cbGroups->SelectAll();
    }

    cbGroupsChange(cbGroups);

    for (int i = 0; i < words->Items[index]->meaningcnt; i++)
        cbMeaning->Items->Add(IntToStr(i + 1) + ". " + words->Items[index]->data[i].meaning);
    cbMeaning->ItemIndex = 0;
    cbMeaning->Enabled = words->Items[index]->meaningcnt > 1;

    if (edMeaning->Enabled)
        ActiveControl = edMeaning;
    else
        ActiveControl = cbGroups;

    editword = -1;
    wcallback();

    ShowModal();
    if (editword >= 0)
        EditWordDef(dictionary->Items[editcoll], editword, editmeaning);
    else if (ModalResult == mrOk)
    {
        settings.vocabcollection = cbCollection->Text.Trim();
        settings.vocabgroup = cbGroups->Text.Trim();
    }

    return ModalResult;
}
//---------------------------------------------------------------------------
void __fastcall TfAddVocab::pbKanjiPaint(TObject *Sender)
{
    TPaintBox *pb = (TPaintBox*)Sender;
    TRect r = pb->ClientRect;
    TCanvas *c = pb->Canvas;
    wchar_t *str = (Sender == pbKanji ? words->Items[index]->kanji : words->Items[index]->kana);

    SetTextAlign(c->Handle, TA_BASELINE | TA_LEFT);
    c->Brush->Color = clWhite;
    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = pb->ClientHeight * 0.6;
    c->Font->Color = clBlack;

    ExtTextOutW(c->Handle, 1, pb->Height*0.8, ETO_CLIPPED | ETO_OPAQUE, &r, str, wcslen(str), NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfAddVocab::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if (ModalResult == mrCancel)
        return;

    int r;
    int coll;

    if (!ValidFileName(cbCollection->Text.Trim()))
    {
        MsgBox("Dictionary name not valid! Try using alphanumeric characters and space.", "Error", MB_OK);
        CanClose = false;
        return;
    }

    MODON
    bool mod = false;
    bool dictmod = false;

    if (CollectionIndex < 0) // New dictionary?
    {
        r = MsgBox("Do you want to create a new dictionary?", "Query", MB_YESNOCANCEL);
        if (r == IDNO)
        {
            ModalResult = mrCancel;
            return;
        }
        if (r == IDCANCEL)
        {
            CanClose = false;
            return;
        }
        coll = dictionary->Add(cbCollection->Text.Trim()/*,wctWords*/);
        if (coll < 0)
        {
            MsgBox("Dictionary with the same name already exists.", "Error", MB_OK);
            CanClose = false;
            return;
        }
        mod = true;
        dictmod = true;
    }
    else
        coll = CollectionIndex;

    fmeaning = cbMeaning->ItemIndex; // Which meaning is assigned.
    int wix = index;

    if (coll != srcindex)
    {
        if (!cbDefinition->Visible || !cbDefinition->Enabled || !cbDefinition->Checked) // Add new word.
        {
            if ((wix = dictionary->WordIndex(coll, words->Items[index]->kanji, words->Items[index]->kana)) >= 0)
            {
                if (dictionary->WordMeaningIndex(coll, wix, !edMeaning->Text.Trim().IsEmpty() ? edMeaning->Text.Trim().c_str() : words->Items[index]->data[meaning].meaning ) != -1)
                {
                    MsgBox(L"A meaning with the same text is already added to the word. Duplicates are not allowed.", L"Error", MB_OK);
                    CanClose = false;
                }

                dictionary->BeginWordChange(coll);
                try
                {
                    TWord *w = words->Items[index];
                    if (edMeaning->Text.Trim().IsEmpty())
                        fmeaning = dictionary->CopyWordMeaning(coll, wix, w, meaning);
                    else
                        fmeaning = dictionary->AddWordMeaning(coll, wix, edMeaning->Text.Trim().c_str(), w->data[meaning].types, w->data[meaning].notes, w->data[meaning].fields, w->data[meaning].nametag);
                    mod = true;
                    dictmod = true;
                }
                catch (...)
                {
                    ;
                }
                dictionary->EndWordChange();
            }
            else
            {
                dictionary->BeginWordChange(coll);
                try
                {
                    wix = dictionary->AddWord(coll, words->Items[index], meaning, (!edMeaning->Text.Trim().IsEmpty() ? edMeaning->Text.Trim() : UnicodeString()));
                    mod = true;
                    dictmod = true;
                }
                catch (...)
                {
                    ;
                }
                dictionary->EndWordChange();
                fmeaning = 0; // New word's first meaning.
            }
        }
        else // Use existing word.
        {
            wix = dictionary->WordIndex(coll, words->Items[index]->kanji, words->Items[index]->kana);
            fmeaning = wobj->pos;
        }
    }

    int group;

    // Create new group when needed.
    if (GroupIndex < 0)
    {
        if (!cbGroups->Text.Trim().Length())
        {
            if (mod)
                dictionary->Items[coll]->Modify(dictmod);
            return;
        }
        group = dictionary->AddWordGroup(coll, cbGroups->Text.Trim(), false);
        mod = true;
    }
    else
        group = GroupIndex;

    findex = wix;
    if (dictionary->Items[coll]->groups->WordInGroup(group, wix, meaning))
        return;

    dictionary->Items[coll]->groups->Items[group]->AddWord(wix, meaning);
    if (mod)
        dictionary->Items[coll]->Modify(dictmod);
    MODOFF
}
//---------------------------------------------------------------------------
void __fastcall TfAddVocab::cbGroupsChange(TObject *Sender)
{
    if (cbDefinition->Visible && edMeaning->Text.Trim().IsEmpty())
        cbDefinition->Checked = true;
    change_buttons();
}
//---------------------------------------------------------------------------
void __fastcall TfAddVocab::cbCollectionChange(TObject *Sender)
{
    if (CollectionIndex < 0)
    {
        cbGroups->Items->Clear();
        change_buttons(true);
        return;
    }

    int coll = CollectionIndex;

    cbGroups->Items->BeginUpdate();
    try
    {
        TWordGroupList *group = dictionary->Items[coll]->groups;
        cbGroups->Items->Clear();
        while (cbGroups->Items->Count < group->Count)
            cbGroups->Items->Add(group->Items[cbGroups->Items->Count]->name);
    }
    catch (...)
    {
        ;
    }
    cbGroups->Items->EndUpdate();
    if (cbGroups->Items->Count)
        cbGroups->ItemIndex = 0;
    change_buttons(true);
}
//---------------------------------------------------------------------------
void TfAddVocab::change_buttons(bool change_dictionary)
{
    lbMeaning->Enabled = CollectionIndex != srcindex;
    edMeaning->Enabled = CollectionIndex != srcindex;

    int coll = CollectionIndex;
    bool okenabled = CollectionIndex < 0 && ValidFileName(cbCollection->Text.Trim()) || (coll >= 0 && coll != srcindex || cbCollection->Items->Count < 255 && cbGroups->Text.Trim().Length() > 0);

    bool wordlist = ClientHeight != pbMeanings->Top + sbInfo->Height;

    int wix;

    if (coll >= 0 && coll != srcindex && (wix = dictionary->WordIndex(coll, words->Items[index]->kanji, words->Items[index]->kana)) >= 0)
    {
        if (change_dictionary)
        {
            TWordIndexList *l = new TWordIndexList(dictionary->Items[coll]);
            for (int j = 0; j < dictionary->Items[coll]->words->Items[wix]->meaningcnt; j++)
                l->Add(wix, j);
            wobj->set_by_indexes(l);
            ShowHidePanels(true);
            sbInfo->Panels->Items[0]->Text = IntToStr(l->Count) + " :";
            delete l;

            if (wobj->count)
                menurows->Rows[1]->Enabled[2] = dictionary->Items[0] != wobj->collection || dictionary->Originals->IsAdded(wobj->currentword);
        }
        cbDefinition->Enabled = !cbGroups->Text.Trim().IsEmpty();
        if (!cbDefinition->Enabled)
            cbDefinition->Checked = false;

        if (!wordlist && wobj->count)
        {
            Height = settings.vocabheight;
            Constrain(MinWidth, pbMeanings->Top + 67 * scaleres + sbInfo->Height + (Height - ClientHeight));
        }

        cbDefinition->Visible = true;

        lbWarning->Visible = (!cbDefinition->Checked || !edMeaning->Text.Trim().IsEmpty()) && wobj->count != 0 && wobj->count < 100;

        if (((!cbGroups->Text.Trim().Length() || !cbDefinition->Checked) || (!edMeaning->Text.Trim().IsEmpty())) && wobj->count == 100)
            okenabled = false;
//        if (lbWarning->Visible && cbDefinition->Checked)
//        {
//            cbDefinition->Checked = false;
//            wcallback();
//        }
//        else if (edMeaning->Text.Trim().IsEmpty() && !cbDefinition->Checked && cbGroups->Text.Trim().Length())
//        {
//            cbDefinition->Checked = true;
//            wcallback();
//        }
        lbInfo->Visible = false;
    }
    else
    {
        cbDefinition->Visible = false;
        lbWarning->Visible = false;
        lbInfo->Visible = true;

        if (wordlist)
        {
            Constrain(MinWidth, pbMeanings->Top + sbInfo->Height + (Height - ClientHeight));
            ClientHeight = pbMeanings->Top + sbInfo->Height;
        }

        wobj->set_by_indexes(NULL);
        ShowHidePanels(false);
    }

    btnOk->Enabled = okenabled;
    wcallback();
}
//---------------------------------------------------------------------------
int TfAddVocab::get_collectionindex()
{
    return RealIndex(cbCollection);
}
//---------------------------------------------------------------------------
int TfAddVocab::get_groupindex()
{
    return RealIndex(cbGroups);
}
//---------------------------------------------------------------------------
void __fastcall TfAddVocab::cbDefinitionClick(TObject *Sender)
{
    change_buttons();
    wcallback();
}
//---------------------------------------------------------------------------
void __fastcall TfAddVocab::cbMeaningChange(TObject *Sender)
{
    wcallback();
    change_buttons();
}
//---------------------------------------------------------------------------
void __fastcall TfAddVocab::FormPaint(TObject *Sender)
{
    DrawPillowPanel(this, NULL, b0);
    DrawPillowPanel(this, b2, b3, true);
    DrawPillowPanel(this, b3, b4, true);
    DrawPillowPanel(this, b4, b5, true);
}
//---------------------------------------------------------------------------
void TfAddVocab::popupcallback(TZMenuRows *rows, int col, int row)
{
    TZMenuRow *r = rows->Rows[row];
    TZPopupCommand cmd = (TZPopupCommand)r->Data[col];

    if (cmd == pcNone)
        return;

    try
    {
        switch (cmd)
        {
        case pcCopy1:
            ClipReplace(poptext);
            break;
        case pcAppend1:
            ClipAppend(poptext);
            break;
        case pcEditEntry:
            editword = wobj->current;
            editmeaning = wobj->pos;
            editcoll = CollectionIndex;
            ModalResult = mrCancel;
            break;
        case pcDeleteEntry:
            if (wobj->count > 1)
            {
                if (MsgBox("This definition will be removed from every group that contains it.\nDo you wish to continue?", "Warning", MB_YESNO) == IDNO)
                    return;
                dictionary->BeginWordChange(dictionary->Index[wobj->collection]);
                try
                {
                    dictionary->DeleteWordMeaning(dictionary->Index[wobj->collection], wobj->current, wobj->currentmeaning);
                }
                catch (...)
                {
                    ;
                }
                dictionary->EndWordChange();
                change_buttons(true);
            }
            else
            {
                if (wobj->collection == dictionary->Items[0] && !dictionary->Originals->IsAdded(wobj->currentword))
                {
                    MsgBox("Words not added by the user that are in the base dictionary cannot be deleted.", "Warning", MB_OK);
                    return;
                }
                if (MsgBox("The word will be removed from this dictionary and from every group in the\ndictionary that contains it. The word's study statistics will be removed as well.\nDo you wish to continue?", "Warning", MB_YESNO) == IDNO)
                    return;
                dictionary->BeginWordChange(dictionary->Index[wobj->collection]);
                try
                {
                    dictionary->DeleteWord(dictionary->Index[wobj->collection], wobj->current);
                }
                catch (...)
                {
                    ;
                }
                dictionary->EndWordChange();
                ModalResult = mrCancel;
            }
            break;
        }
    }
    catch (...)
    {
        ;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfAddVocab::pbMeaningsMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button == mbLeft)
        return;

    int ix;
    char m;
    if (!wobj->WordAtPos(y, ix, m))
        return;

    poptext = (x < wobj->colsize[0] + wobj->colsize[1] ? wobj->words[wobj->pos]->kanji : wobj->words[wobj->pos]->kana);

    TPoint p = pbMeanings->ClientToScreen(Point(x, y));
    menu->Popup(menurows, p.x, p.y, &popupcallback);
}
//---------------------------------------------------------------------------
void __fastcall TfAddVocab::pbMeaningsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button == mbLeft && Shift.Contains(ssDouble))
    {
        TPoint p;
        GetCursorPos(&p);
        p = pbMeanings->ScreenToClient(p);
        int ix;
        char m;
        if (!wobj->WordAtPos(p.y, ix, m))
            return;

        editword = wobj->current;
        editmeaning = wobj->pos;
        editcoll = CollectionIndex;
        ModalResult = mrCancel;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfAddVocab::FormResize(TObject *Sender)
{
    if (Visible && ClientHeight != pbMeanings->Top + sbInfo->Height)
        settings.vocabheight = Height;
}
//---------------------------------------------------------------------------
void __fastcall TfAddVocab::edMeaningExit(TObject *Sender)
{
    change_buttons();
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(171);
#endif


