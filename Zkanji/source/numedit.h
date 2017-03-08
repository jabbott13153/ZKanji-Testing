//---------------------------------------------------------------------------

#ifndef numeditH
#define numeditH
//---------------------------------------------------------------------------

typedef void(__closure * TFuncPtr)();

template<typename T>
class TCustomNumEdit
{
private:
    T fmin;
    T fmax;
    T saved;
    bool empty;
    bool fimmediate;
    bool objfocused;
    bool erestore;
    bool etrigger;

    bool ffloat;
    bool fhex;

    TEdit *eedit;
    TComboBox *cedit;

    TFuncPtr ffunc;

    TNotifyEvent fonchange;
    TNotifyEvent fonenter;
    TNotifyEvent fonexit;
    TKeyPressEvent fonkeypress;

    void set_min(T val);
    void set_max(T val);
    T get_value();
    void set_value(T val);
    bool get_valid();

    TWinControl* get_control();
    UnicodeString get_text();

    void __fastcall edOnChange(TObject *Sender);
    void __fastcall edOnEnter(TObject *Sender);
    void __fastcall edOnExit(TObject *Sender);
    void formActivate(void *caller);
    void formDeactivate(void *caller);
    void appActivate(void *caller);
    void appDeactivate(void *caller);
    void Deactivated();
    void Activated();

    void __fastcall edOnKeyPress(TObject *Sender, wchar_t &Key);

protected:
    virtual void do_activate();
    virtual void do_deactivate();

    TCustomNumEdit(TWinControl *ed, T amin = 0, T amax = -1, bool afloat = false, bool ahex = false, bool aimmediate = false);
    virtual ~TCustomNumEdit();

public:
    __property T min = { read = fmin, write = set_min };
    __property T max = { read = fmax, write = set_max };
    __property bool allow_empty = { read = empty, write = empty };
    __property bool escape_restore = { read = erestore, write = erestore }; //escape key restores saved value
    __property bool exit_triggers = { read = etrigger, write = etrigger }; //trigger callback when the focus is moved to another window
    __property bool immediate_callback = { read = fimmediate };
    __property T value = { read = get_value, write = set_value };
    __property bool value_valid = { read = get_valid };
    __property TFuncPtr callback = { read = ffunc, write = ffunc };
    __property TWinControl *control = { read = get_control };
    __property bool focused = { read = objfocused };
    __property UnicodeString text = { read = get_text };
};

class TNumEdit : public TCustomNumEdit<int>
{
private:
    typedef TCustomNumEdit<int> inherited;
public:
    TNumEdit(TWinControl *ed, int amin = 0, int amax = -1, bool aimmediate = false) : inherited(ed, amin, amax, false, false, aimmediate)
    {
        ;
    }
};

class TFloatEdit : public TCustomNumEdit<double>
{
private:
    typedef TCustomNumEdit<double> inherited;
public:
    TFloatEdit(TWinControl *ed, double amin = 0.0, double amax = -1.0, bool aimmediate = false) : inherited(ed, amin, amax, true, false, aimmediate)
    {
        ;
    }
};

class THexEdit : public TCustomNumEdit<word>
{
private:
    typedef TCustomNumEdit<word> inherited;
public:
    THexEdit(TWinControl *ed, bool aimmediate = false) : inherited(ed, 0, 65535, false, true, aimmediate)
    {
        ;
    }
};

#endif
