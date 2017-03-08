//---------------------------------------------------------------------------


#pragma hdrstop

#include "minmax.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
class TIntegerList;
struct TGlobalTest
{
    static TIntegerList *numbers;

    int num;
    TGlobalTest(int num);
    ~TGlobalTest();
};
static TGlobalTest tester1(102);
#endif
//---------------------------------------------------------------------------
byte max(byte a, byte b) { return (a > b ? a : b); }
byte min(byte a, byte b) { return (a < b ? a : b); }
char max(char a, char b) { return (a > b ? a : b); }
char min(char a, char b) { return (a < b ? a : b); }
short max(short a, short b) { return (a > b ? a : b); }
short min(short a, short b) { return (a < b ? a : b); }
unsigned short max(unsigned short a, unsigned short b) { return (a > b ? a : b); }
unsigned short min(unsigned short a, unsigned short b) { return (a < b ? a : b); }
int max(int a, int b) { return (a > b ? a : b); }
int min(int a, int b) { return (a < b ? a : b); }
unsigned int max(unsigned int a, unsigned int b) { return (a > b ? a : b); }
unsigned int min(unsigned int a, unsigned int b) { return (a < b ? a : b); }
long max(long a, long b) { return (a > b ? a : b); }
long min(long a, long b) { return (a < b ? a : b); }
double max(double a, double b) { return (a > b ? a : b); }
double min(double a, double b) { return (a < b ? a : b); }
int max(int a, byte b) { return (a > (int)b ? a : b); }
int min(int a, byte b) { return (a < (int)b ? a : b); }
int max(byte a, int b) { return ((int)a > b ? a : b); }
int min(byte a, int b) { return ((int)a < b ? a : b); }
int max(int a, unsigned int b) { return (a > (int)b ? a : b); }
int min(int a, unsigned int b) { return (a < (int)b ? a : b); }
int max(unsigned int a, int b) { return ((int)a > b ? a : b); }
int min(unsigned int a, int b) { return ((int)a < b ? a : b); }
int max(int a, char b) { return (a > b ? a : b); }
int min(int a, char b) { return (a < b ? a : b); }
int max(char a, int b) { return (a > b ? a : b); }
int min(char a, int b) { return (a < b ? a : b); }
char max(char a, byte b) { return (a > b ? a : b); }
char min(char a, byte b) { return (a < b ? a : b); }
char max(byte a, char b) { return (a > b ? a : b); }
char min(byte a, char b) { return (a < b ? a : b); }
double max(int a, double b) { return (a > b ? a : b); }
double min(int a, double b) { return (a < b ? a : b); }
double max(double a, int b) { return (a > b ? a : b); }
double min(double a, int b) { return (a < b ? a : b); }
int max(int a, short b) { return (a > b ? a : b); }
int min(int a, short b) { return (a < b ? a : b); }
int max(short a, int b) { return (a > b ? a : b); }
int min(short a, int b) { return (a < b ? a : b); }
int max(int a, unsigned short b) { return (a > (int)b ? a : b); }
int min(int a, unsigned short b) { return (a < (int)b ? a : b); }
int max(unsigned short a, int b) { return ((int)a > b ? a : b); }
int min(unsigned short a, int b) { return ((int)a < b ? a : b); }
int round(double a) { return a < 0 ? (int)(a-0.5) : (int)(a+0.5); }
double fpart(double a) { return a > 0 ? a-(int)a : 1-(int)a+a; }


#ifdef _DEBUG
static TGlobalTest tester2(103);
#endif

