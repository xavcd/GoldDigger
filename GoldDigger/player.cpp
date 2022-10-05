#include "player.h"

Player::Player(const Vector3f& position, float rotX, float rotY) : m_position(position), m_rotX(rotX), m_rotY(rotY)
{

}

void Player::TurnLeftRight(float value)
{
	m_rotY += value;
}

void Player::TurnTopBottom(float value)
{
	if (m_rotX <= 90 && m_rotX >= -90)
		m_rotX += value;
	else if (m_rotX > 90)
		m_rotX = 90;
	else if (m_rotX < -90)
		m_rotX = -90;
}

void Player::Move(bool front, bool back, bool left, bool right, float elapsedTime)
{
	if (front)
	{
		float xrotrad, yrotrad;
		yrotrad = (m_rotY / 180 * 3.141592654f);
		xrotrad = (m_rotX / 180 * 3.141592654f);
		m_position.x += float(sin(yrotrad)) * elapsedTime * 20;
		m_position.z -= float(cos(yrotrad)) * elapsedTime * 20;
	}

	if (back)
	{
		float xrotrad, yrotrad;
		yrotrad = (m_rotY / 180 * 3.141592654f);
		xrotrad = (m_rotX / 180 * 3.141592654f);
		m_position.x -= float(sin(yrotrad)) * elapsedTime * 20;
		m_position.z += float(cos(yrotrad)) * elapsedTime * 20;
	}

	if (right)
	{
		float yrotrad;
		yrotrad = (m_rotY / 180 * 3.141592654f);
		m_position.x += float(cos(yrotrad)) * elapsedTime * 15;
		m_position.z += float(sin(yrotrad)) * elapsedTime * 15;
	}

	if (left)
	{
		float yrotrad;
		yrotrad = (m_rotY / 180 * 3.141592654f);
		m_position.x -= float(cos(yrotrad)) * elapsedTime * 15;
		m_position.z -= float(sin(yrotrad)) * elapsedTime * 15;
	}


}

void Player::ApplyTransformation(Transformation& transformation) const
{
	transformation.ApplyRotation(-m_rotX, 1.f, 0, 0);
	transformation.ApplyRotation(-m_rotY, 0, 1.f, 0);
	transformation.ApplyTranslation(-m_position);

}
