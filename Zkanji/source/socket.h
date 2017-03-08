//---------------------------------------------------------------------------

#ifndef socketH
#define socketH
//---------------------------------------------------------------------------
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include "templatelist.h"

class TConnectionHandler;
typedef TTemplateList<TConnectionHandler, false> TConnectionHandlerList;

enum TConnectionState { csReceiving, csDone, csNotFound, csTerminated, csError, csNoConnection, csOutOfMemory };
typedef void(__closure *TConnectionGetProc)(unsigned int threadID, TConnectionState state, const char *data, unsigned int datalen);

class TConnectionHandler
{
private:
    static unsigned int lastid;

    struct TTempSocketData
    {
        sockaddr *addr;
        int addrlen;
        SOCKET s;
        WSAEVENT e;

        bool connected;
    };

    class TTempSocketList : public TTemplateList<TTempSocketData, true>
    {
    private:
        typedef TTemplateList<TTempSocketData, true> inherited;
    protected:
        virtual void FreeItem(TTempSocketData *item, int ix);
    public:
        virtual ~TTempSocketList();
        int Add(struct sockaddr *addr, int addrlen, SOCKET s, WSAEVENT e);
    };

    class TGetAddrInfoAsync : public TThread
    {
    private:
        bool fdone;
        int err;
        const char *address;
        const char *servname;
        addrinfo *hints;
        addrinfo **result;
    protected:
        void __fastcall Execute();
    public:
        TGetAddrInfoAsync(const char *aaddress, const char *aservname, addrinfo *ahints, addrinfo **aresult);

        __property bool done = { read = fdone };
        __property int error = { read = err };
    };

    class TConnectionThread;
    typedef void(__closure * TGetProc)(const char *data, unsigned int datalen, void* customdata);

    class TConnectionThread : public TThread
    {
    public:
        struct TGetProcData
        {
            const char *data;
            unsigned int datalen;
        };
        TGetProcData *getprocdata;

        bool fsending;
        bool freceiving;
        bool fclosing;
        bool fconnected;

        void* customdata;

        UnicodeString ferrstr;
        int ferrno;
        int lasterror;

        TConnectionHandler *owner;
        TGetProc proc;

        TTempSocketList *sockets;

        SOCKET s;
        WSAEVENT e;
        unsigned int ftimeout;

        void __fastcall doproc();

        void socket_connect(SOCKET s, int error);
        void socket_read(SOCKET s, int error);
        void socket_write(SOCKET s, int error);
        void socket_close(SOCKET s, int error);

        void connectTo(const char *address);

        void set_error(int r, bool make_exception = true);
        void finish();

        UnicodeString message;
        void __fastcall domsg()
        {
            MsgBox(message, "Message", MB_OK);
        }
        void syncmsg(UnicodeString msg)
        {
            message = msg;
            TThread::Synchronize(domsg);
        }

    protected:
        void terminate();
    public:
        __fastcall TConnectionThread(TConnectionHandler *aowner, TGetProc aproc, unsigned int atimeout, void* customdata);
        virtual __fastcall ~TConnectionThread();

        __property unsigned int timeout = { read = ftimeout };
        __property int errornumber = { read = ferrno };
    };

    class TGetterThread : public TConnectionThread
    {
    public:
        char *address;
        char *path;
        bool head;
    protected:
        void __fastcall Execute();
    public:
        __fastcall TGetterThread(TConnectionHandler *aowner, TGetProc aproc, void* customdata, bool ahead, unsigned int atimeout, const char *aaddress, const char *apath);
        virtual __fastcall ~TGetterThread();

        __property Terminated;
    };

    enum TConnectionThreadType
    {
        cttHead, cttGet
    };
    enum TConnectionThreadState
    {
        ctsNotFound, ctsUserTerminated, ctsHead, ctsReceive, ctsChunkReceive, ctsChunkEnded
    };
    struct TConnectionThreadItem
    {
        unsigned int id;

        TConnectionGetProc proc;
        TConnectionThread *thread;
        TConnectionThreadType type;
        TConnectionThreadState state;
        char *data;
        unsigned int len;
        unsigned int alloced;
        unsigned int chunklen;

        TStringList *header;
    };
    class TConnectionThreads : public TTemplateList<TConnectionThreadItem, true>
    {
    private:
        typedef TTemplateList<TConnectionThreadItem, true> inherited;
    protected:
        virtual void FreeItem(TConnectionThreadItem *item, int ix);
    public:
        virtual ~TConnectionThreads()
        {
            Clear();
        }
    };

    UnicodeString ferrstr;
    int ferrno;
    int lasterror;

    bool finited;

    unsigned int ftimeout;

    TConnectionThreads *threads;
    TFuncPtr fonterminated;

    void init();
    void close();
    void set_error(int r, bool make_exception = true);

    void __fastcall threadterminate(TObject *Sender); // Notification message of an exiting thread.
    void getheadproc(const char *data, unsigned int datalen, void* customdata); // Notification of receiving data from a gethead thread.
    void getgetproc(const char *data, unsigned int datalen, void* customdata); // Notification of receiving data from a get thread.

    // Finds out if full header is stored in the two buffers when concatenated (without really concatenating).
    // Returns position in *data after the last byte of \r\n\r\n
    int fullhead(const char *buf, const unsigned int buflen, const char *data, const unsigned int datalen);
    // Finds out if the chunk's header is stored in the two buffers when concatenated (without really concatenating)
    // Returns position in *data after the last byte of %chunksize%\r\n
    int chunkhead(const char *buf, const unsigned int buflen, const char *data, const unsigned int datalen);
    void expandthreadbuf(TConnectionThreadItem *item, const unsigned int len);
    void copythreadbuf(TConnectionThreadItem *item, const char *data, const unsigned int datalen);
    int headerfrombuf(char *data, TStringList *header);

    // These functions fill a structure with data for new thread.
    void gethead(TConnectionThreadItem *item, TConnectionGetProc proc, const char *address, const char *path);
    void getget(TConnectionThreadItem *item, TConnectionGetProc proc, const char *address, const char *path);
    void addget(TConnectionThreadItem *item, TConnectionGetProc proc, TGetterThread *getter, TConnectionThreadType type);

    TConnectionThreadItem* getthread(unsigned int hthread);

    friend class TConnectionThread;
public:
    TConnectionHandler();
    ~TConnectionHandler();

    unsigned int CreateGet(TConnectionGetProc proc, const char *address, const char *path);
    void Abort(unsigned int hthread); // Aborts selected socket thread.
    void Abort(); // Aborts all threads.
    bool Running(unsigned int hthread); // Returns whether a given thread is still active.
    UnicodeString GetHeaderString(unsigned int hthread, UnicodeString key);

    __property bool inited = { read = finited };

    __property unsigned int timeout = { read = ftimeout, write = ftimeout };

    __property TFuncPtr OnTerminated = { read = fonterminated, write = fonterminated }; // Called when all threads exited.
};

#endif
