package common;

public class Tile {
  public static final String wind_names[] = {"EAST", "SOUTH", "WEST", "NORTH"};
  
  // Constants
  public static final byte Void = 0;
  public static final byte Mega = 1;
  public static final byte Disc = 2;
  public static final byte Stick = 3;
  public static final byte Wind = 4;
  public static final byte Dragon = 5;
  public static final byte Flower = 6;

  public static byte getFlavor (byte v) {
    if (v >= 1 && v <= 9)
      return Tile.Mega;
    if (v >= 10 && v <= 18)
      return Tile.Disc;
    if (v >= 19 && v <= 27)
      return Tile.Stick;
    return Tile.Void;
  }

  public static int getSerial (byte v) {
    if (v >= 1 && v <= 9)
      return (int)v;
    if (v >= 10 && v <= 18)
      return (int)v - 9;
    if (v >= 19 && v <= 27)
      return (int)v - 18;
    return 0;
  }

  public static byte stepUp (byte v) {
    if (v < 1 || v > 27)
      return 0;
    if (v == 9 || v == 18 || v == 27)
      return 0;
    return (byte)(v + 1);
  }

  public static byte stepDn (byte v) {
    if (v < 1 || v > 27)
      return 0;
    if (v == 1 || v == 10 || v == 19)
      return 0;
    return (byte)(v - 1);
  }

  public static boolean isFlower (byte v) {
    return (v > 34) && (v <= 42);
  }

  public static boolean isWord (byte v) {
    return (v > 27 && v < 35);
  }

  public static boolean isSuit (byte v) {
    return (v > 0 && v < 28);
  }
}

