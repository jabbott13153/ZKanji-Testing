//---------------------------------------------------------------------------

#ifndef angleH
#define angleH
//---------------------------------------------------------------------------

#include <Math.h>

//const double tinyval = 0.0001;
//const double smallval = 0.001;
extern const double tinyval;
extern const double smallval;

inline bool fnear(const double &a, const double &b)
{
    return fabs(a - b) < tinyval;
}
inline bool fnearw(const double &a)
{
    return fnear(a, round(a));
}

struct TDoublePoint
{
    TDoublePoint();
    TDoublePoint(const TDoublePoint &p);
    TDoublePoint(const TPoint &p);
    TDoublePoint(double xx, double yy);
    double x;
    double y;
    operator TPoint();

    inline TPoint point()
    {
        return TPoint(*this);
    }
};
TDoublePoint DoublePoint(double x, double y);
bool operator == (TDoublePoint const &a, TDoublePoint const &b);
bool operator != (TDoublePoint const &a, TDoublePoint const &b);
TDoublePoint operator -(TDoublePoint const &a, TDoublePoint const &b);

struct TDoubleRect;
TDoubleRect DoubleRect(double l, double t, double r, double b);
struct TDoubleRect
{
    TDoubleRect();
    inline TDoubleRect(const TRect &r)
    {
        Left = r.left, Top = r.Top, Right = r.Right, Bottom = r.Bottom;
    };
    TDoubleRect(const TDoubleRect &r);
    TDoubleRect(const TDoublePoint &a, const TDoublePoint &b);
    TDoubleRect(double aleft, double atop, double aright, double abottom);
    double Width() const;
    double Height() const;
    inline bool Empty() const
    {
        return fnear(Left, Right) && fnear(Top, Bottom);
    };

    TDoubleRect Union(const TDoubleRect &r, bool checkempty = true) const;
    inline TDoubleRect Offset(double x, double y) const
    {
        return TDoubleRect(Left + x, Top + y, Right + x, Bottom + y);
    };

    inline TRect rect() const
    {
        return Rect(floor(Left), floor(Top), ceil(Right), ceil(Bottom));
    }

    double Left;
    double Right;
    double Top;
    double Bottom;
};

inline TDoubleRect DoubleRect(const TRect &r)
{
    return TDoubleRect(r);
}

//radian
struct TRad
{
    TRad();
    TRad(const TRad&);
    TRad(double x);

    TRad Normalize(const TRad &r) const;
    void Normalize();
    TRad Reverse();

    bool operator < (const TRad &r) const;
    bool operator > (const TRad &r) const;
    bool operator <= (const TRad &r) const;
    bool operator >= (const TRad &r) const;
    TRad operator -(const TRad &r) const;
    TRad& operator -= (const TRad &r);

    double Sin() const;
    double Cos() const;

    double Abs() const;

    byte Horizontal() const; //result 0: non-horizontal. otherwise direction (1-2)
    byte Vertical() const; //result 0: non-vertical. otherwise direction (1-2)
    byte Diagonal() const; //result 0: non-diagonal. otherwise direction (1-4)

    double data;
};

template<typename T>
inline int sgn(T a)
{
    return a < 0 ? -1 : a > 0 ? 1 : 0;
}

inline int round(double a)
{
    return floor(a + 0.5); //a < 0 ? (int)(a-0.5) : (int)(a+0.5);
}
inline double fpart(double a)
{
    return a - floor(a); //a > 0 ? a-(int)a : 1-(int)a+a;
}
inline int intfloor(double a)
{
    return floor(a);
}
inline int intceil(double a)
{
    return ceil(a);
}

TRad Rad(const TDoublePoint &a, const TDoublePoint &b);
TRad RadAvg(const TRad &a, const TRad &b, const double &ratio);
double Len(const TDoublePoint &a, const TDoublePoint &b);

TDoublePoint zpointaverage(const TDoublePoint &a, const TDoublePoint &b);

enum TZStrokeType
{
        stNormalTip = 0, stEndPointed, stEndThin, stStartPointed, stStartThin, stStartPointedEndPointed,
        stStartPointedEndThin, stStartThinEndPointed, stStartThinEndThin, stSingleDot
};

int zbeziersteps(TDoublePoint p1, TDoublePoint c1, TDoublePoint c2, TDoublePoint p2, int bits = 1);
TRect zline(HDC handle, double width, int color, TDoublePoint p1, TDoublePoint p2, TZStrokeType starttip, TZStrokeType endtip, int bits = 1, int bit = 0);
TRect zbezier(HDC handle, double width, int color, TDoublePoint p1, TDoublePoint c1, TDoublePoint c2, TDoublePoint p2, TZStrokeType starttip, TZStrokeType endtip, int bits = 1, int bit = 0);
double zbezierlen(TDoublePoint p1, TDoublePoint c1, TDoublePoint c2, TDoublePoint p2);
TDoubleRect zbezierdimensions(TDoublePoint p1, TDoublePoint c1, TDoublePoint c2, TDoublePoint p2);
double zbezierdistance(TDoublePoint l1, TDoublePoint c1, TDoublePoint c2, TDoublePoint l2, TDoublePoint p, double &where, bool endpoints);
double zpointdistance(TDoublePoint p1, TDoublePoint p2);
double zlinedistance(TDoublePoint l1, TDoublePoint l2, TDoublePoint p, bool endpoints);
double zgenericlinedistance(TDoublePoint l1, TDoublePoint l2, TDoublePoint p);
bool zgenericlineside(TDoublePoint l1, TDoublePoint l2, TDoublePoint p);
TDoublePoint zlineintersect(TDoublePoint l1, TDoublePoint l2, TDoublePoint p);
int zlineintersect(TDoublePoint l1, TDoublePoint l2, TDoublePoint p1, TDoublePoint p2, TDoublePoint &result);
double zpointposition(TDoublePoint l1, TDoublePoint l2, TDoublePoint p, double *distance = NULL);
void zbeziersplit(TDoublePoint p1, TDoublePoint &c1, TDoublePoint &c2, TDoublePoint p2, TDoublePoint &newpoint, TDoublePoint &newc1, TDoublePoint &newc2, double where);

#endif
