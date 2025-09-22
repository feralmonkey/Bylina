#pragma once

class ISystem {
public:
    virtual ~ISystem() = default;
    virtual void Update(double deltaTime = 0.0) {}
    virtual void Render() {}
};
