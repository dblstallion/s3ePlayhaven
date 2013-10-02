/*
 * android-specific implementation of the s3ePlayhaven extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "s3ePlayhaven_internal.h"

#include "s3eEdk.h"
#include "s3eEdk_android.h"
#include <jni.h>
#include "IwDebug.h"

static jobject g_Obj;
static jmethodID g_s3ePlayhavenInitWithKeys;
static jmethodID g_s3ePHRegisterCallback;
static jmethodID g_s3ePHSendAppOpen;
static jmethodID g_s3ePHSendContentRequest;
static jmethodID g_s3ePHPreloadContentRequest;
static jmethodID g_s3ePHCancelAllContentRequests;
static jmethodID g_s3ePHReportResolution;
static jmethodID g_s3ePHSendPublisherIAPTrackingRequest;
static jmethodID g_s3ePHShowNotificationView;
static jmethodID g_s3ePHClearNotificationView;
static jmethodID g_s3ePHRefreshNotificationView;
static jmethodID g_s3ePHSetOptOutStatus;

///////////////////////////////////////////////////////////////////////////////

void JNICALL s3ePH_contentStartCallback( JNIEnv* env, jobject obj, jobject param );
void JNICALL s3ePH_contentReceivedCallback( JNIEnv* env, jobject obj, jobject param );
void JNICALL s3ePH_contentWillDisplayCallback( JNIEnv* env, jobject obj, jobject param );
void JNICALL s3ePH_contentDidDisplayCallback( JNIEnv* env, jobject obj, jobject param );
void JNICALL s3ePH_contentDismissingCallback( JNIEnv* env, jobject obj, jobject param );
void JNICALL s3ePH_contentFailedCallback( JNIEnv* env, jobject obj, jobject param );
void JNICALL s3ePH_contentUnlockRewardCallback( JNIEnv* env, jobject obj, jobject param );
void JNICALL s3ePH_contentMakePurchaseCallback( JNIEnv* env, jobject obj, jobject param );

///////////////////////////////////////////////////////////////////////////////

s3eResult s3ePlayhavenInit_platform()
{
    // Get the environment from the pointer
    JNIEnv* env = s3eEdkJNIGetEnv();
    jobject obj = NULL;
    jmethodID cons = NULL;

    // Get the extension class
    jclass cls = s3eEdkAndroidFindClass("s3ePlayhaven");
    if (!cls)
        goto fail;

    // Get its constructor
    cons = env->GetMethodID(cls, "<init>", "()V");
    if (!cons)
        goto fail;

    // Construct the java class
    obj = env->NewObject(cls, cons);
    if (!obj)
        goto fail;

    // Get all the extension methods
    g_s3ePlayhavenInitWithKeys = env->GetMethodID(cls, "s3ePlayhavenInitWithKeys", "(Ljava/lang/String;Ljava/lang/String;)I");
    if (!g_s3ePlayhavenInitWithKeys)
        goto fail;

    g_s3ePHRegisterCallback = env->GetMethodID(cls, "s3ePHRegisterCallback", "()V");
    if (!g_s3ePHRegisterCallback)
        goto fail;

    g_s3ePHSendAppOpen = env->GetMethodID(cls, "s3ePHSendAppOpen", "(Ljava/lang/String;)V");
    if (!g_s3ePHSendAppOpen)
        goto fail;

    g_s3ePHSendContentRequest = env->GetMethodID(cls, "s3ePHSendContentRequest", "(Ljava/lang/String;Z)V");
    if (!g_s3ePHSendContentRequest)
        goto fail;

    g_s3ePHPreloadContentRequest = env->GetMethodID(cls, "s3ePHPreloadContentRequest", "(Ljava/lang/String;)V");
    if (!g_s3ePHPreloadContentRequest)
        goto fail;

    g_s3ePHCancelAllContentRequests = env->GetMethodID(cls, "s3ePHCancelAllContentRequests", "()V");
    if (!g_s3ePHCancelAllContentRequests)
        goto fail;

    g_s3ePHReportResolution = env->GetMethodID(cls, "s3ePHReportResolution", "(I)V");
    if (!g_s3ePHReportResolution)
        goto fail;

    g_s3ePHSendPublisherIAPTrackingRequest = env->GetMethodID(cls, "s3ePHSendPublisherIAPTrackingRequest", "(Ljava/lang/String;II)V");
    if (!g_s3ePHSendPublisherIAPTrackingRequest)
        goto fail;

    g_s3ePHShowNotificationView = env->GetMethodID(cls, "s3ePHShowNotificationView", "(IIZZ)I");
    if (!g_s3ePHShowNotificationView)
        goto fail;

    g_s3ePHClearNotificationView = env->GetMethodID(cls, "s3ePHClearNotificationView", "()V");
    if (!g_s3ePHClearNotificationView)
        goto fail;

    g_s3ePHRefreshNotificationView = env->GetMethodID(cls, "s3ePHRefreshNotificationView", "(Z)V");
    if (!g_s3ePHRefreshNotificationView)
        goto fail;

    g_s3ePHSetOptOutStatus = env->GetMethodID(cls, "s3ePHSetOptOutStatus", "(Z)V");
    if (!g_s3ePHSetOptOutStatus)
        goto fail;
	
	{
		jclass receiver=s3eEdkAndroidFindClass( "s3ePlayhavenFacade" );

		static const JNINativeMethod methods[]=
		{
			{ "native_contentStart",			"(Ljava/lang/String;)V",					(void*)&s3ePH_contentStartCallback },
			{ "native_contentReceived",			"(Ljava/lang/String;)V",					(void*)&s3ePH_contentReceivedCallback },
			{ "native_contentWillDisplay",		"(Ls3ePlayhavenFacade$PublisherContent;)V",	(void*)&s3ePH_contentWillDisplayCallback },
			{ "native_contentDidDisplay",		"(Ls3ePlayhavenFacade$PublisherContent;)V",	(void*)&s3ePH_contentDidDisplayCallback },
			{ "native_contentDismissing",		"(Ls3ePlayhavenFacade$PublisherContent;)V",	(void*)&s3ePH_contentDismissingCallback },
			{ "native_contentFailed",			"(Ljava/lang/String;)V",					(void*)&s3ePH_contentFailedCallback },
			{ "native_contentUnlockReward",		"(Ls3ePlayhavenFacade$Reward;)V",			(void*)&s3ePH_contentUnlockRewardCallback },
			{ "native_contentMakePurchase",		"(Ls3ePlayhavenFacade$Purchase;)V",			(void*)&s3ePH_contentMakePurchaseCallback }
		};
		
		// Register the native hooks
		if(env->RegisterNatives( receiver, methods, sizeof(methods)/sizeof(methods[0])) )
			goto fail;
			
		// Clean up
		env->DeleteGlobalRef(receiver);
		
	}

    IwTrace(PLAYHAVEN, ("PLAYHAVEN init success"));
    g_Obj = env->NewGlobalRef(obj);
    env->DeleteLocalRef(obj);
    env->DeleteGlobalRef(cls);

    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;

fail:
    jthrowable exc = env->ExceptionOccurred();
    if (exc)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        IwTrace(s3ePlayhaven, ("One or more java methods could not be found"));
    }
    return S3E_RESULT_ERROR;

}

void s3ePlayhavenTerminate_platform()
{
    // Add any platform-specific termination code here
}

s3eResult s3ePlayhavenInitWithKeys_platform(const char* token, const char* secret)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring token_jstr = env->NewStringUTF(token);
    jstring secret_jstr = env->NewStringUTF(secret);
    return (s3eResult)env->CallIntMethod(g_Obj, g_s3ePlayhavenInitWithKeys, token_jstr, secret_jstr);
}

void s3ePHRegisterCallback_platform(s3ePHCallbackType callbackID, s3eCallback callbackFn, void* userData)
{
	//register the callback here rather than calling the (empty) java version

	s3eEdkCallbacksRegister(
		S3E_EXT_PLAYHAVEN_HASH,
		S3EPH_MAX_ID,
		callbackID,
		callbackFn,
		userData,
		S3E_FALSE);

    //JNIEnv* env = s3eEdkJNIGetEnv();
    //env->CallVoidMethod(g_Obj, g_s3ePHRegisterCallback);
}

void s3ePHSendAppOpen_platform(const char* customUDID)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring customUDID_jstr = env->NewStringUTF(customUDID);
    env->CallVoidMethod(g_Obj, g_s3ePHSendAppOpen, customUDID_jstr);
}

void s3ePHSendContentRequest_platform(const char* placement, bool showOverlayImmediately)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring placement_jstr = env->NewStringUTF(placement);
    env->CallVoidMethod(g_Obj, g_s3ePHSendContentRequest, placement_jstr, showOverlayImmediately);
}

void s3ePHPreloadContentRequest_platform(const char* placement)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring placement_jstr = env->NewStringUTF(placement);
    env->CallVoidMethod(g_Obj, g_s3ePHPreloadContentRequest, placement_jstr);
}

void s3ePHCancelAllContentRequests_platform()
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    env->CallVoidMethod(g_Obj, g_s3ePHCancelAllContentRequests);
}

void s3ePHReportResolution_platform(s3ePHPurchaseResolutionType resolution)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    env->CallVoidMethod(g_Obj, g_s3ePHReportResolution,(int) resolution);
}

void s3ePHSendPublisherIAPTrackingRequest_platform(const char* product, int quantity, s3ePHPurchaseResolutionType resolution)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring product_jstr = env->NewStringUTF(product);
    env->CallVoidMethod(g_Obj, g_s3ePHSendPublisherIAPTrackingRequest, product_jstr, quantity,(int) resolution);
}

void s3ePHSendPublisherIAPTrackingRequestWithReceipt_platform(const char* product, int quantity, s3ePHPurchaseResolutionType resolution, const void* receiptData, size_t receiptSize)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring product_jstr = env->NewStringUTF(product);
    env->CallVoidMethod(g_Obj, g_s3ePHSendPublisherIAPTrackingRequest, product_jstr, quantity,(int) resolution);
}

s3eResult s3ePHShowNotificationView_platform(int x, int y, bool useGLView, bool testing)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    return (s3eResult)env->CallIntMethod(g_Obj, g_s3ePHShowNotificationView, x, y, useGLView, testing);
}

void s3ePHClearNotificationView_platform()
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    env->CallVoidMethod(g_Obj, g_s3ePHClearNotificationView);
}

void s3ePHRefreshNotificationView_platform(bool testing)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    env->CallVoidMethod(g_Obj, g_s3ePHRefreshNotificationView, testing);
}

void s3ePHSetOptOutStatus_platform(bool on)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    env->CallVoidMethod(g_Obj, g_s3ePHSetOptOutStatus, on);
}


//CALLBACKS

static char* jstringToChar( JNIEnv* env, jstring str )
{
	if( str )
	{
		jsize len = env->GetStringLength( str );

		const char* utf=env->GetStringUTFChars( str, NULL );
		char* retval = new char[ len + 1 ];
		memcpy( retval, utf, len + 1 );
		env->ReleaseStringUTFChars( str, utf );
		
		return retval;
	}

	return NULL;
}

char *StringFieldAccessor( JNIEnv* env, jclass cls, const char *szFieldName, jobject obj )
{
	jfieldID field = env->GetFieldID( cls, szFieldName, "Ljava/lang/String;" );
	
	if (!env->ExceptionOccurred())
	{
		if ( field )
		{
			jstring str = (jstring) env->GetObjectField( obj, field );
					
			if( str )
			{
				return jstringToChar( env, str );
			}
		}
	}
	
	if (env->ExceptionOccurred())
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
			
		IwTrace(PLAYHAVEN, ("One or more java fields could not be found"));
	}

	return NULL;
}

int IntFieldAccessor( JNIEnv* env, jclass cls, const char *szFieldName, jobject obj )
{
	jfieldID field = env->GetFieldID( cls, szFieldName, "I" );
	
	if (!env->ExceptionOccurred())
	{
		if ( field )
		{
			return (int) env->GetIntField( obj, field );
		}
	}
	
	if (env->ExceptionOccurred())
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
			
		IwTrace(PLAYHAVEN, ("One or more java fields could not be found"));
	}

	return 0;
}

// Post callback cleanup

static void s3ePH_ReleaseStringCallback(uint32, int32, void*, void*, int32, void* pData )
{
	s3eDebugOutputString( "s3ePH_ReleaseStringCallback" );
	
	const char * pItemData = (const char*) pData;
	
	if ( pItemData != NULL)	
		delete[] pItemData;
}

static void s3ePH_ReleasePublisherContentCallback(uint32, int32, void*, void*, int32, void* pData )
{
	s3eDebugOutputString( "s3ePH_ReleasePublisherContentCallback" );
	
	s3ePHPublisherContent* pItemData = (s3ePHPublisherContent*) pData;
	
	if ( pItemData->placement != NULL)	
		delete[] pItemData->placement;

	if ( pItemData->url != NULL)	
		delete[] pItemData->url;

	if ( pItemData->closeButtonURLPath != NULL)	
		delete[] pItemData->closeButtonURLPath;

	if ( pItemData->dismissReason != NULL)	
		delete[] pItemData->dismissReason;

    delete[] pItemData;
}

static void s3ePH_ReleaseRewardCallback(uint32, int32, void*, void*, int32, void* pData )
{
	s3eDebugOutputString( "s3ePH_ReleaseRewardContentCallback" );
	
	s3ePHReward* pItemData = (s3ePHReward*) pData;
	
	if ( pItemData->name != NULL)	
		delete[] pItemData->name;

	if ( pItemData->receipt != NULL)	
		delete[] pItemData->receipt;

    delete[] pItemData;
}

static void s3ePH_ReleasePurchaseCallback(uint32, int32, void*, void*, int32, void* pData )
{
	s3eDebugOutputString( "s3ePH_ReleasePurchaseCallback" );
	
	s3ePHPurchase* pItemData = (s3ePHPurchase*) pData;
	
	if ( pItemData->productIdentifier != NULL)	
		delete[] pItemData->productIdentifier;

	if ( pItemData->receipt != NULL)	
		delete[] pItemData->receipt;

    delete[] pItemData;
}


void JNICALL s3ePH_contentStartCallback( JNIEnv* env, jobject obj, jobject param )
{
	s3eDebugOutputString("s3ePH_contentStartCallback: start");

	const char* pItemData = jstringToChar( env, (jstring) param );
	
	s3eEdkCallbacksEnqueue(	S3E_EXT_PLAYHAVEN_HASH, S3EPH_CONTENT_START , (void*)pItemData, 
		0 , NULL, false, &s3ePH_ReleaseStringCallback, (void*)pItemData );
}

void JNICALL s3ePH_contentReceivedCallback( JNIEnv* env, jobject obj, jobject param )
{
	s3eDebugOutputString("s3ePH_contentReceivedCallback: start");

	const char* pItemData = jstringToChar( env, (jstring) param );

	s3eEdkCallbacksEnqueue(	S3E_EXT_PLAYHAVEN_HASH, S3EPH_CONTENT_RECEIVED , (void*)pItemData, 
		0 , NULL, false, &s3ePH_ReleaseStringCallback, (void*)pItemData );
}

void JNICALL s3ePH_contentWillDisplayCallback( JNIEnv* env, jobject obj, jobject param )
{
	s3eDebugOutputString("s3ePH_contentWillDisplayCallback: start");

	s3ePHPublisherContent* pItemData = new s3ePHPublisherContent();
	
	jclass cls = env->FindClass( "s3ePlayhavenFacade$PublisherContent" );
	
	if (env->ExceptionOccurred())
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
	
	pItemData->placement			= StringFieldAccessor( env, cls, "placement",		param );
	pItemData->url					= StringFieldAccessor( env, cls, "url",				param );
	pItemData->transition			= (s3ePHContentTransitionType) IntFieldAccessor(    env, cls, "transition",		param );
	pItemData->closeButtonURLPath	= StringFieldAccessor( env, cls, "closeButtonURLPath",	param );
	pItemData->dismissReason		= StringFieldAccessor( env, cls, "dismissReason",		param );

	s3eEdkCallbacksEnqueue(	S3E_EXT_PLAYHAVEN_HASH, S3EPH_CONTENT_WILL_DISPLAY, (void*)pItemData, 
		0 , NULL, false, &s3ePH_ReleasePublisherContentCallback, (void*)pItemData );
}

void JNICALL s3ePH_contentDidDisplayCallback( JNIEnv* env, jobject obj, jobject param )
{
	s3eDebugOutputString("s3ePH_contentDidDisplayCallback: start");

	s3ePHPublisherContent* pItemData = new s3ePHPublisherContent();
	
	jclass cls = env->FindClass( "s3ePlayhavenFacade$PublisherContent" );
	
	if (env->ExceptionOccurred())
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
	
	pItemData->placement			= StringFieldAccessor( env, cls, "placement",		param );
	pItemData->url					= StringFieldAccessor( env, cls, "url",				param );
	pItemData->transition			= (s3ePHContentTransitionType) IntFieldAccessor(    env, cls, "transition",		param );
	pItemData->closeButtonURLPath	= StringFieldAccessor( env, cls, "closeButtonURLPath",	param );
	pItemData->dismissReason		= StringFieldAccessor( env, cls, "dismissReason",		param );

	s3eEdkCallbacksEnqueue(	S3E_EXT_PLAYHAVEN_HASH, S3EPH_CONTENT_DID_DISPLAY, (void*)pItemData, 
		0 , NULL, false, &s3ePH_ReleasePublisherContentCallback, (void*)pItemData );
}

void JNICALL s3ePH_contentDismissingCallback( JNIEnv* env, jobject obj, jobject param )
{
	s3eDebugOutputString("s3ePH_contentDismissingCallback: start");

	s3ePHPublisherContent* pItemData = new s3ePHPublisherContent();
	
	jclass cls = env->FindClass( "s3ePlayhavenFacade$PublisherContent" );
	
	if (env->ExceptionOccurred())
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
	
	pItemData->placement			= StringFieldAccessor( env, cls, "placement",		param );
	pItemData->url					= StringFieldAccessor( env, cls, "url",				param );
	pItemData->transition			= (s3ePHContentTransitionType) IntFieldAccessor(    env, cls, "transition",		param );
	pItemData->closeButtonURLPath	= StringFieldAccessor( env, cls, "closeButtonURLPath",	param );
	pItemData->dismissReason		= StringFieldAccessor( env, cls, "dismissReason",		param );

	s3eEdkCallbacksEnqueue(	S3E_EXT_PLAYHAVEN_HASH, S3EPH_CONTENT_DISMISSING, (void*)pItemData, 
		0 , NULL, false, &s3ePH_ReleasePublisherContentCallback, (void*)pItemData );
}

void JNICALL s3ePH_contentFailedCallback( JNIEnv* env, jobject obj, jobject param )
{
	s3eDebugOutputString("s3ePH_contentFailedCallback: start");

	const char* pItemData = jstringToChar( env, (jstring) param );
	
	s3eEdkCallbacksEnqueue(	S3E_EXT_PLAYHAVEN_HASH, S3EPH_CONTENT_FAILED , (void*)pItemData, 
		0 , NULL, false, &s3ePH_ReleaseStringCallback, (void*)pItemData );
}

void JNICALL s3ePH_contentUnlockRewardCallback( JNIEnv* env, jobject obj, jobject param )
{
	s3eDebugOutputString("s3ePH_contentUnlockRewardCallback: start");

	s3ePHReward* pItemData = new s3ePHReward();
	
	jclass cls = env->FindClass( "s3ePlayhavenFacade$Reward" );
	
	if (env->ExceptionOccurred())
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
	
	pItemData->name			= StringFieldAccessor( env, cls, "name",		param );
	pItemData->quantity		= IntFieldAccessor(    env, cls, "quantity",	param );
	pItemData->receipt		= StringFieldAccessor( env, cls, "receipt",		param );
	
	s3eEdkCallbacksEnqueue(	S3E_EXT_PLAYHAVEN_HASH, S3EPH_CONTENT_UNLOCK_REWARD, (void*)pItemData, 
		0 , NULL, false, &s3ePH_ReleaseRewardCallback, (void*)pItemData );
}

void JNICALL s3ePH_contentMakePurchaseCallback( JNIEnv* env, jobject obj, jobject param )
{
	s3eDebugOutputString("s3ePH_contentMakePurchaseCallback: start");

	s3ePHPurchase* pItemData = new s3ePHPurchase();
	
	jclass cls = env->FindClass( "s3ePlayhavenFacade$Purchase" );
	
	if (env->ExceptionOccurred())
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
	
	pItemData->productIdentifier	= StringFieldAccessor( env, cls, "productIdentifier",		param );
	pItemData->quantity				= IntFieldAccessor(    env, cls, "quantity",		param );
	pItemData->receipt				= StringFieldAccessor( env, cls, "receipt",		param );

	s3eEdkCallbacksEnqueue(	S3E_EXT_PLAYHAVEN_HASH, S3EPH_CONTENT_MAKE_PURCHASE, (void*)pItemData, 
		0 , NULL, false, &s3ePH_ReleasePurchaseCallback, (void*)pItemData );
}
