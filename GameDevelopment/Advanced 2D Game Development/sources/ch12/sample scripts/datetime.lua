--[[
	GAM215 SCRIPTING
	BENCHMARK TIMING PROGRAM

]]--

start_time = 0
function Stopwatch(ms)
	if Timer() > start_time + ms then
		start_time = Timer()
		return true
	else
		return false
	end
end

function Timer()
	return os.clock() * 1000
end

print("Date: " .. os.date())
print("Time: " .. os.time())

repeat

	if Stopwatch(1000) then
		print("os.clock(): " .. os.clock())
		print("Timer(): " .. Timer())
	end

until false

