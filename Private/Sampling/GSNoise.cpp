// Copyright Gradientspace Corp. All Rights Reserved.
#include "Sampling/GSNoise.h"

using namespace GS;

// port of https://mrl.cs.nyu.edu/~perlin/noise/

namespace PerlinStuff
{

// permutation table, twice. Seems dumb to have to store it twice, couldn't we just modulo-index ?
static const int perlin_p[] = { 151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,

151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,

};

static double falloff_curve(double t) {
    return (t*t*t) * ( (6.0*t - 15.0)*t + 10.0 ); 
}

[[maybe_unused]] static double grad3(int hash, double x, double y, double z) 
{
    int h = hash & 15;              // extract low-order 4 bits
    double u = (h < 8) ? x : y;     // convert into 12 directions
    double v = (h < 4) ? y : ( (h == 12 || h == 14) ? x : z );
    return ( ((h & 1) == 0) ? u : -u) + ( ((h & 2) == 0) ? v : -v);
}

// switch version of perlin's grad3 from https://riven8192.blogspot.com/2010/08/calculate-perlinnoise-twice-as-fast.html
static double grad3v2(int hash, double x, double y, double z)
{
    switch (hash & 0xF)
    {
    case 0x0: return  x + y;
    case 0x1: return -x + y;
    case 0x2: return  x - y;
    case 0x3: return -x - y;
    case 0x4: return  x + z;
    case 0x5: return -x + z;
    case 0x6: return  x - z;
    case 0x7: return -x - z;
    case 0x8: return  y + z;
    case 0x9: return -y + z;
    case 0xA: return  y - z;
    case 0xB: return -y - z;
    case 0xC: return  y + x;
    case 0xD: return -y + z;
    case 0xE: return  y - x;
    case 0xF: return -y - z;
    default: return 0; // never happens
    }
}

Vector2d grad2(const Vector2d& p) 
{
    return Vector2d::Zero();
    //vec4 v = texture2D(iChannel0, vec2(p.x / texture_width, p.y / texture_width));
    //return normalize(v.xy * 2.0 - vec2(1.0)); /* remap sampled value to [-1; 1] and normalize */
}


static double lerp(double t, double a, double b) { return a + t*(b-a); }

}  // end namespace PerlinStuff



double GS::PerlinNoise2D(const Vector2d& Pos)
{
    using namespace PerlinStuff;
    // https://gpfault.net/posts/perlin-noise.txt.html

    // this function is not implemented yet...don't quite understand it...
    abort();

#if 0
    Vector2d p0 = Vector2d(floor(Pos.X), floor(Pos.Y));
    Vector2d p1 = p0 + Vector2d(1.0, 0.0);
    Vector2d p2 = p0 + Vector2d(0.0, 1.0);
    Vector2d p3 = p0 + Vector2d(1.0, 1.0);

    Vector2d g0 = grad2(p0);
    Vector2d g1 = grad2(p1);
    Vector2d g2 = grad2(p2);
    Vector2d g3 = grad2(p3);

    double t0 = Pos.X - p0.X;
    double fade_t0 = falloff_curve(t0); /* Used for interpolation in horizontal direction */

    double t1 = Pos.Y - p0.Y;
    double fade_t1 = falloff_curve(t1); /* Used for interpolation in vertical direction. */

    /* Calculate dot products and interpolate.*/
    // (these are lerps...)
    double p0p1 = (1.0 - fade_t0) * g0.Dot(Pos-p0) + fade_t0 * g1.Dot(Pos-p1); /* between upper two lattice points */
    double p2p3 = (1.0 - fade_t0) * g2.Dot(Pos-p2) + fade_t0 * g3.Dot(Pos-p3); /* between lower two lattice points */

    /* Calculate final result (lerp) */
    return (1.0-fade_t1) * p0p1 + fade_t1 * p2p3;
#endif
}


double GS::PerlinNoise3D(const Vector3d& Pos)
{
    using namespace PerlinStuff;
    const int* p = PerlinStuff::perlin_p;

    double x = Pos.X, y = Pos.Y, z = Pos.Z;

    // find unit cube that contains point
    int X = ((int)floor(x)) & 255;
    int Y = ((int)floor(y)) & 255;
    int Z = ((int)floor(z)) & 255;

    // make x/y/z to relative to cube
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    double u = falloff_curve(x);
    double v = falloff_curve(y);
    double w = falloff_curve(z);

    // hash coordinates of the cube corners
    int A = (p[X] + Y), AA = (p[A] + Z), AB = (p[A+1] + Z),
        B = (p[X+1] + Y), BA = (p[B] + Z), BB = (p[B+1] + Z);

    // buncha lerps...maybe re-ordering this is a good idea?
    return 
        lerp(w, 
            lerp(v, 
                lerp(u, grad3v2(p[AA], x, y, z),    grad3v2(p[BA], x-1, y, z)),
                lerp(u, grad3v2(p[AB], x, y-1, z),  grad3v2(p[BB], x-1, y-1, z))
            ),
            lerp(v, 
                lerp(u, grad3v2(p[AA+1], x, y, z-1),    grad3v2(p[BA+1], x-1, y, z-1)),
                lerp(u, grad3v2(p[AB+1], x, y-1, z-1),  grad3v2(p[BB+1], x-1, y-1, z-1))
            )
        );
}
