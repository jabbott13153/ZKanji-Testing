//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Themes.hpp>
//#include <Registry.hpp>
//#include <IniFiles.hpp>
#include <shlobj.h>
#include "main.h"
#include "dropdown.h"
#include "myfonts.h"
#include "kanji.h"
#include "collection.h"
#include "kanjiobj.h"
#include "wordobj.h"
#include "settingswindow.h"
#include "elementsettings.h"
#include "searchtree.h"
#include "zkformats.h"
#include "popupdict.h"
#include "kanjiinfo.h"
#include "grouplist.h"
#include "kanjilist.h"
#include "wordlist.h"
#include "clipview.h"
#include "module.h"
#include "words.h"
#include "romajizer.h"
#include "newword.h"
#include "redefine.h"
#include "zmenu.h"
#include "kanjidef.h"
#include "kanjiform.h"
#include "wordgen.h"
#include "msgform.h"
#include "zrads.h"
#include "kanjielement.h"
#include "unicodehint.h"
#include "kanjihint.h"
#include "repetition.h"
#include "kanapractice.h"
#include "studylist.h"
#include "modallist.h"
#include "recognizer.h"
#include "recobj.h"
#include "docker.h"
#include "sitemanager.h"
#include "update.h"
#include "warningbubble.h"
#include "gdipbmp.h"
#include "stats.h"
#include "genworddata.h"
#include "wordfilter.h"
#include "dummymainform.h"
#include "taskbar.h"
#include "dictupdater.h"
#include "sentences.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"


#ifdef _DEBUG
static TGlobalTest tester1(-1);
#endif

TfMain *fMain;
extern TKanjiHint *kanjihint;
extern TSiteList *sitelist;
//---------------------------------------------------------------------------
extern bool ThemesAreTurnedOn;
extern TBaseForm *fStudyList;
extern TBaseForm *fTest;


bool freeing_user_data = false;

TProgramPaths paths;

TGdiPlus *gdi = NULL;

struct zappevent;
struct zsettingsevent;
struct zwordgroupevent;
TTemplateList<zappevent, true> *appevents = NULL;
TTemplateList<zsettingsevent, true> *settingevents = NULL;
TTemplateList<zwordgroupevent, true> *wordgroupevents = NULL;

SYSTEMTIME dictionary_date; // Exact date when the main dictionary was generated.
SYSTEMTIME user_date;
SYSTEMTIME example_date;
wchar_t Version_String[] = L"0.731";
double dictionary_build_version = 0.731; // Version number of zkanji when the dictionary was created.
// Previous version: v0.73 - keep this comment line here. The automatic release script needs this information.

wchar_t TasklistAppID[] = L"zkanjiappid";

#define MCONCAT2(a,b)   MCONCAT(a,b)
#define MCONCAT(a,b)   a ## b

#define _L(a)   MCONCAT2(L,a)
//wchar_t Dictionary_Export_Version_String[] = L"0.02";
//wchar_t Dictionary_Group_Export_Version_Number[] = L"0.01";
wchar_t Release_Date[] = _L(__DATE__);

const int dictionary_name_length_limit = 128;
const int group_name_length_limit = 255;

bool *validkanji = NULL;
bool *validcode = NULL;
byte validcode_lowerhalf;
const int validcode_count = (0x30ff - 0x3000) + (0xffe0 - 0xff00);
const word kanji_count = 6355;

wchar_t Stroke_Filename[] = L"zdict.zks";

wchar_t stroketext[] = L"The stroke order database included with zkanji is Copyright © 2009-2013 Sólyom Zoltán.  Please contact me if you discover errors in the stroke order.";

wchar_t nostroketext[] = L"The database for the kanji stroke order diagrams is not loaded. Make sure you have \"%s\" in the data folder of the program!";
wchar_t jmdicttext[] = L"This program uses a compilation of the JMdict and KANJIDIC dictionary files which are the\nproperties of The Electronic Dictionary Research and Development Group, Monash University.\n\
The group can be found at: http://www.edrdg.org/\n\
\n\
KANJIDIC : http://www.csse.monash.edu.au/~jwb/kanjidic.html\n\
JMDICT: http://www.csse.monash.edu.au/~jwb/j_jmdict.html\n\
RADKFILE : http://www.csse.monash.edu.au/~jwb/kradinf.html\n\
\n\
© 2001~ The Electronic Dictionary Research and Development Group, Monash University";

bool hasstrokes = false; // Stroke order database.
bool hasrecdata = true; // Stroke recognition data loaded.
bool hasexamples = false;

bool AppActive = true;

bool TestRun;

TTemplateList<TCustomKanjiGrid, false> *kanjigrids = NULL;

THintWindow *hint8 = NULL;

word *partlist[252];
TKanjiItemList *kanjis;
TRadicalList *radlist;
TStudentProfile *profile;

TWordCollectionList *dictionary; // Dictionaries in the system. #0 is jmdict.

TDropDownList *romajihistory;
TDropDownList *kanjihistory;

TfMain::TUpdateTasksThread *TfMain::TUpdateTasksThread::thread = NULL;
bool TfMain::TUpdateTasksThread::waiting = false;
UnicodeString TfMain::TUpdateTasksThread::langname;


//TTemplateList<TBaseForm,false> *WindowList;

/*
 void ChangeFocus(bool back) {
 int i = WindowList->IndexOf((void*)Screen->ActiveForm);
 if (i < 0)
 return;
 int circle = 0;
 do {
 i += (back ? -1 : 1);
 if (i < 0) {
 i = WindowList->Count+i;
 } else if ( i >= WindowList->Count) {
 i -= WindowList->Count;
 }
 circle++;
 } while (circle < WindowList->Count && !WindowList->Items[i]->Visible);
 WindowList->Items[i]->SetFocus();
 }
 */
//---------------------------------------------------------------------------
void FormActivated(TBaseForm *f)
{
    if (!activelist || activelist->IndexOf(f) <= 0)
        return;

    activelist->Delete(activelist->IndexOf(f));
    activelist->Insert(0, f);
}
//---------------------------------------------------------------------------
void polyrect(TCanvas *c, TRect r, int width)
{
    for (int i = 0; i < width; i++)
    {
        TPoint pt[5] = { Point(r.left - i, r.top - i), Point(r.left - i, r.bottom + i), Point(r.right + i, r.bottom + i), Point(r.right + i, r.top - i), Point(r.left - i, r.top - i) };
        c->Polyline(pt, 4);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool interminating = false;
int DoTerminate();

bool __fastcall terminateproc()
{
    if (!interminating)
        DoTerminate();
    return true;
}

//---------------------------------------------------------------------------
WNDPROC oldwnd;
bool appwndunicode;

LRESULT CALLBACK appWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static bool achange = false;

    if (uMsg == WM_ACTIVATEAPP && achange)
    {
        PostMessage(hwnd, uMsg, wParam, lParam);
        return NULL;
    }

    if (uMsg == WM_HOTKEY)
    {
        if (modallist && modallist->Count && (int)wParam >= 0)
        {
            SetForegroundWindow(modallist->Items[modallist->Count - 1]->Handle);
        }
        else
        {
            if (wParam == 0)
                fPopupDict->Popup(true, dmKanji);
            if (wParam == 1)
                fPopupDict->Popup(true, dmRomaji);
            if (wParam == 2)
                fKanji->Popup(!fKanji->ontop);
        }
    }

    if (activelist && activelist->Count >= 3 && fClipboard && fInfo)
    {
        if (uMsg == WM_SHOWWINDOW && hwnd == Application->Handle && settings.intray)
            return NULL;

        if (uMsg == WM_SYSCOMMAND && wParam == SC_MINIMIZE && hwnd == Application->Handle)
        {
            fMain->Minimize();
            if (settings.minimizetotray)
                return NULL;
        }

        try
        {
            if (fDict && uMsg == WM_ACTIVATEAPP)
            {
                achange = true;
                if (wParam == 0)
                {
                    PostMessage(Application->Handle, CM_DEACTIVATE, 0, lParam);
                    if (fInfo && fClipboard && (!settings.intray || !fKanji->Visible))
                    {
                        fInfo->OnTop = false;
                        fClipboard->OnTop = false;
                    }
                }
                else
                {
                    PostMessage(Application->Handle, CM_ACTIVATE, 0, lParam);
                    if (fInfo && fClipboard)
                    {
                        fInfo->OnTop = true;
                        fClipboard->OnTop = true;
                    }
                }
                return NULL;
            }

            if (fDict && (uMsg == CM_ACTIVATE || uMsg == CM_DEACTIVATE))
            {
                if (AppActive == (uMsg == CM_ACTIVATE) && settings.intray)
                {
                    achange = false;
                    return NULL;
                }

                AppActive = uMsg == CM_ACTIVATE;

                if (settings.intray)
                {
                    if (AppActive)
                    {
                        HDWP hdwp = 0;
                        try
                        {
                            hdwp = BeginDeferWindowPos((fPopupDict->Visible ? 1 : 0) + (fKanji->Visible ? 1 : 0) + (modallist && modallist->Count && modallist->Items[modallist->Count - 1]->Handle ? modallist->Count : 0));
                            if (fPopupDict->Visible)
                                hdwp = DeferWindowPos(hdwp, fPopupDict->Handle, modallist && modallist->Count ? HWND_TOP : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
                            if (fKanji->Visible)
                                hdwp = DeferWindowPos(hdwp, fKanji->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

                            if (modallist && modallist->Count && modallist->Items[modallist->Count - 1]->Handle)
                            {
                                for (int ix = 0; ix < modallist->Count; ++ix)
                                    hdwp = DeferWindowPos(hdwp, modallist->Items[ix]->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
                            }
                        }
                        catch (...)
                        {
                            ;
                        }
                        if (hdwp)
                            EndDeferWindowPos(hdwp);
                    }
                    else // Deactivate.
                    {
                        ;
                    }
                }
                else if (AppActive)
                {
                    bool ondock = fDock && fDock->Visible && fDock->HandleAllocated();

                    HDWP hdwp = 0;
                    try
                    {
                        hdwp = BeginDeferWindowPos((ondock ? 1 : 0) + (modallist && modallist->Count && modallist->Items[modallist->Count - 1]->Handle ? modallist->Count : 0));
                        if (ondock)
                            hdwp = DeferWindowPos(hdwp, fDock->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

                        if (modallist && modallist->Count && modallist->Items[modallist->Count - 1]->Handle)
                        {
                            for (int ix = 0; ix < modallist->Count; ++ix)
                                hdwp = DeferWindowPos(hdwp, modallist->Items[ix]->Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
                        }
                    }
                    catch (...)
                    {
                        ;
                    }
                    if (hdwp)
                        EndDeferWindowPos(hdwp);
                }
                else // Deactivate.
                {
                    if (fInfo && (fInfo->Visible || fClipboard->Visible))
                    {
                        HWND h = GetNextWindow(fInfo->Handle, GW_HWNDNEXT);
                        HWND hthread = NULL;
                        while (h)
                        {
                            long tid = GetWindowThreadProcessId(h, NULL);
                            if (tid == lParam && !(GetWindowLong(h, GWL_EXSTYLE) & WS_EX_TOPMOST) && IsWindowVisible(h))
                            {
                                TRect r;
                                GetWindowRect(h, &r);
                                if (r.Width() && r.Height())
                                    hthread = h;
                            }
                            h = GetNextWindow(h, GW_HWNDNEXT);
                        }

                        if (hthread)
                        {
                            if (fInfo->Visible)
                                SetWindowPos(fInfo->Handle, hthread, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                            if (fClipboard->Visible)
                                SetWindowPos(fClipboard->Handle, hthread, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                        }
                    }
                }
            }
        }
        catch (...)
        {
            ;
        }
        if (uMsg == CM_ACTIVATE || uMsg == CM_DEACTIVATE)
            achange = false;
    }

    if (uMsg == WM_ENDSESSION)
    {
        if (!interminating)
            DoTerminate();
    }

    LRESULT r = 0;
    try
    {
        r = (appwndunicode ? CallWindowProcW : CallWindowProcA)((LRESULT(__stdcall *)(HWND, UINT, WPARAM, LPARAM))oldwnd, hwnd, uMsg, wParam, lParam);
    }
    catch (...)
    {
        ;
    }

    return r;
}

/*
 int _keynamesort_proc(TStringList *s, int a, int b)
 {
 int ka = -1, kb = -1;
 try {
 ka = s->Strings[a].ToInt();
 kb = s->Strings[b].ToInt();
 } catch(...) {
 if (ka >= 0 || kb >= 0)
 return (ka < 0 ? -1 : 1);
 return (a-b);
 }
 return ka-kb;
 }
 */
__fastcall TfMain::TfMain(TComponent* Owner) : TBaseForm(Owner)
{
    // Check if we are in program files folder and if there, create local appdata folder for the user.
    // paths->inipath and paths->datapath both point to a writeable path that can be used for storing data and ini files.
    // In case the program was not run from the Program Files folder they are inside the zkanji folder.

    wchar_t ptmp[MAX_PATH];

    paths.installpath = ExtractFileDir(ParamStr(0));
    if (paths.installpath.LastDelimiter(L"\\") != paths.installpath.Length())
        paths.installpath += L"\\";
    paths.installdatapath = paths.installpath + L"data\\";
    bool ininstall = pathaccess(paths.installpath);
    if (ininstall)
        paths.inipath = paths.installpath;

    if (!ininstall)
    {
        // Check if we already have data in one of the system folders.
        UnicodeString appdata;
        bool inapp = false;
        UnicodeString appdata2;
        bool inapp2 = false;
        int r;

        r = SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, ptmp);
        if (r != E_FAIL && r != E_INVALIDARG)
        {
            if (wcslen(ptmp) <= 1)
            {
                GetCurrentDirectoryW(MAX_PATH, ptmp);
                GetFullPathNameW(ptmp, MAX_PATH, ptmp, NULL);
            }
            appdata = ptmp;
            if (appdata.Length())
            {
                if (appdata.LastDelimiter(L"\\") != appdata.Length())
                    appdata += L"\\";
                if (DirectoryExists(appdata + L"zkanji", false) && DirectoryExists(appdata + L"zkanji\\data", false) && pathaccess(appdata + L"zkanji\\"))
                {
                    inapp = true;
                    paths.inipath = appdata + L"zkanji\\";
                }
            }
        }

        if (!inapp)
        {
            r = SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, ptmp);
            if (r != E_FAIL && r != E_INVALIDARG)
            {
                if (wcslen(ptmp) <= 1)
                {
                    GetCurrentDirectoryW(MAX_PATH, ptmp);
                    GetFullPathNameW(ptmp, MAX_PATH, ptmp, NULL);
                }
                appdata2 = ptmp;
                if (appdata2.Length())
                {
                    if (appdata2.LastDelimiter(L"\\") != appdata2.Length())
                        appdata2 += L"\\";
                    if (DirectoryExists(appdata2 + L"zkanji", false) && DirectoryExists(appdata2 + L"zkanji\\data", false) && pathaccess(appdata2 + L"zkanji\\"))
                    {
                        inapp2 = true;
                        paths.inipath = appdata2 + L"zkanji\\";
                    }
                }
            }
        }

        if (!inapp && !inapp2)
        {
            if (appdata.Length() && pathaccess(appdata))
                paths.inipath = appdata + L"zkanji\\";
            else if (appdata2.Length() && pathaccess(appdata2))
                paths.inipath = appdata2 + L"zkanji\\";
            else
                THROW(L"Error with permissions! Couldn't find folder to store user data!");
        }
    }

    paths.datapath = paths.inipath + "data\\";
    if (!DirectoryExists(paths.inipath, false))
    {
        if (!CreateDirectory(paths.inipath.c_str(), NULL))
            THROW(L"Error with permissions! Couldn't find folder to store user data!");
    }
    if (!DirectoryExists(paths.datapath, false))
    {
        if (!CreateDirectory(paths.datapath.c_str(), NULL))
            THROW(L"Error with permissions! Couldn't find folder to store user data!");
    }

    /*
     TRegistry *reg = NULL;
     TStringList *keylist = NULL;
     try {
     bool usereg = false;

     reg = new TRegistry;
     reg->Access = KEY_READ;
     reg->RootKey = HKEY_LOCAL_MACHINE;
     if (reg->KeyExists(L"Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\zkanji.exe")) {
     Application->MessageBox((UnicodeString("Reg!")).c_str(),L"Error",MB_OK);
     usereg = true;
     } else {
     Application->MessageBox((UnicodeString("No reg!")).c_str(),L"Error",MB_OK);
     }

     wchar_t apppath[MAX_PATH];
     if (usereg) {
     int r = SHGetFolderPathW(NULL,CSIDL_COMMON_APPDATA,NULL,SHGFP_TYPE_CURRENT,apppath);
     if (r == E_FAIL || r == S_FALSE || r == E_INVALIDARG) {
     Application->MessageBox((UnicodeString("Failed to get common appdata folder: ")+r).c_str(),L"Error",MB_OK);
     usereg = false;
     } else {
     if (apppath[wcslen(apppath)-1] != L'\\' && apppath[wcslen(apppath)-1] != L'/')
     wcscat(apppath,L"\\");
     if (!DirectoryExists(UnicodeString(apppath)+"zkanji")) {
     Application->MessageBox((UnicodeString("Folder doesn't exist. Trying to create.")).c_str(),L"Message",MB_OK);
     if (!CreateDirectory((UnicodeString(apppath)+"zkanji").c_str(),NULL)) {
     usereg = false;
     Application->MessageBox((UnicodeString("Couldn't create common appdata folder!")).c_str(),L"Error",MB_OK);
     }
     }
     }
     }

     if (usereg) {
     mini = new TMemIniFile(UnicodeString(apppath)+"zkanji\\userpath.ini",TEncoding::UTF8);
     keylist = new TStringList();
     mini->ReadSection("paths",keylist);
     keylist->CustomSort(&_keynamesort_proc);

     int empty = 0;
     for (int ix = 0; ix < keylist->Count && empty >= 0; ++ix) {
     int k = -1;
     try {
     k = keylist->Strings[ix].ToInt();
     } catch(...) {
    ;
     }
     if (k < 0)
     continue;

     if (empty >= k)
     empty = k+1;
     if (mini->ReadString("paths",keylist->Strings[ix],"") == paths.inipath)
     empty = -1;
     }

     if (empty >= 0) {
     mini->WriteString("paths",IntToStr(empty),paths.inipath);
     mini->UpdateFile();
     }
     }

     delete mini;
     delete keylist;
     delete reg;
     mini = NULL;
     keylist = NULL;
     reg = NULL;
     } catch(...) {
     delete mini;
     delete keylist;
     delete reg;
     throw;
     }
     */

    fsi = new TFormSizeInfo[WINDOW_SIZE_COUNT];
    modallist = new TModalList; // Must be created before any window is ever searched for.

    fRadicals = NULL;
    fMsg = NULL;
    hint = NULL;
    fClipboard = NULL;
    fInfo = NULL;
    fRecognize = NULL;
    fPopupDict = NULL;
    fKanji = NULL;
    fGroup = NULL;
    fDict = NULL;
    fDock = NULL;
    fStudyList = NULL;
    fTest = NULL;
    fBubble = NULL;
    fStats = NULL;

    inTimer = false;
    secTimer = 0;
    checkTimer = 0;
    secAutosave = 0;
    inited = 0;
    bool appwndunicode = IsWindowUnicode(Application->Handle);
    oldwnd = (WNDPROC)(appwndunicode ? SetWindowLongPtrW : SetWindowLongPtrA)(Application->Handle, GWL_WNDPROC, (long)appWndProc);

    if (!appevents)
        appevents = new TTemplateList<zappevent, true>;
    if (!settingevents)
        settingevents = new TTemplateList<zsettingsevent, true>;
    if (!wordgroupevents)
        wordgroupevents = new TTemplateList<zwordgroupevent, true>;

    int compenabled = MatchVersion(6, 0);
    if (compenabled)
        DwmIsCompositionEnabled(&compenabled);

    Width = imgR->Left + imgR->Width;
    imgTR->Left = Width - imgTR->Width;
    imgBR->Left = Width - imgBR->Width;
    imgR->Left = Width - imgR->Width;
    imgTopBg->Left = imgLogo->Left + imgLogo->Width;

    if (!compenabled)
        BorderStyle = Forms::bsNone;

    gdi = new TGdiPlus();

}
//---------------------------------------------------------------------------
void TfMain::Init()
{
    if (inited)
        return;
    inited = 1;

    wchar_t temppath[MAX_PATH];
    GetTempPath(MAX_PATH, temppath);
    UnicodeString dlfilename = UnicodeString(temppath) + "zkanjisetup.exe";
    if (FileExists(dlfilename, false))
        DeleteFile(dlfilename);
    dlfilename = UnicodeString(temppath) + "zkanjisetupdict.exe";
    if (FileExists(dlfilename, false))
        DeleteFile(dlfilename);

    wordfilters = new TWordFilterList;
    dictionaryfilters = new TWordFilterSelection;
    kanjiwordfilters = new TWordFilterSelection;
    wordgroupfilters = new TWordFilterSelection;

    int cdelimpos = 0;
    for (byte ix = 0; ix < 255; ++ix)
        if (ix != '-' && (ispunct(ix) || ix >= '0' && ix <= '9' || ix == ' ' || ix == '\t' || ix == '\n' || ix == '\r'))
            cdelim[cdelimpos++] = ix;
    cdelim[cdelimpos] = 0;

    sitelist = new TSiteList;

    if (!validkanji)
        validkanji = new bool[0x9faf - 0x4e00 + 1];

    generate_validunicode();

    menu = new TZPopupMenu;

    lbVer->Caption = (UnicodeString)"v" + Version_String;

    lbReleaseDate->Caption = (UnicodeString)"Build: " + Release_Date;

    Randomize();

    lbLoadMsg->Caption = " Initializing...";
    pLoad->Refresh();

    memset(partlist, 0, sizeof(partlist) / sizeof(word*));

    romajihistory = new TDropDownList;
    kanjihistory = new TDropDownList;
    kanjis = new TKanjiItemList;
    kanjis->Capacity = kanji_count;
    profile = new TStudentProfile;
    dictionary = TWordCollectionList::Create();
    settings.dictionary_index = 0;
    radlist = new TRadicalList();

    lbLoadMsg->Caption = L" Reading dictionaries...";
    Refresh();

    store = new TWordStore();

    error_code = 0;
    try
    {
        if (!FileExists(paths.installdatapath + L"zdict.zkj") || !FileExists(paths.installdatapath + L"English.zkj"))
        {
            TWordCollection *wcbase = new TWordCollection();
            store->Collection = wcbase;

            error_code = 1;
            import_kanji(L"data\\kanji.txt");
            generate_validkanji();

            error_code = 2;
            import_radicals(L"data\\radkfile.txt");
            error_code = 3;
            import_zradicals(L"data\\zradfile.txt");
            error_code = 4;
            import_dictionary(L"data\\zdict.txt", wcbase);

            error_code = 5;
            import_tree(L"data\\eromajilines.txt", L"data\\eromajilinesskip.txt", wcbase->rtree);
            error_code = 6;
            import_tree(L"data\\ekanalines.txt", L"data\\ekanalinesskip.txt", wcbase->ktree);
            error_code = 7;
            import_tree(L"data\\ekanablines.txt", L"data\\ekanablinesskip.txt", wcbase->btree);

            error_code = 8;
            filter_kanji(wcbase); // Drop those readings that have no match in dictionary.
            error_code = 9;
            sort_kanji_readings();

            error_code = 10;
            find_kanji_words(wcbase, kanjiindex);
            error_code = 11;
            set_word_frequencies(wcbase);

            error_code = 12;
            import_jlpt(wcbase, L"data\\JLPTNData.txt");
            error_code = 13;

            wcbase->info =
L"This dictionary is based on JMDict, and is mainly a compilation of the following sources in accordance with the licence provisions of the Electronic Dictionary Research and Development Group:\r\n\
\r\n\
KANJIDIC : http://www.csse.monash.edu.au/~jwb/kanjidic.html\r\n\
JMDICT: http://www.csse.monash.edu.au/~jwb/j_jmdict.html\r\n\
RADKFILE : http://www.csse.monash.edu.au/~jwb/kradinf.html\r\n\
\r\n\
Visit http://www.edrdg.org for more information.";

            save_dictionary_base(paths.installdatapath + L"zdict.zkj");
            save_dictionary(paths.installdatapath + "English.zkj", wcbase, true);

            delete wcbase;
            store->Collection = NULL;

        }
        else
        {
            if (!FileExists(paths.installdatapath + L"zdict.zkj") || !load_dictionary_base(paths.installdatapath + L"zdict.zkj"))
                THROW(L"Error loading base dictionary.");
        }

        if (!FileExists(paths.datapath + L"English.zkd") || !get_dictionary_date(paths.datapath + L"English.zkd", user_date) || !load_dictionary(paths.datapath + L"English.zkd", dictionary->Items[dictionary->Add(L"English")], true, false))
        {
            dictionary->Clear();
            if (!load_dictionary(paths.installdatapath + L"English.zkj", dictionary->Items[dictionary->Add(L"English")], true, false))
                THROW(L"Error loading main English dictionary.");
            CopyFile((paths.installdatapath + L"English.zkj").c_str(), (paths.datapath + L"English.zkd").c_str(), FALSE);
            memcpy(&user_date, &dictionary_date, sizeof(SYSTEMTIME));
            dictionary->Items[0]->Modify(true);
        }

        store->Collection = dictionary->Items[0];
    }
    catch (...)
    {
        MsgBox(((UnicodeString)L"Couldn't open main dictionary. The program won't run! (code: " + IntToStr(error_code) + ")").c_str(), "Error", MB_OK);
        Application->Terminate();
        return;
    }

    error_code = 0;

    if (FileExists(paths.installdatapath + L"similar.txt"))
    {
        lbLoadMsg->Caption = L" Loading similar kanji list...";
        pLoad->Refresh();
        import_confused((paths.installdatapath + L"similar.txt").c_str());
    }

    if (FileExists(paths.installdatapath + L"irregulars.txt"))
    {
        lbLoadMsg->Caption = L" Loading irregular readings list...";
        pLoad->Refresh();
        import_irregulars((paths.installdatapath + L"irregulars.txt").c_str());
    }

    models = new TStrokeModels();
    cmodels = new TStrokeModels();
    hasstrokes = FileExists(paths.installdatapath + Stroke_Filename); //stroke order database
    elements = new TElementList(0);

    if (!hasstrokes)
        NoStrokeText();

    pLoad->Refresh();

    lbLoadMsg->Caption = L" Initializing font list...";
    pLoad->Refresh();

    initfonts();

    lbLoadMsg->Caption = L" Creating windows...";
    pLoad->Refresh();

    fRecognize = new TfRecognize(Application);
    fPopupDict = new TfPopupDict(Application);
    fDock = new TfDock(Application);
    fDict = new TfDict(Application);
    fGroup = new TfGroup(Application);
    fKanji = new TfKanji(Application);
    fInfo = new TfInfo(Application);
    fClipboard = new TfClipboard(Application);

    lbLoadMsg->Caption = L" Loading settings...";
    pLoad->Refresh();

    LoadSettings();

    lbLoadMsg->Caption = L" Reading backup folder...";
    pLoad->Refresh();

    dictionary->ReadBackup();

    lbLoadMsg->Caption = L" Loading user dictionaries and saved user data...";
    pLoad->Refresh();

    try
    {
        dictionary->Load();
    }
    catch (...)
    {
        MsgBox(L"An error occurred while opening user data files! Quit zkanji and DON't start it again, unless you restore the backup files first! Contact me if the problem persists.", L"Error", MB_OK);
        throw;
    }
    if (Application->Terminated)
        return;

    try
    {
        if (FileExists(paths.installdatapath + "examples.zkj"))
        {
            lbLoadMsg->Caption = L" Loading word example indexes...";
            pLoad->Refresh();
            if (load_examples(paths.installdatapath + "examples.zkj"))
                hasexamples = true;
        }
    }
    catch (...)
    {
        MsgBox((UnicodeString)"Invalid examples file! Please download it again! Skipping...", "Error", MB_OK);
        store->ClearExampleData();
//        for (int ix = 0; ix < store->Count; ++ix)
//        {
//            if (store->Items[ix]->examplecnt)
//            {
//                store->Items[ix]->examplecnt = 0;
//                delete[] store->Items[ix]->examples;
//                store->Items[ix]->examples = NULL;
//            }
//        }
    }

    if (elementsettings.lastfilename != "")
    {
        try
        {
            lbLoadMsg->Caption = L" Loading custom stroke order diagrams...";
            pLoad->Refresh();

            elements->LoadFromFile(elementsettings.lastfilename);
        }
        catch (...)
        {
            elementsettings.lastfilename = "";
            try
            {
                delete elements;
            }
            catch (...)
            {
                ;
            }
            elements = new TElementList(0);
        }
    }

    if (hasstrokes && !elements->Count)
    {
        try
        {
            delete elements;
            elements = 0;

            lbLoadMsg->Caption = L" Loading stroke order diagrams...";
            pLoad->Refresh();

            elements = new TElementList(paths.installdatapath + Stroke_Filename);
        }
        catch (...)
        {
            hasstrokes = false;
            try
            {
                delete elements;
            }
            catch (...)
            {
                ;
            }
            elements = new TElementList(0);
            MsgBox(L"Couldn't load stroke order datafile. Get the full version if you want to use the handwriting recognition and stroke order diagrams.", L"Error", MB_OK);
            DisableRecognize();
        }
    }
    else if (!hasstrokes)
    {
        MsgBox(L"Couldn't load stroke order datafile. Get the full version if you want to use the handwriting recognition and stroke order diagrams.", L"Error", MB_OK);
        DisableRecognize();
    }
    fInfo->ElementsChanged(elements->Count);

    lbLoadMsg->Caption = L" Loading stroke order diagrams...";
    pLoad->Refresh();

    lbLoadMsg->Caption = L" Loading UI settings...";
    Refresh();

    fill_menu();
    LoadUISettings();

    if (settings.debug)
    {
        lbVer->Caption = lbVer->Caption/* + L" +debug"*/;
        data->aStrokeEdit->Visible = true;
        data->aJLPTEdit->Visible = true;
        data->aBeta->Visible = true;
    }

    lbLoadMsg->Caption = L" Restoring window positions...";
    pLoad->Refresh();

    if (settings.positionrestore && !settings.firstrun)
    {
        fClipboard->SizeInfo = fsi[2];
        fInfo->SizeInfo = fsi[3];
        fClipboard->RestorePosition(true, true);
        fInfo->RestorePosition(false, true);
    }
    else
    {
        fInfo->Left = (double)(Screen->Width - fInfo->Width) / 2.0;
        fInfo->Top = Screen->Height / 4;
        fInfo->SaveFormSize(true);

        fClipboard->Left = fInfo->Width;
        fClipboard->Top = Screen->Height / 3;
        fClipboard->SaveFormSize(true);
    }

    FILE *f;
    if (settings.searchsave && FileExists(paths.inipath + L"zkanji.uni") && (f = _wfopen((paths.inipath + L"zkanji.uni").c_str(), L"rb")) != NULL)
    {
        try
        {

            lbLoadMsg->Caption = L" Reading dictionary search history...";
            pLoad->Refresh();

            romajihistory->LoadFromFile(f);
            kanjihistory->LoadFromFile(f);
        }
        catch (...)
        {
        }
        fclose(f);
    }

    if (jfonts->Count == 0)
        MsgBox("The system doesn't seem to have any Japanese fonts installed. Without them, Japanese characters will show up as boxes, and you won't be able to make full use of the program.", "Message", MB_OK);

    AddTerminateProc(&terminateproc);

    PostMessage(Handle, WM_Z_HIDE, 0, 0);
}
//---------------------------------------------------------------------------
void TfMain::z_hide(TMessage &msg)
{
    if (FormState.Contains(fsShowing))
    {
        PostMessage(Handle, WM_Z_HIDE, 0, 0);
        return;
    }

    Hide();

    pLoad->Visible = false;

    bbHide->Visible = true;

    hint = new THintFormW(this);
    hint8 = new THintWindow(this);
    kanjihint = new TKanjiHint();

    bool dictupdated = false;

    // Update English dictionary with new main dictionary if necessary.
    if (memcmp(&user_date, &dictionary_date, sizeof(SYSTEMTIME)))
    {
        DISABLEMOD

        TWordStateList *states = new TWordStateList();
        states->Build(dictionary->Items[0], dictionary->Originals);
        TWordCollection *coll = new TWordCollection();
        load_dictionary(paths.installdatapath + L"English.zkj", coll, true, true);

        TGapList<int> changes;
        TWordIndexList *list = new TWordIndexList(coll);
        for (int ix = 0; ix < states->Count; ++ix)
        {
            changes.Items[states->Items[ix]->windex] = FindWordIndex(list, states->Words[ix]->kanji, states->Words[ix]->kana);
            list->Clear();
        }
        delete list;
        if (states->Convert(changes, coll,
                 L"The main dictionary has changed. Some words in user groups or study data conflict with the new dictionary. You will be able to resolve the conflicts that zkanji cannot automatically handle in the next dialog, OR you can abort the dialog and continue using zkanji with the old dictionary",
                 L"If you abort the dictionary update process, zkanji will continue to use the old dictionary and this dialog will be shown the next time you run the program. Are you sure?"))
        {
            states->Commit();
            dictionary->Items[0]->Modify(true);
            dictupdated = true;
        }
        else
            memcpy(&dictionary_date, &user_date, sizeof(SYSTEMTIME));

        delete coll;
        delete states;
    }

    if (dictupdated)
        dictionary->Save();

    fDock->init();

    activelist = new TTemplateList<TBaseForm, false>;
    if (settings.startintray || (settings.intray && settings.remembertray))
    {
        settings.intray = false;
        TMessage msg;
        msg.LParam = 0;
        msg.WParam = 0;
        z_minimize(msg);
    }

    fDock->miExamples->Visible = (paths.inipath == paths.installpath);

    if (settings.autosave)
        StartAutosave(settings.autosavedelay);

    SettingsChangeEnd();

    lastTimedDay = get_day(Now());
    // Start 1 second timer.
    SetTimer(Handle, 1, 1000, NULL);

    if (!hasexamples)
    {
        fDict->disable_examples();
        fPopupDict->disable_examples();
    }

    inited = 2;

    if (!settings.intray)
    {
        fDock->Show();
        SetForegroundWindow(fDock->Handle);
    }
    else if (!settings.minimizetotray)
    {
        ShowWindow(fDock->Handle, SW_SHOWMINIMIZED);
        fDock->Visible = true;
    }
    Application->ProcessMessages();

    if (settings.firstrun)
        MsgBox(L"This is FREE and open-source software! If you encounter a bug, please\ntell me about it!\n\nThis window won't come up again, unless you delete or modify the .ini file.", ((UnicodeString)L"zkanji v" + Version_String + L" - Message"), MB_OK);

    bool firstcheck = settings.checkinterval < 0;
    if (firstcheck)
    {
        if (MsgBox(L"Would you like to let zkanji automatically check for updates online?\n\n(You can change how zkanji behaves in the settings.)", ((UnicodeString)L"zkanji v" + Version_String + L" - Message"), MB_YESNO) == IDYES)
            settings.autocheck = true;
        settings.checkinterval = 0;
    }

    activelist->Insert(0, fKanji);
    activelist->Insert(0, fGroup);
    activelist->Insert(0, fDict);

    if (settings.autocheck)
    {
        bool check = firstcheck;

        if (!firstcheck)
        {
            switch (settings.checkinterval)
            {
            case 0: // Check on startup every time.
                check = true;
                break;
            case 1: // Weekly.
                check = settings.lastcheck + 7 <= (int)Now();
                break;
            case 2: // Monthly.
                check = (double)settings.lastcheck + 30.42 <= (int)Now();
                break;
            case 3: // Every 3 months.
                check = (double)settings.lastcheck + (30.42) * 3 <= (int)Now();
                break;
            }
        }

        if (check)
            CheckForUpdates(false);
    }

    if (!ScreenSaverOn() && profilesettings.kwarntime == kwtStartup)
    {
        int duecnt = 0;
        int duemax = 0;
        for (int ix = dictionary->Count - 1; ix >= 0; --ix)
        {
            int due = dictionary->Items[ix]->wordstudylist->itemsdue();
            duecnt += due;
            duemax = max(duemax, due);
        }
        if (duemax >= profilesettings.kwarnthres)
            BubbleWarning(duecnt);
    }

    fPopupDict->Recreate(false);

    if (ParamCount() == 1 && ParamStr(1).Pos(L"-p=") == 1 && ParamStr(1).Length() == 4)
    {
        int p = max(0, ParamStr(1)[4] - '0');
        if (p > 0 && p <= 3)
            PostMessage(Handle, WM_Z_RESTORE, p, 0);
    }
    else
        SetTaskbarTasks();
}
//---------------------------------------------------------------------------
void TfMain::SetTaskbarTasks()
{
    if (!MatchVersion(6, 1))
        return;

    TUpdateTasksThread::SetTaskbarTasks();
}
//---------------------------------------------------------------------------
void TfMain::TUpdateTasksThread::SetTaskbarTasks()
{
    if (waiting || TUpdateTasksThread::langname == dictionary->Strings[settings.dictionary_index])
        return;
    waiting = true;
    try
    {
        while (thread)
            Application->ProcessMessages();
        TUpdateTasksThread::langname = dictionary->Strings[settings.dictionary_index];
        thread = new TUpdateTasksThread();
    }
    catch (...)
    {
        ;
    }
    waiting = false;
}
//---------------------------------------------------------------------------
__fastcall TfMain::TUpdateTasksThread::TUpdateTasksThread() : inherited()
{
    FreeOnTerminate = true;
}
//---------------------------------------------------------------------------
__fastcall TfMain::TUpdateTasksThread::~TUpdateTasksThread()
{
    thread = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfMain::TUpdateTasksThread::Execute()
{
    TTaskbar bar(TasklistAppID);

    //bar->ClearTasks();
    //bar->AddTask(L"Open zkanji",ParamStr(0).c_str(),L"",L"Restore the zkanji main window",0);
    //bar->AddSeparator();
    bar.AddTask(L"Japanese to " + langname, ParamStr(0).c_str(), L"-p=1", L"Show the popup dictionary in Japanese to " + langname + L" mode.", 1);
    bar.AddTask(langname + L" to Japanese", ParamStr(0).c_str(), L"-p=2", L"Show the popup dictionary in " + langname + L" to Japanese mode.", 2);
    bar.AddSeparator();
    bar.AddTask(L"Show kanji list", ParamStr(0).c_str(), L"-p=3", L"Show the popup kanji list", 3);
    bar.Commit();
}
//---------------------------------------------------------------------------
void TfMain::NoStrokeText()
{
    wchar_t *c = new wchar_t[wcslen(Stroke_Filename) + wcslen(nostroketext) - 1];
    try
    {
        swprintf(c, nostroketext, Stroke_Filename);
        lbStrokes->Caption = c;
    }
    catch (...)
    {
        ;
    }
    delete[] c;
}
//---------------------------------------------------------------------------
void TfMain::HasStrokeText()
{
    lbStrokes->Caption = stroketext;
}
//---------------------------------------------------------------------------

extern wchar_t *compare_a_w;
extern wchar_t *compare_b_w;
extern wchar_t *sort_romanized_w;

//---------------------------------------------------------------------------
int DoTerminate()
{
    if (interminating)
        return 0;
    interminating = true;

    error_code = 0;

    try
    {
        dictionary->ResetChange();
        dictionary->Save();
        dictionary->RestoreChange();

// Moved code to SaveSettings().
//
//        if (settings.positionrestore)
//        {
//            fClipboard->SaveFormSize();
//            error_code = 32;
//
//            fInfo->SaveFormSize();
//            error_code = 33;
//
//            fsi[2] = fClipboard->SizeInfo;
//            error_code = 37;
//
//            fsi[3] = fInfo->SizeInfo;
//            error_code = 38;
//        }
//
//        fDict->SaveSettings();
//        error_code = 47;
//
//        fPopupDict->SaveSettings();
//        error_code = 48;
//
//        fGroup->SaveSettings();
//        error_code = 49;

        SaveSettings();
        error_code = 50;

        delete Examples;
        Examples = NULL;
        error_code = 51;

        FILE *f;
        if (settings.searchsave && (f = _wfopen((paths.inipath + L"zkanji.uni").c_str(), L"wb")) != NULL)
        {
            try
            {
                romajihistory->SaveToFile(f);
                kanjihistory->SaveToFile(f);
            }
            catch (...)
            {
                ;
            }
            fclose(f);
        }
        delete romajihistory;
        delete kanjihistory;
        romajihistory = 0;
        kanjihistory = 0;

        error_code = 0;
    }
    catch (...)
    {
        // Exception can occur if we are in the middle of a save alread. In that case doing nothing is accepted.
       ;
    }
    return error_code;
}
//---------------------------------------------------------------------------
void __fastcall TfMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (Visible)
        return;

    if (fDock && fDock->Visible)
    {
        fDock->Hide();
        UndockWindows(false);
    }

    error_code = 0;

    try
    {
        if (fPopupDict->Visible)
            fPopupDict->Popup(false);
        error_code = 1;
        ::KillTimer(Handle, 1);
        error_code = 2;

        if (!interminating)
        {
            error_code = DoTerminate(); //save settings before anything else
            if (error_code)
                THROW(L"Error");
        }

        delete activelist;
        activelist = NULL;
        error_code = 3;

        delete kanjihint;
        kanjihint = NULL;
        error_code = 4;

        delete hint;
        hint = NULL;
        error_code = 5;

        delete hint8;
        hint8 = NULL;
        error_code = 6;

        (appwndunicode ? SetWindowLongPtrW : SetWindowLongPtrA)(Application->Handle, GWL_WNDPROC, (long)oldwnd);
        error_code = 7;

        delete kanjieditlist;
        kanjieditlist = NULL;
        error_code = 8;

        delete kanjigrids;
        kanjigrids = NULL;
        error_code = 9;

        delete menu;
        menu = 0;
        error_code = 10;

        delete radlist;
        radlist = 0;
        error_code = 11;

        delete[] sort_romanized_w;
        error_code = 12;

        delete[] compare_a_w;
        error_code = 13;

        delete[] compare_b_w;
        error_code = 14;

        UnregisterHotKey(Application->Handle, 0);
        UnregisterHotKey(Application->Handle, 1);
        UnregisterHotKey(Application->Handle, 2);
        error_code = 15;

        fClipboard->Visible = false;
        error_code = 42;

        fInfo->Visible = false;
        error_code = 43;

        fKanji->Visible = false;
        error_code = 40;

        fDict->Visible = false;
        error_code = 41;
        fRecognize->Visible = false;
        error_code = 44;

        fGroup->Visible = false;
        error_code = 45;

        error_code = 46;

        delete wordfilters;
        error_code = 501;

        delete dictionaryfilters;
        delete kanjiwordfilters;
        delete wordgroupfilters;
        error_code = 502;

        error_code = 51;

        for (int i = 0; i < 252; i++)
            delete[] partlist[i];
        error_code = 53;

        delete models;
        models = 0;
        error_code = 54;

        delete cmodels;
        cmodels = 0;
        error_code = 545;

        delete elements;
        elements = 0;
        error_code = 55;

        delete modallist;
        modallist = 0;

        error_code = 56;

        delete[] validkanji;
        delete[] validcode;

        error_code = 57;

        delete sitelist;

        error_code = 58;

        //delete Examples;
        //Examples = NULL;

        error_code = 59;

        //delete JLPT;
        //JLPT = NULL;

        error_code = 60;

        delete store;
        store = NULL;

        error_code = 70;

        delete[] fsi;
        fsi = NULL;

        delete gdi;
        error_code = 0;

    }
    catch (...)
    {
        MsgBox((UnicodeString)L"Error during exit. Code #" + error_code, L"Error", MB_OK);
    }

    ACME->Close();
}
//---------------------------------------------------------------------------
void __fastcall TfMain::FormDestroy(TObject *Sender)
{
    freefonts();

    freeing_user_data = true;
    dictionary->FreeUp();
    freeing_user_data = false;
    dictionary = 0;

    free_kanji();
    delete kanjis;
    delete profile;
    kanjis = 0;
    profile = 0;

    delete wordgroupevents;
    wordgroupevents = 0;
    delete settingevents;
    settingevents = 0;
    delete appevents;
    appevents = 0;
}
//---------------------------------------------------------------------------
void TfMain::WMNCCalcSize(TMessage &msg)
{
    if (BorderStyle == Forms::bsNone)
    {
        msg.Result = DefWindowProc(Handle, msg.Msg, msg.WParam, msg.LParam);
        return;
    }

    if (msg.WParam)
    {
        msg.Result = 0;
        return;
    }
    inherited::Dispatch(&msg);
}
//---------------------------------------------------------------------------
void TfMain::z_composition(TMessage &msg)
{
    if (!MatchVersion(6, 0))
    {
        msg.Result = DefWindowProc(Handle, msg.Msg, msg.WParam, msg.LParam);
        return;
    }

    int compenabled = true;
    DwmIsCompositionEnabled(&compenabled);
    if (compenabled && BorderStyle != Forms::bsSingle)
    {
        GlassFrame->Enabled = true;
        BorderStyle = Forms::bsSingle;
    }
    else if (!compenabled && BorderStyle != Forms::bsNone)
    {
        GlassFrame->Enabled = false;
        BorderStyle = Forms::bsNone;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfMain::bbHideClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TfMain::FormCreate(TObject *Sender)
{
    if (!TestRun)
        Caption = "zkanji";
    else
        Caption = "zkanji test version";

    HasStrokeText();

    reJMDICT->Color = pJMDICT->Color;
    reJMDICT->SetBounds(pJMDICT->Left + 6 * scaleres, pJMDICT->Top + 6 * scaleres, pJMDICT->Width - 12 * scaleres, pJMDICT->Height - 12 * scaleres);

    int mask = SendMessage(reJMDICT->Handle, EM_GETEVENTMASK, 0, 0);
    SendMessage(reJMDICT->Handle, EM_SETEVENTMASK, 0, mask | ENM_LINK);
    SendMessage(reJMDICT->Handle, EM_AUTOURLDETECT, TRUE, 0);

    if (!FileExists(paths.installdatapath + Stroke_Filename))
        NoStrokeText();

    reJMDICT->Lines->SetText(jmdicttext);
}
//---------------------------------------------------------------------------
void FindKanjiWord(TWordIndexList *list, UnicodeString c, bool inflections, TWordFilterSelection *filter)
{
    // Remove excess '+' from end.
    int clen = c.Length();
    while (clen > 1 && c[clen] == L'+' && c[clen - 1] == L'+')
        clen--;
    if (clen != c.Length())
        c = c.SubString(1, clen);
    if (!clen)
        return;

    // Find kanji or valid code character in search with least number of associated words.
    TKanjiData *k;
    TValidData *v;
    int which = -1;
    int n = 99999;
    int pos;

    bool codechar = false; // It's a valid coded character, not a kanji.

    for (int i = 1; i <= c.Length(); i++)
    {
        if (KANJI(c[i]))
        {
            pos = kanjis->IndexOf(c[i]);
            if (pos >= 0 && list->collection->kanjidat[pos].wordscnt < n)
            {
                codechar = false;
                which = pos;
                n = list->collection->kanjidat[pos].wordscnt;
                if (n < 200)
                    break; // No need to look for less.
            }
        }
        else if (VALIDCODE(c[i]))
        {
            pos = validindex(c[i]);
            if (list->collection->validdat[pos].wordscnt < n)
            {
                codechar = true;
                which = pos;
                n = list->collection->validdat[pos].wordscnt;
                if (n < 200)
                    break; // No need to look for less.
            }
        }
    }

    if (which < 0)
        return;

    if (codechar)
    {
        v = list->collection->validdat + which;
        if (!v)
            return;
    }
    else
    {
        k = list->collection->kanjidat + which;
        if (!k)
            return;
    }

    TWordInfList *l = new TWordInfList;
    int type = 0;

    c = Hiraganize(c.c_str());
    clen = c.Length();
    if (inflections)
        DeInflect(l, (c[clen] == L'+' ? c.SubString(1, clen - 1) : c), type);

    int cnt = codechar ? v->wordscnt : k->wordscnt;

    for (int j = 0; j < cnt; j++)
    {
        TWord *w;
        int windex;
        if (codechar)
            windex = v->words[j];
        else
            windex = k->words[j];
        w = list->collection->words->Items[windex];
        if (filter && !filter->match(w))
            continue;
        if (reg_match(w->kanji, c.c_str()))
            list->Add(windex);
        else /*if (list->collection->type == wctWords || list->collection->type == wctBase)*/
        {
            for (int k = 0; k < w->meaningcnt; ++k)
            {
                if (type & ((TMeaningData*)w->data)[k].types)
                {
                    // We have a deinflected word of this type.
                    for (int i = 0; i < l->Count; i++)
                    {
                        if ((1 << (l->Items[i]->type)) & ((TMeaningData*)w->data)[k].types && list->IndexOf(windex) < 0 && reg_match(w->kanji, l->Items[i]->str))
                            list->AddInflected(windex, l->Items[i]->inf);
                    }
                }
            }
        }
    }

    delete l;

    list->KanjiSort();
}
//---------------------------------------------------------------------------
bool reg_match2(const wchar_t *w, const wchar_t *exp);

bool reg_match(const wchar_t *w, const wchar_t *exp)
{
    UnicodeString k = Hiraganize(w);
    UnicodeString e;

    int elen = wcslen(exp);
    for (int i = 0; i < elen; i++)
    {
        if (exp[i] == L'+' || JAPAN(exp[i]))
            e += exp[i];
    }

    return reg_match2(k.c_str(), e.c_str());
}
//---------------------------------------------------------------------------
// Return true when exp expression matches the w string.
bool reg_match2(const wchar_t *w, const wchar_t *exp)
{
    bool skip = false;
    int wpos = 0, epos = 0;
    int wlen = wcslen(w);
    int elen = wcslen(exp);

    while (wpos < wlen && epos < elen)
    {
        while (exp[epos] == L'+')
        {
            skip = true;
            epos++;
        }

        if (skip)
        {
            while (wpos < wlen && w[wpos] != exp[epos])
                wpos++;

            if (wpos == wlen)
                return epos == elen;

            if (reg_match2(w + wpos + 1, exp + epos - 1))
                return true;
            skip = false;
        }
        if (w[wpos] != exp[epos])
            return false;
        wpos++;
        epos++;
    }

    return (wpos == wlen && (epos == elen || epos == elen - 1 && exp[epos] == L'+'));
}
//---------------------------------------------------------------------------
bool GetClipText(UnicodeString &c, int maxlen)
{
    if (!OpenClipboard(fMain->Handle))
        return false;
    bool didit = false;
    try
    {
        if (IsClipboardFormatAvailable(CF_UNICODETEXT))
        {
            void *h = GetClipboardData(CF_UNICODETEXT);
            if (!h)
            {
                int e = GetLastError();
                CloseClipboard();
                if (settings.debug)
                    MsgBox((UnicodeString)"Error while opening clipboard: " + e, "Error", MB_OK);
                return false;
            }

            didit = true;
            if (maxlen)
            {
                wchar_t *tmp = (wchar_t*)GlobalLock(h);
                maxlen = min(maxlen, wcslen(tmp));
                c = UnicodeString(tmp, maxlen);
            }
            else
                c = (wchar_t*)GlobalLock(h);
            GlobalUnlock(h);
        }
    }
    catch (...)
    {
        didit = false;
    }
    CloseClipboard();
    return didit;
}
//---------------------------------------------------------------------------

void SetClipText(UnicodeString const &c)
{
    if (!OpenClipboard(fMain->Handle))
        return;
    try
    {
        EmptyClipboard();
        // First set unicode format.
        void *h = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, (c.Length() + 1) * 2);
        if (!h)
            THROW(L"Error allocating global memory for clipboard!");
        wchar_t *w = (wchar_t*)GlobalLock(h);
        wcscpy(w, c.c_str());
        GlobalUnlock(h);
        SetClipboardData(CF_UNICODETEXT, h);

        // Set other format.
        int n = WideCharToMultiByte(CP_ACP, NULL, c.c_str(), -1, NULL, 0, 0, 0);
        h = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, n);
        char *s = (char*)GlobalLock(h);
        WideCharToMultiByte(CP_ACP, NULL, c.c_str(), -1, s, n, 0, 0);
        GlobalUnlock(h);
        SetClipboardData(CF_TEXT, h);
    }
    catch (...)
    {
    }
    CloseClipboard();
}
//---------------------------------------------------------------------------
void RestoreFormSize(TBaseForm *f, TFormSizeInfo size, bool dimensions)
{
    f->Left = size.left;
    f->Top = size.top;
    if (dimensions)
    {
        f->Width = size.width;
        f->Height = size.height;
    }
    f->SizeInfo = size;
}
//---------------------------------------------------------------------------
void TfMain::SearchK(TWordIndexList *l, bool start, bool end, bool infl, UnicodeString c, TWordFilterSelection *filter)
{
    FindKanjiWord(l, ((UnicodeString)(start ? L"+" : L"") + c.Trim() + (end ? L"+" : L"")), infl, filter);
}
//---------------------------------------------------------------------------
void TfMain::SearchLatin(TWordIndexList *l, bool start, bool end, bool infl, UnicodeString c, TWordFilterSelection *filter)
{
    SearchW(l, start, end, infl, c, L"", filter);
}
//---------------------------------------------------------------------------
void TfMain::SearchJapan(TWordIndexList *l, bool start, bool end, bool infl, UnicodeString c, TWordFilterSelection *filter)
{
    SearchW(l, start, end, infl, L"", c, filter);
}
//---------------------------------------------------------------------------
void TfMain::SearchW(TWordIndexList *l, bool start, bool end, bool infl, UnicodeString c1, UnicodeString c2, TWordFilterSelection *filter)
{
    int n;

    bool meaning = c1.Length();
    UnicodeString s = "";

    if (!meaning)
    {
        s = TrimRomanized(c2);

    }
    else
        s = c1.LowerCase().Trim();

    n = 0;
    for (int i = 0; i < s.Length(); i++)
    {
        // Count valuable length of word, don't add ' from single n characters.
        if (!wcschr(cdelim, s[i + 1]))
            n++;
        else if (s[i + 1] == ' ')
            n = 0;
        if (n > 2)
            break;
    }

    if (n)
    {
        // For too short words look for exact match.
        if ((meaning || !wcschr(L"aiueo", s[1])) && n <= 1)
            start = end = false;

        FindWord(s.c_str(), l, !meaning, !(!meaning ? (end || start) : end), !meaning && start, infl, filter);
    }

}
//---------------------------------------------------------------------------
void TfMain::StartAutosave(int mins)
{
    lastAutosave = secTimer;
    secAutosave = mins * 60;
}
//---------------------------------------------------------------------------
void TfMain::StopAutosave()
{
    secAutosave = -1;
}
//---------------------------------------------------------------------------
void TfMain::WMTimer(TMessage &msg)
{
    ++secTimer;
    ++checkTimer;
    if (inTimer)
        return;
    inTimer = true;

    if (secAutosave > 0 && lastAutosave + secAutosave <= secTimer && !dictionary->save_in_progress && !dictionary->Changing && !dictionary->SaveDisabled)
    {
        lastAutosave = secTimer;
        try
        {
            dictionary->Save();
        }
        catch (...)
        {
            ;
        }
    }

    fDict->Time(secTimer);
    fPopupDict->Time(secTimer);

    TDateTime n = Now();

    if (!dictionary->save_in_progress)
        dictionary->CreateBackup(n, false);

    int dn = get_day(n);
    if (dn != lastTimedDay)
    {
        // Study day changed.
        for (int i = 0; i < Screen->FormCount; i++)
            PostMessage(Screen->Forms[i]->Handle, WM_Z_DAYCHANGED, lastTimedDay, dn);
        lastTimedDay = dn;
    }

    if (profilesettings.kwarntime != kwtNever && profilesettings.kwarntime != kwtStartup && !ScreenSaverOn())
    {
        int duecnt = 0;
        int duemax = 0;
        for (int ix = dictionary->Count - 1; ix >= 0; --ix)
        {
            int due = dictionary->Items[ix]->wordstudylist->itemsdue();
            duecnt += due;
            duemax = max(duemax, due);
        }
        if (duemax >= profilesettings.kwarnthres)
        {
            int after = 0;
            switch (profilesettings.kwarntime)
            {
            case kwt1:
                after = 1;
                break;
            case kwt2:
                after = 2;
                break;
            case kwt6:
                after = 6;
                break;
            case kwt24:
                after = 24;
                break;
            }
            if (double(n) - double(after) / 24.0 > profilesettings.klastwarning)
                BubbleWarning(duecnt);
        }
    }

    if (!fUpdate && checkTimer >= 3600)
    {
        bool check = false;
        checkTimer = 0;

        switch (settings.checkinterval)
        {
//        case 0: // Check on startup every time.
//            check = true;
//            break;
        case 1: // Weekly.
            check = settings.lastcheck + 7 <= (int)Now();
            break;
        case 2: // Monthly.
            check = (double)settings.lastcheck + 30.42 <= (int)Now();
            break;
        case 3: // Every 3 months.
            check = (double)settings.lastcheck + (30.42) * 3 <= (int)Now();
            break;
        }

        if (check)
            CheckForUpdates(false);
    }


    inTimer = false;
}
//---------------------------------------------------------------------------
void RectToPoints(TRect r, TPoint pts[5])
{
    pts[0] = Point(r.Left, r.Top);
    pts[1] = Point(r.Right - 1, r.Top);
    pts[2] = Point(r.Right - 1, r.Bottom - 1);
    pts[3] = Point(r.Left, r.Bottom - 1);
    pts[4] = pts[0];
}
void TfMain::z_init(TMessage &msg)
{
    Init();
}
//---------------------------------------------------------------------------
void TfMain::z_themechange(TMessage &msg)
{
    TThemeServices *t = ThemeServices();
    ThemesAreTurnedOn = settings.windowdecoration && t && t->ThemesAvailable && t->ThemesEnabled;
    for (int ix = 0; ix < Screen->FormCount; ++ix)
        Screen->Forms[ix]->Invalidate();
}
//---------------------------------------------------------------------------
void TfMain::Minimize(TMinimizeAction maction)
{
    if (fInfo->Visible)
        fInfo->Close();
    if (fClipboard->Visible)
        fClipboard->Close();

    PostMessage(Handle, WM_Z_MINIMIZE, maction, 0);
}
//---------------------------------------------------------------------------
void TfMain::Restore()
{
    if (!settings.intray)
        THROW(L"Can't restore not minimized windows!");

    settings.intray = false;
    UnregisterHotKey(Application->Handle, 0);
    UnregisterHotKey(Application->Handle, 1);
    UnregisterHotKey(Application->Handle, 2);

    SendMessage(fMain->Handle, WM_Z_DOCK, 0, 0);

    if (fDock && settings.minimizetotray)
    {
        fDock->Show();
        SetForegroundWindow(fDock->Handle);
    }
}
//---------------------------------------------------------------------------
void TfMain::z_restore(TMessage &msg)
{
    ReleaseCapture();
    if (modallist->Count || msg.WParam < 1 || msg.WParam > 3)
    {
        data->Restore();
        return;
    }

    Application->BringToFront();
    switch (msg.WParam)
    {
    case 1:
        if (!settings.intray)
            Minimize(maPopupJE);
        else
        {
            fPopupDict->Popup(true, dmKanji);
            SetForegroundWindow(fPopupDict->Handle);
        }
        break;
    case 2:
        if (!settings.intray)
            Minimize(maPopupEJ);
        else
        {
            fPopupDict->Popup(true, dmRomaji);
            SetForegroundWindow(fPopupDict->Handle);
        }
        break;
    case 3:
        if (!settings.intray)
            Minimize(maKanji);
        else
        {
            fKanji->Popup(true, true);
            SetForegroundWindow(fKanji->Handle);
        }
        break;
    }
}
//---------------------------------------------------------------------------
void TfMain::z_minimize(TMessage &msg)
{
    if (modallist->Count || settings.intray)
        return;

    HideKanjiHintNow();

    settings.intray = true;

    if (fPopupDict->Visible || fKanji->ontop)
    {
        if (fKanji->ontop)
            fKanji->Popup(false);
        if (fPopupDict->Visible)
            fPopupDict->Popup(false);
    }
    else
    {
        if (settings.minimizetotray)
            fDock->Hide();
        else if (fDock->WindowState != wsMinimized)
            fDock->WindowState = wsMinimized;
        SendMessage(fMain->Handle, WM_Z_UNDOCK, 0, 0);

        data->Minimize();

        if (settings.syskeys)
        {
            RegisterHotKey(Application->Handle, 0, (settings.JKeys == 0 || settings.JKeys == 2 ? MOD_CONTROL : 0) | (settings.JKeys == 0 || settings.JKeys == 1 ? MOD_ALT : 0) | (settings.JShift ? MOD_SHIFT : 0), (int)settings.JChar);
            RegisterHotKey(Application->Handle, 1, (settings.EKeys == 0 || settings.EKeys == 2 ? MOD_CONTROL : 0) | (settings.EKeys == 0 || settings.EKeys == 1 ? MOD_ALT : 0) | (settings.EShift ? MOD_SHIFT : 0), (int)settings.EChar);
            RegisterHotKey(Application->Handle, 2, (settings.KKeys == 0 || settings.KKeys == 2 ? MOD_CONTROL : 0) | (settings.KKeys == 0 || settings.KKeys == 1 ? MOD_ALT : 0) | (settings.KShift ? MOD_SHIFT : 0), (int)settings.KChar);
        }

        switch (msg.WParam)
        {
        case maPopupJE:
            fPopupDict->Popup(true, dmKanji);
            break;
        case maPopupEJ:
            fPopupDict->Popup(true, dmRomaji);
            break;
        case maKanji:
            fKanji->Popup(true, true);
            break;
        }

    }
}
//---------------------------------------------------------------------------
void TfMain::z_dock(TMessage &msg)
{
    DockWindows(msg.WParam);
}
//---------------------------------------------------------------------------
void TfMain::z_undock(TMessage &msg)
{
    UndockWindows(msg.WParam);
}
//---------------------------------------------------------------------------
void TfMain::z_modal(TMessage &msg)
{
    if (inited < 2)
        return;

    if (!modallist->Count)
        THROW(L"No modal list created!");

    fRecognize->Hide();

    if (settings.intray)
    {
        fPopupDict->OnTop = false;
        if (fKanji->Visible)
            SetWindowPos(fKanji->Handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }
    HWND h = GetForegroundWindow();
    modallist->UpdateTop(h);
}
//---------------------------------------------------------------------------
void __fastcall TfMain::WndProc(TMessage &msg)
{
    if (msg.Msg == WM_NOTIFY)
    {
        TWMNotify *nmsg = ((TWMNotify*) &msg);
        if (nmsg->NMHdr->code == EN_LINK)
        {
            TENLink *el = (TENLink*)nmsg->NMHdr;
            if (el->msg == WM_LBUTTONDOWN)
            {
                PostMessage(nmsg->NMHdr->hwndFrom, WM_LBUTTONUP, msg.WParam, msg.LParam);
                if (nmsg->NMHdr->hwndFrom == reJMDICT->Handle)
                {
                    TRichEdit *re = reJMDICT;
                    SendMessage(nmsg->NMHdr->hwndFrom, EM_EXSETSEL, 0, (int)(&el->chrg));
                    UnicodeString s = re->SelText;
                    showlink(s);
                    re->SelLength = 0;
                }
            }
        }
    }
    TBaseForm::WndProc(msg);
}
//---------------------------------------------------------------------------
void __fastcall TfMain::FormMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (!bbHide->Visible)
        return;

    ReleaseCapture();
    DefWindowProc(Handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}
//---------------------------------------------------------------------------
void TfMain::z_closeupdate(TMessage &msg)
{
    if (!fUpdate || fUpdate->Visible)
        return;

    delete fUpdate;
    fUpdate = NULL;

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ConstraintSizing(TWMSizing &msg, int minwidth, int minheight, int maxwidth, int maxheight)
{
    ConstraintSizing(msg, minwidth, minheight, maxwidth, maxheight, 0, 0, 0, 0);
}
//---------------------------------------------------------------------------
void ConstraintSizing(TWMSizing &msg, int minwidth, int minheight)
{
    ConstraintSizing(msg, minwidth, minheight, 999999, 999999, 0, 0, 0, 0);
}
//---------------------------------------------------------------------------
void ConstraintSizing(TWMSizing &msg, int minwidth, int minheight, int maxwidth, int maxheight, int widthdiff, int modx, int heightdiff, int mody)
{
    int base = MIN(maxwidth, MAX(minwidth, (int)(msg.lprc->Right - msg.lprc->Left))); //the new width
    if (modx)
    {
        int w = base - widthdiff;
        if (w % modx < modx / 2)
            base -= w % modx;
        else
            base += modx - (w % modx);
    }

    switch (msg.fwSide)
    {
    case (WMSZ_BOTTOMLEFT):
    case (WMSZ_TOPLEFT):
    case (WMSZ_LEFT):
        msg.lprc->Left = msg.lprc->Right - base;
        break;
    case (WMSZ_BOTTOMRIGHT):
    case (WMSZ_TOPRIGHT):
    case (WMSZ_RIGHT):
        msg.lprc->Right = msg.lprc->Left + base;
        break;
    }

    base = MIN(maxheight, MAX(minheight, (int)(msg.lprc->Bottom - msg.lprc->Top)));

    if (mody)
    {
        int h = base - heightdiff;
        if (h % mody < mody / 2)
            base -= h % mody;
        else
            base += mody - (h % mody);
    }

    switch (msg.fwSide)
    {
    case (WMSZ_TOPLEFT):
    case (WMSZ_TOPRIGHT):
    case (WMSZ_TOP):
        msg.lprc->Top = msg.lprc->Bottom - base;
        break;
    case (WMSZ_BOTTOMLEFT):
    case (WMSZ_BOTTOMRIGHT):
    case (WMSZ_BOTTOM):
        msg.lprc->Bottom = msg.lprc->Top + base;
        break;
    }

    msg.Result = 1;
}
//---------------------------------------------------------------------------
void EditWordDef(TWordCollection *collection, int wordindex, char wordmeaning)
{
    fRedefine = new TfRedefine(Application);
    fRedefine->ShowModalEx(collection, wordindex, wordmeaning);
    delete fRedefine;
}
//---------------------------------------------------------------------------
void DeleteWordEntry(byte collection, int wordindex, char meaning)
{
    if (meaning < 0 || dictionary->Items[collection]->words->Items[wordindex]->meaningcnt == 1)
    {
        if (MsgBox("The selected word will be removed from this dictionary and from every group\nin the dictionary that contains it. Study statistics will be removed as well.\n\nDo you wish to continue?", "Warning", MB_YESNO) == IDNO)
        {
            return;
        }
        MODON
        dictionary->BeginWordChange(collection);
        try
        {
            dictionary->DeleteWord(collection, wordindex);
        }
        catch (...)
        {
            ;
        }
        dictionary->EndWordChange();
        dictionary->Items[collection]->Modify(true);
        MODOFF
    }
    else
    {
        if (MsgBox("The selected meaning will be removed from this dictionary and from every group\nin the dictionary that contains it. Study statistics will be removed as well.\n\nDo you wish to continue?", "Warning", MB_YESNO) == IDNO)
        {
            return;
        }
        MODON
        dictionary->BeginWordChange(collection);
        try
        {
            dictionary->DeleteWordMeaning(collection, wordindex, meaning);
        }
        catch (...)
        {
            ;
        }
        dictionary->EndWordChange();
        dictionary->Items[collection]->Modify(true);
        MODOFF
    }
}
//---------------------------------------------------------------------------
void RevertWordEntry(int wordindex)
{
    TWord *w = dictionary->Items[0]->words->Items[wordindex];
    if (w->group != NULL)
    {
        int r = dictionary->Revertable(wordindex);
        if (r != -1)
        {
            if (r == 0 && MsgBox(L"Reverting this word is potentially unsafe because its original form has either different number of meanings, or they are not in the same order as the current meanings. Only continue if you are sure that the meanings in groups are at the same position as in the original word.\n\nAre you sure?", L"Error.", MB_YESNO) != IDYES)
                return;
        }
        else
        {
            MsgBox(L"The word is added to at least one user group with a meaning that does not match any in the original word, and it cannot be safely reverted. Remove it from all groups and try again.", L"Error.", MB_OK);
            return;
        }
    }

    MODON
    dictionary->BeginWordChange(0);
    try
    {
        dictionary->RevertWord(wordindex);
    }
    catch( ...)
    {
        ;
    }
    dictionary->EndWordChange();
    dictionary->Items[0]->Modify(true);
    MODOFF
}
//---------------------------------------------------------------------------
void ScaledTextOutExW(TCanvas *c, float sx, float sy, int x, int y, TRect r, wchar_t *str, int length)
{
    TPoint op;
    TXForm xf;
    SetViewportOrgEx(c->Handle, x, y, &op);

    int ogm = SetGraphicsMode(c->Handle, GM_ADVANCED);
    memset(&xf, 0, sizeof(TXForm));
    xf.eM11 = sx;
    xf.eM22 = sy;
    SetWorldTransform(c->Handle, &xf);

    OffsetRect(r, -x, -y);
    ExtTextOutW(c->Handle, 0, 0, 0, &r, str, length, NULL);

    xf.eM11 = 1;
    xf.eM22 = 1;
    SetWorldTransform(c->Handle, &xf);

    SetGraphicsMode(c->Handle, ogm);

    SetViewportOrgEx(c->Handle, op.x, op.y, NULL);
}
//---------------------------------------------------------------------------
void AngleTextOutEx(TCanvas *c, int angle, int x, int y, TRect r, UnicodeString str)
{
    TLogFontW lr;
    HANDLE OldFontHandle, NewFontHandle;
    GetObject(c->Font->Handle, sizeof(lr), &lr);
    lr.lfOrientation = lr.lfEscapement = angle * 10;
    NewFontHandle = CreateFontIndirectW(&lr);
    OldFontHandle = SelectObject(c->Handle, NewFontHandle);
    ExtTextOutW(c->Handle, x, y, ETO_CLIPPED | (c->Brush->Style != bsClear ? ETO_OPAQUE : 0), &r, str.c_str(), str.Length(), NULL);
    NewFontHandle = SelectObject(c->Handle, OldFontHandle);
    DeleteObject(NewFontHandle);
}
//---------------------------------------------------------------------------
void Repaint(TWinControl *w)
{
    TPoint p = Point(0, 0);
    p = w->ClientToScreen(p);
    TRect r = Rect(w->Left - p.x, w->Top - p.y, w->Width + w->Left - p.x, w->Height + w->Top - p.y);
    RedrawWindow(w->Handle, &r, NULL, RDW_FRAME | RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW);
}
//---------------------------------------------------------------------------
void fill_menu()
{
    UnicodeString sname;

    TMenuItem *mi;
    for (int i = fDock->miDictSelect->Count - 1; i >= 0; --i)
    {
        mi = fDock->miDictSelect->Items[i];
        if (mi->Checked)
            sname = StripHotkey(mi->Caption);
        fDock->miDictSelect->Delete(i);
        delete mi;
    }

    settings.dictionary_index = 0;
    for (int i = 0; i < dictionary->Count; i++)
    {
        if (dictionary->Strings[i] == sname)
        {
            settings.dictionary_index = i;
            break;
        }
    }

    if (fGroup->cbDictionary->ItemIndex >= 0)
        sname = fGroup->cbDictionary->Items->Strings[fGroup->cbDictionary->ItemIndex];
    else
        sname = "";
    fGroup->cbDictionary->Items->Clear();

    for (int i = 0; i < dictionary->Count; i++)
    {
        fGroup->cbDictionary->Items->Add(dictionary->Strings[i]);

        mi = new TMenuItem(fDock->miDictSelect);
        mi->Caption = dictionary->Strings[i];
        mi->Checked = i == settings.dictionary_index;
        mi->Name = "miD" + IntToStr(i);
        mi->RadioItem = true;
        mi->GroupIndex = 1;
        mi->Tag = i;
        mi->OnClick = fDict->miClick;
        mi->ShortCut = TextToShortCut((UnicodeString)"Alt+" + (i + 1));
        if (i < 9)
            mi->ShortCut = TextToShortCut((UnicodeString)"Alt+" + (i + 1));
        fDock->miDictSelect->Add(mi);
    }
    fGroup->cbDictionary->ItemIndex = fGroup->cbDictionary->Items->IndexOf(sname);
}
//---------------------------------------------------------------------------
void DictionaryDeleted(byte coll)
{
    fDict->DictionaryDeleted(coll);

    TMenuItem *mi;
    mi = fDock->miDictSelect->Items[coll];
    fDock->miDictSelect->Delete(coll);
    delete mi;

    for (int i = coll; i < dictionary->Count - 1; ++i)
    {
        mi = fDock->miDictSelect->Items[coll];
        mi->Name = "miD" + IntToStr(i);
        mi->Tag = i;
        mi->ShortCut = ShortCut('1' + i, TShiftState() << ssAlt);
    }
}
//---------------------------------------------------------------------------
void DictionaryAdded(UnicodeString s)
{
    fGroup->cbDictionary->Items->Add(s);
    fGroup->SelectDictionary(fGroup->cbDictionary->Items->Count - 1);

    TMenuItem *mi;

    int i = dictionary->Count - 1;

    mi = new TMenuItem(fDock->miDictSelect);
    mi->Caption = dictionary->Strings[i];
    mi->Checked = false;
    mi->Name = "mi" + IntToStr(i);
    mi->RadioItem = true;
    mi->GroupIndex = 1;
    mi->Tag = i;
    mi->OnClick = fDict->miClick;
    if (i < 9)
        mi->ShortCut = TextToShortCut((UnicodeString)"Alt+" + (i + 1));
    fDock->miDictSelect->Add(mi);
}
//---------------------------------------------------------------------------
void KanjiToGroup(TKanji *k)
{
    TKanjiItemList *kl = new TKanjiItemList;
    try
    {
        kl->Add(k->index);
        KanjiToGroup(kl);
    }
    catch (...)
    {
        ;
    }
    delete kl;
}
//---------------------------------------------------------------------------
void KanjiToGroup(TKanjiItemList *kl)
{
    fKToGroup = new TfKToGroup(Application);
    if (fKToGroup->ShowModalEx(kl) == mrOk)
    {
        int i;
        MODON
        i = fGroup->AddKanjiGroup(fKToGroup->cbDictionary->ItemIndex, fKToGroup->cbGroup->Text.Trim());
        fGroup->KanjiToGroup(fKToGroup->cbDictionary->ItemIndex, i, kl);
        MODOFF
    }
    delete fKToGroup;
}
//---------------------------------------------------------------------------
void GenerateWordGroup(TKanji *k)
{
    TKanjiItemList *kl = new TKanjiItemList();
    kl->Add(k->ch);
    GenerateWordGroup(kl);
    delete kl;
}
//---------------------------------------------------------------------------
void GenerateWordGroup(TKanjiList *kl)
{
    fGen = new TfGen(Application);

    if (fGen->ShowModalEx(kl) == mrOk)
        fGroup->WordsToGroup(fGen->cbDest->ItemIndex, fGen->cbGroups->Text, fGen->base);
    delete fGen;
}
//---------------------------------------------------------------------------
bool InfoWindowVisible()
{
    return (fInfo && fInfo->Visible) || (fClipboard && fClipboard->Visible);
}
//---------------------------------------------------------------------------
extern int ignore_esc_char;
bool HideInfoWindows(bool inkeydown)
{
    if (kanjieditlist)
    {
        for (int i = 0; i < kanjieditlist->Count; i++)
            if (!kanjieditlist->Items[i]->CanInput())
            {
                if (inkeydown)
                    ++ignore_esc_char;
                return true;
            }
    }

    bool r = false;
    if (fInfo && fInfo->Visible)
    {
        r = true;
        fInfo->Close();
    }
    if (fClipboard && fClipboard->Visible)
    {
        r = true;
        fClipboard->Close();
    }
    if (inkeydown && r)
        ++ignore_esc_char;

    return r;
}
//---------------------------------------------------------------------------
/*
 void ImportElements()
 {
 if (elements)
 delete elements;
 elements = NULL;
 import_taka_dir("data\\taka\\",(UnicodeString)"data\\"+Stroke_Filename);
 }
 */
//---------------------------------------------------------------------------
bool wasinfo;
bool wasclipboard;
bool wasallhidden = false;
void HideAllWindows()
{
    if (wasallhidden)
        return;

    wasallhidden = true;
    fDock->Visible = false;
    fKanji->Visible = false;
    wasinfo = fInfo && fInfo->Visible;
    wasclipboard = fClipboard && fClipboard->Visible;
    if (wasinfo)
        fInfo->Visible = false;
    if (wasclipboard)
        fClipboard->Visible = false;
}
//---------------------------------------------------------------------------
void ShowAllWindows()
{
    if (!wasallhidden)
        THROW(L"Windows are not hidden! Bad call!");

    fKanji->Visible = true;
    fDock->Visible = true;
    if (wasinfo)
        fInfo->Visible = true;
    if (wasclipboard)
        ShowClipboard();
    wasallhidden = false;

    Application->BringToFront();
}
//---------------------------------------------------------------------------
void KanaPractice()
{
    fKana = new TfKana(Application);
    try
    {
        fKana->ShowModalEx();
    }
    catch (...)
    {
        ;
    }
    delete fKana;
}
//---------------------------------------------------------------------------
void DisableRecognize()
{
    hasrecdata = false;
    profilesettings.ktypekanji = false;

    if (kanjieditlist)
        for (int ix = 0; ix < kanjieditlist->Count; ++ix)
            kanjieditlist->Items[ix]->UpdateRecognizer();
}
//---------------------------------------------------------------------------
void EnableRecognize()
{
    hasrecdata = true;

    if (kanjieditlist)
        for (int ix = 0; ix < kanjieditlist->Count; ++ix)
            kanjieditlist->Items[ix]->UpdateRecognizer();
}
//---------------------------------------------------------------------------
void ChangeDef(int collection, wchar_t k, int pos)
{
    TKanjiItemList *kl = NULL;
    try
    {
        kl = new TKanjiItemList;
        kl->Add(k);
        ChangeDef(collection, kl, pos);
    }
    catch (...)
    {
        ;
    }
    delete kl;
}
//---------------------------------------------------------------------------
void ChangeDef(int collection, TKanjiList *list, int pos)
{
    fDef = new TfDef(Application);
    int r = fDef->ShowModalEx(collection, list, pos);
    delete fDef;

    if (r == mrCancel)
        return;

    if (collection == settings.dictionary_index)
        fKanji->DefChanged();

    if (collection == fGroup->cbDictionary->ItemIndex)
        fGroup->DefChanged();
}
//---------------------------------------------------------------------------
void __fastcall TfMain::LinkLabel1LinkClick(TObject *Sender, const UnicodeString Link, TSysLinkType LinkType)
{
    showlink(Link);
    //ShellExecuteW(Handle, L"open", Link.c_str(), 0, 0, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TfMain::LinkLabel2LinkClick(TObject *Sender, const UnicodeString Link, TSysLinkType LinkType)
{
    showlink(Link);
    //ShellExecuteW(Handle, L"open", Link.c_str(), 0, 0, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void TfMain::showlink(UnicodeString link)
{
    wchar_t *l = new wchar_t[link.Length() + 1];
    wcscpy(l, link.c_str());
    PostMessage(Handle, WM_Z_LINK, (int)l, 0);
}
//---------------------------------------------------------------------------
void TfMain::z_link(TMessage &msg)
{
    ShellExecuteW(Handle, L"open", (wchar_t*)msg.WParam, 0, 0, SW_SHOWNORMAL);
    delete[] (wchar_t*)msg.WParam;
}
//---------------------------------------------------------------------------
extern bool runupdate;
extern bool dictupdate;
void TfMain::z_update(TMessage &msg)
{
    if (!runupdate)
        return;
    while (fUpdate != NULL)
        Application->ProcessMessages();

    if (!dictupdate)
        Close();
    else
    {
        if (fMsg == NULL)
        {
            dictionary->Save();

            wchar_t temppath[MAX_PATH];
            GetTempPath(MAX_PATH, temppath);
            UnicodeString dlfilename = UnicodeString(temppath) + "zkanjisetupdict.exe";
            if (FileExists(dlfilename, false))
                ShellExecuteW(NULL, L"open", dlfilename.c_str(), (L"/S /D=" + paths.installpath).c_str(), ExtractFilePath(dlfilename).c_str(), SW_SHOWNORMAL);
            else
            {
                dictupdate = false;
                runupdate = false;
                MsgBox(L"Can't find update on disk. Please try again.", L"Error", MB_OK);
                return;
            }

            ShowMessageForm(L"Please wait while the main dictionary is being updated.");
        }
        else
        {
            bool updated = false;

            dictupdate = false;
            runupdate = false;

            Examples->clear_cache();

            // The dictionary setup sends us a WM_Z_UPDATE message once it finished. The WParam is 1 on success.
            if (msg.WParam != 1)
            {
                HideMessageForm();
                MsgBox(L"Couldn't update main dictionary. If the program doesn't start correctly the next time, install it again to its current location.", L"Error", MB_OK);
            }
            else
            {
                DISABLEMOD

                SYSTEMTIME tmpdate;
                double tmpver;
                memcpy(&tmpdate, &dictionary_date, sizeof(SYSTEMTIME));
                tmpver = dictionary_build_version;
                try
                {
                    if (!load_dictionary_base(paths.installdatapath + L"zdict.zkj"))
                        THROW(L"Couldn't load updated dictionary data.");
                }
                catch(...)
                {
                    memcpy(&dictionary_date, &tmpdate, sizeof(SYSTEMTIME));
                    dictionary_build_version = tmpver;
                    HideMessageForm();
                    MsgBox(L"Corrupted data file or unsupported dictionary format. Re-install the program to its current location before running it again.", L"Error", MB_OK);
                    return;
                }

                // Load the updated dictionary
                TWordStateList *states = new TWordStateList();
                states->Build(dictionary->Items[0], dictionary->Originals);
                TWordCollection *coll = new TWordCollection();
                load_dictionary(paths.installdatapath + L"English.zkj", coll, true, true);

                TGapList<int> changes;
                TWordIndexList *list = new TWordIndexList(coll);
                for (int ix = 0; ix < states->Count; ++ix)
                {
                    changes.Items[states->Items[ix]->windex] = FindWordIndex(list, states->Words[ix]->kanji, states->Words[ix]->kana);
                    list->Clear();
                }
                delete list;

                HideMessageForm();
                if (states->Convert(changes, coll,
                        L"The main dictionary has changed. Some words in user groups or study data conflict with the new dictionary. You will be able to resolve the conflicts that zkanji cannot automatically handle in the next dialog, OR you can abort the dialog and continue using zkanji with the old dictionary",
                        L"If you abort the dictionary update process, zkanji will continue to use the old dictionary and this dialog will be shown the next time you run the program. Are you sure?"))
                {
                    states->Commit();
                    dictionary->Items[0]->Modify(true);
                    updated = true;
                }
                else
                {
                    memcpy(&dictionary_date, &tmpdate, sizeof(SYSTEMTIME));
                    dictionary_build_version = tmpver;
                }

                delete coll;
                delete states;

                wchar_t temppath[MAX_PATH];
                GetTempPath(MAX_PATH, temppath);
                UnicodeString dlfilename = UnicodeString(temppath) + "zkanjisetupdict.exe";
                if (FileExists(dlfilename, false))
                    DeleteFile(dlfilename);

                if (updated)
                    MsgBox(L"The dictionary has been updated to the new version.", L"Message", MB_OK);
            }

            if (updated == true)
                dictionary->Save();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfMain::FormShow(TObject *Sender)
{
    LinkLabel2->Left = LinkLabel2->Parent->Width - LinkLabel2->Width - 6 * scaleres;
    if (!inited)
        PostMessage(Handle, WM_Z_INIT, 0, 0);
}
//---------------------------------------------------------------------------
__int64 TfMain::RunningTime()
{
    return secTimer;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(-2);
#endif

