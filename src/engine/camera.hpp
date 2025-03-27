#ifndef CAMERA_H
#define CAMERA_H
#include <string>
#include "../classes/point.hpp"

using namespace std;

class camera {
  private:
    Point position, lookat, up;
    float fov, near, far;

public:
    camera();
    Point get_position();
    void set_position(Point* position);
    Point get_lookAt();
    void set_lookAt(Point* lookAt);
    Point get_up();
    void set_up(Point* up);
    float get_fov();
    void set_fov(float fov);
    float get_near();
    void set_near(float near);
    float get_far();
    void set_far(float far);
};

#endif