//---------------------------------------------------------------------------

#ifndef kanawritingtestH
#define kanawritingtestH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "baseform.h"
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

struct TKanjiElement;
class TKanjiPanelObj;
class TStrokeObj;
class TKanjiDrawObj;
enum TDrawStrokeState;

class TfKanaWriting : public TBaseForm
{
__published: //IDE-managed Components
    TLabel *Label1;
    TBevel *bv1;
    TLabel *lbType;
    TLabel *lbQuestion;
    TZPaintBox *pb;
    TBevel *bv3;
    TBevel *bv4;
    TBitBtn *btnSet;
    TBitBtn *BitBtn1;
    TLabel *lbMis;
    TLabel *lbSec;
    TLabel *lbMin;
    TLabel *Label3;
    TSpeedButton *btnUndo;
    TSpeedButton *btnReset;
    TPanel *pResults;
    TBevel *Bevel22;
    TSpeedButton *btnResBack;
    TSpeedButton *btnResFore;
    TZPaintBox *pbResults;
    TLabel *lbAnswer;
    TZPaintBox *pbAnswer;
    TLabel *Label5;
    TLabel *lbTested;
    TBevel *bvl;
    TBevel *bvr;
    TBitBtn *btnGen;
    TTimer *tTime;
    TTimer *t;
    TLabel *lbPrevQ;
    TLabel *lbPrevA;
    TSpeedButton *btnShowAnswer;
    TSpeedButton *btnStopAnswer;
    TZPaintBox *pbK;
    TTimer *tK;
    void __fastcall btnGenClick(TObject *Sender);
    void __fastcall btnResBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnResForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pbResultsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnUndoClick(TObject *Sender);
    void __fastcall btnResetClick(TObject *Sender);
    void __fastcall pbAnswerPaint(TObject *Sender);
    void __fastcall tTimeTimer(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall tTimer(TObject *Sender);
    void __fastcall btnShowAnswerClick(TObject *Sender);
    void __fastcall btnStopAnswerClick(TObject *Sender);
    void __fastcall tKTimer(TObject *Sender);
private: //User declarations
    typedef TBaseForm inherited;

    int cnt;

    int starttime;

    int mistakecnt; //mistakes total
    byte mistakes; //mistakes with the current character

    bool donegood; //show typed answer as either good (green) or wrong (red) for a second
    bool donewrong;

    bool typing;

    TKanjiDrawObj *kdraw;
    bool playing;

    void kproc(TDrawStrokeState state);
    int kpos;

    int pos;

    TIntegerList *testlist;
    TKanjiPanelObj *poresults;
    TStrokeObj *sobj;

    void pcallback();

    UnicodeString answer;

    int katacnt;
    int hiracnt;
    wchar_t **wbuff; //buffer holding all kana characters to be tested
    wchar_t **rbuff; //buffer of roomaji corresponding to kana in wbuff

    void rcolor(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected, TColor &background, TColor &fontcolor);
    void rdraw(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected);
    void scallback();

    void set_buttons();
    void add_char(wchar_t kana);
    void step();
public: //User declarations
    __fastcall TfKanaWriting(TComponent* Owner);

    int ShowModalEx(const Bits &ahira, const Bits &akata);
};
//---------------------------------------------------------------------------
extern PACKAGE TfKanaWriting *fKanaWriting;
//---------------------------------------------------------------------------
#endif
