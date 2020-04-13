#include "PlayerAnimController.h"

#include <queue>
#include <sstream>
#include <ComponentRegister.h>
#include <GameObject.h>

#include <InputSystem.h>
#include "Animator.h"

#include "Attack.h"
#include "Jump.h"
#include "Health.h"
#include "GhostManager.h"
#include "Dodge.h"
#include "UltimateGhostPunch.h"
#include "Grab.h"
#include "Block.h"
#include "GameManager.h"
#include "RigidBody.h"
#include "PlayerController.h"


REGISTER_FACTORY(PlayerAnimController);

PlayerAnimController::PlayerAnimController(GameObject* gameObject) : UserComponent(gameObject), inputSystem(nullptr), body(nullptr), anim(nullptr), jump(nullptr), grab(nullptr), block(nullptr), state(IDLE), runThreshold(0.50f), fallThreshold(1.0f)
{
}

void PlayerAnimController::start()
{
	inputSystem = InputSystem::GetInstance();
	anim = gameObject->getComponent<Animator>();
	body = gameObject->getComponent<RigidBody>();
	std::vector<GameObject*> aux = gameObject->findChildrenWithTag("groundSensor");
	if (aux.size() > 0)
	{
		jump = aux[0]->getComponent<Jump>();
		block = aux[0]->getComponent<Block>();
	}
	aux = gameObject->findChildrenWithTag("grabSensor");
	if (aux.size() > 0) grab = aux[0]->getComponent<Grab>();

	if (anim == nullptr)
		LOG("ERROR: Animator component not found in player.\n");
	else
		anim->printAllAnimationsNames();
}

void PlayerAnimController::update(float deltaTime)
{
	// Update the current state
	updateState();

	// Handle the current state
	handleState();

	anim->updateAnimationSequence();
}

/******************************
	ANIMATION METHODS
*******************************/

void PlayerAnimController::jumpAnimation() //  JUMP ANIMATION //
{
	if (anim->getCurrentAnimation() == "JumpStart" || anim->getCurrentAnimation() == "JumpChange" || anim->getCurrentAnimation() == "Fall")
		return;

	anim->playAnimation("JumpStart");
	anim->setLoop(false);
	state = JUMP;
}

void PlayerAnimController::hurtAnimation() //  HURT ANIMATION //
{
	anim->playAnimation("Hurt");
	anim->setLoop(false);
	state = NOT_LOOPING_STATE;
}

void PlayerAnimController::grabAnimation() //  GRAB ANIMATION //
{
	if (anim->getCurrentAnimation() == "GrabStart" || anim->getCurrentAnimation() == "GrabHold" || anim->getCurrentAnimation() == "GrabFailed")
		return;

	anim->playAnimation("GrabStart");
	anim->setLoop(false);
	state = GRABBING;
}

void PlayerAnimController::quickAttackAnimation() //  QUICK ATTACK ANIMATION //
{
	attackAnimation(0);
}

void PlayerAnimController::strongAttackAnimation() //  STRONG ATTACK ANIMATION //
{
	attackAnimation(1);
}

void PlayerAnimController::blockAnimation() //  BLOCK ANIMATION //
{
	if (anim->getCurrentAnimation() == "BlockStart" || anim->getCurrentAnimation() == "BlockHold")
		return;

	anim->playAnimationSequence({ "BlockStart", "BlockHold" }, true);
	anim->setLoop(false);
	state = BLOCKING;
}

void PlayerAnimController::blockedAttackAnimation()
{
	anim->playAnimation("BlockAttack");
	anim->setLoop(false);
	state = NOT_LOOPING_STATE;
}

void PlayerAnimController::blockedEnemyGrabAnimation()
{
	anim->playAnimation("Knockback");
	anim->setLoop(false);
	state = NOT_LOOPING_STATE;
}

void PlayerAnimController::enemyBlockedMyGrabAnimation()
{
	if (anim->getCurrentAnimation() != "GrabStart")
		anim->playAnimation("GrabStart");
	anim->setLoop(false);
	state = STUNNED;
}

void PlayerAnimController::stunnedAnimation()
{
	if (anim->getCurrentAnimation() == "StunnedStart" || anim->getCurrentAnimation() == "StunnedHold")
		return;

	anim->playAnimationSequence({ "StunnedStart", "StunnedHold" }, true);
	anim->setLoop(false);
	state = STUNNED;
}

void PlayerAnimController::dashAnimation()
{
	anim->playAnimation("DashFront");
	anim->setLoop(false);
	state = NOT_LOOPING_STATE;
}

void PlayerAnimController::attackAnimation(int type) //  ATTACK ANIMATION //
{
	if (type == 0)
		anim->playAnimation("AttackA");
	else
		anim->playAnimation("AttackB");

	anim->setLoop(false);
	state = NOT_LOOPING_STATE;
}

/******************************
	STATE CHANGES
*******************************/

void PlayerAnimController::updateState()
{
	switch (state)
	{
	case PlayerAnimController::IDLE:				// IDLE //
		updateIdle();
		break;
	case PlayerAnimController::RUN:					// RUN //
		updateRun();
		break;
	case PlayerAnimController::JUMP:				// JUMP //
		updateJump();
		break;
	case PlayerAnimController::FALL:				// FALL //
		updateFall();
		break;
	case PlayerAnimController::BLOCKING:			// BLOCKING // 
		updateBlocking();
		break;
	case PlayerAnimController::GRABBING:			//  GRABBING //
		updateGrabbing();
		break;
	case PlayerAnimController::GRABBED:				// GRABBED //
		break;
	case PlayerAnimController::STUNNED:				// STUNNED //
		updateStunned();
		break;
	case PlayerAnimController::NOT_LOOPING_STATE:	// NOT_LOOPING_STATE //
		updateNotLoopingState();
		break;
	default:
		break;
	}
}


void PlayerAnimController::updateIdle()	//  IDLE //
{
	// TRANSITION TO FALL IF
	if (!jump->isGrounded())
	{
		state = FALL;
		return;
	}

	// TRANSITION TO RUN IF
	if (abs(body->getLinearVelocity().x) >= runThreshold)
	{
		state = RUN;
		return;
	}

}

void PlayerAnimController::updateRun() //  RUN //
{
	// TRANSITION TO FALL IF
	if (!jump->isGrounded())
	{
		state = FALL;
		return;
	}

	// TRANSITION TO IDLE IF
	if (abs(body->getLinearVelocity().x) < runThreshold)
	{
		state = IDLE;
		return;
	}
}

void PlayerAnimController::updateJump() //  JUMP //
{
	if (anim->getCurrentAnimation() == "JumpStart" && anim->hasEnded() && abs(body->getLinearVelocity().y) <= fallThreshold)
	{
		anim->playAnimationSequence({ "JumpChange", "Fall" }, true);
		anim->setLoop(false);
		return;
	}

	if (anim->getCurrentAnimation() == "Fall") // Player is falling: transition to Fall
		state = FALL;
}

void PlayerAnimController::updateFall() //  FALL //
{
	if (!jump->isGrounded())
		return;

	// If isGrounded, the player has landed
	anim->playAnimation("Land");
	anim->setLoop(false);
	state = NOT_LOOPING_STATE;
}

void PlayerAnimController::updateGrabbing() //  GRABBING //
{
	if (anim->getCurrentAnimation() == "GrabStart" && anim->hasEnded())
	{
		if (grab->isGrabbing())
		{
			anim->playAnimation("GrabHold");
			anim->setLoop(true);
		}
		else
		{
			anim->playAnimation("GrabFail");
			anim->setLoop(false);
		}
		return;
	}

	if ((anim->getCurrentAnimation() == "GrabHold" && !grab->isGrabbing())
		|| anim->getCurrentAnimation() == "GrabFailed" && anim->hasEnded())
	{
		state = IDLE;
		updateIdle();
	}
}

void PlayerAnimController::updateBlocking() //  BLOCKING //
{
	if (anim->getCurrentAnimation() == "BlockHold" && !block->blocking())
	{
		anim->playAnimation("BlockEnd");
		anim->setLoop(false);
		return;
	}

	if (anim->getCurrentAnimation() == "BlockEnd" && anim->hasEnded() )
	{
		state = IDLE;
		updateIdle();
		return;
	}
}

void PlayerAnimController::updateStunned()
{
	if (anim->getCurrentAnimation() == "GrabStart" && anim->hasEnded())
	{
		stunnedAnimation();
		return;
	}

	if (anim->getCurrentAnimation() == "StunnedHold")
	{
		bool active = true;
		PlayerController* controller = gameObject->getComponent<PlayerController>();
		if (controller != nullptr) active = controller->isActive();
		if (!active) return;
		anim->playAnimation("StunnedEnd");
		anim->setLoop(false);
		return;
	}

	if (anim->getCurrentAnimation() == "StunnedEnd" && anim->hasEnded())
	{
		state = IDLE;
		updateIdle();
		return;
	}
}

void PlayerAnimController::updateNotLoopingState()
{
	// Only transition when the animation has finished
	if (!anim->getLoop() && !anim->hasEnded())
		return;

	// TRANSITION TO FALL IF
	if (!jump->isGrounded())
	{
		state = FALL;
		return;
	}

	// TRANSITION TO IDLE IF
	if (abs(body->getLinearVelocity().x) < runThreshold)
	{
		state = IDLE;
		return;
	}
	else
	{
		// TRANSITION TO RUN
		state = RUN;
		return;
	}
}

/******************************
	STATE HANDLING
*******************************/

void PlayerAnimController::handleState()
{
	switch (state)
	{
	case PlayerAnimController::IDLE:				// IDLE //
		if (anim->getCurrentAnimation() != "Idle")
		{
			anim->playAnimation("Idle");
			anim->setLoop(true);
		}
		break;
	case PlayerAnimController::RUN:					// RUN //
		if (anim->getCurrentAnimation() != "Run")
		{
			anim->playAnimation("Run");
			anim->setLoop(true);
		}
		break;
	case PlayerAnimController::FALL:					// FALL //
		if (anim->getCurrentAnimation() != "Fall")
		{
			anim->playAnimation("Fall");
			anim->setLoop(true);
		}
		break;
	default:
		break;
	}
}


