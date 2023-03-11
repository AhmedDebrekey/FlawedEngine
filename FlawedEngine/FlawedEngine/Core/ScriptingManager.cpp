#include "ScriptingManager.h"
#include "Models/ObjectManager.h"

namespace FlawedEngine
{
	cScriptingManager* cScriptingManager::sScriptingInstance = nullptr;

	cScriptingManager::cScriptingManager()
		:mNextStateId(0)
	{

	}

	cScriptingManager::~cScriptingManager()
	{

	}

	int cScriptingManager::InitScripting()
	{
		int ID = mNextStateId++;
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);
		mStates[ID] = L;

		std::function<void(const char*, int)> SpawnObjectFn = std::bind(&cScriptingManager::SpawnObject, this, std::placeholders::_1, std::placeholders::_2);
		using namespace luabridge;
		getGlobalNamespace(L)
			.addFunction("SpawnObject", SpawnObjectFn);
		return ID;
	}

	lua_State* cScriptingManager::GetLuaState(int ID)
	{
		auto State = mStates.find(ID);
		return (State != mStates.end()) ? State->second : nullptr;
	}

	void cScriptingManager::DestroyState(int ID)
	{
		auto State = GetLuaState(ID);
		if (State)
		{
			lua_close(State);
			mStates.erase(ID);
		}
	}

	bool cScriptingManager::RunFile(int ID, const std::string& FileName)
	{
		lua_State* L = GetLuaState(ID);
		if (!L)
		{
			return false;
		}

		int result = luaL_dofile(L, FileName.c_str());
		if (result != LUA_OK)
		{
			const char* error = lua_tostring(L, -1);
			std::cout << "[ERROR LUA]: " << error << std::endl;
			return false;
		}
		return true;
	}

	bool cScriptingManager::LoadFile(int ID, const std::string& FileName)
	{
		lua_State* L = GetLuaState(ID);
		if (!L)
		{
			return false;
		}
		int result = luaL_loadfile(L, FileName.c_str());
		if (result != LUA_OK)
		{
			const char* error = lua_tostring(L, -1);
			std::cout << "[ERROR LUA] " << error << std::endl;
			return false;
		}
		return true;
	}

	void cScriptingManager::RunFunction(int ID, const std::string& FuncName)
	{
		lua_State* L = GetLuaState(ID);
		if (!L)
		{
			return;
		}

		lua_getglobal(L, FuncName.c_str());
		if (!lua_isfunction(L, -1))
		{
			std::cout << "[ERROR LUA] " << FuncName << " is not a function!" << std::endl;
			lua_pop(L, 1);
			return;
		}
		lua_pcall(L, 0, 0, 0);
	}

	//TODO: Make it templated
	void cScriptingManager::RegisterFunction(int ID, const std::string& FuncName, std::function<void(float, float, float)> Func)
	{
		using namespace luabridge;
		lua_State* L = GetLuaState(ID);
		if (!L)
		{
			std::cout << "[ERROR] Couldn't Register Function, Lua State is undefined" << std::endl;
			return;
		}
		getGlobalNamespace(L).addFunction(FuncName.c_str(), Func);
	}

	//TODO: Make it templated
	void cScriptingManager::RegisterFunctionInNamespace(int ID, const std::string& Namespace, const std::string& FuncName, std::function<float()> Func)
	{
		using namespace luabridge;
		lua_State* L = GetLuaState(ID);
		if (!L)
		{
			std::cout << "[ERROR] Couldn't Register Function, Lua State is undefined" << std::endl;
			return;
		}
		getGlobalNamespace(L).beginNamespace(Namespace.c_str()).addFunction(FuncName.c_str(), Func);
	}
	

	void cScriptingManager::SpawnObject(const char* Name, int Type)
	{
		cObjectManager& ObjectMan = cObjectManager::get();
		ObjectMan.AddObject((eBasicObject)Type, Name);
	}

	cScriptingManager& cScriptingManager::get()
	{
		if (sScriptingInstance == nullptr)
		{
			sScriptingInstance = new cScriptingManager();
		}
		return *sScriptingInstance;
	}
}