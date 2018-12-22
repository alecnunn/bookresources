echo Testing OS File System...
call FakeGame -startup startup.txt -resource_dir resources

echo Testing Resource File...
call FakeGame -startup startup.txt -resource_file resource.jam

echo Testing Optimized Resource File...
call FakeGame -startup startup.txt -resource_file resource_opt.jam

pause