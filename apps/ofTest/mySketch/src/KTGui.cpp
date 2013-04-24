#include "KTGui.h"

KTGui::KTGui()
{	
	// raise & lower arrow
  SimpleButton _reset("reset.jpg", ofVec2f(20.0f,ofGetHeight()-60), ofVec2f(45.0f,40.0f));
	_reset.m_type = KT_RESET;
  SimpleButton _export("save.jpg", ofVec2f(75.0f,ofGetHeight()-60), ofVec2f(45.0f,40.0f));
	_export.m_type = KT_EXPORT;

	m_buttons.push_back(_reset);
  m_buttons.push_back(_export);
}

void KTGui::Draw()
{
	unsigned int t_size = m_buttons.size();
	unsigned int index = 0;
	
	for(index; index < t_size; index++)
	{
		SimpleButton *button = &m_buttons[index];
		button->setHeight(ofGetHeight()-60);
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