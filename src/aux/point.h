#ifndef POINT_H
#define POINT_H

#include <cmath>

/**
 * @brief Vector3 class for 3D point and vector operations
 */
class Vector3 {
    public:
        float x, y, z;
    
        /**
         * @brief Constructor for Vector3
         * 
         * @param _x The x coordinate (default 0.0f)
         * @param _y The y coordinate (default 0.0f)
         * @param _z The z coordinate (default 0.0f)
         */
        Vector3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}
    
        /**
         * @brief Vector addition operator
         * 
         * @param v The vector to add
         * @return Vector3 A new vector representing the sum
         */
        Vector3 operator+(const Vector3& v) const { 
            return Vector3(x + v.x, y + v.y, z + v.z); 
        }
        
        /**
         * @brief Vector subtraction operator
         * 
         * @param v The vector to subtract
         * @return Vector3 A new vector representing the difference
         */
        Vector3 operator-(const Vector3& v) const { 
            return Vector3(x - v.x, y - v.y, z - v.z); 
        }
        
        /**
         * @brief Scalar multiplication operator
         * 
         * @param s The scalar value to multiply by
         * @return Vector3 A new scaled vector
         */
        Vector3 operator*(float s) const { 
            return Vector3(x * s, y * s, z * s); 
        }
        
        /**
         * @brief Calculates the length of the vector
         * 
         * @return float The vector's magnitude
         */
        float length() const { 
            return sqrt(x * x + y * y + z * z); 
        }
        
        /**
         * @brief Calculates the distance between this vector and another
         * 
         * @param v The other vector
         * @return float The Euclidean distance between vectors
         */
        float distance(const Vector3& v) const {
            float dx = x - v.x;
            float dy = y - v.y;
            float dz = z - v.z;
            return sqrt(dx * dx + dy * dy + dz * dz);
        }
        
        /**
         * @brief Normalizes the vector to unit length
         * 
         * @return Vector3 A new unit vector in the same direction
         */
        Vector3 normalize() const {
            float len = length();
            if (len < 0.00001f) return Vector3();
            return Vector3(x / len, y / len, z / len);
        }
        
        /**
         * @brief Calculates the cross product with another vector
         * 
         * @param v The other vector
         * @return Vector3 The cross product vector
         */
        Vector3 cross(const Vector3& v) const {
            return Vector3(
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x
            );
        }
        
        /**
         * @brief Calculates the dot product with another vector
         * 
         * @param v The other vector
         * @return float The dot product value
         */
        float dot(const Vector3& v) const {
            return x * v.x + y * v.y + z * v.z;
        }
    };

#endif

