#include "input_handling.h"

using namespace std;

/**
 * @brief Processes mouse movement for camera control
 * 
 * @param x The current x-coordinate of the mouse pointer
 * @param y The current y-coordinate of the mouse pointer
 */
void process_mouse_motion(int x, int y) {
    // ignore the next motion callback after a glutWarpPointer
    static bool first_mouse = false;
    static float last_x2 = 0.0f;
    static float last_y2 = 0.0f;

    const int cx = window_width/2;
    const int cy = window_height/2;

    // while we're re‐centering, ignore every callback
    if (first_mouse) {
            // cursor is now centered, reset state
            first_mouse = false;
            last_x2 = cx;
            last_y2 = cy;
        
        return;
    }
    if(camera.mode == Camera::FPS) {
        if (x <= 0 || x >= window_width || y <= 0 || y >= window_height) {
            first_mouse = true;
            glutWarpPointer(cx, cy);
            return;
        }
        float xoffset = x - last_x2;
        float yoffset = last_y2 - y;  // invert Y
        
        xoffset *= camera.sensitivity;
        yoffset *= camera.sensitivity;
        
        camera.fps_yaw += xoffset;
        camera.fps_pitch += yoffset;
        
        // Constrain pitch to avoid screen flip
        camera.fps_pitch = camera.fps_pitch < -89.0f ? -89.0f : (camera.fps_pitch > 89.0f ? 89.0f : camera.fps_pitch);
        
        camera.update_fps_vectors();

    }
    last_x2 = x;
    last_y2 = y;
}

/**
 * @brief Processes mouse button events for camera control
 * 
 * @param button The mouse button that was pressed or released
 * @param state The state of the button (GLUT_DOWN or GLUT_UP)
 * @param x The x-coordinate where the button was pressed/released
 * @param y The y-coordinate where the button was pressed/released
 */
void process_mouse_buttons(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON) {
        mouse_dragging = (state == GLUT_DOWN);
        last_mouse_x = x;
        last_mouse_y = y;
    }
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
