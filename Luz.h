//
// Created by hgallegos on 24/08/2023.
//

#ifndef INC_2023_2_RAYTRACING_LUZ_H
#define INC_2023_2_RAYTRACING_LUZ_H
#include "vec3.h"

class Luz {
public:
    Luz(vec3 posc, vec3 _color):pos(posc),color(_color){}
vec3 pos, color;
};


#endif //INC_2023_2_RAYTRACING_LUZ_H
