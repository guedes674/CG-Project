#include "camera.hpp"

camera::camera(){
  set_position(new Point(0, 0, 0));
  set_lookAt(new Point(0, 0, 0));
  set_up(new Point(0, 0, 0));
  camera::fov = 0;
  camera::near = 0;
  camera::far = 0;
}

Point camera::get_position() {
  return position;
}

void camera::set_position(Point * position) {
  camera::position = *position;
}

Point camera::get_lookAt() {
  return lookat;
}

void camera::set_lookAt(Point * lookat) {
  camera::lookat = *lookat;
}

Point camera::get_up() {
  return up;
}

void camera::set_up(Point * up) {
  camera::up = *up;
}

float camera::get_fov() {
  return fov;
}

void camera::set_fov(float fov) {
  camera::fov = fov;
}

float camera::get_near() {
  return near;
}

void camera::set_near(float near) {
  camera::near = near;
}

float camera::get_far() {
  return far;
}

void camera::set_far(float far) {
  camera::far = far;
}