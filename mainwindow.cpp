#include <QMessageBox>
#include <math.h>
#include <map>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#define M_PI          3.14159265358979323846

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->calcBtn,  SIGNAL(clicked()),              SLOT(calcBtn()));
    connect(ui->cleanBtn, SIGNAL(clicked()), ui->outText, SLOT(clear()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::throwError(const QString& text)
{
    QMessageBox box(this);
    box.setWindowTitle("Chyba!");
    box.setText(text);
    box.setIcon(QMessageBox::Critical);
    box.exec();
}

void MainWindow::append(const QString& text)
{
    ui->outText->appendPlainText(text);
}

void MainWindow::calcBtn()
{
    bool ok = true;

    quint32 power = ui->powerLine->text().toUInt(&ok);
    if(!ok)
        return throwError("Spatny format cisla vykonu");

    quint32 rounds = ui->roundsLine->text().toUInt(&ok);
    if(!ok)
        return throwError("Spatny format cisla otacek");

    try
    {
        float   mk          = calcMk(power, rounds);
        quint32 d           = calcD(mk);
        quint32 screwsD     = calcScrews(d);
        quint32 friction    = calcFriction(mk, screwsD);
        quint32 norm        = calcNorm(friction);
        float diameter      = calcDiameter(norm);
        float peroDepth     = calcDepth(diameter);
        /*float peroLen   =*/ calcLen(mk, peroDepth, d);

    }
    catch(const QString& err)
    {
        return throwError(err);
    }
}

float MainWindow::calcMk(quint32 power, quint32 rounds)
{
    float rounds_s = float(rounds)/60;

    float mk = float(power) / (2*M_PI*rounds_s);

    mk *= 1000; // to mm

    append("Kroutici moment Mk = " + QString::number(mk, 'f') + " Nmm");

    return mk;
}

quint32 MainWindow::calcD(float mk)
{
    static const quint8 Tdk = 40; // Mpa

    // z tabulek
    static const quint8 hint[] = { 6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 25, 28, 32, 35 };

    float d = pow((16.f*mk)/(M_PI*Tdk), (1.f/3.f));

    if(d > hint[sizeof(hint)-1])
        throw QString("Prumer hridele je vetsi nez max hodnota v tabulkach (35mm, vysledek je " + QString::number(d, 'f') + ")");

    for(quint8 i = 0; i < sizeof(hint); ++i)
    {
        if(hint[i] >= d)
        {
            append("Prumer hridele d = " + QString::number(hint[i]) + " mm");
            return hint[i];
        }
    }

    throw QString("Nebyla nalezena hodnota v tabulkach (vysledek: " + QString::number(d, 'f') + ")");
}

quint32 MainWindow::calcScrews(quint32 d)
{
    quint32 res = d*3;
    append("Prumer roztecne kruznice sroubu: " + QString::number(res) + " mm");
    return res;
}

quint32 MainWindow::calcFriction(float mk, quint32 D)
{
    quint32 friction = round(2.f*mk/D);
    append("Treci sila: " + QString::number(friction) + " N");
    return friction;
}

quint32 MainWindow::calcNorm(quint32 friction)
{
    static const float coef = 0.15f;
    static const quint8 screwCnt = 4;

    quint32 norm = round(float(friction)/(coef*screwCnt));
    append("Normalova sila: " + QString::number(norm) + " N");
    return norm;
}

float MainWindow::calcDiameter(quint32 norm)
{
    static const quint8 Sdt = 80; // MPa
    quint8 tablesCnt = sizeof(screws)/(2*sizeof(float));


    float d = sqrt(((norm/Sdt)*4)/M_PI);

    if(d > screws[tablesCnt-1][0])
    {
        throw QString("Vypocitanej prumer sroubu je vetsi nez max hodnota"
                      "v tabulkach (92.639, vypocitano je " + QString::number(d, 'f') + ")");
    }

    for(quint8 i = 0; i < tablesCnt; ++i)
    {
        if(screws[i][0] >= d)
        {

            append("Prumer sroubu: " + QString::number(screws[i][1], 'f') + " mm");
            return screws[i][1];
        }
    }

    throw QString("Prumer sroubu nenalezen v tabulkach, vypocitana hodnota " + QString::number(d, 'f'));
}

float MainWindow::calcDepth(float diameter)
{
    quint32 size = sizeof(peroTables)/sizeof(peroLine);

    for(quint32 i = 0; i < size; ++i)
    {
        if(diameter >= peroTables[i].from && diameter < peroTables[i].to)
        {
            append("Hloubka pera t1 = " + QString::number(peroTables[i].t1, 'f') + " mm");
            append("Vyska pera h = " + QString::number(peroTables[i].h, 'f') + " mm");
            append("Sirka pera b = " + QString::number(peroTables[i].b, 'f') + " mm");
            return peroTables[i].t1;
        }
    }
    throw QString("Hodnoty pera pro prumer sroubu nebyly nalezeny!");
}

float MainWindow::calcLen(float mk, float t1, float d)
{
    static const float Pd = 45.f; // MPa
    static const quint16 tableVals[] = {
        9, 10, 12, 14, 16, 18, 20, 22, 25, 28, 32, 36, 40,
        45, 50, 56, 63, 70, 80, 90, 100, 110, 125, 140, 160,
        180, 200, 220, 250, 280, 315, 355, 400
    };
    static const quint8 valsCnt = sizeof(tableVals)/sizeof(quint16);

    float l = (mk/(d/2.f))/(Pd*t1);

    append("Delka pera l = " + QString::number(l, 'f') + " mm");

    if(l > tableVals[valsCnt-1])
        throw QString("Delka pera je vetsi nez 400 - mimo tabulkovou radu");

    for(quint8 i = 0; i < valsCnt; ++i)
    {
        if(tableVals[i] >= l)
        {
            QString ret = QString("Delka pera podle tabulkove rady: %1 mm (").arg(tableVals[i]);
            if(i != 0)
                ret += "predchozi: " + QString::number(tableVals[i-1]) + " mm; ";
            if(i+1 != valsCnt)
                ret += "nasledujici: " + QString::number(tableVals[i+1]) + " mm";
            append(ret + ")");
            return tableVals[i];
        }
    }

    throw QString("Nebyla nalezena hodnota delky pera v tabulkove rade!");

    return l;
}

