#include "input_handling.h"

using namespace std;

// Guarda a posição inicial do clique
int startX = 0, startY = 0;

void update_camera_target_from_index() {
    if (current_target_index >= 0 && current_target_index < (int)position_keys.size()) {
        int id = position_keys[current_target_index];
        camera.orbit_look_at = position_dict[id];
    } else {
        camera.orbit_look_at = Vector3(0.0f, 0.0f, 0.0f); // reset para origem
    }
}

/**
 * @brief Processa os cliques do rato (drag ou scroll)
 */
void process_mouse_buttons(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        // start dragging on left button in both FPS and ORBIT
        if (button == GLUT_LEFT_BUTTON) {
            startX = x;
            startY = y;
            mouse_dragging = true;
        }
        else if (camera.mode == Camera::ORBIT) {
            // scroll wheel zoom in/out
            if (button == 3) camera.orbit_radius = std::max(camera.orbit_radius - 0.25f, 1.0f);
            else if (button == 4) camera.orbit_radius += 0.25f;
        }
    }
    else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
        mouse_dragging = false;
    }

    glutPostRedisplay();
}


/**
 * @brief Arrasta a câmara com o rato em modo ORBIT
 */
void process_mouse_motion(int x, int y) {
    if (!mouse_dragging) return;

    float dx;
    float dy;
    if (camera.mode == Camera::FPS) {
        // --- FPS rotation ---
        dx = (x - startX) * camera.sensitivity;
        dy = (y - startY) * camera.sensitivity;
        camera.fps_yaw   += dx;
        camera.fps_pitch -= dy;
        camera.fps_pitch = camera.fps_pitch < -89.0f ? -89.0f : (camera.fps_pitch > 89.0f ? 89.0f : camera.fps_pitch);
        camera.update_fps_vectors();

    } else if (camera.mode == Camera::ORBIT) {
        dx = (x - startX) * camera.sensitivity_orbit;
        dy = (y - startY) * camera.sensitivity_orbit;
        // --- ORBIT drag rotates around target ---
        camera.orbit_azimuth += dx;
        camera.orbit_polar  -= dy;
        // clamp beta to avoid gimbal‐lock at poles
        float limit = M_PI/2.0f - 0.01f;
        camera.orbit_polar = std::max(std::min(camera.orbit_polar, limit), -limit);
    }

    startX = x;
    startY = y;
    glutPostRedisplay();
}



/**
 * @brief Updates camera position and orientation based on user input
 */
void update_camera() {
    if (camera.mode == Camera::ORBIT) {
        // --- existing orbit controls ---
        if(keys[(int)'a']) camera.orbit_azimuth -= 0.01f;
        if(keys[(int)'d']) camera.orbit_azimuth += 0.01f;
        if(keys[(int)'w']) camera.orbit_polar  = std::min(camera.orbit_polar + 0.01f, float(M_PI/2 - .01));
        if(keys[(int)'s']) camera.orbit_polar  = std::max(camera.orbit_polar - 0.01f, float(-M_PI/2 + .01));
        if(special_keys[GLUT_KEY_UP])   camera.orbit_radius = std::max(camera.orbit_radius - .25f, 1.0f);
        if(special_keys[GLUT_KEY_DOWN]) camera.orbit_radius += .25f;

        // ← add this block to continuously follow the tracked object
        if (current_target_index >= 0 && current_target_index < (int)position_keys.size()) {
            int id = position_keys[current_target_index];
            camera.follow_target(position_dict[id]);
        }
    } else {
        // --- FPS controls ---
        float velocity = camera.move_speed;
        if(keys[(int)'w']) camera.fps_position = camera.fps_position + camera.fps_front * velocity;
        if(keys[(int)'s']) camera.fps_position = camera.fps_position - camera.fps_front * velocity;
        if(keys[(int)'a']) camera.fps_position = camera.fps_position - camera.fps_right * velocity;
        if(keys[(int)'d']) camera.fps_position = camera.fps_position + camera.fps_right * velocity;
        if(keys[(int)' ']) camera.fps_position.y += velocity;  // Space to ascend
        if(keys[(int)'c']) camera.fps_position.y -= velocity;  // C to descend
    }

}

/**
 * @brief Handles keyboard input for regular keys
 * 
 * @param key The key that was pressed
 * @param x The x-coordinate of the mouse when the key was pressed
 * @param y The y-coordinate of the mouse when the key was pressed
 */
void default_key_func(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    keys[tolower(key)] = true;

    if (tolower(key) == 'm') {
        camera.toggle_mode();
        glutSetWindowTitle(
            camera.mode == Camera::ORBIT ? "CG@33 - Orbit Mode" : "CG@33 - FPS Mode"
        );
    }

    if (tolower(key) == 'q') {
        // Clean up resources before exiting
        for(auto& entry : model_dict) {
            glDeleteBuffers(1, &entry.second->vertices);
            glDeleteBuffers(1, &entry.second->indexes);
            delete entry.second;
        }
        exit(0);
    }

    if (tolower(key) == 'b') {
        show_bounding_box = !show_bounding_box;
    }

    if (tolower(key) == 'p') {
        show_catmull_curves = !show_catmull_curves;
    }
    
    if (tolower(key) == 'o') {
        show_axes = !show_axes;
    }

    glutPostRedisplay();
}

/**
 * @brief Handles keyboard release for regular keys
 * 
 * @param key The key that was released
 * @param x The x-coordinate of the mouse when the key was released
 * @param y The y-coordinate of the mouse when the key was released
 */
void default_key_up_func(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    keys[tolower(key)] = false;
}

/**
 * @brief Handles keyboard input for special keys (arrows, etc.)
 * 
 * @param key The special key that was pressed
 * @param x The x-coordinate of the mouse when the key was pressed
 * @param y The y-coordinate of the mouse when the key was pressed
 */
void special_key_func(int key, int x, int y) {
    (void)x;
    (void)y;
    special_keys[key] = true;
    
    if (camera.mode == Camera::ORBIT) {
        if (key == GLUT_KEY_LEFT) {
            current_target_index--;
            if (current_target_index < -1)
                current_target_index = position_keys.size() - 1;
            update_camera_target_from_index();
        } else if (key == GLUT_KEY_RIGHT) {
            current_target_index++;
            if (current_target_index > (int)position_keys.size() - 1)
                current_target_index = -1;
            update_camera_target_from_index();
        }
    }

    glutPostRedisplay();
}

/**
 * @brief Handles keyboard release for special keys
 * 
 * @param key The special key that was released
 * @param x The x-coordinate of the mouse when the key was released
 * @param y The y-coordinate of the mouse when the key was released
 */
void special_key_up_func(int key, int x, int y) {
    (void)x;
    (void)y;
    special_keys[key] = false;
}
