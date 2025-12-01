#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), projectileItem(nullptr)
{
    setupUI();
    setupGame();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateGame);
}

MainWindow::~MainWindow()
{
    delete engine;
}

void MainWindow::setupUI()
{
    // Widget central
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Scene y View
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 800, 600);
    scene->setBackgroundBrush(QBrush(QColor(135, 206, 235))); // Cielo azul

    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(view);

    // Panel de controles
    QGroupBox *controlBox = new QGroupBox("Controles de Lanzamiento");
    QHBoxLayout *controlLayout = new QHBoxLayout(controlBox);

    // Ángulo
    QVBoxLayout *angleLayout = new QVBoxLayout();
    angleLabel = new QLabel("Ángulo: 45°");
    angleSlider = new QSlider(Qt::Horizontal);
    angleSlider->setRange(0, 90);
    angleSlider->setValue(45);
    angleLayout->addWidget(angleLabel);
    angleLayout->addWidget(angleSlider);
    controlLayout->addLayout(angleLayout);

    // Velocidad
    QVBoxLayout *speedLayout = new QVBoxLayout();
    speedLabel = new QLabel("Velocidad: 150");
    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(50, 300);
    speedSlider->setValue(150);
    speedLayout->addWidget(speedLabel);
    speedLayout->addWidget(speedSlider);
    controlLayout->addLayout(speedLayout);

    // Botón de lanzamiento
    launchButton = new QPushButton("LANZAR");
    launchButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 10px; }");
    controlLayout->addWidget(launchButton);

    mainLayout->addWidget(controlBox);

    // Panel de estado
    QHBoxLayout *statusLayout = new QHBoxLayout();
    playerLabel = new QLabel("Turno: Jugador 1");
    playerLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    statusLabel = new QLabel("Ajusta el ángulo y velocidad, luego presiona LANZAR");
    statusLayout->addWidget(playerLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(statusLabel);
    mainLayout->addLayout(statusLayout);

    // Conexiones
    connect(angleSlider, &QSlider::valueChanged, this, &MainWindow::updateAngleLabel);
    connect(speedSlider, &QSlider::valueChanged, this, &MainWindow::updateSpeedLabel);
    connect(launchButton, &QPushButton::clicked, this, &MainWindow::launchProjectile);

    setWindowTitle("Juego de Estrategia Militar - Práctica 5");
    resize(900, 750);
}

void MainWindow::setupGame()
{
    engine = new GameEngine(800, 600);

    // Infraestructura Jugador 1 (izquierda)
    engine->addInfrastructure(1, Infrastructure(50, 450, 40, 100, 200));
    engine->addInfrastructure(1, Infrastructure(100, 450, 40, 100, 200));
    engine->addInfrastructure(1, Infrastructure(150, 450, 40, 100, 100));

    // Infraestructura Jugador 2 (derecha)
    engine->addInfrastructure(2, Infrastructure(610, 450, 40, 100, 200));
    engine->addInfrastructure(2, Infrastructure(660, 450, 40, 100, 200));
    engine->addInfrastructure(2, Infrastructure(710, 450, 40, 100, 100));

    renderScene();
}

void MainWindow::renderScene()
{
    scene->clear();
    player1InfraItems.clear();
    player2InfraItems.clear();
    resistanceLabels1.clear();
    resistanceLabels2.clear();

    // Dibujar suelo
    QGraphicsRectItem *ground = scene->addRect(0, 550, 800, 50, QPen(Qt::NoPen), QBrush(QColor(101, 67, 33)));

    // Dibujar infraestructura Jugador 1
    const QVector<Infrastructure>& infra1 = engine->getPlayer1Infrastructure();
    for (int i = 0; i < infra1.size(); ++i) {
        if (!infra1[i].isDestroyed()) {
            QRectF rect = infra1[i].getRect();
            QGraphicsRectItem *item = scene->addRect(rect, QPen(Qt::black, 2), QBrush(QColor(70, 130, 180)));
            player1InfraItems.append(item);

            QGraphicsTextItem *label = scene->addText(QString::number((int)infra1[i].getResistance()));
            label->setPos(rect.center().x() - 10, rect.center().y() - 10);
            label->setDefaultTextColor(Qt::white);
            resistanceLabels1.append(label);
        }
    }

    // Dibujar infraestructura Jugador 2
    const QVector<Infrastructure>& infra2 = engine->getPlayer2Infrastructure();
    for (int i = 0; i < infra2.size(); ++i) {
        if (!infra2[i].isDestroyed()) {
            QRectF rect = infra2[i].getRect();
            QGraphicsRectItem *item = scene->addRect(rect, QPen(Qt::black, 2), QBrush(QColor(220, 20, 60)));
            player2InfraItems.append(item);

            QGraphicsTextItem *label = scene->addText(QString::number((int)infra2[i].getResistance()));
            label->setPos(rect.center().x() - 10, rect.center().y() - 10);
            label->setDefaultTextColor(Qt::white);
            resistanceLabels2.append(label);
        }
    }

    // Etiquetas de jugadores
    QGraphicsTextItem *p1Label = scene->addText("JUGADOR 1");
    p1Label->setPos(50, 580);
    p1Label->setDefaultTextColor(QColor(70, 130, 180));
    QFont font = p1Label->font();
    font.setBold(true);
    p1Label->setFont(font);

    QGraphicsTextItem *p2Label = scene->addText("JUGADOR 2");
    p2Label->setPos(680, 580);
    p2Label->setDefaultTextColor(QColor(220, 20, 60));
    p2Label->setFont(font);
}

void MainWindow::updateGame()
{
    bool projectileActive = engine->update(0.016); // ~60 FPS

    if (projectileActive) {
        const Projectile *proj = engine->getActiveProjectile();
        if (proj && proj->isActive()) {
            QPointF pos = proj->getPosition();

            if (projectileItem == nullptr) {
                projectileItem = scene->addEllipse(0, 0, 16, 16,
                                                   QPen(Qt::black), QBrush(Qt::black));
            }

            projectileItem->setPos(pos.x() - 8, pos.y() - 8);
            updateResistanceLabels();
        }
    } else {
        if (projectileItem) {
            scene->removeItem(projectileItem);
            delete projectileItem;
            projectileItem = nullptr;
        }

        timer->stop();
        launchButton->setEnabled(true);

        if (engine->isGameOver()) {
            QMessageBox::information(this, "¡Juego Terminado!",
                                     QString("¡Jugador %1 gana!").arg(engine->getWinner()));
            statusLabel->setText("Juego terminado");
        } else {
            playerLabel->setText(QString("Turno: Jugador %1").arg(engine->getCurrentPlayer()));
            statusLabel->setText("Ajusta el ángulo y velocidad, luego presiona LANZAR");
            renderScene();
        }
    }
}

void MainWindow::launchProjectile()
{
    if (engine->isGameOver()) return;

    double angle = angleSlider->value();
    double speed = speedSlider->value();

    engine->launchProjectile(engine->getCurrentPlayer(), angle, speed);

    launchButton->setEnabled(false);
    statusLabel->setText("Proyectil en vuelo...");
    timer->start(16); // ~60 FPS
}

void MainWindow::updateAngleLabel(int value)
{
    angleLabel->setText(QString("Ángulo: %1°").arg(value));
}

void MainWindow::updateSpeedLabel(int value)
{
    speedLabel->setText(QString("Velocidad: %1").arg(value));
}

void MainWindow::updateResistanceLabels()
{
    const QVector<Infrastructure>& infra1 = engine->getPlayer1Infrastructure();
    for (int i = 0; i < resistanceLabels1.size() && i < infra1.size(); ++i) {
        resistanceLabels1[i]->setPlainText(QString::number((int)infra1[i].getResistance()));
    }

    const QVector<Infrastructure>& infra2 = engine->getPlayer2Infrastructure();
    for (int i = 0; i < resistanceLabels2.size() && i < infra2.size(); ++i) {
        resistanceLabels2[i]->setPlainText(QString::number((int)infra2[i].getResistance()));
    }
}
