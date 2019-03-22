#ifndef __ChaseCameraObject_H__
#define __ChaseCameraObject_H__

#include "GameObjects/Camera.h"

class GameObject;

class ChaseCameraObject : public Camera
{
protected:
    GameObject* m_ObjectFollowing;
    float m_Distance;

    bool m_MouseLocked;
    vec2 m_MouseChangeSinceLastFrame;
    vec2 m_LastMousePosition;

public:
    ChaseCameraObject(Scene* pScene, std::string name, Transform transform);
    virtual ~ChaseCameraObject();

    virtual void OnEvent(Event* pEvent);
    virtual void Update(float deltaTime);

    void SetObjectToFollow(GameObject* pObject, float dist) { m_ObjectFollowing = pObject; m_Distance = dist; }
};

#endif //__ChaseCameraObject_H__
