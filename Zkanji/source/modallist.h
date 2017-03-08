//---------------------------------------------------------------------------

#ifndef modallistH
#define modallistH
//---------------------------------------------------------------------------

struct TKanji;
//class TWordCollection;

class TBaseForm;

struct TModalItem
{
    HWND Handle;
    TBaseForm *form; // NULL for message boxes and non TBaseForms

    bool info;
    bool clip;

    byte iColl;
    TKanji* iKanji;
};

class TModalList : public TTemplateList<TModalItem, true>
{
private:
    typedef TTemplateList<TModalItem, true> inherited;

    virtual void Delete(int ix)
    {
        inherited::Delete(ix);
    }
public:
    virtual ~TModalList()
    {
        Clear();
    }

    int IndexOf(const HWND h);
    int Add(TBaseForm *form, const HWND h);
    void Delete(const HWND h);

    void Update(const HWND hold, const HWND hnew); // Update stored window handle when a form is recreated.
    void UpdateTop(const HWND hnew); // Change topmost handle when it's a non VCL form and we need to store it.
};

extern TModalList *modallist;
#endif
