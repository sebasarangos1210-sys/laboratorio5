#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "particle.h"
#include "obstacle.h"
#include "box.h"
#include <QVector>
#include <QString>
#include <QTextStream>

struct CollisionEvent {
    double time;
    QString description;
};

class Simulator
{
public:
    Simulator(double boxWidth, double boxHeight, double dt);

    void addParticle(const Particle& particle);
    void addObstacle(const Obstacle& obstacle);

    void run(double duration);
    void exportToFile(const QString& filename);

private:
    Box box;
    QVector<Particle> particles;
    QVector<Obstacle> obstacles;
    double dt;  // intervalo de tiempo
    double currentTime;

    // Datos de simulación
    QVector<QVector<QPointF>> trajectories;  // trayectorias[particleId][timeStep]
    QVector<CollisionEvent> collisions;

    // Constantes físicas
    const double restitutionCoefficient = 0.7;  // para colisiones con obstáculos

    void updateParticles();
    void handleWallCollisions();
    void handleObstacleCollisions();
    void handleParticleCollisions();

    void recordPositions();
};

#endif // SIMULATOR_H
