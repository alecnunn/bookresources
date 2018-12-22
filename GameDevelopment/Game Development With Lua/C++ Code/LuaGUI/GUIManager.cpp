// GUIManager.cpp: implementation of the CGUIManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <cLua.h>
#include <DXBase.h>

#include "GUIObject.h"
#include "GUISprite.h"
#include "GUIButton.h"
#include "GUITextField.h"
#include "GUIManager.h"
#include "UserInterface.h"

#include <WinConsole.h>

extern CWinConsole *g_Console;

extern "C" {
typedef struct 
{
	const char *name;
	int (*func)(lua_State *);
}luaDef;
}

extern luaDef GUIGlue[];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUIManager *CGUIManager::m_pInstance = NULL;

CGUIManager::CGUIManager()
{
	m_pInstance		= this;
	m_pCurrentUI	= NULL;
	m_LuaState		= NULL;

	m_fButtonHoverCountdownTime = 1.0f;
}

CGUIManager::~CGUIManager()
{
	ClearCache();
	m_pInstance		= NULL;
}

void CGUIManager::ClearCache(void)
{
	std::map<std::string, CUserInterface *>::iterator it = m_GUIMap.begin();
	while(it != m_GUIMap.end())
	{
		SAFE_DELETE((*it).second);
		it++;
	}

	m_GUIMap.clear();

}


bool CGUIManager::Init(cLua *pContext)
{
	m_LuaState = pContext;

	// init the glue functions required for the GUI
	for(int i=0; GUIGlue[i].name; i++)
	{
		pContext->AddFunction(GUIGlue[i].name, GUIGlue[i].func);
	}

	// create the startup GUI, this has all the global script
	// values defined for other lua scripts. It needs to be run.
	StartGUI("StartGUI.lua");

	return true;
}

bool  CGUIManager::StartGUI(const char *pFilenameIn)
{
	char *pFilename  = new char[strlen(pFilenameIn)+1];
	memset(pFilename, 0, strlen(pFilenameIn)+1);
	for(int i=0; i<(int) strlen(pFilenameIn); i++)
		pFilename[i] = toupper(pFilenameIn[i]);

	std::map<std::string, CUserInterface *>::iterator it = m_GUIMap.find(pFilename);

	if(it == m_GUIMap.end())
	{
		m_pCurrentUI = new CUserInterface;
		m_GUIMap[pFilename] = m_pCurrentUI;
		// create the startup GUI
		// prepend the scripts directory name to the filename for loading
		char path[254];
		sprintf(path, "Scripts\\%s", pFilename);
		m_pCurrentUI->Init(path);
		SendEvent(GUI_ENTER_INTERFACE, 0);

	}
	else
	{
		m_pCurrentUI = (*it).second;

		if(!m_pCurrentUI->GetEventHandler())
		{
			m_pCurrentUI->Init(pFilename);
			SendEvent(GUI_ENTER_INTERFACE, 0);
		}
		else
		{
			SendEvent(GUI_REENTER_INTERFACE, 0);
		}


	}

	delete pFilename;
	return true;
}

void CGUIManager::SendEvent(int iEventCode, int id, float arg1, float arg2, float arg3, float arg4)
{
	if(m_pCurrentUI)
	{
		if(m_pCurrentUI->GetEventHandler())
		{
			char command[254];

			sprintf(command, "%s(%d,%d,%f,%f,%f,%f);", m_pCurrentUI->GetEventHandler(), id, iEventCode, arg1, arg2, arg3, arg4);
			ScriptExecuteString(command);
		}
	}
}


bool  CGUIManager::KeyHit(int ascii)
{
	if(m_pCurrentUI)
	{
		return m_pCurrentUI->KeyHit(ascii);
	}
	return false;
}



bool  CGUIManager::Update(float fSecsElapsed)
{
	m_bMouseOverGUI = false;
	if(m_pCurrentUI)
	{
		m_bMouseOverGUI = m_pCurrentUI->IsMouseOverGUI();
		m_pCurrentUI->Update(fSecsElapsed);
	}

	return true;
}

bool  CGUIManager::Render()
{
	if(m_pCurrentUI)
	{

		m_pCurrentUI->Render();

	}
	return true;
}

bool CGUIManager::IsMouseOverGUI()
{
	return m_bMouseOverGUI;
}

void CGUIManager::NotifyMouseButtonDown(int button)
{
		SendEvent(GUI_MOUSE_BUTTON_DOWN, button); 
}

void CGUIManager::NotifyMouseButtonUp(int button)
{
		SendEvent(GUI_MOUSE_BUTTON_UP, button); 
}


void CGUIManager::ScriptExecuteString(char *command)
{
	m_LuaState->RunString(command);
}

void CGUIManager::ResChangeNotify(void)
{
	std::map<std::string, CUserInterface *>::iterator it = m_GUIMap.begin();
	while(it != m_GUIMap.end())
	{
		(*it).second->ResChangeNotify();
		it++;
	}

}


//Lua API for GUI

bool InRect(int x, int y, const RECT &r)
{
	if((x < r.left) || (x > r.right))
		return false;
	if((y < r.top) || (y > r.bottom))
		return false;

	return true;
}



extern "C" int GUI_CreateItem(lua_State *L)
{
	CUserInterface *pUI = CGUIManager::GetInstance()->GetCurrentUI();
	if(pUI)
	{
		cLua *lua = CGUIManager::GetInstance()->GetLuaContext();

		unsigned int id = (unsigned int) lua->GetNumberArgument(1);
		const char *type = lua->GetStringArgument(2);

		CGUIObject *pObject = NULL;

		if(strcmp(type, kpSpriteName) == 0)
		{
			//create a sprite
			CGUISprite *pSprite = new CGUISprite;
			pSprite->SetTexture((char *) lua->GetStringArgument(3));
			pObject = pSprite;
		}

		if(strcmp(type, kpTextFieldName) == 0)
		{
			//create a Text Field
			CTextField  *pTextField = new CTextField;
			pTextField->SetTexture((char *) lua->GetStringArgument(3, NULL));
			pObject = pTextField;
		}

		if(strcmp(type, kpButtonName) == 0)
		{
			//create a button
			CButton *pButton = new CButton;
			char *norm	= (char *) lua->GetStringArgument(3, NULL);
			char *hover = (char *) lua->GetStringArgument(4, NULL);
			char *sel	= (char *) lua->GetStringArgument(5, NULL);
			char *dis	= (char *) lua->GetStringArgument(6, NULL);

			pButton->SetButtonTextures(norm, hover, sel, dis);
			pObject = pButton;
		}

		if(pObject)
		{
			pUI->DeleteGUIObject(id);
			pObject->SetID(id);
			pUI->AddGUIObject(id, pObject);
		}
	}
	return 0;
}

extern "C" int GUI_DeleteItem(lua_State *L)
{
	CUserInterface *pUI = CGUIManager::GetInstance()->GetCurrentUI();
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	pUI->DeleteGUIObject((unsigned int) lua->GetNumberArgument(1));
	return 0;
}

extern "C" int GUI_SetItemPosition(lua_State *L)
{
	CUserInterface *pUI = CGUIManager::GetInstance()->GetCurrentUI();
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	pUI->SetGUIObjectPosition(	(unsigned int) lua->GetNumberArgument(1),
								(float) lua->GetNumberArgument(2),
								(float) lua->GetNumberArgument(3),
								(float) lua->GetNumberArgument(4),
								(float) lua->GetNumberArgument(5));
	return 0;
}

extern "C" int GUI_SetEventHandler(lua_State *L)
{
	CUserInterface *pUI = CGUIManager::GetInstance()->GetCurrentUI();
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	pUI->SetEventHandler(lua->GetStringArgument(1, NULL));

	return 0;
}

extern "C" int GUI_ItemCommand(lua_State *L)
{
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	unsigned int id = (unsigned int) lua->GetNumberArgument(1);
	char *pCommand = (char *) lua->GetStringArgument(2);
	int retVal = 0;

	CUserInterface *pUI = CGUIManager::GetInstance()->GetCurrentUI();
	if(pUI)
	{
		retVal = pUI->ObjectCommand(id, pCommand);
	}

	return retVal;
}

extern "C" int GUI_RunGUI(lua_State *L)
{
	// start up a new GUI 
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	char *scriptName = (char *) lua->GetStringArgument(1);

	CGUIManager::GetInstance()->StartGUI(scriptName);

	return 0;
}

extern "C" int GUI_SetCoordTrans(lua_State *L)
{
#if 0
	float width  = (float) luaL_checknumber(L, 1);
	float height = (float) luaL_checknumber(L, 2);

	CUserInterface *pUI = CGUIManager::GetInstance()->GetCurrentUI();
	if(pUI)
		pUI->SetCoordTrans(width, height);
#endif
	return 0;
}


extern "C" int GUI_SetFont(lua_State *L)
{
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();

	unsigned int id       = (unsigned int) lua->GetNumberArgument(1);
	char *fontName		  = (char *) lua->GetStringArgument(2);
	unsigned int fontSize = (unsigned int) lua->GetNumberArgument(3);


	CUserInterface *pUI = CGUIManager::GetInstance()->GetCurrentUI();
	if(pUI)
		pUI->SetGUIObjectFont(id, fontName, fontSize);
	return 0;
}
	
extern "C" int GUI_SetTexture(lua_State *L)
{
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();

	unsigned int id       = (unsigned int) lua->GetNumberArgument(1);
	char *texName		  = (char *) lua->GetStringArgument(2);


	CUserInterface *pUI = CGUIManager::GetInstance()->GetCurrentUI();
	if(pUI)
	{
		CGUIObject *pObject = pUI->FindObject(id);
		if(pObject)
		{
			pObject->SetTexture(texName, false);
		}
	}
	return 0;
}

extern "C" int GUI_EnableObject(lua_State *L)
{
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	unsigned int id			= (unsigned int) lua->GetNumberArgument(1);
	int disabled			= (int) lua->GetNumberArgument(2);
	int keepDrawing			= (int) lua->GetNumberArgument(3, 0);

	CUserInterface *pUI = CGUIManager::GetInstance()->GetCurrentUI();
	if(pUI)
		pUI->EnableObject(id, disabled==0, keepDrawing!=0);

	return 0;
}

extern "C" int GUI_StartTimer(lua_State *L)
{
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	CUserInterface *pUI = CGUIManager::GetInstance()->GetCurrentUI();
	if(pUI)
		pUI->StartTimer((float) lua->GetNumberArgument(1));

	return 0;
}

extern int g_mouseButtons;
extern POINT g_mousePoint;

extern "C" int GUI_GetMousePosition(lua_State *L)
{
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	lua->PushNumber(g_mousePoint.x);
	lua->PushNumber(g_mousePoint.y);
	return 2;
}

static std::string findScript(const char *pFname)
{
	FILE *fTest;

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( pFname, drive, dir, fname, ext );

	std::string strTestFile = (std::string) drive + dir + "Scripts\\" + fname + ".LUB";
	fTest = fopen(strTestFile.c_str(), "r");
	if(fTest == NULL)
	{
		//not that one...
		strTestFile = (std::string) drive + dir + "Scripts\\" + fname + ".LUA";
		fTest = fopen(strTestFile.c_str(), "r");
	}

	if(fTest == NULL)
	{
		//not that one...
		strTestFile = (std::string) drive + dir + fname + ".LUB";
		fTest = fopen(strTestFile.c_str(), "r");
	}

	if(fTest == NULL)
	{
		//not that one...
		//not that one...
		strTestFile = (std::string) drive + dir + fname + ".LUA";
		fTest = fopen(strTestFile.c_str(), "r");
	}

	if(fTest == NULL)
	{
		g_Console->Write("Script Search failed\nScript Name:");
		g_Console->Write(pFname);
		g_Console->Write("\n");
	}

	fclose(fTest);

	return strTestFile;
}


void RunScript(const char *pFname)
{
	std::string strFilename = findScript(pFname);
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	lua->RunScript(strFilename.c_str());
}

void RunString(const char *pScript)
{
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	lua->RunString(pScript);
}


extern "C" int BASE_RunScript(lua_State *L)
{
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	RunScript(lua->GetStringArgument(1));
	return 0;
}

extern "C" int BASE_RunString(lua_State *L)
{
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	RunString(lua->GetStringArgument(1));
	return 0;
}


extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

extern "C" int GUI_GetCollisions(lua_State *L)
{
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	unsigned int id	= (unsigned int) lua->GetNumberArgument(1);

	// create the return table on the stack 	
    lua_newtable (L);

	CUserInterface *pUI = CGUIManager::GetInstance()->GetCurrentUI();
	unsigned int i = 1;
	if(pUI)
	{
		std::list<unsigned int> list = pUI->SpriteCollision(id);
		
		std::list<unsigned int>::iterator it = list.begin();
		while(it != list.end())
		{
			lua_pushnumber(L, i);
			lua_pushnumber(L, (*it));
			lua_settable (L, -3);
			++i;
			++it;
		}
	}

	// now add the number of entries in the table as member "n" (t.n)
	lua_pushstring(L, "n");
	lua_pushnumber(L, i-1);
	lua_settable (L, -3);

	return 1;
}

extern "C" int GUI_HitTest(lua_State *L)
{
	cLua *lua = CGUIManager::GetInstance()->GetLuaContext();
	float x	= (float) lua->GetNumberArgument(1);
	float y	= (float) lua->GetNumberArgument(2);

	// create the return table on the stack 	
    lua_newtable (L);

	CUserInterface *pUI = CGUIManager::GetInstance()->GetCurrentUI();
	unsigned int i = 1;
	if(pUI)
	{
		std::list<unsigned int> list = pUI->HitTest(x, y);
		
		std::list<unsigned int>::iterator it = list.begin();
		while(it != list.end())
		{
			lua_pushnumber(L, i);
			lua_pushnumber(L, (*it));
			lua_settable (L, -3);
			++i;
			++it;
		}
	}

	// now add the number of entries in the table as member "n" (t.n)
	lua_pushstring(L, "n");
	lua_pushnumber(L, i-1);
	lua_settable (L, -3);

	return 1;
}

luaDef GUIGlue[] = 
{
  {"SetCoordTrans",			GUI_SetCoordTrans},
  {"CreateItem",			GUI_CreateItem},
  {"DeleteItem",			GUI_DeleteItem},
  {"SetItemPosition",		GUI_SetItemPosition},
  {"SetEventHandler",		GUI_SetEventHandler},
  {"ItemCommand",			GUI_ItemCommand},
  {"RunGUI",				GUI_RunGUI},

  {"SetFont",				GUI_SetFont},
  {"SetTexture",			GUI_SetTexture},
  {"EnableObject",			GUI_EnableObject},
  {"StartTimer",			GUI_StartTimer},

  {"GetMousePosition",		GUI_GetMousePosition},
  {"GetCollisions",			GUI_GetCollisions},
  {"HitTest",				GUI_HitTest},

  // Script running function
  {"RunScript",				BASE_RunScript},
  {"RunString",				BASE_RunString},

  {NULL, NULL}
};

