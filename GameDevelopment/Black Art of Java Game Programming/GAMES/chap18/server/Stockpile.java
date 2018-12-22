package server;

import common.*;

class Stockpile {
  private int draw_head;
  private int draw_tail;
  private Hand tiles;
  private Table table;

  public Stockpile (Table t) {
    int i = 0;
    tiles = new Hand(144);
    for (i = 1; i <= 34; i++) {
      for (int j = 0; j < 4; j++)
	tiles.addTile((byte)i);
    }
    for (; i <= 42; i++)
      tiles.addTile((byte)i);
    draw_head = 0;
    draw_tail = 143;
    table = t;
  }
  
  public synchronized void shuffle () {
    draw_head = 0;
    draw_tail = 143;
    tiles.shuffle();
  }

  public synchronized byte DrawHead (int s) {
    if (draw_head >= draw_tail - 12)
      return (byte)0;
    byte t = tiles.tileAt(draw_head++);
    while (Tile.isFlower(t)) {
      table.on_flower = true;
      table.pub_hands[s].addTile(t);
      t = tiles.tileAt(draw_tail--);
    }
    table.pri_hands[s].addTile(t);
    return t;
  }

  public synchronized byte DrawTail (int s) {
    byte t = tiles.tileAt(draw_tail--);
    table.on_flower = true;
    while (Tile.isFlower(t)) {
      table.pub_hands[s].addTile(t);
      t = tiles.tileAt(draw_tail--);
    }
    table.pri_hands[s].addTile(t);
    return t;
  }
}
