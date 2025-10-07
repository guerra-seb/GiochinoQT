#pragma once
#include <QWidget>
#include <QString>
#include <QVector>

class QLabel;
class QGridLayout;
class QPushButton;
class QTimer;
class QElapsedTimer;

class MemoryPage : public QWidget {
    Q_OBJECT
public:
    explicit MemoryPage(QWidget *parent = nullptr);
    void startNew(int rows, int cols); // anche rettangolare (3x2, 3x4, ...)

signals:
    void backRequested();
    void changeSizeRequested(); // torna alla pagina di scelta

private:
    // UI
    QLabel *m_title = nullptr;
    QLabel *m_info  = nullptr; // mosse + tempo
    QLabel *m_result= nullptr; // BRAVO!/SEI SCARSO!
    QGridLayout *m_grid = nullptr;
    QVector<QPushButton*> m_cards;

    // Stato gioco
    int m_rows = 4;
    int m_cols = 4;
    QVector<QString> m_symbols;       // N*N voci (doppioni mescolati)
    int m_first = -1, m_second = -1;  // selezioni
    bool m_lock = false;              // blocca click durante il flip
    int m_moves = 0;
    int m_matchedPairs = 0;

    // Timer
    QTimer *m_tick = nullptr;
    QElapsedTimer *m_elapsed = nullptr;

    // Helpers
    void buildSymbols(int rows, int cols);
    void buildGrid();
    void updateInfo();
    void reveal(int idx);
    void hidePair(int a, int b);
    void onCardClicked(int idx);
};
