package common;

import java.util.Random;

public class Hand {
  // Constants
  public static final byte Single = 0;
  public static final byte Head = 1;
  public static final byte Middle = 2;
  public static final byte Tail = 3;

  private static final int IncLength = 4;
  
  private int length = 0;
  private byte tiles[];
  private byte groups[];
  private boolean select[];

  public Hand () {
    tiles = new byte[8];
    groups = new byte[8];
    select = new boolean[8];
    length = 0;
  }

  // Construct a new hand with given buffer size.
  public Hand (int size) {
    int i = 0, j = 0, s = 0;
    if (size < 0)
      size = 15;
    tiles = new byte[size];
    groups = new byte[size];
    select = new boolean[size];
  }

  // Construct a hand from a string description.
  public Hand (String s) {
    tiles = new byte[20];
    groups = new byte[20];
    select = new boolean[20];
    length = 0;
    int p = 0, d = 0, v = 0;
    boolean grp = false, hd = false;
    
    while (true) {
      while (Character.isSpace(s.charAt(p))) p++;
      if (s.charAt(p) == '.') {
	return;
      }
      if (s.charAt(p) == '[') {
	p++;
	grp = true;
	hd = true;
	continue;
      }
      if (s.charAt(p) == ']') {
	p++;
	if (length > 0) {
	  groups[length - 1] = Tail;
	  grp = false;
	}
	continue;
      }
      v = 0;
      while ((d = Character.digit(s.charAt(p), 10)) != -1) {
	v = v * 10 + d;
	p++;
      }
      if (v == 0) {
	length = 0;
	return;
      }
      if (!grp) {
	tiles[length] = (byte)v;
	groups[length] = Single;
	length++;
      } else if (hd) {
	tiles[length] = (byte)v;
	groups[length] = Head;
	length++;
	hd = false;
      } else {
	tiles[length] = (byte)v;
	groups[length] = Middle;
	length++;
      }
    }
  }

  public synchronized int length () {
    return length;
  }

  public synchronized void setLength (int l) {
    if (l > length) {
      throw new IndexOutOfBoundsException();
    }
    length = l;
  }

  public synchronized String toString () {
    int i = 0;
    StringBuffer buf = new StringBuffer(6 * length);
    for (i = 0; i < length; i++) {
      if (select[i])
	buf.append('!');
      if (groups[i] == Head)
	buf.append('[');
      buf.append(tiles[i]);
      if (groups[i] == Tail)
	buf.append(']');
      buf.append(' ');
    }
    buf.append('.');
    return buf.toString();
  }
	
  public synchronized byte tileAt (int p) {
    if (p < 0 || p >= length) {
      throw new IndexOutOfBoundsException();
    }
    return tiles[p];
  }

  public synchronized byte groupAt (int p) {
    if (p < 0 || p >= length) {
      throw new IndexOutOfBoundsException();
    }
    return groups[p];
  }    

  public synchronized boolean isSelected (int p) {
    if (p < 0 || p >= length) {
      throw new IndexOutOfBoundsException();
    }
    return select[p];
  }    

  public synchronized void setTileAt (int p, byte v, byte g) {
    if (p < 0 || p >= length) {
      throw new IndexOutOfBoundsException();
    }
    tiles[p] = v;
    groups[p] = g;
  }

  // Add a single tile to the end of the hand.
  public synchronized Hand addTile (byte v, byte g, boolean s) {
    if (length == tiles.length) {
      byte old_tiles[] = tiles;
      byte old_groups[] = groups;
      boolean old_select[] = select;

      tiles = new byte[length + IncLength];
      groups = new byte[length + IncLength];
      select = new boolean[length + IncLength];

      System.arraycopy(old_tiles, 0, tiles, 0, length);
      System.arraycopy(old_groups, 0, groups, 0, length);
      System.arraycopy(old_select, 0, select, 0, length);
    }
    tiles[length] = v;
    groups[length] = g;
    select[length] = s;
    length++;
    return this;
  }

  public synchronized Hand addTile (byte v) {
    return addTile(v, Single, false);
  }

  public synchronized Hand killTile (int p) {
    if (p < 0 || p >= length) {
      throw new IndexOutOfBoundsException();
    }
    if (p == length - 1) {
      length--;
      return this;
    }
    System.arraycopy(tiles, p + 1, tiles, p, length - p - 1);
    System.arraycopy(groups, p + 1, groups, p, length - p - 1);
    System.arraycopy(select, p + 1, select, p, length - p - 1);
    length--;
    return this;
  }

  // Insert a tile.
  public synchronized Hand insertTile (int p, byte v, byte g, boolean s) {
    if (p >= length) {
      return addTile(v, g, s);
    }
    if (p < 0) {
      p = 0;
    }
    if (length == tiles.length) {
      byte old_tiles[] = tiles;
      byte old_groups[] = groups;
      boolean old_select[] = select;

      tiles = new byte[length + IncLength];
      groups = new byte[length + IncLength];
      select = new boolean[length + IncLength];

      System.arraycopy(old_tiles, 0, tiles, 0, p);
      System.arraycopy(old_groups, 0, groups, 0, p);
      System.arraycopy(old_select, 0, select, 0, p);

      System.arraycopy(old_tiles, p, tiles, p + 1, length - p);
      System.arraycopy(old_groups, p, groups, p + 1, length - p);
      System.arraycopy(old_select, p, select, p + 1, length - p);
      
      tiles[p] = v;
      groups[p] = g;
      select[p] = s;
      length++;
      return this;
    }
    System.arraycopy(tiles, p, tiles, p + 1, length - p);
    System.arraycopy(groups, p, groups, p + 1, length - p);
    System.arraycopy(select, p, select, p + 1, length - p);
    
    tiles[p] = v;
    groups[p] = g;
    select[p] = s;
    length++;
    return this;
  }

  // Append another hand to the end of this one.
  public synchronized Hand append (Hand h) {
    if (length + h.length > tiles.length) {
      byte old_tiles[] = tiles;
      byte old_groups[] = groups;
      boolean old_select[] = select;

      tiles = new byte[length + h.length + IncLength];
      groups = new byte[length + h.length + IncLength];
      select = new boolean[length + h.length + IncLength];

      System.arraycopy(old_tiles, 0, tiles, 0, length);
      System.arraycopy(old_groups, 0, groups, 0, length);
      System.arraycopy(old_select, 0, select, 0, length);
    }
    System.arraycopy(h.tiles, 0, tiles, length, h.length);
    System.arraycopy(h.groups, 0, groups, length, h.length);
    System.arraycopy(h.select, 0, select, length, h.length);
    length += h.length;
    return this;
  }

  // What's the point of Object.clone()?
  public synchronized Hand Clone () {
    Hand h = new Hand(length);

    System.arraycopy(tiles, 0, h.tiles, 0, length);
    System.arraycopy(groups, 0, h.groups, 0, length);
    System.arraycopy(select, 0, h.select, 0, length);
    h.length = length;
    return h;
  }

  // Make a selection.
  // If the selected tile is in a group, select the entire group.
  // Return the position of the first tile of the selected group.
  public synchronized int select (int p) {
    if (p < 0 || p >= length)
      throw new ArrayIndexOutOfBoundsException();
    if (groups[p] == Single) {
      select[p] = true;
      return p;
    } else {  // this tile belongs to a group
      int i = p, j = p;
      
      while (groups[i] != Head) {
	select[i] = true;
	i--;
      }
      select[i] = true;
      while (groups[j] != Tail) {
	select[j] = true;
	j++;
      }
      select[j] = true;
      return i;
    }
  }

  // Undo a selection.
  // If the selection is a group, unselect all tiles in the group.
  // Return the position of the first tile of the unselected group.
  public synchronized int unselect (int p) {
    if (p < 0 || p >= length)
      throw new ArrayIndexOutOfBoundsException();
    if (groups[p] == Single) {
      select[p] = false;
      return p;
    } else {  // this tile belongs to a group
      int i = p, j = p;
      
      while (groups[i] != Head) {
	select[i] = false;
	i--;
      }
      select[i] = false;
      while (groups[j] != Tail) {
	select[j] = false;
	j++;
      }
      select[j] = false;
      return i;
    }
  }

  // Cancel all selections.
  public synchronized void unselectAll () {
    int i = 0;
    for (i = 0; i < length; i++)
      select[i] = false;
  }

  // Select all tiles at once.
  public synchronized void selectAll () {
    int i = 0;
    for (i = 0; i < length; i++)
      select[i] = true;
  }

  // Make all tiles Single, and cancel all selections.
  public synchronized void clearAll () {
    int i = 0;
    for (i = 0; i < length; i++) {
      groups[i] = Single;
      select[i] = false;
    }
  }

  // Put all the selected tiles into a single group.
  // We need to be careful not to disturb existing groups.
  // Return whether any tiles are selected.
  public synchronized boolean group () {
    int i = 0, q = 0, p = 0;
  loop: 
    {
      for (i = 0; i < length; i++) {
	if (select[i]) {
	  p = q = i;
	  break loop;
	}
      }
      return false;
    }
    for (i++; i < length; i++) {
      if (select[i]) {
	int j = 0;
	byte v = tiles[i];
	
	for (j = i; j > q; j--) {
	  tiles[j] = tiles[j - 1];
	  groups[j] = groups[j - 1];
	  select[j] = select[j - 1];
	}
	q++;
	tiles[q] = v;
	select[q] = true;
      }
    }
    if (p == q) {
      groups[p] = Single;
      return true;
    }
    groups[p] = Head;
    groups[q] = Tail;
    for (i = p + 1; i < q; i++)
      groups[i] = Middle;
    return true;
  }

  // Mark all the selected tiles as Single.
  // Return whether there are selected tiles.
  public synchronized boolean ungroup () {
    int i = 0;
    boolean tf = false;

    for (i = 0; i < length; i++) {
      if (select[i]) {
	tf = true;
	groups[i] = Single;
      }
    }
    return tf;
  }

  // See if the selected tiles are a single group.
  public synchronized boolean isGroup () {
    int i = 0;
  loop:
    {
      for (i = 0; i < length; i++) {
	if (select[i])
	  break loop;
      }
      return false;
    }
    if (groups[i] == Single) {
      for (i++; i < length; i++) {
	if (select[i])
	  return false;
      }
      return true;
    }
    if (groups[i] != Head)
      return false;
    for (i++; i < length; i++) {
      if (groups[i] == Middle) {
	if (!select[i])
	  return false;
      } else
	break;
    }
    if (!select[i])
      return false;
    for (i++; i < length; i++) {
      if (select[i])
	return false;
    }
    return true;
  }
  
	
  // Delete all the selected tiles.
  public synchronized boolean delete () {
    int i = 0, j = 0;
    boolean tf = false;

    for (i = 0; i < length; i++) {
      if (!select[i]) {
	tiles[j] = tiles[i];
	groups[j] = groups[i];
	select[j] = select[i];
	j++;
      } else {
	tf = true;
      }
    }
    length = j;
    return tf;
  }
/*
  // Look for and select all tiles with the given value.
  // Selected tiles must be single, or only grouped with his twins.
  // Return the number of tiles found.
  public int search (byte t) {
    int s = 0, c = 0, g = 0, i = 0;

    for (s = 0; s < length; s++) {
    inner_loop:
      if (tiles[s].value == t) {
	g = tiles[s].getGroup();
	if (g == Tile.Single) {
	  tiles[s].marks |= Select;
	  c++;
	} else if (g == Tile.Head) {
	  i = s;
	  do {
	    i++;
	    if (tiles[i].value != t)
	      break inner_loop;
	  } while (tiles[i].getGroup() != Tile.Tail);
	  for (; s <= i; s++) {
	    tiles[s].marks |= Select;
	    c++;
	  }
	}
      }
    }
    return c;
  }
*/
  // Look for n tiles of the given value, and select them.
  // We assume that they are all single tiles.
  public synchronized int searchN (byte v, int n) {
    int i = 0, c = 0;
    
    for (i = 0; i < length; i++) {
      if (tiles[i] == v && n > 0) {
	select[i] = true;
	c++;
	n--;
      }
    }
    return c;
  }
  
  // Return a hand with the selected tiles.  s controls whether the new
  // hand keeps all the tiles selected or not.
  public synchronized Hand selection (boolean s) {
    int i = 0;
    Hand h = new Hand(4);
    for (i = 0; i < length; i++) {
      if (select[i]) {
	h.addTile(tiles[i], groups[i], s);
      }
    }
    return h;
  }

  // Return a hand with the unselected tiles.
  public synchronized Hand unselected (boolean s) {
    int i = 0;
    Hand h = new Hand(length);
    for (i = 0; i < length; i++) {
      if (!select[i]) {
	h.addTile(tiles[i], groups[i], s);
      }
    }
    return h;
  }
  
  // Compare another hand's tiles against this one's.
  // Return 0 if they match one-to-one, -1 if the other hand has more tiles,
  // and n if this hand has n more tiles.
  public synchronized int compareTo (Hand h) {
    byte v = 0;
    int s = 0, d = 0, n = 0;
    unselectAll();
    for (s = 0; s < h.length; s++) {
    inner_loop: 
      {
	v = h.tiles[s];
	for (d = 0; d < length; d++) {
	  if ((v == tiles[d]) && !select[d]) {
	    select[d] = true;
	    break inner_loop;
	  }
	}
	unselectAll();
	return -1;
      }
    }
    for (d = 0; d < length; d++) {
      if (!select[d])
	n++;
    }
    unselectAll();
    return n;
  }
/*
  // Move a group at position from to position to, to the left only.
  public void moveLeft (int to, int from) {
    int f0 = from, f1 = from, t0 = to;
    int g = 0;
    while ((g = groups[f0]) != Head && g != Single)
      f0--;
    while ((g = groups[f1]) != Tile.Tail && g != Tile.Single)
      f1++;
    while ((g = tiles[t0].getGroup()) != Tile.Head && g != Tile.Single)
      t0--;
    if (f0 <= t0)
      return;
    int fl = f1 - f0 + 1, i = 0;
    Tile buf[] = new Tile[fl];
    for (i = 0; i < fl; i++)
      buf[i] = tiles[f0 + i];
    for (i = 0; i < f0 - t0; i++)
      tiles[f1 - i] = tiles[f0 - 1 - i];
    for (i = 0; i < fl; i++)
      tiles[t0 + i] = buf[i];
    return;
  }

  // Move a group at position from to position to, to the right only.
  public void moveRight (int to, int from) {
    int f0 = from, f1 = from, t1 = to;
    int g = 0;
    while ((g = tiles[f0].getGroup()) != Tile.Head && g != Tile.Single)
      f0--;
    while ((g = tiles[f1].getGroup()) != Tile.Tail && g != Tile.Single)
      f1++;
    while ((g = tiles[t1].getGroup()) != Tile.Tail && g != Tile.Single)
      t1++;
    if (f1 >= t1)
      return;
    int fl = f1 - f0 + 1, i = 0;
    Tile buf[] = new Tile[fl];
    for (i = 0; i < fl; i++)
      buf[i] = tiles[f0 + i];
    for (i = 0; i < t1 - f1; i++) 
      tiles[f0 + i] = tiles[f1 + 1 + i];
    for (i = 0; i < fl; i++)
      tiles[t1 - i] = buf[fl - 1 - i];
    return;
  }
*/

  public synchronized void shuffle () {
    Random rand = new Random();
    int i = 0, j = 0;
    byte v;

    clearAll();
    for (i = 1; i < length; i++) {
      j = Math.abs(rand.nextInt()) % (i + 1);
      v = tiles[i];
      tiles[i] = tiles[j];
      tiles[j] = v;
    }
  }

  public synchronized void sort () {
    int i = 0;
    boolean switched = false;
    byte v = 0, g = 0;
    boolean s = false;

    do {
      switched = false;
      for (i = 0; i < length - 1; i++) {
	if (tiles[i] > tiles[i + 1]) {
	  v = tiles[i];
	  tiles[i] = tiles[i + 1];
	  tiles[i + 1] = v;
	  g = groups[i];
	  groups[i] = groups[i + 1];
	  groups[i + 1] = g;
	  s = select[i];
	  select[i] = select[i + 1];
	  select[i + 1] = s;
	  switched = true;
	}
      }
    } while (switched);
  }
}

