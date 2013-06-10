/*
 *    James Mead Dagres
 *    jdagres@vt.edu
 *    905748682
 *
 *    ECE 3574
 *    Homework 5
 *    04-07-13
 *
 *    CentralProcessingUnit Class Header File
 */

#ifndef CENTRALPROCESSINGUNIT_H
#define CENTRALPROCESSINGUNIT_H

#include <iomanip>
#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QtGui/QTextEdit>
#include <QtGui>
#include <QList>
#include <QMainWindow>
#include <QMap>
#include <QObject>
#include <QStack>
#include <QString>
#include <QTableView>
#include <QTextEdit>
#include <QWidget>
#include <QTextStream>

/* Data struct to be passed, a structure is used so that multiple values
 * can be passed and emitted between functions and classes
 */
struct InstructionData
{
    QString tag;
    QString row;
    QString cacheLine;
    QString dirtyBit;
    QString key;
    int populatCounter; // Keeps track of the popular memory runs
    int decimalLocation;
    int cacheRowCount;
    int startLocation;  // Start of memory block address
    int endLocation;
    QVector<int> cacheDataMemoryVector;
};

class CentralProcessingUnit : public QObject
{
    Q_OBJECT
public:
    explicit CentralProcessingUnit(QObject *parent = 0);

private:

    /* the reset function clears all of the counters and arrays and is called
     * by the constructor and anytime the values need to reset
     */
    void reset();

    /* Resets the values of the above structure
     */
    void resetStructure();

    /* The load function loads the value from the speficied memory
     * addres <hex-data-address> and loads it into the accumulator
     */
    void load(QString instruction);

    /* The store function stores the current value in the accumulator
     * to a Hex memory address in the memory, the address given are
     * all <hex-data-address>
     */
    void store(QString instruction);

    /* The addv function adds the signed decimal number to the accumulator
     * from there it emmits the new accumulator number to the gui class
     * in order to update the gui
     */
    void addv(QString numberToAdd, int currentAccumulator);

    /* The ADD function adds the current accumulator value with the value
     * stored in the passed hex address
     */
    void add(QString instruction);

    /* the bgtz or branch on greater than zero function sets the Program Counter
     * to the value given but the decimal instruction address if the value of
     * the accumulator is greater than 0. If the value is less than or eqaul
     * to zero then the count counter is incremented by one
     */
    void bgtz(QString instruction);

    /* increments program counts
     */
    void nop(QString instruction);

    // The clear function simply resest the value in the accculmulator
    void clear();

    /* the halt function emits the signal to disable the program
     */
    void halt(QString invalidInstruction);

    /* BadInstruction halts the program, and flushes the cache if it's enabled
     */
    void badInstruction(QString badInstruction);

    /* The flushCache function is called whenever the program halts and emits all
     * of the data stored in the cache to the data memory and displays it on the sim gui
     */
    void flushCache();

    /* The out function outputs a string to standard output,
     * using a special symbol to pring the value of the accumulator
     */
    void out(QString instruction);

    /* The loada function takes the data stored in the memory address of
     * the accumulators current value, and takes that data and sets the
     * accumulator to that value
     */
    void loada();

    /* The storea function takes the decimal value stored at a data memory addres call it "A"
     * listed after STOREA. It then takes that value "A" as an address itself, it then
     * stores the current accumulator value in the "A"th address
     */
    void storea(QString newAccumulator);

    /* The return accumulator converts the int to a Qstring to emit and checks
     * that it's in range it then emits to the gui class with the newAccumulator
     */
    void returnAccumulator(int newAccumulator);

    /* The cacheBuild updates the structure with all of the data, it takes the current address
     * and instruction and gets the tag, row, cache and size information.
     */
    void cacheBuild(int decimalAdress);

    // the cacheBuildHelper function inserts the data into a map and emits to the gui
    void cacheBuildHelper(InstructionData newBlock);

    // the cacheHelper function is almost the same as cacheBuild and is called my cacheAdd, cacheLoad and cacheLoadA
    void cacheHelper();

    /* The cacheStore function is called by the cacheStore and adds it to the map it determines if change
     * in data the cacheStore
     */
    void cacheStore();
    void cacheStoreA(int decimalAddreas);

    // Similar to the cacheStore function but with load and loadA functionality
    void cacheLoad();
    void cacheLoadA();

    // Similar to the cacheStore function but with add functionality
    void cacheAdd();

    /* The getTag function is called when a tag needs to be received. It determins the tag size
     * then based on the selected cache size and cache line size determines and returns the tag
     */
    QString getTag(QString addressValue);

    /* The getRow function is called when a tag needs to be received. It determins the tag size
     * then based on the selected cache size and cache line size determines and returns the row
     */
    QString getRow(QString address);
    QString getCacheLineValue(QString address);

    /* The getCacheLineBits is called to return the number of cache bits which are based on the line size
     * cacheLine bits = x; where x is 2^x = cache line size
     */
    int getCacheLineBits();

    /* The getCacheLineBits is called to return the number of cache bits which are based on the cache size
     * cacheSize bits = x; where x is 2^x = cache size
     */
    int getCacheSizeBits();

    /* The instructionProcessHelper helps process the function, it checks to see if the current line is a breakpoint
     * it splits the instruction by spaces to process the instruction string, and returns that split string
     */
    QString instructionProcessHelper(QString instruction, int currentAccumulator, int currentProgramCounter);

    /* The next three strings globaly store the value of the file name
     * This enables the file to be called throughout the class
     */
    QString fileNameStored, m_FileName;

    /* In order for the QObject functions to manipute address values they must
     * store an array and pass it back and forth. The next two lines contain the arrays
     * of the instructions and the registry data. Needs to be allocated to 128 and 512,
     * because the null character counts as one position
     */
    int dataMemoryArray[512];

    /* The qmap cacheTag holds all of the cacheData
     *
     */
    QMap<QString, InstructionData> myCache;

    /* instructionData type of the structure
     */
    InstructionData currentProcess;

    /* The programCounter is the variable that keeps tracks of the iterations through the max 128
     * commands, it is reset everytime a program is ran and is
     * maintained through the stepThroughButton and runAllButton.
     */
    int programCounter;

    /* The accumulatorCounter is the variable that keeps tracks of the iterations through the max
     * 512 data locations, it is reset everytime a program is ran and is
     * maintained through the stepThroughButton and runAllButton.
     */
    int accumulatorCounter;

    /* The numberOfInstructions tracks how many instructions there and ensure that BGTZ doesn't try
     * to branch to an instruction that doesn't exist
     */
    int numberOfInstructions;

    /* The int breakPointLine variable stores the line the breakpoint is at, and everytime an
     * instruction is emitted the programCounter is compared to this value
     */
    int breakPointLine;

    /* The hitCounter counts the number of hits, a hit is when the read instruction is matched to
     * a value stored in the cache, if it is a hit then it's highlighted in green
     */
    int hitCounter;

    // Counts the number of times the cache was searched
    int numberOfAttempts;

    /* The cacheLineNumber and cachMemory are updated from the from the
     * dropdown box and are used to set the Cache Table
     */
    int numberOfCacheLines, cacheSize;

    /* The cacheInitialized variable is set when both the cache line size and cache size are
     * are selected, if it is false then the program does not run any of the cache functionality
     */
    bool cacheInitialized;

    // The data to be changed
    int subtrahend;

    /* The start and end location have to be made private variables outside of the structure variables in order to
     * change their value and use them to iterate through, where the structure values need to retain their original values
     */
    int startLocation, endLocation;

signals: // See definitions in the related slot
    void invalidInstruction(QString badInput);
    void updateAccumulator(QString newAccumulator);
    void updateProgramCounter(int newProgramCounter);
    void changeStoredMemory(int accumulator, int hexAddress);
    void cacheChangeStoredMemory(InstructionData block);
    void hasHalted(bool hasHalted);
    void output(QString outputString, int accumulator);
    void breaked(int);
    // ------ Signals to the View 2 Cache ------
    // Sends the programInstruction and the hitCount
    void updateHitCounter(int, int, bool);
    void cacheDataReloaded(QMap<QString, InstructionData>);

private slots:
    // Receives the file and ensures it exists and gets the number of instructions
    void instructionOpenedSlot(int instructionNumber);

    /* the instructionProcess slot takes each instruction and determines which instruction
     * was inputted or if it was an invalid instruction, from there it
     * and distributes it to other QObject functions, or emits it back via a signal
     * to a function in the gui class to update the gui
     */
    void instructionProcessSlot(QString instruction, int currentAccumulator, int currentProgramCounter);

    // instructionResetSlot resets all the values by calling the reset function
    void instructionResetSlot();

    // The breakPointSlot stores the breakLine in the global breakPointLine variable
    void breakPointSlot(int breakLine);

    /* The cacheSetSlot slot is emitted to when the cache has been set,
     * it stores the cacheLine and Cachesize and the bool set
     */
    void cacheSetSlot(int setCacheLines, int setCacheSize, bool set);

    /* the loadCacheDataSlot is emitted when every instruction is run and the cache needs
     * to be flushed.
     */
    void loadCacheDataSlot();

public slots: // None!

};

#endif // CENTRALPROCESSINGUNIT_H
