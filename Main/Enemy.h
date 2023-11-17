#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Vector2.h"

class Enemy
{
private:
	Vector2 position;
	const Vector2* characterPosition;

	float speed;
	float lifeSpanTimer = 0.0f;
	float lifeSpan;

public:
	Enemy(const Vector2 position, const Vector2* characterPosition, float speed, float lifeSpan);

	bool TrackingCharacter();

	// Getters
	Vector2* GetPosition();
	float GetSpeed();
	float GetLifeSpan();

	// Setters
	void SetTarget(const Vector2* target);
};

#endif