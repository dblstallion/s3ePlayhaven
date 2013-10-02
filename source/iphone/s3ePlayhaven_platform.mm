/*
 * iphone-specific implementation of the s3ePlayhaven extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "s3ePlayhaven_internal.h"
#include "s3eEdk.h"
#include "s3eEDK_iPhone.h"

#import "PlayHavenSDK.h"
#import "PHContent.h"
#import "PHReward.h"
#import "PHPurchase.h"


// global key storage
static NSString* gPHToken = NULL;
static NSString* gPHSecret = NULL;

// we need to hold onto the purchase object so the Marmalade App can notify it when it's done
static PHPurchase* gLastPurchase = NULL;

// we allow one notification view
static PHNotificationView *gNotificationView = NULL;

// Helper functions
// convert an NSString to a utf8 char* caller must dispose of memory using s3eEdkFreeOS
static char* CopyToCString(NSString* string)
{
    if (string)
    {
        const char* pStr = [string UTF8String];
        char* pCopy = (char*) s3eEdkMallocOS(strlen(pStr) + 1);
        if (pCopy)
        {
            strcpy(pCopy, pStr);
            return pCopy;
        }
    }
    return NULL;
}

// caller must dispose of memory using s3eEdkFreeOS
static char* CopyCString(const char* pStr)
{
	if (pStr)
	{
		char* pCopy = (char*) s3eEdkMallocOS(strlen(pStr) + 1);
		if (pCopy)
        {
            strcpy(pCopy, pStr);
            return pCopy;
        }
	}
	return NULL;
}

// we need to marshall data for the C++ callback and deallocate when the callback is finished
// marmalade callbacks are queued until the next yield and this will be called at that point
static void s3ePHContentDeallocator(uint32 extID, int32 notification, void *systemData, void *instance, int32 returnCode, void *completeData)
{
    if (!systemData) return;
    s3ePHPublisherContent *content = (s3ePHPublisherContent*)systemData;
	if (content->placement) s3eEdkFreeOS((void*)content->placement);
	if (content->url) s3eEdkFreeOS((void*)content->url);
	if (content->closeButtonURLPath) s3eEdkFreeOS((void*)content->closeButtonURLPath);
	if (content->dismissReason) s3eEdkFreeOS((void*)content->dismissReason);
    s3eEdkFreeOS(content);
}

static void s3ePHRewardDeallocator(uint32 extID, int32 notification, void *systemData, void *instance, int32 returnCode, void *completeData)
{
    if (!systemData) return;
	s3ePHReward *reward = (s3ePHReward*)systemData;
	if (reward->name) s3eEdkFreeOS((void*)reward->name);
	if (reward->receipt) s3eEdkFreeOS((void*)reward->receipt);
	s3eEdkFreeOS(reward);
}

static void s3ePHPurchaseDeallocator(uint32 extID, int32 notification, void *systemData, void *instance, int32 returnCode, void *completeData)
{
    if (!systemData) return;
	s3ePHPurchase *purchase = (s3ePHPurchase*)systemData;
	if (purchase->productIdentifier) s3eEdkFreeOS((void*)purchase->productIdentifier);
	if (purchase->receipt) s3eEdkFreeOS((void*)purchase->receipt);
	s3eEdkFreeOS(purchase);
}

// Simple Playhaven Delegate
//----------------------------------------------------------------------------

@interface s3ePlayhavenContentDelegate : NSObject <PHPublisherContentRequestDelegate> // inherits from NSObject implements PHPublisherContentRequestDelegate
@end // s3ePlayhavenContentDelegate interface

// The delegeate implementation marshalls selected Playhaven Objective C data and passes it to the C++ callback if registered

@implementation s3ePlayhavenContentDelegate

-(void)requestWillGetContent:(PHPublisherContentRequest *)request
{
   NSLog(@"Playhaven:requestWillGetContent:%@", request.placement);

	if (s3eEdkCallbacksIsRegistered(S3E_EXT_PLAYHAVEN_HASH,S3EPH_CONTENT_START))
	{
		// s3eCallbacksEnqueue will copy the string
		const char *placementMsg = request.placement ? [request.placement UTF8String] : NULL;
        s3eEdkCallbacksEnqueue(S3E_EXT_PLAYHAVEN_HASH, S3EPH_CONTENT_START, (void*)placementMsg, (placementMsg ? strlen(placementMsg)+1 : 0));
	}
}

-(void)requestDidGetContent:(PHPublisherContentRequest *)request
{
   NSLog(@"Playhaven:requestDidGetContent:%@", request.placement);

	if (s3eEdkCallbacksIsRegistered(S3E_EXT_PLAYHAVEN_HASH,S3EPH_CONTENT_START))
	{
		// s3eCallbacksEnqueue will copy the string
		const char *placementMsg = request.placement ? [request.placement UTF8String] : NULL;
        s3eEdkCallbacksEnqueue(S3E_EXT_PLAYHAVEN_HASH, S3EPH_CONTENT_START, (void*)placementMsg, (placementMsg ? strlen(placementMsg)+1 : 0));
	}
}

-(void)request:(PHPublisherContentRequest *)request contentWillDisplay:(PHContent *)content
{
	NSLog(@"Playhaven:contentWillDisplay:%@, %@, %@", request.placement, content.URL,content.closeButtonURLPath);

	if ((s3eEdkCallbacksIsRegistered(S3E_EXT_PLAYHAVEN_HASH,S3EPH_CONTENT_WILL_DISPLAY)) && (content))
	{
		// marshall the strings into our custom structure
        s3ePHPublisherContent *cont = (s3ePHPublisherContent*)s3eEdkMallocOS(sizeof(s3ePHPublisherContent));
		cont->placement = (request.placement)?CopyToCString(request.placement):NULL;
		cont->url = (content.URL)?CopyToCString(content.URL.absoluteString):NULL;
		cont->transition = (s3ePHContentTransitionType)content.transition;
		cont->closeButtonURLPath = (content.closeButtonURLPath)?CopyToCString(content.closeButtonURLPath):NULL;
		cont->dismissReason = NULL;
        
        // queue the callback and specify our deallocator which kicks in after the callback completes - the deallocator isn't
        // invoked until well after the scope of this function exits.
        // the sysDataSize is zero which means Marmalade won't make a copy
        s3eEdkCallbacksEnqueue(S3E_EXT_PLAYHAVEN_HASH, S3EPH_CONTENT_WILL_DISPLAY, cont, 0, NULL, false, s3ePHContentDeallocator, cont);
	}
	// clear any notifications
	if (gNotificationView)
		s3ePHClearNotificationView_platform();
}

-(void)request:(PHPublisherContentRequest *)request contentDidDisplay:(PHContent *)content
{
	NSLog(@"Playhaven:contentDidDisplay:%@, %@, %@", request.placement, content.URL, content.closeButtonURLPath);

	if ((s3eEdkCallbacksIsRegistered(S3E_EXT_PLAYHAVEN_HASH,S3EPH_CONTENT_DID_DISPLAY)) && (content))
	{
		// marshall the strings into our custom structure
        s3ePHPublisherContent *cont = (s3ePHPublisherContent*)s3eEdkMallocOS(sizeof(s3ePHPublisherContent));
		cont->placement = (request.placement)?CopyToCString(request.placement):NULL;
		cont->url = (content.URL)?CopyToCString(content.URL.absoluteString):NULL;
		cont->transition = (s3ePHContentTransitionType)content.transition;
		cont->closeButtonURLPath = (content.closeButtonURLPath)?CopyToCString(content.closeButtonURLPath):NULL;
		cont->dismissReason = NULL;
        
        // queue the callback and specify our deallocator which kicks in after the callback completes - the deallocator isn't
        // invoked until well after the scope of this function exits.
        // the sysDataSize is zero which means Marmalade won't make a copy
        s3eEdkCallbacksEnqueue(S3E_EXT_PLAYHAVEN_HASH,S3EPH_CONTENT_DID_DISPLAY,cont,0,NULL,false,s3ePHContentDeallocator,cont);
	}
}

-(void)request:(PHPublisherContentRequest *)request contentDidDismissWithType:(PHPublisherContentDismissType *)type
{
	NSLog(@"Playhaven:contentDidDismissWithType:%@, %@", request.placement, type);

	if ((s3eEdkCallbacksIsRegistered(S3E_EXT_PLAYHAVEN_HASH,S3EPH_CONTENT_DISMISSING)) && (type))
	{
		// marshall the strings into our custom structure
        s3ePHPublisherContent *cont = (s3ePHPublisherContent*)s3eEdkMallocOS(sizeof(s3ePHPublisherContent));
		cont->placement = (request.placement)?CopyToCString(request.placement):NULL;
		cont->url = NULL;
		cont->transition = s3ePHContentTransitionUnknown;
		cont->closeButtonURLPath = NULL;
		cont->dismissReason = (type)?CopyToCString(type):NULL;
        
        // queue the callback and specify our deallocator which kicks in after the callback completes - the deallocator isn't
        // invoked until well after the scope of this function exits.
        // the sysDataSize is zero which means Marmalade won't make a copy
        s3eEdkCallbacksEnqueue(S3E_EXT_PLAYHAVEN_HASH,S3EPH_CONTENT_DISMISSING,cont,0,NULL,false,s3ePHContentDeallocator,cont);
	}
}

-(void)request:(PHPublisherContentRequest *)request didFailWithError:(NSError *)error
{	
	NSLog(@"Playhaven:didFailWithError:%@ %@", request.placement, error);

	if (s3eEdkCallbacksIsRegistered(S3E_EXT_PLAYHAVEN_HASH,S3EPH_CONTENT_FAILED))
	{
		const char* placementMsg = (request.placement)?CopyToCString(request.placement):CopyCString("");
		const char* errorMsg = (error)?CopyToCString([error localizedDescription]):CopyCString("");
		char* concat = (char*)s3eEdkMallocOS(strlen(placementMsg) + strlen(errorMsg) + 2);
		sprintf(concat,"%s:%s",placementMsg,errorMsg);

        s3eEdkCallbacksEnqueue(S3E_EXT_PLAYHAVEN_HASH,S3EPH_CONTENT_FAILED,concat,strlen(concat)+1);
		s3eEdkFreeOS((void*)concat);
		s3eEdkFreeOS((void*)errorMsg);
		s3eEdkFreeOS((void*)placementMsg);
	}
}

-(void)request:(PHPublisherContentRequest *)request unlockedReward:(PHReward *)reward
{
	NSLog(@"Playhaven:unlockedReward:%@ %@", request.placement, reward);

	if ((s3eEdkCallbacksIsRegistered(S3E_EXT_PLAYHAVEN_HASH,S3EPH_CONTENT_UNLOCK_REWARD)) && (reward))
	{
		// marshall the strings into our custom structure
        s3ePHReward *rew = (s3ePHReward*)s3eEdkMallocOS(sizeof(s3ePHReward));
		rew->name = (reward.name)?CopyToCString(reward.name):NULL;
		rew->quantity = reward.quantity;
		rew->receipt = (reward.receipt)?CopyToCString(reward.receipt):NULL;

        // queue the callback and specify our deallocator which kicks in after the callback completes - the deallocator isn't
        // invoked until well after the scope of this function exits.
        // the sysDataSize is zero which means Marmalade won't make a copy
        s3eEdkCallbacksEnqueue(S3E_EXT_PLAYHAVEN_HASH,S3EPH_CONTENT_UNLOCK_REWARD,rew,0,NULL,false,s3ePHRewardDeallocator,rew);
	}
}

-(void)request:(PHPublisherContentRequest *)request makePurchase:(PHPurchase *)purchase
{
	NSLog(@"Playhaven:makePurchase:%@", purchase);

	if (s3eEdkCallbacksIsRegistered(S3E_EXT_PLAYHAVEN_HASH,S3EPH_CONTENT_MAKE_PURCHASE))
	{
		if (purchase)
		{
			// marshall the strings into our custom structure
			s3ePHPurchase *pur = (s3ePHPurchase*)s3eEdkMallocOS(sizeof(s3ePHPurchase));
			pur->productIdentifier = (purchase.productIdentifier)?CopyToCString(purchase.productIdentifier):NULL;
			pur->quantity = purchase.quantity;
			pur->receipt = (purchase.receipt)?CopyToCString(purchase.receipt):NULL;

			// queue the callback and specify our deallocator which kicks in after the callback completes - the deallocator isn't
			// invoked until well after the scope of this function exits.
			// the sysDataSize is zero which means Marmalade won't make a copy
			s3eEdkCallbacksEnqueue(S3E_EXT_PLAYHAVEN_HASH,S3EPH_CONTENT_MAKE_PURCHASE,pur,0,NULL,false,s3ePHPurchaseDeallocator,pur);

			// store the purchase object so that we can call reportResolution on it later - we assume only one can be in flight
			if (gLastPurchase)
			{
				NSLog(@"Playhaven:makePurchase:warning an uncleared purchase was already in flight. You must call s3ePHReportResolution when a purchase is complete.");
				[gLastPurchase release];
				gLastPurchase = NULL;
			}
			[purchase retain];
			gLastPurchase = purchase;
		}
	}
}

@end // s3ePlayhavenContentDelegate implementation

// the Content Delegate singleton
static s3ePlayhavenContentDelegate *gContentDelegate = NULL;

s3eResult s3ePlayhavenInit_platform()
{
    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;
}

void s3ePlayhavenTerminate_platform()
{
	NSLog(@"Playhaven:s3ePlayhavenTerminate releasing resources");

	[gPHToken release]; 
	gPHToken = NULL;	
	[gPHSecret release];
	gPHSecret = NULL;
	if (gNotificationView)
	{
		[gNotificationView release];
		gNotificationView = NULL;
	}
    if (gContentDelegate)
    {
        [gContentDelegate release];
        gContentDelegate = NULL;
    }
}

s3eResult s3ePlayhavenInitWithKeys_platform(const char* token, const char* secret)
{
	// store the keys so that the user doesn't need to keep passing them
	gPHToken = [[NSString alloc] initWithUTF8String:token];
	gPHSecret = [[NSString alloc] initWithUTF8String:secret];
    
    if (!gContentDelegate)
        gContentDelegate = [[s3ePlayhavenContentDelegate alloc] init];

	NSLog(@"Playhaven:s3ePlayhavenInit with keys %@ %@",gPHToken,gPHSecret);

    return S3E_RESULT_SUCCESS;
}

void s3ePHRegisterCallback_platform(s3ePHCallbackType callbackID, s3eCallback callbackFn, void* userData)
{
	s3eEdkCallbacksRegister(
                            S3E_EXT_PLAYHAVEN_HASH,
                            S3EPH_MAX_ID,
                            callbackID,
                            callbackFn,
                            userData,
                            S3E_FALSE);
}

void s3ePHSendAppOpen_platform(const char* customUDID)
{
	NSLog(@"Playhaven:s3ePHSendAppOpen");

	PHPublisherOpenRequest *request = [PHPublisherOpenRequest requestForApp:gPHToken secret:gPHSecret];
	if (customUDID)
	{
		NSString *nsCustomUDID = [[NSString alloc] initWithUTF8String:customUDID];
		request.customUDID = nsCustomUDID;
		[nsCustomUDID release];
	}
    @try { 
		[request send]; 
	}
    @catch (NSException *exception) { // catch all exceptions so Marmalade keeps going
        NSLog(@"s3ePHSendAppOpen: Caught %@: %@", [exception name], [exception reason]);
        NSLog(@"StacK: %@", [exception callStackSymbols]);
    }
}

void s3ePHSendContentRequest_platform(const char* placement, bool showOverlayImmediately)
{
	NSString *nsPlacement = [[NSString alloc] initWithUTF8String:placement];

	NSLog(@"Playhaven:s3ePHSendContentRequest for placement %@",nsPlacement);
	
	PHPublisherContentRequest *request = [PHPublisherContentRequest requestForApp:gPHToken secret:gPHSecret placement:nsPlacement delegate:gContentDelegate];
	request.showsOverlayImmediately = showOverlayImmediately; 
    @try { 
		[request send];
	}
    @catch (NSException *exception) { // catch all exceptions so Marmalade keeps going
        NSLog(@"s3ePHSendContentRequest: Caught %@: %@", [exception name], [exception reason]);
        NSLog(@"StacK: %@", [exception callStackSymbols]);
    }
	[nsPlacement release];
}

void s3ePHPreloadContentRequest_platform(const char* placement)
{	
	NSString *nsPlacement = [[NSString alloc] initWithUTF8String:placement];

	NSLog(@"Playhaven:s3ePHSendContentRequest for placement %@",nsPlacement);
	
	PHPublisherContentRequest *request = [PHPublisherContentRequest requestForApp:gPHToken secret:gPHSecret placement:nsPlacement delegate:gContentDelegate];
    @try { 
		[request preload];
	}
    @catch (NSException *exception) { // catch all exceptions so Marmalade keeps going
        NSLog(@"s3ePHSendContentRequest: Caught %@: %@", [exception name], [exception reason]);
        NSLog(@"StacK: %@", [exception callStackSymbols]);
    }
	[nsPlacement release];
}

void s3ePHCancelAllContentRequests_platform()
{
    if (gContentDelegate)
        [PHAPIRequest cancelAllRequestsWithDelegate:(id)gContentDelegate]; // we share one delegate across all content views
}

void s3ePHReportResolution_platform(s3ePHPurchaseResolutionType resolution)
{
	NSLog(@"Playhaven:s3ePHReportResolution");

	if (!gLastPurchase)
	{
		NSLog(@"s3ePHReportResolution invoked without any purchases in flight");
		return;
	}
	[gLastPurchase reportResolution:(PHPurchaseResolutionType)resolution];
	[gLastPurchase release];
	gLastPurchase = NULL;
}

void s3ePHSendPublisherIAPTrackingRequest_platform(const char* product, int quantity, s3ePHPurchaseResolutionType resolution)
{
	NSString *nsProduct = [[NSString alloc] initWithUTF8String:product];
	NSLog(@"Playhaven:s3ePHSendPublisherIAPTrackingRequest %@",nsProduct);

	PHPublisherIAPTrackingRequest *request =  [PHPublisherIAPTrackingRequest requestForApp:gPHToken secret:gPHSecret product:nsProduct quantity:quantity resolution:(PHPurchaseResolutionType)resolution];
	[request send]; 

	[nsProduct release];
}

void s3ePHSendPublisherIAPTrackingRequestWithReceipt_platform(const char* product, int quantity, s3ePHPurchaseResolutionType resolution, const void* receiptData, size_t receiptSize)
{
	NSString *nsProduct = [[NSString alloc] initWithUTF8String:product];
	NSData *receipt = [[NSData alloc] initWithBytes:receiptData length:receiptSize];
	
	NSLog(@"Playhaven:s3ePHSendPublisherIAPTrackingRequestWithReceipt %@",nsProduct);

	PHPublisherIAPTrackingRequest *request = [PHPublisherIAPTrackingRequest requestForApp:gPHToken secret:gPHSecret product:nsProduct quantity:quantity resolution:(PHPurchaseResolutionType)resolution receiptData:receipt];
	[request send]; 

	[nsProduct release];
	[receipt release];
}

s3eResult s3ePHShowNotificationView_platform(int x,int y,bool useGLView,bool testing)
{
	NSLog(@"Playhaven:s3ePHShowNotificationView");
	if (gNotificationView)
	{
		NSLog(@"s3ePHShowNotificationView notification view already exists");
		return S3E_RESULT_ERROR;
	}

	gNotificationView = [[PHNotificationView alloc] initWithApp:gPHToken secret:gPHSecret placement:@"more_games"];

	UIView *view;
	if (useGLView)
		view = s3eEdkGetGLUIView();
	else
		view = s3eEdkGetSurfaceUIView();
	if (!view)
	{
		NSLog(@"s3ePHShowNotificationView unable to obtain view pointer");
		return S3E_RESULT_ERROR;
	}
	gNotificationView.center = CGPointMake(x,y);
	[view addSubview:gNotificationView];
	if (!testing)
		[gNotificationView refresh];
	else
		[gNotificationView test];

	return S3E_RESULT_SUCCESS;
}

void s3ePHClearNotificationView_platform()
{
	NSLog(@"Playhaven:s3ePHClearNotificationView");

	if (!gNotificationView)
	{
		NSLog(@"s3ePHClearNotificationView no notification to clear");
		return;
	}
	[gNotificationView removeFromSuperview];
	[gNotificationView release];
	gNotificationView = NULL;
}

void s3ePHRefreshNotificationView_platform(bool testing)
{	
	NSLog(@"Playhaven:s3ePHRefreshNotificationView");

	if (!gNotificationView)
	{
		NSLog(@"s3ePHRefreshNotificationView no notification to refresh");
		return;
	}
	if (!testing)
		[gNotificationView refresh];
	else
		[gNotificationView test];
}

void s3ePHSetOptOutStatus_platform(bool on)
{
	NSLog(@"Playhaven:s3ePHSetOptOutStatus");

	[PHAPIRequest setOptOutStatus:(BOOL)on];
}
