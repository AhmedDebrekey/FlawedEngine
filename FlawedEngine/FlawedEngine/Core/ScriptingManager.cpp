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
		int id = mNextStateId++;
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);
		mStates[id] = L;

		std::function<void(const char*, int)> SpawnObjectFn = std::bind(&cScriptingManager::SpawnObject, this, std::placeholders::_1, std::placeholders::_2);
		using namespace luabridge;
		getGlobalNamespace(L)
			.addFunction("SpawnObject", SpawnObjectFn);
		return id;
	}

	lua_State* cScriptingManager::GetLuaState(int id)
	{
		auto State = mStates.find(id);
		return (State != mStates.end()) ? State->second : nullptr;
	}

	void cScriptingManager::DestroyState(int id)
	{
		auto State = GetLuaState(id);
		if (State)
		{
			lua_close(State);
			mStates.erase(id);
		}
	}

	bool cScriptingManager::RunFile(int id, const std::string& FileName)
	{
		lua_State* L = GetLuaState(id);
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