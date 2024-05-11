#include "../ecs/world.hpp"
#include "../application.hpp"
#include "text-renderer.hpp"

namespace our {
    // This is responsible for picking objects and either inserting them into player hand or putting them in the invwentory
    struct PickableObject {

    };

    class PickingSystem {
        Application *app;
        TextRenderer * renderer;
        bool inventoryState = false;
        std::vector<std::string> inventoryEntities = {"key1", "key2","key3","key4","key5","key6","key7","key8","key9"};
        std::vector<glm::vec3> inventoryPositions;
        std::string itemInRightHand;
        double currentTime = 0;
        double lastTimePicked = 0 ;
        double lastTimeInInventory = 0 ;
    public:
        std::string pickedObject;
        std::string mousePickedObject;

        bool isPickable(std::string);

        void update(our::World *world, our::Application *app, std::string &inHandItem,
                          TextRenderer *renderer, std::string & songName, float & songDuration ,
                          double deltaTime);

        void pick(World *world, const std::string&);

        void addToInventory(World *world, std::string);

        void showInventory(World *world);

        void hideInventory(World *world);

        std::string getClickedInventoryItem(World *world,const int &mouseX, const int &mouseY);
        void showMessage(std::string, double time);
    };
}