//---------------------------------------------------------------------------

#ifndef sitemanagerH
#define sitemanagerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "ZPaintBox.h"
//---------------------------------------------------------------------------
#include "baseform.h"
#include "templatelist.h"
#include "wordgrid.h"
#include "edit2.h"

struct TSite
{
    UnicodeString name; //custom name for site to show in menus
    UnicodeString address; //web address
    int pos; //position of inserted word to look up
};

class TSiteList : public TTemplateList<TSite, true>
{
private:
    typedef TTemplateList<TSite, true> inherited;
protected:
    virtual void CopyElem(TSite *dest, TSite *src);
public:
    virtual ~TSiteList()
    {
        Clear();
    }
    void Update(int index, UnicodeString name, UnicodeString address, int pos);
    int Add(UnicodeString name, UnicodeString address, int pos);
    int IndexOf(UnicodeString name);
};

typedef void(__closure * TSimpleListHeaderType)(unsigned int col, bool &ownerdraw);
typedef UnicodeString(__closure *TSimpleListGetHeaderText)(unsigned int col);
typedef void(__closure * TSimpleListDrawHeader)(unsigned int col, TCanvas * c, TRect r);
typedef void(__closure * TSimpleListCellType)(unsigned int col, unsigned int row, bool &ownerdraw);
typedef UnicodeString(__closure *TSimpleListGetCellText)(unsigned int col, unsigned int row);
typedef void(__closure * TSimpleListDrawCell)(unsigned int col, unsigned int row, TCanvas * c, TRect r);
class TSimpleWordList : public TWordListBase
{
private:
    typedef TWordListBase inherited;
protected:
    virtual void DoDrawHead(int col, TRect r);
    virtual void DoDrawCell(int col, int row, TRect r);
    virtual void update_count(int &count);
    virtual void DoPrepareDraw(int col, int row);

    TSimpleListHeaderType fhtype;
    TSimpleListGetHeaderText fhtext;
    TSimpleListDrawHeader fhdraw;
    TSimpleListCellType fctype;
    TSimpleListGetCellText fctext;
    TSimpleListDrawCell fcdraw;
public:
    TSimpleWordList(TZPaintBox *abox, int arowheight, bool aheader, TSimpleListHeaderType ahtype, TSimpleListGetHeaderText ahtext, TSimpleListDrawHeader ahdraw, TSimpleListCellType actype, TSimpleListGetCellText actext, TSimpleListDrawCell acdraw, TFuncPtr afunc);
    TSimpleWordList(TZPaintBox *abox, int arowheight, bool aheader, TSimpleListGetHeaderText ahtext, TSimpleListGetCellText actext, TFuncPtr afunc);
};

class TfSites : public TBaseForm
{
__published: //IDE-managed Components
    TEdit2 *edName;
    TEdit2 *edAddress;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TButton *btnAdd;
    TSpeedButton *btnUp;
    TSpeedButton *btnDown;
    TLabel *lb5;
    TLabel *Label7;
    TLabel *lbCursor;
    TButton *btnRemove;
    TButton *btnTest;
    TPanel *Panel2;
    TButton *btnOk;
    TButton *btnCancel;
    TBevel *b2;
    TBevel *b1;
    TZPaintBox *pbList;
    TButton *btnNew;
    TBevel *b3;
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall edNameChange(TObject *Sender);
    void __fastcall btnAddClick(TObject *Sender);
    void __fastcall btnOkClick(TObject *Sender);
    void __fastcall btnRemoveClick(TObject *Sender);
    void __fastcall btnUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnDownMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnNewClick(TObject *Sender);
    void __fastcall btnTestClick(TObject *Sender);
private: //User declarations
    typedef TBaseForm inherited;

    TSiteList *tmplist;
    TSimpleWordList *list;

    void headertype(unsigned int col, bool &ownerdraw);
    void drawheader(unsigned int col, TCanvas *c, TRect r);
    UnicodeString getheadertext(unsigned int col);
    void celltype(unsigned int col, unsigned int row, bool &ownerdraw);
    void drawcell(unsigned int col, unsigned int row, TCanvas *c, TRect r);
    UnicodeString getcelltext(unsigned int col, unsigned int row);
    void lcallback();

public: //User declarations
    __fastcall TfSites(TComponent* Owner);
    virtual __fastcall ~TfSites();
};
//---------------------------------------------------------------------------
extern PACKAGE TfSites *fSites;
//---------------------------------------------------------------------------

void LaunchSiteSearch(UnicodeString str, int siteindex, TSiteList *list = NULL);

extern TSiteList *sitelist;
#endif
