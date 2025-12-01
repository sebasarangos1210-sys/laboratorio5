#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <QPointF>
#include <QGraphicsEllipseItem>

class Projectile
{
public:
    Projectile(double x, double y, double angle, double speed, double mass);

    QPointF getPosition() const { return position; }
    QPointF getVelocity() const { return velocity; }
    double getMass() const { return mass; }
    double getRadius() const { return radius; }
    bool isActive() const { return active; }

    void setPosition(const QPointF& pos) { position = pos; }
    void setVelocity(const QPointF& vel) { velocity = vel; }
    void setActive(bool a) { active = a; }

    void update(double dt);

private:
    QPointF position;
    QPointF velocity;
    double mass;
    double radius;
    bool active;
    const double gravity = 9.8;
};

#endif // PROJECTILE_H
