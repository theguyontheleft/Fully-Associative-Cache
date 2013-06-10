/*
 *    James Mead Dagres
 *    jdagres@vt.edu
 *
 *
 *    04-07-13
 *
 *    Controller Class Header File
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "computersimgui.h" // first main window
#include "centralprocessingunit.h" // Contains all the # includes

class controller : public QObject
{
    Q_OBJECT
public:
    explicit controller(QObject *parent = 0);

    /* the init function initializes all of the connects for the signals and slots
     * it is called during the constructor, and initializes the two views (GUI's)
     */
    void init();

signals:

public slots:

private:
    // Instansiates an object for the model, view and QObject ca;ss
    CentralProcessingUnit myCPU; // QObject, model
    ComputerSimGUI mySimGui; // View 1
    Cachegui myCache; // View 2

private slots:
    void closeWidgetSlot();
    void displayWidgetSlot();
};

#endif // CONTROLLER_H
