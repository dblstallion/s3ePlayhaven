/*
 * Internal header for the s3ePlayhaven extension.
 *
 * This file should be used for any common function definitions etc that need to
 * be shared between the platform-dependent and platform-indepdendent parts of
 * this extension.
 */

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#ifndef S3EPLAYHAVEN_INTERNAL_H
#define S3EPLAYHAVEN_INTERNAL_H

#include "s3eTypes.h"
#include "s3ePlayhaven.h"
#include "s3ePlayhaven_autodefs.h"


/**
 * Initialise the extension.  This is called once then the extension is first
 * accessed by s3eregister.  If this function returns S3E_RESULT_ERROR the
 * extension will be reported as not-existing on the device.
 */
s3eResult s3ePlayhavenInit();

/**
 * Platform-specific initialisation, implemented on each platform
 */
s3eResult s3ePlayhavenInit_platform();

/**
 * Terminate the extension.  This is called once on shutdown, but only if the
 * extension was loader and Init() was successful.
 */
void s3ePlayhavenTerminate();

/**
 * Platform-specific termination, implemented on each platform
 */
void s3ePlayhavenTerminate_platform();
s3eResult s3ePlayhavenInitWithKeys_platform(const char* token, const char* secret);

void s3ePHRegisterCallback_platform(s3ePHCallbackType callbackID, s3eCallback callbackFn, void* userData);

void s3ePHSendAppOpen_platform(const char* customUDID);

void s3ePHSendContentRequest_platform(const char* placement, bool showOverlayImmediately);

void s3ePHPreloadContentRequest_platform(const char* placement);

void s3ePHCancelAllContentRequests_platform();

void s3ePHReportResolution_platform(s3ePHPurchaseResolutionType resolution);

void s3ePHSendPublisherIAPTrackingRequest_platform(const char* product, int quantity, s3ePHPurchaseResolutionType resolution);

s3eResult s3ePHShowNotificationView_platform(int x, int y, bool useGLView, bool testing);

void s3ePHClearNotificationView_platform();

void s3ePHRefreshNotificationView_platform(bool testing);

void s3ePHSetOptOutStatus_platform(bool on);

void s3ePHSendPublisherIAPTrackingRequestWithReceipt_platform(const char* product, int quantity, s3ePHPurchaseResolutionType resolution, const void* receiptData, size_t receiptSize);


#endif /* !S3EPLAYHAVEN_INTERNAL_H */