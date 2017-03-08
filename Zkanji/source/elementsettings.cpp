//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "elementsettings.h"
#include "myfonts.h"
#include "elementedit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(40);
#endif

TfElemSettings *fElemSettings;

TElementSettings elementsettings;

double elembkfontsizes[5] = { 0.6, 0.7, 0.75, 0.8, 0.87 };
//---------------------------------------------------------------------------
__fastcall TfElemSettings::TfElemSettings(TComponent* Owner) : TBaseForm(Owner)
{
}
//---------------------------------------------------------------------------
__fastcall TfElemSettings::~TfElemSettings()
{
    fElemSettings = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfElemSettings::ShowEx(int aleft, int atop)
{
    backup.kanjifont = elementsettings.kanjifont;
    backup.kanjicharset = elementsettings.kanjicharset;
    backup.kanjicolor = elementsettings.kanjicolor;
    backup.kanjisize = elementsettings.kanjisize;
    backup.forcecentre = elementsettings.forcecentre;

    cbForceCentre->Checked = elementsettings.forcecentre;

    cbFonts->Items->BeginUpdate();
    cbFonts->Items->Clear();
    for (int ix = 0; ix < jfonts->Count; ++ix)
    {
        cbFonts->Items->AddObject(jfonts->Strings[ix], jfonts->Objects[ix]);
    }
    cbFonts->ItemIndex = cbFonts->Items->IndexOf(elementsettings.kanjifont);
    if (cbFonts->ItemIndex < 0)
        cbFonts->ItemIndex = 0;
    cbFonts->Items->EndUpdate();

    for (unsigned int ix = 0; ix < sizeof(elembkfontsizes) / sizeof(double); ++ix)
        if (elembkfontsizes[ix] == elementsettings.kanjisize)
        {
            cbSizes->ItemIndex = ix;
            break;
        }

    try
    {
        Left = aleft;
        Top = atop;
        Show();
    }
    catch (...)
    {
        ;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfElemSettings::pbColorPaint(TObject *Sender)
{
    TCanvas *c = pbColor->Canvas;
    c->Brush->Color = elementsettings.kanjicolor;
    c->FillRect(pbColor->ClientRect);
}
//---------------------------------------------------------------------------
void __fastcall TfElemSettings::btnCancelClick(TObject *Sender)
{
    if (Visible)
        Reset();
}
//---------------------------------------------------------------------------
void __fastcall TfElemSettings::btnOkClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TfElemSettings::Reset()
{
    elementsettings.kanjifont = backup.kanjifont;
    elementsettings.kanjicharset = backup.kanjicharset;
    elementsettings.kanjicolor = backup.kanjicolor;
    elementsettings.kanjisize = backup.kanjisize;
    elementsettings.forcecentre = backup.forcecentre;
    fEditElem->redrawbg();
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TfElemSettings::cbForceCentreClick(TObject *Sender)
{
    elementsettings.forcecentre = cbForceCentre->Checked;
    fEditElem->redrawbg();
}
//---------------------------------------------------------------------------
void __fastcall TfElemSettings::btnColorClick(TObject *Sender)
{
    dColor->Color = elementsettings.kanjicolor;
    if (ExecuteDialog(dColor))
    {
        elementsettings.kanjicolor = (TColor)ColorToRGB(dColor->Color);
        pbColor->Invalidate();
        fEditElem->redrawbg();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfElemSettings::cbSizesChange(TObject *Sender)
{
    int ix = RealIndex(cbSizes);
    if (ix >= 0)
    {
        elementsettings.kanjisize = elembkfontsizes[ix];
        fEditElem->redrawbg();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfElemSettings::cbFontsChange(TObject *Sender)
{
    int ix = RealIndex(cbFonts);
    if (ix >= 0)
    {
        elementsettings.kanjifont = jfonts->Strings[ix];
        elementsettings.kanjicharset = (int)jfonts->Objects[ix];
        fEditElem->redrawbg();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfElemSettings::FormKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == VK_ESCAPE)
        btnCancel->Click();
    if (Key == VK_RETURN)
        btnOk->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfElemSettings::FormDeactivate(TObject *Sender)
{
    btnCancel->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfElemSettings::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caFree;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(41);
#endif

