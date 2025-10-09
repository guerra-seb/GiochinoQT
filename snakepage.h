#pragma once
#include <QWidget>
#include <QPoint>
#include <QVector>

class QLabel;
class QPushButton;
class QComboBox;
class QTimer;
class SnakeBoard;

class SnakePage : public QWidget {
    Q_OBJECT
public:
    explicit SnakePage(QWidget *parent=nullptr);
    friend class SnakeBoard;   // ⬅️ permette a SnakeBoard di leggere i private

signals:
    void backRequested();

protected:
    void keyPressEvent(class QKeyEvent *e) override;

private:
    // UI
    QWidget     *m_board      = nullptr; // area di gioco (ridisegnata)
    QLabel      *m_scoreLbl   = nullptr;
    QPushButton *m_backBtn    = nullptr;
    QPushButton *m_newBtn     = nullptr;
    QComboBox   *m_speedCombo = nullptr;
    QTimer      *m_timer      = nullptr;

    // logica
    enum Dir { Up, Down, Left, Right };
    Dir               m_dir = Right, m_pendingDir = Right;
    QVector<QPoint>   m_snake;        // [0] = testa
    QPoint            m_food;
    int               m_cols = 20, m_rows = 20;
    int               m_score = 0;
    bool              m_running = false, m_paused = false;

    void newGame();
    void placeFood();
    void step();
    void updateScore();
    void setSpeedIndex(int idx); // 0=Lento, 1=Normale, 2=Veloce

    static int rnd(int maxExclusive); // helper RNG compatibile Qt5/Qt6
};
