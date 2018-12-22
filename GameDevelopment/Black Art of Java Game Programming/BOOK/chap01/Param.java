class Param {
  static void foo (int bar) {
    bar++;
  }
  static void foo2 (int a[]) {
    a[0] = 1;
  }
  static public void main(String argv[]) {
    int x;
    int y[];
    x = 3;
    y = new int[2];
    y[0] = 17;
    foo(x);
    System.out.println("x = " + x);
    foo2(y);
    System.out.println("y[0] = " + y[0]);
  }
}