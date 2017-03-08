//---------------------------------------------------------------------------

#ifndef taskbarH
#define taskbarH
//---------------------------------------------------------------------------

class TTaskbar
{
private:
    UnicodeString id;

    struct TTask
    {
        UnicodeString title;
        UnicodeString path;
        UnicodeString params;
        UnicodeString description;
        UnicodeString iconpath;
        int iconindex;
    };
    typedef TTemplateList<TTask, true> TTaskList;
    TTaskList *tasks;

    void InitList(ICustomDestinationList* &cdl);
public:
    TTaskbar(UnicodeString appID);
    ~TTaskbar();
    void ClearTasks();
    void AddTask(UnicodeString title, UnicodeString path, UnicodeString params, UnicodeString description, int iconindex = 0, UnicodeString iconpath = L"");
    void AddSeparator();
    void Commit();
};

#endif
