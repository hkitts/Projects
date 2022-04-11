package jtalk;

public class SyncronizedInterrupted{
	private boolean interrupted;


	public SyncronizedInterrupted(){
		interrupted = false;
	}

	public synchronized void serverShutdown(){
		interrupted = true;
	}

	public synchronized boolean interrupted(){
		if(interrupted) return true;

		return false;
	}

}
