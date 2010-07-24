package de.cstimming.konphidroid;

import java.text.SimpleDateFormat;
import java.util.Date;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.location.LocationProvider;
import android.os.Bundle;
import android.provider.Settings;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

public class KonPhiActivity extends Activity implements LocationListener {

	private ToggleButton m_togglebuttonGps;
	
	private SimpleDateFormat m_dateFormatter;
	
	private Location m_lastLocation;
	private boolean m_lastLocationValid;

	private long m_senderIntervalSecs;
	
	private static final int DIALOG_GPSWARNING = 1;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		m_dateFormatter = new SimpleDateFormat("HH:mm:ss");
		m_senderIntervalSecs = 30;
		
		final LocationManager locationManager = (LocationManager) getApplicationContext().getSystemService(
				Context.LOCATION_SERVICE);

		m_togglebuttonGps = (ToggleButton) findViewById(R.id.togglebutton);
		m_togglebuttonGps.setChecked(locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER));
		
		if (!locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
			showDialog(DIALOG_GPSWARNING);
		}

		m_togglebuttonGps.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				// Perform action on clicks
//				if (togglebuttonGps.isChecked()) {
//					Toast.makeText(KonPhiActivity.this, "Checked", Toast.LENGTH_SHORT).show();
//				} else {
//					Toast.makeText(KonPhiActivity.this, "Not checked", Toast.LENGTH_SHORT).show();
//				}
				final Intent i = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
				i.addFlags(Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
				startActivity(i);
				m_togglebuttonGps.setChecked(locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER));
			}
		});

		locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER,
				5000, 10,
				this);
	}

	protected Dialog onCreateDialog(int d) {
		Dialog result;
		switch (d) {
		case DIALOG_GPSWARNING:
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			builder.setMessage("The GPS receiver is not active. Do you want to activate it now?")
			       .setCancelable(false)
			       .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
			           public void onClick(DialogInterface dialog, int id) {
							final Intent i = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
							i.addFlags(Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
							startActivity(i);
			           }
			       })
			       .setNegativeButton("No", new DialogInterface.OnClickListener() {
			           public void onClick(DialogInterface dialog, int id) {
			                dialog.cancel();
			           }
			       });
			result = builder.create();
			break;
		default:
			result = null;
		}
		return result;
	}
	
	@Override
	public void onLocationChanged(Location loc) {
		Toast.makeText(KonPhiActivity.this, "Received location from " + loc.getProvider(), Toast.LENGTH_SHORT).show();

		final TextView viewgpsage = (TextView) findViewById(R.id.TextViewGPSAge);
		final TextView viewLat = (TextView) findViewById(R.id.TextViewLat);
		final TextView viewLon = (TextView) findViewById(R.id.TextViewLon);
		final TextView viewAcc = (TextView) findViewById(R.id.TextViewAcc);
		final TextView viewDist = (TextView) findViewById(R.id.TextViewDist);
		final TextView viewSecs = (TextView) findViewById(R.id.TextViewSecs);

		viewgpsage.setText(m_dateFormatter.format(new Date(loc.getTime())));
		viewLat.setText(String.valueOf(loc.getLatitude()));
		viewLon.setText(String.valueOf(loc.getLongitude()));
		viewAcc.setText(loc.hasAccuracy() ? String.valueOf(loc.getAccuracy()) : "...");

		String dist = "...";
		String secs = "...";
		if (m_lastLocationValid) {
			long secdiff = (loc.getTime() - m_lastLocation.getTime()) / 1000;
			if (secdiff >= m_senderIntervalSecs) {
				sendPair(m_lastLocation, loc);
			}
			secs = String.valueOf(secdiff);
			dist = String.valueOf(m_lastLocation.distanceTo(loc));
		}
		viewDist.setText(dist);
		viewSecs.setText(secs);
		
		m_lastLocationValid = true;
		m_lastLocation = loc;
	}

	private void sendPair(Location startLoc, Location endLoc) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onProviderDisabled(String provider) {
		//Toast.makeText(KonPhiActivity.this, "Oh: Provider " + provider + " disabled", Toast.LENGTH_SHORT).show();
		m_togglebuttonGps.setChecked(false);
	}

	@Override
	public void onProviderEnabled(String provider) {
		//Toast.makeText(KonPhiActivity.this, "Good: Provider " + provider + " enabled", Toast.LENGTH_SHORT).show();
		m_togglebuttonGps.setChecked(true);
	}

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {
		switch (status) {
		case LocationProvider.OUT_OF_SERVICE:
			//Toast.makeText(KonPhiActivity.this, "Oh: Provider " + provider + " out of service", Toast.LENGTH_SHORT).show();
			m_togglebuttonGps.setChecked(false);
			break;
		case LocationProvider.TEMPORARILY_UNAVAILABLE:
			//Toast.makeText(KonPhiActivity.this, "Oh: Provider " + provider + " tmp. unavailable", Toast.LENGTH_SHORT).show();
			m_togglebuttonGps.setChecked(false);
			break;
		case LocationProvider.AVAILABLE:
			//Toast.makeText(KonPhiActivity.this, "Good: Provider " + provider + " available", Toast.LENGTH_SHORT).show();
			m_togglebuttonGps.setChecked(true);
			break;
		}
	}
}