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
    printf("IsMobile: %s\n", getPropertyValueAsString(results, "IsMobile"));
    printf("Screen Width: %s\n", getPropertyValueAsString(results, "ScreenPixelsWidth"));
    printf("Screen Height: %s\n", getPropertyValueAsString(results, "ScreenPixelsHeight"));

    resultsStruct->isMobile = getPropertyValueAsString(results, "IsMobile");
    resultsStruct->screenWidth = atoi(getPropertyValueAsString(results, "ScreenPixelsWidth"));
    resultsStruct->screenHeight = atoi(getPropertyValueAsString(results, "ScreenPixelsHeight"));

    ResultsHashFree(results);
    // Free the resources used by the manager.
    ResourceManagerFree(&manager);

    return resultsStruct;
}

//Complete one function
uaInfoStructPtr detectDeviceFromUserAgent(const char *userAgent){

    printf("Detecting...\n");

    uaInfoStructPtr resultsStruct = (uaInfoStructPtr) malloc(sizeof(struct uaInfoStruct));
    if (resultsStruct == NULL) {
        printf("Malloc error\n");
        return NULL;
    }

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
    
    resultsStruct = fiftyoneDegreesHashGettingStarted(userAgent, dataFilePath, &config);
    if (resultsStruct == NULL) {
        printf("Hash Getting Started Error\n");
        return NULL;
    }
    return resultsStruct;
}