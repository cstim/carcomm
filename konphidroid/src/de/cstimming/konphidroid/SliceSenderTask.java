package de.cstimming.konphidroid;

import java.io.IOException;

import org.apache.http.HttpResponse;
import org.apache.http.StatusLine;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;

import android.graphics.Color;
import android.os.AsyncTask;

public class SliceSenderTask extends AsyncTask<HttpPost, Void, HttpResponse> {
	HttpClient m_httpclient;
	SliceSenderResult m_callback;
	String m_resultprefix;

	public SliceSenderTask(HttpClient client, String resultprefix, SliceSenderResult callback) {
		super();
		m_httpclient = client;
		m_resultprefix = resultprefix;
		m_callback = callback;
	}

	@Override
	protected void onPreExecute() {
		String text = "Sending Way...";
		int color = Color.YELLOW;
		m_callback.sliceSenderResult(m_resultprefix + text, true, color);
	}

	@Override
	protected HttpResponse doInBackground(HttpPost... params) {
		if (params.length != 1)
			return null;
		try {
			// Execute HTTP Post Request
			HttpResponse response = m_httpclient.execute(params[0]);
			return response;
		} catch (ClientProtocolException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}

	@Override
	protected void onPostExecute(HttpResponse response) {
		// baos.reset();
		// response.getEntity().writeTo(baos);
		// m_webview.loadData(baos.toString(), "text/html", "utf-8");

		String text;
		boolean good = true;
		if (response != null) {
			StatusLine rstatus = response.getStatusLine();
			int rcode = rstatus.getStatusCode();
			text = String.valueOf(rcode) + " " + rstatus.getReasonPhrase();
			if (rcode >= 200 && rcode < 300) {
			} else {
				good = false;
			}
		} else {
			text = "Exception during HTTP connect";
			good = false;
		}
		m_callback.sliceSenderResult(m_resultprefix + text, good, good ? Color.GREEN : Color.RED);
	}

}
