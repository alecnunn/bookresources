--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  LuaSupport.lua
-- author:    Paul Schuytema
-- created:   April 17, 2005
-- descrip:   function files for particle demo
--=======================================

--particle system definitions



partSystem = {}
partSystem.Texture = "kaboom_11.tga"
partSystem.BirthRate = .1
partSystem.AvgLife = 2
partSystem.LifeVariance = 1
partSystem.X = 200
partSystem.XVariance = 10
partSystem.Y = 200
partSystem.YVariance = 10
partSystem.XDrift = 10
partSystem.XDriftVariance = 6
partSystem.YDrift = -5
partSystem.YDriftVariance = 6
partSystem.Scale = 25
partSystem.ScaleVariance = 4
partSystem.ScaleChange = 1.08
partSystem.ScaleChangeVariance = .1
partSystem.BaseID = 1000
partSystem.Life = 20

function EvenDivide(num1, num2)
    --a fuzzy divide here
    print((num1/num2) - math.floor(num1/num2))
    if ((num1/num2) - math.floor(num1/num2)) < 1 then
        --even division
        return true
    else
        return false
    end
end



function InitSystem()
    curTime = 0
    timeSlice = .1
    particles = {}
    idCounter = 0
end


function CreateNewParticle()
    tempPart = {}
    tempPart.Age = 0
    idCounter = idCounter + 1
    myValue = math.random(1, partSystem.LifeVariance * 10)
    if math.random(1,100) > 50 then
        tempPart.Life = partSystem.AvgLife + (myValue/10)
    else
        tempPart.Life = partSystem.AvgLife - (myValue/10)
    end
    myValue = math.random(1, partSystem.XVariance)
    if math.random(1,100) > 50 then
        tempPart.X = partSystem.X + myValue
    else
        tempPart.X = partSystem.X - myValue
    end
    myValue = math.random(1, partSystem.YVariance)
    if math.random(1,100) > 50 then
        tempPart.Y = partSystem.Y + myValue
    else
        tempPart.Y = partSystem.Y - myValue
    end
    myValue = math.random(1,partSystem.ScaleVariance)
    if math.random(1,100) > 50 then
        tempPart.Scale = partSystem.Scale + myValue
    else
        tempPart.Scale = partSystem.Scale - myValue
    end
    tempPart.ID = partSystem.BaseID + idCounter

    table.insert(particles, tempPart)
    print("new particle created")
end


function ProcessParticles()
    --increment cumulative counter
    curTime = curTime +  timeSlice
    if curTime < partSystem.Life then
        --new particle born
        if EvenDivide(curTime, partSystem.BirthRate) then
            CreateNewParticle()
        end
    end
    removeMe = nil
    --apply particle changes
    for index,value in ipairs(particles) do
        --increment age
        value.Age =  value.Age + timeSlice
        --process drift
        myValue = math.random(1, partSystem.XDriftVariance)
        if math.random(1,100) > 50 then
            value.X = value.X + partSystem.XDrift + myValue
        else
            value.X = value.X + partSystem.XDrift - myValue
        end
        myValue = math.random(1, partSystem.YDriftVariance)
        if math.random(1,100) > 50 then
            value.Y = value.Y + partSystem.YDrift + myValue
        else
            value.Y = value.Y + partSystem.YDrift - myValue
        end
        --process scale change
        myValue = math.random(0, partSystem.ScaleChangeVariance * 10)
        if math.random(1,100) > 50 then
            value.Scale = (value.Scale + (myValue/10)) * partSystem.ScaleChange
        else
            value.Scale = (value.Scale - (myValue/10)) * partSystem.ScaleChange
        end
        --render particle
        if value.Age < value.Life then
            CreateItem(value.ID, "Sprite", partSystem.Texture)
            SetItemPosition(value.ID, value.X, value.Y, value.Scale, value.Scale)
        else
            DeleteItem(value.ID)
            removeMe = index
        end
    end
    --now remove old particles
    if removeMe ~= nil then
        table.remove(particles, removeMe)
    end
    if table.getn(particles) > 0 then
        StartTimer(timeSlice)
    end
end



