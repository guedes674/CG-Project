#ifndef GROUP_H
#define GROUP_H
#include <math.h>
#include "camera.hpp"
#include "transform.hpp"
#include "../classes/figure.hpp"
#include "../classes/list.hpp"
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

class group {
    private:
      vector<Figure> models;
      vector<Translate> translates;
      vector<Rotate> rotates;
      vector<Scale> scales;
      vector<group> child_groups;

    public:
        void addModel(Figure model);
        vector<Figure> getModels();
        void addTranslate(Translate translate);
        vector<Translate> getTranslate();
        void addRotate(Rotate rotate);
        vector<Rotate> getRotate();
        void addScale(Scale scale);
        vector<Scale> getScale();
        void addGroup(group group);
        vector<group> getGroups();
        void translate(float x, float y, float z);
        void rotate(float angle, float x, float y, float z);
        void scale(float x, float y, float z);
};

#endif