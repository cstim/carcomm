package de.cstimming.jamanalysis;

public interface SenderInterface {
	public void startedSending();
	public void stoppedSending(boolean successful, boolean mayRetry);
}
