--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  LuaSupport.lua
-- author:    Nick Carlson
-- created:   February 10, 2005
-- descrip:   ny functions that are required are defined here.
--=======================================


function CommaFormatBigInteger(aNumber)

	isNegative = 0
	if aNumber < 0 then
		isNegative = 1
		aNumber = -1*aNumber
	end
	baseString = string.format("%d", aNumber)
	baseLength = string.len(baseString)
	numBlocks = math.floor(baseLength/3)

	if numBlocks == 0 then
		if isNegative ~= 0 then
			newString = string.format("%d", -1*aNumber)
		else
			newString = baseString
		end
	else
		if isNegative ~= 0 then
			newString = "-"
		else
			newString = ""
		end
		numLeaders = baseLength - 3*numBlocks;
		if numLeaders > 0 then
			newString = newString .. string.sub (baseString, 1 , numLeaders) .. ","
		end
		for i = 1,numBlocks do
			newString = newString .. string.sub (baseString, numLeaders+1+(i-1)*3 , numLeaders+i*3) .. ","
		end
		newLength = string.len(newString)
		newString = string.sub(newString, 1, newLength-1); -- strip off final comma
	end

	return newString
end
