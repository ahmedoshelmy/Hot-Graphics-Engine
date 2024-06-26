#pragma once

#include "../ecs/world.hpp"
#include "../components/movement.hpp"
#include "../components/camera.hpp"
#include "../components/ground-or-stairs.hpp"
#include "../components/mesh-renderer.hpp"
#include "components/free-camera-controller.hpp"
#include "components/fps-camera-controller.hpp"
#include "application.hpp"
#include "components/trigger.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <components/rigid-body.hpp>
#include <iostream>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

#include <systems/drawer-opener.hpp>
namespace our {
    struct PixelInfo {
        unsigned int ObjectID = 0;
        unsigned int DrawID = 0;
        unsigned int PrimID = 0;

        void Print()
        {
            printf("Object %d draw %d prim %d\n", ObjectID, DrawID, PrimID);
        }
    };
    struct bulletCollisionCallback : public btCollisionWorld::ContactResultCallback {
        unsigned int collided_id;

        float distance = 0.0;
		virtual btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
            collided_id = (size_t)colObj1Wrap->getCollisionObject()->getUserPointer();
            distance = cp.getDistance();
    		return 1.f;
        }
    };

    // The Collision system is responsible for detecting collision between objects
    class PhysicsSystem {
    public:
        // need for bullet integration
        btBroadphaseInterface* broadphase;
        // Set up the collision configuration and dispatcher
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        // The actual physics solver
        btSequentialImpulseConstraintSolver* solver;
        // The world.
        btDiscreteDynamicsWorld* dynamicsWorld;
        // store the rigid bodies of system 
        std::map<unsigned int, btRigidBody*> rigidBodies;
        std::map<unsigned int, std::string> mp_ids; // mesh id => entity name
        std::map<unsigned int, bool> isGroundOrStairs; // mesh id => is ground or not
        // BulletDebugDrawer_OpenGL* mydebugdrawer;
        // btKinematicCharacterController character;
        // Initialize the renderer 
        btCollisionObject *ghost;
        bulletCollisionCallback collisionCallback;
        // store info for avoid collision
        float closestDistance = 5.0f;
        float delta = 1.0f;
        float deltaReverseSpeed = 20.0f;
        int minusOrPlus = 1;
        float hit_fraction = 0.0f;
        float constDist = 4.0f;
        float factorDist = 10.0f;
        // store entity names of selected Items
        std::string cameraSelected, prevCameraSelected;
        std::string mouseSelected,  prevMouseSelected;

        // Objects for accelerated GPU picking
        GLuint castingFBO, castingVAO;
        Texture2D* primitiveCastingTarget, *depthCastingTarget;
        TexturedMaterial* castingMaterial;
        std::map<unsigned int, std::string> mapped_entities;
        // These window size will be used on multiple occasions picking
        glm::ivec2 windowSize;
    public:
    
        void initialize(World* world, glm::ivec2 windowSize);

        // This should be called every frame to update all entities containing a CollisionComponent.
        void update(World *world, Application* app, float deltaTime);

        void destroy();

        bool checkCollision(const glm::vec3 &box1_min, const glm::vec3 &box1_max,
                            const glm::vec3 &box2_min, const glm::vec3 &box2_max);


        bool checkCollisionByPosition(const glm::vec3 &position, const glm::vec3 &box_max, const glm::vec3 &box_min);

        static bool checkSpheresCollison(const glm::vec3 &center1, const glm::vec3 &center2, const int &radius1,
                                         const int &radius2);

        void reverseMovement(float deltaTime, Application* app, Entity * player, float deltaReverseSpeed = 20.0f);

        std::pair<glm::vec3, glm::vec3> getCollisionBox(Entity *entity);

        unsigned int  getCameraCollidedMesh(World *world, float deltaTime, float distance = 5.0f) ; // return mesh id that collided from camera within certain distance
        unsigned int  getMouseCollidedMesh(World *world, Application* app, float deltaTime, float distance = 1000.0f);
        unsigned int  getPersonCollidedMesh(World *world, float deltaTime) ; // return mesh id that person collided with
        unsigned int  allowMoveOnGround(World *world, float deltaTime, float distance = 5.0f);

        bool isOpenDoor(Entity *);

        void pickingPhaseRenderer(our::World *world, glm::ivec2 windowSize);

        void showGUI(World* world) {

            ImGui::Begin("Collision System");
            ImGui::Text("Ground Parameters");
            ImGui::InputFloat("floor closest distance", &closestDistance, 0.01, 0.02, 3, 0);
            ImGui::InputFloat("delta", &delta, 0.01, 0.02, 3, 0);
            ImGui::InputFloat("delta reverse speed", &deltaReverseSpeed, 0.01, 0.02, 3, 0);
            ImGui::InputFloat("constDist", &constDist, 0.01, 0.02, 3, 0);
            ImGui::InputFloat("factorDist", &factorDist, 0.01, 0.02, 3, 0);
            ImGui::InputInt("minus or plus", &minusOrPlus, 0.1, 0.2, 0);
            ImGui::End();
        }
    
    PixelInfo readPixel(unsigned int x, unsigned int y)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, castingFBO);

        glReadBuffer(GL_COLOR_ATTACHMENT0);

        PixelInfo Pixel;
        glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &Pixel);

        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        return Pixel;
    }

    private:
        void initCastingBuffer();
        void avoidCollision(World *world, Application* app, float deltaTime);
        void updatePicking(World *world, Application* app, float deltaTime);

    };

}   
