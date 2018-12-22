--=======================================
-- Game Development With Lua
-- by Paul Schuytema and Mark Manyen
-- (c) copyright 2005, Charles River Media
-- All Rights Reserved.  U.S.A.
--=======================================
-- filename:  StartGUI.lua
-- author:    Nick Carlson
-- created:   February 10, 2005
-- descrip:   Special start-up script
--=======================================

-- Define constant values for all scripts

-- Standard LuaGUI event codes
GUI_EVENT_BUTTON_UP = 0
GUI_EVENT_BUTTON_DOWN = 1
GUI_EVENT_SELECTION_CHANGED = 2
GUI_EVENT_TEXTFIELD_CLICKED = 3
GUI_KEY_PRESS = 4
GUI_REENTER_INTERFACE = 5
GUI_TIMER_EXPIRED = 6;
GUI_ENTER_INTERFACE = 7
GUI_TEXT_SCROLL_END = 8

--Use constants
NO = 0
YES = 1
HORIZONTAL = 2
VERTICAL = 3

-- Sub-interface constants
GUI_INGAME = 0
GUI_RUNTIME_SPRITES = 1000
GUI_MAIN_MENU = 2000
GUI_ESCAPE = 3000
GUI_KEY_SELECT = 4000
GUI_LOADING = 5000
GUI_END_GAME = 6000


-- This initializes the game at startup.
math.randomseed(os.date("%d%H%M%S"))

-- Load in the support functions
dofile("Scripts\\LuaSupport.Lua")

--Start with the loading screen
RunGUI("GUI_MainMenu.lua")
