--[[
	THIS IS VERY LIKELY MY FIRST LUA GAME!
]]--

function round(num, places)
  local mult = 10^(places or 0)
  return math.floor(num * mult + 0.5) / mult
end

function gen_random(max)
    local temp = math.mod((42 * math.random() + 29573), 139968)
    local ret = math.mod( ((100 * temp)/139968) * 1000000, max)
    return round(ret + 0.5)
end

function random_range(min,max)
	return gen_random(max-min) + min
end

function GetInput()
	return io.stdin:read("*l")
end

print "Try To Guess My Secret Number (1-100)"
math.randomseed( os.time() )
answer = random_range(1,100)
guess = 0
total = 0

repeat
	input = GetInput()
	guess = tonumber(input)
	if guess > answer then
		print("THE ANSWER IS LOWER")
	elseif guess < answer then
		print("THE ANSWER IS HIGHER")
	end	
	total = total + 1

until guess == answer

print("You got it in " .. total .. " tries.")

