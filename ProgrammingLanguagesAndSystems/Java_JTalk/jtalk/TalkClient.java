package jtalk;

import java.net.*;
import java.io.*;

class TalkClient{

	public TalkClient(String args[]){
		String hostName;
		int portNumber;


		if(args.length < 3){
			System.out.printf("usage: jtalk.TalkClient hostname portname chatname%n");
			System.exit(1);
		}
		hostName = args[0];
		portNumber = Integer.parseInt(args[1]);


		try(
			Socket clientSocket = new Socket(hostName, portNumber);
			PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
			BufferedReader in = new BufferedReader(
				new InputStreamReader(clientSocket.getInputStream()));
			BufferedReader stdIn = new BufferedReader(
				new BufferedReader(new InputStreamReader(System.in)));
		) {
			
			SyncronizedInterrupted si = new SyncronizedInterrupted();
			new WriteConsoleThread(in, si).start();
		
			out.println(args[2]);

			String clientMessage;
			while((clientMessage = stdIn.readLine()) != null){
				if(si.interrupted()) break;
				out.println(clientMessage);				
			}


		} catch (UnknownHostException e) {
			System.err.println("Don't know about host " + hostName);
			System.exit(1);
		} catch (IOException e) {
			System.err.println("Couldn't get I/O for the connection to " + hostName);
			System.exit(1);
		}
	}


	public static void main(String args[]){
		TalkClient cs = new TalkClient(args);
	}

}
