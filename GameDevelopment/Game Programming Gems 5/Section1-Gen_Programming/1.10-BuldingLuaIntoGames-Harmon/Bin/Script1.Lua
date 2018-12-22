-- Test1.lua
-- Beep every 3 seconds.


---------------------------------------------------------------
-- ENTRY POINT

    print("Script1: starting up.");
    print("  This script just beeps every 3 seconds.");
    Script.Beep(440, 10);

    while (1) do
        Script.WaitSec(3);
        Script.Beep(440, 20);
        print("Script1: Beep");
    end;
