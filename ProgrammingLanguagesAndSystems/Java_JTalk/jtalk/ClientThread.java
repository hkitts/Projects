package jtalk;

import java.util.*;
import java.net.*;
import java.io.*;

public class ClientThread extends Thread{
	private String name;
	private ChatData current_room = null;
	private TreeMap<String, ChatData> room_map;
	private Socket socket = null;
	private PrintWriter messageOut = null;

	public ClientThread(Socket socket, TreeMap<String, ChatData> room_map){	
		this.socket = socket;
		this.room_map = room_map;
	}

	public boolean checkRoom(String s){
		this.current_room = room_map.get(s); 
		if(this.current_room == null){
			return false;
		}

		this.current_room.addClient(name, this);
		return true;
		
	}

	public void sendMessage(String s){
		messageOut.println(s);
	}

	public void run(){
		try(
			PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
			BufferedReader in = new BufferedReader(
				new InputStreamReader(
					socket.getInputStream()));
		)  {
			messageOut = out;
			
			name = in.readLine();

			for(Map.Entry<String, ChatData> entry : room_map.entrySet()){
				String key = entry.getKey();
				out.printf("%s: ", key);
				ChatData cd = entry.getValue();
				cd.broadcastMembers(out);
				out.println();
			}
			out.println();
			out.println("Enter chat room:");
	
			boolean read = false;
			String roomInput;
			while ((roomInput = in.readLine()) != null && !read) {
				if(checkRoom(roomInput)){ read = true; break;}
				out.println("Enter chat room:");
			}

			String clientMessage;
			while((clientMessage = in.readLine()) != null){
					current_room.distributeMessage(name + ": " + clientMessage);
			}
			
			if(current_room != null){
				current_room.deleteClient(name);
			}

		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}
