//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "dropdown.h"
#include "settings.h"
#include "kanjiedit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(30);
#endif

#define DROPDOWNLIST_VERSION    1

TfDropDown *fDropDown;
//---------------------------------------------------------------------------

__fastcall TfDropDown::TfDropDown(TComponent* Owner) : TBaseForm(Owner)
{
    laston = -1;
    scaled = false;
    AppActivateEvent(this, appdeactivate, false, true);
}
//---------------------------------------------------------------------------
__fastcall TfDropDown::~TfDropDown()
{
    AppActivateEvent(this, appdeactivate, false, false);
}
//---------------------------------------------------------------------------
void __fastcall TfDropDown::CreateParams(TCreateParams &Params)
{
    inherited::CreateParams(Params);

    Params.Style = WS_POPUP;

    //Params.WindowClass.style = Params.WindowClass.style | CS_SAVEBITS;
    if (NewStyleControls)
        Params.ExStyle = WS_EX_TOOLWINDOW;

    Params.ExStyle |= WS_EX_TOPMOST | WS_EX_LAYERED;

    if (MatchVersion(5, 1))
        Params.WindowClass.style = Params.WindowClass.style | CS_DROPSHADOW;

    AddBiDiModeExStyle(Params.ExStyle);
}
//---------------------------------------------------------------------------
void TfDropDown::appdeactivate(void *caller)
{
    if (Visible || GetCapture() == Handle)
        hide_me();
}
//---------------------------------------------------------------------------
void __fastcall TfDropDown::WndP(TMessage &msg)
{
    if (msg.Msg == WM_NCACTIVATE)
        msg.WParam = 1;

    fform->CallWindowProc(this, WndP, msg);
    //oldproc(msg);
}
//---------------------------------------------------------------------------
void TfDropDown::Popup(TRect r, TDropDownList *alist, THistFunc aevent, TBaseForm *aform, bool aautoupdate)
{
    if (Visible)
    {
        hide_me();
        return;
    }

    if (!scaled)
    {
        scaled = true;
        lb->ItemHeight *= scaleres;
    }

    fexy = fexx = -9999;

    fautoupdate = aautoupdate;
    fonselect = aevent;

    fmouseon = -1;

    fform = (aform->Parent ? (TBaseForm*)aform->Parent : aform);

    fform->SetWindowProc(this, WndP);
    //oldproc = fform->WindowProc;
    //fform->WindowProc = WndP;

    flist = alist;
    lb->Count = flist->Count;
    Width = r.Width();
    Height = MIN(12, lb->Items->Count) * lb->ItemHeight + GetSystemMetrics(SM_CYBORDER) * 2;
    Left = r.left;

    tagMONITORINFO inf = MonitorInfo(r.Left, r.Top);

    if (r.Bottom + Height >= inf.rcMonitor.bottom)
        Top = r.top - Height - 1;
    else
        Top = r.bottom + 1;
    lb->Font->Name = settings.kanafont;
    lb->Font->Charset = settings.kanacharset;
    lb->Font->Size = double(lb->ItemHeight) * 0.7 / scaleres;
    lb->ItemIndex = -1;
    Visible = true;
    SetWindowPos(Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
}
//---------------------------------------------------------------------------
void TfDropDown::hide_me()
{
    fform->RestoreWindowProc(this, WndP);
    //fform->WindowProc = oldproc;

    TPoint p;
    if (GetCapture() == Handle)
        ReleaseCapture();
    fform->SetFocus();
    Visible = false;
}
//---------------------------------------------------------------------------
void TfDropDown::set_mouseon(int val)
{
    int om = fmouseon;
    fmouseon = val;
    if (om != fmouseon)
    {
        TRect r;
        if (om >= 0)
        {
            r = lb->ItemRect(om);
            InvalidateRect(lb->Handle, &r, false);
        }
        if (fmouseon >= 0)
        {
            r = lb->ItemRect(fmouseon);
            InvalidateRect(lb->Handle, &r, false);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDropDown::lbDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State)
{
    TCanvas *c = lb->Canvas;
    UnicodeString s;
    SetTextAlign(c->Handle, TA_BASELINE | TA_LEFT);
    if (flist->Count)
    {
        s = flist->Strings[Index];
        c->Brush->Color = (Index == mouseon ? clHighlight : clWindow);
        c->Font->Color = (Index == mouseon ? clHighlightText : clBtnText);
        ExtTextOutW(c->Handle, Rect.left + 2, Rect.top + lb->ItemHeight * 0.85, ETO_CLIPPED | ETO_OPAQUE, &Rect, s.c_str(), s.Length(), NULL);
    }
    else
    {
        c->Brush->Color = clWindow;
        c->FillRect(Rect);
    }
}
//---------------------------------------------------------------------------
void TfDropDown::StartCapture()
{
    SetCapture(Handle);
    SetWindowPos(Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
    Application->ProcessMessages();
}
//---------------------------------------------------------------------------
void __fastcall TfDropDown::FormKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == 0x1B || Key == 0x0D) // Escape or Enter key.
    {
        if (Key == 0x0D && !fautoupdate && fmouseon >= 0) // Enter key.
            fonselect(fmouseon, flist->Strings[fmouseon], flist->Data[fmouseon]);
        hide_me();
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfDropDown::lbKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_RETURN)
//    {
//        if (Key == VK_RETURN && !fautoupdate && fmouseon >= 0)
//            fonselect(fmouseon, flist->Strings[fmouseon], flist->Data[fmouseon]);
//        hide_me();
        return;
//    }

    if (Key == VK_UP || Key == VK_DOWN)
    {
        int oldmouseon = fmouseon;
        mouseon = MAX(0, MIN(fmouseon + (Key == VK_UP ? -1 : 1), lb->Count - 1));
        if (oldmouseon != fmouseon && fonselect && fautoupdate)
            fonselect(fmouseon, flist->Strings[fmouseon], flist->Data[fmouseon]);
    }

    Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfDropDown::FormDeactivate(TObject *Sender)
{
    appdeactivate(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfDropDown::lbMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    if (x == fexx && y == fexy)
        return;
    fexx = x;
    fexy = y;

    int item = ItemAt(x, y);

    if (item >= 0)
        laston = -1;
    else
    {
        laston = fmouseon;
        return;
    }

    mouseon = MAX(0, MIN(item, lb->Count - 1));
}
//---------------------------------------------------------------------------
void __fastcall TfDropDown::lbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button != mbLeft)
        return;

    if (flist->Count > 0 && fonselect && (laston >= 0 || mouseon >= 0) && wason)
    {
        hide_me();
        fonselect(laston < 0 ? mouseon : laston, flist->Strings[(laston < 0 ? mouseon : laston)], flist->Data[(laston < 0 ? mouseon : laston)]);
    }
    else if (ItemAt(x, y) >= 0)
        SetCapture(Handle);
    else
        hide_me();
}
//---------------------------------------------------------------------------
void __fastcall TfDropDown::lbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button != mbLeft)
        return;

    int item = ItemAt(x, y);
    wason = item >= 0;
    if (wason)
        mouseon = laston = MAX(0, MIN(item, lb->Count - 1));
}
//---------------------------------------------------------------------------
int TfDropDown::ItemAt(int x, int y)
{
    TPoint p = ClientToScreen(TPoint(x, y));
    p = lb->ScreenToClient(p);
#pragma warn -8084
    int item = SendMessage(lb->Handle, LB_ITEMFROMPOINT, 0, MAKELPARAM(p.x, p.y));
#pragma warn .8084
    if (HIWORD(item) != 0)
        return -1;
    item = LOWORD(item);
    if (item < 0 || item >= lb->Items->Count)
        return -1;
    return item;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TDropDownList::TDropDownList(bool unlimited, bool duplicates) : funlimited(unlimited), fduplicates(duplicates), items(NULL)
{
    items = new TStringList;
}
//---------------------------------------------------------------------------
TDropDownList::~TDropDownList()
{
    Clear();
    delete items;
}
//---------------------------------------------------------------------------
void TDropDownList::Clear()
{
    items->Clear();
}
//---------------------------------------------------------------------------
void TDropDownList::Delete(int ix)
{
    if (ix < 0 || ix >= items->Count)
        THROW(L"Deleting not existing item in drop-down list!");

    items->Delete(ix);
}
//---------------------------------------------------------------------------
UnicodeString TDropDownList::get_string(int ix)
{
    if (ix < 0 || ix >= items->Count)
        THROW(L"Invalid index in drop-down list!");
    return items->Strings[ix];
}
//---------------------------------------------------------------------------
void* TDropDownList::get_data(int ix)
{
    if (ix < 0 || ix >= items->Count)
        THROW(L"Invalid index in drop-down list!");
    return (void*)items->Objects[ix];
}
//---------------------------------------------------------------------------
int TDropDownList::get_count()
{
    return items->Count;
}
//---------------------------------------------------------------------------
void TDropDownList::Popup(TRect acrect, THistFunc aevent, TBaseForm *aform, bool aautoupdate)
{
    fDropDown->Popup(acrect, this, aevent, aform, aautoupdate);
}
//---------------------------------------------------------------------------
void TDropDownList::Add(UnicodeString s, void* data)
{
    if (s.IsEmpty())
        return;

    for (int i = 0; i < Count && !fduplicates; i++)
    {
        if (s == Strings[i])
        {
            if (i == 0)
                return;

            Delete(i);
            break;
        }
    }

    if (!funlimited)
        while (items->Count >= settings.histcnt)
            items->Delete(items->Count - 1);

    items->InsertObject(0, s, (TObject*)data);
}
//---------------------------------------------------------------------------
void TDropDownList::Limit()
{
    if (funlimited)
        return;
    while (items->Count > settings.histcnt)
        items->Delete(items->Count - 1);
}
//---------------------------------------------------------------------------
void TDropDownList::FillComboBox(TComboBox *cb)
{
    cb->Items->BeginUpdate();
    cb->Items->Clear();
    for (int i = 0; i < items->Count; i++)
        cb->Items->Add(Strings[i]);
    cb->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void TDropDownList::SaveToFile(FILE *f)
{
    char verstr[9];
    sprintf(verstr, "zhddl%03i", DROPDOWNLIST_VERSION);
    fwrite(verstr, sizeof(char), 8, f);

    int c = items->Count;
    fwrite(&c, sizeof(int), 1, f);
    for (int i = 0; i < c; i++)
        write(Strings[i], f, sltWord);
}
//---------------------------------------------------------------------------
void TDropDownList::LoadFromFile(FILE *f)
{
    int pos = ftell(f);

    char tmp[9];
    tmp[8] = 0;
    fread(tmp, sizeof(char), 8, f);
    int version = 0;
    if (!strncmp("zhddl", tmp, 5))
        version = strtol(tmp + 5, 0, 10);
    else
        fseek(f, pos, SEEK_SET);

    Clear();
    word w;
    int c;
    fread(&c, sizeof(int), 1, f);
    items->Capacity = c;

    for (int i = 0; i < c; i++)
    {
        if (version > 0) // Added 2013.05.27 (and will be deleted immediately after the next release! - remove next part if you see this and the previous text makes no sense any more)
        {
            UnicodeString str;
            read(str, f, sltWord);
            items->Add(str);
        }
        else
        {
            fread(&w, sizeof(word), 1, f);
            wchar_t *tmp = new wchar_t[w];
            fread(tmp, sizeof(wchar_t), w, f);
            UnicodeString str(tmp, w);
            delete[] tmp;
            items->Add(str);
        }
    }
}
//---------------------------------------------------------------------------
bool TDropDownList::get_visible()
{
    return fDropDown->Visible;
}
//---------------------------------------------------------------------------
TBaseForm* TDropDownList::get_form()
{
    return fDropDown->fform;
}
//---------------------------------------------------------------------------
void TDropDownList::Capture()
{
    fDropDown->StartCapture();
}
//---------------------------------------------------------------------------
void* TDropDownList::get_handle()
{
    return fDropDown->Handle;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(31);
#endif


