#include "bezier.h"

using namespace std;

int read_patch_file(char* patch, vector<point> &points, vector<vector<int>> &indexes){
    std::ifstream file;
    file.open(patch, ios::in);

    if(file.fail()) return 1;

}

void bezier(char* patch, int detail, vector<float>vertices, vector<unsigned int> indexes){

    vector<point> points;

    
}