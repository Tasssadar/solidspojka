#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

static const float screws[][2] =
{
    {0.693, 1},
    {0.893, 1.2},
    {1.171, 1.6},
    {1.509, 2},
    {1.948, 2.5},
    {2.387, 3},
    {3.141, 4},
    {4.019, 5},
    {4.773, 6},
    {6.466, 8},
    {8.160, 10},
    {9.853, 12},
    {11.546, 14},
    {13.546, 16},
    {16.933, 20},
    {20.319, 24},
    {25.706, 30},
    {31.093, 36},
    {36.479, 42},
    {41.866, 48},
    {49.253, 56},
    {56.639, 64},
    {64.639, 72},
    {72.639, 80},
    {82.639, 90},
    {92.639, 100},
};

struct peroLine
{
    explicit peroLine(float f, float t, float _t1, float _b, float _h, float p = 0)
    {
        from = f;
        to = t;
        t1 = _t1;
        b = _b;
        h = _h;
        pl = p;
    }

    float from;
    float to;
    float t1;
    float pl;
    float b;
    float h;
};

static const peroLine peroTables[] =
{
    peroLine(6, 8, 0.9, 2, 2),
    peroLine(8, 10, 1.3, 3, 3),
    peroLine(10, 12, 1.6, 4, 4, 0.2),
    peroLine(12, 17, 2.1, 5, 5, 0.1),
    peroLine(17, 22, 2.5, 6, 6),
    peroLine(22, 30, 2.9, 8, 7),
    peroLine(30, 38, 3.3, 10, 8),
    peroLine(38, 44, 3.1, 12, 8),
    peroLine(44, 50, 3.5, 14, 9),
    peroLine(50, 58, 3.8, 16, 10),
    peroLine(58, 65, 4.2, 18, 11),
    peroLine(65, 75, 4.6, 20, 12, 0.4),
    peroLine(75, 85, 5.5, 22, 14, 0.2),
    peroLine(85, 95, 5.3, 25, 14),
    peroLine(95, 110, 6.1, 28, 16),
    peroLine(110, 130, 6.9, 32, 18),
    peroLine(130, 150, 7.7, 36, 20),
    peroLine(150, 170, 8.5, 40, 22),
    peroLine(170, 200, 9.7, 45, 25, 0.6),
    peroLine(200, 230, 11, 50, 28, 0.3)
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void calcBtn();
    
private:
    void throwError(const QString& text);
    void append(const QString& text);

    float calcMk(quint32 power, quint32 rounds);
    quint32 calcD(float mk);
    quint32 calcScrews(quint32 d);
    quint32 calcFriction(float mk, quint32 D);
    quint32 calcNorm(quint32 friction);
    float calcDiameter(quint32 norm);
    float calcDepth(float diameter);
    float calcLen(float mk, float t1, float d);

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
