-- Scrip3.lua
-- Example of how C can call a Lua function.

---------------------------------------------------------------
-- BEEP LIKE CRAZY

function Beeps(count)
    print("Script3: Beeps("..count..") called by C program.");
    for i=0,count do
        Script.Beep(880, 50);
        Script.Beep(440, 50);
        end
    end;

---------------------------------------------------------------
-- ENTRY POINT

    print("Script3: Loaded.");
    print("  Press [3] to call the function Beeps() defined in Script3.lua");
    Script.Beep(440, 10);
