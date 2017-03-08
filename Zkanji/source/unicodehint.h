//---------------------------------------------------------------------------

#ifndef unicodehintH
#define unicodehintH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <AppEvnts.hpp>
#include "ZPaintBox.h"
//---------------------------------------------------------------------------

typedef void(__closure * TZPaintProc)(TObject * Sender, TCanvas * Canvas, TRect area, void *data);

class THintFormW : public TCustomControl
{
private: //User declarations
    typedef TCustomControl inherited;

//TZPaintProc fonpaint;
//void *fdata;
    TBmp *bmp;

    bool fborders;
    int fmargin;

    int lineh;
    int maxwidth;
    int xpos;
    int ypos;

    int alpha;
    bool showing;
    bool timing;

    void __fastcall WMNCHitTest(TWMNCHitTest &msg);
    void __fastcall WMNCPaint(TMessage &msg);
    void __fastcall WMTimer(TWMTimer &msg);
    void __fastcall Paint();
    void __fastcall Erase(TMessage &msg);
    void __fastcall Init(TMessage &msg);
    void __fastcall WMNCActivate(TMessage &msg);
    void __fastcall CreateParams(TCreateParams &Params);
    void set_borders(bool val);
    void set_acolor(TColor val);
public: //User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_NCHITTEST, TWMNCHitTest, WMNCHitTest)
        MESSAGE_HANDLER(WM_ERASEBKGND, TMessage, Erase)
        MESSAGE_HANDLER(WM_NCPAINT, TMessage, WMNCPaint)
        MESSAGE_HANDLER(WM_TIMER, TWMTimer, WMTimer)
    END_MESSAGE_MAP(TCustomControl)

    __fastcall THintFormW(TComponent* Owner);
    virtual __fastcall ~THintFormW();

    void Popup(TRect apos, TZPaintProc aonpaint, void *adata = NULL);
    void Hide(bool immediate);

    __property Canvas;
    __property bool Border = { read = fborders, write = fborders };
};

extern THintFormW *hint;
//---------------------------------------------------------------------------
#endif
