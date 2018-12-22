package client;

import java.io.*;
import java.net.*;
import java.awt.*;
import java.applet.*;
import java.util.StringTokenizer;

import common.Config;

public class Greeting extends Applet implements Runnable {
  public static Image tiles_large[] = new Image[43];
  public static Image tiles_small[] = new Image[43];
  public static Image dukes[] = new Image[10];
  public static Image frame_icon;

  public String doc_base = "http://www.math.princeton.edu/~ztfeng/";
  Button main_button = null;
  Client main = null;
  
  public void init () {
    new Thread(this).start();
    main_button = new Button("Initializing...");
    BorderLayout bl = new BorderLayout();
    setLayout(bl);
    bl.addLayoutComponent("Center", main_button);
    add(main_button);
    show();
    showStatus("IMS: loading client...");
    main = new Client();  // load the client now
    main_button.setLabel("Connect NOW!");
    String db = getParameter("DocBase");
    if (db != null)
      doc_base = db;
  }

  public void restart () {
    main = null;
    main_button.setLabel("Connect again");
    main_button.enable();
    showStatus("");
  }

  public synchronized boolean action (Event evt, Object what) {
    Toolkit tk = getToolkit();
    if (evt.target == main_button) {
      main_button.disable();
      int server_port = Config.server_port;
      String server_port_str = getParameter("ServerPort");
      if (server_port_str != null) {
	try {
	  server_port = Integer.parseInt(server_port_str);
	} catch (NumberFormatException e) {
	}
      }
      String server_host_list = getParameter("ServerHost");
      if (server_host_list == null) 
	server_host_list = Config.server_host;
      StringTokenizer st = new StringTokenizer(server_host_list, ":");
      while (st.hasMoreTokens()) {
	String host = st.nextToken();
	try {
	  showStatus("IMS: Connecting to " + host + "...");
	  Socket sock = new Socket(host, server_port);
	  main_button.setLabel("Connected!");
	  main = new Client();
	  main.start_login(this, sock, host);
	  showStatus("IMS: connected");
	  return true;
	} catch (Exception e) {
	  System.out.println("Connect: " + host + ": " + e.toString());
	}
      }
      System.out.println("Failed to connect to server.");
      restart();
    }
    return true;
  }

  public void run () {
    int n_try = 1;
    showStatus("IMS: Loading images...");
    MediaTracker tracker = new MediaTracker(this);
    Object errors[] = null;
    frame_icon = getImage(getDocumentBase(), "images/Icon.gif");
    for (int i = 0; i < 43; i++) {
      if (tiles_large[i] == null) {
	tiles_large[i] = getImage(getDocumentBase(), "tiles/T" + i + ".gif");
	tracker.addImage(tiles_large[i], 0);
      }
    }
    for (int i = 0; i < 10; i++) {
      if (dukes[i] == null) {
	dukes[i] = getImage(getDocumentBase(), "images/D" + i + ".gif");
	tracker.addImage(dukes[i], 1);
      }
    }
    while (true) {
      try {
	tracker.waitForAll();
      } catch (InterruptedException e) {
      }
      if ((errors = tracker.getErrorsAny()) == null) {
	break;
      }
      showStatus("IMS: Loading images (try " + (++n_try) + ")...");
      for (int i = 0; i < 43; i++) {
	for (int j = 0; j < errors.length; j++) {
	  if (tiles_large[i] == errors[j]) {
	    tiles_large[i] = getImage(getDocumentBase(), "tiles/T" + i + ".gif");
	    tracker.addImage(tiles_large[i], 0);
	  }
	}
      }
      for (int i = 0; i < 10; i++) {
	for (int j = 0; j < errors.length; j++) {
	  if ((Object)dukes[i] == errors[j]) {
	    dukes[i] = getImage(getDocumentBase(), "dukester/D" + i + ".gif");
	    tracker.addImage(dukes[i], 0);
	  }
	}
      }
    }
    showStatus("IMS: Done loading images.");
  }

  public String getAppletInfo () {
    return "Internet MahJong Server, Client v0.1.  Copyright (C) 1996 Thomas Zuwei Feng";
  }

  public void ShowDoc (String file) {
    try {
      URL url = new URL(doc_base + file);
      getAppletContext().showDocument(url);
    } catch (MalformedURLException e) {
    }
  }
}

