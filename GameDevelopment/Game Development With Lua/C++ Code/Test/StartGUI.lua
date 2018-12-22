--=======================================
-- (c) copyright 2004, Magic Lantern, Inc
-- All Rights Reserved.  U.S.A.
--=======================================
--  filename: StartGUI.lua
--  author: Mark Manyen
--  created: Sept 17, 2004
--  descrip: Defines constants and globals
--=========================================--

-- define constant values for all scripts
-- Standard LuaGUI event codes
GUI_EVENT_BUTTON_UP = 0
GUI_EVENT_BUTTON_DOWN = 1
GUI_EVENT_SELECTION_CHANGED = 2
GUI_EVENT_TEXTFIELD_CLICKED = 3
GUI_KEY_PRESS = 4
GUI_REENTER_INTERFACE = 5
GUI_TIMER_EXPIRED = 6
GUI_ENTER_INTERFACE = 7
GUI_TEXT_SCROLL_END = 8
GUI_EVENT_TEXTFIELD_RETURN = 9
GUI_EVENT_HOVER_TIMED_START = 10
GUI_EVENT_HOVER_END = 11


-- Define screen res values
RES640 = 0
RES800 = 1
RES1024 = 2
RES1600 = 3

-- Define audio volume levels
AUDIO_VOLUME_OFF   = 0
AUDIO_VOLUME_LOW   = 1
AUDIO_VOLUME_MED   = 2
AUDIO_VOLUME_HIGH  = 3

--boolean contants
YES = 1
NO = 0
NONE = 0


--seed the random number
math.randomseed(os.date("%d%H%M%S"))



--start in the loading screen
RunGUI("GUIMainMenu.lua")

