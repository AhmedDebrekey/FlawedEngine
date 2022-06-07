#include "Triangle.h"

namespace FlawedEngine
{
	cTriangle::cTriangle()
	{

		mVertexBuffer  = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		}; 
		
	}

	cTriangle::~cTriangle()
	{

	}

	void cTriangle::Render() 
	{
		//Rendering of a triangle
	}
}