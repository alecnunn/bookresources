-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- Published by Charles River Media

-- Lua script example

-- Listing 10.1

--Asteroids ship data example

--PLAYER_SHIP_ID = 1000
--myShip = {}

--myShip.ID = PLAYER_SHIP_ID
--myShip.Width = 64
--myShip.Height = 64
--myShip.Xposition = 100
--myShip.Yposition = 100
--myShip.Rotation = 0
--myShip.Xthrust = 0
--myShip.Ythrust = 0
--myShip.ShowThrust = false

--=======================================
-- function:  UpdateShip(theShip)
-- author:    Paul Schuytema
-- created:   February 15, 2005
-- returns:   nothing (process)
-- descrip:   Updates the postion and momvent data of a ship
--=======================================
--function UpdateShip(theShip)
--    theShip.Xposition = theShip.Xposition + myShip.Xthrust
--    theShip.Yposition = theShip.Yposition + myShip.Ythrust
    --SetItemPosition(theShip.ID, theShip.Xposition, theShip.Yposition, myShip.Width, myShip.Height)
--    if myShip.ShowThrust then
        --DrawThurstImage(theShip.ID)
--    end
--end

myShip = {}
yValue = 60
for index = 1,5 do
    myShip[index] = {}
    myShip[index].Xposition = 100
    myShip[index].Yposition = yValue
    yValue = yValue + 20
    myShip[index].Rotation = 0
    myShip[index].Xthrust = 0
    myShip[index].Ythrust = 0
    myShip[index].ShowThrust = false
end

lastEntry = table.getn(myShip)
myShip[lastEntry].Xposition = 100
myShip[lastEntry].Yposition = 100
myShip[lastEntry].Rotation = 0
myShip[lastEntry].Xthrust = 100
myShip[lastEntry].Ythrust = 0
myShip[lastEntry].ShowThrust = false
