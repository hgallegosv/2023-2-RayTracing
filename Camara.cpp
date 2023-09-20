//
// Created by hgallegos on 30/03/2023.
//


#include <iostream>
#include <cmath>
#include "Camara.h"

using namespace std;
void Camara::configurar(float _near, float fov, int ancho, int alto,
                vec3 pos_eye, vec3 center, vec3 up) {
    f = _near;
    w = ancho;
    h = alto;
    a = 2 * f * tan(fov * M_PI/360);
    b = w / h * a;
    eye = pos_eye;
    ze = eye - center;
    ze.normalize();
    xe = up.cruz(ze);
    xe.normalize();
    ye = ze.cruz(xe);
}
void Camara::renderizar(int x) {
    Rayo rayo;
    rayo.ori = eye;
    vec3 dir;

    pImg = new CImg<BYTE>(w, h, 1, 3);
    CImgDisplay dis_img((*pImg), "Imagen RayTracing en Perspectiva desde una Camara Pinhole");
    Luz luz(vec3(0,10,10),  vec3(1,1,1));
    Esfera esf(vec3(2,0,0), 8, vec3(0,0,1));
    esf.setConstantes(0.8, 0.2, 32);
    vec3 color, Pi, N;
    float t;
    for(int x=0;  x < w; x++) {
        for (int y = 0; y < h; y++) {
            dir = ze * (-f) + ye * a * (y / h - 0.5) + xe * b * (x / w - 0.5);
            dir.normalize();
            rayo.dir = dir;
            color.set(0,0,0);
            if ( esf.intersectar(rayo, t, Pi, N)) {
                // color = esf.color;
                vec3 ambiente = vec3(1,1,1) * 0.2;
                vec3 L = luz.pos - Pi;
                L.normalize();
                float difuso = L.punto(N);
                float color_difuso = esf.kd * max(0.0f, difuso);
                vec3 R = 2 * difuso * N - L;
                vec3 V = -dir;
                float color_especular = esf.ks * pow( max(0.0f, R.punto(V)), esf.n);
                color = esf.color * (ambiente + luz.color*(color_difuso + color_especular));
                color.max_to_one();
            }
            (*pImg)(x,h-1-y,0) = (BYTE)(color.x * 255);
            (*pImg)(x,h-1-y,1) = (BYTE)(color.y * 255);
            (*pImg)(x,h-1-y,2) = (BYTE)(color.z * 255);
        }
    }
    dis_img.render((*pImg));
    dis_img.paint();
    string nombre_archivo = "imagen" + to_string(x) + ".bmp";
    pImg->save(nombre_archivo.c_str());
    while (!dis_img.is_closed()) {
        dis_img.wait();
    }

}
void Camara::renderizar2(int x) {
    Rayo rayo;
    rayo.ori = eye;
    vec3 dir;

    pImg = new CImg<BYTE>(w, h, 1, 3);
    CImgDisplay dis_img((*pImg), "Imagen RayTracing en Perspectiva desde una Camara Pinhole");
    Luz luz(vec3(0,10,10),  vec3(1,1,1));
    vector<Luz*> luces;
    //luces.emplace_back(&luz);

    vec3 posLuz1(-10,10,-10); vec3 colorLuz1(1,1,0);
    Esfera esfLuz(posLuz1, 2,colorLuz1);
    esfLuz.setConstantes(0.8, 0.2, 32);
    esfLuz.luz = true;
    Luz luz1(posLuz1, colorLuz1);
    luces.emplace_back(&luz1);

    Esfera esf(vec3(2,0,0), 8, vec3(0,0,1));
    esf.setConstantes(0.8, 0.2, 32, 0.9);
    Esfera esf2(vec3(-10,0,10), 6, vec3(1,0,0));
    esf2.setConstantes(0.8, 0.2, 32, 0, true, 1.5);
    Esfera esf3(vec3(-30,0,-50), 10, vec3(1,1,0));
    esf3.setConstantes(0.8, 0.2, 32, 0, true, 1.5);

    Plano plano(vec3(0,-10,0), vec3(0,1,0), vec3(0,1,1));
    plano.setConstantes(0.3,0.1,8,0.4);
    vector<Objeto*> objetos;
    objetos.emplace_back(&esf);
    objetos.emplace_back(&esf2);
    objetos.emplace_back(&esf3);
    objetos.emplace_back(&esfLuz);
    objetos.emplace_back(&plano);
    vec3 color;
    for(int x=0;  x < w; x++) {
        for (int y = 0; y < h; y++) {
            if (x==246 and y==h-390) {
                cout << "aqui";
            }
            dir = ze * (-f) + ye * a * (y / h - 0.5) + xe * b * (x / w - 0.5);
            dir.normalize();
            rayo.dir = dir;
            //color.set(0,0,0);
            color = iluminacion(rayo, luces, objetos, 1);
            (*pImg)(x,h-1-y,0) = (BYTE)(color.x * 255);
            (*pImg)(x,h-1-y,1) = (BYTE)(color.y * 255);
            (*pImg)(x,h-1-y,2) = (BYTE)(color.z * 255);
        }
    }
    //dis_img.render((*pImg));
    //dis_img.paint();
    string nombre_archivo = "imagen" + to_string(x) + ".bmp";
    pImg->save(nombre_archivo.c_str());
    /*while (!dis_img.is_closed()) {
        dis_img.wait();
    }*/

}
vec3 Camara::iluminacion(Rayo &rayo, vector<Luz*> &luces, vector<Objeto*> &objetos, int prof) {
    float t= 1000000, t_tmp;
    vec3 Pi, Pi_tmp, N, N_tmp, color(0,0,0);
    bool interseccion = false;
    Objeto *objeto;
    for (auto &pObj : objetos){
        if (pObj->intersectar(rayo, t_tmp, Pi_tmp, N_tmp) and t_tmp < t){
            interseccion = true;
            t = t_tmp;
            Pi = Pi_tmp;
            N = N_tmp;
            objeto = pObj;
        }
    }
    if (interseccion and objeto->luz){
        return objeto->color;
    }

    if ( interseccion ) {
        Luz luz = *(luces[0]);
        // color = esf.color;
        Pi = Pi + 0.00005*N;
        vec3 ambiente = vec3(1,1,1) * 0.2;
        float color_difuso = 0, color_especular = 0;
        vec3 V = -rayo.dir;
        vec3 color_reflexion(0,0,0);
        // refraccion
        vec3 refractionColor(0,0,0);
        if (objeto->transparente and prof + 1 <= 7) {
            // compute fresnel
            float kr;
            fresnel(rayo.dir, N, objeto->ior, kr);
            bool outside = rayo.dir.punto(N) < 0;
            vec3 bias = 0.005 * N;
            // compute refraction if it is not a case of total internal reflection
            if (kr < 1) {
                vec3 refractionDirection = refract(rayo.dir, N, objeto->ior);
                refractionDirection.normalize();
                vec3 refractionRayOrig = outside ? Pi - bias : Pi + bias;
                Rayo rayo_refraccion(refractionRayOrig, refractionDirection);
                refractionColor = iluminacion(rayo_refraccion, luces, objetos, prof + 1);
            }
            vec3 reflectionDirection = 2*V.punto(N)*N - V; //reflect(rayo.dir, N);
            reflectionDirection.normalize();
            vec3 reflectionRayOrig = outside ? Pi + bias : Pi - bias;
            Rayo rayo_reflexion(reflectionRayOrig,reflectionDirection);
            vec3 reflectionColor = iluminacion(rayo_reflexion,  luces, objetos, prof + 1);

            // mix the two
            refractionColor = reflectionColor * kr + refractionColor * (1 - kr);
        } else {
            vec3 L = luz.pos - Pi;
            float L_dist = L.modulo();
            // sombra
            L.normalize();
            Rayo rayo_sombra(Pi, L);
            bool en_sombra = false;
            for (auto &pObj: objetos) {
                if (not pObj->luz and pObj->intersectar(rayo_sombra, t_tmp, Pi_tmp, N_tmp)){
                    if ((Pi_tmp - Pi).modulo() < L_dist) {
                        en_sombra = true;
                        break;
                    }
                }
            }
            if (not en_sombra){
                float difuso = L.punto(N);
                color_difuso = objeto->kd * max(0.0f, difuso);
                // iluminacion especular
                vec3 R = 2 * difuso * N - L;
                color_especular = objeto->ks * pow( max(0.0f, R.punto(V)), objeto->n);
            }
            if (objeto->ke > 0 and prof + 1 <= 7) {        // reflexion
                vec3 dir_ref = 2 * V.punto(N) * N - V;
                dir_ref.normalize();
                Rayo rayo_reflejado(Pi + 0.005 * N, dir_ref);
                color_reflexion = objeto->ke * iluminacion(rayo_reflejado, luces, objetos, prof + 1);
            }
        }
        color = objeto->color * (ambiente + luz.color*(color_difuso + color_especular)) + color_reflexion + refractionColor;
        color.max_to_one();
    }
    return color;
}
void Camara::fresnel(vec3 &I, vec3 &N, float &ior, float &kr) {
    float cosi = clamp(-1, 1, I.punto(N));
    float etai = 1, etat = ior;
    if (cosi > 0) { std::swap(etai, etat); }
    // Compute sini using Snell's law
    float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
    // Total internal reflection
    if (sint >= 1) {
        kr = 1;
    }
    else {
        float cost = sqrtf(std::max(0.f, 1 - sint * sint));
        cosi = fabsf(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        kr = (Rs * Rs + Rp * Rp) / 2;
    }
    // As a consequence of the conservation of energy, the transmittance is given by:
    // kt = 1 - kr;
}
vec3 Camara::refract(vec3 &I, vec3 &N, float &ior) {
    float cosi = clamp(-1, 1, I.punto(N));
    float etai = 1, etat = ior;
    vec3 n = N;
    if (cosi < 0) { cosi = -cosi; } else { std::swap(etai, etat); n= -N; }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return k < 0 ? vec3(0,0,0) : eta * I + (eta * cosi - sqrtf(k)) * n;
}