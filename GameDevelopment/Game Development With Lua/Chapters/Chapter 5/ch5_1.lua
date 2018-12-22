-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- Published by Charles River Media

-- Lua script example

-- Listing 5.1

-- Function examples


--=======================================
-- function:  Wow()
-- author:    Paul Schuytema
-- created:   January 20, 2005
-- returns:   nothing (process)
-- descrip:   prints out a favorable statement
--=======================================
function Wow()
    print(" ")
	print("Wow, that was awesome!")
    print(" ")
end

--=======================================
-- function:  SetName(myString)
-- author:    Paul Schuytema
-- created:   January 20, 2005
-- returns:   nothing (process)
-- descrip:   prints out a person's name
--=======================================
function SetName(myString)
    print(" ")
	print("Your name is:", myString)
    print(" ")
end

--=======================================
-- function:  MyInfo(myName, myAge)
-- author:    Paul Schuytema
-- created:   January 20, 2005
-- returns:   nothing (process)
-- descrip:   prints out a person's data
--=======================================
function MyInfo(myName, myAge)
    print(" ")
	print("Your name is:", myName)
    print("Your age is:", myAge)
    print(" ")
end

--=======================================
-- function:  HowMany(...)
-- author:    Paul Schuytema
-- created:   January 20, 2005
-- returns:   nothing (process)
-- descrip:   prints out a variable number of arguments
--=======================================
function HowMany(...)
    if arg.n > 0 then
        for indx = 1, arg.n do
            local myString = string.format("%s%d", "Argument ", indx, ":")
            print(myString, arg[indx])
        end
    else
        print("No variables entered.")
    end
end

--=======================================
-- function:  Multiply(val1, val2, ...)
-- author:    Paul Schuytema
-- created:   January 20, 2005
-- returns:   nothing (process)
-- descrip:   Multiplies two numbers together arg[1] times
--=======================================
function Multiply(val1, val2, ...)
    --the default is one
    local myString
    if arg.n == 0 then
        myString = string.format("%d%s%d%s%d", val1, " * ", val2, " = ", val1 * val2)
    else
        local val3 = val1 * val2 * arg[1]
        myString = string.format("%d%s%d%s%d%s%d", val1, " * ", val2, " * ", arg[1], " = ", val3)
    end
    print(myString)
end

--=======================================
-- function:  TimesTwo(myValue)
-- author:    Paul Schuytema
-- created:   January 20, 2005
-- returns:   myValue * 2
-- descrip:   Multiplies arg1 by 2
--=======================================
function TimesTwo(myValue)
    myValue = myValue * 2
    return myValue
end

--=======================================
-- function:  ThreeDice()
-- author:    Paul Schuytema
-- created:   January 24, 2005
-- returns:   the values of 3D6
-- descrip:   returns four values, 1D6 (x3) and the total
--=======================================
function ThreeDice()
    d1 = math.random(1,6)
    d2 = math.random(1,6)
    d3 = math.random(1,6)
    myTotal = d1 + d2 + d3
    return d1, d2, d3, myTotal
end
