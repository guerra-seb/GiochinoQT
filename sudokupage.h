#pragma once
#include <QWidget>

class QLabel;
class QGridLayout;
class QLineEdit;
class QComboBox;
class QPushButton;

class SudokuPage : public QWidget {
    Q_OBJECT
public:
    explicit SudokuPage(QWidget *parent = nullptr);

signals:
    void backRequested();

private:
    // UI
    QLabel *m_title = nullptr;
    QLabel *m_info  = nullptr;
    QLabel *m_result= nullptr;
    QComboBox *m_diff = nullptr;
    QGridLayout *m_grid = nullptr;
    QLineEdit* m_cell[9][9] = {{nullptr}};

    // Dati
    int m_solution[9][9] = {{0}};
    int m_puzzle[9][9]   = {{0}};

    // Azioni
    void newGame();
    void checkBoard();
    void revealSolution();

    // Helpers UI
    void buildGrid();
    void loadPuzzleToUI();
    void styleGiven(QLineEdit *e, bool given);
    void setAllEditable(bool en);
    void applyCellBorder(QLineEdit *e, int r, int c);

    // Generatore/solver
    void clearBoard(int b[9][9]);
    void copyBoard(const int src[9][9], int dst[9][9]);
    bool solveRandom(int b[9][9]);                  // riempie risolvendo con ordine casuale
    bool findEmpty(const int b[9][9], int &r, int &c) const;
    bool isSafe(const int b[9][9], int r, int c, int num) const;
    int  boxStart(int x) const { return (x/3)*3; }

    void generatePuzzle(int removals);              // parte da solution e rimuove con controllo unicità
    int  countSolutionsLimited(int b[9][9], int limit); // conta fino a 'limit'
};
