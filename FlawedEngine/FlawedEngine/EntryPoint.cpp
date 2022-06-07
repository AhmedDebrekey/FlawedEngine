#include "Core/Engine.h"

int main()
{
	std::unique_ptr<FlawedEngine::cEngine> Engine = std::make_unique<FlawedEngine::cEngine>();
	Engine->Run();
}