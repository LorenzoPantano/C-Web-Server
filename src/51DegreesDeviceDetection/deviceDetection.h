#ifdef _DEBUG
#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include "dmalloc.h"
#endif
#endif

#include <stdio.h>
#include "./src/hash/hash.h"
#include "./src/hash/fiftyone.h"

// Variables
static const char *dataDir = "../src/51DegreesDeviceDetection/device-detection-data";
static const char *dataFileName = "51Degrees-LiteV4.1.hash";
static char valueBuffer[1024] = "";

int detectDeviceFromUserAgent(const char *userAgent);