/*
java implementation of the s3ePlayhaven extension.

Add android-specific functionality here.

These functions are called via JNI from native code.
*/
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */

import com.ideaworks3d.marmalade.LoaderAPI;
import android.app.Activity;

import com.playhaven.src.publishersdk.content.PHPublisherContentRequest;
import com.playhaven.src.publishersdk.metadata.PHNotificationView;
import com.playhaven.src.publishersdk.open.PHPublisherOpenRequest;
import com.playhaven.src.common.PHConfig;

import android.text.Layout;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;

class s3ePlayhaven
{
    private static final String TAG = "s3ePlayhaven";
    
    private s3ePlayhavenFacade PHFacade;
    
    private int currentHash;

    public int s3ePlayhavenInitWithKeys(String token, String secret)
    {
		final Activity activity = LoaderAPI.getActivity();

		Log.d(TAG,"Setting PHConfig token and secret");

		PHFacade = new s3ePlayhavenFacade(activity,token,secret);
			
		//PHFacade.openRequest(1234);
		
        return 0;
    }
    public void s3ePHRegisterCallback()
    {
        
    }
    public void s3ePHSendAppOpen(String customUDID)
    {
		Log.d(TAG,"openRequest");

		currentHash = (customUDID == null) ? 0 : customUDID.hashCode();
		
		PHFacade.openRequest(currentHash);
    }

    public void s3ePHSendContentRequest(String placement, boolean showOverlayImmediately)
    {
		Log.d(TAG,"contentRequest");

		PHFacade.contentRequest(currentHash, placement);
    }
    
    public void s3ePHPreloadContentRequest(String placement)
    {
    	Log.d(TAG,"preloadRequest");

		PHFacade.preloadRequest(currentHash, placement);
    }

    public void s3ePHCancelAllContentRequests()
    {
    	//NYI
    }
    
    public void s3ePHReportResolution(int resolution)
    {
    	Log.d(TAG,"reportResolution");

    	PHFacade.reportResolution(resolution);
    }

    public void s3ePHSendPublisherIAPTrackingRequest(String product, int quantity,int resolution)
    {
    	Log.d(TAG,"iapTrackingRequest");

    	PHFacade.iapTrackingRequest(product, quantity, resolution);
    }
    
    public int s3ePHShowNotificationView(int x, int y, boolean useGLView, boolean testing)
    {
    	final Activity activity = LoaderAPI.getActivity();
    	ViewGroup viewGroup = (ViewGroup)(activity.getWindow().getDecorView());

    	//TODO: this doesn't display
    	
    	PHNotificationView notifyView = new PHNotificationView(activity,"more_games");
    	
    	viewGroup.addView(notifyView);
    	
    	notifyView.refresh();
        
        return 0;
    }

    public void s3ePHClearNotificationView()
    {
    	//NYI
    }

    public void s3ePHRefreshNotificationView(boolean testing)
    {
    	//NYI
    }
    
    public void s3ePHSetOptOutStatus(boolean on)
    {
    	//NYI
    }
}
