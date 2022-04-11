package jtalk;

import java.util.*;
import java.net.*;
import java.io.*;

public class ListeningThread extends Thread{
	private Socket socket = null;
	private TreeMap<String, ChatData> room_map;

	public ListeningThread(Socket socket, TreeMap<String, ChatData> rm){
		this.socket = socket;
		this.room_map = rm;
	}

	public void run(){
		new ClientThread(socket, room_map).start();
	}

}
