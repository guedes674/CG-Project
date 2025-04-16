#ifndef POINT_H
#define POINT_H

class point{
    public:
        float x, y, z;
        point() {
            x = y = z = 0;
        }
        point(float new_x, float new_y, float new_z) {
            x = new_x;
            y = new_y;
            z = new_z;
        }
};

#endif

