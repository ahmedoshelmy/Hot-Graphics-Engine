#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>


// #include "../components/camera.hpp"
// #include "../components/mesh-renderer.hpp"
// #include "../ecs/entity.hpp"

// class ShaderProgram;
#include "../shader/shader.hpp"
namespace our::physics_utils {
    // =========== functions ===============================
    /*
        - get motion state of entity (rotation / position)
        - set scaling of mesh component
        - this function use to transform rigid body dynamics from our system to bullet engine system
    */
    btDefaultMotionState* getMotionStateEntity(Entity* entity) {
        glm::mat3 transform =  entity->getLocalToWorldMatrix();
        MeshRendererComponent* meshComponent = entity->getComponent<MeshRendererComponent>();

        btVector3 btScaling = btVector3(entity->localTransform.scale.x, entity->localTransform.scale.y, entity->localTransform.scale.z);
        btVector3 btRotation = btVector3(entity->localTransform.rotation.x, entity->localTransform.rotation.y, entity->localTransform.rotation.z);
        btVector3 btPosition = btVector3(entity->localTransform.position.x, entity->localTransform.position.y, entity->localTransform.position.z);
        
        btQuaternion btRotationQuat;
        btRotationQuat.setEuler(btRotation.y(), btRotation.x(), btRotation.z());

        btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(btRotationQuat, btPosition));
        meshComponent->mesh->shape->setLocalScaling(btScaling);
        return motionstate;
    }

    /*
        - this function used to convert mouse coord from (viewPort space to world space)
        - should be used for interacting with mouse
    */
    glm::vec3 getMouseWorldSpace(glm::vec2 MouseCoord, CameraComponent *camera, glm::ivec2 windowSize) {
        glm::vec2 ray = MouseCoord;

        glm::vec2 ndc =   (ray / glm::vec2(windowSize));
                ndc = glm::vec2( 2.0f * ndc.x - 1.0f, 1.0f - 2.0f * ndc.y); // [-1, 1]
        // to homogoneouse clip (ray's z to point forwards)
        glm::vec4 ray_clip = glm::vec4(ndc, -1.0, 1.0);
        // to eye (camera) space (inverse of projection)
        glm::mat4 inv_proj = glm::inverse(camera->getProjectionMatrix(windowSize));
        glm::vec4 ray_eye = inv_proj * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
        // to world coordinates (finally) (inverse of view)
        glm::mat4 inv_view = glm::inverse(camera->getViewMatrix());
        glm::vec3 ray_world = glm::vec3(inv_view * ray_eye);
        // ray_world = glm::normalize(ray_world);
        return ray_world;
    }



    /*
        this class should be used for debugging bullet
        TODO: fix it
    */
    class BulletDebugDrawer_OpenGL : public btIDebugDraw {
        GLuint VBO, VAO;
        ShaderProgram* lineShader;
    public:
        BulletDebugDrawer_OpenGL() {
            lineShader = new ShaderProgram();
            lineShader->attach("assets/shaders/bullet_d.vert", GL_VERTEX_SHADER);
            lineShader->attach("assets/shaders/bullet_d.frag", GL_FRAGMENT_SHADER);
            lineShader->link();
        }
        ~BulletDebugDrawer_OpenGL() {
            delete lineShader;
        }
        void SetMatrices(glm::mat4 pViewMatrix, glm::mat4 pProjectionMatrix) 
        {
            // Vertex data
            lineShader->use();
            lineShader->set("projection", pProjectionMatrix);
            lineShader->set("view", pViewMatrix);
        }

        virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) 
        {
            // std::cout << from.x() << " " << from.y() << " " << from.z() << " \n";
            // std::cout << to.x() << " " << to.y() << " " << to.z() << " \n";
            lineShader->use();
            GLfloat points[12];

            points[0] = from.x();
            points[1] = from.y();
            points[2] = from.z();
            points[3] = color.x();
            points[4] = color.y();
            points[5] = color.z();

            points[6] = to.x();
            points[7] = to.y();
            points[8] = to.z();
            points[9] = color.x();
            points[10] = color.y();
            points[11] = color.z();

            glDeleteBuffers(1, &VBO);
            glDeleteVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
            glBindVertexArray(0);

            glBindVertexArray(VAO);
            glDrawArrays(GL_LINES, 0, 2);
            glBindVertexArray(0);

        }
        virtual void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) {}
        virtual void reportErrorWarning(const char *) {}
        virtual void draw3dText(const btVector3 &, const char *) {}
        virtual void setDebugMode(int p) {
            m = p;
        }
        int getDebugMode(void) const { return 3; }
        int m;
    };
    
}