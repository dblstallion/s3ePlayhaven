// Playhaven Marmalade Extension Example
// Note: As with any Marmalade extension this has to be run on a device, the extension will not be loaded in an emulator

#include "IwNUI.h"
#include "s3ePlayhaven.h"
#include "IwDebug.h"
#include "s3e.h"
#include "ExampleUI.h"
#include "IwRandom.h"

bool OnButton1Click(void* data, CButton* button)
{
	ExampleUI *ui = (ExampleUI*)data;
    ui->Log("sending content request for level_complete");
	s3ePHSendContentRequest("level_complete",false);
	return true;
}

bool OnButton2Click(void* data, CButton* button)
{
	ExampleUI *ui = (ExampleUI*)data;
    ui->Log("sending content request for level_failed");
	s3ePHSendContentRequest("level_failed",false);
	return true;
}

bool OnButton3Click(void* data, CButton* button)
{
	ExampleUI *ui = (ExampleUI*)data;
    ui->Log("sending content request for level_up");
	s3ePHSendContentRequest("level_up",true);
	return true;
}

bool OnButton4Click(void* data, CButton* button)
{
	ExampleUI *ui = (ExampleUI*)data;
    ui->Log("cancelling all content requests");
	s3ePHCancelAllContentRequests();
	return true;
}

bool OnButton5Click(void* data, CButton* button)
{
	ExampleUI *ui = (ExampleUI*)data;
    ui->Log("Preloading level_up");
	s3ePHPreloadContentRequest("level_up");
	return true;
}

bool OnButton6Click(void* data, CButton* button)
{
	ExampleUI *ui = (ExampleUI*)data;
    ui->Log("Preloading level_failed");
	s3ePHPreloadContentRequest("level_failed");
	return true;
}

bool OnButton7Click(void* data, CButton* button)
{
	ExampleUI *ui = (ExampleUI*)data;
    ui->Log("Preloading level_up");
	s3ePHPreloadContentRequest("level_up");
	return true;
}

bool OnButton8Click(void* data, CButton* button)
{
	ExampleUI *ui = (ExampleUI*)data;
    ui->Log("Adding notifier badge");
	s3ePHShowNotificationView(10,10,false,true);
	return true;
}

// S3EPH_CONTENT_START callback
// systemData is a const char* to a placement ID
// note: this message will be deallocated once this function quits
int32 ContentStartFn(void *systemData,void *userData)
{
    if ((systemData) && (userData))
    {
		ExampleUI *ui = (ExampleUI*)userData; // the data we passed in when we registered the callback
        const char* msg = (const char*)systemData;
		ui->Log(string_format("s3ePH_CONTENT_START %s",msg));
    }
	return 0; // return is ignored
}

// S3EPH_CONTENT_RECEIVED callback
// systemData is a const char* to a placement ID
// note: this message will be deallocated once this function quits
int32 ContentReceivedFn(void *systemData,void *userData)
{
    if ((systemData) && (userData))
    {
        const char* msg = (const char*)systemData;
 		ExampleUI *ui = (ExampleUI*)userData; // the data we passed in when we registered the callback
		ui->Log(string_format("s3ePH_CONTENT_RECEIVED %s",msg));
    }
	return 0; // return is ignored
}

// S3EPH_CONTENT_WILL_DISPLAY callback
// systemData is a (const s3ePHPublisherContent*)
// note: this message will be deallocated once this function quits
int32 ContentWillDisplayFn(void *systemData,void *userData)
{
    if ((systemData) && (userData))
    {
        const s3ePHPublisherContent* content = (const s3ePHPublisherContent*)systemData;
  		ExampleUI *ui = (ExampleUI*)userData; // the data we passed in when we registered the callback
 		ui->Log(string_format("s3ePH_CONTENT_WILL_DISPLAY %s",content->placement));
    }
	return 0; // return is ignored
}

// S3EPH_CONTENT_DID_DISPLAY callback
// systemData is a (const s3ePHPublisherContent*)
// note: this message will be deallocated once this function quits
int32 ContentDidDisplayFn(void *systemData,void *userData)
{
    if ((systemData) && (userData))
    {
        const s3ePHPublisherContent* content = (const s3ePHPublisherContent*)systemData;
 		ExampleUI *ui = (ExampleUI*)userData; // the data we passed in when we registered the callback
		ui->Log(string_format("s3ePH_CONTENT_DID_DISPLAY %s",content->placement));
    }
	return 0; // return is ignored
}

// S3EPH_CONTENT_DISMISSING callback
// systemData is a (const s3ePHPublisherContent*)
// note: this message will be deallocated once this function quits
int32 ContentDismissingFn(void *systemData,void *userData)
{
    if ((systemData) && (userData))
    {
        const s3ePHPublisherContent* content = (const s3ePHPublisherContent*)systemData;
  		ExampleUI *ui = (ExampleUI*)userData; // the data we passed in when we registered the callback
 		ui->Log(string_format("s3ePH_CONTENT_DISMISSING %s",content->placement));
    }
	return 0; // return is ignored
}

// S3EPH_CONTENT_FAILED callback
// systemData is a (const char*) containing the placement and error message concatenated
// note: this message will be deallocated once this function quits
int32 ContentFailedFn(void *systemData,void *userData)
{
    if ((systemData) && (userData))
    {
        const char* msg = (const char*)systemData;
  		ExampleUI *ui = (ExampleUI*)userData; // the data we passed in when we registered the callback
		ui->Log(string_format("s3ePH_CONTENT_FAILED %s",msg));
    }
	return 0; // return is ignored
}

// S3EPH_CONTENT_UNLOCK_REWARD callback
// systemData is a (const s3ePHReward*)
// note: this message will be deallocated once this function quits
int32 ContentUnlockRewardFn(void *systemData,void *userData)
{
    if ((systemData) && (userData))
    {
        const s3ePHReward* reward = (const s3ePHReward*)systemData;
		ExampleUI *ui = (ExampleUI*)userData; // the data we passed in when we registered the callback
		ui->Log(string_format("s3ePH_CONTENT_UNLOCK_REWARD %s awarding $%d",reward->name,reward->quantity));
		ui->AddMoney(reward->quantity);
     }
	return 0; // return is ignored
}

// S3EPH_CONTENT_MAKE_PURCHASE callback
// systemData is a (const s3ePHPurchase*)
// note: this message will be deallocated once this function quits
int32 ContentMakePurchaseFn(void *systemData,void *userData)
{
    if ((systemData) && (userData))
    {
        const s3ePHPurchase* purchase = (const s3ePHPurchase*)systemData;
		ExampleUI *ui = (ExampleUI*)userData; // the data we passed in when we registered the callback
		ui->Log(string_format("S3EPH_CONTENT_MAKE_PURCHASE %s",purchase->productIdentifier));
    }
	return 0; // return is ignored
}

int32 UnpauseCallback(void *systemData,void *userData)
{
    s3ePHSendAppOpen(NULL); // notify Playhaven when the application comes back to the foreground
    return 0;
}

#define ANDROID_PH_APP_KEY    "c179e2021c2446dfac774ebb5fc1708c" // your app keys from dashboard.playhaven.com
#define ANDROID_PH_APP_SECRET "32895dbe2ef949dfad45d4872850dcb1"

#define IOS_PH_APP_KEY    "1ffeb171f99e403ea289355803e384bb" // your app keys from dashboard.playhaven.com
#define IOS_PH_APP_SECRET "6477a44574d7448ca8bff7a2704d5820"

int main()
{
	// seed RNG
	int32 ms = (int32)s3eTimerGetMs();
	IwRandSeed(ms);

	// create our Marmalade UI interface
    ExampleUI *ui = new ExampleUI(); 
	ui->Log("main()");

	s3ePHRegisterCallback(S3EPH_CONTENT_START,ContentStartFn,ui);
	s3ePHRegisterCallback(S3EPH_CONTENT_RECEIVED,ContentReceivedFn,ui);
	s3ePHRegisterCallback(S3EPH_CONTENT_WILL_DISPLAY,ContentWillDisplayFn,ui);
	s3ePHRegisterCallback(S3EPH_CONTENT_DID_DISPLAY,ContentDidDisplayFn,ui);
	s3ePHRegisterCallback(S3EPH_CONTENT_DISMISSING,ContentDismissingFn,ui);
	s3ePHRegisterCallback(S3EPH_CONTENT_FAILED,ContentFailedFn,ui);
	s3ePHRegisterCallback(S3EPH_CONTENT_UNLOCK_REWARD,ContentUnlockRewardFn,ui);
	s3ePHRegisterCallback(S3EPH_CONTENT_MAKE_PURCHASE,ContentMakePurchaseFn,ui);

	// Initialise Playhaven - buttons will enable when complete
	if (s3eDeviceGetInt(S3E_DEVICE_OS) == S3E_OS_ID_ANDROID)
	{
		//for android
		ui->Log("Sending Android Keys");
	    s3ePlayhavenInitWithKeys(ANDROID_PH_APP_KEY,ANDROID_PH_APP_SECRET);	}
	else
	{
		//for iOS:
		ui->Log("Sending IOS Keys");
	    s3ePlayhavenInitWithKeys(IOS_PH_APP_KEY,IOS_PH_APP_SECRET);
	}

	// send the app open event for Playhaven to track
    s3ePHSendAppOpen(NULL);
    // register a callback to receive UNPAUSE events to send future AppOpen events
    s3eDeviceRegister(S3E_DEVICE_UNPAUSE,UnpauseCallback,NULL);

	ui->EnableAllButtons(true);

	// create the Unit Test singleton
	//gTests = new UnitTests(ui); // DH: Not implemented for this extension yet

    // run the app
	while (1)
	{
		//gTests->Update(); // update the tests if they're running
		ui->Update(); // update the UI
		s3eDeviceYield();
	}

    return 0;
}

