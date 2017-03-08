//---------------------------------------------------------------------------

#ifndef msgformH
#define msgformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"

class TfMsg : public TBaseForm
{
__published: //IDE-managed Components
    TLabel *lbMsg;
    TButton *btnAbort;
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall btnAbortClick(TObject *Sender);
private: //User declarations
    typedef TBaseForm inherited;

    void __fastcall Show()
    {
        TBaseForm::Show();
    }
    void *p;
    HWND last;

    void (*abortproc)();

    bool ontop;
    void appactivate(void *caller);
protected:
    void __fastcall CreateParams(TCreateParams &Params);
public: //User declarations
    __fastcall TfMsg(TComponent* Owner);
    virtual __fastcall ~TfMsg();
    void __fastcall ShowEx(UnicodeString amsg, void (*abortcallback)() = NULL, UnicodeString abortcaption = UnicodeString());
};
//---------------------------------------------------------------------------
extern PACKAGE TfMsg *fMsg;
//---------------------------------------------------------------------------
void ShowMessageForm(UnicodeString message, void (*abortcallback)() = NULL, UnicodeString abortcaption = UnicodeString());
void HideMessageForm();

#endif
