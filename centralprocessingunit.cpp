/*
 *    James Mead Dagres
 *    jdagres@vt.edu
 *    905748682
 *
 *    ECE 3574
 *    Homework 5
 *    04-07-13
 *
 *    CentralProcessingUnit Class Implementation File
 *    Contains all of the # includes
 */

#include "centralprocessingunit.h"

CentralProcessingUnit::CentralProcessingUnit(QObject *parent) :
    QObject(parent)
{
    reset();
    cacheInitialized = false;
}

void CentralProcessingUnit::reset()
{
    programCounter = 0;
    accumulatorCounter = 0;
    hitCounter = 0;
    numberOfAttempts = 0;
    numberOfCacheLines = 0;
    cacheSize = 0;
    startLocation = 0;
    endLocation = 0;

    for(int i=0; i< 512; i++) // Construct an empty array
    {
        dataMemoryArray[i]=0;
    }

    resetStructure();
}

void CentralProcessingUnit::resetStructure()
{
    currentProcess.tag="";
    currentProcess.row="";
    currentProcess.cacheLine="";
    currentProcess.key = "";
    currentProcess.populatCounter=0; // Keeps track of the popular memory runs
    subtrahend = 0; // Data that will change
    currentProcess.cacheRowCount=0;
    currentProcess.startLocation=0;  // Start of memory block address
    currentProcess.endLocation=0;
    currentProcess.decimalLocation=0;
    currentProcess.cacheDataMemoryVector.clear();
    myCache.clear(); // Clear the map
}

void CentralProcessingUnit::load(QString instruction)
{
    QString hexAddress = instruction;
    hexAddress = hexAddress.remove(0,5); // removes "LOAD " from the string
    qDebug() << "Hex address" << hexAddress;

    bool conversion;
    // Converts the string address to hexidecimal int
    int addressBase10 = hexAddress.toInt(&conversion,16);

    if(addressBase10 < 512 && addressBase10 >= 0 && !hexAddress.isEmpty() && conversion)
    {
        if(cacheInitialized)
        {
            //            emit updateAccumulator(tempAccumulator);
            cacheBuild(addressBase10);
            cacheLoad();
        }
        else
        {
            accumulatorCounter = dataMemoryArray[addressBase10];
            returnAccumulator(accumulatorCounter);
        }
    }
    else
    {
        halt(instruction.append(" Error Loading.")); // Calls the halt function to emit the erorr and halt signal
    }
}

void CentralProcessingUnit::store(QString instruction)
{
    // Convert the instruction to the address
    QString address = instruction;
    address = address.remove(0,6); // removes store and space from the string
    //    qDebug() << " address" << address;

    // Converts the string address to hexidecimal int
    bool conversion;
    int addressBase10 = address.toInt(&conversion,16);

    if( addressBase10 <= 511 && addressBase10 >= 0 && !address.isEmpty() && conversion)
    {
        dataMemoryArray[addressBase10] = accumulatorCounter;

        QString tempAccumulator;
        tempAccumulator.setNum(accumulatorCounter);

        if(cacheInitialized)
        {
            cacheBuild(addressBase10);
            cacheStore();
        }
        else
        {
            // qDebug() << "Cache isn't initialized?" << cacheInitialized;
            emit changeStoredMemory(accumulatorCounter, addressBase10);
        }
    }
    else // Invalid input
    {
        halt(instruction.append(" Error Storing.")); // Calls the halt function to emit the erorr and halt signal
    }
}

void CentralProcessingUnit::addv(QString instructionAndData, int currentAccumulator)
{
    // Gets the sign and the number that comes after "ADDV "
    QString sign = QString(instructionAndData[5]), numberToAdd;

    // Make sure there is a number provided
    if( ((sign == "-" || sign == "+") && (instructionAndData.mid(6)).toInt()==0 ) // There is a sign
            || ((instructionAndData.mid(5)).toInt() == 0))  // there is no sign
    {
        halt(instructionAndData); // Calls the halt function to emit the erorr and halt signal
        return;
    }

    if(sign == "-")
    {
        numberToAdd = QString(instructionAndData.mid(6));
        currentAccumulator -= numberToAdd.toInt();
    }
    else if(sign == "+")
    {
        numberToAdd = QString(instructionAndData.mid(6));
        currentAccumulator += numberToAdd.toInt();
    }
    else // There was no sign given so it's positive
    {
        numberToAdd = QString(instructionAndData.mid(5));
        currentAccumulator += numberToAdd.toInt();
    }

    if(numberToAdd.toInt() <= -512 || numberToAdd.toInt() >= 512)
    {
        halt(instructionAndData); // Calls the halt function to emit the erorr and halt signal
        return;
    }

    returnAccumulator(currentAccumulator);
}

void CentralProcessingUnit::add(QString instruction)
{
    // removes add and the following space from the string, getting the address
    QString hexAddress = instruction;
    hexAddress = hexAddress.remove(0,4);

    // Converts the string address to hexidecimal int
    bool conversion;
    int addressBase10 = hexAddress.toInt(&conversion,16);

    // Makes sure an address was given, else calls the halt function to emit the erorr and halt signal
    if(hexAddress.isEmpty() || !conversion)
    {
        halt(instruction);
        return;
    }

    // Makes sure a proper Address was given
    if( addressBase10 <= 512 && addressBase10 >= 0)
    {
        if(cacheInitialized)
        {
            cacheBuild(addressBase10);
            cacheAdd();
        }
        else
        {
            int newAccumulator =  dataMemoryArray[addressBase10];
            newAccumulator = newAccumulator + accumulatorCounter;
            returnAccumulator(newAccumulator);
        }
    }
    else // Invalid input
    {
        // Calls the halt function to emit the erorr and halt signal
        halt(instruction.append(" is an invalid instruction."));
    }
}

void CentralProcessingUnit::bgtz(QString instruction)
{
    // removes "bgtz" and the following space from the string, getting the address
    QString tempDecimalAddress = instruction;
    tempDecimalAddress = tempDecimalAddress.remove(0,5);
    int decimalAddress = tempDecimalAddress.toInt();

    // Makes sure an address was given
    if(( tempDecimalAddress.isEmpty())||(decimalAddress > numberOfInstructions) || (decimalAddress < 0))
    {
        halt(instruction); // Calls the halt function to emit the erorr and halt signal
        return;
    }

    if(accumulatorCounter > 0) // If so then branch!
    {
        // qDebug() <<"decimalAddress loaded from" << decimalAddress << "newProgramCounterValue" << programCounter;
        programCounter = decimalAddress;
        emit updateProgramCounter(programCounter);
    }
    // else It's less than or equal to zero so increment, which the program does anyways
}

void CentralProcessingUnit::nop(QString instruction)
{
    // removes "NOP " from the string, getting the address
    QString testIfSomethingElse = instruction;
    testIfSomethingElse = testIfSomethingElse.remove(0,4);
    if (testIfSomethingElse != NULL)
    {
        halt(instruction);
    }
}

void CentralProcessingUnit::clear()
{
    returnAccumulator(0);
}

void CentralProcessingUnit::halt(QString invalidInstruction)
{
    badInstruction(invalidInstruction);
    emit hasHalted(true); // Program has halted
}

void CentralProcessingUnit::badInstruction(QString badInstruction)
{
    // If the cache has been initialized then emit the stored cache to the data memory
    if(cacheInitialized)
    {
        flushCache();
    }

    emit invalidInstruction(badInstruction);
}

void CentralProcessingUnit::flushCache()
{
    qDebug() << "Cache flush initiated"; // << myCache;

    // Traverse through the map and emit the stored cache to the data memory
    // Gets the size of the stored array in the structure inside the QMAP
    int j = 0;

    QMap<QString, InstructionData>::iterator mapIterator;
    for(mapIterator = myCache.begin(); mapIterator != myCache.end(); // && j < myCache.value(keys[j]).cacheDataMemoryVector.size();
        mapIterator++, j++)
    {
        emit cacheChangeStoredMemory(mapIterator.value());
    }
}
void CentralProcessingUnit::out(QString instruction)

{
    QString newString = instruction, tempNewString;
    newString = (newString.remove(0,4)).toUpper(); // removes "OUT " from the string
    // Creates a temp string to remove 4 characters from ("%A"), if that string is empty then it's invalid
    tempNewString = newString;

    // Gets rid of the parenthesis:
    newString = newString.remove(0,1); // Remove the " at the beginning and the end
    newString = newString.remove(newString.size()-1, 1);
    QStringList newStringList = newString.split("");

    for(int j =0; j < newStringList.size(); j++ )
    {   // Traverse through the string list to find where %A is
        if((QString(newStringList[j])== "%") && (QString(newStringList[j+1]) == "A") )
        {
            QString tempAccumulator;
            tempAccumulator = tempAccumulator.setNum(accumulatorCounter);

            /* It's j-1 because the beginning " was removed, the second parameter is 2 to replace
             * the 2 is there because you want to replace % and A
             */
            newString = newString.replace(j-1, 2, tempAccumulator + " ");
        }
    }
    emit output(newString, accumulatorCounter); // (QString, int)
}

void CentralProcessingUnit::loada()
{
    qDebug() << "accumulatorCounter before" << accumulatorCounter;

    if(cacheInitialized)
    {
        cacheBuild(accumulatorCounter);
        cacheLoad();
    }
    else
    {
        accumulatorCounter = dataMemoryArray[accumulatorCounter];
        qDebug() << "accumulatorCounter after" << accumulatorCounter;
        returnAccumulator(accumulatorCounter);
    }
}

void CentralProcessingUnit::storea(QString instruction)
{
    QString newString = instruction;
    newString = (newString.remove(0,7)).toUpper(); // removes "STOREA " from the string
    //    qDebug() << "accumulatorCounter: " << accumulatorCounter;

    bool conversion;
    // Converts the string address to hexidecimal int
    int addressBase10 = newString.toInt(&conversion,16);

    if( addressBase10 < 512 && addressBase10 >= 0 && !newString.isEmpty() && conversion) // && (accumulatorCounter >= 0))
    {
        int locationToStore = dataMemoryArray[addressBase10]; // Should probably check in the map if its stored there as well

        if(cacheInitialized)
        {
            cacheBuild(locationToStore);
            cacheStoreA(locationToStore);
        }
        else
        {
            emit changeStoredMemory(accumulatorCounter, locationToStore); // (int newValue, int newValuesPosition)
        }
        qDebug() << "valueToStore: " << locationToStore;
    }
    else // Invalid input
    {
        halt(instruction.append(" Errror in STOREA"));
    }
}

void CentralProcessingUnit::returnAccumulator(int newAccumulator)
{
    QString error;
    accumulatorCounter = newAccumulator;

    if(accumulatorCounter > 511)
    {
        error = "Accumulator Overflow! Reset to -512";
        accumulatorCounter = -512;
        badInstruction(error);
    }
    else if(accumulatorCounter < -512)
    {
        error = "Accumulator Overflow! Reset to 511";
        accumulatorCounter = 511;
        badInstruction(error);
    }

    QString tempAccumulator;
    tempAccumulator.setNum(accumulatorCounter);
    qDebug() << "New Accumulator" << tempAccumulator;
    emit updateAccumulator(tempAccumulator);
}

void CentralProcessingUnit::cacheBuild(int decimalLocation)
{
    //    qDebug() << "decimalLocation" << decimalLocation;
    numberOfAttempts = numberOfAttempts + 1; // Increase the attempt counter

    if(!cacheInitialized || numberOfCacheLines == 0)
    {
        qDebug() << "Cache not initialized";
        return;
    }

    // Fills in missing zero's from the decimal address
    QString tempTag(QString::number(decimalLocation,2));
    tempTag = tempTag.rightJustified(9, '0');

    currentProcess.tag = getTag(tempTag);
    currentProcess.row = getRow(tempTag);
    currentProcess.cacheLine = getCacheLineValue(tempTag);
    subtrahend = decimalLocation % numberOfCacheLines;

    startLocation = decimalLocation - subtrahend;
    currentProcess.startLocation = startLocation;
    endLocation = currentProcess.startLocation + numberOfCacheLines;
    currentProcess.endLocation = endLocation;
    qDebug() << "startLocation: " << currentProcess.startLocation << "endLocation " << currentProcess.endLocation;

    currentProcess.decimalLocation = decimalLocation;
    QString temp = currentProcess.tag;
    currentProcess.key = temp.append(currentProcess.row);
    // qDebug() << "Tempkey" <<     currentProcess.key;

    // Resizes the vector to the number of column bits, this prevents out of range errors
    currentProcess.cacheDataMemoryVector.resize(numberOfCacheLines);
}

void CentralProcessingUnit::cacheBuildHelper(InstructionData newProcess)
{
    QString tempRemovedTag = "";
    // Create a counter to roll through everypossible iteration
    int unpopularCount = 999999;
    // Gets the max number of rows in the cache
    int cacheRows = cacheSize/numberOfCacheLines;

    if((cacheRows-1) < myCache.size()) // If the cache is full
    {
        QMap<QString, InstructionData>::iterator mapIterator;
        QList<QString> keyList = myCache.keys();
        int j = 0;
        for(mapIterator = myCache.begin(); mapIterator != myCache.end(); mapIterator++)
        {
            if(myCache.value(keyList[j]).populatCounter <  unpopularCount)
            {
                tempRemovedTag = mapIterator.key();
                unpopularCount = myCache.value(keyList[j]).populatCounter;
            }
        }
        int startLocationToRemove = myCache.value(tempRemovedTag).startLocation;

        // Stores the memory address about ot be removed
        for(int j = 0; startLocationToRemove < myCache.value(tempRemovedTag).endLocation;  j++, startLocationToRemove++)
        {
            dataMemoryArray[newProcess.startLocation] = myCache.value(tempRemovedTag).cacheDataMemoryVector[j];
            qDebug() << "Vector to remove inside buildhelper" << myCache.value(tempRemovedTag).cacheDataMemoryVector[j];
        }
        // Sends the removed data to the simgui before removing
        emit cacheChangeStoredMemory(myCache.value(tempRemovedTag));
        //  emit changeStoredMemory(accumulatorCounter, currentProcess.startLocation);
        qDebug() << "tempRemovedTag: " << tempRemovedTag << "successfully removed";

        myCache.remove(tempRemovedTag);
    }
    // Updates the map with the new data
    QString tempKey = newProcess.tag;
    newProcess.key = tempKey.append(newProcess.row);

    myCache.insert(newProcess.key, newProcess);
    emit cacheDataReloaded(myCache);

    currentProcess.cacheDataMemoryVector.clear();
}

void CentralProcessingUnit::cacheHelper()
{
    currentProcess.dirtyBit = "0"; // Dirty bit is 0 for anything that isn't storing
    if(myCache.contains(currentProcess.key)) // If it's a hit
    {
        currentProcess = myCache.value(currentProcess.key);
        // Increase the hit counters
        hitCounter = hitCounter + 1;
        currentProcess.populatCounter = currentProcess.populatCounter + 1;

        accumulatorCounter = (currentProcess.cacheDataMemoryVector[subtrahend]);
        returnAccumulator(accumulatorCounter);
        emit updateHitCounter(numberOfAttempts, hitCounter, true);
    }
    else // It's a miss
    {
        for(int j=0; startLocation <= endLocation; j++, startLocation++)
        {
            if(subtrahend == j)
            {
                // qDebug() << "currentProcess.cacheDataMemoryVector" << currentProcess.cacheDataMemoryVector;
                accumulatorCounter = currentProcess.cacheDataMemoryVector[subtrahend]; // Breaks here
                returnAccumulator(accumulatorCounter);
            }
            // Insert the start location in every spot
            currentProcess.cacheDataMemoryVector.insert(j, dataMemoryArray[startLocation]);
        }

        emit updateHitCounter(numberOfAttempts, hitCounter, false);
        currentProcess.populatCounter = 0;
    }
    cacheBuildHelper(currentProcess);
}

void CentralProcessingUnit::cacheStore()
{
    currentProcess.dirtyBit = "1"; // Dirty bit is always one for storing
    if(myCache.contains(currentProcess.key)) // If it's a hit
    {
        currentProcess = myCache.value(currentProcess.key);
        // Increase the hit counters
        hitCounter = hitCounter + 1;
        currentProcess.populatCounter = currentProcess.populatCounter + 1;

        currentProcess.cacheDataMemoryVector[subtrahend] = accumulatorCounter;
        emit updateHitCounter(numberOfAttempts, hitCounter, true);
    }
    else // It's a miss
    {
        for(int j=0; startLocation <= endLocation; j++, startLocation++)
        {
            if(subtrahend == j)
            {
                currentProcess.cacheDataMemoryVector.insert(j,accumulatorCounter);
            }
            else
            {
                currentProcess.cacheDataMemoryVector.insert(j,dataMemoryArray[startLocation]);
            }
        }
        currentProcess.cacheRowCount = currentProcess.cacheRowCount - 1;

        emit updateHitCounter(numberOfAttempts, hitCounter, false);
        currentProcess.populatCounter = 0;
    }
    cacheBuildHelper(currentProcess);
}

void CentralProcessingUnit::cacheStoreA(int decimalLocation)
{
    currentProcess.dirtyBit = "1"; // Dirty bit is always one for storing

    QString tempValue(QString::number(decimalLocation,2));
    tempValue = tempValue.rightJustified(9, '0');

    if(myCache.contains(tempValue)) // If it's a hit
    {
        decimalLocation = myCache.value(tempValue).cacheDataMemoryVector[subtrahend];
    }
    else // It wasn't found
    {
        decimalLocation = dataMemoryArray[subtrahend];
    }

    if(decimalLocation < 512 && decimalLocation >= 0)
    {
        // If the address is in range then call the cacheStore function
        cacheStore();
    }
    else // There was an error! call the halt function
    {
        halt("STOREA");
    }
}

void CentralProcessingUnit::cacheLoad()
{
    cacheHelper();
}

void CentralProcessingUnit::cacheLoadA()
{
    cacheHelper();
}

void CentralProcessingUnit::cacheAdd()
{
    cacheHelper();
}

QString CentralProcessingUnit::getTag(QString tempTag)
{
    int numberOfCacheLineBits = getCacheLineBits();
    int numberOfCacheSizeBits = getCacheSizeBits();
    int rowSize = numberOfCacheSizeBits - numberOfCacheLineBits;
    int tagSize = 9 - rowSize - numberOfCacheLineBits;

    tempTag = tempTag.left(tagSize);

    //    qDebug() << "retuned tag" << tempTag;
    return tempTag;
}

QString CentralProcessingUnit::getRow(QString tempString)
{
    int numberOfCacheLineBits = getCacheLineBits();
    int numberOfCacheSizeBits = getCacheSizeBits();
    // qDebug() << "numberOfCacheLineBits " << numberOfCacheLineBits << "numberOfCacheSizeBits" << numberOfCacheSizeBits;
    int rowSize = numberOfCacheSizeBits - numberOfCacheLineBits;
    int tagSize = 9 - rowSize - numberOfCacheLineBits;

    tempString = tempString.remove(0, tagSize); // Removes the tag from the string
    tempString = tempString.left(rowSize);

    // qDebug() << "Returned row" << tempString;
    return tempString;
}

QString CentralProcessingUnit::getCacheLineValue(QString tempString)
{
    int numberOfCacheLineBits = getCacheLineBits();

    while( tempString.size() > numberOfCacheLineBits)
    {
        tempString = tempString.remove(0, 1);
    }

    qDebug() << "getCacheLineValue after" << tempString;

    return tempString;
}

int CentralProcessingUnit::getCacheLineBits()
{
    int numberOfCacheBits = 0;

    if(2 == numberOfCacheLines)
    {
        numberOfCacheBits = 1;
    }
    else if(4 == numberOfCacheLines)
    {
        numberOfCacheBits = 2;
    }
    else if(8 == numberOfCacheLines)
    {
        numberOfCacheBits = 3;
    }
    else
    {
        qDebug() << "ERROR! Invalid Line Size";
    }

    return numberOfCacheBits;
}

int CentralProcessingUnit::getCacheSizeBits()
{
    int numberOfCacheBits = 0;

    if(64 == cacheSize)
    {
        numberOfCacheBits = 6;
    }
    else if(32 == cacheSize)
    {
        numberOfCacheBits = 5;
    }
    else if(16 == cacheSize)
    {
        numberOfCacheBits = 4;
    }
    else
    {
        qDebug() << "ERROR! Invalid Cache Size";
    }

    return numberOfCacheBits;
}

QString CentralProcessingUnit::instructionProcessHelper(QString instruction, int currentAccumulator, int currentProgramCounter)
{
    qDebug() << "Current Instruction: " << instruction; // For debugging purposes

    accumulatorCounter = currentAccumulator; // Updates the stored counter to the newest value
    programCounter = currentProgramCounter;

    // qDebug() << "Stored breakLine: " << breakPointLine << " Current Program Count: " << programCounter;
    // Creates a stringlist of the instruction parameter, then splits it by a space
    QStringList instructionList = instruction.split(' ');
    // instructionDefition contains the first half of the string before the space
    QString instructionDefinition = QString(instructionList[0]).toUpper();

    return instructionDefinition;
}

// ~~~~~~~~~~ Start of Slot Definitions ~~~~~~~~~~

void CentralProcessingUnit::instructionProcessSlot(QString instruction, int currentAccumulator, int currentProgramCounter)
{
    if(programCounter == breakPointLine && breakPointLine != 0)
    {
        emit breaked(breakPointLine);
        return;
    }

    QString instructionDefinition = instructionProcessHelper(instruction, currentAccumulator, currentProgramCounter);

    if(instructionDefinition == "LOAD")
    {
        load(instruction);
    }
    else if(instructionDefinition == "STORE")
    {
        store(instruction);
    }
    else if(instructionDefinition == "ADDV")
    {
        addv(instruction, currentAccumulator);
    }
    else if(instructionDefinition == "ADD")
    {
        add(instruction);
    }
    else if(instructionDefinition ==  "BGTZ")
    {
        bgtz(instruction);
    }
    else if(instructionDefinition ==  "NOP")
    {
        nop(instruction);
    }
    else if(instructionDefinition == "CLEAR")
    {
        clear();
    }
    else if(instructionDefinition == "HALT")
    {
        halt("HALT INSTRUCTION GIVEN");
    }
    else if(instructionDefinition.left(4) == "OUT")
    {
        out(instruction);
    }
    else if(instructionDefinition == "LOADA")
    {
        loada();
    }
    else if(instructionDefinition == "STOREA")
    {
        storea(instruction);
    }
    else  // Calls the halt function to emit the erorr and halt signal
    {
        halt(instruction);
        return;
    }
    programCounter++;
}

void CentralProcessingUnit::instructionResetSlot()
{
    if(cacheInitialized == false)
    {
        reset();
    }
}

void CentralProcessingUnit::instructionOpenedSlot(int instructionNumber)
{
    numberOfInstructions = instructionNumber;
}

void CentralProcessingUnit::breakPointSlot(int breakLine)
{
    breakPointLine = breakLine;
    // qDebug() << "breakPointLine Set at: " << breakPointLine;
}

void CentralProcessingUnit::cacheSetSlot(int setCacheLines, int setCacheSize, bool set)
{
    cacheInitialized = set;
    // qDebug() << "cache is Initialized?: " << set;
    if(!set)
    {
        numberOfCacheLines = 0;
        cacheSize = 0;
        return;
    }
    numberOfCacheLines = setCacheLines;
    cacheSize = setCacheSize;
}

void CentralProcessingUnit::loadCacheDataSlot()
{
    // If the cache has been initialized then emit the stored cache to the data memory
    if(cacheInitialized)
    {
        flushCache();
    }
}
