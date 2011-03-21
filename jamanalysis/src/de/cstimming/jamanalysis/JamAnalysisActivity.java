package de.cstimming.jamanalysis;

import java.text.NumberFormat;
import java.text.SimpleDateFormat;
import java.util.TimeZone;

import de.cstimming.jamanalysis.R;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.location.LocationProvider;
import android.os.Bundle;
import android.provider.Settings;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.TextView;
import android.widget.Toast;

public class JamAnalysisActivity extends Activity implements LocationListener, SliceSenderResult, SenderFloatResult {

	private SimpleDateFormat m_dateFormatSender;
	private NumberFormat m_formatterSeconds, m_formatterSpeed;

	private long m_recordIntervalSecs;
	private String m_server = "http://carcomm.cstimming.de/";

	private int m_instanceId;
	private int m_categoryId;

	private LocationCollector m_locationCollector;
	private LocationManager m_locationManager;

	private TextView m_textViewStauanfang, m_textViewJamend;
	private TextView m_textViewStatus, m_textViewPrognose;
	private RadioButton m_buttonJamstart, m_buttonJamslow, m_buttonNojam;
	private int m_numLocTotal = 0;

	private static final int DIALOG_GPSWARNING = 1;
	private static final int DIALOG_RECORDINTERVAL_MULTICHOICE = 2;
	private static final int DIALOG_SENDERINTERVAL_MULTICHOICE = 3;
	private static final int DIALOG_ABOUT = 4;
	private static final int MENU_CHANGE_RECORDING = 101;
	private static final int MENU_CHANGE_SENDING = 102;
	private static final int MENU_QUIT = 103;
	private static final int MENU_ABOUT = 104;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		// Lock our orientation to portrait
		this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

		m_dateFormatSender = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss'Z'");
		m_dateFormatSender.setTimeZone(TimeZone.getTimeZone("GMT00"));
		//m_dateFormatter = new SimpleDateFormat("HH:mm:ss");

		m_formatterSeconds = NumberFormat.getInstance();
		m_formatterSeconds.setMaximumFractionDigits(0);
		m_formatterSpeed = NumberFormat.getInstance();
		m_formatterSpeed.setMaximumFractionDigits(1);

		m_textViewStauanfang = (TextView) findViewById(R.id.TextViewJamstop);
		m_textViewJamend = (TextView) findViewById(R.id.TextViewJamend);
		m_textViewStatus = (TextView) findViewById(R.id.TextViewStatus);
		m_textViewPrognose = (TextView) findViewById(R.id.TextViewPrognose);
		m_textViewStauanfang.setEnabled(false);
		m_textViewJamend.setEnabled(false);

		m_buttonJamstart = (RadioButton) findViewById(R.id.RadioButtonJamstart);
		m_buttonJamslow = (RadioButton) findViewById(R.id.RadioButtonJam);
		m_buttonNojam = (RadioButton) findViewById(R.id.RadioButtonNojam);

		java.util.Random rg = new java.util.Random();
		m_instanceId = rg.nextInt();
		m_categoryId = 1;

		m_locationManager = (LocationManager) getApplicationContext()
		.getSystemService(Context.LOCATION_SERVICE);

		m_recordIntervalSecs = 0;
		m_locationCollector = new LocationCollector(m_server, this, m_instanceId, m_categoryId, this, this);

		setRecordIntervalSecs(2);
		setSenderIntervalSecs(10);

		// Print the version number into the UI
		TextView versionView = (TextView) findViewById(R.id.TextViewVersion);
		String versionString;
		try {
			versionString = getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
		} catch (NameNotFoundException e) {
			versionString = getString(R.string.version_not_found);
			e.printStackTrace();
		}
		versionView.setText(versionString);

		verifyGpsAvailable();

		m_buttonJamstart.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				verifyGpsAvailable();
				setCategoryId(10);
				m_textViewStauanfang.setEnabled(true);
				m_textViewJamend.setEnabled(false);
			}
		});
		m_buttonJamslow.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				verifyGpsAvailable();
				setCategoryId(20);
				m_textViewStauanfang.setEnabled(false);
				m_textViewJamend.setEnabled(true);
			}
		});
		m_buttonNojam.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				verifyGpsAvailable();
				setCategoryId(1);
				m_textViewStauanfang.setEnabled(false);
				m_textViewJamend.setEnabled(false);
			}
		});

		Button buttonFeedback = (Button) findViewById(R.id.ButtonFeedback);
		buttonFeedback.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				Toast.makeText(JamAnalysisActivity.this, R.string.toast_feedback, Toast.LENGTH_SHORT).show();
			}
		});

		registerAtLocationManager(getRecordIntervalSecs());
	}

	private void verifyGpsAvailable() {
		if (!m_locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
			showDialog(DIALOG_GPSWARNING);
		}
	}

	private void registerAtLocationManager(long intervalSecs) {
		final LocationManager locationManager = (LocationManager) getApplicationContext()
		.getSystemService(Context.LOCATION_SERVICE);
		LocationListener listener = m_locationCollector;//this;
		if (intervalSecs == 0) {
			locationManager.removeUpdates(listener);
		} else {
			if (!locationManager
					.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
				showDialog(DIALOG_GPSWARNING);
			}

			long minTime = 1000 * intervalSecs; // [milliseconds]
			float minDistance = 0; // 10; // [meters]
			locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER,
					minTime, minDistance, listener);
		}
	}

	/** Creates the menu items */
	public boolean onCreateOptionsMenu(Menu menu) {
		menu.add(0, MENU_CHANGE_RECORDING, 0, R.string.menu_choose_recording_interval);
		menu.add(0, MENU_CHANGE_SENDING, 0, R.string.menu_choose_sending_interval);
		menu.add(0, MENU_ABOUT, 0, R.string.about);
		menu.add(0, MENU_QUIT, 0, R.string.quit);
		return true;
	}

	/** Handles item selections */
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case MENU_CHANGE_RECORDING:
			showDialog(DIALOG_RECORDINTERVAL_MULTICHOICE);
			return true;
		case MENU_CHANGE_SENDING:
			showDialog(DIALOG_SENDERINTERVAL_MULTICHOICE);
			return true;
		case MENU_QUIT:
			quit();
			return true;
		case MENU_ABOUT:
			showDialog(DIALOG_ABOUT);
			return true;
		}
		return false;
	}

	public void quit() {
		registerAtLocationManager(0);
		finish();
	}

	public long getRecordIntervalSecs() {
		return m_recordIntervalSecs;
	}

	public void setSenderIntervalSecs(long v) {
		if (v == m_locationCollector.getSendingIntervalSecs())
			return;
		m_locationCollector.setSendingIntervalSecs(v);
	}

	public void setRecordIntervalSecs(long v) {
		registerAtLocationManager(v);
		m_recordIntervalSecs = v;
	}

	protected Dialog onCreateDialog(int d) {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		switch (d) {
		case DIALOG_GPSWARNING:

			builder.setMessage(R.string.ask_activate_gps_settings)
			.setCancelable(false)
			.setPositiveButton(R.string.yes,
					new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog,
						int id) {
					final Intent i = new Intent(
							Settings.ACTION_LOCATION_SOURCE_SETTINGS);
					i.addFlags(Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
					startActivity(i);
				}
			})
			.setNegativeButton(R.string.no,
					new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog,
						int id) {
					dialog.cancel();
				}
			});
			break;
		case DIALOG_ABOUT:
			builder.setMessage(R.string.text_about)
			.setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog, int id) {
					dialog.cancel(); }
			});
			break;
		case DIALOG_RECORDINTERVAL_MULTICHOICE:
			builder.setTitle(R.string.choose_record_interval);
			final CharSequence[] items = {"30", "20", "10", "2", "1", "off"};
			builder.setSingleChoiceItems(items, 3, new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					String item = items[which].toString();
					final int secs = (item == "off" ? 0 : Integer.parseInt(item));
					setRecordIntervalSecs(secs);
					dismissDialog(DIALOG_RECORDINTERVAL_MULTICHOICE);
				}
			});
			break;
		case DIALOG_SENDERINTERVAL_MULTICHOICE:
			builder.setTitle(R.string.choose_sending_interval);
			final CharSequence[] items1 = {"120", "60", "30", "20", "10", "off"};
			builder.setSingleChoiceItems(items1, 4, new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					String item = items1[which].toString();
					final int secs = (item == "off" ? 0 : Integer.parseInt(item));
					setSenderIntervalSecs(secs);
					dismissDialog(DIALOG_SENDERINTERVAL_MULTICHOICE);
				}
			});
			break;
		default:
			return null;
		}
		Dialog result = builder.create();
		return result;
	}

	@Override
	public void onLocationChanged(Location loc) {
		m_numLocTotal += 1;
		printLocInfo(loc);

		//m_lastLocationValid = true;
		//m_lastLocation = loc;
	}

	private void printLocInfo(Location loc) {
		//float speed = loc.getSpeed();
		//String speedtext = "Geschwindigkeit: " + m_formatterSpeed.format(3.6 * speed) + " km/h";
		//sliceSenderResult(speedtext, true, Color.GREEN);

		if (m_numLocTotal <= 6) {
			setTextPrognose(getString(R.string.only_few_gps_points, m_numLocTotal), Color.YELLOW);
		} else {
			setTextPrognose(getString(R.string.sufficient_gps_points), Color.GREEN); 
		}
	}

	@Override
	public void onProviderDisabled(String provider) {
		String text = getString(R.string.gps_not_switched_on);
		//sliceSenderResult(text, false, Color.RED);
		setTextPrognose(text, Color.RED);
		m_buttonJamslow.setEnabled(false);
		m_buttonJamstart.setEnabled(false);
	}

	@Override
	public void onProviderEnabled(String provider) {
		String text = getString(R.string.gps_switched_on);
		sliceSenderResult(text, true, Color.GREEN);
		setTextPrognose(text, Color.YELLOW);
		m_buttonJamslow.setEnabled(true);
		m_buttonJamstart.setEnabled(true);
	}

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {
		switch (status) {
		case LocationProvider.OUT_OF_SERVICE:
			sliceSenderResult(getString(R.string.no_gps_available), false, Color.RED);
			// Toast.makeText(KonPhiActivity.this, "Oh: Provider " + provider +
			// " out of service", Toast.LENGTH_SHORT).show();
			//			m_togglebuttonGps.setChecked(false);
			break;
		case LocationProvider.TEMPORARILY_UNAVAILABLE:
			// Toast.makeText(KonPhiActivity.this, "Oh: Provider " + provider +
			// " tmp. unavailable", Toast.LENGTH_SHORT).show();
			//			m_togglebuttonGps.setChecked(false);
			break;
		case LocationProvider.AVAILABLE:
			// Toast.makeText(KonPhiActivity.this, "Good: Provider " + provider
			// + " available", Toast.LENGTH_SHORT).show();
			//			m_togglebuttonGps.setChecked(true);
			break;
		}
	}

	@Override
	public void sliceSenderResult(String text, boolean good, int color) {
		m_textViewStatus.setText(text);
		m_textViewStatus.setTextColor(ColorStateList.valueOf(color));
	}

	public void setTextPrognose(String text, int color) {
		m_textViewPrognose.setText(text);
		m_textViewPrognose.setTextColor(ColorStateList.valueOf(color));
	}

	@Override
	public void resultFloat(float value, boolean good) {
		if (!good)
			return;

		String vstring = m_formatterSeconds.format(value);
		if (value == -1) {
			vstring = getString(R.string.display_standstill);
		} else if (value == 0) {
			vstring = getString(R.string.display_freeflow);
		}
//		int color = Color.RED;
//		if (good)
//		{
//			color = value > 0 ? Color.GREEN : Color.YELLOW;
//		}
		//sliceSenderResult(vstring, true, color); // gets overwritten anyway

		if (m_buttonJamstart.isChecked()) {
			// Heranfahren an Stau
			setTextJamstanding(true, vstring);
			setTextJamend(false, "");
		} else if (m_buttonJamslow.isChecked()) {
			// Im Stau
			setTextJamstanding(false, "");
			setTextJamend(true, vstring);
		} else {
			// Nix Stau
			setTextJamstanding(false, "");
			setTextJamend(false, "");
		}
	}

	private void setTextJamstanding(boolean active, String duration) {
		m_textViewStauanfang.setEnabled(active);
		if (!active) {
			duration = "--";
		}
		m_textViewStauanfang.setText(getString(R.string.seconds_till_standstill, duration));
	}

	private void setTextJamend(boolean active, String duration) {
		m_textViewJamend.setEnabled(active);
		if (!active) {
			duration = "--";
		}
		m_textViewJamend.setText(getString(R.string.seconds_till_free_drive, duration));
	}

	public void setCategoryId(int id) {
		m_categoryId = id;
		m_locationCollector.setCategoryId(id);
	}
}
