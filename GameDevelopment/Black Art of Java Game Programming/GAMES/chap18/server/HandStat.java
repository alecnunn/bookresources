package server;

import common.Tile;
import common.Hand;

class HandStat {
  long all_tiles = 0;
  boolean select[] = new boolean[5];
  long groups[] = new long[5];
  byte flavors[] = new byte[5];
  int n_flowers = 0;
  int n_pub_quads = 0;
  int n_pri_quads = 0;
  
  public HandStat (Hand h) {
    int i = 0, c = 1, j = 0;
    while (i < h.length()) {
      if (h.groupAt(i) == Hand.Single) {
	i++;
	n_flowers++;
      } else {
	long bits = 0;
	int n = 0;
	byte g = 0;
	boolean selected = false;
	do {
	  n++;
	  bits |= (1L << (int)h.tileAt(i));
	  all_tiles |= (1L << (int)h.tileAt(i));
	  selected |= h.isSelected(i);
	  i++;
	} while (i < h.length() &&
		 ((g = h.groupAt(i)) == Hand.Middle ||
		  g == Hand.Tail));
	if (n == 2) {
	  j = 0;
	} else {
	  j = c++;
	}
	groups[j] = bits;
	select[j] = selected;
	if (bits < (1L << 10)) {
	  flavors[j] = Tile.Mega;
	} else if (bits < (1L << 19)) {
	  flavors[j] = Tile.Disc;
	} else if (bits < (1L << 28)) {
	  flavors[j] = Tile.Stick;
	} else if (bits < (1L << 32)) {
	  flavors[j] = Tile.Wind;
	} else {
	  flavors[j] = Tile.Dragon;
	}
	if (n == 4) {
	  if (selected)
	    n_pub_quads++;
	  else
	    n_pri_quads++;
	}

//	System.out.println("Group[" + j + "], bits = " + bits + 
//			   ", flavor = " + flavors[j]);
      }
    }
  }
  
  public boolean isAllConn () {
    int i = 1;
    for (i = 1; i < 5; i++) {
      if ((groups[i] & (groups[i] - 1L)) == 0)
	return false;
    }
    return true;
  }
  
  public boolean isSisters () {
    long temp[] = { 7L, 14L, 28L, 56L, 112L, 224L, 448L, 896L, 1792L };
    int shift[] = { 1, 10, 19 };
    int suits[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int i = 1, j = 0, k = 0;
    long b = 0;
    for (i = 1; i < 5; i++) {
    loop:
      if ((flavors[i] != Tile.Wind) && (flavors[i] != Tile.Dragon)) {
	for (k = 0; k < 3; k++) {
	  b = groups[i] >> shift[k];
	  for (j = 0; j < 9; j++) {
	    if (temp[j] == b) {
	      suits[j] |= (1 << k);
	      break loop;
	    }
	  }
	}
      }
    }
    for (j = 0; j < 9; j++) {
      if (suits[j] == 7) {
	return true;
      }
    }
    return false;
  }

  public boolean isMixSnake () {
    long temp[] = { 14L, 112L, 896L, 512L * 14L, 512L * 112L, 512L * 896L,
		    262144L * 14L, 262144L * 112L, 262144L * 896L };
    long patterns[] = { 14L | 512L * 112L | 262144L * 896L,
			14L | 512L * 896L | 262144L * 112L,
			112L | 512L * 14L | 262144L * 896L,
			112L | 512L * 896L | 262144L * 14L,
			896L | 512L * 14L | 262144L * 112L,
			896L | 512L * 112L | 262144L * 14L, };
    long bits = 0;
    for (int i = 0; i < 4; i++) {
    loop:
      for (int j = 0; j < temp.length; j++) {
	if (groups[i] == temp[j]) {
	  bits |= groups[i];
	  break loop;
	}
      }
    }
    for (int i = 0; i < patterns.length; i++) {
      if ((bits & patterns[i]) == patterns[i]) {
	return true;
      }
    }
    return false;
  }    
  
  public boolean isOneSuit () {
    int suits = 0;
    int i = 0;
    for (i = 0; i < 5; i++) {
      if ((flavors[i] != Tile.Wind) && (flavors[i] != Tile.Dragon)) {
	suits |= (1 << flavors[i]);
      }
    }
    if (suits == 0)
      return false;
    if ((suits & (suits - 1)) == 0) {
      return true;
    } else
      return false;
  }
  
  public boolean isAllTrip () {
    int i = 1;
    for (i = 1; i < 5; i++) {
      if ((groups[i] & (groups[i] - 1L)) != 0)
	return false;
    }
    return true;
  }
  
  public boolean isAllOneNine () {
    long mask = ((1L << 1) | (1L << 9) | (1L << 10) |
		 (1L << 18) | (1L << 19) | (1L << 27));
    int i = 0, c = 0;
    for (i = 0; i < 5; i++) {
      if (groups[i] >= (1L << 28))
	continue;
      if ((groups[i] & mask) == 0)
	return false;
      c++;
    }
    if (c == 0)
      return false;
    return true;
  }
  
  public boolean isSnake () {
    long temp[] = { 7L, 56L, 448L };
    int shift[] = { 1, 10, 19 };
    int snakes[] = { 0, 0, 0 };
    int i = 1, j = 0, k = 0;
    long b = 0;
    for (i = 1; i < 5; i++) {
    loop:
      if ((flavors[i] != Tile.Wind) && (flavors[i] != Tile.Dragon)) {
	for (k = 0; k < 3; k++) {
	  b = groups[i] >> shift[k];
	  for (j = 0; j < 3; j++) {
	    if (b == temp[j]) {
	      snakes[k] |= (1 << j);
	      break loop;
	    }
	  }
	}
      }
    }
    for (k = 0; k < 3; k++) {
      if (snakes[k] == 7) {
	return true;
      }
    }
    return false;
  }

  public boolean isConcealedSnake () {  // call only after isSnake()
    long bits = 0;
    long temps[] = { 1022L, 1022L * 512L, 1022L * 262144L };
    for (int i = 1; i < 5; i++) {
      if (!select[i])
	bits |= groups[i];
    }
//    System.out.println("bits = " + bits);
    for (int i = 0; i < temps.length; i++) {
      if ((bits & temps[i]) == temps[i])
	return true;
    }
    return false;
  }

  public boolean isPureSuit () {
    int suits = 0;
    int i = 0;
    for (i = 0; i < 5; i++) {
      if ((flavors[i] == Tile.Wind) || (flavors[i] == Tile.Dragon))
	return false;
      suits |= (1 << flavors[i]);
    }
    if ((suits & (suits - 1)) == 0) {
      return true;
    } else
      return false;
  }

  public boolean isPureWord () {
    int i = 0;
    for (i = 0; i < 5; i++) {
      if ((flavors[i] != Tile.Wind) && (flavors[i] != Tile.Dragon))
	return false;
    }
    return true;
  }

  public int countWords (byte round_wind, byte seat_wind) {
    int i = 0, s = 0;
    for (i = 1; i < 5; i++) {
      if (groups[i] == (1L << round_wind))
	s++;
      if (groups[i] == (1L << seat_wind))
	s++;
      if (flavors[i] == Tile.Dragon)
	s++;
    }
    return s;
  }
}

class MJStat {
  public int fan = 0;
  public int bonus = 0;
  public int score = 0;
  public Hand hand = null;
  public MJContext mjc = null;
  public StringBuffer summary = new StringBuffer();
}

class MJContext {
  public boolean self_feed = false;
  public boolean win_on_flower = false;
  public boolean grab_fourth = false;
  public boolean no_grabs = false;
  public boolean all_grabs = false;
  public boolean is_dealer = false;
  public int feeder = -1;
  public byte round_wind, seat_wind;
}
