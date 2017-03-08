//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Ws2tcpip.h>
#include <Winsock2.h>
#include "socket.h"
#include "gaplist.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#define ZWSATERMINATE      1
#define ZWSAINVALIDSOCKET  2
#define ZWSACOULDNTSEND    3
#define ZWSAFAILED         4
#define ZWSACLOSE          9999
#define ZWSATIMEOUTBASE    1000

#ifdef _DEBUG
static TGlobalTest tester1(148);
#endif

extern wchar_t Version_String[];

unsigned int TConnectionHandler::lastid = 0;

TGapStringList *socketmessages = NULL;
void init_WS_error_messages()
{
    if (socketmessages)
        return;
    socketmessages = new TGapStringList();
    socketmessages->Items[WSASYSNOTREADY] = L"The underlying network subsystem is not ready for network communication.";
    socketmessages->Items[WSAVERNOTSUPPORTED] = L"The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.";
    socketmessages->Items[WSAEINPROGRESS] = L"A blocking Windows Sockets 1.1 operation is in progress.";
    socketmessages->Items[WSAEPROCLIM] = L"Limit on the number of tasks supported by the Windows Sockets implementation has been reached.";
    socketmessages->Items[WSAEFAULT] = L"The lpWSAData is not a valid pointer.";
    socketmessages->Items[WSATRY_AGAIN] = L"A temporary failure in name resolution occurred.";
    socketmessages->Items[WSAEINVAL] = L"An invalid value was provided for the ai_flags member of the hints parameter.";
    socketmessages->Items[WSANO_RECOVERY] = L"A nonrecoverable failure in name resolution occurred.";
    socketmessages->Items[WSAEAFNOSUPPORT] = L"The ai_family member of the hints parameter is not supported.";
    socketmessages->Items[WSA_NOT_ENOUGH_MEMORY] = L"A memory allocation failure occurred.";
    socketmessages->Items[WSAHOST_NOT_FOUND] = L"The name does not resolve for the supplied parameters or the nodename and servname parameters were not provided.";
    socketmessages->Items[WSATYPE_NOT_FOUND] = L"The servname parameter is not supported for the specified ai_socktype member of the hints parameter.";
    socketmessages->Items[WSAESOCKTNOSUPPORT] = L"The ai_socktype member of the hints parameter is not supported.";
    socketmessages->Items[WSANOTINITIALISED] = L"A successful WSAStartup call must occur before using this function.";
    socketmessages->Items[WSAENETDOWN] = L"The network subsystem or the associated service provider has failed.";
    socketmessages->Items[WSAEMFILE] = L"No more socket descriptors are available.";
    socketmessages->Items[WSAENOBUFS] = L"No buffer space is available. The socket cannot be created.";
    socketmessages->Items[WSAEPROTONOSUPPORT] = L"The specified protocol is not supported.";
    socketmessages->Items[WSAEPROTOTYPE] = L"The specified protocol is the wrong type for this socket.";
    socketmessages->Items[WSAEADDRINUSE] = L"The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR.";
    socketmessages->Items[WSAEINTR] = L"The blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.";
    socketmessages->Items[WSAEALREADY] = L"A nonblocking connect call is in progress on the specified socket.";
    socketmessages->Items[WSAEADDRNOTAVAIL] = L"The remote address is not a valid address (such as ADDR_ANY).";
    socketmessages->Items[WSAECONNREFUSED] = L"The attempt to connect was forcefully rejected.";
    socketmessages->Items[WSAEISCONN] = L"The socket is already connected (connection-oriented sockets only).";
    socketmessages->Items[WSAENETUNREACH] = L"The network cannot be reached from this host at this time.";
    socketmessages->Items[WSAEHOSTUNREACH] = L"A socket operation was attempted to an unreachable host.";
    socketmessages->Items[WSAENOTSOCK] = L"The descriptor is not a socket.";
    socketmessages->Items[WSAETIMEDOUT] = L"Operation timed out."; //"Attempt to connect timed out without establishing a connection.";
    socketmessages->Items[WSAEWOULDBLOCK] = L"The socket is marked as nonblocking and the connection cannot be completed immediately.";
    socketmessages->Items[WSAEACCES] = L"Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled.";
    socketmessages->Items[WSAENETRESET] = L"The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress.";
    socketmessages->Items[WSAENOTCONN] = L"The socket is not connected.";
    socketmessages->Items[WSAEOPNOTSUPP] = L"MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations.";
    socketmessages->Items[WSAESHUTDOWN] = L"The socket has been shut down; it is not possible to send on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH.";
    socketmessages->Items[WSAEMSGSIZE] = L"The socket is message oriented, and the message is larger than the maximum supported by the underlying transport.";
    socketmessages->Items[WSAECONNABORTED] = L"The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.";
    socketmessages->Items[WSAECONNRESET] = L"The virtual circuit was reset by the remote side executing a hard or abortive close. For UDP sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a \"Port Unreachable\" ICMP packet. The application should close the socket as it is no longer usable.";
    //socketmessages->Items[WSAINVALIDPROVIDER] = "The service provider returned a version other than 2.2.";
    //socketmessages->Items[WSAINVALIDPROCTABLE] = "The service provider returned an invalid or incomplete procedure table to the WSPStartup.";
}
void delete_WS_error_messages()
{
    delete socketmessages;
    socketmessages = NULL;
}

TConnectionHandler::TGetAddrInfoAsync::TGetAddrInfoAsync(const char *aaddress, const char *aservname, addrinfo *ahints, addrinfo **aresult) : address(aaddress), servname(aservname), hints(ahints), result(aresult), fdone(false), err(0), TThread(false)
{

}
//---------------------------------------------------------------------------
void __fastcall TConnectionHandler::TGetAddrInfoAsync::Execute()
{
    //---- Place thread code here ----
    err = getaddrinfo(address, servname, hints, result);
    fdone = true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TConnectionHandler::TTempSocketList::~TTempSocketList()
{
    Clear();
}
//---------------------------------------------------------------------------
int TConnectionHandler::TTempSocketList::Add(sockaddr *addr, int addrlen, SOCKET s, WSAEVENT e)
{
    TTempSocketData *data = new TTempSocketData;
    data->addr = addr;
    data->addrlen = addrlen;
    data->s = s;
    data->e = e;
    data->connected = false;
    return inherited::Add(data);
}
//---------------------------------------------------------------------------
void TConnectionHandler::TTempSocketList::FreeItem(TTempSocketData *item, int ix)
{
    if (!item->connected)
    {
        shutdown(item->s, SD_BOTH);
        closesocket(item->s);
        WSACloseEvent(item->e);
    }
    delete item;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TConnectionHandler::TConnectionThread::TConnectionThread(TConnectionHandler *aowner, TGetProc aproc, unsigned int atimeout, void* customdata) : owner(aowner), customdata(customdata), ftimeout(atimeout), proc(aproc), fsending(false), freceiving(false), fclosing(false), fconnected(false), lasterror(0), ferrno(0), ferrstr(L""), s(INVALID_SOCKET), sockets(NULL), TThread(true)
{
    FreeOnTerminate = true;
    getprocdata = new TGetProcData;
}
//---------------------------------------------------------------------------
__fastcall TConnectionHandler::TConnectionThread::~TConnectionThread()
{
    delete getprocdata;
}
//---------------------------------------------------------------------------
void __fastcall TConnectionHandler::TConnectionThread::doproc()
{
    proc(getprocdata->data, getprocdata->datalen, customdata);
}
//---------------------------------------------------------------------------
void TConnectionHandler::TConnectionThread::set_error(int r, bool make_exception)
{
    if (r == 0 || r == ZWSACLOSE)
        return;

    if (!socketmessages->Items[r])
    {
        ferrstr = L"Unknown error";
        ferrno = r;
    }
    else
    {
        ferrstr = socketmessages->Items[r];
        ferrno = r;
    }
    if (make_exception)
        THROW(ferrstr.c_str());
}
//---------------------------------------------------------------------------
void TConnectionHandler::TConnectionThread::socket_connect(SOCKET s, int error)
{
    if (!sockets)
        return;

    int ix = 0;
    for (; ix < sockets->Count; ++ix)
        if (sockets->Items[ix]->s == s)
            break;
    if (ix == sockets->Count)
        return;

    if (error)
    {
        lasterror = error;
        sockets->Delete(ix);
        return;
    }

    if (fconnected)
        return;
    fconnected = true;
    fsending = true;
    sockets->Items[ix]->connected = true;
    this->s = sockets->Items[ix]->s;
    this->e = sockets->Items[ix]->e;
}
//---------------------------------------------------------------------------
void TConnectionHandler::TConnectionThread::socket_read(SOCKET s, int error)
{
    if (this->s != s)
        return;

    if (error)
        lasterror = error;
    else
        freceiving = true;
}
//---------------------------------------------------------------------------
void TConnectionHandler::TConnectionThread::socket_write(SOCKET s, int error)
{
    if (this->s != s)
        return;

    if (error)
        lasterror = error;
    else
        fsending = true;
}
//---------------------------------------------------------------------------
void TConnectionHandler::TConnectionThread::socket_close(SOCKET s, int error)
{
    if (error)
        lasterror = error;
    else
        fclosing = true;
}
//---------------------------------------------------------------------------
void TConnectionHandler::TConnectionThread::connectTo(const char *address)
{
    DWORD starttime = GetTickCount();

    addrinfo ahints, *selected, *ares = NULL;
    memset(&ahints, 0, sizeof(ahints));

    ahints.ai_flags = AI_PASSIVE | AI_CANONNAME;
    ahints.ai_family = AF_UNSPEC;
    ahints.ai_socktype = SOCK_STREAM;
    ahints.ai_protocol = IPPROTO_TCP;
    int r;
    TGetAddrInfoAsync *getaddrinfo_async = NULL;
    try
    {
        getaddrinfo_async = new TGetAddrInfoAsync(address, "http", &ahints, &ares);
        while (!getaddrinfo_async->done && !Terminated)
        {
            Sleep(5);
            if (GetTickCount() - starttime >= timeout)
            {
                ferrno = ZWSATIMEOUTBASE + 1;
                THROW(L"Operation timed out!");
            }
        }

        if (Terminated)
        {
            ferrno = ZWSATERMINATE;
            THROW(L"Operation was terminated forcefully.");
        }
        set_error(getaddrinfo_async->error);
        delete getaddrinfo_async;
    }
    catch (...)
    {
        delete getaddrinfo_async;
        if (ares)
            freeaddrinfo(ares);
        throw;
    }

    s = INVALID_SOCKET;
    sockets = new TTempSocketList;
    SOCKET tmps;

    try
    {
        selected = ares;
        while (s == INVALID_SOCKET && selected)
        {

            tmps = socket(selected->ai_addr->sa_family, SOCK_STREAM, IPPROTO_TCP);

            if (tmps == INVALID_SOCKET)
                lasterror = WSAGetLastError();
            else
            {
                WSAEVENT tmpevent = WSACreateEvent();
                if (tmpevent == WSA_INVALID_EVENT)
                    continue;

                if (WSAEventSelect(tmps, tmpevent, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE) == SOCKET_ERROR)
                {
                    lasterror = WSAGetLastError();
                }
                else
                    sockets->Add(selected->ai_addr, selected->ai_addrlen, tmps, tmpevent);
            }
            while (selected->ai_next && selected->ai_addr == selected->ai_next->ai_addr)
                selected = selected->ai_next;

            selected = selected->ai_next;
        }
        if (!sockets->Count)
            set_error(WSAGetLastError());
    }
    catch (...)
    {
        sockets->Clear();
        delete sockets;
        freeaddrinfo(ares);
        throw;
    }

    for (int ix = 0; ix < sockets->Count; ++ix)
    {
        if (connect(sockets->Items[ix]->s, sockets->Items[ix]->addr, sockets->Items[ix]->addrlen) == SOCKET_ERROR)
        {
            r = WSAGetLastError();
            if (r != WSAEWOULDBLOCK)
            {
                shutdown(sockets->Items[ix]->s, SD_BOTH);
                closesocket(sockets->Items[ix]->s);
                sockets->Delete(ix--);
                lasterror = r;
            }
        }
        else
        {
            socket_connect(sockets->Items[ix]->s, 0);
            break;
        }
    }

    starttime = GetTickCount();

    while (!fconnected && !Terminated && sockets->Count)
    {
        for (int ix = sockets->Count - 1; ix >= 0; ix--)
        {
            WSANETWORKEVENTS we;
            if (WSAEnumNetworkEvents(sockets->Items[ix]->s, sockets->Items[ix]->e, &we) == SOCKET_ERROR)
            {
                sockets->Delete(ix);
                lasterror = WSAGetLastError();
            }
            if (we.lNetworkEvents & FD_CONNECT)
                socket_connect(sockets->Items[ix]->s, we.iErrorCode[FD_CONNECT_BIT]);
        }
        Sleep(5);

        if (GetTickCount() - starttime >= timeout)
        {
            ferrno = ZWSATIMEOUTBASE + 2;
            THROW(L"Operation timed out!");
        }
    }

    if (!sockets->Count || Terminated)
    {
        try
        {
            if (Terminated)
            {
                ferrno = ZWSATERMINATE;
                THROW(L"Operation was terminated forcefully.");
            }
            set_error(lasterror);
        }
        catch (...)
        {
            delete sockets;
            freeaddrinfo(ares);
            throw;
        }
    }

    lasterror = 0;

    if (Terminated && s != INVALID_SOCKET)
    {
        shutdown(s, SD_BOTH);
        closesocket(s);
        s = INVALID_SOCKET;
        WSACloseEvent(e);
    }

    sockets->Clear();
    delete sockets;
    sockets = NULL;
    freeaddrinfo(ares);

    if (s == INVALID_SOCKET)
    {
        ferrno = ZWSAINVALIDSOCKET;
        THROW(L"Invalid socket after connect!");
    }
}
//---------------------------------------------------------------------------
void TConnectionHandler::TConnectionThread::finish()
{
    if (s != INVALID_SOCKET)
    {
        shutdown(s, SD_BOTH);
        closesocket(s);
    }
    s = INVALID_SOCKET;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TConnectionHandler::TGetterThread::TGetterThread(TConnectionHandler *aowner, TGetProc aproc, void* customdata, bool ahead, unsigned int atimeout, const char *aaddress, const char *apath) : head(ahead), TConnectionThread(aowner, aproc, atimeout, customdata)
{
    address = new char[strlen(aaddress) + 1];
    strcpy(address, aaddress);
    path = new char[strlen(apath) + 1];
    strcpy(path, apath);
}
//---------------------------------------------------------------------------
__fastcall TConnectionHandler::TGetterThread::~TGetterThread()
{
    delete[] address;
    delete[] path;
}
//---------------------------------------------------------------------------
void __fastcall TConnectionHandler::TGetterThread::Execute()
{
    char *recbuf = NULL;
    try
    {
        connectTo(address);

        if (Terminated)
        {
            ferrno = ZWSATERMINATE;
            THROW(L"Operation was terminated forcefully.");
        }

        AnsiString agent = AnsiString("zkanji/") + Version_String;
        AnsiString sendbuf = (AnsiString)(head ? "HEAD " : "GET ") + path + " HTTP/1.1\r\n" + "From: z-one@freeblog.hu\r\n" + "User-Agent: " + agent + "\r\n" + "Host: " + address + "\r\n" + "Connection: close\r\n" + "\r\n";

        int r = 0;
        int pos = 0;
        DWORD starttime = GetTickCount();
        while (lasterror != WSAEWOULDBLOCK && pos < sendbuf.Length())
        {

            if (r && r != SOCKET_ERROR)
                starttime = GetTickCount();

            if (GetTickCount() - starttime >= timeout)
            {
                ferrno = ZWSATIMEOUTBASE + 3;
                THROW(L"Operation timed out!");
            }

            r = 0;

            if (Terminated)
            {
                ferrno = ZWSATERMINATE;
                THROW(L"Operation was terminated forcefully.");
            }

            if (lasterror || fsending && (r = send(s, sendbuf.c_str(), sendbuf.Length(), 0)) == SOCKET_ERROR)
            {
                if (r == SOCKET_ERROR)
                    lasterror = WSAGetLastError();
                if (lasterror != WSAEWOULDBLOCK)
                    set_error(lasterror);
                lasterror = 0;
            }
            else
                pos += r;

            fsending = false;

            WSANETWORKEVENTS we;
            if (WSAEnumNetworkEvents(s, e, &we) == SOCKET_ERROR)
                lasterror = WSAGetLastError();
            if (we.lNetworkEvents & FD_WRITE)
                socket_write(s, we.iErrorCode[FD_WRITE_BIT]);

            Sleep(1);
        }
        lasterror = 0;

        if (pos < sendbuf.Length())
        {
            ferrno = ZWSACOULDNTSEND;
            THROW(L"Couldn't send full request.");
        }

        if (Terminated)
        {
            ferrno = ZWSATERMINATE;
            THROW(L"Operation was terminated forcefully.");
        }

        const recbuflen = 1024 * 20;
        recbuf = new char[recbuflen + 1];
        memset(recbuf, 0, recbuflen + 1);

        starttime = GetTickCount();
        unsigned long rec = 0;
        WSABUF bufs;
        bufs.buf = recbuf;
        bufs.len = recbuflen;
        do
        {
            r = 0;
            unsigned long dummy = 0;
            if (lasterror && lasterror != WSAEWOULDBLOCK || freceiving && (r = WSARecv(s, &bufs, 1, &rec, &dummy, NULL, NULL)) == SOCKET_ERROR)
            {
                if (r == SOCKET_ERROR)
                    lasterror = WSAGetLastError();
                if (lasterror != WSAEWOULDBLOCK)
                    set_error(lasterror);
                r = 0;
            }
            else if (freceiving)
                r = rec;

            if (r)
            {
                lasterror = 0;
                getprocdata->data = recbuf;
                getprocdata->datalen = r;
                Synchronize(doproc);

                starttime = GetTickCount();
            }
            else if (GetTickCount() - starttime >= timeout)
            {
                ferrno = ZWSATIMEOUTBASE + 4;
                THROW(L"Operation timed out!");
            }

            freceiving = false;

            if (Terminated)
            {
                ferrno = ZWSATERMINATE;
                THROW(L"Operation was terminated forcefully.");
            }

            WSANETWORKEVENTS we;
            if (WSAEnumNetworkEvents(s, e, &we) == SOCKET_ERROR)
                lasterror = WSAGetLastError();
            else
            {
                if ((we.lNetworkEvents & FD_READ) || (we.lNetworkEvents & FD_CLOSE))
                    lasterror = 0;

                if (we.lNetworkEvents & FD_READ)
                    socket_read(s, we.iErrorCode[FD_READ_BIT]);
                if (we.lNetworkEvents & FD_CLOSE)
                    socket_close(s, we.iErrorCode[FD_CLOSE_BIT]);
            }

        }
        while (r || (!lasterror || lasterror == WSAEWOULDBLOCK) && (!fclosing || freceiving));

        if (Terminated)
        {
            ferrno = ZWSATERMINATE;
            THROW(L"Operation was terminated forcefully.");
        }

        if (!fclosing)
        {
            ferrno = ZWSAFAILED;
            THROW(L"Operation didn't succeed!");
        }

        if (!r)
        {
            finish();
            getprocdata->data = recbuf;
            getprocdata->datalen = 0;
            Synchronize(doproc);
            delete[] recbuf;
            recbuf = 0;
            return;
        }

        ferrno = 0;
    }
    catch (const wchar_t *c)
    {
        delete[] recbuf;
        ferrstr = c;
        if (!ferrno)
            ferrno = -1;
    }
    catch (...)
    {
        delete[] recbuf;
        ferrno = -1;
        ferrstr = L"Unknown error";
    }
    finish();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TConnectionHandler::TConnectionThreads::FreeItem(TConnectionThreadItem *item, int ix)
{
    if (item->thread)
        item->thread->Terminate();

    delete[] item->data;
    item->alloced = 0;
    delete item->header;
    delete item;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TConnectionHandlerList *connectionhandlerlist = NULL;
int connectionhandlerlistusagecnt = 0;
//---------------------------------------------------------------------------
TConnectionHandler::TConnectionHandler() : finited(false), fonterminated(NULL), ftimeout(8000)
{
    if (!connectionhandlerlist)
        connectionhandlerlist = new TConnectionHandlerList;

    init_WS_error_messages();
    threads = new TConnectionThreads;

    connectionhandlerlistusagecnt++;
    connectionhandlerlist->Add(this);
}
//---------------------------------------------------------------------------
TConnectionHandler::~TConnectionHandler()
{
    for (int ix = 0; ix < threads->Count; ++ix)
        threads->Items[ix]->thread->OnTerminate = NULL;

    close();
    delete_WS_error_messages();

    delete threads;

    for (int ix = 0; ix < connectionhandlerlist->Count; ++ix)
        if (connectionhandlerlist->Items[ix] == this)
            connectionhandlerlist->Delete(ix);
    connectionhandlerlistusagecnt--;
    if (!connectionhandlerlistusagecnt)
    {
        delete connectionhandlerlist;
        connectionhandlerlist = NULL;
    }
}
//---------------------------------------------------------------------------
void TConnectionHandler::set_error(int r, bool make_exception)
{
    if (r == 0 || r == ZWSACLOSE)
        return;

    if (!socketmessages->Items[r])
    {
        ferrstr = L"Unknown error";
        ferrno = r;
    }
    else
    {
        ferrstr = socketmessages->Items[r];
        ferrno = r;
    }
    if (make_exception)
        THROW(ferrstr.c_str());
}
//---------------------------------------------------------------------------
void TConnectionHandler::init()
{
    if (finited)
        return;
    WSADATA wdata;
#pragma warn -8084
    WORD wVersion = MAKEWORD(2, 2);
#pragma warn .8084
    int r;
    if ((r = WSAStartup(wVersion, &wdata)) == 0)
    {
        finited = true;
        return;
    }

    set_error(r);
}
//---------------------------------------------------------------------------
void __fastcall TConnectionHandler::threadterminate(TObject *Sender)
{
    int ix = 0;
    for (; ix < threads->Count; ++ix)
    {
        if (threads->Items[ix]->thread == Sender)
            break;
    }
    if (ix == threads->Count)
        return;
    TConnectionThreadItem *item = threads->Items[ix];

    if (item->state == ctsUserTerminated)
        item->proc(item->id, csTerminated, NULL, 0);
    else if (item->state == ctsNotFound)
        item->proc(item->id, csNotFound, NULL, 0);
    else if (item->thread->errornumber == WSAHOST_NOT_FOUND)
        item->proc(item->id, csNoConnection, NULL, 0);
    else if (item->thread->errornumber == WSA_NOT_ENOUGH_MEMORY)
        item->proc(item->id, csOutOfMemory, NULL, 0);
    else if (item->thread->errornumber)
        item->proc(item->id, csError, NULL, item->thread->errornumber);

    item->thread = NULL;
    threads->Delete(ix);

    if (!threads->Count && fonterminated)
        fonterminated();
}
//---------------------------------------------------------------------------
void TConnectionHandler::getgetproc(const char *data, unsigned int datalen, void* customdata)
{
    TConnectionThreadItem *item = (TConnectionThreadItem*)customdata;

    if (item->state == ctsUserTerminated)
        return;

    if (item->state == ctsHead)
    {
        int hpos = fullhead(item->data, item->len, data, datalen);
        int hlen = !hpos ? datalen : hpos;
        if (hlen)
        {
            expandthreadbuf(item, item->len + hlen);
            copythreadbuf(item, data, hlen);
        }
        if (!hpos)
            return;
        data += hpos;
        datalen -= hpos;

        int code;
        try
        {
            item->header = new TStringList;
            code = headerfrombuf(item->data, item->header);
        }
        catch (...)
        {
            delete item->header;
            item->header = NULL;
            item->thread->Terminate();
            return;
        }

        int ch = item->header->IndexOfName("Transfer-Encoding");

        delete[] item->data;
        item->data = NULL;
        item->len = 0;
        item->alloced = 0;

        if (ch >= 0 && item->header->ValueFromIndex[ch] == "chunked")
            item->state = ctsChunkReceive;
        else
        {
            item->state = ctsReceive;
            if (!datalen)
            {
                ch = item->header->IndexOfName("Content-Length");
                try
                {
                    if (ch >= 0 && item->header->ValueFromIndex[ch].ToInt() > 0)
                        return;
                }
                catch (...)
                {
                    ;
                }
            }
        }

        if (code != 200)
        {
            item->thread->Terminate();
            return;
        }
    }

    try
    {
        if (item->state == ctsReceive)
        {
            if (datalen)
                item->proc(item->id, csReceiving, data, datalen);
            else
                item->proc(item->id, csDone, NULL, 0);
        }
        else if (item->state == ctsChunkReceive)
        {
            // Get chunks.
            while (datalen)
            {
                if (!item->chunklen)
                { // Read new chunk or store remaining data in buffer if chunk size cannot be determined yet.
                    int hpos = chunkhead(item->data, item->len, data, datalen);
                    int hlen = !hpos ? datalen : hpos;
                    if (hlen)
                    {
                        expandthreadbuf(item, item->len + hlen);
                        copythreadbuf(item, data, hlen);
                    }
                    if (!hpos)
                        break;
                    item->chunklen = strtol(item->data, NULL, 16) + 2;
                    data += hpos;
                    datalen -= hpos;
                    if (item->chunklen == 2)
                    {
                        item->proc(item->id, csDone, NULL, 0);
                        item->state = ctsChunkEnded;
                        break;
                    }

                    delete[] item->data;
                    item->data = NULL;
                    item->len = 0;
                    item->alloced = 0;
                }

                int siz = min(datalen, item->chunklen - 2);
                if (siz > 0)
                {
                    item->proc(item->id, csReceiving, data, siz);
                    data += siz;
                    item->chunklen -= siz;
                    datalen -= siz;
                }
                if (item->chunklen <= 2)
                {
                    siz = min(item->chunklen, datalen);
                    data += siz;
                    item->chunklen -= siz;
                    datalen -= siz;
                }
            }
        }
    }
    catch (...)
    {
        item->thread->Terminate();
    }
}
//---------------------------------------------------------------------------
void TConnectionHandler::getheadproc(const char *data, unsigned int datalen, void* customdata)
{
    TConnectionThreadItem *item = (TConnectionThreadItem*)customdata;

    if (item->state != ctsHead) // Terminated or had an error.
        return;

    if (!datalen)
    {
        try
        {
            item->header = new TStringList;
            int code = headerfrombuf(item->data, item->header);

            TGetterThread *getter = (TGetterThread*)item->thread;

            if (code > 399 && code < 500)
            {
                item->state = ctsNotFound;
                THROW(L"HTTP resource not found");
            }

            if (code < 200 || code > 399)
                THROW(L"HTTP request failed.");

            if (code < 300)
            {
                delete item->header;
                item->header = NULL;

                delete[] item->data;
                getget(item, item->proc, getter->address, getter->path);
                item->thread->Start();
                return;
            }
            else
            {
                int ix = item->header->IndexOfName("Location");
                if (ix < 0)
                    THROW(L"Location missing from redirect.");
                AnsiString location = item->header->ValueFromIndex[ix];
                delete item->header;
                item->header = NULL;

                int lpos = 0;
                if (location[1] != '/')
                {
                    if (strncmpi("http://", location.c_str(), 7))
                        THROW(L"Bad location string in redirect.");
                    location = location.SubString(8, location.Length());
                    lpos = location.Pos("/");
                    if (!lpos)
                        THROW(L"Bad location string in redirect.");
                }

                delete[] item->data;
                gethead(item, item->proc, !lpos ? getter->address : location.SubString(1, lpos - 1).c_str(), !lpos ? location.c_str() : location.SubString(lpos, location.Length()).c_str());
                item->thread->Start();
            }

        }
        catch (...)
        {
            delete item->header;
            item->header = NULL;
            item->thread->Terminate();
            return;
        }
        return;
    }

    expandthreadbuf(item, item->len + datalen);
    copythreadbuf(item, data, datalen);
}
//---------------------------------------------------------------------------
int TConnectionHandler::fullhead(const char *buf, const unsigned int buflen, const char *data, const unsigned int datalen)
{
    const char lnln[] = "\r\n\r\n";
    for (int ix = 3; ix > 0; ix--)
    {
        if (buflen < (unsigned int)ix || datalen < (unsigned int)(4 - ix))
            continue;
        if (!strncmp(buf + buflen - ix, lnln, ix) && !strncmp(data, lnln + ix, 4 - ix))
            return 4 - ix;
    }
    if (datalen < 4)
        return 0;
    const char *rnpos = strstr(data, lnln);
    if (!rnpos)
        return 0;
    return (rnpos - data) + 4;
}
//---------------------------------------------------------------------------
int TConnectionHandler::chunkhead(const char *buf, const unsigned int buflen, const char *data, const unsigned int datalen)
{
    if (buflen >= 1 && datalen >= 1 && buf[buflen - 1] == '\r' && data[0] == '\n')
        return 1;

    if (datalen < 2)
        return 0;

    const char *rnpos = strstr(data, "\r\n");
    if (!rnpos)
        return 0;
    return (rnpos - data) + 2;
}
//---------------------------------------------------------------------------
void TConnectionHandler::expandthreadbuf(TConnectionThreadItem *item, const unsigned int len)
{
    unsigned int realloclen = item->alloced ? item->alloced : 64;
    while (len >= realloclen)
        realloclen *= 4;
    if (realloclen > item->alloced)
    {
        char *newdata = new char[realloclen];
        *newdata = 0;
        if (item->len)
        {
            memcpy(newdata, item->data, item->len + 1);
            delete[] item->data;
        }
        item->data = newdata;
        item->alloced = realloclen;
    }
}
//---------------------------------------------------------------------------
void TConnectionHandler::copythreadbuf(TConnectionThreadItem *item, const char *data, const unsigned int datalen)
{
    memcpy(item->data + item->len, data, datalen);
    item->len += datalen;
    item->data[item->len] = 0;
}
//---------------------------------------------------------------------------
int TConnectionHandler::headerfrombuf(char *data, TStringList *header)
{
    if (!data)
        THROW(L"No data in header");

    if (header)
        header->NameValueSeparator = ':';

    const char *rnpos = strstr(data, "\r\n\r\n");
    if (!rnpos)
        THROW(L"Header not fully defined.");
    rnpos += 2;

    int result = -1;

    int ix = 0;
    while (data < rnpos)
    {
        char *rn = strstr(data, "\r\n");
        if (!ix)
        {
            if (strncmpi(data, "HTTP/1.", 7) || rn - data < 15 || data[7] < '0' || data[7] > '1' || (data[8] != ' ' && data[8] != '\t') || (data[12] != ' ' && data[12] != '\t'))
                THROW(L"Not a valid HTTP header.");
            data += 9;
            data[3] = 0;

            result = atoi(data);
            if (!header)
                break;

            data = rn + 2;
            ix++;
            continue;
        }

        char *delim = strpbrk(data, ":\r");
        if (delim && delim < rn)
        {
            char *val = delim + 1;
            while (*val == ' ' || *val == '\t')
                val++;
            header->Add(AnsiString(data, delim - data + 1) + AnsiString(val, rn - val));
        }
        else
            THROW(L"Malformed HTTP header.");

        data = rn + 2;
    }

    return result;
}
//---------------------------------------------------------------------------
TConnectionHandler::TConnectionThreadItem* TConnectionHandler::getthread(unsigned int hthread)
{
    if (hthread == 0)
        return NULL;
    for (int ix = 0; ix < threads->Count; ++ix)
        if (threads->Items[ix]->id == hthread)
            return threads->Items[ix];
    return NULL;
}
//---------------------------------------------------------------------------
void TConnectionHandler::Abort(unsigned int hthread)
{
    TConnectionThreadItem *item = getthread(hthread);
    if (item == NULL)
        return;
    item->state = ctsUserTerminated;
    item->thread->Terminate();
}
//---------------------------------------------------------------------------
void TConnectionHandler::Abort()
{
    for (int ix = threads->Count - 1; ix >= 0; --ix)
    {
        if (!threads->Items[ix]->thread)
            continue;
        threads->Items[ix]->state = ctsUserTerminated;
        threads->Items[ix]->thread->Terminate();
    }
}
//---------------------------------------------------------------------------
bool TConnectionHandler::Running(unsigned int hthread)
{
    return getthread(hthread) != NULL;
}
//---------------------------------------------------------------------------
UnicodeString TConnectionHandler::GetHeaderString(unsigned int hthread, UnicodeString key)
{
    TConnectionThreadItem *item = getthread(hthread);
    if (!item->header)
        return "";
    for (int ix = 0; ix < item->header->Count; ++ix)
        if (item->header->Strings[ix].SubString(1, key.Length()).LowerCase() == key.LowerCase())
            return item->header->Strings[ix].SubString(key.Length() + 2, item->header->Strings[ix].Length());
    return "";
}
//---------------------------------------------------------------------------
unsigned int TConnectionHandler::CreateGet(TConnectionGetProc proc, const char *address, const char *path)
{
    init();
    TConnectionThreadItem *item = NULL;
    try
    {
        item = new TConnectionThreadItem;
        memset(item, 0, sizeof(TConnectionThreadItem));
        item->id = ++lastid;
        gethead(item, proc, address, path);
    }
    catch (...)
    {
        delete item;
        throw;
    }

    threads->Add(item);
    item->thread->Start();

    return item->id;
}
//---------------------------------------------------------------------------
void TConnectionHandler::gethead(TConnectionThreadItem *item, TConnectionGetProc proc, const char *address, const char *path)
{
    addget(item, proc, new TGetterThread(this, getheadproc, item, true, timeout, address, path), cttHead);
}
//---------------------------------------------------------------------------
void TConnectionHandler::getget(TConnectionThreadItem *item, TConnectionGetProc proc, const char *address, const char *path)
{
    addget(item, proc, new TGetterThread(this, getgetproc, item, false, timeout, address, path), cttGet);
}
//---------------------------------------------------------------------------
void TConnectionHandler::addget(TConnectionThreadItem *item, TConnectionGetProc proc, TGetterThread *getter, TConnectionThreadType type)
{
    getter->OnTerminate = &threadterminate;
    item->state = ctsHead;
    item->thread = getter;
    item->proc = proc;
    item->type = type;
    item->len = 0;
    item->alloced = 0;
    item->data = NULL;
}
//---------------------------------------------------------------------------
void TConnectionHandler::close()
{
    if (!finited)
        return;

    while (threads->Count)
        threads->Delete(0);

    WSACleanup();
    ferrno = 0;
    ferrstr = L"";
    lasterror = 0;
    finited = false;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(149);
#endif

