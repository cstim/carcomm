package de.cstimming.konphidroid;

import java.io.CharArrayWriter;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Iterator;
import java.util.List;

import android.location.Location;

public class GpxCharArrayWriter extends CharArrayWriter {

	private int pointsCount = 0;
	private SimpleDateFormat date = new SimpleDateFormat("yyyy-MM-dd");		
	private SimpleDateFormat time = new SimpleDateFormat("HH:mm:ss");		
	Calendar cal = Calendar.getInstance();

	private String gpxFoter =
		"</trkseg>\n"+
		"</trk>\n"+
		"</gpx>\n";

	public GpxCharArrayWriter(List<Location> l, int instanceid){ 
		super(50); // FIXME: calculate meaningful value

		String gpxHeader = 
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
			"<gpx\n"+
			" version=\"1.0\"\n"+
			"creator=\"KonPhiDroid\"\n"+
			"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"+
			"xmlns=\"http://www.topografix.com/GPX/1/0\"\n"+
			"xsi:schemaLocation=\"http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd\">\"\n"+
			"<time>"+this.date.format(this.cal.getTime())+"T"+this.time.format(this.cal.getTime())+"Z</time>\n" +
			"<trk>\n"+
			"<trkseg>\n"
			;

		super.append(gpxHeader);

		for (Iterator<Location> iter = l.iterator(); iter.hasNext(); ) {
			saveLocation(iter.next(), instanceid);
		}
		closeFile();

	}
	private void closeFile() {
		super.append(this.gpxFoter);
	}

	public void saveLocation(Location loc, int instanceid) {
		super.append(
				"<trkpt lat=\""+loc.getLatitude()
				+ "\" lon=\""+loc.getLongitude() 
				+ "\" avgvel=\"" + loc.getSpeed()
				+ "\" headingdeg=\"" + loc.getBearing()
				+ "\" accuracy=\"" + loc.getAccuracy()
				+ "\" instanceid=\"" + instanceid
				+ "\">\n"+
				" <time>"+this.date.format(loc.getTime())
				+ "T" + this.time.format(loc.getTime())+"Z</time>\n" +
				//"<name>TP"+(pointsCount++)+"</name>\n"+
				//"<fix>none</fix>\n"+
		"</trkpt>\n");
	}

}
