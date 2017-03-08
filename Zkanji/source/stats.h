//---------------------------------------------------------------------------

#ifndef statsH
#define statsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"

class TWordCollection;

class TfStats : public TBaseForm
{
__published: //IDE-managed Components
    TBitBtn *BitBtn1;
    TLabel *lbBuild;
    TLabel *lbBuildEx;
    TBevel *b1;
    TLabel *Label5;
    TComboBox *cbDict;
    TBevel *b0;
    TBevel *b4;
    TBevel *b3;
    TPanel *pStats;
    TLabel *Label1;
    TLabel *lbWCnt;
    TLabel *Label2;
    TLabel *lbKWCnt;
    TLabel *Label3;
    TLabel *lbKCnt;
    TLabel *Label4;
    TLabel *lbKECnt;
    TLabel *Label6;
    TLabel *lbPop;
    TLabel *Label8;
    TLabel *lbMed;
    TLabel *Label10;
    TLabel *lbNon;
    TLabel *Label12;
    TLabel *lbCnt;
    TBevel *Bevel2;
    TBevel *Bevel3;
    TLabel *Label7;
    TLabel *lbMCnt;
    TBevel *b2;
    TLabel *lbInfo;
    TMemo *mInfo;
    void __fastcall cbDictClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormResize(TObject *Sender);
private: //User declarations
    TWordCollection *fcol;

    typedef void(__closure * TDoneProc)(byte coll);

    class TCollectDataThread : public TThread
    {
    private:
        typedef TThread inherited;

        bool fending;
        bool ffinished;
        bool fcompleted;
        byte coll;
        TDoneProc proc;

        //collected data:
        int pop; //number of popular words (words with frequency above 3500)
        int med; //number of medium popularity words (not pop and freq above 1500)
        int non; //not popular nor medium frequent

        int cnt; //unique word count of words with same meaning but either different kanji or different kana form (but not both)
        int mcnt; //unique meaning count
        //---------------

        bool samemeaning(TWord *a, TWord *b);

        void __fastcall ActOnTerminate(TObject *Sender);
        void __fastcall Done();
    public:
        TCollectDataThread(byte acoll, TDoneProc aproc);
        virtual __fastcall void Execute();

        void Terminate();

        __property bool Ending = { read = fending };
        __property bool Finished = { read = ffinished };
        __property bool Completed = { read = fcompleted };

        //only call once Completed is true
        __property int Popular = { read = pop };
        __property int Medium = { read = med };
        __property int Nonfrequent = { read = non };
        __property int WordCount = { read = cnt };
        __property int MeaningCount = { read = mcnt };
    };

    friend int __thread_sort_proc(TfStats::TCollectDataThread *thread, TWord **a, TWord **b, bool &terminated);
    friend int __thread_sort_proc2(TfStats::TCollectDataThread *thread, TWord **a, TWord **b, bool &terminated);

    class TCollectDataThreadList : public TTemplateList<TCollectDataThread, false>
    {
    private:
        typedef TTemplateList<TCollectDataThread, false> inherited;

        bool terminating;
        TfStats *owner;

        void ThreadCompleted(byte coll);
    public:
        TCollectDataThreadList(TfStats *aowner);
        virtual ~TCollectDataThreadList();

        void Start(byte ix);
        bool Completed(byte ix);
        bool Terminate();
    };
    TCollectDataThreadList *threads;

    friend TfStats::TCollectDataThreadList;

    void SelectDictionary(int ix);
    void FillData(byte ix);
    void ClearData();
    void z_hide(TMessage &msg);
public: //User declarations
    __fastcall TfStats(TComponent* Owner);
    virtual __fastcall ~TfStats();

    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_Z_HIDE, TMessage, z_hide)
    END_MESSAGE_MAP(TBaseForm)

    bool ShowModalEx(int ix);
};
//---------------------------------------------------------------------------
extern PACKAGE TfStats *fStats;
//---------------------------------------------------------------------------
#endif
