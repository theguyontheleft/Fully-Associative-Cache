/*
 *    James Mead Dagres
 *    jdagres@vt.edu
 *    905748682
 *
 *    ECE 3574
 *    Homework 5
 *    04-07-13
 *
 *    ComputerSimGUI Class Implementation File
 */

#include "computersimgui.h"
#include "ui_computersimgui.h"

ComputerSimGUI::ComputerSimGUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ComputerSimGUI)
{
    ui->setupUi(this);
    notFirst = false;
    clear();
    notFirst = true;
    // fileOpened = false;
}

ComputerSimGUI::~ComputerSimGUI()
{
    delete ui;
}

void ComputerSimGUI::on_clearButton_released() // Or reset
{
    clear();
    statusBar()->showMessage(tr("Program Cleared"), 3000);
}

void ComputerSimGUI::on_resetButton_released() // Or reset
{
    // Flsh the cache the program halted
    if(programCounter != 0)
    {
        emit resetCache();
    }
    reset();
    statusBar()->showMessage(tr("Program Reset"), 3000);
}

void ComputerSimGUI::clear()
{
    // Clears the selected file
    fileName ="";
    fileNameStored ="";
    m_FileName ="";
    fileOpened = false;

    reset();

    if(notFirst)
    {
        instructionMemoryView->clear();
        emit flushCache();
    }
}

void ComputerSimGUI::reset()
{
    // Resets all of the counters
    stepCount = 0;
    programCounter = 0;
    accumulatorCounter = 0;
    errorCounter = 0;
    halted = false;
    hitCache = false;
    cacheSet = false;
    cacheFunction = false;

    // Reset all of the GUI windows to the defaults
    if(fileOpened && notFirst)
    {
        displaysTableCells();
        defaultPopulateDataListView(); // seg 1
        displayAllInstructions(); // seg 2
        // Clears all 128 instructions
    }
    else
    {
        // Disables the buttons
        enableButtons(false);
        return;
    }

    // Reset the LCD displays and drop downbox
    this->ui->programCounterlcdNumber->display(programCounter);
    this->ui->accumulatorlcdNumber->display(accumulatorCounter);

    // Re-enables the buttons except the breakLineSpinBox
    enableButtons(true);
    ui->breakLineSpinBox->setEnabled(false);
    notFirst = true;

    emit instructionReset(); // Reset QObject counters
}

void ComputerSimGUI::enableButtons(bool enabled)
{
    ui->stepThroughButton->setEnabled(enabled);
    ui->runAllButton->setEnabled(enabled);
    ui->breakLineSpinBox->setEnabled(enabled);
    ui->addBreakPointButton->setEnabled(enabled);
}

void ComputerSimGUI::openFile()
{
    // Uses QDileDialog to browse the files
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
    {
        loadFile(fileName);

        bool temp = true;
        fileOpened = temp;
    }
}

void ComputerSimGUI::loadFile(const QString &fileName)
{
    QFile file(fileName);
    // Ensures that the file was successfully opened
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Simple Mian Window Example"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName).arg(file.errorString()));
        return;
    }

    QString tempFileName = fileName;
    fileNameStored = tempFileName;
    bool tempFileOpened = true;
    fileOpened = tempFileOpened;

    // Create and show the new widget
    emit displayWidget();
    //    emit createCacheGui();

    reset(); // Reset because a new file was opened
    statusBar()->showMessage(tr("File Opened"), 2000);
}

void ComputerSimGUI::setCurrentFile(const QString &fileName)
{   // Gives you global control of the filename
    m_FileName = fileName;
}

void ComputerSimGUI::on_actionOpen_triggered()
{   // If the open file button was hit then call the openfile function
    openFile();
}

void ComputerSimGUI::on_runAllButton_released()
{
    if(!fileOpened) // Ensure a file has been selected
    {
        qDebug() << "Please open the instruction file first";
        statusBar()->showMessage(tr("Please Open The Instruction File First"), 3000);
        return;
    }
    else if (stepCount >= 127) // Ensures it doesn't go beyond the max number of instructions in the file
    {
        if(cacheSet) // Emits to the CPU class to release the cache
        {
            emit loadCacheData();
        }

        statusBar()->showMessage(tr("Instructions Completed"), 3000);
        return;
    }

    // Displays the instruction memory and instructions
    runAllInstructions();

    this->ui->programCounterlcdNumber->display(programCounter); // Update the counter in the GUI
    this->ui->accumulatorlcdNumber->display(accumulatorCounter);
}

void ComputerSimGUI::on_stepThroughButton_released()
{
    if(!fileOpened)
    {
        qDebug() << "Please open the instruction file first";
        statusBar()->showMessage(tr("Please Open The Instruction File First"), 3000);
        return;
    }
    else if (stepCount >= 128)
    {
        if(cacheSet) // Emits to the CPU class to release the cache
        {
            emit loadCacheData();
        }
        enableButtons(false);
        statusBar()->showMessage(tr("Instructions Completed"), 3000);
        return;
    }
    else if(stepCount == breakPointLine)
    {
        resetBreakPoint();
        statusBar()->showMessage(tr("Breakpoint Reached"), 2000);
    }
    else
    {
        statusBar()->showMessage(tr("Step Complete"), 1000);
    }

    stepCount = stepCount + 1; // Increase the step count, the next line is going to be displayed

    // Displays the instruction memory for all of the steps
    stepThoughInstructions();
    this->ui->programCounterlcdNumber->display(programCounter);
}

void ComputerSimGUI::runAllInstructions()
{
    // Reset the program to run all instructions
    reset();

    // While stepcount is in range and the program hasn't halted, keep calling the step function
    while((stepCount<=127) && !halted)
    {
        stepCount = stepCount + 1; // Increase the step count, the next line is going to be displayedf
        stepThoughInstructions();
    }

    qDebug() << "Stored breakLine: " << breakPointLine << " current Program Count: " << programCounter;
    if(programCounter-1 == breakPointLine) // && ui->breakLineSpinBox->isEnabled())
    {
        qDebug() << "Reset the halt";
        halted = false;

        QModelIndex line = instructionMemoryView->index(breakPointLine, 0);
        ui->instructionMemoryView->setCurrentIndex(line);

        // The instruction at the breakpoint did not run;
        programCounter = programCounter-1;

        resetBreakPoint();
        statusBar()->showMessage(tr("Breakpoint Reached"), 3000);
    }
    else
    {
        emit loadCacheData();

        enableButtons(false); // Disables the buttons
        statusBar()->showMessage(tr("Run All Completed"), 2000);
    }
}

void ComputerSimGUI::stepThoughInstructions()
{
    QFile sm_fileName(fileNameStored);
    sm_fileName.open(QIODevice::ReadOnly);
    QTextStream readFile(& sm_fileName);

    /* Quickly reads in everyline up to the current step count which was increased to ensure
     * it doesn't go beyond the max number of instructions to display the new line
     */
    for(int j = 0; ((j <= (stepCount)) && stepCount<=127) && !halted && !readFile.atEnd(); j++)
    {
        QString inputCarriage = readFile.readLine();
        inputCarriage = inputCarriage.toUpper();

        if((j+1) == stepCount) // Process the newest instruction
        {
            emit instructionProcess(inputCarriage, accumulatorCounter, programCounter);
            programCounter = programCounter+1;
        }

        QStandardItem *instructionData = new QStandardItem(QString(inputCarriage));
        instructionMemoryView->setItem(j,0,instructionData); // Sets the location to display
    }
    sm_fileName.close();

    // Process possible cases for the for loop exiting
    if(stepCount == 128 && halted) // If it's at the final instruction then increase the program count
    {                    // as halt doesn't increment
        programCounter = programCounter+1;
        if(cacheSet) // Emits to the CPU class to release the cache
        {
            emit loadCacheData();
        }
    }
    else if(halted && stepCount != 128) // Breakpoint was reached, instruction wasn't run so decrement the count
    {
        stepCount = stepCount-1;
        cacheSet = true; // Breakpoint resets the cachSet boolean
    }
    ui->instructionMemoryView->setModel(instructionMemoryView); // Display the table

    highlightFunction();
    cacheFunction = false;
}

void ComputerSimGUI::displayAllInstructions()
{
    int tempNumberOfInstructions = 0;
    QFile sm_fileName(fileNameStored);
    sm_fileName.open(QIODevice::ReadOnly);
    QTextStream readFile(& sm_fileName);

    // Simply inputs all of the instructions
    for(int j = 0; (j <= 127) && !readFile.atEnd(); j++)
    {
        QString inputCarriage = readFile.readLine();
        inputCarriage = inputCarriage.toUpper();

        QStandardItem *instructionData = new QStandardItem(QString(inputCarriage));
        instructionMemoryView->setItem(j,0,instructionData);         // Sets the location to display
        tempNumberOfInstructions = j;
    }
    ui->instructionMemoryView->setModel(instructionMemoryView); // Display the table

    emit instructionFileSelected(tempNumberOfInstructions);
}

void ComputerSimGUI::displaysTableCells()
{
    // Gets a string list to set the the size ofthe row index (0 through 127/511)
    QStringList instructionRowIndex, dataRowIndex;
    for(int j =  0; j < 512; j++)
    {
        QString tempRowIndex;
        tempRowIndex.setNum(j);
        if(j < 128)
        {
            instructionRowIndex.append(tempRowIndex);
        }
        dataRowIndex.append(tempRowIndex);
    }

    // Make the Instruction Memory View Table:
    instructionMemoryView = new QStandardItemModel(128,1,this); // (rows,columns,pointer)
    instructionMemoryView->setHorizontalHeaderItem(0, new QStandardItem(QString("Instructions:")));
    instructionMemoryView->setVerticalHeaderLabels(instructionRowIndex);
    //allows columns to adjust with size of window
    ui->instructionMemoryView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    // Display the table
    ui->instructionMemoryView->setModel(instructionMemoryView);

    // Make the Date List View Table:
    dataListView = new QStandardItemModel(512,2,this);
    dataListView->setHorizontalHeaderItem(0, new QStandardItem(QString("Addresses:")));
    dataListView->setHorizontalHeaderItem(1, new QStandardItem(QString("Values:")));
    dataListView->setVerticalHeaderLabels(instructionRowIndex);
    ui->dataTableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->dataTableView->setModel(dataListView);

    // Make the Error List View Table:
    errorListView = new QStandardItemModel(1,1,this);
    errorListView->setHorizontalHeaderItem(0, new QStandardItem(QString("Standard Output:")));
    ui->errorView->horizontalHeader()->setResizeMode(QHeaderView::Stretch); // Spans the entire window
    ui->errorView->setModel(errorListView);
}

void ComputerSimGUI::defaultPopulateDataListView()
{
    QFile sm_fileName(fileNameStored);
    sm_fileName.open(QIODevice::ReadOnly);
    QTextStream readFile(& sm_fileName);

    for(int j = 0; j < 512; j++) // Outputs the 512 registry addresses and their values
    {
        QStandardItem *dataValue = new QStandardItem( "0" ); // Start the values at 0
        dataListView->setItem(j, 1, dataValue);

        if(j < 256)
        {
            QStandardItem *registryAddress = new QStandardItem("0x0" + QString::number(j, 16).toUpper());
            dataListView->setItem(j, 0, registryAddress);
        }
        else  if( j < 16 )
        {
            QStandardItem *registryAddress = new QStandardItem("0x00" + QString::number(j, 16).toUpper());
            dataListView->setItem(j, 0, registryAddress);
        }
        else
        {
            QStandardItem *registryAddress = new QStandardItem("0x" + QString::number(j, 16).toUpper());
            dataListView->setItem(j, 0, registryAddress);
        }
    }
    ui->dataTableView->setModel(dataListView);  // Updates the windows
    sm_fileName.close();
}

void ComputerSimGUI::on_addBreakPointButton_released()
{
    // Toggles the breakline spinbox off and on
    if(ui->breakLineSpinBox->isEnabled())
    {
        resetBreakPoint();
    }
    else
    {
        ui->breakLineSpinBox->setEnabled(true);
        on_breakLineSpinBox_valueChanged(breakPointLine);
        statusBar()->showMessage(tr("Breakpoint Enabled"), 2000);
    }
}

void ComputerSimGUI::on_breakLineSpinBox_valueChanged(int arg1)
{
    qDebug() << "Change of break point";
    // Removes the highlight on the previous breakpoint
    if(arg1 != breakPointLine && arg1 != 0)
    {
        QModelIndex index = instructionMemoryView->index(breakPointLine, 0);
        ui->instructionMemoryView->setCurrentIndex(index);
        instructionMemoryView->setData(instructionMemoryView->index((breakPointLine),0), Qt::white, Qt::BackgroundRole);

        // Updates and highlight the new breakpoint
        breakPointLine = arg1;
    }
    else if(breakPointLine == 0)
    {   // garbage value
        breakPointLine = 222;
    }
    else
    {
        return;
    }

    highLightBreakPoint(breakPointLine);
    emit breakPoint(breakPointLine);
}

void ComputerSimGUI::resetBreakPoint()
{
    ui->breakLineSpinBox->setValue(0);
    ui->breakLineSpinBox->update();
    ui->breakLineSpinBox->setEnabled(false);

    if(breakPointLine > 0 && breakPointLine < 128) // Recolors the breakline
    {
        instructionMemoryView->setData(instructionMemoryView->index(breakPointLine,0), Qt::white, Qt::BackgroundColorRole);
    }

    breakPointLine = 0;

    // Sends a garbage value to QObject so that it disables the breakLine
    emit breakPoint(222);
    statusBar()->showMessage(tr("Breakpoint Disabled"), 2000);
}

void ComputerSimGUI::highLightBreakPoint(int breakLine)
{
    // Ensures breakpoint is enabled
    if(breakLine > 127 || breakLine < 1)
    {
        return;
    }
    breakPointLine = breakLine;

    // Colors the breakline
    instructionMemoryView->setData(instructionMemoryView->index(breakPointLine,0), Qt::yellow, Qt::BackgroundColorRole);

    // Resets the index to the current program counter
    QModelIndex line = instructionMemoryView->index(programCounter, 0);
    ui->instructionMemoryView->setCurrentIndex(line);
}

void ComputerSimGUI::highlightFunction()
{
    if(cacheSet && cacheFunction) // If the cache is set and a cache instruction was just read
    {
        if(hitCache)
        {
            // Colors the breakline
            instructionMemoryView->setData(instructionMemoryView->index(programCounter-1,0), Qt::green, Qt::BackgroundColorRole);
        }
        else
        {
            instructionMemoryView->setData(instructionMemoryView->index(programCounter-1,0), Qt::red, Qt::BackgroundColorRole);
        }
        // QModelIndex index = instructionMemoryView->index((programCounter), 0);
        // ui->instructionMemoryView->setCurrentIndex(index);
    }
    else
    {
        /* The next two lines moves the scrollbar to show the current step ran,
         * which is the previous step (stepCount-1), since step count incremented
         */
        QModelIndex index = instructionMemoryView->index((stepCount-1), 0);
        ui->instructionMemoryView->setCurrentIndex(index);
    }

}

// ~~~~~~~~~~ Start of Slot Definitions ~~~~~~~~~~

void ComputerSimGUI::invalidInstructionSlot(QString badInstruction)
{
    qDebug() << "Invalid Instruction: " << badInstruction; // For debugging purposes

    errorListView->setHorizontalHeaderItem(0, new QStandardItem(QString("Standard Output:")));
    QStandardItem *item1 = new QStandardItem(QString(badInstruction));
    errorListView->setItem(errorCounter,0,item1); // Display the error

    // The next three lines ensure the scrollbar shows the changed registry
    QModelIndex index = errorListView->index(errorCounter, 0);
    ui->errorView->setCurrentIndex(index);
    ui->errorView->setModel(errorListView);

    errorCounter = errorCounter +1;

    statusBar()->showMessage(tr("INVALID INSTRUCTION, PROGRAM HALTED"), 3000);
}

void ComputerSimGUI::updateAccumulatorSlot(QString newAccumulator)
{
    accumulatorCounter = newAccumulator.toInt();
    this->ui->accumulatorlcdNumber->display(accumulatorCounter);
}

void ComputerSimGUI::updateProgramCounterSlot(int newProgramCount)
{
    stepCount = newProgramCount;
    // The 'minus one '-1' is here because program counter will increment after instructions are ran
    programCounter = stepCount-1;
    this->ui->programCounterlcdNumber->display(programCounter);
}

void ComputerSimGUI::changeStoredMemorySlot(int newValue, int newValuesPosition)
{
    qDebug() << "******** newValue" << newValue << "newValuesPosition" << newValuesPosition;
    // Creates and updates the GUI output
    QString outputAddressValue;
    outputAddressValue.setNum(newValue);
    QStandardItem *newAddressValue = new QStandardItem(outputAddressValue);

    // The next two lines ensure the scrollbar shows the changed registry
    QModelIndex index = dataListView->index(newValuesPosition, 0);
    ui->dataTableView->setCurrentIndex( index );

    dataListView->setItem(newValuesPosition, 1, newAddressValue);
    ui->dataTableView->setModel(dataListView);  // Updates the windows
}

void ComputerSimGUI::cacheChangeStoredMemorySlot(InstructionData memoryBlock)
{
    QString outputAddressValue;

    for(int j=0; memoryBlock.startLocation < memoryBlock.endLocation; j++, memoryBlock.startLocation++ )
    {
        outputAddressValue.setNum(memoryBlock.cacheDataMemoryVector[j]);
        QStandardItem *newAddressValue = new QStandardItem(outputAddressValue);
        dataListView->setItem(memoryBlock.startLocation, 1, newAddressValue);
    }

    QModelIndex index = dataListView->index(memoryBlock.endLocation, 1);
    ui->dataTableView->setCurrentIndex(index);
}

void ComputerSimGUI::hasHaltedSlot(bool didHalt)
{
    halted = didHalt;
    // Disable the run and step buttons
    ui->stepThroughButton->setEnabled(FALSE);
    ui->runAllButton->setEnabled(FALSE);
    ui->breakLineSpinBox->setEnabled(FALSE);
    ui->addBreakPointButton->setEnabled(FALSE);

    // Flush the cache the program halted
    //    emit resetCache();
    statusBar()->showMessage(tr("Program Halted"), 12000);
}

void ComputerSimGUI::outputSlot(QString toDisplay, int accumulator)
{
    QModelIndex index = errorListView->index((errorCounter+1), 0);
    ui->errorView->setCurrentIndex(index);

    errorListView->setHorizontalHeaderItem(0, new QStandardItem(QString("Standard Output:")));
    QStandardItem *item1 = new QStandardItem(QString(toDisplay));
    errorListView->setItem(errorCounter,0,item1);
    errorCounter = errorCounter +1;

    ui->errorView->setModel(errorListView); // Display the table

    this->ui->accumulatorlcdNumber->display(accumulator);
    statusBar()->showMessage(tr("Output Declared"), 2000);
}

void ComputerSimGUI::breakedSlot(int breakLine)
{
    breakPointLine = breakLine;
    halted = true;
}

void ComputerSimGUI::highlightHitSlot(bool hit, bool cacheInstruction)
{
    hitCache = hit;
    cacheFunction = cacheInstruction;
}

void ComputerSimGUI::cacheSetForGuiSlot(bool set)
{
    cacheSet = set;
}
