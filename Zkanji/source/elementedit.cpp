//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Math.h>
#include <IniFiles.hpp>

#include "elementedit.h"
#include "numedit.h"
#include "kanjielement.h"
#include "collection.h"
#include "mouseevent.h"
#include "module.h"
#include "kanji.h"
#include "settings.h"
#include "elementsettings.h"
#include "mouseevent.h"
#include "elementselector.h"
#include "module.h"
#include "msgform.h"
#include "recobj.h"
#include "angle.h"
#include "localeinfo.h"
#include "numinput.h"
#include "elementpoints.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(34);
#endif

TfEditElem *fEditElem;

#define HIT_LEFT   (1 << 0)
#define HIT_TOP    (1 << 1)
#define HIT_RIGHT  (1 << 2)
#define HIT_BOTTOM (1 << 3)

#define PADDING    500.0

extern TColor scolors[32];
extern bool hasstrokes;
extern bool hasrecdata;
extern wchar_t Stroke_Filename[];

//---------------------------------------------------------------------------
TElementGrid::TElementGrid(TZPaintBox *pb, int aboxsize, void(__closure *acallback)(int elem)) :
        fcount(0), fitems(NULL), fcallback(acallback), fshowval(false), inherited(pb, aboxsize, (TFuncPtr)NULL)
{
}
//---------------------------------------------------------------------------
TElementGrid::~TElementGrid()
{
    delete[] fitems;
}
//---------------------------------------------------------------------------
int TElementGrid::get_count()
{
    return fcount;
}
//---------------------------------------------------------------------------
void TElementGrid::DoDrawCell(TCanvas *c, const TRect &r, int acell)
{
    c->Brush->Color = (TColor)colorsettings.background;
    c->Pen->Color = (TColor)colorsettings.maintext;

    if (acell == mousecell)
        c->Brush->Color = coloravg((TColor)0xe6fbfb, c->Brush->Color, 0.5);
    //DARKEN(c->Brush->Color,0xf6fdfd);

    c->FillRect(r);

    //c->Pen->Width = 2;
    elements->Draw(Rect(r.Left + 2, r.Top + 2, r.Right - 3, r.Bottom - 3), c->Handle, 1.6, c->Pen->Color, fitems[acell], 0);

    c->Pen->Width = 1;
    c->Pen->Color = (TColor)colorsettings.gridline;
    c->MoveTo(r.Right, r.Top);
    c->LineTo(r.Right, r.Bottom);
    c->LineTo(r.Left - 1, r.Bottom);

    if (fshowval)
    {
        c->Brush->Style = bsClear;
        TSize s;
        UnicodeString str = IntToStr(fitems[acell] + 1);
        GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);
        c->Font->Color = (TColor)colorsettings.wordtype;
        c->TextOutW(r.Right - s.cx - 2, r.Bottom - s.cy - 1, str);
        c->Brush->Style = bsSolid;
    }
}
//---------------------------------------------------------------------------
void TElementGrid::set_showval(bool val)
{
    if (val == fshowval)
        return;
    fshowval = val;
    Redraw();
}
//---------------------------------------------------------------------------
void TElementGrid::set_items(const word *aitems, int acount)
{
    word *tmp = fitems;
    if (acount)
    {
        fitems = new word[acount];
        memcpy(fitems, aitems, sizeof(word)*acount);
    }
    else
        fitems = 0;

    delete[] tmp;
    fcount = acount;

    changed(0);
}
//---------------------------------------------------------------------------
void TElementGrid::DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y)
{
    inherited::DoMouseDown(Button, Shift, x, y);

    if (!fcallback || Button != mbLeft)
        return;

    int acell;
    if ((acell = CellAtPos(Point(x, y))) >= 0)
        fcallback(fitems[acell]);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TVariantGrid::TVariantGrid(TZPaintBox *apb, int aboxsize, bool showpos, bool showval, TEVCallBack acallback, TEVCallBackR agetitem) : fcount(0), callelem(ElementVariant(-1, 255)), fcallback(acallback), fgetitem(agetitem), fshowpos(showpos), fshowval(showval), inherited(apb, aboxsize, false, callback)
{
}
//---------------------------------------------------------------------------
TVariantGrid::~TVariantGrid()
{
    //delete[]   fitems;
    //fitems = 0;
}
//---------------------------------------------------------------------------
void TVariantGrid::empty()
{
    fcount = 0;
    callelem = ElementVariant(-1, 255);
    changed(0);
}
//---------------------------------------------------------------------------
TElementVariant TVariantGrid::getitem(int apos)
{
    int elem;
    byte var;
    fgetitem(apos, elem, var);
    return ElementVariant(elem, var);
}
//---------------------------------------------------------------------------
void TVariantGrid::set_items(int acount)
{
    fcount = acount;
    if (fcount)
        callelem = getitem(0);
    else
        callelem = ElementVariant(-1, 0);

    changed(0);

    if (fcallback && fshowpos)
    {
        if (fcount)
        {
            TElementVariant item = getitem(0);
            fcallback(item.element, item.variant);
        }
        else
            fcallback(-1, 0);
    }
}
//---------------------------------------------------------------------------
void TVariantGrid::DoDrawCell(TCanvas *c, const TRect &r, int acell)
{
    c->Brush->Color = (TColor)colorsettings.background;
    c->Pen->Color = (TColor)colorsettings.maintext;

    if (fshowpos && selected[acell])
        c->Brush->Color = focused ? (TColor)colorsettings.highlight : coloravg((TColor)colorsettings.highlight, (TColor)colorsettings.background, 0.8);

    if (acell == mousecell)
        c->Brush->Color = coloravg((TColor)0xe6fbfb, c->Brush->Color, 0.5);

    c->FillRect(r);

    c->Pen->Width = 2;
    TElementVariant item = getitem(acell);
    elements->Draw(Rect(r.Left + 2, r.Top + 2, r.Right - 3, r.Bottom - 3), c->Handle, 1.8, c->Pen->Color, item.element, item.variant);

    c->Pen->Width = 1;
    c->Pen->Color = (TColor)colorsettings.gridline;
    c->MoveTo(r.Right, r.Top);
    c->LineTo(r.Right, r.Bottom);
    c->LineTo(r.Left - 1, r.Bottom);

    if (fshowval)
    {
        c->Brush->Style = bsClear;
        TSize s;
        UnicodeString str = IntToStr(item.element + 1);
        GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);
        c->Font->Color = (TColor)colorsettings.wordtype;
        c->TextOutW(r.Right - s.cx - 2, r.Bottom - s.cy - 1, str);
        c->Brush->Style = bsSolid;
    }
}
//---------------------------------------------------------------------------
void TVariantGrid::callback()
{
    if (pos >= 0 && fcount > pos)
    {
        TElementVariant item = getitem(pos);

        if (callelem != item)
        {
            callelem = item;
            if (item.element >= 0)
                fcallback(item.element, item.variant);
        }
    }
}
//---------------------------------------------------------------------------
void TVariantGrid::DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y)
{
    inherited::DoMouseDown(Button, Shift, x, y);

    if (!fcallback || Button != mbLeft)
        return;

    int acell;
    if ((acell = CellAtPos(Point(x, y))) >= 0)
    {
        callelem = getitem(acell);
        if (callelem.element >= 0)
            fcallback(callelem.element, callelem.variant);
    }
}
//---------------------------------------------------------------------------
void TVariantGrid::set_showval(bool val)
{
    if (val == fshowval)
        return;
    fshowval = val;
    Redraw();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TPointListGrid::TPointListGrid(TZPaintBox *apb, int aboxsize, TFuncPtr acallback) : inherited(apb, aboxsize, false, acallback)
{
}
TPointListGrid::~TPointListGrid()
{;
}
//---------------------------------------------------------------------------
void TPointListGrid::DoDrawCell(TCanvas *c, const TRect &r, int acell)
{
    TPointListEx *p = GetPoints(acell);
    c->Brush->Color = (TColor)colorsettings.background;

    if (pos == acell || selected[acell])
        c->Brush->Color = focused ? (TColor)colorsettings.highlight : coloravg((TColor)colorsettings.highlight, (TColor)colorsettings.background, 0.8);

    if (acell == mousecell)
        c->Brush->Color = coloravg((TColor)0xe6fbfb, c->Brush->Color, 0.5);
    c->FillRect(r);
    c->Pen->Width = 1;
    c->Pen->Color = (TColor)colorsettings.gridline;
    c->MoveTo(r.Right, r.Top);
    c->LineTo(r.Right, r.Bottom);
    c->LineTo(r.Left - 1, r.Bottom);

    TRect rr = r;
    InflateRect(&rr, -5, -5);
    c->Pen->Width = 2;
    c->Pen->Color = (TColor)colorsettings.maintext;
    p->draw(c, rr);

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TStrokeModelGrid::TStrokeModelGrid(TZPaintBox *apb, int aboxsize, TFuncPtr acallback) : list(NULL), inherited(apb, aboxsize, acallback)
{
    multiselect = true;
}
//---------------------------------------------------------------------------
TStrokeModelGrid::~TStrokeModelGrid()
{;
}
//---------------------------------------------------------------------------
TPointListEx* TStrokeModelGrid::GetPoints(int index)
{
    return list ? list->Items[index] : NULL;
}
//---------------------------------------------------------------------------
int TStrokeModelGrid::get_count()
{
    return list ? list->Count : 0;
}
//---------------------------------------------------------------------------
void TStrokeModelGrid::set_models(TStrokeModels *amodels)
{
    list = amodels;
    changed(0);
}
//---------------------------------------------------------------------------
void TStrokeModelGrid::DoDrawCell(TCanvas *c, const TRect &r, int acell)
{
    inherited::DoDrawCell(c, r, acell);
    if (!fnumproc)
        return;

    UnicodeString str = fnumproc(acell);
    if (!str.Length())
        return;

    c->Brush->Style = bsClear;
    TSize s;
    GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);
    c->Font->Color = (TColor)colorsettings.wordtype;
    c->TextOutW(r.Right - s.cx - 2, r.Bottom - s.cy - 1, str);
    c->Brush->Style = bsSolid;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TfEditElem::TUndoItems::TUndoItems(getvnameproc agetvname, setvnameproc asetvname) : elem(NULL), copy(NULL), fpos(-1), getvname(agetvname), setvname(asetvname), inherited()
{
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::Clear()
{
    inherited::Clear();
    if (copy)
        FreeCopy(copy);
    copy = NULL;
    fpos = -1;
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::FreeItem(TUndoItem *item, int ix)
{
    FreeCopy(item);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::FreeCopy(TUndoItem *acopy)
{
    if (!acopy)
        return;

    TKanjiVariant *var = &acopy->variant;
    if (!var->standalone)
        delete[] var->positions;
    else
    {
        for (int j = 0; j < var->strokecount; ++j)
            delete[] var->strokes[j].points;
        delete[] var->strokes;
    }

    delete acopy;
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::SetItem(TKanjiElement *aelem, byte avariant)
{
    Clear();
    elem = aelem;
    variant = avariant;
    copy = CreateCopy(uiNone);
    for (int ix = 0; ix < 4; ++ix)
        if (elem->parts[ix] >= 0)
            partpos[ix] = ix;
        else
            partpos[ix] = -1;
    fEditElem->setundo();
}
//---------------------------------------------------------------------------
bool TfEditElem::TUndoItems::HasType(TUndoItemType type)
{
    for (int ix = 0; ix <= fpos; ++ix)
        if (Items[ix]->type == type)
            return true;
    return false;
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::Add(TUndoItemType type)
{
    while (Count - 1 > fpos)
        Delete(Count - 1);

    inherited::Add(CreateCopy(type));
    fpos++;
    fEditElem->setundo();
}
//---------------------------------------------------------------------------
TfEditElem::TUndoItem* TfEditElem::TUndoItems::CreateCopy(TUndoItemType type)
{
    TUndoItem *item = new TUndoItem;
    item->type = type;
    item->owner = elem->owner;
    item->pattern = elem->pattern;
    item->varname = getvname();
    memcpy(item->parts, elem->parts, sizeof(short)*4);
    memcpy(item->partpos, partpos, sizeof(char)*4);
    memcpy(&item->variant, &elem->variants[variant], sizeof(TKanjiVariant));
    if (item->variant.standalone)
    {
        item->variant.strokes = new TElementStroke[item->variant.strokecount];
        for (int ix = 0; ix < item->variant.strokecount; ++ix)
        {
            item->variant.strokes[ix].pointcount = elem->variants[variant].strokes[ix].pointcount;
            item->variant.strokes[ix].points = new TElementPoint[item->variant.strokes[ix].pointcount];
            memcpy(item->variant.strokes[ix].points, elem->variants[variant].strokes[ix].points, sizeof(TElementPoint)*item->variant.strokes[ix].pointcount);
            item->variant.strokes[ix].tips = elem->variants[variant].strokes[ix].tips;
        }
    }
    else
    {
        item->variant.positions = new TPartPosition[4];
        memcpy(item->variant.positions, elem->variants[variant].positions, sizeof(TPartPosition)*4);
    }
    return item;
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::PartRemoved(byte part)
{
    memmove(&partpos[part], &partpos[part + 1], sizeof(char)*(3 - part));
    partpos[3] = -1;
    Add(uiPartRemove);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::PartAdded()
{
    Add(uiPartAdd);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::PartChanged()
{
    Add(uiPart);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::VariantChanged()
{
    Add(uiVariant);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::PositionChanged()
{
    Add(uiPosition);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::StrokeChanged()
{
    Add(uiStroke);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::PatternChanged()
{
    Add(uiPattern);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::StandaloneChanged()
{
    Add(uiStandalone);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::CentreChanged()
{
    Add(uiCentre);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::TipChanged()
{
    Add(uiTip);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::FixChanged()
{
    Add(uiFix);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::OwnerChanged()
{
    Add(uiOwner);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::VarnameChanged()
{
    Add(uiVariantName);
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::Undo()
{
    if (fpos < 0)
        return;

    fpos--;

    AssignCopy();
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::Redo()
{
    if (fpos >= Count - 1)
        return;

    fpos++;

    AssignCopy();
}
//---------------------------------------------------------------------------
void TfEditElem::TUndoItems::AssignCopy()
{
    TUndoItem *item = fpos >= 0 ? Items[fpos] : copy;
    elem->owner = item->owner;
    elem->pattern = item->pattern;
    memcpy(elem->parts, item->parts, sizeof(short)*4);
    memcpy(partpos, item->partpos, sizeof(char)*4);

    setvname(item->varname);

    delete[] elem->variants[variant].positions;

    if (elem->variants[variant].standalone)
        for (int ix = 0; ix < elem->variants[variant].strokecount; ++ix)
            delete[] elem->variants[variant].strokes[ix].points;

    delete[] elem->variants[variant].strokes;

    TKanjiVariant *v = &elem->variants[variant];
    memcpy(v, &item->variant, sizeof(TKanjiVariant));
    if (v->standalone)
    {
        v->strokes = new TElementStroke[v->strokecount];
        for (int ix = 0; ix < v->strokecount; ++ix)
        {
            v->strokes[ix].pointcount = item->variant.strokes[ix].pointcount;
            v->strokes[ix].points = new TElementPoint[v->strokes[ix].pointcount];
            memcpy(v->strokes[ix].points, item->variant.strokes[ix].points, sizeof(TElementPoint)*v->strokes[ix].pointcount);
            v->strokes[ix].tips = item->variant.strokes[ix].tips;
        }
    }
    else
    {
        v->positions = new TPartPosition[4];
        memcpy(v->positions, item->variant.positions, sizeof(TPartPosition)*4);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall strideconvert(const TPoint &diff, TPoint &newpoint, bool allowdiag, int diagthreshold = 0)
{
    enum TStrideDirection { sdNone, sdHorz, sdVert, sdDiag, sdDiagB };

    newpoint = Point(0, 0);
    TStrideDirection ways;

    if (!diff.x)
        ways = sdVert;
    else if (!diff.y)
        ways = sdHorz;
    else if (allowdiag && MAX(abs(diff.x), abs(diff.y)) >= diagthreshold && sgn(diff.y) == sgn(diff.x) && (double)diff.y / diff.x < 1.5 && (double)diff.x / diff.y < 1.5)
        ways = sdDiag;
    else if (allowdiag && MAX(abs(diff.x), abs(diff.y)) >= diagthreshold && sgn(diff.y) != sgn(diff.x) && (double)diff.x / diff.y > -1.5 && (double)diff.y / diff.x > -1.5)
        ways = sdDiagB;
    else if (abs(diff.x) < abs(diff.y))
        ways = sdVert;
    else
        ways = sdHorz;

    if (ways == sdVert)
    {
        newpoint.x = -diff.x;
    }
    else if (ways == sdHorz)
    {
        newpoint.y = -diff.y;
    }
    else if (ways == sdDiag)
    {
        newpoint.x = (diff.y - diff.x) / 2;
        newpoint.y = (diff.x - diff.y) / 2;
    }
    else if (ways == sdDiagB)
    {
        newpoint.x = -(diff.x + diff.y) / 2;
        newpoint.y = -(diff.x + diff.y) / 2;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TfEditElem::TPartList::TPartList() : felem(NULL), felemindex(-1), fvarindex(-1), fthres(100), fmoving(false), fmovepart(-1), selectpt(Point(99999, 99999))//,inherited()
{
}
//---------------------------------------------------------------------------
TfEditElem::TPartList::~TPartList()
{
    Clear();
}
//---------------------------------------------------------------------------
void TfEditElem::TPartList::Clear()
{
    felem = 0;
    felemindex = -1;
    fvarindex = -1;
}
//---------------------------------------------------------------------------
void TfEditElem::TPartList::Add(TKanjiElement *aelem, short aindex, byte avar)
{
    felemindex = aindex;
    fvarindex = avar;
    memset(selected, 0, sizeof(bool)*4);
    felem = aelem;
}
//---------------------------------------------------------------------------
void TfEditElem::TPartList::Deselect()
{
    memset(selected, 0, sizeof(bool)*4);
}
//---------------------------------------------------------------------------
void TfEditElem::TPartList::draw(TCanvas *c, const TRect &r)
{
    for (int ix = 0; ix < Count; ++ix)
        drawpart(c, r, ix);
}
//---------------------------------------------------------------------------
void TfEditElem::TPartList::drawpart(TCanvas *c, const TRect &r, int p)
{
    c->Pen->Width = 1;

    TDoubleRect area;

    TKanjiElement *ke = elements->Items[felem->parts[p]];
    TKanjiVariant *kv = &ke->variants[position[p]->variant];

    area.Left = r.Left + (double)(position[p]->x - PADDING) * r.Width() / 10000.0;
    area.Top = r.Top + (double)(position[p]->y - PADDING) * r.Height() / 10000.0;
    area.Right = area.Left + (double)(position[p]->width + PADDING * 2) * r.Width() / 10000.0;
    area.Bottom = area.Top + (double)(position[p]->height + PADDING * 2) * r.Height() / 10000.0;

    c->Pen->Color = part[p] ? clMaroon : clNavy;
    if (!part[p])
    {
        c->Pen->Style = psDash;
        c->Brush->Style = bsClear;
    }
    c->MoveTo(area.Left, area.Top);
    c->LineTo(area.Right, area.Top);
    c->LineTo(area.Right, area.Bottom);
    c->LineTo(area.Left, area.Bottom);
    c->LineTo(area.Left, area.Top);
    c->Pen->Style = psSolid;
    c->Brush->Style = bsSolid;

    area.Left = (double)r.Left + ((double)position[p]->x - (double)kv->x * position[p]->width / kv->width) * r.Width() / 10000.0;
    area.Top = (double)r.Top + ((double)position[p]->y - (double)kv->y * position[p]->height / kv->height) * r.Height() / 10000.0;
    area.Right = (double)area.Left + (double)position[p]->width * r.Width() / kv->width;
    area.Bottom = (double)area.Top + (double)position[p]->height * r.Height() / kv->height;

    //c->Pen->Width = r.Width()/50;
    //c->Pen->Color = part[p] ? clBlue : clBlack;

    elements->Draw(area.rect(), c->Handle, r.Width() / 50, part[p] ? clBlue : clBlack, felem->parts[p], position[p]->variant);
}
//---------------------------------------------------------------------------
double TfEditElem::TPartList::partdistance(int x, int y, byte apart)
{
    TRect r = Rect(position[apart]->x - PADDING, position[apart]->y - PADDING, position[apart]->x + position[apart]->width + PADDING, position[apart]->y + position[apart]->height + PADDING);
    if (x >= r.Left && x < r.Right && y >= r.Top && y < r.Bottom)
        return 0.0;

    return sqrt((x >= r.Left && x < r.Right ? 0 : pow(x < r.Left ? r.Left - x : x - r.Right, 2)) + (y >= r.Top && y < r.Bottom ? 0 : pow(y < r.Top ? r.Top - y : y - r.Bottom, 2)));
}
//---------------------------------------------------------------------------
void TfEditElem::TPartList::onpart(const TRect &r, int x, int y, bool alternate, int &apart)
{
    apart = -1;

    int dist = 99999;
    x = (double)(x - r.Left) * 10000.0 / r.Width();
    y = (double)(y - r.Top) * 10000.0 / r.Height();

    for (int ix = Count - 1; ix >= 0; --ix)
    {
        int tmp = partdistance(x, y, ix) * r.Width() / 10000.0;
        if (tmp > fthres)
            continue;

        if (tmp < dist && apart < 0 || (!alternate && (part[ix] || tmp < dist && !part[apart]) || alternate && part[apart]))
        {
            apart = ix;
            dist = tmp;
        }
    }
}
//---------------------------------------------------------------------------
bool TfEditElem::TPartList::select(const TRect &r, int x, int y, bool alternate, bool add)
{
    bool sel = selcount;

    int p;
    onpart(r, x, y, alternate && abs(selectpt.x - x) < 3 && abs(selectpt.y - y) < 3 && !add, p);

    selectpt = Point(x, y);

    if (p < 0)
    {
        if (!add)
        {
            Deselect();
            return sel;
        }
        return false;
    }

    if (add)
    {
        part[p] = !part[p];
        return true;
    }
    else if (part[p])
        return false;

    Deselect();
    part[p] = true;

    return true;
}
//---------------------------------------------------------------------------
bool TfEditElem::TPartList::select(const TRect &r, const TRect &selr, bool add)
{
    bool changed = false;

    if (!add && selcount)
    {
        Deselect();
        changed = true;
    }

    for (int ix = 0; ix < Count; ++ix)
    {
        TDoubleRect area = DoubleRect(position[ix]->x - PADDING, position[ix]->y - PADDING, position[ix]->x + position[ix]->width + PADDING, position[ix]->y + position[ix]->height + PADDING);
        area.Left = r.Left + area.Left * r.Width() / 10000.0;
        area.Top = r.Top + area.Top * r.Height() / 10000.0;
        area.Right = r.Left + area.Right * r.Width() / 10000.0;
        area.Bottom = r.Top + area.Bottom * r.Height() / 10000.0;

        if (RectOverlap(area.rect(), selr) && !part[ix])
        {
            part[ix] = true;
            changed = true;
        }
    }

    return changed;
}
//---------------------------------------------------------------------------
bool TfEditElem::TPartList::nearside(const TRect &r, int x, int y, TRect &ahitrect, byte &aside)
{
    int p;
    aside = 0;

    onpart(r, x, y, false, p);
    if (p < 0)
        return false;

    ahitrect = Rect(position[p]->x - PADDING, position[p]->y - PADDING, position[p]->x + position[p]->width + PADDING, position[p]->y + position[p]->height + PADDING);
    ahitrect.Left = r.Left + (double)ahitrect.Left * r.Width() / 10000.0;
    ahitrect.Top = r.Top + (double)ahitrect.Top * r.Height() / 10000.0;
    ahitrect.Right = r.Left + (double)ahitrect.Right * r.Width() / 10000.0;
    ahitrect.Bottom = r.Top + (double)ahitrect.Bottom * r.Height() / 10000.0;

    bool inside = x >= ahitrect.Left + fthres && x <= ahitrect.Right - fthres && y >= ahitrect.Top + fthres && y <= ahitrect.Bottom - fthres;
    if (inside)
        return true;

    if (y >= ahitrect.Top - fthres && y <= ahitrect.Bottom + fthres)
    {
        if (/*x >= ahitrect.Left-fthres &&*/x <= ahitrect.Left + fthres)
            aside |= HIT_LEFT;
        if (x >= ahitrect.Right - fthres/*&& x <= ahitrect.Right+fthres*/)
            aside |= HIT_RIGHT;
    }

    if (x >= ahitrect.Left - fthres && x <= ahitrect.Right + fthres)
    {
        if (/*y >= ahitrect.Top-fthres &&*/y <= ahitrect.Top + fthres)
            aside |= HIT_TOP;
        if (y >= ahitrect.Bottom - fthres/*&& y <= ahitrect.Bottom+fthres*/)
            aside |= HIT_BOTTOM;
    }

    if (aside && selcount > 1 && part[p])
        aside = 0;

    return true;
}
//---------------------------------------------------------------------------
void TfEditElem::TPartList::startmove(const TRect &r, int x, int y)
{
    fmoving = true;
    TRect dummy;
    if (selcount == 1)
        nearside(r, x, y, dummy, fmoveside);
    else
        fmoveside = 0;
    fmovestart = Point((double)(x - r.Left) * 10000.0 / r.Width(), (double)(y - r.Top) * 10000.0 / r.Height());
    fmovepos = Point(0, 0);
}
//---------------------------------------------------------------------------
void TfEditElem::TPartList::endmove()
{
    fmoving = false;
    fmovepart = -1;
    fmoveside = 0;
}
//---------------------------------------------------------------------------
void TfEditElem::TPartList::move(const TRect &r, int x, int y, bool striding)
{
    if (!fmoving || !selcount)
        THROW(L"We are not moving!");

    x = (double)(x - r.Left) * 10000.0 / r.Width();
    y = (double)(y - r.Top) * 10000.0 / r.Height();

    if (x != fmovestart.x || y != fmovestart.y)
        selectpt = Point(99999, 99999);

    int pl = 99999;
    int pr = -99999;
    int pt = 99999;
    int pb = -99999;

    if (fmoveside)
    {
        int p;
        for (p = 0; p < Count && !part[p]; ++p);

        TRect area = Rect(position[p]->x - PADDING, position[p]->y - PADDING, position[p]->x + position[p]->width + PADDING, position[p]->y + position[p]->height + PADDING);

        if (fmoveside & HIT_LEFT)
            x = MAX(-PADDING, MIN(x, (int)area.Right - 1000));
        if (fmoveside & HIT_TOP)
            y = MAX(-PADDING, MIN(y, (int)area.Bottom - 1000));
        if (fmoveside & HIT_RIGHT)
            x = MIN(10000 + PADDING, MAX((int)area.Left + 1000, x));
        if (fmoveside & HIT_BOTTOM)
            y = MIN(10000 + PADDING, MAX((int)area.Top + 1000, y));

        if (striding && ((fmoveside & (HIT_LEFT | HIT_RIGHT)) || (fmoveside & (HIT_TOP | HIT_BOTTOM))))
        {
            if (fmoveside & HIT_LEFT)
            {
                if (fmoveside & HIT_TOP)
                {
                    x = MAX(-PADDING, (int)area.Right - MAX(((int)area.Right - x), ((int)area.Bottom - y)));
                    y = area.Bottom - (area.Right - x);
                    if (y < -PADDING)
                    {
                        x -= PADDING + y;
                        y = -PADDING;
                    }
                }
                else
                {
                    x = MAX(-PADDING, (int)area.Right - MAX(((int)area.Right - x), (y - (int)area.Top)));
                    y = area.Top + (area.Right - x);
                    if (y > (10000 + PADDING))
                    {
                        x += y - (10000 + PADDING);
                        y = (10000 + PADDING);
                    }
                }
            }
            else
            {
                if (fmoveside & HIT_TOP)
                {
                    x = MIN((10000 + PADDING), (int)area.Left + MAX((x - (int)area.Right), ((int)area.Bottom - y)));
                    y = area.Bottom - (x - area.Left);
                    if (y < -PADDING)
                    {
                        x += PADDING + y;
                        y = -PADDING;
                    }
                }
                else
                {
                    x = MIN((10000 + PADDING), (int)area.Left + MAX((x - (int)area.Right), (y - (int)area.Top)));
                    y = area.Top + (x - area.Left);
                    if (y > (10000 + PADDING))
                    {
                        x -= y - (10000 + PADDING);
                        y = (10000 + PADDING);
                    }
                }
            }
        }

        if (fmoveside & HIT_LEFT)
            area.Left = x;
        if (fmoveside & HIT_TOP)
            area.Top = y;
        if (fmoveside & HIT_RIGHT)
            area.Right = x;
        if (fmoveside & HIT_BOTTOM)
            area.Bottom = y;

        position[p]->x = area.Left + PADDING;
        position[p]->y = area.Top + PADDING;
        position[p]->width = area.Width() - PADDING * 2;
        position[p]->height = area.Height() - PADDING * 2;

    }
    else
    {
        if (striding)
        {
            TPoint p;
            strideconvert(Point(x, y) - fmovestart, p, false);
            p += Point(x, y);
            x = p.x;
            y = p.y;
        }

        for (int ix = 0; ix < Count; ++ix)
        {
            if (!part[ix])
                continue;

            position[ix]->x -= fmovepos.x;
            position[ix]->y -= fmovepos.y;

            pl = MIN(pl, position[ix]->x);
            pt = MIN(pt, position[ix]->y);
            pr = MAX(pr, position[ix]->x + position[ix]->width);
            pb = MAX(pb, position[ix]->y + position[ix]->height);
        }

        fmovepos = Point(x - fmovestart.x, y - fmovestart.y);
        fmovepos.x = MAX(-pl, MIN(10000 - pr, (int)fmovepos.x));
        fmovepos.y = MAX(-pt, MIN(10000 - pb, (int)fmovepos.y));

        for (int ix = 0; ix < Count; ++ix)
        {
            if (!part[ix])
                continue;

            position[ix]->x += fmovepos.x;
            position[ix]->y += fmovepos.y;
        }
    }
}
//---------------------------------------------------------------------------
int TfEditElem::TPartList::get_count()
{
    if (!felem || felemindex < 0)
        return 0;
    int ix = 0;
    for (; ix < 4 && felem->parts[ix] >= 0; ++ix);
    return ix;
}
//---------------------------------------------------------------------------
TPartPosition* TfEditElem::TPartList::get_pos(byte ix)
{
    return &felem->variants[fvarindex].positions[ix];
}
//---------------------------------------------------------------------------
bool TfEditElem::TPartList::get_partselected(int ix)
{
    return selected[ix];
}
//---------------------------------------------------------------------------
void TfEditElem::TPartList::set_partselected(int ix, bool val)
{
    selected[ix] = val;
}
//---------------------------------------------------------------------------
int TfEditElem::TPartList::get_selcount()
{
    int fsel = 0;
    for (int ix = 0; ix < Count; ++ix)
        if (part[ix])
            ++fsel;
    return fsel;
}
//---------------------------------------------------------------------------
void TfEditElem::TPartList::alignpart(TPartAlignment align)
{
    TRect r = Rect(99999, 99999, -99999, -99999);
    for (int ix = Count - 1; ix >= 0; --ix)
    {
        if (!selected[ix])
            continue;

        r.Left = MIN((int)r.Left, felem->variants[fvarindex].positions[ix].x);
        r.Top = MIN((int)r.Top, felem->variants[fvarindex].positions[ix].y);
        r.Right = MAX((int)r.Right, felem->variants[fvarindex].positions[ix].x + felem->variants[fvarindex].positions[ix].width);
        r.Bottom = MAX((int)r.Bottom, felem->variants[fvarindex].positions[ix].y + felem->variants[fvarindex].positions[ix].height);
    }

    for (int ix = Count - 1; ix >= 0; --ix)
    {
        if (!selected[ix])
            continue;
        switch (align)
        {
        case paVert:
            felem->variants[fvarindex].positions[ix].y = r.Top + (r.Height() - felem->variants[fvarindex].positions[ix].height) / 2;
            break;
        case paHorz:
            felem->variants[fvarindex].positions[ix].x = r.Left + (r.Width() - felem->variants[fvarindex].positions[ix].width) / 2;
            break;
        case paVertFrame:
            felem->variants[fvarindex].positions[ix].y = ((10000.0 - r.Height()) / 2) + (felem->variants[fvarindex].positions[ix].y - r.Top);
            break;
        case paHorzFrame:
            felem->variants[fvarindex].positions[ix].x = ((10000.0 - r.Width()) / 2) + (felem->variants[fvarindex].positions[ix].x - r.Left);
            break;
        }
        felem->variants[fvarindex].positions[ix].y = max(0, min(felem->variants[fvarindex].positions[ix].y, 10000 - felem->variants[fvarindex].positions[ix].height));
        felem->variants[fvarindex].positions[ix].x = max(0, min(felem->variants[fvarindex].positions[ix].x, 10000 - felem->variants[fvarindex].positions[ix].width));
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TPartList::alignpart(TPartAlignment align, int topart)
{
    TRect r = Rect(felem->variants[fvarindex].positions[topart].x, felem->variants[fvarindex].positions[topart].y, felem->variants[fvarindex].positions[topart].x + felem->variants[fvarindex].positions[topart].width, felem->variants[fvarindex].positions[topart].y + felem->variants[fvarindex].positions[topart].height);
    for (int ix = Count - 1; ix >= 0; --ix)
    {
        if (!selected[ix])
            continue;
        switch (align)
        {
        case paVert:
            felem->variants[fvarindex].positions[ix].y = r.Top + (r.Height() - felem->variants[fvarindex].positions[ix].height) / 2;
            break;
        case paHorz:
            felem->variants[fvarindex].positions[ix].x = r.Left + (r.Width() - felem->variants[fvarindex].positions[ix].width) / 2;
            break;
        case paVertFrame:
            felem->variants[fvarindex].positions[ix].y = ((10000.0 - r.Height()) / 2) + (felem->variants[fvarindex].positions[ix].y - r.Top);
            break;
        case paHorzFrame:
            felem->variants[fvarindex].positions[ix].x = ((10000.0 - r.Width()) / 2) + (felem->variants[fvarindex].positions[ix].x - r.Left);
            break;
        }
        felem->variants[fvarindex].positions[ix].y = max(0, MIN(felem->variants[fvarindex].positions[ix].y, 10000 - felem->variants[fvarindex].positions[ix].height));
        felem->variants[fvarindex].positions[ix].x = max(0, MIN(felem->variants[fvarindex].positions[ix].x, 10000 - felem->variants[fvarindex].positions[ix].width));
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TPartList::resizepart(TPartResize resize, int size)
{
    size = MAX(size, 1000);
    for (int ix = Count - 1; ix >= 0; --ix)
    {
        if (!selected[ix])
            continue;
        if (resize == prWidth)
            felem->variants[fvarindex].positions[ix].width = size;
        if (resize == prHeight)
            felem->variants[fvarindex].positions[ix].height = size;
        felem->variants[fvarindex].positions[ix].y = min(felem->variants[fvarindex].positions[ix].y, 10000 - felem->variants[fvarindex].positions[ix].height);
        felem->variants[fvarindex].positions[ix].x = min(felem->variants[fvarindex].positions[ix].x, 10000 - felem->variants[fvarindex].positions[ix].width);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TfEditElem::TStrokeManager::TStrokeManager(TKanjiVariant *avariant)
{
    var = avariant;

    if (!count)
    {
        sel = NULL;
        return;
    }

    sel = new bool*[count];
    for (int ix = 0; ix < count; ++ix)
    {
        sel[ix] = new bool[length[ix]];
        memset(sel[ix], 0, sizeof(bool)*length[ix]);
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::freeup()
{
    for (int ix = 0; ix < count && sel; ++ix)
        delete[] sel[ix];
    delete[] sel;
}
//---------------------------------------------------------------------------
int TfEditElem::TStrokeManager::prev_vert(int astroke, int avert, bool &connected)
{
    int ix = avert;
    connected = true;
    for (; ix >= 0 && (ix == avert || selected[astroke][ix]); --ix)
        if (ix && points[astroke][ix]->type == ptMoveTo)
            connected = false;

    return ix;
}
//---------------------------------------------------------------------------
int TfEditElem::TStrokeManager::next_vert(int astroke, int avert, bool &connected)
{
    int ix = avert + 1;
    connected = true;
    for (; ix < length[astroke] && selected[astroke][ix]; ++ix)
        if (points[astroke][ix]->type == ptMoveTo)
            connected = false;
    if (connected && ix < length[astroke] && points[astroke][ix]->type == ptMoveTo)
        connected = false;

    return ix;
}
//---------------------------------------------------------------------------
bool TfEditElem::TStrokeManager::keep_vertex_del(int astroke, int avert)
{
    if (selected[astroke][avert])
        return false;

    bool cprev, cnext;
    int prev = prev_vert(astroke, avert, cprev);
    int next = next_vert(astroke, avert, cnext);

    return prev >= 0 && cprev || next < length[astroke] && cnext;
}
//---------------------------------------------------------------------------
bool TfEditElem::TStrokeManager::keep_segment_del(int astroke, int avert)
{
    return !selected[astroke][avert] || points[astroke][avert]->type != ptMoveTo && !selected[astroke][avert - 1] || avert < length[astroke] - 1 && points[astroke][avert + 1]->type != ptMoveTo && !selected[astroke][avert + 1];
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::insert_vertex(int astroke, int avert)
{
    bool *tmps = sel[astroke];
    TElementPoint *tmp = elem[astroke].points;
    int len = length[astroke];

    sel[astroke] = new bool[len + 1];
    sel[astroke][avert] = false;
    elem[astroke].points = new TElementPoint[len + 1];

    if (avert)
    {
        memcpy(sel[astroke], tmps, sizeof(bool)*avert);
        memcpy(elem[astroke].points, tmp, sizeof(TElementPoint)*avert);
    }
    if (avert < len)
    {
        memcpy(sel[astroke] + avert + 1, tmps + avert, sizeof(bool)*(len - avert));
        memcpy(elem[astroke].points + avert + 1, tmp + avert, sizeof(TElementPoint)*(len - avert));
    }

    if (len)
    {
        delete[] tmps;
        delete[] tmp;
    }
    elem[astroke].pointcount++;
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::delete_vertices(int astroke)
{
    //count number of vertices intact
    int sc = 0;
    for (int ix = 0; ix < length[astroke]; ++ix)
        if (keep_vertex_del(astroke, ix))
            ++sc;

    if (sc == length[astroke])
        return;

    if (sc)
    {
        TElementPoint *tmp = new TElementPoint[sc];
        bool *tmps = new bool[sc];
        memset(tmps, 0, sizeof(bool)*sc);

        int prev = -1;
        int tpos = 0;
        bool hasbreak = false;
        bool hasstraight = false;
        bool hasdeleted = false;

        for (int ix = 0; ix < length[astroke]; ++ix)
        {
            if (!keep_vertex_del(astroke, ix))
            {
                hasbreak |= ix && points[astroke][ix]->type == ptMoveTo;
                hasstraight |= points[astroke][ix]->type == ptLineTo;
                hasdeleted = true;
                continue;
            }
            tmp[tpos] = elem[astroke].points[ix];

            if (hasdeleted)
            {
                if (!hasbreak && !hasstraight && prev >= 0 && prev < ix - 1 && elem[astroke].points[ix].type == ptCurve)
                {
                    TDoublePoint p1 = DoublePoint(elem[astroke].points[prev].x, elem[astroke].points[prev].y);
                    TDoublePoint c1 = DoublePoint(elem[astroke].points[prev + 1].c1x, elem[astroke].points[prev + 1].c1y);
                    TDoublePoint c2 = DoublePoint(elem[astroke].points[ix].c2x, elem[astroke].points[ix].c2y);
                    TDoublePoint p2 = DoublePoint(elem[astroke].points[ix].x, elem[astroke].points[ix].y);

                    double len = 0, lena = 0, lenb = 0;
                    for (int j = prev; j < ix; j++)
                    {
                        double tmp = zbezierlen(DoublePoint(elem[astroke].points[j].x, elem[astroke].points[j].y), DoublePoint(elem[astroke].points[j + 1].c1x, elem[astroke].points[j + 1].c1y), DoublePoint(elem[astroke].points[j + 1].c2x, elem[astroke].points[j + 1].c2y), DoublePoint(elem[astroke].points[j + 1].x, elem[astroke].points[j + 1].y));
                        if (j == prev)
                            lena = tmp;
                        if (j == ix - 1)
                            lenb = tmp;
                        len += tmp;
                    }
                    lena = len / lena;
                    lenb = len / lenb;
                    tmp[tpos].c1x = p1.x + (c1.x - p1.x) * lena;
                    tmp[tpos].c1y = p1.y + (c1.y - p1.y) * lena;
                    tmp[tpos].c2x = p2.x + (c2.x - p2.x) * lenb;
                    tmp[tpos].c2y = p2.y + (c2.y - p2.y) * lenb;
                }
                else
                {
                    tmp[tpos].c1x = 0;
                    tmp[tpos].c1y = 0;
                    tmp[tpos].c2x = 0;
                    tmp[tpos].c2y = 0;
                    if (hasbreak || tpos == 0)
                        tmp[tpos].type = ptMoveTo;
                    else if (hasstraight && tmp[tpos].type == ptCurve)
                        tmp[tpos].type = ptLineTo;
                    else if (tmp[tpos].type == ptCurve)
                    {;
                    }
                }
            }

            hasbreak = false;
            hasstraight = false;
            hasdeleted = false;
            prev = ix;
            ++tpos;
        }

        delete[] sel[astroke];
        sel[astroke] = tmps;
        delete[] elem[astroke].points;
        elem[astroke].points = tmp;
        elem[astroke].pointcount = sc;
    }
    else
        delete_stroke(astroke);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::delete_segments(int astroke)
{
    int sc = 0;
    for (int ix = 0; ix < length[astroke]; ++ix)
        if (keep_segment_del(astroke, ix))
            sc++;

    int st = 0;
    for (int ix = 0; ix < length[astroke] - 1; ++ix)
        if (selected[astroke][ix] && selected[astroke][ix + 1] && points[astroke][ix + 1]->type != ptMoveTo)
            st++;

    if (sc == length[astroke] && !st)
        return;

    if (sc)
    {
        TElementPoint *tmp = new TElementPoint[sc];
        bool *tmps = new bool[sc];
        memset(tmps, 0, sizeof(bool)*sc);

        int tpos = 0;
        bool hasbreak = false;
        for (int ix = 0; ix < length[astroke]; ++ix)
        {
            if (!keep_segment_del(astroke, ix))
            {
                hasbreak = true;
                continue;
            }
            if (ix && selected[astroke][ix - 1] && selected[astroke][ix] && points[astroke][ix]->type != ptMoveTo)
                hasbreak = true;

            tmp[tpos] = elem[astroke].points[ix];
            if (hasbreak)
            {
                tmp[tpos].type = ptMoveTo;
                tmp[tpos].c1x = 0;
                tmp[tpos].c1y = 0;
                tmp[tpos].c2x = 0;
                tmp[tpos].c2y = 0;
            }
            hasbreak = false;
            ++tpos;
        }

        delete[] sel[astroke];
        sel[astroke] = tmps;
        delete[] elem[astroke].points;
        elem[astroke].points = tmp;
        elem[astroke].pointcount = sc;
    }
    else
        delete_stroke(astroke);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::delete_stroke(int astroke)
{
    bool **tmps = sel;
    TElementStroke *tmp = elem;
    if (count > 1)
    {
        sel = new bool*[count - 1];
        var->strokes = new TElementStroke[count - 1];

        if (astroke)
        {
            memcpy(sel, tmps, sizeof(bool*)*astroke);
            memcpy(var->strokes, tmp, sizeof(TElementStroke)*astroke);
        }
        if (astroke < count - 1)
        {
            memcpy(sel + astroke, tmps + astroke + 1, sizeof(bool*)*(count - astroke - 1));
            memcpy(var->strokes + astroke, tmp + astroke + 1, sizeof(TElementStroke)*(count - astroke - 1));
        }
    }
    else
    {
        sel = 0;
        var->strokes = 0;
    }

    var->strokecount--;

    delete[] tmps[astroke];
    delete[] tmps;
    delete[] tmp[astroke].points;
    delete[] tmp;
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::deselect(int astroke)
{
    memset(sel[astroke], 0, sizeof(bool)*length[astroke]);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::deselect()
{
    for (int ix = 0; ix < count; ++ix)
        memset(sel[ix], 0, sizeof(bool)*length[ix]);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::curve_segments(int astroke, bool tocurve)
{
    for (int ix = length[astroke] - 1; ix >= 1; ix--)
    {
        if (points[astroke][ix]->type == ptMoveTo || !selected[astroke][ix] || !selected[astroke][ix - 1])
            continue;

        if (points[astroke][ix]->type == ptCurve && !tocurve)
        {
            points[astroke][ix]->type = ptLineTo;
            points[astroke][ix]->c1x = 0;
            points[astroke][ix]->c2x = 0;
            points[astroke][ix]->c1y = 0;
            points[astroke][ix]->c2y = 0;
        }
        else if (points[astroke][ix]->type == ptLineTo && tocurve)
        {
            points[astroke][ix]->type = ptCurve;
            points[astroke][ix]->c1x = points[astroke][ix - 1]->x + (points[astroke][ix]->x - points[astroke][ix - 1]->x) / 3;
            points[astroke][ix]->c1y = points[astroke][ix - 1]->y + (points[astroke][ix]->y - points[astroke][ix - 1]->y) / 3;
            points[astroke][ix]->c2x = points[astroke][ix]->x - (points[astroke][ix]->x - points[astroke][ix - 1]->x) / 3;
            points[astroke][ix]->c2y = points[astroke][ix]->y - (points[astroke][ix]->y - points[astroke][ix - 1]->y) / 3;
        }
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::split_segment(int astroke, int avert, double where)
{
    insert_vertex(astroke, avert + 1);

    TElementPoint *elempoint = &elem[astroke].points[avert + 1];
    try
    {
        if (points[astroke][avert + 2]->type == ptCurve)
        {
            TDoublePoint p1, p2, c1, c2, nc1, nc2, npt;
            p1 = DoublePoint(points[astroke][avert]->x, points[astroke][avert]->y);
            c1 = DoublePoint(points[astroke][avert + 2]->c1x, points[astroke][avert + 2]->c1y);
            c2 = DoublePoint(points[astroke][avert + 2]->c2x, points[astroke][avert + 2]->c2y);
            p2 = DoublePoint(points[astroke][avert + 2]->x, points[astroke][avert + 2]->y);

            zbeziersplit(p1, c1, c2, p2, npt, nc1, nc2, where);

            elempoint->type = ptCurve;
            elempoint->x = npt.x;
            elempoint->y = npt.y;
            elempoint->c1x = c1.x;
            elempoint->c1y = c1.y;
            elempoint->c2x = nc1.x;
            elempoint->c2y = nc1.y;
            points[astroke][avert + 2]->c1x = nc2.x;
            points[astroke][avert + 2]->c1y = nc2.y;
            points[astroke][avert + 2]->c2x = c2.x;
            points[astroke][avert + 2]->c2y = c2.y;

        }
        else if (points[astroke][avert + 2]->type == ptLineTo)
        {
            memset(elempoint, 0, sizeof(TElementPoint));
            elempoint->x = points[astroke][avert]->x + (points[astroke][avert + 2]->x - points[astroke][avert]->x) * where;
            elempoint->y = points[astroke][avert]->y + (points[astroke][avert + 2]->y - points[astroke][avert]->y) * where;
            elempoint->type = ptLineTo;
        }
        else
            THROW(L"We are not ready for this!");
    }
    catch (...)
    {
        delete elem;
        return;
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::weld_selected(int astroke)
{
    int sc = 0;
    TIntegerList *welds = new TIntegerList;

    bool con;

    for (int ix = 0; ix < length[astroke]; ++ix)
    {
        if (!selected[astroke][ix])
            sc++;
        else
        {
            if (ix && points[astroke][ix]->type != ptMoveTo)
            {
                welds->Add(ix);
                sc++;
                ix = next_vert(astroke, ix, con) - 1;
                welds->Add(ix);
            }
            else
            {
                int prev = ix;
                ix = next_vert(astroke, ix, con) - 1;
                if (ix != length[astroke] - 1 && points[astroke][ix + 1]->type != ptMoveTo)
                {
                    sc++;
                    welds->Add(prev);
                    welds->Add(ix);
                }
            }
        }
    }

    if (sc == length[astroke])
    {
        delete welds;
        return;
    }

    TElementPoint *tmp = new TElementPoint[sc];
    bool *tmps = new bool[sc];
    memset(tmps, 0, sizeof(bool)*sc);

    int tpos = 0;
    int ix = 0;
    int p, n;
    while (ix < length[astroke])
    {
        if (welds->Count)
        {
            p = welds->Items[0];
            welds->Delete(0);
            n = welds->Items[0];
            welds->Delete(0);
        }
        else
            n = p = length[astroke];

        while (ix < p)
        {
            *(tmp + tpos) = elem[astroke].points[ix];

            ++tpos;
            ++ix;
        }

        if (ix < length[astroke])
        {

            int x = 0, y = 0;
            while (ix <= n)
            {
                x += points[astroke][ix]->x;
                y += points[astroke][ix]->y;
                ++ix;
            }
            x /= n - p + 1;
            y /= n - p + 1;
            memset(tmp + tpos, 0, sizeof(TElementPoint));
            tmp[tpos].type = points[astroke][p]->type;
            tmp[tpos].x = x;
            tmp[tpos].y = y;
            if (tmp[tpos].type == ptCurve)
            {
                tmp[tpos].c1x = points[astroke][p]->c1x;
                tmp[tpos].c1y = points[astroke][p]->c1y;
                tmp[tpos].c2x = points[astroke][p]->c2x + x - points[astroke][p]->x;
                tmp[tpos].c2y = points[astroke][p]->c2y + y - points[astroke][p]->y;
            }
            if (n + 1 < length[astroke] && points[astroke][n + 1]->type == ptCurve)
            {
                points[astroke][n + 1]->c1x += x - points[astroke][n]->x;
                points[astroke][n + 1]->c1y += y - points[astroke][n]->y;
            }
        }
        ++tpos;
    }

    delete[] sel[astroke];
    sel[astroke] = tmps;
    delete[] elem[astroke].points;
    elem[astroke].points = tmp;
    elem[astroke].pointcount = sc;

    delete welds;
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::reverse_stroke(int astroke)
{
    if (!length[astroke])
        return;

    for (int ix = 0; ix < length[astroke] - 1; ++ix)
    {
        memswap(points[astroke][ix]->type, points[astroke][ix + 1]->type);
        memswap(points[astroke][ix]->c1x, points[astroke][ix + 1]->c2x);
        memswap(points[astroke][ix]->c2x, points[astroke][ix + 1]->c1x);
        memswap(points[astroke][ix]->c1y, points[astroke][ix + 1]->c2y);
        memswap(points[astroke][ix]->c2y, points[astroke][ix + 1]->c1y);
    }

    for (int ix = 0; ix < length[astroke] / 2; ++ix)
    {
        if (length[astroke] - 1 - ix != ix)
            memswap(elem[astroke].points[ix], elem[astroke].points[length[astroke] - 1 - ix]);
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::add_stroke(int x, int y)
{
    /*reusing variable names*/ {
        bool **tmp = sel;
        sel = new bool*[count + 1];
        if (tmp)
        {
            memcpy(sel, tmp, sizeof(bool*)*count);
            delete[] tmp;
        }
        sel[count] = new bool[2];
        memset(sel[count], 0, sizeof(bool)*2);
    }

    /*reusing variable names*/ {
        TElementStroke *tmp = var->strokes;
        var->strokes = new TElementStroke[count + 1];
        if (tmp)
        {
            memcpy(var->strokes, tmp, sizeof(TElementStroke)*count);
            delete[] tmp;
        }
        var->strokes[count].tips = stNormalTip;
        var->strokes[count].pointcount = 2;
        var->strokes[count].points = new TElementPoint[2];
        memset(var->strokes[count].points, 0, sizeof(TElementPoint)*2);
        var->strokes[count].points[0].type = ptMoveTo;
        var->strokes[count].points[1].type = ptLineTo;
        var->strokes[count].points[1].x = var->strokes[count].points[0].x = x;
        var->strokes[count].points[1].y = var->strokes[count].points[0].y = y;
        ++var->strokecount;
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::add_vertex(int x, int y, int astroke)
{
    /*reusing variable names*/ {
        bool *tmp = sel[astroke];
        sel[astroke] = new bool[length[astroke] + 1];
        if (tmp)
        {
            memcpy(sel[astroke], tmp, sizeof(bool)*length[astroke]);
            delete[] tmp;
        }
        sel[astroke][length[astroke]] = false;
    }

    /*reusing variable names*/ {
        TElementStroke *es = &var->strokes[astroke];
        TElementPoint *tmp = es->points;
        es->points = new TElementPoint[es->pointcount + 1];
        if (tmp)
        {
            memcpy(es->points, tmp, sizeof(TElementPoint)*es->pointcount);
            delete[] tmp;
        }
        memset(&es->points[es->pointcount], 0, sizeof(TElementPoint));
        es->points[es->pointcount].type = es->pointcount ? ptLineTo : ptMoveTo;
        es->points[es->pointcount].x = x;
        es->points[es->pointcount].y = y;
        ++es->pointcount;
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::move_stroke(int astroke, int topos)
{
    if (astroke == topos)
        return;

    bool *ssave = sel[astroke];
    TElementStroke save = var->strokes[astroke];
    if (astroke < topos)
    {
        memmove(var->strokes + astroke, var->strokes + astroke + 1, sizeof(TElementStroke)*(topos - astroke));
        memmove(sel + astroke, sel + astroke + 1, sizeof(bool*)*(topos - astroke));
        var->strokes[topos] = save;
        sel[topos] = ssave;
    }
    else
    {
        memmove(var->strokes + topos + 1, var->strokes + topos, sizeof(TElementStroke)*(astroke - topos));
        memmove(sel + topos + 1, sel + topos, sizeof(bool*)*(astroke - topos));
        var->strokes[topos] = save;
        sel[topos] = ssave;
    }
}
//---------------------------------------------------------------------------
int TfEditElem::TStrokeManager::get_count()
{
    return var->strokecount;
}
//---------------------------------------------------------------------------
TElementStroke* TfEditElem::TStrokeManager::get_elem()
{
    return var->strokes;
}
//---------------------------------------------------------------------------
int TfEditElem::TStrokeManager::get_selcount(int astroke)
{
    int sc = 0;
    for (int ix = 0; ix < length[astroke]; ++ix)
        sc += sel[astroke][ix];
    return sc;
}
//---------------------------------------------------------------------------
bool TfEditElem::TStrokeManager::get_selected(int astroke, int avert)
{
    return sel[astroke][avert];
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeManager::set_selected(int astroke, int avert, bool val)
{
    sel[astroke][avert] = val;
}
//---------------------------------------------------------------------------
int TfEditElem::TStrokeManager::get_len(int astroke)
{
    return elem[astroke].pointcount;
}
//---------------------------------------------------------------------------
TElementPoint* TfEditElem::TStrokeManager::get_point(int astroke, int avert)
{
    return &elem[astroke].points[avert];
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TfEditElem::TStrokeList::TStrokeList() : fthres(100), fvsize(80), items(NULL), fmoving(false), fadding(false)
{
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::Add(TKanjiElement *aelem, short aindex, byte avar)
{
    felemindex = aindex;
    fvarindex = avar;
    felem = aelem;

    delete items;
    items = new TStrokeManager(&aelem->variants[avar]);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::Deselect()
{
    for (int ix = 0; ix < count; ++ix)
        items->deselect(ix);
}
//---------------------------------------------------------------------------
bool TfEditElem::TStrokeList::get_strokesel(int ix)
{
    return items->selcount[ix];
}
//---------------------------------------------------------------------------
bool TfEditElem::TStrokeList::get_vertsel(int astroke, int avert)
{
    return items->selected[astroke][avert];
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::set_vertsel(int astroke, int ix, bool val)
{
    items->selected[astroke][ix] = val;
}
//---------------------------------------------------------------------------
int TfEditElem::TStrokeList::get_selcount()
{
    int c = 0;
    for (int ix = 0; ix < count; ++ix)
        c += items->selcount[ix];
    return c;
}
//---------------------------------------------------------------------------
int TfEditElem::TStrokeList::get_strokeselcount()
{
    int c = 0;
    for (int ix = 0; ix < count; ++ix)
        if (items->selcount[ix])
            ++c;
    return c;
}
//---------------------------------------------------------------------------
int TfEditElem::TStrokeList::get_segselcount()
{
    int c = 0;
    for (int ix = 0; ix < count; ++ix)
        if (items->selcount[ix] > 1)
        {
            for (int j = 0; j < items->length[ix] - 1; ++j)
                if (items->selected[ix][j] && items->selected[ix][j + 1] && items->points[ix][j + 1]->type != ptMoveTo)
                    c++;
        }
    return c;
}
//---------------------------------------------------------------------------
int TfEditElem::TStrokeList::get_curveselcount()
{
    int c = 0;
    for (int ix = 0; ix < count; ++ix)
        if (items->selcount[ix] > 1)
        {
            for (int j = 0; j < items->length[ix] - 1; ++j)
                if (items->selected[ix][j] && items->selected[ix][j + 1] && items->points[ix][j + 1]->type == ptCurve)
                    c++;
        }
    return c;
}
//---------------------------------------------------------------------------
bool TfEditElem::TStrokeList::get_curved(int astroke, int avert)
{
    return items->points[astroke][avert]->type == ptCurve;
}
//---------------------------------------------------------------------------
TZStrokeType TfEditElem::TStrokeList::get_tips(int astroke)
{
    return (TZStrokeType)items->elem[astroke].tips;
}
//---------------------------------------------------------------------------
TRect TfEditElem::TStrokeList::vertrect(const TRect &r, int astroke, int avert, int vsize)
{
    if (vsize <= 0)
        vsize = fvsize;

    TElementPoint *p = items->points[astroke][avert];
    double px = (double)p->x * r.Width() / 10000.0 + r.Left;
    double py = (double)p->y * r.Height() / 10000.0 + r.Top;
    return Rect(px - vsize, py - vsize, px + vsize, py + vsize);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::draw(TCanvas *c, const TRect &r)
{
    for (int ix = 0; ix < count; ++ix)
    {
        if (stroke[ix])
            continue;
        drawstroke(c, r, ix);
    }
    for (int ix = 0; ix < count; ++ix)
    {
        if (stroke[ix])
            continue;
        //draw vertices
        for (int j = 0; j < items->length[ix]; ++j)
        {
            if (vertex[ix][j])
                continue;
            drawvertex(c, r, ix, j);
        }
    }

    for (int ix = 0; ix < count; ++ix)
    {
        if (!stroke[ix])
            continue;
        drawstroke(c, r, ix);
    }
    for (int ix = 0; ix < count; ++ix)
    {
        if (!stroke[ix])
            continue;
        for (int j = 0; j < items->length[ix]; ++j)
        {
            if (vertex[ix][j])
                continue;
            drawvertex(c, r, ix, j);
        }
        for (int j = 0; j < items->length[ix]; ++j)
            if (vertex[ix][j])
                drawvertex(c, r, ix, j);
    }

}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::drawstroke(TCanvas *c, const TRect &r, int astroke)
{
    c->Pen->Width = r.Width() / 50;

    TDoublePoint penpos;

    //draw the line
    for (int ix = 0; ix < items->length[astroke]; ++ix)
    {
        c->Pen->Color = stroke[astroke] ? ix > 0 && vertex[astroke][ix - 1] && vertex[astroke][ix] ? COLORDEC(clRed, 0x303030) : clBlue : clBlack;
        TElementPoint *p = items->points[astroke][ix];

        TZStrokeType starttip = stNormalTip;
        TZStrokeType endtip = stNormalTip;
        if (ix == 1)
        {
            if (items->elem[astroke].tips == (byte)stSingleDot)
                starttip = stSingleDot;
            else if (items->elem[astroke].tips == (byte)stStartPointed || items->elem[astroke].tips == (byte)stStartPointedEndPointed || items->elem[astroke].tips == (byte)stStartPointedEndThin)
                starttip = stStartPointed;
            else if (items->elem[astroke].tips == (byte)stStartThin || items->elem[astroke].tips == (byte)stStartThinEndPointed || items->elem[astroke].tips == (byte)stStartThinEndThin)
                starttip = stStartThin;
        }
        if (ix == items->length[astroke] - 1)
        {
            if (items->elem[astroke].tips == (byte)stSingleDot);
            else if (items->elem[astroke].tips == (byte)stEndPointed || items->elem[astroke].tips == (byte)stStartThinEndPointed || items->elem[astroke].tips == (byte)stStartPointedEndPointed)
                endtip = stEndPointed;
            else if (items->elem[astroke].tips == (byte)stEndThin || items->elem[astroke].tips == (byte)stStartThinEndThin || items->elem[astroke].tips == (byte)stStartPointedEndThin)
                endtip = stEndThin;
        }

        if (p->type == ptMoveTo)
        {
            penpos = DoublePoint((double)p->x * r.Width() / 10000.0 + r.Left, (double)p->y * r.Height() / 10000.0 + r.Top);
            //c->MoveTo((double)p->x*r.Width()/10000.0+r.Left, (double)p->y*r.Height()/10000.0+r.Top);
        }
        else if (p->type == ptLineTo)
        {
            TDoublePoint p2 = DoublePoint((double)p->x * r.Width() / 10000.0 + r.Left, (double)p->y * r.Height() / 10000.0 + r.Top);
            zline(c->Handle, (double)c->Pen->Width*0.9, c->Pen->Color, penpos, p2, starttip, endtip);
            penpos = p2;
            //c->LineTo((double)p->x*r.Width()/10000.0+r.Left, (double)p->y*r.Height()/10000.0+r.Top);
        }
        else if (p->type == ptCurve)
        {
            TDoublePoint p1, p2, c1, c2;
            p1 = penpos; //c->PenPos;
            p2 = DoublePoint((double)p->x * r.Width() / 10000.0 + r.Left, (double)p->y * r.Height() / 10000.0 + r.Top);
            c1 = DoublePoint((double)p->c1x * r.Width() / 10000.0 + r.Left, (double)p->c1y * r.Height() / 10000.0 + r.Top);
            c2 = DoublePoint((double)p->c2x * r.Width() / 10000.0 + r.Left, (double)p->c2y * r.Height() / 10000.0 + r.Top);
            zbezier(c->Handle, (double)c->Pen->Width*0.9, c->Pen->Color, p1, c1, c2, p2, starttip, endtip);
            penpos = p2;
        }
        else
            THROW(L"We are not ready for this!");
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::drawvertex(TCanvas *c, const TRect &r, int astroke, int avert)
{
    bool sel = stroke[astroke];

    c->Pen->Width = 1;
    c->Pen->Color = !sel ? clBlack : vertex[astroke][avert] ? clYellow : COLORINC(clBlue, 0x80a0a0);
    c->Brush->Color = !sel ? clWhite : vertex[astroke][avert] ? clRed : COLORDEC(clBlue, 0x6060a0);
    if (avert == 0)
    {
        if (!sel)
            c->Pen->Color = clYellow;
        c->Brush->Color = clLime;
    }

    TRect rr = vertrect(r, astroke, avert, sel ? -1 : fvsize - 2);
    c->Rectangle(rr.Left, rr.Top, rr.Right, rr.Bottom);

    if (vertex[astroke][avert])
        drawcontrolpoint(c, r, astroke, avert, 1);
    if (vertex[astroke][avert])
        drawcontrolpoint(c, r, astroke, avert, 2);

    //draw number indicating order of stroke
    if (!avert && fnumbers/*&& !fmoving*/ && !fadding)
    {
        c->Pen->Style = psClear;
        c->Brush->Color = felem->variants[fvarindex].centrepoint - 1 == astroke ? (TColor)0x1010da : stroke[astroke] ? (TColor)0x60eafa : scolors[astroke];

        int size = fvsize;

        TElementPoint *p = items->points[astroke][avert];
        double px = (double)p->x * r.Width() / 10000.0 + r.Left;
        double py = (double)p->y * r.Height() / 10000.0 + r.Top;

        c->Font->Name = "Arial";
        c->Font->Size = size * 2.6;
        c->Font->Style = TFontStyles() << fsBold;
        c->Font->Color = stroke[astroke] ? clBlack : clWhite;

        TSize s;
        UnicodeString num = astroke + 1;
        GetTextExtentPoint32W(c->Handle, num.c_str(), num.Length(), &s);
        rr = Rect(px - size - s.cx - size, py - size - s.cy - size / 3, px - size, py - size);
        c->RoundRect(rr.Left, rr.Top, rr.Right, rr.Bottom, size, size);

        c->Brush->Style = bsClear;

        int ta = GetTextAlign(c->Handle);
        SetTextAlign(c->Handle, TA_CENTER | TA_BASELINE);
        c->TextOut(rr.Left + rr.Width() / 2, rr.Top + rr.Height()*0.8, num);
        c->Font->Style = TFontStyles();
        SetTextAlign(c->Handle, ta);

        c->Brush->Style = bsSolid;
        c->Pen->Style = psSolid;
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::drawcontrolpoint(TCanvas *c, const TRect &r, int astroke, int avert, int acontrol)
{
    if (avert < 0)
        return;

    if (avert >= items->length[astroke] || items->points[astroke][avert]->type != ptCurve && acontrol == 2)
        return;

    TPoint p1, p2;

    p1 = Point((double)items->points[astroke][avert]->x * r.Width() / 10000.0 + r.Left, (double)items->points[astroke][avert]->y * r.Height() / 10000.0 + r.Top);
    if (acontrol == 1)
    {
        if (avert == items->length[astroke] - 1 || items->points[astroke][avert + 1]->type != ptCurve)
            return;
        p2 = Point((double)items->points[astroke][avert + 1]->c1x * r.Width() / 10000.0 + r.Left, (double)items->points[astroke][avert + 1]->c1y * r.Height() / 10000.0 + r.Top);
    }
    else
        p2 = Point((double)items->points[astroke][avert]->c2x * r.Width() / 10000.0 + r.Left, (double)items->points[astroke][avert]->c2y * r.Height() / 10000.0 + r.Top);

    c->Pen->Width = 1;
    c->Pen->Mode = pmNotXor;
    c->Pen->Color = clRed;
    c->MoveTo(p1.x, p1.y);
    c->LineTo(p2.x, p2.y);
    c->Pen->Mode = pmCopy;
    c->Brush->Color = clRed;
    if (vertex[astroke][avert])
        c->Brush->Color = clYellow;
    c->Ellipse(p2.x - fvsize + 1, p2.y - fvsize + 1, p2.x + fvsize - 1, p2.y + fvsize - 1);
}
//---------------------------------------------------------------------------
double TfEditElem::TStrokeList::onstroke(const TRect &r, int x, int y, bool endpoints, int &restroke, int &revertex)
{
    double dist = 99999;
    restroke = -1;
    revertex = -1;

    x = (double)(x - r.Left) * 10000.0 / r.Width();
    y = (double)(y - r.Top) * 10000.0 / r.Height();
    int tmpv;
    for (int ix = 0; ix < count; ++ix)
    {
        double tmp = ptdistance(ix, x, y, tmpv, endpoints) * r.Height() / 10000.0;
        if (tmp < fthres && (restroke < 0 || stroke[ix] || !stroke[restroke]) && dist > tmp)
        {
            restroke = ix;
            dist = tmp;
            revertex = tmpv - 1;
        }
    }
    return dist;
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::onvertex(const TRect &r, int x, int y, bool alternate, bool controlpoints, int &restroke, int &revertex, byte &recontrol)
{
    double dist = 99999;

    restroke = -1;
    revertex = -1;
    recontrol = 0;

    //first select control points if present
    for (int ix = 0; ix < count && controlpoints; ++ix)
    {

        if (!stroke[ix])
            continue; //no control point visible here

        for (int j = 0; j < items->length[ix]; ++j)
        {
            for (int cp = 1; cp < 3; ++cp)
            {
                if (cp == 1 && j == items->length[ix] - 1 || cp == 2 && j == 0)
                    continue;

                int v = cp != 1 ? j : j + 1;
                if (cp && (items->points[ix][v]->type != ptCurve) || !vertex[ix][j])
                    continue;

                TElementPoint *ep = items->points[ix][v];
                TPoint dp = cp == 1 ? Point(ep->c1x, ep->c1y) : Point(ep->c2x, ep->c2y);
                double tmp = zpointdistance(Point((double)(x - r.Left) * 10000.0 / r.Width(), (double)(y - r.Top) * 10000.0 / r.Height()), dp) * r.Width() / 10000.0;
                if (tmp < fthres)
                {
                    //previously found vertex selected, this one is not
                    if (restroke >= 0 && vertex[restroke][revertex] && !vertex[ix][j])
                        continue;

                    //both vertices are selected or none - but this is closer
                    if (dist > tmp || vertex[ix][j])
                    {
                        restroke = ix;
                        revertex = j;
                        recontrol = cp;
                        dist = tmp;
                    }

                }
            }
        }
    }

    if (restroke >= 0)
        return;

    //select vertex
    for (int ix = 0; ix < count; ++ix)
    {
        for (int j = 0; j < items->length[ix]; ++j)
        {
            TElementPoint *ep = items->points[ix][j];
            TPoint dp = Point(ep->x, ep->y);
            double tmp = zpointdistance(Point((double)(x - r.Left) * 10000.0 / r.Width(), (double)(y - r.Top) * 10000.0 / r.Height()), dp) * r.Width() / 10000.0;
            if (tmp < fthres)
            {
                //previously found vertex selected, this one is not
                if (restroke >= 0 && (alternate && (!vertex[restroke][revertex] && vertex[ix][j]) || !alternate && (vertex[restroke][revertex] && !vertex[ix][j])))
                    continue;

                //both vertices are selected or none - but this is closer
                if (dist > tmp || !alternate && vertex[ix][j] || restroke >= 0 && alternate && !vertex[ix][j] && vertex[restroke][revertex])
                {
                    restroke = ix;
                    revertex = j;
                    dist = tmp;
                }
            }
        }
    }

}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::nearestpt(const TRect &r, int x, int y, int &ptx, int &pty)
{
    int s = -1;
    int v = -1;
    double dist = onstroke(r, x, y, false, s, v);

    x = (double)(x - r.Left) * 10000.0 / r.Width();
    y = (double)(y - r.Top) * 10000.0 / r.Height();
    if (v >= 0 && v < items->length[s] - 1 && dist < fthres && items->points[s][v + 1]->type != ptMoveTo)
    {
        double where;
        if (items->points[s][v + 1]->type == ptCurve)
        {
            TDoublePoint p1, p2, c1, c2, nc1, nc2, npt;
            p1 = DoublePoint(items->points[s][v]->x, items->points[s][v]->y);
            c1 = DoublePoint(items->points[s][v + 1]->c1x, items->points[s][v + 1]->c1y);
            c2 = DoublePoint(items->points[s][v + 1]->c2x, items->points[s][v + 1]->c2y);
            p2 = DoublePoint(items->points[s][v + 1]->x, items->points[s][v + 1]->y);
            npt = DoublePoint(x, y);
            zbezierdistance(p1, c1, c2, p2, npt, where, false);
            zbeziersplit(p1, c1, c2, p2, npt, nc1, nc2, where);
            ptx = npt.x;
            pty = npt.y;
        }
        else
        {
            TDoublePoint p1, p2, p;
            p1 = DoublePoint(items->points[s][v]->x, items->points[s][v]->y);
            p2 = DoublePoint(items->points[s][v + 1]->x, items->points[s][v + 1]->y);
            p = DoublePoint(x, y);
            double pd = zpointdistance(p1, p2);
            where = pd ? zpointposition(p1, p2, p) / pd : 0;
            ptx = items->points[s][v]->x + (items->points[s][v + 1]->x - items->points[s][v]->x) * where;
            pty = items->points[s][v]->y + (items->points[s][v + 1]->y - items->points[s][v]->y) * where;
        }
        ptx = (double)ptx * r.Width() / 10000.0 + r.Left;
        pty = (double)pty * r.Height() / 10000.0 + r.Top;
    }
    else
    {
        ptx = 99999;
        pty = 99999;
    }

}
//---------------------------------------------------------------------------
bool TfEditElem::TStrokeList::select(const TRect &r, int x, int y, bool alternate, bool controlpoints, bool add)
{
    bool sel = selcount > 0;
    int s, v;
    byte c;

    onvertex(r, x, y, alternate && !add && abs(selectpt.x - x) < 3 && abs(selectpt.y - y) < 3, controlpoints, s, v, c);

    //set this in case we need it
    fmovestroke = s;
    fmovevertex = v;
    fmovecp = c;
    if (fmovecp == 1 && items->points[fmovestroke][fmovevertex]->type == ptCurve)
    {
        TElementPoint *prep = items->points[fmovestroke][fmovevertex];
        fmovecpdist = zpointdistance(DoublePoint(prep->x, prep->y), DoublePoint(prep->c2x, prep->c2y)); //cp1
    }
    else if (fmovecp == 2 && fmovevertex < items->length[fmovestroke] - 1 && items->points[fmovestroke][fmovevertex + 1]->type == ptCurve)
    {
        TElementPoint *ep = items->points[fmovestroke][fmovevertex];
        TElementPoint *nextep = items->points[fmovestroke][fmovevertex + 1];
        fmovecpdist = zpointdistance(DoublePoint(ep->x, ep->y), DoublePoint(nextep->c1x, nextep->c1y)); //cp2
    }
    else
        fmovecpdist = 0;

    selectpt = Point(x, y);

    if (s >= 0 && vertex[s][v] && !add)
        return false;

    if (s >= 0)
    {
        if (!add)
            Deselect();
        items->selected[s][v] = !items->selected[s][v];
    }
    else
    {

        onstroke(r, x, y, false, s, v);

        if (add && s >= 0 || s < 0 || !stroke[s] || !vertex[s][v] || !vertex[s][v + 1])
        {
            if (!add)
                Deselect();
            if (s >= 0)
            {
                bool on = true;
                if (add && vertex[s][v] && vertex[s][v + 1])
                    on = false;
                vertex[s][v] = on;
                vertex[s][v + 1] = on;
            }
        }
    }

    return s >= 0 || s < 0 && sel;
}
//---------------------------------------------------------------------------
bool TfEditElem::TStrokeList::select(const TRect &r, const TRect &selr, bool add)
{
    if (!add)
        Deselect();

    TRect r2;
    r2.Left = (double)(selr.Left - r.Left) * 10000.0 / r.Width();
    r2.Right = (double)(selr.Right - r.Left) * 10000.0 / r.Width();
    r2.Top = (double)(selr.Top - r.Top) * 10000.0 / r.Height();
    r2.Bottom = (double)(selr.Bottom - r.Top) * 10000.0 / r.Height();

    for (int ix = 0; ix < count; ++ix)
    {
        for (int k = 0; k < items->length[ix]; ++k)
        {
            if (PtInRect(r2, Point(items->points[ix][k]->x, items->points[ix][k]->y)))
                vertex[ix][k] = true;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
int TfEditElem::TStrokeList::ptdistance(int astroke, int x, int y, int &segment, bool endpoints)
{
    TPoint pos, nextpos;
    double dist = 99999;
    segment = -1;
    for (int ix = 0; ix < items->length[astroke]; ++ix)
    {
        TElementPoint *p = items->points[astroke][ix];
        TPoint nextpos = Point(p->x, p->y);
        if (p->type == ptMoveTo)
        {;
        }
        else if (p->type == ptLineTo)
        {
            int zd = zlinedistance(pos, nextpos, Point(x, y), endpoints);
            if (zd < dist)
            {
                dist = zd;
                segment = ix;
            }
        }
        else if (p->type == ptCurve)
        {
            TPoint p1, p2, c1, c2;
            p1 = pos;
            p2 = nextpos;
            c1 = Point(p->c1x, p->c1y);
            c2 = Point(p->c2x, p->c2y);
            double dummy;
            int zd = zbezierdistance(p1, c1, c2, p2, Point(x, y), dummy, endpoints);
            if (zd < dist)
            {
                dist = zd;
                segment = ix;
            }

        }
        else
            THROW(L"We are not ready for this!");

        pos = nextpos;
    }
    return dist;
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::startmove(const TRect &r, int x, int y)
{
    fmoving = true;
    x = (double)(x - r.Left) * 10000.0 / r.Width();
    y = (double)(y - r.Top) * 10000.0 / r.Height();

    fmovestart = Point(x, y);
    fmovepos = Point(0, 0);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::endmove()
{
    if (!fmoving)
        return;
    fmoving = false;
    fmovestroke = -1;
    fmovevertex = -1;
    fmovecp = 0;
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::startadd(const TRect &r, int x, int y, int astroke)
{
    fmoving = true;
    fadding = true;

    int xx = (double)(x - r.Left) * 10000.0 / r.Width();
    int yy = (double)(y - r.Top) * 10000.0 / r.Height();

    fmovestart = Point(xx, yy);
    fmovepos = Point(0, 0);
    if (astroke < 0)
    {
        fmovestroke = items->count;
        fmovevertex = 1;
        items->add_stroke(xx, yy);
        items->deselect();
        vertex[items->count - 1][1] = true;
    }
    else
    {
        fmovestroke = astroke;
        add(r, x, y);
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::add(const TRect &r, int x, int y)
{
    x = (double)(x - r.Left) * 10000.0 / r.Width();
    y = (double)(y - r.Top) * 10000.0 / r.Height();

    fmovestart = Point(x, y);
    fmovepos = Point(0, 0);
    fmovevertex = items->length[fmovestroke];

    items->add_vertex(x, y, fmovestroke);
    items->deselect();
    vertex[fmovestroke][items->length[fmovestroke] - 1] = true;
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::endadd(bool keeplast)
{
    if (!fadding)
        return;
    fadding = false;
    if (!keeplast)
    {
        if (fmovevertex == 1)
            items->delete_stroke(fmovestroke);
        else
            items->delete_vertices(fmovestroke);
    }
    endmove();
}
//---------------------------------------------------------------------------
bool TfEditElem::TStrokeList::onpoint(const TRect &r, int x, int y, bool controlpoints, int *astroke, int *avertex)
{
    int s, v;
    byte c;

    onvertex(r, x, y, false, controlpoints, s, v, c);
    bool res = s >= 0 && v >= 0;

    if (!res && s)
        onstroke(r, x, y, false, s, v);
    if (astroke)
        * astroke = s;
    if (avertex && v >= 0)
        * avertex = v + 1;

    return res;
}
//---------------------------------------------------------------------------
bool TfEditElem::TStrokeList::pointpt(const TRect &r, int x, int y, bool controlpoints, TPoint &pt)
{
    int s, v;
    byte c;

    onvertex(r, x, y, false, controlpoints, s, v, c);
    if (s >= 0 && v >= 0)
    {
        switch (c)
        {
        case 0:
            pt = Point((double)items->points[s][v]->x * r.Width() / 10000.0 + r.Left, (double)items->points[s][v]->y * r.Height() / 10000.0 + r.Top);
            break;
        case 1:
            pt = Point((double)items->points[s][v + 1]->c1x * r.Width() / 10000.0 + r.Left, (double)items->points[s][v + 1]->c1y * r.Height() / 10000.0 + r.Top);
            break;
        case 2:
            pt = Point((double)items->points[s][v]->c2x * r.Width() / 10000.0 + r.Left, (double)items->points[s][v]->c2y * r.Height() / 10000.0 + r.Top);
            break;
        }

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::move(const TRect &r, int x, int y, bool striding)
{
    if (!fmoving)
        THROW(L"Not moving anything!");

    if (!selcount)
        return;

    x = (double)(x - r.Left) * 10000.0 / r.Width();
    y = (double)(y - r.Top) * 10000.0 / r.Height();

    if (x != fmovestart.x || y != fmovestart.y)
        selectpt = Point(99999, 99999);

    if (fmovecp)
    {
        if (fmovecp == 1)
        {
            TElementPoint *ep = items->points[fmovestroke][fmovevertex + 1];
            ep->c1x -= fmovepos.x;
            ep->c1y -= fmovepos.y;

            if (striding && items->points[fmovestroke][fmovevertex]->type == ptLineTo)
            {
                TDoublePoint p1 = DoublePoint(items->points[fmovestroke][fmovevertex - 1]->x, items->points[fmovestroke][fmovevertex - 1]->y);
                TDoublePoint p2 = DoublePoint(items->points[fmovestroke][fmovevertex]->x, items->points[fmovestroke][fmovevertex]->y);
                TDoublePoint p = DoublePoint(x, y);

                p = zlineintersect(p1, p2, p);
                x = p.x + (fmovestart.x - ep->c1x);
                y = p.y + (fmovestart.y - ep->c1y);
            }

            fmovepos = Point(x, y) - fmovestart;

            ep->c1x += fmovepos.x;
            ep->c1y += fmovepos.y;

            if (striding && items->points[fmovestroke][fmovevertex]->type == ptCurve)
            {
                TElementPoint *prep = items->points[fmovestroke][fmovevertex];

                if (fmovecpdist)
                {
                    if (prep->x != ep->c1x)
                    {
                        double m = fabs(prep->y - ep->c1y) / fabs(prep->x - ep->c1x);
                        double dx = sqrt(pow(fmovecpdist, 2) / (1 + pow(m, 2)));
                        prep->c2x = prep->x + dx * (ep->c1x < prep->x ? 1 : -1);
                        prep->c2y = prep->y + dx * m * (ep->c1y < prep->y ? 1 : -1);
                    }
                    else
                    {
                        prep->c2x = prep->x;
                        prep->c2y = prep->y + fmovecpdist * (ep->c1y < prep->y ? 1 : -1);
                    }
                }
            }

        }
        else if (fmovecp == 2)
        {
            TElementPoint *ep = items->points[fmovestroke][fmovevertex];
            ep->c2x -= fmovepos.x;
            ep->c2y -= fmovepos.y;

            if (striding && fmovevertex < items->length[fmovestroke] - 1 && items->points[fmovestroke][fmovevertex + 1]->type == ptLineTo)
            {
                TDoublePoint p1 = DoublePoint(items->points[fmovestroke][fmovevertex]->x, items->points[fmovestroke][fmovevertex]->y);
                TDoublePoint p2 = DoublePoint(items->points[fmovestroke][fmovevertex + 1]->x, items->points[fmovestroke][fmovevertex + 1]->y);
                TDoublePoint p = DoublePoint(x, y);

                p = zlineintersect(p1, p2, p);
                x = p.x + (fmovestart.x - ep->c2x);
                y = p.y + (fmovestart.y - ep->c2y);
            }

            fmovepos = Point(x, y) - fmovestart;

            ep->c2x += fmovepos.x;
            ep->c2y += fmovepos.y;

            if (striding && fmovevertex < items->length[fmovestroke] - 1 && items->points[fmovestroke][fmovevertex + 1]->type == ptCurve)
            {
                TElementPoint *nextep = items->points[fmovestroke][fmovevertex + 1];

                if (fmovecpdist)
                {
                    if (ep->x != ep->c2x)
                    {
                        double m = fabs(ep->y - ep->c2y) / fabs(ep->x - ep->c2x);
                        double dx = sqrt(pow(fmovecpdist, 2) / (1 + pow(m, 2)));
                        nextep->c1x = ep->x + dx * (ep->c2x < ep->x ? 1 : -1);
                        nextep->c1y = ep->y + dx * m * (ep->c2y < ep->y ? 1 : -1);
                    }
                    else
                    {
                        nextep->c1x = ep->x;
                        nextep->c1y = ep->y + fmovecpdist * (ep->c2y < ep->y ? 1 : -1);
                    }
                }
            }

        }
    }
    else
    {
        if (striding)
        {
            TPoint p;
            strideconvert(Point(x, y) - fmovestart, p, true, 5*10000.0 / r.Width());
            p += Point(x, y);
            x = p.x;
            y = p.y;
        }

        moveselected(-fmovepos.x, -fmovepos.y);

        int pl = 99999;
        int pt = 99999;
        int pr = 0;
        int pb = 0;

        for (int ix = 0; ix < count; ++ix)
        {
            if (!stroke[ix])
                continue;

            for (int j = 0; j < items->length[ix]; ++j)
            {
                if (!vertex[ix][j])
                    continue;
                TElementPoint *ep = items->points[ix][j];
                pl = MIN(pl, ep->x);
                pt = MIN(pt, ep->y);
                pr = MAX(pr, ep->x);
                pb = MAX(pb, ep->y);
            }
        }

        fmovepos = Point(x, y) - fmovestart;
        fmovepos.x = MAX(-pl, MIN(10000 - pr, (int)fmovepos.x));
        fmovepos.y = MAX(-pt, MIN(10000 - pb, (int)fmovepos.y));

        moveselected(fmovepos.x, fmovepos.y);
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::moveselected(int dx, int dy)
{
    for (int ix = 0; ix < count; ++ix)
    {
        if (!stroke[ix])
            continue;
        for (int j = 0; j < items->length[ix]; ++j)
        {
            TElementPoint *ep = items->points[ix][j];
            if (ep->type == ptCurve && j && vertex[ix][j - 1])
            {
                ep->c1x += dx;
                ep->c1y += dy;
            }
            if (vertex[ix][j])
            {
                if (ep->type == ptCurve)
                {
                    ep->c2x += dx;
                    ep->c2y += dy;
                }
                ep->x += dx;
                ep->y += dy;
            }
        }
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::connectvertices()
{
    int ix;
    for (ix = count - 1; ix >= 0; --ix)
    {
        if (!stroke[ix])
            continue;
        for (int j = items->length[ix] - 1; j >= 1; --j)
        {
            if (!vertex[ix][j] || !vertex[ix][j - 1] || items->points[ix][j]->type != ptMoveTo)
                continue;
            items->points[ix][j]->type = ptLineTo;
        }
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::deletevertices()
{
    for (int ix = count - 1; ix >= 0; --ix)
        items->delete_vertices(ix);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::deletesegments()
{
    for (int ix = count - 1; ix >= 0; --ix)
        items->delete_segments(ix);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::curvesegments(bool acurve)
{
    for (int ix = count - 1; ix >= 0; --ix)
        items->curve_segments(ix, acurve);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::insertvertex(const TRect &r, int x, int y)
{
    int s, v;
    if (onstroke(r, x, y, false, s, v) >= fthres)
        return;

    double where = 0;
    x = (x - r.Left) * 10000.0 / r.Width();
    y = (y - r.Top) * 10000.0 / r.Height();

    if (items->points[s][v + 1]->type == ptCurve)
    {
        TDoublePoint p1, p2, c1, c2, nc1, nc2, npt;
        p1 = DoublePoint(items->points[s][v]->x, items->points[s][v]->y);
        c1 = DoublePoint(items->points[s][v + 1]->c1x, items->points[s][v + 1]->c1y);
        c2 = DoublePoint(items->points[s][v + 1]->c2x, items->points[s][v + 1]->c2y);
        p2 = DoublePoint(items->points[s][v + 1]->x, items->points[s][v + 1]->y);
        npt = DoublePoint(x, y);
        zbezierdistance(p1, c1, c2, p2, npt, where, false);
    }
    else
    {
        TDoublePoint p1, p2, p;
        p1 = DoublePoint(items->points[s][v]->x, items->points[s][v]->y);
        p2 = DoublePoint(items->points[s][v + 1]->x, items->points[s][v + 1]->y);
        p = DoublePoint(x, y);
        double pd = zpointdistance(p1, p2);
        where = pd ? zpointposition(p1, p2, p) / pd : 0;
    }
    items->split_segment(s, v, where);
    Deselect();
    vertex[s][v + 1] = true;
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::weldselected()
{
    for (int ix = count - 1; ix >= 0; --ix)
        items->weld_selected(ix);
}
//---------------------------------------------------------------------------
bool TfEditElem::TStrokeList::canweld()
{
    bool weld = strokeselcount == 1;
    if (strokeselcount == 1)
        for (int ix = 0; ix < count; ++ix)
        {
            if (!stroke[ix])
                continue;
            weld = !vertex[ix][0] || !vertex[ix][items->length[ix] - 1];
            int first = -1, last = -1;
            for (int k = 1; weld && k < items->length[ix] - 2; ++k)
            {
                if (!vertex[ix][k])
                    continue;

                if (first < 0)
                    first = k;
                else if (!vertex[ix][k - 1])
                {
                    weld = false;
                    break;
                }
                if (vertex[ix][k + 2] && !vertex[ix][k + 1])
                    weld = false;
                last = k + (weld && vertex[ix][k + 2] ? 2 : vertex[ix][k + 1] ? 1 : 0);
            }
            weld = weld && (first < 0 || items->points[ix][first]->type != ptMoveTo || last < items->length[ix] - 1 && items->points[ix][last + 1]->type != ptMoveTo);

            break;
        }
    return weld;
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::movestroke(int pos)
{
    int ix;
    for (ix = 0; ix < count; ++ix)
        if (stroke[ix])
            break;
    if (ix == count || ix == pos)
        return;

    items->move_stroke(ix, pos);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::changetip(TZStrokeType type, bool remove)
{
    int ix;
    for (ix = 0; ix < count; ++ix)
    {
        if (!stroke[ix])
            continue;

        byte &t = items->elem[ix].tips;
        TZStrokeType starttip = stNormalTip;
        TZStrokeType endtip = stNormalTip;

        if (t == stStartPointed || t == stStartPointedEndPointed || t == stStartPointedEndThin)
            starttip = stStartPointed;
        else if (t == stStartThin || t == stStartThinEndPointed || t == stStartThinEndThin)
            starttip = stStartThin;

        if (t == stEndPointed || t == stStartPointedEndPointed || t == stStartThinEndPointed)
            endtip = stEndPointed;
        else if (t == stEndThin || t == stStartPointedEndThin || t == stStartThinEndThin)
            endtip = stEndThin;

        if (remove)
        {
            if (type == stStartPointed || type == stStartThin)
                t = endtip;
            if (type == stEndPointed || type == stEndThin)
                t = starttip;
            continue;
        }

        if (type == stSingleDot)
        {
            t = stSingleDot;
            continue;
        }

        if (type == stStartPointed || type == stStartThin)
            starttip = type;
        else if (type == stEndPointed || type == stEndThin)
            endtip = type;

        if (starttip == stNormalTip)
            t = endtip;
        else if (endtip == stNormalTip)
            t = starttip;
        else
        {
            if (starttip == stStartPointed)
            {
                if (endtip == stEndPointed)
                    t = stStartPointedEndPointed;
                else
                    t = stStartPointedEndThin;
            }
            else if (starttip == stStartThin)
            {
                if (endtip == stEndPointed)
                    t = stStartThinEndPointed;
                else
                    t = stStartThinEndThin;
            }
        }
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::alignvertex(TVertexAlignment alignment, int coord)
{
    if (alignment != vaHorz && alignment != vaVert)
        THROW(L"Alignment argument makes no sense!");
    for (int ix = count - 1; ix >= 0; --ix)
    {
        if (!stroke[ix])
            continue;
        for (int j = items->length[ix] - 1; j >= 0; --j)
        {
            if (!vertex[ix][j])
                continue;
            if (alignment == vaVert)
                items->points[ix][j]->y = coord;
            else if (alignment == vaHorz)
                items->points[ix][j]->x = coord;
        }
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::alignvertex(TVertexAlignment alignment, int astroke, int avert)
{
    if (alignment != vaHorz && alignment != vaVert)
        THROW(L"Alignment argument makes no sense!");

    alignvertex(alignment, alignment == vaHorz ? items->points[astroke][avert]->x : items->points[astroke][avert]->y);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::alignvertex(TVertexAlignment alignment)
{
    if (alignment != vaHorz && alignment != vaVert)
        THROW(L"Alignment argument makes no sense!");

    int x = 0;
    int y = 0;

    for (int ix = count - 1; ix >= 0; --ix)
    {
        if (!stroke[ix])
            continue;
        for (int j = items->length[ix] - 1; j >= 0; --j)
        {
            if (!vertex[ix][j])
                continue;

            x += items->points[ix][j]->x;
            y += items->points[ix][j]->y;
        }
    }
    alignvertex(alignment, alignment == vaHorz ? x / selcount : y / selcount);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::alignvertex(TVertexAlignment alignment, const TRect &r)
{
    if (alignment != vaHorzFrame && alignment != vaVertFrame)
        THROW(L"Alignment argument makes no sense!");

    int d1x = 10000;
    int d1y = 10000;
    int d2x = 0;
    int d2y = 0;

    for (int ix = count - 1; ix >= 0; --ix)
    {
        if (!stroke[ix])
            continue;
        for (int j = items->length[ix] - 1; j >= 0; --j)
        {
            if (!vertex[ix][j])
                continue;
            d1x = MIN(d1x, items->points[ix][j]->x);
            d2x = MAX(d2x, items->points[ix][j]->x);
            d1y = MIN(d1y, items->points[ix][j]->y);
            d2y = MAX(d2y, items->points[ix][j]->y);
        }
    }

    int dx = (10000.0 - d2x + d1x) / 2 - d1x;
    int dy = (10000.0 - d2y + d1y) / 2 - d1y;

    moveselected(alignment == vaHorzFrame ? dx : 0, alignment == vaVertFrame ? dy : 0);
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::reversestrokes()
{
    for (int ix = count - 1; ix >= 0; --ix)
    {
        if (!stroke[ix])
            continue;

        items->reverse_stroke(ix);
    }
}
//---------------------------------------------------------------------------
void TfEditElem::TStrokeList::selectwholestroke(int ix, bool select)
{
    for (int j = items->length[ix] - 1; j >= 0; --j)
        vertex[ix][j] = select;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TfEditElem::TfEditElem(TComponent* Owner) : cnumbers(false), knumbers(false), destroying(false), TBaseForm(Owner)
{
    noupdate = false;
    EraseBackground = false;

    miGen->Enabled = !hasrecdata && models->Count;

    Constrain(572*scaleres + (Width - ClientWidth), 346*scaleres + (Height - ClientHeight));

    edPart1->Color = (TColor)colorsettings.background;
    edPart2->Color = (TColor)colorsettings.background;
    edPart3->Color = (TColor)colorsettings.background;
    edPart4->Color = (TColor)colorsettings.background;
    edPart1->Font->Color = (TColor)colorsettings.maintext;
    edPart2->Font->Color = (TColor)colorsettings.maintext;
    edPart3->Font->Color = (TColor)colorsettings.maintext;
    edPart4->Font->Color = (TColor)colorsettings.maintext;
    pVar1->Color = (TColor)colorsettings.background;
    pVar2->Color = (TColor)colorsettings.background;
    pVar3->Color = (TColor)colorsettings.background;
    pVar4->Color = (TColor)colorsettings.background;
    pPart1->Color = (TColor)colorsettings.background;
    pPart2->Color = (TColor)colorsettings.background;
    pPart3->Color = (TColor)colorsettings.background;
    pPart4->Color = (TColor)colorsettings.background;

    numElem = new TNumEdit(edElem, 1, elements->Count);
    numElem->callback = ncallback;

    numPart1 = new TNumEdit(edPart1, 0, elements->Count);
    numPart2 = new TNumEdit(edPart2, 0, elements->Count);
    numPart3 = new TNumEdit(edPart3, 0, elements->Count);
    numPart4 = new TNumEdit(edPart4, 0, elements->Count);
    numPart1->callback = n1callback;
    numPart2->callback = n2callback;
    numPart3->callback = n3callback;
    numPart4->callback = n4callback;

    numCentre = new TNumEdit(edCentre, 0, 0);
    numCentre->callback = ccallback;

    numUNICODE = new THexEdit(cbUNICODE);
    numUNICODE->allow_empty = true;
    numUNICODE->callback = &jcallback;

    varnames = new TStringList;

    bmp = new TBmp;
    back = new TBmp;
    elem = NULL;
    felemindex = -1;
    linepos.x = 99999;
    linepos.y = 99999;
    fundo = new TUndoItems(&savevarname, &undovarname);

    bgchar = 0;
    vstate = 0;

    setunchanged();

    edCentre->Hint = (UnicodeString)"Specify the stroke that should only be written after all other\n" + "elements finished drawing in the parent element. (eg. the bottom of a box.)\n" + "\n0 = no centre";
    egrid = new TElementGrid(pbParents, (pbParents->ClientWidth - 6) / 2, ecallback);
    egrid->scrollsize = 6;
    egrid->showvalue = true;

    pgrid1 = new TVariantGrid(pbPart1, pbPart1->Width, false, false, &pcallback, &pgiveelem1);
    pgrid1->scrollbars = false;
    pgrid2 = new TVariantGrid(pbPart2, pbPart2->Width, false, false, &pcallback, &pgiveelem2);
    pgrid2->scrollbars = false;
    pgrid3 = new TVariantGrid(pbPart3, pbPart3->Width, false, false, &pcallback, &pgiveelem3);
    pgrid3->scrollbars = false;
    pgrid4 = new TVariantGrid(pbPart4, pbPart4->Width, false, false, &pcallback, &pgiveelem4);
    pgrid4->scrollbars = false;

    vgrid = new TVariantGrid(pbVariants, pbVariants->ClientWidth - 6, true, false, &vcallback, &vgiveelem);
    vgrid->scrollsize = 6;
    rgrid = new TVariantGrid(pbRepository, (pbRepository->ClientWidth - 6) / 2, true, true, &rcallback, &rgiveelem);
    rgrid->scrollsize = 6;

    mgrid = new TStrokeModelGrid(pbModels, (pbModels->ClientWidth - 6) / 2, &mcallback);
    //mgrid->orientation = goHorizontal;
    mgrid->scrollsize = 6;
    mgrid->set_models(sbKModels->Down ? models : cmodels);
    mgrid->numproc = &strokenumproc;

    update_repositories(0);

    faction = eaNone;

    dlgOpen->InitialDir = paths.datapath;
    dlgSave->InitialDir = paths.datapath;

    AppActivateEvent(this, deactivated, false, true);
    AppActivateEvent(this, activated, true, true);

    strokelist = new TStrokeList;
    strokelist->threshold = 8;
    strokelist->vertsize = 4;

    partlist = new TPartList;
    partlist->threshold = 6;

    if (elements->Version < 1)
        elements->RepositionAll();
}
//---------------------------------------------------------------------------
__fastcall TfEditElem::~TfEditElem()
{
    TMemIniFile *ini = new TMemIniFile(paths.inipath + L"elements.ini", TEncoding::UTF8);
    try
    {
        ini->WriteBool("view", "show font background", elementsettings.kanjibackground);
        ini->WriteBool("view", "show stroke order", elementsettings.strokenumbers);
        ini->WriteBool("fonts", "background font force centered", elementsettings.forcecentre);
        ini->WriteString("fonts", "background font name", elementsettings.kanjifont);
        ini->WriteInteger("fonts", "background font charset", elementsettings.kanjicharset);
        ini->WriteInteger("fonts", "background font color", elementsettings.kanjicolor);
        UnicodeString tmpfontsize = elementsettings.kanjisize;
        int dp = tmpfontsize.Pos(locale.decimal);
        if (dp > 0)
            tmpfontsize[dp] = '.';
        ini->WriteString("fonts", "background font size", tmpfontsize);
        ini->WriteString("files", "last", elementsettings.lastfilename);
    }
    catch (...)
    {;
    }
    ini->UpdateFile();
    delete ini;

    delete fundo;
    fundo = 0;

    AppActivateEvent(this, deactivated, false, false);
    AppActivateEvent(this, activated, true, false);

    delete strokelist;
    delete partlist;

    elem_free(elem);

    delete[] vstate;

    delete numElem;
    delete numPart1;
    delete numPart2;
    delete numPart3;
    delete numPart4;
    delete egrid;
    delete pgrid1;
    delete pgrid2;
    delete pgrid3;
    delete pgrid4;
    delete vgrid;
    delete rgrid;
    delete numCentre;
    delete numUNICODE;
    delete mgrid;

    delete varnames;

    delete bmp;
    delete back;

    clear_recparts();
}
//---------------------------------------------------------------------------
int TfEditElem::ShowModalEx(int aindex)
{
    if (!elements->Count)
        addfirst();
    sbBg->Down = elementsettings.kanjibackground;
    sbNumbers->Down = elementsettings.strokenumbers;
    sbNumbersClick(sbNumbers);
    elemindex = aindex;
    ncallback();

    dlgOpen->FileName = elementsettings.lastfilename;
    dlgSave->FileName = elementsettings.lastfilename;
    if (elementsettings.lastfilename.Length())
        Caption = (UnicodeString)"Stroke order diagram editor" + " - " + elementsettings.lastfilename;

    int r = mrCancel;
    MODON
    try
    {
        r = ShowModal();
    }
    catch (...)
    {;
    }
    MODOFF

    return r;
}
//---------------------------------------------------------------------------
void TfEditElem::AddUNICODE(TKanji *k)
{
    TStrings *s = cbUNICODE->Items;
    s->BeginUpdate();
    try
    {
        s->AddObject(wtoAS(k->_uni), (TObject*)k);
    }
    catch (...)
    {;
    }
    s->EndUpdate();
}
//---------------------------------------------------------------------------
void TfEditElem::addfirst()
{
    TKanjiElement *first = new TKanjiElement;
    memset(first, 0, sizeof(TKanjiElement));
    for (int ix = 0; ix < 4; ++ix)
        first->parts[ix] = -1;
    first->variantcount = 1;
    first->variants = new TKanjiVariant[1];
    memset(first->variants, 0, sizeof(TKanjiVariant));
    first->variants[0].standalone = true;
    elements->Add(first);
}
//---------------------------------------------------------------------------
void TfEditElem::deactivated(void *caller)
{
    if (sbShownBg->Down)
        sbShownBg->Down = false;

    if (fElemSettings)
        fElemSettings->Reset();

    strokelist->endadd(false);
    strokelist->endmove();
    if (faction == eaMoving || faction == eaAdding)
    {
        if (elemview == evSingle)
            fundo->StrokeChanged();
        else if (elemview == evCompound)
            fundo->PositionChanged();
    }
    faction = eaNone;
    mousehit = mhNone;
    pbEditor->Invalidate();
}
//---------------------------------------------------------------------------
void TfEditElem::activated(void *caller)
{
}
//---------------------------------------------------------------------------
void TfEditElem::ncallback()
{
    controlinit();
    elemindex = edElem->Text.ToInt() - 1;
}
//---------------------------------------------------------------------------
void TfEditElem::vcallback(int element, byte variant)
{
    sbToRep->Enabled = elem && elem->variantcount > variant && elem->variants[variant].strokecount && elements->RepositoryCount;
    if (varindex == variant)
        return;
    if (!partquery())
    {
        vgrid->pos = varindex;
        return;
    }

    varindex = variant;
}
//---------------------------------------------------------------------------
void TfEditElem::pcallback(int element, byte variant)
{
    ReleaseCapture();
    numElem->value = element + 1;
    vgrid->pos = variant;
}
//---------------------------------------------------------------------------
void TfEditElem::ecallback(int aelem)
{
    ReleaseCapture();
    if (sbShownBg->Down)
    {
        sbShownBg->Down = false;
        if (aelem >= 0 && aelem < elements->Count)
        {
            if (elements->Items[aelem]->owner < NO_OWNER)
            {
                bgchar = kanjis->Kanjis[elements->Items[aelem]->owner]->ch;
                redrawbg();
            }
            else if (elements->Items[aelem]->owner > NO_OWNER)
            {
                bgchar = elements->Items[aelem]->owner;
                redrawbg();
            }
        }
        return;
    }
    numElem->value = aelem + 1;
}
//---------------------------------------------------------------------------
void TfEditElem::n1callback()
{
    setpart(0, numPart1->value - 1);
}
//---------------------------------------------------------------------------
void TfEditElem::n2callback()
{
    setpart(1, numPart2->value - 1);
}
//---------------------------------------------------------------------------
void TfEditElem::n3callback()
{
    setpart(2, numPart3->value - 1);
}
//---------------------------------------------------------------------------
void TfEditElem::n4callback()
{
    setpart(3, numPart4->value - 1);
}
//---------------------------------------------------------------------------
void TfEditElem::ccallback()
{
    if (noupdate || elem->variants[fvarindex].centrepoint == numCentre->value || !elem->variants[fvarindex].standalone)
        return;

    elem->variants[fvarindex].centrepoint = numCentre->value;
    fundo->CentreChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::btnUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    numElem->value--;

    MouseRepeat(btnUpMouseDown, Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::btnDownMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    numElem->value++;

    MouseRepeat(btnDownMouseDown, Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void TfEditElem::setunchanged()
{
    delete[] vstate;

    if (!elem)
        return;

    vstate = new TVariantChangeState[elem->variantcount];

    for (int ix = 0; ix < elem->variantcount; ++ix)
        vstate[ix] = vsUntouched;
}
//---------------------------------------------------------------------------
bool TfEditElem::changed()
{
    for (int ix = 0; ix < elem->variantcount; ++ix)
        if (vstate[ix] != vsUntouched)
            return true;
    return false;
}
//---------------------------------------------------------------------------
bool TfEditElem::cansave()
{
    bool noempty = elem->variantcount > 0;
    for (int ix = 0; ix < elem->variantcount && noempty; ++ix)
    {
        if (elem->variants[ix].standalone && !elem->variants[ix].strokecount || !elem->variants[ix].standalone && elem->parts[0] < 0)
            noempty = false;
    }
    return noempty;
}
//---------------------------------------------------------------------------
void TfEditElem::set_elemindex(int aindex)
{
    if (felemindex == aindex)
        return;

    varnames->Clear();

    TVariantGrid *pgrids[5] = { pgrid1, pgrid2, pgrid3, pgrid4, vgrid };
    for (int ix = 0; ix < (int)(sizeof(pgrids) / sizeof(TVariantGrid*)); ++ix)
        pgrids[ix]->empty();

    mousehit = mhNone;
    pbEditor->Cursor = crCross;
    felemindex = aindex;
    if (elements->Items[aindex]->owner < NO_OWNER)
        bgchar = kanjis->Kanjis[elements->Items[aindex]->owner]->ch;
    else if (elements->Items[aindex]->owner > NO_OWNER)
        bgchar = elements->Items[aindex]->owner;
    else
        bgchar = 0;
    redrawbg();

    strokelist->Clear();
    partlist->Clear();

    elem_free(elem);
    elem = elem_copy(elements->Items[aindex], false);

    cbUNICODE->Text = elem->owner < NO_OWNER ? wtoAS(kanjis->Kanjis[elem->owner]->_uni) : elem->owner > NO_OWNER ? wtoAS(elem->owner) : (UnicodeString)"";

    edElem->Text = aindex + 1;
    egrid->set_items(elem->parents, elem->parentcnt);

    setunchanged();
    fvarindex = -1;

    btnSaveVarName->Enabled = elem->variantcount;
    if (elem->variantcount)
    {
        for (int ix = 0; ix < elem->variantcount; ++ix)
            varnames->Add(elements->VariantName(aindex, ix, false));
        vgrid->set_items(elem->variantcount);
    }
    else
        vgrid->empty();

    varindex = 0;

    cbPattern->ItemIndex = elem->pattern;

    fill_recparts();
}
//---------------------------------------------------------------------------
void TfEditElem::updatevarbuttons()
{
    bool hasempty = false;
    for (int ix = 0; ix < elem->variantcount && !hasempty; ++ix)
        hasempty = !elem->variants[ix].strokecount;
    sbAddVariant->Enabled = elem->variantcount < 32 && !hasempty;
    miDupVar->Enabled = sbAddVariant->Enabled;
    sbDelVariant->Enabled = elem->variantcount > 1;
    miDelVar->Enabled = sbDelVariant->Enabled;
}
//---------------------------------------------------------------------------
void TfEditElem::set_varindex(int aindex)
{
    if (aindex >= elem->variantcount)
    {
        aindex = -1;
        edVariant->Text = L"";
    }

    if (aindex == fvarindex || aindex < 0)
        return;

    if (vgrid->pos != aindex)
    {
        vgrid->pos = aindex;
        return;
    }

    if (fvarindex >= 0 && vstate[fvarindex] == vsModified)
        vstate[fvarindex] = vsFinal;
    fvarindex = aindex;
    cbStandalone->Checked = elem->variants[aindex].standalone;
    setcentre();

    edVariant->Text = varnames->Count > fvarindex ? varnames->Strings[fvarindex] : UnicodeString(L"");

    fundo->SetItem(elem, fvarindex);

    update_points();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::pbEditorPaint(TObject *Sender)
{
    pbEditor->Canvas->Draw(0, 0, bmp);
    TCanvas *c = pbEditor->Canvas;

    c->Pen->Mode = pmNotXor;
    if (elemview == evSingle)
    {
        if (faction == eaSelecting)
        {
            c->Pen->Width = 1;
            c->Pen->Style = psDot;
            c->Brush->Style = bsClear;
            c->Pen->Color = clBlack;
            c->MoveTo(mousestart.x, mousestart.y);
            c->LineTo(mousepos.x, mousestart.y);
            c->LineTo(mousepos.x, mousepos.y);
            c->LineTo(mousestart.x, mousepos.y);
            c->LineTo(mousestart.x, mousestart.y);

            c->Brush->Style = bsSolid;
            c->Pen->Style = psSolid;
        }
        else if (faction == eaNone && mousehit != mhNone)
        {
            c->Pen->Width = 1;
            c->Pen->Color = clGreen;

            if (mousehit == mhLine)
            {
                int xsize = strokelist->threshold;
                c->MoveTo(linepos.x - xsize, linepos.y - xsize);
                c->LineTo(linepos.x + xsize, linepos.y + xsize);
                c->MoveTo(linepos.x + xsize, linepos.y - xsize);
                c->LineTo(linepos.x - xsize, linepos.y + xsize);
            }
            else if (mousehit == mhPoint)
            {
                int vsize = strokelist->vertsize + 1;
                c->MoveTo(linepos.x - vsize, linepos.y - vsize);
                c->LineTo(linepos.x + vsize - 1, linepos.y - vsize);
                c->LineTo(linepos.x + vsize - 1, linepos.y + vsize - 1);
                c->LineTo(linepos.x - vsize, linepos.y + vsize - 1);
                c->LineTo(linepos.x - vsize, linepos.y - vsize);
            }
        }
    }
    else if (elemview == evCompound)
    {
        if (faction == eaSelecting)
        {
            c->Pen->Width = 1;
            c->Pen->Style = psDot;
            c->Pen->Color = clBlack;
            c->Brush->Style = bsClear;
            c->MoveTo(mousestart.x, mousestart.y);
            c->LineTo(mousepos.x, mousestart.y);
            c->LineTo(mousepos.x, mousepos.y);
            c->LineTo(mousestart.x, mousepos.y);
            c->LineTo(mousestart.x, mousestart.y);
            c->Pen->Style = psSolid;
            c->Brush->Style = bsSolid;
        }
        else if (faction == eaNone && mousehit != mhNone)
        {
            c->Pen->Width = 1;
            c->Pen->Color = clGray;
            if (mousehit != mhNone)
            {
                c->MoveTo(partpos.Left - 2, partpos.Top - 2);
                c->LineTo(partpos.Right + 2, partpos.Top - 2);
                c->LineTo(partpos.Right + 2, partpos.Bottom + 2);
                c->LineTo(partpos.Left - 2, partpos.Bottom + 2);
                c->LineTo(partpos.Left - 2, partpos.Top - 2);
            }

        }
    }
    c->Pen->Mode = pmCopy;
}
//---------------------------------------------------------------------------
void TfEditElem::update_points()
{
    if (noupdate)
        return;

    faction = eaNone;

    if (!elem->variantcount)
        elemview = evEmpty;
    else if (elem->variants[varindex].standalone)
    {
        elemview = evSingle;
        strokelist->Add(elem, felemindex, fvarindex);
    }
    else
    {
        elemview = evCompound;
        partlist->Add(elem, felemindex, fvarindex);
    }

    TVariantGrid *pgrids[4] = { pgrid1, pgrid2, pgrid3, pgrid4 };
    TNumEdit *nparts[4] = { numPart1, numPart2, numPart3, numPart4 };
    TSpeedButton *vbtn[4] = { sbVar1, sbVar2, sbVar3, sbVar4 };
    for (int ix = 0; ix < 4; ++ix)
    {
        if (elem->parts[ix] < 0 || elem->variantcount == 0 || elements->Items[elem->parts[ix]]->variantcount <= (elem->variants[fvarindex].standalone ? 0 : elem->variants[fvarindex].positions[ix].variant))
        {
            pgrids[ix]->empty();
            vbtn[ix]->Enabled = false;
        }
        else
        {
            pgrids[ix]->set_items(1);
            vbtn[ix]->Enabled = !elem->variants[fvarindex].standalone && elements->Items[elem->parts[ix]]->variantcount > 1;
        }
        nparts[ix]->value = elem->parts[ix] + 1;
    }

}
//---------------------------------------------------------------------------
void TfEditElem::elemchanged()
{
    update_points();
    cbStandalone->Checked = elem->variants[varindex].standalone;
    cbPattern->ItemIndex = elem->pattern;
    edCentre->Text = elem->variants[varindex].centrepoint;

    setundo();
    redraw();
}
//---------------------------------------------------------------------------
void TfEditElem::redrawbg()
{
    const blocks = 12; //must be even number

    bool sizechange = back->Width != pbEditor->ClientWidth || back->Height != pbEditor->ClientHeight;
    if (sizechange)
        back->SetSize(pbEditor->ClientWidth, pbEditor->ClientHeight);

    double block = (double)(MIN(pbEditor->ClientWidth, pbEditor->ClientHeight) + 1) / blocks;
    bmprect = Rect(block, block, block * (blocks - 1) - 1, block * (blocks - 1) - 1);
    OffsetRect(bmprect, (pbEditor->ClientWidth - bmprect.Width()) / 2 - block, (pbEditor->ClientHeight - bmprect.Height()) / 2 - block);

    if (sizechange)
    {
        strokelist->threshold = bmprect.Width() / 50;
        strokelist->vertsize = MAX(4, strokelist->threshold / 2);
        partlist->threshold = strokelist->threshold * 2 / 3;
    }

    TCanvas *c = back->Canvas;
    c->Brush->Color = clWhite;
    c->FillRect(pbEditor->ClientRect);

    /*draw the grid*/

    if (sbBg->Down && bgchar != 0)
    {
        c->Font->Name = elementsettings.kanjifont;
        c->Font->Charset = elementsettings.kanjicharset;
        c->Font->Color = elementsettings.kanjicolor;
        c->Font->Size = bmprect.Height() * elementsettings.kanjisize;
        c->Font->Style = TFontStyles();
        SetTextAlign(c->Handle, TA_LEFT | TA_TOP);

        if (!elementsettings.forcecentre)
            DrawTextW(c->Handle, &bgchar, 1, &bmprect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        else
        {
            TBmp *tmp = new TBmp;
            try
            {
                tmp->SetSize(back->Width, back->Height + 200);
                int w = tmp->Width;
                int h = tmp->Height;

                TCanvas *tc = tmp->Canvas;
                tc->Font->Assign(c->Font);
                tc->Brush->Color = clBlack;
                tc->Font->Color = clWhite;
                tmp->PixelFormat = pf1bit;
                tmp->Monochrome = true;

                SetTextAlign(tc->Handle, TA_CENTER | TA_BASELINE);
                TRect r = Rect(0, 0, w, h);
                ExtTextOutW(tc->Handle, w / 2, (h - 200)*0.8 + 100, ETO_CLIPPED | ETO_OPAQUE, &r, &bgchar, 1, NULL);

                int top = -1, bottom = -1;

                char *lines = (char*)tmp->ScanLine[0];
                int linelen = (char*)tmp->ScanLine[1] - lines;
                //scan tmp for first and last line being painted on
                for (int m = 0; m < h && top < 0; ++m)
                    for (int l = 0; l < (w + 7) / 8 && top < 0; ++l)
                    {
                        char *c = lines + linelen * m;
                        if (c[l] != 0)
                            top = m;
                    }
                for (int m = h - 1; m >= 0 && bottom < 0; --m)
                    for (int l = 0; l < (w + 7) / 8 && bottom < 0; ++l)
                    {
                        char *c = lines + linelen * m;
                        if (c[l] != 0)
                            bottom = m;
                    }

                if (top >= 0 && bottom > top)
                {
                    int delta = h / 2 - (top + bottom) / 2;
                    SetTextAlign(c->Handle, TA_CENTER | TA_BASELINE);
                    ExtTextOutW(c->Handle, back->Width / 2, back->Height*0.8 + delta, ETO_CLIPPED | ETO_OPAQUE, &bmprect, &bgchar, 1, NULL);
                }

            }
            catch (...)
            {;
            }
            delete tmp;
        }
    }

    c->Pen->Width = 1;
    c->Pen->Color = (TColor)(0xe0e0e0);
    c->Pen->Style = psDot;
    c->Brush->Style = bsClear;

    double x = (double)bmprect.Left + block - 1, y = (double)bmprect.Top + block - 1;

    for (int ix = 0; ix < blocks - 1; ++ix)
    {
        c->MoveTo(bmprect.Left - block, y - block);
        c->LineTo(bmprect.Right + block, y - block);
        c->MoveTo(x - block, bmprect.Top - block);
        c->LineTo(x - block, bmprect.Bottom + block);

        x += block;
        y += block;
    }
    c->Pen->Style = psSolid;
    c->Brush->Style = bsSolid;
    c->Pen->Color = clSilver;

    c->MoveTo(bmprect.Left, bmprect.Top + block*5 - 1);
    c->LineTo(bmprect.Right, bmprect.Top + block*5 - 1);
    c->MoveTo(bmprect.Left + block*5 - 1, bmprect.Top);
    c->LineTo(bmprect.Left + block*5 - 1, bmprect.Bottom);

    c->Pen->Color = clGray;
    c->MoveTo(bmprect.Left, bmprect.Top);
    c->LineTo(bmprect.Right, bmprect.Top);
    c->LineTo(bmprect.Right, bmprect.Bottom);
    c->LineTo(bmprect.Left, bmprect.Bottom);
    c->LineTo(bmprect.Left, bmprect.Top);
    c->MoveTo(bmprect.Left - 1, bmprect.Top - 1);
    c->LineTo(bmprect.Right + 1, bmprect.Top - 1);
    c->LineTo(bmprect.Right + 1, bmprect.Bottom + 1);
    c->LineTo(bmprect.Left - 1, bmprect.Bottom + 1);
    c->LineTo(bmprect.Left - 1, bmprect.Top - 1);
    /*-------------*/

    redraw();
}
//---------------------------------------------------------------------------
void TfEditElem::redraw()
{
    if (destroying)
        return;

    if (bmp->Width != pbEditor->ClientWidth || bmp->Height != pbEditor->ClientHeight)
        bmp->SetSize(pbEditor->ClientWidth, pbEditor->ClientHeight);

    TCanvas *c = bmp->Canvas;
    c->Brush->Color = clWhite;

    c->Draw(0, 0, back);

    if (elemview == evSingle)
        strokelist->draw(c, bmprect);
    else if (elemview == evCompound)
        partlist->draw(c, bmprect);

    pbEditor->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::btnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miDelVertexClick(TObject *Sender)
{
    strokelist->deletevertices();
    if (elem->variants[fvarindex].standalone && elem->variants[fvarindex].centrepoint > elem->variants[fvarindex].strokecount)
        elem->variants[fvarindex].centrepoint = elem->variants[fvarindex].strokecount;
    fundo->StrokeChanged();
    redraw();
    setcentre();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miDelSegmentClick(TObject *Sender)
{
    strokelist->deletesegments();
    if (elem->variants[fvarindex].standalone && elem->variants[fvarindex].centrepoint > elem->variants[fvarindex].strokecount)
        elem->variants[fvarindex].centrepoint = elem->variants[fvarindex].strokecount;
    fundo->StrokeChanged();
    redraw();
    setcentre();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miCurveClick(TObject *Sender)
{
    strokelist->curvesegments(!((TMenuItem*)Sender)->Checked);
    fundo->StrokeChanged();
    redraw();
    setcentre();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miInsertClick(TObject *Sender)
{
    strokelist->insertvertex(bmprect, mousepos.x, mousepos.y);
    fundo->StrokeChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miWeldClick(TObject *Sender)
{
    strokelist->weldselected();
    fundo->StrokeChanged();
    redraw();
}
//---------------------------------------------------------------------------
void TfEditElem::setuptipsmenu(TZStrokeType stype)
{
    if (stype == stSingleDot)
    {
        miTipS1->Checked = false;
        miTipS2->Checked = false;
        miTipS3->Checked = false;
        miTipE1->Checked = false;
        miTipE2->Checked = false;
        miTipE3->Checked = false;

        miTipD->Checked = true;
        return;
    }

    miTipD->Checked = false;

    if (stype == stStartThin || stype == stStartThinEndPointed || stype == stStartThinEndThin)
        miTipS2->Checked = true;
    else if (stype == stStartPointed || stype == stStartPointedEndPointed || stype == stStartPointedEndThin)
        miTipS3->Checked = true;
    else
        miTipS1->Checked = true;

    if (stype == stEndThin || stype == stStartThinEndThin || stype == stStartPointedEndThin)
        miTipE2->Checked = true;
    else if (stype == stEndPointed || stype == stStartThinEndPointed || stype == stStartPointedEndPointed)
        miTipE3->Checked = true;
    else
        miTipE1->Checked = true;
}
//---------------------------------------------------------------------------
void TfEditElem::fillalign(TMenuItem *mvert, TMenuItem *mhorz)
{
    while (mvert->Count)
        delete mvert->Items[0];
    while (mhorz->Count)
        delete mhorz->Items[0];

    if (elemview == evSingle)
    {
        for (int ix = 0; ix < strokelist->count; ++ix)
        {
            if (!strokelist->stroke[ix])
                continue;
            for (int j = 0; j < strokelist->length[ix]; ++j)
            {
                if (!strokelist->vertex[ix][j])
                    continue;
                TMenuItem *i = new TMenuItem(mvert);
                i->Caption = (UnicodeString)"Vert. to S" + (ix + 1) + " V" + (j + 1);
                i->Tag = (ix & 0xffff) | ((j & 0xffff) << 16);
                i->OnClick = vertToVertex;
                if (!(mvert->Count % 8) && mvert->Count)
                    i->Break = mbBreak;
                mvert->Add(i);
                i = new TMenuItem(mhorz);
                i->Caption = (UnicodeString)"Horiz. to S" + (ix + 1) + " V" + (j + 1);
                i->Tag = (ix & 0xffff) | ((j & 0xffff) << 16);
                i->OnClick = horzToVertex;
                if (!(mhorz->Count % 8) && mhorz->Count)
                    i->Break = mbBreak;
                mhorz->Add(i);
            }
        }
    }
    else if (elemview == evCompound)
    {
        for (int ix = 0; ix < 4 && elem->parts[ix] >= 0; ++ix)
        {
            if (!partlist->part[ix])
                continue;
            TMenuItem *i = new TMenuItem(mvert);
            i->Caption = (UnicodeString)"part " + (ix + 1);
            i->Tag = ix;
            i->OnClick = vertToPart;
            mvert->Add(i);
            i = new TMenuItem(mhorz);
            i->Caption = (UnicodeString)"part " + (ix + 1);
            i->Tag = ix;
            i->OnClick = horzToPart;
            mhorz->Add(i);
        }
    }
}
//---------------------------------------------------------------------------
void TfEditElem::fillposition(TMenuItem *mpos)
{
    while (mpos->Count)
        delete mpos->Items[0];

    mpos->Enabled = strokelist->count > 1 && strokelist->strokeselcount == 1;
    if (!mpos->Enabled)
        return;

    for (int ix = 0; ix < strokelist->count; ++ix)
    {
        if (strokelist->stroke[ix])
            continue;
        TMenuItem *i = new TMenuItem(mpos);
        i->Caption = (UnicodeString)(ix + 1);
        i->OnClick = strokeToPos;
        i->Tag = ix;
        if (!(mpos->Count % 8) && mpos->Count)
            i->Break = mbBreak;
        mpos->Add(i);
    }
}
//---------------------------------------------------------------------------
void TfEditElem::fillresize(TMenuItem *mwidth, TMenuItem *mheight)
{
    while (mwidth->Count)
        delete mwidth->Items[0];
    while (mheight->Count)
        delete mheight->Items[0];

    if (elemview == evCompound)
    {
        for (int ix = 0; ix < 4 && elem->parts[ix] >= 0; ++ix)
        {
            if (!partlist->part[ix])
                continue;
            TMenuItem *i = new TMenuItem(mwidth);
            i->Caption = (UnicodeString)"part " + (ix + 1);
            i->Tag = ix;
            i->OnClick = widthToPart;
            mwidth->Add(i);
            i = new TMenuItem(mheight);
            i->Caption = (UnicodeString)"part " + (ix + 1);
            i->Tag = ix;
            i->OnClick = heightToPart;
            mheight->Add(i);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::widthToPart(TObject *Sender)
{
    partlist->resizepart(prWidth, elem->variants[fvarindex].positions[((TMenuItem*)Sender)->Tag].width);
    fundo->PositionChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::heightToPart(TObject *Sender)
{
    partlist->resizepart(prHeight, elem->variants[fvarindex].positions[((TMenuItem*)Sender)->Tag].height);
    fundo->PositionChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::vertToVertex(TObject *Sender)
{
    int s = ((TMenuItem*)(Sender))->Tag & 0xffff;
    int v = (((TMenuItem*)(Sender))->Tag & 0xffff0000) >> 16;
    strokelist->alignvertex(vaVert, s, v);
    fundo->StrokeChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::horzToVertex(TObject *Sender)
{
    int s = ((TMenuItem*)(Sender))->Tag & 0xffff;
    int v = (((TMenuItem*)(Sender))->Tag & 0xffff0000) >> 16;

    strokelist->alignvertex(vaHorz, s, v);
    fundo->StrokeChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::vertToPart(TObject *Sender)
{
    partlist->alignpart(paVert, ((TMenuItem*)Sender)->Tag);
    fundo->PositionChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::horzToPart(TObject *Sender)
{
    partlist->alignpart(paHorz, ((TMenuItem*)Sender)->Tag);
    fundo->PositionChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::strokeToPos(TObject *Sender)
{
    int p = ((TMenuItem*)Sender)->Tag;
    strokelist->movestroke(p);
    fundo->StrokeChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miVertClick(TObject *Sender)
{
    strokelist->alignvertex(vaVert);
    fundo->StrokeChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miHorzClick(TObject *Sender)
{
    strokelist->alignvertex(vaHorz);
    fundo->StrokeChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miVertFrameClick(TObject *Sender)
{
    strokelist->alignvertex(vaVertFrame, bmprect);
    fundo->StrokeChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miHorzFrameClick(TObject *Sender)
{
    strokelist->alignvertex(vaHorzFrame, bmprect);
    fundo->StrokeChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miConnectMulClick(TObject *Sender)
{
    strokelist->connectvertices();
    fundo->StrokeChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::pmVertexPopup(TObject *Sender)
{
    popupshown = true;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miReverseClick(TObject *Sender)
{
    strokelist->reversestrokes();
    fundo->StrokeChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::pbEditorMouseLeave(TObject *Sender)
{
    if (popupshown)//only call mouseleave when we don't show a popupmenu from the editor
                return;

    mousehit = mhNone;
    mousepos = Point(99999, 99999);
    pbEditor->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::pbEditorMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    if (pbEditor->Tag)
        return;

    pbEditor->Tag = 1;

    try
    {
        mousepos = Point(x, y);

        if (faction == eaMoveStart)
            faction = eaMoving;

        if (elemview == evSingle)
        {
            if (faction == eaNone)
            {
                mousehit = mhNone;
                TPoint oldpos = linepos;
                if (!strokelist->pointpt(bmprect, x, y, true, linepos))
                {
                    int sx, sy;
                    strokelist->nearestpt(bmprect, x, y, sx, sy);
                    linepos.x = sx;
                    linepos.y = sy;
                    mousehit = mhLine;
                }
                else
                    mousehit = mhPoint;

                if (oldpos != linepos)
                    pbEditor->Invalidate();
            }
            else if (faction == eaMoving || faction == eaAdding || faction == eaAddStart)
            {

                linepos.x = 99999;
                linepos.y = 99999;

                strokelist->move(bmprect, x, y, Shift.Contains(ssShift));

                redraw();
            }
            else if (faction == eaSelectStart && (abs(mousepos.x - mouselast.x) > 3 || abs(mousepos.y - mouselast.y) > 3))
                faction = eaSelecting;

            if (faction == eaSelecting)
                pbEditor->Invalidate();
        }
        else if (elemview == evCompound)
        {
            if (faction == eaNone)
            {
                TMouseHitItem oldhit = mousehit;
                TRect oldpos = partpos;
                byte oldpart = parthit;
                mousehit = mhNone;
                if (partlist->nearside(bmprect, x, y, partpos, parthit))
                {
                    if (parthit == 0)
                    {
                        mousehit = mhInside;
                        pbEditor->Cursor = crCross;
                    }
                    else if (parthit == (HIT_TOP | HIT_LEFT) || parthit == (HIT_BOTTOM | HIT_RIGHT))
                    {
                        mousehit = mhCornerDiag;
                        pbEditor->Cursor = crSizeNWSE;
                    }
                    else if (parthit == (HIT_TOP | HIT_RIGHT) || parthit == (HIT_BOTTOM | HIT_LEFT))
                    {
                        mousehit = mhCornerDiagB;
                        pbEditor->Cursor = crSizeNESW;
                    }
                    else
                    {
                        if (parthit & (HIT_TOP | HIT_BOTTOM))
                            pbEditor->Cursor = crSizeNS;
                        if (parthit & (HIT_LEFT | HIT_RIGHT))
                            pbEditor->Cursor = crSizeWE;
                        mousehit = mhSide;
                    }
                }
                else
                    pbEditor->Cursor = crCross;

                if (oldpos != partpos || oldpart != parthit || oldhit != mousehit)
                    pbEditor->Invalidate();
            }
            else if (faction == eaMoving)
            {

                partlist->move(bmprect, x, y, Shift.Contains(ssShift) && mousehit != mhSide);

                redraw();
            }
            else if (faction == eaSelectStart && (abs(mousepos.x - mouselast.x) > 3 || abs(mousepos.y - mouselast.y) > 3))
                faction = eaSelecting;

            if (faction == eaSelecting)
                pbEditor->Invalidate();
        }
        mouselast = mousepos;
        Application->ProcessMessages();
    }
    catch (...)
    {;
    }
    pbEditor->Tag = 0;
    Application->ProcessMessages();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::pbEditorMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    bool selchange = Shift.Contains(ssShift) || Shift.Contains(ssCtrl);

    mousestart = Point(x, y);
    mouselast = mousepos = mousestart;

    if (elemview == evSingle)
    {
        if (faction == eaAdding || faction == eaAddStart)
        {
            if (Button == mbRight || x < 0 || y < 0 || x >= pbEditor->ClientWidth || y >= pbEditor->ClientWidth)
            {
                strokelist->endadd(false);
                if (faction == eaAdding)
                    fundo->StrokeChanged();
                faction = eaNone;
                redraw();
                ReleaseCapture();
            }
            else
            {
                strokelist->add(bmprect, x, y);
                if (strokelist->length[strokelist->count - 1] == 255)
                {
                    strokelist->endadd(false);
                    if (faction == eaAdding)
                        fundo->StrokeChanged();
                    faction = eaNone;
                    redraw();
                }
                else
                    faction = eaAdding;
            }
            setcentre();
            return;
        }

        if (strokelist->select(bmprect, x, y, Button == mbLeft && !Shift.Contains(ssDouble), Button == mbLeft, selchange) || strokelist->selcount && Button == mbLeft && !selchange && Shift.Contains(ssDouble))
        {
            if (Button == mbLeft && !selchange && Shift.Contains(ssDouble))
                for (int ix = strokelist->count - 1; ix >= 0; --ix)
                    if (strokelist->stroke[ix])
                        strokelist->selectwholestroke(ix, true);
            redraw();
        }

        if (Button == mbRight)
        {
            TPoint p = pbEditor->ClientToScreen(mousepos);
            try
            {
                int s, v;
                bool r = strokelist->onpoint(bmprect, x, y, false, &s, &v);

                if (strokelist->selcount == 1)
                { //single vertex
                    miAddVertex->Enabled = r && strokelist->length[s] < 255 && v == strokelist->length[s];
                    pmVertex->Tag = (mousepos.x & 0xffff) | ((mousepos.y & 0xffff) << 16);
                    miAddVertex->Tag = s;
                    fillposition(miPosVertex);
                    pmVertex->Popup(p.x, p.y);
                }
                else if (strokelist->selcount == 2 && strokelist->segselcount == 1)
                {
                    miAddVert->Enabled = r && strokelist->length[s] < 255 && v == strokelist->length[s] - 1;
                    pmVertex->Tag = (mousepos.x & 0xffff) | ((mousepos.y & 0xffff) << 16);
                    miAddVertex->Tag = s;

                    for (int ix = 0; ix < strokelist->count && s < 0; ++ix)
                        if (strokelist->stroke[ix])
                            s = ix;
                    miInsert->Enabled = strokelist->length[s] < 255 && !r;
                    miCurve->Checked = strokelist->curveselcount;
                    miTip->Parent->Remove(miTip);
                    pmSegment->Items->Insert(pmSegment->Items->IndexOf(miCurve) + 1, miTip);
                    miTip->Enabled = true;
                    if (miTip->Enabled)
                        setuptipsmenu(strokelist->tips[s]);
                    miWeld->Enabled = strokelist->canweld();
                    fillalign(miVertAlign, miHorzAlign);
                    fillposition(miPos);
                    pmSegment->Popup(p.x, p.y);
                }
                else if (strokelist->selcount)
                {
                    miAddVertMul->Enabled = r && strokelist->length[s] < 255 && v == strokelist->length[s] - 1;
                    pmVertex->Tag = (mousepos.x & 0xffff) | ((mousepos.y & 0xffff) << 16);
                    miAddVertex->Tag = s;

                    for (int ix = 0; ix < strokelist->count && s < 0; ++ix)
                        if (strokelist->stroke[ix])
                            s = ix;
                    miInsertMul->Enabled = strokelist->length[s] < 255 && !r;
                    miWeldMul->Enabled = strokelist->canweld();
                    miConnectMul->Enabled = !miInsertMul->Enabled && miWeldMul->Enabled && strokelist->selcount == 2;
                    miCurveMul->Enabled = !r && strokelist->segselcount > 0 && s >= 0;
                    miCurveMul->Checked = miCurveMul->Enabled && strokelist->curved[s][v];
                    miDelSegmentMul->Enabled = strokelist->segselcount > 0;
                    miTip->Parent->Remove(miTip);
                    pmMulti->Items->Insert(pmMulti->Items->IndexOf(miCurveMul) + 1, miTip);
                    miTip->Enabled = true;
                    if (strokelist->strokeselcount == 1)
                        setuptipsmenu(strokelist->tips[s]);
                    else
                    {
                        miTipS1->Checked = false;
                        miTipS2->Checked = false;
                        miTipS3->Checked = false;
                        miTipE1->Checked = false;
                        miTipE2->Checked = false;
                        miTipE3->Checked = false;
                    }
                    fillalign(miVertAlignMul, miHorzAlignMul);
                    fillposition(miPosMul);
                    pmMulti->Popup(p.x, p.y);
                }
                else if (strokelist->count < 63)
                {
                    miNewStroke->Enabled = strokelist->count < 32;
                    pmNewVertex->Tag = (mousepos.x & 0xffff) | ((mousepos.y & 0xffff) << 16);
                    pmNewVertex->Popup(p.x, p.y);
                }
            }
            catch (...)
            {;
            }
            if (popupshown)
            {
                Application->ProcessMessages();
                popupshown = false;
                if (!pbEditor->Focused())
                    pbEditorMouseLeave(Sender);
            }
            return;
        }

        if (strokelist->selcount && !selchange)
        {
            faction = eaMoveStart;
            strokelist->startmove(bmprect, x, y);
        }
        else
            faction = eaSelectStart;
    }
    else if (elemview == evCompound)
    {
        if (partlist->select(bmprect, x, y, Button == mbLeft && !Shift.Contains(ssDouble), selchange))
            redraw();

        if (Button == mbRight && partlist->selcount > 0)
        {
            TPoint p = pbEditor->ClientToScreen(mousepos);
            miVertCenterPart->Visible = partlist->selcount > 1;
            miHorzCenterPart->Visible = partlist->selcount > 1;
            miCenterPart->Visible = partlist->selcount > 1;
            miVertToPart->Visible = partlist->selcount > 1;
            miHorzToPart->Visible = partlist->selcount > 1;
            miWidthToPart->Visible = partlist->selcount > 1;
            miHeightToPart->Visible = partlist->selcount > 1;
            if (partlist->selcount > 1)
            {
                fillalign(miVertToPart, miHorzToPart);
                fillresize(miWidthToPart, miHeightToPart);
            }

            pmPart->Popup(p.x, p.y);
            return;
        }

        if (partlist->selcount && !selchange)
        {
            faction = eaMoveStart;
            partlist->startmove(bmprect, x, y);
        }
        else
            faction = eaSelectStart;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::pbEditorMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    bool selchange = Shift.Contains(ssShift) || Shift.Contains(ssCtrl);

    if (elemview == evSingle)
    {
        if (faction == eaSelecting)
        {
            if (strokelist->select(bmprect, Rect(MIN(mousestart.x, mousepos.x), MIN(mousestart.y, mousepos.y), MAX(mousestart.x, mousepos.x), MAX(mousestart.y, mousepos.y)), selchange))
                redraw();
            else
                pbEditor->Invalidate();
            faction = eaNone;
        }
        else if (faction == eaMoving || faction == eaMoveStart)
        {
            strokelist->endmove();
            if (faction == eaMoving)
                fundo->StrokeChanged();
            faction = eaNone;
            redraw();
        }
        else if (faction == eaAdding || faction == eaAddStart)
        {
            if (x >= 0 && x < pbEditor->ClientWidth && y >= 0 && y < pbEditor->ClientHeight)
                SetCapture(pbEditor->Handle);
            else
            {
                strokelist->endadd(false);
                if (faction == eaAdding)
                    fundo->StrokeChanged();
                faction = eaNone;
                redraw();
            }
        }
        else
            faction = eaNone;
    }
    else if (elemview == evCompound)
    {
        if (faction == eaSelecting)
        {
            if (partlist->select(bmprect, Rect(MIN(mousestart.x, mousepos.x), MIN(mousestart.y, mousepos.y), MAX(mousestart.x, mousepos.x), MAX(mousestart.y, mousepos.y)), selchange))
                redraw();
            else
                pbEditor->Invalidate();
            faction = eaNone;
        }
        else if (faction == eaSelectStart)
        {
            faction = eaNone;
        }
        else if (faction == eaMoving || faction == eaMoveStart)
        {
            partlist->endmove();
            pbEditor->Invalidate();
            if (faction == eaMoving)
                fundo->PositionChanged();
            faction = eaNone;
        }
    }

}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::pbEditorKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (elemview == evSingle)
    {
        if (faction == eaMoveStart && Key == VK_SHIFT)
            faction = eaMoving;

        if (faction == eaMoving)
        {
            if (Key == VK_SHIFT)
            {
                strokelist->move(bmprect, mousepos.x, mousepos.y, true);
                redraw();
            }
        }
    }
    else if (elemview == evCompound)
    {
        if (faction == eaMoveStart && Key == VK_SHIFT)
            faction = eaMoving;

        if (faction == eaMoving)
        {
            if (Key == VK_SHIFT && mousehit != mhSide)
            {
                partlist->move(bmprect, mousepos.x, mousepos.y, true);
                redraw();
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::pbEditorKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (elemview == evSingle)
    {
        if (faction == eaMoving)
        {
            if (Key == VK_SHIFT)
            {
                strokelist->move(bmprect, mousepos.x, mousepos.y, false);
                redraw();
            }
        }
    }
    else if (elemview == evCompound)
    {
        if (faction == eaMoving)
        {
            if (Key == VK_SHIFT && mousehit != mhSide)
            {
                partlist->move(bmprect, mousepos.x, mousepos.y, false);
                redraw();
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::dlgSaveClose(TObject *Sender)
{
    data->ModalEnd(((TCommonDialog*)Sender)->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::dlgSaveShow(TObject *Sender)
{
    data->ModalBegin(NULL, ((TCommonDialog*)Sender)->Handle);
}
//---------------------------------------------------------------------------

void __fastcall TfEditElem::miOpenFileClick(TObject *Sender)
{
    if (ExecuteDialog(dlgOpen))
    {
        strokelist->Clear();
        partlist->Clear();

        noupdate = true;

        try
        {
            elements->LoadFromFile(dlgOpen->FileName);
            update_repositories(0);
        }
        catch (...)
        {
            MsgBox("There was an error opening the selected file. Make sure it's a valid\nkanji stroke order diagram file before opening.", "Error", MB_OK);
            dlgOpen->FileName = elementsettings.lastfilename;
            try
            {
                delete elements;
            }
            catch (...)
            {;
            }
            elements = new TElementList(0);
            addfirst();
            felemindex = 0;
            fvarindex = 0;
            if (elementsettings.lastfilename.Length())
                elements->LoadFromFile(elementsettings.lastfilename);
            else if (hasstrokes)
                elements->LoadFromFile(paths.installdatapath + Stroke_Filename);
        }

        if (elements->Version < 2)
        {
            elements->RepositionAll();
            elementsettings.lastfilename = "";
            dlgSave->FileName = "";
            int p = Caption.Pos(" - ");
            if (p)
                Caption = Caption.SubString(1, p - 1);
        }
        controlinit();
        if (elements->Version >= 2)
        {
            dlgOpen->InitialDir = ExtractFilePath(dlgOpen->FileName);
            dlgSave->InitialDir = dlgOpen->InitialDir;
            dlgSave->FileName = dlgOpen->FileName;

            int p = Caption.Pos(" - ");
            if (!p)
                p = Caption.Length();
            Caption = Caption.SubString(1, p - 1) + " - " + dlgOpen->FileName;
            elementsettings.lastfilename = dlgOpen->FileName;
        }

        modified = false;

        noupdate = false;
        felemindex = -1;
        fvarindex = -1;
        elemindex = 0;
        ncallback();

        miGen->Enabled = !hasrecdata && models->Count;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miSaveAsClick(TObject *Sender)
{
    if (ExecuteDialog(dlgSave))
    {
        if (dlgSave->FileName == paths.installdatapath + Stroke_Filename)
        {
            MsgBox("Please don't write over the original stroke order diagram file, because you might need it in the future.", "Warning", MB_OK);
            miSaveAsClick(Sender);
            return;
        }
        elements->SaveToFile(dlgSave->FileName);
        modified = false;
        dlgSave->InitialDir = ExtractFilePath(dlgSave->FileName);
        dlgOpen->InitialDir = dlgSave->InitialDir;
        dlgOpen->FileName = dlgSave->FileName;
        elementsettings.lastfilename = dlgSave->FileName;
        int p = Caption.Pos(" - ");
        if (!p)
            p = Caption.Length();
        Caption = Caption.SubString(1, p - 1) + " - " + dlgSave->FileName;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miSaveFileClick(TObject *Sender)
{
    if (!FileExists(dlgSave->FileName))
    {
        miSaveAsClick(Sender);
    }
    else
    {
        elements->SaveToFile(dlgSave->FileName);
        modified = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::Close1Click(TObject *Sender)
{
    btnClose->Click();
}
//---------------------------------------------------------------------------
bool TfEditElem::Finalize()
{
    int patnum[] = { -1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 3, 2, 3, 4, 3, -1, -1, -1, -1, -1 };

    for (int ix = 0; ix < elements->Count && elem->owner != NO_OWNER; ++ix)
    {
        if (ix == felemindex)
            continue;

        if (elements->Items[ix]->owner == elem->owner)
        {
            MsgBox(((UnicodeString)"Character code of element #" + (ix + 1) + " matches current element! Set a different code before saving.").c_str(), "Error", MB_OK);
            return false;
        }
    }

    if (!elem->variants[fvarindex].standalone && patnum[cbPattern->ItemIndex] > 0 && (elem->parts[patnum[cbPattern->ItemIndex] - 1] < 0 || patnum[cbPattern->ItemIndex] < 4 && elem->parts[patnum[cbPattern->ItemIndex]] >= 0))
    {
        if (MsgBox("Pattern doesn't match number of parts! Save anyway?", "Warning", MB_OKCANCEL) == IDCANCEL)
            return false;
    }

    if (!partquery())
        return false;

    int UNIix = elem->owner < NO_OWNER ? cbUNICODE->Items->IndexOf(wtoAS(kanjis->Kanjis[elem->owner]->_uni)) : -1;
    if (UNIix >= 0)
    {
        cbUNICODE->Items->BeginUpdate();
        try
        {
            cbUNICODE->Items->Delete(UNIix);
            cbUNICODE->Text = wtoAS(kanjis->Kanjis[elem->owner]->_uni);
        }
        catch (...)
        {
        }
        cbUNICODE->Items->EndUpdate();
    }

    mousehit = mhNone;
    pbEditor->Cursor = crCross;

    elements->Update(felemindex, elem);
    for (int ix = 0; ix < elem->variantcount; ++ix)
    {
        if (vstate[ix] != vsUntouched)
            elements->PositionUpdate(felemindex, ix);

        elements->SetVariantName(felemindex, ix, varnames->Strings[ix]);
    }

    modified = true;

    if (elements->Items[felemindex]->owner < NO_OWNER)
        bgchar = kanjis->Kanjis[elements->Items[felemindex]->owner]->ch;
    else if (elements->Items[felemindex]->owner > NO_OWNER)
        bgchar = elements->Items[felemindex]->owner;
    else
        bgchar = 0;

    redrawbg();

    setunchanged();
    egrid->Redraw();
    vgrid->Redraw();
    fundo->SetItem(elem, fvarindex);
    setcentre();

    update_repbuttons();
    fill_recparts();

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::btnReadyClick(TObject *Sender)
{
    Finalize();
}
//---------------------------------------------------------------------------
void TfEditElem::setcentre()
{
    if (cbStandalone->Checked)
    {
        numCentre->max = elem->variants[fvarindex].centrepoint;
        numCentre->max = elem->variants[fvarindex].strokecount;

        numCentre->value = elem->variants[fvarindex].centrepoint;
        edCentre->Enabled = true;
    }
    else
    {
        numCentre->min = 0;
        numCentre->max = 0;

        edCentre->Text = "0";
        edCentre->Enabled = false;
    }

    sbAddChar->Enabled = elem && cbStandalone->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::pbEditorResize(TObject *Sender)
{
    redrawbg();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miCenterFrameVertexClick(TObject *Sender)
{
    strokelist->alignvertex(vaVertFrame, bmprect);
    strokelist->alignvertex(vaHorzFrame, bmprect);
    fundo->StrokeChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miCenterClick(TObject *Sender)
{
    strokelist->alignvertex(vaVert);
    strokelist->alignvertex(vaHorz);
    fundo->StrokeChanged();
    redraw();
}
//---------------------------------------------------------------------------

struct TKanjiElementVariant
{
    TKanjiElement *elem;
    byte variant;

    TElemPattern pattern;
    int level;

    int x;
    int y;
    double w;
    double h;
};

void __fastcall TfEditElem::cbStandaloneClick(TObject *Sender)
{
    if (noupdate || !elem || fvarindex < 0 || elem->variants[fvarindex].standalone == cbStandalone->Checked)
        return;

    TKanjiVariant *v = &elem->variants[fvarindex];

    strokelist->Clear();
    partlist->Clear();

    if (cbStandalone->Checked)
    {
        v->strokecount = 0;
        int stackpos = 0;

        TTemplateList<TKanjiElementVariant, false> *stack = new TTemplateList<TKanjiElementVariant, false>;
        try
        {
            TKanjiElementVariant *ev = new TKanjiElementVariant;
            ev->elem = elem;
            ev->variant = varindex;
            ev->w = 1;
            ev->h = 1;
            ev->level = 0;
            ev->pattern = epUndef;
            ev->x = ev->elem->variants[ev->variant].x;
            ev->y = ev->elem->variants[ev->variant].y;
            stack->Add(ev);
            for (int ix = 0; ix < 4 && elem->parts[ix] >= 0; ++ix)
                v->strokecount += elements->Items[elem->parts[ix]]->variants[elem->variants[fvarindex].positions[ix].variant].strokecount;
            v->strokes = new TElementStroke[v->strokecount];

            int spos = 0;

            bool backed = false;
            while (stack->Count)
            {
                TKanjiElementVariant *ev = stack->Items[stackpos];
                int level = ev->level;
                try
                {
                    if (!ev->elem->variants[ev->variant].standalone)
                    {
                        stack->Delete(stackpos);
                        backed = false;
                        for (int ix = 0; ix < 4 && ev->elem->parts[ix] >= 0; ++ix)
                        {
                            TKanjiElementVariant *eev = new TKanjiElementVariant;
                            eev->pattern = ev->elem->pattern;
                            eev->level = ev->level + 1;
                            eev->elem = elements->Items[ev->elem->parts[ix]];
                            eev->variant = ev->elem->variants[ev->variant].positions[ix].variant;
                            eev->w = ((double)ev->elem->variants[ev->variant].positions[ix].width / eev->elem->variants[eev->variant].width) * ev->w;
                            eev->h = ((double)ev->elem->variants[ev->variant].positions[ix].height / eev->elem->variants[eev->variant].height) * ev->h;
                            eev->x = (double)(ev->elem->variants[ev->variant].positions[ix].x - ev->elem->variants[ev->variant].x) * ev->w + ev->x;
                            eev->y = (double)(ev->elem->variants[ev->variant].positions[ix].y - ev->elem->variants[ev->variant].y) * ev->h + ev->y;
                            stack->Insert(stackpos + ix, eev);
                        }
                        stackpos--;
                        delete ev;
                    }
                    else
                    {
                        bool center = (ev->pattern >= epEbl && ev->pattern <= epElr) && ev->elem->variants[ev->variant].centrepoint;
                        int ix = backed ? ev->elem->variants[ev->variant].centrepoint - 1 : 0;
                        for (; ix < ev->elem->variants[ev->variant].strokecount; ++ix)
                        {
                            if (!backed && center && ix == ev->elem->variants[ev->variant].centrepoint - 1 && stackpos + 1 < stack->Count && stack->Items[stackpos + 1]->level == ev->level)
                                break;

                            v->strokes[spos].tips = ev->elem->variants[ev->variant].strokes[ix].tips;
                            v->strokes[spos].pointcount = ev->elem->variants[ev->variant].strokes[ix].pointcount;
                            v->strokes[spos].points = new TElementPoint[v->strokes[spos].pointcount];
                            for (int j = 0; j < v->strokes[spos].pointcount; ++j)
                            {
                                TElementPoint *op = &ev->elem->variants[ev->variant].strokes[ix].points[j];
                                TElementPoint *ep = &v->strokes[spos].points[j];
                                ep->type = op->type;
                                ep->x = (double)(op->x - ev->elem->variants[ev->variant].x) * ev->w + ev->x;
                                ep->y = (double)(op->y - ev->elem->variants[ev->variant].y) * ev->h + ev->y;
                                ep->c1x = (double)(op->c1x - ev->elem->variants[ev->variant].x) * ev->w + ev->x;
                                ep->c1y = (double)(op->c1y - ev->elem->variants[ev->variant].y) * ev->h + ev->y;
                                ep->c2x = (double)(op->c2x - ev->elem->variants[ev->variant].x) * ev->w + ev->x;
                                ep->c2y = (double)(op->c2y - ev->elem->variants[ev->variant].y) * ev->h + ev->y;
                            }
                            ++spos;
                        }
                        if (ix == ev->elem->variants[ev->variant].strokecount)
                        {
                            stack->Delete(stackpos);
                            delete ev;
                            --stackpos;
                            backed = stackpos >= 0 && (stack->Items[stackpos])->level <= level;
                        }
                    }
                }
                catch (...)
                {;
                }
                if (!backed)
                    ++stackpos;
            }
        }
        catch (...)
        {;
        }
        delete stack;

        delete[] v->positions;
        v->positions = 0;
    }
    else
    {
        for (int ix = 0; ix < v->strokecount; ++ix)
            delete[] v->strokes[ix].points;
        delete[] v->strokes;
        v->strokes = 0;
        v->strokecount = 0;

        v->positions = new TPartPosition[4];
        memset(v->positions, 0, sizeof(TPartPosition)*4);

        int top = 0;
        for (int ix = 3; ix >= 0; --ix)
        {
            if (elem->parts[ix] < 0)
                continue;
            else if (!top)
                top = ix + 1;

            position_elem(v->positions[ix], elem->pattern, ix, top);
            v->strokecount += elements->Items[elem->parts[ix]]->variants[v->positions[ix].variant].strokecount;
        }
    }

    v->standalone = cbStandalone->Checked;
    fundo->StandaloneChanged();
    setcentre();

    update_points();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::cbPatternDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State)
{
    TCanvas *c = ((TComboBox*)Control)->Canvas;
    TDrawingStyle ds = Imglist::dsNormal;
    if (State.Contains(odFocused))
        ds = dsFocus;
    if (State.Contains(odSelected))
        ds = dsSelected;

    c->FillRect(Rect);
    data->imgPattern->Draw(c, Rect.Left + (Rect.Width() - 20) / 2, Rect.Top + (Rect.Height() - 20) / 2, Index, ds, itImage, true);
    if (State.Contains(odFocused))
        DrawFocusRect(c->Handle, &Rect);
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::cbPatternMeasureItem(TWinControl *Control, int Index, int &Height)
{
    Height = Index < 0 ? 20 : 22;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::cbPatternChange(TObject *Sender)
{
    if (noupdate || elem->pattern == (TElemPattern)cbPattern->ItemIndex)
        return;

    elem->pattern = (TElemPattern)cbPattern->ItemIndex;
    fundo->PatternChanged();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbVar1Click(TObject *Sender)
{
    TSpeedButton *btn = (TSpeedButton*)Sender;
    pmVariants->Tag = btn->Tag;
    TPoint p = pbPart1->ClientToScreen(Point(0, pbPart1->Height));

    while (pmVariants->Items->Count)
        delete pmVariants->Items->Items[0];

    for (int ix = 0; ix < elements->Items[elem->parts[btn->Tag]]->variantcount; ++ix)
    {
        TMenuItem *mi = new TMenuItem(pmVariants);
        mi->Tag = ix;
        mi->OnMeasureItem = miVarMeasureItem;
        mi->OnDrawItem = miVarDrawItem;
        mi->OnClick = miVarClick;
        pmVariants->Items->Add(mi);
    }

    pmVariants->Popup(p.x + (pbPart2->Left - pbPart1->Left)*btn->Tag, p.y);
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miVarMeasureItem(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height)
{
    Width = 40;
    Height = 50;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miVarDrawItem(TObject* Sender, TCanvas* ACanvas, const TRect &ARect, bool Selected)
{
    TMenuItem *mi = (TMenuItem*)Sender;

    if (!elem->variants[fvarindex].standalone && mi->Tag == elem->variants[fvarindex].positions[pmVariants->Tag].variant)
        ACanvas->Brush->Color = COLORDEC(ACanvas->Brush->Color, 0x361a17);
    ACanvas->FillRect(ARect);
    elements->Draw(Rect(ARect.Left + (ARect.Width() - 40) / 2, ARect.Top + (ARect.Height() - 40) / 2, ARect.Left + (ARect.Width() - 40) / 2 + 40, ARect.Top + (ARect.Height() - 40) / 2 + 40), ACanvas->Handle, 1.8, ACanvas->Pen->Color, elem->parts[pmVariants->Tag], mi->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miVarClick(TObject *Sender)
{
    TMenuItem *mi = (TMenuItem*)Sender;
    if (elem->variants[fvarindex].positions[pmVariants->Tag].variant != mi->Tag)
    {
        elem->variants[fvarindex].positions[pmVariants->Tag].variant = mi->Tag;
        fundo->VariantChanged();
    }

    update_points();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miVertCenterPartClick(TObject *Sender)
{
    partlist->alignpart(paVert);
    fundo->PositionChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miHorzCenterPartClick(TObject *Sender)
{
    partlist->alignpart(paHorz);
    fundo->PositionChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miVertFramePartClick(TObject *Sender)
{
    partlist->alignpart(paVertFrame);
    fundo->PositionChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miHorzFramePartClick(TObject *Sender)
{
    partlist->alignpart(paHorzFrame);
    fundo->PositionChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miCenterPartClick(TObject *Sender)
{
    partlist->alignpart(paVert);
    partlist->alignpart(paHorz);
    fundo->PositionChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miCenterFramePartClick(TObject *Sender)
{
    partlist->alignpart(paVertFrame);
    partlist->alignpart(paHorzFrame);
    fundo->PositionChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miW1000Click(TObject *Sender)
{
    partlist->resizepart(prWidth, ((TMenuItem*)Sender)->Tag);
    fundo->PositionChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miH1000Click(TObject *Sender)
{
    partlist->resizepart(prHeight, ((TMenuItem*)Sender)->Tag);
    fundo->PositionChanged();
    redraw();
}
//---------------------------------------------------------------------------
void TfEditElem::setpart(int part, int newindex)
{

    TNumEdit *nparts[4] = { numPart1, numPart2, numPart3, numPart4 };

    if (noupdate || !elem || newindex == elem->parts[part])
        return;

    bool errequal, errempty, errhaspart;
    if (newindex >= 0 && ((errequal = (newindex == felemindex)) == true || (errempty = !elements->Items[newindex]->variantcount || !elements->Items[newindex]->variants[0].strokecount) == true || (errhaspart = elements->HasPart(newindex, felemindex)) == true))
    {
        if (errequal)
            MsgBox("Cannot set self as part of the element.", "Error", MB_OK);
        else if (errempty)
            MsgBox("Cannot set element with no strokes.", "Error", MB_OK);
        else if (errhaspart)
            MsgBox("Cannot set element that has this element as its part.", "Error", MB_OK);
        nparts[part]->value = elem->parts[part] + 1;
        return;
    }

    int firstnull = 4;
    for (int ix = 0; ix < 4 && firstnull == 4; ++ix)
        if (elem->parts[ix] < 0)
            firstnull = ix;

    if (firstnull < part && newindex >= 0)
    {
        nparts[part]->value = 0;
        nparts[firstnull]->value = newindex + 1;
        nparts[firstnull]->control->SetFocus();
        return;
    }

    partlist->Clear();
    strokelist->Clear();

    if (newindex < 0)
    {
        if (part < 3)
        {
            if (!elem->variants[fvarindex].standalone)
                memmove(elem->variants[fvarindex].positions + part, elem->variants[fvarindex].positions + part + 1, sizeof(TPartPosition)*(3 - part));
            memmove(elem->parts + part, elem->parts + part + 1, sizeof(short)*(3 - part));
            elem->parts[firstnull - 1] = -1;
        }
        else
            elem->parts[3] = -1;
        for (int ix = 0; ix < 4; ++ix)
            nparts[ix]->value = elem->parts[ix] + 1;
        fundo->PartRemoved(part);
    }
    else
    {
        bool added = elem->parts[part] < 0;
        if (!elem->variants[fvarindex].standalone)
        {
            if (elem->parts[part] < 0)
            {
                TPartPosition *pp = &elem->variants[fvarindex].positions[part];
                pp->variant = 0;
                position_elem(*pp, elem->pattern, part, 4);
            }
            else
                elem->variants[fvarindex].positions[part].variant = 0;
        }
        elem->parts[part] = newindex;

        if (added)
            fundo->PartAdded();
        else
            fundo->PartChanged();
    }

    update_points();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miNewStrokeClick(TObject *Sender)
{
    faction = eaAddStart;

    strokelist->startadd(bmprect, pmNewVertex->Tag & 0xffff, (pmNewVertex->Tag >> 16) & 0xffff);
    redraw();
    SetCapture(pbEditor->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::pbEditorExit(TObject *Sender)
{
    strokelist->endadd(false);
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miAddVertexClick(TObject *Sender)
{
    faction = eaAddStart;
    strokelist->startadd(bmprect, pmVertex->Tag & 0xffff, (pmVertex->Tag >> 16) & 0xffff, miAddVertex->Tag);
    redraw();
    SetCapture(pbEditor->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbBgClick(TObject *Sender)
{
    elementsettings.kanjibackground = sbBg->Down;
    redrawbg();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbNumbersClick(TObject *Sender)
{
    elementsettings.strokenumbers = sbNumbers->Down;
    strokelist->numbers = elementsettings.strokenumbers;
    redrawbg();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbSettingsClick(TObject *Sender)
{
    fElemSettings = new TfElemSettings(Application);
    TPoint p = ClientToScreen(Point(2, sbSettings->Top + sbSettings->Height + 2));
    fElemSettings->ShowEx(p.x, p.y);
}
//---------------------------------------------------------------------------
void TfEditElem::setundo()
{
    updatevarbuttons();

    btnUndo->Enabled = fundo->Pos >= 0;
    btnRedo->Enabled = fundo->Pos < fundo->Count - 1;
    miUndo->Enabled = btnUndo->Enabled;
    miRedo->Enabled = btnRedo->Enabled;

    if (vstate[fvarindex] == vsModified && fundo->Pos == -1)
        vstate[fvarindex] = vsUntouched;
    if (vstate[fvarindex] == vsUntouched && fundo->Pos >= 0)
        vstate[fvarindex] = vsModified;

    miMerge->Enabled = cansave();
    btnReady->Enabled = miMerge->Enabled && changed();
    btnMerge->Enabled = miMerge->Enabled;
    btnGenerate->Enabled = !btnReady->Enabled && elem;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::btnUndoMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    MouseRepeat(btnUndoMouseDown, Sender, Button, Shift, X, Y, NULL);

    fundo->Undo();
    elemchanged();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::btnRedoMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    MouseRepeat(btnRedoMouseDown, Sender, Button, Shift, X, Y, NULL);

    fundo->Redo();
    elemchanged();
}
//---------------------------------------------------------------------------
bool TfEditElem::partquery()
{
    bool changed = false;
    if (elem->variantcount > 1 && (fundo->HasType(uiPartRemove) || fundo->HasType(uiPartAdd) || fundo->HasType(uiPart)))
    {
        for (int ix = 0; ix < elem->variantcount; ++ix)
            if (ix != fvarindex && !elem->variants[ix].standalone)
            {
                changed = true;
                vstate[ix] = vsFinal;
            }
    }
    if (changed)
        updatevariants();

    return true;
}
//---------------------------------------------------------------------------
void TfEditElem::updatevariants()
{
    if (!fundo->Count)
        return;

    TUndoItem *current = fundo->Items[fundo->Pos];

    for (int ix = 0; ix < elem->variantcount; ++ix)
        if (ix != fvarindex && !elem->variants[ix].standalone)
        {
            TKanjiVariant *v = &elem->variants[ix];

            for (int j = 0; j < 4; ++j)
            {
                if (current->partpos[j] < 0)
                    break;

                if (current->partpos[j] > j)
                    memmove(&v->positions[j], &v->positions[current->partpos[j]], sizeof(TPartPosition));
                if (elements->Items[current->parts[j]]->variantcount <= v->positions[j].variant)
                    v->positions[j].variant = 0;
            }
            int cnt = 0;
            for (int j = 3; j >= 0 && current->partpos[j] < 0/*maxix*/; --j)
            {
                if (current->parts[j] < 0)
                    continue;
                if (!cnt)
                    cnt = j + 1;
                v->positions[j].variant = 0;
                position_elem(v->positions[j], elem->pattern, j, cnt);
            }
        }
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbAddVariantClick(TObject *Sender)
{
    if (btnReady->Enabled && (MsgBox("Adding a new variant finalizes your current changes! Do you want to continue?", "Warning", MB_YESNO) == IDNO || !Finalize()))
        return;

    strokelist->Clear();
    partlist->Clear();

    elements->DuplicateVariant(felemindex, fvarindex);

    modified = true;

    int ix = felemindex;

    fvarindex = -1;
    felemindex = -1;
    elemindex = ix;
    varindex = elem->variantcount - 1;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbDelVariantClick(TObject *Sender)
{
    bool untouched = true;
    for (int ix = 0; ix < elem->variantcount && untouched; ++ix)
        untouched = vstate[ix] == vsUntouched;

    if (!untouched && MsgBox("Deleting a variant finalizes your current changes! Do you want to continue?", "Warning", MB_YESNO) == IDNO)
        return;

    bool hasvar = false;
    for (int ix = 0; ix < elem->parentcnt && !hasvar; ++ix)
    {
        TKanjiElement *e = elements->Items[elem->parents[ix]];
        for (int j = 0; j < 4 && e->parts[j] >= 0 && !hasvar; ++j)
            if (e->parts[j] == felemindex)
                for (int k = 0; k < e->variantcount && !hasvar; ++k)
                    hasvar |= !e->variants[k].standalone && e->variants[k].positions[j].variant == fvarindex;
    }

    int newvariant = fvarindex + 1;
    if (hasvar && (!InputQueryInt((UnicodeString)"The currently selected variant is used " + "in a parent element. Please select the index of another variant replacing " + "the current one after its deletion.", "Select alternate variant", newvariant, 1, elem->variantcount) || newvariant == fvarindex + 1))
        return;
    newvariant--;

    if (!Finalize())
        return;

    if (hasvar)
        elements->ChangeVariant(felemindex, fvarindex, newvariant);

    elements->RemoveVariant(felemindex, fvarindex);

    strokelist->Clear();
    partlist->Clear();

    modified = true;

    int oldv = min(fvarindex, elem->variantcount - 1);
    int olde = elemindex;
    fvarindex = -1;
    felemindex = -1;
    elemindex = olde;
    varindex = oldv;

    update_repositories(cbRepository->ItemIndex);
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbShownBgClick(TObject *Sender)
{
    if (sbShownBg->Down)
        SetCapture(pbParents->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::pbParentsMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (sbShownBg->Down && egrid->CellAtPos(Point(X, Y)) < 0)
    {
        ReleaseCapture();
        sbShownBg->Down = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miUndoClick(TObject *Sender)
{
    fundo->Undo();
    elemchanged();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miRedoClick(TObject *Sender)
{
    fundo->Redo();
    elemchanged();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbPart1Click(TObject *Sender)
{
    TNumEdit *nparts[5] = { numPart1, numPart2, numPart3, numPart4, numElem };

    int n = ((TSpeedButton*)Sender)->Tag;
    int part;
    if ((part = elementbrowse(nparts[n]->value - 1)) >= 0)
        nparts[n]->value = part + 1;
}
//---------------------------------------------------------------------------
int TfEditElem::elementbrowse(int start)
{
    fSelectElem = new TfSelectElem(Application);
    int r;
    try
    {
        r = fSelectElem->ShowModalEx(start);
    }
    catch (...)
    {;
    }
    delete fSelectElem;
    fSelectElem = 0;
    return r == mrCancel ? -1 : start;
}
//---------------------------------------------------------------------------
bool TfEditElem::get_modified()
{
    return Caption[1] == '*';
}
//---------------------------------------------------------------------------
void TfEditElem::set_modified(bool val)
{
    if (modified != val)
    {
        if (modified)
            Caption = Caption.SubString(2, Caption.Length() - 1);
        else
            Caption = "*" + Caption;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose = true;
    destroying = true;
    if (modified)
    {
        int r = MsgBox("You have modified one or more of the stroke order diagrams. Would you like to save them now?", "Warning", MB_YESNOCANCEL);
        if (r == IDCANCEL)
        {
            destroying = false;
            CanClose = false;
        }
        else if (r == IDYES)
            miSaveFileClick(miSaveFile);
        else
        { //"lose" changes here
            UnicodeString f = dlgSave->FileName;
            if (!f.Length() || !FileExists(f))
                f = paths.installdatapath + Stroke_Filename;
            if (!FileExists(f))//whatever;
                        elements->Clear();
            else
            {
                strokelist->Clear();
                partlist->Clear();

                elements->LoadFromFile(f);
                modified = false;
            }
        }
    }

}
//---------------------------------------------------------------------------
void TfEditElem::jcallback()
{
    static bool changing = false;
    if (changing)
        return;
    changing = true;
    try
    {
        int kindex = -1;
        if (cbUNICODE->Text.Trim().Length() >= 4)
        {
            word uni = AStow(cbUNICODE->Text);
            if (UNICODE_J(uni))
                kindex = uni;

            if (KANJI(uni))
            {
                for (int ix = 0; ix < kanjis->Count && kindex < 0; ++ix)
                {
                    if (kanjis->Kanjis[ix]->_uni == uni)
                        kindex = ix;
                }
            }
        }

        if (elem->owner != kindex)
            fundo->OwnerChanged();

        if (kindex < 0 && (elem->owner != NO_OWNER || bgchar || cbUNICODE->Text.Trim().Length()))
        {
            cbUNICODE->Text = "";
            bgchar = 0;
            elem->owner = NO_OWNER;
            redrawbg();
        }
        else if (kindex >= 0 && (!bgchar || elem->owner != kindex || cbUNICODE->Text != wtoAS(kanjis->Kanjis[kindex]->_uni)))
        {
            if (kindex < NO_OWNER)
            {
                elem->owner = kindex;
                cbUNICODE->Text = wtoAS(kanjis->Kanjis[kindex]->_uni);
                bgchar = kanjis->Kanjis[kindex]->ch;
            }
            else
            {
                elem->owner = kindex;
                cbUNICODE->Text = wtoAS(kindex);
                bgchar = kindex;
            }

            cbUNICODE->SelLength = 0;
            cbUNICODE->SelStart = 4;
            redrawbg();
        }
    }
    catch (...)
    {;
    }
    changing = false;

}
//---------------------------------------------------------------------------
void TfEditElem::mcallback()
{
    sbDelChar->Enabled = mgrid->count;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbDeleteClick(TObject *Sender)
{
    if (elements->Items[felemindex]->parentcnt > 0)
    {
        MsgBox("Cannot delete element that is part of other elements.", "Error", MB_OK);
        return;
    }

    elements->DeleteElement(felemindex);
    update_repositories(cbRepository->ItemIndex);
    modified = true;

    partlist->Clear();
    strokelist->Clear();
    int old = felemindex;
    felemindex = -1;
    elemindex = MAX(0, old - 1);
    controlinit();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbAddClick(TObject *Sender)
{
    TKanjiElement *e = new TKanjiElement;
    memset(e, 0, sizeof(TKanjiElement));
    e->owner = -1;
    for (int ix = 0; ix < 4; ++ix)
        e->parts[ix] = -1;
    e->variantcount = 1;
    e->variants = new TKanjiVariant[1];
    memset(e->variants, 0, sizeof(TKanjiVariant));
    TKanjiVariant *v = &e->variants[0];
    v->standalone = true;
    elements->Add(e);
    modified = true;
    controlinit();
    numElem->value = numElem->max;
}
//---------------------------------------------------------------------------
void TfEditElem::controlinit()
{
    numElem->min = 1;
    numElem->max = elements->Count;
    numPart1->max = elements->Count;
    numPart2->max = elements->Count;
    numPart3->max = elements->Count;
    numPart4->max = elements->Count;

    btnUp->Enabled = numElem->value > 1;
    btnDown->Enabled = numElem->value < numElem->max;

    sbDelete->Enabled = numElem->max > 1;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::FormPaint(TObject *Sender)
{
    TRect r;
    DrawButtonBar(this, NULL, b1);

    DrawCloseBar(Canvas, Rect(0, bb->Top + 1, ClientWidth, ClientHeight));

    DrawGrad(Canvas, COLORINC(clBtnFace, 0x080808), COLORDEC(clBtnFace, 0x040404), ChildRect(pElement, 1, 1, -1, -1));
    r = ChildRect(pParts, 1, 1, -1, -1);
    r.bottom = r.top + 45;
    DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, r);
    r = ChildRect(pParents, 1, 1, -1, -1);
    r.bottom = r.top + 45;
    DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, r);
    r = ChildRect(pRepository, 1, 1, -1, -1);
    r.bottom = r.top + 45;
    DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, r);
    r = ChildRect(pModels, 1, 1, -1, -1);
    r.bottom = r.top + 45;
    DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, r);

    DrawPillowPanel(Canvas, Rect(ChildRect(pModels).Right, b1->Top, ClientWidth, b2->Top));

    Canvas->Brush->Color = clBtnFace;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::FormResize(TObject *Sender)
{
    TRect r = Rect(0, bb->Top, ClientWidth, ClientHeight);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miGenClick(TObject *Sender)
{
    if (hasrecdata)
        THROW(L"Shouldn't generate everything if we already have the needed data.");

    if (changed() && MsgBox("Stroke data generation will undo all uncommited changes! Press 'Cancel' if you would first like to finish editing the current element.", "Warning", MB_OKCANCEL) == IDCANCEL)
        return;

    felemindex = -1;
    fvarindex = -1;
    elemindex = 0;
    ncallback();

    ShowMessageForm("Generating stroke recognition data. May take minutes...");

    try
    {
        for (int ix = 0; ix < elements->Count; ++ix)
        {
            if (!(ix % 50))
                Application->ProcessMessages();
            elements->RegenerateRecognizeData(ix);
        }
        HideMessageForm();
        EnableRecognize();
        modified = true;
    }
    catch (...)
    {
        HideMessageForm();
        MsgBox("Stroke data generation failed.", "Error", MB_OK);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbAddRepClick(TObject *Sender)
{
    if (elements->RepositoryCount >= 32000)
    {
        MsgBox("You already have too many groups.", "Error", MB_OK);
        return;
    }

    UnicodeString s = "";
    if (QueryBox("Enter a unique name for the new repository group:", "New repository group", s) && (s = s.Trim()).Length())
    {
        try
        {
            update_repositories(elements->AddRepository(s));
            modified = true;
        }
        catch (...)
        {
            MsgBox("Couldn't add new repository group! Avoid existing names.", "Error", MB_OK);
        }
    }
}
//---------------------------------------------------------------------------
void TfEditElem::update_repositories(int newpos)
{
    TStrings *ii = cbRepository->Items;
    ii->BeginUpdate();
    try
    {
        ii->Clear();
        for (int ix = 0; ix < elements->RepositoryCount; ++ix)
            ii->Add(elements->RepositoryNames[ix]);
        cbRepository->ItemIndex = min(newpos, ii->Count - 1);
        cbRepositoryChange(cbRepository);
    }
    catch (...)
    {;
    }
    ii->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbToRepClick(TObject *Sender)
{
    int g = rgrid->count;
    int ix = elements->ToRepository(cbRepository->ItemIndex, felemindex, fvarindex);
    if (g == ix)
        modified = true;

    rgrid->set_items(elements->RepositoriesCount[cbRepository->ItemIndex]);
    rgrid->pos = ix;
}
//---------------------------------------------------------------------------
void TfEditElem::pgiveelem1(int pos, int & element, byte & variant)
{
    element = elem->parts[0];
    variant = elem->variants[fvarindex].standalone ? 0 : elem->variants[fvarindex].positions[0].variant;
}
//---------------------------------------------------------------------------
void TfEditElem::pgiveelem2(int pos, int & element, byte & variant)
{
    element = elem->parts[1];
    variant = elem->variants[fvarindex].standalone ? 0 : elem->variants[fvarindex].positions[1].variant;
}
//---------------------------------------------------------------------------
void TfEditElem::pgiveelem3(int pos, int & element, byte & variant)
{
    element = elem->parts[2];
    variant = elem->variants[fvarindex].standalone ? 0 : elem->variants[fvarindex].positions[2].variant;
}
//---------------------------------------------------------------------------
void TfEditElem::pgiveelem4(int pos, int & element, byte & variant)
{
    element = elem->parts[3];
    variant = elem->variants[fvarindex].standalone ? 0 : elem->variants[fvarindex].positions[3].variant;
}
//---------------------------------------------------------------------------
void TfEditElem::vgiveelem(int pos, int & element, byte & variant)
{
    element = felemindex;
    variant = pos;
}
//---------------------------------------------------------------------------
void TfEditElem::rcallback(int element, byte variant)
{
    update_repbuttons();
}
//---------------------------------------------------------------------------
void TfEditElem::rgiveelem(int pos, int & element, byte & variant)
{
    TElementVariant v = elements->RepositoryItem[cbRepository->ItemIndex][pos];
    element = v.element;
    variant = v.variant;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::cbRepositoryChange(TObject *Sender)
{
    if (cbRepository->ItemIndex >= 0)
        rgrid->set_items(elements->RepositoriesCount[cbRepository->ItemIndex]);
    else
        rgrid->set_items(0);

    update_repbuttons();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbDelRepVarClick(TObject *Sender)
{
    elements->RemoveFromRepository(cbRepository->ItemIndex, rgrid->pos);
    int ix = rgrid->pos;
    rgrid->set_items(elements->RepositoriesCount[cbRepository->ItemIndex]);
    rgrid->pos = ix;
    update_repbuttons();
    modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbDelRepClick(TObject *Sender)
{
    if (elements->RepositoriesCount[cbRepository->ItemIndex])
    {
        if (MsgBox("The selected element repository is not empty. Do you want to delete it?", "Warning", MB_YESNO) == IDNO)
            return;
    }
    elements->DeleteRepository(cbRepository->ItemIndex);
    update_repositories(cbRepository->ItemIndex);
    modified = true;
}
//---------------------------------------------------------------------------
void TfEditElem::update_repbuttons()
{
    sbDelRep->Enabled = cbRepository->ItemIndex >= 0;
    sbToRep->Enabled = elem && elem->variantcount > fvarindex && elem->variants[fvarindex].strokecount && elements->RepositoryCount && cbRepository->ItemIndex >= 0;
    sbDelRepVar->Enabled = rgrid->pos >= 0;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::pbVariantsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button == mbRight && vgrid->CellAtPos(Point(X, Y)) >= 0)
    {
        ReleaseCapture();
        TPoint p = pbVariants->ClientToScreen(Point(X, Y));
        pmVariantEdit->Popup(p.x, p.y);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miMergeClick(TObject *Sender)
{
    if (btnReady->Enabled && (MsgBox("Merging this element into another finalizes your current changes! Do you want to continue?", "Warning", MB_YESNO) == IDNO || !Finalize()))
        return;

    int e;
    if ((e = elementbrowse(felemindex)) <= 0 || e == felemindex || elements->Items[e]->variantcount + elem->variantcount > 32)
        return;

    TKanjiElement *dest = elements->Items[e];
    TKanjiElement *src = elements->Items[felemindex];
    for (int ix = 0; ix < 4; ++ix)
        if (dest->parts[ix] != src->parts[ix])
        {
            MsgBox("Cannot merge elements with different parts. Change the parts of one element and try again.", "Error", MB_OK);
            return;
        }

    strokelist->Clear();
    partlist->Clear();

    elements->Merge(felemindex, e);
    modified = true;

    if (e > felemindex)
        e--;

    fvarindex = -1;
    felemindex = -1;

    elemindex = e;
    update_repositories(cbRepository->ItemIndex);
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbRegenClick(TObject *Sender)
{
    elements->RepositionAll();
    modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbAddCharClick(TObject *Sender)
{
    if (!elem || !elem->variantcount || !elem->variants[0].standalone || !elem->variants[0].strokecount)
        return;
    int val = 1;
    if (!InputQueryInt(L"Which stroke should be added to the stroke model list?", L"Query", val, 1, elem->variants[0].strokecount))
        return;

    if (sbKModels->Down)
        elements->AddKanjiModel(elem, 0, val - 1);
    else
        elements->AddCharacterModel(elem, 0, val - 1);
    mgrid->changed(mgrid->count);

    modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbDelCharClick(TObject *Sender)
{
    if (mgrid->pos < 0 || !mgrid->count)
        return;

    if (MsgBox("Would you like to delete the selected character stroke models from the database?", "Query", MB_YESNO) == IDNO)
        return;

    int p = mgrid->pos;
    for (int ix = mgrid->selcount - 1; ix >= 0; --ix)
    {
        if (sbKModels->Down)
            elements->RemoveKanjiModel(mgrid->selpos[ix]);
        else
            elements->RemoveCharacterModel(mgrid->selpos[ix]);
    }

    mgrid->changed(min(p, (sbKModels->Down ? models : cmodels)->Count));

    modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::btnGenerateClick(TObject *Sender)
{
    elements->RegenerateRecognizeData(numElem->value - 1);
    fill_recparts();
    modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::btnDuplicateClick(TObject *Sender)
{
    if (btnReady->Enabled && (MsgBox("Duplicating this element finalizes your current changes! Do you want to continue?", "Warning", MB_YESNO) == IDNO || !Finalize()))
        return;

    elements->Duplicate(numElem->value - 1);
    modified = true;
    controlinit();
    numElem->value = numElem->max;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbKModelsClick(TObject *Sender)
{
    mgrid->set_models(((TSpeedButton*)Sender)->Tag == 0 ? models : cmodels);
}
//---------------------------------------------------------------------------
void TfEditElem::clear_recparts()
{
    for (unsigned int ix = 0; ix < knumbers.TrueCount; ++ix)
        delete knumbers.TrueItems[ix];
    knumbers.Clear();
    for (unsigned int ix = 0; ix < cnumbers.TrueCount; ++ix)
        delete cnumbers.TrueItems[ix];
    cnumbers.Clear();
}
//---------------------------------------------------------------------------
void TfEditElem::fill_recparts()
{
    clear_recparts();
    if (felemindex < 0 || !elements->Items[felemindex]->recdata)
    {
        mgrid->Redraw();
        return;
    }

    TRecognizeData *rec = elements->Items[felemindex]->recdata;
    for (int ix = 0; ix < elements->Items[felemindex]->variants[0].strokecount; ++ix)
    {
        if (rec[ix].data.index < models->Count)
        {
            TIntegerList *l;
            if (!knumbers.Items[rec[ix].data.index])
                knumbers.Items[rec[ix].data.index] = l = new TIntegerList;
            else
                l = knumbers.Items[rec[ix].data.index];
            l->Add(ix);
        }
        else
        {
            if (!cnumbers.Items[rec[ix].data.index - models->Count])
                cnumbers.Items[rec[ix].data.index - models->Count] = new TIntegerList;
            (cnumbers.Items[rec[ix].data.index - models->Count])->Add(ix);
        }
    }

    mgrid->Redraw();
}
//---------------------------------------------------------------------------
UnicodeString TfEditElem::strokenumproc(int ix)
{
    UnicodeString str = "";
    if (sbKModels->Down)
    {
        if (knumbers.Items[ix])
        {
            for (int iy = 0; iy < knumbers.Items[ix]->Count; ++iy)
            {
                if (iy)
                    str += ", ";
                str += (knumbers.Items[ix])->Items[iy] + 1;
            }
        }
    }
    else
    {
        if (cnumbers.Items[ix])
        {
            for (int iy = 0; iy < cnumbers.Items[ix]->Count; ++iy)
            {
                if (iy)
                    str += ", ";
                str += (cnumbers.Items[ix])->Items[iy] + 1;
            }
        }
    }

    return str;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::miTipS1Click(TObject *Sender)
{
    TZStrokeType type;
    bool remove = false;
    switch (((TMenuItem*)Sender)->Tag)
    {
    case 0:
        remove = true;
        type = stStartPointed;
        break;
    case 3:
        remove = true;
        type = stEndPointed;
        break;
    case 1:
        type = stStartThin;
        break;
    case 2:
        type = stStartPointed;
        break;
    case 4:
        type = stEndThin;
        break;
    case 5:
        type = stEndPointed;
        break;
    case 6:
        type = stSingleDot;
        break;
    };
    strokelist->changetip(type, remove);
    fundo->TipChanged();
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::btnNextClick(TObject *Sender)
{
    int curr = numElem->value;
    for (int ix = curr; ix < elements->Count; ++ix)
    {
        TKanjiElement *ke = elements->Items[ix];
        bool found = false;
        for (int j = 0; j < ke->variantcount && !found; ++j)
        {
            if (ke->variants[j].standalone)
                found = true;
        }
        if (found)
        {
            numElem->value = ix + 1;
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::btnFixClick(TObject *Sender)
{
    if (fixelement(elem, varindex))
    {
        fundo->FixChanged();
        redraw();
    }
}
//---------------------------------------------------------------------------
bool TfEditElem::fixelement(TKanjiElement *e, byte variant, bool onlydimensions)
{
    if (!e->variantcount || variant >= e->variantcount)
        return false;
    TKanjiVariant *v = &e->variants[variant];

    if (v->standalone)
    {
        TRect r = Rect(99999, 99999, 0, 0);
        for (int ix = 0; ix < v->strokecount; ++ix)
        {
            for (int iy = 0; iy < v->strokes[ix].pointcount; ++iy)
            {
                r.Left = min((int)r.Left, v->strokes[ix].points[iy].x);
                r.Right = max((int)r.Right, v->strokes[ix].points[iy].x);
                r.Top = min((int)r.Top, v->strokes[ix].points[iy].y);
                r.Bottom = max((int)r.Bottom, v->strokes[ix].points[iy].y);
            }
        }
        if (onlydimensions && (r.Width() < 7000 && r.Height() < 7000))
            return false;

        double mul = min(8500.0 / (r.Width() ? r.Width() : 1), 8000.0 / (r.Height() ? r.Height() : 1));
        int x = (10000 - r.Width() * mul) / 2;
        int y = (10000 - r.Height() * mul) / 2;

        TRect r2 = r;
        r = Rect(99999, 99999, 0, 0);
        for (int ix = 0; ix < v->strokecount; ++ix)
        {
            for (int iy = 0; iy < v->strokes[ix].pointcount; ++iy)
            {
                v->strokes[ix].points[iy].x = (v->strokes[ix].points[iy].x - r2.Left) * mul + x;
                v->strokes[ix].points[iy].y = (v->strokes[ix].points[iy].y - r2.Top) * mul + y;

                if (v->strokes[ix].points[iy].type == ptCurve)
                {
                    v->strokes[ix].points[iy].c1x = (v->strokes[ix].points[iy].c1x - r2.Left) * mul + x;
                    v->strokes[ix].points[iy].c1y = (v->strokes[ix].points[iy].c1y - r2.Top) * mul + y;
                    v->strokes[ix].points[iy].c2x = (v->strokes[ix].points[iy].c2x - r2.Left) * mul + x;
                    v->strokes[ix].points[iy].c2y = (v->strokes[ix].points[iy].c2y - r2.Top) * mul + y;
                }

                r.Left = min((int)r.Left, v->strokes[ix].points[iy].x);
                r.Right = max((int)r.Right, v->strokes[ix].points[iy].x);
                r.Top = min((int)r.Top, v->strokes[ix].points[iy].y);
                r.Bottom = max((int)r.Bottom, v->strokes[ix].points[iy].y);
            }
        }

        v->x = r.Left;
        v->y = r.Top;
        v->width = r.Width();
        v->height = r.Height();
    }
    else
    {
        const int NEAR_LIMIT = 250;

        if (onlydimensions && (v->width < 7000 && v->height < 7000))
            return false;

        bool lefts[3];
        memset(lefts, false, sizeof(bool)*3);
        bool tops[3];
        memset(tops, false, sizeof(bool)*3);
        bool rights[3];
        memset(rights, false, sizeof(bool)*3);
        bool bottoms[3];
        memset(bottoms, false, sizeof(bool)*3);

        if (!onlydimensions)
        {
            for (int ix = 0; ix < 3 && e->parts[ix + 1] >= 0 && e->parts[ix] >= 0; ++ix)
            {
                if (abs(v->positions[ix].x - v->positions[ix + 1].x) < NEAR_LIMIT)
                    lefts[ix] = true;
                if (abs(v->positions[ix].y - v->positions[ix + 1].y) < NEAR_LIMIT)
                    tops[ix] = true;
                if (abs(v->positions[ix].x + v->positions[ix].width - v->positions[ix + 1].x - v->positions[ix + 1].width) < NEAR_LIMIT)
                    rights[ix] = true;
                if (abs(v->positions[ix].y + v->positions[ix].height - v->positions[ix + 1].y - v->positions[ix + 1].height) < NEAR_LIMIT)
                    bottoms[ix] = true;
            }
            for (int ix = 1; ix < 4 && e->parts[ix] >= 0; ++ix)
            {
                for (int iy = ix - 1; iy >= 0 && lefts[iy]; --iy)
                    v->positions[iy].x = v->positions[ix].x;
                for (int iy = ix - 1; iy >= 0 && tops[iy]; --iy)
                    v->positions[iy].y = v->positions[ix].y;
                for (int iy = ix - 1; iy >= 0 && rights[iy]; --iy)
                    v->positions[iy].width = (v->positions[ix].x + v->positions[ix].width) - v->positions[iy].x;
                for (int iy = ix - 1; iy >= 0 && bottoms[iy]; --iy)
                    v->positions[iy].height = (v->positions[ix].y + v->positions[ix].height) - v->positions[iy].y;
            }
        }

        TRect r = Rect(99999, 99999, 0, 0);
        for (int ix = 0; ix < 4 && e->parts[ix] >= 0; ++ix)
        {
            r.Left = min((int)r.Left, v->positions[ix].x);
            r.Right = max((int)r.Right, v->positions[ix].x + v->positions[ix].width);
            r.Top = min((int)r.Top, v->positions[ix].y);
            r.Bottom = max((int)r.Bottom, v->positions[ix].y + v->positions[ix].height);
        }
        double mul = min(8500.0 / (r.Width() ? r.Width() : 1), 8000.0 / (r.Height() ? r.Height() : 1));
        int x = (10000 - r.Width() * mul) / 2;
        int y = (10000 - r.Height() * mul) / 2;

        TRect r2 = r;
        r = Rect(99999, 99999, 0, 0);

        for (int ix = 0; ix < 4 && e->parts[ix] >= 0; ++ix)
        {
            v->positions[ix].x = (v->positions[ix].x - r2.Left) * mul + x;
            v->positions[ix].y = (v->positions[ix].y - r2.Top) * mul + y;
            v->positions[ix].width *= mul;
            v->positions[ix].height *= mul;

            r.Left = min((int)r.Left, v->positions[ix].x);
            r.Right = max((int)r.Right, v->positions[ix].x + v->positions[ix].width);
            r.Top = min((int)r.Top, v->positions[ix].y);
            r.Bottom = max((int)r.Bottom, v->positions[ix].y + v->positions[ix].height);
        }
        v->x = r.Left;
        v->y = r.Top;
        v->width = r.Width();
        v->height = r.Height();
    }

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::sbFixAllClick(TObject *Sender)
{
    if (elem->owner != NO_OWNER)
        fixelement(elem, 0, true);
    for (int ix = 0; ix < elements->Count; ++ix)
    {
        if (elements->Items[ix]->owner == NO_OWNER)
            continue;
        fixelement(elements->Items[ix], 0, true);
    }
    redraw();
}
//---------------------------------------------------------------------------
void __fastcall TfEditElem::btnSaveVarNameClick(TObject *Sender)
{
    if (varnames->Strings[fvarindex] == edVariant->Text.Trim())
        return;

    varnames->Strings[fvarindex] = edVariant->Text.Trim();
    fundo->VarnameChanged();
}
//---------------------------------------------------------------------------
UnicodeString TfEditElem::savevarname()
{
    return varnames->Strings[fvarindex];
}
//---------------------------------------------------------------------------
void TfEditElem::undovarname(UnicodeString vname)
{
    edVariant->Text = vname;
    varnames->Strings[fvarindex] = vname;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(35);
#endif

