#ifndef PARTICLE_H
#define PARTICLE_H

#include <QPointF>
#include <QVector>

class Particle
{
public:
    Particle(double x, double y, double vx, double vy, double mass, double radius);

    // Getters
    QPointF getPosition() const { return position; }
    QPointF getVelocity() const { return velocity; }
    double getMass() const { return mass; }
    double getRadius() const { return radius; }
    bool isActive() const { return active; }

    // Setters
    void setPosition(const QPointF& pos) { position = pos; }
    void setVelocity(const QPointF& vel) { velocity = vel; }
    void setMass(double m) { mass = m; }
    void setRadius(double r) { radius = r; }
    void setActive(bool a) { active = a; }

    // Métodos de actualización
    void updatePosition(double dt);

    // Detección de colisiones
    bool checkCollisionWithParticle(const Particle& other) const;

    // Combinar partículas (colisión completamente inelástica)
    static Particle merge(const Particle& p1, const Particle& p2);

private:
    QPointF position;  // (x, y)
    QPointF velocity;  // (vx, vy)
    double mass;
    double radius;
    bool active;       // false si la partícula se fusionó con otra
};

#endif // PARTICLE_H
