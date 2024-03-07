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

		bool LoadFile(int ID, const std::string& FileName);

		void RunFunction(int ID, const std::string& FuncName);
		void RunFunctionWithArgs(int ID, const std::string& FuncName, const std::string& args);

		template<typename Function>
		void RegisterFunctionInNamespace(int ID, const std::string& Namespace, const std::string& FuncName, Function funcptr)
		{
			using namespace luabridge;
			lua_State* L = GetLuaState(ID);
			if (!L)
			{
				std::cout << "[ERROR] Couldn't Register Function, Lua State is undefined" << std::endl;
				return;
			}
			getGlobalNamespace(L).beginNamespace(Namespace.c_str()).addFunction(FuncName.c_str(), funcptr).endNamespace();
		}

		template<typename Function>
		void RegisterFunction(int ID, const std::string& FuncName, Function funcptr)
		{
			using namespace luabridge;
			lua_State* L = GetLuaState(ID);
			if (!L)
			{
				std::cout << "[ERROR] Couldn't Register Function, Lua State is undefined" << std::endl;
				return;
			}
			getGlobalNamespace(L).addFunction(FuncName.c_str(), funcptr);
		}
	private:
		int mNextStateId;
		std::unordered_map<int, lua_State*> mStates;
		void SpawnObject(const char*, int);
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