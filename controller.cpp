/*
 *    James Mead Dagres
 *    jdagres@vt.edu
 *
 *
 *    04-07-13
 *
 *    Controller Class Implementation File
 */

#include "controller.h"

controller::controller(QObject *parent) :
    QObject(parent)
{

}

void controller::init()
{
    //~~~~~~~~ Signals that emit from SimGui (view 1) class to the QObject (Model) class: ~~~~~~~~
    // This signal sees if the file was opened and checks to see if the file exists
    connect(&mySimGui, SIGNAL(instructionFileSelected(int)), &myCPU, SLOT(instructionOpenedSlot(int)));
    // The following connect communicates to the 9 functions that the programs the program must send signals too:
    connect(&mySimGui, SIGNAL(instructionProcess(QString, int, int)), &myCPU, SLOT(instructionProcessSlot(QString, int, int)));
    // The following connect tells the QObject class to resest
    connect(&mySimGui, SIGNAL(instructionReset()), &myCPU, SLOT(instructionResetSlot()));
    // Sends the breakPointLine to the QObject class
    connect(&mySimGui, SIGNAL(breakPoint(int)), &myCPU, SLOT(breakPointSlot(int)));

    //~~~~~~~~ Signals that emit from the QObject (Model) class to SimGui class: ~~~~~~~~
    // Emits to the invalidInstruction slot
    connect(&myCPU, SIGNAL(invalidInstruction(QString)), &mySimGui, SLOT(invalidInstructionSlot(QString)));
    // Emits an updated accumulator signal
    connect(&myCPU, SIGNAL(updateAccumulator(QString)), &mySimGui, SLOT(updateAccumulatorSlot(QString)));
    // Emits an updated ProgramCounter signal
    connect(&myCPU, SIGNAL(updateProgramCounter(int)), &mySimGui, SLOT(updateProgramCounterSlot(int)));
    // Emits any change in the memory address, could be positive or negative
    connect(&myCPU, SIGNAL(changeStoredMemory(int, int)), &mySimGui, SLOT(changeStoredMemorySlot(int, int)));
    connect(&myCPU, SIGNAL(cacheChangeStoredMemory(InstructionData)), &mySimGui, SLOT(cacheChangeStoredMemorySlot(InstructionData)));
    // Emits a halt boolean to stop the program
    connect(&myCPU, SIGNAL(hasHalted(bool)), &mySimGui, SLOT(hasHaltedSlot(bool)));
    // Emits the string
    connect(&myCPU, SIGNAL(output(QString, int)), &mySimGui, SLOT(outputSlot(QString, int)));
    // Emits the break signal!
    connect(&myCPU, SIGNAL(breaked(int)), &mySimGui, SLOT(breakedSlot(int)));

    //~~~~~~~~ Signals that emit from the simGui class to the QObject (Model) class: ~~~~~~~~
    connect(&mySimGui, SIGNAL(loadCacheData()), &myCPU, SLOT(loadCacheDataSlot()));

    // ~~~~~~~~ Signals that emit from the SimGui (view 1) class to CacheGUI (view 2) class: ~~~~~~~~
    connect(&mySimGui, SIGNAL(flushCache()), &myCache, SLOT(flushCacheSlot()));
    connect(&mySimGui, SIGNAL(resetCache()), &myCache, SLOT(resetCacheSlot()));

    // ~~~~~~~~ Signals that emit between the QObject model class to  CacheGUI (view 2) class: ~~~~~~~~
    // the storeCalledSlot is called when a store instruction is read
    connect(&myCPU, SIGNAL(cacheDataReloaded(QMap<QString, InstructionData>)), &myCache, SLOT(cacheDataReloadedSlot(QMap<QString, InstructionData>)));

    connect(&myCPU, SIGNAL(updateHitCounter(int, int, bool)), &myCache, SLOT(updateHitCounterSlot(int, int, bool)));

    // The cacheSet signals are used when the cache settings were set or changed
    connect(&myCache, SIGNAL(cacheSet(int, int, bool)), &myCPU, SLOT(cacheSetSlot(int, int, bool)));
    connect(&myCache, SIGNAL(cacheSetForGui(bool)), &mySimGui, SLOT(cacheSetForGuiSlot(bool)));

    // ~~~~~~~~ Signals that emit from the cacheGui (view 2) class to SimGui (view 1) class: ~~~~~~~~
    connect(&myCache, SIGNAL(highlightHit(bool, bool)), &mySimGui, SLOT(highlightHitSlot(bool, bool)));

    // ~~~~~~~~ Signals that emit from the CacheGUI (view 1) class to this controller class: ~~~~~~~~
    connect(&mySimGui, SIGNAL(displayWidget()), this, SLOT(displayWidgetSlot()));

    // ~~~~~~~~ Signals that emit from the CacheGUI (view 2) class to this controller class: ~~~~~~~~
    connect(&myCache, SIGNAL(closeWidget()), this, SLOT(closeWidgetSlot()));

    mySimGui.show();
}

// ---------------- Start of Slot Definitions ----------------
void controller::displayWidgetSlot()
{
    myCache.show();
}

void controller::closeWidgetSlot()
{
    myCache.close();
}
