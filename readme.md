## Game Controls :
**Movement:**
-   Use WASD keys to control movement:
    -   W: Forward
    -   A: Left
    -   S: Backward
    -   D: Right
        **Inventory:**

-   Right-click:
    -   Pick up objects and add them to your inventory.
    -   (Optional) If already holding an object, it will be automatically added to the inventory when picking up a new one. **
    
**Inventory**
  - I: Show inventory
  - ESC: Hide inventory

**Interaction:**
-   Right-click: Open doors if cursor is pointing to them 

# Hot Graphics Engine Parts 

## Physics 
### The Physics system includes several features including the following: 

- ### PhysicsSystem::allowMoveOnGround: Ground Check
  Checks if player can move on ground.
  - Casts a ray from camera down to ground (specified distance).
  - Returns ground/stair entity ID if collision detected within distance, or 0 otherwise (camera not found or no collision).

- ### PhysicsSystem::getPersonCollidedMesh: Collision Check
  Checks if player can move on ground.
    - Checks collisions between ghost (player) and other objects.
    - Returns collided entity ID (0 if none).



# The Solution of the game 

- Door 7 --> Door 8 --> Door 5 --> Door 6 --> Door 4 --> Door 2 --> Door 3 --> Door 1 