#ifndef Midware_MidwareManager
#define Midware_MidwareManager

#include <vector>
#include "midwareComponent/midwareComponent.hpp"

using namespace std;

namespace Midware {
    class MidwareManager {
        // private:
        //     vector<MidwareComponent> componentList;
        public:
            vector<MidwareComponent*> componentList;
            MidwareManager();
            ~MidwareManager();
            void registerMidwareComponent(MidwareComponent*);
    };

    MidwareManager::MidwareManager() {}
    MidwareManager::~MidwareManager() {}

    void MidwareManager::registerMidwareComponent(MidwareComponent* component) {
        componentList.push_back(component);
    }
}

#endif