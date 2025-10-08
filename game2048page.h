#pragma once
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QVector>

class Game2048Page : public QWidget {
    Q_OBJECT
public:
    explicit Game2048Page(QWidget *parent=nullptr);
signals:
    void backRequested();

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    QLabel* m_cells[4][4]{};
    int     m_grid[4][4]{};   // valori 0,2,4,8,...
    int     m_score = 0;

    QLabel      *m_scoreLbl = nullptr;
    QPushButton *m_newBtn   = nullptr;
    QPushButton *m_backBtn  = nullptr;
    QGridLayout *m_gridLay  = nullptr;

    void newGame();
    void spawnTile();                 // genera 2 (90%) o 4 (10%)
    bool moveLeft();
    bool moveRight();
    bool moveUp();
    bool moveDown();
    bool canMove() const;
    void updateUI();
    void setCellStyle(QLabel* lab, int value);
};
