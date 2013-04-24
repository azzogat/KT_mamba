#include "ofVec2f.h"
#include "ofVbo.h"
#include "ofRectangle.h"

// debug graphics
#include "ofGraphics.h"
#include "ofImage.h"


enum KT_PRESSED
{
	KT_NONE,
	KT_RESET,
  KT_EXPORT
};

class KTGui 
{
	struct SimpleButton 
	{
		ofRectangle m_boundingBox;		// bounding box of rectangle
		ofImage		m_texture;			// button
		KT_PRESSED	m_type;				// button type

		SimpleButton( char* _texture, ofVec2f& _pos, ofVec2f& _size)
		{
			m_texture.loadImage(std::string(_texture));
			m_boundingBox.x			= _pos.x;
			m_boundingBox.y			= _pos.y;
			m_boundingBox.width		= _size.x;
			m_boundingBox.height	= _size.y;
		}
	};

	ofVbo						m_guiVBO;
	std::vector<SimpleButton>	m_buttons;
	ofImage						m_imageHandler;
public:
	/// ctor.initialize buttons here
	KTGui();			
	/// draw the simple gui
	void Draw();									
	/// returns the button currently pressed or KT_NONE
	KT_PRESSED GetAtPoint(unsigned int _index, const ofVec2f& _pos);	
};