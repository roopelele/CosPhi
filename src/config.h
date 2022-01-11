#ifndef CONGIG_H

// Stereo cameras have 2 images next to each other.
// Setting this to true cuts the image in half
// to only use one image.
#define USE_STEREO_CAMERA true

// Invert colors. 0 or 1 instead of true/false for C code
#define INVERT_COLORS 0

// Camera resolution
#define CAMERA_WIDTH  1280 // Pixels
#define CAMERA_HEIGHT 720  // Pixels

// Target area size for calibration
#define AREA_WIDTH  1.00 // Meters
#define AREA_LENGTH 1.00 // Meters

// Amount of markers to track
#define MARKER_COUNT 4
// Marker size
#define CIRCLE_DIAMETER 0.075 // Meters

//// GUI settings
// Default value for useGui. Can be overridden with flag --gui or --nogui
#define USE_GUI       true
#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

//// Debug settings
#define PRINT_LOCATION false
#define PRINT_TIME     true
#define PRINT_STATS    false

#define CONFIG_H
#endif