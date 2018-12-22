local N = 10
local ave = 0

for i = 0, N do

	local h = Hero.Create(i)


	local t0 = os.clock()
	for i=1,1000000 do
		Hero.SetEnergy(h,i)
		if Hero.GetEnergy(h)~=i then
			error("failed");
		end
	end
	local dt = os.clock()-t0
	if i~=0 then
	 ave = ave + dt
	 print(Hero.GetName(h), dt)
	end

end

print('access (average elapsed time):',ave/N)
