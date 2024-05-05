#pragma once

#include <glad/gl.h>
#include "vertex.hpp"
#include <btBulletCollisionCommon.h>
#include<atomic>
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
    protected:
    public:
        static std::atomic<unsigned int>  ID;
        unsigned int id;
        // btCollisionShape* collisionShape;
        btTriangleIndexVertexArray* data;
        btBvhTriangleMeshShape* shape;
        glm::vec3 min, max; // They represent the minimum and maximum coordinates in the mesh

        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering 
        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &elements)  {
            id = ++Mesh::ID;
            // ID++;
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

            // collisionShape = new btConvexHullShape();
            // for (auto &i : elements) {
            //     collisionShape->addPoint(btVector3(vertices[i].position.x, vertices[i].position.y, vertices[i].position.z));
            // }

            // first create a btTriangleIndexVertexArray
            // NOTE: we must track this pointer and delete it when all shapes are done with it!
            data = new btTriangleIndexVertexArray;
            // add an empty mesh (data makes a copy)
            btIndexedMesh tempMesh;
            data->addIndexedMesh(tempMesh, PHY_FLOAT);
            // get a reference to internal copy of the empty mesh
            btIndexedMesh& mesh = data->getIndexedMeshArray()[0];
            // allocate memory for the mesh
            mesh.m_numTriangles = elementCount / 3;
            if (elementCount < std::numeric_limits<int16_t>::max()) {
                // we can use 16-bit indices
                mesh.m_triangleIndexBase = new unsigned char[sizeof(int16_t) * (size_t)elementCount];
                mesh.m_indexType = PHY_SHORT;
                mesh.m_triangleIndexStride = 3 * sizeof(int16_t);
            } else {
                // we need 32-bit indices
                mesh.m_triangleIndexBase = new unsigned char[sizeof(int32_t) * (size_t)elementCount];
                mesh.m_indexType = PHY_INTEGER;
                mesh.m_triangleIndexStride = 3 * sizeof(int32_t);
            }
            mesh.m_numVertices = vertices.size();
            mesh.m_vertexBase = new unsigned char[3 * sizeof(btScalar) * (size_t)mesh.m_numVertices];
            mesh.m_vertexStride = 3 * sizeof(btScalar);

            // copy vertices into mesh
            btScalar* vertexData = static_cast<btScalar*>((void*)(mesh.m_vertexBase));
            for (int32_t i = 0; i < mesh.m_numVertices; ++i) {
                int32_t j = i * 3;
                const glm::vec3& point = vertices[i].position;
                vertexData[j] = point.x;
                vertexData[j + 1] = point.y;
                vertexData[j + 2] = point.z;
            }
            // copy indices into mesh
            if (elementCount < std::numeric_limits<int16_t>::max()) {
                // 16-bit case
                int16_t* indices = static_cast<int16_t*>((void*)(mesh.m_triangleIndexBase));
                for (int32_t i = 0; i < elementCount; ++i) {
                    indices[i] = (int16_t)elements[i];
                }
            } else {
                // 32-bit case
                int32_t* indices = static_cast<int32_t*>((void*)(mesh.m_triangleIndexBase));
                for (int32_t i = 0; i < elementCount; ++i) {
                    indices[i] = elements[i];
                }
            }

            // create the shape
            // NOTE: we must track this pointer and delete it when all btCollisionObjects that use it are done with it!
            const bool USE_QUANTIZED_AABB_COMPRESSION = true;
            shape = new btBvhTriangleMeshShape(data, USE_QUANTIZED_AABB_COMPRESSION);

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
            delete data;
            delete shape;
        }

        

        Mesh(Mesh const &) = delete;

        Mesh &operator=(Mesh const &) = delete;
    };

};
