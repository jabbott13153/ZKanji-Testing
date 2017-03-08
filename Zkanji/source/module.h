
//---------------------------------------------------------------------------

#ifndef moduleH
#define moduleH
//---------------------------------------------------------------------------
#include <ActnList.hpp>
#include <AppEvnts.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <StdActns.hpp>
//---------------------------------------------------------------------------

class TWordListObjBase;
class TWordCollection;
struct TKanji;
struct TWord;
class TZPaintBox;
class TKanjiEdit;
enum TWordColumnType;
class TZMenuRows;
class TBaseForm;

enum TWordFilterMenuKind;
namespace Stdactns
{
    class TBrowseForFolder;
}
//---------------------------------------------------------------------------
class Tdata : public TDataModule
{
__published:
    TPopupMenu *pmTray;
    TMenuItem *miRestore;
    TMenuItem *miExit;
    TMenuItem *N1;
    TMenuItem *miToJap;
    TMenuItem *miFromJap;
    TApplicationEvents *appEvents;
    TActionList *aMnu;
    TAction *aExit;
    TAction *aAbout;
    TAction *aSettings;
    TMenuItem *N2;
    TAction *aClipboard;
    TAction *aJtoE;
    TAction *aEtoJ;
    TImageList *imgMenu;
    TAction *aJ;
    TMenuItem *N3;
    TMenuItem *miKanji;
    TImageList *imgBtn;
    TAction *aKanaPractice;
    TImageList *imgCloseButton;
    TPopupMenu *pmKanji;
    TMenuItem *miCut;
    TMenuItem *miCopy;
    TMenuItem *miPaste;
    TMenuItem *miDelete;
    TMenuItem *N4;
    TMenuItem *miSelectAll;
    TImageList *imgPattern;
    TAction *aStats;
    TAction *aStrokeEdit;
    TAction *aShowHideEx;
    TAction *aFilterWords;
    TAction *aWordStart;
    TAction *aWordEnd;
    TAction *aInfConj;
    TAction *aBeta;
    TMenuItem *N5;
    TMenuItem *miInfo;
    TMenuItem *miSites;
    TMenuItem *N6;
    TAction *aUpdate;
    TImageList *imgSmall;
    TImageList *imgCloseButtonAlpha;
    TAction *aStudyList;
    TImageList *imgSmallCloseButton;
    TImageList *imgSmallCloseButtonAlpha;
    TImageList *imgChangeArrows;
    TImageList *imgChangeArrowsAlpha;
    TAction *aJLPTEdit;
    TAction *aEditFilters;
    TImageList *imgMenuCheck;
    TImageList *imgMenuCheckAlpha;
    TImageList *imgMidCloseButton;
    TImageList *imgMidCloseButtonAlpha;
    TImageList *imgPinButton;
    TImageList *imgPinButtonAlpha;
    TImageList *imgMinButton;
    TImageList *imgMinButtonAlpha;
    TImageList *imgFloatButton;
    TImageList *imgFloatButtonAlpha;
    TOpenDialog *dlgOpenImport;
    TBrowseForFolder *dlgFolder;
    TOpenDialog *dlgImportEx;
    void __fastcall aExitExecute(TObject *Sender);
    void __fastcall miRestoreClick(TObject *Sender);
    void __fastcall miToJapClick(TObject *Sender);
    void __fastcall miFromJapClick(TObject *Sender);
    void __fastcall appEventsDeactivate(TObject *Sender);
    void __fastcall aAboutExecute(TObject *Sender);
    void __fastcall aSettingsExecute(TObject *Sender);
    void __fastcall aClipboardExecute(TObject *Sender);
    void __fastcall aJtoEExecute(TObject *Sender);
    void __fastcall aEtoJExecute(TObject *Sender);
    void __fastcall miESep1MeasureItem(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height);
    void __fastcall miESep1DrawItem(TObject *Sender, TCanvas *ACanvas, TRect &ARect, bool Selected);
    void __fastcall appEventsActivate(TObject *Sender);
    void __fastcall aJExecute(TObject *Sender);
    void __fastcall miKanjiClick(TObject *Sender);
    void __fastcall appEventsSettingChange(TObject *Sender, int Flag, const UnicodeString Section, int &Result);
    void __fastcall aKanaPracticeExecute(TObject *Sender);
    void __fastcall miCutClick(TObject *Sender);
    void __fastcall miCopyClick(TObject *Sender);
    void __fastcall miPasteClick(TObject *Sender);
    void __fastcall miDeleteClick(TObject *Sender);
    void __fastcall miSelectAllClick(TObject *Sender);
    void __fastcall aStatsExecute(TObject *Sender);
    void __fastcall aStrokeEditExecute(TObject *Sender);
    void __fastcall aShowHideExExecute(TObject *Sender);
    void __fastcall aFilterWordsExecute(TObject *Sender);
    void __fastcall aInfConjExecute(TObject *Sender);
    void __fastcall aWordStartExecute(TObject *Sender);
    void __fastcall aWordEndExecute(TObject *Sender);
    void __fastcall aBetaExecute(TObject *Sender);
    void __fastcall miInfoClick(TObject *Sender);
    void __fastcall aUpdateExecute(TObject *Sender);
    void __fastcall aStudyListExecute(TObject *Sender);
    void __fastcall aJLPTEditExecute(TObject *Sender);
    void __fastcall aEditFiltersExecute(TObject *Sender);
    void __fastcall dlgFolderCallback(TBrowseForFolder *Sender, HWND Wnd, DWORD uMsg,
          int lParam, int lpData, int &Result);
    void __fastcall dlgFolderAccept(TObject *Sender);
    void __fastcall dlgFolderCancel(TObject *Sender);
    void __fastcall appEventsMessage(tagMSG &Msg, bool &Handled);

private:
    bool fminimized;

    TZMenuRows *firstrows;
    TZMenuRows *kfirstrows;
    TZMenuRows *secondrows;
    TZMenuRows *thirdrows;
    TZMenuRows *kextrarows;
    TZMenuRows *kextrauserrows;
    TZMenuRows *wextrarows;
    TZMenuRows *selrows;
    TZMenuRows *wextrauserrows;
    TZMenuRows *wextrauserrows2;
    TZMenuRows *pwunion;

    TZMenuRows *siterows;

    TZMenuRows *editrows;
    TZMenuRows *filterrows;

    void __fastcall TrayWnd(TMessage &msg);
    HWND ftrayhandle;

    void AddIcon();
    void RemoveIcon();

    void __fastcall siteClick(TObject *Sender);
    UnicodeString SiteSearchString;

    UnicodeString clip1; // Text to copy or append in the popup callback for whole words or selected characters.
    UnicodeString clip2; // Kanji to copy or append in the popup callback for single kanji character.
    UnicodeString popkanjistr; // All kanji in the word's written form in a string without anything else.
    UnicodeString poptranslate; // Text to translate.
    UnicodeString popsearchonline; // Text to search online.
    TWordCollection *popcol;
    byte popcolindex;
    TKanji *popkanji;
    int popwordindex;
    byte popwordmeaning;

    DWORD myprocessid;

    void popupwordcallback(TZMenuRows *rows, int col, int row);

    void wordFiltersEdit(TZMenuRows *rows, int col, int row);
    void wordFiltersCheck(TZMenuRows *rows, int col, int row);

    void __fastcall BrowseForFolderEnteredFolderInvalid(Stdactns::TBrowseForFolder* Sender, HWND Wnd, unsigned uMsg, int lParam, int lpData, int &Result);
    //friend bool BrowseForFolder(UnicodeString caption, UnicodeString &path, bool newfolderbtn, bool folderedit, bool showshares, bool showfiles, bool validatedir, TControl *parent);
public:
    __fastcall Tdata(TComponent* Owner);
    virtual __fastcall ~Tdata();

    void ModalBegin(TBaseForm *form, const HWND hwnd);
    void ModalEnd(const HWND hwnd);
    void PopupWord(int x, int y, TWordListObjBase *wobj, TWordColumnType wcc, TWord *w, TKanji *k);
    void PopupKanji(int x, int y, TKanjiEdit *eobj, wchar_t kanji);
    void PopupFilters(TWordFilterMenuKind kind, int x, int y);

    void Minimize();
    void Restore();

    __property bool Minimized = { read = fminimized };
};

//---------------------------------------------------------------------------
extern PACKAGE Tdata *data;
//---------------------------------------------------------------------------

HWND MessageBoxHandle();
void SettingsChangeBegin();
void SettingsChangeEnd();

typedef void(__closure * TWordGroupFuncPtr)(void *caller, TWordCollection * coll, TWord * w);
void WordGroupChanged(TWordCollection *coll, TWord *w);
void WordGroupEvent(void *caller, TWordGroupFuncPtr fprc); // Set fprc to NULL when removing event.

enum TWordFilterMenuKind { mkDictionary, mkKanjiWords, mkWordGroups };

#endif
