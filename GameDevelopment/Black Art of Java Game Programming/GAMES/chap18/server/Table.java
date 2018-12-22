package server;

import java.util.Random;

import common.*;

class Table {
  private static final int DEALER_BONUS = 5;

  public static Table global[] = new Table[Config.max_tables];

  // Constants for Table.status.
  public static final int IDLE = 0;  // waiting to start a new game
  public static final int DISCARD = 1;  // someone's turn to discard a tile
  public static final int AUCTION = 2;  // a tile has been discarded
  public static final int FOURTH = 3;  // a 4th tile of a quadruplet

  int id = -1;
  int n_players = 0;
  int status = IDLE;
  int turn = -1;
  int dealer = -1;
  int first_dealer = -1;
  int round = 0;
  TableOptions opt = null;
  Stockpile stockpile = null;
  int last_seat = -1;
  byte last_active = 0;
  byte active = 0;
  boolean on_flower = false;
  Player players[] = new Player[Config.max_table_players];
  byte actions[] = { -1, -1, -1, -1 };
  int action_masks[] = { 1, 1, 1, 1 };
  MJStat mj_stats[] = new MJStat[4];
  int cur_scores[] = { -1, -1, -1, -1 };
  Hand pri_hands[] = new Hand[4];
  Hand pub_hands[] = new Hand[4];
  Hand semi_pubs[] = new Hand[4];
  Hand out_tiles[] = new Hand[4];
  int n_grabs[] = { 0, 0, 0, 0 };
  boolean invite_only = false;
  int invitations[] = { -1, -1, -1, -1, -1, -1, -1, -1 };
  private Random rand = new Random();
  private long last_urge = 0;

  public Table (TableOptions options, boolean invite, Player first) throws FullServerException {
    boolean tf = false;
    synchronized (global) {
      for (int i = 0; i < global.length; i++) {
	if (global[i] == null) {
	  id = i;
	  global[i] = this;
	  tf = true;
	  break;
	}
      }
    }
    if (!tf) {
      throw new FullServerException();
    }
    stockpile = new Stockpile(this);
    last_urge = System.currentTimeMillis();
    for (int i = 0; i < players.length; i++) {
      players[i] = null;
    }
    for (int i = 0; i < 4; i++) {
      pri_hands[i] = new Hand(15);
      pub_hands[i] = new Hand(15);
      semi_pubs[i] = new Hand(15);
      out_tiles[i] = new Hand(30);
    }
    invite_only = invite;
    opt = options;
    players[0] = first;
    first.table = this;
    first.seat = 0;
    if (invite_only)
      invitations[0] = first.id;
    Player.outputAll(Packet.SPTableOpt(id, opt));
    Player.outputAll(Packet.SPTablePlayer(id, 0, first.id));
  }

  static int nextTurn (int s) {
    return (s + 3) % 4;
  }

  byte roundWind () {
    return (byte)(28 + round);
  }

  byte seatWind (int seat) {
    return (byte)(28 + (4 - seat + dealer) % 4);
  }

  public synchronized void outputTable (byte[] p) {
    int i = 0;
    for (i = 0; i < players.length; i++) {
      if (players[i] != null) {
	players[i].output(p);
      }
    }
  }

  public synchronized void outputHands (Player p, int seat, boolean priv) {
    int i = 0;
    byte[] spt = Packet.SPTiles(p.id, seat, pri_hands[seat].length(),
				pub_hands[seat], semi_pubs[seat], false);
    if (priv) 
      p.output(Packet.SPYourPrivate(pri_hands[seat]));
    p.output(Packet.SPTiles(p.id, seat, 
			    pri_hands[seat].length(),
			    pub_hands[seat], semi_pubs[seat], true));
    for (i = 0; i < players.length; i++) {
      if ((players[i] != null) && (i != seat)) {
	players[i].output(spt);
      }
    }
  }
   
  boolean AllSeated () {
    int i = 0;
    for (i = 0; i < 4; i++) {
      if (players[i] == null)
	return false;
    }
    return true;
  }

  boolean AllPlayed () {
    int i = 0;
    for (i = 0; i < 4; i++) {
      if (players[i] == null)
	return false;
      if (actions[i] == -1 && turn != i)
	return false;
    }
    return true;
  }

  public synchronized void SetOptions (TableOptions options, Player him) {
    if (him.seat != 0) {
      him.output(Packet.SPMessage(Packet.MSG_GOD, 0,
			       "You cannot set this table's options!"));
      return;
    }
    if (status != IDLE) {
      him.output(Packet.SPMessage(Packet.MSG_GOD, 0,
			       "You cannot set options when a game is in progress!"));
      return;
    }
    opt = options;
    Player.outputAll(Packet.SPTableOpt(id, opt));
  }

  public synchronized void urge (Player whiner) {
    long time = System.currentTimeMillis();
    if (time - last_urge > 30000) {
      last_urge = time;
      for (int s = 0; s < 4; s++) {
	byte[] pkt = Packet.SPUrge();
	if (players[s] != null) {
	  players[s].output(pkt);
	}
      }
    } else {
      whiner.output(Packet.SPMessage(Packet.MSG_GOD, 0, "Chill out!"));
    }
  }

  public synchronized void Play (Player him, int seat, byte action, byte tile) {
    if (seat < 0 || seat > 3) {
      him.output(Packet.SPMessage(Packet.MSG_GOD, 0, 
			       "You can only observe the game."));
      return;
    }
    Hand pri = pri_hands[seat];
    Hand pub = pub_hands[seat];
    Hand sem = semi_pubs[seat];
    switch (status) {
    case IDLE:
      actions[seat] = Packet.PLAY_PASS;
      outputTable(Packet.SPWhoPlayed(actions));
      if (AllPlayed()) {
	NewGame();
      }
      return;
    case DISCARD:
      if (actions[seat] != -1) {
	him.output(Packet.SPYourPlay(Packet.YPLAY_DISCARD, false, 1));
	return;
      }
      if (((1 << action) & action_masks[seat]) == 0) {
	him.output(Packet.SPYourPlay(Packet.YPLAY_DISCARD, false, 0));
	return;
      }
      switch (action) {
      case Packet.PLAY_DISCARD:
	pri.unselectAll();
	if (pri.searchN(tile, 1) == 0) {
	  him.output(Packet.SPYourPlay(Packet.YPLAY_DISCARD, false, 2));
	  return;
	}
	pri.delete();
	RegisterDiscard(tile);
	return;
      case Packet.PLAY_FOUR:
	pri.unselectAll();
	if (pri.searchN(tile, 4) == 4) {
	  Hand played = pri.selection(true);
	  played.group();
	  played.unselectAll();
	  sem.append(played);
	  pri.delete();
	  byte drawn = stockpile.DrawTail(seat);
	  outputHands(him, seat, false);
	  him.output(Packet.SPYourPlay(Packet.YPLAY_DISCARD, true, 0));
	  him.output(Packet.SPYourDraw(drawn));
	  RegisterDiscardMode();
	  return;
	}
	pri.unselectAll();
	if (pri.searchN(tile, 1) == 1) {
	  pub.unselectAll();
	  if (pub.searchN(tile, 3) != 3 || !pub.isGroup()) {
	    pub.unselectAll();
	    him.output(Packet.SPYourPlay(Packet.YPLAY_DISCARD, false, 3));
	    return;
	  }
	  pri.delete();
	  RegisterFourth(tile);
	  return;
	}
	him.output(Packet.SPYourPlay(Packet.YPLAY_DISCARD, false, 4));
	return;
      case Packet.PLAY_WIN:
	RegisterWin(seat);
	actions[seat] = Packet.PLAY_WIN;
	EndGame();
	return;
      default:  // shouldn't fall through anyway
	return;
      }
    case FOURTH:
      if (actions[seat] != -1) {
	him.output(Packet.SPYourPlay(Packet.YPLAY_BID, false, 1));
	return;
      }
      if (((1 << action) & action_masks[seat]) == 0) {
	him.output(Packet.SPYourPlay(Packet.YPLAY_BID, false, 0));
	return;
      }
      RegisterBid(seat, action);
      return;
    case AUCTION:
      if (actions[seat] != -1) {
	him.output(Packet.SPYourPlay(Packet.YPLAY_BID, false, 1));
	return;
      }
      if (((1 << action) & action_masks[seat]) == 0) {
	him.output(Packet.SPYourPlay(Packet.YPLAY_BID, false, 0));
	return;
      }
      pri.unselectAll();
      switch (action) {
      case Packet.PLAY_PASS:
      case Packet.PLAY_DRAW:
	if (nextTurn(turn) == seat) {
	  RegisterBid(seat, Packet.PLAY_DRAW);
	} else {
	  RegisterBid(seat, Packet.PLAY_PASS);
	}
	return;
      case Packet.PLAY_CONN_UP:
	if (nextTurn(turn) != seat) {
	  him.output(Packet.SPYourPlay(Packet.YPLAY_BID, false, 5));
	  return;
	}
	if (pri.searchN(Tile.stepUp(active), 1) == 0 ||
	    pri.searchN(Tile.stepUp(Tile.stepUp(active)), 1) == 0) {
	  pri.unselectAll();
	  him.output(Packet.SPYourPlay(Packet.YPLAY_BID, false, 6));
	  return;
	}
	RegisterBid(seat, Packet.PLAY_CONN_UP);
	return;
      case Packet.PLAY_CONN_MD:
	if (nextTurn(turn) != seat) {
	  him.output(Packet.SPYourPlay(Packet.YPLAY_BID, false, 5));
	  return;
	}
	if (pri.searchN(Tile.stepDn(active), 1) == 0 ||
	    pri.searchN(Tile.stepUp(active), 1) == 0) {
	  pri.unselectAll();
	  him.output(Packet.SPYourPlay(Packet.YPLAY_BID, false, 6));
	  return;
	}
	RegisterBid(seat, Packet.PLAY_CONN_MD);
	return;
      case Packet.PLAY_CONN_DN:
	if (nextTurn(turn) != seat) {
	  him.output(Packet.SPYourPlay(Packet.YPLAY_BID, false, 5));
	  return;
	}
	if (pri.searchN(Tile.stepDn(active), 1) == 0 ||
	    pri.searchN(Tile.stepDn(Tile.stepDn(active)), 1) == 0) {
	  him.output(Packet.SPYourPlay(Packet.YPLAY_BID, false, 6));
	  return;
	}
	RegisterBid(seat, Packet.PLAY_CONN_DN);
	return;
      case Packet.PLAY_TRIP:
	if (pri.searchN(active, 2) != 2) {
	  pri.unselectAll();
	  him.output(Packet.SPYourPlay(Packet.YPLAY_BID, false, 7));
	  return;
	}
	RegisterBid(seat, Packet.PLAY_TRIP);
	return;
      case Packet.PLAY_QUAD:
	if (pri.searchN(active, 3) != 3) {
	  pri.unselectAll();
	  him.output(Packet.SPYourPlay(Packet.YPLAY_BID, false, 5));
	  return;
	}
	RegisterBid(seat, Packet.PLAY_QUAD);
	return;
      case Packet.PLAY_WIN:
	RegisterBid(seat, Packet.PLAY_WIN);
	return;
      }
    }
  }

  void RegisterDiscard (byte tile) {
    int s = 0;
    
    active = tile;
    last_active = tile;
    last_seat = turn;
    on_flower = false;

    for (s = 0; s < 4; s++) {
      if (s == turn) {
	actions[s] = Packet.PLAY_PASS;
	action_masks[s] = 0;
	continue;
      }
      actions[s] = -1;
      Hand h = pri_hands[s];
      if (s == nextTurn(turn)) {
	action_masks[s] = 1 << (int)Packet.PLAY_DRAW;
	boolean up1, up2, dn1, dn2;
	up1 = (h.searchN(Tile.stepUp(tile), 1) != 0);
	up2 = (h.searchN(Tile.stepUp(Tile.stepUp(tile)), 1) != 0);
	dn1 = (h.searchN(Tile.stepDn(tile), 1) != 0);
	dn2 = (h.searchN(Tile.stepDn(Tile.stepDn(tile)), 1) != 0);
	if (up1 && up2) {
	  action_masks[s] |= 1 << (int)Packet.PLAY_CONN_UP;
	}
	if (up1 && dn1) {
	  action_masks[s] |= 1 << (int)Packet.PLAY_CONN_MD;
	}
	if (dn1 && dn2) {
	  action_masks[s] |= 1 << (int)Packet.PLAY_CONN_DN;
	}
      } else {
	action_masks[s] = 1 << (int)Packet.PLAY_PASS;
      }
      int n_id = h.searchN(tile, 3);
      if (n_id >= 2) {
	action_masks[s] |= 1 << (int)Packet.PLAY_TRIP;
	if (n_id == 3) {
	  action_masks[s] |= 1 << (int)Packet.PLAY_QUAD;
	}
      }
      h.unselectAll();
      if ((mj_stats[s] = CheckMahJong(s, tile)) != null) {
	action_masks[s] |= 1 << (int)Packet.PLAY_WIN;
      }
    }
    status = AUCTION;
    outputHands(players[turn], turn, false);
    outputTable(Packet.SPDiscard(tile, Packet.DISCARD_DISCARD));
    outputTable(Packet.SPWhoPlayed(actions));
    for (s = 0; s < 4; s++) {
      if (players[s] != null) {
	players[s].output(Packet.SPYourChoices(action_masks[s],
					       cur_scores[s]));
      }
    }
  }

  void RegisterFourth (byte tile) {
    int s = 0;
    
    active = tile;
    last_active = tile;
    last_seat = turn;
    on_flower = false;

    status = FOURTH;
    outputHands(players[turn], turn, false);
    outputTable(Packet.SPDiscard(tile, Packet.DISCARD_FOURTH));
    outputTable(Packet.SPWhoPlayed(actions));
    for (s = 0; s < 4; s++) {
      if (s == turn) {
	actions[s] = Packet.PLAY_PASS;
	action_masks[s] = 0;
	continue;
      }
      actions[s] = -1;
      action_masks[s] = 1 << (int)Packet.PLAY_PASS;
      if ((mj_stats[s] = CheckMahJong(s, tile)) != null) {
	action_masks[s] |= 1 << (int)Packet.PLAY_WIN;
      }
    }
    for (s = 0; s < 4; s++) {
      if (players[s] != null) {
	players[s].output(Packet.SPYourChoices(action_masks[s],
					       cur_scores[s]));
      }
    }
  }    

  void RegisterDiscardMode () {
    int i = 0;
    status = DISCARD;
    actions[turn] = -1;
    action_masks[turn] = 1 << (int)Packet.PLAY_DISCARD;
  check_four:
    {
      for (i = 0; i < pri_hands[turn].length(); i++) {
	byte v = pri_hands[turn].tileAt(i);
	pri_hands[turn].unselectAll();
	if (pri_hands[turn].searchN(v, 4) == 4) {
	  action_masks[turn] |= 1 << (int)Packet.PLAY_FOUR;
	  break check_four;
	}
	pub_hands[turn].unselectAll();
	if (pub_hands[turn].searchN(v, 3) == 3 &&
	    pub_hands[turn].isGroup()) {
	  action_masks[turn] |= 1 << (int)Packet.PLAY_FOUR;
	  break check_four;
	}
      }
    }
    pri_hands[turn].unselectAll();
    pub_hands[turn].unselectAll();
    if ((mj_stats[turn] = CheckMahJong(turn)) != null) {
      action_masks[turn] |= 1 << (int)Packet.PLAY_WIN;
    }
    players[turn].output(Packet.SPYourChoices(action_masks[turn],
					      cur_scores[turn]));
  }
	  
  void RegisterWin (int seat) {
    int score = mj_stats[seat].score;
    int bonus = mj_stats[seat].bonus;

    pri_hands[seat].setLength(0);
    semi_pubs[seat].setLength(0);
    pub_hands[seat] = mj_stats[seat].hand;
    outputHands(players[seat], seat, true);
    if (mj_stats[seat].mjc.self_feed) {
      for (int s = 0; s < 4; s++) {
	if ((s != seat) && (players[s] != null)) {
	  players[s].stat.subBalance(score);
	}
      }
      players[seat].stat.addBalance(3 * score);
    } else {
      for (int s = 0; s < 4; s++) {
	if (s == last_seat && players[s] != null) {
	  players[s].stat.subBalance(score);
	} else if (s != seat && players[s] != null) {
	  players[s].stat.subBalance(bonus);
	}
      }
      players[seat].stat.addBalance(score + 2 * bonus);
    }
    for (int s = 0; s < 4; s++) {
      if (players[s] != null) {
	Player.outputAll(Packet.SPStat(players[s].id, 
				    players[s].stat.getBalance()));
      }
    }
    action_masks[seat] = 0;
    outputTable(Packet.SPMessage(Packet.MSG_GOD, 0, "Summary of " +
			      players[seat].name + "'s Hand: \n" +
			      mj_stats[seat].summary));
    outputTable(Packet.SPWin(players[seat].id, seat, score, last_seat));
  }
  
  void RegisterBid (int seat, byte action) {
    Hand played = null;
    int high_action = 0, near_dist = 4;
    byte drawn = 0;
    boolean grab = false;
    Player him = null;

    actions[seat] = action;
    outputTable(Packet.SPWhoPlayed(actions));

    if (!AllPlayed())
      return;

    for (int s = 0; s < 4; s++) {
      if (actions[s] == Packet.PLAY_WIN) {
	high_action = Packet.PLAY_WIN;
	if (opt.multiple_wins) {
	  RegisterWin(s);
	} else {
	  int dist = (4 - s + dealer) % 4;
	  if (dist < near_dist) {
	    near_dist = dist;
	    him = players[s];
	  }
	}
      } else {
	if (actions[s] > high_action) {
	  him = players[s];
	  high_action = actions[s];
	}
      }
    }
    if (high_action == Packet.PLAY_WIN && opt.multiple_wins) {
      EndGame();
      return;
    }
    if (high_action == Packet.PLAY_WIN) {
      RegisterWin(him.seat);
      EndGame();
      return;
    }
    if (status == FOURTH) {
      last_active = 0;
      last_seat = -1;
      him = players[turn];
      Hand h = pub_hands[turn];
      h.addTile(active);
      h.select(h.length() - 1);
      h.group();
      h.unselectAll();
      drawn = stockpile.DrawTail(turn);
      outputHands(him, turn, false);
      him.output(Packet.SPYourDraw(drawn));
      outputTable(Packet.SPTurn(Packet.TURN_FOURTH, turn));
      RegisterDiscardMode();
      for (int s = 0; s < 4; s++) {
	if (s != turn) {
	  actions[s] = -1;
	  action_masks[s] = 0;
	  if (players[s] != null) {
	    players[s].output(Packet.SPYourChoices(0, -1));
	  }
	}
      }
      return;
    }
    switch (high_action) {
    case Packet.PLAY_DRAW:
      out_tiles[turn].addTile(active);
      last_active = 0;
      last_seat = -1;
      drawn = stockpile.DrawHead(him.seat);
      if (drawn == 0) {
	outputTable(Packet.SPWin(-1, 0, 0, 0));
	EndGame();
	return;
      }
      turn = him.seat;
      him.output(Packet.SPYourDraw(drawn));
      outputHands(him, turn, false);
      outputTable(Packet.SPTurn(Packet.TURN_DRAW, turn));
      break;
    case Packet.PLAY_CONN_UP:
    case Packet.PLAY_CONN_MD:
    case Packet.PLAY_CONN_DN:
    case Packet.PLAY_TRIP:
      grab = true;
      played = pri_hands[him.seat].selection(true);
      played.addTile(active);
      played.selectAll();
      played.sort();
      played.group();
      played.unselectAll();
      pub_hands[him.seat].append(played);
      pri_hands[him.seat].delete();
      turn = him.seat;
      outputHands(him, turn, false);
      outputTable(Packet.SPTurn(Packet.TURN_GRAB, turn));
      break;
    case Packet.PLAY_QUAD:
      grab = true;
      played = pri_hands[him.seat].selection(true);
      played.addTile(active);
      played.selectAll();
      played.group();
      played.unselectAll();
      pub_hands[him.seat].append(played);
      pri_hands[him.seat].delete();
      turn = him.seat;
      drawn = stockpile.DrawTail(turn);
      outputHands(him, turn, false);
      him.output(Packet.SPYourDraw(drawn));
      outputTable(Packet.SPTurn(Packet.TURN_GRAB, turn));
      break;
    }
    RegisterDiscardMode();
    if (high_action != Packet.PLAY_DRAW) {
      n_grabs[turn]++;
    }
    for (int s = 0; s < 4; s++) {
      if (s != turn) {
	action_masks[s] = 0;
	actions[s] = -1;
	players[s].output(Packet.SPYourChoices(0, -1));
      }
    }
  }

  public synchronized void Invite (Player inviter, int invited, boolean add) {
    int i = 0;
    Player him = null;
    if (add) {
      if (invited < 0 || invited >= Player.global.length ||
	  (him = Player.global[invited]) == null || him.stat == null) {
	inviter.output(Packet.SPMessage(Packet.MSG_GOD, 0, "Player " + 
				     invited + " does not exist."));
	return;
      }
    find_slot:
      {
	for (i = 0; i < invitations.length; i++) {
	  if (invitations[i] == -1) {
	    invitations[i] = invited;
	    break find_slot;
	  }
	}
	inviter.output(Packet.SPMessage(Packet.MSG_GOD, 0, 
				     "You cannot invite any more players."));
	return;
      }
      him.output(Packet.SPMessage(Packet.MSG_GOD, 0,
			       "You are invited to join table <" + id + ">."));
      return;
    } else {
      for (i = 0; i < invitations.length; i++) {
	if (invitations[i] == invited) {
	  invitations[i] = -1;
	}
      }
    }
  }

  public synchronized String GetInvited () {
    StringBuffer buf = new StringBuffer(20);
    int i = 0;
    for (i = 0; i < invitations.length; i++) {
      if (invitations[i] != -1) {
	buf.append("  " + invitations[i]);
      }
    }
    return buf.toString();
  }
	      
  public synchronized void Join (Player p, byte mode) {
    int i = 0;
    int seat = -1;
    if (mode == Packet.JOIN_PLAY) {  // join as a player
    check_invitation:
      if (invite_only) {  // check for invitation 
	for (i = 0; i < invitations.length; i++) {
	  if (invitations[i] == p.id)
	    break check_invitation;
	}
	p.output(Packet.SPMessage(Packet.MSG_GOD, 0, "This table is invitation-only.  Ask to be invited."));
	return;
      }
      if (players[0] == null) {
	seat = 0;
      } else {
	for (i = 1; i < 4; i++) {
	  if (players[i] == null) {
	    if (seat == -1) {
	      seat = i;
	    } else if ((rand.nextInt() & 1) == 0) {
	      seat = i;
	    }
	  }
	}
      }
      if (seat == -1) {
	p.output(Packet.SPMessage(Packet.MSG_GOD, 0, "Table is full."));
	return;
      }
      p.table = this;
      p.seat = seat;
      players[seat] = p;
      Player.outputAll(Packet.SPTablePlayer(id, seat, p.id));
      if (status != IDLE) {
	p.output(Packet.SPGame(id, round, dealer, Packet.GAME_PROGRESS));
	pri_hands[seat].unselectAll();
	for (i = 0; i < 4; i++) {
	  if (players[i] != null) {
	    if (i == seat) {
	      p.output(Packet.SPTiles(p.id, i, pri_hands[i].length(),
				   pub_hands[i], semi_pubs[i], true));
	      p.output(Packet.SPYourPrivate(pri_hands[seat]));
	    } else {
	      p.output(Packet.SPTiles(players[i].id, i, pri_hands[i].length(),
				   pub_hands[i], semi_pubs[i], false));
	    }
	    p.output(Packet.SPOutTiles(players[i].id, i, out_tiles[i]));
	  }
	}
	outputHands(p, seat, true);
	if (status == DISCARD) {
	  p.output(Packet.SPTurn(Packet.TURN_NONE, turn));
	  actions[seat] = -1;
	} 
	if (status == AUCTION) {
	  p.output(Packet.SPTurn(Packet.TURN_NONE, turn));
	  p.output(Packet.SPDiscard(active, Packet.DISCARD_DISCARD));
	  if (seat != turn) {
	    actions[seat] = -1;
	    outputTable(Packet.SPWhoPlayed(actions));
	  } else {
	    RegisterBid(seat, Packet.PLAY_PASS);
	  }
	}
	if (status == FOURTH) {
	  p.output(Packet.SPTurn(Packet.TURN_NONE, turn));
	  p.output(Packet.SPDiscard(active, Packet.DISCARD_FOURTH));
	  if (seat != turn) {
	    actions[seat] = -1;
	    outputTable(Packet.SPWhoPlayed(actions));
	  } else {
	    RegisterBid(seat, Packet.PLAY_PASS);
	  }
	}
	p.output(Packet.SPYourChoices(action_masks[seat], cur_scores[seat]));
	if (AllSeated()) {
	  outputTable(Packet.SPMessage(Packet.MSG_GOD, 1,
				    "Game resumed."));
	}
      } else {  // status is IDLE
	actions[seat] = -1;
	outputTable(Packet.SPWhoPlayed(actions));
	if (AllSeated()) {
	  outputTable(Packet.SPMessage(Packet.MSG_GOD, 1,
				    "Table is full.  Select \"New Game\" now."));
	}
      }
      return;
    } else {  // joining to spectate
      for (i = 4; i < players.length; i++) {
	if (players[i] == null) {
	  p.table = this;
	  p.seat = i;
	  players[i] = p;
	  Player.outputAll(Packet.SPTablePlayer(id, i, p.id));
	  if (status != IDLE) {
	    p.output(Packet.SPGame(id, round, dealer, Packet.GAME_PROGRESS));
	    int j = 0;
	    for (j = 0; j < 4; j++) {
	      if (players[j] != null) {
		p.output(Packet.SPTiles(players[j].id, j, pri_hands[j].length(),
				     pub_hands[j], semi_pubs[j], false));
		p.output(Packet.SPOutTiles(players[j].id, j, out_tiles[j]));
	      }
	    }
	    if (status == DISCARD) {
	      p.output(Packet.SPTurn(Packet.TURN_NONE, turn));
	    } 
	    if (status == AUCTION) {
	      p.output(Packet.SPTurn(Packet.TURN_NONE, turn));
	      p.output(Packet.SPDiscard(active, Packet.DISCARD_DISCARD));
	    }
	    if (status == FOURTH) {
	      p.output(Packet.SPTurn(Packet.TURN_NONE, turn));
	      p.output(Packet.SPDiscard(active, Packet.DISCARD_FOURTH));
	    }
	  }
	  return;
	}
      }
      p.output(Packet.SPMessage(Packet.MSG_GOD, 0, "No more spectator seats."));
      return;
    }
  }

  public void Leave (Player p) {
    if (p.table != this) return;
    synchronized (global) {
      synchronized (this) {
	Player.outputAll(Packet.SPTablePlayer(id, p.seat, Packet.TABLE_LEAVE));
	if (p.seat >= 0 && p.seat < 4 && AllSeated() && status != IDLE) {
	  Player.outputAll(Packet.SPMessage(Packet.MSG_GOD, 0,
					    "Game stalled on table <" + id +
					    ">, please join that table."));
	  // penalty: 5 points for 'escaping'
	  p.stat.subBalance(5);
	  Player.outputAll(Packet.SPStat(p.id, p.stat.getBalance()));
	}
	players[p.seat] = null;
	p.table = null;
	p.seat = -1;
	for (int i = 0; i < 4; i++) {
	  if (players[i] != null)
	    return;
	}
	// all 4 players gone, remove the table
	for (int i = 4; i < players.length; i++) {
	  if (players[i] != null) {
	    players[i].table = null;
	    players[i].seat = -1;
	    players[i] = null;
	    Player.outputAll(Packet.SPTablePlayer(id, i, Packet.TABLE_LEAVE));
	  }
	}
	global[id] = null;
      }
    }
  }

  public synchronized void Update (Player p) {
    for (int i = 0; i < players.length; i++) {
      if (players[i] != null) {
	p.output(Packet.SPTablePlayer(id, i, players[i].id));
      }
    }
  }

  void NewGame () {
    int s = 0;
    
    stockpile.shuffle();

    if (dealer == -1) {
      turn = dealer = Math.abs(rand.nextInt()) % 4;
      first_dealer = dealer;
    } else {
      turn = dealer;
    }

    // Distribute tiles to players.
    for (s = 0; s < 4; s++) {
      Player p = players[s];
      pri_hands[s].setLength(0);
      pub_hands[s].setLength(0);
      semi_pubs[s].setLength(0);
      out_tiles[s].setLength(0);
      n_grabs[s] = 0;
      int n = (turn == s) ? 14 : 13;
      while (pri_hands[s].length() < n)
	stockpile.DrawHead(s);
    }
    last_active = 0;
    last_seat = -1;

    outputTable(Packet.SPGame(id, round, dealer, Packet.GAME_START));
    for (s = 0; s < 4; s++) {
      outputHands(players[s], s, true);
    }
    outputTable(Packet.SPTurn(Packet.TURN_DRAW, turn));
    RegisterDiscardMode();
    for (s = 0; s < 4; s++) {
      if (s != turn) {
	action_masks[s] = 0;
	actions[s] = -1;
	if (players[s] != null) {
	  players[s].output(Packet.SPYourChoices(0, -1));
	}
      }
    }
  }

  void EndGame () {
    status = IDLE;
    for (int s = 0; s < 4; s++) {
      if (actions[s] == Packet.PLAY_WIN) {
	if (actions[dealer] != Packet.PLAY_WIN) {
	  dealer = nextTurn(dealer);
	  if (dealer == first_dealer) {
	    round = (round + 1) % 4;
	  }
	  break;
	}
      }
    }
    for (int s = 0; s < 4; s++) {
      action_masks[s] = 1 << Packet.PLAY_PASS;
      actions[s] = -1;
      if (players[s] != null) {
	PlayerRecord.updateStat(players[s].stat);
      }
    }
    outputTable(Packet.SPGame(id, 0, 0, Packet.GAME_END));
  }

  MJStat MahJongTrial (Hand cold, Hand hot, boolean has_pair, MJContext mjc) {
    int cold_len = cold.length();
    Hand sel = hot.selection(true);
    Hand unsel = hot.unselected(false);
    sel.sort();
    sel.group();
    sel.unselectAll();
    cold.append(sel);
    MJStat mjs = RegularMahJong(cold, unsel, has_pair || sel.length() == 2,
				mjc);
    cold.setLength(cold_len);
    return mjs;
  }
  
  MJStat RegularMahJong (Hand cold, Hand hot, boolean has_pair, MJContext mjc) {
    MJStat mjs = null, high_mjs = null;
    int cold_len = cold.length();
    if (hot.length() == 0) {
      return RegularScore(cold, mjc);
    }
    byte v = hot.tileAt(0);
    hot.unselectAll();
    if (!has_pair && hot.searchN(v, 2) == 2) {
      if ((mjs = MahJongTrial(cold, hot, has_pair, mjc)) != null) {
	if (high_mjs == null || mjs.score > high_mjs.score)
	  high_mjs = mjs;
      }
    }
    hot.unselectAll();
    if (hot.searchN(v, 3) == 3) {
      if ((mjs = MahJongTrial(cold, hot, has_pair, mjc)) != null) {
	if (high_mjs == null || mjs.score > high_mjs.score)
	  high_mjs = mjs;
      }
    }
    hot.unselectAll();
    if (hot.searchN(Tile.stepUp(v), 1) == 1 &&
	hot.searchN(Tile.stepUp(Tile.stepUp(v)), 1) == 1) {
      hot.select(0);
      if ((mjs = MahJongTrial(cold, hot, has_pair, mjc)) != null) {
	if (high_mjs == null || mjs.score > high_mjs.score)
	  high_mjs = mjs;
      }
    }
    hot.unselectAll();
    if (hot.searchN(Tile.stepDn(v), 1) == 1 &&
	hot.searchN(Tile.stepDn(Tile.stepDn(v)), 1) == 1) {
      hot.select(0);
      if ((mjs = MahJongTrial(cold, hot, has_pair, mjc)) != null) {
	if (high_mjs == null || mjs.score > high_mjs.score)
	  high_mjs = mjs;
      }
    }
    hot.unselectAll();
    if (hot.searchN(Tile.stepUp(v), 1) == 1 &&
	hot.searchN(Tile.stepDn(v), 1) == 1) {
      hot.select(0);
      if ((mjs = MahJongTrial(cold, hot, has_pair, mjc)) != null) {
	if (high_mjs == null || mjs.score > high_mjs.score)
	  high_mjs = mjs;
      }
    }
    return high_mjs;
  }
    
  MJStat RegularScore (Hand mj_hand, MJContext mjc) {
    MJStat mjs = new MJStat();
    mjs.mjc = mjc;
    mjs.hand = mj_hand.Clone();
    mjs.hand.unselectAll();
    HandStat hs = new HandStat(mj_hand);
    int c = 0;
    if (hs.isAllConn()) {
      mjs.fan += 1;
      mjs.summary.append(     ">   All Connections:   1\n");
    }
    if (hs.isSisters()) {
      mjs.fan += 1;
      mjs.summary.append(     ">         3 Sisters:   1\n");
    }
    if (hs.isAllOneNine()) {
      mjs.fan += 2;
      mjs.summary.append(     ">     All 1's & 9's:   2\n");
    }
    if (hs.isOneSuit()) {
      if (hs.isPureSuit()) {
	mjs.fan += 6;
	mjs.summary.append(   ">     One Pure Suit:   6\n");
      } else {
	mjs.fan += 3;
	mjs.summary.append(   ">    One Mixed Suit:   3\n");
      }
    }
    if (hs.isAllTrip()) {
      if (hs.isPureWord()) {
	mjs.fan += opt.max_fan;
	mjs.summary.append(   ">        Pure Words:  " + opt.max_fan + "\n");
      } else {
	mjs.fan += 3;
	mjs.summary.append(   ">      All Triplets:   3\n");
      }
    }
    if (hs.isSnake()) {
      if (hs.isConcealedSnake()) {
	mjs.fan += 3;
	mjs.summary.append(   ">   Concealed Snake:   3\n");
      } else {
	mjs.fan += 2;
	mjs.summary.append(   ">    Revealed Snake:   2\n");
      }
    }
    if (mjc.no_grabs) {
      mjs.fan += 1;
      mjs.summary.append(     ">  Front Door Clean:   1\n");
    }
    if (mjc.all_grabs) {
      mjs.fan += 1;
      mjs.summary.append(     "> All Tiles Grabbed:   1\n");
    }
    if (mjc.grab_fourth) {
      mjs.fan += 1;
      mjs.summary.append(     ">Robbing Quadruplet:   1\n");
    }
    c = hs.countWords(mjc.round_wind, mjc.seat_wind);
    if (c > 0) {
      mjs.fan += c;
      mjs.summary.append(     ">     Word Triplets:   " + c + "\n");
    }
    c = hs.n_pub_quads + hs.n_pri_quads;
    if (c > 0) {
      mjs.fan += c;
      mjs.summary.append(     ">       Quadruplets:   " + c + "\n");
    }
    if (mjc.win_on_flower) {
      mjs.fan += 1;
      mjs.summary.append(     ">     Win On Flower:   1\n");
    }
    if (mjs.fan > opt.max_fan)
      mjs.fan = opt.max_fan;
    mjs.summary.append(">> Total Fan: " + mjs.fan + ".\n");
    if (opt.count_flowers && hs.n_flowers > 0) {
      mjs.bonus += hs.n_flowers;
      mjs.summary.append(     ">           Flowers:   " + hs.n_flowers + "\n");
    }
    if (hs.n_pri_quads > 0) {
      mjs.bonus += 2 * hs.n_pri_quads;
      mjs.summary.append(     ">   Concealed Quads:   " + hs.n_pri_quads + "\n");
    }
    if (mjc.is_dealer) {
      mjs.bonus += DEALER_BONUS;
      mjs.summary.append(     ">      Dealer Bonus:   " + DEALER_BONUS + "\n");
    }
    mjs.summary.append(">> Total Bonus: " + mjs.bonus + ".\n");
    if (mjs.fan > 4) {
      mjs.score = mjs.fan * mjs.fan + mjs.bonus;
    } else {
      mjs.score = (1 << mjs.fan) + mjs.bonus;
    }
    return mjs;
  }

  MJStat SevenPairs (Hand cold, Hand hot, MJContext mjc) {
    if (hot.length() != 14) {
      return null;
    }
    int cold_len = cold.length();
    int i = 0, score = 0;
    int flavors = 0;
    hot = hot.Clone();
    hot.unselectAll();
    for (i = 0; i < 7; i++) {
      byte v = hot.tileAt(0);
      if (hot.searchN(v, 2) != 2) {
	cold.setLength(cold_len);
	return null;
      }
      hot.group();
      cold.append(hot.selection(false));
      hot.delete();
      flavors |= (1 << (int)Tile.getFlavor(v));
    }
    MJStat mjs = new MJStat();
    mjs.mjc = mjc;
    mjs.hand = cold.Clone();
    cold.setLength(cold_len);
    if ((flavors & (flavors - 1)) == 0) {
      mjs.fan = opt.max_fan;
      mjs.summary.append(     ">   Pure Seven Pairs: " + opt.max_fan + "\n");
    } else {
      mjs.fan = 5;
      mjs.summary.append(     ">        Seven Pairs:  5\n");
    }
    if (mjc.win_on_flower) {
      mjs.fan += 1;
      mjs.summary.append(     ">      Win On Flower:  1\n");
    }
    if (mjs.fan > opt.max_fan)
      mjs.fan = opt.max_fan;
    mjs.summary.append(">> Total Fan: " + mjs.fan + ".\n");
    if (opt.count_flowers && cold_len > 0) {
      mjs.bonus += cold_len;
      mjs.summary.append(     ">           Flowers:   " + cold_len + "\n");
    }
    if (mjc.is_dealer) {
      mjs.bonus += DEALER_BONUS;
      mjs.summary.append(     ">      Dealer Bonus:   " + DEALER_BONUS + "\n");
    }
    mjs.summary.append(">> Total Bonus: " + mjs.bonus + ".\n");
    if (mjs.fan > 4) {
      mjs.score = mjs.fan * mjs.fan + mjs.bonus;
    } else {
      mjs.score = (1 << mjs.fan) + mjs.bonus;
    }
    return mjs;
  }
  
  MJStat ThirteenIndiv (Hand cold, Hand hot, MJContext mjc) {
    long patterns[] = { 
      1L * 146 + 512 * 292 + 262144 * 584,
      1L * 146 + 512 * 584 + 262144 * 292,
      1L * 292 + 512 * 146 + 262144 * 584,
      1L * 292 + 512 * 584 + 262144 * 146,
      1L * 584 + 512 * 146 + 262144 * 292,
      1L * 584 + 512 * 292 + 262144 * 146,
    };
    long bits = 0;
    int i = 0;
    boolean pair = false;

    if (hot.length() != 14) {
      return null;
    }
    for (i = 0; i < 14; i++) {
      byte v = hot.tileAt(i);
      if ((bits & (1L << (int)v)) != 0) {
	if (pair)
	  return null;
	if (v < 28)
	  return null;
	pair = true;
      }
      bits |= (1L << (int)v);
    }
    if (!pair)
      return null;
    bits &= ((1L << 28) - 1);
  loop: 
    {
      for (i = 0; i < patterns.length; i++) {
	if (bits == patterns[i])
	  break loop;
      }
      return null;
    }
    MJStat mjs = new MJStat();
    mjs.mjc = mjc;
    mjs.hand = cold.Clone().append(hot);
    mjs.hand.sort();
    mjs.fan = 6;
    mjs.summary.append(       ">    13 Individuals:   6\n");
    if (mjc.win_on_flower) {
      mjs.fan += 1;
      mjs.summary.append(     ">     Win On Flower:   1\n");
    }
    mjs.summary.append(">> Total Fan: " + mjs.fan + ".\n");
    if (opt.count_flowers && cold.length() > 0) {
      mjs.bonus += cold.length();
      mjs.summary.append(     ">           Flowers:   " + cold.length() + "\n");
    }
    if (mjc.is_dealer) {
      mjs.bonus += DEALER_BONUS;
      mjs.summary.append(     ">      Dealer Bonus:   " + DEALER_BONUS + "\n");
    }
    mjs.summary.append(">> Total Bonus: " + mjs.bonus + ".\n");
    if (mjs.fan > 4) {
      mjs.score = mjs.fan * mjs.fan + mjs.bonus;
    } else {
      mjs.score = (1 << mjs.fan) + mjs.bonus;
    }
    return mjs;
  }

  MJStat Thirteen19 (Hand cold, Hand hot, MJContext mjc) {
    long pattern = (1L << 1) | (1L << 9) | (1L << 10) | 
      (1L << 18) | (1L << 19) | (1L << 27) | (1L << 28) | (1L << 29) |
	(1L << 30) | (1L << 31) | (1L << 32) | (1L << 33) | (1L << 34);
    long bits = 0;
    int i = 0;
    boolean pair = false;

    if (hot.length() != 14) {
      return null;
    }
    for (i = 0; i < 14; i++) {
      byte v = hot.tileAt(i);
      if ((bits & (1L << (int)v)) != 0) {
	if (pair)
	  return null;
	pair = true;
      }
      bits |= (1L << (int)v);
    }
    if (bits != pattern)
      return null;
    MJStat mjs = new MJStat();
    mjs.mjc = mjc;
    mjs.hand = cold.Clone().append(hot);
    mjs.hand.sort();
    mjs.fan = opt.max_fan;
    mjs.summary.append(       ">        13 Orphans:  " + mjs.fan + "\n");
    mjs.summary.append(">> Total Fan: " + mjs.fan + ".\n");
    if (opt.count_flowers && cold.length() > 0) {
      mjs.bonus += cold.length();
      mjs.summary.append(     ">           Flowers:   " + cold.length() + "\n");
    }
    if (mjc.is_dealer) {
      mjs.bonus += DEALER_BONUS;
      mjs.summary.append(     ">      Dealer Bonus:   " + DEALER_BONUS + "\n");
    }
    mjs.summary.append(">> Total Bonus: " + mjs.bonus + ".\n");
    mjs.score = mjs.fan * mjs.fan + mjs.bonus;
    return mjs;
  }    
    
  
  MJStat CheckMahJong (int seat, byte tile) {
    int pri_hand_len = pri_hands[seat].length();
    pri_hands[seat].addTile(tile);
    MJStat mjs = CheckMahJong(seat);
    pri_hands[seat].setLength(pri_hand_len);
    return mjs;
  }

  MJStat CheckMahJong (int seat) {
    MJStat high_mjs = null, mjs = null;
    MJContext mjc = new MJContext();
    int pub_hand_len = pub_hands[seat].length();

    if (last_active != 0 && last_seat != seat) {
      mjc.feeder = last_seat;
    } else {
      mjc.self_feed = true;
    }
    if (seat == dealer) {
      mjc.is_dealer = true;
    }
    mjc.win_on_flower = on_flower;
    if (status == FOURTH) {
      mjc.grab_fourth = true;
    }
    if (n_grabs[seat] == 0) {
      mjc.no_grabs = true;
    }
    if (n_grabs[seat] == 4 && !mjc.self_feed) {
      mjc.all_grabs = true;
    }
    mjc.round_wind = roundWind();
    mjc.seat_wind = seatWind(seat);

    pub_hands[seat].selectAll();
    pub_hands[seat].append(semi_pubs[seat]);
    cur_scores[seat] = -1;
    high_mjs = RegularMahJong(pub_hands[seat], pri_hands[seat], false, mjc);
    pub_hands[seat].unselectAll();

    if ((mjs = SevenPairs(pub_hands[seat], pri_hands[seat], mjc)) != null) {
      if (high_mjs == null || mjs.score > high_mjs.score)
	high_mjs = mjs;
    }
    if ((mjs = ThirteenIndiv(pub_hands[seat], pri_hands[seat], mjc)) != null) {
      if (high_mjs == null || mjs.score > high_mjs.score)
	high_mjs = mjs;
    }
    if ((mjs = Thirteen19(pub_hands[seat], pri_hands[seat], mjc)) != null) {
      if (high_mjs == null || mjs.score > high_mjs.score)
	high_mjs = mjs;
    }
    if (high_mjs != null) {
      cur_scores[seat] = high_mjs.score;
    }
    pub_hands[seat].setLength(pub_hand_len);
//    System.out.println("CheckMahJong: table = " + id + ", seat = " +
//		       seat + ", result: " + (high_mjs != null));
    if (high_mjs != null && high_mjs.fan < opt.min_fan)
      return null;
    else
      return high_mjs;
  }
}
