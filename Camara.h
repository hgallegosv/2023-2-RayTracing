//
// Created by hgallegos on 30/03/2023.
//

#ifndef CG2023_CAMARA_H
#define CG2023_CAMARA_H

#include "Rayo.h"
#include "vec3.h"
#include "CImg.h"
#include "Objeto.h"
#include "Luz.h"
#include <vector>
using namespace std;
using namespace cimg_library;
typedef unsigned char BYTE;

class Camara {
    vec3 eye, xe, ye, ze;
    float f, a, b, w, h;
    CImg<BYTE> *pImg;

public:
    Camara() {}
    void configurar(float _near, float fov, int ancho, int alto,
                    vec3 pos_eye, vec3 center, vec3 up);
    void renderizar(int x);
    void renderizar2(int x);
    vec3 iluminacion(Rayo &rayo, vector<Luz*> &luces, vector<Objeto*> &objetos, int prof);

    void fresnel(vec3 &I, vec3 &N, float &ior, float &kr);
    vec3 refract(vec3 &I, vec3 &N, float &ior);
};


#endif //CG2023_CAMARA_H
