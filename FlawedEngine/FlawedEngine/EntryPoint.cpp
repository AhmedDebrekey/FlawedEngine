#include <iostream>

#include "Core/Window.h"

int main()
{
	FlawedEngine::cWindow& EngineWindow = FlawedEngine::cWindow::get();
	EngineWindow.Init();
	EngineWindow.CreateWindow();

	while (!EngineWindow.ShouldClose())
	{
		EngineWindow.Update();
	}
}