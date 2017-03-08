//---------------------------------------------------------------------------

#ifndef writeobjH
#define writeobjH
//---------------------------------------------------------------------------

class TPointListEx;

class TStrokeTester
{
private:
    int fstroke;
    int felemindex;
    TTemplateList<TPointListEx, true> *strokes;

    void __fastcall set_elem(int ix);
    void __fastcall set_stroke(int ix);
public:
    __fastcall TStrokeTester();
    virtual __fastcall ~TStrokeTester();

    bool __fastcall test(TPointListEx *list, TPointListEx *match, TPointListEx *strokelist = NULL, TPointListEx *orig = NULL, TStrings *errors = NULL);

    __property int element = { read = felemindex, write = set_elem };
    __property int stroke = { read = fstroke, write = set_stroke };
};

#endif
