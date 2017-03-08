//---------------------------------------------------------------------------

#ifndef kanapracticeH
#define kanapracticeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include <ComCtrls.hpp>

class TfKana : public TBaseForm
{
__published: //IDE-managed Components
    TBevel *bv1;
    TBitBtn *btnRStart;
    TBitBtn *BitBtn2;
    TBevel *bv2;
    TLabel *Label1;
    TTabControl *tcKana;
    TCheckBox *CheckBox105;
    TCheckBox *CheckBox106;
    TCheckBox *CheckBox107;
    TCheckBox *CheckBox108;
    TCheckBox *CheckBox109;
    TCheckBox *CheckBox110;
    TCheckBox *CheckBox111;
    TCheckBox *CheckBox112;
    TCheckBox *CheckBox113;
    TCheckBox *CheckBox114;
    TCheckBox *CheckBox115;
    TCheckBox *CheckBox116;
    TCheckBox *CheckBox117;
    TCheckBox *CheckBox118;
    TCheckBox *CheckBox119;
    TCheckBox *CheckBox120;
    TCheckBox *CheckBox121;
    TCheckBox *CheckBox122;
    TCheckBox *CheckBox123;
    TCheckBox *CheckBox124;
    TCheckBox *CheckBox125;
    TCheckBox *CheckBox126;
    TCheckBox *CheckBox127;
    TCheckBox *CheckBox128;
    TCheckBox *CheckBox129;
    TCheckBox *CheckBox130;
    TCheckBox *CheckBox131;
    TCheckBox *CheckBox132;
    TCheckBox *CheckBox133;
    TCheckBox *CheckBox134;
    TCheckBox *CheckBox135;
    TCheckBox *CheckBox136;
    TCheckBox *CheckBox137;
    TCheckBox *CheckBox138;
    TCheckBox *CheckBox139;
    TCheckBox *CheckBox140;
    TCheckBox *CheckBox141;
    TCheckBox *CheckBox142;
    TCheckBox *CheckBox143;
    TCheckBox *CheckBox144;
    TCheckBox *CheckBox145;
    TCheckBox *CheckBox146;
    TCheckBox *CheckBox147;
    TCheckBox *CheckBox148;
    TCheckBox *CheckBox149;
    TCheckBox *CheckBox150;
    TCheckBox *CheckBox151;
    TCheckBox *CheckBox152;
    TCheckBox *CheckBox153;
    TCheckBox *CheckBox154;
    TCheckBox *CheckBox155;
    TCheckBox *CheckBox156;
    TCheckBox *CheckBox157;
    TCheckBox *CheckBox158;
    TCheckBox *CheckBox159;
    TCheckBox *CheckBox160;
    TCheckBox *CheckBox161;
    TCheckBox *CheckBox162;
    TCheckBox *CheckBox163;
    TCheckBox *CheckBox164;
    TCheckBox *CheckBox165;
    TCheckBox *CheckBox166;
    TCheckBox *CheckBox167;
    TCheckBox *CheckBox168;
    TCheckBox *CheckBox169;
    TCheckBox *CheckBox170;
    TCheckBox *CheckBox171;
    TCheckBox *CheckBox172;
    TCheckBox *CheckBox173;
    TCheckBox *CheckBox174;
    TCheckBox *CheckBox175;
    TCheckBox *CheckBox176;
    TCheckBox *CheckBox177;
    TCheckBox *CheckBox178;
    TCheckBox *CheckBox179;
    TCheckBox *CheckBox180;
    TCheckBox *CheckBox181;
    TCheckBox *CheckBox182;
    TCheckBox *CheckBox183;
    TCheckBox *CheckBox184;
    TCheckBox *CheckBox185;
    TCheckBox *CheckBox186;
    TCheckBox *CheckBox187;
    TCheckBox *CheckBox188;
    TCheckBox *CheckBox189;
    TCheckBox *CheckBox190;
    TCheckBox *CheckBox191;
    TCheckBox *CheckBox192;
    TCheckBox *CheckBox193;
    TCheckBox *CheckBox194;
    TCheckBox *CheckBox195;
    TCheckBox *CheckBox196;
    TCheckBox *CheckBox197;
    TCheckBox *CheckBox198;
    TCheckBox *CheckBox199;
    TCheckBox *CheckBox200;
    TCheckBox *CheckBox201;
    TCheckBox *CheckBox202;
    TCheckBox *CheckBox203;
    TCheckBox *CheckBox204;
    TCheckBox *CheckBox205;
    TCheckBox *CheckBox206;
    TCheckBox *CheckBox207;
    TCheckBox *CheckBox208;
    TLabel *Label50;
    TLabel *Label51;
    TLabel *Label52;
    TLabel *Label53;
    TLabel *Label54;
    TLabel *Label55;
    TLabel *Label56;
    TLabel *Label57;
    TLabel *Label58;
    TLabel *Label59;
    TLabel *Label60;
    TLabel *Label61;
    TLabel *Label62;
    TLabel *Label63;
    TLabel *Label64;
    TLabel *Label65;
    TLabel *Label66;
    TLabel *Label67;
    TLabel *Label68;
    TLabel *Label69;
    TLabel *Label70;
    TLabel *Label71;
    TLabel *Label72;
    TLabel *Label73;
    TLabel *Label74;
    TLabel *Label75;
    TLabel *Label76;
    TLabel *Label77;
    TLabel *Label78;
    TLabel *Label79;
    TLabel *Label80;
    TLabel *Label81;
    TLabel *Label82;
    TLabel *Label83;
    TLabel *Label84;
    TLabel *Label85;
    TLabel *Label86;
    TLabel *Label87;
    TLabel *Label88;
    TLabel *Label89;
    TLabel *Label90;
    TLabel *Label91;
    TLabel *lbVowels;
    TLabel *Label93;
    TLabel *Label94;
    TLabel *Label95;
    TLabel *Label96;
    TLabel *Label97;
    TSpeedButton *sbMinus;
    TSpeedButton *sbPlus;
    TLabel *lbInstruction;
    TBitBtn *btnWStart;
    TLabel *lbPage;
    void __fastcall CheckBox1Click(TObject *Sender);
    void __fastcall sbPlusClick(TObject *Sender);
    void __fastcall CheckBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall tcKanaChange(TObject *Sender);
private: //User declarations
    Bits hira;
    Bits kata;

    Bits *curr;
    int cnt;

    int GetPos(UnicodeString s);
public: //User declarations
    __fastcall TfKana(TComponent* Owner);
    int ShowModalEx();
};
//---------------------------------------------------------------------------
extern PACKAGE TfKana *fKana;
extern wchar_t *hints[104];
//---------------------------------------------------------------------------
#endif
