--=======================================
-- (c) copyright 2005, Magic Lantern, Inc
-- All Rights Reserved.  U.S.A.
--=======================================--
--  filename: Chapter7.lua
--  author:   Paul Schuytema
--  created:  Feb 6, 2005
--  descrip:  Example
--=======================================--


myBoard = {} 
myBoard[1] = {1,0,0} 
myBoard[2] = {0,2,0} 
myBoard[3] = {0,0,0} 

move = GetMove(myBoard[1][1], myBoard[1][2], myBoard[1][3],
			   myBoard[2][1], myBoard[2][2], myBoard[2][3],
			   myBoard[3][1], myBoard[3][2], myBoard[3][3],
			   1)

SetEventHandler("TTTEvent")



function TTTEvent(id, eventCode)

    if eventCode == GUI_KEY_PRESS then

		print "Hello";

		QuitProgram();
    end

end
