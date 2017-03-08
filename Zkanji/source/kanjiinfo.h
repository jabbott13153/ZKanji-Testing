//---------------------------------------------------------------------------

#ifndef kanjiinfoH
#define kanjiinfoH
//---------------------------------------------------------------------------
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------

#include "baseform.h"
#include <ComCtrls.hpp>
#include <ActnList.hpp>

class TMouseOvers;
class TKanjiDrawObj;
class TKanjiPanelObj;
class TDropDownList;
struct TKanji;
struct TKanjiElement;
enum TDrawStrokeState;
class TCloseButton;
enum TDrawSpeed : byte;

enum TInfoOption { ioNone, ioRef, ioSimilar, ioParts, ioParents, ioEnlarge, ioShrink };
enum TKanjiDataItem;

class TfInfo : public TBaseForm
{
__published: //IDE-managed Components
    TZPaintBox *pbReadings;
    TZPaintBox *pbKanji;
    TBevel *Bevel19;
    TPanel *pSimilar;
    TBevel *Bevel22;
    TZPaintBox *pbSimilar;
    TSpeedButton *btnSimilarBack;
    TSpeedButton *btnSimilarFore;
    TPopupMenu *pmHide;
    TMenuItem *miHide;
    TPanel *pOrder;
    TBevel *Bevel2;
    TLabel *Label3;
    TSpeedButton *sbSpeed1;
    TSpeedButton *sbSpeed2;
    TSpeedButton *sbSpeed3;
    TPanel *Panel1;
    TSpeedButton *sbFF;
    TSpeedButton *sbFore;
    TLabel *lbStroke;
    TSpeedButton *sbBack;
    TSpeedButton *sbRew;
    TBevel *Bevel3;
    TSpeedButton *sbPause;
    TSpeedButton *sbPlay;
    TSpeedButton *sbStop;
    TLabel *lbSimilar;
    TPanel *pParts;
    TBevel *Bevel5;
    TSpeedButton *btnPartsBack;
    TSpeedButton *btnPartsFore;
    TLabel *lbParts;
    TZPaintBox *pbParts;
    TPanel *pParents;
    TBevel *Bevel6;
    TSpeedButton *btnParentsBack;
    TSpeedButton *btnParentsFore;
    TLabel *lbParents;
    TZPaintBox *pbParents;
    TSpeedButton *sbShadow;
    TSpeedButton *sbNumbers;
    TBevel *Bevel7;
    TImageList *ilMenu;
    TSpeedButton *sbDirection;
    TMenuItem *N2;
    TMenuItem *miCopy;
    TMenuItem *miAppend;
    TPanel *pButtons;
    TSpeedButton *btnBack;
    TSpeedButton *btnHistory;
    TBevel *Bevel8;
    TSpeedButton *btnFind;
    TBevel *Bevel4;
    TPopupMenu *pmRef;
    TMenuItem *miCopyRef;
    TMenuItem *miAppendRef;
    TBevel *b0;
    TSpeedButton *sbRef;
    TSpeedButton *sbOrder;
    TSpeedButton *sbSimilar;
    TSpeedButton *sbParts;
    TSpeedButton *sbParents;
    TMenuItem *N3;
    TMenuItem *miHideRef;
    TMenuItem *N4;
    TMenuItem *miRefView;
    TMenuItem *miRefAll;
    TMenuItem *N6;
    TPanel *pSize;
    TSpeedButton *sbCloseKanji;
    TSpeedButton *sbOpenKanji;
    TPanel *pTitle;
    TBevel *Bevel12;
    TBevel *Bevel9;
    TLabel *Label1;
    TCheckBox *cbGrid;
    TBevel *Bevel13;
    TPanel *pRef;
    TBevel *Bevel1;
    TLabel *lStrokesTitle;
    TLabel *lStrokes;
    TBevel *bX;
    TPanel *pSOD;
    TSpeedButton *sbStopS;
    TSpeedButton *sbPlayS;
    TSpeedButton *sbPauseS;
    TSpeedButton *sbFFS;
    TSpeedButton *sbRewS;
    TSpeedButton *sbBackS;
    TSpeedButton *sbForeS;
    TBevel *Bevel10;
    TLabel *lbStrokeS;
    TBevel *Bevel11;
    TBevel *Bevel14;
    TSpeedButton *sbOptionS;
    TPopupMenu *pmSOD;
    TMenuItem *miShadow;
    TMenuItem *miNumbers;
    TMenuItem *miDirection;
    TMenuItem *N1;
    TMenuItem *miSpeed1;
    TMenuItem *miSpeed2;
    TMenuItem *miSpeed3;
    TMenuItem *miRadical;
    TMenuItem *N5;
    TMenuItem *miCopyKanji;
    TMenuItem *miAppendKanji;
    TMenuItem *miPartsPanel;
    TMenuItem *N8;
    TMenuItem *miRefPanel;
    TMenuItem *miSimilarPanel;
    TMenuItem *N9;
    TMenuItem *miParentsPanel;
    TMenuItem *N7;
    TMenuItem *Copykanji1;
    TMenuItem *Appendkanji1;
    TMenuItem *N10;
    TMenuItem *Copykanjidata1;
    TMenuItem *Appendkanjidata1;
    TMenuItem *N11;
    TMenuItem *Referencepanel1;
    TMenuItem *Similarkanjipanel1;
    TMenuItem *Partspanel1;
    TMenuItem *Partofpanel1;
    TActionList *ActionList1;
    TAction *aCopyKanji;
    TAction *aAppendKanji;
    TAction *aCopyRef;
    TAction *aAppendRef;
    TAction *aRefPanel;
    TAction *aSimilarPanel;
    TAction *aPartsPanel;
    TAction *aParentsPanel;
    TPanel *pControl;
    TTimer *tControl;
    TSpeedButton *sbSpeedS1;
    TSpeedButton *sbSpeedS2;
    TSpeedButton *sbSpeedS3;
    TSpeedButton *sbShadowS;
    TSpeedButton *sbNumbersS;
    TSpeedButton *sbDirectionS;
    TShape *ctrlShape1;
    TBevel *Bevel15;
    TAction *aTranslateKanji;
    TMenuItem *N12;
    TMenuItem *miTranslateKanji;
    TMenuItem *NNN;
    TMenuItem *miTranslateKanji2;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall cbGridClick(TObject *Sender);
    void __fastcall pbReadingsPaint(TObject *Sender);
    void __fastcall btnFindClick(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall btnBackClick(TObject *Sender);
    void __fastcall pbSimilarMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall miHideClick(TObject *Sender);
    void __fastcall FormDeactivate(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall miOrderClick(TObject *Sender);
    void __fastcall sbPlayClick(TObject *Sender);
    void __fastcall btnSimilarBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnSimilarForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbStopClick(TObject *Sender);
    void __fastcall sbShadowClick(TObject *Sender);
    void __fastcall sbPauseClick(TObject *Sender);
    void __fastcall sbBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbFFClick(TObject *Sender);
    void __fastcall sbRewClick(TObject *Sender);
    void __fastcall sbSpeed1Click(TObject *Sender);
    void __fastcall sbNumbersClick(TObject *Sender);
    void __fastcall btnPartsBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnPartsForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnParentsBackMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnParentsForeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnHistoryClick(TObject *Sender);
    void __fastcall sbOpenKanjiClick(TObject *Sender);
    void __fastcall sbCloseKanjiClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall pbKanjiMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbDirectionClick(TObject *Sender);
    void __fastcall pmHidePopup(TObject *Sender);
    void __fastcall miCopyClick(TObject *Sender);
    void __fastcall miAppendClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall miRefAllClick(TObject *Sender);
    void __fastcall edRefMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall miHideRefClick(TObject *Sender);
    void __fastcall pmRefPopup(TObject *Sender);
    void __fastcall miShadowClick(TObject *Sender);
    void __fastcall miNumbersClick(TObject *Sender);
    void __fastcall miDirectionClick(TObject *Sender);
    void __fastcall miSpeed1Click(TObject *Sender);
    void __fastcall miSpeed2Click(TObject *Sender);
    void __fastcall miSpeed3Click(TObject *Sender);
    void __fastcall sbOptionSClick(TObject *Sender);
    void __fastcall cbGridMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall miRadicalClick(TObject *Sender);
    void __fastcall pTitleDblClick(TObject *Sender);
    void __fastcall pbKanjiMouseLeave(TObject *Sender);
    void __fastcall pbKanjiMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall sbRefClick(TObject *Sender);
    void __fastcall sbSimilarClick(TObject *Sender);
    void __fastcall sbPartsClick(TObject *Sender);
    void __fastcall sbParentsClick(TObject *Sender);
    void __fastcall aCopyKanjiExecute(TObject *Sender);
    void __fastcall aAppendKanjiExecute(TObject *Sender);
    void __fastcall aCopyRefExecute(TObject *Sender);
    void __fastcall aAppendRefExecute(TObject *Sender);
    void __fastcall aRefPanelExecute(TObject *Sender);
    void __fastcall aSimilarPanelExecute(TObject *Sender);
    void __fastcall aPartsPanelExecute(TObject *Sender);
    void __fastcall aParentsPanelExecute(TObject *Sender);
    void __fastcall pbReadingsVScroll(TObject *Sender, TScrollCode ScrollCode, int &ScrollPos);
    void __fastcall pbReadingsClick(TObject *Sender);
    void __fastcall pbKanjiMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pControlMouseEnter(TObject *Sender);
    void __fastcall pControlMouseLeave(TObject *Sender);
    void __fastcall sbOptionSMouseEnter(TObject *Sender);
    void __fastcall sbOptionSMouseLeave(TObject *Sender);
    void __fastcall tControlTimer(TObject *Sender);
    void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall aTranslateKanjiExecute(TObject *Sender);
    void __fastcall pmSODPopup(TObject *Sender);
private: //User declarations
    typedef TBaseForm inherited;

    TPoint mmousept;
    bool mmoving;
    void start_moving();
    void stop_moving();

    bool modaldisabled; //true when a modal window is visible, so we had to disable some buttons that wouldn't work

    TMouseOvers *mobjON;
    void doMPrepare(TCanvas *c, int index, bool selected);
    void doMONMeasure(TCanvas *c, int index, int &x, int &y, TRect &rect, int &data, bool &active);
    void doMONDraw(TCanvas *c, const TRect &r, int index, int data, bool selected);
    void doMChange(const TRect &r, int index, int data, bool selected);

    TMouseOvers *mobjKun;
    void doMKunMeasure(TCanvas *c, int index, int &x, int &y, TRect &rect, int &data, bool &active);
    void doMKunDraw(TCanvas *c, const TRect &r, int index, int data, bool selected);

    TKanjiDrawObj *kobj;
    TKanji *fkanji; //current kanji, NULL when no kanji selected or element shown
    int felement; //current element, not used when fkanji is not NULL
    TBmp *bm2;
    byte fcollection;

    bool fstrokeorder;

    int fdataindex;

    TCloseButton *closebtn;
    void cbgptr(TCanvas *c, TRect const & r);

    void p1callback();
    void p2callback();
    void p3callback();

    TKanjiPanelObj *posimilar;
    TKanjiPanelObj *poparts;
    TKanjiPanelObj *poparents;

    TDropDownList *history;

    TLabel **titles; //title for reference numbers
    TLabel **refs; //reference numbers

  //opened position and size of small panel with controls
    int ctrlheight;
    int ctrltop;
    enum { ctrlHidden, ctrlVisible, ctrlShow, ctrlHideAfterShow, ctrlHide } ctrlaction;

    void histfunc(int index, UnicodeString w, void *data);

    void update_references();
    void fill_references();
    void refview_setall();
    void enable_player();
    UnicodeString reference_number(TKanjiDataItem item);

    UnicodeString kanjidata();

    void build_refmenu();

    bool same_ON(TKanji *a, TKanji *b);

    void scolor(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected, TColor &background, TColor &fontcolor);
    void sdraw(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected);
    void pcolor(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected, TColor &background, TColor &fontcolor);
    void pdraw(TCanvas *c, TRect r, int index, TKanjiElement *elem, bool selected);
    void playcallback(TDrawStrokeState state);

    void update_similar_buttons();
    void update_parts_buttons();
    void update_parents_buttons();

    void settingsChanged(void *caller, TSettingsProcState state);
    void appDeactivate(void *caller);
    void repaintProc(void *caller);

    void set_data(TKanji *k, int elem);
    void set_kanji_data(TKanji *k);
    void set_kanji(TKanji *k);
    void set_elem_data(int elem);
    void set_elem(int elem);

    TKanji* get_kanji();

    void clear_history();
    void add_history(TKanji *k);
    void add_history(int elem);
    void set_collection(byte ix);

    void roundbounds(int fw, int fh);
    void WMNCCalcSize(TMessage &msg);
    void WMNCHitTest(TWMNCHitTest &msg);
    void WMNCPaint(TMessage &msg);

    void WMActivate(TMessage &msg);

    void WMMenuChar(TMessage &msg);

    void show_option(TInfoOption opt);

    void set_speed(TDrawSpeed speed);
    void set_shadow(bool show);
    void set_numbers(bool show);
    void set_dots(bool show);

    void show_controlpanel();
    void hide_controlpanel();
    void remove_controlpanel();

    void __fastcall miRefViewClick(TObject *Sender);

    void set_strokeorder(bool val);

    friend void ShowKanjiInfo(int collection, TKanji *k);

    void DrawMultilineString(TBmp *bmp, UnicodeString str, int &top);
public: //User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_NCCALCSIZE, TMessage, WMNCCalcSize)
        MESSAGE_HANDLER(WM_NCHITTEST, TWMNCHitTest, WMNCHitTest)
        MESSAGE_HANDLER(WM_NCPAINT, TMessage, WMNCPaint)
        MESSAGE_HANDLER(WM_MENUCHAR, TMessage, WMMenuChar)
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfInfo(TComponent* Owner);
    virtual __fastcall ~TfInfo();

    void enable_buttons(bool enable);
    void enable_readings(bool enable);

    void Show();
    void ElementsChanged(bool available);

  //CAUTION: current kanji can be a part kanji from get
    __property TKanji *kanji = { read = get_kanji, write = set_kanji };
    __property byte collection = { read = fcollection, write = set_collection };
    __property bool strokeorder = { read = fstrokeorder, write = set_strokeorder };
};

void ShowKanjiInfo(int collection, TKanji *k);

extern const wchar_t *refstr[];

//---------------------------------------------------------------------------
extern PACKAGE TfInfo *fInfo;
//---------------------------------------------------------------------------
#endif
