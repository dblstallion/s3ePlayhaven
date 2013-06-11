
// COPYRIGHT(c) 2011, Medium Entertainment, Inc., a Delaware corporation, which operates a service
// called PlayHaven., All Rights Reserved
//  
// NOTICE:  All information contained herein is, and remains the property of Medium Entertainment, Inc.
// and its suppliers, if any.  The intellectual and technical concepts contained herein are 
// proprietary to Medium Entertainment, Inc. and its suppliers and may be covered by U.S. and Foreign
// Patents, patents in process, and are protected by trade secret or copyright law. Dissemination of this 
// information or reproduction of this material is strictly forbidden unless prior written permission 
// is obtained from Medium Entertainment, Inc.
// 
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
// 
// Contact: support@playhaven.com


// Adapted from the version in the Unity SDK to support native calls for interfacing with Marmalade 

import com.playhaven.src.common.PHConfig;
import com.playhaven.src.common.PHAPIRequest;
import com.playhaven.src.common.PHSession;
import com.playhaven.src.publishersdk.open.PHPublisherOpenRequest;
import com.playhaven.src.publishersdk.content.PHPublisherContentRequest;
import com.playhaven.src.publishersdk.content.PHPublisherContentRequest.PHDismissType;
import com.playhaven.src.publishersdk.content.PHReward;
import com.playhaven.src.publishersdk.content.PHContent;
import com.playhaven.src.publishersdk.content.PHPurchase;
import com.playhaven.src.publishersdk.metadata.PHPublisherMetadataRequest;
import com.playhaven.src.publishersdk.purchases.PHPublisherIAPTrackingRequest;
import com.playhaven.src.publishersdk.content.PHContentView.ButtonState;

import android.app.Activity;
import android.util.Log;
import android.graphics.Bitmap;

import org.json.JSONObject;
//import org.json.JSONException;

//import java.util.HashMap;
import java.lang.Runnable;

/**
 *
 */
public class s3ePlayhavenFacade implements PHPublisherContentRequest.ContentDelegate, PHPublisherContentRequest.PurchaseDelegate, PHPublisherContentRequest.RewardDelegate, PHPublisherContentRequest.CustomizeDelegate, PHPublisherContentRequest.FailureDelegate
{	
	private Activity currentActivity;
	private PHPurchase currentPurchase;

	//java versions of the native structs which are passed to the native functions
	
	public static class PublisherContent
	{
        public String placement;
        public String url;
        public int transition;
        public String closeButtonURLPath;
        public String dismissReason;
	}

	public static class Reward
	{
		public String name;
		public int quantity;
		public String receipt;
	}
	
	public static class Purchase
	{
		public String productIdentifier;
		public int quantity;
		public String receipt;
	}

	//native functions on the c++ side
	
	public static native void native_contentStart( String placement );
	public static native void native_contentReceived( String placement );
	public static native void native_contentWillDisplay( PublisherContent param );
	public static native void native_contentDidDisplay( PublisherContent param );
	public static native void native_contentDismissing( PublisherContent param );
	public static native void native_contentFailed( String placementPlusError );
	public static native void native_contentUnlockReward( Reward reward );
	public static native void native_contentMakePurchase( Purchase purchase );
	
	
	private class RequestRunner implements Runnable
	{
		private PHAPIRequest request;
		
		public void run(final Activity currentActivity, final PHAPIRequest request)
		{
			this.request = request;
			currentActivity.runOnUiThread(this);	
		}
		
		public void run()
		{
			request.send();
		}
	}

	private class PreloadRequestRunner implements Runnable
	{
		private PHPublisherContentRequest contentRequest;
		
		public void run(final Activity currentActivity, final PHPublisherContentRequest contentRequest)
		{
			this.contentRequest = contentRequest;
			currentActivity.runOnUiThread(this);
		}
		
		public void run()
		{
			contentRequest.preload();
		}
	}    
	
	/**
	  * Constructor.
	  * @param currentActivity The current activity.
      * @param token The application token, obtained from the PlayHaven web dashboard for your game.
      * @param secret The application secret, obtained from the PlayHaven web dashboard for your game.
	  */
	public s3ePlayhavenFacade(final Activity currentActivity, String token, String secret)
	{		
		setCurrentActivity(currentActivity);
		setKeys(token, secret);
	}
	
	/**
	  * Set the current activity.  This is needed so that asynchronous code can be
	  * executed on the UI thread.
	  * @param currentActivity The current activity.
	  */
	public void setCurrentActivity(final Activity currentActivity)
	{
		this.currentActivity = currentActivity;
		PHConfig.cacheDeviceInfo(currentActivity);
	}
	
    /**
     * Set the token and secret keys with the Android SDK.
     * @param token The application token, obtained from the PlayHaven web dashboard for your game.
     * @param secret The application secret, obtained from the PlayHaven web dashboard for your game.
     */
    public void setKeys(String token, String secret)
    {
 		Log.d("s3ePlayhavenFacade", "setKeys");
		PHConfig.token = token;
		PHConfig.secret = secret;
    }

    /**
     * Register for tracking.
     */
    public void register()
    {
        if (currentActivity != null)
            PHSession.register(currentActivity);
    }
    
    /**
     * Un-register for tracking.
     */
    public void unregister()
    {
        if (currentActivity != null)
            PHSession.unregister(currentActivity);
    }
    
	/**
	 * Send the Open() request to PlayHaven, notifying the system that the game
	 * has launched.
     * @param hash A hash value that uniquely identifies the request.
	 */
	public void openRequest(int hash)
	{
 		Log.d("s3ePlayhavenFacade", "openRequest");
		PHPublisherOpenRequest request = new PHPublisherOpenRequest(currentActivity, this);
		request.setRequestTag(hash);
		new RequestRunner().run(currentActivity, request);
	}
	
	/**
	 * Send a metadata request to PlayHaven.
     * @param hash A hash value that uniquely identifies the request.
     * @param placement The placement to associate the metadata to.
	 */
	public void metaDataRequest(int hash, String placement)
	{
 		Log.d("s3ePlayhavenFacade", "metaDataRequest");		
		PHPublisherMetadataRequest request = new PHPublisherMetadataRequest(currentActivity, this, placement);
		request.setRequestTag(hash);
		new RequestRunner().run(currentActivity, request);
	}
	
	/**
	 * Report a resolution to a purchase promotion content unit.
	 */
	public void reportResolution(int resolution)
	{
 		Log.d("s3ePlayhavenFacade", "reportResolution");
		if (currentPurchase != null)
		{
			currentPurchase.reportResolution(PHPurchase.Resolution.values()[resolution], currentActivity);
		}
		currentPurchase = null;
	}
	
	/**
	 * Submit an IAP tracking request.
	 */
	public void iapTrackingRequest(String productId, int quantity, int resolution)
	{
 		Log.d("s3ePlayhavenFacade", "iapTrackingRequest");
		PHPublisherIAPTrackingRequest request = new PHPublisherIAPTrackingRequest(currentActivity, this);
		request.product = productId;
		request.quantity = quantity;
		request.resolution = PHPurchase.Resolution.values()[resolution];
		new RequestRunner().run(currentActivity, request);		
	}
	
	/**
	 * Send a content request to PlayHaven.
     * @param hash A hash value that uniquely identifies the request.
     * @param placement The placement to associate the metadata to.
	 */
	public void contentRequest(int hash, String placement)
	{
 		Log.d("s3ePlayhavenFacade", "contentRequest");	
		PHPublisherContentRequest request = new PHPublisherContentRequest(currentActivity, this, placement);
		request.setRequestTag(hash);
		new RequestRunner().run(currentActivity, request);
	}

	/**
	 * Send a content preload request to PlayHaven.
     * @param hash A hash value that uniquely identifies the request.
     * @param placement The placement to associate the metadata to.
	 */
	public void preloadRequest(int hash, String placement)
	{
 		Log.d("s3ePlayhavenFacade", "preloadRequest");
		PHPublisherContentRequest request = new PHPublisherContentRequest(currentActivity, this, placement);
		request.setRequestTag(hash);
		new PreloadRequestRunner().run(currentActivity, request);
	}
	
	// PHAPIRequestDelegate interface
	
	public void requestSucceeded(PHAPIRequest request, JSONObject responseData)
	{
 		Log.d("s3ePlayhavenFacade", "requestSucceeded");	
	}
	
	public void requestFailed(PHAPIRequest request, Exception e)
	{
 		Log.d("s3ePlayhavenFacade", "requestFailed");
		
 		String responseData = e.getMessage();

 		s3ePlayhavenFacade.native_contentFailed(responseData);
	}
	
	public void didFail(PHPublisherContentRequest request, String description)
	{
 		Log.d("s3ePlayhavenFacade", "requestFailed");
		
 		String responseData = description;

 		s3ePlayhavenFacade.native_contentFailed(responseData);

	}
	
	public void willGetContent(PHPublisherContentRequest request)
	{}
	
	private int TransitionToInt(PHContent.TransitionType transition)
	{
		switch (transition)
		{
			default:
			case Unknown: return 0;
			case Modal: return 1;
			case Dialog: return 2;
		}
	}
		
	public void willDisplayContent(PHPublisherContentRequest request, PHContent content)
	{
		Log.d("s3ePlayhavenFacade", "willDisplayContent");	
		
		PublisherContent responseData = new PublisherContent();
		
		responseData.placement = "";
		responseData.url = content.url.toString();
		responseData.transition = TransitionToInt(content.transition);
		responseData.closeButtonURLPath = content.closeURL;
        responseData.dismissReason = "";        
		
		s3ePlayhavenFacade.native_contentWillDisplay(responseData);
		
	}
	
	public void didDisplayContent(PHPublisherContentRequest request, PHContent content)
	{
		Log.d("s3ePlayhavenFacade", "didDisplayContent");	
		
		PublisherContent responseData = new PublisherContent();
		
		responseData.placement = "";
		responseData.url = content.url.toString();
		responseData.transition = TransitionToInt(content.transition);
		responseData.closeButtonURLPath = content.closeURL;
		responseData.dismissReason = "";        
		
		s3ePlayhavenFacade.native_contentDidDisplay(responseData);
	}
	
	public void didDismissContent(PHPublisherContentRequest request, PHDismissType type)
	{
		Log.d("s3ePlayhavenFacade", "didDismissContent");	
		
		PublisherContent responseData = new PublisherContent();
		
		responseData.placement = "";
		responseData.url = "";
		responseData.transition = 0;
		responseData.closeButtonURLPath = "";
		responseData.dismissReason = type.toString(); 
		
		s3ePlayhavenFacade.native_contentDismissing(responseData);
	}
	
	public void didFail(PHPublisherContentRequest request, Exception e)
	{
		requestFailed(request, e);		
	}
	
	public void contentDidFail(PHPublisherContentRequest request, Exception e)
	{
		requestFailed(request, e);
	}

	public Bitmap closeButton(PHPublisherContentRequest request, ButtonState state)
	{
		return null; // resort to default
	}
	
	public int borderColor(PHPublisherContentRequest request, PHContent content)
	{
		return -1; // resort to default
	}
	
	public void unlockedReward(PHPublisherContentRequest request, PHReward reward)
	{
		Log.d("s3ePlayhavenFacade", "unlockedReward");	
		
		Reward responseData = new Reward();
		
		responseData.name = reward.name;
		responseData.quantity = reward.quantity;
		responseData.receipt = reward.receipt;
		
		s3ePlayhavenFacade.native_contentUnlockReward(responseData);
	}
	
	public void shouldMakePurchase(PHPublisherContentRequest request, PHPurchase purchase)
	{
		Log.d("s3ePlayhavenFacade", "shouldMakePurchase");
		
		Purchase responseData = new Purchase();
		
		//TODO: shouldn't Purchase have a "name" field as well ?
		
		responseData.productIdentifier = purchase.product;
		responseData.quantity = purchase.quantity;
		responseData.receipt = purchase.receipt;
		
		s3ePlayhavenFacade.native_contentMakePurchase(responseData);

	}
}
