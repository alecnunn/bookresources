-- Scrip2.lua
-- Beep like crazy every 10 seconds and report if the user aborted the wait.

---------------------------------------------------------------
-- BEEP LIKE CRAZY

function BeepLikeCrazy()
    for i=0,16 do
        frequency = 440 + math.random(8)*55;
        Script.Beep(frequency, 50);
        end
    end;

---------------------------------------------------------------
-- ENTRY POINT

    print("Script2: Starting up.");
    print("  Script2 will delay for 20 seconds and then beep like crazy.");
    print("  Press [2] to bypass the current delay for script 2.");
    Script.Beep(440, 10);

    while (1) do
        ret = Script.WaitSec(10);
        if (ret == 1) then
            print("Script2: User aborted wait.");
            end

        print("Script2: Crazy beeps!");
        BeepLikeCrazy();
    end;
