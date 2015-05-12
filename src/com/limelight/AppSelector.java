package com.limelight;

import com.vrmatter.streamtheater.MainActivity;
import com.vrmatter.streamtheater.R;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.util.List;
import java.util.UUID;

import com.limelight.binding.PlatformBinding;
import com.limelight.computers.ComputerManagerListener;
import com.limelight.computers.ComputerManagerService;
import com.limelight.nvstream.http.ComputerDetails;
import com.limelight.nvstream.http.NvApp;
import com.limelight.nvstream.http.NvHTTP;
import com.limelight.utils.CacheHelper;
import com.limelight.utils.ServerHelper;
import com.limelight.utils.SpinnerDialog;

import android.app.Service;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.IBinder;
import android.util.Log;

public class AppSelector {
	public static final String TAG = "AppSelector";
	private MainActivity activity;
    private String uuidString;
    private List<NvApp> appList;

    private ComputerDetails computer;
    private ComputerManagerService.ApplistPoller poller;
    private SpinnerDialog blockingLoadSpinner;
    private String lastRawApplist;
    private int lastRunningAppId;
    private boolean suspendGridUpdates;

    public final static String NAME_EXTRA = "Name";
    public final static String UUID_EXTRA = "UUID";

    private ComputerManagerService.ComputerManagerBinder managerBinder;
    private final ServiceConnection serviceConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder binder) {
            final ComputerManagerService.ComputerManagerBinder localBinder =
                    ((ComputerManagerService.ComputerManagerBinder)binder);

            // Wait in a separate thread to avoid stalling the UI
            new Thread() {
                @Override
                public void run() {
                    // Wait for the binder to be ready
                    localBinder.waitForReady();

                    // Now make the binder visible
                    managerBinder = localBinder;

                    // Get the computer object
                    computer = managerBinder.getComputer(UUID.fromString(uuidString));

                    // Start updates
                    startComputerUpdates();

                    // Load the app grid with cached data (if possible)
                    populateAppGridWithCache();
                }
            }.start();
        }

        public void onServiceDisconnected(ComponentName className) {
            managerBinder = null;
        }
    };

    private void startComputerUpdates() {
        if (managerBinder == null) {
            return;
        }

        managerBinder.startPolling(new ComputerManagerListener() {
            @Override
            public void notifyComputerUpdated(ComputerDetails details) {
                // Do nothing if updates are suspended
                if (suspendGridUpdates) {
                    return;
                }

                // Don't care about other computers
                if (!details.uuid.toString().equalsIgnoreCase(uuidString)) {
                    return;
                }

                if (details.state == ComputerDetails.State.OFFLINE) {
                    // The PC is unreachable now
                	activity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            // Display a toast to the user and quit the activity
                            MainActivity.nativeShowError(activity.appPtr, activity.getResources().getString(R.string.lost_connection));
                        }
                    });

                    return;
                }

                // App list is the same or empty
                if (details.rawAppList == null || details.rawAppList.equals(lastRawApplist)) {

                    // Let's check if the running app ID changed
                    if (details.runningGameId != lastRunningAppId) {
                        // Update the currently running game using the app ID
                        lastRunningAppId = details.runningGameId;
                    }

                    return;
                }

                lastRunningAppId = details.runningGameId;
                lastRawApplist = details.rawAppList;

                try {
                    updateAppList(NvHTTP.getAppListByReader(new StringReader(details.rawAppList)));

                    if (blockingLoadSpinner != null) {
                        blockingLoadSpinner.dismiss();
                        blockingLoadSpinner = null;
                    }
                } catch (Exception ignored) {}
            }
        });

        if (poller == null) {
            poller = managerBinder.createAppListPoller(computer);
        }
        poller.start();
    }

    //TODO: We should be closing down this properly
    private void stopComputerUpdates() {
        if (poller != null) {
            poller.stop();
        }

        if (managerBinder != null) {
            managerBinder.stopPolling();
        }
    }

    public AppSelector(MainActivity startingActivity, String computerUUID) {
    	activity = startingActivity;

        uuidString = computerUUID;

        // Bind to the computer manager service
        activity.bindService(new Intent(activity, ComputerManagerService.class), serviceConnection,
                Service.BIND_AUTO_CREATE);
    }

    private void populateAppGridWithCache() {
        try {
            // Try to load from cache
            lastRawApplist = CacheHelper.readInputStreamToString(CacheHelper.openCacheFileForInput(activity.getCacheDir(), "applist", uuidString));
            List<NvApp> applist = NvHTTP.getAppListByReader(new StringReader(lastRawApplist));
            updateAppList(applist);
            LimeLog.info("Loaded applist from cache");
        } catch (Exception e) {
            if (lastRawApplist != null) {
                LimeLog.warning("Saved applist corrupted: "+lastRawApplist);
                e.printStackTrace();
            }
            LimeLog.info("Loading applist from the network");
            // We'll need to load from the network
            //loadAppsBlocking();
        }
    }

    private void loadAppsBlocking() {
        blockingLoadSpinner = SpinnerDialog.displayDialog(activity, activity.getResources().getString(R.string.applist_refresh_title),
                activity.getResources().getString(R.string.applist_refresh_msg), true);
    }
    
    public Bitmap createAppPoster(final ComputerDetails comp, final int appId)
    {
    	LimeLog.info("Network asset load starting");
		NvApp app = null;
		
		for(int i=0;i<appList.size();i++) {
			if ( appList.get(i).getAppId() == appId ) {
				app = appList.get(i);
				break;
			}
		}
		if(app == null) return null;
		
		NvHTTP http = new NvHTTP(ServerHelper.getCurrentAddressFromComputer(comp), 
				managerBinder.getUniqueId(),
                PlatformBinding.getDeviceName(),
				PlatformBinding.getCryptoProvider(activity));
		
		InputStream in = null;
		try {
			in = http.getBoxArt(app);
		} catch (IOException e) {}
		
		if (in != null) {
			LimeLog.info("Network asset load complete: " + app.getAppName());
		}
		else {
			LimeLog.info("Network asset load failed: " + app.getAppName());
		}
		
		Bitmap bmp = null;
		try {
			bmp = BitmapFactory.decodeStream(in);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			Log.e("HELP!","Bitmap decode exception!", e);
		}

		LimeLog.info("BMPification done");

		return bmp;
    }

    //TODO: We'll want this eventually to display the current running app from the selection menus
    private int getRunningAppId() {
        int runningAppId = -1;
        for (int i = 0; i < appList.size(); i++) {
            NvApp app = appList.get(i);
            if (app.getIsRunning()) {
                runningAppId = app.getAppId();
                break;
            }
        }
        return runningAppId;
    }

    private void updateAppList(final List<NvApp> newAppList) {
		boolean updated = false;
		appList = newAppList;
		// First handle app updates and additions
		for (NvApp app : appList) {
		    boolean foundExistingApp = false;
		
		    // Try to update an existing app in the list first
		    for (int i = 0; i < appList.size(); i++) {
		        NvApp existingApp = appList.get(i);
		        if (existingApp.getAppId() == app.getAppId()) {
		            // Found the app; update its properties
		            if (existingApp.getIsRunning() != app.getIsRunning()) {
		                existingApp.setIsRunning(app.getIsRunning());
		                updated = true;
		            }
		            if (!existingApp.getAppName().equals(app.getAppName())) {
		                existingApp.setAppName(app.getAppName());
		                updated = true;
		            }
		
		            foundExistingApp = true;
		            break;
		        }
		    }
		
		    if (!foundExistingApp) {
		        // This app must be new
		    	appList.add(app);
		        updated = true;
		    }
		    String fileName = activity.getFilesDir() + "/gameposters/" + app.getAppName() + ".png";
		    //TODO: Get this working
//		    File posterFile = new File( fileName );
//		    if(!posterFile.exists())
//		    {
//		    	activity.createVideoThumbnail(computer.uuid.toString(), app.getAppId(), fileName, 228, 344);
//		    }
		    
		    MainActivity.nativeAddApp(activity.appPtr, app.getAppName(), fileName, app.getAppId());
		}
		
		// Next handle app removals
		int i = 0;
		while (i < appList.size()) {
		    boolean foundExistingApp = false;
		    NvApp existingApp = appList.get(i);
		
		    // Check if this app is in the latest list
		    for (NvApp app : appList) {
		        if (existingApp.getAppId() == app.getAppId()) {
		            foundExistingApp = true;
		            break;
		        }
		    }
		
		    // This app was removed in the latest app list
		    if (!foundExistingApp) {
		    	appList.remove(existingApp);
		    	MainActivity.nativeRemoveApp(activity.appPtr, existingApp.getAppId());
		        updated = true;
		
		        // Check this same index again because the item at i+1 is now at i after
		        // the removal
		        continue;
		    }
		
		    // Move on to the next item
		    i++;
		}
		
		if (updated) {
		    
		}
    }
}
