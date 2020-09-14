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
void fiftyoneDegreesHashGettingStarted(const char *userAgent, const char *dataFilePath, ConfigHash *config) {

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

    /**
     * TODO: The user-Agent string has to be passed 
    */

    //My code here:
    //const char* userAgent is passed as parameter
    ResultsHashFromUserAgent(results, userAgent, strlen(userAgent), exception);
    if (EXCEPTION_FAILED) {
        printf("%s\n", ExceptionGetMessage(exception));
    }
    printf("IsMobile: %s\n", getPropertyValueAsString(results, "IsMobile"));
    printf("Screen Width: %s\n", getPropertyValueAsString(results, "ScreenPixelsWidth"));
    printf("Screen Height: %s\n", getPropertyValueAsString(results, "ScreenPixelsHeight"));

    /*
    // User-Agent string of an iPhone mobile device.
    const char* mobileUserAgent = (
        "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) "
        "AppleWebKit/537.51.2 (KHTML, like Gecko) Version/7.0 Mobile/11D167 "
        "Safari/9537.53");
    // User-Agent string of Firefox Web browser version 41 on desktop.
    const char* desktopUserAgent = (
        "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) "
        "Gecko/20100101 Firefox/41.0");
    // User-Agent string of a MediaHub device.
    const char* mediaHubUserAgent = (
        "Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core "
        "Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 "
        "Chrome/30.0.0.0 Safari/537.36");
    printf("Starting Getting Started Example.\n");
    
    // Carries out a match for a mobile User-Agent.
    printf("\nMobile User-Agent: %s\n", mobileUserAgent);
    ResultsHashFromUserAgent(
        results,
        mobileUserAgent,
        strlen(mobileUserAgent),
        exception);
    if (EXCEPTION_FAILED) {
        printf("%s\n", ExceptionGetMessage(exception));
    }
    printf("   IsMobile: %s\n",
        getPropertyValueAsString(results, "IsMobile"));
    // Carries out a match for a desktop User-Agent.
    printf("\nDesktop User-Agent: %s\n", desktopUserAgent);
    ResultsHashFromUserAgent(
        results,
        desktopUserAgent,
        strlen(desktopUserAgent),
        exception);
    if (EXCEPTION_FAILED) {
        printf("%s\n", ExceptionGetMessage(exception));
    }
    printf("   IsMobile: %s\n",
        getPropertyValueAsString(results, "IsMobile"));
    // Carries out a match for a MediaHub User-Agent.
    printf("\nMedia hub User-Agent: %s\n", mediaHubUserAgent);
    ResultsHashFromUserAgent(
        results,
        mediaHubUserAgent,
        strlen(mediaHubUserAgent),
        exception);
    if (EXCEPTION_FAILED) {
        printf("%s\n", ExceptionGetMessage(exception));
    }
    printf("   IsMobile: %s\n",
        getPropertyValueAsString(results, "IsMobile"));
    */
    // Ensure the results are freed to avoid memory leaks.
    ResultsHashFree(results);
    // Free the resources used by the manager.
    ResourceManagerFree(&manager);
}

//Complete one function
int detectDeviceFromUserAgent(const char *userAgent){
    StatusCode status = SUCCESS;
    ConfigHash config = HashDefaultConfig;
    char dataFilePath[FILE_MAX_PATH];
    status = FileGetPath(dataDir, dataFileName, dataFilePath, sizeof(dataFilePath));
    //Error
    if (status != SUCCESS) {
        reportStatus(status, dataFileName);
        fgetc(stdin);
        return 1;
    }
    if (CollectionGetIsMemoryOnly()) {
        config = HashInMemoryConfig;
    }
#ifdef _DEBUG
#ifndef _MSC_VER
    dmalloc_debug_setup("log-stats,log-non-free,check-fence,log=dmalloc.log");
#endif
#endif
    
    fiftyoneDegreesHashGettingStarted(userAgent, dataFilePath, &config);
#ifdef _DEBUG
#ifdef _MSC_VER
    _CrtDumpMemoryLeaks();
#else
    printf("Log file is %s\r\n", dmalloc_logpath);
#endif
#endif
    // Wait for a character to be pressed.
    fgetc(stdin);
    return 0;
}

/*

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


static const char *dataDir = "./device-detection-data";

static const char *dataFileName = "51Degrees-LiteV4.1.hash";

static char valueBuffer[1024] = "";

static const char* getPropertyValueAsString(
	ResultsHash *results,
	const char *propertyName) {
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

/**
 * Reports the status of the data file initialization.

static void reportStatus(StatusCode status,
	const char* fileName) {
	const char *message = StatusGetMessage(status, fileName);
	printf("%s\n", message);
	Free((void*)message);
}

void fiftyoneDegreesHashGettingStarted(
	const char *dataFilePath,
	ConfigHash *config) {
	ResourceManager manager;
	EXCEPTION_CREATE;

	// Set the properties to be returned for each User-Agent.
	PropertiesRequired properties = PropertiesDefault;
	properties.string = "ScreenPixelsWidth,HardwareModel,IsMobile,BrowserName";

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

	// User-Agent string of an iPhone mobile device.
	const char* mobileUserAgent = (
		"Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) "
		"AppleWebKit/537.51.2 (KHTML, like Gecko) Version/7.0 Mobile/11D167 "
		"Safari/9537.53");

	// User-Agent string of Firefox Web browser version 41 on desktop.
	const char* desktopUserAgent = (
		"Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) "
		"Gecko/20100101 Firefox/41.0");

	// User-Agent string of a MediaHub device.
	const char* mediaHubUserAgent = (
		"Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core "
		"Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 "
		"Chrome/30.0.0.0 Safari/537.36");

	printf("Starting Getting Started Example.\n");
	
	// Carries out a match for a mobile User-Agent.
	printf("\nMobile User-Agent: %s\n", mobileUserAgent);
	ResultsHashFromUserAgent(
		results,
		mobileUserAgent,
		strlen(mobileUserAgent),
		exception);
	if (EXCEPTION_FAILED) {
		printf("%s\n", ExceptionGetMessage(exception));
	}
	printf("   IsMobile: %s\n",
		getPropertyValueAsString(results, "IsMobile"));

	// Carries out a match for a desktop User-Agent.
	printf("\nDesktop User-Agent: %s\n", desktopUserAgent);
	ResultsHashFromUserAgent(
		results,
		desktopUserAgent,
		strlen(desktopUserAgent),
		exception);
	if (EXCEPTION_FAILED) {
		printf("%s\n", ExceptionGetMessage(exception));
	}
	printf("   IsMobile: %s\n",
		getPropertyValueAsString(results, "IsMobile"));

	// Carries out a match for a MediaHub User-Agent.
	printf("\nMedia hub User-Agent: %s\n", mediaHubUserAgent);
	ResultsHashFromUserAgent(
		results,
		mediaHubUserAgent,
		strlen(mediaHubUserAgent),
		exception);
	if (EXCEPTION_FAILED) {
		printf("%s\n", ExceptionGetMessage(exception));
	}
	printf("   IsMobile: %s\n",
		getPropertyValueAsString(results, "IsMobile"));

	// Ensure the results are freed to avoid memory leaks.
	ResultsHashFree(results);

	// Free the resources used by the manager.
	ResourceManagerFree(&manager);
}

#ifndef TEST


//main

StatusCode status = SUCCESS;
	ConfigHash config = HashDefaultConfig;
	char dataFilePath[FILE_MAX_PATH];
	if (argc > 1) {
		strcpy(dataFilePath, argv[1]);
	}
	else {
		status = FileGetPath(
			dataDir,
			dataFileName,
			dataFilePath,
			sizeof(dataFilePath));
	}
	if (status != SUCCESS) {
		reportStatus(status, dataFileName);
		fgetc(stdin);
		return 1;
	}
	if (CollectionGetIsMemoryOnly()) {
		config = HashInMemoryConfig;
	}

#ifdef _DEBUG
#ifndef _MSC_VER
	dmalloc_debug_setup("log-stats,log-non-free,check-fence,log=dmalloc.log");
#endif
#endif
	
	fiftyoneDegreesHashGettingStarted(
		dataFilePath,
		&config);

#ifdef _DEBUG
#ifdef _MSC_VER
	_CrtDumpMemoryLeaks();
#else
	printf("Log file is %s\r\n", dmalloc_logpath);
#endif
#endif

	// Wait for a character to be pressed.
	fgetc(stdin);

#endif
*/