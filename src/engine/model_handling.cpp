#include "model_handling.h"


/**
 * @brief Processes time-based translations for object animations
 * 
 * @param translation The translation parameters from XML
 */
void time_translation(translation_xml translation) {
    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    t /= translation.time;
    t -= floor(t);  // Loop animation

    int tessellation = 1000;
    int num_curve_points = (tessellation + 1) * translation.time_trans.points.size();
    float* curve_points = new float[num_curve_points * 3];
    float* div = new float[num_curve_points * 3];

    catmullrom_curve(tessellation, translation.time_trans.points, curve_points, div);

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_curve_points; ++i) {
        int idx = i * 3;
        glVertex3f(curve_points[idx], curve_points[idx + 1], curve_points[idx + 2]);
    }
    glEnd();

    int current_index = static_cast<int>(t * num_curve_points);
    current_index = current_index % num_curve_points;

    int offset = current_index * 3;
    float px = curve_points[offset];
    float py = curve_points[offset + 1];
    float pz = curve_points[offset + 2];

    float current_div[3];
    current_div[0] = div[offset];
    current_div[1] = div[offset + 1];
    current_div[2] = div[offset + 2];

    glTranslatef(px, py, pz);

    if (translation.time_trans.align == 0) {
        static float y[4] = { 0.0f, 1.0f, 0.0f };
        float m[16];
        generate_catmull_matrix(current_div, y, m);
        glMultMatrixf(m);
    }

    delete[] div;
    delete[] curve_points;
}

void time_rotation(rotation_xml rotation) {
    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    t /= rotation.time;
    t -= floor(t);  // Loop animation

    float angle = rotation.angle * t;
    glRotatef(angle, rotation.x, rotation.y, rotation.z);
}

/**
 * @brief Recursively draws a group and all its children
 * 
 * @param group The group to draw
 */
void recursive_draw(const group_xml& group) {
    glPushMatrix();

    int order[3] = {0};
    for(int i = 0; i < 3; i++) {
        if(group.transformations.rotation_exists && group.transformations.rotation.order == i)
            order[i] = 1;
        if(group.transformations.translation_exists && group.transformations.translation.order == i)
            order[i] = 2;
        if(group.transformations.scale_exists && group.transformations.scale.order == i)
            order[i] = 3;
    }

    for(int i = 0; i < 3; i++) {
        switch(order[i]) {
            case 1: // Rotation
                if(group.transformations.rotation.time == 0)
                    glRotatef(group.transformations.rotation.angle,
                            group.transformations.rotation.x,
                            group.transformations.rotation.y,
                            group.transformations.rotation.z);
                else
                    time_rotation(group.transformations.rotation);
                break;
            case 2: // Translation
                if(group.transformations.translation.time == 0)
                    glTranslatef(group.transformations.translation.x,
                                group.transformations.translation.y,
                                group.transformations.translation.z);
                else
                    time_translation(group.transformations.translation);
                break;
            case 3: // Scale
                glScalef(group.transformations.scale.x,
                        group.transformations.scale.y,
                        group.transformations.scale.z);
                break;
        }
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3f(1.0f, 1.0f, 1.0f);

    for(const auto& model : group.models) {
        nmodels++;
        vbo* current_vbo = model_dict[model.file_name];
        total_models++;

        glBindBuffer(GL_ARRAY_BUFFER, current_vbo->vertices);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_vbo->indexes);
        glDrawElements(GL_TRIANGLES, current_vbo->total_indexes, GL_UNSIGNED_INT, 0);
    }

    glDisableClientState(GL_VERTEX_ARRAY);

    for(const auto& subgroup : group.groups)
        recursive_draw(subgroup);

    glPopMatrix();
}
/**
 * @brief Loads models into VBOs and populates the model dictionary
 * 
 * @param group The group containing models to load
 * @param dict The dictionary to store model VBOs
 * @return int 0 on success, 1 on failure
 */
int populate_dict(const group_xml& group, unordered_map<string, vbo*>& dict) {
    for(const auto& model : group.models) {
        if(dict.find(model.file_name) == dict.end()) {
            vector<float> vertices;
            vector<unsigned int> indexes;
            
            if(read_model(model.file_name, vertices, indexes)) {
                cerr << "Error loading model: " << model.file_name << endl;
                return 1;
            }

            GLuint vboID, iboID;
            glGenBuffers(1, &vboID);
            glBindBuffer(GL_ARRAY_BUFFER, vboID);
            glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

            glGenBuffers(1, &iboID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size()*sizeof(unsigned int), indexes.data(), GL_STATIC_DRAW);

            dict[model.file_name] = new vbo(vboID, vertices.size()/3, iboID, indexes.size());
        }
    }

    for(const auto& subgroup : group.groups)
        if(populate_dict(subgroup, dict)) return 1;

    return 0;
}