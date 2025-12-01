#include "particle.h"
#include <cmath>

Particle::Particle(double x, double y, double vx, double vy, double m, double r)
    : position(x, y), velocity(vx, vy), mass(m), radius(r), active(true)
{
}

void Particle::updatePosition(double dt)
{
    if (!active) return;

    // Actualizar posición usando velocidad actual
    position.setX(position.x() + velocity.x() * dt);
    position.setY(position.y() + velocity.y() * dt);
}

bool Particle::checkCollisionWithParticle(const Particle& other) const
{
    if (!active || !other.active) return false;

    // Calcular distancia entre centros
    double dx = position.x() - other.position.x();
    double dy = position.y() - other.position.y();
    double distance = std::sqrt(dx * dx + dy * dy);

    // Colisión si la distancia es menor que la suma de radios
    return distance < (radius + other.radius);
}

Particle Particle::merge(const Particle& p1, const Particle& p2)
{
    // Conservación del momento lineal: m1*v1 + m2*v2 = (m1+m2)*v'
    double totalMass = p1.mass + p2.mass;

    // Calcular nueva velocidad usando conservación del momento
    double newVx = (p1.mass * p1.velocity.x() + p2.mass * p2.velocity.x()) / totalMass;
    double newVy = (p1.mass * p1.velocity.y() + p2.mass * p2.velocity.y()) / totalMass;

    // Calcular posición del centro de masa
    double newX = (p1.mass * p1.position.x() + p2.mass * p2.position.x()) / totalMass;
    double newY = (p1.mass * p1.position.y() + p2.mass * p2.position.y()) / totalMass;

    // Calcular nuevo radio conservando aproximadamente el área
    // Área total = π*r1² + π*r2² = π*r'²
    // r' = sqrt(r1² + r2²)
    double newRadius = std::sqrt(p1.radius * p1.radius + p2.radius * p2.radius);

    // Crear y retornar la nueva partícula fusionada
    return Particle(newX, newY, newVx, newVy, totalMass, newRadius);
}
