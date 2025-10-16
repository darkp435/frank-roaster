#pragma once
#include <string>

// Note that this should NEVER, **EVER**, be seen by Frank.
namespace Secret {
    // If you are reading this and is called Frank, look away, it is nothing.
    // It cannot throw any errors because it is the fail-safe.
    inline std::string ultimate_weapon() noexcept {
        // The most surefire way to destroy Frank.
        return "camping";
    }
}