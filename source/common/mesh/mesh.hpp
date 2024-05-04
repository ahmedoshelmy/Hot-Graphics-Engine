#pragma once

#include <glad/gl.h>
#include "vertex.hpp"
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
namespace our {

#define ATTRIB_LOC_POSITION 0
#define ATTRIB_LOC_COLOR    1
#define ATTRIB_LOC_TEXCOORD 2
#define ATTRIB_LOC_NORMAL   3

    class Mesh {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements 
        GLsizei elementCount;
        btConvexHullShape* collisionShape;

    public:
        glm::vec3 min, max; // They represent the minimum and maximum coordinates in the mesh

        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering 
        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &elements) {
            //TODO: (Req 2) Write this function
            // remember to store the number of elements in "elementCount" since you will need it for drawing
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc

            // Creating & Binding  Vertex Buffer
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);


            // Generating Element Buffer
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            elementCount = elements.size();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * elementCount, elements.data(), GL_STATIC_DRAW);

            //Creating Vertex Array
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            // START of Recording on the Vertex Array

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // It enables the attribute with index 0 (Position)
            glEnableVertexAttribArray(0);
            // Defining the attributes
            // 0 is because it's the first attribute
            // 3 here refers to that the vertex position is 3 numbers
            // GL_FLOAT since the position is 3 floats
            // False Since we do not want to normalize
            // Stride is defined as size of Vertex
            glVertexAttribPointer(ATTRIB_LOC_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);

            // Here normalize is set to true since color is byte (0 to 255) and the color should be from 0 to 1
            glVertexAttribPointer(ATTRIB_LOC_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *) offsetof(Vertex, color));
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);

            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tex_coord));
            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);

            glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            // It means we are ready now you can draw since we attached the Vertex Array to null
            glBindVertexArray(0);

            for (auto &vertix : vertices)
            {
                this->min.x = std::min(this->min.x, vertix.position.x);
                this->min.y = std::min(this->min.y, vertix.position.y);
                this->min.z = std::min(this->min.z, vertix.position.z);
                this->max.x = std::max(this->max.x, vertix.position.x);
                this->max.y = std::max(this->max.y, vertix.position.y);
                this->max.z = std::max(this->max.z, vertix.position.z);
            }

            collisionShape = new btConvexHullShape();
            for (auto &vertix : vertices) {
                collisionShape->addPoint(btVector3(vertix.position.x, vertix.position.y, vertix.position.z));
            }
        }

        // this function should render the mesh
        void draw() {
            //TODO: (Req 2) Write this function
            // It's necessary to bind the vertex array each time when drawing
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, (void *) 0);

        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh() {
            //TODO: (Req 2) Write this function
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteVertexArrays(1, &VAO);
            delete collisionShape;
        }

        

        Mesh(Mesh const &) = delete;

        Mesh &operator=(Mesh const &) = delete;
    };

}