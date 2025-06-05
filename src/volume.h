#ifndef VOLUME_H
#define VOLUME_H
#include <math.h>

// Constants describing the water tank geometry (in mm)
#define RADIUS_INTERIOR 1170
#define HEIGHT_TRUNCATED 880
#define HEIGHT_MAX 1520
#define SENSOR_HEIGHT 300

// Calculate the volume of water based on measured distance (in cm)
static inline float calculateVolume(int distance) {
    float h = HEIGHT_MAX + SENSOR_HEIGHT - distance * 10; // distance cm -> height mm
    if (h < 0) h = 0;
    if (h <= HEIGHT_TRUNCATED) {
        return (M_PI * h * h / 3) * (3 * RADIUS_INTERIOR - h) / 1000;
    } else {
        float h_truncated = HEIGHT_MAX - h;
        return (4.0f / 3.0f * M_PI * RADIUS_INTERIOR * RADIUS_INTERIOR * RADIUS_INTERIOR
                - M_PI * h_truncated * h_truncated / 3.0f * (3 * RADIUS_INTERIOR - h_truncated)) / 1000;
    }
}

#endif // VOLUME_H
