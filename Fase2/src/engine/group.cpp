#include "group.hpp"

void group::addModel(Figure model) {
    group::models.push_back(model);
}

vector<Figure> group::getModels() {
    return group::models;
}

void group::addTranslate(Translate translate) {
    group::translates.push_back(translate);
}

vector<Translate> group::getTranslate() {
    return group::translates;
}

void group::addRotate(Rotate rotate) {
    group::rotates.push_back(rotate);
}

vector<Rotate> group::getRotate() {
    return group::rotates;
}

void group::addScale(Scale scale) {
    group::scales.push_back(scale);
}

vector<Scale> group::getScale() {
    return group::scales;
}

void group::addGroup(group group) {
    group::child_groups.push_back(group);
}

vector<group> group::getGroups() {
    return group::child_groups;
}

void group::translate(float x, float y, float z) {
    for(Figure& model : group::models) {
        for(Point point : model.getPoints()) {
            point.x = point.x + x;
            point.y = point.y + y;
            point.z = point.z + z;
        }
    }
    for(group group : group::child_groups)
        group.translate(x, y, z);
}

void group::rotate(float angle, float x, float y, float z) {
    for(Figure& model : group::models) {
        for(Point point : model.getPoints()) {
            if(x == 1){
                float yy = (point.y)*cosf(angle) - (point.z)*sinf(angle);
                float zz = (point.y)*sinf(angle) + (point.z)*cosf(angle);
                point.y = yy;
                point.z = zz;
            }
            if(y == 1){
                float xx = (point.x)*cosf(angle) + (point.z)*sinf(angle);
                float zz = - (point.x)*sinf(angle) + (point.z)*cosf(angle);
                point.x = xx;
                point.z = zz;
            }
            if(z == 1) {
                float xx = (point.x)*cosf(angle) - (point.y)*sinf(angle);
                float yy = (point.x)*sinf(angle) + (point.y)*cosf(angle);
                point.x = xx;
                point.y = yy;
            }
        }
    }
    for(group group : group::child_groups)
        group.rotate(angle, x, y, z);
}

void group::scale(float x, float y, float z) {
    for(Figure& model : group::models) {
        for(Point point : model.getPoints()) {
            point.x = point.x * x;
            point.y = point.y * y;
            point.z = point.z * z;
        }
    }
    for(group group : group::child_groups)
        group.scale(x, y, z);
}