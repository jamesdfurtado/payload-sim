#pragma once

#include <raylib.h>
#include <vector>

class Widget {
public:
    virtual ~Widget() = default;
    virtual void update(float /*dt*/) {}
    virtual void draw() const {} 
    virtual bool onMouseDown(Vector2 /*pos*/) { return false; }
    virtual bool onMouseUp(Vector2 /*pos*/) { return false; }
    virtual bool onMouseMove(Vector2 /*pos*/) { return false; }

    void setBounds(const Rectangle& r) { bounds = r; }
    const Rectangle& getBounds() const { return bounds; }
    bool contains(Vector2 p) const { return CheckCollisionPointRec(p, bounds); }

protected:
    Rectangle bounds{0,0,0,0};
};


