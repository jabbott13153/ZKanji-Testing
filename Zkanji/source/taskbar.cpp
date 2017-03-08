//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "taskbar.h"
#include <propvarutil.h>
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(216);
#endif

//---------------------------------------------------------------------------
TTaskbar::TTaskbar(UnicodeString appID) : tasks(NULL), id(appID)
{
    int res = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (res != S_FALSE && res != S_OK)
        THROW(L"Couldn't initialize COM!");

    if (MatchVersion(6, 1))
        tasks = new TTaskList;

}
//---------------------------------------------------------------------------
TTaskbar::~TTaskbar()
{
    delete tasks;
    CoUninitialize();
}
//---------------------------------------------------------------------------
void TTaskbar::ClearTasks()
{
    if (!MatchVersion(6, 1))
        return;

    ICustomDestinationList *cdl;
    InitList(cdl);
    cdl->CommitList();
    cdl->Release();
}
//---------------------------------------------------------------------------

//HRESULT InitPropVariantFromString(PCWSTR psz,PROPVARIANT *ppropvar);
extern "C" const PROPERTYKEY DECLSPEC_SELECTANY PKEY_Title = { { 0xF29F85E0, 0x4FF9, 0x1068, { 0xAB, 0x91, 0x08, 0x00, 0x2B, 0x27, 0xB3, 0xD9 } } , 2 };
extern "C" const PROPERTYKEY DECLSPEC_SELECTANY PKEY_AppUserModel_IsDestListSeparator = { { 0x9F4C2855, 0x9F79, 0x4B39, { 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3 } } , 6 };

void TTaskbar::AddTask(UnicodeString title, UnicodeString path, UnicodeString params, UnicodeString description, int iconindex, UnicodeString iconpath)
{
    if (!MatchVersion(6, 1))
        return;

    TTask *task = new TTask;
    task->title = title;
    task->path = path;
    task->params = params;
    task->description = description;
    task->iconindex = iconindex;
    task->iconpath = iconpath;

    tasks->Add(task);
}
//---------------------------------------------------------------------------
void TTaskbar::AddSeparator()
{
    if (!MatchVersion(6, 1))
        return;

    AddTask("", "", "", "", 0, "");
}
//---------------------------------------------------------------------------
void TTaskbar::InitList(ICustomDestinationList* &cdl)
{
    int res;
    if ((res = CoCreateInstance(CLSID_DestinationList, NULL, CLSCTX_ALL, IID_ICustomDestinationList, (void**) &cdl)) != S_OK)
    {
        switch (res)
        {
        case REGDB_E_CLASSNOTREG:
            THROW(L"Couldn't create interface ICustomDestinationList!");
        case CLASS_E_NOAGGREGATION:
            THROW(L"Couldn't create interface ICustomDestinationList!");
        case E_NOINTERFACE:
            THROW(L"Couldn't create interface ICustomDestinationList!");
        case E_POINTER:
            THROW(L"Couldn't create interface ICustomDestinationList!");
        default:
            THROW(L"Couldn't create interface ICustomDestinationList! Error.");
        }
    }

    unsigned int mslot;
    IObjectArray *removedoa;
    cdl->BeginList(&mslot, IID_IObjectArray, (void**)&removedoa);
    removedoa->Release();
}
//---------------------------------------------------------------------------
void TTaskbar::Commit()
{
    if (!MatchVersion(6, 1))
        return;

    ICustomDestinationList *cdl;
    InitList(cdl);

    //if (cdl->SetAppID(id.c_str()) != S_OK)
    //THROW(L"Couldn't set application ID for tasks list!";

    IObjectCollection *oc;
    int res;
    if ((res = CoCreateInstance(CLSID_EnumerableObjectCollection, NULL, CLSCTX_ALL, IID_IObjectCollection, (void**) &oc)) != S_OK)
    {
        switch (res)
        {
        case REGDB_E_CLASSNOTREG:
            THROW(L"Couldn't create interface IObjectCollection!");
        case CLASS_E_NOAGGREGATION:
            THROW(L"Couldn't create interface IObjectCollection!");
        case E_NOINTERFACE:
            THROW(L"Couldn't create interface IObjectCollection!");
        case E_POINTER:
            THROW(L"Couldn't create interface IObjectCollection!");
        default:
            THROW(L"Couldn't create interface IObjectCollection! Error.");
        }
    }

    IShellLink *sl;
    for (int ix = 0; ix < tasks->Count; ++ix)
    {
        if ((res = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_ALL, IID_IShellLinkW, (void**) &sl)) != S_OK)
        {
            switch (res)
            {
            case REGDB_E_CLASSNOTREG:
                THROW(L"Couldn't create interface IShellLink!");
            case CLASS_E_NOAGGREGATION:
                THROW(L"Couldn't create interface IShellLink!");
            case E_NOINTERFACE:
                THROW(L"Couldn't create interface IShellLink!");
            case E_POINTER:
                THROW(L"Couldn't create interface IShellLink!");
            default:
                THROW(L"Couldn't create interface IShellLink! Error.");
            }
        }

        TTask *task = tasks->Items[ix];

        if (task->title != L"")
        {
            sl->SetDescription(task->description.c_str());
            sl->SetPath(task->path.c_str());
            sl->SetArguments(task->params.c_str());
            sl->SetIconLocation(task->iconpath == L"" ? task->path.c_str() : task->iconpath.c_str(), task->iconindex);

            IPropertyStore *ps;
            sl->QueryInterface(IID_IPropertyStore, (void**)&ps);

            PROPVARIANT pv;
            InitPropVariantFromString(task->title.c_str(), &pv);
            ps->SetValue(PKEY_Title, pv);
            PropVariantClear(&pv);

            ps->Commit();
            ps->Release();
        }
        else
        {
            IPropertyStore *ps;
            sl->QueryInterface(IID_IPropertyStore, (void**)&ps);
            PROPVARIANT pv;
            InitPropVariantFromBoolean(true, &pv);
            ps->SetValue(PKEY_AppUserModel_IsDestListSeparator, pv);
            PropVariantClear(&pv);

            ps->Commit();
            ps->Release();
        }

        oc->AddObject(sl);
        sl->Release();
    }

    IObjectArray *taskoa;
    oc->QueryInterface(IID_IObjectArray, (void**)&taskoa);
    oc->Release();

    if (cdl->AddUserTasks(taskoa) != S_OK)
        THROW(L"Couldn't add tasks to task icon!");

    cdl->CommitList();
    cdl->Release();
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(217);
#endif

