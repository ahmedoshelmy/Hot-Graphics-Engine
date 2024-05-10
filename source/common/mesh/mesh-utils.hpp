#pragma once

#include "mesh.hpp"
#include <string>

namespace our::mesh_utils {
    // Load an ".obj" file into the mesh
    Mesh* loadOBJ(const std::string& filename);
    // Create a sphere (the vertex order in the triangles are CCW from the outside)
    // Segments define the number of divisions on the both the latitude and the longitude
    Mesh* sphere(const glm::ivec2& segments);

    // create a rectangle whose top-left corner is at the origin and its size is 1x1.
    // Note that the texture coordinates at the origin is (0.0, 1.0) since we will use the 
    // projection matrix to make the origin at the the top-left corner of the screen.
    Mesh* rectangle();
}