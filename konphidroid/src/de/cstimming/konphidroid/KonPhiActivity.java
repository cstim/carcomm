package de.cstimming.konphidroid;

import java.text.SimpleDateFormat;
import java.util.Date;

import android.app.Activity;
import android.content.Context;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.location.LocationProvider;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

public class KonPhiActivity extends Activity implements LocationListener {

	private ToggleButton togglebuttonGps;
	
	private SimpleDateFormat dateFormatter;
	
	private Location m_lastLocation;
	private boolean m_lastLocationValid;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		dateFormatter = new SimpleDateFormat("hh:mm:ss");
		
		LocationManager locationManager = (LocationManager) getApplicationContext().getSystemService(
				Context.LOCATION_SERVICE);

		togglebuttonGps = (ToggleButton) findViewById(R.id.togglebutton);
		togglebuttonGps.setChecked(locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER));
		
		togglebuttonGps.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				// Perform action on clicks
				if (togglebuttonGps.isChecked()) {
					Toast.makeText(KonPhiActivity.this, "Checked", Toast.LENGTH_SHORT).show();
				} else {
					Toast.makeText(KonPhiActivity.this, "Not checked", Toast.LENGTH_SHORT).show();
				}
			}
		});

		locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER,
				5000, 10,
				this);
	}

	@Override
	public void onLocationChanged(Location loc) {
		final TextView viewgpsage = (TextView) findViewById(R.id.TextViewGPSAge);
		final TextView viewLat = (TextView) findViewById(R.id.TextViewLat);
		final TextView viewLon = (TextView) findViewById(R.id.TextViewLon);
		final TextView viewAcc = (TextView) findViewById(R.id.TextViewAcc);
		final TextView viewDist = (TextView) findViewById(R.id.TextViewDist);

		viewgpsage.setText(dateFormatter.format(new Date(loc.getTime())));
		viewLat.setText(String.valueOf(loc.getLatitude()));
		viewLon.setText(String.valueOf(loc.getLongitude()));
		viewAcc.setText(loc.hasAccuracy() ? String.valueOf(loc.getAccuracy()) : "...");

		String dist = "...";
		if (m_lastLocationValid) {
			dist = String.valueOf(m_lastLocation.distanceTo(loc));
		}
		viewDist.setText(dist);
		
		m_lastLocationValid = true;
		m_lastLocation = loc;
	}

	@Override
	public void onProviderDisabled(String provider) {
		Toast.makeText(KonPhiActivity.this, "Oh: Provider " + provider + " disabled", Toast.LENGTH_SHORT).show();
	}

	@Override
	public void onProviderEnabled(String provider) {
		Toast.makeText(KonPhiActivity.this, "Good: Provider " + provider + " enabled", Toast.LENGTH_SHORT).show();
	}

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {
		switch (status) {
		case LocationProvider.OUT_OF_SERVICE:
			Toast.makeText(KonPhiActivity.this, "Oh: Provider " + provider + " out of service", Toast.LENGTH_SHORT).show();
			togglebuttonGps.setChecked(false);
			break;
		case LocationProvider.TEMPORARILY_UNAVAILABLE:
			Toast.makeText(KonPhiActivity.this, "Oh: Provider " + provider + " tmp. unavailable", Toast.LENGTH_SHORT).show();
			togglebuttonGps.setChecked(false);
			break;
		case LocationProvider.AVAILABLE:
			Toast.makeText(KonPhiActivity.this, "Good: Provider " + provider + " available", Toast.LENGTH_SHORT).show();
			togglebuttonGps.setChecked(true);
			break;
		}
	}
}