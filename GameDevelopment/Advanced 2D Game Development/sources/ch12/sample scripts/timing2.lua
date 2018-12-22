--[[

	FUNCTION PRIFILING TEST
	(EMBEDDED SQRT VERSION)

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

function Timer()
	return os.clock() * 1000
end

function SlowMathFunction()
	r = random_range(1,999999)
	num = math.sqrt(r)
	return num
end

print("Profiling function...")

TOTAL = 2000000
start = Timer()

for n = 1,TOTAL,1 do
	--var = SlowMathFunction()
	r = random_range(1,999999)
	num = math.sqrt(r)
end

finish = Timer()
delta = finish-start
print("Total run time: " .. delta .. " ms" )

print("Function run time: ")

milli = round(delta / TOTAL, 8)
micro = round(milli * 1000, 8)
nano = round(micro * 1000, 8)

print("  milliseconds: " .. milli )
print("  microseconds: " .. micro )
print("  nanoseconds : " .. nano )


