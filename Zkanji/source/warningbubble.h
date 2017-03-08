//---------------------------------------------------------------------------

#ifndef warningbubbleH
#define warningbubbleH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "baseform.h"
//---------------------------------------------------------------------------

class TSmallCloseButton;

enum TBubbleShowState { bssShowing, bssNormal, bssHiding };
class TfBubble : public TBaseForm
{
__published: //IDE-managed Components
    TLabel *lb2;
    TLabel *Label2;
    TTimer *tTimer;
    TLabel *lbDue;
    TLabel *Label4;
    TLabel *Label3;
    TLabel *lb1;
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall tTimerTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private: //User declarations
    TBubbleShowState state;
    TSmallCloseButton *btnClose;
    void cbgptr(TCanvas *c, TRect const &r);

    void WMEnable(TMessage &msg);
    void WMZEnable(TMessage &msg);
public: //User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_ENABLE, TMessage, WMEnable)
        MESSAGE_HANDLER(WM_Z_ENABLE, TMessage, WMZEnable)
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfBubble(TComponent* Owner);
    virtual __fastcall ~TfBubble();
};
//---------------------------------------------------------------------------
extern PACKAGE TfBubble *fBubble;
//---------------------------------------------------------------------------
void BubbleWarning(int num);

#endif
