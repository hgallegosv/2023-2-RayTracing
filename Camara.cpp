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
    Esfera esf(vec3(2,0,0), 8, vec3(0,0,1));
    esf.setConstantes(0.8, 0.2, 32, 0.9);
    Esfera esf2(vec3(-10,0,10), 6, vec3(1,0,0));
    esf2.setConstantes(0.8, 0.2, 32, 0.9);
    vector<Objeto*> objetos;
    objetos.emplace_back(&esf);
    objetos.emplace_back(&esf2);
    vec3 color, Pi, N;
    float t, t_tmp;
    for(int x=0;  x < w; x++) {
        for (int y = 0; y < h; y++) {
            if (x==330 and y==h-305) {
                cout << "aqui";
            }
            dir = ze * (-f) + ye * a * (y / h - 0.5) + xe * b * (x / w - 0.5);
            dir.normalize();
            rayo.dir = dir;
            //color.set(0,0,0);
            color = iluminacion(rayo, luz, objetos, 1);
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
vec3 Camara::iluminacion(Rayo &rayo, Luz &luz, vector<Objeto*> &objetos, int prof) {
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

    if ( interseccion ) {
        // color = esf.color;
        vec3 ambiente = vec3(1,1,1) * 0.2;
        vec3 L = luz.pos - Pi;
        L.normalize();
        float difuso = L.punto(N);
        float color_difuso = objeto->kd * max(0.0f, difuso);
        // iluminacion especular
        vec3 R = 2 * difuso * N - L;
        vec3 V = -rayo.dir;
        float color_especular = objeto->ks * pow( max(0.0f, R.punto(V)), objeto->n);
        // reflexion
        vec3 color_reflexion(0,0,0);
        if ( objeto->ke > 0 and prof + 1 <= 7){
            vec3 dir_ref = 2*V.punto(N)*N - V;
            dir_ref.normalize();
            Rayo rayo_reflejado(Pi+0.005*N, dir_ref);
            color_reflexion = iluminacion(rayo_reflejado, luz, objetos, prof+1);
        }

        color = objeto->color * (ambiente + luz.color*(color_difuso + color_especular)) + objeto->ke*color_reflexion;
        color.max_to_one();
    }
    return color;
}
