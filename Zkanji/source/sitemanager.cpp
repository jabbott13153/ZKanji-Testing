//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "sitemanager.h"
#include "mouseevent.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(146);
#endif

TfSites *fSites;
TSiteList *sitelist;
TSiteList *tmpsitelist;

//---------------------------------------------------------------------------
TSimpleWordList::TSimpleWordList(TZPaintBox *abox, int arowheight, bool aheader, TSimpleListHeaderType ahtype, TSimpleListGetHeaderText ahtext, TSimpleListDrawHeader ahdraw, TSimpleListCellType actype, TSimpleListGetCellText actext, TSimpleListDrawCell acdraw, TFuncPtr afunc) : TWordListBase(abox, arowheight, aheader, afunc), fhtype(ahtype), fhtext(ahtext), fhdraw(ahdraw), fctype(actype), fctext(actext), fcdraw(acdraw)
{
}
//---------------------------------------------------------------------------
TSimpleWordList::TSimpleWordList(TZPaintBox *abox, int arowheight, bool aheader, TSimpleListGetHeaderText ahtext, TSimpleListGetCellText actext, TFuncPtr afunc) : TWordListBase(abox, arowheight, aheader, afunc), fhtype(NULL), fhtext(ahtext), fhdraw(NULL), fctype(NULL), fctext(actext), fcdraw(NULL)
{
}
//---------------------------------------------------------------------------
void TSimpleWordList::DoDrawHead(int col, TRect r)
{
    bool od = false; //ownerdraw
    if (fhtype != NULL)
        fhtype(col, od);
    if (!od)
    {
        UnicodeString str = "";
        if (fhtext)
            str = fhtext(col);
        DrawHeadGen(col, r, str);
    }
    else if (fhdraw)
        fhdraw(col, Canvas, r);
}
//---------------------------------------------------------------------------
void TSimpleWordList::DoDrawCell(int col, int row, TRect r)
{
    bool od = false; //ownerdraw
    if (fctype != NULL)
        fctype(col, row, od);
    if (!od)
    {
        UnicodeString str = "";
        if (fctext)
            str = fctext(col, row);

        ExtTextOutW(Canvas->Handle, r.left + 3, r.Top + rowsize*0.76, ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);

    }
    else if (fcdraw)
        fcdraw(col, row, Canvas, r);
}
//---------------------------------------------------------------------------
void TSimpleWordList::DoPrepareDraw(int col, int row)
{
    inherited::DoPrepareDraw(col, row);
    Canvas->Font->Size--;
}
//---------------------------------------------------------------------------
void TSimpleWordList::update_count(int &count)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TSiteList::CopyElem(TSite *dest, TSite *src)
{
    dest->name = src->name;
    dest->address = src->address;
    dest->pos = src->pos;
}
//---------------------------------------------------------------------------
void TSiteList::Update(int index, UnicodeString name, UnicodeString address, int pos)
{
    if (index < 0 || index >= Count)
        THROW(L"Invalid index when accessing sites.");
    TSite *e = Items[index];
    e->name = name;
    e->address = address;
    e->pos = pos;
}
//---------------------------------------------------------------------------
int TSiteList::Add(UnicodeString name, UnicodeString address, int pos)
{
    TSite *e = new TSite;
    e->name = name;
    e->address = address;
    e->pos = pos;
    return inherited::Add(e);
}
//---------------------------------------------------------------------------
int TSiteList::IndexOf(UnicodeString name)
{
    for (int ix = 0; ix < Count; ++ix)
        if (Items[ix]->name == name)
            return ix;
    return -1;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TfSites::TfSites(TComponent* Owner) : TBaseForm(Owner)
{
    EraseBackground = false;
    tmplist = new TSiteList;
    tmplist->Assign(*tmpsitelist);
    list = new TSimpleWordList(pbList, 18, true, headertype, getheadertext, drawheader, celltype, getcelltext, drawcell, lcallback);
    list->colcount = 2;
    list->defsize[0] = 120;
    list->defsize[1] = 2500;
    list->colsize[0] = 120;
    list->colsize[1] = 2500;
    list->count = tmplist->Count;
    list->colresize[1] = false;
    list->allownopos = true;
    list->selecttype = wlsSingleSelect;
    Constrain(Width - ClientWidth + lbCursor->Left * 2 + lbCursor->Width, Height - ClientHeight + pbList->Top + (ClientHeight - btnNew->Top) + btnUp->Height * 3 + (btnUp->Top - pbList->Top) * 2);
}
//---------------------------------------------------------------------------
__fastcall TfSites::~TfSites()
{
    delete list;
    tmplist->Clear();
    delete tmplist;
}
//---------------------------------------------------------------------------
void __fastcall TfSites::FormResize(TObject *Sender)
{
    TRect r = Rect(0, pbList->Top, ClientWidth, ClientHeight);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE);
}
//---------------------------------------------------------------------------
void __fastcall TfSites::edNameChange(TObject *Sender)
{
    btnAdd->Enabled = edName->Text.Trim() != "" && edAddress->Text.Trim() != "";
}
//---------------------------------------------------------------------------
void __fastcall TfSites::btnAddClick(TObject *Sender)
{
    int ix = list->pos;
    int iy = tmplist->IndexOf(edName->Text.Trim());
    if (iy >= 0 && iy != ix)
    {
        MsgBox(L"Please specify a different name which is not in the list already.", L"Message", MB_OK);
        return;
    }

    if (ix < 0)
    {
        tmplist->Add(edName->Text.Trim(), edAddress->Text.Trim(), edAddress->SelStart);
        list->count++;
        edName->Text = "";
        edAddress->Text = L"http://";
        edAddress->SelStart = 7;
        list->pos = -1;
    }
    else
    {
        tmplist->Update(ix, edName->Text.Trim(), edAddress->Text.Trim(), edAddress->SelStart);
        list->invalidaterow(list->pos);
    }
    edName->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfSites::btnOkClick(TObject *Sender)
{
    tmpsitelist->Assign(*tmplist);
}
//---------------------------------------------------------------------------
void __fastcall TfSites::btnRemoveClick(TObject *Sender)
{
    if (list->pos < 0)
        return;
    edName->Text = "";
    edAddress->Text = L"http://";
    edAddress->SelStart = 7;
    int ix = list->pos;
    list->pos = -1;
    tmplist->Delete(ix);
    list->count--;
    edName->SetFocus();
}
//---------------------------------------------------------------------------
void TfSites::headertype(unsigned int col, bool &ownerdraw)
{
}
//---------------------------------------------------------------------------
void TfSites::drawheader(unsigned int col, TCanvas *c, TRect r)
{
}
//---------------------------------------------------------------------------
UnicodeString TfSites::getheadertext(unsigned int col)
{
    switch (col)
    {
    case 0:
        return L"Name";
    case 1:
        return L"Web address";
    }
    return "";
}
//---------------------------------------------------------------------------
void TfSites::celltype(unsigned int col, unsigned int row, bool &ownerdraw)
{
    ownerdraw = col == 1;
}
//---------------------------------------------------------------------------
void TfSites::drawcell(unsigned int col, unsigned int row, TCanvas *c, TRect r)
{
    UnicodeString str = tmplist->Items[row]->address;
    int p = tmplist->Items[row]->pos;
    UnicodeString tmp = L"•";
    str = str.SubString(1, p) + tmp + str.SubString(p + 1, str.Length() - p + 1);
    ExtTextOutW(c->Handle, r.left + 3, r.Top + list->rowsize*0.76, ETO_OPAQUE | ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);
}
//---------------------------------------------------------------------------
UnicodeString TfSites::getcelltext(unsigned int col, unsigned int row)
{
    return tmplist->Items[row]->name;
}
//---------------------------------------------------------------------------
void TfSites::lcallback()
{
    btnRemove->Enabled = list->pos >= 0;
    btnTest->Enabled = btnRemove->Enabled;
    btnDown->Enabled = list->pos >= 0 && list->pos < list->count - 1;
    btnUp->Enabled = list->pos >= 1;

    if (list->pos >= 0)
    {
        edName->Text = tmplist->Items[list->pos]->name;
        edAddress->Text = tmplist->Items[list->pos]->address;
        edAddress->SelStart = tmplist->Items[list->pos]->pos;
        edAddress->SelLength = 0;
    }
    else
    {
        edName->Text = "";
        edAddress->Text = L"http://";
        edAddress->SelStart = 7;
    }

    btnAdd->Caption = list->pos < 0 ? L"Add" : L"Modify";

}
//---------------------------------------------------------------------------
void __fastcall TfSites::btnUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    MouseRepeat(btnUpMouseDown, this, Button, Shift, X, Y, NULL);

    if (list->pos <= 0)
        return;
    tmplist->Exchange(list->pos, list->pos - 1);
    list->pos--;
}
//---------------------------------------------------------------------------
void __fastcall TfSites::btnDownMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    MouseRepeat(btnDownMouseDown, this, Button, Shift, X, Y, NULL);

    if (list->pos < 0 || list->pos >= list->count - 1)
        return;
    tmplist->Exchange(list->pos, list->pos + 1);
    list->pos++;
}
//---------------------------------------------------------------------------
void __fastcall TfSites::FormPaint(TObject *Sender)
{
    //top
    DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, Rect(0, 0, ClientWidth, 20));

    DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, Rect(0, b1->Top + 2, ClientWidth, b1->Top + 22));
    DrawGrad(Canvas, clBtnFace, COLORINC(clBtnFace, 0x101010), Rect(0, b1->Top - 20, ClientWidth, b1->Top));
    DrawGrad(Canvas, clBtnFace, COLORINC(clBtnFace, 0x101010), Rect(0, b2->Top - 20, ClientWidth, b2->Top));

    //bottom buttons
    DrawCloseBar(this, b3, NULL);

    Canvas->Brush->Color = clBtnFace;
    Canvas->FillRect(Rect(0, 20, ClientWidth, b1->Top - 20));
    Canvas->FillRect(Rect(0, b1->Top + 22, pbList->Left - 1, b2->Top - 20));
    Canvas->FillRect(Rect(pbList->Left + pbList->Width + 1, b1->Top + 22, ClientWidth, b2->Top - 20));
    Canvas->FillRect(Rect(pbList->Left, b1->Top + 22, pbList->Left + pbList->Width + 1, pbList->Top));
    Canvas->FillRect(Rect(pbList->Left, pbList->Top + pbList->Height, pbList->Left + pbList->Width + 1, b2->Top - 20));

    Canvas->Pen->Color = clBtnShadow;
    Canvas->MoveTo(pbList->Left - 1, pbList->Top);
    Canvas->LineTo(pbList->Left - 1, pbList->Top + pbList->Height);
    Canvas->LineTo(pbList->Left + pbList->Width, pbList->Top + pbList->Height);
    Canvas->LineTo(pbList->Left + pbList->Width, pbList->Top - 1);
}
//---------------------------------------------------------------------------
void __fastcall TfSites::btnNewClick(TObject *Sender)
{
    list->pos = -1;
    edName->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfSites::btnTestClick(TObject *Sender)
{
    LaunchSiteSearch(L"テスト", list->pos, tmplist);
}
//---------------------------------------------------------------------------
void LaunchSiteSearch(UnicodeString str, int siteindex, TSiteList *list)
{
    if (list == NULL)
        list = sitelist;

    int mullen = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.Length(), NULL, 0, NULL, NULL);

    if (mullen <= 0)
        return;

    int conv;
    char *mul = new char[mullen + 3];
    try
    {
        conv = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.Length(), mul, mullen + 3, NULL, NULL);
        if (conv != mullen)
            THROW(L"Error while converting text!");

        UnicodeString ad = "";
        for (int ix = 0; ix < conv; ++ix)
            ad += (UnicodeString)"%" + IntToHex((byte)(mul[ix]), 2);

        UnicodeString url = list->Items[siteindex]->address.Trim();
        int p = list->Items[siteindex]->pos;
        url = url.SubString(1, p) + ad + url.SubString(p + 1, url.Length() - p + 1);

        UnicodeString urlcmp = url.SubString(1, 8).LowerCase();
        if (urlcmp.Pos(L"http://") == 0 && urlcmp.Pos(L"https://") == 0)
            url = L"http://" + url;
        ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
    catch (...)
    {
        ;
    }
    delete[] mul;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(147);
#endif

