#pragma once
#ifndef JUMP_H
#define JUMP_H

#include <UserComponent.h>

class RigidBody;

class Jump : public UserComponent
{
private:
	RigidBody* rigidBody;
	GameObject* parent;

	float jumpForce;	// Force used to add impulse
	float jumpDecay;	// Rate of attenuation when jump is cancelled
	float coyoteTime;	// Extra time when leaving a platform so jumping is still able
	float coyoteTimer;

	int playersBelow;	// Used as a boolean, count number of players the sensor is colliding with
	bool grounded;		// Only true when sensor detects collision with floor
	bool jumping;
	int landed;
	int jumped;

protected:
	virtual void start();
	virtual void update(float deltaTime);
	virtual void postUpdate(float deltaTime);
	virtual void onObjectEnter(GameObject* other);
	virtual void onObjectExit(GameObject* other);
	virtual void handleData(ComponentData* data);

public:
	Jump(GameObject* gameObject);
	virtual ~Jump();

	void jump();
	void cancelJump();

	void setJumpForce(float force);
	void setCoyoteTime(float time);

	bool isAbovePlayer() const;
	bool isGrounded() const;
	bool isJumping() const;
	bool isFalling() const;
	bool canJump() const;

	bool hasLanded() const;
	bool hasJumped() const;
};

#endif