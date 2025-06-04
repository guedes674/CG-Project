#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <string>

class Translate {
    private:
        float x, y, z;

    public:
        Translate(float x, float y, float z) {
            Translate::x = x;
            Translate::y = y;
            Translate::z = z;
        }
        float get_x();
        float get_y();
        float get_z();
};

class Rotate {
    private:
        float angle, x, y, z;

    public:
        Rotate(float angle, float x, float y, float z) {
            Rotate::angle = angle;
            Rotate::x = x;
            Rotate::y = y;
            Rotate::z = z;
        }
        float get_angle();
        float get_x();
        float get_y();
        float get_z();
};

class Scale {
    private:
        float x, y, z;

    public:
        Scale(float x, float y, float z) {
            Scale::x = x;
            Scale::y = y;
            Scale::z = z;
        }
        float get_x();
        float get_y();
        float get_z();
};

#endif