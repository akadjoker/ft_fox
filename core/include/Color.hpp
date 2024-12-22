
#pragma once
#include "Config.hpp"

enum PixelFormat
{
    GRAYSCALE = 1,     // 8 bit per pixel (no alpha)
    GRAY_ALPHA,        // 8*2 bpp (2 channels)
    R8G8B8,            // 24 bpp
    R8G8B8A8,          // 32 bpp    
};


class  Color 
{
public:
    Color() : r(255), g(255), b(255), a(255) {}

    Color(u8 r, u8 g, u8 b, u8 a=255) : r(r), g(g), b(b), a(a) {}
    
    
    
//     Color(float r, float g, float b, float a) : 
//     r((u8)(roundf(r * 255.0f))), 
//     g((u8)(roundf(g * 255.0f))), 
//     b((u8)(roundf(b * 255.0f))), 
//     a((u8)(roundf(a * 255.0f))) 
// {} 
    Color(u32 color) : r((color >> 24) & 0xFF), g((color >> 16) & 0xFF), b((color >> 8) & 0xFF), a(color & 0xFF) {}

    Color Lerp(const Color &rhs, float t) const;


    u32 ToUInt() const;

    void Set(u8 r, u8 g, u8 b, u8 a);

    static float  getRed(u32 value){return (float)((value >> 16) & 0xFF) / 255.0f;}
    static	float getGreen(u32 value)    {    return (float)((value >> 8) & 0xFF) / 255.0f;    }
    static float  getBlue(u32 value)    {    return (float)((value >> 0) & 0xFF) / 255.0f;    }
    static float  getAlpha(u32 value)    {    return (float)((value >> 24) & 0xFF) / 255.0f;    }


    u8 r, g, b, a;


	
	static const Color WHITE;
	
	static const Color GRAY;
	
	static const Color BLACK;
	
	static const Color RED;
	
	static const Color GREEN;
	
	static const Color BLUE;
	
	static const Color CYAN;
	
	static const Color MAGENTA;
	
	static const Color YELLOW;
	
	static const Color BLANK;
};