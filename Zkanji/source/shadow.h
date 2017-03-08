//---------------------------------------------------------------------------

#ifndef shadowH
#define shadowH
//---------------------------------------------------------------------------
#include "baseform.h"

class TBehindShadow : public TCustomControl
{
private: //User declarations
    typedef TCustomControl inherited;

    TBmp *bmp;

    TBaseForm *form;

    bool factive;
    bool fncactive;
    int fleft;
    int ftop;
    int fwidth;
    int fheight;

    void paint(TRect *r = NULL);
    void hide();
    void drawbmp(HDC sdc, int w, int h, bool hot);
    void copybits(byte *dest, int x, int y, int h, int linew, const int *src, int cx, int cy);
    void fillbitsh(byte *dest, int x, int y, int h, int cnt, int linew, const int *src, int cy);
    void fillbitsv(byte *dest, int x, int y, int h, int cnt, int linew, const int *src, int cx);

    TRect dimensions();

    void wmposchanging(TWMWindowPosChanging &msg);
    void wmreschange(TMessage &msg);
    void wmactivate(TMessage &msg);

    void activated(void *caller);
    void deactivated(void *caller);

    void __fastcall WProc(TMessage &msg);
    void __fastcall CreateParams(TCreateParams &Params);
public:
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosChanging, wmposchanging)
        MESSAGE_HANDLER(WM_DISPLAYCHANGE, TMessage, wmreschange)
        MESSAGE_HANDLER(WM_ACTIVATE, TMessage, wmactivate)
    END_MESSAGE_MAP(inherited)

    __fastcall TBehindShadow(TComponent* Owner, TBaseForm *frm);
    virtual __fastcall ~TBehindShadow();
} ;
#endif
