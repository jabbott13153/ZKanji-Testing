//---------------------------------------------------------------------------

#ifndef studyreadingH
#define studyreadingH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include <stdio.h>
#include "baseform.h"
#include <ComCtrls.hpp>

class TVirtualWordListObj;
class TKanjiDrawObj;

class TKanjiStudyList;
class TWordCollection;
class TWord;
class TKanjiEdit;
class TStudyList;
struct TConvertItem;
class TWordStudyList;
enum TWordColumnType;

/*TKanjiStudy - Only used for compatibility with earlier long-term study lists. Should be removed after 2011 July*/
struct TKanjiStudy
{
    int kanjiindex;
    byte reading;

    bool correct;
    int tries;

    struct TItem
    {
        int wordindex;
        byte charpos;
        char *meaning;
    };
    class TItems : public TTemplateList<TItem, true>
    {
    private:
        typedef TTemplateList<TItem, true> inherited;

        void FreeItem(TItem *item, int ix);
    public:
        virtual ~TItems()
        {
            Clear();
        }

        int Add(int wordindex, char *meaning, byte kpos);

        void SaveToFile(FILE *f);
        void LoadFromFile(FILE *f, int version, TConvertItem **converts, int convertcount);
    };
    TItems *words;
};
class TKanjiStudyList : public TTemplateList<TKanjiStudy, true>
{
private:
    typedef TTemplateList<TKanjiStudy, true> inherited;

    int fcurrent;
    void FreeItem(TKanjiStudy *item, int ix);
    int IndexOf(int kanjiindex, byte reading, int wordindex, bool &hasword);
public:
    TKanjiStudyList();
    virtual ~TKanjiStudyList()
    {
        Clear();
    }

    void SaveToFile(FILE *f);
    void LoadFromFile(FILE *f, int version, TConvertItem **converts, int convertcount);

    int Add(TWord *w, char *meaning, int wordindex, int kpos);
    void RemoveLine(int windex);
    void Sort();
    void Next();
    void Prev();

    __property int current = { read = fcurrent };
};
/*------- end of remove ----------------------------------------------*/

class TZtringList
{
private:
    class TWList : public TTemplateList<wchar_t, true>
    {
    private:
        virtual void FreeItem(wchar_t *item, int ix);
    public:
        virtual ~TWList()
        {
            Clear();
        }
    };
    TWList *flist;
    int get_count()
    {
        return flist->Count;
    }
    UnicodeString get_item(int ix)
    {
        return flist->Items[ix];
    }
public:
    TZtringList();
    virtual ~TZtringList();

    void Clear()
    {
        flist->Clear();
    }
    int Add(UnicodeString str);

    __property UnicodeString Items[int ix] = { read = get_item };
    __property int Count = { read = get_count };
};

class TfKStudy : public TBaseForm
{
__published: //IDE-managed Components
    TPanel *Panel1;
    TZPaintBox *pbKanji;
    TZPaintBox *pbTries;
    TBevel *Bevel1;
    TPanel *pButtons;
    TSpeedButton *btnSuspend;
    TBevel *b0;
    TPanel *Panel3;
    TPanel *pAnswer;
    TSpeedButton *btnAccept;
    TZPaintBox *edKanji;
    TLabel *Label2;
    TPanel *pAgain;
    TSpeedButton *btnAgain;
    TLabel *Label3;
    TPanel *pNext;
    TSpeedButton *btnNext;
    TLabel *lbNResult;
    TPanel *pFinished;
    TSpeedButton *btnFinished;
    TLabel *lbFResult;
    TStatusBar *sbInfo;
    TZPaintBox *pbWords;
    TBevel *b1;
    void __fastcall btnAcceptClick(TObject *Sender);
    void __fastcall btnNextClick(TObject *Sender);
    void __fastcall btnAgainClick(TObject *Sender);
    void __fastcall btnFinishedClick(TObject *Sender);
    void __fastcall pbTriesPaint(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall btnSuspendClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall pbWordsKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall edKanjiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);

private: //User declarations
    TWordCollection *coll;

    TVirtualWordListObj *wobj;
    TKanjiDrawObj *kobj;
    TKanjiEdit *edkanji;

    int itemsleft;

    TWordStudyList *study;
    TZtringList *ftries;

    bool ffull;

    void updateinfo();
    UnicodeString meaningproc(int ix);
    void prepareproc(int col, int row, TWordColumnType ctype, TCanvas *c);
    void drawproc(int col, int row, TCanvas *c, const TRect &r);
    UnicodeString currentreading();
    int indexproc(unsigned int ix);
    void edfunc();
    void WMSizing(TWMSizing &msg);
public: //User declarations
    __fastcall TfKStudy(TComponent* Owner);
    virtual __fastcall ~TfKStudy();

    int ShowModalEx(TWordStudyList *astudy);
};
//---------------------------------------------------------------------------
extern PACKAGE TfKStudy *fKStudy;
//---------------------------------------------------------------------------
#endif
