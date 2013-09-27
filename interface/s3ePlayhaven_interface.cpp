/*
 * WARNING: this is an autogenerated file and will be overwritten by
 * the extension interface script.
 */

#include "s3eExt.h"
#include "IwDebug.h"
#include "s3eDevice.h"


#include "s3ePlayhaven.h"


// For MIPs (and WP8) platform we do not have asm code for stack switching 
// implemented. So we make LoaderCallStart call manually to set GlobalLock
#if defined __mips || defined S3E_ANDROID_X86 || (defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP))
#define LOADER_CALL
#endif

/**
 * Definitions for functions types passed to/from s3eExt interface
 */
typedef  s3eResult(*s3ePlayhavenInitWithKeys_t)(const char* token, const char* secret);
typedef       void(*s3ePHRegisterCallback_t)(s3ePHCallbackType callbackID, s3eCallback callbackFn, void* userData);
typedef       void(*s3ePHSendAppOpen_t)(const char* customUDID);
typedef       void(*s3ePHSendContentRequest_t)(const char* placement, bool showOverlayImmediately);
typedef       void(*s3ePHPreloadContentRequest_t)(const char* placement);
typedef       void(*s3ePHCancelAllContentRequests_t)();
typedef       void(*s3ePHReportResolution_t)(s3ePHPurchaseResolutionType resolution);
typedef       void(*s3ePHSendPublisherIAPTrackingRequest_t)(const char* product, int quantity, s3ePHPurchaseResolutionType resolution);
typedef  s3eResult(*s3ePHShowNotificationView_t)(int x, int y, bool useGLView, bool testing);
typedef       void(*s3ePHClearNotificationView_t)();
typedef       void(*s3ePHRefreshNotificationView_t)(bool testing);
typedef       void(*s3ePHSetOptOutStatus_t)(bool on);

/**
 * struct that gets filled in by s3ePlayhavenRegister
 */
typedef struct s3ePlayhavenFuncs
{
    s3ePlayhavenInitWithKeys_t m_s3ePlayhavenInitWithKeys;
    s3ePHRegisterCallback_t m_s3ePHRegisterCallback;
    s3ePHSendAppOpen_t m_s3ePHSendAppOpen;
    s3ePHSendContentRequest_t m_s3ePHSendContentRequest;
    s3ePHPreloadContentRequest_t m_s3ePHPreloadContentRequest;
    s3ePHCancelAllContentRequests_t m_s3ePHCancelAllContentRequests;
    s3ePHReportResolution_t m_s3ePHReportResolution;
    s3ePHSendPublisherIAPTrackingRequest_t m_s3ePHSendPublisherIAPTrackingRequest;
    s3ePHShowNotificationView_t m_s3ePHShowNotificationView;
    s3ePHClearNotificationView_t m_s3ePHClearNotificationView;
    s3ePHRefreshNotificationView_t m_s3ePHRefreshNotificationView;
    s3ePHSetOptOutStatus_t m_s3ePHSetOptOutStatus;
} s3ePlayhavenFuncs;

static s3ePlayhavenFuncs g_Ext;
static bool g_GotExt = false;
static bool g_TriedExt = false;
static bool g_TriedNoMsgExt = false;

static bool _extLoad()
{
    if (!g_GotExt && !g_TriedExt)
    {
        s3eResult res = s3eExtGetHash(0x6e72b638, &g_Ext, sizeof(g_Ext));
        if (res == S3E_RESULT_SUCCESS)
            g_GotExt = true;
        else
            s3eDebugAssertShow(S3E_MESSAGE_CONTINUE_STOP_IGNORE,                 "error loading extension: s3ePlayhaven");
            
        g_TriedExt = true;
        g_TriedNoMsgExt = true;
    }

    return g_GotExt;
}

static bool _extLoadNoMsg()
{
    if (!g_GotExt && !g_TriedNoMsgExt)
    {
        s3eResult res = s3eExtGetHash(0x6e72b638, &g_Ext, sizeof(g_Ext));
        if (res == S3E_RESULT_SUCCESS)
            g_GotExt = true;
        g_TriedNoMsgExt = true;
        if (g_TriedExt)
            g_TriedExt = true;
    }

    return g_GotExt;
}

s3eBool s3ePlayhavenAvailable()
{
    _extLoadNoMsg();
    return g_GotExt ? S3E_TRUE : S3E_FALSE;
}

s3eResult s3ePlayhavenInitWithKeys(const char* token, const char* secret)
{
    IwTrace(PLAYHAVEN_VERBOSE, ("calling s3ePlayhaven[0] func: s3ePlayhavenInitWithKeys"));

    if (!_extLoad())
        return S3E_RESULT_ERROR;

#ifdef LOADER_CALL
    s3eDeviceLoaderCallStart(S3E_TRUE, NULL);
#endif

    s3eResult ret = g_Ext.m_s3ePlayhavenInitWithKeys(token, secret);

#ifdef LOADER_CALL
    s3eDeviceLoaderCallDone(S3E_TRUE, NULL);
#endif

    return ret;
}

void s3ePHRegisterCallback(s3ePHCallbackType callbackID, s3eCallback callbackFn, void* userData)
{
    IwTrace(PLAYHAVEN_VERBOSE, ("calling s3ePlayhaven[1] func: s3ePHRegisterCallback"));

    if (!_extLoad())
        return;

#ifdef LOADER_CALL
    s3eDeviceLoaderCallStart(S3E_TRUE, NULL);
#endif

    g_Ext.m_s3ePHRegisterCallback(callbackID, callbackFn, userData);

#ifdef LOADER_CALL
    s3eDeviceLoaderCallDone(S3E_TRUE, NULL);
#endif

    return;
}

void s3ePHSendAppOpen(const char* customUDID)
{
    IwTrace(PLAYHAVEN_VERBOSE, ("calling s3ePlayhaven[2] func: s3ePHSendAppOpen"));

    if (!_extLoad())
        return;

#ifdef LOADER_CALL
    s3eDeviceLoaderCallStart(S3E_TRUE, NULL);
#endif

    g_Ext.m_s3ePHSendAppOpen(customUDID);

#ifdef LOADER_CALL
    s3eDeviceLoaderCallDone(S3E_TRUE, NULL);
#endif

    return;
}

void s3ePHSendContentRequest(const char* placement, bool showOverlayImmediately)
{
    IwTrace(PLAYHAVEN_VERBOSE, ("calling s3ePlayhaven[3] func: s3ePHSendContentRequest"));

    if (!_extLoad())
        return;

#ifdef LOADER_CALL
    s3eDeviceLoaderCallStart(S3E_TRUE, NULL);
#endif

    g_Ext.m_s3ePHSendContentRequest(placement, showOverlayImmediately);

#ifdef LOADER_CALL
    s3eDeviceLoaderCallDone(S3E_TRUE, NULL);
#endif

    return;
}

void s3ePHPreloadContentRequest(const char* placement)
{
    IwTrace(PLAYHAVEN_VERBOSE, ("calling s3ePlayhaven[4] func: s3ePHPreloadContentRequest"));

    if (!_extLoad())
        return;

#ifdef LOADER_CALL
    s3eDeviceLoaderCallStart(S3E_TRUE, NULL);
#endif

    g_Ext.m_s3ePHPreloadContentRequest(placement);

#ifdef LOADER_CALL
    s3eDeviceLoaderCallDone(S3E_TRUE, NULL);
#endif

    return;
}

void s3ePHCancelAllContentRequests()
{
    IwTrace(PLAYHAVEN_VERBOSE, ("calling s3ePlayhaven[5] func: s3ePHCancelAllContentRequests"));

    if (!_extLoad())
        return;

#ifdef LOADER_CALL
    s3eDeviceLoaderCallStart(S3E_TRUE, NULL);
#endif

    g_Ext.m_s3ePHCancelAllContentRequests();

#ifdef LOADER_CALL
    s3eDeviceLoaderCallDone(S3E_TRUE, NULL);
#endif

    return;
}

void s3ePHReportResolution(s3ePHPurchaseResolutionType resolution)
{
    IwTrace(PLAYHAVEN_VERBOSE, ("calling s3ePlayhaven[6] func: s3ePHReportResolution"));

    if (!_extLoad())
        return;

#ifdef LOADER_CALL
    s3eDeviceLoaderCallStart(S3E_TRUE, NULL);
#endif

    g_Ext.m_s3ePHReportResolution(resolution);

#ifdef LOADER_CALL
    s3eDeviceLoaderCallDone(S3E_TRUE, NULL);
#endif

    return;
}

void s3ePHSendPublisherIAPTrackingRequest(const char* product, int quantity, s3ePHPurchaseResolutionType resolution)
{
    IwTrace(PLAYHAVEN_VERBOSE, ("calling s3ePlayhaven[7] func: s3ePHSendPublisherIAPTrackingRequest"));

    if (!_extLoad())
        return;

#ifdef LOADER_CALL
    s3eDeviceLoaderCallStart(S3E_TRUE, NULL);
#endif

    g_Ext.m_s3ePHSendPublisherIAPTrackingRequest(product, quantity, resolution);

#ifdef LOADER_CALL
    s3eDeviceLoaderCallDone(S3E_TRUE, NULL);
#endif

    return;
}

s3eResult s3ePHShowNotificationView(int x, int y, bool useGLView, bool testing)
{
    IwTrace(PLAYHAVEN_VERBOSE, ("calling s3ePlayhaven[8] func: s3ePHShowNotificationView"));

    if (!_extLoad())
        return S3E_RESULT_ERROR;

#ifdef LOADER_CALL
    s3eDeviceLoaderCallStart(S3E_TRUE, NULL);
#endif

    s3eResult ret = g_Ext.m_s3ePHShowNotificationView(x, y, useGLView, testing);

#ifdef LOADER_CALL
    s3eDeviceLoaderCallDone(S3E_TRUE, NULL);
#endif

    return ret;
}

void s3ePHClearNotificationView()
{
    IwTrace(PLAYHAVEN_VERBOSE, ("calling s3ePlayhaven[9] func: s3ePHClearNotificationView"));

    if (!_extLoad())
        return;

#ifdef LOADER_CALL
    s3eDeviceLoaderCallStart(S3E_TRUE, NULL);
#endif

    g_Ext.m_s3ePHClearNotificationView();

#ifdef LOADER_CALL
    s3eDeviceLoaderCallDone(S3E_TRUE, NULL);
#endif

    return;
}

void s3ePHRefreshNotificationView(bool testing)
{
    IwTrace(PLAYHAVEN_VERBOSE, ("calling s3ePlayhaven[10] func: s3ePHRefreshNotificationView"));

    if (!_extLoad())
        return;

#ifdef LOADER_CALL
    s3eDeviceLoaderCallStart(S3E_TRUE, NULL);
#endif

    g_Ext.m_s3ePHRefreshNotificationView(testing);

#ifdef LOADER_CALL
    s3eDeviceLoaderCallDone(S3E_TRUE, NULL);
#endif

    return;
}

void s3ePHSetOptOutStatus(bool on)
{
    IwTrace(PLAYHAVEN_VERBOSE, ("calling s3ePlayhaven[11] func: s3ePHSetOptOutStatus"));

    if (!_extLoad())
        return;

#ifdef LOADER_CALL
    s3eDeviceLoaderCallStart(S3E_TRUE, NULL);
#endif

    g_Ext.m_s3ePHSetOptOutStatus(on);

#ifdef LOADER_CALL
    s3eDeviceLoaderCallDone(S3E_TRUE, NULL);
#endif

    return;
}
