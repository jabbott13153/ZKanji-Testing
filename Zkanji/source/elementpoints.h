//---------------------------------------------------------------------------

#ifndef elementpointsH
#define elementpointsH
//---------------------------------------------------------------------------

#include "angle.h"

class TKanjiElement;
class Bits;
struct TRad;
struct TDoublePoint;

class TPointListEx : public TPointList
{
private:
    typedef TPointList inherited;

    using inherited::Add;
    using inherited::Delete;
    using inherited::Assign;

    double *lengths;
    TRad *rads;
    byte *sections;
    byte fsectcnt;
    TRad *sectrads;
    TRad *radsum;

    TDoubleRect frect;

    void generate_sections();

    double flen;
    double get_len();
    TRad get_rad(int ix);
    TRad get_radsum();
    TRad get_sectrad(int ix);
    double get_lens(int ix);
    double get_pos(int pt);
    int get_sect(int ix);
    int get_sectcnt();
    int get_sectsect(int ix);
    double get_sectlen(int six);
    TRect get_dim();

    void reset(bool resetrect = true);

    void backtrack(double *matrix, int m, int n, TIntegerList *points);

    struct TDoublePointPair
    {
        TDoublePoint p1;
        TDoublePoint p2;
        TDoublePoint operator[](int ix)
        {
            if (ix < 0 || ix > 1)
                throw "Point index out of range!";
            return (ix == 0 ? p1 : p2);
        }
    };

    TDoublePointPair get_pointpair(int ix);

    __property TDoublePointPair Pairs[int ix] = { read = get_pointpair };
protected:
    virtual void Clear();
public:
    TPointListEx();
    TPointListEx(TPointListEx *copy);
    virtual ~TPointListEx();

    int add(TDoublePoint *p);
    int add(TDoublePoint const & p);
    int add(double x, double y)
    {
        return add(DoublePoint(x, y));
    }
    void del(int ix);
    void clear()
    {
        reset();
        Clear();
    }

  /*splitpoints:
   *   returns cnt points evenly distributed on the original TPointListEx including first and last points
*/
    void splitpoints(TDoublePoint *pt, int cnt);
    void assign(TDoublePoint *pt, int cnt);
    void assign(TPointListEx *list);

    void split(TPointListEx *a, TPointListEx *b, double pos);

    double compare(TPointListEx *list);
    double backtrackcompare(TPointListEx *list, TIntegerList *points);

    double divisionlength(int first, int last);
    double arcsize(int first, int last);

    void draw(TCanvas *c, TRect const & r);
    void resize(double newwidth, double newheight);
    void resize(const TDoubleRect &r);
    void offset(double deltax, double deltay);

    void dissect(TDoublePoint *p, int cnt, TIntegerList *l);

    struct TIntersections
    {
        union TIntersectionPair
        {
            typedef struct
            {
                double pos1; //position of intersection in this stroke
                double pos2; //position of intersection in the other stroke
            };
            typedef struct
            {
                double position;
                double distance;
            };
        };

        double nearness; //closest distance of one stroke to the other when they do not intersect
        int count; //number of intersections up to 6 points, not counting the end and start points
        TIntersectionPair intersection[6];
        TIntersectionPair startpoint;
        TIntersectionPair endpoint;
        TIntersectionPair liststartpoint;
        TIntersectionPair listendpoint;
    };

    TIntersections intersections(TPointListEx *list, double threshold);

    __property double Length = { read = get_len }; //length of the whole curve
    __property double Lengths[int ix] = { read = get_lens }; //length of a given part between two points
    __property double PointPos[int ix] = { read = get_pos }; //returns the position of the given point between 0 and 1 inside the whole curve
    __property TRad Radian[int ix] = { read = get_rad }; //angle of a given part relative to the given 0°. 0 is facing west. north is half pi.
    __property TRad RadianSum = { read = get_radsum }; //angle of the straight line between the starting and ending points
    __property TRect Dimension = { read = get_dim }; //smallest rectangle that contains all the points
    __property int Sections[int ix] = { read = get_sect }; //section a given part is in. Section is the longest part of the curve that does not contain sharp cornesr. (See sharp corner definition in the get_sect function)
    __property int SectionCount = { read = get_sectcnt }; //number of sections
    __property double SectionLength[int six] = { read = get_sectlen }; //length of a given section
    __property double SectionRadian[int six] = { read = get_sectrad }; //average radian of a given section
    __property int SectionSections[int six] = { read = get_sectsect }; //number of line sections (point pairs) in a section
};

//simplifies stroke by removing points that lie too near to the imaginary stroke center, without influencing the shape too much
void ProcessStroke(TPointListEx *fpoints, TPointListEx *result, bool from_mouse);

//creates a TPointListEx point list from the stroke of a variant of a kanji element, in the size of the given rectangle
void GetElementStrokePoints(const TRect &r, TKanjiElement *e, int variant, int stroke, TPointListEx *pts);

//fills the bit bits depending on the relative position of the original and the compared rectangle. more details in the cpp
void ProcessDimensions(Bits &bit, TRect orig, TRect comp, const TRect &fullsize);

//creates destination stroke by only including those points of source that either mark a corner or a big turn. also adds step number of internal points between these.
//use checkarc to add even more steps in a bigger curve, because the curve direction must be measured elsewhere
void DissectStroke(TPointListEx *src, TPointListEx *dest, int steps = 3, bool checkarc = false);

/*Functions should be called in the order of:
 * 1. ProcessStroke(strokepoints,stroke,...)
 * 2. stroke->resize(10000,10000)
 * 3. DissectStroke(stroke,...)
*/

#endif
