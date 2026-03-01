// Created by Patricio Palma on 08-03-26.

#ifndef LETSLEARNSDL_COLLISIONSYSTEM_H
#define LETSLEARNSDL_COLLISIONSYSTEM_H

#include "SceneSubsystem.h"
#include "ColliderComponent.h"
#include <vector>
#include <unordered_set>
#include <utility>
#include <algorithm>

class CollisionSystem : public SceneSubsystem {
public:
    void onComponentAdded(Component* c) override {
        if (auto* col = dynamic_cast<ColliderComponent*>(c))
            colliders.push_back(col);
    }

    void onComponentRemoved(Component* c) override {
        auto* col = dynamic_cast<ColliderComponent*>(c);
        if (!col) return;
        std::erase(colliders, col);
        std::erase_if(prevOverlaps, [col](const auto& p) {
            return p.first == col || p.second == col;
        });
    }

    void update() override {
        resolveCollisions();
    }

private:
    struct PairHash {
        size_t operator()(const std::pair<ColliderComponent*, ColliderComponent*>& p) const noexcept {
            size_t h1 = std::hash<ColliderComponent*>{}(p.first);
            size_t h2 = std::hash<ColliderComponent*>{}(p.second);
            return h1 ^ (h2 + 0x9e3779b9u + (h1 << 6) + (h1 >> 2));
        }
    };
    using OverlapSet = std::unordered_set<std::pair<ColliderComponent*, ColliderComponent*>, PairHash>;

    struct ColliderEntry { ColliderComponent* col; SDL_FRect bounds; };

    std::vector<ColliderComponent*> colliders;
    std::vector<ColliderEntry>      activeSnapshot;
    OverlapSet                      prevOverlaps;

    void resolveCollisions() {
        if (colliders.size() < 2) return;

        activeSnapshot.clear();
        for (auto* c : colliders) {
            if (c->getOwner()->isActive() && c->isEnabled())
                activeSnapshot.push_back({c, c->getBounds()});
        }

        // Broad phase: sort and prune on X axis.
        // Sorts by minX, skips pairs where minX of j exceeds maxX of i.
        // Y overlap is checked as a secondary AABB filter before narrow phase.
        std::sort(activeSnapshot.begin(), activeSnapshot.end(),
            [](const ColliderEntry& a, const ColliderEntry& b) {
                return a.bounds.x < b.bounds.x;
            });

        OverlapSet current;
        for (size_t i = 0; i < activeSnapshot.size(); i++) {
            const SDL_FRect& ra   = activeSnapshot[i].bounds;
            const float      maxX = ra.x + ra.w;

            for (size_t j = i + 1; j < activeSnapshot.size(); j++) {
                const SDL_FRect& rb = activeSnapshot[j].bounds;
                if (rb.x >= maxX) break;
                if (ra.y >= rb.y + rb.h || ra.y + ra.h <= rb.y) continue;
                if (!activeSnapshot[i].col->overlaps(*activeSnapshot[j].col)) continue;

                auto* ca = activeSnapshot[i].col;
                auto* cb = activeSnapshot[j].col;
                current.insert(ca < cb ? std::make_pair(ca, cb) : std::make_pair(cb, ca));
            }
        }

        for (const auto& [a, b] : current) {
            if (!prevOverlaps.contains({a, b})) {
                if (a->onEnter) a->onEnter(b);
                if (b->onEnter) b->onEnter(a);
            } else {
                if (a->onStay) a->onStay(b);
                if (b->onStay) b->onStay(a);
            }
        }
        for (const auto& [a, b] : prevOverlaps) {
            if (!current.contains({a, b})) {
                if (a->onExit) a->onExit(b);
                if (b->onExit) b->onExit(a);
            }
        }

        prevOverlaps = std::move(current);
    }
};

#endif //LETSLEARNSDL_COLLISIONSYSTEM_H
