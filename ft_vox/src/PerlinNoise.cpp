#include "PerlinNoise.hpp"
#include <cmath>

inline float Fade(float a)
{
    return (((a * 6 - 15) * a + 10) * a * a * a);
}

inline float Clamp(float value)
{
    if (value < 0.0f)
        return 0.0f;
    else if (value > 1.0f)
        return 1.0f;
    else
        return value;
}
inline float Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}
inline float InverseLerp(float a, float b, float value)
{
    if (a != b)
        return ((value - a) / (b - a));
    else
        return 0.0f;
}

inline int FastFloor(float a)
{
    int ai = (int)a;
    return (a < ai) ? ai - 1 : ai;
}



float basis[12][4] =
    {
        {1, 1, 0},
        {-1, 1, 0},
        {1, -1, 0},
        {-1, -1, 0},
        {1, 0, 1},
        {-1, 0, 1},
        {1, 0, -1},
        {-1, 0, -1},
        {0, 1, 1},
        {0, -1, 1},
        {0, 1, -1},
        {0, -1, -1},
};
// not same permutation table as Perlin's reference to avoid copyright issues;
// Perlin's table can be found at http://mrl.nyu.edu/~perlin/noise/
static unsigned char RAND_TAB[512] =
{
   23, 125, 161, 52, 103, 117, 70, 37, 247, 101, 203, 169, 124, 126, 44, 123,
   152, 238, 145, 45, 171, 114, 253, 10, 192, 136, 4, 157, 249, 30, 35, 72,
   175, 63, 77, 90, 181, 16, 96, 111, 133, 104, 75, 162, 93, 56, 66, 240,
   8, 50, 84, 229, 49, 210, 173, 239, 141, 1, 87, 18, 2, 198, 143, 57,
   225, 160, 58, 217, 168, 206, 245, 204, 199, 6, 73, 60, 20, 230, 211, 233,
   94, 200, 88, 9, 74, 155, 33, 15, 219, 130, 226, 202, 83, 236, 42, 172,
   165, 218, 55, 222, 46, 107, 98, 154, 109, 67, 196, 178, 127, 158, 13, 243,
   65, 79, 166, 248, 25, 224, 115, 80, 68, 51, 184, 128, 232, 208, 151, 122,
   26, 212, 105, 43, 179, 213, 235, 148, 146, 89, 14, 195, 28, 78, 112, 76,
   250, 47, 24, 251, 140, 108, 186, 190, 228, 170, 183, 139, 39, 188, 244, 246,
   132, 48, 119, 144, 180, 138, 134, 193, 82, 182, 120, 121, 86, 220, 209, 3,
   91, 241, 149, 85, 205, 150, 113, 216, 31, 100, 41, 164, 177, 214, 153, 231,
   38, 71, 185, 174, 97, 201, 29, 95, 7, 92, 54, 254, 191, 118, 34, 221,
   131, 11, 163, 99, 234, 81, 227, 147, 156, 176, 17, 142, 69, 12, 110, 62,
   27, 255, 0, 194, 59, 116, 242, 252, 19, 21, 187, 53, 207, 129, 64, 135,
   61, 40, 167, 237, 102, 223, 106, 159, 197, 189, 215, 137, 36, 32, 22, 5,

   // and a second copy so we don't need an extra mask or static initializer
   23, 125, 161, 52, 103, 117, 70, 37, 247, 101, 203, 169, 124, 126, 44, 123,
   152, 238, 145, 45, 171, 114, 253, 10, 192, 136, 4, 157, 249, 30, 35, 72,
   175, 63, 77, 90, 181, 16, 96, 111, 133, 104, 75, 162, 93, 56, 66, 240,
   8, 50, 84, 229, 49, 210, 173, 239, 141, 1, 87, 18, 2, 198, 143, 57,
   225, 160, 58, 217, 168, 206, 245, 204, 199, 6, 73, 60, 20, 230, 211, 233,
   94, 200, 88, 9, 74, 155, 33, 15, 219, 130, 226, 202, 83, 236, 42, 172,
   165, 218, 55, 222, 46, 107, 98, 154, 109, 67, 196, 178, 127, 158, 13, 243,
   65, 79, 166, 248, 25, 224, 115, 80, 68, 51, 184, 128, 232, 208, 151, 122,
   26, 212, 105, 43, 179, 213, 235, 148, 146, 89, 14, 195, 28, 78, 112, 76,
   250, 47, 24, 251, 140, 108, 186, 190, 228, 170, 183, 139, 39, 188, 244, 246,
   132, 48, 119, 144, 180, 138, 134, 193, 82, 182, 120, 121, 86, 220, 209, 3,
   91, 241, 149, 85, 205, 150, 113, 216, 31, 100, 41, 164, 177, 214, 153, 231,
   38, 71, 185, 174, 97, 201, 29, 95, 7, 92, 54, 254, 191, 118, 34, 221,
   131, 11, 163, 99, 234, 81, 227, 147, 156, 176, 17, 142, 69, 12, 110, 62,
   27, 255, 0, 194, 59, 116, 242, 252, 19, 21, 187, 53, 207, 129, 64, 135,
   61, 40, 167, 237, 102, 223, 106, 159, 197, 189, 215, 137, 36, 32, 22, 5,
};


static unsigned char RAND_TAB_IDX[512] =
{
    7, 9, 5, 0, 11, 1, 6, 9, 3, 9, 11, 1, 8, 10, 4, 7,
    8, 6, 1, 5, 3, 10, 9, 10, 0, 8, 4, 1, 5, 2, 7, 8,
    7, 11, 9, 10, 1, 0, 4, 7, 5, 0, 11, 6, 1, 4, 2, 8,
    8, 10, 4, 9, 9, 2, 5, 7, 9, 1, 7, 2, 2, 6, 11, 5,
    5, 4, 6, 9, 0, 1, 1, 0, 7, 6, 9, 8, 4, 10, 3, 1,
    2, 8, 8, 9, 10, 11, 5, 11, 11, 2, 6, 10, 3, 4, 2, 4,
    9, 10, 3, 2, 6, 3, 6, 10, 5, 3, 4, 10, 11, 2, 9, 11,
    1, 11, 10, 4, 9, 4, 11, 0, 4, 11, 4, 0, 0, 0, 7, 6,
    10, 4, 1, 3, 11, 5, 3, 4, 2, 9, 1, 3, 0, 1, 8, 0,
    6, 7, 8, 7, 0, 4, 6, 10, 8, 2, 3, 11, 11, 8, 0, 2,
    4, 8, 3, 0, 0, 10, 6, 1, 2, 2, 4, 5, 6, 0, 1, 3,
    11, 9, 5, 5, 9, 6, 9, 8, 3, 8, 1, 8, 9, 6, 9, 11,
    10, 7, 5, 6, 5, 9, 1, 3, 7, 0, 2, 10, 11, 2, 6, 1,
    3, 11, 7, 7, 2, 1, 7, 3, 0, 8, 1, 1, 5, 0, 6, 10,
    11, 11, 0, 2, 7, 0, 10, 8, 3, 5, 7, 1, 11, 1, 0, 7,
    9, 0, 11, 5, 10, 3, 2, 3, 5, 9, 7, 9, 8, 4, 6, 5,

    // and a second copy so we don't need an extra mask or static initializer
    7, 9, 5, 0, 11, 1, 6, 9, 3, 9, 11, 1, 8, 10, 4, 7,
    8, 6, 1, 5, 3, 10, 9, 10, 0, 8, 4, 1, 5, 2, 7, 8,
    7, 11, 9, 10, 1, 0, 4, 7, 5, 0, 11, 6, 1, 4, 2, 8,
    8, 10, 4, 9, 9, 2, 5, 7, 9, 1, 7, 2, 2, 6, 11, 5,
    5, 4, 6, 9, 0, 1, 1, 0, 7, 6, 9, 8, 4, 10, 3, 1,
    2, 8, 8, 9, 10, 11, 5, 11, 11, 2, 6, 10, 3, 4, 2, 4,
    9, 10, 3, 2, 6, 3, 6, 10, 5, 3, 4, 10, 11, 2, 9, 11,
    1, 11, 10, 4, 9, 4, 11, 0, 4, 11, 4, 0, 0, 0, 7, 6,
    10, 4, 1, 3, 11, 5, 3, 4, 2, 9, 1, 3, 0, 1, 8, 0,
    6, 7, 8, 7, 0, 4, 6, 10, 8, 2, 3, 11, 11, 8, 0, 2,
    4, 8, 3, 0, 0, 10, 6, 1, 2, 2, 4, 5, 6, 0, 1, 3,
    11, 9, 5, 5, 9, 6, 9, 8, 3, 8, 1, 8, 9, 6, 9, 11,
    10, 7, 5, 6, 5, 9, 1, 3, 7, 0, 2, 10, 11, 2, 6, 1,
    3, 11, 7, 7, 2, 1, 7, 3, 0, 8, 1, 1, 5, 0, 6, 10,
    11, 11, 0, 2, 7, 0, 10, 8, 3, 5, 7, 1, 11, 1, 0, 7,
    9, 0, 11, 5, 10, 3, 2, 3, 5, 9, 7, 9, 8, 4, 6, 5,
};

inline float Grad(int grad_idx, float x, float y, float z)
{
    float *grad = basis[grad_idx];
    return grad[0] * x + grad[1] * y + grad[2] * z;
}

float PerlinNoise::BM(float x, float z) const 
{
    float total = 0;        // Soma total do ruído
    float freq = frequency; // Começa com a frequência base
    float amp = amplitude;  // Começa com a amplitude base
    float maxValue = 0;     // Para normalização
    
    // Soma várias camadas (oitavas) de ruído
    for (int i = 0; i < octaves; i++) 
    {
        // Adiciona uma camada de ruído
        total += std::abs(noise(x * freq, z * freq) * amp);
        maxValue += amp;
        
        // Prepara para próxima oitava
        amp *= persistence;  // Reduz a amplitude (cada oitava tem menos impacto)
        freq *= 2;          // Dobra a frequência (cada oitava tem mais detalhes)
    }

    return total / maxValue; // Normaliza o resultado para [0,1]
}

int PerlinNoise::generateHeight(int x, int z) const 
{
    const float scale = 0.01f;  // Escala do terreno
    const int maxHeight = 64;   // Altura máxima

    float nx = x * scale;
    float nz = z * scale;
    
    float noiseValue = BM(nx, nz);
    return static_cast<int>(noiseValue * maxHeight);
}
float PerlinNoise::noise(float x, float z) const {
    float u, v;
    float n00, n01, n10, n11;
    float n0, n1;

    int px = FastFloor(x);
    int py = FastFloor(z);
    

    int x0 = (px & 255);          
    int x1 = ((px + 1) & 255);    
    int y0 = (py & 255);         
    int y1 = ((py + 1) & 255);    
    

    unsigned char seedByte = static_cast<unsigned char>(seed & 255);
    
    x -= px;
    u = Fade(x);
    z -= py;
    v = Fade(z);


    int r0 = RAND_TAB[(x0 + seedByte) & 511];
    int r1 = RAND_TAB[(x1 + seedByte) & 511];

    int r00 = RAND_TAB[(r0 + y0) & 511];
    int r01 = RAND_TAB[(r0 + y1) & 511];
    int r10 = RAND_TAB[(r1 + y0) & 511];
    int r11 = RAND_TAB[(r1 + y1) & 511];

    n00 = Grad(RAND_TAB_IDX[r00 & 511], x, z, 0);
    n01 = Grad(RAND_TAB_IDX[r01 & 511], x, z - 1, 0);
    n10 = Grad(RAND_TAB_IDX[r10 & 511], x - 1, z, 0);
    n11 = Grad(RAND_TAB_IDX[r11 & 511], x - 1, z - 1, 0);

    n0 = Lerp(n00, n01, v);
    n1 = Lerp(n10, n11, v);

    return Lerp(n0, n1, u);
}


float PerlinNoise::noise(float x, float y, float z) const {
    int px = FastFloor(x);
    int py = FastFloor(y);
    int pz = FastFloor(z);

    x -= px;
    y -= py;
    z -= pz;

    px &= 255;
    py &= 255;
    pz &= 255;

    unsigned char seedByte = static_cast<unsigned char>(seed & 255);

    float u = Fade(x);
    float v = Fade(y);
    float w = Fade(z);

    int r0 = RAND_TAB[(px + seedByte) & 511];
    int r1 = RAND_TAB[(px + 1 + seedByte) & 511];

    int r00 = RAND_TAB[(r0 + py) & 511];
    int r01 = RAND_TAB[(r0 + py + 1) & 511];
    int r10 = RAND_TAB[(r1 + py) & 511];
    int r11 = RAND_TAB[(r1 + py + 1) & 511];

    int r000 = RAND_TAB[(r00 + pz) & 511];
    int r001 = RAND_TAB[(r00 + pz + 1) & 511];
    int r010 = RAND_TAB[(r01 + pz) & 511];
    int r011 = RAND_TAB[(r01 + pz + 1) & 511];
    int r100 = RAND_TAB[(r10 + pz) & 511];
    int r101 = RAND_TAB[(r10 + pz + 1) & 511];
    int r110 = RAND_TAB[(r11 + pz) & 511];
    int r111 = RAND_TAB[(r11 + pz + 1) & 511];

    float n000 = Grad(RAND_TAB_IDX[r000 & 511], x, y, z);
    float n001 = Grad(RAND_TAB_IDX[r001 & 511], x, y, z - 1);
    float n010 = Grad(RAND_TAB_IDX[r010 & 511], x, y - 1, z);
    float n011 = Grad(RAND_TAB_IDX[r011 & 511], x, y - 1, z - 1);
    float n100 = Grad(RAND_TAB_IDX[r100 & 511], x - 1, y, z);
    float n101 = Grad(RAND_TAB_IDX[r101 & 511], x - 1, y, z - 1);
    float n110 = Grad(RAND_TAB_IDX[r110 & 511], x - 1, y - 1, z);
    float n111 = Grad(RAND_TAB_IDX[r111 & 511], x - 1, y - 1, z - 1);

    // Interpolação trilinear
    float n00 = Lerp(n000, n001, w);
    float n01 = Lerp(n010, n011, w);
    float n10 = Lerp(n100, n101, w);
    float n11 = Lerp(n110, n111, w);

    float n0 = Lerp(n00, n01, v);
    float n1 = Lerp(n10, n11, v);

    return Lerp(n0, n1, u);
}

float PerlinNoise::BM3D(float x, float y, float z) const
 {
    float total = 0;
    float freq = frequency;
    float amp = amplitude;
    float maxValue = 0;
    
    for (int i = 0; i < octaves; i++) {
        total += std::abs(noise(x * freq, y * freq, z * freq) * amp);
        maxValue += amp;
        amp *= persistence;
        freq *= 2;
    }
    
    return total / maxValue;
}

float PerlinNoise::smoothNoise(float x, float z) const
{
    float corners = (noise(x - 1, z - 1) + noise(x + 1, z - 1) + noise(x - 1, z + 1) + noise(x + 1, z + 1)) / 16.0f;
    float sides = (noise(x - 1, z) + noise(x + 1, z) +noise(x, z - 1) + noise(x, z + 1)) /    8.0f;
    float center = noise(x, z) / 4.0f;
    return corners + sides + center;
}