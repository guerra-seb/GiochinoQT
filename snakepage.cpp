#include "snakepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QMessageBox>
#include <QStyle>
#include <QKeyEvent>
#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
  #include <QRandomGenerator>
  static inline int rndBounded(int n){ return QRandomGenerator::global()->bounded(n); }
#else
  #include <random>
  static inline int rndBounded(int n){
      static std::mt19937 rng{ std::random_device{}() };
      if (n<=1) return 0;
      std::uniform_int_distribution<int> d(0, n-1);
      return d(rng);
  }
#endif
int SnakePage::rnd(int maxExclusive){ return rndBounded(maxExclusive); }

// Piccolo widget interno per disegnare la board
class SnakeBoard : public QWidget{
public:
    explicit SnakeBoard(SnakePage *owner): QWidget(owner), m_owner(owner) {
        setMinimumSize(360, 360);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
protected:
    void paintEvent(QPaintEvent*) override;
private:
    SnakePage *m_owner;
};

SnakePage::SnakePage(QWidget *parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(8);

    // Barra superiore
    auto *top = new QHBoxLayout;
    m_backBtn = new QPushButton(QString::fromUtf8(u8"← Indietro"));
    m_backBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    m_backBtn->setAutoDefault(false);
    connect(m_backBtn, &QPushButton::clicked, this, [this]{ emit backRequested(); });

    m_scoreLbl = new QLabel("Punteggio: 0");
    QFont fs = m_scoreLbl->font(); fs.setPointSize(14); fs.setBold(true);
    m_scoreLbl->setFont(fs);

    m_newBtn = new QPushButton("Nuova partita");
    connect(m_newBtn, &QPushButton::clicked, this, [this]{ newGame(); setFocus(); });

    m_speedCombo = new QComboBox(this);
    m_speedCombo->addItems({"Lento","Normale","Veloce"});
    m_speedCombo->setCurrentIndex(1);
    connect(m_speedCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [this](int i){ setSpeedIndex(i); });

    top->addWidget(m_backBtn);
    top->addSpacing(8);
    top->addWidget(m_scoreLbl);
    top->addStretch();
    top->addWidget(new QLabel("Velocità:"));
    top->addWidget(m_speedCombo);
    top->addSpacing(8);
    top->addWidget(m_newBtn);
    root->addLayout(top);

    // Board
    m_board = new SnakeBoard(this);
    root->addWidget(m_board, 1);

    // Timer
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [this]{
        if (m_running && !m_paused){ step(); m_board->update(); }
    });

    // Avvio
    setSpeedIndex(1);
    newGame();
}

void SnakePage::setSpeedIndex(int idx){
    int ms = 120;
    if (idx==0) ms = 200;
    else if (idx==1) ms = 120;
    else if (idx==2) ms = 70;
    m_timer->setInterval(ms);
}

void SnakePage::updateScore(){
    m_scoreLbl->setText(QString("Punteggio: %1").arg(m_score));
}

void SnakePage::newGame(){
    m_snake.clear();
    m_snake << QPoint(m_cols/2, m_rows/2)
            << QPoint(m_cols/2 -1, m_rows/2)
            << QPoint(m_cols/2 -2, m_rows/2);
    m_dir = m_pendingDir = Right;
    m_score = 0;
    m_paused = false;
    m_running = true;
    placeFood();
    updateScore();
    m_timer->start();
    m_board->update();
}

void SnakePage::placeFood(){
    while(true){
        QPoint p(rnd(m_cols), rnd(m_rows));
        bool onSnake=false;
        for(const auto &s : m_snake){ if (s==p){ onSnake=true; break; } }
        if(!onSnake){ m_food = p; return; }
    }
}

void SnakePage::step(){
    // applica direzione pendente (evita inversione se opposta)
    auto opposite = [](Dir a, Dir b){
        return (a==Up && b==Down) || (a==Down && b==Up) || (a==Left && b==Right) || (a==Right && b==Left);
    };
    if (!opposite(m_dir, m_pendingDir)) m_dir = m_pendingDir;

    QPoint head = m_snake.front();
    switch(m_dir){
        case Up:    head.ry() -= 1; break;
        case Down:  head.ry() += 1; break;
        case Left:  head.rx() -= 1; break;
        case Right: head.rx() += 1; break;
    }
    // collisioni: muri
    if (head.x()<0 || head.x()>=m_cols || head.y()<0 || head.y()>=m_rows){
        m_running = false;
        m_timer->stop();
        m_scoreLbl->setText(QString("Game Over — Punteggio: %1").arg(m_score));
        m_board->update();
        return;
    }

    // collisioni: corpo
    for (const auto &s : m_snake){
        if (s == head){
            m_running = false;
            m_timer->stop();
            m_scoreLbl->setText(QString("Game Over — Punteggio: %1").arg(m_score));
            m_board->update();
            return;
        }
    }

    // avanza
    m_snake.prepend(head);
    if (head == m_food){
        m_score += 10;
        updateScore();
        placeFood();
    } else {
        m_snake.removeLast();
    }
}

void SnakePage::keyPressEvent(QKeyEvent *e){
    switch(e->key()){
    case Qt::Key_Left:  case Qt::Key_A: m_pendingDir = Left;  break;
    case Qt::Key_Right: case Qt::Key_D: m_pendingDir = Right; break;
    case Qt::Key_Up:    case Qt::Key_W: m_pendingDir = Up;    break;
    case Qt::Key_Down:  case Qt::Key_S: m_pendingDir = Down;  break;
    case Qt::Key_Space:
        m_paused = !m_paused;
        if (!m_paused && m_running) m_timer->start();
        break;
    case Qt::Key_N:     newGame(); break;
    case Qt::Key_Escape: emit backRequested(); return;
    default: QWidget::keyPressEvent(e); return;
    }
}

static QRect cellRect(const QRect &area, int cols, int rows, int cx, int cy){
    const int cell = qMin(area.width()/cols, area.height()/rows);
    const int ox = area.x() + (area.width()  - cell*cols)/2;
    const int oy = area.y() + (area.height() - cell*rows)/2;
    return QRect(ox + cx*cell, oy + cy*cell, cell, cell);
}

void SnakeBoard::paintEvent(QPaintEvent*){
    auto &P = *m_owner;
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // area board centrata
    const int cell = qMin(width()/P.m_cols, height()/P.m_rows);
    const int W = cell*P.m_cols, H = cell*P.m_rows;
    const int ox = (width()-W)/2, oy = (height()-H)/2;
    const QRect board(ox, oy, W, H);

    // sfondo
    p.fillRect(board, palette().alternateBase());

    // griglia leggera
    QPen grid(palette().mid().color());
    grid.setColor(grid.color().lighter(130));
    grid.setWidth(1);
    p.setPen(grid);
    for (int x=0;x<=P.m_cols;++x) p.drawLine(ox + x*cell, oy, ox + x*cell, oy + H);
    for (int y=0;y<=P.m_rows;++y) p.drawLine(ox, oy + y*cell, ox + W, oy + y*cell);

    // cibo
    QRect fr = cellRect(board, P.m_cols, P.m_rows, P.m_food.x(), P.m_food.y());
    fr.adjust(cell/8, cell/8, -cell/8, -cell/8);
    p.setBrush(QColor("#e33a3a"));
    p.setPen(Qt::NoPen);
    p.drawEllipse(fr);

    // serpente
    QColor body = palette().highlight().color(); // segue tema
    QColor head = body.darker(115);
    for (int i=0;i<P.m_snake.size(); ++i){
        const QPoint &s = P.m_snake[i];
        QRect r = cellRect(board, P.m_cols, P.m_rows, s.x(), s.y());
        r.adjust(2,2,-2,-2);
        p.setBrush(i==0 ? head : body);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(r, 20, 20, Qt::RelativeSize);
    }
}
