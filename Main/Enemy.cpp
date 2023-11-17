#include "Enemy.h"
#include "Time.h"

Enemy::Enemy(const Vector2 position, const Vector2* characterPosition, float speed, float lifeSpan) : position(position), characterPosition(characterPosition), speed(speed), lifeSpan(lifeSpan) { }

bool Enemy::TrackingCharacter()
{
	Vector2 direction((*characterPosition - position).Normalized());

	double deltaTime = Time::GetDeltaTime();

	position.x += direction.x * speed * deltaTime;
	position.y += direction.y * speed * deltaTime;
	lifeSpanTimer += deltaTime;

	return lifeSpanTimer < lifeSpan;
}

Vector2* Enemy::GetPosition()
{
	return &position;
}

float Enemy::GetSpeed()
{
	return speed;
}

float Enemy::GetLifeSpan()
{
	return lifeSpan;
}

void Enemy::SetTarget(const Vector2* target)
{
	characterPosition = target;
}