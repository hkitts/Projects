package jtalk;

import java.util.*;
import java.net.*;
import java.io.*;

class ChatServer{

	public ChatServer(String args[]){
		int port;
		TreeMap<String, ChatData> room_map;

		if(args.length < 2){
			System.out.printf("usage: jtalk.ChatServer port Chat-room-Names ...%n");
			System.exit(1);
		}

		port = Integer.parseInt(args[0]);
		if(port < 8000){
			System.out.printf("Must use a port >= 8000%n");
			System.exit(1);
		}
		
		room_map = new TreeMap<String, ChatData>();
		for(int i = 1; i < args.length; i++){
			room_map.put(args[i], new ChatData(args[i]));
		}
		
		boolean listening = true;

		try(ServerSocket serverSocket = new ServerSocket(port)){
			while(listening){
				new ListeningThread(serverSocket.accept(), room_map).start();
			}
		} catch (IOException e) {
			System.err.println("Could not listen on port " + port);
			System.exit(-1);
		}
	
	}

	public static void main(String args[]){
		ChatServer cs = new ChatServer(args);
	}

}
