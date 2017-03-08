//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Themes.hpp>
#include <vsstyle.h>
#include "ZPaintBox.h"

#include "checktreeview.h"
#include "baseform.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(12);
#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TCheckTreeElem::TCheckTreeElem(/*TCheckElemExtensionType atype,*/ const wchar_t *atitle, const bool ausebox, const void *adata)
{
    fowner = NULL;
    fnode = NULL;

    ftitle = atitle;
//ftype = atype;
    fusebox = ausebox;
    fbox.checked = false;
    fbox.hovered = false;
    fbox.down = false;
    fdata = adata;
}
//---------------------------------------------------------------------------
TCustomTreeView* TCheckTreeElem::get_tree()
{
    if (!fowner)
        THROW(L"The node is not added to a tree yet.");

    return fowner->tree;
}
//---------------------------------------------------------------------------
TCheckTreeElem* TCheckTreeElem::get_parent()
{
    if (!fowner)
        THROW(L"The node is not added to a tree yet.");
    return fnode->Parent == NULL ? NULL : (TCheckTreeElem*)fnode->Parent->Data;
}
//---------------------------------------------------------------------------
bool TCheckTreeElem::get_checked()
{
    if (!fusebox)
        return false;
    return fbox.checked;
}
//---------------------------------------------------------------------------
void TCheckTreeElem::set_checked(bool val)
{
    SetChecked(val, true);
}
//---------------------------------------------------------------------------
bool TCheckTreeElem::has_checked()
{
    if (!fowner)
        THROW(L"The node is not added to a tree yet.");

    if (HasCheckbox && Checked)
        return true;
    TTreeNode *n = fnode->getFirstChild();
    while (n)
    {
        if (((TCheckTreeElem*)n->Data)->HasChecked)
            return true;
        n = n->getNextSibling();
    }
    return false;
}
//---------------------------------------------------------------------------
void TCheckTreeElem::SetChecked(bool val, bool notify)
{
    if (fbox.checked == val)
        return;
    fbox.checked = val;

    if (!fowner)
        return;

    TRect r = checkboxrect();

    InvalidateRect(Tree->Handle, &r, true);
    updateparents(val);

    if (notify && fowner->foncheck)
        fowner->foncheck(val, Data);
}
//---------------------------------------------------------------------------
void TCheckTreeElem::CheckAll(bool check)
{
    if (!fowner)
        THROW(L"The node is not added to a tree yet.");

    if (fusebox)
        Checked = check;

    for (int ix = 0; ix < fnode->Count; ++ix)
    {
        TCheckTreeElem *elem = (TCheckTreeElem*)fnode->Item[ix]->Data;
        elem->CheckAll(check);
    }
    if (fnode->HasChildren && check)
        fnode->Expand(false);
}
//---------------------------------------------------------------------------
void TCheckTreeElem::updateparents(bool check)
{
    if (!fowner)
        THROW(L"The node is not added to a tree yet.");

    if (!fnode->Parent)
        return;
    TCheckTreeElem *elem = (TCheckTreeElem*)fnode->Parent->Data;
    if (check && elem->fusebox && !elem->Checked)
        elem->Checked = true;
    InvalidateRect(Tree->Handle, &elem->fnode->DisplayRect(false), true);
    elem->updateparents(check);
}
//---------------------------------------------------------------------------
void TCheckTreeElem::updatenodechilds()
{
    if (!fowner)
        THROW(L"The node is not added to a tree yet.");

    TTreeNode *node = fnode;
    int ix = 0;
    InvalidateRect(Tree->Handle, &fnode->DisplayRect(false), true);
    while (ix < node->Count)
    {
        TCheckTreeElem *elem = (TCheckTreeElem*)node->Item[ix]->Data;
        if (elem->fnode->HasChildren)
        {
            node = elem->fnode;
            InvalidateRect(Tree->Handle, &node->DisplayRect(false), true);
            ix = 0;
            continue;
        }
        ix++;
        if (ix == node->Count)
        {
            if (!node->Parent || node == fnode)
                break;
            ix = node->AbsoluteIndex - node->Parent->AbsoluteIndex;
            node = node->Parent;
        }
    }
}
//---------------------------------------------------------------------------
void TCheckTreeElem::Draw()
{
    if (!fowner)
        THROW(L"The node is not added to a tree yet.");

    TCanvas *c = Tree->Canvas;
    TRect r = fnode->DisplayRect(false);
    TRect textr = fnode->DisplayRect(true);
    c->Brush->Style = bsClear;
    TSize s;

    if (fnode->HasChildren)
    {
        c->Font->Style = TFontStyles() << fsBold;
        if (HasChecked)
            c->Font->Color = coloravg(c->Font->Color, (TColor)0x000000ff, 0.15);
    }
    GetTextExtentPoint32W(c->Handle, ftitle.c_str(), ftitle.Length(), &s);
    textr.Right = textr.Left + s.cx;
    ExtTextOutW(c->Handle, textr.Left, textr.Top + (textr.Height() - s.cy) / 2, ETO_CLIPPED, &textr, ftitle.c_str(), ftitle.Length(), NULL);
    if (fusebox)
        drawcheckbox();
}
//---------------------------------------------------------------------------
void TCheckTreeElem::drawcheckbox()
{
    if (!fowner)
        THROW(L"The node is not added to a tree yet.");

    if (!fusebox)
        return;

    TRect r = checkboxrect();
    TThemeServices *t = ThemeServices();
    if (t && t->ThemesAvailable && t->ThemesEnabled)
    {
        TThemedElementDetails ted;
        ted.Element = teButton;
        ted.Part = BP_CHECKBOX;

        if (!fbox.hovered)
            ted.State = fbox.checked ? CBS_CHECKEDNORMAL : CBS_UNCHECKEDNORMAL;
        else if (fbox.down)
            ted.State = fbox.checked ? CBS_CHECKEDPRESSED : CBS_UNCHECKEDPRESSED;
        else if (fbox.hovered)
            ted.State = fbox.checked ? CBS_CHECKEDHOT : CBS_UNCHECKEDHOT;
        t->DrawElement(Tree->Canvas->Handle, ted, r);
        return;
    }
    DrawFrameControl(Tree->Canvas->Handle, &r, DFC_BUTTON, DFCS_BUTTONCHECK | (fbox.checked ? DFCS_CHECKED : 0) | (fbox.down && fbox.hovered ? DFCS_PUSHED : 0) | (fbox.hovered ? DFCS_HOT : 0));
}
//---------------------------------------------------------------------------
TRect TCheckTreeElem::checkboxrect()
{
    if (!fowner)
        THROW(L"The node is not added to a tree yet.");

    int boxw = 13 * scaleres;
    int boxh = 13 * scaleres;

    TRect textr = fnode->DisplayRect(true);
    if (fnode->HasChildren)
    {
        TFontStyles saved = Tree->Canvas->Font->Style;
        Tree->Canvas->Font->Style = TFontStyles() << fsBold;
        TSize s;
        GetTextExtentPoint32W(Tree->Canvas->Handle, ftitle.c_str(), ftitle.Length(), &s);
        Tree->Canvas->Font->Style = saved;
        textr.Right = textr.Left + s.cx;

        int l = textr.Right + 2;
        l += boxw / 2 - (l % (boxw / 2));

        return Rect(l, textr.Top + (textr.Height() - boxh) / 2, l + boxw, textr.Top + (textr.Height() + boxh) / 2);
    }
    return Rect(textr.Left - boxw - 4*scaleres, textr.Top + (textr.Height() - boxh) / 2, textr.Left - 4*scaleres, textr.Top + (textr.Height() + boxh) / 2);

}
//---------------------------------------------------------------------------
void TCheckTreeElem::MouseMove(TShiftState Shift, int X, int Y)
{
    TRect r = checkboxrect();
    bool inbox = PtInRect(r, Point(X, Y));
    if (Shift.Contains(ssLeft) && !fbox.down || fbox.hovered == inbox)
        return;

    fbox.hovered = inbox;
    InvalidateRect(Tree->Handle, &r, false);
}
//---------------------------------------------------------------------------
void TCheckTreeElem::MouseDown(TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (!fbox.hovered || Button != mbLeft)
        return;
    fbox.down = true;
    InvalidateRect(Tree->Handle, &checkboxrect(), false);
}
//---------------------------------------------------------------------------
void TCheckTreeElem::MouseUp(TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (!fbox.hovered || Button != mbLeft)
    {
        if (Button == mbLeft)
            fbox.down = false;
        return;
    }

    if (fbox.down)
    {
        fbox.down = false;
        if (Tree->SelectionCount > 1)
        {
            bool val = !Checked;
            fowner->BeginUpdate();
            for (unsigned int ix = 0; ix < Tree->SelectionCount; ++ix)
            {
                TCheckTreeElem *elem = (TCheckTreeElem*)Tree->Selections[ix]->Data;
                elem->Checked = val;
            }
            fowner->EndUpdate();
        }
        else
            Checked = !Checked;
    }
    else
        InvalidateRect(Tree->Handle, &checkboxrect(), false);
}
//---------------------------------------------------------------------------
void TCheckTreeElem::ReleaseButton()
{
    if (fbox.down)
    {
        fbox.down = false;
        if (fbox.hovered)
            InvalidateRect(Tree->Handle, &checkboxrect(), false);
    }
}
//---------------------------------------------------------------------------
void TCheckTreeElem::MouseLeave()
{
    if (fbox.hovered)
    {
        fbox.hovered = false;
        InvalidateRect(Tree->Handle, &checkboxrect(), false);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TSimpleCheckTreeElem::TSimpleCheckTreeElem(const wchar_t *atitle, const bool ausebox, const void *adata) : inherited(/*etSimple,*/atitle, ausebox, adata)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TCheckboxTreeView::TCheckboxTreeView(TTreeView *tv) :
        lastelem(NULL), fmousedown(false), foncheck(NULL), update_counter(0), searchstr(L""), tree(tv)
{
    tree->ReadOnly = true;
    tree->RowSelect = true;
    tree->ShowLines = false;

    listitems = new TTreeElemList;

    onAdvancedCustomDrawItem = tree->OnAdvancedCustomDrawItem;
    onMouseMove = tree->OnMouseMove;
    onMouseDown = tree->OnMouseDown;
    onMouseUp = tree->OnMouseUp;
    onMouseLeave = tree->OnMouseLeave;
    onKeyPress = tree->OnKeyPress;
    onKeyDown = tree->OnKeyDown;
    onExit = tree->OnExit;

    tree->OnAdvancedCustomDrawItem = AdvancedCustomDrawItem;
    tree->OnMouseMove = MouseMove;
    tree->OnMouseDown = MouseDown;
    tree->OnMouseUp = MouseUp;
    tree->OnMouseLeave = MouseLeave;
    tree->OnKeyPress = KeyPress;
    tree->OnKeyDown = KeyDown;
    tree->OnExit = Exit;

    AppActivateEvent(this, &Deactivate, false, true);
    TBaseForm *owner = (TBaseForm*)tree->Owner;
    owner->AddEvent(tree, &Deactivate, eltDeactivate);
}
//---------------------------------------------------------------------------
TCheckboxTreeView::~TCheckboxTreeView()
{
    TBaseForm *owner = (TBaseForm*)tree->Owner;
    owner->RemoveEvent(tree, &Deactivate, eltDeactivate);
    AppActivateEvent(this, &Deactivate, false, false);

    tree->OnExit = onExit;
    tree->OnKeyDown = onKeyDown;
    tree->OnKeyPress = onKeyPress;
    tree->OnMouseLeave = onMouseLeave;
    tree->OnMouseUp = onMouseUp;
    tree->OnMouseDown = onMouseDown;
    tree->OnMouseMove = onMouseMove;
    tree->OnAdvancedCustomDrawItem = onAdvancedCustomDrawItem;
    lastelem = NULL;
    delete listitems;
}
//---------------------------------------------------------------------------
void TCheckboxTreeView::AddChild(TCheckTreeElem *parent, TCheckTreeElem *child)
{
    child->fowner = this;
    listitems->Add(child);

    TTreeNode *item;

    if (parent)
        item = tree->Items->AddChild(parent->Node, L"");
    else
        item = tree->Items->Add(NULL, L"");
    child->fnode = item;
    child->fnode->Data = child;
}
//---------------------------------------------------------------------------
void __fastcall TCheckboxTreeView::AdvancedCustomDrawItem(TCustomTreeView *Sender, TTreeNode *Node, TCustomDrawState State, TCustomDrawStage Stage, bool &PaintImages, bool &DefaultDraw)
{
    if (Node->Data == NULL)
        return;

    if (Stage == cdPostPaint)
    {
        TCheckTreeElem *elem = (TCheckTreeElem*)Node->Data;
        elem->Draw();
    }
}
//---------------------------------------------------------------------------
void __fastcall TCheckboxTreeView::MouseLeave(TObject *Sender)
{
    if (lastelem != NULL)
        lastelem->MouseLeave();
    lastelem = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TCheckboxTreeView::MouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    TCheckTreeElem *elem = NULL;
    TTreeNode *node = !Shift.Contains(ssLeft) ? tree->GetNodeAt(X, Y) : NULL;
    if (node || Shift.Contains(ssLeft))
    {
        elem = Shift.Contains(ssLeft) ? lastelem : (TCheckTreeElem*)node->Data;
        elem->MouseMove(Shift, X, Y);
    }
    if (lastelem && lastelem != elem)
        lastelem->MouseLeave();

    lastelem = elem;

    if (onMouseMove)
        onMouseMove(Sender, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TCheckboxTreeView::MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    searchstr = "";
    if (lastelem && Button == mbLeft)
    {
        fmousedown = true;
        lastelem->MouseDown(Button, Shift, X, Y);
    }

    if (onMouseDown)
        onMouseDown(Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TCheckboxTreeView::MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button == mbLeft)
    {
        if (lastelem)
            lastelem->MouseUp(Button, Shift, X, Y);
        fmousedown = false;
    }

    if (onMouseUp)
        onMouseUp(Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
bool TCheckboxTreeView::findnext(TCheckTreeElem *elem, wchar_t Key)
{
    int index = !elem ? 0 : listitems->IndexOf(elem);
    UnicodeString s = elem ? elem->Title.LowerCase() : UnicodeString(L"");
    UnicodeString sKey = searchstr + Key;

    if (elem && s.SubString(1, sKey.Length()) == sKey)
    {
        searchstr = sKey;
        return true;
    }

    int skipelem = elem ? 1 : 0;

    for (int ix = 0; ix < listitems->Count - skipelem; ++ix)
    {
        int pos = ix + index + skipelem >= listitems->Count ? ix + index + skipelem - listitems->Count : ix + index + skipelem;
        elem = listitems->Items[pos];
        s = elem->Title.LowerCase();
        if (s.SubString(1, sKey.Length()) == sKey)
        {
            searchstr = sKey;
            elem->Node->Selected = true;
            return true;
        }
        if (searchstr.Length() && searchstr[searchstr.Length()] == Key && s.SubString(1, searchstr.Length()) == searchstr)
        {
            elem->Node->Selected = true;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TCheckboxTreeView::KeyPress(TObject *Sender, wchar_t &Key)
{
    if (onKeyPress)
    {
        onKeyPress(Sender, Key);
        if (!Key)
            return;
    }

    TCheckTreeElem *elem = NULL;
    if (tree->Selected)
        elem = ((TCheckTreeElem*)tree->Selected->Data);

    if (Key == 0x08) // Backspace key.
    {
        if (!searchstr.Length())
            return;
        searchstr.SetLength(searchstr.Length() - 1);
        Key = 0;
        return;
    }
    else if (Key == 0x0D) // Enter key.
        searchstr = L"";
    else if (Key != L' ' || searchstr.Length())
    {
        wchar_t K2;
        LCMapStringW(LOCALE_USER_DEFAULT, LCMAP_LOWERCASE, &Key, 1, &K2, 1);
        if (findnext(elem, K2))
        {
            Key = 0;
            return;
        }
        searchstr = L"";
        return;
    }

    if (!searchstr.Length() && Key == L' ' && elem && elem->HasCheckbox)
    {
        if (tree->SelectionCount > 1)
        {
            BeginUpdate();
            bool val = !elem->Checked;
            for (unsigned int ix = 0; ix < tree->SelectionCount; ++ix)
            {
                TTreeNode *node = tree->Selections[ix];
                elem = (TCheckTreeElem*)node->Data;
                elem->Checked = val;
            }
            EndUpdate();
        }
        else
        {
            elem->Checked = !elem->Checked;
        }
        Key = 0;
        return;
    }

    if (Key == 0x1B && searchstr.Length())
    {
        searchstr = L"";
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TCheckboxTreeView::KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_SUBTRACT || Key == VK_OEM_MINUS || Key == VK_ADD || Key == VK_OEM_PLUS)
    {
        Key = 0;
        return;
    }

    if (onKeyDown)
    {
        onKeyDown(Sender, Key, Shift);
        if (!Key)
            return;
    }

    if (Key == VK_UP || Key == VK_DOWN || Key == VK_LEFT || Key == VK_RIGHT || Key == VK_TAB || Key == VK_NEXT || Key == VK_PRIOR || Key == VK_HOME || Key == VK_END || Key == VK_RETURN)
    {
        searchstr = L"";
        return;
    }

    if (Key == VK_ESCAPE)
    {
        if (searchstr.Length())
        {
//            searchstr = L"";
            Key = 0;
        }
        return;
    }

}
//---------------------------------------------------------------------------
void __fastcall TCheckboxTreeView::Exit(TObject *Sender)//OnExit event
{
    Deactivate(Sender);
    if (onExit)
        onExit(Sender);
}
//---------------------------------------------------------------------------
void TCheckboxTreeView::Deactivate(void *caller)
{
    fmousedown = false;
    searchstr = L"";
    if (lastelem)
        lastelem->ReleaseButton();
}
//---------------------------------------------------------------------------
int TCheckboxTreeView::get_count()
{
    return listitems->Count;
}
//---------------------------------------------------------------------------
const void* TCheckboxTreeView::get_data(int ix)
{
    return listitems->Items[ix]->Data;
}
//---------------------------------------------------------------------------
bool TCheckboxTreeView::get_checked(int ix)
{
    return listitems->Items[ix]->Checked;
}
//---------------------------------------------------------------------------
void TCheckboxTreeView::set_checked(int ix, bool val)
{
    if (!listitems->Items[ix]->HasCheckbox || listitems->Items[ix]->Checked == val)
        return;
    listitems->Items[ix]->SetChecked(val, false);
}
//---------------------------------------------------------------------------
void TCheckboxTreeView::BeginUpdate()
{
    if (!update_counter)
    {
        SendMessage(tree->Handle, WM_SETREDRAW, FALSE, 0);
        LockWindowUpdate(tree->Handle);
    }
    update_counter++;
}
//---------------------------------------------------------------------------
void TCheckboxTreeView::EndUpdate()
{
    update_counter--;
    if (!update_counter)
    {
        SendMessage(tree->Handle, WM_SETREDRAW, TRUE, 0);
        LockWindowUpdate(NULL);
        tree->Invalidate();
    }
}
//---------------------------------------------------------------------------
void TCheckboxTreeView::OpenChecked(TTreeNode *root)
{
    if (!listitems->Count || root && !root->HasChildren)
        return;

    BeginUpdate();

    if (!root)
    {
        TTreeNode *n = tree->Items->Item[0];
        while (n)
        {
            if (n->HasChildren)
                OpenChecked(n);
            n = n->getNextSibling();
        }
    }
    else
    {
        TTreeNode *n = root->getFirstChild();
        while (n)
        {
            if (n->HasChildren)
                OpenChecked(n);
            n = n->getNextSibling();
        }

        if (((TCheckTreeElem*)root->Data)->HasChecked)
            root->Expand(false);
        else
            root->Collapse(true);
    }

    EndUpdate();

}
//---------------------------------------------------------------------------
void TCheckboxTreeView::Clear()
{
    lastelem = NULL;
    BeginUpdate();
    try
    {
        listitems->Clear();
        tree->Items->Clear();
    }
    catch (...)
    {
        ;
    }
    EndUpdate();
}
//---------------------------------------------------------------------------
bool TCheckboxTreeView::has_checked()
{
    TTreeNode *n = tree->Items->GetFirstNode();
    while (n)
    {
        TCheckTreeElem *elem = (TCheckTreeElem*)n->Data;
        if (elem->HasChecked)
            return true;
        n = n->getNextSibling();
    }
    return false;
}
//---------------------------------------------------------------------------
void TCheckboxTreeView::UpdateNodes()
{
    tree->HandleNeeded();
    if (listitems->Count != tree->Items->Count)
        THROW(L"Item count does not match.");

    for (int ix = 0; ix < listitems->Count; ++ix)
        listitems->Items[ix]->fnode = tree->Items->Item[ix];
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
TZTreeNode::TZTreeNode(UnicodeString atext, bool checkbox) : text(atext), data(NULL), parent(NULL), prev(NULL), next(NULL), firstchild(NULL), showbox(checkbox), expanded(false), tree(NULL), glyphhovered(false), hovered(false)
{
    memset(&box, 0, sizeof(box));

}
//---------------------------------------------------------------------------
TZTreeNode::TZTreeNode(UnicodeString atext, bool checkbox, void *adata) : text(atext), data(adata), parent(NULL), prev(NULL), next(NULL), firstchild(NULL), showbox(checkbox), expanded(false), tree(NULL), glyphhovered(false), hovered(false)
{
    memset(&box, 0, sizeof(box));
}
//---------------------------------------------------------------------------
TZTreeNode::~TZTreeNode()
{
    clear();
}
//---------------------------------------------------------------------------
void TZTreeNode::clear()
{
    TZTreeNode *n = firstchild;
    TZTreeNode *n2;
    while (n)
    {
        n2 = n->next;
        delete n;
        n = n2;
    }
}
//---------------------------------------------------------------------------
TZTreeNode* TZTreeNode::get_last()
{
    TZTreeNode *n = this;
    while (n->next)
        n = n->next;
    return n;
}
//---------------------------------------------------------------------------
void TZTreeNode::add_after(TZTreeNode* node)
{
    node->prev = this;
    node->next = next;
    next = node;
    node->tree = tree;
    node->parent = parent;
}
//---------------------------------------------------------------------------
void TZTreeNode::add_child(TZTreeNode *node)
{
    node->tree = tree;
    node->parent = this;
    if (!firstchild)
        firstchild = node;
    else
        firstchild->LastOnLevel->add_after(node);
}
//---------------------------------------------------------------------------
bool TZTreeNode::has_checked()
{
    if (tree->ShowBoxes && showbox && box.checked)
        return true;
    TZTreeNode *n = firstchild;
    while (n)
    {
        if (n->HasChecked)
            return true;
        n = n->next;
    }
    return false;
}
//---------------------------------------------------------------------------
bool TZTreeNode::has_unchecked()
{
    if (showbox && !box.checked)
        return true;
    TZTreeNode *n = firstchild;
    while (n)
    {
        if (n->HasUnchecked)
            return true;
        n = n->next;
    }
    return false;
}
//---------------------------------------------------------------------------
bool TZTreeNode::get_vis()
{
    TZTreeNode *n = parent;
    while (n)
    {
        if (!n->expanded)
            return false;
        n = n->parent;
    }
    return true;
}
//---------------------------------------------------------------------------
bool TZTreeNode::get_hovered()
{
    return hovered && tree->HotTrack;
}
//---------------------------------------------------------------------------
int TZTreeNode::get_level()
{
    int l = 0;
    TZTreeNode *n = parent;
    while (n)
    {
        l++;
        n = n->parent;
    }
    return l;
}
//---------------------------------------------------------------------------
void TZTreeNode::Draw(int ay, bool selected)
{
    drawy = ay;
    TCanvas *c = tree->Canvas;
    TRect textr = displayrect(true);
    c->Brush->Style = bsSolid;
    TSize s;

    if (firstchild)
        c->Font->Style = TFontStyles() << fsBold;
    else
        c->Font->Style = TFontStyles();

    drawbkg(selected);

    GetTextExtentPoint32W(c->Handle, text.c_str(), text.Length(), &s);
    textr.Right = textr.Left + s.cx;
    c->Brush->Style = bsClear;
    ExtTextOutW(c->Handle, textr.Left, textr.Top + (textr.Height() - s.cy) / 2, ETO_CLIPPED, &textr, text.c_str(), text.Length(), NULL);
    if (firstchild)
        drawglyph();
    if (tree->ShowBoxes && showbox)
        drawcheckbox();

}
//---------------------------------------------------------------------------
void TZTreeNode::drawglyph()
{
    TRect r = glyphrect();
    if (r.Right < 0 || r.Left >= tree->pb->ClientWidth || r.Bottom < 0 || r.Top >= tree->pb->ClientHeight)
        return;

    TCanvas *c = tree->Canvas;

    TThemeServices *t = ThemeServices();
    if (t && t->ThemesAvailable && t->ThemesEnabled)
    {
        if (MatchVersion(6, 0))
            tree->handler.DrawThemedGlyph(c, r, expanded, glyphhovered);
        else
        {
            TThemedElementDetails dt;
            dt.Element = teTreeview;
            dt.Part = TVP_GLYPH; //would use TVP_HOTGLYPH for hovered glyphs but it doesn't work
            dt.State = !expanded ? GLPS_CLOSED : GLPS_OPENED;
            t->DrawElement(c->Handle, dt, r);
        }
    }
    else
    {
        c->Brush->Color = clBtnText;
        c->Brush->Style = bsSolid;
        c->Pen->Color = clBtnText;
        c->Pen->Width = 1;
        c->Pen->Style = psSolid;

        c->FrameRect(r);
        if (!expanded)
        {
            c->MoveTo(r.Left + (r.Right - r.Left) / 2, r.Top + 2);
            c->LineTo(r.Left + (r.Right - r.Left) / 2, r.Bottom - 2);
        }
        c->MoveTo(r.Left + 2, r.Top + (r.Bottom - r.Top) / 2);
        c->LineTo(r.Right - 2, r.Top + (r.Bottom - r.Top) / 2);
    }
}
//---------------------------------------------------------------------------
void TZTreeNode::drawcheckbox()
{
    if (!tree->ShowBoxes || !showbox)
        return;

    TRect r = checkboxrect();
    if (r.Right < 0 || r.Left >= tree->pb->ClientWidth || r.Bottom < 0 || r.Top >= tree->pb->ClientHeight)
        return;

    TThemeServices *t = ThemeServices();
    if (t && t->ThemesAvailable && t->ThemesEnabled)
    {
        TThemedElementDetails ted;
        ted.Element = teButton;
        ted.Part = BP_CHECKBOX;

        if (!box.hovered)
            ted.State = box.checked ? CBS_CHECKEDNORMAL : CBS_UNCHECKEDNORMAL;
        else if (box.down)
            ted.State = box.checked ? CBS_CHECKEDPRESSED : CBS_UNCHECKEDPRESSED;
        else if (box.hovered)
            ted.State = box.checked ? CBS_CHECKEDHOT : CBS_UNCHECKEDHOT;
        t->DrawElement(tree->Canvas->Handle, ted, r);
        return;
    }
    DrawFrameControl(tree->Canvas->Handle, &r, DFC_BUTTON, DFCS_BUTTONCHECK | (box.checked ? DFCS_CHECKED : 0) | (box.down && box.hovered ? DFCS_PUSHED : 0) | (box.hovered ? DFCS_HOT : 0));
}
//---------------------------------------------------------------------------
void TZTreeNode::drawbkg(bool selected)
{
    TRect r = displayrect(false);
    TCanvas *c = tree->pb->Canvas;
    c->Brush->Color = clWindow;
    c->Brush->Style = bsSolid;
    c->FillRect(r);

    if (!selected && !Hovered || !selected && !tree->HotTrack)
        return;

    TThemeServices *t = ThemeServices();
    if (MatchVersion(6, 0) && t && t->ThemesAvailable && t->ThemesEnabled)
    {
        tree->handler.DrawThemedBG(c, r, selected, Hovered);
    }
    else
    {

    }
}
//---------------------------------------------------------------------------
TRect TZTreeNode::checkboxrect()
{
    if (!tree)
        THROW(L"The node is not added to a tree yet.");

    int boxw = 13 * scaleres;
    int boxh = 13 * scaleres;

    TRect textr = displayrect(true);
    return Rect(textr.Left - boxw - 4*scaleres, textr.Top + (textr.Height() - boxh) / 2, textr.Left - 4*scaleres, textr.Top + (textr.Height() + boxh) / 2);
}
//---------------------------------------------------------------------------
TRect TZTreeNode::glyphrect()
{
    if (!firstchild)
        return Rect(0, 0, 0, 0);
    TRect r = displayrect(true);

    if (showbox)
        r.Left -= 15 * scaleres;

    int x;
    int y;

    TThemeServices *t = ThemeServices();
    if (t && t->ThemesAvailable && t->ThemesEnabled)
    {
        TSize s;
        if (MatchVersion(6, 0))
            tree->handler.GetGlyphSize(tree->Canvas, s);
        else
            GetThemePartSize((void*)t->Theme[teTreeview], tree->Canvas->Handle, TVP_GLYPH, TREIS_NORMAL, &Rect(0, 0, 999, 999), TS_DRAW, &s);
        y = r.Top + (r.Height() - s.cy) / 2;
        x = r.Left - s.cx - 2 * scaleres;
        return Rect(x, y, x + s.cx, y + s.cy);
    }
    else
    {
        int glyphwidth = 9 * scaleres;
        if (!(glyphwidth % 2))
            glyphwidth++;
        y = r.Top + (r.Height() - glyphwidth) / 2;
        x = r.Left - glyphwidth - 2 * scaleres;
        return Rect(x, y, x + glyphwidth, y + glyphwidth);
    }
}
//---------------------------------------------------------------------------
TRect TZTreeNode::displayrect(bool textonly)
{
    TRect r = Rect(-tree->pb->HorzScrollBar->Position, drawy, max(tree->pb->ClientWidth, tree->pb->HMax - tree->pb->HorzScrollBar->Position), drawy + tree->fsize);
    if (!textonly)
        return r;

    TCanvas *c = tree->Canvas;
/*
  int glyphwidth = 0;

  TThemeServices *t = ThemeServices();
  if (t && t->ThemesAvailable && t->ThemesEnabled) {
    TThemedElementDetails dt;
    dt.Element = teTreeview;
    dt.Part = TVP_GLYPH;
    dt.State = TREIS_NORMAL;
    TSize s;
    if (MatchVersion(6,0))
      tree->handler.GetGlyphSize(tree->Canvas, s);
    else
      GetThemePartSize((void*)t->Theme[teTreeview],c->Handle,TVP_GLYPH,TREIS_NORMAL,&Rect(0,0,999,999),TS_DRAW,&s);
    glyphwidth = s.cx;
  } else {
    glyphwidth = 9*scaleres;
    if (!(glyphwidth % 2))
      glyphwidth++;
  }
  if (glyphwidth)
    glyphwidth += 2*scaleres;
*/
    r.Left += tree->fmargin + Level * tree->fident + (tree->ShowBoxes && showbox ? 15 * scaleres : 0);
    TSize s;
    GetTextExtentPoint32W(c->Handle, text.c_str(), text.Length(), &s);
    r.Right = r.Left + s.cx;
    return r;
}
//---------------------------------------------------------------------------
void TZTreeNode::MouseMove(TShiftState Shift, int X, int Y)
{
    TRect r;
    bool inbox;

    if (tree->ShowBoxes && showbox)
    {
        r = checkboxrect();
        inbox = PtInRect(r, Point(X, Y));
        if ((!Shift.Contains(ssLeft) || box.down) && box.hovered != inbox)
        {
            box.hovered = inbox;
            InvalidateRect(tree->pb->Handle, &r, false);
        }
    }

    if (!Shift.Contains(ssLeft) && firstchild)
    {
        r = glyphrect();
        inbox = PtInRect(r, Point(X, Y));
        if (inbox != glyphhovered)
        {
            glyphhovered = inbox;
            InvalidateRect(tree->pb->Handle, &r, false);
        }

    }

    r = displayrect(false);
    inbox = PtInRect(r, Point(X, Y));
    if (tree->HotTrack && hovered != inbox)
        InvalidateRect(tree->pb->Handle, &r, false);
    hovered = inbox;
}
//---------------------------------------------------------------------------
void TZTreeNode::MouseDown(TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft)
        return;
    if (box.hovered)
    {
        if (!tree->IsSelected(this))
            tree->Select(this, false);

        box.down = true;
        InvalidateRect(tree->pb->Handle, &checkboxrect(), false);
    }
    else if (glyphhovered || firstchild && Shift.Contains(ssDouble))
        Expanded = !Expanded;
    else
        tree->Select(this, Shift.Contains(ssShift));
}
//---------------------------------------------------------------------------
void TZTreeNode::MouseUp(TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft)
        return;

    if (box.down)
    {
        box.down = false;
        if (!box.hovered)
        {
            return;
        }
        if (tree->firstsel != this)
        {
            bool val = !Checked;
            tree->BeginUpdate();

            TZTreeNode *n = tree->backsel ? tree->lastsel : tree->firstsel;
            while (n)
            {
                n->Checked = val;
                if (n == (tree->backsel ? tree->firstsel : tree->lastsel))
                    break;
                n = n->NextVisible;
            }
            tree->EndUpdate();
        }
        else
            Checked = !Checked;
//InvalidateRect(tree->pb->Handle,&checkboxrect(),false);
    }
}
//---------------------------------------------------------------------------
TZTreeNode* TZTreeNode::get_prevvis()
{
    TZTreeNode *n = this;
    TZTreeNode *top = this;
    while (n->parent)
    {
        n = n->parent;
        if (!n->expanded)
            top = n;
    }

    if (top != this)
        return top;

    if (!prev)
        return parent;

    n = prev;

    while (n->firstchild && n->expanded)
    {
        n = n->firstchild;
        while (n->next)
            n = n->next;
    }
    return n;
}
//---------------------------------------------------------------------------
TZTreeNode* TZTreeNode::get_nextvis()
{
    TZTreeNode *n = this;
    while (n->parent && !n->parent->expanded)
        n = n->parent;

    if (expanded && firstchild)
        return firstchild;

    if (!n->next && n->parent)
    {
        do
        {
            n = n->parent;
        }
        while (n && !n->next);
    }
    if (n)
        n = n->next;

    return n;
}
//---------------------------------------------------------------------------
TZTreeNode* TZTreeNode::get_absnext()
{
    if (firstchild)
        return firstchild;

    TZTreeNode *n = this;
    while (n && !n->next)
        n = n->parent;

    if (n)
        return n->next;

    return NULL;
}
//---------------------------------------------------------------------------
void TZTreeNode::ReleaseButton()
{
    if (box.down)
    {
        box.down = false;
        if (box.hovered)
            InvalidateRect(tree->pb->Handle, &checkboxrect(), false);
    }
}
//---------------------------------------------------------------------------
void TZTreeNode::MouseLeave()
{
    if (box.hovered)
    {
        box.hovered = false;
        InvalidateRect(tree->pb->Handle, &checkboxrect(), false);
    }

    glyphhovered = false;

    if (Hovered)
        InvalidateRect(tree->pb->Handle, &displayrect(false), false);
    hovered = false;
}
//---------------------------------------------------------------------------
void TZTreeNode::set_expanded(bool val)
{
    updateexpanded(val, true);
}
//---------------------------------------------------------------------------
void TZTreeNode::updateexpanded(bool val, bool notify)
{
    if (expanded == val)
        return;

    expanded = val;

    if (!tree)
        return;

    tree->update(true);
}
//---------------------------------------------------------------------------
bool TZTreeNode::get_checked()
{
    if (!showbox)
        return false;
    return box.checked;
}
//---------------------------------------------------------------------------
void TZTreeNode::set_checked(bool val)
{
    updatechecked(val, true);
}
//---------------------------------------------------------------------------
void TZTreeNode::updatechecked(bool val, bool notify)
{
    if (!showbox || box.checked == val)
        return;
    box.checked = val;

    if (!tree)
        return;

    TRect r = checkboxrect();

    InvalidateRect(tree->pb->Handle, &r, true);
//updateparents(val);

    if (notify)
    {
        if (tree->OnCheck)
            tree->OnCheck(tree, this, tree->updating);
        tree->checkupdate(this);
    }
}
//---------------------------------------------------------------------------
int TZTreeNode::ComparePosition(TZTreeNode *node)
{
    return tree->ComparePosition(this, node);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TZTreeThemeHandler::TZTreeThemeHandler()
{
    treetheme = NULL;
    usecnt = 0;
}

void TZTreeThemeHandler::DrawThemedBG(TCanvas *Canvas, const TRect &r, bool selected, bool hovered)
{
    if (!selected && !hovered)
        return;

    TThemeServices *t = ThemeServices();
    if (!MatchVersion(6, 0) || !t || !t->ThemesAvailable || !t->ThemesEnabled)
        THROW(L"Access when no themes present");

    if (!treetheme)
        treetheme = OpenThemeData(Application->MainForm->Handle, L"Explorer::Treeview");
    DrawThemeBackground(treetheme, Canvas->Handle, TVP_TREEITEM, selected && hovered ? TREIS_HOTSELECTED : selected ? TREIS_SELECTED : TREIS_HOT, &r, &r);
}

void TZTreeThemeHandler::DrawThemedGlyph(TCanvas *Canvas, const TRect &r, bool open, bool hovered)
{
    TThemeServices *t = ThemeServices();
    if (!MatchVersion(6, 0) || !t || !t->ThemesAvailable || !t->ThemesEnabled)
        THROW(L"Access when no themes present");

    if (!treetheme)
        treetheme = OpenThemeData(Application->MainForm->Handle, L"Explorer::Treeview");
    DrawThemeBackground(treetheme, Canvas->Handle, hovered ? TVP_HOTGLYPH : TVP_GLYPH, hovered ? (open ? GLPS_OPENED : GLPS_CLOSED) : (open ? HGLPS_OPENED : HGLPS_CLOSED), &r, &r);
}

void TZTreeThemeHandler::GetGlyphSize(TCanvas *Canvas, TSize &s)
{
    TThemeServices *t = ThemeServices();
    if (!MatchVersion(6, 0) || !t || !t->ThemesAvailable || !t->ThemesEnabled)
        THROW(L"Access when no themes present");

    if (!treetheme)
        treetheme = OpenThemeData(Application->MainForm->Handle, L"Explorer::Treeview");

    GetThemePartSize(treetheme, Canvas->Handle, TVP_GLYPH, GLPS_OPENED, &Rect(0, 0, 999, 999), TS_TRUE, &s);
}

void TZTreeThemeHandler::Register()
{
    TThemeServices *t = ThemeServices();
    if (!t)
        return;

    if (!usecnt)
    {
        themeevent = t->OnThemeChange;
        t->OnThemeChange = &ThemeEvent;
    }
    usecnt++;
}
void TZTreeThemeHandler::Unregister()
{
    TThemeServices *t = ThemeServices();
    if (!t)
        return;

    usecnt--;
    if (!usecnt)
    {
        if (treetheme)
            CloseThemeData(treetheme);
        treetheme = NULL;
        t->OnThemeChange = themeevent;
    }
}

void __fastcall TZTreeThemeHandler::ThemeEvent(TObject *Sender)
{
    if (treetheme)
        CloseThemeData(treetheme);
    treetheme = NULL;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TZTreeThemeHandler TZTreeView::handler;

TZTreeView::TZTreeView(TZPaintBox *abox) : updating(0), fshowbox(true), pb(abox), flines(0), fchange(false),
        fcheckchange(false), root(NULL), lastnode(NULL), firstsel(0), lastsel(0),
        backsel(0), fmulti(true), fhottrack(true), topnode(NULL), lastscrollpos(0)
{
    handler.Register();

    pb->InvalidateOnScroll = false;

    fonpaint = pb->OnPaint;
    fongetoverflow = pb->OnGetOverflow;
    fonresize = pb->OnResize;

    fonmousemove = pb->OnMouseMove;
    fonmousedown = pb->OnMouseDown;
    fonmouseup = pb->OnMouseUp;
    fonmouseleave = pb->OnMouseLeave;
    foncapturechanged = pb->OnCaptureChanged;
    fonkeypress = pb->OnKeyPress;
    fonkeydown = pb->OnKeyDown;
    fonexit = pb->OnExit;
    fonvscroll = pb->OnVScroll;
    fonhscroll = pb->OnHScroll;

    pb->OnPaint = pbPaint;
    pb->OnGetOverflow = pbGetOverflow;
    pb->OnResize = pbResize;

    pb->OnMouseMove = pbMouseMove;
    pb->OnMouseDown = pbMouseDown;
    pb->OnMouseUp = pbMouseUp;
    pb->OnMouseLeave = pbMouseLeave;
    pb->OnCaptureChanged = pbCaptureChanged;
    pb->OnKeyPress = pbKeyPress;
    pb->OnKeyDown = pbKeyDown;
    pb->OnExit = pbExit;
    pb->OnVScroll = pbVScroll;
    pb->OnHScroll = pbHScroll;

    pb->HorzScrollBar->SmallChange = 16 * scaleres;
    pb->VertScrollBar->SmallChange = 1;

    TSize s;
    GetTextExtentPoint32W(pb->Canvas->Handle, L"My", 2, &s);
    fsize = max(16, s.cy + 4 * scaleres);
    fmargin = (fsize + 2) * scaleres;
    fident = fsize * 0.6;
}
//---------------------------------------------------------------------------
__fastcall TZTreeView::~TZTreeView()
{
    Clear();

    pb->OnVScroll = fonhscroll;
    pb->OnHScroll = fonvscroll;
    pb->OnExit = fonexit;
    pb->OnMouseUp = fonmouseup;
    pb->OnMouseDown = fonmousedown;
    pb->OnMouseMove = fonmousemove;
    pb->OnKeyDown = fonkeydown;
    pb->OnKeyPress = fonkeypress;
    pb->OnCaptureChanged = foncapturechanged;
    pb->OnMouseLeave = fonmouseleave;

    pb->OnResize = fonresize;
    pb->OnGetOverflow = fongetoverflow;
    pb->OnPaint = fonpaint;

    handler.Unregister();
}
//---------------------------------------------------------------------------
void __fastcall TZTreeView::pbGetOverflow(TObject *Sender, int clientwidth, int clientheight, TZScrollOverflow &data)
{
    data.visiblewidth = clientwidth;
    data.hiddenwidth = 0;
    data.visibleheight = max(1, clientheight / max(1, fsize));
    data.hiddenheight = flines - data.visibleheight;
    measure_sizes();
}
//---------------------------------------------------------------------------
void __fastcall TZTreeView::pbResize(TObject *Sender)
{
    measure_sizes();
}
//---------------------------------------------------------------------------
void TZTreeView::measure_sizes()
{
    pb->HorzScrollBar->LargeChange = pb->ClientWidth * 0.8;
    pb->VertScrollBar->LargeChange = max(1, pb->ClientHeight / max(1, fsize) - 1);
}
//---------------------------------------------------------------------------
TCanvas* TZTreeView::get_canvas()
{
    return pb->Canvas;
}
//---------------------------------------------------------------------------
void __fastcall TZTreeView::pbPaint(TObject *Sender)
{
    TCanvas *c = Canvas;
    int y = 0;

    TZTreeNode *n = FirstVisibleNode();
    bool insel = ComparePosition(n, backsel ? lastsel : firstsel) >= 0 && ComparePosition(n, backsel ? firstsel : lastsel) <= 0;

    while (n && y < pb->ClientHeight)
    {
        if (n == (backsel ? lastsel : firstsel))
            insel = true;

        n->Draw(y, insel);
        y += fsize;

        if (n == (backsel ? firstsel : lastsel))
            insel = false;

        if (n->firstchild && n->expanded)
            n = n->firstchild;
        else if (n->next)
            n = n->next;
        else
        {
            while (n && !n->next)
                n = n->parent;
            if (n)
                n = n->next;
        }
    }

    if (y < pb->ClientHeight)
    {
        c->Brush->Style = bsSolid;
        c->Brush->Color = clWindow;
        c->FillRect(Rect(0, y, pb->ClientWidth, pb->ClientHeight));
    }
}
//---------------------------------------------------------------------------
void TZTreeView::update(bool changed)
{
    fchange |= changed;
    if (updating || pb->ComponentState.Contains(csDestroying))
        return;

    if (fchange)
    {
        fchange = false;
        updateselection();
        countlines();
        pb->ScrollResize();
    }

    if (pb->Visible)
        pb->Invalidate();
}
//---------------------------------------------------------------------------
void TZTreeView::checkupdate(bool checked)
{
    fcheckchange |= checked;

    if (updating || pb->ComponentState.Contains(csDestroying))
        return;

    if (fcheckchange)
    {
        fcheckchange = false;

        if (fonmulticheck)
            fonmulticheck(this);
    }
}
//---------------------------------------------------------------------------
void TZTreeView::countlines()
{
    flines = 0;
    TZTreeNode *n = root;
    while (n)
    {
        flines++;
        if (n->firstchild && n->expanded)
            n = n->firstchild;
        else if (n->next)
            n = n->next;
        else
        {
            while (n && !n->next)
                n = n->parent;
            if (n)
                n = n->next;
        }
    }
}
//---------------------------------------------------------------------------
void TZTreeView::BeginUpdate()
{
    updating++;
}
//---------------------------------------------------------------------------
void TZTreeView::EndUpdate()
{
    if (updating)
        updating--;
    checkupdate(false);
    update(false);
}
//---------------------------------------------------------------------------
void TZTreeView::StopUpdate()
{
    updating = 0;
    checkupdate(false);
    update(false);
}
//---------------------------------------------------------------------------
void TZTreeView::Clear()
{
    BeginUpdate();
    delete root;
    root = NULL;
    firstsel = NULL;
    lastsel = NULL;
    topnode = NULL;
    lastscrollpos = 0;
    backsel = false;
    EndUpdate();
}
//---------------------------------------------------------------------------
void TZTreeView::AddNode(TZTreeNode *parent, TZTreeNode *child)
{
    if (parent && parent->tree != this)
        THROW(L"The specified parent does not belong to this tree!");
    if (!child)
        THROW(L"Cannot add NULL as child.");
    if (child->tree)
        THROW(L"This child node is already added to a tree.");

    child->tree = this;
    if (!parent)
    {
        if (!root)
        {
            root = child;
            firstsel = root;
            lastsel = root;
            backsel = false;
            child->tree = this;
            topnode = root;
            lastscrollpos = 0;
        }
        else
            root->LastOnLevel->add_after(child);
    }
    else
        parent->add_child(child);

    if (!updating && topnode && child->Visible)
    {
        int pos = ComparePosition(topnode, child);
        if (pos > 0)
            topnode = topnode->PreviousVisible;
    }
    else if (updating)
    {
        topnode = NULL;
        lastscrollpos = pb->VPos;
    }

    if (!updating && child->Visible)
    {
        flines++;
        pb->ScrollResize();
    }

    update(true);
}
//---------------------------------------------------------------------------
bool TZTreeView::has_checked()
{
    TZTreeNode *n = root;
    if (!n)
        return false;
    while (n)
    {
        if (n->HasChecked)
            return true;
        n = n->next;
    }
    return false;
}
//---------------------------------------------------------------------------
bool TZTreeView::has_unchecked()
{
    TZTreeNode *n = root;
    if (!n)
        return false;
    while (n)
    {
        if (n->HasUnchecked)
            return true;
        n = n->next;
    }
    return false;
}
//---------------------------------------------------------------------------
bool TZTreeView::has_sel()
{
    return firstsel != NULL;
}
//---------------------------------------------------------------------------
bool TZTreeView::more_sel()
{
    return firstsel != lastsel;
}
//---------------------------------------------------------------------------
TZTreeNode* TZTreeView::get_selected(int ix)
{
    TZTreeNode *n = firstsel;
    while (ix--)
    {
        if (n->expanded && n->firstchild)
            n = n->firstchild;
        else if (n->next)
            n = n->next;
        else
        {
            while (n && !n->next)
                n = n->parent;
            if (n)
                n = n->next;
        }
    }

    if (!n)
        THROW(L"Bad index in selection.");

    return n;
}
//---------------------------------------------------------------------------
void __fastcall TZTreeView::pbMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    TZTreeNode *node = !Shift.Contains(ssLeft) ? GetNodeAt(Y) : lastnode;
    if (node)
        node->MouseMove(Shift, X, Y);

    if (lastnode && node != lastnode)
        lastnode->MouseLeave();

    lastnode = node;

    pb->Cursor = lastnode ? crHandPoint : crDefault;

    if (fonmousemove)
        fonmousemove(Sender, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TZTreeView::pbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TShiftState s2 = Shift;
    s2 >> (Button == mbLeft ? ssLeft : Button == mbRight ? ssRight : ssMiddle);
    pbMouseMove(this, s2, X, Y);

    searchstr = "";
    if (lastnode && Button == mbLeft)
        lastnode->MouseDown(Button, Shift, X, Y);

    if (fonmousedown)
        fonmousedown(Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TZTreeView::pbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button == mbLeft && lastnode)
        lastnode->MouseUp(Button, Shift, X, Y);

    if (fonmouseup)
        fonmouseup(Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TZTreeView::pbMouseLeave(TObject *Sender)
{
    if (lastnode != NULL)
        lastnode->MouseLeave();
    lastnode = NULL;

    if (fonmouseleave)
        fonmouseleave(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TZTreeView::pbCaptureChanged(TObject *Sender)
{
    if (lastnode)
        lastnode->ReleaseButton();
}
//---------------------------------------------------------------------------
bool TZTreeView::findnext(TZTreeNode *node, wchar_t Key)
{

    TZTreeNode *index = !node ? root : node;
    if (!index)
        return false;

    UnicodeString s = node ? node->text.LowerCase() : UnicodeString(L"");
    UnicodeString sKey = searchstr + Key;

    if (node && s.SubString(1, sKey.Length()) == sKey)
    {
        searchstr = sKey;
        return true;
    }

    bool matchindex = index != node;
    if (!matchindex)
    {
        node = node->AbsoluteNext;
        if (!node)
            node = root;
    }

    while (matchindex || node != index)
    {
        s = node->text.LowerCase();
        if (s.SubString(1, sKey.Length()) == sKey)
        {
            searchstr = sKey;
            Select(node, false);
            return true;
        }
        if (searchstr.Length() && searchstr[searchstr.Length()] == Key && s.SubString(1, searchstr.Length()) == searchstr)
        {
            Select(node, false);
            return true;
        }
        if (matchindex && node == index)
            break;
        node = node->AbsoluteNext;
        if (!node)
            node = root;
    }

    return false;
}
//---------------------------------------------------------------------------
void __fastcall TZTreeView::pbKeyPress(TObject *Sender, wchar_t &Key)
{
    if (fonkeypress)
    {
        fonkeypress(Sender, Key);
        if (!Key)
            return;
    }

    TZTreeNode *node = NULL;
    if (lastsel)
        node = lastsel;

  //L'\b' // backspace
  //L'\r' // enter
    if (Key != L'\b' && Key != L'\r' && (Key != L' ' || searchstr.Length()))
    {
        wchar_t K2;
        LCMapStringW(LOCALE_USER_DEFAULT, LCMAP_LOWERCASE, &Key, 1, &K2, 1);
        if (findnext(node, K2))
        {
            Key = 0;
            return;
        }
        searchstr = L"";
        return;
    }
    if (Key == L'\b')
    {
        if (!searchstr.Length())
            return;
        searchstr.SetLength(searchstr.Length() - 1);
        Key = 0;
        return;
    }

    if (Key == L'\r')
        searchstr = L"";

    if (!searchstr.Length() && Key == L' ' && node && node->HasCheckbox)
    {
        if (firstsel != lastsel)
        {
            BeginUpdate();
            bool val = !node->Checked;
            node = backsel ? lastsel : firstsel;
            do
            {
                node->Checked = val;
                if (node == (backsel ? firstsel : lastsel))
                    break;
                node = node->NextVisible;
            }
            while (true);
            EndUpdate();
        }
        else
            node->Checked = !node->Checked;
        Key = 0;
        return;
    }

}
//---------------------------------------------------------------------------
void __fastcall TZTreeView::pbKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (fonkeydown)
    {
        fonkeydown(Sender, Key, Shift);
        if (!Key)
            return;
    }

    if (Key == VK_SUBTRACT || Key == VK_OEM_MINUS || Key == VK_ADD || Key == VK_OEM_PLUS)
    {
        Key = 0;
        return;
    }

    if (Key == VK_UP || Key == VK_DOWN || Key == VK_LEFT || Key == VK_RIGHT || Key == VK_TAB || Key == VK_NEXT || Key == VK_PRIOR || Key == VK_HOME || Key == VK_END || Key == VK_RETURN)
    {
        TZTreeNode *n, *n2;
        int lines;

        if (root)
            switch (Key)
            {
            case VK_UP:
                n = lastsel ? lastsel->PreviousVisible : root;
                Select(n ? n : lastsel, Shift.Contains(ssShift));
                break;
            case VK_DOWN:
                n = lastsel ? lastsel->NextVisible : root;
                Select(n ? n : lastsel, Shift.Contains(ssShift));
                break;
            case VK_HOME:
                if (lastsel != root)
                    Select(root, Shift.Contains(ssShift));
                break;
            case VK_END:
                n = root->LastOnLevel;
                while (n->firstchild && n->expanded)
                    n = n->firstchild->LastOnLevel;
                Select(n, Shift.Contains(ssShift));
                break;
            case VK_LEFT:
                if (firstsel && firstsel->expanded)
                    firstsel->Expanded = false;
                else if (firstsel && firstsel->parent)
                    Select(firstsel->parent, Shift.Contains(ssShift));
                break;
            case VK_RIGHT:
                if (firstsel && !firstsel->expanded)
                    firstsel->Expanded = true;
                break;
            case VK_PRIOR:
                if (!lastsel)
                    break;
                lines = max(1, pb->ClientHeight / fsize - 1);
                n = lastsel;
                while (lines--)
                {
                    n2 = n->PreviousVisible;
                    if (!n2)
                        break;
                    n = n2;
                }
                Select(n, Shift.Contains(ssShift));
                break;
            case VK_NEXT:
                if (!lastsel)
                    break;
                lines = max(1, pb->ClientHeight / fsize - 1);
                n = lastsel;
                while (lines--)
                {
                    n2 = n->NextVisible;
                    if (!n2)
                        break;
                    n = n2;
                }
                Select(n, Shift.Contains(ssShift));
                break;
            }

        searchstr = L"";
        return;
    }

    if (Key == VK_ESCAPE)
    {
        if (searchstr.Length())
        {
            searchstr = L"";
            Key = 0;
        }
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TZTreeView::pbExit(TObject *Sender)
{
    searchstr = L"";
    if (fonexit)
        fonexit(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TZTreeView::pbVScroll(TObject *Sender, TScrollCode ScrollCode, int &ScrollPos)
{
    if (ScrollPos == 0)
    {
        topnode = root;
        lastscrollpos = 0;
        pb->Invalidate();
    }
    else if (topnode)
    {
        if (ScrollPos > lastscrollpos)
        {
            while (ScrollPos != lastscrollpos)
            {
                topnode = topnode->NextVisible;
                lastscrollpos++;
            }
            pb->Invalidate();
        }
        else if (ScrollPos < lastscrollpos)
        {
            while (ScrollPos != lastscrollpos)
            {
                topnode = topnode->PreviousVisible;
                lastscrollpos--;
            }
            pb->Invalidate();
        }
    }
    else if (lastscrollpos != ScrollPos)
        pb->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TZTreeView::pbHScroll(TObject *Sender, TScrollCode ScrollCode, int &ScrollPos)
{
    pb->Invalidate();
}
//---------------------------------------------------------------------------
TZTreeNode* TZTreeView::FirstVisibleNode()
{
    if (topnode)
        return topnode;

    int top = pb->VPos;
    topnode = root;
    while (top--)
        topnode = topnode->NextVisible;
    return topnode;
}
//---------------------------------------------------------------------------
TZTreeNode* TZTreeView::GetNodeAt(int y)
{
    TZTreeNode *n = FirstVisibleNode();
    int top = y / fsize;
    while (n && top--)
        n = n->NextVisible;
    return n;
}
//---------------------------------------------------------------------------
void TZTreeView::Select(TZTreeNode *node, bool multiend)
{
    if (node->tree != this)
        THROW(L"Node is not in this tree view!");

    if (!multiend && node == firstsel || multiend && node == firstsel && node == lastsel)
        return;

    if (!multiend || !fmulti || !firstsel)
    {
        InvalidateSelected();
        lastsel = firstsel = node;
        if (firstsel)
        {
            if (InView(node))
                InvalidateRect(pb->Handle, &firstsel->displayrect(false), false);
            else
                ScrollInView(node);
        }
        if (fonselchange)
            fonselchange(this);
        return;
    }

    bool inview = !node || InView(node);
    if (inview)
        InvalidateSelected();

    lastsel = node;
    backsel = lastsel && firstsel ? ComparePosition(firstsel, lastsel) > 0 : false;

    if (inview)
        InvalidateSelected();
    else
        ScrollInView(node);

    if (fonselchange)
        fonselchange(this);
}
//---------------------------------------------------------------------------
void TZTreeView::set_multi(bool val)
{
    if (fmulti == val)
        return;
    fmulti = val;
    if (val)
        return;

    InvalidateSelected();
    lastsel = firstsel;
}
//---------------------------------------------------------------------------
void TZTreeView::InvalidateSelected()
{
    TZTreeNode *n = backsel ? lastsel : firstsel;
    while (n)
    {
        InvalidateRect(pb->Handle, &n->displayrect(false), false);
        if (n == (backsel ? firstsel : lastsel))
            break;
        n = n->NextVisible;
    }
}
//---------------------------------------------------------------------------
bool TZTreeView::IsSelected(TZTreeNode *node)
{
    if (node->tree != this)
        THROW(L"Node is not in this tree view!");

    if (!node->Visible)
        return false;

    bool sel = false;
    TZTreeNode *n = backsel ? lastsel : firstsel;
    while (n && !sel)
    {
        if (n == node)
            sel = true;
        if (n == (backsel ? firstsel : lastsel))
            break;
        n = n->NextVisible;
    }
    return sel;
}
//---------------------------------------------------------------------------
void TZTreeView::updateselection()
{
    bool firstvis = !firstsel || firstsel->Visible;
    bool lastvis = !lastsel || lastsel->Visible;

    if (firstvis && lastvis)
        return;

    //bottom visible only
    if (backsel ? (firstvis && !lastvis) : (lastvis && !firstvis))
    {
        if (backsel)
            lastsel = lastsel->NextVisible;
        else
            firstsel = firstsel->NextVisible;
    }
    else if (backsel ? (lastvis && !firstvis) : (firstvis && !lastvis))
    { //top only
        if (backsel)
            firstsel = firstsel->PreviousVisible;
        else
            lastsel = lastsel->PreviousVisible;
    }
    else
        firstsel = lastsel = firstsel->PreviousVisible;

}
//---------------------------------------------------------------------------
bool TZTreeView::InView(TZTreeNode *node)
{
    if (node->tree != this)
        THROW(L"Node is not in this tree view!");

    if (!node->Visible)
        return false;

    int y = fsize * 2;
    TZTreeNode *n = FirstVisibleNode();

    while (n && y < pb->ClientHeight && node != n)
    {
        n = n->NextVisible;
        y += fsize;
    }
    return n == node;
}
//---------------------------------------------------------------------------
void TZTreeView::ScrollInView(TZTreeNode *node)
{
    if (node->tree != this)
        THROW(L"Node is not in this tree view!");
    TZTreeNode *top = FirstVisibleNode();
    if (node == top)
        return;

    BeginUpdate();
    if (!node->Visible)
    {
        TZTreeNode *p = node->parent;
        while (p)
        {
            p->Expanded = true;
            p = p->parent;
        }
    }

    int dif = 0;
    if (ComparePosition(top, node) < 0)
    {
        while (top != node)
        {
            top = top->NextVisible;
            dif++;
        }
        int lines = pb->ClientHeight / fsize;
        if (lines > dif)
        {
            EndUpdate();
            return;
        }

        dif = (dif - lines + 1);
    }
    else
    {
        while (top != node)
        {
            top = top->PreviousVisible;
            dif--;
        }
    }

    pb->VPos += dif;
    EndUpdate();
}
//---------------------------------------------------------------------------
int TZTreeView::ComparePosition(TZTreeNode *anode, TZTreeNode *bnode)
{
    if (anode->tree != bnode->tree)
        THROW(L"The nodes are not in the same tree.");

    if (anode == bnode)
        return 0;

    int al = anode->Level;
    int bl = bnode->Level;

    while (bl > al)
    {
        bnode = bnode->parent;
        bl--;
    }
    if (bnode == anode)
        return -1;

    while (bl < al)
    {
        anode = anode->parent;
        al--;
    }
    if (anode == bnode)
        return 1;

    while (bnode->parent != anode->parent)
    {
        bnode = bnode->parent;
        anode = anode->parent;
    }

    TZTreeNode *an = anode;
    TZTreeNode *bn = bnode;
    while (bn && an && bn != anode && an != bnode)
    {
        bn = bn->next;
        an = an->next;
    }

    if (!bn || an == bnode)
        return -1;
    return 1; //when (!an || bn == anode)
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(13);
#endif

