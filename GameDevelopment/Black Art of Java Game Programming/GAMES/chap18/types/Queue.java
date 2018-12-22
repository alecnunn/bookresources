package types;

class QueueLink {
  public QueueLink next;
  public Object obj;

  public QueueLink (Object o) {
    obj = o;
    next = null;
  }
}

public class Queue {
  private QueueLink head = null, tail = null;
  
  public synchronized Object pop () {
    while (head == null) {
      try {
	wait();
      } catch (InterruptedException e) {
      }
    }
    Object o = head.obj;
    head = head.next;
    if (head == null) {
      tail = null;
    }
    return o;
  }
  
  public synchronized void push (Object o) {
    if (head == null) {
      head = tail = new QueueLink(o);
      notify();
    } else {
      tail.next = new QueueLink(o);
      tail = tail.next;
    }
  }
}
   
