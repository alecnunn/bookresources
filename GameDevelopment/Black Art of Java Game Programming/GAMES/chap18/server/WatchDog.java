package server;

class WatchDog extends Thread {
  int time = 0;

  public WatchDog (int t) {
    time = t;
  }
  
  public void run () {
    Player scratch[] = new Player[Player.global.length];
    
    while (true) {
      try {
	sleep(time);
      } catch (InterruptedException e) {
      }
      int n = 0;
      synchronized (Player.global) {
	for (int i = 0; i < Player.global.length; i++) {
	  Player him = Player.global[i];
	  if (him != null) {
	    if (him.ping) {
	      him.ping = false;
	    } else {
	      scratch[n++] = him;
	    }
	  }
	}
      }
      for (int i = 0; i < n; i++) {
	Player him = scratch[i];
	him.StopThread();
	him.out_thread.StopThread();
	System.out.println("WatchDog: time out on " + him.id + "...");
	him.GB(1);
	him.Disconnect();
	scratch[i] = null;
      }
    }
  }
}
