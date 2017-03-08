//---------------------------------------------------------------------------

#ifndef updateH
#define updateH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "baseform.h"
#include "socket.h"
#include <ComCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TConnectionHandler;

class TfUpdate : public TBaseForm
{
__published: //IDE-managed Components
    TTimer *tTimer;
    TLabel *lbMsg;
    TLabel *lbDots;
    TPanel *pProgress;
    TLabel *lbProgress;
    TProgressBar *pbProgress;
    TLabel *lbAdminWarn;
    TButton *btnAction;
    TBevel *bBottom;
    TRichEdit *mResult;
    TPanel *pUpdates;
    TButton *btnUpdate;
    TLabel *lbUpdate;
    TButton *btnDict;
    TLabel *lbDict;
    TPanel *pLink;
    TPanel *pViewHide;
    TSpeedButton *btnHiddenButton;
    void __fastcall btnActionClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall tTimerTimer(TObject *Sender);
    void __fastcall btnUpdateClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall pLinkClick(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall pViewHideClick(TObject *Sender);
    void __fastcall pViewHideMouseEnter(TObject *Sender);
    void __fastcall pViewHideMouseLeave(TObject *Sender);
    void __fastcall mResultKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall btnHiddenButtonClick(TObject *Sender);

private: //User declarations
    typedef TBaseForm inherited;

    TRect origsize;

    TConnectionHandler *web;
    unsigned int getid;
    unsigned int getdictid;

    int tick;
    unsigned int timeout;

    FILE *dlfile;
    UnicodeString dlfilename;

    UnicodeString result;
    UnicodeString dictresult;
    void getproc(unsigned int threadID, TConnectionState state, const char *data, unsigned int datalen); // Download process for update.txt and dictupdate.txt.
    void getproc2(unsigned int threadID, TConnectionState state, const char *data, unsigned int datalen);

    void dlproc(unsigned int threadID, TConnectionState state, const char *data, unsigned int datalen);

    enum TUpdateState { usChecking, usDownloading, usError, usAborted, usDone };
    TUpdateState updatestate;

    bool oldversion; // This program is too old to download at least on available version, which can't handle its user file format.

    struct TVersionData
    {
        AnsiString version; // Version string of separate releases to be shown in update window.
        AnsiString path; // Path to release text when loading list. Updated to file path once every version data is found.

        UnicodeString info;

        char utf8part[6]; // We get the data in char, so to handle utf8 strings correctly, we have to keep a buffer of partial utf8 characters
        byte partlen;

        unsigned int threadid;
        bool done;

        TVersionData(AnsiString version, AnsiString path);
        ~TVersionData();
    };

    TTemplateList<TVersionData, true> versions;
    UnicodeString dictversion; // Date of latest dictionary that can be downloaded.
    UnicodeString dictpath; // Path to latest dictionary to be downloaded.

    bool versionpassed;
    enum TVersionFor { vfGeneral, vfProgram, vfDictionary };
    void addversion(const wchar_t *line, TVersionFor type); // Interprets lines in update.txt and dictupdate.txt

    void ShowMessage(UnicodeString msg, bool showwindow = true, bool hidepanel = true);
    void HideMessage(); // Hides the top message and the "animated" dots, and stops the timer.

    void updateterminateproc(); // Called by web handler when all sockets are closed

    void startget(); // Starts download of update.txt
    void startgetdict(); // Starts download of dictupdate.txt
    void doit(); // Starts download of update information of available versions.

    bool gettersrunning();

    void abortall();
    void showupdates();
    void hideupdates();

    void msg_modal(TMessage &msg);
    void msg_hide(TMessage &msg);
    void nccalcsize(TMessage &msg);
    void msg_notify(TWMNotify &msg);
    void msg_hittest(TWMNCHitTest &msg);
    void msg_sch(TMessage &msg);
public: //User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_Z_MODALBEGIN, TMessage, msg_modal);
        MESSAGE_HANDLER(WM_Z_HIDE, TMessage, msg_hide);
        MESSAGE_HANDLER(WM_NOTIFY, TWMNotify, msg_notify);
        MESSAGE_HANDLER(WM_NCHITTEST, TWMNCHitTest, msg_hittest);
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfUpdate(TComponent* Owner);
    virtual __fastcall ~TfUpdate();
};
//---------------------------------------------------------------------------
extern PACKAGE TfUpdate *fUpdate;
//---------------------------------------------------------------------------

void CheckForUpdates(bool show = true);

#endif
