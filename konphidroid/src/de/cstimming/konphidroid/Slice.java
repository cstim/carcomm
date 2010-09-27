package de.cstimming.konphidroid;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import org.apache.http.NameValuePair;
import org.apache.http.message.BasicNameValuePair;

import android.location.Location;

public class Slice {
	private Location m_startLoc, m_endLoc;
	private int m_instanceId;
	private int m_categoryId;
	private SimpleDateFormat m_dateFormatSender;

	public Slice(Location startLoc, Location endLoc, int instanceId, int categoryId, SimpleDateFormat dateFormat) {
		m_startLoc = startLoc;
		m_endLoc = endLoc;
		m_instanceId = instanceId;
		m_categoryId = categoryId;
		m_dateFormatSender = dateFormat;
	}

	private static String degToString(double deg) {
		return String.valueOf(deg);
	}

	public List<NameValuePair> toNameValuePair() {
		String startLat = degToString(m_startLoc.getLatitude());
		String startLon = degToString(m_startLoc.getLongitude());
		String endLat = degToString(m_endLoc.getLatitude());
		String endLon = degToString(m_endLoc.getLongitude());
		String duration = String
		.valueOf((m_endLoc.getTime() - m_startLoc.getTime()) / 1000);
		String endTime = m_dateFormatSender.format(new Date(m_endLoc.getTime()));
		// FIXME: Need to transfer speed at both points, and also
		// group selection

		List<NameValuePair> nvpairs = new ArrayList<NameValuePair>(10);
		nvpairs.add(new BasicNameValuePair("slice[startlat]",
				startLat));
		nvpairs.add(new BasicNameValuePair("slice[startlon]",
				startLon));
		nvpairs.add(new BasicNameValuePair("slice[lat]", endLat));
		nvpairs.add(new BasicNameValuePair("slice[lon]", endLon));
		nvpairs.add(new BasicNameValuePair("slice[duration]",
				duration));
		nvpairs.add(new BasicNameValuePair("slice[time]", endTime));
		nvpairs.add(new BasicNameValuePair("slice[startaccuracy]", String.valueOf(m_startLoc.getAccuracy())));
		nvpairs.add(new BasicNameValuePair("slice[endaccuracy]", String.valueOf(m_endLoc.getAccuracy())));
		nvpairs.add(new BasicNameValuePair("slice[instanceid]", String.valueOf(m_instanceId)));
		nvpairs.add(new BasicNameValuePair("slice[categoryid]", String.valueOf(m_categoryId)));
		return nvpairs;
	}
}
