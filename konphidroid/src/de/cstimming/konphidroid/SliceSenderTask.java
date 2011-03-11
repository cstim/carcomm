package de.cstimming.konphidroid;

import java.io.IOException;

import org.apache.http.HttpResponse;
import org.apache.http.StatusLine;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.HttpResponseException;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.BasicResponseHandler;

import android.graphics.Color;
import android.os.AsyncTask;

public class SliceSenderTask extends AsyncTask<HttpPost, Void, HttpResponse> {
	private HttpClient m_httpclient;
	private SliceSenderResult m_text_cb;
	private String m_resultprefix;
	private SenderInterface m_sender_cb;

	public SliceSenderTask(HttpClient client, String resultprefix, SliceSenderResult callback, SenderInterface callback2) {
		super();
		m_httpclient = client;
		m_resultprefix = resultprefix;
		m_text_cb = callback;
		m_sender_cb = callback2;
	}

	@Override
	protected void onPreExecute() {
		String text = "Sending Way...";
		int color = Color.YELLOW;
		if (m_text_cb != null) {
			m_text_cb.sliceSenderResult(m_resultprefix + text, true, color);
		}
		if (m_sender_cb != null) {
			m_sender_cb.startedSending();
		}
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
				try {
					text = new BasicResponseHandler().handleResponse(response);
					//System.out.println("rtext=" + rtext);
				} catch (HttpResponseException e) {
					good = false;
				} catch (IOException e) {
					good = false;
				}
			} else {
				good = false;
			}
		} else {
			text = "Exception during HTTP connect";
			good = false;
		}
		if (m_text_cb != null) {
			m_text_cb.sliceSenderResult(m_resultprefix + text, good, good ? Color.GREEN : Color.RED);
		}
		if (m_sender_cb != null) {
			m_sender_cb.stoppedSending(good);			
		}
	}

}
