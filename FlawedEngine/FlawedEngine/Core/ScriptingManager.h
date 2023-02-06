#pragma once

#include <unordered_map>

extern "C"
{
# include <Lua/lua.h>
# include <Lua/lauxlib.h>
# include <Lua/lualib.h>
}

#include <LuaBridge/LuaBridge.h>
#include <functional>
namespace FlawedEngine
{
	class cScriptingManager
	{
	public:
		int InitScripting();

		lua_State* GetLuaState(int ID);

		void DestroyState(int ID);

		bool RunFile(int ID, const std::string& FileName);

		void RegisterFunction(int ID, const std::string& FuncName, std::function<void(float, float, float)> Func);
	private:
		int mNextStateId;
		std::unordered_map<int, lua_State*> mStates;
	public:
		static cScriptingManager& get();
		~cScriptingManager();
		
		cScriptingManager(cScriptingManager const&) = delete;
		void operator=(cScriptingManager const&) = delete;

	private:
		static cScriptingManager* sScriptingInstance;

		cScriptingManager();
		cScriptingManager(cScriptingManager&);
	};
}


