#include "simulator.h"
#include <QFile>
#include <QDebug>
#include <cmath>

Simulator::Simulator(double boxWidth, double boxHeight, double deltaT)
    : box(boxWidth, boxHeight), dt(deltaT), currentTime(0.0)
{
}

void Simulator::addParticle(const Particle& particle)
{
    particles.append(particle);
    trajectories.append(QVector<QPointF>());
}

void Simulator::addObstacle(const Obstacle& obstacle)
{
    obstacles.append(obstacle);
}

void Simulator::run(double duration)
{
    int steps = static_cast<int>(duration / dt);

    qDebug() << "Ejecutando simulación con" << steps << "pasos...";

    for (int step = 0; step < steps; ++step) {
        currentTime = step * dt;

        // Actualizar posiciones de todas las partículas
        updateParticles();

        // Manejar todos los tipos de colisiones
        handleWallCollisions();
        handleObstacleCollisions();
        handleParticleCollisions();

        // Registrar posiciones actuales para la trayectoria
        recordPositions();

        // Mostrar progreso cada 10% de la simulación
        if (step % (steps / 10) == 0) {
            qDebug() << "Progreso:" << (step * 100 / steps) << "%";
        }
    }

    qDebug() << "Simulación completada.";
    qDebug() << "Total de colisiones registradas:" << collisions.size();
}

void Simulator::updateParticles()
{
    for (int i = 0; i < particles.size(); ++i) {
        if (particles[i].isActive()) {
            particles[i].updatePosition(dt);
        }
    }
}

void Simulator::handleWallCollisions()
{
    for (int i = 0; i < particles.size(); ++i) {
        if (!particles[i].isActive()) continue;

        QPointF pos = particles[i].getPosition();
        QPointF vel = particles[i].getVelocity();
        double radius = particles[i].getRadius();

        int collision = box.checkWallCollision(pos.x(), pos.y(), radius);

        if (collision > 0) {
            QString wall;

            // Colisiones perfectamente elásticas con las paredes
            if (collision == 1) {  // pared izquierda
                vel.setX(-vel.x());
                pos.setX(radius);
                wall = "izquierda";
            }
            else if (collision == 2) {  // pared derecha
                vel.setX(-vel.x());
                pos.setX(box.getWidth() - radius);
                wall = "derecha";
            }
            else if (collision == 3) {  // pared superior
                vel.setY(-vel.y());
                pos.setY(radius);
                wall = "arriba";
            }
            else if (collision == 4) {  // pared inferior
                vel.setY(-vel.y());
                pos.setY(box.getHeight() - radius);
                wall = "abajo";
            }

            particles[i].setVelocity(vel);
            particles[i].setPosition(pos);

            // Registrar evento de colisión
            CollisionEvent event;
            event.time = currentTime;
            event.description = QString("Partícula %1 colisiona con pared %2")
                                    .arg(i).arg(wall);
            collisions.append(event);
        }
    }
}

void Simulator::handleObstacleCollisions()
{
    for (int i = 0; i < particles.size(); ++i) {
        if (!particles[i].isActive()) continue;

        QPointF pos = particles[i].getPosition();
        QPointF vel = particles[i].getVelocity();
        double radius = particles[i].getRadius();

        for (int j = 0; j < obstacles.size(); ++j) {
            if (obstacles[j].checkCollision(pos, radius)) {
                // Calcular posición anterior para determinar lado de colisión
                QPointF prevPos = pos - vel * dt;
                int side = obstacles[j].getCollisionSide(pos, prevPos);

                // Aplicar coeficiente de restitución (colisión inelástica)
                // v'⊥ = -ε * v⊥  (componente perpendicular)
                // v'∥ = v∥      (componente paralela se mantiene)

                if (side == 0 || side == 2) {  // arriba o abajo (perpendicular en Y)
                    vel.setY(-vel.y() * restitutionCoefficient);
                } else {  // izquierda o derecha (perpendicular en X)
                    vel.setX(-vel.x() * restitutionCoefficient);
                }

                particles[i].setVelocity(vel);

                // Registrar evento de colisión
                CollisionEvent event;
                event.time = currentTime;
                QString sideStr;
                switch(side) {
                case 0: sideStr = "arriba"; break;
                case 1: sideStr = "derecha"; break;
                case 2: sideStr = "abajo"; break;
                case 3: sideStr = "izquierda"; break;
                }
                event.description = QString("Partícula %1 colisiona con obstáculo %2 (lado %3)")
                                        .arg(i).arg(j).arg(sideStr);
                collisions.append(event);

                // Solo procesar una colisión por partícula por paso de tiempo
                break;
            }
        }
    }
}

void Simulator::handleParticleCollisions()
{
    // Revisar todas las parejas de partículas
    for (int i = 0; i < particles.size(); ++i) {
        if (!particles[i].isActive()) continue;

        for (int j = i + 1; j < particles.size(); ++j) {
            if (!particles[j].isActive()) continue;

            // Verificar si hay colisión
            if (particles[i].checkCollisionWithParticle(particles[j])) {
                // Colisión completamente inelástica: las partículas se fusionan
                Particle merged = Particle::merge(particles[i], particles[j]);

                // Registrar evento de colisión
                CollisionEvent event;
                event.time = currentTime;
                event.description = QString("Partícula %1 (masa=%.2f) y Partícula %2 (masa=%.2f) se fusionan en nueva partícula (masa=%.2f)")
                                        .arg(i)
                                        .arg(particles[i].getMass())
                                        .arg(j)
                                        .arg(particles[j].getMass())
                                        .arg(merged.getMass());
                collisions.append(event);

                qDebug() << "Fusión detectada en t=" << currentTime << "s:"
                         << "Partícula" << i << "+ Partícula" << j;

                // Desactivar las partículas originales
                particles[i].setActive(false);
                particles[j].setActive(false);

                // Agregar la nueva partícula fusionada
                particles.append(merged);
                trajectories.append(QVector<QPointF>());

                // Solo procesar una fusión a la vez
                return;
            }
        }
    }
}

void Simulator::recordPositions()
{
    // Guardar la posición actual de cada partícula activa
    for (int i = 0; i < particles.size(); ++i) {
        if (i < trajectories.size()) {
            if (particles[i].isActive()) {
                trajectories[i].append(particles[i].getPosition());
            }
        }
    }
}

void Simulator::exportToFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "No se pudo abrir el archivo para escritura:" << filename;
        return;
    }

    QTextStream out(&file);

    // Escribir encabezado con información de la simulación
    out << "# ============================================\n";
    out << "# Simulación de Sistema de Colisiones Múltiples\n";
    out << "# ============================================\n";
    out << "# Dimensiones de la caja: " << box.getWidth() << " x " << box.getHeight() << "\n";
    out << "# Paso de tiempo (dt): " << dt << " segundos\n";
    out << "# Número de partículas iniciales: " << particles.size() << "\n";
    out << "# Número de obstáculos: " << obstacles.size() << "\n";
    out << "# Coeficiente de restitución: " << restitutionCoefficient << "\n";
    out << "# ============================================\n\n";

    // Escribir trayectorias
    out << "# TRAYECTORIAS\n";
    out << "# Formato: Tiempo(s), Partícula_ID, X, Y\n";
    out << "# ============================================\n";

    for (int i = 0; i < trajectories.size(); ++i) {
        for (int t = 0; t < trajectories[i].size(); ++t) {
            double time = t * dt;
            out << time << ","
                << i << ","
                << trajectories[i][t].x() << ","
                << trajectories[i][t].y() << "\n";
        }
    }

    // Escribir colisiones
    out << "\n# COLISIONES\n";
    out << "# Formato: Tiempo(s), Descripción\n";
    out << "# ============================================\n";

    for (const CollisionEvent& event : collisions) {
        out << event.time << "," << event.description << "\n";
    }

    // Escribir resumen final
    out << "\n# ============================================\n";
    out << "# RESUMEN\n";
    out << "# ============================================\n";
    out << "# Total de puntos de trayectoria registrados: ";
    int totalPoints = 0;
    for (const auto& traj : trajectories) {
        totalPoints += traj.size();
    }
    out << totalPoints << "\n";
    out << "# Total de colisiones registradas: " << collisions.size() << "\n";

    // Contar tipos de colisiones
    int wallCollisions = 0;
    int obstacleCollisions = 0;
    int fusionCollisions = 0;

    for (const CollisionEvent& event : collisions) {
        if (event.description.contains("pared")) {
            wallCollisions++;
        } else if (event.description.contains("obstáculo")) {
            obstacleCollisions++;
        } else if (event.description.contains("fusionan")) {
            fusionCollisions++;
        }
    }

    out << "# Colisiones con paredes: " << wallCollisions << "\n";
    out << "# Colisiones con obstáculos: " << obstacleCollisions << "\n";
    out << "# Fusiones de partículas: " << fusionCollisions << "\n";
    out << "# ============================================\n";

    file.close();

    qDebug() << "Datos exportados exitosamente a" << filename;
    qDebug() << "Total de puntos:" << totalPoints;
    qDebug() << "Total de colisiones:" << collisions.size();
}
