#include "game2048page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <algorithm>
#include <QtGlobal>
#include <QStyle>
#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
  #include <QRandomGenerator>
  static inline int rnd(int maxExclusive) {
      return QRandomGenerator::global()->bounded(maxExclusive);
  }
#else
  #include <QTime>
  static inline int rnd(int maxExclusive) {
      static bool seeded = false;
      if (!seeded) {
          const QTime t = QTime::currentTime();
          qsrand(uint(t.msec() + t.second()*1000));
          seeded = true;
      }
      return (maxExclusive > 0) ? (qrand() % maxExclusive) : 0;
  }
#endif

static QString scoreText(int s){ return QString("Punteggio: %1").arg(s); }

// palette ispirata al 2048 classico
static QString bgFor(int v){
    switch(v){
    case 0:   return "#cdcfd6";
    case 2:   return "#eee4da";
    case 4:   return "#ede0c8";
    case 8:   return "#f2b179";
    case 16:  return "#f59563";
    case 32:  return "#f67c5f";
    case 64:  return "#f65e3b";
    case 128: return "#edcf72";
    case 256: return "#edcc61";
    case 512: return "#edc850";
    case 1024:return "#edc53f";
    default:  return "#edc22e";
    }
}

Game2048Page::Game2048Page(QWidget *parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(8);

    auto *top = new QHBoxLayout;

    // bottone Indietro
    m_backBtn = new QPushButton(QString::fromUtf8(u8"← Indietro"));
    m_backBtn->setAutoDefault(false);
    m_backBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    connect(m_backBtn, &QPushButton::clicked, this, [this]{ emit backRequested(); });

    // punteggio
    m_scoreLbl = new QLabel(scoreText(0));
    QFont fs = m_scoreLbl->font(); fs.setPointSize(14); fs.setBold(true);
    m_scoreLbl->setFont(fs);

    // nuova partita
    m_newBtn = new QPushButton("Nuova partita");
    connect(m_newBtn, &QPushButton::clicked, this, [this]{ newGame(); setFocus(); });

    top->addWidget(m_backBtn);
    top->addSpacing(8);
    top->addWidget(m_scoreLbl);
    top->addStretch();
    top->addWidget(m_newBtn);

    root->addLayout(top);

    auto *wrap = new QWidget;
    wrap->setObjectName("board");
    auto *wrapLay = new QVBoxLayout(wrap);
    wrapLay->setContentsMargins(8,8,8,8);

    m_gridLay = new QGridLayout;
    m_gridLay->setSpacing(8);
    for(int r=0;r<4;++r){
        for(int c=0;c<4;++c){
            auto *lab = new QLabel;
            lab->setAlignment(Qt::AlignCenter);
            QFont f = lab->font(); f.setPointSize(20); f.setBold(true);
            lab->setFont(f);
            lab->setMinimumSize(72,72);
            lab->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            m_gridLay->addWidget(lab, r, c);
            m_cells[r][c] = lab;
        }
    }
    wrapLay->addLayout(m_gridLay);
    root->addWidget(wrap);

    // bordi arrotondati estetici
    setStyleSheet(R"(
        QWidget#board { background:#b8c0cc; border-radius:12px; }
        QLabel { border-radius:10px; color:#222; }
    )");

    newGame();
}

void Game2048Page::newGame(){
    m_score = 0;
    for(auto &row : m_grid) for(int &v : row) v = 0;
    spawnTile(); spawnTile();
    updateUI();
}

void Game2048Page::spawnTile(){
    QVector<QPair<int,int>> empty;
    for(int r=0;r<4;++r) for(int c=0;c<4;++c) if(m_grid[r][c]==0) empty.append({r,c});
    if(empty.isEmpty()) return;
    auto rc = empty.at(rnd(empty.size()));
    int val = (rnd(10) == 0) ? 4 : 2;
    m_grid[rc.first][rc.second] = val;
}

void Game2048Page::setCellStyle(QLabel* lab, int v){
    lab->setText(v? QString::number(v) : QString());
    QString txtColor = (v>=8) ? "#f9f9f9" : "#444";
    lab->setStyleSheet(QString("QLabel{background:%1;color:%2;}").arg(bgFor(v), txtColor));
}

void Game2048Page::updateUI(){
    for(int r=0;r<4;++r)
        for(int c=0;c<4;++c)
            setCellStyle(m_cells[r][c], m_grid[r][c]);
    m_scoreLbl->setText(scoreText(m_score));
}

bool Game2048Page::moveLeft(){
    bool moved=false;
    for(int r=0;r<4;++r){
        int line[4]; int n=0;
        // compattamento
        for(int c=0;c<4;++c) if(m_grid[r][c]!=0) line[n++]=m_grid[r][c];
        for(int i=n;i<4;++i) line[i]=0;
        // merge
        for(int i=0;i<3;++i){
            if(line[i]!=0 && line[i]==line[i+1]){
                line[i]*=2; m_score+=line[i];
                line[i+1]=0; i++;
            }
        }
        // ricompattamento finale
        int out[4]; int k=0;
        for(int i=0;i<4;++i) if(line[i]!=0) out[k++]=line[i];
        for(int i=k;i<4;++i) out[i]=0;

        for(int c=0;c<4;++c){
            if(m_grid[r][c]!=out[c]){ m_grid[r][c]=out[c]; moved=true; }
        }
    }
    return moved;
}
bool Game2048Page::moveRight(){
    // inverti, usa left, reinverti
    for(int r=0;r<4;++r) std::reverse(m_grid[r], m_grid[r]+4);
    bool m=moveLeft();
    for(int r=0;r<4;++r) std::reverse(m_grid[r], m_grid[r]+4);
    return m;
}
bool Game2048Page::moveUp(){
    bool moved=false;
    for(int c=0;c<4;++c){
        int col[4], n=0;
        for(int r=0;r<4;++r) if(m_grid[r][c]!=0) col[n++]=m_grid[r][c];
        for(int i=n;i<4;++i) col[i]=0;
        for(int i=0;i<3;++i){
            if(col[i]!=0 && col[i]==col[i+1]){
                col[i]*=2; m_score+=col[i];
                col[i+1]=0; i++;
            }
        }
        int out[4], k=0;
        for(int i=0;i<4;++i) if(col[i]!=0) out[k++]=col[i];
        for(int i=k;i<4;++i) out[i]=0;

        for(int r=0;r<4;++r){
            if(m_grid[r][c]!=out[r]){ m_grid[r][c]=out[r]; moved=true; }
        }
    }
    return moved;
}
bool Game2048Page::moveDown(){
    // inverti colonna, usa up, reinverti
    for(int c=0;c<4;++c){
        std::swap(m_grid[0][c], m_grid[3][c]);
        std::swap(m_grid[1][c], m_grid[2][c]);
    }
    bool m=moveUp();
    for(int c=0;c<4;++c){
        std::swap(m_grid[0][c], m_grid[3][c]);
        std::swap(m_grid[1][c], m_grid[2][c]);
    }
    return m;
}

bool Game2048Page::canMove() const{
    // c'è uno zero?
    for(int r=0;r<4;++r) for(int c=0;c<4;++c) if(m_grid[r][c]==0) return true;
    // celle adiacenti uguali?
    for(int r=0;r<4;++r) for(int c=0;c<4;++c){
        if(r<3 && m_grid[r][c]==m_grid[r+1][c]) return true;
        if(c<3 && m_grid[r][c]==m_grid[r][c+1]) return true;
    }
    return false;
}

void Game2048Page::keyPressEvent(QKeyEvent *e){
    bool moved=false;
    switch(e->key()){
    case Qt::Key_Escape:
        emit backRequested();
        return;
    case Qt::Key_Left:  moved=moveLeft();  break;
    case Qt::Key_Right: moved=moveRight(); break;
    case Qt::Key_Up:    moved=moveUp();    break;
    case Qt::Key_Down:  moved=moveDown();  break;
    default: QWidget::keyPressEvent(e); return;
    }
    if(moved){
        spawnTile();
        updateUI();
        if(!canMove()){
            updateUI();
            QMessageBox::information(this,"2048","Partita finita!\n"+scoreText(m_score));
        }
    }
}
