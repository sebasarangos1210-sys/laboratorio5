#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QRectF>
#include <QPointF>

class Obstacle
{
public:
    Obstacle(double x, double y, double width, double height);

    QRectF getRect() const { return rect; }

    // Detectar colisión con círculo
    bool checkCollision(const QPointF& center, double radius) const;

    // Determinar el lado de colisión (0=arriba, 1=derecha, 2=abajo, 3=izquierda)
    int getCollisionSide(const QPointF& center, const QPointF& prevCenter) const;

private:
    QRectF rect;
};

#endif // OBSTACLE_H
