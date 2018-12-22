echo Building Resource File...
call ResourcePacker resources resource.jam

del filelog.txt

echo Creating Resource Log...
call FakeGame -startup startup.txt -resource_dir resources -log

echo Building Optimized Resource File...
call ResourcePacker resources resource_opt.jam filelog.txt

pause