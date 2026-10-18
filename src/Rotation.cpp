#include "Rotation.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

void advanceRotation(RotationState& state, float deltaSeconds)
{
    if (!state.enabled)
        return;

    state.angleRadians = std::fmod(
        state.angleRadians + state.speedRadiansPerSecond * std::max(deltaSeconds, 0.0f),
        2.0f * std::numbers::pi_v<float>);
}
