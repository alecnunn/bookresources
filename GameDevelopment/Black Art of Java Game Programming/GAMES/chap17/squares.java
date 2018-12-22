//squares, an implementation of a game that I like to play on my portable
//Merlin game.
//					--Kyle Palmer

/* $Id: squares.java,v 1.6 1996/05/04 01:41:26 kyle Exp $ */


import java.awt.*;
import java.util.*;
import java.lang.*;

public class squares extends java.applet.Applet {
  int squares;  //the number of squares to a side
  int length;   //the length of one side
  int colors;   //the current number of colors working with
  game_board main_board;  //the main board to play with
  game_board save_board;  //a board to save when solving
  Panel buttons;  //the panel for the buttons
  TextArea solve_area;  //a text area to put the solve status on
  Panel main_panel;  //a main panel to put buttons and textarea on
  solver solve_thread;

public void init(){  //initialize this applet

  squares = 3;     //start with 3 squares to a side
  length = 200;    //start with 100 pixels to a side
  colors = 2;      //start with two colors
  main_board = new game_board(squares,length,colors); //create the board
  main_board.randomize();  //randomize the board



  //set up the panels, is simple, so not given own class

  setLayout(new BorderLayout()); //set the layout 

  main_panel = new Panel();  //get the main panel

  main_panel.setLayout(new BorderLayout()); //want a border layout on main

  buttons = new Panel();  //create the panel
  buttons.setLayout(new GridLayout(3,2));  //arrange buttons in a grid
  buttons.add(new Button("More Squares")); //add the buttons
  buttons.add(new Button("Less Squares"));
  buttons.add(new Button("More Colors"));
  buttons.add(new Button("Less Colors"));
  buttons.add(new Button("Solve"));
  buttons.add(new Button("Restore"));

  solve_area = new TextArea(8,25);  //create the window for solve text
  solve_area.setEditable(false); //don't want to edit this

  main_panel.add("North",buttons); //add the button panel to top of main panel
  main_panel.add("South",solve_area); //add the panel to the main panel

  add("South",main_panel);  //add buttons and window to screen
}



public void paint (Graphics g){ //just draw the board to repaint
  main_board.draw_board(0,0,g); //draw the board at 0,0
}

public boolean mouseDown (Event ent,int x,int y){ //handle mouse click
  main_board.apply_move_p(x,y); //apply the move  
  repaint();  //redraw the board
  return true;
}


public boolean action (Event evt, Object arg){ //process buttons
  String label = (String)arg;  //converts the button to a string
  

  if (label.equals("Solve")) { //if request to start solving the board
    if (solve_thread != null){ //potentially running solve
      solve_thread.stop();  //stop the thread
      solve_thread = null;  //allow garbage collecting
    }
    save_board = new game_board(main_board);  //save the board

    solve_thread = new solver(main_board,solve_area); //create the solver
    solve_thread.setPriority(Thread.MIN_PRIORITY); //lower priority
    solve_thread.start();                   //start solving in background
    return true;
  }
  if (label.equals("Restore")){ //restore the board to when started solve
    if (save_board == null) //don't do anything if haven't saved a board before
      return true;

    main_board = new game_board(save_board); //otherwise create a new board
    repaint();
    return true;    //return here so we don't randomize
      
  }

  if (label.equals("More Colors")){ //if request for more colors
    colors ++;
  }
  else if (label.equals("Less Colors")){
    colors --;
  }
  else if (label.equals("More Squares")){
    squares += 2;
  }
  else if (label.equals("Less Squares")){
    squares -= 2;
  }

  if (squares > 35)  //max 35x35 squares
    squares = 35;
  if (squares < 3)
    squares = 3;
  if (colors > 5)  //maximum 5 colors
    colors = 5;
  if (colors <2)  //at least two colors
    colors = 2;
  
  main_board = new game_board(squares,length,colors); //create new board
  main_board.randomize();  //randomize the new game board

  repaint();  //copy new board to screen
  return true;
}


}




class game_board {  //the board class to hold and manipulate a board
  int board[][];   //the 2-D array to hold the board itself 
  int side_squares; //to store the number of squares to a side
  int max_color; //store the maximum color of a square
  int side_length;  //the length of one side


  game_board(int squares,int side_pixels,int colors){ //constructor
    board = new int[squares][squares]; //get the data for the board
    side_squares = squares;  //save the size of the board
    side_length = side_pixels;  //save the size in pixels of the board
    max_color = colors - 1;  //-1 becuase colours start at zero
  }

  game_board(game_board source){ //a copying constructor
    board = new int[source.side_squares][source.side_squares];
    side_squares = source.side_squares; //save the number of squares to side
    max_color = source.max_color;      //save the maximum color
    side_length = source.side_length; //save the length in pixels
    copy_board(source);     //copy over the board data
  }

public void randomize(){ //randomize the board
  Random num_gen = new Random(); //random number generator
  int i,j;
  
  for (i=0;i<side_squares;i++)
    for (j=0;j<side_squares;j++){
      board[i][j]=(num_gen.nextInt()%(max_color+1));
      if (board[i][j] < 0)
	board[i][j]*=-1;
    }
}

public  void increment_square (int x1,int y1){  //increments this square
    if (x1<0 || x1 > side_squares -1 || y1 <0 || y1 > side_squares-1)
      return;                      //return if not a valid square
    else{                          //otherwise increment the square
      board[x1][y1]++;
      if (board[x1][y1]>max_color)
	board[x1][y1] = 0;
    }
  }


public  void apply_move_s (int x,int y){  //what to do on a click,takes squares
  //first convert from pixels to squares

  //now increment the appropriate squares
  increment_square(x,y);     //invalid squares handled in increment_square
  increment_square(x-1,y);
  increment_square(x+1,y);
  increment_square(x,y-1);
  increment_square(x,y+1);
}
  

public void apply_move_p(int xpix,int ypix){ //apply click, takes pixels
  int x,y;

  if (xpix>side_length || ypix >side_length) //only take valid moves
    return;

   x = xpix/((int)(side_length/side_squares)); //convert pixels to location
   y = ypix/((int)(side_length/side_squares));

   apply_move_s(x,y);  //call the square version of apply_move
}

public void undo_move(int x,int y){ //undoes a given move
  int i;

  for (i=0;i<max_color;i++)  //apply the same move once for each color-1
    apply_move_s(x,y);
}

public void draw_board(int x,int y,Graphics g){ //draws the board 
  int i,j;
  
  //now fill the squares
  for (i=0;i<side_squares;i++)
    for (j=0;j<side_squares;j++)
      fill_square(x+i,y+j,g);   //fill in all the appropriate squares  
}


public void fill_square(int x1,int y1,Graphics g){//fills the square based 
  int x,y,side;  //the x and coordinates, and the length of the square

  //set appropriate colours

  if (board[x1][y1]==0)
    g.setColor(Color.lightGray);
  else if (board[x1][y1]==1)
    g.setColor(Color.red);
  else if (board[x1][y1]==2)
    g.setColor(Color.blue);
  else if (board[x1][y1]==3)
    g.setColor(Color.green);
  else if (board[x1][y1]==4)
    g.setColor(Color.pink);
  else if (board[x1][y1]==5)
    g.setColor(Color.yellow);

  
  x = x1 * (int)(side_length/side_squares)+1;  //calculate the x and y
  y = y1 * (int)(side_length/side_squares)+1;

  side = (int)(side_length/side_squares)-1;  //offset 1, leave outline
  g.fillRect(x,y,side,side); //fill the rectangle
}

public boolean copy_board(game_board source){ //copy in a board
  if (source.side_squares != side_squares)  //if boards are not the same size
    return false;           //return that couln't complete

  int i,j;

  for (i=0;i<side_squares;i++)
     for (j=0;j<side_squares;j++)
       board[i][j] = source.board[i][j];  //copy over every square

  return true;   //everything succeeded

}

public boolean is_completed(){ //return if this is a solved board
  int color;
  int i,j;
  
  color = board[0][0];  //compare this color with all others
  
  for (i=0;i<side_squares;i++) //go through, checking for if color not same
    for(j=0;j<side_squares;j++)
      if (board[i][j] != color)
	return false;
  
  return true;
}

} 

class solver extends Thread{  //a class to solve a board,run in background
  game_board start_board;  //the board to start from
  TextArea solve_window;  //The textarea to put the solve info into
  int max_depth = 12;  //the maximum depth to probe to
  int current_depth = 1;  //current depth in the solve
  int visited = 0;  //the number of boards that have been visited
  int solved[][] = new int[max_depth+1][2]; //hold the moves

  solver (game_board board,TextArea t){  //thread constructor
    start_board = new game_board(board);  //create and copy new board
    solve_window = t;   //get the textarea to write to
  }
  
public void run(){  //The thread's run method, what to do on start
  int i;
  
  visited = 0;
  solve_window.appendText("Beginning solve.\n");  //little greeting


  for (i=1;i<=max_depth;i++){ //go through each depth of search
    solve_window.appendText("Depth search of "+i+"\n");
    if (try_all_squares(i) == true){   /*try every position from here*/
      break;
    }
    solve_window.appendText(visited+" boards visited.\n");
  }     
  solve_window.appendText("Done solving\n");

}

public boolean try_all_squares(int depth){ //do a board search of depth depth
  int i,j,k,l,m;  //counter variables
  
  i=j=0;

  if (start_board.is_completed()){ //start off the bat checking if completed
    print_solution();
    return true;
  }

  if (current_depth > depth)  //basis, don't recurse too far
    return false;             //return didn't find anything


   try {
     this.sleep(7);  //Brief sleep to appease netscape
   } catch (InterruptedException e){}


  for (i=0;i<start_board.side_squares;i++) //go through each possible move
    for (j=0;j<start_board.side_squares;j++){
      start_board.apply_move_s(i,j);  //apply the current move
      solved[current_depth][0]=i; //save the current move on solved stack
      solved[current_depth][1]=j;

      if (is_repeat()){  //if this move would be a repeated move
	start_board.undo_move(i,j);  //undo the last move so can resume
	continue;       //skip this move, should have some savings
      }
      visited++;  //count the number of boards seen

      
      if (start_board.is_completed()){ //if this move completed the board
	print_solution();  //print out the solution
	return true;
      }

      else { //didn't solve the board with this move

	current_depth++;         //go to next depth
	if (try_all_squares(depth)==true) //if next depth found a solution
	  return true;              //then done, return
	else
	  current_depth--;   //come back to this depth and try another

      }
      start_board.undo_move(i,j);  //undo the last move so can continue
   }
  start_board.undo_move(i,j); //restore the board
  return false;
}  

public void print_solution(){ //print out the found solution
  int m;

  solve_window.appendText("Solution after "+visited+" boards.");
  solve_window.appendText("\nMoves are:\n");

  

  if (current_depth >= 1 && visited > 0) //if actually had to make a move
    for (m=1;m<=current_depth;m++)  //print out each stored move
      solve_window.appendText((solved[m][0]+1) +"," + (solved[m][1]+1)+"\n");
  else  //didn't have to make a move
    solve_window.appendText("Board already solved.\n");
}


public boolean is_repeat(){ //finds redundant repeats
  int tx,ty,j;
  int count=0;                  //holds the number of repeated

  if (current_depth < start_board.max_color+1) //if not deep enough
    return false;  //there could not possibly be a repeat

  tx = solved[current_depth][0];  //get the last move made
  ty = solved[current_depth][1];

  j = current_depth;   //count down from the current depth

  while ((tx == solved[j][0]) && (ty == solved[j][1]) && j >= 1){ 
    count++;     //count the repeat
    j--;         //go back one depth
  }
  return (count >= start_board.max_color+1);//return if redundant
}

}
















