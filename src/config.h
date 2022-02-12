#ifndef CONGIG_H

// Define TEST_PERFORMANCE to measure performance
//#define TEST_PERFORMANCE

//// Image processing settings
// Stereo cameras have 2 images next to each other.
// Setting this to true cuts the image in half to only use one image.
#define USE_STEREO_CAMERA false

// Use inverted colors for processing.
#define INVERT_COLORS true

// Camera resolution
#define CAMERA_WIDTH  1920 // Pixels
#define CAMERA_HEIGHT 1080 // Pixels

// Target area size for calibration
#define AREA_WIDTH  1.00 // Meters
#define AREA_LENGTH 1.00 // Meters

// Amount of markers to track
#define MARKER_COUNT 4
#define MAX_PATTERNS 12
// Marker size
#define CIRCLE_DIAMETER 0.075 // Meters

//// GUI settings
// Default value for useGui. Can be overridden with flag --gui or --nogui
#define USE_GUI       true
#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

#define DEBUG_LEVEL   0
#define DRAW_SEGMENTS false

//// Debug settings
#ifndef TEST_PERFORMANCE

#define PRINT_LOCATION   false
#define PRINT_TIME_EVAL  false
#define PRINT_TIME_TOTAL true
#define PRINT_STATS      false

#else

#define PRINT_LOCATION   false
#define PRINT_TIME_EVAL  false
#define PRINT_TIME_TOTAL false
#define PRINT_STATS      false

#endif

#define CONFIG_H
#endif