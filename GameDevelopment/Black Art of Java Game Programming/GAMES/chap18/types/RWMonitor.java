// a monitor that allows only 1 writer, but arbitrary # of readers.

package types;

class Monitor {
  public synchronized void mywait () throws InterruptedException {
    wait();
  }
  public synchronized void mynotify () {
    notify();
  }
  public synchronized void mynotifyAll () {
    notifyAll();
  }
}

public class RWMonitor {
  private int n_readers = 0;
  private int n_writers = 0;
  private int n_queued = 0;
  private Monitor reader_mon = new Monitor();
  private Monitor writer_mon = new Monitor();
  private String name = "";
  private StringBuffer comments = new StringBuffer(100);

  public RWMonitor (String s) {
    name = s;
  }

  private synchronized boolean readable (String comment) {
    if (n_writers > 0) {
      comments.append(comment + ": queued after " + n_writers + " writers.\n");
      return false;
    }
    n_readers++;
    return true;
  }

  private synchronized boolean writable (String comment) {
    n_writers++;
    if (n_writers > 1) {
      System.out.println(comment + ": inWrite() queued after " + (n_writers - 1) + " writers.");
      return false;
    }
    if (n_readers > 0) {
      System.out.println(comment + ": inWrite() queued after " + n_readers + " readers::");
      System.out.println(comments);
      comments.setLength(0);
      return false;
    }
    return true;
  }

  public void inRead (String comment) {
    while (!readable(comment)) {
      try {
	reader_mon.mywait();
      } catch (InterruptedException e) {
	System.out.println(comment + ": inRead() interrupted!");
      }
    }
    comments.append(comment + ": >>inRead()\n");
  }
  
  public synchronized void outRead (String comment) {
    comments.append(comment + ": >>outRead()\n");
    if (--n_readers == 0) {
      comments.setLength(0);
      if (n_writers > 0) {
	writer_mon.mynotify();
      }
    }
  }

  public void inWrite (String comment) {
    if (!writable(comment)) {
      try {
	writer_mon.mywait();
      } catch (InterruptedException e) {
	System.out.println(comment + ": inWrite() interrupted!");
      }
      System.out.println(comment + ": >>inWrite()");
    }
  }

  public synchronized void outWrite (String comment) {
    if (--n_writers > 0) {
      writer_mon.mynotify();
    } else {
      reader_mon.mynotifyAll();
    }
  }
}
