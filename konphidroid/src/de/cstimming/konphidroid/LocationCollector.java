package de.cstimming.konphidroid;

import java.io.CharArrayWriter;
import java.io.UnsupportedEncodingException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import org.apache.http.HttpEntity;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.DefaultHttpClient;

import android.location.Location;
import android.location.LocationListener;
import android.os.Bundle;

public class LocationCollector implements LocationListener, SenderInterface {
	private ArrayList<Location> m_collectingLocations;
	private ArrayList<Location> m_sendingLocations;
	private boolean m_currentlySending;
	private String m_server;
	private SliceSenderResult m_result_cb;
	private SimpleDateFormat m_dateFormatter;
	private int m_instanceid;
	private int m_categoryId;
	private LocationListener m_location_cb;
	private long m_sendingIntervalSecs;

	public LocationCollector(String serveraddress, SliceSenderResult result_cb, int instanceid, int mCategoryId, LocationListener another_location_cb) {
		m_currentlySending = false;
		m_server = serveraddress;
		m_result_cb = result_cb;
		m_instanceid = instanceid;
		m_categoryId = mCategoryId;
		m_location_cb = another_location_cb;
		m_dateFormatter = new SimpleDateFormat("HH:mm:ss");
		resetCollecting();
	}
	private void resetCollecting() {
		m_collectingLocations = new ArrayList<Location>();
	}

	@Override
	public void onLocationChanged(Location loc) {
		if (m_location_cb != null) {
			m_location_cb.onLocationChanged(loc);
		}
		m_collectingLocations.add(loc);
		checkForSending();
	}

	public void setSendingIntervalSecs(long secs) {
		m_sendingIntervalSecs = secs;
	}
	
	public long getSendingIntervalSecs() {
		return m_sendingIntervalSecs;
	}

	private void checkForSending() {
		if (m_currentlySending
				|| m_collectingLocations.size() <= 1
				|| m_sendingIntervalSecs == 0) {
			return;
		}
		Location first = m_collectingLocations.get(0);
		Location last = m_collectingLocations.get(m_collectingLocations.size() - 1);
		long secdiff = (last.getTime() - first.getTime()) / 1000;
		if (m_sendingIntervalSecs > 0 && secdiff >= m_sendingIntervalSecs) {
			sendNow();
		}
	}

	private void sendNow() {
		m_sendingLocations = m_collectingLocations;
		m_currentlySending = true;
		scheduleSending();
		resetCollecting();
	}

	private static String listToString(ArrayList<Location> ul, int instanceid) {
		CharArrayWriter gpxoutput = new GpxCharArrayWriter(ul, instanceid);
		String gpxstring = gpxoutput.toString();
		return gpxstring;
	}

	private void scheduleSending() {
		HttpClient httpclient = new DefaultHttpClient();
		HttpPost httppost = new HttpPost(m_server + "home/gpxUpload"); // FIXME: Different URL for GPX upload!
		final String displaytime = m_dateFormatter.format(new Date(m_sendingLocations.get(m_sendingLocations.size() - 1).getTime())) + ": ";

		// Copied from http://www.anddev.org/upload_files_to_web_server-t443-s30.html
		final String lineEnd = "\r\n";
		final String twoHyphens = "--";
		final String boundary = "*****";
		httppost.setHeader("Content-Type", "multipart/form-data; boundary="+boundary);

		String gpxstring = listToString(m_sendingLocations, m_instanceid);
		String output = twoHyphens + boundary + lineEnd;
		output += "Content-Disposition: form-data; name=\"upload\"; filename=\"anonym\"" + lineEnd;
		output += "Content-Type: text/plain; charset=UTF-8" + lineEnd;
		output += lineEnd;
		// send file data
		output += gpxstring;
		// send multipart form data necesssary after file data.
		output += lineEnd;
		output += twoHyphens + boundary + twoHyphens + lineEnd;

		// Put the string into the http entity
		try {
			HttpEntity entity = new StringEntity(output);
			httppost.setEntity(entity);
		} catch (UnsupportedEncodingException e) {
			// Auto-generated catch block
			e.printStackTrace();
			return;
		}
		//HttpEntity entity = listToStreamEntity(m_sendingLocations, m_instanceid);

		// The network connection is moved into a separate task
		SliceSenderTask task = new SliceSenderTask(httpclient, displaytime, m_result_cb, this);

		// Execute HTTP Post Request
		task.execute(httppost);

	}

	@Override
	public void startedSending() {
		// TODO Auto-generated method stub

	}
	@Override
	public void stoppedSending(boolean successful) {
		if (successful) {
			m_currentlySending = false;
			m_sendingLocations = null;
		} else {
			scheduleSending(); // TODO: what now?
		}
	}


	@Override
	public void onProviderDisabled(String arg0) {
		// TODO Auto-generated method stub
		if (m_location_cb != null) {
			m_location_cb.onProviderDisabled(arg0);
		}

	}
	@Override
	public void onProviderEnabled(String arg0) {
		// TODO Auto-generated method stub
		if (m_location_cb != null) {
			m_location_cb.onProviderEnabled(arg0);
		}

	}
	@Override
	public void onStatusChanged(String arg0, int arg1, Bundle arg2) {
		// TODO Auto-generated method stub

	}

}
