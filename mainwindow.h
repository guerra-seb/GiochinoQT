#pragma once
#include <QMainWindow>

class QStackedWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget *m_stack = nullptr;
    int m_idxHome    = -1;
    int m_idxSecond  = -1; // Becca il Codice
    int m_idxOpts    = -1;
    int m_idxHangman = -1; // Impiccato / Indovina la Parola
    int m_idxAnagram = -1; // Anagrammi
    int m_idxMemCfg  = -1; // Memory: scelta dimensione
    int m_idxMemory  = -1; // Memory: gioco
    int m_idxSudoku  = -1; // Sudoku
    int m_idx2048    = -1; // 2048
    int m_idxSnake   = -1; // Snake
    int m_idxMines   = -1; // Prato Fiorito

    void setupMenus();
};
