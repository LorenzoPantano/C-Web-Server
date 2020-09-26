/**
 * User Agent Info Struct
 * Stores the results of the device detection.
*/

struct uaInfoStruct {
    int isMobile;
    int screenWidth;
    int screenHeight;
};
typedef struct uaInfoStruct *uaInfoStructPtr;