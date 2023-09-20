//
// Created by hgallegos on 30/03/2023.
//

#ifndef CG2023_OBJETO_H
#define CG2023_OBJETO_H
#include "vec3.h"
#include "Rayo.h"

class Objeto {
public:
    vec3 color;
    float kd, ks, n;
    float ke, ior;
    bool transparente;
    Objeto(vec3 col):color{col}{}
    void setConstantes(float _kd, float _ks, float _n, float _ke=0, bool trans=false, float _ior=1) {
        kd=_kd; ks=_ks; n=_n; ke=_ke;
        transparente = trans;
        ior = _ior;
    }
    virtual bool intersectar(Rayo ray, float &t, vec3 &Pi, vec3 &N)=0;
};

class Esfera : public Objeto {
public:
    vec3 centro;
    float radio;

    Esfera(vec3 cen, float r, vec3 col):
    centro{cen}, radio{r}, Objeto(col) {}
    bool intersectar(Rayo ray, float &t, vec3 &Pi, vec3 &N) {
        vec3 d = ray.dir;
        vec3 o = ray.ori;
        float a = d.punto(d);
        float b = 2 * d.punto(o - centro);
        float c = (o - centro).punto(o - centro) - radio*radio;
        float D = b*b - 4*a*c;
        if (D <= 0) { return false; }
        float t1 = (-b + sqrt(D)) / (2*a);
        float t2 = (-b - sqrt(D)) / (2*a);
        t = std::min(t1, t2);
        if (t <= 0) { return false; }
        Pi = o + t*d;
        N = Pi - centro;
        N.normalize();
        return true;
    }
};
class Plano : public Objeto {
public:
    vec3 punto, normal;
    Plano(vec3 _punto, vec3 _normal, vec3 col):punto{_punto}, normal{_normal}, Objeto(col) { normal.normalize(); }
    bool intersectar(Rayo ray, float &t, vec3 &Pi, vec3 &N) {
        float den = (ray.dir).punto(normal);
        if (den == 0) {
            return false;
        }
        t = (punto - ray.ori).punto(normal) / den;
        if (t <= 0){
            return false;
        }
        N = normal;
        Pi = ray.ori + t * ray.dir;
        return true;
    }
};


#endif //CG2023_OBJETO_H
