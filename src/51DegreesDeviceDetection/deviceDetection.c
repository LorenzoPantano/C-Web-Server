#include "deviceDetection.h"


/**
 * Returns the property value (isMobile, ScreenSizes ecc.)
 * in String format.
 * @param results A ResultsHash where to take the results
 * @param propertyName The property to be converted
 * @return String rapresentation of the property
*/
static const char* getPropertyValueAsString(ResultsHash *results, const char *propertyName) {
    EXCEPTION_CREATE;
    ResultsHashGetValuesString(
        results,
        propertyName,
        valueBuffer,
        sizeof(valueBuffer),
        ",",
        exception);
    EXCEPTION_THROW;
    return valueBuffer;
}

// Report Status
static void reportStatus(StatusCode status, const char* fileName) {
    const char *message = StatusGetMessage(status, fileName);
    printf("%s\n", message);
    Free((void*)message);
}

/**
 * HashGettingStarted
 * 
*/
uaInfoStructPtr fiftyoneDegreesHashGettingStarted(const char *userAgent, const char *dataFilePath, ConfigHash *config) {

    uaInfoStructPtr resultsStruct = (uaInfoStructPtr) malloc(sizeof(struct uaInfoStruct));
    if (resultsStruct == NULL) {
        printf("Error allocation uaInfoStruct\n");
        return NULL;
    }

    ResourceManager manager;
    EXCEPTION_CREATE;
    // Set the properties to be returned for each User-Agent.
    PropertiesRequired properties = PropertiesDefault;
    properties.string = "ScreenPixelsWidth,ScreenPixelsHeight,IsMobile,BrowserName";
    // Initialise the manager for device detection.
    StatusCode status = HashInitManagerFromFile(
        &manager,
        config,
        &properties,
        dataFilePath,
        exception);
    EXCEPTION_THROW;
    if (status != SUCCESS) {
        reportStatus(status, dataFilePath);
        fgetc(stdin);
        return;
    }
    // Create a results instance to store and process User-Agents.
    ResultsHash *results = ResultsHashCreate(&manager, 1, 0);

    ResultsHashFromUserAgent(results, userAgent, strlen(userAgent), exception);
    if (EXCEPTION_FAILED) {
        printf("%s\n", ExceptionGetMessage(exception));
    }
    if (strcmp(getPropertyValueAsString(results, "IsMobile"),"True") == 0) {
        resultsStruct->isMobile = 1;
    } else {
        resultsStruct->isMobile = 0;
    }
    resultsStruct->screenWidth = atoi(getPropertyValueAsString(results, "ScreenPixelsWidth"));
    resultsStruct->screenHeight = atoi(getPropertyValueAsString(results, "ScreenPixelsHeight")); 

    ResultsHashFree(results);
    // Free the resources used by the manager.
    ResourceManagerFree(&manager);
    return resultsStruct;
}

/**
 * Detect Device From User-Agent
 * @param userAgent the user-agent string to be analized
 * @return A pointer to a uaInfoStruct which will contain the results of the 
 * detection, such as:
 * IsMobile: (char *) Tells if the device is a mobile device
 * SceenWidth: (int) The Screen Width of the device
 * ScreenHeight: (int) The Screen Height of the device
*/
uaInfoStructPtr detectDeviceFromUserAgent(const char *userAgent){

    printf("Detecting...\n");

    StatusCode status = SUCCESS;
    ConfigHash config = HashDefaultConfig;
    char dataFilePath[FILE_MAX_PATH];
    status = FileGetPath(dataDir, dataFileName, dataFilePath, sizeof(dataFilePath));
    //Error
    if (status != SUCCESS) {
        reportStatus(status, dataFileName);
        fgetc(stdin);
        return NULL;
    }
    if (CollectionGetIsMemoryOnly()) {
        config = HashInMemoryConfig;
    }
#ifdef _DEBUG
#ifndef _MSC_VER
    dmalloc_debug_setup("log-stats,log-non-free,check-fence,log=dmalloc.log");
#endif
#endif
    return fiftyoneDegreesHashGettingStarted(userAgent, dataFilePath, &config);
}