#ifndef PLAYER_H__
#define PLAYER_H__
#include "vector3.h"
#include "transformation.h"
class Player {

public:
	Player(const Vector3f& position, float rotX = 0, float rotY = 0);
	void TurnLeftRight(float value);
	void TurnTopBottom(float value);
	Vector3f SimulateMove(bool front, bool back, bool left, bool right, bool sprint, float elapsedTime);
	void ApplyTransformation(Transformation& transformation) const;
	void SetPosition(Vector3f Position);
	Vector3f Position();

private:
	float m_rotX = 0.f;
	float m_rotY = 0.f;
	Vector3f m_position;
};


#endif