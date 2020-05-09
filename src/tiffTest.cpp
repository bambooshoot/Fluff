#include <iostream>
#include <ArTexture.h>

template<class T>
void displayLine(T& texObj, cfloat u, cfloat v)
{
    if constexpr (texture_data_type<T>::value == 0)
        printf("%f ", texObj(u, v));
    else if constexpr (texture_data_type<T>::value == 1) {
        AtVector vec = texObj(u, v);
        printf("%f %f %f ", vec.x, vec.y, vec.z);
    }
    else {
        AtRGB rgb = texObj(u, v);
        printf("%f %f %f ", rgb.r, rgb.g, rgb.b);
    }
        
}

template<class T>
void printData(const char * texName, T & texObj)
{
    printf(texName);
    printf("\n");
    float u, v = 0, uStep = 0.1, vStep = 0.1;
    while (v < 1) {
        u = 0;
        while (u < 1) {
            displayLine<T>(texObj, u, v);
            u += uStep;
        }
        printf("\n");
        v += vStep;
    }
}

int main()
{
    AtString texFile("D:/asunlab/github/Fluff/test/layers.exr");
    ArMultiExr tex(texFile);

    TextureDensity tden(&tex);
    TextureSplit tClmp(&tex);
    TextureColor tClr(&tex);

    printData<TextureDensity>("density texture", tden);
    printData<TextureSplit>("split texture", tClmp);
    printData<TextureColor>("color texture", tClr);

    return 0;
}