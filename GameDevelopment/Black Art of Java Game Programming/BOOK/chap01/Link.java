class Node {
  int value;
  Node next;
  
  public Node(int i) {
    value = i;
    next = null;
  }

  public void print() {
    System.out.println(value);
  }

  public Node next() {
    return next;
  }

  public void setNext(Node n) {
    next = n;
  }

}

class Link {
  public static void main(String argv[]) {
    Node a;
    Node b;
    Node c;
    a = new Node(1);
    b = new Node(2);
    c = new Node(3);
    a.setNext(b);
    b.setNext(c);
    c.setNext(a);
    Node i;
    i = a;
    while (i != null) {
      i.print();
      i = i.next();
    }
  }
}
      
