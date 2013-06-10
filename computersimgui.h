/*
 *    James Mead Dagres
 *    jdagres@vt.edu
 *
 *    04-07-13
 *
 *    ComputerSimGUI Class Header File
 */

#ifndef ComputerSimGUI_H
#define ComputerSimGUI_H

#include "centralprocessingunit.h"
#include "cachegui.h"
#include <QObject>

// forward declarations
class QLabel;
class QTextEdit;
class QSplitter;

namespace Ui {
class ComputerSimGUI;
}

class ComputerSimGUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit ComputerSimGUI(QWidget *parent = 0);
    ~ComputerSimGUI();

private slots:
    /* The virtual declaration means that it can override its inherents
     * The openFile() function calls an open helper function which initializes the
     * process of file manipulation
     */
    virtual void openFile();

    /* The next on_actionOpen_triggered() functions creates the menu bar function calls
     * the set open file function
     */
    void on_actionOpen_triggered();

    /* The stepThroughButton function is run when the step button is released
     * it runs through one input and sends the instruction signal to the instructionSlot
     * in the centralprocessingunit class
     */
    void on_stepThroughButton_released();

    /* The runAllButton function is run when the run button is released
     * it runs through every input and sends the instruction signal to the instructionSlot
     * in the centralprocessingunit class
     */
    void on_runAllButton_released();

    /* The resetButton resets; all of the counters, LCD displays and buttons
     */
    void on_resetButton_released();

    /* The clearButton does everything the reset button does and clears; all
     * of the windows, the file selected, and the displayed tables
     */
    void on_clearButton_released();

    void on_addBreakPointButton_released();

    void on_breakLineSpinBox_valueChanged(int arg1);

    /* The invalidInstruction slot displays the bad instruction on the error
     * window and halts the program
     */
    void invalidInstructionSlot(QString badInstruction);

    /* The updateAccumulatorSlot receives the new
     * accumulator or programCount and updates the GUI
     */
    void updateAccumulatorSlot(QString newAccumulator);

    /* The updateProgramCounterSlot function runs when a signal is changed from the QObject
     * class. It is most commonly run by a BGTZ instruction which resets the program count
     */
    void updateProgramCounterSlot(int newAccumulator);

    /* This changeStoredMemorySlot is called when a memory address is changed
     * it then updates the address and the GUI
     */
    void changeStoredMemorySlot(int accumulator, int hexAddress);
    void cacheChangeStoredMemorySlot(InstructionData block);

    /* The hasHalted slot returns true if the program got an error if the halt instruction
     * was run, or if there was an error
     */
    void hasHaltedSlot(bool didHalt);

    /* The outputSlot displays an output of the current accumulator
     */
    void outputSlot(QString toDisplay, int accumulator);

    /* If the break point is reach the breakedSlot is signalled
       */
    void breakedSlot(int breakLine);

    /* The resetBreakPoint function resets the break point, is ran when the button is disabled or when
     * the breakpoint is reached
     */
    void resetBreakPoint();

    /* Highlights the line green if it's a hit, if not then highlight it as red
     */
    void highlightHitSlot(bool hit, bool cacheInstruction);

    // Tells the simgui the cache has been set
    void cacheSetForGuiSlot(bool set);

signals:
    // ------ Signals to the Model CPU Class ------
    // Signal to pass the filename back to the gui class
    void instructionFileSelected(int instructionNumber);
    // Signal to pass the instruction to the QObject
    void instructionProcess(QString instruction, int currentAccumulator, int currentProgramCounter);
    // instructionReset resets all the values
    void instructionReset();
    // Sends the breakpoint
    void breakPoint(int);

    // ------ Signals to the View 2 Cache ------
    void flushCache();
    void resetCache();
    void createCacheGui();
    void displayWidget();

    // ------ Signals to the Model QOBject CPU class ------
    void loadCacheData();

private:
    Ui::ComputerSimGUI *ui;

    /* This is a free QT source code function that nicely formats the list tables on the gui to look
     * like excel spread sheets.
     */
    void displaysTableCells();

    /* The stepPopulateInstructionMemoryView is called when the step button is hit and
     *  populates the table with the appropriate values
     */
    void stepThoughInstructions();

    /* The runAllInstructions is called when the run all button is  hit and calls the
     * stepThrough function until all the instructions are read or the program halts
     */
    void runAllInstructions();

    /* The defaultPopulateDataListView function displays the default adresses with the
     * default registry value of zero. The function is called when the file is opened
     * or when the clear button is hit
     */
    void defaultPopulateDataListView();

    // The displayAllInstructions function simply loads all of the instructions
    void displayAllInstructions();

    /* The reset function resets the instruction values stored and all of the counters
     * it retains the current file and it's loaded instructions
     */
    void reset();

    /* The clear function first calls the reset function to reset the values, but then
     * clears all of the instruction memory
     */
    void clear();

    /* The highLightBreakPoint highlights the breakpoint if it exists
     */
    void highLightBreakPoint(int breakLine);

    /* The enableButtons function tells the buttons to either be enalbed or disabled depending
     * on the boolean value
     */
    void enableButtons(bool enabled);

    /* The highlightFuction sets the current index and highlights depending the following private booleans:
     * Highlights the line green if it's a hit, if not then highlight it as red this function is
     * important and must exist here because it has to be called after after the index has been
     * set otherwise the highlight is overwritten. Similarly the index is trailing by one
     */
    void highlightFunction();

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

    // The next five objects are used to display different windows and prompts in the GUI
    QSplitter *m_horizontalSplitter;
    QTextEdit *m_TextEdit1;
    QTextEdit *m_TextEdit2;
    QAction *m_OpenAction;
    QAction *m_ExitAction;

    /* The next three strings globaly store the value of the file name
     * This enables the file to be called throughout the class
     */
    QString fileName, fileNameStored, m_FileName;

    // This bool ensures a file has been selected
    bool fileOpened;
    bool notFirst;

    /* Booleans used to highlight, the cacheSet is set when the cache settings are set up
     * the hitCache function is going to be true when it's a hit and false when it's a miss
     * the cacheFunction is true when the function called is a cache funceion and false when it's not
     */
    bool hitCache, cacheSet, cacheFunction;

    /* The step count function keeps track of the current steps as the step
     * button is selected and run
     */
    int stepCount;

    // The int error count counts the number of error instructions displayed
    int errorCounter;

    /* This boolean is triggered when the HALT instruction or an invalid input is sent
     * it is essential for the run all to stop running
     */
    bool halted;

    /* The int breakPointLine variable stores the line the breakpoint is at, and everytime an
     * instruction is emitted the programCounter is compared to this value
     */
    int breakPointLine;

    // QT objects used for the menu file open
    QMenu *m_FileMenu;
    QMenu *m_ExitMenu;
    QLabel *m_StatusLabel;

    /* Globalizes the QStandardItems for the three list views
     */
    QStandardItemModel *instructionMemoryView;
    QStandardItemModel *dataListView;
    QStandardItemModel *errorListView;

    /* The loadFile functions load the current file based on the filenamepath parameter
     */
    virtual void loadFile(const QString &fileName);

    // This function sets the current file based on the filenamepath parameter
    virtual void setCurrentFile(const QString &fileName);
};

#endif // ComputerSimGUI_H
