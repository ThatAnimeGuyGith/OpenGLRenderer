#pragma once

struct RotationState {
    bool enabled = true;
    float speedRadiansPerSecond = 0.35f;
    float angleRadians = 0.0f;
};

void advanceRotation(RotationState& state, float deltaSeconds);
