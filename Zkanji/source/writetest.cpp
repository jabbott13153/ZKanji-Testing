//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "writetest.h"
#include "writeobj.h"
#include "numedit.h"
#include "kanjielement.h"
#include "elementpoints.h"
#include "kanji.h"
#include "settings.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(202);
#endif

TfWriter *fWriter = NULL;

//---------------------------------------------------------------------------
TWriteObj::TWriteObj(TZPaintBox *fb) : fcallback(NULL), TStrokeObjBase(fb), fdebug(true), fhint(false)
{
    tester = new TStrokeTester();
}
//---------------------------------------------------------------------------
TWriteObj::~TWriteObj()
{
    delete tester;
}
//---------------------------------------------------------------------------
void TWriteObj::set_debug(bool val)
{
    if (fdebug == val)
        return;

    fdebug = val;
    fWriter->memo->Lines->Clear();
    if (!fdebug && fhint)
    {
        clear_bmp();
        TCanvas *c = bmp->Canvas;
        //c->Pen->Width = 8;
        //c->Pen->Color = clBlack;
        if (tester->stroke)
            elements->Draw(Rect(0, 0, bmp->Width, bmp->Height), c->Handle, 8, 0, element, 0, 0, tester->stroke - 1);
        //c->Pen->Width = 5;
        //c->Pen->Color = (TColor)0xf0f0f0;
        elements->Draw(Rect(0, 0, bmp->Width, bmp->Height), c->Handle, 5.0, 0xf0f0f0, element, 0, tester->stroke, tester->stroke);
        pb->Invalidate();
    }
}
//---------------------------------------------------------------------------
void TWriteObj::set_hint(bool val)
{
    if (fhint == val)
        return;
    fhint = val;
    if (fdebug)
        return;
    TCanvas *c = bmp->Canvas;
    clear_bmp();
    if (tester->stroke)
    {
        //c->Pen->Width = 8;
        //c->Pen->Color = clBlack;
        elements->Draw(Rect(0, 0, bmp->Width, bmp->Height), c->Handle, 8, 0, element, 0, 0, tester->stroke - 1);
    }
    if (fhint)
    {
        //c->Pen->Width = 5;
        //c->Pen->Color = (TColor)0xf0f0f0;
        elements->Draw(Rect(0, 0, bmp->Width, bmp->Height), c->Handle, 5.0, 0xf0f0f0, element, 0, tester->stroke, tester->stroke);
    }
    pb->Invalidate();
}
//---------------------------------------------------------------------------
void TWriteObj::procstroke()
{
    clear_bmp();

    TCanvas *c = bmp->Canvas;
    c->Pen->Width = 8;
    if (fdebug)
        elements->Draw(Rect(0, 0, bmp->Width, bmp->Height), c->Handle, 8, 0, element, 0, 0, tester->stroke - 1);

    TPointListEx *dest = NULL;
    TPointListEx *src = NULL;
    TPointListEx *stroke = NULL;
    try
    {
        dest = new TPointListEx;
        src = new TPointListEx;
        stroke = new TPointListEx;
        if (fdebug)
            draw(points);
        fWriter->memo->Lines->Clear();
        bool match = tester->test(points, dest, stroke, src, fdebug ? fWriter->memo->Lines : NULL);
        if (fdebug)
        {
            draw(stroke);

            for (int ix = 0; ix < dest->Count; ++ix)
            {
                if (ix < src->Count && ix < dest->Count)
                {
                    c->Pen->Width = 1;
                    c->Pen->Color = clMaroon;
                    c->MoveTo((src->Items[ix]->x*pb->ClientWidth) / 10000, (src->Items[ix]->y*pb->ClientHeight) / 10000);
                    c->LineTo((dest->Items[ix]->x*pb->ClientWidth) / 10000, (dest->Items[ix]->y*pb->ClientHeight) / 10000);
                }
                c->Pen->Width = max(3, pb->ClientWidth / 60);
                if (ix < dest->Count)
                {
                    c->Pen->Color = clRed;
                    TPoint p = Point((dest->Items[ix]->x * pb->ClientWidth) / 10000, (dest->Items[ix]->y * pb->ClientHeight) / 10000);
                    c->Ellipse(p.x - 2, p.y - 2, p.x + 2, p.y + 2);
                }
                if (ix < src->Count)
                {
                    c->Pen->Color = clBlue;
                    TPoint p = Point((src->Items[ix]->x * pb->ClientWidth) / 10000, (src->Items[ix]->y * pb->ClientHeight) / 10000);
                    c->Ellipse(p.x - 2, p.y - 2, p.x + 2, p.y + 2);
                }
            }
        }
        else
        {
            //c->Pen->Color = clBlack;
            //c->Pen->Width = 8;
            elements->Draw(Rect(0, 0, bmp->Width, bmp->Height), c->Handle, 8.0, 0, element, 0, 0, tester->stroke - (match ? 0 : 1));
        }

        c->Font->Size = 24;
        c->Font->Color = match ? clGreen : clRed;
        c->TextOut(5, 5, match ? "O" : "X");

        if (match)
        {
            if (!fWriter->btnLockStroke->Down)
            {
                if (tester->stroke < elements->Items[element]->variants[0].strokecount - 1)
                {
                    tester->stroke++;
                    if (fhint && !fdebug)
                    {
                        //c->Pen->Color = (TColor)0xf0f0f0;
                        //c->Pen->Width = 5;
                        elements->Draw(Rect(0, 0, bmp->Width, bmp->Height), c->Handle, 5.0, 0xf0f0f0, element, 0, tester->stroke, tester->stroke);
                    }
                }
                else
                {
                    tester->stroke = 0;
                    if (fhint && !fdebug)
                    {
                        //c->Pen->Color = (TColor)0xf0f0f0;
                        //c->Pen->Width = 5;
                        elements->Draw(Rect(0, 0, bmp->Width, bmp->Height), c->Handle, 5.0, 0xf0f0f0, element, 0, 0, 0);
                    }
                }
            }

            if (fcallback)
                fcallback();
        }

        pb->Invalidate();
    }
    catch (...)
    {
    }
    delete dest;
    delete stroke;
    delete src;
}
//---------------------------------------------------------------------------
void TWriteObj::set_elem(int ix)
{
    tester->element = ix;
    clear_bmp();
}
//---------------------------------------------------------------------------
int TWriteObj::get_elem()
{
    return tester->element;
}
//---------------------------------------------------------------------------
void TWriteObj::set_stroke(int ix)
{
    tester->stroke = ix;
}
//---------------------------------------------------------------------------
int TWriteObj::get_stroke()
{
    return tester->stroke;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TfWriter::TfWriter(TComponent* Owner) : TBaseForm(Owner)
{
    wobj = new TWriteObj(pb);
    wobj->element = 0;
    wobj->callback = wcallback;
    nelem = new TNumEdit(edElem, 1, elements->Count);
    nelem->callback = ncallback;
    nstroke = new TNumEdit(edStroke, 1, 1);
    nstroke->callback = scallback;
    ncallback();
}
//---------------------------------------------------------------------------
__fastcall TfWriter::~TfWriter()
{
    delete nelem;
    delete nstroke;
    delete wobj;
}
//---------------------------------------------------------------------------
void TfWriter::ShowModalEx(short element, bool showmatch)
{
    nelem->value = element + 1;
    cbDebug->Checked = showmatch;
    wobj->debug = showmatch;
    cbHint->Enabled = !cbDebug->Checked;
    ShowModal();
}
//---------------------------------------------------------------------------
void TfWriter::ncallback()
{
    nstroke->value = 1;
    wobj->element = nelem->value - 1;
    nstroke->max = elements->Items[wobj->element]->variants[0].strokecount;
    if (elements->Items[wobj->element]->owner < NO_OWNER)
        ch = kanjis->Kanjis[elements->Items[wobj->element]->owner]->ch;
    else
        ch = L' ';
    pbChar->Invalidate();
}
//---------------------------------------------------------------------------
void TfWriter::scallback()
{
    wobj->stroke = nstroke->value - 1;
}
//---------------------------------------------------------------------------
void TfWriter::wcallback()
{
    nstroke->value = wobj->stroke + 1;
}
//---------------------------------------------------------------------------
void __fastcall TfWriter::pbCharPaint(TObject *Sender)
{
    TCanvas *c = pbChar->Canvas;
    int w = pbChar->ClientWidth;
    int h = pbChar->ClientHeight;
    c->MoveTo(0, 0);
    c->LineTo(w - 1, 0);
    c->LineTo(w - 1, h - 1);
    c->LineTo(0, h - 1);
    c->LineTo(0, 0);

    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = h * 0.65;
    SetTextAlign(c->Handle, TA_BASELINE | TA_CENTER);

    TRect r = Rect(1, 1, w - 1, h - 1);
    ExtTextOutW(c->Handle, w / 2, h*0.8, ETO_CLIPPED | ETO_OPAQUE, &r, &ch, 1, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfWriter::cbDebugClick(TObject *Sender)
{
    wobj->debug = cbDebug->Checked;
    cbHint->Enabled = !cbDebug->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfWriter::FormKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == VK_ESCAPE)
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TfWriter::cbHintClick(TObject *Sender)
{
    wobj->hint = cbHint->Checked;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(203);
#endif

