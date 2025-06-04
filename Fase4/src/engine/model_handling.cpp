#include "model_handling.h"

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
        glVertex3f(bounding_box[1] ,bounding_box[2] ,bounding_box[4] );
        glVertex3f(bounding_box[0] ,bounding_box[2] ,bounding_box[4] );
        glVertex3f(bounding_box[0] ,bounding_box[2] ,bounding_box[5] );
        glVertex3f(bounding_box[1] ,bounding_box[2] ,bounding_box[5] );

        glVertex3f(bounding_box[0] ,bounding_box[3] ,bounding_box[4] );
        glVertex3f(bounding_box[1] ,bounding_box[3] ,bounding_box[4] );
        glVertex3f(bounding_box[1] ,bounding_box[3] ,bounding_box[5] );
        glVertex3f(bounding_box[0] ,bounding_box[3] ,bounding_box[5] );

        glVertex3f(bounding_box[1] ,bounding_box[3] ,bounding_box[4] );
        glVertex3f(bounding_box[1] ,bounding_box[2] ,bounding_box[4] );
        glVertex3f(bounding_box[1] ,bounding_box[2] ,bounding_box[5] );
        glVertex3f(bounding_box[1] ,bounding_box[3] ,bounding_box[5] );

        glVertex3f(bounding_box[0] ,bounding_box[2] ,bounding_box[4] );
        glVertex3f(bounding_box[0] ,bounding_box[3] ,bounding_box[4] );
        glVertex3f(bounding_box[0] ,bounding_box[3] ,bounding_box[5] );
        glVertex3f(bounding_box[0] ,bounding_box[2] ,bounding_box[5] );

        glVertex3f(bounding_box[0] ,bounding_box[3] ,bounding_box[4] );
        glVertex3f(bounding_box[0] ,bounding_box[2] ,bounding_box[4] );
        glVertex3f(bounding_box[1] ,bounding_box[2] ,bounding_box[4] );
        glVertex3f(bounding_box[1] ,bounding_box[3] ,bounding_box[4] );

        glVertex3f(bounding_box[0] ,bounding_box[2] ,bounding_box[5] );
        glVertex3f(bounding_box[0] ,bounding_box[3] ,bounding_box[5] );
        glVertex3f(bounding_box[1] ,bounding_box[3] ,bounding_box[5] );
        glVertex3f(bounding_box[1] ,bounding_box[2] ,bounding_box[5] );
	glEnd();
}
/**
 * @brief Processes time-based translations for object animations
 * 
 * @param translation The translation parameters from XML
 */
void time_translation(translation_xml translation) {
    double t = elapsed_time / 1000.0;
    t /= translation.time;
    t -= floor(t);  // Loop animation

    int tessellation = 400;
    if ((current_target_index> -1) && (translation.time_trans.tracking_id == position_keys[current_target_index])) 
        tessellation = 2000;
    int num_curve_points = (tessellation + 1) * translation.time_trans.points.size();
    float* curve_points = new float[num_curve_points * 3];
    float* div = new float[num_curve_points * 3];

    catmullrom_curve(tessellation, translation.time_trans.points, curve_points, div);
    float emissive_full[4] = {1.0f,1.0f,1.0f,1.0f};
    float emissive_def[4] = {0.0f,0.0f,0.0f,1.0f};
    if (show_catmull_curves){
        glMaterialfv(GL_FRONT, GL_EMISSION, emissive_full);
        glBegin(GL_LINE_LOOP);
            for (int i = 0; i < num_curve_points; ++i) {
                int idx = i * 3;
                glVertex3f(curve_points[idx], curve_points[idx + 1], curve_points[idx + 2]);
            }
        glEnd();
        glMaterialfv(GL_FRONT, GL_EMISSION, emissive_def);
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
    double t = elapsed_time / 1000.0;
    t /= rotation.time;
    t -= floor(t);  // Loop animation

    float angle = rotation.angle * t;
    glRotatef(angle, rotation.x, rotation.y, rotation.z);
}

int load_texture(std::string texture_name) {

	unsigned int texture,texture_width,texture_height;
	unsigned char *texture_data;
	unsigned int texture_id = 0;

	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilGenImages(1,&texture);
	ilBindImage(texture);
	if (ilLoadImage((ILstring)texture_name.c_str()) == IL_TRUE){
		texture_width = ilGetInteger(IL_IMAGE_WIDTH);
		texture_height = ilGetInteger(IL_IMAGE_HEIGHT);
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		texture_data = ilGetData();

		glGenTextures(1,&texture_id);
		
		glBindTexture(GL_TEXTURE_2D,texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
    
        // Anisotropic filtering if supported
        if (glewIsSupported("GL_EXT_texture_filter_anisotropic")) {
            float max_aniso = 0.0f;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
        }    

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return texture_id;
}

void apply_color(color model_color){

		float diffuse[4] = {(float)model_color.diffuse_r/255,(float)model_color.diffuse_g/255,(float)model_color.diffuse_b/255,1.0f};
		float ambient[4] = {(float)model_color.ambient_r/255,(float)model_color.ambient_g/255,(float)model_color.ambient_b/255,1.0f};
		float specular[4] = {(float)model_color.specular_r/255,(float)model_color.specular_g/255,(float)model_color.specular_b/255,1.0f};
		float emissive[4] = {(float)model_color.emissive_r/255,(float)model_color.emissive_g/255,(float)model_color.emissive_b/255,1.0f};

		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, emissive);
		glMaterialf(GL_FRONT, GL_SHININESS, (float)model_color.shine);

}

/**
 * @brief Draw normals as lines from vertices with arrow tips
 * 
 * @param vbo_model Pointer to the VBO containing vertex and normal data
 * @param normal_length Length of the normal lines
 */
void draw_normals(vbo* vbo_model, float normal_length = 0.1f) {
    // Save current OpenGL state
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
    
    // Disable lighting and textures for better visibility
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    // Set normal line appearance
    glColor3f(0.0f, 1.0f, 0.0f); // Green for normals
    glLineWidth(1.0f);
    
    // Get vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo_model->vertices);
    float* vertices = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    
    // Get normal data
    glBindBuffer(GL_ARRAY_BUFFER, vbo_model->normals);
    float* normals = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    
    if (vertices && normals) {
        // Constants for arrow head
        float arrow_size = normal_length * 0.2f;  // 20% of normal length
        float arrow_angle = 0.5f;  // ~30 degrees in radians
        
        glBegin(GL_LINES);
        for (unsigned int i = 0; i < vbo_model->total_vertices; i++) {
            // Extract vertex and normal coordinates
            float vx = vertices[i*3];
            float vy = vertices[i*3+1];
            float vz = vertices[i*3+2];
            
            float nx = normals[i*3] * normal_length;
            float ny = normals[i*3+1] * normal_length;
            float nz = normals[i*3+2] * normal_length;
            
            // Normalize the normal vector
            float length = sqrtf(nx*nx + ny*ny + nz*nz);
            float norm_x = nx / length;
            float norm_y = ny / length;
            float norm_z = nz / length;
            
            // Find perpendicular vectors for arrow head
            float px, py, pz;
            // First perpendicular vector (using cross product with arbitrary direction)
            if (fabsf(norm_y) < 0.99f) {
                // Cross with up vector if normal is not almost vertical
                px = norm_z;
                py = 0.0f;
                pz = -norm_x;
            } else {
                // Cross with right vector if normal is almost vertical
                px = 0.0f;
                py = -norm_z;
                pz = norm_y;
            }
            
            // Normalize first perpendicular vector
            length = sqrtf(px*px + py*py + pz*pz);
            px /= length;
            py /= length;
            pz /= length;
            
            // Second perpendicular vector (cross product of normal and first perpendicular)
            float qx = norm_y * pz - norm_z * py;
            float qy = norm_z * px - norm_x * pz;
            float qz = norm_x * py - norm_y * px;
            
            // Main normal line
            glVertex3f(vx, vy, vz);
            glVertex3f(vx + nx, vy + ny, vz + nz);
            
            // Calculate arrow head points
            float tip_x = vx + nx;
            float tip_y = vy + ny;
            float tip_z = vz + nz;
            
            // Left arrow head line
            glVertex3f(tip_x, tip_y, tip_z);
            glVertex3f(
                tip_x - arrow_size * (norm_x * cosf(arrow_angle) + px * sinf(arrow_angle)),
                tip_y - arrow_size * (norm_y * cosf(arrow_angle) + py * sinf(arrow_angle)),
                tip_z - arrow_size * (norm_z * cosf(arrow_angle) + pz * sinf(arrow_angle))
            );
            
            // Right arrow head line
            glVertex3f(tip_x, tip_y, tip_z);
            glVertex3f(
                tip_x - arrow_size * (norm_x * cosf(arrow_angle) - px * sinf(arrow_angle)),
                tip_y - arrow_size * (norm_y * cosf(arrow_angle) - py * sinf(arrow_angle)),
                tip_z - arrow_size * (norm_z * cosf(arrow_angle) - pz * sinf(arrow_angle))
            );
            
            // Top arrow head line
            glVertex3f(tip_x, tip_y, tip_z);
            glVertex3f(
                tip_x - arrow_size * (norm_x * cosf(arrow_angle) + qx * sinf(arrow_angle)),
                tip_y - arrow_size * (norm_y * cosf(arrow_angle) + qy * sinf(arrow_angle)),
                tip_z - arrow_size * (norm_z * cosf(arrow_angle) + qz * sinf(arrow_angle))
            );
            
            // Bottom arrow head line
            glVertex3f(tip_x, tip_y, tip_z);
            glVertex3f(
                tip_x - arrow_size * (norm_x * cosf(arrow_angle) - qx * sinf(arrow_angle)),
                tip_y - arrow_size * (norm_y * cosf(arrow_angle) - qy * sinf(arrow_angle)),
                tip_z - arrow_size * (norm_z * cosf(arrow_angle) - qz * sinf(arrow_angle))
            );
        }
        glEnd();
    }
    
    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, vbo_model->normals);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_model->vertices);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    
    // Restore previous OpenGL state
    glPopAttrib();
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

    get_mvp_matrix(gl_matrix);

    glColor3f(1.0f, 1.0f, 1.0f);
    for(const auto& model : group.models) {
        GLuint texture_id = 0;
        vbo* current_vbo = model_dict[model.file_name];
        // Check here for view frustum culling
        
        if (check_viewfrustum_draw(gl_matrix, current_vbo->bounding_box)){
            current_models++;
            bool has_texture = (model.texture_name != "") && (texture_dict.find(model.texture_name) != texture_dict.end());
            apply_color(model.model_color);

            glBindBuffer(GL_ARRAY_BUFFER, current_vbo->vertices);
            glVertexPointer(3, GL_FLOAT, 0, 0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_vbo->indexes);
            glBindBuffer(GL_ARRAY_BUFFER, current_vbo->normals);

	        glNormalPointer(GL_FLOAT,0,0);

            if (has_texture){
                texture_id = texture_dict[model.texture_name];
                glBindTexture(GL_TEXTURE_2D, texture_id);
                glBindBuffer(GL_ARRAY_BUFFER, current_vbo->textures);
                glTexCoordPointer(2, GL_FLOAT, 0, 0);
            }

            glDrawElements(GL_TRIANGLES, current_vbo->total_indexes, GL_UNSIGNED_INT, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            
            if (show_normals) draw_normals(current_vbo, 0.5f);

            if (show_bounding_box)draw_bounding_box(current_vbo->bounding_box);
        }
    }

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
int populate_dict(const group_xml& group, unordered_map<string, vbo*>& dict, unordered_map<string, GLuint>& texture_dict) {
    for(const auto& model : group.models) {
        if(dict.find(model.file_name) == dict.end()) {
            vector<float> vertices;
            vector<unsigned int> indexes;
            vector<float> normals;
            vector<float> textures;
            float bounding_box[6];
            Vector3 center;
            float radius;
            if(read_model(model.file_name, vertices, indexes,normals,textures,bounding_box,center,radius)) {
                cerr << "Error loading model: " << model.file_name << endl;
                cerr << "Texture file: "<< model.texture_name << endl;
                return 1;
            }

            GLuint vertices_id, indexes_id, normals_id, textures_id;
            glGenBuffers(1, &vertices_id);
            glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
            glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

            glGenBuffers(1, &normals_id);
            glBindBuffer(GL_ARRAY_BUFFER, normals_id);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

            glGenBuffers(1, &indexes_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexes_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size()*sizeof(unsigned int), indexes.data(), GL_STATIC_DRAW);

            //Create VBO textura
            glGenBuffers(1, &textures_id);
            glBindBuffer(GL_ARRAY_BUFFER, textures_id);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textures.size(), textures.data(), GL_STATIC_DRAW);

            dict[model.file_name] = new vbo(vertices_id, vertices.size()/3, indexes_id, indexes.size(),normals_id,textures_id,bounding_box,center,radius);
        }
        if ((model.texture_name!="") && (texture_dict.find(model.texture_name) == texture_dict.end())) {

            GLuint text_id = load_texture(model.texture_name);
            //inserts pair of file's name and respective VBO information
            texture_dict.insert(std::make_pair(model.texture_name, text_id));
        }
    }

    for(const auto& subgroup : group.groups)
        if(populate_dict(subgroup, dict, texture_dict)) return 1;

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