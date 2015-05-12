/************************************************************************************

Filename    :   MainActivity.java
Content     :   Media player controller.
Created     :   September 3, 2013
Authors     :	Jim Dose, based on a fork of MainActivity.java from VrVideo by John Carmack.   

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/
package com.vrmatter.streamtheater;

import java.io.File;
import java.io.IOException;
import java.io.FileOutputStream;
import java.lang.IllegalStateException;
import java.lang.IllegalArgumentException;
import java.lang.System;

import android.content.SharedPreferences.Editor;
import android.graphics.Bitmap;
import android.graphics.SurfaceTexture;
import android.graphics.Matrix;
import android.media.MediaMetadataRetriever;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnSeekCompleteListener;
import android.media.ThumbnailUtils;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.content.Context;
import android.media.AudioManager;
import com.oculusvr.vrlib.VrActivity;
import com.oculusvr.vrlib.VrLib;
import android.content.Intent;

public class MainActivity extends VrActivity implements SurfaceHolder.Callback,
		MediaPlayer.OnVideoSizeChangedListener, 
		AudioManager.OnAudioFocusChangeListener		
{
	public static final String TAG = "Cinema";

	/** Load jni .so on initialization */
	static 
	{
		Log.d( TAG, "LoadLibrary" );
		System.loadLibrary( "cinema" );
	}
	
	public static native void 			nativeSetVideoSize( long appPtr, int width, int height, int rotation, int duration );
	public static native SurfaceTexture nativePrepareNewVideo( long appPtr );
	public static native long nativeSetAppInterface( VrActivity act, String fromPackageNameString, String commandString, String uriString );

	public static final int MinimumRemainingResumeTime = 60000;	// 1 minute
	public static final int MinimumSeekTimeForResume = 60000;	// 1 minute

	String 				currentMovieFilename;
	
	boolean				playbackFinished = true;
	boolean				playbackFailed = false;
	
	private boolean 	waitingForSeek = false;
	private boolean 	haveSeekWaiting = false;
	private int 		nextSeekPosition = 0;
	private long       	startTime = 0;
	
	SurfaceTexture 		movieTexture = null;
	Surface 			movieSurface = null;

	MediaPlayer 		mediaPlayer = null;
	AudioManager 		audioManager = null;

	@Override
	protected void onCreate( Bundle savedInstanceState ) 
	{
		Log.d( TAG, "onCreate" );
		super.onCreate( savedInstanceState );

		Intent intent = getIntent();
		String commandString = VrLib.getCommandStringFromIntent( intent );
		String fromPackageNameString = VrLib.getPackageStringFromIntent( intent );
		String uriString = VrLib.getUriStringFromIntent( intent );

		appPtr = nativeSetAppInterface( this, fromPackageNameString, commandString, uriString );

		audioManager = ( AudioManager )getSystemService( Context.AUDIO_SERVICE );
	}

	@Override
	protected void onDestroy() 
	{
		// Abandon audio focus if we still hold it
		releaseAudioFocus();
		super.onDestroy();
    }

	@Override
	protected void onPause() 
	{
		Log.d( TAG, "onPause()" );
		
		pauseMovie();

		super.onPause();
	}

	protected void onResume() 
	{
		Log.d( TAG, "onResume()" );
		super.onResume();
	}
	
    public void onAudioFocusChange( int focusChange ) 
    {
		switch( focusChange ) 
		{
		case AudioManager.AUDIOFOCUS_GAIN:
			// resume() if coming back from transient loss, raise stream volume if duck applied
			Log.d( TAG, "onAudioFocusChangedListener: AUDIOFOCUS_GAIN" );
			break;
		case AudioManager.AUDIOFOCUS_LOSS:				// focus lost permanently
			// stop() if isPlaying
			Log.d( TAG, "onAudioFocusChangedListener: AUDIOFOCUS_LOSS" );		
			break;
		case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT:	// focus lost temporarily
			// pause() if isPlaying
			Log.d( TAG, "onAudioFocusChangedListener: AUDIOFOCUS_LOSS_TRANSIENT" );	
			break;
		case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK:	// focus lost temporarily
			// lower stream volume
			Log.d( TAG, "onAudioFocusChangedListener: AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK" );		
			break;
		default:
			break;
		}
	}
    
	private int getRotationFromMetadata( final String filePath ) 
	{
		MediaMetadataRetriever metaRetriever = new MediaMetadataRetriever();
		metaRetriever.setDataSource( filePath );
		String value = metaRetriever.extractMetadata( MediaMetadataRetriever.METADATA_KEY_VIDEO_ROTATION );
		metaRetriever.release();

		if ( value == null ) 
		{
			return 0;
		}

		if ( value.equals( "0" ) ) 
		{
			return 0;
		} 
		else if ( value.equals( "90" ) ) 
		{
			return 90;
		} 
		else if ( value.equals( "180" ) ) 
		{
			return 180;
		} 
		else if ( value.equals( "270" ) ) 
		{
			return 270;
		} 

		return 0;
	}
	
	public void onVideoSizeChanged( MediaPlayer mp, int width, int height ) 
	{
		Log.v( TAG, String.format( "onVideoSizeChanged: %dx%d", width, height ) );
		int rotation = getRotationFromMetadata( currentMovieFilename );
		int duration = getDuration();
		nativeSetVideoSize( appPtr, width, height, rotation, duration );
	}

	private void requestAudioFocus()
	{
		// Request audio focus
		int result = audioManager.requestAudioFocus( this, AudioManager.STREAM_MUSIC,
			AudioManager.AUDIOFOCUS_GAIN );
		if ( result == AudioManager.AUDIOFOCUS_REQUEST_GRANTED ) 
		{
			Log.d(TAG,"startMovie(): GRANTED audio focus");
		} 
		else if ( result == AudioManager.AUDIOFOCUS_REQUEST_FAILED ) 
		{
			Log.d(TAG,"startMovie(): FAILED to gain audio focus");
		}				
	}
	
	private void releaseAudioFocus()
	{
		audioManager.abandonAudioFocus( this );
	}

	/*
	 * Whenever we pause or switch to another movie, save the current movie
	 * position so we will return there when the same file is viewed again.
	 */
	private void saveCurrentMovieLocation() 
	{
		Log.d(TAG, "saveCurrentMovieLocation()" );
		if ( mediaPlayer == null ) 
		{
			return;
		}
		if ( currentMovieFilename.length() < 1 ) 
		{
			return;
		}

		int duration = 0;
		int currentPos = 0;
		
		try
		{
			duration = mediaPlayer.getDuration();
			currentPos = mediaPlayer.getCurrentPosition();
		}
		
		catch( IllegalStateException ise )
    	{
    		// Can be thrown by the media player if state changes while we're being
    		// queued for execution on the main thread
			Log.d( TAG, "saveCurrentMovieLocation(): Caught IllegalStateException" );
    	}

		if ( playbackFinished )
		{
			currentPos = 0;
		}
		
		// Save the current movie now that it was successfully started
		Editor edit = getPreferences( MODE_PRIVATE ).edit();
		Log.d(TAG, "set resume point: " + currentMovieFilename );
		Log.d(TAG, "pos: " + currentPos );
		Log.d(TAG, "len: " + duration );
		edit.putInt( currentMovieFilename + "_pos",	currentPos );
		edit.putInt( currentMovieFilename + "_len",	duration );
		edit.commit();
	}

	private String fileNameFromPathName( String pathname ) 
	{
		File f = new File( pathname );
		return f.getName();
	}
	
	private void Fail( final String message )
	{
		Log.e(TAG, message );
		mediaPlayer.release();
		mediaPlayer = null;
		playbackFinished = true;
		playbackFailed = true;
		releaseAudioFocus();		
	}
	
	// ==================================================================================
	//
	//  Callable from native code
	//
	// ==================================================================================

	public String getExternalCacheDirectory() 
	{
		return getExternalCacheDir().getAbsolutePath();
	}
	
	public boolean createVideoThumbnail( final String videoFilePath, final String outputFilePath, final int width, final int height )
	{
		Log.e( TAG, "Create video thumbnail: " + videoFilePath + "\noutput path: " + outputFilePath );
		Bitmap bmp = ThumbnailUtils.createVideoThumbnail( videoFilePath,  MediaStore.Images.Thumbnails.MINI_KIND );
		if ( bmp == null )
		{
			return false;
		}

		float desiredAspectRatio = ( float )width / ( float )height;
		float aspectRatio = ( float )bmp.getWidth() / ( float )bmp.getHeight();
		
		int cropWidth = bmp.getWidth();
		int cropHeight = bmp.getHeight();
		boolean shouldCrop = false;
		
		if ( aspectRatio < desiredAspectRatio )
		{
			cropWidth = bmp.getWidth();
			cropHeight = ( int )( ( float )cropWidth / desiredAspectRatio );
			shouldCrop = true;
		}
		else if ( aspectRatio > desiredAspectRatio )
		{
			cropHeight = bmp.getHeight();
			cropWidth = ( int )( ( float )cropHeight * desiredAspectRatio );
			shouldCrop = true;
		}
		
		if ( shouldCrop )
		{
			int cropX = ( bmp.getWidth() - cropWidth ) / 2;
			int cropY = ( bmp.getHeight() - cropHeight ) / 2;
			
			try 
			{
				Bitmap croppedBmp = Bitmap.createBitmap( bmp, cropX, cropY, cropWidth, cropHeight, new Matrix(), false );
				if ( croppedBmp == null )
				{
					return false;
				}
				
				bmp = croppedBmp;
			}
			
			catch ( Exception e ) 
			{
				Log.e( TAG, "Cropping video thumbnail failed: " + e.getMessage() );
				return false;
			}
		}
		
		boolean failed = false;
		FileOutputStream out = null;
		try 
		{
			int sep = outputFilePath.lastIndexOf( '/' );
			File directory = new File( outputFilePath.substring( 0, sep ) );
			if ( directory.mkdirs() )
			{
				Log.d(TAG, "Created directory: " + directory );
			}
		    out = new FileOutputStream( outputFilePath );
		    bmp.compress( Bitmap.CompressFormat.PNG, 100, out );
		}
		
		catch ( Exception e ) 
		{
			failed = true;
			Log.e( TAG, "Writing video thumbnail failed: " + e.getMessage() );
		}
		
		finally 
		{
		    try 
		    {
		        if ( out != null ) 
		        {
		            out.close();
		        }
		    } 
		    
		    catch( IOException e ) 
		    {
				failed = true;
				Log.e( TAG, "Closing video thumbnail failed: " + e.getMessage() );
		    }
		}	
		
		if ( !failed )
		{
			Log.e( TAG, "Wrote " + outputFilePath );
		}
		
		return !failed;
	}

	public boolean checkForMovieResume( final String pathName ) 
	{
		Log.d( TAG, "checkForMovieResume: " + pathName );

		try
		{
			final int seekPos = getPreferences( MODE_PRIVATE ).getInt( pathName + "_pos", 0 );
			final int duration = getPreferences( MODE_PRIVATE ).getInt( pathName + "_len", -1 );
		
			Log.d(TAG, "Saved Location: " + seekPos );
			Log.d(TAG, "Saved duration: " + duration );
			
			if ( seekPos < MinimumSeekTimeForResume )
			{
				Log.d(TAG, "below minimum.  Restart movie." );
				return false;
			}
			
			// early versions didn't save a duration, so if we don't have one, it's ok to resume
			if ( duration == -1 )
			{
				Log.d(TAG, "No duration.  Resume movie." );
				return true;
			}
			
			if ( seekPos > ( duration - MinimumRemainingResumeTime ) )
			{
				Log.d(TAG, "Past maximum.  Restart movie." );
				return false;
			}
			
			Log.d(TAG, "Resume movie." );		
			return true;
		}
		
		catch ( IllegalStateException t )
		{
			Log.e( TAG, "checkForMovieResume caught exception: " + t.getMessage() );
			return false;
		}
	}

	public boolean isPlaying()
	{
		if ( mediaPlayer != null ) 
		{
			try
			{
				return mediaPlayer.isPlaying();
			}
			
			catch ( IllegalStateException t )
			{
				Log.e(TAG, "isPlaying() caught illegalStateException" );
				return false;
			}
		}
		return false;
	}

	public boolean isPlaybackFinished()
	{
		return playbackFinished;
	}
	
	public boolean hadPlaybackError()
	{
		return playbackFailed;
	}
	
	public int getPosition()
	{
		if ( mediaPlayer != null ) 
		{
			try
			{
				return mediaPlayer.getCurrentPosition();
			}
			catch( IllegalStateException ise )
        	{
        		// Can be thrown by the media player if state changes while we're being
        		// queued for execution on the main thread
				Log.d( TAG, "getPosition(): Caught illegalStateException" );
				return 0;
        	}
		}
		return 0;	
	}
	
	public int getDuration()
	{
		if ( mediaPlayer != null ) 
		{
			try
			{
				return mediaPlayer.getDuration();
			}
			catch( IllegalStateException ise )
        	{
        		// Can be thrown by the media player if state changes while we're being
        		// queued for execution on the main thread
				Log.d( TAG, "getDuration(): Caught illegalStateException" );
				return 0;
        	}
		}
		return 0;	
	}

	public void setPosition( int positionMilliseconds )
	{
		try
		{
			if ( mediaPlayer != null ) 
			{
				boolean wasPlaying = isPlaying();
				if ( wasPlaying ) 
				{
					mediaPlayer.pause();
				}
				int duration = mediaPlayer.getDuration();
				int newPosition = positionMilliseconds;
				if ( newPosition >= duration ) 
				{
					// pause if seeking past end
					Log.d( TAG, "seek past end" );
					mediaPlayer.seekTo( duration );
					return;
				}
				if ( newPosition < 0 ) 
				{
					newPosition = 0;
				}
				
				if ( waitingForSeek )
				{
					haveSeekWaiting = true;
					nextSeekPosition = newPosition;
				}
				else
				{
					waitingForSeek = true;
					Log.d(TAG, "seek started");
					mediaPlayer.seekTo( newPosition );
				}
	
				if ( wasPlaying ) 
				{
					mediaPlayer.start();
				}
			}
		}

		catch( IllegalStateException ise )
		{
			// Can be thrown by the media player if state changes while we're being
			// queued for execution on the main thread
			Log.d( TAG, "setPosition(): Caught illegalStateException" );
		}
	}
	
	public void seekDelta( int deltaMilliseconds ) 
	{
		try
		{
			if ( mediaPlayer != null ) 
			{
				boolean wasPlaying = isPlaying();
				if ( wasPlaying ) 
				{
					mediaPlayer.pause();
				}
				
				int position = mediaPlayer.getCurrentPosition();
				int duration = mediaPlayer.getDuration();
				int newPosition = position + deltaMilliseconds;
				if ( newPosition >= duration ) 
				{
					// pause if seeking past end
					Log.d( TAG, "seek past end" );
					mediaPlayer.seekTo( duration );
					return;
				}
				if ( newPosition < 0 ) 
				{
					newPosition = 0;
				}
				
				if ( waitingForSeek )
				{
					haveSeekWaiting = true;
					nextSeekPosition = newPosition;
				}
				else
				{
					waitingForSeek = true;
					Log.d( TAG, "seek started" );
					mediaPlayer.seekTo( newPosition );
				}
	
				if ( wasPlaying ) 
				{
					mediaPlayer.start();
				}
			}
		}
		
		catch( IllegalStateException ise )
    	{
    		// Can be thrown by the media player if state changes while we're being
    		// queued for execution on the main thread
			Log.d( TAG, "seekDelta(): Caught illegalStateException" );
    	}
	}

	public void startMovie( final String pathName, final boolean resumePlayback, final boolean isEncrypted, final boolean loop ) 
	{
		// set playbackFinished and playbackFailed to false immediately so it's set when we return to native
		playbackFinished = false;
		playbackFailed = false;
		
    	runOnUiThread( new Thread()
    	{
		 @Override
    		public void run()
    		{
			 	startMovieLocal( pathName, resumePlayback, isEncrypted, loop );
    		}
    	} );
	}
	
	private void startMovieLocal( final String pathName, final boolean resumePlayback, boolean isEncrypted, final boolean loop ) 
	{
		Log.v(TAG, "startMovie " + pathName + " resume " + resumePlayback );
		
		synchronized( this ) 
		{
			requestAudioFocus();
	
			playbackFinished = false;
			playbackFailed = false;
			
			waitingForSeek = false;
			haveSeekWaiting = false;
			nextSeekPosition = 0;		
	
			currentMovieFilename = pathName;
			
			// Have native code pause any playing movie,
			// allocate a new external texture,
			// and create a surfaceTexture with it.
			movieTexture = nativePrepareNewVideo( appPtr );
			movieSurface = new Surface( movieTexture );
	
			if (mediaPlayer != null) 
			{
				mediaPlayer.release();
			}
	
			Log.v( TAG, "MediaPlayer.create" );
	
			mediaPlayer = new MediaPlayer();
			mediaPlayer.setOnVideoSizeChangedListener( this );
			mediaPlayer.setSurface( movieSurface );
	
			try 
			{
				Log.v(TAG, "mediaPlayer.setDataSource()");
				mediaPlayer.setDataSource(currentMovieFilename);
			} 
			
			catch ( IOException t ) 
			{
				Fail( "mediaPlayer.setDataSource threw IOException" );
				return;
			}
			
			catch ( IllegalStateException t )
			{
				Fail( "mediaPlayer.setDataSource threw illegalStateException" );
				return;
			}
	
			catch ( IllegalArgumentException t ) 
			{
				Fail( "mediaPlayer.setDataSource threw IllegalArgumentException" );
				return;
			}
	
			try 
			{
				Log.v(TAG, "mediaPlayer.prepare");
				mediaPlayer.prepare();
			} 
			
			catch( IOException t ) 
			{
				Fail( "mediaPlayer.prepare threw IOException" );
				return;
			}
			
			catch ( IllegalStateException t )
			{
				Fail( "mediaPlayer.prepare threw illegalStateException" );
				return;
			}
		
			// ensure we're at max volume
			mediaPlayer.setVolume( 1.0f, 1.0f );
			
			Log.v( TAG, "mediaPlayer.start" );
	
			// If this movie has a saved position, seek there before starting
			Log.d( TAG, "checkForMovieResume: " + currentMovieFilename );
			final int seekPos = getPreferences( MODE_PRIVATE ).getInt( currentMovieFilename + "_pos", 0 );
			Log.v( TAG, "seekPos = " + seekPos );
			Log.v( TAG, "resumePlayback = " + resumePlayback );
			
			try
			{
				if ( resumePlayback && ( seekPos > 0 ) ) 
				{
					Log.v( TAG, "resuming at saved location" );
					mediaPlayer.seekTo( seekPos );
				}
				else
				{
					// start at beginning
					Log.v( TAG, "start at beginning" );
					mediaPlayer.seekTo( 0 );
				}
			}
			
			catch ( IllegalStateException t )
			{
				Fail( "mediaPlayer.seekTo threw illegalStateException" );
				return;
			}
	
			mediaPlayer.setLooping( loop );
			mediaPlayer.setOnCompletionListener( new OnCompletionListener() 
			{
	        	public void onCompletion( MediaPlayer mp )
	        	{
	        		Log.v(TAG, "mediaPlayer.onCompletion" );
	        		playbackFinished = true;
	        		saveCurrentMovieLocation();
	        		releaseAudioFocus();
	        	}        			
	        });
			
			mediaPlayer.setOnSeekCompleteListener( new OnSeekCompleteListener() 
			{
	        	public void onSeekComplete( MediaPlayer mp )
	        	{
	        		if ( haveSeekWaiting )
	        		{
	        			mediaPlayer.seekTo( nextSeekPosition );
	        			haveSeekWaiting = false;
	        		}
	        		else
	        		{
	        			waitingForSeek = false;
	        		}
	        	}        			
	        });
			
			try
			{
				mediaPlayer.start();
			}
	
			catch ( IllegalStateException t )
			{
				Fail( "mediaPlayer.start threw illegalStateException" );
			}
	
			// Save the current movie now that it was successfully started
			Editor edit = getPreferences( MODE_PRIVATE ).edit();
			edit.putString( "currentMovie", currentMovieFilename );
			edit.commit();
		}
		
		Log.v( TAG, "exiting startMovie" );
	}

	public void pauseMovie() 
	{
		Log.d( TAG, "pauseMovie()" );
		if ( mediaPlayer != null ) 
		{
			if ( isPlaying() )
			{
				saveCurrentMovieLocation();
			}
			
			try
			{
				mediaPlayer.pause();
			}
			
			catch ( IllegalStateException t )
			{
				Log.e(TAG, "pauseMovie() caught illegalStateException" );
			}			
		}
	}

	public void resumeMovie() 
	{
		Log.d(TAG, "resumeMovie()" );
		if ( mediaPlayer != null ) 
		{
			try
			{
				mediaPlayer.start();
			}

			catch ( IllegalStateException t )
			{
				Log.e( TAG, "resumeMovie() caught illegalStateException" );
			}
		}
	}

	public void stopMovie()
	{
		Log.v( TAG, "stopMovie" );
		
		synchronized (this) 
		{
			if ( mediaPlayer != null ) 
			{
				// don't save location if not playing
				if ( isPlaying() )
				{
					saveCurrentMovieLocation();
				}
				
				try
				{
					mediaPlayer.stop();
				}
				
				catch ( IllegalStateException t )
				{
					Log.e( TAG, "mediaPlayer.stop threw illegalStateException" );
				}
	
				mediaPlayer.release();
				mediaPlayer = null;
			}
			releaseAudioFocus();
			
			playbackFailed = false;
			playbackFinished = true;
		}
	}

	public boolean togglePlaying()
	{
		boolean result = false;
		
		Log.d( TAG,  "MainActivity.togglePlaying()" );
		if ( mediaPlayer != null ) 
		{
			if ( isPlaying() ) 
			{
				pauseMovie();
				result = false;
			}
			else 
			{
				resumeMovie();
				result = true;
			}
		}
		else
		{
			Log.d( TAG, "mediaPlayer == null" );
		}
		
		return result;
	}
}
