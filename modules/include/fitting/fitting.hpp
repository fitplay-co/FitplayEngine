#include <string>
#include <vector>
#include "json.hpp"
#include "glm/glm.hpp"

using namespace nlohmann;

namespace fitplay {
    class fitting {
        private:
            /* data */
        public:
            fitting();
            ~ fitting();
            void process(json& data);
    };

    fitting::fitting() {}
    fitting::~fitting() {}
    void fitting::process(json& data) {
        data["wasm_fitting_version"] = "0.0.1";
        //check each joint direction
        
    }
}
