#ifndef CONGIG_H

// Stereo cameras have 2 images next to each other.
// Setting this to true cuts the image in half
// to only use one image.
#define USE_STEREO_CAMERA false

// Comment this out for normal behaviour
#define INVERT_COLORS

// Camera resolution
#define CAMERA_WIDTH  1280 // Pixels
#define CAMERA_HEIGHT 720  // Pixels

// Target area size for calibration
#define AREA_WIDTH  1.00 // Meters
#define AREA_LENGTH 1.00 // Meters

// Marker size
#define CIRCLE_DIAMETER 0.075 // Meters

// GUI settings
#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

#define CONFIG_H
#endif