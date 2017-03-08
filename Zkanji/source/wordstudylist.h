//---------------------------------------------------------------------------

#ifndef wordstudylistH
#define wordstudylistH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include "baseform.h"
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Buttons.hpp>
#include <Menus.hpp>
#include "edit2.h"
//---------------------------------------------------------------------------

class TAlphaImageList;
class TStatObj;
class TStatColumns;
class TStatColumn;
class TKanjiEdit;
class TVirtualWordListObj;
class TWordCollection;
class TWordStudyList;
class TWordListObjBase;
class TWord;
class TKanji;
enum TWordColumnType;
enum TQuestionType;
enum THintType;

enum TChangeArrowType { atUp, atDown, atDot, atQuestion, atDotDefault, atDotSelected, atNone = 99 };

class TChangeArrows
{
private:
    static int usagecnt;
    static TAlphaImageList *images;

    byte get_width();
    byte get_height();
public:
    TChangeArrows();
    virtual ~TChangeArrows();

    void Draw(TCanvas *c, int x, int y, TChangeArrowType type);
    void Draw(TCanvas *c, int x, int y, TColor bk, TChangeArrowType type);

    __property byte Width = { read = get_width };
    __property byte Height = { read = get_height };
};

enum TWordStatPage { wspDeck, wspScore, wspAuto };
enum TGlobalStatPage { gspResults, gspItems, gspTime, gstpDecks, gspTries, gspAvg, gspAuto };

class TfStudyList : public TBaseForm
{
__published: //IDE-managed Components
    TPanel *pList;
    TBevel *b1;
    TTabControl *tTabs;
    TLabel *Label2;
    TZPaintBox *pbFilter;
    TBevel *b2;
    TBitBtn *BitBtn1;
    TPopupMenu *pmList;
    TSpeedButton *sbWordStats;
    TSpeedButton *sbList1;
    TPopupMenu *pmItems;
    TMenuItem *N11;
    TMenuItem *N21;
    TMenuItem *N31;
    TMenuItem *N41;
    TMenuItem *N51;
    TMenuItem *N61;
    TMenuItem *N81;
    TMenuItem *N91;
    TMenuItem *N101;
    TLabel *Label1;
    TSpeedButton *sbShowKanji;
    TSpeedButton *sbShowKana;
    TSpeedButton *sbShowMeaning;
    TBevel *Bevel3;
    TMenuItem *miPriority;
    TMenuItem *miAddItem;
    TMenuItem *miRemoveItem;
    TMenuItem *N1;
    TSpeedButton *sbList2;
    TButton *btnTest;
    TPanel *pWordStats;
    TZPaintBox *pbWordStats;
    TPanel *pWordList;
    TZPaintBox *pbList;
    TStatusBar *sbItems;
    TPanel *pMeaning;
    TBevel *Bevel4;
    TLabel *Label3;
    TSpeedButton *btnSave;
    TSpeedButton *btnReset;
    TEdit2 *edMeaning;
    TPanel *Panel2;
    TBevel *Bevel2;
    TSpeedButton *sbAddItem;
    TSpeedButton *sbRemoveItem;
    TMenuItem *miToQueue;
    TSpeedButton *sbToQueue;
    TPanel *pStats;
    TZPaintBox *pbGlobalStats;
    TBevel *bv7;
    TPanel *Panel1;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TLabel *lbIDueTitle;
    TLabel *Label11;
    TLabel *Label12;
    TLabel *Label13;
    TLabel *Label14;
    TLabel *Label15;
    TLabel *Label16;
    TLabel *Label17;
    TLabel *Label19;
    TSpeedButton *sbG1;
    TSpeedButton *sbG2;
    TSpeedButton *sbG3;
    TSpeedButton *sbG4;
    TSpeedButton *sbG5;
    TSpeedButton *sbG6;
    TBevel *bv6;
    TBevel *bv4;
    TBevel *bv5;
    TLabel *lbIWords;
    TLabel *lbIKanji;
    TLabel *lbIDue;
    TLabel *lbTFirst;
    TLabel *lbTLast;
    TLabel *lbTDays;
    TLabel *lbTSkipped;
    TLabel *lbTOverall;
    TLabel *lbTAverage;
    TLabel *lbTAAverage;
    TBevel *Bevel6;
    TBevel *Bevel7;
    TLabel *lbITotal;
    TLabel *Label10;
    TLabel *Label18;
    TLabel *Label20;
    TShape *splitWordStats;
    TBevel *bv3;
    TPanel *pWordStatButtons;
    TLabel *Label4;
    TSpeedButton *sbWordDeck;
    TSpeedButton *sbWordScore;
    TBevel *Bevel5;
    TBevel *Bevel8;
    TShape *sh1;
    TBitBtn *btnSettings;
    TBevel *Bevel1;
    TBevel *Bevel9;
    TMenuItem *miStudyOptions;
    TMenuItem *N2;
    TMenuItem *miDeckDec;
    TMenuItem *miDeckInc;
    TMenuItem *N3;
    TMenuItem *miResetData;
    TMenuItem *N4;
    TMenuItem *miToGroup;
    TSpeedButton *sbList3;
    TBevel *Bevel10;
    TImageList *ImageList1;
    TMenuItem *miHint;
    TMenuItem *miHintDefault;
    TMenuItem *miHintWritten;
    TMenuItem *miHintKana;
    TMenuItem *miHintMeaning;
    TMenuItem *N5;
    TPanel *pHack;
    TButton *btnHack;
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall tTabsChange(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall pbListMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbFreeClick(TObject *Sender);
    void __fastcall sbList1Click(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall pbListKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbFilterKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall N11Click(TObject *Sender);
    void __fastcall sbShowKanjiClick(TObject *Sender);
    void __fastcall sbAddItemClick(TObject *Sender);
    void __fastcall sbRemoveItemMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbRemoveItemClick(TObject *Sender);
    void __fastcall miAddItemClick(TObject *Sender);
    void __fastcall miRemoveItemClick(TObject *Sender);
    void __fastcall edMeaningChange(TObject *Sender);
    void __fastcall btnResetClick(TObject *Sender);
    void __fastcall btnSaveClick(TObject *Sender);
    void __fastcall sbWordStatsClick(TObject *Sender);
    void __fastcall splitWordStatsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall splitWordStatsMouseEnter(TObject *Sender);
    void __fastcall splitWordStatsMouseLeave(TObject *Sender);
    void __fastcall splitWordStatsMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall splitWordStatsMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbWordDeckClick(TObject *Sender);
    void __fastcall miToQueueClick(TObject *Sender);
    void __fastcall sbToQueueClick(TObject *Sender);
    void __fastcall btnTestClick(TObject *Sender);
    void __fastcall sbG1Click(TObject *Sender);
    void __fastcall btnSettingsClick(TObject *Sender);
    void __fastcall miDeckIncClick(TObject *Sender);
    void __fastcall miResetDataClick(TObject *Sender);
    void __fastcall miToGroupClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall miHintDefaultClick(TObject *Sender);
    void __fastcall btnHackClick(TObject *Sender);
    void __fastcall pbListKeyPress(TObject *Sender, wchar_t &Key);

private: //User declarations
    enum TPageType { ptList, ptStats, ptNone };
    enum TListType { ltFree, ltLocked, ltTested, ltNone = 99 };

    bool mousesizing;
    int mousesize;
    int mousey;
    int savesize;

    bool gstatinvalid;

    TStatObj *wsobj;
    TStatColumns *scols;

    TPageType fpagetype;
    TListType flisttype;

    TDateTime snapshotNow;

    TKanjiEdit *edFilter;
    TVirtualWordListObj *wobj;
    TWordCollection *fcollection;
    TWordStudyList *study;

    TIntegerList *words;

    Bits freecolvis;
    Bits lockedcolvis;
    Bits testedcolvis;

    TWordStatPage fwordpage;
    TGlobalStatPage fstatpage;

    TChangeArrows *arrows;

    int gindex; //index of word group that was last used when adding a word to it

    double incinterval; //new interval if we select deck+ in the popup menu for studied items
    double decinterval; //new interval if we select deck- in the popup menu for studied items

    void wcallback();
    void wpopup(int x, int y, TWordListObjBase *wobj, TWordColumnType wcc, TWord *w, TKanji *k);
    void ecallback();

    void generate_popup();
    void setpriority(int priority);
    void sethint(THintType hint);

    void showpage(TPageType apagetype);
    void showlist(TListType alisttype);

    void savesizes();

    void filter();
    void reorder();

    void removeitem();

    UnicodeString getheaderproc(int col);
    UnicodeString meaningproc(int row);
    void headerpressproc(int oldcol);
    void prepareproc(int col, int row, TWordColumnType ctype, TCanvas *c);
    void drawproc(int col, int row, TCanvas *c, const TRect &r);
    int indexproc(unsigned int ix);

    void count_wordstat(TWordStatPage page = wspAuto);
    void count_globstat(TGlobalStatPage page = gspAuto);

    int realrow(int row); //returns real index when the list is filtered

    bool allowchange();

    void repaint_panels();
    void splitsize(int size);

    void fillstats();

    UnicodeString numproc(int val);
    UnicodeString keyproc(int ix, int key);
    UnicodeString hintproc(int ix, const TStatColumn &column);

    void setcaption();

    void z_daychange(TMessage &msg);
    void settingsChanged(void *caller, TSettingsProcState state);

    void __fastcall colClick(TObject *Sender);
public: //User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_Z_DAYCHANGED, TMessage, z_daychange);
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfStudyList(TComponent* Owner);
    virtual __fastcall ~TfStudyList();

    int ShowModalEx(byte coll, bool aftertest);
};
//---------------------------------------------------------------------------
extern PACKAGE TfStudyList *fStudyList;
//---------------------------------------------------------------------------
#endif
