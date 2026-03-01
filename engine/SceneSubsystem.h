// Created by Patricio Palma on 08-03-26.

#ifndef LETSLEARNSDL_SCENESUBSYSTEM_H
#define LETSLEARNSDL_SCENESUBSYSTEM_H

class Component;

class SceneSubsystem {
public:
    virtual ~SceneSubsystem() = default;

    virtual void onComponentAdded(Component* c)   {}
    virtual void onComponentRemoved(Component* c) {}
    virtual void update()                         {}
};

#endif //LETSLEARNSDL_SCENESUBSYSTEM_H
