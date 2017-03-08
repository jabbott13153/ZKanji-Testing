//---------------------------------------------------------------------------

#ifndef sodpopupH
#define sodpopupH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include "baseform.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TKanjiDrawObj;
class TWordCollection;
class TKanji;
enum TDrawStrokeState;

class TfSOD : public TBaseForm
{
__published: //IDE-managed Components
    TZPaintBox *pb;
    TPanel *pan;
    TSpeedButton *sbStop;
    TSpeedButton *sbPlay;
    TSpeedButton *sbPause;
    TSpeedButton *sbRew;
    TSpeedButton *sbBack;
    TSpeedButton *sbFore;
    TSpeedButton *sbNum;
    void __fastcall FormMouseLeave(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall sbStopClick(TObject *Sender);
    void __fastcall sbPlayClick(TObject *Sender);
    void __fastcall sbPauseClick(TObject *Sender);
    void __fastcall sbRewClick(TObject *Sender);
    void __fastcall sbFFClick(TObject *Sender);
    void __fastcall sbBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall sbNumClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
private: //User declarations
    TKanjiDrawObj *kobj;
    TFuncPtr fproc;
    TBaseForm *wnd;

    int rwidth;
    int rpos;
    bool topside;
    bool factivating;

    void kproc(TDrawStrokeState state);
    void deactivate(void *caller);
    void activate(void *caller);

    void z_pos(TWMWindowPosChanging &msg);
//void z_posed(TWMWindowPosChanged &msg);
    void z_init(TMessage &msg);
public: //User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosChanging, z_pos)
//MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, TWMWindowPosChanged, z_posed)
        MESSAGE_HANDLER(WM_Z_INIT, TMessage, z_init)
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfSOD(TComponent* Owner);
    virtual __fastcall ~TfSOD();

    void ShowEx(TBaseForm *form, const TRect &pos, TFuncPtr aproc, wchar_t kanji);

    __property bool Activating = { read = factivating };
};
//---------------------------------------------------------------------------
extern PACKAGE TfSOD *fSOD;
//---------------------------------------------------------------------------
#endif
