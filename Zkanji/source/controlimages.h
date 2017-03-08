//---------------------------------------------------------------------------

#ifndef controlimagesH
#define controlimagesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ImgList.hpp>
#include "alphaimagelist.h"

class TAlphaImageList;

enum TDrawnControlboxState
{
        /* only for checkbox and radiobox */ dcsDisabled, dcsDisabledChecked, dcsEmpty, dcsEmptyHovered,
        dcsEmptyDown, dcsChecked, dcsCheckedHovered, dcsCheckedDown,
        /* only for plusminus */ dcsNormal, dcsHovered, dcsDown
};
enum TDrawnControlType { dctCheckbox, dctRadiobox, dctPlusbutton, dctMinusbutton };

template<TDrawnControlType ctype>
class TDrawnControlboxTemplate
{
private:
    static int usagecnt;
    static TAlphaImageList *images;
    int get_width();
    int get_height();
public:
    TDrawnControlboxTemplate();
    ~TDrawnControlboxTemplate();

    void draw(TCanvas *c, int x, int y, TDrawnControlboxState state);
    void draw(TCanvas *c, int x, int y, TDrawnControlboxState state, TColor bk);

    __property int width = { read = get_width };
    __property int height = { read = get_height };
};

template<TDrawnControlType ctype>
int TDrawnControlboxTemplate<ctype>::usagecnt = 0;
template<TDrawnControlType ctype>
TAlphaImageList *TDrawnControlboxTemplate<ctype>::images = NULL;

//---------------------------------------------------------------------------
template<TDrawnControlType ctype>
TDrawnControlboxTemplate<ctype>::TDrawnControlboxTemplate()
{
    if (!usagecnt)
    {
        switch (ctype)
        {
        case dctCheckbox:
            images = new TAlphaImageList(cdata->imgCheckbox, cdata->imgCheckboxAlpha);
            break;
        case dctRadiobox:
            images = new TAlphaImageList(cdata->imgRadiobox, cdata->imgRadioboxAlpha);
            break;
        case dctPlusbutton:
        case dctMinusbutton:
            images = new TAlphaImageList(cdata->imgPlusMinusbtn, cdata->imgPlusMinusbtnAlpha);
            break;
        }
    }

    usagecnt++;
}
//---------------------------------------------------------------------------
template<TDrawnControlType ctype>
TDrawnControlboxTemplate<ctype>::~TDrawnControlboxTemplate()
{
    usagecnt--;

    if (!usagecnt)
    {
        delete images;
    }
}
//---------------------------------------------------------------------------
template<TDrawnControlType ctype>
void TDrawnControlboxTemplate<ctype>::draw(TCanvas *c, int x, int y, TDrawnControlboxState state)
{
    switch (ctype)
    {
    case dctPlusbutton:
        state = (TDrawnControlboxState)(int(state) -int(dcsNormal));
        break;
    case dctMinusbutton:
        state = (TDrawnControlboxState)(int(state) -int(dcsNormal) + 3);
        break;
    }
    images->draw(c, x, y, (int)state);
}
//---------------------------------------------------------------------------
template<TDrawnControlType ctype>
void TDrawnControlboxTemplate<ctype>::draw(TCanvas *c, int x, int y, TDrawnControlboxState state, TColor bk)
{
    switch (ctype)
    {
    case dctPlusbutton:
        state = (TDrawnControlboxState)(int(state) -int(dcsNormal));
        break;
    case dctMinusbutton:
        state = (TDrawnControlboxState)(int(state) -int(dcsNormal) + 3);
        break;
    }
    images->draw(c, x, y, (int)state, bk);
}
//---------------------------------------------------------------------------
template<TDrawnControlType ctype>
int TDrawnControlboxTemplate<ctype>::get_width()
{
    return images->width;
}
//---------------------------------------------------------------------------
template<TDrawnControlType ctype>
int TDrawnControlboxTemplate<ctype>::get_height()
{
    return images->height;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class Tcdata : public TDataModule
{
__published: //IDE-managed Components
    TImageList *imgCheckbox;
    TImageList *imgCheckboxAlpha;
    TImageList *imgRadiobox;
    TImageList *imgRadioboxAlpha;
    TImageList *imgPlusMinusbtn;
    TImageList *imgPlusMinusbtnAlpha;
private: //User declarations
public: //User declarations
    __fastcall Tcdata(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tcdata *cdata;
//---------------------------------------------------------------------------

typedef TDrawnControlboxTemplate<dctCheckbox> TDrawnCheckbox;
typedef TDrawnControlboxTemplate<dctRadiobox> TDrawnRadiobox;
//typedef TDrawnControlboxTemplate<dctPlusbutton> TDrawnPlusbutton;
//typedef TDrawnControlboxTemplate<dctMinusbutton> TDrawnMinusbutton;
class TDrawnPlusbutton : public TDrawnControlboxTemplate<dctPlusbutton>
{

};
class TDrawnMinusbutton : public TDrawnControlboxTemplate<dctMinusbutton>
{

};

#endif
