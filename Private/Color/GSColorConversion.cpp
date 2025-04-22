// Copyright Gradientspace Corp. All Rights Reserved.
#include "Color/GSColorConversion.h"

#include <iostream>

using namespace GS;

// https://gist.github.com/rygorous/2203834

union FP32
{
    uint32_t u;
    float f;
    struct {
        uint32_t Mantissa : 23;
        uint32_t Exponent : 8;
        uint32_t Sign : 1;
    };
};

// Returns "exact" float value. Round to nearest integer for conversion.
// Done this way so that conversion error can be estimated properly.
static float float_to_srgb8_ref(float f, float scale = 255.0f)
{
    float s;

    if (!(f > 0.0f)) // also covers NaNs
        s = 0.0f;
    else if (f <= 0.0031308f)
        s = 12.92f * f;
    else if (f < 1.0f)
        s = 1.055f * pow(f, 1.0f / 2.4f) - 0.055f;
    else
        s = 1.0f;

    return s * scale;
}
static uint8_t float_to_srgb8_ref_int(float f)
{
    return (uint8_t)(float_to_srgb8_ref(f) + 0.5f); // round, then float->int
}
static float srgb8_to_float_table(uint8_t val)
{
    // There's just 256 different input valus - just use a table.
    //static float table[256];
    //static bool init;
    //if (!init) {
    //    init = true;
    //    for (int i = 0; i < 256; i++)
    //    {
    //        // Matches the conversion mandated by D3D10 spec
    //        float c = (float)i * (1.0f / 255.0f);
    //        if (c <= 0.04045f)
    //            table[i] = c / 12.92f;
    //        else
    //            table[i] = pow((c + 0.055f) / 1.055f, 2.4f);
    //    }
    //}
    //return table[val];

    // static version of table above, avoids issues w/ multiple threads accessing table 
    // after init=true but before it is fully populated (it happens!)
    static float table[256] = {
        0.0f,0.000303527f,0.000607054f,0.000910581f,0.00121411f,0.00151763f,0.00182116f,0.00212469f,0.00242822f,0.00273174f,0.00303527f,0.00334654f,0.00367651f,
        0.00402472f,0.00439144f,0.00477695f,0.00518152f,0.00560539f,0.00604883f,0.00651209f,0.00699541f,0.00749903f,0.00802319f,0.00856812f,0.00913406f,0.00972122f,
        0.0103298f,0.0109601f,0.0116122f,0.0122865f,0.012983f,0.0137021f,0.0144438f,0.0152085f,0.0159963f,0.0168074f,0.017642f,0.0185002f,0.0193824f,
        0.0202886f,0.021219f,0.0221739f,0.0231534f,0.0241576f,0.0251869f,0.0262412f,0.0273209f,0.028426f,0.0295568f,0.0307135f,0.031896f,0.0331048f,
        0.0343398f,0.0356013f,0.0368895f,0.0382044f,0.0395462f,0.0409152f,0.0423114f,0.043735f,0.0451862f,0.0466651f,0.0481718f,0.0497066f,0.0512695f,
        0.0528607f,0.0544803f,0.0561285f,0.0578054f,0.0595112f,0.0612461f,0.06301f,0.0648033f,0.066626f,0.0684782f,0.0703601f,0.0722719f,0.0742136f,
        0.0761854f,0.0781874f,0.0802198f,0.0822827f,0.0843762f,0.0865005f,0.0886556f,0.0908417f,0.093059f,0.0953075f,0.0975874f,0.0998987f,0.102242f,
        0.104616f,0.107023f,0.109462f,0.111932f,0.114435f,0.116971f,0.119538f,0.122139f,0.124772f,0.127438f,0.130137f,0.132868f,0.135633f,
        0.138432f,0.141263f,0.144129f,0.147027f,0.14996f,0.152926f,0.155926f,0.158961f,0.162029f,0.165132f,0.168269f,0.171441f,0.174647f,
        0.177888f,0.181164f,0.184475f,0.187821f,0.191202f,0.194618f,0.198069f,0.201556f,0.205079f,0.208637f,0.212231f,0.215861f,0.219526f,
        0.223228f,0.226966f,0.23074f,0.234551f,0.238398f,0.242281f,0.246201f,0.250158f,0.254152f,0.258183f,0.262251f,0.266356f,0.270498f,
        0.274677f,0.278894f,0.283149f,0.287441f,0.291771f,0.296138f,0.300544f,0.304987f,0.309469f,0.313989f,0.318547f,0.323143f,0.327778f,
        0.332452f,0.337164f,0.341914f,0.346704f,0.351533f,0.3564f,0.361307f,0.366253f,0.371238f,0.376262f,0.381326f,0.38643f,0.391573f,
        0.396755f,0.401978f,0.40724f,0.412543f,0.417885f,0.423268f,0.428691f,0.434154f,0.439657f,0.445201f,0.450786f,0.456411f,0.462077f,
        0.467784f,0.473532f,0.47932f,0.48515f,0.491021f,0.496933f,0.502887f,0.508881f,0.514918f,0.520996f,0.527115f,0.533277f,0.53948f,
        0.545725f,0.552012f,0.558341f,0.564712f,0.571125f,0.577581f,0.584079f,0.590619f,0.597202f,0.603828f,0.610496f,0.617207f,0.623961f,
        0.630757f,0.637597f,0.64448f,0.651406f,0.658375f,0.665387f,0.672443f,0.679543f,0.686685f,0.693872f,0.701102f,0.708376f,0.715694f,
        0.723055f,0.730461f,0.737911f,0.745404f,0.752942f,0.760525f,0.768151f,0.775822f,0.783538f,0.791298f,0.799103f,0.806952f,0.814847f,
        0.822786f,0.83077f,0.838799f,0.846873f,0.854993f,0.863157f,0.871367f,0.879623f,0.887923f,0.89627f,0.904661f,0.913099f,0.921582f,
        0.930111f,0.938686f,0.947307f,0.955974f,0.964687f,0.973445f,0.982251f,0.991102f,1.0f
    };
    return table[val];
}
static float srgb8_to_float_ref(uint8_t val)
{
    int i = val;

    // Matches the conversion mandated by D3D10 spec
    float c = (float)i * (1.0f / 255.0f);
    if (c <= 0.04045f)
        return c / 12.92f;
    else
        return pow((c + 0.055f) / 1.055f, 2.4f);
}

static const uint32_t fp32_to_srgb8_tab4[104] = {
    0x0073000d, 0x007a000d, 0x0080000d, 0x0087000d, 0x008d000d, 0x0094000d, 0x009a000d, 0x00a1000d,
    0x00a7001a, 0x00b4001a, 0x00c1001a, 0x00ce001a, 0x00da001a, 0x00e7001a, 0x00f4001a, 0x0101001a,
    0x010e0033, 0x01280033, 0x01410033, 0x015b0033, 0x01750033, 0x018f0033, 0x01a80033, 0x01c20033,
    0x01dc0067, 0x020f0067, 0x02430067, 0x02760067, 0x02aa0067, 0x02dd0067, 0x03110067, 0x03440067,
    0x037800ce, 0x03df00ce, 0x044600ce, 0x04ad00ce, 0x051400ce, 0x057b00c5, 0x05dd00bc, 0x063b00b5,
    0x06970158, 0x07420142, 0x07e30130, 0x087b0120, 0x090b0112, 0x09940106, 0x0a1700fc, 0x0a9500f2,
    0x0b0f01cb, 0x0bf401ae, 0x0ccb0195, 0x0d950180, 0x0e56016e, 0x0f0d015e, 0x0fbc0150, 0x10630143,
    0x11070264, 0x1238023e, 0x1357021d, 0x14660201, 0x156601e9, 0x165a01d3, 0x174401c0, 0x182401af,
    0x18fe0331, 0x1a9602fe, 0x1c1502d2, 0x1d7e02ad, 0x1ed4028d, 0x201a0270, 0x21520256, 0x227d0240,
    0x239f0443, 0x25c003fe, 0x27bf03c4, 0x29a10392, 0x2b6a0367, 0x2d1d0341, 0x2ebe031f, 0x304d0300,
    0x31d105b0, 0x34a80555, 0x37520507, 0x39d504c5, 0x3c37048b, 0x3e7c0458, 0x40a8042a, 0x42bd0401,
    0x44c20798, 0x488e071e, 0x4c1c06b6, 0x4f76065d, 0x52a50610, 0x55ac05cc, 0x5892058f, 0x5b590559,
    0x5e0c0a23, 0x631c0980, 0x67db08f6, 0x6c55087f, 0x70940818, 0x74a007bd, 0x787d076c, 0x7c330723,
};

static uint8_t float_to_srgb8_var2(float in)
{
    static const FP32 almostone = { 0x3f7fffff }; // 1-eps
    static const FP32 minval = { (127 - 13) << 23 };
    FP32 f;

    // Clamp to [2^(-13), 1-eps]; these two values map to 0 and 1, respectively.
    // The tests are carefully written so that NaNs map to 0, same as in the reference
    // implementation.
    if (!(in > minval.f)) // written this way to catch NaNs
        in = minval.f;
    if (in > almostone.f)
        in = almostone.f;

    // Do the table lookup and unpack bias, scale
    f.f = in;
    uint32_t tab = fp32_to_srgb8_tab4[(f.u - minval.u) >> 20];
    uint32_t bias = (tab >> 16) << 9;
    uint32_t scale = tab & 0xffff;

    // Grab next-highest mantissa bits and perform linear interpolation
    uint32_t t = (f.u >> 12) & 0xff;
    return (uint8_t)((bias + scale * t) >> 16);
}




Vector3f GS::SRGBToLinear(const Color3b& Colorb)
{
    return Vector3f(
        srgb8_to_float_table(Colorb.R), srgb8_to_float_table(Colorb.G), srgb8_to_float_table(Colorb.B));
}
Vector4f GS::SRGBToLinear(const Color4b& Colorb)
{
    return Vector4f(
        srgb8_to_float_table(Colorb.R), srgb8_to_float_table(Colorb.G), srgb8_to_float_table(Colorb.B), GS::Clamp((float)Colorb.A/255.0f, 0.0f, 1.0f));
}

Color3b GS::LinearToSRGB(const Vector3f& Colorf)
{
    return Color3b(
        float_to_srgb8_var2(Colorf.X), float_to_srgb8_var2(Colorf.Y), float_to_srgb8_var2(Colorf.Z));
}

Vector3f GS::LinearToSRGBfExact(const Vector3f& Colorf, float Scale)
{
    return Vector3f(
        float_to_srgb8_ref(Colorf.X, Scale), float_to_srgb8_ref(Colorf.Y, Scale), float_to_srgb8_ref(Colorf.Z, Scale));
}

Color3b GS::LinearToSRGBExact(const Vector3f& Colorf)
{
    return Color3b(
        float_to_srgb8_ref_int(Colorf.X), float_to_srgb8_ref_int(Colorf.Y), float_to_srgb8_ref_int(Colorf.Z));
}

Vector3f GS::SRGBToLinearExact(const Color3b& Colorb)
{
    return Vector3f(
        srgb8_to_float_ref(Colorb.R), srgb8_to_float_ref(Colorb.G), srgb8_to_float_ref(Colorb.B));
}



Vector3f GS::RGBtoHSV(const Vector3f& RBGColor)
{
    float r = RBGColor.X, g = RBGColor.Y, b = RBGColor.Z;
    float K = 0.0f;

    if (g < b) {
        std::swap(g, b);
        K = -1.0f;
    }

    if (r < g) {
        std::swap(r, g);
        K = -2.0f / 6.0f - K;
    }

    float chroma = r - std::min(g, b);
    float h = fabs(K + (g - b) / (6.0f * chroma + 1e-20f));
    float s = chroma / (r + 1e-20f);
    float v = r;
    return Vector3f(h * 360.0f, s, v);
}


Vector3f GS::HSVtoRGB(const Vector3f& HSVColor)
{
    float h = HSVColor.X, s = HSVColor.Y, v = HSVColor.Z;

    if (h > 360.0f)
        h -= 360.0f;
    if (h < 0.0f)
        h += 360.0f;
    h = GS::Clamp(h, 0.0f, 360.0f);
    s = GS::Clamp(s, 0.0f, 1.0f);
    v = GS::Clamp(v, 0.0f, 1.0f);
    float c = v * s;
    float hp = h / 60.0f;
    float x = c * (1.0f - fabs(fmod(hp, 2.0f) - 1.0f));
    float m = v - c;

    float rp = 0, gp = 0, bp = 0;

    int a = (int)(hp);
    switch (a) {
    case 0:
        rp = c; gp = x; bp = 0; break;
    case 1:
        rp = x; gp = c; bp = 0; break;
    case 2:
        rp = 0; gp = c; bp = x; break;
    case 3:
        rp = 0; gp = x; bp = c; break;
    case 4:
        rp = x; gp = 0; bp = c; break;
        // case 5:
    default:
        rp = c; gp = 0; bp = x; break;
    }

    return Vector3f(
        GS::Clamp(rp + m, 0.0f, 1.0f),
        GS::Clamp(gp + m, 0.0f, 1.0f),
        GS::Clamp(bp + m, 0.0f, 1.0f));
}
