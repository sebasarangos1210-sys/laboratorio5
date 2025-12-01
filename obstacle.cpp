#include "obstacle.h"
#include <cmath>
#include <algorithm>

Obstacle::Obstacle(double x, double y, double w, double h)
    : rect(x, y, w, h)
{
}

bool Obstacle::checkCollision(const QPointF& center, double radius) const
{
    // Encontrar el punto más cercano del rectángulo al círculo
    double closestX = std::max(rect.left(), std::min(center.x(), rect.right()));
    double closestY = std::max(rect.top(), std::min(center.y(), rect.bottom()));

    // Calcular distancia
    double dx = center.x() - closestX;
    double dy = center.y() - closestY;
    double distance = std::sqrt(dx * dx + dy * dy);

    return distance < radius;
}

int Obstacle::getCollisionSide(const QPointF& center, const QPointF& prevCenter) const
{
    // Calcular distancias a cada lado
    double distTop = std::abs(center.y() - rect.top());
    double distBottom = std::abs(center.y() - rect.bottom());
    double distLeft = std::abs(center.x() - rect.left());
    double distRight = std::abs(center.x() - rect.right());

    // Encontrar la distancia mínima
    double minDist = std::min({distTop, distBottom, distLeft, distRight});

    if (minDist == distTop) return 0;      // arriba
    if (minDist == distRight) return 1;    // derecha
    if (minDist == distBottom) return 2;   // abajo
    return 3;                               // izquierda
}
