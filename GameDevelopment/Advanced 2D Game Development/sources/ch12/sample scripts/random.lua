-- This program demonstrates how to calculate random numbers

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



math.randomseed( os.time() )
output = ""
for n = 1,20 do
	a = random_range(1,1000)
	output = output .. tostring(a) .. ","
end
print(output)
