#pragma once

#include "../Entity.h"

namespace FlawedEngine
{
	class cTriangle : public cEntity
	{
	public:
		cTriangle();
		virtual ~cTriangle() override;

		virtual void Render() override;

	};
}
