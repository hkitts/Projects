package jtalk;

import java.util.*;
import java.net.*;
import java.io.*;

public class ChatData{
	private String room_name;
	private HashMap<String, ClientThread> client_map;
	

	public ChatData(String rn){
		this.room_name = rn;
		this.client_map = new HashMap<>();
	}

	public synchronized void distributeMessage(String m){
		for(Map.Entry mapElement : client_map.entrySet()){
			ClientThread temp = (ClientThread)mapElement.getValue();
			temp.sendMessage(m);
		}
	}
	
	public synchronized void addClient(String name, ClientThread client){
		client_map.put(name, client);

		distributeMessage(name + " has joined");
	}
	
	public synchronized void deleteClient(String c){
		client_map.remove(c);
		distributeMessage(c + " has left");
	}
	
	public synchronized void broadcastMembers(PrintWriter out){
		for(String key : client_map.keySet()){
			out.printf("%s ", key);
		}
	}

}
