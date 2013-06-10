/*
 *    James Mead Dagres
 *    jdagres@vt.edu
 *    905748682
 *
 *    ECE 3574
 *    Homework 5
 *    04-07-13
 *
 *    CacheGui Class Implementation File
 */

#ifndef CACHEGUI_H
#define CACHEGUI_H

#include <QObject>
#include <QtGui>
#include <QWidget>
#include "centralprocessingunit.h"

namespace Ui {
class Cachegui;
}

class Cachegui : public QWidget
{
    Q_OBJECT

public:
    explicit Cachegui(QWidget *parent = 0);
    ~Cachegui();

private slots:

    /* THe flushCacheSlot is emitted to when the clear button is hit, it then
     * calles the clear function
     */
    void flushCacheSlot();

    /* The resetCacheSlot does the same as the flushCacheSlot, except it doesn't close the gui
     * as it calls reset instead of clear
     */
    void resetCacheSlot();

    /* The CacheSizeBox is where the user selects the size of the cache, for some strange reason
     * all dropdown box declaration must be under the private slots section in order to function properly
     */
    void on_cacheSizeSelectionBox_currentIndexChanged(const QString &arg1);

    /* The CacheLineSelectionBox is where the user selects the line size of the cache
     */
    void on_cacheLineBox_currentIndexChanged(const QString &arg1);

    /* The setCache button toggles the cache, this is important because if the values of the cache change
     * during program runtime the entire cache has to be deleted
     */
    void on_setCache_clicked();

    /* The updateHitCounterSlot receives the hitCounter and the program count, it then updates the LCD display
     * and then calls the updateRatio function which calculates the hit percentage and then updates that LCD display
     */
    void updateHitCounterSlot(int numberOfAttempts, int hitCount, bool hit);

    /* This slot is called by the CPU class whenever the cache has been updated, it pass the map with the structure
     * containing all of the needed variables
     */
    void cacheDataReloadedSlot(QMap<QString, InstructionData> cacheData);

signals:
    void closeWidget();
    //    void cacheUpdated(bool);
    // Sends the start signal, is used everytime the settings are toggled
    void cacheSet(int, int, bool);
    void cacheSetForGui(bool);
    void highlightHit(bool, bool);

private:
    Ui::Cachegui *ui;

    /* the void displaysCacheTable displays the table in the list view
     */
    void displaysCacheTable();

    /* The reset function resets all of the counters
     */
    void reset();

    /* The clear function resets all of the counters and all of the cache memory,
     * it is called when constructing or when a flush signal is emitted
     */
    void clear();

    // Closes the gui
    void closeGui();

    /* The displayData function determines if a file has been opened, and the cache line and memory
     * values have been selected, if any of those haven't been set then it doesn't display the data
     */
    void displayData(bool isSet);

    /* The columnSize() function uses private variables to return a stringList of the column
     */
    QStringList columnSize();

    /* The enableButtons function tells the buttons to either be enalbed or disabled depending
     * on the boolean value
     */
    void enableButtons(bool enabled);

    /* The resetButtons function resets the value of the dropdown boxes
     */
    void resetButtons();

    /* The updateRatio function recieves the numberOfAttempts and a boolean if the last instruction hit or not.
     * if the instruction did not hit then it determines if the last instruction needs to be added to the cache
     */
    void updateRatio(int numberOfTrys, int hitCount);

    /* Globalizes the QStandardItems for the three list views
     */
    QStandardItemModel *cacheDataTable;

    /* The cacheLineNumber and cachMemory are updated from the from the
     * dropdown box and are used to set the Cache Table
     */
    int numberOfCacheLines, cacheSize, numberOfCacheBits;

    /* The row count stores the number of rows inputred
     */
    int rowCount;

    // Tells the cache the button has been set
    bool cacheInitialized;
};

#endif // CACHEGUI_H
