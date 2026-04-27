#include "Animation.h"

void Animator::play(const AnimClip& clip, bool forceRestart) {
    if (current == &clip && !forceRestart) return;
    current = &clip;
    frameIdx = 0;
    elapsed = 0.f;
}

void Animator::update(float dt) {
    if (!current || current->frames.empty()) return;

    elapsed += dt;
    while (elapsed >= current->frameDuration) {
        elapsed -= current->frameDuration;
        ++frameIdx;
        if (frameIdx >= (int)current->frames.size()) {
            frameIdx = current->loop ? 0 : (int)current->frames.size() - 1;
        }
    }
}

IntRect Animator::getCurrentRect() const {
    if (!current || current->frames.empty()) return {};
    return current->frames[frameIdx];
}

bool Animator::isFinished() const {
    if (!current || current->loop) return false;
    return frameIdx >= (int)current->frames.size() - 1;
}