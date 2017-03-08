//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "import.h"
#include "importgroups.h"
#include "importgroupchoice.h"
#include "importdict.h"
#include "dictimport.h"
#include "wordgroup.h"
#include "dictsel.h"
#include "zkformats.h"
#include "searchtree.h"
#include "collection.h"
#include "qsrt.h"
#include "settings.h"
#include "kanjigroup.h"
#include "romajizer.h"
#include "grouplist.h"
#include "wordlist.h"
#include "kanjidefimport.h"
#include "importexsubst.h"
#include "module.h"
#include "dictupdater.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfImporter *fImporter;
//---------------------------------------------------------------------------
void AddError(int linenum, TImportErrorType type, UnicodeString msg);
void AddMessage(UnicodeString msg);
void DictionaryAdded(UnicodeString s); // From main.h
void HideAllWindows();
void ShowAllWindows();
//---------------------------------------------------------------------------
__fastcall TfImporter::TfImporter(TComponent* Owner) : TBaseForm(Owner), aborted(false), finished(false)
{
    Constrain(160 * scaleres, 140 * scaleres);
}
//---------------------------------------------------------------------------
bool TfImporter::get_able()
{
    return btnClose->Tag == 1;
}
//---------------------------------------------------------------------------
void TfImporter::set_able(bool value)
{
    if (value)
    {
        btnClose->Tag = 1;
        btnClose->Caption = L"Abort";
        btnClose->Enabled = true;
    }
    else
    {
        btnClose->Tag = 2;
        btnClose->Caption = L"Close";
    }
}
//---------------------------------------------------------------------------
void TfImporter::Finish()
{
    if (finished)
        return;
    finished = true;
}
//---------------------------------------------------------------------------
void TfImporter::Abort()
{
    if (aborted)
        return;
    aborted = true;
    btnClose->Enabled = false;
    btnClose->Tag = 2;
}
//---------------------------------------------------------------------------
void __fastcall TfImporter::FormPaint(TObject *Sender)
{
    DrawPillowPanel(this, NULL, bTop, false, false);
    DrawCloseBar(this, bBottom, NULL);

    Canvas->Brush->Color = clWindow;
    Canvas->FillRect(Rect(0, bTop->Top + bTop->Height, edit->Left, bBottom->Top));
}
//---------------------------------------------------------------------------
void __fastcall TfImporter::FormResize(TObject *Sender)
{
    TRect r = Rect(0, bBottom->Top + bBottom->Height, ClientWidth, ClientHeight);
    InvalidateRect(Handle, &r, false);
    r = Rect(0, bTop->Top + bTop->Height, edit->Left, bBottom->Top);
    InvalidateRect(Handle, &r, false);
}
//---------------------------------------------------------------------------
void __fastcall TfImporter::btnCloseClick(TObject *Sender)
{
    if (btnClose->Tag == 0)
        Close();
    else
    {
        aborted = true;
        btnClose->Enabled = false;
        btnClose->Tag = 2;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfImporter::FormClose(TObject *Sender, TCloseAction &Action)
{
    fImporter = NULL;
    Action = caFree;
    ShowAllWindows();
}
//---------------------------------------------------------------------------
void __fastcall TfImporter::editKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == 0x03) // Ctrl-C
    {
        if (edit->SelLength != 0)
            ClipReplace(edit->SelText);
        else
            ClipReplace(edit->Text);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfImporter::editKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
//    if (Shift.Contains(ssCtrl) && Key == L'C')
//    {
//        if (edit->SelLength != 0)
//            ClipReplace(edit->SelText);
//        else
//            ClipReplace(edit->Text);
//    }
}
//---------------------------------------------------------------------------
void __fastcall TfImporter::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if (btnClose->Tag != 0)
    {
        if (btnClose->Enabled)
            btnClose->Click();
        CanClose = false;
    }
}
//---------------------------------------------------------------------------
void AddError(int linenum, TImportErrorType type, UnicodeString msg)
{
    //errors.Add(new TImportError(linenum, type, msg));
    UnicodeString typemsg;
    switch(type)
    {
    case ietBadFormat:
        typemsg = L"bad format";
        break;
    case ietDataMissing:
        typemsg = L"missing data";
        break;
    case ietDuplicate:
        typemsg = L"duplicate";
        break;
    case ietGeneralError:
        typemsg = L"general error";
        break;
    case ietInvalidValue:
        typemsg = L"invalid value";
        break;
    case ietLimit:
        typemsg = L"limit";
        break;
    case ietNotANumber:
        typemsg = L"number expected";
        break;
    case ietUnrecognizedToken:
        typemsg = L"unrecognized token";
        break;
    }

    TTextAttributes *attrib = fImporter->edit->DefAttributes;
    AddStyledText(fImporter->edit, L"Error on line " + IntToStr(linenum) + L": [" + typemsg + L"] " + msg, attrib->Name, attrib->Style, clRed, attrib->Height, attrib->Pitch, attrib->Charset);
    SendMessage(fImporter->edit->Handle, WM_VSCROLL, SB_BOTTOM, 0);
    Application->ProcessMessages();
}
//---------------------------------------------------------------------------
void AddMessage(UnicodeString msg)
{
    fImporter->edit->SelStart = fImporter->edit->Text.Length();
    fImporter->edit->Lines->Add(msg);
    SendMessage(fImporter->edit->Handle, WM_VSCROLL, SB_BOTTOM, 0);
    Application->ProcessMessages();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Helper structure for reloading dictionary on abort.
struct ImportGuard
{
    int cindex;

    ImportGuard() : cindex(-1)
    {
        HideAllWindows();
        fImporter = new TfImporter(Application);
        fImporter->Show();
    }

    void Set(int aindex)
    {
        cindex = aindex;
    }

    void Disable()
    {
        cindex = -1;
    }

    ~ImportGuard()
    {
        fImporter->btnClose->Enabled = false;
        if (cindex != -1)
        {
            AddMessage(L"Reloading dictionary...");
            dictionary->Reload(cindex);
            if (fGroup->cbDictionary->ItemIndex == cindex)
            {
                fGroup->DeselectDictionary();
                fGroup->SelectDictionary(cindex);
            }
            if (settings.dictionary_index == cindex)
            {
                fDict->DeselectDictionary();
                fDict->SelectDictionary(cindex);
            }
            AddMessage(L"Done.");
        }

        if (fImporter->Aborted)
        {
            MessageBeep(MB_ICONERROR);
            MsgBox(L"Import aborted", L"Message", MB_OK);
            fImporter->btnClose->Tag = 0;
            fImporter->Close();
        }
        else
        {
            if (fImporter->Finished)
                MessageBeep(MB_OK);
            else
                MessageBeep(MB_ICONERROR);
            fImporter->btnClose->Tag = 0;
            fImporter->btnClose->Enabled = true;
        }
    }
};

//---------------------------------------------------------------------------
void GroupImport()
{
    ImportGuard guard;

    AddMessage(L"Saving dictionary data.");
    dictionary->Save();

    DISABLEMOD

    int cix = -1;
    try
    {
        TImportOptions options;
        int mresult = mrOk;
        TWordCollection *coll = NULL;
        UnicodeString FileName;

        AddMessage(L"Starting group import...");

        fImportGroups = NULL;
        try
        {
            fImportGroups = new TfImportGroups(Application);
            if ((mresult = fImportGroups->ShowModalEx(&options)) == mrOk)
            {
                coll = fImportGroups->Collection;

                FileName = fImportGroups->FileName;
            }
            delete fImportGroups;
        }
        catch(...)
        {
            delete fImportGroups;
            throw;
        }
        if (mresult != mrOk)
        {
            AddMessage(L"Aborted.");
            return;
        }

        cix = dictionary->Index[coll];
        AddMessage(L"Target dictionary: " + dictionary->Strings[cix]);

        TImportList list(coll);

        // 1. Read in file, collecting words and kanji and skipping errors. Only the necessary data is read.
        int r;
        if ((r = list.ReadExportedDictionary(FileName, options)) == mrClose)
        {
            AddMessage(L"Errors with export file. Stopping.");
            return;
        }
        if (r == mrAbort)
        {
            AddMessage(L"Aborted.");
            return;
        }

        // 2. Rename or skip imported groups depending on user choice. Does not change the dictionary.

        // Word groups
        if (list.WordGroupCount != 0 && options.wduplicate != gioMerge)
        {
            AddMessage(L"Managing word groups to import.");
            if (options.wduplicate == gioChoose)
            {
                fImportGroupChoice = NULL;
                try
                {
                    fImportGroupChoice = new TfImportGroupChoice(Application);
                    mresult = fImportGroupChoice->ShowModalEx(coll, &list, true);
                    delete fImportGroupChoice;
                }
                catch(...)
                {
                    delete fImportGroupChoice;
                    throw;
                }
                if (mresult != mrOk)
                {
                    AddMessage(L"Aborted.");
                    return;
                }
            }
            else
            {
                // Look for matching group names in existing data.
                for (int ix = 0; ix != list.WordGroupCount; ++ix)
                {
                    int gix = coll->groups->IndexOf(list.WordGroups[ix]->Name);
                    if (gix != -1)
                    {
                        if (options.wduplicate == gioRename)
                        {
                            int num = 1;
                            while (coll->groups->IndexOf(UnicodeString(list.WordGroups[ix]->Name) + L"(" + IntToStr(++num) + L")") != -1)
                                ;

                            while (list.WordGroupIndex(UnicodeString(list.WordGroups[ix]->Name) + L"(" + IntToStr(num) + L")") != -1)
                                ++num;

                            AddMessage(UnicodeString(L"Renaming import group: ") + list.WordGroups[ix]->Name);
                            list.WordGroups[ix]->Name = (UnicodeString(list.WordGroups[ix]->Name) + L"(" + IntToStr(num) + L")").c_str();
                            AddMessage(UnicodeString(L"New name: ") + list.WordGroups[ix]->Name);
                        }
                        else
                        {
                            if (options.wduplicate == gioReplace)
                                list.WordGroups[ix]->Action = giaReplace;
                            else if (options.wduplicate == gioSkip)
                                list.WordGroups[ix]->Action = giaSkip;
                        }
                    }
                }

            }
        }

        // Kanji groups
        if (list.KanjiGroupCount != 0 && options.kduplicate != gioMerge)
        {
            AddMessage(L"Managing kanji groups to import.");
            if (options.kduplicate == gioChoose)
            {
                fImportGroupChoice = NULL;
                try
                {
                    fImportGroupChoice = new TfImportGroupChoice(Application);
                    mresult = fImportGroupChoice->ShowModalEx(coll, &list, false);
                    delete fImportGroupChoice;
                }
                catch(...)
                {
                    delete fImportGroupChoice;
                    throw;
                }
                if (mresult != mrOk)
                {
                    AddMessage(L"Aborted.");
                    return;
                }
            }
            else
            {
                // Look for matching group names in existing data.
                for (int ix = 0; ix != list.KanjiGroupCount; ++ix)
                {
                    UnicodeString listname = list.KanjiGroups[ix]->Name;
                    int gix = coll->kgroups->IndexOf(listname);
                    if (gix != -1)
                    {
                        if (options.kduplicate == gioRename)
                        {
                            int num = 1;
                            do
                            {
                                listname = UnicodeString(list.KanjiGroups[ix]->Name) + L"(" + IntToStr(++num) + L")";
                            }
                            while (coll->kgroups->IndexOf(listname) != -1);

                            while (list.KanjiGroupIndex(UnicodeString(list.KanjiGroups[ix]->Name) + L"(" + IntToStr(num) + L")") != -1)
                                ++num;
                            AddMessage(UnicodeString(L"Renaming import group: ") + list.KanjiGroups[ix]->Name);
                            list.KanjiGroups[ix]->Name = (UnicodeString(list.KanjiGroups[ix]->Name) + L"(" + IntToStr(num) + L")").c_str();
                            AddMessage(UnicodeString(L"New name: ") + list.KanjiGroups[ix]->Name);
                        }
                        else
                        {
                            if (options.kduplicate == gioReplace)
                                list.KanjiGroups[ix]->Action = giaReplace;
                            else if (options.kduplicate == gioSkip)
                                list.KanjiGroups[ix]->Action = giaSkip;
                        }
                    }
                }
            }
        }

        // 3. Groups were sorted by name until this point because it is easier for users to find and rename them. IMPORTANT: groups must be sorted by Id by the time MergeSameGroups() is called.
        AddMessage(L"Sorting import groups...");
        list.SortGroups();

        // 4. Merge groups with the same name.
        AddMessage(L"Merging import groups with matching name...");
        list.MergeSameGroups();

        // 5. Erase empty and skipped groups. Erase existing word items which were only added to skipped groups or no groups at all. If options does not contain dictionary update, erase definitions not added to groups.
        AddMessage(L"Removing empty or skipped import groups...");
        list.CleanupGroups(options);

        if (options.modifydict)
        {
            AddMessage(L"Checking dictionary for missing or conflicting words...");

            // 6.a Decide which words to add when updating the dictionary
            list.CollectForImport(options);

            AddMessage(L"Select action on conflict if needed...");
            // 6.b Let the user select the action to take for each word.
            fDictImport = NULL;
            try
            {
                fDictImport = new TfDictImport(Application);
                mresult = fDictImport->ShowModalEx(&list, &options, false);
                delete fDictImport;
            }
            catch(...)
            {
                delete fDictImport;
                throw;
            }
            if (mresult != mrOk)
            {
                AddMessage(L"Aborted.");
                return;
            }

            // 6.c Remove unwanted words and meanings from update list, and sort according to the selected order.
            AddMessage(L"Removing unwanted words and meanings from the import...");
            list.CleanupDefs(options);
        }

        if (options.modifydict)
        {
            guard.Set(cix);

            // 6.d Update dictionary
            AddMessage(L"Updating the dictionary...");
            list.EntryUpdate(options);

            // 6.e Clean up again to remove definitions of new words not in groups. This time no need to worry about keeping all definitions because they were only needed for file import.
            AddMessage(L"Removing word definitions not added to groups...");
            list.CleanupNewWords(options);
        }

        if (options.wordgroups)
        {
            // 7.a Make a list for word group import.
            AddMessage(L"Collecting words to be added to groups...");
            list.CollectForReplace(options);

            if (options.wordsubst)
            {
                AddMessage(L"Word substitution if needed...");
                // 7.b Let the user select the action to take for each word.
                fDictImport = NULL;
                try
                {
                    fDictImport = new TfDictImport(Application);
                    mresult = fDictImport->ShowModalEx(&list, &options, true);
                    delete fDictImport;
                }
                catch(...)
                {
                    delete fDictImport;
                    throw;
                }
                if (mresult != mrOk)
                {
                    fImporter->Abort();
                    AddMessage(L"Aborted.");
                    return;
                }
            }

            guard.Set(cix);

            // 7.c Update word data according to selected meaning matches.
            AddMessage(L"Updating word data before adding to groups...");
            list.DoWordReplace(options);

            // 7.d Modifying word groups.
            AddMessage(L"Importing words to groups...");
            list.WordGroupUpdate(options);
        }

        // 8. List kanji example words not in the dictionary, and substitute as necessary.
        if (options.kanjiex)
        {
            // 8.a After the previous dictionary update, some word examples not found previously could have been added to the dictionary. Update kanji example word indexes accordingly.
            AddMessage(L"Updating kanji example word indexes...");
            list.ReviewExamples();

            // 8.b Dialog to select substitutes for examples not found in the current dictionary.
            AddMessage(L"Word example substitution if needed...");
            if (options.kanjisubst)
            {
                fExampleSub = NULL;
                try
                {
                    fExampleSub = new TfExampleSub(Application);
                    mresult = fExampleSub->ShowModalEx(&list);
                    delete fExampleSub;
                }
                catch(...)
                {
                    delete fExampleSub;
                    throw;
                }
                if (mresult != mrOk)
                {
                    fImporter->Abort();
                    AddMessage(L"Aborted.");
                    return;
                }
            }

            guard.Set(cix);

            // 8.c Import kanji example words.
            AddMessage(L"Importing kanji example words...");
            list.ExamplesUpdate();

            // 8.d Import user definition of example words.
            if (options.kanjiexdef)
            {
                AddMessage(L"Importing kanji example word user definitions...");
                list.ExampleDefUpdate();
            }
        }

        // 9. Set user definition of kanji.
        if (options.kanjiuserdef)
        {
            guard.Set(cix);

            AddMessage(L"Importing kanji user definitions...");
            list.UserDefUpdate();
        }

        // 10. Updating or creating kanji groups with imported kanji.
        if (options.kanjigroups)
        {
            guard.Set(cix);

            AddMessage(L"Importing kanji to groups...");
            list.KanjiGroupUpdate(options);
        }

        list.Collection->Modify(true);

        // Refill the group lists in the group list window.
        fGroup->SelectDictionary(dictionary->Index[list.Collection]);
    }
    catch(...)
    {
        MsgBox(L"Unexpected error. Aborting operation.", L"Error", MB_OK);
        fImporter->Abort();
        return;
    }

    if (fImporter->Aborted)
        return;

    guard.Disable();

    fImporter->Finish();

    AddMessage(L"Saving dictionary data.");
    dictionary->Save();

    if (fGroup->cbDictionary->ItemIndex == cix)
    {
        fGroup->DeselectDictionary();
        fGroup->SelectDictionary(cix);
    }
    if (settings.dictionary_index == cix)
    {
        fDict->DeselectDictionary();
        fDict->SelectDictionary(cix);
    }

    AddMessage(L"Import completed.");
}
//---------------------------------------------------------------------------
void DictImport()
{
    ImportGuard guard;

    AddMessage(L"Saving dictionary data.");
    dictionary->Save();

    DISABLEMOD

    int cix = -1;
    try
    {
        TImportOptions options;
        int mresult = mrOk;

        fImportDict = NULL;
        fDictImport = NULL;

        TWordCollection *coll = NULL;
        UnicodeString FileName;

        AddMessage(L"Starting dictionary import...");

        fImportDict = new TfImportDict(Application);
        try
        {
            if ((mresult = fImportDict->ShowModalEx(&options)) == mrOk)
            {
                coll = fImportDict->Collection;
                FileName = fImportDict->FileName;
            }
            delete fImportDict;
        }
        catch(...)
        {
            delete fImportDict;
            throw;
        }
        if (mresult != mrOk)
        {
            AddMessage(L"Aborted.");
            return;
        }

        cix = dictionary->Index[coll];
        AddMessage(L"Target dictionary: " + dictionary->Strings[cix]);

        TImportList list(coll);

        // 1. Read in file, collecting words and kanji and skipping errors. Only the necessary data is read.
        int r;
        if ((r = list.ReadExportedDictionary(FileName, options)) == mrClose)
        {
            AddMessage(L"Errors with export file. Stopping.");
            return;
        }
        if (r == mrAbort)
        {
            AddMessage(L"Aborted.");
            return;
        }

        // 2. Decide which words to add when updating the dictionary.
        AddMessage(L"Collecting words for import...");
        list.CollectForImport(options);

        // 3. Let the user select the action to take for each word.
        AddMessage(L"Select action on conflict if needed...");
        fDictImport = NULL;
        try
        {
            fDictImport = new TfDictImport(Application);
            mresult = fDictImport->ShowModalEx(&list, &options, false);
            delete fDictImport;
        }
        catch(...)
        {
            delete fDictImport;
            throw;
        }
        if (mresult != mrOk)
        {
            AddMessage(L"Aborted.");
            return;
        }

        // 4. Remove unwanted words and meanings from update list, and sort according to the selected order.
        AddMessage(L"Removing unwanted words and meanings from the import...");
        list.CleanupDefs(options);

        guard.Set(cix);

        // 5. Update dictionary
        AddMessage(L"Updating the dictionary...");
        list.EntryUpdate(options);

        // 6. Make list of kanji definitions that conflict with existing ones. (Only for user dictionaries.)
        if (dictionary->Index[coll] != 0 && options.kanjidictdef)
        {
            if (options.kanjidefselect)
            {
                // 6.a Show dialog for checking conflicting kanji definitions.
                AddMessage(L"Select definitions to import if needed...");
                fKanjiDefImport = NULL;
                try
                {
                    fKanjiDefImport = new TfKanjiDefImport(Application);
                    mresult = fKanjiDefImport->ShowModalEx(&list);
                    delete fKanjiDefImport;
                }
                catch(...)
                {
                    fImporter->Abort();
                    delete fKanjiDefImport;
                    throw;
                }
                if (mresult != mrOk)
                {
                    fImporter->Abort();
                    AddMessage(L"Aborted.");
                    return;
                }
            }

            // 6.b Update the kanji definitions in the dictionary.
            AddMessage(L"Importing kanji definitions...");
            list.DictDefUpdate();
        }

        list.Collection->Modify(true);

        // Refill the group lists in the group list window.
        fGroup->SelectDictionary(dictionary->Index[list.Collection]);
    }
    catch (...)
    {
        MsgBox(L"Unexpected error. Aborting operation.", L"Error", MB_OK);
        fImporter->Abort();
        return;
    }

    if (fImporter->Aborted)
        return;

    guard.Disable();

    fImporter->Finish();

    AddMessage(L"Saving dictionary data.");
    dictionary->Save();
    if (fGroup->cbDictionary->ItemIndex == cix)
    {
        fGroup->DeselectDictionary();
        fGroup->SelectDictionary(cix);
    }
    if (settings.dictionary_index == cix)
    {
        fDict->DeselectDictionary();
        fDict->SelectDictionary(cix);
    }

    AddMessage(L"Import completed.");
}
//---------------------------------------------------------------------------
void FullImport()
{
    ImportGuard guard;

    AddMessage(L"Saving dictionary data.");
    dictionary->Save();


    DISABLEMOD

    int cix = -1;
    //bool aborted = false;
    try
    {
        AddMessage(L"Starting full dictionary import...");

        UnicodeString dictname = DictionarySelect(L"Enter a distinct name for a new dictionary, or select the target dictionary to be overwritten with the imported data.", false, L"<Enter target dictionary>");
        if (dictname.Length() == 0)
        {
            AddMessage(L"Aborted.");
            return;
        }

        UnicodeString fname;
        int origix = dictionary->IndexOf(dictname);
        AddMessage(L"Target dictionary: " + dictname);

        data->dlgOpenImport->InitialDir = settings.exportdir;
        if (ExecuteDialog(data->dlgOpenImport))
        {
            fname = data->dlgOpenImport->FileName;
            settings.exportdir = ExtractFileDir(data->dlgOpenImport->FileName);
        }
        else
        {
            AddMessage(L"Aborted.");
            return;
        }

        TImportOptions options;

        memset(&options, 0, sizeof(TImportOptions));
        options.fullimport = true;

        // Add an extra ? to the name if necessary, so there is no clash with existing dictionaries. Only valid file names are valid dictionary names, and the ? is not a valid file name character.
        if (origix == -1)
            AddMessage(L"Adding new dictionary.");
        else
            AddMessage(L"Adding temporary dictionary for reading.");
        cix = dictionary->Add(dictname + (origix == -1 ? L"" : L"?"));
        TWordCollection *coll = dictionary->Items[cix];
        TImportList list(coll);

        int r;
        if ((r = list.ReadExportedDictionary(fname, options)) == mrClose)
        {
            AddMessage(L"Errors with export file. Stopping.");
            dictionary->Delete(cix);
            return;
        }
        if (r == mrAbort)
        {
            dictionary->Delete(cix);
            AddMessage(L"Aborted.");
            return;
        }

        if (origix != -1)
            guard.Set(origix);

        // Fill the dictionary with imported words.
        if ((r = list.FullDictImport()) == mrClose)
        {
            AddMessage(L"Unexpected error occurred while importing the dictionary. Reverting changes...");
            THROW(L"Unexpected error occurred while importing the dictionary. Reverting changes...");
        }
        if (r == mrAbort)
        {
            dictionary->Delete(cix);
            AddMessage(L"Aborted.");
            return;
        }

        // Set kanji dictionary definitions from import.
        AddMessage(L"Importing kanji definitions...");
        list.DictDefUpdate();

        // Do the update if our dictionary was made to replace another
        if (origix != -1)
        {
            AddMessage(L"Initializing dictionary update...");

            TWordStateList *states = new TWordStateList();
            states->Build(dictionary->Items[origix], NULL);

            TGapList<int> changes;
            TWordIndexList *wl = new TWordIndexList(coll);

            AddMessage(L"Looking up words in the original dictionary...");
            for (int ix = 0; ix < states->Count; ++ix)
            {
                changes.Items[states->Items[ix]->windex] = FindWordIndex(wl, states->Words[ix]->kanji, states->Words[ix]->kana);
                wl->Clear();
            }
            delete wl;

            AddMessage(L"Select action to take for changed words with conflict in groups...");
            if (states->Convert(changes, coll, UnicodeString(), L"Do you wish to abort the full dictionary import process?"))
            {
                AddMessage(L"Updating dictionary...");
                states->Commit();
                AddMessage(L"Dictionary update completed...");
                guard.Disable();
            }
            else
            {
                AddMessage(L"Aborted.");
                fImporter->Abort();
                //aborted = true;
            }

            delete states;

            dictionary->Delete(cix);

            if (!fImporter->Aborted)
            {
                if (!list.AboutInfo.IsEmpty())
                    AddMessage(L"Updating dictionary information text.");
                dictionary->Items[origix]->info = list.AboutInfo;
                dictionary->Items[origix]->Modify(true);

                // Refill the group lists in the group wl window.
                fGroup->SelectDictionary(origix);
            }
        }
        else
        {
            if (!list.AboutInfo.IsEmpty())
                AddMessage(L"Setting dictionary information text.");
            dictionary->Items[cix]->info = list.AboutInfo;
            DictionaryAdded(dictname);
            dictionary->Items[cix]->Modify(true);
        }
    }
    catch (...)
    {
        if (cix != -1)
            dictionary->Delete(cix);
        MsgBox(L"Unexpected error. Aborting operation.", L"Error", MB_OK);
        fImporter->Abort();
        return;
    }

    if (fImporter->Aborted)
        return;

    fImporter->Finish();

    AddMessage(L"Saving dictionary data.");
    dictionary->Save();

    AddMessage(L"Import completed.");
}
//---------------------------------------------------------------------------
// Helper functions in sorting or searching.
static int exportedwdefnumproc(TExportedDefinition **def)
{
    return (*def)->num;
}
//---------------------------------------------------------------------------
static int exportedkchnumproc(TExportedKanji **kanji)
{
    return (*kanji)->Kanji;
}
//---------------------------------------------------------------------------
static int exportedkgrpnumproc(TImportKanjiGroup **grp)
{
    return (int)*grp;
}
//---------------------------------------------------------------------------
template<typename T>
static int importgrpnameproc(const wchar_t *name, T **grp)
{
    return wcscmp(name, (*grp)->Name);
}
//---------------------------------------------------------------------------
template<typename T>
static int importitementryproc(int entry, T *item)
{
    return entry - (*item)->entry;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Word data.
//---------------------------------------------------------------------------
TExportedWord::TExportedWord(int windex, const wchar_t *akanji, const wchar_t *akana) : windex(windex), kanji(NULL), kana(NULL), freq(0)
{
    if (windex < 0)
    {
        kanji = new wchar_t[wcslen(akanji) + 1];
        wcscpy(kanji, akanji);
        kana = new wchar_t[wcslen(akana) + 1];
        wcscpy(kana, akana);
    }
}
//---------------------------------------------------------------------------
TExportedWord::~TExportedWord()
{
    delete[] kanji;
    delete[] kana;
}
//---------------------------------------------------------------------------
int TExportedWord::get_defcnt()
{
    return deflist.Count;
}
//---------------------------------------------------------------------------
TExportedDefinition* TExportedWord::get_def(int index)
{
    return deflist[index];
}
//---------------------------------------------------------------------------
void TExportedWord::InsertMeaning(int insertindex, int meaningnum, const wchar_t *meaning, int wtype, int wnote, int wfield, int wname)
{
    // In theory this function is not called after the word has 100 definitions already.
    if (deflist.Count == 100)
        return;
    if (insertindex < 0 || insertindex > deflist.Count)
        THROW(L"Wrong insert index. Should be between 0 and the number of already added definitions.");

    TExportedDefinition *def = new TExportedDefinition();
    def->data.meaning = new wchar_t[wcslen(meaning) + 1];
    wcscpy(def->data.meaning, meaning);
    def->data.types = wtype;
    def->data.notes = wnote;
    def->data.fields = wfield;
    def->data.nametag = wname;
    def->groupcnt = 0;
    def->groups = NULL;
    def->num = meaningnum;

    deflist.Insert(insertindex, def);
}
//---------------------------------------------------------------------------
static int exportedgroupidproc(TExportedWordGroup *grp)
{
    return (int)grp->group;
}
bool TExportedWord::AddToGroup(int defindex, TImportWordGroup *group, int entryindex)
{
    if (defindex < 0 || defindex >= deflist.Count)
        THROW(L"Definition index out of range.");

    TExportedDefinition *dat = deflist[defindex];

    TExportedWordGroup *tmp = dat->groups;

    TExportedWordGroup newgroup;
    newgroup.group = group;
    newgroup.entry = entryindex;

    int insertindex = FindDefGroup(defindex, group); //logsearch(tmp, 0, dat->groupcnt - 1, groupid, &exportedgroupidproc);
    if (insertindex >= 0) // Already added.
        return false;

    insertindex = -1 - insertindex;
    dat->groups = new TExportedWordGroup[dat->groupcnt + 1];

    if (insertindex > 0)
        memcpy(dat->groups, tmp, sizeof(TExportedWordGroup) * insertindex);
    dat->groups[insertindex] = newgroup;
    if (insertindex < dat->groupcnt)
        memcpy(dat->groups + insertindex + 1, tmp + insertindex, sizeof(TExportedWordGroup) * (dat->groupcnt - insertindex));
    ++dat->groupcnt;

    delete[] tmp;

    return true;
}
//---------------------------------------------------------------------------
int TExportedWord::FindDefGroup(int defindex, TImportWordGroup *group)
{
    TExportedDefinition *dat = deflist[defindex];
    return logsearch(dat->groups, 0, dat->groupcnt - 1, (int)group, &exportedgroupidproc);
}
//---------------------------------------------------------------------------
void TExportedWord::ChangeDefGroup(int defindex,  TImportWordGroup *oldgroup,  TImportWordGroup *newgroup, int entryindex)
{
    if (oldgroup == newgroup)
        THROW(L"Smack the programmer");

    int defgroup = FindDefGroup(defindex, oldgroup);
    if (defgroup < 0)
        THROW(L"Smack the programmer twice");
    TExportedWordGroup *gdat = deflist[defindex]->groups + defgroup;
    gdat->group = newgroup;
    gdat->entry = entryindex;
}
//---------------------------------------------------------------------------
/*
void TExportedWord::DeleteDefinition(int defindex)
{
    for (int j = 0; j < deflist[defindex]->groupcnt; ++j)
    {
        TExportedDefinition *def = deflist[defindex];
        def->groups[j].group->RemoveWord(def->groups[j].entry, this, def);
    }
    deflist.Delete(defindex);
}
*/
//---------------------------------------------------------------------------
wchar_t* TExportedWord::Kanji(TWordCollection *coll)
{
    if (kanji)
        return kanji;
    return coll->words->Items[windex]->kanji;
}
//---------------------------------------------------------------------------
wchar_t* TExportedWord::Kana(TWordCollection *coll)
{
    if (kana)
        return kana;
    return coll->words->Items[windex]->kana;
}
//---------------------------------------------------------------------------
int TExportedWord::MeaningPos(int meaningnum)
{
    return logsearch(deflist.List, 0, deflist.Count - 1, meaningnum, &exportedwdefnumproc);
}
//---------------------------------------------------------------------------
static int __exportedwordsortdef(TExportedDefinition **defa, TExportedDefinition **defb)
{
    return (*defa)->num - (*defb)->num;
}
void TExportedWord::SortDefinitions()
{
    deflist.Sort(&__exportedwordsortdef);
}
//---------------------------------------------------------------------------
void TExportedWord::CleanupGroups(const TImportOptions &options, bool keepnewdefs)
{
    bool ingroup = false;
    for (int iy = deflist.Count - 1; iy >= 0; --iy)
    {
        TExportedDefinition *def = deflist[iy];

        int newgcnt = 0;
        for (int z = 0; z < def->groupcnt; ++z)
            if (def->groups[z].group->Action == giaSkip)
                def->groups[z].group = NULL;
            else
                ++newgcnt;

        if (newgcnt)
            ingroup = true;
        else if (!keepnewdefs || windex >= 0 || !options.modifydict) // Remove definition not in any groups, if it is not used for word import. Entirely new words need all their definitions not in groups too, in case they are to be added to the dictionary.
        {
            deflist.Delete(iy);
            continue;
        }

        if (newgcnt == def->groupcnt)
            continue;

        TExportedWordGroup *tmp = def->groups;
        if (newgcnt)
        {
            def->groups = new TExportedWordGroup[newgcnt];
            int pos = 0;
            for (int z = 0; z < def->groupcnt; ++z)
            {
                if (tmp[z].group != NULL)
                    memcpy(def->groups + (pos++), tmp + z, sizeof(TExportedWordGroup));
            }
        }
        else
            def->groups = NULL;

        def->groupcnt = newgcnt;
        delete[] tmp;
    }

    if (!ingroup)
        deflist.Clear();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TExportedDefinition::TExportedDefinition() : num(-1), groups(NULL), groupcnt(0)
{
    memset(&data, 0, sizeof(TMeaningData));
}
//---------------------------------------------------------------------------
TExportedDefinition::~TExportedDefinition()
{
    delete[] data.meaning;
    delete[] groups;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TExportedKanji::TExportedKanji(wchar_t ch) : ch(ch), ddef(NULL), udef(NULL), groups(NULL), groupcnt(0)
{
    kindex = kanjis->IndexOf(ch);
}
//---------------------------------------------------------------------------
TExportedKanji::~TExportedKanji()
{
    delete[] ddef;
    delete[] udef;
    delete[] groups;
}
//---------------------------------------------------------------------------
void TExportedKanji::set_ddef(wchar_t *adef)
{
    delete[] ddef;
    if (adef != 0 && adef[0] != 0)
    {
        ddef = new wchar_t[wcslen(adef) + 1];
        wcscpy(ddef, adef);
    }
    else
        ddef = NULL;
}
//---------------------------------------------------------------------------
void TExportedKanji::set_udef(wchar_t *adef)
{
    delete[] udef;
    if (adef != 0 && adef[0] != 0)
    {
        udef = new wchar_t[wcslen(adef) + 1];
        wcscpy(udef, adef);
    }
    else
        udef = NULL;
}
//---------------------------------------------------------------------------
bool TExportedKanji::AddToGroup(TImportKanjiGroup *group)
{
    int gix = FindGroup(group);
    if (gix >= 0)
        return false;
    gix = -1 - gix;

    TImportKanjiGroup **tmp = groups;
    groups = new TImportKanjiGroup*[groupcnt + 1];
    if (gix > 0)
        memcpy(groups, tmp, sizeof(TImportKanjiGroup*) * gix);
    groups[gix] = group;
    if (gix < groupcnt)
        memcpy(groups + gix + 1, tmp + gix, sizeof(TImportKanjiGroup*) * (groupcnt - gix));
    ++groupcnt;
    delete[] tmp;

    return true;
}
//---------------------------------------------------------------------------
int TExportedKanji::FindExample(TWordCollection *coll, const wchar_t *kanji, const wchar_t *kana)
{
    int min = 0, max = exlist.Count - 1;
    while (min <= max)
    {
        int mid = (min + max) / 2;

        int d = WordCompare(kanji, kana, exlist[mid]->Kanji(coll), exlist[mid]->Kana(coll));
        if (d < 0)
            max = mid - 1;
        else if (d > 0)
            min = mid + 1;
        else
        {
            min = mid;
            break;
        }
    }

    if (min >= 0 && min < exlist.Count && !wcscmp(exlist[min]->Kanji(coll), kanji) && !wcscmp(exlist[min]->Kana(coll), kana))
        return min;
    return -min - 1;
}
//---------------------------------------------------------------------------
int TExportedKanji::FindGroup(TImportKanjiGroup *group)
{
    return logsearch(groups, 0, groupcnt - 1, (int)group, &exportedkgrpnumproc);
}
//---------------------------------------------------------------------------
void TExportedKanji::ChangeGroup(TImportKanjiGroup *oldgroup, TImportKanjiGroup *newgroup, int newentryindex)
{
    if (oldgroup == newgroup)
        THROW(L"Smack the programmer");

    int g = FindGroup(oldgroup);
    if (g < 0)
        THROW(L"Smack the programmer twice");
    groups[g] = newgroup;
}
//---------------------------------------------------------------------------
void TExportedKanji::CleanupGroups()
{
    int newgcnt = 0;
    for (int iy = 0; iy < groupcnt; ++iy)
        if (groups[iy]->Action == giaSkip)
            groups[iy] = NULL;
        else
            ++newgcnt;
    if (newgcnt == groupcnt)
        return;
    TImportKanjiGroup **tmp = groups;
    if (newgcnt)
    {
        groups = new TImportKanjiGroup*[newgcnt];
        int pos = 0;
        for (int iy = 0; iy < groupcnt; ++iy)
        {
            if (tmp[iy] != NULL)
                memcpy(groups + (pos++), tmp + iy, sizeof(TImportKanjiGroup*));
        }
    }
    else
        groups = NULL;
    groupcnt = newgcnt;
    delete[] tmp;
}
//---------------------------------------------------------------------------
TExportedKanjiExample* TExportedKanji::AddExampleWord(TWordCollection *coll, wchar_t *kanji, wchar_t *kana)
{
    if (exlist.Count == 65535)
        return NULL;

    int eix = FindExample(coll, kanji, kana);
    if (eix < 0)
    {
        eix = -1 - eix;
        int wix = FindWordIndex(coll, kanji, kana);
        exlist.Insert(eix, new TExportedKanjiExample(wix, kanji, kana));
    }

    return exlist[eix];
}
//---------------------------------------------------------------------------
int TExportedKanji::get_excnt()
{
    return exlist.Count;
}
//---------------------------------------------------------------------------
TExportedKanjiExample* TExportedKanji::get_ex(int ix)
{
    return exlist[ix];
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TExportedKanjiExample::TExportedKanjiExample(int windex, const wchar_t *akanji, const wchar_t *akana) : windex(windex), kanji(NULL), kana(NULL), meaning(NULL)
{
    if (windex < 0)
    {
        kanji = new wchar_t[wcslen(akanji) + 1];
        wcscpy(kanji, akanji);
        kana = new wchar_t[wcslen(akana) + 1];
        wcscpy(kana, akana);
    }
}
//---------------------------------------------------------------------------
TExportedKanjiExample::~TExportedKanjiExample()
{
    delete[] kanji;
    delete[] kana;
    delete[] meaning;
}
//---------------------------------------------------------------------------
wchar_t* TExportedKanjiExample::Kanji(TWordCollection *coll)
{
    if (kanji)
        return kanji;
    return coll->words->Items[windex]->kanji;
}
//---------------------------------------------------------------------------
wchar_t* TExportedKanjiExample::Kana(TWordCollection *coll)
{
    if (kana)
        return kana;
    return coll->words->Items[windex]->kana;
}
//---------------------------------------------------------------------------
void TExportedKanjiExample::set_meaning(wchar_t *ameaning)
{
    delete[] meaning;
    int mlen = wcslen(ameaning);
    if (mlen != 0)
    {
        meaning = new wchar_t[mlen + 1];
        wcscpy(meaning, ameaning);
    }
    else
        meaning = NULL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TImportGroupBase::TImportGroupBase(const wchar_t* aname, int id) : name(NULL), id(id), action(giaMerge)
{
    if (!aname)
        THROW(L"?");

    name = new wchar_t[wcslen(aname) + 1];
    wcscpy(name, aname);
}
//---------------------------------------------------------------------------
TImportGroupBase::~TImportGroupBase()
{
    delete[] name;
}
//---------------------------------------------------------------------------
void TImportGroupBase::set_name(const wchar_t* newname)
{
    if (!newname)
        THROW(L"?");

    delete[] name;
    name = new wchar_t[wcslen(newname) + 1];
    wcscpy(name, newname);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TImportWordGroup::TImportWordGroup(const wchar_t* aname, int id) : inherited(aname, id)
{
}
//---------------------------------------------------------------------------
TImportWordGroup::~TImportWordGroup()
{
}
//---------------------------------------------------------------------------
void TImportWordGroup::AddWord(int entryindex, TExportedWord *exportedword, TExportedDefinition *exporteddef)
{
    TImportedWordElem *elem = new TImportedWordElem();
    elem->data = exportedword;
    elem->def = exporteddef;
    elem->entry = entryindex;

    int ix;
    if (entryindex < 0)
        ix = items.Count;
    else
    {
        int last = 0;
        for (int iy = items.Count - 1; iy >= 0 && items[iy]->entry < 0; --iy)
            ++last;

        // Search between the first and last entries with a positive entry index. All other elements must stay at the end of the list.
        ix = logsearch(items.List, 0, items.Count - 1 - last, entryindex, importitementryproc<TImportedWordElem*>);
        if (ix < 0)
            ix = -1 - ix;
        else
        {
            while (++ix < items.Count && items[ix]->entry == entryindex)
                ;
        }
    }

    items.Insert(ix, elem);
}
//---------------------------------------------------------------------------
void TImportWordGroup::RemoveWord(int entryindex, TExportedWord *exportedword, TExportedDefinition *exporteddef)
{
    int index = -1;
    if (entryindex < 0)
    {
        for (int iy = items.Count - 1 ; iy >= 0 && items[iy]->entry < 0 && index < 0; --iy)
        {
            if (items[iy]->data == exportedword && items[iy]->def == exporteddef)
                index = iy;
        }
    }
    else
    {
        int last = 0;
        for (int iy = items.Count - 1; iy >= 0 && items[iy]->entry < 0; --iy)
            ++last;

        index = logsearch(items.List, 0, items.Count - 1 - last, entryindex, importitementryproc<TImportedWordElem*>);
    }

    if (index < 0)
        THROW(L"Replace programmer. Entry not found when should have been.");

    items.Delete(index);
}
//---------------------------------------------------------------------------
void TImportWordGroup::UpdateIndexes()
{
    int entryindex = 0;
    for (int ix = 0; ix < items.Count; ++ix)
    {
        if (ix == items.Count - 1 || (items[ix + 1]->entry != -1 && items[ix + 1]->entry == items[ix]->entry))
            items[ix]->entry = entryindex;
        else
        {
            items[ix]->entry = entryindex;
            ++entryindex;
        }

        for (int iy = 0; iy < items[ix]->def->groupcnt; ++iy)
            if (items[ix]->def->groups[iy].group == this)
            {
                items[ix]->def->groups[iy].entry = items[ix]->entry;
                break;
            }
    }
}
//---------------------------------------------------------------------------
int TImportWordGroup::NextIndex()
{
    if (items.Count == 0)
        return 0;
    return items[items.Count - 1]->entry + 1;
}
//---------------------------------------------------------------------------
int TImportWordGroup::get_count()
{
    return items.Count;
}
//---------------------------------------------------------------------------
TExportedWord* TImportWordGroup::get_item(int ix)
{
    return items[ix]->data;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TImportKanjiGroup::TImportKanjiGroup(const wchar_t* aname, int id) : inherited(aname, id)
{
}
//---------------------------------------------------------------------------
TImportKanjiGroup::~TImportKanjiGroup()
{
}
//---------------------------------------------------------------------------
void TImportKanjiGroup::AddKanji(int entryindex, TExportedKanji *item)
{
    TImportedKanjiElem *elem = new TImportedKanjiElem();
    elem->data = item;
    elem->entry = entryindex;

    int ix;
    if (entryindex < 0)
        ix = items.Count;
    else
    {
        int last = 0;
        for (int iy = items.Count - 1; iy >= 0 && items[iy]->entry < 0; --iy) // Skip kanji from the back of the list which had no entryindex specified in the export file.
            ++last;

        // Search between the first and last entries with a positive entry index. All other elements must stay at the end of the list.
        ix = logsearch(items.List, 0, items.Count - 1 - last, entryindex, importitementryproc<TImportedKanjiElem*>);
        if (ix < 0)
            ix = -1 - ix;
        else
        {
            while (++ix < items.Count && items[ix]->entry == entryindex) // Add kanji after all others with the same index.
                ;
        }
    }
    items.Insert(ix, elem);
}
//---------------------------------------------------------------------------
void TImportKanjiGroup::UpdateIndexes()
{
    int entryindex = 0;
    for (int ix = 0; ix < items.Count; ++ix)
        items[ix]->entry = entryindex++;
}
//---------------------------------------------------------------------------
int TImportKanjiGroup::NextIndex()
{
    if (items.Count == 0)
        return 0;
    return items[items.Count - 1]->entry + 1;
}
//---------------------------------------------------------------------------
int TImportKanjiGroup::get_count()
{
    return items.Count;
}
//---------------------------------------------------------------------------
TExportedKanji* TImportKanjiGroup::get_item(int ix)
{
    return items[ix]->data;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template<typename T> TImportGroupList<T>::TImportGroupList()
{
}
//---------------------------------------------------------------------------
template<typename T> TImportGroupList<T>::~TImportGroupList()
{
}
//---------------------------------------------------------------------------
template<typename T>
static int importgrplistidsrt(T *a, T *b)
{
    return (*a)->Id - (*b)->Id;
}
template<typename T> void TImportGroupList<T>::Sort()
{
    inherited::Sort(importgrplistidsrt<T*>);
}
//---------------------------------------------------------------------------
template<typename T> int TImportGroupList<T>::IndexOf(UnicodeString name)
{
    name = name.LowerCase();
    for (int ix = 0; ix < Count; ++ix)
        if (UnicodeString(Items[ix]->Name).LowerCase() == name)
            return ix;
    return -1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TImportedDefinition::~TImportedDefinition()
{
    delete[] dictdefs;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TImportError::TImportError(int linenum, TImportErrorType type, const wchar_t *message) : linenum(linenum), type(type), msg(NULL)
{
    if (message)
    {
        msg = new wchar_t[wcslen(message) + 1];
        wcscpy(msg, message);
    }
}
//---------------------------------------------------------------------------
TImportError::~TImportError()
{
    delete[] msg;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TImportList::TImportList(TWordCollection *coll) : coll(coll)
{
    cix = dictionary->Index[coll];
}
//---------------------------------------------------------------------------
TImportList::~TImportList()
{
    Clear();
}
//---------------------------------------------------------------------------
void TImportList::Clear()
{
    wordlist.Clear();
    kanjilist.Clear();
    //errors.Clear();
    wordgroups.Clear();
    kanjigroups.Clear();
}
//---------------------------------------------------------------------------
int TImportList::FindWord(const wchar_t *kanji, const wchar_t *kana)
{
    int min = 0, max = wordlist.Count - 1;
    while (min <= max)
    {
        int mid = (min + max) / 2;
        TExportedWord *w = wordlist.Items[mid];
        int d = WordCompare(kanji, kana, w->Kanji(coll), w->Kana(coll));
        if (d < 0)
            max = mid - 1;
        else if (d > 0)
            min = mid + 1;
        else
        {
            min = mid;
            break;
        }
    }

    if (min >= 0 && min < wordlist.Count && !wcscmp(wordlist.Items[min]->Kanji(coll), kanji) && !wcscmp(wordlist.Items[min]->Kana(coll), kana))
        return min;
    return -min - 1;
}
//---------------------------------------------------------------------------
int TImportList::FindWord(int wordindex)
{
    TWord *w = coll->words->Items[wordindex];
    return FindWord(w->kanji, w->kana);
}
//---------------------------------------------------------------------------
int TImportList::FindWord(TExportedWord *w)
{
    return FindWord(w->Kanji(coll), w->Kana(coll));
}
//---------------------------------------------------------------------------
TImportWordGroup* TImportList::AddWordGroup(const wchar_t *groupname)
{
    int cnt = wordgroups.Count;
    int ix = logsearch(wordgroups.List, 0, cnt - 1, groupname, importgrpnameproc<TImportWordGroup>);
    if (ix >= 0)
        return wordgroups[ix];
    ix = -1 - ix;

    TImportWordGroup *group = new TImportWordGroup(groupname, cnt);
    wordgroups.Insert(ix, group);
    return group;
}
//---------------------------------------------------------------------------
TImportKanjiGroup* TImportList::AddKanjiGroup(const wchar_t *groupname)
{
    int cnt = kanjigroups.Count;
    int ix = logsearch(kanjigroups.List, 0, cnt - 1, groupname, importgrpnameproc<TImportKanjiGroup>);
    if (ix >= 0)
        return kanjigroups[ix];
    ix = -1 - ix;

    TImportKanjiGroup *group = new TImportKanjiGroup(groupname, cnt);
    kanjigroups.Insert(ix, group);
    return group;
}
//---------------------------------------------------------------------------
void TImportList::WordToGroup(const wchar_t *groupname, int entryindex, TExportedWord *w, TExportedDefinition *def)
{
    int ix = logsearch(wordgroups.List, 0, wordgroups.Count - 1, groupname, importgrpnameproc<TImportWordGroup>);
    if (ix < 0)
        THROW(L"Group not found.");
    wordgroups[ix]->AddWord(entryindex, w, def);
}
//---------------------------------------------------------------------------
void TImportList::KanjiToGroup(const wchar_t *groupname, int entryindex, TExportedKanji *k)
{
    int ix = logsearch(kanjigroups.List, 0, kanjigroups.Count - 1, groupname, importgrpnameproc<TImportKanjiGroup>);
    if (ix < 0)
        THROW(L"Group not found.");
    kanjigroups[ix]->AddKanji(entryindex, k);
}
//---------------------------------------------------------------------------
int TImportList::get_wc()
{
    return wordlist.Count;
}
//---------------------------------------------------------------------------
int TImportList::get_kc()
{
    return kanjilist.Count;
}
//---------------------------------------------------------------------------
TExportedWord* TImportList::get_word(int ix)
{
    return wordlist[ix];
}
//---------------------------------------------------------------------------
TExportedKanji* TImportList::get_kanji(int ix)
{
    return kanjilist[ix];
}
//---------------------------------------------------------------------------
int TImportList::get_wgc()
{
    return wordgroups.Count;
}
//---------------------------------------------------------------------------
int TImportList::get_kgc()
{
    return kanjigroups.Count;
}
//---------------------------------------------------------------------------
TImportWordGroup* TImportList::get_wgroup(int ix)
{
    return wordgroups[ix];
}
//---------------------------------------------------------------------------
TImportKanjiGroup* TImportList::get_kgroup(int ix)
{
    return kanjigroups[ix];
}
//---------------------------------------------------------------------------
int TImportList::get_ic()
{
    return importlist.Count;
}
//---------------------------------------------------------------------------
TImportedWord* TImportList::get_iword(int ix)
{
    return importlist[ix];
}
//---------------------------------------------------------------------------
int TImportList::get_rc()
{
    return replacelist.Count;
}
//---------------------------------------------------------------------------
TReplacedWord* TImportList::get_rword(int ix)
{
    return replacelist[ix];
}
//---------------------------------------------------------------------------
TExportedWord* TImportList::AddWord(int windex, const wchar_t *kanji, const wchar_t *kana, short frequency)
{
    int eix = windex < 0 ? FindWord(kanji, kana) : FindWord(windex);
    if (eix >= 0)
        return wordlist.Items[eix];
    else
    {
        TExportedWord *exp = new TExportedWord(windex, kanji, kana);
        exp->Frequency = max(0, frequency);
        wordlist.Insert(-1 - eix, exp);
        return exp;
    }
}
//---------------------------------------------------------------------------
TExportedKanji* TImportList::AddKanji(wchar_t kanjich)
{
    int kix = logsearch<TExportedKanji*>(kanjilist.List, 0, kanjilist.Count - 1, kanjich, &exportedkchnumproc);
    if (kix >= 0)
        return kanjilist.Items[kix];
    TExportedKanji *k = new TExportedKanji(kanjich);
    kanjilist.Insert(-1 - kix, k);
    return k;
}
//---------------------------------------------------------------------------
/*
void TImportList::RemoveDefDuplicates()
{
    for (int ix = 0; ix < wordlist.Count; ++ix)
    {
        TExportedWord *w = wordlist[ix];

        // Find a duplicate for each definition.
        for (int iy = 0; iy < w->DefCount - 1; ++iy)
        {
            for (int z = w->DefCount - 1; z > iy; --z)
            {
                if (wcscmp(w->Def[z]->data.meaning, w->Def[iy]->data.meaning)) // Definition meanings differ.
                    continue;

                // Duplicate definition found. Remove it from groups and then delete the duplicate.
                w->DeleteDefinition(z);
            }
        }
    }
}
*/
//---------------------------------------------------------------------------
void TImportList::SortGroups()
{
    wordgroups.Sort();
    kanjigroups.Sort();
}
//---------------------------------------------------------------------------
int TImportList::WordGroupIndex(UnicodeString name)
{
    return wordgroups.IndexOf(name);
}
//---------------------------------------------------------------------------
int TImportList::KanjiGroupIndex(UnicodeString name)
{
    return kanjigroups.IndexOf(name);
}
//---------------------------------------------------------------------------
void TImportList::MergeSameGroups()
{
    // Merge words to groups.
    for (int ix = 0; ix < wordgroups.Count; ++ix)
    {
        if (wordgroups[ix]->Action == giaSkip) // No need to merge with groups that should be skipped.
            continue;

        UnicodeString name = wordgroups[ix]->Name;
        name = name.LowerCase();

        // Look for matching group name with higher index than the previous one.
        for (int iy = ix + 1; iy < wordgroups.Count; ++iy)
        {
            if (wordgroups[iy]->Action == giaSkip) // No need to merge with groups that should be skipped.
                continue;
            UnicodeString name2 = wordgroups[iy]->Name;
            if (name != name2.LowerCase()) // Don't merge when two groups have different names.
                continue;

            // Mark second group to skip.
            wordgroups[iy]->Action = giaSkip;

            // Find words in the second group to be moved to the first one.
            for (int z = 0; z < wordgroups[iy]->Count; ++z)
            {
                TExportedWord *w = wordgroups[iy]->Items[z];

                // Find which definitions of the word are in the second group only and mark them as belonging to the first group.
                for (int k = 0; k < w->DefCount; ++k)
                {
                    // Make sure the first group does not contain the same word definition.
                    if (w->FindDefGroup(k, wordgroups[ix]) >= 0)
                        continue;

                    int dix = w->FindDefGroup(k, wordgroups[iy]);
                    if (dix < 0)
                        THROW(L"This cannot happen");

                    int oldindex = w->Def[k]->groups[dix].entry;

                    int next = wordgroups[ix]->NextIndex();
                    w->ChangeDefGroup(k, wordgroups[iy], wordgroups[ix], next);
                    wordgroups[ix]->AddWord(next, w, w->Def[k]);

                    // Make sure the definitions with the same entry index in the second group are moved together to the same index in the first group.
                    for (int j = k + 1; j < w->DefCount; ++j)
                    {
                        if (w->FindDefGroup(j, wordgroups[ix]) >= 0)
                            continue;
                        dix = w->FindDefGroup(j, wordgroups[iy]);
                        if (dix >= 0 && w->Def[j]->groups[dix].entry == oldindex)
                        {
                            w->ChangeDefGroup(j, wordgroups[iy], wordgroups[ix], next);
                            wordgroups[ix]->AddWord(next, w, w->Def[j]);
                        }
                    }
                }
            }
        }
    }

    // Merge kanji to groups.
    for (int ix = 0; ix < kanjigroups.Count; ++ix)
    {
        if (kanjigroups[ix]->Action == giaSkip) // No need to merge with groups that should be skipped.
            continue;

        UnicodeString name = kanjigroups[ix]->Name;
        name = name.LowerCase();

        // Look for matching group name with higher index than the previous one.
        for (int iy = ix + 1; iy < kanjigroups.Count; ++iy)
        {
            if (kanjigroups[iy]->Action == giaSkip) // No need to merge with groups that should be skipped.
                continue;
            UnicodeString name2 = kanjigroups[iy]->Name;
            if (name != name2.LowerCase()) // Don't merge when two groups have different names.
                continue;

            // Mark second group to skip.
            kanjigroups[iy]->Action = giaSkip;

            // Find kanji in the second group to be moved to the first one.
            for (int z = 0; z < kanjigroups[iy]->Count; ++z)
            {
                // Make sure the first group does not contain the same kanji.
                if (kanjigroups[iy]->Items[z]->FindGroup(kanjigroups[ix]) >= 0)
                    continue;

                int next = kanjigroups[ix]->NextIndex();
                kanjigroups[iy]->Items[z]->ChangeGroup(kanjigroups[iy], kanjigroups[ix], next);
                kanjigroups[ix]->AddKanji(next, kanjigroups[iy]->Items[z]);
            }
        }
    }


}
//---------------------------------------------------------------------------
void TImportList::CleanupGroups(const TImportOptions &options)
{
    // Delete skipped groups from word definitions, and words not added to groups.
    for (int ix = wordlist.Count - 1; ix >= 0; --ix)
    {
        wordlist[ix]->CleanupGroups(options, true);
        if (wordlist[ix]->DefCount == 0)
            wordlist.Delete(ix);
    }
    // Delete skipped or empty groups. Word definitions were updated in the previous step not to include skipped groups already.
    for (int ix = wordgroups.Count - 1; ix >= 0; --ix)
        if (wordgroups[ix]->Count == 0 || wordgroups[ix]->Action == giaSkip)
            wordgroups.Delete(ix);

    // Delete skipped groups from kanji definitions.
    for (int ix = kanjilist.Count - 1; ix >= 0; --ix)
    {
        kanjilist[ix]->CleanupGroups();
        if (kanjilist[ix]->GroupCount == 0)
            kanjilist.Delete(ix);
    }
    // Delete skipped or empty groups. Kanji were updated in the previous step not to include skipped groups already.
    for (int ix = kanjigroups.Count - 1; ix >= 0; --ix)
        if (kanjigroups[ix]->Count == 0 || kanjigroups[ix]->Action == giaSkip)
            kanjigroups.Delete(ix);
}
//---------------------------------------------------------------------------
void TImportList::CleanupNewWords(const TImportOptions &options)
{
    for (int ix = wordlist.Count - 1; ix >= 0; --ix)
    {
        wordlist[ix]->CleanupGroups(options, false);
        if (wordlist[ix]->DefCount == 0)
            wordlist.Delete(ix);
    }
}
//---------------------------------------------------------------------------
static bool TokenMatch(wchar_t *str, wchar_t *match, bool acceptempty = true) // Pass a string found with FirstToken/NextToken and see if it starts with match (either lower or upper case). The passed match string should be upper case.
{
    if (!str)
        return false;
    int tlen = wcslen(str);
    int mlen = wcslen(match);
    if (tlen < mlen || (!acceptempty && tlen == mlen))
        return false;
    for (int ix = 0; ix < mlen; ++ix)
    {
        if (str[ix] != match[ix] && str[ix] != (match[ix] + 0x20))
            return false;
    }
    return true;
}
static wchar_t* NextPattern(TTokenSearchData &searchdata, wchar_t* &marker, bool &error, bool secondary) // Fetches the next pattern using NextToken. Export file patterns are between XX{ ... }XX (YY( ... )YY for secondary), or XX\t ... \t. The marker string is set to the start of the XX marker, while the returned string is the variable part. searchdata.separator is set to bracket or tab or space, depending on what was returned. If no pattern was found, the function returns the result of NextToken(searchdata, L' '), setting marker to NULL. In case of error (pattern not ending correctly or matching tab not found) error is set to true and NULL is returned. RestoreToken cannot be used after this call.
{
    error = false;
    marker = NULL;

    wchar_t *token = NextToken(searchdata, L' ');
    if (!token)
        return NULL;

    const wchar_t openbracket = !secondary ? L'{' : L'(';
    const wchar_t closingbracket = !secondary ? L'}' : L')';

    int pos = 0;
    while (token[pos] != 0 && token[pos] != L'\t' && token[pos] != openbracket && token[pos] != closingbracket)
        ++pos;

    if (token[pos] == 0)
        return token;
    else if (token[pos] == closingbracket)
    {
        error = true;
        marker = NULL;
        return NULL;
    }
    else if (token[pos] == L'\t') // Return the token including the next tab character.
    {
        if (token[pos + 1] == 0)
        {
            error = true;
            marker = NULL;
            return NULL;
        }
        int tlen = wcslen(token);
        marker = token;

        if (token[tlen - 1] != L'\t')
        {
            *(marker + tlen) = L' ';

            NextToken(searchdata, L'\t');

            if (searchdata.separator != L'\t' || (*searchdata.next != L' ' && *searchdata.next != 0)) // End of string before tab character was found or tab was in the middle of a string.
            {
                error = true;
                marker = NULL;
                return NULL;
            }
        }
        else
            *(marker + tlen - 1) = 0;

        *(marker + pos) = 0;
        token = marker + pos + 1;
        if (pos == 0)
            marker = NULL;
        searchdata.separator = L'\t';
        return token;
    }
    else
    {
        RestoreToken(searchdata);

        marker = token;

        //wchar_t del[] = L"\t ";
        //del[1] = closingbracket;

        while (searchdata.separator != closingbracket)
        {
            NextToken(searchdata, L'\t', closingbracket);
            if (searchdata.separator == 0)
            {
                error = true;
                marker = NULL;
                return NULL;
            }
            else if (searchdata.separator == L'\t') // Restore TAB and look for its pair.
            {
                *(searchdata.next - 1) = L'\t';
                NextToken(searchdata, L'\t');
                if (searchdata.separator != L'\t' || (*searchdata.next != L' ' && *searchdata.next != 0 && *searchdata.next != closingbracket)) // End of string before tab character was found or tab was in the middle of a string.
                {
                    error = true;
                    marker = NULL;
                    return NULL;
                }
                *(searchdata.next - 1) = L'\t';
            }
            else // Closing bracket found. If it matches, return the tokens, otherwise error.
            {
                if ((pos != 0 && wcsncmp(marker, searchdata.next, pos)) || (*(searchdata.next + pos) != L' ' && *(searchdata.next + pos) != 0))
                {
                    error = true;
                    marker = NULL;
                    return NULL;
                }
                searchdata.next += pos;
            }
        }

        *(marker + pos) = 0;
        token = marker + pos + 1;
        return token;
    }
}
static wchar_t* FirstPattern(TTokenSearchData &searchdata, wchar_t *str, wchar_t* &marker, bool &error, bool secondary) // Same as NextPattern, with a specified string.
{
    searchdata.separator = 0;
    searchdata.next = str;

    return NextPattern(searchdata, marker, error, secondary);
}
static int WordTagIndex(wchar_t **texts, int textcount, wchar_t *searchstr)
{
    for (int ix = 0; ix < textcount; ++ix)
        if (!wcscmp(texts[ix], searchstr))
            return ix;
    return -1;
}
static void wchartrim(wchar_t* &str) // Returns a trimmed version of str, removing all spaces from the start and end. Modifies the string.
{
    while (*str == L' ')
        ++str;
    wchar_t *strend = str + wcslen(str) - 1;
    while (strend >= str && *strend == L' ')
        --strend;
    *(strend + 1) = 0;
}
static bool onlywhitespace(wchar_t *str) // Returns true if the string only consists of spaces and tabs.
{
    while (str[0] == L' ' || str[0] == L'\t')
        ++str;
    return str[0] == 0;
}
//---------------------------------------------------------------------------
int TImportList::ReadExportedDictionary(UnicodeString fname, const TImportOptions &options)
{
    AddMessage(L"Opening export file...");

    FILE *f = _wfopen(fname.c_str(), L"rb");
    wchar_t line[4096]; // Maximum line length.

    // Enumeration of sections in export files. rsUnknown is used for unrecognized sections that will be ignored during import.
    enum TReadSection { rsNone, rsAbout, rsWords, rsKanji, rsUnknown };

    TReadSection sec = rsNone;
    TTokenSearchData tokendata;
    wchar_t *token;

    if (!f)
    {
        MsgBox(L"Can't open file for import.", L"Error", MB_OK);
        return mrClose;
    }

    fImporter->Cancellable = true;

    AddMessage(L"Reading export file...");

    bool infofirst = true; // First info line in the [About] section.

    int linenum = 0;
    try
    {
        while (!fImporter->Aborted && getlinemb(line, 4096, f))
        {
            ++linenum;
            if (line[0] == L';' || line[0] == L'#' || line[0] == 0 || onlywhitespace(line)) // Skip comments and empty lines.
                continue;

            if (((linenum + 1) % 150) == 0)
            {
                Application->ProcessMessages();
                if (fImporter->Aborted)
                    continue;
            }

            if (line[0] == L'[')
            {
                UnicodeString secname = TrimRight(UnicodeString(line + 1));
                if (secname.Length() == 0 || secname[secname.Length()] != L']')
                {
                    AddError(linenum, ietInvalidValue, L"incomplete section name");
                    continue;

                }
                secname = GenLower(secname.SubString(1, secname.Length() - 1));

                AddMessage(L"Found section: " + secname);
                if (fImporter->Aborted)
                    continue;

                if (secname == L"about")
                {
                    if (sec == rsNone)
                        sec = rsAbout;
                    else
                    {
                        AddError(linenum, ietBadFormat, L"About section not first");
                        sec = rsUnknown;
                    }
                }
                else if (secname == L"words")
                    sec = rsWords;
                else if (secname == L"kanji")
                    sec = rsKanji;
                else
                    sec = rsUnknown;
                continue;
            }
            if (sec == rsNone)
            {
                AddError(linenum, ietGeneralError, L"data outside section");
                continue;
            }

            if (sec == rsUnknown || (sec == rsAbout && !options.fullimport)) // Skip unknown sections.
                continue;

            if (sec == rsAbout)
            {
                if (wcslen(line) > 1000)
                {
                    AddError(linenum, ietBadFormat, L"too long About line");
                    continue;
                }
                if (line[0] != L'-' && line[0] != L'*')
                {
                    AddError(linenum, ietBadFormat, L"About line must start with * or -");
                    continue;
                }

                if (line[0] == L'*' && !infofirst)
                    cinfo += L"\r\n";

                infofirst = false;

                //if (line[0] == L'-' && !cinfo.IsEmpty() && line[1] != 0 && wcschr(cdelim, line[1]) == NULL)
                //{
                //    line[0] = L' ';
                //    cinfo += line;
                //    continue;
                //}

                cinfo += line + 1;
            }
            else if (sec == rsWords && (options.wordgroups || options.entries || options.fullimport))
            {
                // Read kanji and kana, and find entry in words list.
                wchar_t *kanji;
                wchar_t *kana;

                token = FirstToken(tokendata, line, L' ');
                if (!token || token[0] == 0 || !valid_kanji(token)) // Missing kanji.
                {
                    AddError(linenum, ietDataMissing, L"word kanji");
                    continue;
                }
                kanji = token;

                token = NextToken(tokendata, L' ');
                if (!token || token[0] == 0) // Missing kana.
                {
                    AddError(linenum, ietDataMissing, L"word kana");
                    continue;
                }
                if (!only_kana(token))
                {
                    AddError(linenum, ietInvalidValue, L"only kana and dash characters accepted for word kana");
                    continue;
                }
                kana = token;

                TExportedWord *exp = NULL;
                int freq = -1;

                wchar_t *marker;
                bool error;

                bool groupfound = false; // Set to true if any definitions in the word were added to a group.

                token = NextPattern(tokendata, marker, error, false);
                while (!fImporter->Aborted && !error && token != NULL)
                {
                    if ((tokendata.separator == L' ' || tokendata.separator == 0) && TokenMatch(token, L"F")) // Word frequency.
                    {
                        if (wcslen(token) == 1)
                        {
                            AddError(linenum, ietInvalidValue, L"no frequency number");
                        }
                        else if (freq == -1)
                        {
                            try
                            {
                                freq = StrToInt(token + 1);
                                if (freq < 0 || freq > 10000)
                                    THROW(L"Invalid value for frequency.");
                            }
                            catch(EConvertError&)
                            {
                                freq = -1;
                                AddError(linenum, ietNotANumber, L"incorrect frequency");
                            }
                            catch(wchar_t*)
                            {
                                freq = -1;
                                AddError(linenum, ietInvalidValue, L"frequency out of range");
                            }
                        }
                    } /* match closing bracket in next character constant { */
                    else if (tokendata.separator == L'}' && !wcscmpi(marker, L"M")) // Found meaning pattern.
                    {
// Meaning pattern:
// \t[word definition]\t #[meaning number] MT[list of word types separated by comma (see wtypetext in zkformats.cpp)] MN[list of notes (wnotetext)] MF[list of fields (wfieldtext)] NT[list of name tags (ntagtext)] G(\t[group name]\t #[entry index])G
                        TTokenSearchData tdata;
                        token = FirstPattern(tdata, token, marker, error, true);
                        if (error)
                        {
                            AddError(linenum, ietBadFormat, L"corrupt definition data");
                            break;
                        }
                        if (tdata.separator != L'\t' || marker != NULL) // Meaning pattern must start with meaning enclosed between TAB characters.
                        {
                            AddError(linenum, ietDataMissing, L"bad or missing word meaning, skipped");
                            token = NextPattern(tokendata, marker, error, false);
                            continue;
                        }

                        bool typefound = false, notefound = false, fieldfound = false, namefound = false;
                        int wtype = 0, wnote = 0, wfield = 0, wname = 0;

                        wchar_t *meaning = token;

                        bool skipmeaningdata = false; // Indicates whether the meaning has been added once, and only the group at the end should be considered.
                        int meaningnum = -1;
                        int mpos = -1;

                        // Look for a duplicate in the already read data. If found, use that meaning in case groups are specified.
                        if (exp != NULL)
                        {
                            for (int ix = 0; ix < exp->DefCount; ++ix)
                            {
                                if (wcscmp(exp->Def[ix]->data.meaning, meaning))
                                    continue;
                                meaningnum = exp->Def[ix]->num;
                                mpos = ix;
                                skipmeaningdata = true;
                                break;
                            }
                        }

                        token = NextPattern(tdata, marker, error, true);
                        if (!error && token && token[0] == L'#') // Index of meaning.
                        {
                            try
                            {
                                int newnum = StrToInt(token + 1);
                                if (newnum < 0 || newnum > 99)
                                    THROW(L"Invalid number for meaning.");
                                if (!skipmeaningdata) // The number used for the meaning is only useful if it is not matching another meaning already added.
                                    meaningnum = newnum;
                            }
                            catch(EConvertError&)
                            {
                                AddError(linenum, ietNotANumber, L"invalid meaning number, meaning skipped"); // A number must follow the # character. The meaning (and probably the whole line) has an error which can't be ignored.
                                continue;
                            }
                            catch(const wchar_t*)
                            {
                                AddError(linenum, ietInvalidValue, L"meaning number must be between 0 and 99"); // Invalid meaning number doesn't invalidate the whole line, though the data could be corrupted.
                            }
                            token = NextPattern(tdata, marker, error, true);
                        }

                        if (!skipmeaningdata && exp != NULL)
                        {
                            if (meaningnum != -1)
                            {
                                mpos = exp->MeaningPos(meaningnum);
                                if (mpos >= 0) // A meaning with the same data was already specified.
                                    skipmeaningdata = true;
                            }
                            else if (exp->DefCount < 100) // No meaning number specified. Add definition out of order to the end of the definitions array.
                                mpos = -exp->DefCount - 1;

                            if (mpos < 0 && exp->DefCount == 100)
                            {
                                AddError(linenum, ietLimit, L"can't add any more meanings, meaning skipped");
                                continue;
                            }
                        }

                        // Look for word types, notes, fields, name tags or groups.
                        while(!fImporter->Aborted && !error && token)
                        {
//  MT[list of word types separated by comma (see wtypetext in zkformats.cpp)]
//  MN[list of notes (wnotetext)]
//  MF[list of fields (wfieldtext)]
//  NT[list of name tags (ntagtext)]
                            // Look for tokens until group definition found.
                            if (skipmeaningdata || (tdata.separator == L')' && !wcscmpi(marker, L"G")))
                                break;
                            if ((tdata.separator == L' ' || tdata.separator == 0))
                            {
                                //MT[list of word types separated by comma (see wtypetext in zkformats.cpp)]
                                if (!typefound && TokenMatch(token, L"MT")) // type
                                {
                                    typefound = true;
                                    TTokenSearchData tdata2;
                                    wchar_t *typetoken = FirstToken(tdata2, token + 2, L',');
                                    while (typetoken)
                                    {
                                        int t = WordTagIndex(wtypetext, WT_COUNT, typetoken);
                                        if (t >= 0)
                                            wtype |= (1 << t);
                                        //if (tdata2.next) // Restore 0 to separator character after previous token.
                                        //    *(tdata2.next - 1) = tdata2.separator;
                                        typetoken = NextToken(tdata2, L',');
                                    }
                                }
                                //MN[list of notes (wnotetext)]
                                else if (!notefound && TokenMatch(token, L"MN")) // type
                                {
                                    notefound = true;
                                    TTokenSearchData tdata2;
                                    wchar_t *notetoken = FirstToken(tdata2, token + 2, L',');
                                    while (notetoken)
                                    {
                                        int t = WordTagIndex(wnotetext, WN_COUNT, notetoken);
                                        if (t >= 0)
                                            wnote |= (1 << t);
                                        //if (tdata2.next) // Restore 0 to separator character after previous token.
                                        //    *(tdata2.next - 1) = tdata2.separator;
                                        notetoken = NextToken(tdata2, L',');
                                    }
                                }
                                //MF[list of fields (wfieldtext)]
                                else if (!fieldfound && TokenMatch(token, L"MF")) // type
                                {
                                    fieldfound = true;
                                    TTokenSearchData tdata2;
                                    wchar_t *fieldtoken = FirstToken(tdata2, token + 2, L',');
                                    while (fieldtoken)
                                    {
                                        int t = WordTagIndex(wfieldtext, WN_COUNT, fieldtoken);
                                        if (t >= 0)
                                            wfield |= (1 << t);
                                        //if (tdata2.next) // Restore 0 to separator character after previous token.
                                        //    *(tdata2.next - 1) = tdata2.separator;
                                        fieldtoken = NextToken(tdata2, L',');
                                    }
                                }
                                //NM[list of name tags (ntagtext)]
                                else if (!namefound && TokenMatch(token, L"NT")) // type
                                {
                                    namefound = true;
                                    TTokenSearchData tdata2;
                                    wchar_t *nametoken = FirstToken(tdata2, token + 2, L',');
                                    while (nametoken)
                                    {
                                        int t = WordTagIndex(ntagtext, NM_COUNT, nametoken);
                                        if (t >= 0)
                                            wname |= (1 << t);
                                        //if (tdata2.next) // Restore 0 to separator character after previous token.
                                        //    *(tdata2.next - 1) = tdata2.separator;
                                        nametoken = NextToken(tdata2, L',');
                                    }
                                }
                            }
                            token = NextPattern(tdata, marker, error, true);
                        }
                        if (error)
                        {
                            AddError(linenum, ietBadFormat, L"corrupt definition data");
                            break;
                        }
                        if (fImporter->Aborted)
                            continue;

                        int wix = FindWordIndex(coll, kanji, kana);
                        if (options.entries && options.newword == iwoSkip) // Don't add word during dictionary entries import if new words are not allowed.
                            continue;

                        // Add meaning to word after every tag was processed, and we are working on dictionary update or group update if a word is not found in the base dictionary. In the latter case the word will have to be removed if no groups were found for it.
                        if ((options.fullimport || options.entries || options.modifydict) && mpos < 0 && (!options.wordgroups || wix < 0))
                        {
                            if (!exp) // Add word entry first.
                                exp = AddWord(wix, kanji, kana, freq);
                            exp->InsertMeaning(-1 - mpos, meaningnum, meaning, wtype, wnote, wfield, wname);
                            mpos = -1 - mpos;
                        }
// Group pattern:
// \t[group name]\t#[entry index]
                        if (options.wordgroups) // Look for groups.
                        {
                            while (!fImporter->Aborted && !error && token)
                            {
                                if (tdata.separator == L')' && !wcscmpi(marker, L"G"))
                                {
                                    TTokenSearchData gdata;
                                    token = FirstPattern(gdata, token, marker, error, true);
                                    if (error)
                                        break;
                                    if (token && token[0] != 0 && gdata.separator == L'\t' && !marker)
                                    {
                                        wchar_t *gname = token;
                                        if ((int)wcslen(gname) > group_name_length_limit)
                                        {
                                            AddError(linenum, ietInvalidValue, (L"word group name length is limited to " + IntToStr(group_name_length_limit) + L" characters, trimming").c_str());
                                            gname[group_name_length_limit] = 0;
                                        }
                                        int entrynum = -1;
                                        token = NextPattern(gdata, marker, error, true);
                                        if (error)
                                            break;
                                        if (token && (gdata.separator == L' ' || gdata.separator == 0) && token[0] == L'#')
                                        {
                                            try
                                            {
                                                entrynum = StrToInt(token + 1);
                                                if (entrynum < 0)
                                                    THROW(L"Invalid entry index.");
                                            }
                                            catch(EConvertError&)
                                            {
                                                AddError(linenum, ietNotANumber, L"invalid entry index, skipping to next word meaning"); // A number must follow the # character. The meaning (and probably the whole line) has an error which can't be ignored.
                                                break;
                                            }
                                            catch(const wchar_t*)
                                            {
                                                AddError(linenum, ietInvalidValue, L"negative entry index"); // A positive number must follow the # character. The line could still be usable.
                                                entrynum = -1;
                                            }
                                        }

                                        TImportWordGroup* group = AddWordGroup(gname);
                                        if (mpos < 0) // Add word meaning to entry before it could be inserted to group.
                                        {
                                            if (!exp) // Add word entry first.
                                                exp = AddWord(wix, kanji, kana, freq);
                                            exp->InsertMeaning(-1 - mpos, meaningnum, meaning, wtype, wnote, wfield, wname);
                                            mpos = -1 - mpos;
                                        }
                                        if (!exp->AddToGroup(mpos, group, entrynum))
                                            AddError(linenum, ietDuplicate, L"word meaning index in group");
                                        else
                                        {
                                            WordToGroup(gname, entrynum, exp, exp->Def[mpos]);
                                            groupfound = true;
                                        }
                                    }
                                }

                                token = NextPattern(tdata, marker, error, true);
                            }
                            if (error)
                            {
                                AddError(linenum, ietBadFormat, L"corrupt definition data");
                                break;
                            }
                            if (fImporter->Aborted)
                                continue;
                        }
                        if (fImporter->Aborted)
                            continue;

                    }
                    if (fImporter->Aborted)
                        continue;
                    token = NextPattern(tokendata, marker, error, false);
                }
                if (fImporter->Aborted)
                    continue;

                // Entry added to the wordlist not in the base dictionary must be deleted for group import if no groups were found.
                if (exp && options.wordgroups && options.modifydict && !groupfound)
                    wordlist.Delete(FindWord(exp));
            }
            else if (sec == rsKanji && ((!options.entries && (options.kanjigroups || options.kanjiex || options.kanjiuserdef)) || (options.entries && options.kanjidictdef) || options.fullimport))
            {
                TExportedKanji *exp = NULL;

                token = FirstToken(tokendata, line, L' ');
                //[kanji character] D\\t[dictionary kanji meaning]\\t U\\t[user defined kanji meaning]\\t G{\\t[group name]\\t #[entry index]}G W{[word kanji] [word kana] WU\\t[user defined word meaning]\\t}W
                if (!token || !KANJI(token[0]))
                {
                    AddError(linenum, ietInvalidValue, L"kanji character expected");
                    continue;
                }
                if (wcslen(token) != 1)
                {
                    AddError(linenum, ietBadFormat, L"space separator missing");
                    continue;
                }

                wchar_t kanji = token[0];

                wchar_t *dictmeaning = NULL; // Meaning only imported for dictionary imports, not groups.
                wchar_t *usermeaning = NULL; // User definition only imported for group import.

                wchar_t *marker;
                bool error;

                token = NextPattern(tokendata, marker, error, false);
                if (!error && !token)
                {
                    AddError(linenum, ietDataMissing, L"no kanji data to read");
                    continue;
                }
                while (!fImporter->Aborted && !error && token)
                {
                    if (tokendata.separator == L'\t')
                    {
                        if (!wcscmpi(marker, L"D"))
                        {
                            if (dictmeaning)
                            {
                                AddError(linenum, ietDuplicate, L"kanji dictionary meaning");
                                continue;
                            }
                            if (wcslen(token) == 0)
                            {
                                AddError(linenum, ietDataMissing, L"kanji dictionary meaning");
                                continue;
                            }
                            dictmeaning = token;
                            if ((!options.kanjigroups && options.kanjidictdef) || options.fullimport)
                            {
                                if (!exp)
                                    exp = AddKanji(kanji);
                                exp->DictDef = dictmeaning;
                            }
                        }
                        else if (!wcscmpi(marker, L"U"))
                        {
                            if (usermeaning)
                            {
                                AddError(linenum, ietDuplicate, L"kanji user defined meaning");
                                continue;
                            }
                            if (!token || wcslen(token) == 0)
                            {
                                AddError(linenum, ietDataMissing, L"kanji user defined meaning");
                                continue;
                            }
                            usermeaning = token;
                            if (!options.entries && options.kanjiuserdef)
                            {
                                if (!exp)
                                    exp = AddKanji(kanji);
                                exp->UserDef = usermeaning;
                            }
                        }
                    } /* to match closing bracket in next line { */
                    else if (tokendata.separator == L'}')
                    {
                        if (!wcscmpi(marker, L"G") && options.kanjigroups)
                        {
                            //G{\\t[group name]\\t #[entry index]}G

                            TTokenSearchData gdata;
                            token = FirstPattern(gdata, token, marker, error, true);
                            if (error)
                                break;

                            if (token && token[0] != 0 && gdata.separator == L'\t' && !marker) // Group name followed by index of kanji.
                            {
                                wchar_t *gname = token;
                                if ((int)wcslen(gname) > group_name_length_limit)
                                {
                                    AddError(linenum, ietInvalidValue, (L"kanji group name length is limited to " + IntToStr(group_name_length_limit) + L" characters, trimming").c_str());
                                    gname[group_name_length_limit] = 0;
                                }
                                int entrynum = -1;
                                token = NextPattern(gdata, marker, error, true);
                                if (error)
                                    break;
                                if (token && (gdata.separator == L' ' || gdata.separator == 0) && token[0] == L'#')
                                {
                                    try
                                    {
                                        entrynum = StrToInt(token + 1);
                                        if (entrynum < 0)
                                            THROW(L"Invalid entry index.");
                                    }
                                    catch(EConvertError&)
                                    {
                                        AddError(linenum, ietNotANumber, L"invalid entry index, skipping kanji group"); // A number must follow the # character. The meaning (and probably the whole line) has an error which can't be ignored.
                                        continue;
                                    }
                                    catch(const wchar_t*)
                                    {
                                        AddError(linenum, ietInvalidValue, L"negative entry index"); // A positive number must follow the # character. The line could still be usable.
                                        entrynum = -1;
                                    }
                                }

                                TImportKanjiGroup *group = AddKanjiGroup(gname);
                                if (!exp)
                                    exp = AddKanji(kanji);
                                exp->AddToGroup(group);
                                KanjiToGroup(gname, entrynum, exp);
                            }

                        }
                        else if (!wcscmpi(marker, L"W") && options.kanjiex)
                        {
                            //W{[word kanji] [word kana] WU\\t[user defined word meaning]\\t}W

                            TTokenSearchData wdata;
                            token = FirstPattern(wdata, token, marker, error, true);
                            if (error)
                                break;
                            if (!token || token[0] == 0 || (wdata.separator != L' ' && wdata.separator != 0))
                            {
                                AddError(linenum, ietDataMissing, L"kanji example word kanji");
                                continue;
                            }
                            wchar_t *wkanji = token;
                            token = NextPattern(wdata, marker, error, true);
                            if (error)
                                break;
                            if (!token || token[0] == 0 || (wdata.separator != L' ' && wdata.separator != 0))
                            {
                                AddError(linenum, ietDataMissing, L"kanji example word kana");
                                continue;
                            }

                            wchar_t *wkana = token;

                            if (wcschr(wkanji, kanji) == NULL)
                            {
                                AddError(linenum, ietInvalidValue, L"kanji not found in example word");
                                continue;
                            }

                            if (!exp)
                                exp = AddKanji(kanji);
                            TExportedKanjiExample *wex = exp->AddExampleWord(Collection, wkanji, wkana);
                            if (wex == NULL) // Over the 65535 limit.
                            {
                                AddError(linenum, ietLimit, L"at most 65535 kanji example words");
                                continue;
                            }

                            token = NextPattern(wdata, marker, error, true);
                            if (error)
                                break;

                            TTokenSearchData udata;
                            token = FirstPattern(udata, token, marker, error, true);
                            while(!fImporter->Aborted && !error && token)
                            {
                                if (udata.separator == L'\t' && !wcscmpi(marker, L"WU") && options.kanjiexdef)
                                {
                                    if (token[0] == 0)
                                        AddError(linenum, ietDataMissing, L"empty example word meaning");
                                    else if (wex->Meaning != NULL)
                                        AddError(linenum, ietDuplicate, L"example word meaning");
                                    else
                                        wex->Meaning = token;
                                }
                                token = NextPattern(udata, marker, error, true);
                            }
                            if (error || fImporter->Aborted)
                                break;
                        }
                    }
                    token = NextPattern(tokendata, marker, error, false);
                }
                if (error)
                {
                    AddError(linenum, ietBadFormat, L"corrupt kanji line");
                    continue;
                }
                if (fImporter->Aborted)
                    continue;
            }
        }
    }
    catch(const wchar_t *err)
    {
        fclose(f);
        MsgBox(UnicodeString(L"Critical error: ") + err + L"\nLine number: " + IntToStr(linenum) + L"\n\nPlease consult the documentation at the beginning of the export file for the file format. If no documentation is present, export some data and read the documentation there.", L"Error", MB_OK);
        return mrClose;
    }
    catch(...)
    {
        fclose(f);
        MsgBox(L"Critical error occurred while reading the export file.", L"Error", MB_OK);
        return mrClose;
    }
    fclose(f);

    if (fImporter->Aborted)
        return mrAbort;
    else
        fImporter->Cancellable = false;

    for (int ix = 0; ix < WordGroupCount; ++ix)
        WordGroups[ix]->UpdateIndexes();
    for (int ix = 0; ix < KanjiGroupCount; ++ix)
        KanjiGroups[ix]->UpdateIndexes();

    return mrOk;
}
//---------------------------------------------------------------------------
void TImportList::FillImportWord(TImportedWord *rw, const TImportOptions &options)
{
    TExportedWord *w = rw->data;
    int importcnt = w->DefCount;

    if (w->Index < 0) // Word with kanji/kana not found in the original dictionary.
    {
        //if (options.entries && options.newword == iwoSkip)
        //    continue;
        rw->newword = true;
        rw->action = options.entries && options.newword == iwoSkip ? wiaSkip : options.entries && options.newword == iwoAuto ? wiaApply : wiaCheck;
        rw->modtype = wiitDiff;
/*
        for (int ix = 0; ix < importcnt; ++ix)
        {
            TImportedDefinition *d = new TImportedDefinition;
            d->pos = ix;
            d->defcnt = 0;
            d->dictdefs = NULL;
            rw->defs.Add(d);
        }
*/
        return;
    }

    int dictcnt = coll->words->Items[w->Index]->meaningcnt;

    short *defmatch = new short[importcnt]; // Which existing definitions correspond to an imported one. -1 means no match found for the given imported definition.

    enum DefTakenEnum : byte { defNotTaken = 0, defTaken = 1, defFullMatch = 2 };
    DefTakenEnum *deftaken = new DefTakenEnum[dictcnt]; // To avoid selecting the same meaning for the imported definition twice, mark them as taken if a match found.
    short *matchindex = new short[dictcnt]; // Which imported definitions correspond to an existing one. -1 means no match found for the given existing definition.

    memset(matchindex, 255, sizeof(short) * dictcnt);
    memset(deftaken, 0, sizeof(byte) * dictcnt);

    int matchcnt = 0; // Number of definitions where the meaning text is found in the dictionary.
    int fullcnt = 0; // Number of definitions where the types are the same not just the meaning text.
    //int origcnt = 0; // Number of matching definitions which are at the same position both in the import and in the dictionary.

    // Look for corresponding definitions.
    for (int iy = 0; iy < importcnt; ++iy)
    {
        defmatch[iy] = -1;

        for (int z = 0; z < dictcnt; ++z)
        {
            bool fullmatch = false;
            if (deftaken[z] == defFullMatch || !MeaningDataMatch(w->Def[iy]->data, coll->words->Items[w->Index]->data[z], fullmatch))
                continue;

            if (fullmatch || defmatch[iy] == -1) // Either one or the other is true. fullmatch && defmatch[iy] != -1 is impossible as one fullmatch quits the loop.
            {
                if (defmatch[iy] != -1) // Invalidate the previous match, so new definitions can match it.
                {
//                    if (defmatch[iy] == iy)
//                        --origcnt;
                    matchindex[defmatch[iy]] = -1;
                    deftaken[defmatch[iy]] = defNotTaken;
                    defmatch[iy] = -1;
                    --matchcnt;
                }

                if (deftaken[z] == defTaken)
                {
                    if (!fullmatch) // Only fully matching definitions can replace others.
                        continue;

//                    if (matchindex[z] == z)
//                        --origcnt;
                    defmatch[matchindex[z]] = -1;
                    --matchcnt;
                    // Look for another match for the old partial match.
                    for (int k = 0; k < dictcnt; ++k)
                    {
                        bool newfullmatch = false;
                        if (k == z || deftaken[k] != defNotTaken || !MeaningDataMatch(w->Def[matchindex[z]]->data, coll->words->Items[w->Index]->data[k], newfullmatch))
                            continue;
                        deftaken[k] = newfullmatch ? defFullMatch : defTaken;
                        matchindex[k] = matchindex[z];
                        defmatch[matchindex[z]] = k;
                        if (deftaken[k] == defFullMatch)
                            ++fullcnt;
//                        if (defmatch[matchindex[z]] == matchindex[z])
//                            ++origcnt;
                        ++matchcnt;
                        break;
                    }
                }

//                if (iy == z)
//                    ++origcnt;
                defmatch[iy] = z;
                deftaken[z] = fullmatch ? defFullMatch : defTaken;
                matchindex[z] = iy;
                ++matchcnt;

                if (fullmatch)
                {
                    ++fullcnt;
                    break;
                }
            }
        }
    }
    delete[] deftaken;

    bool groupmatch = options.wordgroups && matchcnt == importcnt; // In group import if a match found for every item, there is no need to do anything. Word type mismatch doesn't count.
    bool entryconflict = options.entries && (options.typediff ? fullcnt : matchcnt) != importcnt; // In entries import, words with conflicting meanings might be skipped depending on the selected options.
    bool dmeaningdiff = (options.entries && options.updatetype ? fullcnt : matchcnt) != dictcnt; // In entries import, not all dictionary meanings match the imported.
    bool imeaningdiff = (options.entries && options.updatetype ? fullcnt : matchcnt) != importcnt; // In entries import, not all imported meanings match the dictionary.
    bool dmmeaningdiff = matchcnt != dictcnt; // In entries import, not all dictionary meanings match the imported.
    bool immeaningdiff = matchcnt != importcnt; // In entries import, not all imported meanings match the dictionary.
    bool noconflict = options.entries && !dmeaningdiff && dictcnt == importcnt; // Entries match with those in the dictionary. Only the meanings' order can be different.
    bool additional = options.entries && !dmeaningdiff && importcnt > dictcnt; // In entries import, word has the same definitions as in the dictionary, plus others.
    bool fewer = options.entries && !imeaningdiff && importcnt < dictcnt; // In entries import, word has same but less definitions as dictionary.
    bool madditional = options.entries && !dmmeaningdiff && importcnt > dictcnt; // In entries import, word has the same definitions as in the dictionary, plus others.
    bool mfewer = options.entries && !immeaningdiff && importcnt < dictcnt; // In entries import, word has same but less definitions as dictionary.

    if (noconflict || groupmatch || (options.skipconflict && entryconflict) || (options.newmeaning == iwoSkip && additional) || (options.missingmeaning == iwoSkip && fewer))
    {
        delete[] defmatch;
        delete[] matchindex;
        rw->action = wiaSkip;
        return;
    }

    rw->action = (options.newmeaning == iwoAuto && madditional) || (options.missingmeaning == iwoAuto && mfewer) ? wiaApply : wiaCheck;
    rw->modtype = (!dmmeaningdiff || !immeaningdiff) ? wiitMatch : wiitDiff;

    for (int ix = 0; ix < importcnt; ++ix)
    {
        TImportedDefinition *d = new TImportedDefinition;
        d->pos = ix;
        d->defcnt = 0;
        d->dictdefs = NULL;
        if (defmatch[ix] != -1)
        {
            d->defcnt = 1;
            d->dictdefs = new byte[1];
            d->dictdefs[0] =  defmatch[ix];
            rw->matchdefs.Add(d);
        }
        else
            rw->defs.Add(d);
    }

    if (rw->action != wiaApply)
    {
        if (!dmeaningdiff || !imeaningdiff)
            rw->modtype = options.missingmeaning != iwoAuto ? wiitMatch : wiitMatchDelete;
        else
            rw->modtype = wiitDiff;
    }

    delete[] defmatch;
    delete[] matchindex;

}
//---------------------------------------------------------------------------
void TImportList::CollectForImport(const TImportOptions &options)
{

    for (int ix = 0; ix < wordlist.Count; ++ix)
    {
        TImportedWord *rw = new TImportedWord();
        rw->newword = false;
        rw->data = wordlist[ix];

        FillImportWord(rw, options);

        if (rw->action == wiaSkip || (!rw->newword && rw->matchdefs.Count == 0 && rw->defs.Count == 0))
        {
            delete rw;
            continue;
        }

        importlist.Add(rw);
    }
}
//---------------------------------------------------------------------------
static int wimportcleanupsortposproc(TImportedDefinition **a, TImportedDefinition **b)
{
    return (*a)->pos - (*b)->pos;
}
//---------------------------------------------------------------------------
static int wimportcleanupsortorigproc(TImportedDefinition **a, TImportedDefinition **b)
{
    int amin = 9999;
    for (int ix = 0; ix < (*a)->defcnt; ++ix)
        amin = min(amin, (*a)->dictdefs[ix]);
    int bmin = 9999;
    for (int ix = 0; ix < (*b)->defcnt; ++ix)
        bmin = min(bmin, (*b)->dictdefs[ix]);
    if (amin == bmin)
        return (*a)->pos - (*b)->pos;
    return amin - bmin;
}
//---------------------------------------------------------------------------
void TImportList::CleanupDefs(const TImportOptions &options)
{
    for (int ix = 0; ix < importlist.Count; ++ix)
    {
        TImportedWord *w = importlist[ix];
        if (w->action == wiaSkip)
            continue;

        TWord *d;
        if (w->data->Index >= 0)
        {
            d = coll->words->Items[w->data->Index];
            if (w->newword)
            {
                for (int ix = 0; ix < d->meaningcnt; ++ix)
                {
                    TImportedDefinition *d = new TImportedDefinition;
                    d->pos = ix;
                    d->defcnt = 1;
                    d->dictdefs = new byte[1];
                    d->dictdefs[0] =  ix;
                    w->matchdefs.Add(d);
                }
            }
        }
        else
            continue; // New word.

        int mcnt = w->matchdefs.Count; // Number of meanings remaining in word after import.
        bool match = options.wordgroups;
        if (!w->newword && options.entries && w->data->Index >= 0 && mcnt == d->meaningcnt)
        {
            match = true;
            for (int iy = 0; iy < mcnt && match; ++iy)
            {
                if (w->matchdefs[iy]->pos != w->matchdefs[iy]->dictdefs[0] || (options.updatetype && !WordTypeMatch(w->data->Def[w->matchdefs[iy]->pos]->data, d->data[iy])))
                    match = false;
            }
        }

        for (int iy = 0; iy < w->defs.Count; ++iy)
        {
            if (w->defs[iy]->pos >= 0)
                ++mcnt;
        }

        // "Inserting" word would result in an entry with no definitions or no definitions would be updated.
        if (mcnt == 0 || (match && mcnt == w->matchdefs.Count)/*&& (w->modtype & 0x1000) == 0x1000*/)
        {
            w->action = wiaSkip;
            continue;
        }

        // Move everything from w->matchdefs to w->defs that is to be imported, in the order of import.
        TImportedDefinition **defs = new TImportedDefinition*[mcnt]; // Temporary array for holding the definitions to be used in correct order.
        for (int iy = 0; iy < w->matchdefs.Count; ++iy)
        {
            defs[iy] = w->matchdefs[iy];
            w->matchdefs[iy] = NULL;
        }
        int pos = w->matchdefs.Count;
        w->matchdefs.Clear();
        for (int iy = 0; iy < w->defs.Count; ++iy)
        {
            if (w->defs[iy]->pos < 0)
                continue;
            defs[pos++] = w->defs[iy];
            w->defs[iy] = NULL;
        }
        w->defs.Clear();

        // The order of definitions is decided by the current import options. For group import or with the keeporder option, keep the order of meanings as in the current dictionary.
        quicksort<TImportedDefinition*>(defs, 0, pos - 1, options.wordgroups || (options.entries && (w->modtype & 0x1000) != 0x1000) ?  &wimportcleanupsortorigproc : &wimportcleanupsortposproc);

        // Move the cleaned up definition list back to w->defs.
        for (int iy = 0; iy < pos; ++iy)
            w->defs.Add(defs[iy]);
        delete[] defs;
    }
}
//---------------------------------------------------------------------------
void TImportList::EntryUpdate(const TImportOptions &options)
{
    if (importlist.Count == 0 && options.entries)
    {
        AddMessage(L"No dictionary entries to import.");
        return;
    }

    dictionary->BeginWordChange(cix);
    for (int ix = 0; ix < importlist.Count; ++ix)
    {
        TImportedWord *w = importlist[ix];
        if (w->action == wiaSkip)
            continue;

        if (w->data->Index < 0)
        {
            TWord *d = new TWord;
            memset(d, 0, sizeof(TWord));
            d->frequency = w->data->Frequency;
            d->meaningcnt = w->data->DefCount;
            d->data = new TMeaningData[d->meaningcnt];

            d->kanji = new wchar_t[wcslen(w->data->Kanji(coll)) + 1];
            wcscpy(d->kanji, w->data->Kanji(coll));
            d->kana = new wchar_t[wcslen(w->data->Kana(coll)) + 1];
            wcscpy(d->kana, w->data->Kana(coll));
            UnicodeString romaji = Romanize(d->kana);
            d->romaji = new wchar_t[romaji.Length() + 1];
            wcscpy(d->romaji, romaji.c_str());

            for (int ix = 0; ix < d->meaningcnt; ++ix)
            {
                memcpy(d->data + ix, &w->data->Def[ix]->data, sizeof(TMeaningData));
                d->data[ix].meaning = new wchar_t[wcslen(w->data->Def[ix]->data.meaning) + 1];
                wcscpy(d->data[ix].meaning, w->data->Def[ix]->data.meaning);
            }

            dictionary->AddWord(cix, d);
            continue;
        }

        // Create list of meanings not selected in the dictionary.
        byte mcnt = coll->words->Items[w->data->Index]->meaningcnt;
        char *matched = new char[mcnt];
        byte *replacecnt = new byte[mcnt];
        byte **replacers = new byte*[mcnt];
        memset(matched, -1, sizeof(char) * mcnt);
        memset(replacecnt, 0, sizeof(byte) * mcnt);
        memset(replacers, NULL, sizeof(byte*) * mcnt);

        byte dcnt = mcnt; // Number of dictionary definitions that are not matched by an imported one.
        for (int iy = 0; iy < w->defs.Count; ++iy)
        {
            TImportedDefinition *def = w->defs[iy];
            for (int z = 0; z < def->defcnt; ++z)
            {
                ++replacecnt[def->dictdefs[z]];
                if (matched[def->dictdefs[z]] == -1)
                {
                    matched[def->dictdefs[z]] = iy;
                    --dcnt;
                }
            }
        }

        if (options.entries && (w->modtype & wiitMatch) == wiitMatch && w->defs.Count <= mcnt) // Word matches apart from the meanings to be deleted.
        {
            if ((w->modtype & 0x1000) == 0x1000 || options.updatetype)
            {
                for (int iy = mcnt - 1; iy >= 0; --iy)
                {
                    if (matched[iy] < 0)
                    {
                        if ((w->modtype & 0x1000) == 0x1000)
                            dictionary->DeleteWordMeaning(cix, w->data->Index, iy);
                    }
                    else if (options.updatetype && w->defs[matched[iy]]->defcnt == 1)
                    {
#ifdef _DEBUG
                        if (wcscmp(w->data->Def[w->defs[matched[iy]]->pos]->data.meaning, coll->words->Items[w->data->Index]->data[iy].meaning)) /* SHOULD BE ALWAYS FALSE */
                            THROW(L"All imported definitions should match exactly one word definition.");
#endif
                        TMeaningData &dat = coll->words->Items[w->data->Index]->data[iy];
                        TMeaningData &org = w->data->Def[w->defs[matched[iy]]->pos]->data;
                        dat.types = org.types;
                        dat.notes = org.notes;
                        dat.fields = org.fields;
                        dat.nametag = org.nametag;
                    }
                }
            }
        }
        else
        {

            if (options.entries && w->data->Index >= 0 && (w->modtype & 0x1000) == 0x1000)
                dcnt = 0;

            TWord *orig = coll->words->Items[w->data->Index];

            TWord *d = new TWord;
            memset(d, 0, sizeof(TWord));
            d->frequency = w->data->Frequency;
            d->meaningcnt = w->defs.Count + dcnt;
            d->data = new TMeaningData[d->meaningcnt];

            d->kanji = new wchar_t[wcslen(w->data->Kanji(coll)) + 1];
            wcscpy(d->kanji, w->data->Kanji(coll));
            d->kana = new wchar_t[wcslen(w->data->Kana(coll)) + 1];
            wcscpy(d->kana, w->data->Kana(coll));
            UnicodeString romaji = Romanize(d->kana);
            d->romaji = new wchar_t[romaji.Length() + 1];
            wcscpy(d->romaji, romaji.c_str());

            if (orig->stats)
            {
                d->stats = new TWordStatGlobal[d->meaningcnt];
                memset(d->stats, 0, sizeof(TWordStatGlobal) * d->meaningcnt);
            }

            for (int iy = 0; iy < mcnt; ++iy)
            {
                if (replacecnt[iy] == 0)
                    continue;
                replacers[iy] = new byte[replacecnt[iy]];
                memset(replacers[iy], 0, sizeof(byte) * replacecnt[iy]);
            }
            memset(replacecnt, 0, sizeof(byte) * mcnt);

            if ((w->modtype & 0x1000) == 0x1000)
            {
                for (int iy = 0; iy < w->defs.Count; ++iy)
                {
                    memcpy(d->data + iy, &w->data->Def[w->defs[iy]->pos]->data, sizeof(TMeaningData));
                    d->data[iy].meaning = new wchar_t[wcslen(w->data->Def[w->defs[iy]->pos]->data.meaning) + 1];
                    wcscpy(d->data[iy].meaning, w->data->Def[w->defs[iy]->pos]->data.meaning);

                    int mindef = 999;
                    for (int z = 0; z < w->defs[iy]->defcnt; ++z)
                    {
                        mindef = min(mindef, w->defs[iy]->dictdefs[z]);
                        replacers[w->defs[iy]->dictdefs[z]][replacecnt[w->defs[iy]->dictdefs[z]]] = iy;
                        ++replacecnt[w->defs[iy]->dictdefs[z]];
                    }
                    if (d->stats != NULL && mindef < 100)
                        memcpy(d->stats + iy, orig->stats + mindef, sizeof(TWordStatGlobal));

                }

                if (!options.updatetype)
                {
                    for (int iy = 0; iy < w->defs.Count; ++iy)
                    {
                        if (replacecnt[iy] == 1 && !wcscmp(d->data[replacers[iy][0]].meaning, coll->words->Items[w->data->Index]->data[iy].meaning))
                        {
                            TMeaningData &dat = d->data[replacers[iy][0]];
                            TMeaningData &org = coll->words->Items[w->data->Index]->data[iy];
                            dat.types = org.types;
                            dat.notes = org.notes;
                            dat.fields = org.fields;
                            dat.nametag = org.nametag;
                        }
                    }
                }
            }
            else
            {
                byte nextmatched = 0;
                int datpos = 0;

                for (int iy = 0; iy != w->defs.Count + 1; ++iy)
                {
                    while (nextmatched < mcnt && matched[nextmatched] < iy)
                    {
                        if (matched[nextmatched] < 0)
                        {
                            memcpy(d->data + datpos, orig->data + nextmatched, sizeof(TMeaningData));
                            d->data[datpos].meaning = new wchar_t[wcslen(orig->data[nextmatched].meaning) + 1];
                            wcscpy(d->data[datpos].meaning, orig->data[nextmatched].meaning);
                            if (replacers[nextmatched])
                                THROW(L"No imported definition should replace this one.");
                            replacecnt[nextmatched] = 1;
                            replacers[nextmatched] = new byte[1];
                            replacers[nextmatched][0] = datpos;

                            if (d->stats != NULL)
                                memcpy(d->stats + datpos, orig->stats + nextmatched, sizeof(TWordStatGlobal));

                            ++datpos;
                        }
                        ++nextmatched;
                    }

                    if (iy == w->defs.Count)
                        break;

                    memcpy(d->data + datpos, &w->data->Def[w->defs[iy]->pos]->data, sizeof(TMeaningData));
                    d->data[datpos].meaning = new wchar_t[wcslen(w->data->Def[w->defs[iy]->pos]->data.meaning) + 1];
                    wcscpy(d->data[datpos].meaning, w->data->Def[w->defs[iy]->pos]->data.meaning);

                    int mindef = 999;
                    for (int z = 0; z < w->defs[iy]->defcnt; ++z)
                    {
                        mindef = min(mindef, w->defs[iy]->dictdefs[z]);
                        replacers[w->defs[iy]->dictdefs[z]][replacecnt[w->defs[iy]->dictdefs[z]]] = datpos;
                        ++replacecnt[w->defs[iy]->dictdefs[z]];
                    }
                    if (d->stats != NULL && mindef < 100)
                        memcpy(d->stats + datpos, orig->stats + mindef, sizeof(TWordStatGlobal));
                    ++datpos;
                }

                if (options.wordgroups || !options.updatetype)
                {
                    for (int iy = 0; iy < mcnt; ++iy)
                    {
                        if (replacecnt[iy] == 1 && !wcscmp(d->data[replacers[iy][0]].meaning, coll->words->Items[w->data->Index]->data[iy].meaning))
                        {
                            TMeaningData &dat = d->data[replacers[iy][0]];
                            TMeaningData &org = coll->words->Items[w->data->Index]->data[iy];
                            dat.types = org.types;
                            dat.notes = org.notes;
                            dat.fields = org.fields;
                            dat.nametag = org.nametag;
                        }
                    }
                }
            }

            dictionary->ReplaceWord(cix, w->data->Index, d, replacecnt, replacers);
        }

        delete[] matched;
        delete[] replacecnt;
        for (int ix = 0; ix < mcnt; ++ix)
            delete[] replacers[ix];
        delete[] replacers;
    }
    dictionary->EndWordChange();
    coll->Modify(true);

    if (importlist.Count != 0 && options.entries)
    {
        AddMessage(L"Successfully finished importing entries.");
        return;
    }

    importlist.Clear();
}
//---------------------------------------------------------------------------
void TImportList::FillReplaceWord(TReplacedWord *rw)
{
    char *matched = NULL;
    TExportedWord *w = rw->data;

    TWord *d = NULL;
    if (w->Index >= 0)
    {
        d = coll->words->Items[w->Index];
        matched = new char[d->meaningcnt];
        memset(matched, -1, sizeof(char) * d->meaningcnt);
    }

    //int matchcnt = 0;
    for (int iy = 0; iy < w->DefCount; ++iy)
    {
        TExportedDefinition *def = w->Def[iy];
        TImportedDefinition *rdef = new TImportedDefinition;
        rdef->pos = iy;
        rdef->defcnt = 0;
        rdef->dictdefs = NULL;

        if (w->Index < 0)
        {
            rw->defs.Add(rdef);
            continue;
        }

        bool fullmatch = false;
        for (int z = 0; z < d->meaningcnt; ++z)
        {
            if (matched[z] < 0 && MeaningDataMatch(d->data[z], def->data, fullmatch))
            {
                if ((matched[z] != -1 || -2 - matched[z] == rw->defs.Count) && !fullmatch)
                    continue;

                if (matched[z] != -1 && -2 - matched[z] != rw->defs.Count) // Full match overwrites previous matches. Find another for the previous one.
                {
                    int iy2 = -2 - matched[z];
                    TImportedDefinition *rdef2 = rw->defs[iy2];
                    TExportedDefinition *def2 = rw->data->Def[rdef2->pos];
                    rdef2->defcnt = 0;
                    delete[] rdef2->dictdefs;
                    rdef2->dictdefs = NULL;
                    //--matchcnt;
                    for (int z2 = z + 1; z2 < d->meaningcnt; ++z2)
                    {
                        if (matched[z2] != -1)
                            continue;
                        if (MeaningDataMatch(d->data[z2], def2->data))
                        {
                            rdef2->defcnt = 1;
                            rdef2->dictdefs = new byte[1];
                            rdef2->dictdefs[0] = z2;
                            matched[z2] = -2 - iy2;
                            //++matchcnt;
                            break;
                        }
                    }
                }

                matched[z] = fullmatch ? rw->matchdefs.Count : -2 - rw->defs.Count;
                if (rdef->defcnt == 0)
                {
                    rdef->defcnt = 1;
                    rdef->dictdefs = new byte[1];
                    //++matchcnt;
                }
                rdef->dictdefs[0] = z;
            }
            if (fullmatch)
            {
                rw->matchdefs.Add(rdef);
                break;
            }
        }
        if (!fullmatch)
            rw->defs.Add(rdef);
    }

    delete[] matched;
}
//---------------------------------------------------------------------------
void TImportList::CollectForReplace(const TImportOptions &options)
{
    for (int ix = 0; ix < wordlist.Count; ++ix)
    {
        TExportedWord *w = wordlist[ix];
        TReplacedWord *rw = new TReplacedWord;
        replacelist.Add(rw);
        rw->data = w;
        rw->skip = false;
        rw->newword = w->Index < 0;

        FillReplaceWord(rw);
    }
}
//---------------------------------------------------------------------------
void TImportList::DoWordReplace(const TImportOptions &options)
{
    for (int ix = 0; ix < replacelist.Count; ++ix)
    {
        TReplacedWord *w = replacelist[ix];
        if (w->skip)
            w->data->Index = -1;

        for (int iy = 0; iy < w->data->DefCount; ++iy)
            w->data->Def[iy]->num = -1;

        for (int iy = 0; iy < w->matchdefs.Count; ++iy)
            w->data->Def[w->matchdefs[iy]->pos]->num = ix;
        for (int iy = 0; iy < w->defs.Count; ++iy)
            if (w->defs[iy]->pos >= 0 && w->defs[iy]->defcnt != 0)
                w->data->Def[w->defs[iy]->pos]->num = -2 - ix;
    }
}
//---------------------------------------------------------------------------
static int wgrpsortproc(int **a, int **b)
{
    int ea = (*a)[2];
    int eb = (*b)[2];

    if (ea == -1 || eb == -1)
    {
        if (ea != -1)
            return -1;
        if (eb != -1)
            return 1;
        return 0;
    }

    if (ea == eb)
    {
        return (*a)[0] - (*b)[0];
    }

    return ea - eb;
}
void TImportList::WordGroupUpdate(const TImportOptions &options)
{
    TTemplateList<int, true, true> ints;

    for (int m = 0; m < wordgroups.Count; ++m)
    {
        TImportWordGroup *wgrp = wordgroups[m];

        if (wgrp->Action == giaSkip)
            continue;

        for (int ix = 0; ix < wgrp->Count; ++ix)
        {
            TExportedWord *w = wgrp->Items[ix];
            if (w->Index < 0)
                continue;

            for (int iy = 0; iy < w->DefCount; ++iy)
            {
                TExportedDefinition *def = w->Def[iy];
                if (def->num == -1)
                    continue;

                for (int z = 0; z < def->groupcnt; ++z)
                {
                    if (def->groups[z].group == wgrp)
                    {
                        TReplacedWord *rw = replacelist[def->num >= 0 ? def->num : -2 - def->num];
                        int rwpos = -1;
                        if (def->num >= 0)
                        {
                            for (int j = 0; j < rw->matchdefs.Count && rwpos == -1; ++j)
                                if (rw->matchdefs[j]->pos == iy)
                                    rwpos = j;
                        }
                        else
                        {
                            for (int j = 0; j < rw->defs.Count && rwpos == -1; ++j)
                                if (rw->defs[j]->pos == iy)
                                    rwpos = j;
                        }

                        int *i = new int[4];
                        i[0] = ix;
                        i[1] = iy;
                        i[2] = def->groups[z].entry;
                        i[3] = rwpos;
                        ints.Add(i);
                        break;
                    }
                }
            }
        }

        if (!ints.Count)
            continue;

        ints.Sort(wgrpsortproc);

        // Look for the existing group or create one for import.
        int groupindex;
        if (wgrp->Action == giaReplace)
        {
            groupindex = coll->groups->IndexOf(wgrp->Name);
            if (groupindex != -1)
            {
                coll->groups->Delete(groupindex);
                int i = coll->groups->Add(wgrp->Name);
                coll->groups->Move(i, groupindex);
            }
            else
                groupindex = coll->groups->Add(wgrp->Name);
        }
        else
        {
            groupindex = coll->groups->IndexOf(wgrp->Name);
            if (groupindex == -1) // No group with the same name. Create new for import.
                groupindex = coll->groups->Add(wgrp->Name);
        }

        TWordGroup *wg = coll->groups->Items[groupindex];

        for (int ix = 0; ix < ints.Count; ++ix)
        {
            TExportedWord *w = wgrp->Items[ints[ix][0]];
            TExportedDefinition *def = w->Def[ints[ix][1]];
            TReplacedWord *rw = replacelist[def->num >= 0 ? def->num : -2 - def->num];
            TImportedDefinition *imp = def->num >= 0 ? rw->matchdefs[ints[ix][3]] : rw->defs[ints[ix][3]];

            TWord *d = coll->words->Items[w->Index];

            // Add the word meanings to the group.
            for (int z = 0; z < imp->defcnt; ++z)
            {
                TWordGroups *g = d->group;
                bool ingrp = false;
                while (g)
                {
                    if (g->group == wg && g->meaning == imp->dictdefs[z])
                        ingrp = true;
                    g = g->next;
                }
                if (!ingrp)
                    wg->AddWord(w->Index, imp->dictdefs[z]);
            }
        }

        ints.Clear();
    }
}
//---------------------------------------------------------------------------
void TImportList::DictDefUpdate()
{
    int modified = 0;

    for (int ix = 0; ix < kanjilist.Count; ++ix)
        if (kanjilist[ix]->DictDef != NULL && dictionary->ModifyKanjiMeaning(cix, kanjilist[ix]->Index, kanjilist[ix]->DictDef))
            ++modified;

    if (modified == 0)
        AddMessage(L"No kanji definitions imported.");
    else
        AddMessage(IntToStr(modified) + L" kanji definitions imported.");
}
//---------------------------------------------------------------------------
void TImportList::ReviewExamples()
{
    TWordIndexList *l = new TWordIndexList(coll);
    int cnt = kanjilist.Count;
    for (int ix = 0; ix < cnt; ++ix)
    {
        int kcnt = kanjilist[ix]->ExampleCount;
        for (int iy = 0; iy < kcnt; ++iy)
        {
            if (kanjilist[ix]->Example[iy]->Index < 0)
                kanjilist[ix]->Example[iy]->Index = FindWordIndex(l, kanjilist[ix]->Example[iy]->Kanji(coll), kanjilist[ix]->Example[iy]->Kana(coll));
            l->Clear();
        }
    }
    delete l;
}
//---------------------------------------------------------------------------
void TImportList::ExamplesUpdate()
{
    int cnt = kanjilist.Count;
    for (int ix = 0; ix < cnt; ++ix)
    {
        int kcnt = kanjilist[ix]->ExampleCount;
        for (int iy = 0; iy < kcnt; ++iy)
        {
            TExportedKanji *k = kanjilist[ix];
            int kix = k->Index;
            if (k->Example[iy]->Index >= 0)
            {
                if (coll->kanjidat[kix].card == NULL)
                    coll->kanjidat[kix].card = new TKanjiCard(kanjis->Kanjis[kix]);

                byte kpos = 0;
                int klen = wcslen(k->Example[iy]->Kanji(coll));
                for (int z = 0; z < klen; ++z)
                {
                    if (k->Example[iy]->Kanji(coll)[z] == k->Kanji)
                    {
                        kpos = z;
                        break;
                    }
                }

                if (kpos == 0 && k->Example[iy]->Kanji(coll)[0] != k->Kanji) // Kanji not found in example word.
                {
                    k->Example[iy]->Index = -1; // Probably not necessary, because there is a check to find the kanji in the example when reading the file.
                    continue;
                }
                coll->kanjidat[kix].card->AddExample(FindReading(kpos, dictionary->Words[cix]->Items[k->Example[iy]->Index]), k->Example[iy]->Index, false);
            }
        }
    }
}
//---------------------------------------------------------------------------
void TImportList::ExampleDefUpdate()
{
    int cnt = kanjilist.Count;
    for (int ix = 0; ix < cnt; ++ix)
    {
        TExportedKanji *k = kanjilist[ix];
        int kcnt = k->ExampleCount;
        for (int iy = 0; iy < kcnt; ++iy)
        {
            TExportedKanjiExample *kex = k->Example[iy];
            if (kex->Index >= 0 && kex->Meaning != NULL)
                coll->deflist[kex->Index] = kex->Meaning;
        }
    }
}
//---------------------------------------------------------------------------
void TImportList::UserDefUpdate()
{
    int cnt = kanjilist.Count;
    for (int ix = 0; ix < cnt; ++ix)
    {
        TExportedKanji *k = kanjilist[ix];
        if (k->UserDef != 0)
            coll->kdeflist[k->Index] = k->UserDef;
    }
}
//---------------------------------------------------------------------------
void TImportList::KanjiGroupUpdate(const TImportOptions &options)
{
    for (int m = 0; m < kanjigroups.Count; ++m)
    {
        TImportKanjiGroup *kgrp = kanjigroups[m];

        if (kgrp->Action == giaSkip || kgrp->Count == 0)
            continue;

        // Look for the existing group or create one for import.
        int groupindex;
        if (kgrp->Action == giaReplace)
        {
            UnicodeString kname = kgrp->Name;
            groupindex = coll->kgroups->IndexOf(kname);
            if (groupindex != -1)
            {
                coll->kgroups->Delete(groupindex);
                int i = coll->kgroups->Add(kgrp->Name);
                coll->kgroups->Move(i, groupindex);
            }
            else
                groupindex = coll->kgroups->Add(kgrp->Name);
        }
        else
        {
            UnicodeString kname = kgrp->Name;
            groupindex = coll->kgroups->IndexOf(kname);
            if (groupindex == -1) // No group with the same name. Create new for import.
                groupindex = coll->kgroups->Add(kgrp->Name);
        }
        TKanjiGroup *kg = coll->kgroups->Items[groupindex];

        for (int ix = 0; ix < kgrp->Count; ++ix)
        {
            TExportedKanji *k = kgrp->Items[ix];

            if (kg->IndexOf(k->Index) < 0)
                kg->Add(k->Index);
        }
    }
}
//---------------------------------------------------------------------------
int TImportList::FullDictImport()
{
    bool error = false;

    fImporter->Cancellable = true;
    AddMessage(L"Expanding dictionary with imported words. This can take a while...");

    dictionary->BeginWordChange(cix);
    int mn = 0; // Number of added meanings.
    try
    {
        for (int ix = 0; !fImporter->Aborted && ix < wordlist.Count; ++ix)
        {
            TExportedWord *w = wordlist[ix];
            w->SortDefinitions();

            int wix;
            for (int iy = 0; iy < w->DefCount; ++iy)
            {
                TExportedDefinition *def = w->Def[iy];

                if (iy == 0)
                    wix = dictionary->AddWord(cix, w->Kanji(coll), w->Kana(coll), def->data.meaning, def->data.types, def->data.notes, def->data.fields, def->data.nametag, w->Frequency);
                else
                    dictionary->AddWordMeaning(cix, wix, def->data.meaning, def->data.types, def->data.notes, def->data.fields, def->data.nametag);
                ++mn;
            }

            if (((ix + 1) % 500) == 0)
                Application->ProcessMessages();
            if (((ix + 1) % 5000) == 0)
                AddMessage(L"Passed words: " + IntToStr(ix + 1));

        }
    }
    catch(...)
    {
        error = true;
        fImporter->Abort();
    }
    dictionary->EndWordChange();

    if (!fImporter->Aborted)
    {
        AddMessage(L"Successfully imported: ");
        AddMessage(L"Words: " + IntToStr(wordlist.Count));
        AddMessage(L"Meanings: " + IntToStr(mn));
        fImporter->Cancellable = false;
    }

    return error ? mrClose : !fImporter->Aborted ? mrOk : mrAbort;
}
//---------------------------------------------------------------------------


