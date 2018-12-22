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

-- Sub-interface constants
GUI_INGAME = 0

-- This initializes the game at startup.
math.randomseed(os.date("%d%H%M%S"))

-- Load in the support functions
dofile("Scripts\\LuaSupport.Lua")
dofile("Scripts\\Text.Lua")

--Start in the ingame screen
RunGUI("GUI_Loading.lua")
