/*
Generic implementation of the s3ePlayhaven extension.
This file should perform any platform-indepedentent functionality
(e.g. error checking) before calling platform-dependent implementations.
*/

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#include "s3ePlayhaven_internal.h"
s3eResult s3ePlayhavenInit()
{
    //Add any generic initialisation code here
    return s3ePlayhavenInit_platform();
}

void s3ePlayhavenTerminate()
{
    //Add any generic termination code here
    s3ePlayhavenTerminate_platform();
}

s3eResult s3ePlayhavenInitWithKeys(const char* token, const char* secret)
{
	return s3ePlayhavenInitWithKeys_platform(token, secret);
}

void s3ePHRegisterCallback(s3ePHCallbackType callbackID, s3eCallback callbackFn, void* userData)
{
	s3ePHRegisterCallback_platform(callbackID, callbackFn, userData);
}

void s3ePHSendAppOpen(const char* customUDID)
{
	s3ePHSendAppOpen_platform(customUDID);
}

void s3ePHSendContentRequest(const char* placement, bool showOverlayImmediately)
{
	s3ePHSendContentRequest_platform(placement, showOverlayImmediately);
}

void s3ePHPreloadContentRequest(const char* placement)
{
	s3ePHPreloadContentRequest_platform(placement);
}

void s3ePHCancelAllContentRequests()
{
	s3ePHCancelAllContentRequests_platform();
}

void s3ePHReportResolution(s3ePHPurchaseResolutionType resolution)
{
	s3ePHReportResolution_platform(resolution);
}

void s3ePHSendPublisherIAPTrackingRequest(const char* product, int quantity, s3ePHPurchaseResolutionType resolution)
{
	s3ePHSendPublisherIAPTrackingRequest_platform(product, quantity, resolution);
}

s3eResult s3ePHShowNotificationView(int x, int y, bool useGLView, bool testing)
{
	return s3ePHShowNotificationView_platform(x, y, useGLView, testing);
}

void s3ePHClearNotificationView()
{
	s3ePHClearNotificationView_platform();
}

void s3ePHRefreshNotificationView(bool testing)
{
	s3ePHRefreshNotificationView_platform(testing);
}

void s3ePHSetOptOutStatus(bool on)
{
	s3ePHSetOptOutStatus_platform(on);
}
