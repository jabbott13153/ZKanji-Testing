//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "wordlistbox.h"
#include "collection.h"
#include "settings.h"
#include "words.h"
#include "lineobj.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(188);
#endif

TfWordBox *fWordBox;
//---------------------------------------------------------------------------

__fastcall TfWordBox::TfWordBox(TComponent* Owner) : TBaseForm(Owner)
{
    ActivateOnShow = false;
    list = NULL;
}
//---------------------------------------------------------------------------
__fastcall TfWordBox::~TfWordBox()
{
    delete list;
}
//---------------------------------------------------------------------------
void TfWordBox::ShowEx(TLineObj *LineObj, TWordIndexList *alist, TRect arect)
{
    lobj = LineObj;
    list = new TWordIndexList(alist);
    item_row = -1;

    area = arect;

    TSize s;
    int wid = 0;
    item_height = 0;

    UnicodeString str;

    Canvas->Font->Name = settings.kanafont;
    Canvas->Font->Charset = settings.kanacharset;
    Canvas->Font->Size = 10;
    for (int i = 0; i < list->Count; i++)
    {
        TWord *w = list->Words[i];
        str = (UnicodeString)w->kanji + L"[" + w->kana + L"]";
        GetTextExtentPoint32W(Canvas->Handle, str.c_str(), str.Length(), &s);
        wid = MAX(wid, (int)s.cx);
        item_height = MAX(item_height, (int)s.cy);
    }
    Width = MAX(wid + 6, area.Width());
    Height = item_height * list->Count + 6;

    tagMONITORINFO inf = MonitorInfo(area.Left, area.Top);

    if (Width + area.Left > inf.rcMonitor.right)
    {
        pos = TBoxPositions() << bpLeft;
        Left = area.Right - Width;
    }
    else
    {
        pos = TBoxPositions() << bpRight;
        Left = area.Left;
    }

    if (area.top - Height < 0)
    {
        pos = pos << bpBottom;
        Top = area.Bottom - 1;
    }
    else
    {
        pos = pos << bpTop;
        Top = area.Top - Height + 1;
    }

    Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TfWordBox::FormPaint(TObject *Sender)
{
    UnicodeString str;
    TRect r;
    TCanvas *c = Canvas;

    SetTextAlign(c->Handle, TA_LEFT | TA_BASELINE);

    c->Pen->Color = clBlack;
    c->MoveTo(0, 0);
    c->LineTo(0, ClientHeight);
    c->MoveTo(ClientWidth - 1, 0);
    c->LineTo(ClientWidth - 1, ClientHeight);
    int x1, x2, x0;
    int y1;

    if (pos.Contains(bpLeft))
    {
        x1 = 0;
        x2 = ClientWidth - area.Width() + 1;
        x0 = ClientWidth - 1;
    }
    else
    {
        x1 = ClientWidth - 1;
        x2 = area.Width() - 2;
        x0 = 0;
    }
    if (pos.Contains(bpTop))
    {
        c->MoveTo(0, 0);
        c->LineTo(ClientWidth, 0);
        y1 = ClientHeight - 1;
    }
    else
    {
        c->MoveTo(0, ClientHeight - 1);
        c->LineTo(ClientWidth, ClientHeight - 1);
        y1 = 0;
    }
    c->MoveTo(x1, y1);
    c->LineTo(x2, y1);

    c->Pen->Color = clWhite;
    c->LineTo(x0, y1);

    c->Brush->Color = clWhite;
    c->FillRect(Rect(1, 1, ClientWidth - 1, 3));
    c->FillRect(Rect(1, ClientHeight - 3, ClientWidth - 1, ClientHeight - 1));
    c->FillRect(Rect(1, 3, 3, ClientHeight - 1));
    c->FillRect(Rect(ClientWidth - 3, 3, ClientWidth - 1, ClientHeight - 1));
    for (int i = 0; i < list->Count; i++)
    {
        TWord *w = list->Words[i];
        str = (UnicodeString)w->kanji + L"[" + w->kana + L"]";
        r = Rect(3, 3 + i * item_height, ClientWidth - 3, 3 + (i + 1) * item_height);
        c->Brush->Color = item_row == i ? (TColor)0x00c0c0c0 : clWhite;
        ExtTextOutW(c->Handle, r.left, r.top + r.Height()*0.8, ETO_CLIPPED | ETO_OPAQUE, &r, str.c_str(), str.Length(), NULL);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWordBox::FormMouseLeave(TObject *Sender)
{
    lobj->BoxMouseLeave();
}
//---------------------------------------------------------------------------
void __fastcall TfWordBox::FormMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    int oldrow = item_row;
    if (x < 3 || y < 3 || x > ClientWidth - 4 || y > ClientHeight - 4)
        item_row = -1;
    else
        item_row = (y - 3) / item_height;

    TRect r;
    if (oldrow != item_row)
    {
        if (oldrow >= 0)
        {
            r = Rect(3, 3 + oldrow * item_height, ClientWidth - 3, 3 + (oldrow + 1) * item_height);
            InvalidateRect(Handle, &r, false);
        }
        if (item_row >= 0)
        {
            r = Rect(3, 3 + item_row * item_height, ClientWidth - 3, 3 + (item_row + 1) * item_height);
            InvalidateRect(Handle, &r, false);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWordBox::WMMouseActivate(TWMMouseActivate &msg)
{
    msg.Result = MA_NOACTIVATE;
}
//---------------------------------------------------------------------------
void __fastcall TfWordBox::FormMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button == mbLeft && item_row >= 0)
        lobj->BoxMouseSelect(item_row);
}
//---------------------------------------------------------------------------
int TfWordBox::get_count()
{
    return list->Count;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(189);
#endif

