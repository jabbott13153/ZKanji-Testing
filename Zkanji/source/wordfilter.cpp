//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "wordfilter.h"
#include "settings.h"
#include "zkformats.h"
#include "gengraph.h"
#include "wordlist.h"
#include "popupdict.h"
#include "checktreeview.h"
#include "controlimages.h"
#include "mouseevent.h"
#include "words.h"
#include "genworddata.h"
#include "module.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(176);
#endif

TfFilter *fFilter;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int TPlusMinusCheckTreeElem::instances = 0;
TDrawnPlusbutton* TPlusMinusCheckTreeElem::drawnplus = NULL;
TDrawnMinusbutton* TPlusMinusCheckTreeElem::drawnminus = NULL;
//---------------------------------------------------------------------------
TPlusMinusCheckTreeElem::TPlusMinusCheckTreeElem(const wchar_t *atitle, const bool ausebox, const void *adata) : inherited(/*etPlusMinus,*/atitle, ausebox, adata)
{
    if (++instances == 1)
    {
        drawnplus = new TDrawnPlusbutton;
        drawnminus = new TDrawnMinusbutton;
    }

    memset(&plus, 0, sizeof(TCheckTreeButton));
    memset(&minus, 0, sizeof(TCheckTreeButton));
}
//---------------------------------------------------------------------------
TPlusMinusCheckTreeElem::~TPlusMinusCheckTreeElem()
{
    if (--instances == 0)
    {
        delete drawnplus;
        delete drawnminus;
    }
}
//---------------------------------------------------------------------------
void TPlusMinusCheckTreeElem::Draw()
{
    inherited::Draw();

    if (Node->HasChildren)
    {
        drawbox(pmesPlus);
        drawbox(pmesMinus);
    }
}
//---------------------------------------------------------------------------
void TPlusMinusCheckTreeElem::drawbox(TPlusMinusExtensionSelection which)
{
    TRect r = buttonrect(which);
    switch (which)
    {
    case pmesPlus:
        drawnplus->draw(Tree->Canvas, r.Left, r.Top, !plus.hovered ? dcsNormal : !plus.down ? dcsHovered : dcsDown);
        break;
    case pmesMinus:
        drawnminus->draw(Tree->Canvas, r.Left, r.Top, !minus.hovered ? dcsNormal : !minus.down ? dcsHovered : dcsDown);
        break;
    }
}
//---------------------------------------------------------------------------
TRect TPlusMinusCheckTreeElem::buttonrect(TPlusMinusExtensionSelection which)
{

    if (which == pmesNone)
        return Rect(0, 0, 0, 0);

    TRect r = checkboxrect();
    TRect nr = Node->DisplayRect(true);
    if (!HasCheckbox)
        OffsetRect(r, -GetSystemMetrics(SM_CXMENUCHECK), 0);
    switch (which)
    {
    case pmesPlus:
        r = Rect(r.Right + 2, nr.Top, r.Right + 2 + drawnplus->width, nr.Top + drawnplus->height);
        break;
    case pmesMinus:
        r = Rect(r.Right + drawnplus->width + 2, nr.Top, r.Right + 2 + drawnplus->width + drawnminus->width, nr.Top + drawnplus->height);
        break;
    }
    return r;
}
//---------------------------------------------------------------------------
void TPlusMinusCheckTreeElem::MouseMove(TShiftState Shift, int X, int Y)
{
    TRect rp = buttonrect(pmesPlus);
    TRect rm = buttonrect(pmesMinus);

    if (Shift.Contains(ssLeft) && !plus.down && !minus.down || plus.hovered == PtInRect(rp, Point(X, Y)) && minus.hovered == PtInRect(rm, Point(X, Y)))
    {
        inherited::MouseMove(Shift, X, Y);
        return;
    }

    if (plus.hovered)
    {
        plus.hovered = false;
        InvalidateRect(Tree->Handle, &rp, false);
    }
    else if (minus.hovered)
    {
        minus.hovered = false;
        InvalidateRect(Tree->Handle, &rm, false);
    }

    if (PtInRect(rp, Point(X, Y)) && (!Shift.Contains(ssLeft) || plus.down))
    {
        plus.hovered = true;
        InvalidateRect(Tree->Handle, &rp, false);
    }
    else if (PtInRect(rm, Point(X, Y)) && (!Shift.Contains(ssLeft) || minus.down))
    {
        minus.hovered = true;
        InvalidateRect(Tree->Handle, &rm, false);
    }

    inherited::MouseMove(Shift, X, Y);
}
//---------------------------------------------------------------------------
void TPlusMinusCheckTreeElem::MouseDown(TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (!plus.hovered && !minus.hovered || Button != mbLeft)
    {
        inherited::MouseDown(Button, Shift, X, Y);
        return;
    }

    if (plus.hovered)
    {
        plus.down = true;
        InvalidateRect(Tree->Handle, &buttonrect(pmesPlus), false);
    }
    else
    {
        minus.down = true;
        InvalidateRect(Tree->Handle, &buttonrect(pmesMinus), false);
    }

    inherited::MouseDown(Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void TPlusMinusCheckTreeElem::MouseUp(TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (!plus.down && !minus.down || Button != mbLeft)
    {
        inherited::MouseUp(Button, Shift, X, Y);
        return;
    }

    if ((!plus.down || plus.hovered) && (!minus.down || minus.hovered))
    {
        CheckAll(plus.down);
        updateparents(plus.down);
        updatenodechilds();

        if (plus.down)
        {
            plus.down = false;
            InvalidateRect(Tree->Handle, &buttonrect(pmesPlus), false);
        }
        else
        {
            minus.down = false;
            InvalidateRect(Tree->Handle, &buttonrect(pmesMinus), false);
        }
    }
    else
    {
        plus.down = false;
        minus.down = false;
    }

    inherited::MouseUp(Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void TPlusMinusCheckTreeElem::ReleaseButton()
{
    if (plus.down)
    {
        plus.down = false;
        InvalidateRect(Tree->Handle, &buttonrect(pmesPlus), false);
    }
    else
    {
        minus.down = false;
        InvalidateRect(Tree->Handle, &buttonrect(pmesMinus), false);
    }
    inherited::ReleaseButton();
}
//---------------------------------------------------------------------------
void TPlusMinusCheckTreeElem::MouseLeave()
{
    if (plus.hovered)
    {
        plus.hovered = false;
        InvalidateRect(Tree->Handle, &buttonrect(pmesPlus), false);
    }
    else if (minus.hovered)
    {
        minus.hovered = false;
        InvalidateRect(Tree->Handle, &buttonrect(pmesMinus), false);
    }
    inherited::MouseLeave();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TWordFilterList *wordfilters = 0;
TWordFilterSelection *dictionaryfilters = 0;
TWordFilterSelection *kanjiwordfilters = 0;
TWordFilterSelection *wordgroupfilters = 0;

//---------------------------------------------------------------------------
void SetFilter()
{
    fFilter = new TfFilter(Application);
    int r = fFilter->ShowModal();
    delete fFilter;

    if (r)
    {
        if (!fDict->Visible)
            fDict->filterchanged = true;
        else
            fDict->btnFilter->Click();
        if (!fPopupDict->Visible)
            fPopupDict->filterchanged = true;
        else
            fPopupDict->btnFilter->Click();
    }
}
//---------------------------------------------------------------------------

// Word types
const wchar_t *TfFilter::treewordtypes[] = {
  L">Word types",
    L"Noun",
    L">Verb types",
      L"Godan verb",
      L"Ichidan verb",
      L"Transitive verb",
      L"Intransitive verb",
      L"Takes suru",
      L"-suru verb",
      L"-iku verb",
      L"-aru verb",
      L"-kuru verb",
      L"-Auxiliary verb",
    L"<",
    L">Adjective types",
      L"True adjective (-i)",
      L"Adjectival noun (-na)",
      L"Taru adjective (-to)",
      L"Pre-noun adjective",
      L"Auxiliary adjective",
    L"<",
    L"May take -no",
    L"Adverb",
    L"Expression",
    L"Interjection",
    L"Conjunction",
    L"Prefix",
    L"Suffix",
  L"<"
};

const bool TfFilter::treewordtypesboxes[] = {
  false,
    true,
    false,
      true,
      true,
      true,
      true,
      true,
      true,
      true,
      true,
      true,
      true,
    false,
    false,
      true,
      true,
      true,
      true,
      true,
    false,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
  false
};

const int TfFilter::treewordtypesorder[] = {
  0,
    1,
    0,
      3,
      7,
      6,
      8,
      2,
      15,
      19,
      23,
      21,
      20,
    0,
    0,
      10,
      5,
      13,
      16,
      22,
    0,
    11,
    9,
    4,
    18,
    17,
    14,
    12,
  0
};

// Word usages
const wchar_t *TfFilter::treewordusages[] = {
  L">Word usages",
    L"Abbreviation",
    L">Written form",
      L"Kana used (usually not written with kanji)",
      L"Only kanji usage",
    L"<",
    L">Relation of speaker / listener",
      L"Polite language",
      L"Humble language",
      L"Colloquialism",
      L"Honorifix",
      L"Familiar language",
    L"<",
    L"Idiomatic expression",
    L"Sensitivity",
    L">Style of speech",
      L"Male language",
      L"Female language",
      L"Slang",
      L"Vulgar",
      L"Derogatory",
      L"Manga slang",
      L"X rated",
      L"Archaic language",
    L"<",
    L"Obscure usage",
    L"Rare word or phrase",
    L"Obsolete term",
  L"<"
};

const bool TfFilter::treewordusagesboxes[] = {
  false,
    true,
    false,
      true,
      true,
    false,
    false,
      true,
      true,
      true,
      true,
      true,
    false,
    true,
    true,
    false,
      true,
      true,
      true,
      true,
      true,
      true,
      true,
      true,
    false,
    true,
    true,
    true,
  false
};

const int TfFilter::treewordusagesorder[] = {
  0,
    25,
    0,
      34,
      35,
    0,
    0,
      29,
      27,
      28,
      26,
      31,
    0,
    43,
    24,
    0,
      32,
      33,
      30,
      39,
      41,
      44,
      40,
      36,
    0,
    42,
    37,
    38,
  0
};

// Fields of use
const wchar_t *TfFilter::treewordfields[] = {
  L">Fields of use",
    L"Math",
    L"Linguistics",
    L"Computers",
    L"Food",
    L"Martial arts",
    L"Geometry",
    L"Physics",
    L"Buddhist term",
  L"<"
};

const bool TfFilter::treewordfieldsboxes[] = {
  false,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
  false
};

const int TfFilter::treewordfieldsorder[] = {
  0,
    45,
    46,
    47,
    48,
    49,
    50,
    51,
    52,
  0
};

// JLPT level
const wchar_t *TfFilter::treewordJLPT[] = {
  L">JLPT level",
    L"N5",
    L"N4",
    L"N3",
    L"N2",
    L"N1",
    L"Non-JLPT",
  L"<"
};

const bool TfFilter::treewordJLPTboxes[] = {
  false,
    true,
    true,
    true,
    true,
    true,
    true,
  false
};

const int TfFilter::treewordJLPTorder[] = {
  0,
    -1,
    -2,
    -3,
    -4,
    -5,
    -6,
  0
};

//---------------------------------------------------------------------------
__fastcall TfFilter::TfFilter(TComponent* Owner) : TBaseForm(Owner)
{
    Constrain(Width, Height);

    if (settings.filterwindowwidth >= MinWidth && settings.filterwindowheight >= MinHeight)
    {
        Width = settings.filterwindowwidth;
        Height = settings.filterwindowheight;
    }

    bobj = new TCheckboxTreeView(tvBoxes);
    bobj->OnCheck = &ccallback;

    AddItems(sizeof(treewordtypes) / sizeof(wchar_t*), treewordtypes, treewordtypesboxes, (const void**)treewordtypesorder);
    AddItems(sizeof(treewordusages) / sizeof(wchar_t*), treewordusages, treewordusagesboxes, (const void**)treewordusagesorder);
    AddItems(sizeof(treewordfields) / sizeof(wchar_t*), treewordfields, treewordfieldsboxes, (const void**)treewordfieldsorder);
    AddItems(sizeof(treewordJLPT) / sizeof(wchar_t*), treewordJLPT, treewordJLPTboxes, (const void**)treewordJLPTorder);

    lbFilters->Items->BeginUpdate();
    cbFilters->Items->BeginUpdate();
    try
    {
        while (lbFilters->Items->Count < wordfilters->Count)
        {
            cbFilters->Items->Add(wordfilters->Names[lbFilters->Items->Count]);
            int fix = lbFilters->Items->AddObject(wordfilters->Names[lbFilters->Items->Count], (TObject*)(new TWordFilterItem));
            TWordFilterItem *f = (TWordFilterItem*)lbFilters->Items->Objects[fix];
            f->id = wordfilters->Id[fix];
            f->filter = wordfilters->Filters[fix];
        }
    }
    catch (...)
    {
        ;
    }
    lbFilters->Items->EndUpdate();
    cbFilters->Items->EndUpdate();

    if (lbFilters->Count)
    {
        lbFilters->ItemIndex = 0;
        ApplyFilter(((TWordFilterItem*)lbFilters->Items->Objects[0])->filter);
    }

    UpdateButtons();
}
//---------------------------------------------------------------------------
__fastcall TfFilter::~TfFilter()
{
    delete bobj;
}
//---------------------------------------------------------------------------
void TfFilter::AddItems(unsigned int itemcount, const wchar_t *lines[], const bool useboxes[], const void *customdata[])
{
    TCheckTreeElem *parent = NULL;
    bobj->BeginUpdate();
    try
    {
        for (unsigned int ix = 0; ix < itemcount; ++ix)
        {
            if (lines[ix][0] == '<')
            {
                if (!parent)
                    continue;
                parent = parent->Parent;
            }
            else
            {
                TCheckTreeElem *elem;

                if (lines[ix][0] == '>')
                {
                    /*if (parent)
                     parent = tree->Items->AddChild(parent,L"");
                     else
                     parent = tree->Items->Add(NULL,L"");
                     */
                    elem = new TPlusMinusCheckTreeElem(lines[ix] + 1, useboxes[ix], customdata[ix]);
                    bobj->AddChild(parent, elem);
                    parent = elem;
                }
                else
                {
                    /*TTreeNode *item;
                     if (parent)
                     item = tree->Items->AddChild(parent,L"");
                     else
                     item = tree->Items->Add(NULL,L"");
                     */
                    elem = new TSimpleCheckTreeElem(lines[ix], useboxes[ix], customdata[ix]);
                    bobj->AddChild(parent, elem);
                }
                //listitems->Add(elem);
            }
        }
    }
    catch (...)
    {
        ;
    }
    bobj->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TfFilter::FormClose(TObject *Sender, TCloseAction &Action)
{
    SaveFormSize();
    settings.filterwindowwidth = Width;
    settings.filterwindowheight = Height;

    if (ModalResult == mrOk)
    {
        int dictfiltermatch = 0;
        int kanjiwfiltermatch = 0;
        int wgroupfiltermatch = 0;
        int tmp;

        wordfilters->Clear();

        for (int ix = 0; ix < lbFilters->Items->Count; ++ix)
        {
            TWordFilterItem *item = (TWordFilterItem*)lbFilters->Items->Objects[ix];

            tmp = dictionaryfilters->IndexOf(item->id);
            if (tmp >= 0)
            {
                dictionaryfilters->Exchange(tmp, dictfiltermatch);
                dictfiltermatch++;
            }

            tmp = kanjiwordfilters->IndexOf(item->id);
            if (tmp >= 0)
            {
                kanjiwordfilters->Exchange(tmp, kanjiwfiltermatch);
                kanjiwfiltermatch++;
            }

            tmp = wordgroupfilters->IndexOf(item->id);
            if (tmp >= 0)
            {
                wordgroupfilters->Exchange(tmp, wgroupfiltermatch);
                wgroupfiltermatch++;
            }

            wordfilters->Items[wordfilters->Add(lbFilters->Items->Strings[ix], item->filter)]->id = item->id;
        }

        for (int ix = dictionaryfilters->Count - 1; ix >= dictfiltermatch; --ix)
            dictionaryfilters->Delete(ix);
        for (int ix = kanjiwordfilters->Count - 1; ix >= kanjiwfiltermatch; --ix)
            kanjiwordfilters->Delete(ix);
        for (int ix = wordgroupfilters->Count - 1; ix >= wgroupfiltermatch; --ix)
            wordgroupfilters->Delete(ix);

        //WordFiltersChanged();
    }

    for (int ix = 0; ix < lbFilters->Items->Count; ++ix)
        delete(TWordFilterItem*)lbFilters->Items->Objects[ix];
}
//---------------------------------------------------------------------------
void __fastcall TfFilter::cbFiltersChange(TObject *Sender)
{
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfFilter::cbN5Click(TObject *Sender)
{
    FilterChanged();
}
//---------------------------------------------------------------------------
void TfFilter::CollectFilter(TWordFilter &filter)
{
    memset(&filter, 0, sizeof(TWordFilter));
    filter.typeinclusion = rbAnyType->Checked ? wtiAny : wtiAll;
    filter.allusages = rbAllUsages->Checked;
    for (int ix = 0; ix < bobj->Count; ++ix)
    {
        int n = (int)bobj->Data[ix];
        if (!n || !bobj->Checked[ix])
            continue;
        if (n < 0)
            filter.JLPT[n + 6] = true;
        else if (n <= WT_COUNT)
            filter.types |= (1 << (n - 1));
        else if (n - WT_COUNT <= WN_COUNT)
            filter.notes |= (1 << (n - WT_COUNT - 1));
        else if (n - WT_COUNT - WN_COUNT <= WF_COUNT)
            filter.fields |= (1 << (n - WT_COUNT - WN_COUNT - 1));
    }
}
//---------------------------------------------------------------------------
void TfFilter::ApplyFilter(const TWordFilter &filter)
{
    switch (filter.typeinclusion)
    {
    case wtiAny:
        rbAnyType->Checked = true;
        break;
    case wtiAll:
    default:
        rbAllTypes->Checked = true;
    }
    if (filter.allusages)
        rbAllUsages->Checked = true;
    else
        rbAnyUsage->Checked = true;

    bobj->BeginUpdate();
    for (int ix = 0; ix < bobj->Count; ++ix)
    {
        int n = (int)bobj->Data[ix];
        if (!n)
            continue;
        if (n < 0)
            bobj->Checked[ix] = filter.JLPT[n + 6];
        else if (n <= WT_COUNT)
            bobj->Checked[ix] = (filter.types & (1 << (n - 1)));
        else if (n - WT_COUNT <= WN_COUNT)
            bobj->Checked[ix] = (filter.notes & (1 << (n - WT_COUNT - 1)));
        else if (n - WT_COUNT - WN_COUNT <= WF_COUNT)
            bobj->Checked[ix] = (filter.fields & (1 << (n - WT_COUNT - WN_COUNT - 1)));
    }

    bobj->OpenChecked();
    bobj->EndUpdate();
    FilterChanged();
}
//---------------------------------------------------------------------------
void TfFilter::FilterChanged()
{
    if (!lbFilters->Items->Count)
        return;

    TWordFilter filter;
    CollectFilter(filter);

    for (int ix = 0; ix < lbFilters->Items->Count; ++ix)
    {
        if (filter == ((TWordFilterItem*)lbFilters->Items->Objects[ix])->filter)
        {
            lbFilters->ItemIndex = ix;
            UpdateButtons();
            return;
        }
    }

    lbFilters->ItemIndex = -1;
    UpdateButtons();
}
//---------------------------------------------------------------------------
void TfFilter::ccallback(bool checked, const void *userdata)
{
    FilterChanged();
}
//---------------------------------------------------------------------------
int TfFilter::UniqueId()
{
    int id = generate_unique_filter_id();
    for (int ix = 0; ix < lbFilters->Items->Count; ++ix)
    {
        if (id == ((TWordFilterItem*)lbFilters->Items->Objects[ix])->id)
        {
            id = generate_unique_filter_id();
            ix = -1;
        }
    }
    return id;
}
//---------------------------------------------------------------------------
void __fastcall TfFilter::btnAddFilterClick(TObject *Sender)
{
    if (!btnAddFilter->Enabled)
        return;

    int ix = RealIndex(cbFilters);

    if (ix >= 0)
    { //update
        TWordFilterItem *f = (TWordFilterItem*)lbFilters->Items->Objects[ix];
        CollectFilter(f->filter);
        lbFilters->ItemIndex = ix;
    }
    else
    { //add
        TWordFilterItem *f = new TWordFilterItem;
        f->id = UniqueId();
        CollectFilter(f->filter);
        lbFilters->ItemIndex = lbFilters->Items->AddObject(cbFilters->Text.Trim(), (TObject*)f);
        cbFilters->Items->Add(cbFilters->Text.Trim());
    }

    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfFilter::lbFiltersClick(TObject *Sender)
{
    if (lbFilters->ItemIndex < 0)
        return;
    ApplyFilter(((TWordFilterItem*)lbFilters->Items->Objects[lbFilters->ItemIndex])->filter);
}
//---------------------------------------------------------------------------
void __fastcall TfFilter::sbDeleteClick(TObject *Sender)
{
    int ix = lbFilters->ItemIndex;
    if (ix < 0)
        return;
    cbFilters->Items->Delete(ix);
    delete(TWordFilterItem*)lbFilters->Items->Objects[ix];
    lbFilters->Items->Delete(ix);

    UpdateButtons();
}
//---------------------------------------------------------------------------
void TfFilter::UpdateButtons()
{
    int ix = RealIndex(cbFilters);
    btnAddFilter->Caption = ix >= 0 && lbFilters->ItemIndex < 0 ? L"Update" : L"Add";
    btnAddFilter->Enabled = cbFilters->Text.Trim().Length() && lbFilters->ItemIndex < 0;
    sbUp->Enabled = lbFilters->ItemIndex > 0;
    sbDown->Enabled = lbFilters->ItemIndex >= 0 && lbFilters->ItemIndex < lbFilters->Items->Count - 1;
    sbRename->Enabled = lbFilters->ItemIndex >= 0;
    sbDelete->Enabled = lbFilters->ItemIndex >= 0;
}
//---------------------------------------------------------------------------
void __fastcall TfFilter::sbUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    int lix = lbFilters->ItemIndex;
    if (Button != mbLeft || lix < 0)
        return;

    MouseRepeat(sbUpMouseDown, Sender, Button, Shift, X, Y);

    int dir = ((TSpeedButton*)Sender)->Tag;
    int cix = RealIndex(cbFilters);

    cbFilters->Items->Exchange(lix, lix + dir);
    lbFilters->Items->Exchange(lix, lix + dir);

    if (cix >= 0)
        cbFilters->ItemIndex = cix < min(lix, lix + dir) || cix > max(lix, lix + dir) ? cix : cix == lix + dir ? lix : lix + dir;
    lbFilters->ItemIndex = lix < min(lix, lix + dir) || lix > max(lix, lix + dir) ? lix : lix == lix + dir ? lix : lix + dir;
    UpdateButtons();
}
//---------------------------------------------------------------------------
void __fastcall TfFilter::sbRenameClick(TObject *Sender)
{
    int lix = lbFilters->ItemIndex;
    int cix = RealIndex(cbFilters);

    UnicodeString s = lbFilters->Items->Strings[lix];
    if (QueryBox(L"Enter a new name for this filter", L"Rename filter", s) && s.Length())
    {
        for (int ix = 0; ix < lbFilters->Items->Count; ++ix)
        {
            if (lbFilters->Items->Strings[ix].LowerCase() == s.LowerCase())
            {
                if (ix != lix)
                    MsgBox(L"Filter with the same name already exists.", L"Error", MB_OK);
                else
                {
                    lbFilters->Items->Strings[ix] = s;
                    lbFilters->ItemIndex = lix;
                    cbFilters->Items->Strings[ix] = s;
                    cbFilters->ItemIndex = cix;
                }
                return;
            }
        }

        cbFilters->Items->Strings[lix] = s;
        if (cix == lix)
            cbFilters->ItemIndex = cix;

        lbFilters->Items->Strings[lix] = s;
        lbFilters->ItemIndex = lix;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfFilter::FormPaint(TObject *Sender)
{
    DrawCloseBar(this, bvB, NULL);
    TRect r;

    r = Rect(0, 0, ClientWidth, ChildRect(bv2).Top);
    DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, r, gdVertical);
    r = Rect(0, ChildRect(bv4).Top, ClientWidth, ChildRect(bvB).Top);
    DrawGrad(Canvas, clBtnFace, COLORDEC(clBtnFace, 0x101010), r, gdVertical);

    DrawPillowPanel(this, bv2, bv3, true, true);
    DrawPillowPanel(this, bv3, bv4, true);
}
//---------------------------------------------------------------------------
void __fastcall TfFilter::FormResize(TObject *Sender)
{
    TRect r = Rect(0, ChildRect(bv4).Top + bv4->Height, ClientWidth, ClientHeight);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
}
//---------------------------------------------------------------------------
void __fastcall TfFilter::cbFiltersKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == VK_RETURN && btnAddFilter->Enabled)
    {
        Key = 0;
        btnAddFilter->Click();
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int generate_unique_filter_id()
{
    int r;
    do
    {
        r = random(MaxInt - 1) + 1;
        if (wordfilters->IndexOf(r) >= 0)
            continue;
        if (dictionaryfilters->IndexOf(r) >= 0 || dictionaryfilters->IndexOf(-r) >= 0)
            continue;
        if (kanjiwordfilters->IndexOf(r) >= 0 || kanjiwordfilters->IndexOf(-r) >= 0)
            continue;
        if (wordgroupfilters->IndexOf(r) >= 0 || wordgroupfilters->IndexOf(-r) >= 0)
            continue;
    }
    while (false);
    return r;
}
//---------------------------------------------------------------------------
bool operator == (const TWordFilter &a, const TWordFilter &b)
{
    return !memcmp(&a, &b, sizeof(TWordFilter));
}
//---------------------------------------------------------------------------
bool operator != (const TWordFilter &a, const TWordFilter &b)
{
    return !(a == b);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordFilterList::TWordFilterList() : inherited()
{

}
//---------------------------------------------------------------------------
TWordFilterList::~TWordFilterList()
{
    Clear();
}
//---------------------------------------------------------------------------
void TWordFilterList::normalize(TWordFilter &f)
{
    int val = 0;
    byte nval = 0;
    for (int i = 0; i < WT_COUNT; i++)
    {
        val *= 2;
        val++;
    }
    f.types &= val;
    val = 0;
    for (int i = 0; i < WN_COUNT; i++)
    {
        val *= 2;
        val++;
    }
    f.notes &= val;
    for (int i = 0; i < WF_COUNT; i++)
    {
        nval *= 2;
        nval++;
    }
    f.fields &= nval;
}
//---------------------------------------------------------------------------
int TWordFilterList::Add(UnicodeString name, const TWordFilter &filter)
{
    TWordFilterItem *item = new TWordFilterItem;
    item->name = name;
    item->id = generate_unique_filter_id();
    memcpy(&item->filter, &filter, sizeof(TWordFilter));

    normalize(item->filter);
    return Add(item);
}
//---------------------------------------------------------------------------
int TWordFilterList::IndexOf(UnicodeString name)
{
    for (int ix = 0; ix < Count; ++ix)
        if (Items[ix]->name.LowerCase() == name.LowerCase())
            return ix;
    return -1;
}
//---------------------------------------------------------------------------
int TWordFilterList::IndexOf(int id)
{
    for (int ix = 0; ix < Count; ++ix)
        if (Items[ix]->id == id)
            return ix;
    return -1;
}
//---------------------------------------------------------------------------
int TWordFilterList::get_id(int ix)
{
    return Items[ix]->id;
}
//---------------------------------------------------------------------------
UnicodeString TWordFilterList::get_name(int ix)
{
    return Items[ix]->name;
}
//---------------------------------------------------------------------------
const TWordFilter& TWordFilterList::get_filter(int ix)
{
    return Items[ix]->filter;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool usejlptfilter(const TWordFilter &f)
{
    int used = 0;
    for (int ix = 0; ix < 6; ++ix)
        if (f.JLPT[ix])
            used++;

    return used != 6 && used != 0;
}
//---------------------------------------------------------------------------
bool __fastcall match_filter(TWord *w, const TWordFilter &f, char meaning)
{
    bool usejlpt = usejlptfilter(f);

    if (!f.types && !f.notes && !f.fields && !usejlpt)
        return true;

    if (w->meaningcnt <= meaning)
        return false;

    bool result = false;

    for (int i = max(meaning, 0); i < (meaning < 0 ? w->meaningcnt : meaning + 1) && !result; i++)
    {
        if (f.types)
            result = f.typeinclusion == wtiAll ? ((w->data[i].types & f.types) == f.types) : (w->data[i].types & f.types);
        if ((!f.types || result) && f.notes)
            result = f.allusages ? ((w->data[i].notes & f.notes) == f.notes) : (w->data[i].notes & f.notes);
        if ((!f.types && !f.notes || result) && f.fields)
            result = (w->data[i].fields & f.fields);
    }

    if ((!f.types && !f.notes && !f.fields || result) && usejlpt)
    {
        //TWordJLPTNData *dat = JLPT->FindData(w);
        TStoredWord *sw = store->FindWord(w->kanji, w->kana);
        if (!sw || !sw->jlptnlevel/*!dat*/)
            result = f.JLPT[0];
        else
            result = f.JLPT[sw->jlptnlevel/*dat->Nlevel*/];
    }

    return (result || !f.types && !f.notes && !f.fields && !usejlpt);
}
//---------------------------------------------------------------------------
UnicodeString TWordFilterSelection::get_name(int ix)
{
    ix = wordfilters->IndexOf(abs(Items[ix]));
    if (ix >= 0)
        return wordfilters->Names[ix];
    THROW(L"Filter name does not exist in wordfilters!");
}
//---------------------------------------------------------------------------
const TWordFilter& TWordFilterSelection::get_filter(int ix)
{
    ix = wordfilters->IndexOf(abs(Items[ix]));
    if (ix >= 0)
        return wordfilters->Filters[ix];
    THROW(L"Filter does not exist in wordfilters!");
}
//---------------------------------------------------------------------------
bool TWordFilterSelection::get_exclude(int ix)
{
    return Items[ix] < 0;
}
//---------------------------------------------------------------------------
bool TWordFilterSelection::filters_match(TWordFilterCopyList *copylist)
{
    bool match = copylist && copylist->Count == Count || !copylist && Count == 0;
    for (int ix = 0; ix < Count && match; ++ix)
    {
        bool found = false;
        for (int iy = 0; iy < copylist->Count && !found; ++iy)
            found = (Filters[ix] == copylist->Items[iy]->filter && Exclude[ix] == copylist->Items[iy]->exclude);
        match = found;
    }
    return match;
}
//---------------------------------------------------------------------------
void TWordFilterSelection::create_copy(TWordFilterCopyList *copylist)
{
    copylist->Clear();
    for (int ix = 0; ix < Count; ++ix)
    {
        TWordFilterCopy *copy = new TWordFilterCopy;
        copy->filter = Filters[ix];
        copy->exclude = Exclude[ix];
        copylist->Add(copy);
    }
}
//---------------------------------------------------------------------------
bool TWordFilterSelection::match(TWord *w, char meaning)
{
    if (!Count)
        return true;
    for (int ix = 0; ix < Count; ++ix)
        if (match_filter(w, Filters[ix], meaning) == Exclude[ix])
            return false;
    return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool TWordFilterCopyList::match(TWord *w, char meaning)
{
    if (!Count)
        return true;
    for (int ix = 0; ix < Count; ++ix)
        if (match_filter(w, Items[ix]->filter, meaning) == Items[ix]->exclude)
            return false;
    return true;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(177);
#endif

