//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Math.h>
#include "settings.h"
#include "settingswindow.h"
#include "collection.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(142);
#endif

TSettings settings;
TProfileSettings profilesettings;
TColorSettings colorsettings;
extern TForm *fMain;

//---------------------------------------------------------------------------
bool ShowSettings(UnicodeString page /*TSettingsPage page*/)
{
    UnicodeString oldpage = settings.settingspage;
    if (!page.IsEmpty())
        settings.settingspage = page;

    int r = mrCancel;
    fSettings = new TfSettings(Application);
    try
    {
        MODON
        r = fSettings->ShowModal();
        MODOFF
    }
    catch(...)
    {
        ;
    }
    delete fSettings;
    fSettings = NULL;

    if (!page.IsEmpty())
        settings.settingspage = oldpage;

    return r == mrOk;
}
//---------------------------------------------------------------------------
int rowsizes(int setting)
{
    if (setting < 0)
        setting = settings.dictfontsize;
    return ((setting) == 0 ? 21 * scaleres + 1 : (setting) == 1 ? 17 * scaleres + 1 : (setting) == 2 ? 15 * scaleres + 1 : 0);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(143);
#endif

