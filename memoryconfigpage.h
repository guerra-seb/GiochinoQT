#pragma once
#include <QWidget>

class QPushButton;

class MemoryConfigPage : public QWidget {
    Q_OBJECT
public:
    explicit MemoryConfigPage(QWidget *parent = nullptr);

signals:
    void backRequested();
    void startRequested(int rows, int cols); // es. 3x2, 3x4, 4x4, 6x6, ...

private:
    void choose(int r, int c);
};
