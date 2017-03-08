//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "update.h"
#include "socket.h"
#include "settings.h"
#include "gengraph.h"
#include "main.h"
#include "zkformats.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

// There is a speedbutton "hidden" on pViewHide panel to handle Alt+D key combinations, preventing an annoying sound to be heard.
// Don't remove the button unless a better solution is found!

#ifdef _DEBUG
static TGlobalTest tester1(168);
#endif

TfUpdate *fUpdate = NULL;
extern wchar_t Version_String[];

bool runupdate = false; // Set to true when an update setup is found and downloaded. The result is then checked when zkanji shuts down to start the setup.
bool dictupdate = false; // Set to true if a dictionary update was downloaded, before sending the WM_Z_UPDATE message to the main window.

//#define _LOCALUPDATE 0
#if defined(_LOCALUPDATE) && defined(_DEBUG)
const char updatehost[] = "localhost";
const char updaterootdir[] = "/zkanji";
#else
const char updatehost[] = "zkanji.sourceforge.net";
const char updaterootdir[] = "";
#endif

//---------------------------------------------------------------------------
void CheckForUpdates(bool show)
{
    if (fUpdate)
    {
        if (show)
            PostMessage(fUpdate->Handle, WM_Z_MODALBEGIN, 0, 0);
        return;
    }
    fUpdate = new TfUpdate(Application);
    if (show)
        PostMessage(fUpdate->Handle, WM_Z_MODALBEGIN, 0, 0);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TfUpdate::TVersionData::TVersionData(AnsiString version, AnsiString path) : version(version), path(path), partlen(0), threadid(0), done(false)
{
}
//---------------------------------------------------------------------------
TfUpdate::TVersionData::~TVersionData()
{
    ;
}
//---------------------------------------------------------------------------
__fastcall TfUpdate::TfUpdate(TComponent* Owner) : web(NULL), getid(0), getdictid(0), versionpassed(false), timeout(30000), updatestate(usChecking), oldversion(false), dlfile(NULL), TBaseForm(Owner)
{
    web = new TConnectionHandler;
    web->OnTerminated = &updateterminateproc;

    origsize = Rect(0, 0, Width, Height);

    lbUpdate->Caption = UnicodeString();
    lbDict->Caption = UnicodeString();

    //TRect r = lbAdminWarn->BoundsRect;
    //lbAdminWarn->AutoSize = true;
    //EraseBackground = false;
    //lbAdminWarn->AutoSize = false;
    //lbAdminWarn->BoundsRect = r;

    tick = GetTickCount();

    Constrain(Width, Height);

    Caption = (UnicodeString)"zkanji v" + Version_String;
    startget();
    startgetdict();
}
//---------------------------------------------------------------------------
__fastcall TfUpdate::~TfUpdate()
{
    if (dlfile)
    {
        fclose(dlfile);
        DeleteFile(dlfilename);
    }
    delete web;
    web = NULL;
    fUpdate = NULL;
}
//---------------------------------------------------------------------------
void TfUpdate::startget()
{
    getid = web->CreateGet(getproc, updatehost, (AnsiString(updaterootdir) + "/update.txt").c_str());
}
//---------------------------------------------------------------------------
void TfUpdate::startgetdict()
{
    getdictid = web->CreateGet(getproc, updatehost, (AnsiString(updaterootdir) + "/dictupdate.txt").c_str());
}
//---------------------------------------------------------------------------
void TfUpdate::updateterminateproc()
{
    if (updatestate == usAborted)
    {
        if (Visible)
            PostMessage(Handle, WM_Z_HIDE, 0, 0);
        else
            PostMessage(fMain->Handle, WM_Z_CLOSEUPDATE, 0, 0);
    }
}
//---------------------------------------------------------------------------
void TfUpdate::msg_hide(TMessage &msg)
{
    if (updatestate != usChecking && updatestate != usDownloading)
        ModalResult = mrCancel;
    else
        btnAction->Click();
}
//---------------------------------------------------------------------------
void TfUpdate::msg_modal(TMessage &msg)
{
    if (!Visible)
        ShowModal();
}
//---------------------------------------------------------------------------
void TfUpdate::abortall()
{
    if (fUpdate->Visible)
    {
        tTimer->Enabled = false;
        lbDots->Visible = false;
        pProgress->Visible = false;
        pViewHide->Visible = false;
        btnUpdate->Visible = false;
    }

    web->Abort();
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::btnActionClick(TObject *Sender)
{
    if (updatestate == usAborted)
        return;

    if (updatestate == usChecking || updatestate == usDownloading)
    {
        updatestate = usAborted;
        web->Abort();
    }
    else
        PostMessage(Handle, WM_Z_HIDE, 0, 0);
}
//---------------------------------------------------------------------------
void TfUpdate::msg_notify(TWMNotify &msg)
{
    TENLink *el = (TENLink*)msg.NMHdr;
    if (msg.NMHdr->code != EN_LINK || el->msg != WM_LBUTTONDOWN)
    {
        inherited::Dispatch(&msg);
        return;
    }
    PostMessage(msg.NMHdr->hwndFrom, WM_LBUTTONUP, el->wParam, el->lParam);
    if (msg.NMHdr->hwndFrom == mResult->Handle)
    {
        SendMessage(msg.NMHdr->hwndFrom, EM_EXSETSEL, 0, (int)(&el->chrg));
        UnicodeString s = mResult->SelText;
        ShellExecuteW(Handle, L"open", s.c_str(), 0, 0, SW_SHOWNORMAL);
        mResult->SelLength = 0;
    }
}
//---------------------------------------------------------------------------
void TfUpdate::msg_hittest(TWMNCHitTest &msg)
{
    DefaultHandler(&msg);
    if (pViewHide->Tag == 0 && (msg.Result == HTBOTTOM || msg.Result == HTBOTTOMLEFT || msg.Result == HTBOTTOMRIGHT ||
                                 msg.Result == HTTOP || msg.Result == HTTOPLEFT || msg.Result == HTTOPRIGHT ||
                                 msg.Result == HTLEFT || msg.Result == HTRIGHT || msg.Result == HTTOPRIGHT))
        msg.Result = HTBORDER;
}
//---------------------------------------------------------------------------
void TfUpdate::HideMessage()
{
    tTimer->Enabled = false;
    lbDots->Visible = false;
    lbMsg->Visible = false;
}
//---------------------------------------------------------------------------
void TfUpdate::ShowMessage(UnicodeString msg, bool showwindow, bool hidepanel)
{
    if ((updatestate == usError || updatestate == usAborted) && dlfile)
    {
        fclose(dlfile);
        dlfile = NULL;
        DeleteFile(dlfilename);
    }
    tTimer->Enabled = false;
    lbDots->Visible = false;
    if (hidepanel)
        pUpdates->Visible = false;
    pProgress->Visible = false;

    lbMsg->Caption = msg;

    if (updatestate != usChecking && updatestate != usDownloading)
        btnAction->Caption = L"Close";
    if (!Visible && showwindow)
        PostMessage(fUpdate->Handle, WM_Z_MODALBEGIN, 0, 0);

    RedrawWindow(Handle, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
}
//---------------------------------------------------------------------------
void TfUpdate::doit()
{
    for (int ix = 0; ix < versions.Count && updatestate != usAborted && updatestate != usError; ix++)
    {
        if (versions.Items[ix]->threadid == 0)
        {
            if (updatestate != usAborted && updatestate != usError)
                versions.Items[ix]->threadid = web->CreateGet(getproc2, updatehost, ((AnsiString)updaterootdir + "/updates/" + versions.Items[ix]->path).c_str());
        }
    }
}
//---------------------------------------------------------------------------
void TfUpdate::hideupdates()
{
    pViewHide->Tag = 0;
    pViewHide->Caption = L"  View &details...";
    Constrain(origsize.Width(), origsize.Height());
    SetBounds(Left, Top, origsize.Width(), origsize.Height());
    mResult->Hide();
}
//---------------------------------------------------------------------------
void TfUpdate::showupdates()
{
    pViewHide->Tag = 1;
    pViewHide->Caption = L"  Hide &details...";

    mResult->Visible = true;
    Height += 200 * scaleres;
    Constrain(origsize.Width(), origsize.Height() + 30 * scaleres);

    if (mResult->GetTextLen() != 0)
        return;

    TTextAttributes *attrib = mResult->DefAttributes;

    mResult->Lines->BeginUpdate();
    if (oldversion)
    {
        AddStyledText(mResult, L"This program is too old to update to the latest version. Please get the version offered for download and update again.", attrib->Name, TFontStyles(attrib->Style) << fsBold, attrib->Color, attrib->Height + 2, attrib->Pitch, attrib->Charset);
        mResult->Lines->Add("");
        mResult->Lines->Add("");
    }

    TStringList *strings = new TStringList;
    for (int ix = versions.Count - 1; ix >= 0; --ix)
    {
        strings->Clear();
        TVersionData *dat = versions.Items[ix];
        AddStyledText(mResult, "zkanji v" + dat->version + " release information:", attrib->Name, TFontStyles(attrib->Style) << fsBold, attrib->Color, attrib->Height, attrib->Pitch, attrib->Charset);
        SplitToStrings(strings, dat->info);

        for (int ix = strings->Count - 1; ix != 1; --ix)
        {
            if (strings->Strings[ix].IsEmpty())
                strings->Delete(ix);
            else
                break;
        }

        // The first line is the download path which should be skipped. Lines starting with [BOLD] must be shown as such.
        for (int ix = 1; ix < strings->Count; ++ix)
        {
            if (strings->Strings[ix].SubString(1, 6) == L"[BOLD]")
                AddStyledText(mResult, strings->Strings[ix].SubString(7, strings->Strings[ix].Length() - 6), attrib->Name, TFontStyles(attrib->Style) << fsBold, attrib->Color, attrib->Height, attrib->Pitch, attrib->Charset);
            else
                mResult->Lines->Add(strings->Strings[ix]);
        }

        mResult->Lines->Add("");
    }
    delete strings;
    mResult->Lines->EndUpdate();

/*
    int ln = 0;
    bool mustupdate = false;
    int userformat = 2;
    int sel;
    mResult->Lines->BeginUpdate();
    try
    {
        int mask = SendMessage(mResult->Handle, EM_GETEVENTMASK, 0, 0);
        SendMessage(mResult->Handle, EM_SETEVENTMASK, 0, mask | ENM_LINK);
        SendMessage(mResult->Handle, EM_AUTOURLDETECT, (int)true, 0);

        mResult->Lines->BeginUpdate();
        //mResult->Lines->Add("Visit http://zkanji.sourceforge.net to download the latest version.");
        mResult->Lines->Add("");

        for (int ix = versions->Count - 1; ix >= 0; ix--)
        {
            TVersionData *d = (TVersionData*)versions->Objects[ix];
            bool found = false;
            bool options = false;
            bool path = false;
            UnicodeString l = "";
            sel = mResult->SelStart;
            l = "zkanji v" + versions->Names[ix] + " release information:";
            mResult->Lines->Add(l);
            mResult->SelStart = sel;
            mResult->SelLength = l.Length();
            mResult->SelAttributes->Style = TFontStyles() << fsBold;
            mResult->SelStart = sel + l.Length();
            mResult->SelLength = 0;
            mResult->SelAttributes->Style = TFontStyles();

            while (true)
            {
                int p = d->info.Pos("\r\n");
                int p2 = d->info.Pos("\n");

                if (!p && !p2)
                {
                    if (d->info.Length() && found)
                    {
                        mResult->Lines->Add(d->info);
                        result = "";
                        dictresult = "";
                    }
                    break;
                }

                int cnt = p && p2 ? min(p, p2) - 1 : p ? p - 1 : p2 - 1, cnt2;
                if (p && p2 && p < p2 || !p2)
                    cnt2 = p + 2;
                else if (p && p2 && p2 < p || !p)
                    cnt2 = p2 + 1;

                l = d->info.SubString(1, cnt);
                if (path)
                {
                    d->path = l;
                    path = false;
                }
                else if (options)
                {
                    if (l.Pos("B") || l.Pos("b"))
                        mResult->Lines->Strings[mResult->Lines->Count - 1] = mResult->Lines->Strings[mResult->Lines->Count - 1] + " - mainly bugfix release";
                    if (!mustupdate && (l.Pos("D") || l.Pos("d")))
                    {
                        mustupdate = true;
                        int lc = mResult->Lines->Count;
                        mResult->Lines->Insert(1, "");

                        mResult->Lines->Insert(2, (UnicodeString)"Important: zkanji v" + versions->Names[ix] + " introduces a new datafile format. Please download the full version and update the files.");
                        mResult->SelStart = mResult->Lines->Strings[0].Length() + 2; // Skip first line and crlf twice.
                        mResult->SelLength = 10; // Length of "Important:"
                        mResult->SelAttributes->Style = TFontStyles() << fsBold;
                        mResult->SelStart = mResult->GetTextLen();
                        mResult->SelLength = 0;
                        userformat += mResult->Lines->Count - lc;
                    }
                    if ((l.Pos("U") || l.Pos("u")) && ix != versions->Count - 1)
                    {
                        int l = mResult->Lines->Count;
                        mResult->Lines->Insert(userformat, (UnicodeString)"User file format changed in zkanji v" + versions->Names[ix] + ". Please update to that version first, in case there are more updates.");
                        userformat += mResult->Lines->Count - l;
                        mResult->Lines->Insert(userformat++ , "");
                    }
                    options = false;
                }
                else if (found)
                    mResult->Lines->Add(l);
                else if (l == "#info")
                    found = true;
                else if (l == "#options")
                    options = true;
                else if (l == "#path")
                    path = true;

                d->info = d->info.SubString(cnt2, d->info.Length());
                ln++;
                if (!(ln % 20))
                    Application->ProcessMessages();
            }
            mResult->Lines->Add("");
        }
        mResult->SelStart = 0;
        mResult->Lines->EndUpdate();

        settings.lastcheck = (int)Now();
    }
    catch (...)
    {
        ;
    }
    mResult->Lines->EndUpdate();

    btnUpdate->Caption = L"Update";
    btnUpdate->Visible = true;
    btnAction->Default = false;
    btnUpdate->Default = true;
    ActiveControl = btnUpdate;
    lbAdminWarn->Visible = true;

    if (!fUpdate->Visible)
        PostMessage(fUpdate->Handle, WM_Z_MODALBEGIN, 0, 0);
*/
}
//---------------------------------------------------------------------------
void TfUpdate::addversion(const wchar_t *line, TVersionFor type)
{
    if (line[0] == '#' || line[0] == 0)
        return;

    if (type == vfProgram)
    {
        // Line format:
        // [version] [release info file name] [min dictionary ver] [min user data ver] [min study profile ver]

        // Find the first line with the format: current Version_String=somefilename.txt
        if (!versionpassed)
        {
            const wchar_t *p = wcschr(line, ' ');
            if (p)
                versionpassed = !wcsncmp(line, Version_String, p - line) && wcslen(Version_String) == (unsigned int)(p - line);
            return;
        }

        // Every line coming after the matching version counts as a new one.
        wchar_t *p = wcschr(line, ' '); // After version string.
        if (!p || p == line)
            return;
        *p = 0;
        ++p;
        wchar_t *p2 = wcschr(p + 1, ' '); // Before min dictionary version.
        if (!p2)
            return;
        *p2 = 0;
        ++p2;
        wchar_t *p3 = wcschr(p2 + 1, ' ') + 1; // Before min user data version.
        if (!p3)
            return;
        wchar_t *p4 = wcschr(p3 + 1, ' ') + 1; // Before study profile version.
        if (!p4)
            return;
        wchar_t *p5 = wcschr(p4 + 1, ' '); // End of line or NULL.
        if (p5 == NULL)
            p5 = p4 + wcslen(p4);

        // See if the updated version can handle the user data in the current program.
        try
        {
            unsigned int mindict = StrToInt(UnicodeString(p2, p3 - p2 - 1));
            unsigned int minuser = StrToInt(UnicodeString(p3, p4 - p3 - 1));
            unsigned int minstudy = StrToInt(UnicodeString(p4, p5 - p4));
            if (mindict <= DICTIONARY_VERSION && minuser <= GLOBALGROUP_VERSION && minstudy <= STUDYPROFILE_VERSION)
                versions.Add(new TVersionData(line, p));
            else
                oldversion = true;
        }
        catch(...) // Invalid line format.
        {
            ;
        }
    }
    else // Dictionary
    {
        // Line format:
        // [dict version] [year] [month] [day] [download path]

        wchar_t *p = wcschr(line, ' ') + 1; // Before year.
        if (!p)
            return;
        wchar_t *p2 = wcschr(p + 1, ' ') + 1; // Before month.
        if (!p2)
            return;
        wchar_t *p3 = wcschr(p2 + 1, ' ') + 1; // Before day.
        if (!p3)
            return;
        wchar_t *p4 = wcschr(p3 + 1, ' ') + 1; // Before download path.
        if (!p4)
            return;
        wchar_t *p5 = p4 + wcslen(p4);
        while (p5 - 1 != p4 && *(p5 - 1) == L' ')
            *(--p5) = 0;

        try
        {
            unsigned int dictver = StrToInt(UnicodeString(line, p - line - 1));
            if (dictver != DICTIONARY_VERSION)
                return;

            WORD year = StrToInt(UnicodeString(p, p2 - p - 1));
            WORD month = StrToInt(UnicodeString(p2, p3 - p2 - 1));
            WORD day = StrToInt(UnicodeString(p3, p4 - p3 - 1));

            if (dictionary_date.wYear < year ||
                (dictionary_date.wYear == year && dictionary_date.wMonth < month) ||
                (dictionary_date.wYear == year && dictionary_date.wMonth == month && dictionary_date.wDay < day))
            {
                dictversion = DateString(year, month, day, true);
                dictpath = p4;
            }
        }
        catch(...)
        {
            ;
        }
    }
}
//---------------------------------------------------------------------------
void TfUpdate::getproc2(unsigned int threadID, TConnectionState state, const char *data, unsigned int datalen)
{
    if (updatestate == usError || updatestate == usAborted)
        return;

    int ix = 0;
    TVersionData *vd = NULL;
    for (; ix < versions.Count && updatestate != usError && updatestate != usAborted; ix++)
    {
        if (versions.Items[ix]->threadid == threadID)
        {
            vd = versions.Items[ix];
            break;
        }
    }
    if (!vd)
        return; // Error?

    if (state == csOutOfMemory)
    {
        updatestate = usError;
        abortall();
        ShowMessage("Not enough memory to continue operation.", false);
    }
    else if (state == csNoConnection)
    {
        updatestate = usError;
        abortall();
        ShowMessage("Can't find update server.\r\nPlease check your connection and try again.", false);
    }
    else if (state == csError)
    {
        versions.Items[ix]->info = UnicodeString();
        versions.Items[ix]->partlen = 0;
        versions.Items[ix]->threadid = 0;
        versions.Items[ix]->done = false;
        if (GetTickCount() - tick < timeout)
            doit();
        else
        {
            updatestate = usError;
            abortall();
            ShowMessage(UnicodeString("An error occurred while downloading update information. (") + datalen + ")", false);
        }
    }
    else if (state == csNotFound)
    {
        if (fUpdate->Visible)
        {
            updatestate = usError;
            abortall();
            ShowMessage("Unable to find update information.\r\nPlease try again later.", false);
        }
        else
            PostMessage(fMain->Handle, WM_Z_CLOSEUPDATE, 0, 0);
        settings.lastcheck = (int)Now();
    }
    else if (state == csTerminated)
    {
        updatestate = usAborted;
        abortall();
    }
    else
    {
        if (datalen)
        {
            if (vd->partlen)
            {
                int len = 2;
                for (; (vd->utf8part[0] & (1 << (7 - len))); ++len);
                while (vd->partlen < len)
                {
                    vd->utf8part[vd->partlen++] = *(data++);
                    datalen--;
                }

                vd->info += UTF8String(vd->utf8part, vd->partlen);
            }

            // Check for partial utf8
            vd->partlen = 0;
            if (data[datalen - 1] & (1 << 7)) // Characters with the 8th bit as 0 are single byte, only handle multiple byte length code points.
            {
                int p = datalen - 1;
                while (p >= 0 && !(data[p] & (1 << 6)) && (datalen - p) <= 6)
                    p--;
                if (p < 0 || (datalen - p) > 6 || (data[p] & ~0x1) == ~0x1)
                {
                    updatestate = usError;
                    abortall();
                    ShowMessage(UnicodeString("Invalid character in UTF8 stream."), false);
                    return;
                }

                unsigned int len = 2;
                for (; (data[p] & (1 << (7 - len))); ++len);
                if (len != datalen - p)
                    while ((unsigned int)p != datalen)
                        vd->utf8part[vd->partlen++] = data[p++];
            }

            vd->info += UTF8String(data, datalen - vd->partlen);
        }
        if (state == csDone)
        {
            vd->done = true;
            bool alldone = true;
            for (int ix = 0; ix < versions.Count && alldone && updatestate != usError && updatestate != usAborted; ++ix)
                alldone = versions.Items[ix]->done;

            if (alldone)
            {
                for (int ix = versions.Count - 1; ix != -1; --ix)
                {
                    int p = versions.Items[ix]->info.Pos(L"\r\n");
                    int p2 = versions.Items[ix]->info.Pos(L"\n");
                    p = p != 0 && p2 != 0 ? min(p, p2) : p != 0 ? p : p2;
                    if (p == 0) // Unlikely, but the format is incorrect.
                    {
                        versions.Delete(ix);
                        continue;
                    }
                    versions.Items[ix]->path = versions.Items[ix]->info.SubString(1, p - 1);
                }

                updatestate = usDone;
                settings.lastcheck = (int)Now();
                lbUpdate->Alignment = taRightJustify;
                lbUpdate->Caption = L"New program version found.";
                lbAdminWarn->Visible = true;
                btnUpdate->Visible = true;
                pViewHide->Visible = true;
                if (!oldversion)
                    ShowMessage((UnicodeString)"Update found! Latest version is zkanji v" + versions.Items[versions.Count - 1]->version, true, false);
                else
                {
                    ShowMessage((UnicodeString)"Update found! Latest compatible version is zkanji v" + versions.Items[versions.Count - 1]->version, true, false);
                    showupdates();
                }
                if (!Visible)
                    PostMessage(Handle, WM_Z_MODALBEGIN, 0, 0);
            }
        }
        else
            tick = GetTickCount();
    }
}
//---------------------------------------------------------------------------
void TfUpdate::getproc(unsigned int threadID, TConnectionState state, const char *data, unsigned int datalen)
{
    if (updatestate == usError || updatestate == usAborted || !fUpdate)
        return;

    if (state == csOutOfMemory)
    {
        updatestate = usError;
        abortall();
        ShowMessage("Not enough memory to continue operation.", false);
    }
    else if (state == csNoConnection)
    {
        updatestate = usError;
        abortall();
        ShowMessage("Can't find update server.\r\nPlease check your connection and try again.", false);
    }
    else if (state == csError)
    {
        if (GetTickCount() - tick < timeout)
        {
            versions.Clear();
            if (threadID == getid)
            {
                result = "";
                startget();
            }
            else
            {
                dictresult = "";
                startgetdict();
            }
        }
        else
        {
            updatestate = usError;
            abortall();
            ShowMessage(UnicodeString("An error occurred while checking for updates. (") + datalen + ")", false);
        }
    }
    else if (state == csNotFound)
    {
        if (fUpdate->Visible)
        {
            updatestate = usError;
            abortall();
            ShowMessage("Unable to find update information.\r\nPlease try again later.", false);
        }
        else
            PostMessage(fMain->Handle, WM_Z_CLOSEUPDATE, 0, 0);
        settings.lastcheck = (int)Now();
    }
    else if (state == csTerminated)
    {
        updatestate = usAborted;
        abortall();
    }
    else
    {
        if (datalen)
        {
            if (threadID == getid)
                result += UnicodeString(data, datalen);
            else
                dictresult += UnicodeString(data, datalen);
        }

        if (state != csDone)
            return;

        int ln = 0;
        while (updatestate != usError && updatestate != usAborted)
        {
            int p = (threadID == getid ? result : dictresult).Pos("\r\n");
            int p2 = (threadID == getid ? result : dictresult).Pos("\n");

            if (!p && !p2)
            {
                if ((threadID == getid ? result : dictresult).Length())
                {
                    addversion((threadID == getid ? result : dictresult).c_str(), threadID == getid ? vfProgram : vfDictionary);
                    //result = "";
                }
                break;
            }

            int cnt = p && p2 ? min(p, p2) - 1 : p ? p - 1 : p2 - 1, cnt2;
            if (p && p2 && p < p2 || !p2)
                cnt2 = p + 2;
            else if (p && p2 && p2 < p || !p)
                cnt2 = p2 + 1;

            addversion((threadID == getid ? result : dictresult).SubString(1, cnt).c_str(), threadID == getid ? vfProgram : vfDictionary);

            (threadID == getid ? result : dictresult) = (threadID == getid ? result : dictresult).SubString(cnt2, (threadID == getid ? result : dictresult).Length());
            ln++;
            if (!(ln % 10))
                Application->ProcessMessages();
        }

        if (updatestate == usError || updatestate == usAborted)
        {
            versions.Clear();
            dictversion = UnicodeString();
            dictpath = UnicodeString();
            abortall();
            if (fUpdate->Visible)
                PostMessage(fUpdate->Handle, WM_Z_HIDE, 0, 0);
            else
                PostMessage(fMain->Handle, WM_Z_CLOSEUPDATE, 0, 0);
            return;
        }

        if (threadID == getid)
        {
            if (!versions.Count)
            {
                getid = 0;
                if (!web->Running(getdictid))
                {
                    updatestate = usDone;
                    settings.lastcheck = (int)Now();
                }

                lbUpdate->Caption = L"The program is up to date.";
                //lbUpdate->Font->Color = coloravg(lbUpdate->Font->Color, clBtnFace, 0.4);
                if (fUpdate->Visible)
                {
                    if (getdictid == 0)
                        ShowMessage(L"There are no updates at this time.", false, false);
                    else if (btnDict->Visible)
                        ShowMessage(L"Updated dictionary found.", true, false);
                }
                else if (!web->Running(getdictid))
                    PostMessage(fMain->Handle, WM_Z_CLOSEUPDATE, 0, 0);
            }
            else
            {
                //lbUpdate->Alignment = taRightJustify;
                //lbUpdate->Caption = L"New versions found. Getting details...";
                //if (!Visible)
                //    PostMessage(Handle, WM_Z_MODALBEGIN, 0, 0);
                tick = GetTickCount();
                doit();
            }
        }
        else
        {
            if (dictversion.Length() == 0)
            {
                getdictid = 0;
                if (!gettersrunning())
                {
                    updatestate = usDone;
                    settings.lastcheck = (int)Now();
                }

                lbDict->Caption = L"The base dictionary is up to date.";
                //lbDict->Font->Color = coloravg(lbUpdate->Font->Color, clBtnFace, 0.4);
                if (fUpdate->Visible)
                {
                    if (getid == 0)
                        ShowMessage("There are no updates at this time.", false, false);
                }
                else if (!gettersrunning())
                    PostMessage(fMain->Handle, WM_Z_CLOSEUPDATE, 0, 0);
            }
            else
            {
                if (!gettersrunning())
                {
                    updatestate = usDone;
                    settings.lastcheck = (int)Now();
                }

                lbDict->Alignment = taRightJustify;
                lbDict->Caption = L"New dictionary found. Date: " + dictversion;
                lbAdminWarn->Visible = true;
                btnDict->Visible = true;
                if (getid == 0)
                    ShowMessage(L"Updated dictionary found.", true, false);
                else if (!Visible)
                    PostMessage(Handle, WM_Z_MODALBEGIN, 0, 0);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool TfUpdate::gettersrunning()
{
    bool r = (getid != 0 && web->Running(getid));
    if (r)
        return true;
    for (int ix = 0; ix < versions.Count; ++ix)
    {
        if (!versions.Items[ix]->done && web->Running(versions.Items[ix]->threadid))
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::FormPaint(TObject *Sender)
{
    DrawGrad(Canvas, COLORINC(clBtnFace, 0x101010), clBtnFace, Rect(0, 0, ClientWidth, lbMsg->Top));
    DrawGrad(Canvas, clBtnFace, COLORDEC(clBtnFace, 0x101010), Rect(0, lbMsg->Top, ClientWidth, lbMsg->Top + lbMsg->Height));
    DrawGrad(Canvas, COLORDEC(clBtnFace, 0x101010), COLORINC(clBtnFace, 0x101010), Rect(0, lbMsg->Top + lbMsg->Height, ClientWidth, bBottom->Top));
    DrawCloseBar(this, bBottom, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::FormResize(TObject *Sender)
{
    TRect r = Rect(0, 0, ClientWidth, bBottom->Top);
    HRGN rgn = CreateRectRgnIndirect(&r);
    if (mResult->Visible)
    {
        TRect r2 = mResult->ClientRect;
        OffsetRect(r2, mResult->Left, mResult->Top);
        HRGN rgn2 = CreateRectRgnIndirect(&r2);
        CombineRgn(rgn, rgn, rgn2, RGN_DIFF);
        DeleteObject(rgn2);
    }
    RedrawWindow(Handle, NULL, rgn, RDW_INVALIDATE | RDW_ALLCHILDREN);

    DeleteObject(rgn);

    r = Rect(0, bBottom->Top + bBottom->Height, ClientWidth, ClientHeight);
    InvalidateRect(Handle, &r, false);
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::FormShow(TObject *Sender)
{
    if (lbDots->Visible)
    {
        lbDots->Left = lbMsg->Left + lbMsg->Width + 1;
        tTimer->Enabled = true;
    }
    SetForegroundWindow(Handle);
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::tTimerTimer(TObject *Sender)
{
    lbDots->Tag++;
    if (lbDots->Tag > 3)
        lbDots->Tag = 0;
    UnicodeString dots = "";
    for (int ix = 0; ix < lbDots->Tag; ix++)
        dots += ".";
    lbDots->Caption = dots;
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::btnUpdateClick(TObject *Sender)
{
    if (updatestate == usDownloading)
        return;

    dictupdate = Sender == btnDict;

    updatestate = usDownloading;
    lbMsg->Caption = L"Downloading newer version, please wait";
    lbDots->Left = lbMsg->Left + lbMsg->Width + 1;
    lbDots->Tag = 3;
    lbDots->Caption = "...";
    lbDots->Visible = true;

    pUpdates->Visible = false;
    pProgress->Visible = true;

    btnAction->Tag = 0;
    btnAction->Caption = "Abort";
    tTimer->Enabled = true;

    hideupdates();

    AnsiString host;

    if (Sender == btnUpdate)
        host = versions.Items[versions.Count - 1]->path;
    else
        host = dictpath;
    if (host.Pos("http://") == 1)
        host = host.SubString(8, host.Length());
    AnsiString path = host.SubString(host.Pos("/"), host.Length());
    host = host.SubString(0, host.Pos("/") - 1);

    wchar_t temppath[MAX_PATH];
    GetTempPath(MAX_PATH, temppath);
    dlfilename = UnicodeString(temppath);
    if (Sender == btnUpdate)
        dlfilename += "zkanjisetup.exe";
    else
        dlfilename += "zkanjisetupdict.exe";

    dlfile = _wfopen(dlfilename.c_str(), L"wb");
    if (!dlfile)
    {
        ShowMessage("Couldn't create download file.\r\nPlease check your access to the Temp folder.");
        return;
    }

/*
    btnUpdate->Visible = false;
    btnUpdate->Default = false;
    btnAction->Default = true;
    lbAdminWarn->Visible = false;

    pProgress->Visible = true;
    btnAction->Tag = 0;
    btnAction->Caption = "Abort";
    tTimer->Enabled = true;
*/

    getid = web->CreateGet(&dlproc, host.c_str(), path.c_str());
}
//---------------------------------------------------------------------------
void TfUpdate::dlproc(unsigned int threadID, TConnectionState state, const char *data, unsigned int datalen)
{
    if (updatestate == usError || updatestate == usAborted)
        return;

    if (state == csOutOfMemory)
    {
        updatestate = usError;
        abortall();
        ShowMessage("Not enough memory to continue operation.");
    }
    else if (state == csNoConnection)
    {
        updatestate = usError;
        abortall();
        ShowMessage("Can't find update server.\r\nPlease check your connection and try again.");
    }
    else if (state == csError)
    {
        updatestate = usError;
        abortall();
        ShowMessage(UnicodeString("An error occurred while downloading update. (") + datalen + ")");
    }
    else if (state == csNotFound)
    {
        updatestate = usError;
        abortall();
        ShowMessage("Update not found on the server.\r\nPlease try again later.");
    }
    else if (state == csTerminated)
    {
        updatestate = usAborted;
        abortall();
    }
    else
    {
        if (pbProgress->Position == 0)
        {
            UnicodeString len = web->GetHeaderString(threadID, "Content-Length");
            try
            {
                pbProgress->Max = len.ToInt();
                lbProgress->Caption = UnicodeString("0 / ") + (pbProgress->Max / 1024) + " Kb";
            }
            catch (...)
            {
                abortall();
                updatestate = usError;
                ShowMessage("Couldn't determine download size.");
                return;
            }
        }
        if (datalen)
        {
            fwrite(data, 1, datalen, dlfile);
            pbProgress->Position += datalen;
            pbProgress->Position--;
            pbProgress->Position++;
            lbProgress->Caption = UnicodeString(pbProgress->Position / 1024) + " / " + (pbProgress->Max / 1024) + " Kb";
        }

        if (state == csDone)
        {
            fclose(dlfile);
            dlfile = NULL;
            pProgress->Visible = false;
            lbDots->Visible = false;
            tTimer->Enabled = false;
            btnAction->Enabled = false;
            lbMsg->Caption = L"Starting update... ";
            updatestate = usDone;
            PostMessage(fUpdate->Handle, WM_Z_HIDE, 0, 0);

            runupdate = true;
            PostMessage(fMain->Handle, WM_Z_UPDATE, 0, 0);
            return;
        }

    }
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::pViewHideClick(TObject *Sender)
{
    if (pViewHide->Tag == 1)
        hideupdates();
    else
        showupdates();
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::pLinkClick(TObject *Sender)
{
    fMain->showlink(L"http://zkanji.sourceforge.net");
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose = ModalResult == mrCancel;
    if (ModalResult != mrCancel)
        PostMessage(Handle, WM_Z_HIDE, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::pViewHideMouseEnter(TObject *Sender)
{
    pViewHide->Font->Style = TFontStyles() << fsUnderline;
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::pViewHideMouseLeave(TObject *Sender)
{
    pViewHide->Font->Style = TFontStyles();
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::mResultKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Shift.Contains(ssCtrl) && Key == L'C')
    {
        if (mResult->SelLength != 0)
            ClipReplace(mResult->SelText);
        else
            ClipReplace(mResult->Text);
    }
    else if (Key == VK_ESCAPE && btnAction->Cancel)
        btnAction->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfUpdate::btnHiddenButtonClick(TObject *Sender)
{
    if (pUpdates->Visible && pViewHide->Visible)
        pViewHideClick(pViewHide);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(169);
#endif







