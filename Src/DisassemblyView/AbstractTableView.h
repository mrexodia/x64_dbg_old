#ifndef ABSTRACTTABLEVIEW_H
#define ABSTRACTTABLEVIEW_H

#include <QtGui>
#include <qdebug.h>



class AbstractTableView : public QAbstractScrollArea
{
    Q_OBJECT
public:
    explicit AbstractTableView(QWidget *parent = 0);

    virtual void AbstractMemberFunction(void) = 0;
    void callVirtual();

    enum GuiState_t {NoState, ResizeColumnState, MultiRowsSelectionState, HeaderButtonPressed};

    virtual bool paintContent(QPainter* painter, int rowBase, int rowOffset, int col, int x, int y, int w, int h);
    virtual void paintSelection(QPainter* painter, int rowBase, int rowOffset, int col, int x, int y, int w, int h);

    // Reimplemented Methods
    void paintEvent(QPaintEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void wheelEvent(QWheelEvent* event);

    // ScrollBar Management
    void setScrollBarValue(int value);
    void setRowCount(int count);
    virtual void sliderMovedAction(int type, int value, int delta); // Virtual
    QScrollBar* getScrollbar();

    // Selection Management
    virtual void expandSelectionUpTo(int to);
    virtual void setSingleSelection(int index);
    virtual int getInitialSelection();
    virtual bool isSelected(int index); // Virtual
    virtual void selectNext();
    virtual void selectPrevious();

    // Coordinate analysis
    int getRowIndexFromY(int y);
    int getRowOffsetFromY(int y);
    int getColumnIndexFromX(int x);
    int getColumnPosition(int index);
    int transY(int y);
    int getViewableRowsCount();

    // Add / Remove row and columns
    void addColumnAt(int at, int width, bool isClickable);

    // Getters and Setters
    int getRowCount();
    int getColumnCount();

    int getRowHeight();
    int getColumnWidth(int index);

    void setColumnWidth(int index, int width);

    int getHeaderHeigth();
    int getTableHeigth();

    // Table Data
    virtual QString getStringToPrint(int rowBase, int rowOffset, int col) = 0;
    int getTableOffset();
    int setTableOffset(int value);

    virtual int getIndexFromCount(int index, int count);


signals:
    void headerButtonPressed(int col);
    void headerButtonReleased(int col);

public slots:
    // ScrollBar Management
    void vertSliderActionSlot(int action);

    // Selection Management
    void multiSelTimerSlot();

private:
    typedef struct _ColumnResizingData_t
    {
        bool splitHandle;
        int index;
        int lastPosX;
    } ColumnResizingData_t;

    typedef struct _HeaderButton_t
    {
        bool isClickable;
        bool isPressed;
        bool isMouseOver;
    } HeaderButton_t;

    typedef struct _Column_t
    {
        int width;
        HeaderButton_t header;
    } Column_t;


    typedef struct _Header_t
    {
        bool isVisible;
        int height;
        int activeButtonIndex;
    } Header_t;

    typedef struct _SelectionData_t
    {
        int firstSelectedIndex;
        int fromIndex;
        int toIndex;
    } SelectionData_t;

    QTimer* mMultiSelTimer;

    GuiState_t mGuiState;

    ColumnResizingData_t mColResizeData;

    QPushButton mHeaderButtonSytle;


    QList<Column_t> mColumnList;

    int mRowHeight;
    int mRowCount;

    SelectionData_t mSelection;
    int mTableOffset;
    Header_t mHeader;
};

#endif // ABSTRACTTABLEVIEW_H
