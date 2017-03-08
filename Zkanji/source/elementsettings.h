//---------------------------------------------------------------------------

#ifndef elementsettingsH
#define elementsettingsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#include "baseform.h"

struct TElementSettings
{
    bool kanjibackground; //kanji is visible in the background
    bool strokenumbers; //show numbers for strokes

    bool forcecentre;

    UnicodeString kanjifont;
    int kanjicharset;
    TColor kanjicolor;
    double kanjisize;

    UnicodeString lastfilename;
};

class TfElemSettings : public TBaseForm
{
__published: //IDE-managed Components
    TComboBox *cbFonts;
    TColorDialog *dColor;
    TButton *btnColor;
    TZPaintBox *pbColor;
    TLabel *Label1;
    TComboBox *cbSizes;
    TLabel *Label2;
    TBitBtn *btnOk;
    TLabel *Label3;
    TBitBtn *btnCancel;
    TCheckBox *cbForceCentre;
    void __fastcall pbColorPaint(TObject *Sender);
    void __fastcall btnColorClick(TObject *Sender);
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall btnOkClick(TObject *Sender);
    void __fastcall cbSizesChange(TObject *Sender);
    void __fastcall cbFontsChange(TObject *Sender);
    void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall FormDeactivate(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall cbForceCentreClick(TObject *Sender);
private: //User declarations
    TElementSettings backup;
public: //User declarations
    __fastcall TfElemSettings(TComponent* Owner);
    virtual __fastcall ~TfElemSettings();

    void __fastcall ShowEx(int aleft, int atop);
    void __fastcall Reset();
};
extern PACKAGE TfElemSettings *fElemSettings;

extern TElementSettings elementsettings;

#endif
