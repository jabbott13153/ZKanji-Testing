//---------------------------------------------------------------------------

#ifndef gdipbmpH
#define gdipbmpH
//---------------------------------------------------------------------------

struct TGdiSettings;

enum TGdiOptions { goAntialias, goAntialiasHi };
enum TGdiAntialias { gaAntialias = goAntialias, gaAntialiasHi = goAntialiasHi, gaNone };

struct TGdiToolSettings;
struct TGdiToolSettingsData
{
    bool used;
    TGdiToolSettingsData();
    TGdiToolSettingsData(const TGdiToolSettingsData &orig);
};

struct TGdiToolSettings
{
    TGdiToolSettingsData data;
    TGdiToolSettings() : data()
    {
        ;
    }
    TGdiToolSettings(const TGdiToolSettings &orig) : data(orig.data)
    {
        ;
    }
};

struct TGdiLineSettings;
struct TGdiLineSettingsData : public TGdiToolSettingsData
{
    int color;

    float width;
    float endwidth;

    float x_scale;
    float y_scale;

    TGdiLineSettingsData();
    TGdiLineSettingsData(int color, float width, float endwidth, float x_scale, float y_scale);
    TGdiLineSettingsData(const TGdiLineSettingsData &orig);
    TGdiLineSettingsData& operator = (const TGdiLineSettings & settings);
};

struct TGdiLineSettings : public TGdiToolSettings
{
    TGdiLineSettingsData data;
    bool usecolor;
    bool usewidth;
    bool useendwidth;
    bool usex_scale;
    bool usey_scale;

    TGdiLineSettings(bool usecolor, int color, bool usewidth, float width, bool useendwidth, float endwidth, bool usex_scale, float x_scale, bool usey_scale, float y_scale);
};

struct TGdiFillSettings;
struct TGdiFillSettingsData : public TGdiToolSettingsData
{
    int color;
    TGdiFillSettingsData();
    TGdiFillSettingsData(int color);
    TGdiFillSettingsData(const TGdiFillSettingsData &orig);
    TGdiFillSettingsData& operator = (const TGdiFillSettings & settings);
};

struct TGdiFillSettings : public TGdiToolSettings
{
    TGdiFillSettingsData data;
    bool usecolor;

    TGdiFillSettings(bool usecolor, int color);
};

struct TGdiSettings
{
    TGdiSettings *parent;
    HDC dc;
    TGdiAntialias antialias;
    TGdiLineSettingsData line;
    TGdiFillSettingsData fill;

    TGdiSettings()
    {
        reset();
    }
    TGdiSettings(HDC dc)
    {
        reset();
        this->dc = dc;
    }
    TGdiSettings(TGdiOptions op)
    {
        reset();
        *this << op;
    }

    TGdiSettings& operator << (const TGdiOptions);
    TGdiSettings& operator >> (const TGdiOptions);
    TGdiSettings& operator << (const TGdiLineSettings &);
    TGdiSettings& operator << (const TGdiFillSettings &);
private:
    void reset()
    {
        parent = NULL;
        dc = NULL;
        antialias = gaNone;
        line.color = 0;
        line.width = 1;
        line.used = false;
        fill.color = 0;
        fill.used = false;
    }
};
TGdiSettings GdiSettings(HDC dc)
{
    return TGdiSettings(dc);
}
TGdiLineSettings GdiLineWidth(float width);
TGdiLineSettings GdiLineWidth(float width, float endwidth);
TGdiLineSettings GdiLineColor(int color);
TGdiLineSettings GdiLineScale(float x_scale, float y_scale);
TGdiLineSettings GdiLine(int color, float width);
TGdiLineSettings GdiLine(int color, float width, float endwidth);
TGdiLineSettings GdiLine(int color, float width, float x_scale, float y_scale);
TGdiLineSettings GdiLine(int color, float width, float endwidth, float x_scale, float y_scale);
TGdiFillSettings GdiFill(int color);

class TGdiPlus
{
    static int instances;
    static ULONG token;

    TGdiSettings general;

    void setup(void* graphics, const TGdiSettings &settings);
    void setuppen(void* pen, const TGdiSettings &settings);
public:
    TGdiPlus();
    ~TGdiPlus();

    void Line(TDoublePoint from, TDoublePoint to, const TGdiSettings &settings);
    void Bezier(TDoublePoint from, TDoublePoint from_control, TDoublePoint to_control, TDoublePoint to, const TGdiSettings &settings);
    void Circle(TDoublePoint center, float radius, const TGdiSettings &settings);
    void Ellipse(TDoublePoint center, float horz_radius, float vert_radius, const TGdiSettings &settings);
    void Square(TDoublePoint lefttop, float size, const TGdiSettings &settings);
    void Rectangle(TDoublePoint lefttop, float width, float height, const TGdiSettings &settings);
};

#endif
