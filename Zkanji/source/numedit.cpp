//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "numedit.h"
#include "localeinfo.h"
#include "baseform.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester1(118);
#endif

template<typename T>
TCustomNumEdit<T>::TCustomNumEdit(TWinControl *ed, T amin, T amax, bool afloat, bool ahex, bool aimmediate) : eedit(NULL), erestore(true), etrigger(true), cedit(NULL), fmin(amin), fmax(amax), ffloat(afloat), fhex(ahex), empty(false), ffunc(NULL), fimmediate(aimmediate)
{
    eedit = dynamic_cast<TEdit *>(ed);
    cedit = dynamic_cast<TComboBox *>(ed);

    if (text.Pos('.'))
        text[text.Pos('.')] = locale.decimal;

    if (eedit)
    {
        fonchange = eedit->OnChange;
        fonenter = eedit->OnEnter;
        fonexit = eedit->OnExit;
        fonkeypress = eedit->OnKeyPress;
        eedit->OnChange = edOnChange;
        eedit->OnEnter = edOnEnter;
        eedit->OnExit = edOnExit;
        eedit->OnKeyPress = edOnKeyPress;
    }
    else if (cedit)
    {
        fonchange = cedit->OnChange;
        fonenter = cedit->OnEnter;
        fonexit = cedit->OnExit;
        fonkeypress = cedit->OnKeyPress;
        cedit->OnChange = edOnChange;
        cedit->OnEnter = edOnEnter;
        cedit->OnExit = edOnExit;
        cedit->OnKeyPress = edOnKeyPress;
    }
    else
        THROW(L"Control does not match requirements!");

    ((TBaseForm*)control->Owner)->AddEvent(this, formActivate, eltActivate);
    ((TBaseForm*)control->Owner)->AddEvent(this, formDeactivate, eltDeactivate);
    AppActivateEvent(control, appActivate, true, true);
    AppActivateEvent(control, appDeactivate, false, true);
}
//---------------------------------------------------------------------------
template<typename T>
TCustomNumEdit<T>::~TCustomNumEdit()
{
    if (eedit)
    {
        eedit->OnChange = fonchange;
        eedit->OnEnter = fonenter;
        eedit->OnExit = fonexit;
        eedit->OnKeyPress = fonkeypress;
    }
    else if (cedit)
    {
        cedit->OnChange = fonchange;
        cedit->OnEnter = fonenter;
        cedit->OnExit = fonexit;
        cedit->OnKeyPress = fonkeypress;
    }

    ((TBaseForm*)control->Owner)->RemoveEvents(this);
    AppActivateEvent(control, appActivate, true, false);
    AppActivateEvent(control, appDeactivate, false, false);
}
//---------------------------------------------------------------------------
template<typename T>
void __fastcall TCustomNumEdit<T>::edOnChange(TObject *Sender)
{
    if (!empty || text.Length())
    {
        //remove any non number from the box to stop text pasting bandits!
        int ss;
        if (eedit)
            ss = eedit->SelStart;
        else if (cedit)
            ss = cedit->SelStart;
        int nss = ss;
        bool dot = false;

        UnicodeString s = "";
        for (int i = 1; i <= text.Length(); i++)
        {
            if (((locale.numformat == 1 || locale.numformat == 2) && i == 1 && (int)fmin < 0 && text[i] == locale.negativesign && !fhex) || ((locale.numformat == 3 || locale.numformat == 4) && i == text.Length() && (int)fmin < 0 && text[i] == locale.negativesign && !fhex) || (locale.numformat == 0 && (i == 1 || i == text.Length()) && text[1] == '(' && text[text.Length()] == ')') || text[i] >= '0' && text[i] <= '9' || ffloat && text[i] == locale.decimal && !dot || fhex && (text[i] >= 'a' && text[i] <= 'f' || text[i] >= 'A' && text[i] <= 'F'))
            {
                s = s + text[i];
                if (text[i] == locale.decimal)
                    dot = true;
            }
            else if (i <= ss)
                nss--;
        }

        if (s.Length() != text.Length())
        {
            if (eedit)
            {
                eedit->Text = s;
                eedit->SelStart = nss;
            }
            else if (cedit)
            {
                cedit->Text = s;
                cedit->SelStart = nss;
            }
        }

        if (fimmediate && text.Length() && saved != value)
        {
            saved = value;
            if (ffunc)
                ffunc();
        }
    }

    if (fonchange)
        fonchange(Sender);
}
//---------------------------------------------------------------------------
template<typename T>
void TCustomNumEdit<T>::formActivate(void *caller)
{
    objfocused = control->Focused();
    Activated();
}
//---------------------------------------------------------------------------
template<typename T>
void TCustomNumEdit<T>::formDeactivate(void *caller)
{
    Deactivated();
    objfocused = false;
}
//---------------------------------------------------------------------------
template<typename T>
void TCustomNumEdit<T>::appActivate(void *caller)
{
    objfocused = control->Focused();
    Activated();
}
//---------------------------------------------------------------------------
template<typename T>
void TCustomNumEdit<T>::appDeactivate(void *caller)
{
    Deactivated();
    objfocused = false;
}
//---------------------------------------------------------------------------
template<typename T>
void __fastcall TCustomNumEdit<T>::edOnEnter(TObject *Sender)
{
    objfocused = true;
    Activated();

    if (fonenter)
        fonenter(Sender);
}
//---------------------------------------------------------------------------
template<typename T>
void __fastcall TCustomNumEdit<T>::edOnExit(TObject *Sender)
{
    Deactivated();

    objfocused = false;
    if (fonexit)
        fonexit(Sender);
}
//---------------------------------------------------------------------------
template<typename T>
void TCustomNumEdit<T>::Deactivated()
{
    if (objfocused)
        do_deactivate();
}
//---------------------------------------------------------------------------
template<typename T>
void TCustomNumEdit<T>::Activated()
{
    if (objfocused)
        do_activate();
}
//---------------------------------------------------------------------------
template<typename T>
void TCustomNumEdit<T>::do_activate()
{
    if (!empty || text.Length())
        saved = value;
}
//---------------------------------------------------------------------------
template<typename T>
void TCustomNumEdit<T>::do_deactivate()
{
    if (!etrigger)
    {
        value = saved;
        return;
    }

    if (!empty || text.Length())
    {
        try
        {
            //value is a property! the getter function limits its value between fmin and fmax, and removes unwanted characters
            //also calls callback if necessary
            value = value;
        }
        catch (...)
        {
            ;
        }
    }
    else if (ffunc)
        ffunc();
}
//---------------------------------------------------------------------------
template<typename T>
void __fastcall TCustomNumEdit<T>::edOnKeyPress(TObject *Sender, wchar_t &Key)
{
    if (erestore && Key == VK_ESCAPE)
        value = saved;

    if (!(GetKeyState(VK_CONTROL) & (1 << 15)) && Key != 8 && Key != 13 && (Key < '0' || Key > '9') && ((Key < 'a' || Key > 'f') && (Key < 'A' || Key > 'F') || !fhex) && (Key != locale.decimal || !ffloat) && (Key != locale.negativesign && Key != '(' && Key != ')' || (int)fmin >= 0 || fhex))
        Key = 0;

    //check negative numbers
    if (!locale.numformat && Key == locale.negativesign || locale.numformat && (Key == '(' || Key == ')'))
        Key = 0;

    if (Key == 13)
    {
        if (!empty || text.Length())
        {
            if (!text.Length())
            {
                if (eedit)
                    eedit->Text = '0';
                else if (cedit)
                    cedit->Text = '0';
            }
            try
            {
                //value is a property! the getter function limits its value between fmin and fmax, and removes unwanted characters
                //also calls callback if necessary
                value = value;
            }
            catch (...)
            {
                ;
            }
            Key = 0;
        }
    }

    if (fonkeypress)
        fonkeypress(Sender, Key);
}
//---------------------------------------------------------------------------
template<typename T>
void TCustomNumEdit<T>::set_min(T val)
{
    if (fmin == val)
        return;
    fmin = val;
    if (fmax < fmin || empty && !text.Length())
        return;

    try
    {
        //value is a property! the getter function limits its value between fmin and fmax, and removes unwanted characters
        //also calls callback if necessary
        value = value;
    }
    catch (...)
    {
        ;
    }
}
//---------------------------------------------------------------------------
template<typename T>
void TCustomNumEdit<T>::set_max(T val)
{
    if (fmax == val)
        return;
    fmax = val;
    if (fmax < fmin || empty && !text.Length())
        return;

    try
    {
        //value is a property! the getter function limits its value between fmin and fmax, and removes unwanted characters
        //also calls callback if necessary
        value = value;

    }
    catch (...)
    {
        ;
    }
}
//---------------------------------------------------------------------------
template<typename T>
T TCustomNumEdit<T>::get_value()
{
    if (empty && !text.Length())
        THROW(L"This is a test");

    T t;
    try
    {
        if (fhex)
            t = AStow(text);
        else if (!ffloat)
            t = text.ToInt();
        else
            t = text.ToDouble();
    }
    catch (...)
    {
        return saved;
    }

    t = fmax >= fmin && fmin > t ? fmin : t;
    return fmax < fmin || t < fmax ? t : fmax;
}
//---------------------------------------------------------------------------
template<typename T>
void TCustomNumEdit<T>::set_value(T val)
{
    if (fmin <= fmax)
    {
        if (val < fmin)
            val = fmin;
        if (val > fmax)
            val = fmax;
    }

    if (value_valid && (val == saved && value == val))
        return;

    if (fhex)
    {
        if (eedit)
            eedit->Text = wtoAS(val);
        else if (cedit)
            cedit->Text = wtoAS(val);
    }
    else
    {
        if (eedit)
            eedit->Text = (UnicodeString)val;
        else if (cedit)
            cedit->Text = (UnicodeString)val;
    }
    if (control->Focused())
    {
        if (eedit)
        {
            eedit->SelLength = 0;
            eedit->SelStart = eedit->Text.Length();
        }
        else if (cedit)
        {
            cedit->SelLength = 0;
            cedit->SelStart = cedit->Text.Length();
        }
    }

    if (saved != value)
    {
        saved = value;
        if (ffunc)
            ffunc();
    }
}
//---------------------------------------------------------------------------
template<typename T>
bool TCustomNumEdit<T>::get_valid()
{
    if (empty && !text.Length())
        return true;

    T t;
    try
    {
        if (fhex)
            t = AStow(text);
        else if (!ffloat)
            t = text.ToInt();
        else
            t = text.ToDouble();
    }
    catch (...)
    {
        return false;
    }

    return fmax < fmin || t >= fmin && t <= fmax;
}
//---------------------------------------------------------------------------
template<typename T>
TWinControl* TCustomNumEdit<T>::get_control()
{
    if (eedit)
        return eedit;
    return cedit;
}
//---------------------------------------------------------------------------
template<typename T>
UnicodeString TCustomNumEdit<T>::get_text()
{
    if (eedit)
        return eedit->Text;
    return cedit->Text;
}
//---------------------------------------------------------------------------

template class TCustomNumEdit<int>;
template class TCustomNumEdit<double>;
template class TCustomNumEdit<word>;

#ifdef _DEBUG
static TGlobalTest tester2(119);
#endif

