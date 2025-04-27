#include "model_handling.h"

extern bool snapshot;
extern float gl_last_matrix[16];

//Retorna a Model View Matrix
static inline void get_mvp_matrix(float *matrix){
	float model_view[16],projection[16];

	glGetFloatv(GL_MODELVIEW_MATRIX,model_view);
	glGetFloatv(GL_PROJECTION_MATRIX,projection);

	glPushMatrix();
	glLoadMatrixf(projection);
	glMultMatrixf(model_view);

	glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
	glPopMatrix();
}

static inline bool check_viewfrustum_draw(
    const float mvp[16],     // MVP em column-major
    const float bounds[6])   // { minX, maxX, minY, maxY, minZ, maxZ }
{
    // Extrai os 6 planos: left, right, bottom, top, near, far
    float planes[6][4];
    for (int i = 0; i < 4; ++i) {
        planes[0][i] =  mvp[3 + 4*i] + mvp[0 + 4*i]; // left
        planes[1][i] =  mvp[3 + 4*i] - mvp[0 + 4*i]; // right
        planes[2][i] =  mvp[3 + 4*i] + mvp[1 + 4*i]; // bottom
        planes[3][i] =  mvp[3 + 4*i] - mvp[1 + 4*i]; // top
        planes[4][i] =  mvp[3 + 4*i] + mvp[2 + 4*i]; // near
        planes[5][i] =  mvp[3 + 4*i] - mvp[2 + 4*i]; // far
    }

    // Normaliza cada plano (|normal| = 1)
    for (int p = 0; p < 6; p++) {
        float a = planes[p][0], b = planes[p][1], c = planes[p][2];
        float invLen = 1.0f / sqrtf(a*a + b*b + c*c);
        for (int i = 0; i < 4; i++)
            planes[p][i] *= invLen;
    }

    // Testa cada plano com o "p-vertex" do AABB
    for (int p = 0; p < 6; p++) {
        const float* pl = planes[p];
        float x = (pl[0] >= 0.0f) ? bounds[1] : bounds[0];
        float y = (pl[1] >= 0.0f) ? bounds[3] : bounds[2];
        float z = (pl[2] >= 0.0f) ? bounds[5] : bounds[4];
        if (pl[0]*x + pl[1]*y + pl[2]*z + pl[3] < 0.0f)
            return false;
    }
    return true;
}

void draw_bounding_box(float * bounding_box){
    glBegin(GL_QUADS);
        glVertex3f(bounding_box[1] - 0.1,bounding_box[2] + 0.1,bounding_box[4] + 0.1);
        glVertex3f(bounding_box[0] + 0.1,bounding_box[2] + 0.1,bounding_box[4] + 0.1);
        glVertex3f(bounding_box[0] + 0.1,bounding_box[2] + 0.1,bounding_box[5] - 0.1);
        glVertex3f(bounding_box[1] - 0.1,bounding_box[2] + 0.1,bounding_box[5] - 0.1);

        glVertex3f(bounding_box[0] + 0.1,bounding_box[3] - 0.1,bounding_box[4] + 0.1);
        glVertex3f(bounding_box[1] - 0.1,bounding_box[3] - 0.1,bounding_box[4] + 0.1);
        glVertex3f(bounding_box[1] - 0.1,bounding_box[3] - 0.1,bounding_box[5] - 0.1);
        glVertex3f(bounding_box[0] + 0.1,bounding_box[3] - 0.1,bounding_box[5] - 0.1);

        glVertex3f(bounding_box[1] - 0.1,bounding_box[3] - 0.1,bounding_box[4] + 0.1);
        glVertex3f(bounding_box[1] - 0.1,bounding_box[2] + 0.1,bounding_box[4] + 0.1);
        glVertex3f(bounding_box[1] - 0.1,bounding_box[2] + 0.1,bounding_box[5] - 0.1);
        glVertex3f(bounding_box[1] - 0.1,bounding_box[3] - 0.1,bounding_box[5] - 0.1);

        glVertex3f(bounding_box[0] + 0.1,bounding_box[2] + 0.1,bounding_box[4] + 0.1);
        glVertex3f(bounding_box[0] + 0.1,bounding_box[3] - 0.1,bounding_box[4] + 0.1);
        glVertex3f(bounding_box[0] + 0.1,bounding_box[3] - 0.1,bounding_box[5] - 0.1);
        glVertex3f(bounding_box[0] + 0.1,bounding_box[2] + 0.1,bounding_box[5] - 0.1);

        glVertex3f(bounding_box[0] + 0.1,bounding_box[3] - 0.1,bounding_box[4] + 0.1);
        glVertex3f(bounding_box[0] + 0.1,bounding_box[2] + 0.1,bounding_box[4] + 0.1);
        glVertex3f(bounding_box[1] - 0.1,bounding_box[2] + 0.1,bounding_box[4] + 0.1);
        glVertex3f(bounding_box[1] - 0.1,bounding_box[3] - 0.1,bounding_box[4] + 0.1);

        glVertex3f(bounding_box[0] + 0.1,bounding_box[2] + 0.1,bounding_box[5] - 0.1);
        glVertex3f(bounding_box[0] + 0.1,bounding_box[3] - 0.1,bounding_box[5] - 0.1);
        glVertex3f(bounding_box[1] - 0.1,bounding_box[3] - 0.1,bounding_box[5] - 0.1);
        glVertex3f(bounding_box[1] - 0.1,bounding_box[2] + 0.1,bounding_box[5] - 0.1);
	glEnd();
}
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

    if (show_catmull_curves){
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < num_curve_points; ++i) {
            int idx = i * 3;
            glVertex3f(curve_points[idx], curve_points[idx + 1], curve_points[idx + 2]);
        }
        glEnd();
    }

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

    //cout << "Xml: " << xml << endl;
    if (translation.time_trans.tracking) {
        int id = translation.time_trans.tracking_id;
        position_dict[id] = Vector3(px, py, pz);
        if (std::find(position_keys.begin(), position_keys.end(), id) == position_keys.end())
            position_keys.push_back(id);
    }
    
    glTranslatef(px, py, pz);

    if (translation.time_trans.align == 1) {
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

    float gl_matrix[16];

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
                else{
                    time_translation(group.transformations.translation);}
                break;
            case 3: // Scale
                glScalef(group.transformations.scale.x,
                        group.transformations.scale.y,
                        group.transformations.scale.z);
                break;
        }
    }

    /*
    if (snapshot){
        for (int i = 0; i < 16; i++) gl_matrix[i] = gl_last_matrix[i];
    }
    else{
        get_mvp_matrix(gl_matrix);
        for (int i = 0; i < 16; i++) gl_last_matrix[i] = gl_matrix[i];
    }
    */
    get_mvp_matrix(gl_matrix);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3f(1.0f, 1.0f, 1.0f);

    for(const auto& model : group.models) {
        vbo* current_vbo = model_dict[model.file_name];
        // Check here for view frustum culling
        
        if (check_viewfrustum_draw(gl_matrix, current_vbo->bounding_box)){
            current_models++;

            glBindBuffer(GL_ARRAY_BUFFER, current_vbo->vertices);
            glVertexPointer(3, GL_FLOAT, 0, 0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_vbo->indexes);
            glDrawElements(GL_TRIANGLES, current_vbo->total_indexes, GL_UNSIGNED_INT, 0);
            if (show_bounding_box)draw_bounding_box(current_vbo->bounding_box);
        }
    }
    glDisableClientState(GL_VERTEX_ARRAY);

    for(const auto& subgroup : group.groups){
        recursive_draw(subgroup);
    }

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
            float bounding_box[6];

            if(read_model(model.file_name, vertices, indexes,bounding_box)) {
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

            dict[model.file_name] = new vbo(vboID, vertices.size()/3, iboID, indexes.size(),bounding_box);
        }
    }

    for(const auto& subgroup : group.groups)
        if(populate_dict(subgroup, dict)) return 1;

    return 0;
}


// May be used in the future
void vbo::recomputeAABB_fromVBO(const float model[16]) {
    // 1) liga o buffer e mapeia
    glBindBuffer(GL_ARRAY_BUFFER, vertices);
    float* data = (float*) glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    if (!data) return;  // falha ao mapear, sai sem alterar a box

    // 2) prepara min/max iniciais
    float wminX =  FLT_MAX, wminY =  FLT_MAX, wminZ =  FLT_MAX;
    float wmaxX = -FLT_MAX, wmaxY = -FLT_MAX, wmaxZ = -FLT_MAX;

    // 3) percorre todos os vértices
    for (unsigned i = 0; i < total_vertices; ++i) {
        // posições no VBO
        float x = data[3*i + 0];
        float y = data[3*i + 1];
        float z = data[3*i + 2];

        // multiplica pela matriz model (coluna-major):
        float wx = model[0]*x + model[4]*y + model[ 8]*z + model[12];
        float wy = model[1]*x + model[5]*y + model[ 9]*z + model[13];
        float wz = model[2]*x + model[6]*y + model[10]*z + model[14];

        // atualiza min/max
        wminX = std::min(wminX, wx);  wmaxX = std::max(wmaxX, wx);
        wminY = std::min(wminY, wy);  wmaxY = std::max(wmaxY, wy);
        wminZ = std::min(wminZ, wz);  wmaxZ = std::max(wmaxZ, wz);
    }

    // 4) desmapeia e guarda a nova box
    glUnmapBuffer(GL_ARRAY_BUFFER);

    bounding_box[0] = wminX;  bounding_box[1] = wmaxX;
    bounding_box[2] = wminY;  bounding_box[3] = wmaxY;
    bounding_box[4] = wminZ;  bounding_box[5] = wmaxZ;
}