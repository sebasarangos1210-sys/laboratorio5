
#include "box.h"

Box::Box(double w, double h)
    : width(w), height(h)
{
}

int Box::checkWallCollision(double x, double y, double radius) const
{
    if (x - radius <= 0) return 1;        // pared izquierda
    if (x + radius >= width) return 2;     // pared derecha
    if (y - radius <= 0) return 3;        // pared superior
    if (y + radius >= height) return 4;    // pared inferior

    return 0; // sin colisión
}
