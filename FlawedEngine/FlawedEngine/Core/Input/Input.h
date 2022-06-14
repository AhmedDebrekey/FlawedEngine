#pragma once


namespace FlawedEngine
{
	class cInput
	{
	public:
		void Poll(void* mWindow);
		void SetKeyPressed(int Key);
		int GetKeyPressed();
	private:
		int mKeyPressed; //GLFW_KEY_?
	//I am not sure as how this api should be tbh :(
	public:
		~cInput();
		static cInput& get();
		cInput(cInput const&) = delete;
		void operator=(cInput const&) = delete;
	private:
		static cInput* sInputInstance;
		cInput();
		cInput(cInput&);
	};
}