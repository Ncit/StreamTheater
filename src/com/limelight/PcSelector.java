package com.limelight;

import com.vrmatter.streamtheater.R;
import com.vrmatter.streamtheater.MainActivity;

import java.io.FileNotFoundException;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import com.limelight.binding.PlatformBinding;
import com.limelight.binding.crypto.AndroidCryptoProvider;
import com.limelight.computers.ComputerManagerListener;
import com.limelight.computers.ComputerManagerService;
import com.limelight.nvstream.http.ComputerDetails;
import com.limelight.nvstream.http.NvHTTP;
import com.limelight.nvstream.http.PairingManager;

import android.app.Service;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.preference.PreferenceManager;

public class PcSelector {
	private MainActivity activity;
	private List<ComputerDetails> compList;
	
    private ComputerManagerService.ComputerManagerBinder managerBinder;
    private boolean freezeUpdates, runningPolling;
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

                    // Start updates
                    startComputerUpdates();

                    // Force a keypair to be generated early to avoid discovery delays
                    new AndroidCryptoProvider(activity).getClientCertificate();
                }
            }.start();
        }

        public void onServiceDisconnected(ComponentName className) {
            managerBinder = null;
        }
    };

    public PcSelector(MainActivity creatingActivity) {
		activity = creatingActivity;

        // Bind to the computer manager service
        activity.bindService(new Intent(activity, ComputerManagerService.class), serviceConnection,
                Service.BIND_AUTO_CREATE);
        compList = new ArrayList<ComputerDetails>();

        // Set up default preferences
        PreferenceManager.setDefaultValues(activity, R.xml.preferences, false);
    }

    public void startComputerUpdates() {
        if (managerBinder != null) {
            if (runningPolling) {
                return;
            }

            freezeUpdates = false;
            managerBinder.startPolling(new ComputerManagerListener() {
                @Override
                public void notifyComputerUpdated(final ComputerDetails details) {
                    if (!freezeUpdates) {
                    	activity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                updateComputer(details);
                            }
                        });
                    }
                }
            });
            runningPolling = true;
        }
    }

    public void stopComputerUpdates(boolean wait) {
        if (managerBinder != null) {
            if (!runningPolling) {
                return;
            }

            freezeUpdates = true;

            managerBinder.stopPolling();

            if (wait) {
                managerBinder.waitForPollingStopped();
            }

            runningPolling = false;
        }
    }
    
    public ComputerDetails findByUUID(String compUUID)
    {
        int i = 0;
        for (; i< compList.size(); i++)
        {
        	if(compList.get(i).uuid.equals(UUID.fromString(compUUID)))
        	{
        		return compList.get(i);
        	}
        	
        }
    	return null;
    }
    
    static final int PS_NOT_PAIRED = 0;
    static final int PS_PAIRED = 1;
	static final int PS_PIN_WRONG = 2;
	static final int PS_FAILED = 3;
    public int pairStateFromUUID(final String compUUID)
    {
    	ComputerDetails comp = findByUUID(compUUID);
    	if(comp != null && comp.pairState != null)
    	{
            if(comp.pairState == PairingManager.PairState.NOT_PAIRED)		return PS_NOT_PAIRED;
            else if(comp.pairState == PairingManager.PairState.PAIRED)		return PS_PAIRED;
            else if(comp.pairState == PairingManager.PairState.PIN_WRONG)	return PS_PIN_WRONG;
            else return PS_FAILED;
    	} 
        return PS_FAILED;
    }
    
    static final int CS_ONLINE = 0;
    static final int CS_OFFLINE = 1;
    static final int CS_UNKNOWN = 2;
    public int stateFromUUID(final String compUUID)
    {
    	ComputerDetails comp = findByUUID(compUUID);
    	if(comp != null)
    	{
    		if(comp.state == ComputerDetails.State.ONLINE)	return CS_ONLINE;
    		else if(comp.state == ComputerDetails.State.OFFLINE)	return CS_OFFLINE;
    		else return CS_UNKNOWN;
        }
        return CS_UNKNOWN;
    }
    
    static final int RS_LOCAL = 0;
    static final int RS_REMOTE = 1;
    static final int RS_OFFLINE = 2;
    static final int RS_UNKNOWN = 3;
    public int reachabilityStateFromUUID(final String compUUID)
    {
    	ComputerDetails comp = findByUUID(compUUID);
    	if(comp != null && comp.reachability != null)
    	{
    		return comp.reachability.ordinal();
        }
        return RS_UNKNOWN;
    }

    public void pairWithUUID(final String compUUID)
    {
		doPair(findByUUID(compUUID));
    }
  
    private void doPair(final ComputerDetails computer) {
        if (computer.reachability == ComputerDetails.Reachability.OFFLINE) {
        	MainActivity.nativeShowError(activity.appPtr, activity.getResources().getString(R.string.pair_pc_offline));
            return;
        }
        if (computer.runningGameId != 0) {
            MainActivity.nativeShowError(activity.appPtr, activity.getResources().getString(R.string.pair_pc_ingame));
            return;
        }
        if (managerBinder == null) {
            MainActivity.nativeShowError(activity.appPtr, activity.getResources().getString(R.string.error_manager_not_running));
            return;
        }

        MainActivity.nativeShowError(activity.appPtr, activity.getResources().getString(R.string.pairing));
        new Thread(new Runnable() {
            @Override
            public void run() {
                NvHTTP httpConn;
                String message;
                boolean success = false;
                try {
                    // Stop updates and wait while pairing
                    stopComputerUpdates(true);

                    InetAddress addr = null;
                    if (computer.reachability == ComputerDetails.Reachability.LOCAL) {
                        addr = computer.localIp;
                    }
                    else if (computer.reachability == ComputerDetails.Reachability.REMOTE) {
                        addr = computer.remoteIp;
                    }

                    httpConn = new NvHTTP(addr,
                            managerBinder.getUniqueId(),
                            PlatformBinding.getDeviceName(),
                            PlatformBinding.getCryptoProvider(activity));
                    if (httpConn.getPairState() == PairingManager.PairState.PAIRED) {
                        // Don't display any toast, but open the app list
                        message = null;
                        success = true;
                    }
                    else {
                        final String pinStr = PairingManager.generatePinString();

                        // Spin the dialog off in a thread because it blocks
                        MainActivity.nativeShowPair(activity.appPtr, activity.getResources().getString(R.string.pair_pairing_msg)+" "+pinStr);

                        PairingManager.PairState pairState = httpConn.pair(pinStr);
                        if (pairState == PairingManager.PairState.PIN_WRONG) {
                            message = activity.getResources().getString(R.string.pair_incorrect_pin);
                        }
                        else if (pairState == PairingManager.PairState.FAILED) {
                            message = activity.getResources().getString(R.string.pair_fail);
                        }
                        else if (pairState == PairingManager.PairState.PAIRED) {
                            // Just navigate to the app view without displaying a toast
                            message = null;
                            success = true;
                        }
                        else {
                            // Should be no other values
                            message = null;
                        }
                    }
                } catch (UnknownHostException e) {
                    message = activity.getResources().getString(R.string.error_unknown_host);
                } catch (FileNotFoundException e) {
                    message = activity.getResources().getString(R.string.error_404);
                } catch (Exception e) {
                    e.printStackTrace();
                    message = e.getMessage();
                }

                if (success == true)
                    MainActivity.nativePairSuccess( activity.appPtr );

                if (message != null)
                {
                	MainActivity.nativeShowError(activity.appPtr, message);
                }
                // Start polling again
                startComputerUpdates();
            }
        }).start();
    }

    private void updateComputer(ComputerDetails details) {
        int i = 0;
        boolean found = false;
        for (; i< compList.size(); i++)
        {
        	if(compList.get(i).uuid == details.uuid)
        	{
        		found = true;
        		break;
        	}
        	
        }
        if(found)
        	compList.set(i, details);
        else
        	compList.add(details);
        
        LimeLog.info("Found PC " + details.toString());
        int pairInt=0;
        if(details.pairState == PairingManager.PairState.NOT_PAIRED)		pairInt= PS_NOT_PAIRED;
        else if(details.pairState == PairingManager.PairState.PAIRED)		pairInt= PS_PAIRED;
        else if(details.pairState == PairingManager.PairState.PIN_WRONG)	pairInt= PS_PIN_WRONG;
        else pairInt= PS_FAILED;
        MainActivity.nativeAddPc(activity.appPtr, details.name, details.uuid.toString(), pairInt, managerBinder.getUniqueId());
    }

    public class ComputerObject {
        public ComputerDetails details;

        public ComputerObject(ComputerDetails details) {
            if (details == null) {
                throw new IllegalArgumentException("details must not be null");
            }
            this.details = details;
        }

        @Override
        public String toString() {
            return details.name;
        }
    }
}
