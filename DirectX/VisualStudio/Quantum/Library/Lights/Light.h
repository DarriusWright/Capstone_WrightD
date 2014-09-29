#pragma once
#include <Misc\Common.h>
#include <Components\GameComponent.h>
namespace Library
{
	
	class Light : public GameComponent
	{
		RTTI_DECLARATIONS(Light, GameComponent);
	public:
		Light(Game & game);
		~Light();

		const XMCOLOR & color()const;
		const XMVECTOR & colorVector()const;
		void setColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a);
		void setColor(XMCOLOR color);
		void setColor(FXMVECTOR color);


	private:
		XMCOLOR mColor;

	};

}