#include "KTGui.h"

KTGui::KTGui()
{	
	// raise & lower arrow
	SimpleButton _raise("arrow.jpg", ofVec2f(30.0f,30.0f), ofVec2f(120.0f,120.0f));
	_raise.m_type = KT_RAISE;
	SimpleButton _lower("arrow.jpg", ofVec2f(30.0f,_raise.m_boundingBox.height+_raise.m_boundingBox.x+30.0f), ofVec2f(120.0f,120.0f));
	_lower.m_type = KT_LOWER;
	_lower.m_texture.rotate90(2);

	m_buttons.push_back(_raise);
	m_buttons.push_back(_lower);
}

void KTGui::Draw()
{
	unsigned int t_size = m_buttons.size();
	unsigned int index = 0;
	
	for(index; index < t_size; index++)
	{
		SimpleButton *button = &m_buttons[index];
		button->m_texture.draw( button->m_boundingBox);
	}
}

KT_PRESSED KTGui::GetAtPoint(unsigned int _index, const ofVec2f& _pos)
{
	unsigned int t_size = m_buttons.size();
	unsigned int index = 0;
	for(index; index < t_size; index++)
	{
		SimpleButton *button = &m_buttons[index];
		if(button->m_boundingBox.inside(_pos.x, _pos.y))
		{
			return button->m_type;
		}
	}
	return KT_NONE;
}