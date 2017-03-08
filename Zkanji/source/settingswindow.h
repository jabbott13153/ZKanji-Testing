//---------------------------------------------------------------------------

#ifndef settingswindowH
#define settingswindowH
//---------------------------------------------------------------------------
#include <Buttons.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
#include <Dialogs.hpp>
#include "ZPaintBox.h"
#include "edit2.h"
//---------------------------------------------------------------------------

#include "settings.h"
#include "baseform.h"
#include <CheckLst.hpp>

class TfSettings : public TBaseForm
{
__published: //IDE-managed Components
    TPageControl *pages;
    TTabSheet *tabDictionary;
    TTabSheet *tabFonts;
    TComboBox *cbKanji;
    TComboBox *cbKana;
    TComboBox *cbRomaji;
    TLabel *Label5;
    TLabel *Label6;
    TZPaintBox *pbKanji;
    TComboBox *cbSmallRomaji;
    TLabel *Label7;
    TBevel *Bevel5;
    TComboBox *cbFontSize;
    TLabel *lbDSize;
    TLabel *lbPopSize;
    TComboBox *cbPopFontSize;
    TPanel *Panel2;
    TZPaintBox *pbText1;
    TTabSheet *tabPrint;
    TLabel *Label19;
    TLabel *Label20;
    TComboBox *cbVKana;
    TComboBox *cbVRomaji;
    TCheckBox *cbVDictFonts;
    TZPaintBox *pbVText;
    TBevel *Bevel8;
    TLabel *Label21;
    TComboBox *cbVHeight;
    TLabel *Label22;
    TComboBox *cbVLeft;
    TCheckBox *cbVDouble;
    TComboBox *cbVRight;
    TLabel *Label24;
    TCheckBox *cbVSeparate;
    TLabel *Label25;
    TCheckBox *cbVFurigana;
    TTabSheet *tabStudy;
    TTabSheet *tabColors;
    TColorBox *cbBg;
    TColorBox *cbHl;
    TLabel *Label45;
    TLabel *Label46;
    TLabel *Label48;
    TColorBox *cbGrid;
    TLabel *Label49;
    TColorBox *cbText;
    TLabel *Label51;
    TLabel *Label52;
    TColorBox *cbNoWd;
    TColorBox *cbUns;
    TLabel *Label54;
    TLabel *Label55;
    TLabel *Label56;
    TColorBox *cbTp;
    TColorBox *cbUsg;
    TColorBox *cbFld;
    TBitBtn *btnRestore;
    TLabel *Label57;
    TColorBox *cbInfCon;
    TTabSheet *tabGlobal;
    TCheckBox *cbShowInflectionText;
    TCheckBox *cbShowInflectionAlways;
    TLabel *Label17;
    TComboBox *cbABC;
    TCheckBox *cbAutosize;
    TCheckBox *cbPositionRestore;
    TCheckBox *cbSearchSave;
    TCheckBox *cbKanjiRestore;
    TCheckBox *cbStartInTray;
    TCheckBox *cbColumnRestore;
    TCheckBox *cbRememberTray;
    TLabel *Label60;
    TColorDialog *dColor;
    TLabel *Label64;
    TComboBox *cbFreq;
    TCheckBox *cbKanjiAntialias;
    TLabel *Label13;
    TComboBox *cbInfo;
    TLabel *Label14;
    TCheckBox *cbKanjiHint;
    TCheckBox *cbKanjiHintTime;
    TEdit2 *edKanjiHintTime;
    TLabel *Label2;
    TShape *Shape1;
    TLabel *Label59;
    TLabel *Label66;
    TShape *Shape2;
    TPanel *Panel1;
    TCheckBox *cbAutoStart;
    TLabel *lbAutoStart;
    TLabel *Label58;
    TShape *Shape3;
    TLabel *Label68;
    TCheckBox *cbDictKey;
    TShape *Shape4;
    TShape *Shape5;
    TShape *Shape6;
    TLabel *Label69;
    TPanel *pJShortcut;
    TComboBox *cbJKeys;
    TLabel *Label10;
    TCheckBox *cbJShift;
    TLabel *Label9;
    TComboBox *cbJChar;
    TLabel *Label70;
    TPanel *pEShortcut;
    TLabel *Label74;
    TLabel *Label12;
    TCheckBox *cbEShift;
    TComboBox *cbEKeys;
    TComboBox *cbEChar;
    TLabel *Label11;
    TPanel *pKShortcut;
    TLabel *Label72;
    TLabel *Label73;
    TLabel *Label75;
    TCheckBox *cbKShift;
    TComboBox *cbKKeys;
    TComboBox *cbKChar;
    TLabel *Label8;
    TShape *Shape7;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label26;
    TShape *Shape8;
    TLabel *Label27;
    TLabel *Label47;
    TLabel *Label53;
    TShape *Shape9;
    TShape *Shape10;
    TLabel *Label50;
    TLabel *Label29;
    TLabel *Label30;
    TEdit2 *edDayStart;
    TLabel *Label41;
    TComboBox *cbIncludeInterval;
    TLabel *Label36;
    TComboBox *cbWarnTime;
    TLabel *Label40;
    TLabel *lbKWarnThres;
    TEdit2 *edKWarnThres;
    TLabel *Label65;
    TComboBox *cbRepetitionInterval;
    TEdit2 *edKWarnItems;
    TCheckBox *cbKWarnItems;
    TLabel *Label35;
    TLabel *Label76;
    TShape *Shape11;
    TLabel *Label77;
    TLabel *Label18;
    TLabel *Label78;
    TLabel *Label1;
    TCheckBox *cbPopupPositionRestore;
    TCheckBox *cbPopupDisappear;
    TCheckBox *cbPopupColumnRestore;
    TCheckBox *cbPopupUseClipboard;
    TCheckBox *cbKanjiReset;
    TShape *Shape12;
    TShape *Shape13;
    TShape *Shape14;
    TLabel *Label23;
    TCheckBox *cbVShade;
    TCheckBox *cbVSep;
    TCheckBox *cbVNumber;
    TPanel *Panel3;
    TZPaintBox *pbText2;
    TTabSheet *tabTools;
    TLabel *Label79;
    TShape *Shape15;
    TLabel *Label44;
    TCheckBox *cbHWRememberSize;
    TCheckBox *cbHWRememberPosition;
    TLabel *Label80;
    TButton *btnManageSites;
    TLabel *Label81;
    TLabel *lbSite1;
    TLabel *lbSiteCnt;
    TLabel *lbSite2;
    TShape *Shape16;
    TPanel *Panel4;
    TCheckBox *cbCheckUpdate;
    TComboBox *cbCheckInterval;
    TLabel *Label82;
    TCheckBox *cbInGroup;
    TCheckBox *cbLTOpenTab;
    TCheckBox *cbLTRestoreSizes;
    TEdit2 *edKItemInclude;
    TLabel *Label37;
    TCheckBox *cbKWarnAfter;
    TEdit2 *edKWarnAfter;
    TCheckBox *cbKTypeKanji;
    TCheckBox *cbKTypeKana;
    TCheckBox *cbKTypeMeaning;
    TBevel *bvBottom;
    TButton *btnOk;
    TButton *btnCancel;
    TLabel *Label39;
    TCheckListBox *cbKRef;
    TLabel *Label89;
    TSpeedButton *sbKRefUp;
    TSpeedButton *sbKRefDown;
    TSpeedButton *sbKRefAll;
    TSpeedButton *sbKRefNone;
    TCheckBox *cbWinDecor;
    TLabel *Label90;
    TLabel *Label15;
    TCheckBox *cbShowClipboardCopy;
    TCheckBox *cbShowClipboardAppend;
    TButton *btnKRef;
    TComboBox *cbStatDate;
    TLabel *Label42;
    TCheckBox *cbLabelInterval;
    TLabel *Label91;
    TLabel *Label83;
    TPanel *Panel11;
    TPanel *Panel5;
    TLabel *Label86;
    TPanel *Panel6;
    TLabel *Label87;
    TPanel *Panel7;
    TLabel *Label88;
    TPanel *Panel8;
    TRadioButton *rbKanjiKana;
    TRadioButton *rbKanjiMeaning;
    TPanel *Panel9;
    TRadioButton *rbKanaKanji;
    TRadioButton *rbKanaMeaning;
    TLabel *Label92;
    TLabel *Label85;
    TPanel *Panel10;
    TRadioButton *rbMeaningKanji;
    TRadioButton *rbMeaningKana;
    TBevel *Bevel1;
    TBevel *Bevel2;
    TLabel *Label93;
    TComboBox *cbSuspendAfter;
    TLabel *Label94;
    TCheckBox *cbKNewFirst;
    TCheckBox *cbLTShowLast;
    TLabel *Label43;
    TLabel *Label95;
    TComboBox *cbKReadings;
    TLabel *Label96;
    TComboBox *cbKReadingWords;
    TLabel *Label97;
    TLabel *Label98;
    TLabel *Label99;
    TEdit2 *edKRetryLimit;
    TEdit2 *edKHardLimit;
    TTrackBar *tbTransparency;
    TLabel *Label100;
    TPanel *Panel13;
    TLabel *Label61;
    TZPaintBox *pbFreq1;
    TBitBtn *btnFreq1;
    TPanel *Panel14;
    TLabel *Label101;
    TZPaintBox *pbFreq2;
    TBitBtn *btnFreq2;
    TPanel *Panel15;
    TLabel *Label62;
    TZPaintBox *pbFreq3;
    TBitBtn *btnFreq3;
    TLabel *Label63;
    TColorBox *cbKnO;
    TColorBox *cbN1;
    TColorBox *cbN2;
    TColorBox *cbN3;
    TColorBox *cbN4;
    TColorBox *cbN5;
    TLabel *Label67;
    TLabel *Label102;
    TLabel *Label103;
    TLabel *Label104;
    TLabel *Label105;
    TLabel *Label106;
    TCheckBox *cbInJLPT;
    TCheckBox *cbShowNonKanji;
    TCheckBox *cbOldMenu;
    TCheckBox *cbShadow;
    TCheckBox *cbMinTray;
    TTabSheet *tabData;
    TShape *Shape17;
    TLabel *Label31;
    TEdit2 *edAutoSave;
    TCheckBox *cbAutoSave;
    TLabel *Label16;
    TLabel *Label28;
    TLabel *Label32;
    TCheckBox *cbBckCreate;
    TEdit2 *edBckCount;
    TEdit2 *edBckSpacing;
    TLabel *Label71;
    TShape *Shape18;
    TPanel *Panel12;
    TEdit2 *edBckPath;
    TButton *btnBckBrowse;
    TLabel *Label109;
    TLabel *Label110;
    TLabel *Label111;
    TLabel *Label112;
    TButton *btnBckReset;
    TLabel *Label114;
    TLabel *Label34;
    TLabel *Label33;
    TLabel *lbBck;
    TLabel *Label38;
    TLabel *Label84;
    TEdit2 *edHistCnt;
    TCheckBox *cbTimedHistSave;
    TEdit2 *edHistDelay;
    void __fastcall pbKanjiPaint(TObject *Sender);
    void __fastcall cbKanjiChange(TObject *Sender);
    void __fastcall cbKanaChange(TObject *Sender);
    void __fastcall pbText1Paint(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall cbDictKeyClick(TObject *Sender);
    void __fastcall cbAutoSaveClick(TObject *Sender);
    void __fastcall cbStartInTrayClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall cbVDictFontsClick(TObject *Sender);
    void __fastcall pbVTextPaint(TObject *Sender);
    void __fastcall cbVKanaChange(TObject *Sender);
    void __fastcall cbVRightChange(TObject *Sender);
    void __fastcall cbKanjiHintTimeClick(TObject *Sender);
    void __fastcall cbKanjiHintClick(TObject *Sender);
    void __fastcall cbKWarnItemsClick(TObject *Sender);
    void __fastcall cbWarnTimeChange(TObject *Sender);
    void __fastcall btnRestoreClick(TObject *Sender);
    void __fastcall cbFreqDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
    void __fastcall pbFreq1Paint(TObject *Sender);
    void __fastcall cbBgChange(TObject *Sender);
    void __fastcall btnFreq1Click(TObject *Sender);
    void __fastcall dColorShow(TObject *Sender);
    void __fastcall dColorClose(TObject *Sender);
    void __fastcall cbKanjiAntialiasClick(TObject *Sender);
    void __fastcall btnManageSitesClick(TObject *Sender);
    void __fastcall cbKWarnAfterClick(TObject *Sender);
    void __fastcall cbKRefClick(TObject *Sender);
    void __fastcall sbKRefUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbKRefAllClick(TObject *Sender);
    void __fastcall btnKRefClick(TObject *Sender);
    void __fastcall cbKReadingsChange(TObject *Sender);
    void __fastcall btnBckResetClick(TObject *Sender);
    void __fastcall cbBckCreateClick(TObject *Sender);
    void __fastcall edBckPathExit(TObject *Sender);
    void __fastcall btnBckBrowseClick(TObject *Sender);
private: //User declarations
    TSettings fsettings;

    bool olddecor; //save window decoration setting so we can send a themechanged message to fMain

    HFONT Kfont;

    TNumEdit *numAutoSave;
    TNumEdit *numKanjiHintTime;

    TNumEdit *numKWarnItems;
    TNumEdit *numKWarnAfter;
    TNumEdit *numKItemInclude;

    TNumEdit *numKWarnThres;
    TNumEdit *numDayStart;

    TNumEdit *numKRetryLimit;
    TNumEdit *numKHardLimit;

    TNumEdit *numBckCount;
    TNumEdit *numBckSpacing;

    TNumEdit *numHistCnt;
    TNumEdit *numHistDelay;

    TColor freq1;
    TColor freq2;
    TColor freq3;

    void __fastcall SetSiteText();
    void __fastcall FillKRef();
public: //User declarations
    __fastcall TfSettings(TComponent* Owner);
    virtual __fastcall ~TfSettings();
};
//---------------------------------------------------------------------------
extern PACKAGE TfSettings *fSettings;
//---------------------------------------------------------------------------

//load and save ini file settings
void __fastcall LoadSettings();
void __fastcall SaveSettings();
//load UI settings separately after all user data is loaded
void __fastcall LoadUISettings();

#endif
