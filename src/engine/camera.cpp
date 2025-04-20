#include "camera.h"

/**
 * @brief Converts degrees to radians
 * 
 * @param degrees The angle in degrees
 * @return float The angle in radians
 */
float radians(float degrees) { return degrees * M_PI / 180.0f; }

/**
 * @brief Converts radians to degrees
 * 
 * @param radians The angle in radians
 * @return float The angle in degrees
 */
float degrees(float radians) { return radians * 180.0f / M_PI; }


/**
 * @brief Initializes camera parameters from XML configuration
 */
void Camera::init_from_xml(xml_parser& parser) {
    // Orbit mode initialization
    orbit_look_at = Vector3(parser.cam.lx, parser.cam.ly, parser.cam.lz);
    Vector3 initial_pos(parser.cam.px, parser.cam.py, parser.cam.pz);
    orbit_radius = initial_pos.distance(orbit_look_at);
    
    Vector3 dir = orbit_look_at - initial_pos;
    dir = dir.normalize();
    orbit_alpha = atan2(-dir.x, -dir.z);
    orbit_beta = -asin(dir.y);
    
    // FPS mode initialization
    fps_position = initial_pos;
    update_fps_vectors();
}

/**
 * @brief Updates the front, right, and up vectors for FPS mode
 */
void Camera::update_fps_vectors() {
    Vector3 front;
    front.x = cos(radians(fps_yaw)) * cos(radians(fps_pitch));
    front.y = sin(radians(fps_pitch));
    front.z = sin(radians(fps_yaw)) * cos(radians(fps_pitch));
    fps_front = front.normalize();
    
    fps_right = fps_front.cross(Vector3(0.0f, 1.0f, 0.0f)).normalize();
    fps_up = fps_right.cross(fps_front).normalize();
}

void Camera::update_cursor_mode() {
    if (mode == Camera::FPS) {
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW); 
    } else {
        glutSetCursor(GLUT_CURSOR_CYCLE); 
    }
}
 

/**
 * @brief Toggles between orbit and FPS camera modes
 */
void Camera::toggle_mode() {
    if(mode == ORBIT) {
        // Switch to FPS - maintain current position
        mode = FPS;
    } else {
        // Switch to Orbit - update orbit parameters based on FPS position
        mode = ORBIT;
        orbit_radius = fps_position.distance(orbit_look_at);
        Vector3 dir = orbit_look_at - fps_position;
        dir = dir.normalize();
        orbit_alpha = atan2(-dir.x, -dir.z);
        orbit_beta = -asin(dir.y);
    }
    update_cursor_mode();
}

// This function is hell on earth, but it works, so let's not touch it
void Camera::follow_target(const Vector3& new_target_position) {

    Vector3 old_target = orbit_look_at;
    float   old_radius = orbit_radius;
    Vector3 current_pos = get_orbit_camera_position();

    orbit_look_at = old_target + (new_target_position - old_target);// * smoothFactor;

    Vector3 desired_pos = get_orbit_camera_position();

    Vector3 smoothed_pos = current_pos + (desired_pos - current_pos);// * smoothFactor;

    Vector3 dir = (orbit_look_at - smoothed_pos).normalize();
    //orbit_radius = old_radius;               // ← preserve constant radius
    bool old_alpha = (orbit_alpha > 0.0f);
    orbit_alpha  = atan2(-dir.x, -dir.z);
    if (old_alpha != (orbit_alpha > 0.0f)) {
        orbit_alpha = -orbit_alpha;
    }
    bool old_beta = (orbit_beta > 0.0f);
    orbit_beta   = -asin(dir.y);
    if (old_beta != (orbit_beta > 0.0f)) {
        orbit_beta = -orbit_beta;
    }

}

Vector3 Camera::get_orbit_camera_position() const {
    float x = orbit_look_at.x + orbit_radius * cos(orbit_beta) * sin(orbit_alpha);
    float y = orbit_look_at.y + orbit_radius * sin(orbit_beta);
    float z = orbit_look_at.z + orbit_radius * cos(orbit_beta) * cos(orbit_alpha);
    return Vector3(x, y, z);
}

