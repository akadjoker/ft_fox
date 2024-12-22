#pragma once

class PerlinNoise
{
private:
    unsigned int seed;
    float frequency;
    float amplitude;
    int octaves;
    float persistence;

public:
    PerlinNoise(unsigned int seed = 12345,
                float frequency = 1.0f,
                float amplitude = 1.0f,
                int octaves = 6,
                float persistence = 0.5f)
        : seed(seed), frequency(frequency), amplitude(amplitude), octaves(octaves), persistence(persistence)
    {
    }

    float noise(float x, float z) const;
    float noise(float x, float y, float z) const;

    // "Brownian Motion"
    float BM(float x, float z) const;
    float BM3D(float x, float y, float z) const;
    float smoothNoise(float x, float z) const;
    int generateHeight(int x, int z) const;
};