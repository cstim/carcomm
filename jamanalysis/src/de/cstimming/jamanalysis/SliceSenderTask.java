package de.cstimming.jamanalysis;

import java.io.IOException;
import java.net.UnknownHostException;

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
	private SenderFloatResult m_floatResult_cb;
	private String m_exceptionString;

	public SliceSenderTask(HttpClient client, String resultprefix,
			SliceSenderResult callback, SenderInterface c2, SenderFloatResult c3) {
		super();
		m_httpclient = client;
		m_resultprefix = resultprefix;
		m_text_cb = callback;
		m_sender_cb = c2;
		m_floatResult_cb = c3;
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
		m_exceptionString = "";
		try {
			// Execute HTTP Post Request
			HttpResponse response = m_httpclient.execute(params[0]);
			return response;
		} catch (ClientProtocolException e) {
			e.printStackTrace();
		} catch (UnknownHostException e) {
			m_exceptionString = e.getLocalizedMessage(); 
			System.out.println("Unknown host:" + m_exceptionString);
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
		boolean gotException = (m_exceptionString.length() > 0);
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
			text = gotException ? m_exceptionString : "Exception during HTTP connect";
			good = false;
		}
		float resultfloat = 0;
		//System.out.println("Result text a:" + text);
		try {
			int firstWhitespace = text.indexOf(" ");
			String tmp = (firstWhitespace == -1) ? text : text.substring(0, firstWhitespace);
			resultfloat = Float.parseFloat(tmp);
		} catch (NumberFormatException ex) {
			// do nothing
		}
		if (m_floatResult_cb != null) {
			m_floatResult_cb.resultFloat(resultfloat, good);
		}
		//System.out.println("Result text b:" + text);
		if (m_text_cb != null) {
			m_text_cb.sliceSenderResult(m_resultprefix + text, good, good ? Color.GREEN : Color.RED);
		}
		if (m_sender_cb != null) {
			m_sender_cb.stoppedSending(good, !gotException);			
		}
	}

}
