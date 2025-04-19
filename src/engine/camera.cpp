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
/*
void Camera::follow_target(const Vector3& new_target_position) {
    const float smoothFactor = 0.1f; // in [0,1], lower → more smoothing

    // 1) grab the old target & camera pos
    Vector3 old_target = orbit_look_at;
    Vector3 current_pos = get_orbit_camera_position();

    // 2) smooth the look‐at point
    orbit_look_at = old_target + (new_target_position - old_target) * smoothFactor;

    // 3) compute the desired orbit position (now using the smoothed look‐at)
    Vector3 desired_pos = get_orbit_camera_position();

    // 4) smooth the camera’s world‐position
    Vector3 smoothed_pos = current_pos + (desired_pos - current_pos) * smoothFactor;

    // 5) recompute spherical coords around smoothed_pos & smoothed look‐at
    Vector3 dir = (orbit_look_at - smoothed_pos).normalize();
    orbit_radius = (orbit_look_at - smoothed_pos).length();
    orbit_alpha  = atan2(-dir.x, -dir.z);
    orbit_beta   = -asin(dir.y);
}
*/
void Camera::follow_target(const Vector3& new_target_position) {
    const float smoothFactor = 0.1f; // [0,1], lower → more smoothing

    // 1) remember old look‐at, radius & camera pos
    Vector3 old_target = orbit_look_at;
    float   old_radius = orbit_radius;
    Vector3 current_pos = get_orbit_camera_position();

    // 2) smooth the look‐at
    orbit_look_at = old_target + (new_target_position - old_target) * smoothFactor;

    // 3) compute target cam pos (still using old_radius)
    Vector3 desired_pos = get_orbit_camera_position();

    // 4) smooth the world‑pos
    Vector3 smoothed_pos = current_pos + (desired_pos - current_pos) * smoothFactor;

    // 5) recompute only the angles around smoothed_pos & look‑at
    Vector3 dir = (orbit_look_at - smoothed_pos).normalize();
    orbit_radius = old_radius;               // ← preserve constant radius
    orbit_alpha  = atan2(-dir.x, -dir.z);
    orbit_beta   = -asin(dir.y);
}
/*
void Camera::follow_target(const Vector3& new_target_position) {
    const float f   = 0.1f;    // smoothing factor for angles
    const float eps = 0.001f;  // snap threshold to kill tiny jitter

    // 1) always point exactly at the target
    orbit_look_at = new_target_position;

    // 2) compute ideal angles (radius stays constant)
    Vector3 camPos   = get_orbit_camera_position();
    Vector3 idealDir = (orbit_look_at - camPos).normalize();
    float targetAlpha = atan2(-idealDir.x, -idealDir.z);
    float targetBeta  = -asin(idealDir.y);

    // 3) damp α/β along the shortest path, but snap very small deltas
    auto shortest = [](float to, float from) {
        float d = fmodf(to - from + M_PI, 2.0f*M_PI) - M_PI;
        return d;
    };
    float dA = shortest(targetAlpha, orbit_alpha);
    float dB = shortest(targetBeta,  orbit_beta);

    orbit_alpha += (fabs(dA) < eps ? dA : dA * f);
    orbit_beta  += (fabs(dB) < eps ? dB : dB * f);
}
*/
Vector3 Camera::get_orbit_camera_position() const {
    float x = orbit_look_at.x + orbit_radius * cos(orbit_beta) * sin(orbit_alpha);
    float y = orbit_look_at.y + orbit_radius * sin(orbit_beta);
    float z = orbit_look_at.z + orbit_radius * cos(orbit_beta) * cos(orbit_alpha);
    return Vector3(x, y, z);
}

