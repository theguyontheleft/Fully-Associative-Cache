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

#include "cachegui.h"
#include "ui_cachegui.h"
#include <QtGui>

Cachegui::Cachegui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Cachegui)
{
    cacheInitialized = false;
    ui->setupUi(this);
    clear(); // Sets the counters
}

Cachegui::~Cachegui()
{
    delete ui;
}

void Cachegui::reset()
{
    numberOfCacheLines = 0;
    cacheSize = 0;
    numberOfCacheBits = 0;
    cacheInitialized = false;
    emit cacheSet(numberOfCacheLines, cacheSize, false);
}

void Cachegui::clear()
{
    // See if the table has been initialized
    if(cacheInitialized)
    {
        cacheDataTable->clear();
    }

    // Resets the selection of the dropdown boxes, and LCD displays
    reset();
    enableButtons(true);
    resetButtons();
}

void Cachegui::closeGui()
{
    emit closeWidget();
}

void Cachegui::displaysCacheTable()
{
    int rowCount = cacheSize/numberOfCacheLines, columnCount;
    QStringList columns = columnSize();
    columnCount = columns.size()+2;

    // Make the Cache List View Table:
    cacheDataTable = new QStandardItemModel(rowCount,columnCount,this);
    cacheDataTable->setHorizontalHeaderItem(0, new QStandardItem(QString("Dirty Bit")));
    cacheDataTable->setHorizontalHeaderItem(1, new QStandardItem(QString("Tag")));
    cacheDataTable->setHorizontalHeaderItem(2, new QStandardItem(QString("Row")));

    int tempCount = 3;
    for(int j = 0; j < columns.size(); j++)
    {
        cacheDataTable->setHorizontalHeaderItem(tempCount, new QStandardItem(QString(columns[j])));
        tempCount++;
    }

    ui->cacheDataTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->cacheDataTable->setModel(cacheDataTable);

    //    qDebug() << "displayscacheDataTable done";
}

QStringList Cachegui::columnSize()
{
    QStringList columns;
    for(int j= 0; j < numberOfCacheLines; j++)
    {
        QString bit = QString::number(j,2);
        bit = bit.rightJustified(numberOfCacheBits,'0');
        columns.append(bit);
    }
    return columns;
}

void Cachegui::displayData(bool isSet)
{
    if(!isSet) // If it's not set then clear
    {
        reset();
        return;
    }

    displaysCacheTable();
}

void Cachegui::on_cacheLineBox_currentIndexChanged(const QString &arg1)
{
    // qDebug() << "Cache line changed to: "<< arg1;
    int tempArg = arg1.toInt(), tempCacheSize = cacheSize;
    if(arg1 == "Select")
    {
        reset();
        numberOfCacheLines= 0;
        cacheSize = tempCacheSize; // Restore the cacheSize before the reset
        qDebug() << "Cache Line Number Not Selected";
        return;
    }
    if(2 == tempArg)
    {
        numberOfCacheBits = 1;
    }
    else if(4 == tempArg)
    {
        numberOfCacheBits = 2;
    }
    else if(8 == tempArg)
    {
        numberOfCacheBits = 3;
    }
    else if((2 != tempArg) && (4 != tempArg) && (8 != tempArg))
    {
        reset();
        qDebug() << "Cache Line Number was invalid?: "<< tempArg;
        return;
    }

    numberOfCacheLines = tempArg;

    //    qDebug() << "TempArg "<< tempArg << "numberOfCacheLines: " << numberOfCacheLines;
}

void Cachegui::on_cacheSizeSelectionBox_currentIndexChanged(const QString &arg1)
{
    // qDebug() << "Cache size changed to: "<< arg1;

    int tempArg = arg1.toInt(), tempNumberOfCacheLines = numberOfCacheLines;
    if(arg1 == "Select")
    {
        reset();
        numberOfCacheBits = 0;
        numberOfCacheLines = tempNumberOfCacheLines;
        // qDebug() << "Cache Size Not Selected";
        return;
    }
    else if( (16 != tempArg) && (32 != tempArg) && (64 != tempArg) )
    {
        qDebug() << "Cache Size Number was invalid?: "<< tempArg;
        return;
    }

    cacheSize = tempArg;
}

void Cachegui:: on_setCache_clicked()
{
    // Toggles the dropdownbox off and on
    if(cacheSize != 0 && numberOfCacheLines != 0 && ui->cacheLineBox->isEnabled()
            && ui->cacheSizeSelectionBox->isEnabled())
    {
        enableButtons(false);
        cacheInitialized = true;
        displayData(true);
        // qDebug() << "Cache Enabled";
    }
    else
    {
        resetButtons();
        cacheInitialized = false;

        // If the table has been set the clear it
        const QModelIndex cacheDataTableIndex;

        if(cacheDataTable->hasIndex(1,0,cacheDataTableIndex))
        {
            cacheDataTable->clear();
        }
        enableButtons(true);
        qDebug() << "Cache Disabled";
    }
    emit cacheSet(numberOfCacheLines, cacheSize, cacheInitialized);
    emit cacheSetForGui(cacheInitialized);
}

void  Cachegui::enableButtons(bool enabled)
{
    ui->cacheSizeSelectionBox->setEnabled(enabled);
    ui->cacheLineBox->setEnabled(enabled);
}

void Cachegui::resetButtons()
{
    ui->cacheSizeSelectionBox->setCurrentIndex(0);
    ui->cacheLineBox->setCurrentIndex(0);
    this->ui->hitRatioLCD->display(0);
    this->ui->hitCounterLCD->display(0);
    this->ui->missesLCD->display(0);
}

void Cachegui::updateRatio(int numberOfTrys, int hitCount)
{
    qDebug() << "numberOfTrys" << numberOfTrys << "hitCount" << hitCount;
    double hitRatio;
    int misses = numberOfTrys-hitCount;

    if(numberOfTrys > 0) // Ensures there's no division of zero
    {
        hitRatio = 100 * hitCount/numberOfTrys;
    }
    else
    {
        hitRatio = 0;
    }

    this->ui->hitRatioLCD->display(hitRatio);

    if(misses > 0)
    {
        this->ui->missesLCD->display(misses);
    }
}

// ---------------- Start of Slot Definitions ----------------

void Cachegui::flushCacheSlot()
{
    clear();
    closeGui();
}

void Cachegui::resetCacheSlot()
{
    clear();
}

void Cachegui::updateHitCounterSlot(int numberOfAttempts, int hitCount, bool hit)
{
    updateRatio(numberOfAttempts, hitCount);
    this->ui->hitCounterLCD->display(hitCount);

    // Emits the highlight
    emit highlightHit(hit, true);
}

void Cachegui::cacheDataReloadedSlot(QMap<QString, InstructionData> cacheData)
{
    rowCount++;

    // Make the Cache List View Table:
    displaysCacheTable();
    int rowIndex = cacheSize/numberOfCacheLines;

    // Clear all of the present rows
    if(rowCount+1 > rowIndex)
    {
        for( int i = 0; i < rowIndex; i++ )
        {
            cacheDataTable->removeRow(i);
        }
    }

    // Gets the size of the stored array in the structure inside the QMAP
    int j = 0;
    // Uses QList and a map iterator to iterate through the map and the structure
    QList<QString> keys = cacheData.keys();
    QMap<QString, InstructionData>::iterator mapIterator;
    for(mapIterator = cacheData.begin(); mapIterator != cacheData.end(); mapIterator++, j++)
    {
        QStandardItem *dirtyBitItem = new QStandardItem(cacheData.value(keys[j]).dirtyBit);
        cacheDataTable->setItem(j, 0,  dirtyBitItem);
        QStandardItem *tempItem, *rowItem = new QStandardItem(cacheData.value(keys[j]).row);
        cacheDataTable->setItem(j, 2,  rowItem);

        // Put in current data into table
        for(int k = 0; k < cacheData.value(keys[j]).cacheDataMemoryVector.size()-1 && k < numberOfCacheLines; k++)
        {
            tempItem = new QStandardItem(QString::number(cacheData.value(keys[j]).cacheDataMemoryVector.at(k), 10));
            cacheDataTable->setItem(j,k+3, tempItem);
        }

        QStandardItem *tagItem = new QStandardItem(QString(cacheData.value(keys[j]).tag));
        cacheDataTable->setItem(j, 1, tagItem);
    }
}
