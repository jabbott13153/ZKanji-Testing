//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "closebutton.h"
#include "module.h"
#include "baseform.h"
#include "alphaimagelist.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(16);
#endif

TSysButtonBase::TSysButtonForms *TSysButtonBase::forms = NULL;

int TCloseButton::usagecnt = 0;
TAlphaImageList *TCloseButton::imglist = NULL;
int TSmallCloseButton::usagecnt = 0;
TAlphaImageList *TSmallCloseButton::imglist = NULL;
int TMidCloseButton::usagecnt = 0;
TAlphaImageList *TMidCloseButton::imglist = NULL;
int TPinButton::usagecnt = 0;
TAlphaImageList *TPinButton::imglist = NULL;
int TMinButton::usagecnt = 0;
TAlphaImageList *TMinButton::imglist = NULL;
int TFloatButton::usagecnt = 0;
TAlphaImageList *TFloatButton::imglist = NULL;

void TSysButtonBase::TSysButtonForms::AddButton(TBaseForm *form, TSysButtonBase *btn)
{
    TSysButtonForm *f = NULL;
    for (int ix = 0; ix < Count && !f; ++ix)
        if (Items[ix]->form == form)
            f = Items[ix];

    if (!f)
    {
        f = new TSysButtonForm(form);
        Add(f);
    }
    TSysButtonBase **tmp = f->buttons;
    f->buttons = new TSysButtonBase *[f->buttoncnt + 1];
    if (f->buttoncnt)
    {
        memcpy(f->buttons, tmp, sizeof(TSysButtonBase*)*f->buttoncnt);
        delete[] tmp;
    }
    f->buttons[f->buttoncnt] = btn;
    f->buttoncnt++;
}
//---------------------------------------------------------------------------
void TSysButtonBase::TSysButtonForms::RemoveButton(TBaseForm *form, TSysButtonBase *btn)
{
    TSysButtonForm *f = NULL;
    int fix = 0;
    for (; fix < Count && !f; ++fix)
        if (Items[fix]->form == form)
            f = Items[fix];

    if (!f)
        THROW(L"Form not found in button list!");

    TSysButtonBase **tmp = f->buttons;
    for (int ix = 0; ix < f->buttoncnt; ++ix)
    {
        if (f->buttons[ix] == btn)
        {
            if (f->buttoncnt > 1)
            {
                f->buttons = new TSysButtonBase *[f->buttoncnt - 1];
                if (ix)
                    memcpy(f->buttons, tmp, sizeof(TSysButtonBase*)*ix);
                if (ix < f->buttoncnt - 1)
                    memcpy(f->buttons + ix, tmp + ix + 1, sizeof(TSysButtonBase*)*(f->buttoncnt - ix - 1));
            }
            f->buttoncnt--;
            delete[] tmp;
            if (!f->buttoncnt)
                Delete(fix - 1);

            return;
        }
    }
    THROW(L"Button not found on form!");
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TSysButtonBase::TSysButtonForm::TSysButtonForm(TBaseForm *aform) : buttons(NULL), buttoncnt(0), form(aform)
{
    fonPaint = form->OnPaint;
    fonMouseDown = form->OnMouseDown;
    fonMouseUp = form->OnMouseUp;
    fonMouseMove = form->OnMouseMove;
    fonMouseLeave = form->OnMouseLeave;
    form->OnPaint = &formPaint;
    form->OnMouseDown = &formMouseDown;
    form->OnMouseUp = &formMouseUp;
    form->OnMouseMove = &formMouseMove;
    form->OnMouseLeave = &formMouseLeave;
    form->AddEvent(form, formDeactivate, eltDeactivate);
    AppActivateEvent(this, appDeactivate, false, true);
    SettingsChangeEvent(this, settingsChanged, true);

    form->SetWindowProc(this, wndprc);
}
//---------------------------------------------------------------------------
TSysButtonBase::TSysButtonForm::~TSysButtonForm()
{
    form->RestoreWindowProc(this, wndprc);

    SettingsChangeEvent(this, settingsChanged, false);
    AppActivateEvent(this, appDeactivate, false, false);
    form->RemoveEvent(form, formDeactivate, eltDeactivate);

    form->OnPaint = fonPaint;
    form->OnMouseDown = fonMouseDown;
    form->OnMouseUp = fonMouseUp;
    form->OnMouseMove = fonMouseMove;
    form->OnMouseLeave = fonMouseLeave;
}
//---------------------------------------------------------------------------
void __fastcall TSysButtonBase::TSysButtonForm::wndprc(TMessage &msg)
{
    if (msg.Msg == WM_SIZING)
    {
        TRect oldrect = form->ClientRect;

        TPoint p1 = form->ClientToScreen(Point(0, 0));
        TPoint p2 = form->ClientToScreen(Point(form->Width, form->Height));
        int ld = p1.x - form->Left;
        int td = p1.y - form->Top;
        int rd = form->Left + form->Width - p2.x;
        int bd = form->Top + form->Height - p2.y;

        form->CallWindowProc(this, wndprc, msg);

        TWMSizing *mm = (TWMSizing*)(&msg);
        TRect newrect = Rect(mm->lprc->Left + ld, mm->lprc->Top + td, mm->lprc->Right - rd, mm->lprc->Bottom - bd);

        for (int ix = 0; ix < buttoncnt; ++ix)
        {
            if (!buttons[ix]->fvisible)
                continue;
            TRect r = buttons[ix]->buttonrect(oldrect);
            TRect r2 = buttons[ix]->buttonrect(newrect);

            if (r != r2)
            {
                InvalidateRect(form->Handle, &r, false);
                InvalidateRect(form->Handle, &r2, false);
            }
        }
    }
    else if (msg.Msg == WM_NCHITTEST)
    {
        TPoint p;
        p.x = GET_X_LPARAM(msg.LParam);
        p.y = GET_Y_LPARAM(msg.LParam);
        p = form->ScreenToClient(p);

        msg.Result = HTNOWHERE;
        for (int ix = 0; ix < buttoncnt && msg.Result != HTCLIENT; ++ix)
        {
            if (!buttons[ix]->fvisible)
                continue;
            TRect r = buttons[ix]->buttonrect(form->ClientRect);
            if (PtInRect(&r, p))
                msg.Result = HTCLIENT;
        }
        if (msg.Result != HTCLIENT)
            form->CallWindowProc(this, wndprc, msg);
    }
    else
        form->CallWindowProc(this, wndprc, msg);
}
//---------------------------------------------------------------------------
void TSysButtonBase::TSysButtonForm::appDeactivate(void *caller)
{
    StopHint();
    for (int ix = 0; ix < buttoncnt; ++ix)
    {
        if (!buttons[ix]->fvisible)
            continue;

        buttons[ix]->pressed = false;
        buttons[ix]->state = buttons[ix]->down ? sbsDown : sbsNormal;
        buttons[ix]->Redraw();
    }
}
//---------------------------------------------------------------------------
void TSysButtonBase::TSysButtonForm::formDeactivate(void *caller)
{
    for (int ix = 0; ix < buttoncnt; ++ix)
    {
        if (!buttons[ix]->fvisible)
            continue;

        appDeactivate(caller);
        break;
    }
}
//---------------------------------------------------------------------------
void TSysButtonBase::TSysButtonForm::settingsChanged(void *caller, TSettingsProcState state)
{
    if (state != spsEnd)
        return;

    for (int ix = 0; ix < buttoncnt; ++ix)
        buttons[ix]->Redraw();
}
//---------------------------------------------------------------------------
void __fastcall TSysButtonBase::TSysButtonForm::formPaint(TObject *Sender)
{
    TRect r;

    bool newrgn = false;
    HRGN old = CreateRectRgn(0, 0, 0, 0);
    if (GetClipRgn(form->Canvas->Handle, old) == 0)
    {
        DeleteObject(old);
        newrgn = true;
    }

    for (int ix = 0; ix < buttoncnt; ++ix)
    {
        if (buttons[ix]->fvisible)
        {
            r = buttons[ix]->buttonrect(form->ClientRect);
            form->Canvas->Draw(r.Left, r.Top, buttons[ix]->btn);
        }

        if (fonPaint && buttons[ix]->fvisible)
            ExcludeClipRect(form->Canvas->Handle, r.Left, r.Top, r.Right, r.Bottom);
    }

    fonPaint(Sender);

    if (!newrgn)
    {
        SelectClipRgn(form->Canvas->Handle, old);
        +DeleteObject(old);
    }
    else
        SelectClipRgn(form->Canvas->Handle, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TSysButtonBase::TSysButtonForm::formMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    StopHint();
    for (int ix = 0; ix < buttoncnt; ++ix)
    {
        if (!buttons[ix]->fvisible)
            continue;
        if (Button == mbLeft && buttons[ix]->hover)
        {
            buttons[ix]->state = sbsPressed;
            buttons[ix]->pressed = true;
            buttons[ix]->Redraw();
            return;
        }
    }
    if (fonMouseDown)
        fonMouseDown(Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TSysButtonBase::TSysButtonForm::formMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    for (int ix = 0; ix < buttoncnt; ++ix)
    {
        if (!buttons[ix]->fvisible)
            continue;
        if (Button == mbLeft && buttons[ix]->pressed)
        {
            buttons[ix]->pressed = false;
            if (buttons[ix]->hover)
            {
                if (buttons[ix]->images->count > 3)
                {
                    buttons[ix]->down = !buttons[ix]->down;
                    buttons[ix]->Redraw();
                }
                buttons[ix]->fproc();
            }
            else
                buttons[ix]->Redraw();
            return;
        }
    }
    if (fonMouseUp)
        fonMouseUp(Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TSysButtonBase::TSysButtonForm::formMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    int hovered = -1;

    for (int ix = 0; ix < buttoncnt; ++ix)
    {
        if (!buttons[ix]->fvisible)
            continue;
        TSysButtonState old = buttons[ix]->state;
        TRect r = buttons[ix]->buttonrect(form->ClientRect);
        buttons[ix]->hover = PtInRect(&r, Point(x, y));
        if (buttons[ix]->hover)
            buttons[ix]->state = buttons[ix]->pressed ? sbsPressed : buttons[ix]->down ? sbsDown : sbsHover;
        else
            buttons[ix]->state = buttons[ix]->down ? sbsDown : sbsNormal;
        if (old != buttons[ix]->state)
            buttons[ix]->Redraw();
        if (buttons[ix]->state == sbsHover)
            hovered = ix;

        if (buttons[ix]->pressed)
            return;
    }
    if (hovered >= 0)
        StartHint(buttons[hovered], buttons[hovered]->hint, true);
    else
        StopHint();

    if (fonMouseMove)
        fonMouseMove(Sender, Shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TSysButtonBase::TSysButtonForm::formMouseLeave(TObject *Sender)
{
    StopHint();
    for (int ix = 0; ix < buttoncnt; ++ix)
    {
        if (!buttons[ix]->fvisible)
            continue;
        if (buttons[ix]->state != (buttons[ix]->down ? sbsDown : sbsNormal))
        {
            buttons[ix]->state = buttons[ix]->down ? sbsDown : sbsNormal;
            buttons[ix]->Redraw();
        }
    }
    if (fonMouseLeave)
        fonMouseLeave(Sender);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TSysButtonBase::TSysButtonBase(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, const wchar_t *ahint, TDrawFuncPtr bgdraw_func, TFuncPtr aproc, int &acounter, TAlphaImageList* &aimages, TImageList *normalimages, TImageList *alphaimages) :
        counter(acounter), images(aimages), owner(f), fpos(apos), fposx(aposx), fposy(aposy), fbgdraw(bgdraw_func), fproc(aproc), state(sbsNormal), pressed(false), hover(false), down(false), fvisible(true), hint(ahint)
{
    if (!forms)
        forms = new TSysButtonForms();

    if (!counter)
        images = new TAlphaImageList(normalimages, alphaimages);
    counter++;

    btn = new TBmp;
    btn->SetSize(images->width, images->height);

    forms->AddButton(f, this);

    Redraw();
}
//---------------------------------------------------------------------------
TSysButtonBase::~TSysButtonBase()
{
    forms->RemoveButton(owner, this);
    if (!forms->Count)
    {
        delete forms;
        forms = NULL;
    }

    delete btn;
    counter--;
    if (!counter)
    {
        delete images;
        images = NULL;
    }

}
//---------------------------------------------------------------------------
void TSysButtonBase::Redraw()
{
    if (!fvisible)
        return;

    TRect r = buttonrect(owner->ClientRect);
    btn->Canvas->Brush->Color = clBtnFace;
    btn->Canvas->Brush->Style = bsSolid;
    btn->Canvas->Pen->Color = clBlack;
    btn->Canvas->Pen->Style = psSolid;
    btn->Canvas->Pen->Mode = pmCopy;
    btn->Canvas->Pen->Width = 1;

    if (fbgdraw)
        fbgdraw(btn->Canvas, r);
    else
        btn->Canvas->FillRect(Rect(0, 0, btn->Width, btn->Height));

    images->draw(btn->Canvas, 0, 0, (int)state);

    if (owner->Visible)
        InvalidateRect(owner->Handle, &r, true);
}
//---------------------------------------------------------------------------
TRect TSysButtonBase::get_clientrect()
{
    return buttonrect(owner->ClientRect);
}
//---------------------------------------------------------------------------
TRect TSysButtonBase::buttonrect(const TRect &formrect)
{
    switch (fpos)
    {
    case sbpTopLeft:
        return Rect(formrect.left + fposx, formrect.top + fposy, formrect.left + fposx + images->width, formrect.top + fposy + images->height);
    case sbpTopRight:
        return Rect(formrect.right - fposx - images->width, formrect.top + fposy, formrect.right - fposx, formrect.top + fposy + images->height);
    case sbpBottomLeft:
        return Rect(formrect.left + fposx, formrect.bottom - fposy - images->height, formrect.left + fposx + images->width, formrect.bottom - fposy);
    case sbpBottomRight:
        return Rect(formrect.right - fposx - images->width, formrect.bottom - fposy - images->height, formrect.right - fposx, formrect.bottom - fposy);
    default:
        return Rect(0, 0, 0, 0);
    }
}
//---------------------------------------------------------------------------
void TSysButtonBase::set_visible(bool val)
{
    if (fvisible == val)
        return;
    fvisible = val;
    pressed = false;
    state = down ? sbsDown : sbsNormal;
    Redraw();
}
//---------------------------------------------------------------------------
void TSysButtonBase::set_down(bool val)
{
    if (down == val)
        return;
    down = val;
    pressed = false;
    state = down ? sbsDown : sbsNormal;
    Redraw();
}
//---------------------------------------------------------------------------
int TSysButtonBase::get_height()
{
    if (images)
        return images->height;
    return 0;
}
//---------------------------------------------------------------------------
int TSysButtonBase::get_width()
{
    if (images)
        return images->width;
    return 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TCloseButton::TCloseButton(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, TDrawFuncPtr bgdraw_func, TFuncPtr aproc) :
        inherited(f, apos, aposx, aposy, L"Close", bgdraw_func, aproc ? aproc : closeproc, usagecnt, imglist, data->imgCloseButton, data->imgCloseButtonAlpha)
{
    ;
}
//---------------------------------------------------------------------------
TCloseButton::~TCloseButton()
{
    ;
}
//---------------------------------------------------------------------------
void TCloseButton::closeproc()
{
    Owner->Close();
}
//---------------------------------------------------------------------------
TMidCloseButton::TMidCloseButton(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, TDrawFuncPtr bgdraw_func, TFuncPtr aproc) :
        inherited(f, apos, aposx, aposy, L"Close", bgdraw_func, aproc ? aproc : closeproc, usagecnt, imglist, data->imgMidCloseButton, data->imgMidCloseButtonAlpha)
{
    ;
}
//---------------------------------------------------------------------------
TMidCloseButton::~TMidCloseButton()
{
    ;
}
//---------------------------------------------------------------------------
void TMidCloseButton::closeproc()
{
    Owner->Close();
}
//---------------------------------------------------------------------------
TSmallCloseButton::TSmallCloseButton(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, TDrawFuncPtr bgdraw_func, TFuncPtr aproc) :
        inherited(f, apos, aposx, aposy, L"Close", bgdraw_func, aproc ? aproc : closeproc, usagecnt, imglist, data->imgSmallCloseButton, data->imgSmallCloseButtonAlpha)
{
    ;
}
//---------------------------------------------------------------------------
TSmallCloseButton::~TSmallCloseButton()
{
    ;
}
//---------------------------------------------------------------------------
void TSmallCloseButton::closeproc()
{
    Owner->Close();
}
//---------------------------------------------------------------------------
TPinButton::TPinButton(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, TDrawFuncPtr bgdraw_func, TFuncPtr aproc) :
        inherited(f, apos, aposx, aposy, L"Pin to desktop", bgdraw_func, aproc, usagecnt, imglist, data->imgPinButton, data->imgPinButtonAlpha)
{
    ;
}
//---------------------------------------------------------------------------
TPinButton::~TPinButton()
{
    ;
}
//---------------------------------------------------------------------------
TMinButton::TMinButton(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, TDrawFuncPtr bgdraw_func, TFuncPtr aproc) :
        inherited(f, apos, aposx, aposy, L"Unfloat", bgdraw_func, aproc, usagecnt, imglist, data->imgMinButton, data->imgMinButtonAlpha)
{
    ;
}
//---------------------------------------------------------------------------
TMinButton::~TMinButton()
{
    ;
}
//---------------------------------------------------------------------------
TFloatButton::TFloatButton(TBaseForm *f, TSysButtonPosition apos, int aposx, int aposy, TDrawFuncPtr bgdraw_func, TFuncPtr aproc) :
        inherited(f, apos, aposx, aposy, L"Float", bgdraw_func, aproc, usagecnt, imglist, data->imgFloatButton, data->imgFloatButtonAlpha)
{
    ;
}
//---------------------------------------------------------------------------
TFloatButton::~TFloatButton()
{
    ;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(17);
#endif

