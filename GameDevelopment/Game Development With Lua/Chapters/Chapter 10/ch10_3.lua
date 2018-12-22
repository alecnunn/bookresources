
function TestSave()
    myFile = io.open("c:\\lua_scripts\\test_savegame.lua", "w")
    if myFile ~= nil then
        myFile:write("-- Test Lua SaveGame file")
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        myFile:write(string.format("%s%s", "-- File created on: ", os.date()))
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        myFile:write("myValue = 10")
        io.close(myFile)
    end
end


--Initial player values
myRotation = 1
myX = 390      --Player's x coordinate (#)
myY = 290      --Player's y coordinate (#)
myXThrust = 0  --Player's thrust along the x-axis (#)
myYThrust = 0  --Player's thrust along the y-axis (#)
alive = "yes"  --Player's life status ("yes" or "no")



function WorldSave()
    myFile = io.open("c:\\lua_scripts\\sample_savegame.lua", "w")
    if myFile ~= nil then
        myFile:write("-- Lua SaveGame file for limited Take Away data")
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        myFile:write(string.format("%s%s", "-- File created on: ", os.date()))
        myFile:write(string.char (10))
        myFile:write(string.char (10))
        myFile:write("-- ship data values")
        myFile:write(string.char (10))
        myString = string.format("%s%d", "myRotation = ", myRotation)
        myFile:write(myString)
        myFile:write(string.char (10))
        myString = string.format("%s%d", "myX = ", myX)
        myFile:write(myString)
        myFile:write(string.char (10))
        myString = string.format("%s%d", "myY = ", myY)
        myFile:write(myString)
        myFile:write(string.char (10))
        myString = string.format("%s%d", "myXThrust = ", myXThrust)
        myFile:write(myString)
        myFile:write(string.char (10))
        myString = string.format("%s%d", "myYThrust = ", myYThrust)
        myFile:write(myString)
        myFile:write(string.char (10))
        myString = string.format("%s%s%s", "alive = \"", alive, "\"")
        myFile:write(myString)
        myFile:write(string.char (10))
        io.close(myFile)
    end
end
