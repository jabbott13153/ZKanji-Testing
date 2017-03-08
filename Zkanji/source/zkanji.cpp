//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <Themes.hpp>
#include <ComObj.hpp>
#include "zdefines.h"

#ifdef _DEBUG
static TGlobalTest tester1(206);
#endif

//---------------------------------------------------------------------------
USEFORM("recognizer.cpp", fRecognize);
USEFORM("redefine.cpp", fRedefine);
USEFORM("regroup.cpp", fRegroup);
USEFORM("sodpopup.cpp", fSOD);
USEFORM("stats.cpp", fStats);
USEFORM("settingswindow.cpp", fSettings);
USEFORM("sitemanager.cpp", fSites);
USEFORM("msgform.cpp", fMsg);
USEFORM("newword.cpp", fNewWord);
USEFORM("module.cpp", data); /* TDataModule: File Type */
USEFORM("radicals.cpp", fRadicals);
USEFORM("numinput.cpp", fNumInput);
USEFORM("popupdict.cpp", fPopupDict);
USEFORM("wordstudylist.cpp", fStudyList);
USEFORM("wordstudytest.cpp", fTest);
USEFORM("wordreview.cpp", fWReview);
USEFORM("wordlist.cpp", fDict);
USEFORM("wordlistbox.cpp", fWordBox);
USEFORM("writetest.cpp", fWriter);
USEFORM("unicodehint.cpp", HintFormW);
USEFORM("studyroom.cpp", fWStudy);
USEFORM("studygroup.cpp", fWordStudy);
USEFORM("studyreading.cpp", fKStudy);
USEFORM("update.cpp", fUpdate);
USEFORM("wordgen.cpp", fGen);
USEFORM("wordfilter.cpp", fFilter);
USEFORM("vocabform.cpp", fAddVocab);
USEFORM("vocabprint.cpp", fVocabPrint);
USEFORM("warningbubble.cpp", fBubble);
USEFORM("dropdown.cpp", fDropDown);
USEFORM("dummymainform.cpp", ACME);
USEFORM("elementedit.cpp", fEditElem);
USEFORM("dictsel.cpp", fDictSel);
USEFORM("dictupdater.cpp", fDictUpdater);
USEFORM("docker.cpp", fDock);
USEFORM("elementselector.cpp", fSelectElem);
USEFORM("elementsettings.cpp", fElemSettings);
USEFORM("exportform.cpp", fExport);
USEFORM("dictimport.cpp", fDictImport);
USEFORM("controlimages.cpp", cdata); /* TDataModule: File Type */
USEFORM("clipview.cpp", fClipboard);
USEFORM("kanjiform.cpp", fKToGroup);
USEFORM("kanjidefimport.cpp", fKanjiDefImport);
USEFORM("kanjiinfo.cpp", fInfo);
USEFORM("listimport.cpp", fListImport);
USEFORM("main.cpp", fMain);
USEFORM("kanjilist.cpp", fKanji);
USEFORM("kanjidef.cpp", fDef);
USEFORM("importexsubst.cpp", fExampleSub);
USEFORM("importgroupchoice.cpp", fImportGroupChoice);
USEFORM("importgroups.cpp", fImportGroups);
USEFORM("importdict.cpp", fImportDict);
USEFORM("groupadd.cpp", fWordToGroup);
USEFORM("grouplist.cpp", fGroup);
USEFORM("import.cpp", fImporter);
USEFORM("importquery.cpp", fImportQuery);
USEFORM("kanawritingtest.cpp", fKanaWriting);
USEFORM("kanatest.cpp", fKanaTest);
USEFORM("jlpteditor.cpp", frmJLPTEdit);
USEFORM("kanapractice.cpp", fKana);
USEFORM("dictquery.cpp", fDictQuery);
//---------------------------------------------------------------------------
#ifdef _DEBUG
USEFORM("writetest.cpp", fWriter);
#endif
//---------------------------------------------------------------------------
#define WM_Z_RESTORE     (WM_APP + 100)

extern bool TestRun;

extern PACKAGE TfDropDown *fDropDown;
extern wchar_t Version_String[];
bool ThemesAreTurnedOn = false;
//extern wchar_t TasklistAppID[];

extern bool runupdate;

double scaleres = 0;

WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR cmd, int)
{
    try
    {
#ifdef _DEBUG
        TestRun = true;
#else
        TestRun = !wcscmp(cmd, L"-testrun");
#endif
        HWND h;
        if (!TestRun)
            h = FindWindow(L"TfMain", L"zkanji");
        else
            h = FindWindow(L"TfMain", L"zkanji test version");
        if (h)
        {
            int p = 0;
            if (wcslen(cmd) == 4 && !wcsncmp(cmd, L"-p=", 3))
            {
                p = max(0, cmd[3] - '0');
                if (p > 3)
                    p = 0;
            }
            PostMessage(h, WM_Z_RESTORE, p, 0);
        }
        else
        {
            // When included zkanji won't run on XP. Only kept here in case I need to set an app id for task lists
            //if (MatchVersion(6,1))
            //  SetCurrentProcessExplicitAppUserModelID(TasklistAppID);

            delete PopupList;
            PopupList = ZPopupList = new TZPopupList();

            CoInitFlags = COINIT_MULTITHREADED;

            Application->HintPause = 250;
            Application->HintHidePause = 5000;
            Application->Initialize();
            if (!TestRun)
                Application->Title = L"zkanji";
            else
                Application->Title = L"zkanji test version";
            TThemeServices *t = ThemeServices();
            if (t && t->ThemesAvailable && t->ThemesEnabled)
                ThemesAreTurnedOn = true;

            Application->MainFormOnTaskBar = true;
            Application->ShowMainForm = false;

            Application->CreateForm(__classid(TACME), &ACME);
         Application->Title = (UnicodeString)L"zkanji v" + Version_String;

            scaleres = Screen->PixelsPerInch / 96.0;

            ApplicationRealInit();
            Application->Run();

            delete PopupList;
            PopupList = new TPopupList();

#ifndef _DEBUG
            if (runupdate)
            {
                wchar_t temppath[MAX_PATH];
                GetTempPath(MAX_PATH, temppath);
                UnicodeString dlfilename = UnicodeString(temppath) + "zkanjisetup.exe";
                if (FileExists(dlfilename, false))
                    ShellExecuteW(NULL, L"open", dlfilename.c_str(), (L"/UPDATE=1 /S /DEST=\"" + paths.installpath + L"\"").c_str(), ExtractFilePath(dlfilename).c_str(), SW_SHOWNORMAL);
                else
                    throw Exception("Updater not found!");
            }
#endif
        }
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    catch (...)
    {
        try
        {
            throw Exception("");
        }
        catch (Exception &exception)
        {
            Application->ShowException(&exception);
        }
    }
    return 0;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(207);
#endif
