#include "../ecs/world.hpp"
#include "../application.hpp"

namespace our {
    // This is responsible for picking objects and either inserting them into player hand or putting them in the invwentory
    class PickingSystem {
        Application *app;
        bool inventoryState = false;
        std::vector<Entity *> inventoryEntities = {};
    public:
        void update(World *world, Application *app);

        void pick(World *world, std::string);

        void addToInventory(World *world, std::string);

        void showInventory(World *world);

        void hideInventory(World *world);
    };
}