package jtalk;

import java.net.*;
import java.io.*;

public class WriteConsoleThread extends Thread{
	private BufferedReader in;
	private SyncronizedInterrupted si;

	public WriteConsoleThread(BufferedReader in, SyncronizedInterrupted si){
		this.in = in;
		this.si = si;
	}

	public void run(){
		String serverMessage;
		try{
			while((serverMessage = in.readLine()) != null){
				System.out.println(serverMessage);
			}
			System.out.println("Server has been shutdown");
			si.serverShutdown();

		} catch (IOException e) {
			System.err.println("IOException WriteConsoleThread");
			System.exit(1);
		}
	}

}
