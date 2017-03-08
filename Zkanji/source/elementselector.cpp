//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "elementselector.h"
#include "kanjielement.h"
#include "module.h"
#include "numedit.h"
#include "kanjiedit.h"
#include "kanji.h"
#include "romajizer.h"
#include "settings.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(38);
#endif

TfSelectElem *fSelectElem;

//---------------------------------------------------------------------------
TElementSelectGrid::TElementSelectGrid(TZPaintBox *apb, int aboxsize, TFuncPtr acallback) : fitems(NULL), fcount(0), inherited(apb, aboxsize, false, acallback)
{
}
//---------------------------------------------------------------------------
TElementSelectGrid::~TElementSelectGrid()
{
    delete[] fitems;
    fcount = 0;
}
//---------------------------------------------------------------------------
int TElementSelectGrid::get_count()
{
    return fcount;
}
//---------------------------------------------------------------------------
void TElementSelectGrid::DoDrawCell(TCanvas *c, const TRect &r, int acell)
{
    c->Brush->Color = (TColor)colorsettings.background;
    c->Pen->Color = (TColor)colorsettings.maintext;

    if (selected[acell])
        c->Brush->Color = focused ? (TColor)colorsettings.highlight : coloravg((TColor)colorsettings.highlight, (TColor)colorsettings.background, 0.8);
//c->Brush->Color = (TColor)(focused ? 0xdadada : 0xeaeaea);
    if (acell == mousecell)
        c->Brush->Color = coloravg((TColor)0xe6fbfb, c->Brush->Color, 0.5);
//DARKEN(c->Brush->Color,0xf6fdfd);

    c->FillRect(r);

//c->Pen->Width = 2;
    elements->Draw(Rect(r.Left + 2, r.Top + 2, r.Right - 3, r.Bottom - 3), c->Handle, 1.8, c->Pen->Color, fitems[acell], 0);

    c->Pen->Width = 1;
    c->Pen->Color = (TColor)colorsettings.gridline;
    c->MoveTo(r.Right, r.Top);
    c->LineTo(r.Right, r.Bottom);
    c->LineTo(r.Left - 1, r.Bottom);

    c->Brush->Style = bsClear;
    TSize s;
    UnicodeString str = IntToStr(fitems[acell] + 1);
    GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);
    c->Font->Color = (TColor)colorsettings.wordtype;
    c->TextOutW(r.Right - s.cx - 2, r.Bottom - s.cy - 1, str);
    c->Brush->Style = bsSolid;
}
//---------------------------------------------------------------------------
void TElementSelectGrid::set_items(TIntegerList *items, int newpos)
{
    delete[] fitems;
    fitems = NULL;

    fcount = items->Count;
    if (fcount)
    {
        fitems = new int[fcount];
        for (int ix = 0; ix < fcount; ++ix)
            fitems[ix] = items->Items[ix];
    }
    changed(newpos);
}
//---------------------------------------------------------------------------
int TElementSelectGrid::get_item(int ix)
{
    if (ix < 0 || ix >= fcount)
        THROW(L"Index out of range for element selection grid!");

    return fitems[ix];
}
//---------------------------------------------------------------------------
int TElementSelectGrid::indexof(int elem)
{
    for (int ix = 0; ix < fcount; ++ix)
        if (fitems[ix] == elem)
            return ix;
    return -1;
}
//---------------------------------------------------------------------------
int TElementSelectGrid::elemindex(int x, int y)
{
    int ix = x / BoxSize + Columns * (y / BoxSize + pb->VPos);
    if (count <= ix)
        return -1;
    return fitems[ix];

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TfSelectElem::TfSelectElem(TComponent* Owner) : TBaseForm(Owner)
{
    numStroke = new TNumEdit(edStroke, 1, 32, true);
    numStroke1 = new TNumEdit(edStroke1, 1, 32, true);
    numStroke2 = new TNumEdit(edStroke2, 1, 32, true);
    numStroke3 = new TNumEdit(edStroke3, 1, 32, true);
    numStroke4 = new TNumEdit(edStroke4, 1, 32, true);
    numIndex = new TNumEdit(edIndex, 1, elements->Count, false);
    numStroke->callback = ncallback;
    numStroke1->callback = n1callback;
    numStroke2->callback = n2callback;
    numStroke3->callback = n3callback;
    numStroke4->callback = n4callback;
    numIndex->callback = icallback;
    numIndex->allow_empty = true;

    edreading = new TKanjiEdit(edReading, false, false, NULL, rcallback);

    egrid = new TElementSelectGrid(pbGrid, 50 * scaleres, ecallback);

    fnofilter = false;
}
//---------------------------------------------------------------------------
__fastcall TfSelectElem::~TfSelectElem()
{
    delete egrid;

    delete edreading;

    delete numStroke;
    delete numStroke1;
    delete numStroke2;
    delete numStroke3;
    delete numStroke4;
    delete numIndex;
}
//---------------------------------------------------------------------------
int TfSelectElem::ShowModalEx(int &val)
{

    if (val >= 0)
    {
        fnofilter = true;
        TKanjiElement *e = elements->Items[val];
        cbPattern->ItemIndex = e->pattern;
        numStroke->value = e->variants[0].strokecount;

        if (!e->variants[0].standalone)
        {
            TNumEdit *ns[] = { numStroke1, numStroke2, numStroke3, numStroke4 };
            TComboBox *pt[] = { cbPattern1, cbPattern2, cbPattern3, cbPattern4 };
            TComboBox *b[] = {
                cbS11, cbS12, cbS13, cbS14,
                cbS21, cbS22, cbS23, cbS24,
                cbS31, cbS32, cbS33, cbS34,
                cbS41, cbS42, cbS43, cbS44
            };
            for (int ix = 0; ix < 4 && e->parts[ix] >= 0; ++ix)
            {
                TKanjiElement *pe = elements->Items[e->parts[ix]];
                ns[ix]->value = pe->variants[e->variants[0].positions[ix].variant].strokecount;
                pt[ix]->ItemIndex = pe->pattern;
                pt[ix]->OnChange(pt[ix]);
                for (int j = 0; j < 4 && pe->parts[j] >= 0; ++j)
                {
                    b[ix * 4 + j]->ItemIndex = elements->Items[pe->parts[j]]->pattern;
                }
            }
        }
        fnofilter = false;
    }
    Filter();

    if (val >= 0)
        egrid->pos = egrid->indexof(val);

    int r = ShowModal();
    if (r == mrOk)
        val = egrid->items[egrid->pos];

    return r;
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::cbPattern1MeasureItem(TWinControl *Control, int Index, int &Height)
{
    Height = Index < 0 ? 20 : 22;
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::cbPattern1DrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State)
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
void __fastcall TfSelectElem::BitBtn2Click(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::btnOkClick(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void TfSelectElem::set_enabled(TPanel *p, bool enable)
{
    for (int ix = 0; ix < p->ControlCount; ++ix)
        p->Controls[ix]->Enabled = enable;
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::cbPatternChange(TObject *Sender)
{
    TPanel *pp[] = { p1, p2, p3, p4 };
    int patnum[] = { 4, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 3, 2, 3, 4, 3, 4, 4, 4, 4, 4 };

    for (int ix = 0; ix < patnum[cbPattern->ItemIndex]; ++ix)
        enabled[pp[ix]] = true;
    for (int ix = patnum[cbPattern->ItemIndex]; ix < 4; ++ix)
        enabled[pp[ix]] = false;

    Filter();
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::cbPattern1Change(TObject *Sender)
{
    int patnum[] = { 4, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 3, 2, 3, 4, 3, 4, 4, 4, 4, 4 };
    TComboBox *b[] = {
        cbS11, cbS12, cbS13, cbS14,
        cbS21, cbS22, cbS23, cbS24,
        cbS31, cbS32, cbS33, cbS34,
        cbS41, cbS42, cbS43, cbS44
    };
    int tag = ((TComboBox*)Sender)->Parent->Tag;

    for (int ix = 0; ix < patnum[((TComboBox*)Sender)->ItemIndex]; ++ix)
        b[tag*4 + ix]->Enabled = true;
    for (int ix = patnum[((TComboBox*)Sender)->ItemIndex]; ix < 4; ++ix)
        b[tag*4 + ix]->Enabled = false;

    TCheckBox *cb[] = { cb1, cb2, cb3, cb4 };
    if (!cb[tag]->Checked)
        cb[tag]->Checked = true;
    else
        Filter();
}
//---------------------------------------------------------------------------
void TfSelectElem::ncallback()
{
    if (!cbStroke->Checked)
        cbStroke->Checked = true;
    else
        Filter();
}
//---------------------------------------------------------------------------
void TfSelectElem::n1callback()
{
    if (!cbStroke1->Checked)
        cbStroke1->Checked = true;
    else if (!cb1->Checked)
        cb1->Checked = true;
    else
        Filter();
}
//---------------------------------------------------------------------------
void TfSelectElem::n2callback()
{
    if (!cbStroke2->Checked)
        cbStroke2->Checked = true;
    else if (!cb2->Checked)
        cb2->Checked = true;
    else
        Filter();
}
//---------------------------------------------------------------------------
void TfSelectElem::n3callback()
{
    if (!cbStroke3->Checked)
        cbStroke3->Checked = true;
    else if (!cb3->Checked)
        cb3->Checked = true;
    else
        Filter();
}
//---------------------------------------------------------------------------
void TfSelectElem::n4callback()
{
    if (!cbStroke4->Checked)
        cbStroke4->Checked = true;
    else if (!cb4->Checked)
        cb4->Checked = true;
    else
        Filter();
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::cbStroke1Click(TObject *Sender)
{
    TCheckBox *cb[] = { cb1, cb2, cb3, cb4 };
    if (!cb[((TControl*)Sender)->Parent->Tag]->Checked)
        cb[((TControl*)Sender)->Parent->Tag]->Checked = true;
    else
        Filter();
}
//---------------------------------------------------------------------------
void TfSelectElem::Filter()
{
    if (fnofilter)
        return;
    fnofilter = true;
    TIntegerList *l = new TIntegerList;

    int old = egrid->pos >= 0 ? egrid->items[egrid->pos] : -1;
    int newpos = -1;

    try
    {
        l->Capacity = elements->Count;
        for (int ix = 0; ix < elements->Count; ++ix)
        {
            TKanjiElement *elem = elements->Items[ix];

            if (elem->variantcount == 0)
                continue;

            if (cbIndex->Checked && edIndex->Text.Length())
            {
                bool found = false;
                for (int ix = 0; ix < elem->parentcnt && !found; ++ix)
                    found = elem->parents[ix] == numIndex->value - 1;
                if (!found)
                    continue;
            }

            if (elem->owner >= NO_OWNER && ((cbMeaning->Checked && edMeaning->Text.Trim().Length()) || (cbReading->Checked && edreading->Text.Trim().Length())))
                continue;

            if (cbPattern->ItemIndex && elem->pattern != cbPattern->ItemIndex)
                continue;
            if (cbStroke->Checked && elem->variants[0].strokecount != numStroke->value)
                continue;

            if (cb1->Enabled && cb1->Checked)
            {
                if (elem->parts[0] < 0)
                    continue;
                TKanjiElement *e = elements->Items[elem->parts[0]];
                if (cbPattern1->ItemIndex && e->pattern != cbPattern1->ItemIndex)
                    continue;
                if (cbStroke1->Checked && e->variants[0].strokecount != numStroke1->value)
                    continue;

                if (cbS11->ItemIndex && (e->parts[0] < 0 || elements->Items[e->parts[0]]->pattern != cbS11->ItemIndex))
                    continue;
                if (cbS12->ItemIndex && (e->parts[1] < 0 || elements->Items[e->parts[1]]->pattern != cbS12->ItemIndex))
                    continue;
                if (cbS13->ItemIndex && (e->parts[2] < 0 || elements->Items[e->parts[2]]->pattern != cbS13->ItemIndex))
                    continue;
                if (cbS14->ItemIndex && (e->parts[3] < 0 || elements->Items[e->parts[3]]->pattern != cbS14->ItemIndex))
                    continue;
            }

            if (cb2->Enabled && cb2->Checked)
            {
                if (elem->parts[1] < 0)
                    continue;
                TKanjiElement *e = elements->Items[elem->parts[1]];
                if (cbPattern2->ItemIndex && e->pattern != cbPattern2->ItemIndex)
                    continue;
                if (cbStroke2->Checked && e->variants[0].strokecount != numStroke2->value)
                    continue;

                if (cbS21->ItemIndex && (e->parts[0] < 0 || elements->Items[e->parts[0]]->pattern != cbS21->ItemIndex))
                    continue;
                if (cbS22->ItemIndex && (e->parts[1] < 0 || elements->Items[e->parts[1]]->pattern != cbS22->ItemIndex))
                    continue;
                if (cbS23->ItemIndex && (e->parts[2] < 0 || elements->Items[e->parts[2]]->pattern != cbS23->ItemIndex))
                    continue;
                if (cbS24->ItemIndex && (e->parts[3] < 0 || elements->Items[e->parts[3]]->pattern != cbS24->ItemIndex))
                    continue;
            }

            if (cb3->Enabled && cb3->Checked)
            {
                if (elem->parts[2] < 0)
                    continue;
                TKanjiElement *e = elements->Items[elem->parts[2]];
                if (cbPattern3->ItemIndex && e->pattern != cbPattern3->ItemIndex)
                    continue;
                if (cbStroke3->Checked && e->variants[0].strokecount != numStroke3->value)
                    continue;

                if (cbS31->ItemIndex && (e->parts[0] < 0 || elements->Items[e->parts[0]]->pattern != cbS31->ItemIndex))
                    continue;
                if (cbS32->ItemIndex && (e->parts[1] < 0 || elements->Items[e->parts[1]]->pattern != cbS32->ItemIndex))
                    continue;
                if (cbS33->ItemIndex && (e->parts[2] < 0 || elements->Items[e->parts[2]]->pattern != cbS33->ItemIndex))
                    continue;
                if (cbS34->ItemIndex && (e->parts[3] < 0 || elements->Items[e->parts[3]]->pattern != cbS34->ItemIndex))
                    continue;
            }

            if (cb4->Enabled && cb4->Checked)
            {
                if (elem->parts[3] < 0)
                    continue;
                TKanjiElement *e = elements->Items[elem->parts[3]];
                if (cbPattern4->ItemIndex && e->pattern != cbPattern4->ItemIndex)
                    continue;
                if (cbStroke4->Checked && e->variants[0].strokecount != numStroke4->value)
                    continue;

                if (cbS41->ItemIndex && (e->parts[0] < 0 || elements->Items[e->parts[0]]->pattern != cbS41->ItemIndex))
                    continue;
                if (cbS42->ItemIndex && (e->parts[1] < 0 || elements->Items[e->parts[1]]->pattern != cbS42->ItemIndex))
                    continue;
                if (cbS43->ItemIndex && (e->parts[2] < 0 || elements->Items[e->parts[2]]->pattern != cbS43->ItemIndex))
                    continue;
                if (cbS44->ItemIndex && (e->parts[3] < 0 || elements->Items[e->parts[3]]->pattern != cbS44->ItemIndex))
                    continue;
            }

            if (elem->owner < NO_OWNER)
            {
                TKanji *kanji = kanjis->Kanjis[elem->owner];
                if (cbMeaning->Checked && edMeaning->Text.Trim().Length())
                {
                    UnicodeString s = edMeaning->Text.LowerCase().Trim();
                    bool found = false;
                    wchar_t *c = kanji->meaning;
                    while (c && !found)
                    {
                        c = WCSSTRI(c, s.c_str());
                        if (c && (kanji->meaning == c || wcschr(cdelim, *(c - 1))))
                            found = true;
                        else if (c)
                            c++;
                    }
                    if (!found)
                        continue;
                }

                UnicodeString rtext = Romanize(edreading->Text.Trim().c_str());
                if (cbReading->Checked && rtext.Length())
                {
                    bool found = false;
                    if (cbOn->Checked && kanji->oncnt)
                        for (int iy = 0; iy < kanji->oncnt && !found; ++iy)
                            if (!wcscmp(Romanize(kanji->on[iy]).c_str(), rtext.c_str()))
                                found = true;

                    if (!found && cbKun->Checked && kanji->kuncnt)
                        for (int iy = 0; iy < kanji->kuncnt && !found; iy++)
                        {
                            UnicodeString r = kanji->kun[iy];
                            if (cbOku->Checked && rtext == Romanize(r.c_str()))
                                found = true;
                            if (!cbOku->Checked)
                            {
                                int n = r.Pos(L".");
                                if (rtext == Romanize(r.SubString(1, (n ? n - 1 : r.Length())).c_str()))
                                    found = true;
                            }
                        }

                    if (!found)
                        continue;
                }
            }

            if (old == ix)
                newpos = l->Count;
            l->Add(ix);
        }
    }
    catch (...)
    {
        ;
    }
    egrid->set_items(l, newpos);
    delete l;

    fnofilter = false;
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::sbResetClick(TObject *Sender)
{
    Reset(true);
}
//---------------------------------------------------------------------------
void TfSelectElem::Reset(bool filter)
{
    fnofilter = true;
    bool changed = false;
    try
    {
        TComboBox *b[] = {
            cbPattern,
            cbPattern1, cbPattern2, cbPattern3, cbPattern4,
            cbS11, cbS12, cbS13, cbS14,
            cbS21, cbS22, cbS23, cbS24,
            cbS31, cbS32, cbS33, cbS34,
            cbS41, cbS42, cbS43, cbS44
        };
        for (unsigned int ix = 0; ix < sizeof(b) / sizeof(TComboBox*); ++ix)
        {
            if (b[ix]->ItemIndex != 0)
            {
                changed = true;
                b[ix]->ItemIndex = 0;
                b[ix]->OnChange(b[ix]);
            }
        }

        TNumEdit *ed[] = { numStroke, numStroke1, numStroke2, numStroke3, numStroke4 };
        for (unsigned int ix = 0; ix < sizeof(ed) / sizeof(TNumEdit*); ++ix)
        {
            if (ed[ix]->value != 1)
            {
                changed = true;
                ed[ix]->value = 1;
            }
        }
        if (edIndex->Text.Trim().Length())
        {
            changed = true;
            edIndex->Text = "";
        }
        if (edMeaning->Text.Trim().Length())
        {
            changed = true;
            edMeaning->Text = "";
        }
        if (edreading->Text.Trim().Length())
        {
            changed = true;
            edreading->Text = L"";
        }

        TCheckBox *cb[] = {
            cbStroke,
            cbStroke1, cbStroke2, cbStroke3, cbStroke4,
            cb1, cb2, cb3, cb4, cbIndex, cbReading, cbMeaning
        };
        for (unsigned int ix = 0; ix < sizeof(cb) / sizeof(TCheckBox*); ++ix)
            if (cb[ix]->Checked)
            {
                changed = true;
                cb[ix]->Checked = false;
            }

        TCheckBox *cb2[] = { cbOn, cbKun, cbOku };
        for (unsigned int ix = 0; ix < sizeof(cb2) / sizeof(TCheckBox*); ++ix)
            if (!cb2[ix]->Checked)
            {
                changed = true;
                cb2[ix]->Checked = true;
            }

    }
    catch (...)
    {
    }
    fnofilter = false;

    if (changed && filter)
        Filter();
}
//---------------------------------------------------------------------------
void TfSelectElem::ecallback()
{
    btnOk->Enabled = egrid->pos >= 0;
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::FormPaint(TObject *Sender)
{
    TCanvas *c = Canvas;
    TRect r = Rect(0, pbGrid->Top + pbGrid->Height + 1, ClientWidth, ClientHeight);
    GradientFillCanvas(c, COLORINC(clBtnFace, 0x101010), clBtnFace, r, gdVertical);
    r = Rect(0, 0, pbGrid->Left - 1, b1->Top - 1);
    GradientFillCanvas(c, COLORINC(clBtnFace, 0x101010), clBtnFace, r, gdVertical);

    c->Pen->Color = clBtnShadow;
    c->MoveTo(pbGrid->Left - 1, 0);
    c->LineTo(pbGrid->Left - 1, pbGrid->Top + pbGrid->Height);

    TPanel *pp[] = { p1, p2, p3, p4 };
    for (unsigned int ix = 0; ix < sizeof(pp) / sizeof(TPanel*); ++ix)
    {
        r = pp[ix]->ClientRect;
        InflateRect(&r, -2, -2);
        OffsetRect(&r, pp[ix]->Left, pp[ix]->Top);
        GradientFillCanvas(c, COLORINC(clBtnFace, 0x101010), clBtnFace, r, gdVertical);
    }
}
//---------------------------------------------------------------------------
void TfSelectElem::icallback()
{
    if (!cbIndex->Checked)
        cbIndex->Checked = true;
    else
        Filter();
}
//---------------------------------------------------------------------------
void TfSelectElem::rcallback()
{
    if (edReading->Focused() && edreading->CanInput() && edreading->Text != "" && !cbReading->Checked)
        cbReading->Checked = true;
    else
        Filter();
}
//---------------------------------------------------------------------------

void __fastcall TfSelectElem::edMeaningChange(TObject *Sender)
{
    if (edMeaning->Focused() && edMeaning->Text.Trim().Length() && !cbMeaning->Checked)
        cbMeaning->Checked = true;
    else
        Filter();
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::cbMeaningClick(TObject *Sender)
{
    if (cbMeaning->Checked && Visible)
        edMeaning->SetFocus();
    if (edMeaning->Text.Trim().Length())
        Filter();
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::cbReadingClick(TObject *Sender)
{
    if (cbReading->Checked && Visible)
        edReading->SetFocus();
    if (edreading->Text.Trim().Length())
        Filter();
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::cbIndexClick(TObject *Sender)
{
    if (cbIndex->Checked && Visible)
        edIndex->SetFocus();
    if (edIndex->Text.Trim().Length())
        Filter();
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::cb1Click(TObject *Sender)
{
    Filter();
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::cbStrokeClick(TObject *Sender)
{
    if (cbStroke->Checked && Visible)
        edStroke->SetFocus();
    if (edStroke->Text.Trim().Length())
        Filter();
}
//---------------------------------------------------------------------------
void __fastcall TfSelectElem::pbGridMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button != mbLeft || !Shift.Contains(ssDouble))
        return;
    ReleaseCapture();
    int eix = egrid->elemindex(X, Y);
    Reset(false);
    numIndex->value = eix + 1;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(39);
#endif

