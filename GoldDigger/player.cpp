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
	if (m_rotX + value < 90 && m_rotX + value > -90)
		m_rotX += value;
	else if (m_rotX + value >= 90)
		m_rotX = 90;
	else if (m_rotX + value <= -90)
		m_rotX = -90;
}

Vector3f Player::SimulateMove(bool front, bool back, bool left, bool right, bool up, float elapsedTime)
{
	Vector3f delta(0, 0, 0);
	if (front)
	{
		float xrotrad, yrotrad;
		yrotrad = (m_rotY / 180 * 3.141592654f);
		xrotrad = (m_rotX / 180 * 3.141592654f);
		delta.x += float(sin(yrotrad)) * elapsedTime * 10;
		delta.z -= float(cos(yrotrad)) * elapsedTime * 10;
	}

	if (back)
	{
		float xrotrad, yrotrad;
		yrotrad = (m_rotY / 180 * 3.141592654f);
		xrotrad = (m_rotX / 180 * 3.141592654f);
		delta.x -= float(sin(yrotrad)) * elapsedTime * 10;
		delta.z += float(cos(yrotrad)) * elapsedTime * 10;
	}

	if (right)
	{
		float yrotrad;
		yrotrad = (m_rotY / 180 * 3.141592654f);
		delta.x += float(cos(yrotrad)) * elapsedTime * 7;
		delta.z += float(sin(yrotrad)) * elapsedTime * 7;
	}

	if (left)
	{
		float yrotrad;
		yrotrad = (m_rotY / 180 * 3.141592654f);
		delta.x -= float(cos(yrotrad)) * elapsedTime * 7;
		delta.z -= float(sin(yrotrad)) * elapsedTime * 7;
	}

	if (up)
	{
	}

	return delta;
}

void Player::ApplyTransformation(Transformation& transformation) const
{
	transformation.ApplyRotation(-m_rotX, 1.f, 0, 0);
	transformation.ApplyRotation(-m_rotY, 0, 1.f, 0);
	transformation.ApplyTranslation(-m_position);

}

void Player::SetPosition(Vector3f position)
{
	m_position = position;
}

Vector3f Player::Position()
{
	return m_position;
}