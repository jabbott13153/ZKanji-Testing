//---------------------------------------------------------------------------

#ifndef kanatestH
#define kanatestH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include "edit2.h"

class TfKanaTest : public TBaseForm
{
__published: //IDE-managed Components
    TZPaintBox *pb;
    TBitBtn *BitBtn1;
    TBitBtn *btnGen;
    TBitBtn *btnSet;
    TBevel *bv1;
    TBevel *bv3;
    TLabel *Label1;
    TBevel *bv4;
    TLabel *lbType;
    TZPaintBox *pbType;
    TTimer *t;
    TLabel *lbDone;
    TLabel *lbMin;
    TTimer *tTime;
    TLabel *Label2;
    TLabel *lbSec;
    TLabel *lbError;
    TLabel *lbMis;
    void __fastcall btnGenClick(TObject *Sender);
    void __fastcall pbPaint(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall pbTypePaint(TObject *Sender);
    void __fastcall pbTypeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbTypeEnter(TObject *Sender);
    void __fastcall pbTypeExit(TObject *Sender);
    void __fastcall tTimer(TObject *Sender);
    void __fastcall tTimeTimer(TObject *Sender);
    void __fastcall pbTypeKeyPress(TObject *Sender, wchar_t &Key);

private: //User declarations
    static int testlen;

    TBmp *bmp; //bitmap for drawing the test area of pb

    int starttime;

    int cnt; //number of all kana
    int *gen; //index of all kana in wbuff to be tested
    bool *misses; //set to true if the tester made a mistake with the kana at the given position

    int testpos; //current character to be tested

    int spacew; //width of kana space
    int textw; //width of a double kana consonant
    int texth; //height of a kana text
    int fsize; //font size

    UnicodeString typed;
    byte mistakes; //mistakes with the current syllable
    int mistakecnt; //mistakes made in the whole test
    bool donegood; //show typed answer as either good (green) or wrong (red) for a second
    bool donewrong;

    int katacnt;
    int hiracnt;
    wchar_t **wbuff; //buffer holding all kana characters to be tested
    wchar_t **rbuff; //buffer of roomaji corresponding to kana in wbuff

    bool updatepb;
    void update();
public: //User declarations
    __fastcall TfKanaTest(TComponent* Owner);

    int ShowModalEx(const Bits &ahira, const Bits &akata);
};
//---------------------------------------------------------------------------
extern PACKAGE TfKanaTest *fKanaTest;
//---------------------------------------------------------------------------
#endif
