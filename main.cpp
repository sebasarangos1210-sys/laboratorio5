#include <QCoreApplication>
#include "simulator.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Crear simulador
    // Caja de 800x600, dt = 0.01 segundos
    Simulator sim(800, 600, 0.01);

    // Agregar 4 obstáculos cuadrados
    sim.addObstacle(Obstacle(200, 150, 50, 50));
    sim.addObstacle(Obstacle(550, 150, 50, 50));
    sim.addObstacle(Obstacle(200, 400, 50, 50));
    sim.addObstacle(Obstacle(550, 400, 50, 50));

    // Agregar 4 partículas con diferentes propiedades
    sim.addParticle(Particle(100, 100, 50, 30, 1.0, 10));   // x, y, vx, vy, masa, radio
    sim.addParticle(Particle(700, 100, -40, 40, 1.5, 12));
    sim.addParticle(Particle(100, 500, 60, -35, 0.8, 8));
    sim.addParticle(Particle(700, 500, -45, -25, 1.2, 11));

    qDebug() << "Iniciando simulación...";

    // Ejecutar simulación por 20 segundos
    sim.run(20.0);

    // Exportar resultados
    sim.exportToFile("simulacion_colisiones.txt");

    qDebug() << "Simulación completada.";
    qDebug() << "Los resultados se guardaron en 'simulacion_colisiones.txt'";
    qDebug() << "Puede graficar los datos usando Python/matplotlib o cualquier otra herramienta";

    return 0;
}
