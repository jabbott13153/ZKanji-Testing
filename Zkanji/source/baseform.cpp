//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "baseform.h"
#include "settings.h"
#include "module.h"
#include "modallist.h"
#include "shadow.h"
#include "main.h"
#include "kanjihint.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma link "ZPaintBox"

//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester1(6);
#endif

extern TBaseForm *fKanji;
extern TBaseForm *fGroup;
extern TBaseForm *fDict;
extern TBaseForm *fDock;

extern bool ThemesAreTurnedOn;

TTemplateList<TBaseForm, false> *activelist = NULL;

TBaseForm *TBaseForm::CurrentMainForm = NULL;

int TBaseForm::hookcnt = 0;
HHOOK TBaseForm::hook = NULL;

TFormSizeInfo *fsi;
//---------------------------------------------------------------------------
void InvalidateEmptyArea(TWinControl *c, bool recursive)
{
    if (!c->Visible)
        return;
    if (c->InheritsFrom(__classid(TComboBox)) || c->InheritsFrom(__classid(TListBox)))
        return;

    HRGN rgn = CreateRectRgn(0, 0, c->ClientWidth, c->ClientHeight);
    if (!rgn)
        THROW(L"??");
    try
    {
        for (int i = 0; i < c->ControlCount; i++)
        {
            TControl *cc = c->Controls[i];
            bool ppanel = cc->InheritsFrom(__classid(TPanel));
            if (recursive && (ppanel && ((TPanel*)cc)->ParentBackground) || !ppanel && cc->InheritsFrom(__classid(TWinControl)))
                InvalidateEmptyArea((TWinControl*)cc);
            HRGN rgnc = CreateRectRgn(cc->Left, cc->Top, cc->Left + cc->Width, cc->Top + cc->Height);
            if (rgnc)
            {
                int res = CombineRgn(rgn, rgn, rgnc, RGN_DIFF);
                DeleteObject(rgnc);
                if (res == ERROR)
                    THROW(L"!!");
            }
        }
        InvalidateRgn(c->Handle, rgn, true);
    }
    catch (...)
    {
        ;
    }
    DeleteObject(rgn);
}
//---------------------------------------------------------------------------
void HideKanjiHintNow();
extern TBaseForm *fRecognize;
LRESULT CALLBACK __bf_cbtproc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
        return CallNextHookEx(TBaseForm::hook, nCode, wParam, lParam);

    if (nCode == HCBT_MINMAX && (HWND)wParam == fDock->Handle)
    {
        if (settings.minimizetotray)
        {
            if ((lParam & 0xffff) == SW_MINIMIZE || (lParam & 0xffff) == SW_FORCEMINIMIZE || (lParam & 0xffff) == SW_SHOWMINIMIZED)
            {
                HideKanjiHintNow();
                if (fRecognize != NULL)
                    fRecognize->Hide();
                fMain->Minimize();
                return 1;
            }
        }
        else if ((lParam & 0xffff) == SW_MINIMIZE)
        {
            HideKanjiHintNow();
            if (fRecognize != NULL)
                fRecognize->Hide();
            fMain->Minimize();
        }
        else if ((lParam & 0xffff) == SW_RESTORE && settings.intray)
        {
            data->Restore();
            return 1;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
__fastcall TBaseForm::TBaseForm(TComponent *AOwner) :
        events(NULL), frestoreold(false), fkeepsize(false), fshowing(false), fconstraint(false), ferase(true),
        fshowactivate(true), fposition(Position), positioning(false), fonerase(NULL), fappwnd(false), fnoo(false),
        shadow(NULL), fshadowed(false), wndmethods(NULL), oldwndproc(NULL), modalchildren(NULL), modalowner(NULL), TForm(AOwner)
{
    oldwndproc = WindowProc;
    for (int ix = 0; ix < ComponentCount; ++ix)
        if (dynamic_cast<TStatusBar*>(Components[ix]))
            RescaleStatusbar((TStatusBar*)Components[ix]);

    //if (Win32MajorVersion < 6)
    //Shadowed = true;

    if (!hook)
        hook = SetWindowsHookEx(WH_CBT, &__bf_cbtproc, NULL, GetCurrentThreadId());
    hookcnt++;
}
//---------------------------------------------------------------------------
__fastcall TBaseForm::~TBaseForm()
{
    hookcnt--;
    if (!hookcnt)
        UnhookWindowsHookEx(hook);

    Shadowed = false;

    if (CurrentMainForm == this)
        FindCurrentMainForm();

    /*if (PopupChildren) {
     for (int ix = 0; ix < PopupChildren->Count; ++ix) {
     TBaseForm *f = (TBaseForm*)PopupChildren->Items[ix];
     if (f->popupowner == this)
     f->popupowner = NULL;
     }
     }
     */

    delete events;
    events = NULL;

    if (wndmethods)
    {
        wndmethods->Clear();
        delete wndmethods;
        wndmethods = NULL;
        WindowProc = oldwndproc;
    }
}
//---------------------------------------------------------------------------
void TBaseForm::RescaleStatusbar(TStatusBar *bar)
{
    for (int ix = 0; ix < bar->Panels->Count; ++ix)
        bar->Panels->Items[ix]->Width *= scaleres;
    bar->Height *= scaleres;
}
//---------------------------------------------------------------------------
int TBaseForm::FindWindowProc(void *who, TWndMethod proc)
{
    if (!wndmethods)
        return -1;

    for (int ix = wndmethods->Count - 1; ix >= 0; --ix)
    {
        if (wndmethods->Items[ix]->caller == who && wndmethods->Items[ix]->method == proc)
            return ix;
    }
    return -1;
}
//---------------------------------------------------------------------------
void TBaseForm::SetWindowProc(void *who, TWndMethod proc)
{
    int ix = FindWindowProc(who, proc);
    if (ix >= 0)
        return;
    if (!wndmethods)
        wndmethods = new TWndProcList;
    TWndProcListElem *elem = new TWndProcListElem;
    elem->caller = who;
    elem->method = proc;
    wndmethods->Add(elem);
    WindowProc = proc;
}
//---------------------------------------------------------------------------
void TBaseForm::RestoreWindowProc(void *who, TWndMethod proc)
{
    int ix = FindWindowProc(who, proc);
    if (ix < 0)
        return;

    if (WindowProc == oldwndproc)
        THROW(L"Original method restored when I didn't allow it!");

    if (ix == wndmethods->Count - 1)
        WindowProc = ix == 0 ? oldwndproc : wndmethods->Items[ix - 1]->method;

    wndmethods->Delete(ix);

    if (!wndmethods->Count)
    {
        delete wndmethods;
        wndmethods = NULL;
    }
}
//---------------------------------------------------------------------------
void TBaseForm::CallWindowProc(void *who, TWndMethod proc, TMessage &msg)
{
    int ix = FindWindowProc(who, proc);
    if (ix < 0)
        return;
    if (!ix)
        oldwndproc(msg);
    else
        wndmethods->Items[ix - 1]->method(msg);
}
//---------------------------------------------------------------------------
void TBaseForm::FindCurrentMainForm()
{
    bool found = false;
    for (int ix = 0; ix < Screen->FormCount; ++ix)
    {
        TBaseForm *f;
        if (Screen->Forms[ix] == this || (f = dynamic_cast<TBaseForm *>(Screen->Forms[ix])) == NULL || !f->fappwnd || f->Parent || !f->Visible)
            continue;

        for (int ix = 0; PopupChildren && ix < PopupChildren->Count; ++ix)
        {
            TBaseForm *f2 = dynamic_cast<TBaseForm *>((TCustomForm*)PopupChildren->Items[ix]);
            if (!f2)
                continue;

            f2->ChangePopupOwner(f);
        }

        found = true;
        CurrentMainForm = f;
        break;
    }
    if (!found)
    {
        for (int ix = 0; PopupChildren && ix < PopupChildren->Count; ++ix)
        {
            TBaseForm *f = dynamic_cast<TBaseForm *>((TCustomForm*)PopupChildren->Items[ix]);
            if (f)
                f->popupowner = NULL;
        }
        CurrentMainForm = NULL;
    }
}
//---------------------------------------------------------------------------
void TBaseForm::ChangePopupOwner(TBaseForm *newowner)
{
    if (!Visible || !popupowner || popupowner != CurrentMainForm || newowner == popupowner || newowner == this)
        return;

    HWND who = Handle;
    if (popupowner)
    {
        HWND owner;
        while (who && (owner = (HWND)GetWindowLongPtr(who, GWLP_HWNDPARENT)) != popupowner->Handle)
            who = owner;

        if (!who)
            who = Handle;
    }

    SetWindowLongPtr(who, GWLP_HWNDPARENT, (long)newowner->Handle);
    if (popupowner->PopupChildren->IndexOf(this) >= 0)
    {
        popupowner->PopupChildren->Remove(this);
        if (newowner->PopupChildren->IndexOf(this) < 0)
            newowner->PopupChildren->Add(this);
    }
    RemoveFreeNotification(popupowner);
    FreeNotification(newowner);

    popupowner = newowner;
}
//---------------------------------------------------------------------------
void TBaseForm::ChangeModalOwner(TBaseForm *newowner)
{
    HWND who = Handle;
    HWND owner;
    while (who && (owner = (HWND)GetWindowLongPtr(who, GWLP_HWNDPARENT)) != modalowner->Handle)
        who = owner;
    if (!who && HandleAllocated())
        who = Handle;
    if (who)
    {
        if (newowner->HandleAllocated())
            SetWindowLongPtr(who, GWLP_HWNDPARENT, (long)newowner->Handle);
        else
            SetWindowLongPtr(who, GWLP_HWNDPARENT, (long)NULL);
    }

    if (modalowner->modalchildren->IndexOf(this) >= 0)
    {
        modalowner->modalchildren->Remove(this);
        modalowner = NULL;
    }
    if (newowner && newowner->modalchildren && newowner->modalchildren->IndexOf(this) < 0)
    {
        newowner->modalchildren->Add(this);
        modalowner = newowner;
    }
}
//---------------------------------------------------------------------------
bool TBaseForm::HasPopupOwner(TBaseForm *isowner)
{
    TBaseForm *parent = popupowner;
    while (parent && parent != isowner)
        parent = parent->popupowner;
    return parent == isowner;

}
//---------------------------------------------------------------------------
bool TBaseForm::get_app()
{
    return !Parent && fappwnd;
}
//---------------------------------------------------------------------------
void TBaseForm::set_app(bool val)
{
    if (val && fnoo)
        THROW(L"Don't set AppWindow when NoOwner is toggled!");
    if (fappwnd == val)
        return;

    fappwnd = val;
    if (popupowner)
    {
        popupowner->PopupChildren->Remove(this);
        popupowner = NULL;
    }

    for (int ix = PopupChildren->Count - 1; ix >= 0; --ix)
    {
        TBaseForm *f;
        if ((f = dynamic_cast<TBaseForm *>((TCustomForm*)PopupChildren->Items[ix])) == NULL)
            continue;
        PopupChildren->Delete(ix);
        f->Recreate(false);
    }
    Recreate(false);
}
//---------------------------------------------------------------------------
void TBaseForm::set_noo(bool val)
{
    if (val && fappwnd)
        THROW(L"Don't set NoOwner when AppWindow is toggled!");
    if (val == fnoo)
        return;
    fnoo = val;
    Recreate(false);
}
//---------------------------------------------------------------------------
void TBaseForm::ExecuteEvent(TBaseFormEventListType atype)
{
    if (!events)
        return;

    for (int ix = events->Count - 1; ix >= 0; --ix)
    {
        if (events->Items[ix]->type == atype)
            events->Items[ix]->event((TObject*)events->Items[ix]->sender);
    }
}
//---------------------------------------------------------------------------
void TBaseForm::AddEvent(void *Sender, TCPPNotifyEvent aprc, TBaseFormEventListType atype)
{
    if (!events)
        events = new TEventList;

    TEventListElem *elem = new TEventListElem;
    events->Add(elem);
    elem->sender = Sender;
    elem->type = atype;
    elem->event = aprc;
}
//---------------------------------------------------------------------------
void TBaseForm::RemoveEvent(void *Sender, TCPPNotifyEvent aprc, TBaseFormEventListType atype)
{
    if (!events)
        return;

    for (int ix = events->Count - 1; ix >= 0; --ix)
    {
        if (events->Items[ix]->sender == Sender && events->Items[ix]->event == aprc && events->Items[ix]->type == atype)
        {
            events->Delete(ix);
            break;
        }
    }
}
//---------------------------------------------------------------------------
void TBaseForm::RemoveEvents(void *Sender)
{
    if (!events)
        return;

    for (int ix = events->Count - 1; ix >= 0; --ix)
        if (events->Items[ix]->sender == Sender)
            events->Delete(ix);
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::Activate()
{
    ExecuteEvent(eltActivate);
    if (fappwnd && !Parent && CurrentMainForm != this)
    {
        for (int ix = 0; CurrentMainForm && ix < Screen->FormCount; ++ix)
        {
            TBaseForm *f = dynamic_cast<TBaseForm *>(Screen->Forms[ix]);
            if (!f || f == this)
                continue;
            f->ChangePopupOwner(this);
        }
        CurrentMainForm = this;
    }
    inherited::Activate();
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::Deactivate()
{
    ExecuteEvent(eltDeactivate);
    inherited::Deactivate();
}
//---------------------------------------------------------------------------
int __fastcall TBaseForm::ShowModal()
{
    modalchildren = new TBaseFormList;
    data->ModalBegin(this, Handle);
    if (Screen->ActiveForm && Screen->ActiveForm->Visible && Screen->ActiveForm->HandleAllocated() && Screen->ActiveForm != this && Screen->ActiveForm->Parent == NULL)
        SetWindowLongPtr(Handle, GWLP_HWNDPARENT, (LONG_PTR)Screen->ActiveForm->Handle);
    int r = TForm::ShowModal();
    data->ModalEnd(Handle);
    for (int ix = 0; ix < modalchildren->Count; ++ix)
    {
        TBaseForm *newowner = NULL;
        if (modallist && modallist->Count)
            newowner = modallist->Items[modallist->Count - 1]->form;
        if (!newowner)
            newowner = CurrentMainForm;
        modalchildren->Items[ix]->ChangeModalOwner(newowner);
    }
    delete modalchildren;
    modalchildren = NULL;
    return r;
}
//---------------------------------------------------------------------------
void TBaseForm::Show()
{
    if (!fshowactivate)
        Visible = true;
    else
        inherited::Show();
}
//---------------------------------------------------------------------------
/*HWND taW, ttW, tfW;

 bool __stdcall procfindw(HWND hwnd, LPARAM Param) {
 if (hwnd == taW || hwnd == Application->Handle || !IsWindowVisible(hwnd) || !IsWindowEnabled(hwnd))
 return true;
 bool top = (GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) == WS_EX_TOPMOST;
 if (!top) {
 tfW = hwnd;
 return false;
 }

 if (!ttW)
 ttW = hwnd;

 return true;
 }
 */
//---------------------------------------------------------------------------
extern TBaseForm *fInfo;
void TBaseForm::CMShowingChanged(TMessage &msg)
{
    bool screenactive = Screen->FocusedForm == this;

    if (!Visible)
        HideOwnedKanjiHint(this);

    if (frestoreold && !Visible)
        oldactive = ActiveControl;

    if (!Visible && CurrentMainForm == this)
        FindCurrentMainForm();

    if (modallist && modallist->Count && modallist->Items[modallist->Count - 1]->form && !FormState.Contains(fsModal) && Visible)
    {
        TBaseForm *mo = modallist->Items[modallist->Count - 1]->form;
        if (modalowner && modalowner != mo)
            modalowner->modalchildren->Remove(this);
        modalowner = mo;
        if (mo->modalchildren && mo->modalchildren->IndexOf(this) < 0)
            mo->modalchildren->Add(this);
    }

    if ((!settings.intray || Visible) && fshowactivate)
    {
        TMessage mmsg = msg;
        TForm::Dispatch(&mmsg);
        msg.Result = mmsg.Result;

        if (fontop && Visible)
        {
            SetWindowPos(Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
        }

        if (screenactive)
            Activate();

        if (frestoreold && oldactive && Visible && oldactive->Visible)
        {
            TWinControl *c = dynamic_cast<TWinControl *>(oldactive);
            if (c)
                c->SetFocus();
        }
        return;
    }

    if (fshowing)
        throw EInvalidOperation("Cannot change Visible in OnShow or OnHide");

    fshowing = true;
    try
    {
        if (!Visible)
            ExecuteEvent(eltHide);
        if (OnHide && !Visible)
            OnHide(this);
        else if (OnShow && Visible)
        {
            AutoSize();
            OnShow(this);
        }

        HWND newtop = NULL;
        if (!Visible && GetActiveWindow() == Handle && !IsIconic(Handle))
        {
            /*taW = Handle, tfW = 0, ttW = 0;

             EnumThreadWindows(GetCurrentThreadId(), (int (__stdcall *)(HWND,LPARAM))procfindw, 0);

             newtop = (tfW ? tfW : ttW);
             */
            if (modallist && modallist->Count)
            {
                if (modallist->Items[modallist->Count - 1]->Handle != Handle)
                    newtop = modallist->Items[modallist->Count - 1]->Handle;
                else if (modallist->Count > 1)
                    newtop = modallist->Items[modallist->Count - 2]->Handle;
            }

            if (!newtop && CurrentMainForm && CurrentMainForm->Visible)
                newtop = CurrentMainForm->Handle;

        }

        if (newtop && IsWindowEnabled(newtop))
            SetActiveWindow(newtop);

        if (fontop && Visible)
        {
            SetWindowPos(Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
        }
        else
        {
            if (fontop)
                SetWindowPos(Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE/*| SWP_NOOWNERZORDER*/);
            SetWindowPos(Handle, 0, 0, 0, 0, 0, (Visible ? SWP_SHOWWINDOW : SWP_HIDEWINDOW) | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
            if (!Visible && PopupChildren)
            {
                for (int ix = 0; ix < PopupChildren->Count; ++ix)
                {
                    TBaseForm *f = (TBaseForm*)PopupChildren->Items[ix];
                    f->Hide();
                }
            }
        }

    }
    catch (...)
    {
        ;
    }
    fshowing = false;
}
//---------------------------------------------------------------------------
void TBaseForm::Recreate(bool setbounds)
{
    if (!modallist)
        return;

    TRect r = Rect(Left, Top, Width, Height);

    HWND h = NULL;
    if (HandleAllocated())
        h = Handle; //save old handle to replace in modallist
    RecreateWnd();
    if (h)
        modallist->Update(h, Handle);

    if (setbounds)
    {
        SetBounds(r.Left, r.Top, r.Right, r.Bottom);
        SetWindowPos(Handle, 0, r.Left, r.Top, r.Right, r.Bottom, SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }

    for (int ix = 0; ix < ComponentCount; ++ix)
    {
        if (Components[ix]->ClassType() != __classid(TZPaintBox))
            continue;

        TZPaintBox *pb = (TZPaintBox*)Components[ix];
        if (!pb->VertScrollBar->Visible)
        {
            pb->VertScrollBar->Visible = true;
            pb->VertScrollBar->Visible = false;
        }
        if (!pb->HorzScrollBar->Visible)
        {
            pb->HorzScrollBar->Visible = true;
            pb->HorzScrollBar->Visible = false;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::CreateParams(TCreateParams &Params)
{
    inherited::CreateParams(Params);

    if (MatchVersion(5, 1))
    {
        if (fontop && Visible)
            Params.ExStyle |= WS_EX_TOPMOST;
        else
            Params.ExStyle &= ~WS_EX_TOPMOST;
    }

    if (fappwnd)
        Params.ExStyle |= WS_EX_APPWINDOW;
    else
        Params.ExStyle &= ~WS_EX_APPWINDOW;

    if (fnoo)
        Params.ExStyle |= WS_EX_TOOLWINDOW;

    TBaseForm *current = (!fappwnd || Parent) && CurrentMainForm && CurrentMainForm->Visible ? CurrentMainForm : NULL;
    popupowner = NULL; //initialization, otherwise the constructor would reset this later

    if (fnoo)
        Params.WndParent = NULL;
    else if ((current || modallist && !modallist->Count) && (Application->MainForm && Application->MainForm->HandleAllocated() && Params.WndParent == Application->MainForm->Handle || Params.WndParent == Application->Handle))
    {
        TBaseForm *f = dynamic_cast<TBaseForm *>(Application->MainForm);
        if (f && f->PopupChildren->IndexOf(this) >= 0)
        {
            f->PopupChildren->Remove(this);
            if (current && current != this && current->PopupChildren->IndexOf(this) < 0)
                current->PopupChildren->Add(this);
            popupowner = current;
        }
        RemoveFreeNotification(Application->MainForm);
        if (current && current != this)
            FreeNotification(current);

        if (current && modallist && modallist->Count)
        {
            int ix = modallist->Count;
            do
            {
                current = modallist->Items[--ix]->form;
            }
            while (ix && !current);
        }
        Params.WndParent = (PopupMode == pmExplicit && PopupParent ? PopupParent->Handle : current != this && current && (!fappwnd || Parent) ? current->Handle : GetDesktopWindow());

        if (Params.X == CW_USEDEFAULT && Position != poDefault)
        {
            if (KeepSize && !fsize.maxed)
            {
                Params.X = fsize.left;
                Params.Y = fsize.top;
                Params.Width = fsize.width;
                Params.Height = fsize.height;
            }
            else if (WindowState != wsMaximized)
            {
                Params.X = Left;
                Params.Y = Top;
                Params.Width = Width;
                Params.Height = Height;
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::CreateWnd()
{
    inherited::CreateWnd();
}
//---------------------------------------------------------------------------
void TBaseForm::WMEraseBkgnd(TWMEraseBkgnd &msg)
{
    msg.Result = 1;
    if (!ferase && ThemesAreTurnedOn)
        return;

    if (fonerase)
    {
        fonerase(this);
        return;
    }

    TRect r = ClientRect;
    FillRect(msg.DC, &r, Brush->Handle);
}
//---------------------------------------------------------------------------
void TBaseForm::set_erase(bool val)
{
    if (val == ferase)
        return;

    ferase = val;
    if (ferase || !ThemesAreTurnedOn)
        InvalidateEmptyArea(this);
}
//---------------------------------------------------------------------------
void TBaseForm::set_onerase(TNotifyEvent val)
{
    if (val == fonerase)
        return;

    fonerase = val;
    if (fonerase)
        InvalidateEmptyArea(this);
}
//---------------------------------------------------------------------------
bool TBaseForm::get_ontop()
{
    return (GetWindowLongPtr(Handle, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
}
//---------------------------------------------------------------------------
void TBaseForm::set_ontop(bool val)
{
    if (fontop == val)
        return;
    fontop = val;
    if (!Visible)
        return;
    DoSetOnTop(val);
}
//---------------------------------------------------------------------------
void TBaseForm::DoSetOnTop(bool val)
{
    if (val && Visible && OnTop || !val && !OnTop)
        return;

    if (val && Visible)
    {
        SetWindowPos(Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
        return;
    }

    /*HWND h = Handle;

     if (modallist && modallist->Count) {
     for (int i = modallist->Count - 1; i >= 0; i--) {
     if (modallist->Items[i]->Handle && modallist->Items[i]->Handle != Handle && modallist->Items[i]->form && modallist->Items[i]->form->Visible) {
     h = modallist->Items[i]->Handle;
     break;
     }
     }
     } else if (activelist && !settings.docked)
     h = activelist->Items[0]->Handle;
     else if (settings.docked && fDock)
     h = fDock->Handle;
     */
    if (!val)
    {
        SetWindowPos(Handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE/*| SWP_NOOWNERZORDER*/);
        if (modallist)
        {
            int ix = modallist->IndexOf(Handle);
            for (++ix; ix < modallist->Count; ++ix)
            {
                if (!modallist->Items[ix]->form && CurrentMainForm == this || !modallist->Items[ix]->form->fontop && modallist->Items[ix]->form->HasPopupOwner(this))
                    SetWindowPos(modallist->Items[ix]->Handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
            }
        }
    }
    //if (h != Handle)
    //SetWindowPos(Handle,h,0,0,0,0,SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);

    //SetWindowPos(Handle,HWND_TOP,0,0,0,0,SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);

}
//---------------------------------------------------------------------------
/*void TBaseForm::WMSysCommand(TWMSysCommand &msg)
 {
 if (KeepSize && (msg.CmdType & 0xFFF0) == SC_MAXIMIZE || (msg.CmdType & 0xFFF0) == SC_ZOOM)
 fsize.maxed = true;
 else if (KeepSize &&(msg.CmdType & 0xFFF0) == SC_RESTORE)
 fsize.maxed = false;

 msg.Result = DefWindowProc(Handle,msg.Msg,(*(TMessage*)&msg).WParam,(*(TMessage*)&msg).LParam);
 }*/
//---------------------------------------------------------------------------
void TBaseForm::WMSize(TWMSize &msg)
{
    if (KeepSize)
    {
        if (msg.SizeType == SIZE_MAXIMIZED)
            fsize.maxed = true;
        if (msg.SizeType == SIZE_RESTORED)
            fsize.maxed = false;
    }

    inherited::Dispatch(&msg);
}
//---------------------------------------------------------------------------
void TBaseForm::WMSizing(TMessage &msgo)
{
    TWMSizing *msg = (TWMSizing*) &msgo;
    msgo.Result = DefWindowProc(Handle, msgo.Msg, msgo.WParam, msgo.LParam);

    if (fconstraint)
        ConstraintSizing(*msg, minw, minh, maxw, maxh, wdiff, modx, hdiff, mody);
    if (fkeepsize)
        SaveFormSize(*msg->lprc);
}
//---------------------------------------------------------------------------
void TBaseForm::WMGetMinMaxInfo(TWMGetMinMaxInfo &msg)
{
    TMessage &msgo = *(TMessage*) &msg;
    msg.Result = DefWindowProc(Handle, msgo.Msg, msgo.WParam, msgo.LParam);
    if (!fconstraint)
        return;

    if (maxw > 0 && maxw < Screen->Width * 10)
    {
        msg.MinMaxInfo->ptMaxSize.x = min(msg.MinMaxInfo->ptMaxSize.x, (long)maxw);
        msg.MinMaxInfo->ptMaxTrackSize.x = min(msg.MinMaxInfo->ptMaxTrackSize.x, (long)maxw);
    }
    if (maxh > 0 && maxh < Screen->Height * 10)
    {
        msg.MinMaxInfo->ptMaxSize.y = min(msg.MinMaxInfo->ptMaxSize.y, (long)maxh);
        msg.MinMaxInfo->ptMaxTrackSize.y = min(msg.MinMaxInfo->ptMaxTrackSize.y, (long)maxh);
    }

    if (minw > 0)
    {
        msg.MinMaxInfo->ptMaxSize.x = max(msg.MinMaxInfo->ptMaxSize.x, (long)minw);
        msg.MinMaxInfo->ptMaxTrackSize.x = max(msg.MinMaxInfo->ptMaxTrackSize.x, (long)minw);
        msg.MinMaxInfo->ptMinTrackSize.x = (long)minw;
    }
    if (minh > 0)
    {
        msg.MinMaxInfo->ptMaxSize.y = max(msg.MinMaxInfo->ptMaxSize.y, (long)minh);
        msg.MinMaxInfo->ptMaxTrackSize.y = max(msg.MinMaxInfo->ptMaxTrackSize.y, (long)minh);
        msg.MinMaxInfo->ptMinTrackSize.y = (long)minh;
    }
}
//---------------------------------------------------------------------------
void TBaseForm::WMMoveAndMoving(TMessage &msg)
{
    msg.Result = DefWindowProc(Handle, msg.Msg, msg.WParam, msg.LParam);
    if (fkeepsize && Visible)
        SaveFormSize();
}
//---------------------------------------------------------------------------
/*void TBaseForm::WMMouseActivate(TWMMouseActivate &msg)
 {
 //  msg.Result = MA_NOACTIVATE;
 }*/
//---------------------------------------------------------------------------
void TBaseForm::ConstrainOff()
{
    fconstraint = false;
}
//---------------------------------------------------------------------------
void TBaseForm::Constrain(int minwidth, int minheight)
{
    Constrain(minwidth, minheight, 999999, 999999, 0, 0, 0, 0);
}
//---------------------------------------------------------------------------
void TBaseForm::Constrain(int minwidth, int minheight, int maxwidth, int maxheight)
{
    Constrain(minwidth, minheight, maxwidth, maxheight, 0, 0, 0, 0);
}
//---------------------------------------------------------------------------
void TBaseForm::Constrain(int minwidth, int minheight, int maxwidth, int maxheight, int widthdiff, int modx, int heightdiff, int mody)
{
    fconstraint = true;
    minw = min(Screen->Width, minwidth);
    minh = min(Screen->Height, minheight);
    maxw = maxwidth;
    maxh = maxheight;
    wdiff = widthdiff;
    this->modx = modx;
    hdiff = heightdiff;
    this->mody = mody;

    AutoSize();
}
//---------------------------------------------------------------------------
void TBaseForm::AutoSize()
{
    if (!fconstraint)
        return;

    static bool sizing = false;

    if (sizing)
        return;

    sizing = true;
    try
    {
        TWMSizing msg;
        TRect r = Rect(Left, Top, Left + Width, Top + Height);
        msg.fwSide = WMSZ_BOTTOMRIGHT;
        msg.lprc = &r;
        WMSizing(*reinterpret_cast<TMessage*>(&msg));

        Width = r.Width();
        Height = r.Height();
    }
    catch (...)
    {
        ;
    }
    sizing = false;
}
//---------------------------------------------------------------------------
TRect TBaseForm::GetAbsRect(TWinControl *control)
{
    TRect r;
    GetWindowRect(control->Handle, &r);
    TPoint p = ScreenToClient(Point(r.Left, r.Top));
    OffsetRect(&r, p.x - r.Left, p.y - r.Top);
    return r;
}
//---------------------------------------------------------------------------
void TBaseForm::set_keep(bool val)
{
    if (fkeepsize == val)
        return;
    fkeepsize = val;
    if (val)
        SaveFormSize();
}
//---------------------------------------------------------------------------
void TBaseForm::SaveFormSize(bool forced)
{
    if (!forced && (/*settings.intray ||*/ !Visible || fsize.maxed || WindowState == wsMinimized || WindowState == wsMaximized))
        return;

    SaveFormSize(Rect(Left, Top, Left + Width, Top + Height));
}
//---------------------------------------------------------------------------
void TBaseForm::RestorePosition(bool dimensions, bool topmost)
{
    SetWindowPos(Handle, topmost ? HWND_TOPMOST : HWND_NOTOPMOST, fsize.left, fsize.top, fsize.width, fsize.height, SWP_NOACTIVATE | (dimensions ? 0 : SWP_NOSIZE));
    SetBounds(fsize.left, fsize.top, (dimensions ? fsize.width : Width), (dimensions ? fsize.height : Height));
}
//---------------------------------------------------------------------------
void TBaseForm::SaveFormSize(TRect r)
{
    AutoSize();

    fsize.left = r.Left;
    fsize.top = r.Top;
    fsize.width = r.Width();
    fsize.height = r.Height();
}
//---------------------------------------------------------------------------
TRect TBaseForm::ChildRect(TControl *child)
{
    return ChildRect(child, 0, 0, 0, 0);
}
//---------------------------------------------------------------------------
TRect TBaseForm::ChildRect(TControl *child, int leftoffs, int topoffs, int rightoffs, int bottomoffs)
{
    TRect r = child->BoundsRect;
    if (child->Parent != this)
    {
        TPoint p = ScreenToClient(child->Parent->ClientToScreen(Point(0, 0)));
        OffsetRect(&r, p.x, p.y);
    }
    return Rect(r.Left + leftoffs, r.Top + topoffs, r.Right + rightoffs, r.Bottom + bottomoffs);
}
//---------------------------------------------------------------------------
void TBaseForm::EnableAll()
{
    for (int ix = 0; ix < ControlCount; ++ix)
    {
        Controls[ix]->Enabled = true;
    }
}
//---------------------------------------------------------------------------
void TBaseForm::DisableAll()
{
    for (int ix = 0; ix < ControlCount; ++ix)
    {
        Controls[ix]->Enabled = false;
    }
}
//---------------------------------------------------------------------------
/*bool TBaseForm::get_shadow()
 {
 return shadow != 0;
 }*/
//---------------------------------------------------------------------------
void TBaseForm::set_shadow(bool val)
{
    if (val == fshadowed)
        return;
    if (!val)
    {
        SettingsChangeEvent(this, settingsproc, false);
        delete shadow;
        shadow = NULL;
        fshadowed = false;

        return;
    }

    SettingsChangeEvent(this, settingsproc, true);
    fshadowed = true;

    if (settings.toolshadow)
        shadow = new TBehindShadow(Application, this);
}
//---------------------------------------------------------------------------
void TBaseForm::settingsproc(void *caller, TSettingsProcState state)
{
    if (state != spsEnd)
        return;

    if (settings.toolshadow && fshadowed && !shadow)
        shadow = new TBehindShadow(Application, this);
    else if (!settings.toolshadow && fshadowed && shadow)
    {
        delete shadow;
        shadow = NULL;
    }
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(7);
#endif

