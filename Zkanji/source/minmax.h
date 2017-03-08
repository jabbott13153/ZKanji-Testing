//---------------------------------------------------------------------------

#ifndef minmaxH
#define minmaxH
//---------------------------------------------------------------------------

typedef unsigned short word;
typedef unsigned char byte;

byte max(byte a, byte b);
byte min(byte a, byte b);
char max(char a, char b);
char min(char a, char b);
short max(short a, short b);
short min(short a, short b);
unsigned short max(unsigned short a, unsigned short b);
unsigned short min(unsigned short a, unsigned short b);
int max(int a, int b);
int min(int a, int b);
unsigned int max(unsigned int a, unsigned int b);
unsigned int min(unsigned int a, unsigned int b);
long max(long a, long b);
long min(long a, long b);
double max(double a, double b);
double min(double a, double b);
int max(int a, byte b);
int min(int a, byte b);
int max(byte a, int b);
int min(byte a, int b);
int max(int a, unsigned int b);
int min(int a, unsigned int b);
int max(unsigned int a, int b);
int min(unsigned int a, int b);
int max(int a, char b);
int min(int a, char b);
int max(char a, int b);
int min(char a, int b);
char max(char a, byte b);
char min(char a, byte b);
char max(byte a, char b);
char min(byte a, char b);
double max(int a, double b);
double min(int a, double b);
double max(double a, int b);
double min(double a, int b);
int max(int a, short b);
int min(int a, short b);
int max(short a, int b);
int min(short a, int b);
int max(int a, unsigned short b);
int min(int a, unsigned short b);
int max(unsigned short a, int b);
int min(unsigned short a, int b);

template<typename A, typename B>
A max(A a, B b) { return a > b ? a : b; }
template<typename A, typename B>
A min(A a, B b) { return a < b ? a : b; }
template<typename A, typename B>
B max(A a, B b) { return a > b ? a : b; }
template<typename A, typename B>
B min(A a, B b) { return a < b ? a : b; }
template<typename A>
A max(A a, A b) { return a > b ? a : b; }
template<typename A>
A min(A a, A b) { return a < b ? a : b; }

int round(double a);
double fpart(double a);

#endif
