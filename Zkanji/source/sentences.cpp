//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Zlib.hpp>
#include <ZlibConst.hpp>

#include "sentences.h"
#include "lineobj.h"
#include "collection.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester1(140);
#endif

extern int exampleheadersize;

TTemplateList<TLineObj, false> TExampleSentencer::controls;
TExampleSentencer *Examples = NULL;

__fastcall TExampleSentencer::TExampleSentencer(UnicodeString filename/*, int version*/) : fname(filename)/*, version(version)*/, fmemsize(1024*1024), fmemused(0), list(NULL), f(NULL), blocks(NULL), oldest(NULL)
{
    f = _wfopen(fname.c_str(), L"rb");

    blockcnt = MAX_BLOCK_COUNT;

    try
    {
        fseek(f, exampleheadersize, SEEK_SET);
        fread(&fcount, sizeof(int), 1, f);

        blocks = new TExampleBlockData[blockcnt];

        int cnt = blockcnt - 1;
        for (int i = 0; i < blockcnt; i++)
        {
            fread(&blocks[i].position, sizeof(int), 1, f);
            fread(&blocks[i].unpackedsize, sizeof(int), 1, f);
            if (i)
            {
                if (!blocks[i].position)
                {
                    if (cnt == blockcnt - 1)
                        cnt = i - 1;
                }
                else
                    blocks[i - 1].packedsize = blocks[i].position - blocks[i - 1].position;
            }
            blocks[i].loaded = false;
        }

        fread(&blocks[cnt].packedsize, sizeof(int), 1, f);
        blocks[cnt].packedsize -= blocks[cnt].position;
    }
    catch (...)
    {
        THROW(L"An error occurred while reading example data!");
    }
}
//---------------------------------------------------------------------------
__fastcall TExampleSentencer::~TExampleSentencer()
{
    while (list)
        delete_block(list);
    if (f)
        fclose(f);
    if (blocks)
        delete[] blocks;
}
//---------------------------------------------------------------------------
void TExampleSentencer::clear_cache()
{
    for (int ix = 0; ix < controls.Count; ++ix)
        controls[ix]->set_data(dictionary->Items[0], NULL);

    while (list)
        delete_block(list);
    list = NULL;
    oldest = NULL;
    for (int ix = 0; ix != blockcnt; ++ix)
        blocks[ix].loaded = false;
}
//---------------------------------------------------------------------------
void TExampleSentencer::RegisterLineObj(TLineObj *obj)
{
    if (controls.IndexOf(obj) == -1)
        controls.Add(obj);
}
//---------------------------------------------------------------------------
void TExampleSentencer::UnregisterLineObj(TLineObj *obj)
{
    controls.Remove(obj);
}
//---------------------------------------------------------------------------
TExampleSentence* __fastcall TExampleSentencer::get_sentence(word ablock, byte aline)
{
    TSentenceList *l = get_block(ablock);
    if (!l) // An error occurred!
        THROW(L"Error! Can't load example data!");
    return l->sentence + aline;
}
//---------------------------------------------------------------------------
void __fastcall TExampleSentencer::set_memsize(int val)
{
    while (fmemused > val)
        delete_block(oldest);
    fmemsize = val;
}
//---------------------------------------------------------------------------
int ZDecompressCheck(int code)
{
    int res = code;

    if (code < 0)
        throw EZDecompressionError(_z_errmsg[2 - code]);

    return res;
}

int inflateInit(TZStreamRec &stream)
{
    return inflateInit_(stream, ZLIB_VERSION, sizeof(TZStreamRec));
}

void DecompressToUserBuf(char *InBuf, int InBytes, char *OutBuf, int BufSize)
{
    TZStreamRec strm;
    memset(&strm, 0, sizeof(strm));

    strm.next_in = InBuf;
    strm.avail_in = InBytes;
    strm.next_out = OutBuf;
    strm.avail_out = BufSize;

    ZDecompressCheck(inflateInit(strm));
    try
    {
        if (ZDecompressCheck(inflate(strm, Z_NO_FLUSH)) != Z_STREAM_END)
            throw EZLibError(Zlibconst_sTargetBufferTooSmall);
    }
    catch (Exception &e)
    {
        ZDecompressCheck(inflateEnd(strm));
        throw e;
    }
    ZDecompressCheck(inflateEnd(strm));
}

TSentenceList* __fastcall TExampleSentencer::get_block(word ablock)
{
    TSentenceList *l = list;

    if (blocks[ablock].loaded) // If a block is loaded, remove it from the middle of the loaded blocks' list, and put it to the front.
    {
        while (l && l->block != ablock)
            l = l->next;

        if (l != list)
        {
            if (l->next)
                l->next->prev = l->prev;

            l->prev->next = l->next;
            if (l == oldest)
                oldest = l->prev;

            list->prev = l;
            l->next = list;
            list = l;
            l->prev = NULL;
        }
    }
    else
    {
        while (oldest && fmemused + blocks[ablock].unpackedsize > fmemsize)
            delete_block(oldest);

        fseek(f, exampleheadersize + 4 + blockcnt * 8 + 4 + blocks[ablock].position, SEEK_SET);
        char *c = new char[blocks[ablock].packedsize];
        fread(c, blocks[ablock].packedsize, 1, f);
        char *buff = new char[blocks[ablock].unpackedsize];
        try
        {
            DecompressToUserBuf(c, blocks[ablock].packedsize, buff, blocks[ablock].unpackedsize);
        }
        catch (...)
        {
            delete[] buff;
            buff = 0;
        }
        delete[] c;

        if (buff)
        {
            l = new TSentenceList;
            memset(l->sentence, 0, sizeof(TExampleSentence)*100);
            l->block = ablock;
            l->next = list;
            list = l;
            l->prev = NULL;
            if (!l->next)
                oldest = l;
            else
                l->next->prev = l;
            for (int i = 0; i < 100; i++)
            {
                int pos = *((word*)buff + (i % 100));
                if (i && !pos)
                    break;
                pos += 200;

                int cnt = l->sentence[i].wordscnt = *(buff + pos);
                pos++;
                l->sentence[i].words = new TExampleWordData[cnt];

                byte klen;
                wchar_t *kanji;
                wchar_t *kana;
                word saved;

                for (int ix = 0; ix < cnt; ++ix)
                {
                    TExampleWordData *data = l->sentence[i].words + ix;
                    data->position = *(word*)(buff + pos);
                    pos += sizeof(word);
                    data->length = *(byte*)(buff + pos);
                    pos += sizeof(byte);

                    klen = *(word*)(buff + pos);
                    pos += sizeof(word);
                    kanji = (wchar_t*)(buff + pos);
                    pos += klen *sizeof(wchar_t);

                    klen = *(word*)(buff + pos);
                    buff[pos] = 0;
                    buff[pos + 1] = 0;
                    pos += sizeof(word);
                    kana = (wchar_t*)(buff + pos);
                    pos += klen *sizeof(wchar_t);

                    saved = *(word*)(buff + pos);
                    buff[pos] = 0;
                    buff[pos + 1] = 0;

                    data->data = store->FindWord(kanji, kana);
                    *(word*)(buff + pos) = saved;

                    /*
                     memcpy(buff+buffpos, &data[ix].position, sizeof(word));
                     buffpos += sizeof(word);
                     memcpy(buff+buffpos, &data[ix].length, sizeof(byte));
                     buffpos += sizeof(byte);

                     byte b = wcslen(data[ix].data->kanji);
                     memcpy(buff+buffpos, &b, sizeof(b));
                     buffpos += sizeof(b);
                     memcpy(buff+buffpos, data[ix].data->kanji, sizeof(wchar_t)*b);
                     buffpos += sizeof(wchar_t)*b;

                     b = wcslen(data[ix].data->kana);
                     memcpy(buff+buffpos, &b, sizeof(b));
                     buffpos += sizeof(b);
                     memcpy(buff+buffpos, data[ix].data->kana, sizeof(wchar_t)*b);
                     buffpos += sizeof(wchar_t)*b;
                     */
                }

                //memcpy(l->sentence[i].words,buff+pos,sizeof(TExampleWordData)*cnt);
                //pos += sizeof(TExampleWordData)*cnt;

                //if (version >= 2) {
                l->sentence[i].translation = new wchar_t[*((word*)(buff + pos +sizeof(word))) + 1];
                l->sentence[i].translation[*((word*)(buff + pos +sizeof(word)))] = 0;
                MultiByteToWideChar(CP_UTF8, 0, buff + pos +sizeof(word)*2, *((word*)(buff + pos)), l->sentence[i].translation, *((word*)(buff + pos +sizeof(word))));
                pos += sizeof(word)*2 + sizeof(char) * *((word*)(buff + pos));
                /*} else {
                 l->sentence[i].translation = new wchar_t[*((word*)(buff+pos))+1];
                 wcscpy(l->sentence[i].translation, UnicodeString(buff+pos+sizeof(word),*((word*)(buff+pos))).c_str() );
                 pos += sizeof(word)+sizeof(char) * *((word*)(buff+pos));
                 }*/

                l->sentence[i].japanese = new wchar_t[*((word*)(buff + pos)) + 1];
                wcsncpy(l->sentence[i].japanese, (wchar_t*)(buff + pos +sizeof(word)), *((word*)(buff + pos)));
                l->sentence[i].japanese[*((word*)(buff + pos))] = NULL;
                //pos += sizeof(word)+sizeof(wchar_t)**((word*)(buff+pos));
            }
            delete[] buff;
            fmemused += blocks[ablock].unpackedsize;
            blocks[ablock].loaded = true;
        }
        else
            return NULL;
    }

    return l;
}
//---------------------------------------------------------------------------
void __fastcall TExampleSentencer::delete_block(TSentenceList *l)
{
    if (l == oldest)
        oldest = l->prev;
    else
        l->next->prev = l->prev;

    if (l != list)
        l->prev->next = l->next;
    else
        list = l->next;

    for (int i = 0; i < 100 && l->sentence[i].words; i++)
    {
        delete[] l->sentence[i].words;
        delete[] l->sentence[i].japanese;
        delete[] l->sentence[i].translation;
    }

    fmemused -= blocks[l->block].unpackedsize;
    blocks[l->block].loaded = false;

    delete l;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(141);
#endif
