#pragma once
#include <QWidget>
#include <QVector>
#include <QPoint>

class QLabel;
class QPushButton;
class QComboBox;
class QGridLayout;
class QTimer;

class MinesPage : public QWidget {
    Q_OBJECT
public:
    explicit MinesPage(QWidget *parent=nullptr);

signals:
    void backRequested();

protected:
    void keyPressEvent(class QKeyEvent *e) override;

private:
    // UI
    QLabel      *m_statusLbl = nullptr;
    QLabel      *m_timerLbl  = nullptr;
    QPushButton *m_backBtn   = nullptr;
    QPushButton *m_newBtn    = nullptr;
    QComboBox   *m_diffCombo = nullptr;
    QWidget     *m_boardWrap = nullptr; // contenitore
    QGridLayout *m_grid      = nullptr;
    QTimer      *m_timer     = nullptr;

    // board
    int m_rows = 9, m_cols = 9, m_mines = 10;
    QVector<QVector<bool>> m_isMine;
    QVector<QVector<int>>  m_adj;       // -1 = mina, altrimenti 0..8
    QVector<QVector<bool>> m_revealed;
    QVector<QVector<bool>> m_flagged;

    // stato
    bool m_started = false; // piazzo le mine al primo click (casella sicura)
    bool m_alive   = true;
    int  m_toReveal = 0;    // celle non mina da scoprire per vincere
    int  m_seconds  = 0;

    void rebuildUIGrid();           // ricrea i bottoni della griglia
    void newGame();                 // reset completo
    void setDifficultyFromIndex(int idx);
    void placeMinesAvoiding(int sr, int sc); // piazza mine evitando (sr,sc) e vicine
    void computeAdjacencies();
    void revealCell(int r, int c);
    void floodZeros(int r, int c);
    void toggleFlag(int r, int c);
    void gameOver(bool win);
    void updateStatus();
    static int rnd(int maxExclusive);
};
