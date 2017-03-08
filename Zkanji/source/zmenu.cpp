//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Themes.hpp>
#include <vsstyle.h>

#include "zmenu.h"
#include "alphaimagelist.h"
#include "module.h"
#include "settings.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(210);
#endif

TZPopupMenu *menu = NULL;
//---------------------------------------------------------------------------
TZMenuRow::TZMenuRow(int column_count) : fvisible(true), fchecked(NULL), fcolcnt(column_count)
{
    fitem = new TZItem[column_count];
    for (int ix = 0; ix < fcolcnt; ++ix)
    {
        fitem[ix].title = L"";
        fitem[ix].type = itSimple;
        fitem[ix].data = NULL;
        fitem[ix].enabled = true;
        fitem[ix].callback = NULL;
        fitem[ix].submenu = NULL;
    }
}
//---------------------------------------------------------------------------
TZMenuRow::~TZMenuRow()
{
    delete[] fitem;
    delete[] fchecked;
}
//---------------------------------------------------------------------------
void TZMenuRow::Setup(wchar_t *titles[], TZMenuItemType types[], void *data[], TZMenuItemClickProc callback[], bool enabled[])
{
    for (int ix = 0; ix < fcolcnt; ++ix)
    {
        fitem[ix].title = titles[ix];
        fitem[ix].type = types[ix];
        if (enabled)
            fitem[ix].enabled = enabled[ix];
        if (!fchecked && (types[ix] == itCheck || types[ix] == itCheckAuto))
        {
            fchecked = new bool[fcolcnt];
            memset(fchecked, 0, sizeof(bool)*fcolcnt);
        }
        if (callback)
            fitem[ix].callback = callback[ix];
        if (data)
            fitem[ix].data = data[ix];
    }
}
//---------------------------------------------------------------------------
UnicodeString TZMenuRow::get_title(int ix)
{
    return item[ix].title;
}
//---------------------------------------------------------------------------
void TZMenuRow::set_title(int ix, UnicodeString val)
{
    if (item[ix].title == val)
        return;
    item[ix].title = val;
}
//---------------------------------------------------------------------------
TZMenuItemType TZMenuRow::get_type(int ix)
{
    return item[ix].type;
}
//---------------------------------------------------------------------------
void TZMenuRow::set_type(int ix, TZMenuItemType val)
{
    if (item[ix].type == val)
        return;
    if ((item[ix].type != itCheck && item[ix].type != itCheckAuto) && (val == itCheck || val == itCheckAuto) && !fchecked)
    {
        fchecked = new bool[fcolcnt];
        memset(fchecked, 0, sizeof(bool)*fcolcnt);
    }

    item[ix].type = val;
}
//---------------------------------------------------------------------------
void* TZMenuRow::get_data(int ix)
{
    return item[ix].data;
}
//---------------------------------------------------------------------------
void TZMenuRow::set_data(int ix, void* val)
{
    if (item[ix].data == val)
        return;
    item[ix].data = val;
}
//---------------------------------------------------------------------------
TZMenuItemClickProc TZMenuRow::get_callback(int ix)
{
    return item[ix].callback;
}
//---------------------------------------------------------------------------
void TZMenuRow::set_callback(int ix, TZMenuItemClickProc val)
{
    if (item[ix].callback == val)
        return;

    item[ix].callback = val;
}
//---------------------------------------------------------------------------
bool TZMenuRow::get_check(int ix)
{
    return (item[ix].type == itCheck || item[ix].type == itCheckAuto) && fchecked[ix];
}
//---------------------------------------------------------------------------
void TZMenuRow::set_check(int ix, bool val)
{
    if (item[ix].type != itCheck && item[ix].type != itCheckAuto || fchecked && fchecked[ix] == val)
        return;
    fchecked[ix] = val;
}
//---------------------------------------------------------------------------
bool TZMenuRow::get_enabled(int ix)
{
    return item[ix].enabled;
}
//---------------------------------------------------------------------------
void TZMenuRow::set_enabled(int ix, bool val)
{
    if (item[ix].enabled == val)
        return;
    item[ix].enabled = val;
}
//---------------------------------------------------------------------------
TZMenuRows* TZMenuRow::get_subs(int ix)
{
    return item[ix].submenu;
}
//---------------------------------------------------------------------------
void TZMenuRow::set_subs(int ix, TZMenuRows *val)
{
    item[ix].submenu = val;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TZMenuRows::TZMenuRows(int column_count) : inherited(), fcolcnt(column_count), spans(NULL), ftag(0)
{

}
//---------------------------------------------------------------------------
TZMenuRows::~TZMenuRows()
{
    Clear();
    delete spans;
}
//---------------------------------------------------------------------------
void TZMenuRows::Clear()
{
    inherited::Clear();
    if (spans)
        spans->Clear();
}
//---------------------------------------------------------------------------
void TZMenuRows::FreeItem(TZMenuRowItem *item, int ix)
{
    item->refcount--;
    if (!item->refcount)
    {
        delete item->row;
        delete item;
    }
}
//---------------------------------------------------------------------------
int TZMenuRows::Add(TZMenuRowItem *item)
{
    item->refcount++;
    return inherited::Add(item);
}
//---------------------------------------------------------------------------
TZMenuRow* TZMenuRows::AddRow()
{
    TZMenuRowItem *item = new TZMenuRowItem;
    item->refcount = 0;
    item->row = new TZMenuRow(fcolcnt);
    return Items[Add(item)]->row;
}
//---------------------------------------------------------------------------
void TZMenuRows::AddSpan(int after)
{
    if (!Count || after > Count || after < 0 && spans && spans->Count && spans->Items[spans->Count - 1] == Count - 1 || after >= 0 && spans && spans->IndexOf(after) >= 0)
        return;
    if (!spans)
        spans = new TIntegerList;
    spans->Add(after < 0 ? Count - 1 : after);
}
//---------------------------------------------------------------------------
void TZMenuRows::AppendRows(TZMenuRows *rows, bool addspanbefore)
{
    if (rows->ColCount != ColCount)
        THROW(L"Column count does not match!");

    if (Count && addspanbefore)
        AddSpan();

    for (int ix = 0; ix < rows->Count; ++ix)
    {
        TZMenuRowItem *item = rows->Items[ix];
        Add(item);
    }
}
//---------------------------------------------------------------------------
TZMenuRow* TZMenuRows::get_row(int ix)
{
    return Items[ix]->row;
}
//---------------------------------------------------------------------------
int TZMenuRows::lastrow()
{
    int row = Count - 1;
    while (row >= 0 && !Rows[row]->Visible)
        row--;
    return row;
}
//---------------------------------------------------------------------------
int TZMenuRows::firstrow()
{
    int row = 0;
    while (row < Count && !Rows[row]->Visible)
        row++;
    return row < Count ? row : -1;
}
//---------------------------------------------------------------------------
int TZMenuRows::rowabove(int row)
{
    while (--row >= 0 && !Rows[row]->Visible);
    return row;
}
//---------------------------------------------------------------------------
int TZMenuRows::rowbelow(int row)
{
    while (++row < Count && !Rows[row]->Visible);
    return row < Count ? row : -1;
}
//---------------------------------------------------------------------------
bool TZMenuRows::differentabove(int row, int col)
{
    if (spanabove(row))
        return true;
    TZMenuItemType typ = Rows[row]->Types[col];
    row = rowabove(row);
    if (row < 0)
        return true;
    TZMenuItemType typ2 = Rows[row]->Types[col];
    return (typ2 == itTitle || typ2 == itAltTitle) != (typ == itTitle || typ == itAltTitle);
}
//---------------------------------------------------------------------------
bool TZMenuRows::differentbelow(int row, int col)
{
    if (spanbelow(row))
        return true;
    TZMenuItemType typ = Rows[row]->Types[col];
    row = rowbelow(row);
    if (row < 0)
        return true;
    TZMenuItemType typ2 = Rows[row]->Types[col];
    return (typ2 == itTitle || typ2 == itAltTitle) != (typ == itTitle || typ == itAltTitle);
}
//---------------------------------------------------------------------------
bool TZMenuRows::hasdifferentabove(int row)
{
    for (int ix = 0; ix < ColCount; ++ix)
        if (differentabove(row, ix))
            return true;
    return false;
}
//---------------------------------------------------------------------------
bool TZMenuRows::hasdifferentbelow(int row)
{
    for (int ix = 0; ix < ColCount; ++ix)
        if (differentbelow(row, ix))
            return true;
    return false;
}
//---------------------------------------------------------------------------
bool TZMenuRows::differentleft(int row, int col)
{
    if (col == 0)
        return true;
    TZMenuItemType typ = Rows[row]->Types[col];
    TZMenuItemType typ2 = Rows[row]->Types[col - 1];
    return (typ2 == itTitle || typ2 == itAltTitle) != (typ == itTitle || typ == itAltTitle);
}
//---------------------------------------------------------------------------
bool TZMenuRows::differentright(int row, int col)
{
    if (col == ColCount - 1)
        return true;
    TZMenuItemType typ = Rows[row]->Types[col];
    TZMenuItemType typ2 = Rows[row]->Types[col + 1];
    return (typ2 == itTitle || typ2 == itAltTitle) != (typ == itTitle || typ == itAltTitle);
}
//---------------------------------------------------------------------------
bool TZMenuRows::hasdifferentleft(int col)
{
    for (int ix = 0; ix < Count; ++ix)
        if (differentleft(ix, col))
            return true;
    return false;
}
//---------------------------------------------------------------------------
bool TZMenuRows::hasdifferentright(int col)
{
    for (int ix = 0; ix < Count; ++ix)
        if (differentright(ix, col))
            return true;
    return false;
}
//---------------------------------------------------------------------------
bool TZMenuRows::spanabove(int row)
{
    if (!spans)
        return false;

    int r = rowabove(row);
    int s = spans->IndexOf(r);
    if (s < 0 && r != row - 1)
    {
        s = spans->IndexOf(row - 1);
    }

    return s >= 0;
}
//---------------------------------------------------------------------------
bool TZMenuRows::spanbelow(int row)
{
    if (!spans)
        return false;

    int s = spans->IndexOf(row);
    if (s < 0)
    {
        int r = rowbelow(row);
        if (r > row + 1)
            s = spans->IndexOf(r - 1);
    }
    return s >= 0 && spans->Items[s] != lastrow();
}
//---------------------------------------------------------------------------
bool TZMenuRows::imageincolumn(int col)
{
    for (int ix = 0; ix < Count; ++ix)
    {
        if (!Rows[ix]->Visible)
            continue;
        TZMenuItemType typ = Rows[ix]->Types[col];
        if (typ == itBitmap || typ == itCheck || typ == itCheckAuto)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool TZMenuRows::incolumn(int col, TZMenuItemType type)
{
    for (int ix = 0; ix < Count; ++ix)
    {
        if (!Rows[ix]->Visible)
            continue;
        if (Rows[ix]->Types[col] == type)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TZPopupMenu::TZPopupMenu() : menu(NULL), checkmark(NULL), ftag(0)
{
    checkmark = new TAlphaImageList(data->imgMenuCheck, data->imgMenuCheckAlpha);
    rowlist = new TTemplateList<TZMenuRows, false>;
}
//---------------------------------------------------------------------------
TZPopupMenu::~TZPopupMenu()
{
    delete rowlist;
    delete checkmark;
    delete menu;
}
//---------------------------------------------------------------------------
TZMenuRow* TZPopupMenu::get_item(int ix, int subindex)
{
    if (!rowlist || rowlist->Count <= subindex)
        THROW(L"Rows not set!");

    return rowlist->Items[subindex]->Rows[ix];
}
//---------------------------------------------------------------------------
int TZPopupMenu::get_colcnt(int subindex)
{
    if (!rowlist || rowlist->Count <= subindex)
        THROW(L"Rows not set!");

    return rowlist->Items[subindex]->ColCount;
}
//---------------------------------------------------------------------------
int TZPopupMenu::get_rowcnt(int subindex)
{
    if (!rowlist || rowlist->Count <= subindex)
        THROW(L"Rows not set!");

    return rowlist->Items[subindex]->Count;
}
//---------------------------------------------------------------------------
void TZPopupMenu::Popup(TZMenuRows *rows, int x, int y, TZMenuItemClickProc callback_override)
{
    if (!menu)
    {
        menu = new TPopupMenu(NULL);
        menu->OwnerDraw = true;
    }
    else
        menu->Items->Clear();

    fcallback = callback_override;

    rowlist->Clear();
    fill_menu(rows, menu->Items);

    menu->Popup(x, y);
}
//---------------------------------------------------------------------------
void TZPopupMenu::fill_menu(TZMenuRows *rows, TMenuItem *parent)
{
    if (rowlist->Count > 255)
        THROW(L"Cannot add more than 256 submenus!");

    int sub = rowlist->Count;
    rowlist->Add(rows);

    int lastbreak = 0;
    for (int ix = 0; ix < rows->Count * rows->ColCount; ++ix)
    {
        int row = ix % rows->Count;
        int col = ix / rows->Count;

        if (!rows->Rows[row]->Visible)
            continue;

        TMenuItem *mi = new TMenuItem(menu);
        if (col != lastbreak)
        {
            mi->Break = mbBreak;
            lastbreak = col;
        }
        mi->Caption = L"* " + rows->Rows[row]->Titles[col];
        TZMenuItemType typ = rows->Rows[row]->Types[col];
        if (typ == itEmpty || typ == itTitle || typ == itAltTitle || !rows->Rows[row]->Enabled[col])
            mi->Enabled = false;

        if (rows->Rows[row]->Subrows[col] && rows->Rows[row]->Subrows[col]->Count && (typ == itSimple || typ == itBitmap))
        {
            fill_menu(rows->Rows[row]->Subrows[col], mi);
        }

        mi->Tag = (row & 0xff) | ((col & 0xff) << 8) | ((sub & 0xff) << 16);

        mi->OnMeasureItem = (!settings.oldstylepopup ? &miMeasureItem : &miMeasureItemOld);
        mi->OnAdvancedDrawItem = (TAdvancedMenuDrawItemEvent)(!settings.oldstylepopup ? &miDrawItem : &miDrawItemOld);
        mi->OnClick = &miClick;
        parent->Add(mi);

        if (rows->spanbelow(row))
        {
            mi = new TMenuItem(menu);
            mi->Enabled = false;
            mi->Tag = col | (((rowlist->Count - 1) & 0xff) << 8);
            mi->OnMeasureItem = (!settings.oldstylepopup ? &miSpanMeasureItem : &miSpanMeasureItemOld);
            mi->OnAdvancedDrawItem = (TAdvancedMenuDrawItemEvent)(!settings.oldstylepopup ? &miSpanDrawItem : &miSpanDrawItemOld);
            parent->Add(mi);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TZPopupMenu::miSpanMeasureItemOld(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height)
{
    Width = 0;
    Height = 4;
}
//---------------------------------------------------------------------------
void __fastcall TZPopupMenu::miSpanDrawItemOld(TObject *Sender, TCanvas *c, TRect &r, TOwnerDrawState State)
        //void TZPopupMenu::miSpanDrawItemOld(TObject *Sender, TCanvas *c, TRect &r, bool Selected)
{
    int col = ((TMenuItem*)Sender)->Tag & 0xff;
    int sub = (((TMenuItem*)Sender)->Tag >> 8) & 0xff;

    if (ColCount[sub] > 1 && col < ColCount[sub] - 1)
        r.Right += 2;
    if (ColCount[sub] > 1 && col > 0)
        r.Left -= 2;

    c->Brush->Color = clMenu;
    c->FillRect(r);
    c->Pen->Color = (TColor)0xe0e0e0;
    c->MoveTo(r.left, r.Bottom - 1);
    c->LineTo(r.right, r.Bottom - 1);
}
//---------------------------------------------------------------------------
void __fastcall TZPopupMenu::miMeasureItemOld(TObject *Sender, TCanvas *c, int &Width, int &Height)
{
    int row = ((TMenuItem*)Sender)->Tag & 0xff;
    int col = (((TMenuItem*)Sender)->Tag >> 8) & 0xff;
    int sub = (((TMenuItem*)Sender)->Tag >> 16) & 0xff;

    UnicodeString str = Items[row][sub]->Titles[col];

    c->Font->Name = L"Tahoma";
    c->Font->Charset = DEFAULT_CHARSET;
    c->Font->Size = 8;

    TSize s;
    GetTextExtentPoint32(c->Handle, str.c_str(), str.Length(), &s);
    Width = (s.cx < 40 ? 40 : s.cx) + 14;
    Height = 18;

    if (Items[row][sub]->Types[col] == itCheck || Items[row][sub]->Types[col] == itCheckAuto || Items[row][sub]->Types[col] == itBitmap)
        Width = s.cx + 18 + 6;

    if (Items[row][sub]->Subrows[col] && Items[row][sub]->Subrows[col]->Count && (Items[row][sub]->Types[col] == itSimple || Items[row][sub]->Types[col] == itBitmap))
        Width += 6 + checkmark->width;

    //set true width
    Width = Width - 14 - (ColCount[sub] > 1 && col > 0 ? 2 : 0) - (ColCount[sub] > 1 && col < ColCount[sub] - 1 ? 2 : 0);
}
//---------------------------------------------------------------------------
void __fastcall TZPopupMenu::miDrawItemOld(TObject *Sender, TCanvas *c, TRect &r, TOwnerDrawState State)
{
    int row = ((TMenuItem*)Sender)->Tag & 0xff;
    int col = (((TMenuItem*)Sender)->Tag >> 8) & 0xff;
    int sub = (((TMenuItem*)Sender)->Tag >> 16) & 0xff;
    if (ColCount[sub] > 1 && col < ColCount[sub] - 1)
        r.Right += 2;
    if (ColCount[sub] > 1 && col > 0)
        r.Left -= 2;

    TSize s;

    UnicodeString str = Items[row][sub]->Titles[col];

    bool Selected = State.Contains(odSelected);

    int x;
    int y;

    switch (Items[row][sub]->Types[col])
    {
    case itTitle:
        c->Brush->Color = (TColor)0x00d0d0d0;
        c->Font->Color = clBlack;
        c->Font->Name = L"Tahoma";
        c->Font->Charset = DEFAULT_CHARSET;
        c->Font->Size = 7;
        GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);

        x = (r.Width() - s.cx) / 4 + r.Left;
        y = r.Height() * 0.2 + r.Top;

        c->FillRect(r);

        c->Brush->Style = bsClear;
        break;
    case itAltTitle:
        c->Brush->Color = (TColor)0x00f0f0f0;

        c->Font->Color = clBlack;
        c->Font->Name = L"Tahoma";
        c->Font->Charset = DEFAULT_CHARSET;
        c->Font->Size = 7;
        GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);

        x = (r.Width() - s.cx) / 4 + r.Left;
        y = r.Height() * 0.2 + r.Top;
        c->FillRect(r);
        c->Brush->Style = bsClear;
        break;
    case itCheck:
    case itCheckAuto:
        c->Font->Name = L"Tahoma";
        c->Font->Charset = DEFAULT_CHARSET;
        c->Font->Color = clBlack;
        c->Font->Size = 8;
        if (!Items[row][sub]->Enabled[col] || Items[row][sub]->Types[col] == itEmpty)
        {
            c->Brush->Color = (TColor)(str.Trim().Length() ? 0x00f0f0f0 : 0x00dadada);
        }
        else if (!Selected)
            c->Brush->Color = clWhite;
        else
            c->Brush->Color = (TColor)0x00c0c0c0;

        GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);

        c->FillRect(r);
        c->Brush->Style = bsClear;

        c->Font->Name = L"Webdings";
        y = r.Top;
        if (!str.Length())
        {
            x = r.Left + (r.Width() - 16) / 2;

            c->Font->Size = 10;
            c->Font->Color = clSilver;
            ExtTextOut(c->Handle, x, y, ETO_CLIPPED, &r, L"c", 1, NULL);
            c->Font->Size = 12;
            c->Font->Color = clBlack;
            if (Items[row][sub]->Checked[col])
                ExtTextOut(c->Handle, x, y, ETO_CLIPPED, &r, L"a", 1, NULL);
        }
        else
        {
            x = r.Left + 2;
            TRect r2 = Rect(r.Left, r.Top, r.Left + 20, r.Bottom);

            c->Font->Size = 12;
            c->Font->Color = clBlack;
            if (Items[row][sub]->Checked[col])
                ExtTextOut(c->Handle, x, y, ETO_CLIPPED, &r2, L"a", 1, NULL);
        }

        c->Font->Name = L"Tahoma";
        c->Font->Size = 8;
        x = r.Left + 16 + 4;
        y = r.Height() * 0.15 + r.Top;
        break;
    default:
        c->Font->Name = L"Tahoma";
        c->Font->Charset = DEFAULT_CHARSET;
        c->Font->Color = clBlack;
        c->Font->Size = 8;
        if (!Items[row][sub]->Enabled[col] || Items[row][sub]->Types[col] == itEmpty)
        {
            c->Brush->Color = (TColor)(str.Trim().Length() ? 0x00f0f0f0 : 0x00dadada);
        }
        else if (!Selected)
            c->Brush->Color = clWhite;
        else
            c->Brush->Color = (TColor)0x00c0c0c0;

        GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);

        x = (r.Width() - s.cx) / 2 + r.Left;
        y = r.Height() * 0.15 + r.Top;
        c->FillRect(r);
        c->Brush->Style = bsClear;
        break;
    }

    if (Items[row][sub]->Subrows[col] && Items[row][sub]->Subrows[col]->Count && (Items[row][sub]->Types[col] == itSimple || Items[row][sub]->Types[col] == itBitmap))
    {
        checkmark->draw(c, r.Right - checkmark->width - 2, r.Top + (r.Height() - checkmark->height) / 2, 2);
        ZPopupList->PreventDefault();
    }

    ExtTextOutW(c->Handle, x, y, ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);

    switch (Items[row][sub]->Types[col])
    {
    case itTitle:
        break;
    case itAltTitle:
        c->Pen->Color = (TColor)0x00c0c0c0;
        c->MoveTo(r.Left + 1, r.Bottom - 1);
        c->LineTo(r.Right, r.Bottom - 1);
        break;
    default:
        c->Pen->Color = (TColor)0x00dadada;
        c->MoveTo(r.Left + 1, r.Bottom - 1);
        c->LineTo(r.Right - (col < ColCount[sub] - 1 ? 1 : 0), r.Bottom - 1);
        if (col < ColCount[sub] - 1)
            c->LineTo(r.Right - 1, r.Top);
        break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TZPopupMenu::miSpanMeasureItem(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height)
{
    Width = 1;
    Height = 2;
}
//---------------------------------------------------------------------------
void __fastcall TZPopupMenu::miSpanDrawItem(TObject *Sender, TCanvas *c, TRect &r, TOwnerDrawState State)
{
    int col = ((TMenuItem*)Sender)->Tag & 0xff;
    int sub = (((TMenuItem*)Sender)->Tag >> 8) & 0xff;

    if (ColCount[sub] > 1 && col < ColCount[sub] - 1)
        r.Right += 2;
    if (ColCount[sub] > 1 && col > 0)
        r.Left -= 2;

    c->Brush->Color = clWhite;
    c->FillRect(r);
}
//---------------------------------------------------------------------------
void __fastcall TZPopupMenu::miMeasureItem(TObject *Sender, TCanvas *c, int &Width, int &Height)
{
    int row = ((TMenuItem*)Sender)->Tag & 0xff;
    int col = (((TMenuItem*)Sender)->Tag >> 8) & 0xff;
    int sub = (((TMenuItem*)Sender)->Tag >> 16) & 0xff;

    UnicodeString str = Items[row][sub]->Titles[col];

    c->Font->Name = "Segoe UI";
    c->Font->Size = 8;
    if (c->Font->Name != "Segoe UI")
        c->Font->Name = "Tahoma";

    TSize s;
    GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);

    TZMenuItemType typ = Items[row][sub]->Types[col];
    Height = max(20 * scaleres, (int)s.cy + 2 * scaleres) + (rowlist->Items[sub]->hasdifferentabove(row) ? 2 : 0) + (rowlist->Items[sub]->hasdifferentbelow(row) ? 4 : 0);
    Width = max(18, (int)s.cx + 10 * scaleres) + (rowlist->Items[sub]->imageincolumn(col) && typ != itTitle && typ != itAltTitle ? 18 * scaleres : 0) + (rowlist->Items[sub]->differentleft(row, col) ? 1 : 0) + (typ == itTitle || typ == itAltTitle ? 2 : 0);

    if (Items[row][sub]->Subrows[col] && Items[row][sub]->Subrows[col]->Count && (Items[row][sub]->Types[col] == itSimple || Items[row][sub]->Types[col] == itBitmap))
        Width += 6 + checkmark->width;

    //set true width
    Width = Width - 14 - (ColCount[sub] > 1 && col > 0 ? 2 : 0) - (ColCount[sub] > 1 && col < ColCount[sub] - 1 ? 2 : 0);
    Width = Width + (rowlist->Items[sub]->hasdifferentleft(col) ? 2 : 0) + (rowlist->Items[sub]->hasdifferentright(col) ? 2 : 0); //+(col == 0 ? 2 : 0)+(col == ColCount[sub]-1 ? 2 : 0);
}
//---------------------------------------------------------------------------
void __fastcall TZPopupMenu::miDrawItem(TObject *Sender, TCanvas *c, TRect &r, TOwnerDrawState State)
{
    int row = ((TMenuItem*)Sender)->Tag & 0xff;
    int col = (((TMenuItem*)Sender)->Tag >> 8) & 0xff;
    int sub = (((TMenuItem*)Sender)->Tag >> 16) & 0xff;
    if (ColCount[sub] > 1 && col < ColCount[sub] - 1)
        r.Right += 2;
    if (ColCount[sub] > 1 && col > 0)
        r.Left -= 2;

    bool Selected = State.Contains(odSelected);

    c->Font->Name = "Segoe UI";
    c->Font->Size = 8;
    if (c->Font->Name != "Segoe UI")
        c->Font->Name = "Tahoma";

    UnicodeString str = Items[row][sub]->Titles[col];
    TSize s;
    GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);

    TRect rbg = r, rg;

    TZMenuItemType typ = Items[row][sub]->Types[col];

    bool toprow = rowlist->Items[sub]->differentabove(row, col);
    bool bottomrow = rowlist->Items[sub]->differentbelow(row, col);
    //bool leftcol = rowlist->Items[sub]->differentleft(row,col);
    bool rightcol = rowlist->Items[sub]->differentright(row, col);
    TColor bgcol;
    TColor brcol;
    TColor shcol;
    switch (typ)
    {
    case itTitle:
        bgcol = (TColor)0xd8d8d8;
        shcol = (TColor)0xc4c4c4;
        //brcol = (TColor)0xaeaeae;
        break;
    case itAltTitle:
        bgcol = (TColor)0xf3f3f3;
        shcol = (TColor)0xdfdfdf;
        //brcol = (TColor)0xcacaca;
        break;
    case itEmpty:
        bgcol = (TColor)0xeaeaea;
        shcol = (TColor)0xd4d4d4;
        //brcol = (TColor)0xb9b9b9;
        break;
    default:
        bgcol = clWhite;
        shcol = (TColor)0xe8e8e8;
        //brcol = (TColor)0xcecece;
        break;
    }
    brcol = (TColor)0xaeaeae;

    c->Pen->Color = brcol;
    if (col == 0)
    {
        rbg.Left++;
        c->MoveTo(r.Left, r.Top);
        c->LineTo(r.Left, r.Bottom);
    }

    if (toprow)
    {
        c->MoveTo(rbg.Left, r.Top);
        c->LineTo(r.Right - (col == ColCount[sub] - 1 ? 1 : 0), r.Top);
        rbg.Top++;
    }
    if (bottomrow)
    {
        c->MoveTo(rbg.Left, r.Bottom - 1);
        c->LineTo(r.Right - (col == ColCount[sub] - 1 ? 1 : 0), r.Bottom - 1);
        rbg.Bottom = r.Bottom - 8 * scaleres;
        if (typ == itTitle || typ == itAltTitle)
        {
            rg = Rect(rbg.Left, r.Bottom - 8 * scaleres, r.Right - (col == ColCount[sub] - 1 ? 1 : 0), r.Bottom - 1);
            GradientFillCanvas(c, bgcol, shcol, rg, gdVertical);
        }
        else
        {
            rg = Rect(rbg.Left, r.Bottom - 8 * scaleres, r.Right - (col == ColCount[sub] - 1 ? 1 : 0), r.Bottom - 1);
            GradientFillCanvas(c, bgcol, shcol, rg, gdVertical);
        }
    }
    if ((typ == itTitle || typ == itAltTitle) && col != ColCount[sub] - 1)
    {
        rbg.Right -= 2;
        c->MoveTo(r.Right - 2, r.Top + (toprow ? 1 : 0));
        c->LineTo(r.Right - 2, r.Bottom - (bottomrow ? 1 : 0));
        c->Pen->Color = (TColor)0xdadada;
        c->MoveTo(r.Right - 1, r.Top + (toprow ? 1 : 0));
        c->LineTo(r.Right - 1, r.Bottom - (bottomrow ? 1 : 0));
    }
    else if (rightcol)
    {
        rbg.Right--;
        c->MoveTo(r.Right - 1, r.Top);
        c->LineTo(r.Right - 1, r.Bottom);
    }
    c->Brush->Color = bgcol;
    c->FillRect(rbg);

    bool barright = false;
    c->Pen->Color = (TColor)0xeaeaea;
    if (typ == itSimple || typ == itCheck || typ == itCheckAuto || typ == itBitmap)
    {
        if (col != ColCount[sub] - 1)
        { //vertical bar on right
            TZMenuItemType typ2 = Items[row][sub]->Types[col + 1];
            if (typ2 == itSimple || typ2 == itCheck || typ2 == itCheckAuto || typ2 == itBitmap)
            {
                barright = true;
                int top = r.Top;
                int btm = r.Bottom;
                if (row == rowlist->Items[sub]->firstrow() || rowlist->Items[sub]->spanabove(row))
                    top += 4 * scaleres;
                if (row == rowlist->Items[sub]->lastrow() || rowlist->Items[sub]->spanbelow(row))
                    btm -= 4 * scaleres;
                c->MoveTo(r.Right - 1, top);
                c->LineTo(r.Right - 1, btm);
            }
        }

        if (row != rowlist->Items[sub]->lastrow() && !rowlist->Items[sub]->spanbelow(row))
        { //vertical bar on bottom
            TZMenuItemType typ2 = Items[rowlist->Items[sub]->rowbelow(row)][sub]->Types[col];
            if (typ2 == itSimple || typ2 == itCheck || typ2 == itCheckAuto || typ2 == itBitmap)
            {
                int lft = r.Left;
                int rgh = r.Right;
                if (col == 0 || rowlist->Items[sub]->differentleft(row, col) || row < rowlist->Items[sub]->Count - 1 && rowlist->Items[sub]->differentleft(row + 1, col))
                    lft += 4 * scaleres;
                if (col == ColCount[sub] - 1)
                    rgh -= 5 * scaleres;
                c->MoveTo(lft, r.Bottom - 1);
                c->LineTo(rgh, r.Bottom - 1);
            }
        }
    }

    rbg = r;
    if (rowlist->Items[sub]->hasdifferentleft(col))
        rbg.Left += 1 + (col == 0 ? 1 : 0);
    if (rowlist->Items[sub]->hasdifferentright(col))
        rbg.Right -= 2;
    if (rowlist->Items[sub]->hasdifferentabove(row))
        rbg.Top += 2;
    if (rowlist->Items[sub]->hasdifferentbelow(row))
        rbg.Bottom -= 2;

    if (col < ColCount[sub] - 1)
        rbg.Right -= barright ? 2 : 1;
    if (col > 0 && !rowlist->Items[sub]->differentleft(row, col))
        rbg.Left += 1;

    if (Selected && typ != itTitle && typ != itAltTitle && typ != itEmpty && Items[row][sub]->Enabled[col])
    {
        TThemeServices *t = ThemeServices();
        if (MatchVersion(6, 0) && t && t->ThemesAvailable && t->ThemesEnabled)
        {
            TThemedElementDetails ted;
            ted.Element = teMenu;
            ted.Part = MENU_POPUPITEM;
            ted.State = MPI_HOT;
            t->DrawElement(c->Handle, ted, rbg);
        }
        else
        {
            c->Pen->Color = (TColor)0xecd1ae;
            c->MoveTo(rbg.Left, rbg.Top);
            c->LineTo(rbg.Right - 1, rbg.Top);
            c->LineTo(rbg.Right - 1, rbg.Bottom - 1);
            c->LineTo(rbg.Left, rbg.Bottom - 1);
            c->LineTo(rbg.Left, rbg.Top);
            InflateRect(&rbg, -1, -1);
            GradientFillCanvas(c, (TColor)0xfdeace, (TColor)0xfffbef, rbg, gdVertical);
            InflateRect(&rbg, 1, 1);
        }
    }

    if (Items[row][sub]->Types[col] == itCheck || Items[row][sub]->Types[col] == itCheckAuto)
    {
        rg = rbg;
        if (str.Length())
        {
            rg.Left += 2;
            rg.Right = rg.Left + 16;
            rg.Top += (rbg.Height() - 16) / 2;
            rg.Bottom = rg.Top + 16;
        }
        int cix = Items[row][sub]->Checked[col] ? 0 : 1;
        checkmark->draw(c, rg.Left + (rg.Width() - checkmark->width) / 2, rg.Top + (rg.Height() - checkmark->height) / 2, cix);
    }

    if (Items[row][sub]->Subrows[col] && Items[row][sub]->Subrows[col]->Count && (Items[row][sub]->Types[col] == itSimple || Items[row][sub]->Types[col] == itBitmap))
    {
        checkmark->draw(c, rbg.Right - checkmark->width - 2, rbg.Top + (rbg.Height() - checkmark->height) / 2, 2);
        ZPopupList->PreventDefault();
    }

    if (Items[row][sub]->Types[col] != itEmpty)
    {
        c->Brush->Style = bsClear;
        c->Font->Color = Items[row][sub]->Enabled[col] ? clBlack : clSilver;
        ExtTextOutW(c->Handle, rbg.Left + 5*scaleres + (rowlist->Items[sub]->imageincolumn(col) && typ != itTitle && typ != itAltTitle ? 18*scaleres : 0), rbg.Top + (rbg.Height() - s.cy) / 2, ETO_CLIPPED, &rbg, str.c_str(), str.Length(), 0);
    }

}
//---------------------------------------------------------------------------
void __fastcall TZPopupMenu::miClick(TObject *Sender)
{
    int row = ((TMenuItem*)Sender)->Tag & 0xff;
    int col = (((TMenuItem*)Sender)->Tag >> 8) & 0xff;
    int sub = (((TMenuItem*)Sender)->Tag >> 16) & 0xff;

    TZMenuItemType typ = Items[row][sub]->Types[col];

    if (typ == itCheckAuto)
        Items[row][sub]->Checked[col] = !Items[row][sub]->Checked[col];

    if (fcallback)
        fcallback(rowlist->Items[sub], col, row);
    else if (Items[row][sub]->Callbacks[col])
        Items[row][sub]->Callbacks[col](rowlist->Items[sub], col, row);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(211);
#endif
