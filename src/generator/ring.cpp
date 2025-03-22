#include "ring.hpp"

Figure generateRing(float ri, float re, int slices){
    Figure ring = Figure();
    float a = 0,delta = (2*M_PI) / slices;
       for (int i = 0; i < slices; i++, a += delta){

           ring.addPoint(Point::newPointSph(a, 0.0f, ri));
           ring.addPoint(Point::newPointSph(a,0.0f,re));
           ring.addPoint(Point::newPointSph(a+delta,0.0f,re));

           ring.addPoint(Point::newPointSph(a + delta, 0.0f, ri));
           ring.addPoint(Point::newPointSph(a,0.0f,re));
           ring.addPoint(Point::newPointSph(a+delta,0.0f,re));

           ring.addPoint(Point::newPointSph(a+delta, 0.0f, ri));
           ring.addPoint(Point::newPointSph(a,0.0f,re));
           ring.addPoint(Point::newPointSph(a,0.0f,ri));

           ring.addPoint(Point::newPointSph(a, 0.0f, re));
           ring.addPoint(Point::newPointSph(a+delta,0.0f,ri));
           ring.addPoint(Point::newPointSph(a+delta,0.0f,re));


       }
    return ring;
}