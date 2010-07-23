package de.cstimming.konphidroid;

import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.Log;

/**
 * GPS-Tracker. Holds a {@link Track} and manages it.
 * 
 * @author mb
 */
public class Tracker implements LocationListener {

    /**
     * Tag used for Android-logging.
     */
	private static final String DEBUG_TAG = Tracker.class.getName();

	private static final float LOCATION_MIN_ACCURACY = 200f;

	public static final byte STATE_START = 0;

	public static final byte STATE_PAUSE = 1;

	public static final byte STATE_STOP = 2;

	private int trackingState = STATE_START;

	private Location m_lastLocation;
	private boolean m_lastLocationValid;

	private LocationManager locationManager;

	private long m_GpsInterval;
	private float m_GpsDistance;

	Tracker(final LocationManager locationManager) {
		this.locationManager = locationManager;
		this.m_lastLocationValid = false;
		m_GpsInterval = 5000; // milliseconds
		m_GpsDistance = 20; // meters
	}

	boolean setFollowGps(final boolean followGps) throws FollowGpsException {
		if (followGps) {
			if (m_lastLocationValid) {
				return true;
			} else {
				throw new FollowGpsException("Got no current location");
			}
		} else {
			return false;
		}
	}

	void resetTrack() {
		m_lastLocationValid = false;
	}

	public Location getLocation() throws FollowGpsException {
		if (m_lastLocationValid) {
			return m_lastLocation;
		} else {
			throw new FollowGpsException("Got no current location");
		}
	}
	
	void addLocation(Location loc) {
		// FIXME!!
	}

	void setTrackingState(int newTrackingState) {
		//PAUSE is only a toggle-flag
		if (newTrackingState == STATE_PAUSE && (trackingState == STATE_PAUSE || trackingState == STATE_STOP)) {
			newTrackingState = STATE_START;
		}
		switch (newTrackingState) {
		case STATE_START:
			locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER,
					m_GpsInterval, m_GpsDistance,
					this);
			addLocation(locationManager.getLastKnownLocation(LocationManager.GPS_PROVIDER));
			break;

		case STATE_STOP:
			resetTrack();
			locationManager.removeUpdates(this);
			break;

		case STATE_PAUSE:
			locationManager.removeUpdates(this);
			break;
		}
		trackingState = newTrackingState;
		m_lastLocationValid = false;

	}

	/**
	 * used for disable updates but not to set a new tracking state (e.g. on exiting)
	 */
	void removeUpdates() {
		locationManager.removeUpdates(this);
	}

	@Override
	public void onLocationChanged(final Location location) {
		Log.w(DEBUG_TAG, "onLocationChanged() Got location: " + location);
		if (!location.hasAccuracy() || location.getAccuracy() <= LOCATION_MIN_ACCURACY) {
			addLocation(location);
		}
	}

	@Override
	public void onProviderDisabled(final String provider) {}

	@Override
	public void onProviderEnabled(final String provider) {}

	@Override
	public void onStatusChanged(final String provider, final int status, final Bundle extras) {}

}
