#include "Physics.h"
#include <GLFW/glfw3.h>

namespace FlawedEngine
{
	cPhysics::cPhysics()
	{
	}
	cPhysics::~cPhysics()
	{
		for (int i = mWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = mWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			mWorld->removeCollisionObject(obj);
			delete obj;
		}

		for (int j = 0; j < collisionShapes.size(); j++)
		{
			btCollisionShape* shape = collisionShapes[j];
			collisionShapes[j] = 0;
			delete shape;
		}

		delete mWorld;
		delete mSolver;
		delete mOverlappingPairCache;
		delete mDispatcher;
		delete mCollisionConfiguration;
		collisionShapes.clear();
	}

	void cPhysics::Setup()
	{
		mCollisionConfiguration = new btDefaultCollisionConfiguration();
		mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
		mOverlappingPairCache = new btDbvtBroadphase();
		mSolver = new btSequentialImpulseConstraintSolver;
		mWorld = new btDiscreteDynamicsWorld(mDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration);
		mWorld->setGravity(btVector3(0.0, -0.5, 0));
	}

	void cPhysics::Update()
	{
		if(isPlaying)
			mWorld->stepSimulation(1.f / 60.f, 10);

	}

	std::shared_ptr<btRigidBody> cPhysics::AddBox(eBasicObject Object, sPhysicsProps& Props)
	{
		std::shared_ptr<btCollisionShape> mCollisionShape = SetCollisionShape(Object);

		glm::vec3 Trans = Props.Transformation.Translation;
		glm::vec3 Rotation = Props.Transformation.Rotation;
		glm::vec3 Scale = Props.Transformation.Scale;

		btTransform ObjectTransform;
		ObjectTransform.setIdentity();
		ObjectTransform.setOrigin(btVector3(Trans.x, Trans.y, Trans.z));

		btScalar mass = Props.Mass;

		btVector3 localInertia(0, 0, 0);
		if (mass != 0.f) mCollisionShape->calculateLocalInertia(mass, localInertia);

		btDefaultMotionState* MotionState = new btDefaultMotionState(ObjectTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, MotionState, mCollisionShape.get(), localInertia);
		rbInfo.m_startWorldTransform;
		std::shared_ptr<btRigidBody> RigidBody = std::make_shared<btRigidBody>(rbInfo);

		RigidBody->setFriction(Props.Friction);
		RigidBody->setRestitution(Props.Restitution);

		btTransform trans = RigidBody->getCenterOfMassTransform();
		btQuaternion transrot = trans.getRotation();
		btQuaternion rotquat;
		rotquat = rotquat.getIdentity();
		rotquat.setEuler(glm::radians(Rotation.y), glm::radians(Rotation.x), glm::radians(Rotation.z));
		transrot = rotquat * transrot;
		trans.setRotation(transrot);
		RigidBody->setCenterOfMassTransform(trans);

		mCollisionShape->setLocalScaling(btVector3(btScalar(Scale.x), btScalar(Scale.y), btScalar(Scale.z)));

		RigidBody->setActivationState(DISABLE_DEACTIVATION); //not so good 

		mWorld->addRigidBody(RigidBody.get());
		mWorld->updateSingleAabb(RigidBody.get());

		return RigidBody;
	}

	std::shared_ptr<btRigidBody> cPhysics::AddSphere(eBasicObject Object, sPhysicsProps& Props)
	{
		std::shared_ptr<btCollisionShape> mCollisionShape = SetCollisionShape(Object);

		glm::vec3 Trans = Props.Transformation.Translation;
		glm::vec3 Rotation = Props.Transformation.Rotation;
		glm::vec3 Scale = Props.Transformation.Scale;

		SetCollisionShape(Object);

		btTransform ObjectTransform;
		ObjectTransform.setIdentity();
		ObjectTransform.setOrigin(btVector3(Trans.x, Trans.y, Trans.z));

		btScalar mass = Props.Mass;

		btVector3 localInertia(0, 0, 0);
		if (mass != 0.f) mCollisionShape->calculateLocalInertia(mass, localInertia);

		btDefaultMotionState* MotionState = new btDefaultMotionState(ObjectTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, MotionState, mCollisionShape.get(), localInertia);
		rbInfo.m_startWorldTransform;
		std::shared_ptr<btRigidBody> RigidBody = std::make_shared<btRigidBody>(rbInfo);

		RigidBody->setFriction(Props.Friction);
		RigidBody->setRestitution(Props.Restitution);

		btTransform trans = RigidBody->getCenterOfMassTransform();
		btQuaternion transrot = trans.getRotation();
		btQuaternion rotquat;
		rotquat = rotquat.getIdentity();
		rotquat.setEuler(glm::radians(Rotation.y), glm::radians(Rotation.x), glm::radians(Rotation.z));
		transrot = rotquat * transrot;
		trans.setRotation(transrot);
		RigidBody->setCenterOfMassTransform(trans);

		mCollisionShape->setLocalScaling(btVector3(btScalar(Scale.x), btScalar(Scale.y), btScalar(Scale.z)));

		RigidBody->setActivationState(DISABLE_DEACTIVATION); //not so good 

		mWorld->addRigidBody(RigidBody.get());
		mWorld->updateSingleAabb(RigidBody.get());

		return RigidBody;
	}

	std::shared_ptr<btCollisionShape> cPhysics::SetCollisionShape(eBasicObject Object)
	{
		switch (Object) //if it is not set code will crash ofc
		{
		case FlawedEngine::Cube:
		{
			return std::make_shared<btBoxShape>(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
		}
		break;
		case FlawedEngine::Sphere:
		{
			return std::make_shared<btSphereShape>(1.0);
		}
		break;
		case FlawedEngine::Cone:
			break;
		case FlawedEngine::Torus:
			break;
		case FlawedEngine::Triangle:
			break;
		case FlawedEngine::PointLight:
			break;
		case FlawedEngine::SpotLight:
			break;
		default:
			break;
		}
	}
}