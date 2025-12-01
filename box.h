#ifndef BOX_H
#define BOX_H

#include <QRectF>

class Box
{
public:
    Box(double width, double height);

    double getWidth() const { return width; }
    double getHeight() const { return height; }

    // Verificar si una partícula colisiona con los bordes
    // Retorna: 0=sin colisión, 1=izquierda, 2=derecha, 3=arriba, 4=abajo
    int checkWallCollision(double x, double y, double radius) const;

private:
    double width;
    double height;
};

#endif // BOX_H
