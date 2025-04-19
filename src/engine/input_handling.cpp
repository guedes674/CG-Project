#include "input_handling.h"

using namespace std;

// Guarda a posição inicial do clique
int startX = 0, startY = 0;

/**
 * @brief Processa os cliques do rato (drag ou scroll)
 */
void process_mouse_buttons(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON && camera.mode == Camera::FPS) {
            startX = x;
            startY = y;
            mouse_dragging = true;
        } else if (camera.mode == Camera::ORBIT) {
            if (button == 3) camera.orbit_radius = std::max(camera.orbit_radius - 0.25f, 1.0f);
            else if (button == 4) camera.orbit_radius += 0.25f;
        }
    } else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
        mouse_dragging = false;
    }

    glutPostRedisplay();
}


/**
 * @brief Arrasta a câmara com o rato em modo ORBIT
 */
void process_mouse_motion(int x, int y) {
    if (!mouse_dragging || camera.mode != Camera::FPS) return;

    float dx = (x - startX) * camera.sensitivity;
    float dy = (y - startY) * camera.sensitivity;

    camera.fps_yaw   += dx;
    camera.fps_pitch -= dy;

    camera.fps_pitch = camera.fps_pitch < -89.0f ? -89.0f : (camera.fps_pitch > 89.0f ? 89.0f : camera.fps_pitch);

    // Atualiza a direção do vetor frontal
    camera.update_fps_vectors();

    startX = x;
    startY = y;

    glutPostRedisplay();
}



/**
 * @brief Updates camera position and orientation based on user input
 */
void update_camera() {
    if(camera.mode == Camera::ORBIT) {
        // Original orbit controls
        if(keys[(int)'a']) camera.orbit_alpha -= 0.01f;
        if(keys[(int)'d']) camera.orbit_alpha += 0.01f;
        if(keys[(int)'w']) camera.orbit_beta = std::min(camera.orbit_beta + 0.01f, float(M_PI/2 - 0.01));
        if(keys[(int)'s']) camera.orbit_beta = std::max(camera.orbit_beta - 0.01f, float(-M_PI/2 + 0.01));
        if(special_keys[GLUT_KEY_UP]) camera.orbit_radius = std::max(camera.orbit_radius - 0.25f, 1.0f);
        if(special_keys[GLUT_KEY_DOWN]) camera.orbit_radius += 0.25f;
    } else { // FPS mode
        // FPS movement controls
        float velocity = camera.move_speed;
        if(keys[(int)'w']) camera.fps_position = camera.fps_position + camera.fps_front * velocity;
        if(keys[(int)'s']) camera.fps_position = camera.fps_position - camera.fps_front * velocity;
        if(keys[(int)'a']) camera.fps_position = camera.fps_position - camera.fps_right * velocity;
        if(keys[(int)'d']) camera.fps_position = camera.fps_position + camera.fps_right * velocity;
        if(keys[(int)' ']) camera.fps_position.y += velocity;  // Space to ascend
        if(keys[(int)'c']) camera.fps_position.y -= velocity;  // C to descend
    }
    
    // Common controls
    if(keys[(int)'i']) scale += 0.025f;
    if(keys[(int)'o']) scale = std::max(scale - 0.025f, 0.05f);
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
