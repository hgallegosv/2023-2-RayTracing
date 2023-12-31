#include <iostream>
#include "Camara.h"
void escena1();
void escena2();
void escena3();
int main() {
    escena3();
    return 0;
}
void escena1(){
    Camara cam;
    for (int x=-100, n=1; x < 100; x += 10, n++) {
        cam.configurar(3, 60, 800, 600,
                       vec3(x,5,50),
                       vec3(0,0,0),
                       vec3(0,1,0));
        cam.renderizar(n);
    }
}
void escena2(){
    Camara cam;
    cam.configurar(3, 60, 800, 600,
                   vec3(0,10,60),
                   vec3(0,0,0),
                   vec3(0,1,0));
    cam.renderizar2(1);
}
void escena3(){
    Camara cam;
    for (int x=-100, n=1; x < 100; x += 1, n++) {
        cam.configurar(3, 60, 800, 600,
                       vec3(x,10,60),
                       vec3(0,0,0),
                       vec3(0,1,0));
        cam.renderizar2(n);
    }
}