#include "minespage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QStyle>
#include <QTimer>
#include <QKeyEvent>
#include <QPainter>
#include <QMouseEvent>
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
int MinesPage::rnd(int maxExclusive){ return rndBounded(maxExclusive); }

// --- CellButton: bottone che sa la propria posizione + gestisce il click destro ---
class CellButton : public QPushButton {
    Q_OBJECT
public:
    CellButton(int r,int c, QWidget* parent=nullptr) : QPushButton(parent), m_r(r), m_c(c) {
        setFixedSize(28,28);
        setCheckable(false);
        setFocusPolicy(Qt::NoFocus);
        setText(""); // coperto dal reveal
    }
signals:
    void left(int r,int c);
    void right(int r,int c);
protected:
    void mousePressEvent(QMouseEvent *e) override {
        if (e->button()==Qt::RightButton) emit right(m_r,m_c);
        else if (e->button()==Qt::LeftButton) emit left(m_r,m_c);
        QPushButton::mousePressEvent(e);
    }
private:
    int m_r, m_c;
};
#include "moc_minespage.cpp" // per QObject in classe interna (se usi qmake non serve)

// --- MinesPage ---

MinesPage::MinesPage(QWidget *parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(8);

    // Top bar
    auto *top = new QHBoxLayout;
    m_backBtn = new QPushButton(QString::fromUtf8(u8"← Indietro"));
    m_backBtn->setAutoDefault(false);
    m_backBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    connect(m_backBtn, &QPushButton::clicked, this, [this]{ emit backRequested(); });

    m_statusLbl = new QLabel("Prato Fiorito — 🙂");
    QFont fs = m_statusLbl->font(); fs.setPointSize(14); fs.setBold(true);
    m_statusLbl->setFont(fs);

    m_timerLbl = new QLabel("Tempo: 0 s");

    m_diffCombo = new QComboBox(this);
    m_diffCombo->addItems({"Principiante (9×9, 10)", "Intermedio (16×16, 40)", "Esperto (30×16, 99)"});
    m_diffCombo->setCurrentIndex(0);
    connect(m_diffCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [this](int i){ setDifficultyFromIndex(i); newGame(); });

    m_newBtn = new QPushButton("Nuova");
    connect(m_newBtn, &QPushButton::clicked, this, [this]{ newGame(); setFocus(); });

    top->addWidget(m_backBtn);
    top->addSpacing(8);
    top->addWidget(m_statusLbl);
    top->addStretch();
    top->addWidget(m_timerLbl);
    top->addSpacing(12);
    top->addWidget(new QLabel("Difficoltà:"));
    top->addWidget(m_diffCombo);
    top->addSpacing(8);
    top->addWidget(m_newBtn);
    root->addLayout(top);

    // Board container
    m_boardWrap = new QWidget(this);
    m_grid = new QGridLayout(m_boardWrap);
    m_grid->setSpacing(0);
    m_grid->setContentsMargins(0,0,0,0);
    root->addWidget(m_boardWrap, 1);

    // Timer
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [this]{
        if (m_alive && m_started) {
            ++m_seconds;
            m_timerLbl->setText(QString("Tempo: %1 s").arg(m_seconds));
        }
    });

    // prima partita
    setDifficultyFromIndex(0);
    newGame();
}

void MinesPage::setDifficultyFromIndex(int idx){
    if (idx==0){ m_rows=9;  m_cols=9;  m_mines=10; }
    else if (idx==1){ m_rows=16; m_cols=16; m_mines=40; }
    else { m_rows=16; m_cols=30; m_mines=99; }
}

void MinesPage::rebuildUIGrid(){
    // pulisci
    QLayoutItem *it;
    while ((it = m_grid->takeAt(0))) {
        if (auto *w = it->widget()) w->deleteLater();
        delete it;
    }

    // crea
    for (int r=0; r<m_rows; ++r){
        for (int c=0; c<m_cols; ++c){
            auto *b = new CellButton(r,c,m_boardWrap);
            b->setFixedSize(34,34);                 // più grande
            QFont f = b->font(); f.setPointSize(16); // numeri più grandi
            f.setBold(true); b->setFont(f);
            // coperto (stile base) – contrasto + spigoli meno tondi
            b->setStyleSheet("QPushButton{background:#eef2f8; border:1px solid #b8c3d6; border-radius:4px;}");

            m_grid->addWidget(b, r, c);
            connect(b, &CellButton::left,  this, [this](int rr,int cc){ revealCell(rr,cc); });
            connect(b, &CellButton::right, this, [this](int rr,int cc){ toggleFlag(rr,cc); });
        }
    }
    m_boardWrap->updateGeometry();
}


void MinesPage::newGame(){
    m_started = false;
    m_alive   = true;
    m_seconds = 0;
    m_timerLbl->setText("Tempo: 0 s");

    m_isMine  = QVector<QVector<bool>>(m_rows, QVector<bool>(m_cols,false));
    m_adj     = QVector<QVector<int>>( m_rows, QVector<int>( m_cols, 0));
    m_revealed= QVector<QVector<bool>>(m_rows, QVector<bool>(m_cols,false));
    m_flagged = QVector<QVector<bool>>(m_rows, QVector<bool>(m_cols,false));
    m_toReveal = m_rows*m_cols - m_mines;

    rebuildUIGrid();
    updateStatus();
}

void MinesPage::placeMinesAvoiding(int sr, int sc){
    // Evita (sr,sc) e tutte le sue celle adiacenti per un primo click sicuro
    auto isSafe = [&](int r,int c){
        return qAbs(r-sr)<=1 && qAbs(c-sc)<=1;
    };
    int placed=0;
    while(placed < m_mines){
        int r = rnd(m_rows);
        int c = rnd(m_cols);
        if (m_isMine[r][c]) continue;
        if (isSafe(r,c)) continue;
        m_isMine[r][c]=true;
        ++placed;
    }
    computeAdjacencies();
}

void MinesPage::computeAdjacencies(){
    auto inside = [&](int r,int c){ return r>=0 && r<m_rows && c>=0 && c<m_cols; };
    for (int r=0;r<m_rows;r++){
        for (int c=0;c<m_cols;c++){
            if (m_isMine[r][c]) { m_adj[r][c] = -1; continue; }
            int cnt=0;
            for (int dr=-1; dr<=1; ++dr)
                for (int dc=-1; dc<=1; ++dc){
                    if (!dr && !dc) continue;
                    int rr=r+dr, cc=c+dc;
                    if (inside(rr,cc) && m_isMine[rr][cc]) ++cnt;
                }
            m_adj[r][c] = cnt;
        }
    }
}

void MinesPage::toggleFlag(int r,int c){
    if (!m_alive || m_revealed[r][c]) return;
    m_flagged[r][c] = !m_flagged[r][c];
    if (auto *b = qobject_cast<QPushButton*>(m_grid->itemAtPosition(r,c)->widget())){
        if (m_flagged[r][c]) {
            b->setText(QString::fromUtf8(u8"🚩"));
            b->setStyleSheet("QPushButton{background:#eef2f8; border:1px solid #b8c3d6; border-radius:4px; font-size:16px;}");
        } else {
            b->setText("");
            b->setStyleSheet("QPushButton{background:#eef2f8; border:1px solid #b8c3d6; border-radius:4px;}");
        }
    }
}

void MinesPage::revealCell(int r,int c){
    if (!m_alive) return;
    if (!m_started){
        // piazza mine al primo click
        placeMinesAvoiding(r,c);
        m_started = true;
        m_timer->start(1000);
    }
    if (m_flagged[r][c] || m_revealed[r][c]) return;

    m_revealed[r][c] = true;

    if (m_isMine[r][c]){
        // BOOM
        if (auto *b = qobject_cast<QPushButton*>(m_grid->itemAtPosition(r,c)->widget())){
            b->setText(QString::fromUtf8(u8"💣"));
            b->setStyleSheet("QPushButton{background:#ffdddd; border:1px solid #ff7a7a;}");
        }
        gameOver(false);
        return;
    }

    // aggiorna aspetto cella
    int v = m_adj[r][c];
    if (auto *b = qobject_cast<QPushButton*>(m_grid->itemAtPosition(r,c)->widget())){
        b->setEnabled(false);
        if (v > 0){
            auto colorOf = [](int n)->QString{
                static const char* cols[] = { "#000", "#1976d2","#388e3c","#d32f2f","#303f9f","#795548","#00838f","#000000","#5d4037" };
                return cols[qBound(1,n,8)];
            };
            b->setText(QString::number(v));
            b->setStyleSheet(QString(
                "QPushButton{background:#ffffff; border:1px solid #b8c3d6; border-radius:4px; "
                "color:%1; font-weight:900; font-size:16px;}").arg(colorOf(v)));
        } else {
            b->setText("");
            b->setStyleSheet("QPushButton{background:#ffffff; border:1px solid #b8c3d6; border-radius:4px;}");
        }
    }

    --m_toReveal;
    if (v==0) floodZeros(r,c);
    if (m_toReveal==0) { gameOver(true); return; }
    updateStatus();
}

void MinesPage::floodZeros(int r,int c){
    auto inside = [&](int rr,int cc){ return rr>=0 && rr<m_rows && cc>=0 && cc<m_cols; };
    auto colorOf = [](int n)->QString{
        static const char* cols[] = { "#000", "#1976d2","#388e3c","#d32f2f","#303f9f","#795548","#00838f","#000000","#5d4037" };
        return cols[qBound(1,n,8)];
    };

    QVector<QPair<int,int>> stack;
    stack.push_back({r,c});

    while(!stack.isEmpty()){
        const auto cell = stack.takeLast();
        const int rr = cell.first;
        const int cc = cell.second;

        for (int dr=-1; dr<=1; ++dr)
        for (int dc=-1; dc<=1; ++dc){
            if (!dr && !dc) continue;
            int nr = rr+dr, nc = cc+dc;
            if (!inside(nr,nc))                continue;
            if (m_revealed[nr][nc] || m_flagged[nr][nc]) continue;
            if (m_isMine[nr][nc])              continue;

            m_revealed[nr][nc] = true;
            auto *b = qobject_cast<QPushButton*>(m_grid->itemAtPosition(nr,nc)->widget());
            int v = m_adj[nr][nc];
            if (b){
                if (v > 0){
                    b->setText(QString::number(v));
                    b->setStyleSheet(QString(
                        "QPushButton{background:#ffffff; border:1px solid #b8c3d6; border-radius:4px; "
                        "color:%1; font-weight:900; font-size:16px;}").arg(colorOf(v)));
                } else {
                    b->setText("");
                    b->setStyleSheet("QPushButton{background:#ffffff; border:1px solid #b8c3d6; border-radius:4px;}");
                    stack.push_back({nr,nc}); // continua a espandere gli zero
                }
            }
            --m_toReveal;
        }
    }
}


void MinesPage::gameOver(bool win){
    m_alive = false;
    m_timer->stop();
    // scopri tutte le mine / flag errate
    for (int r=0;r<m_rows;++r)
        for (int c=0;c<m_cols;++c){
            if (m_isMine[r][c]){
                if (auto *b = qobject_cast<QPushButton*>(m_grid->itemAtPosition(r,c)->widget())){
                    b->setText(QString::fromUtf8(u8"💣"));
                    b->setStyleSheet("QPushButton{background:#ffecec; border:1px solid #ff9a9a;}");
                }
            } else if (m_flagged[r][c]) {
                if (auto *b = qobject_cast<QPushButton*>(m_grid->itemAtPosition(r,c)->widget())){
                    b->setText("×"); // flag errata
                    b->setStyleSheet("QPushButton{background:#ffecec; border:1px solid #ff9a9a; border-radius:4px;}");
                }
            }
        }
    if (win) m_statusLbl->setText(QString("Prato Fiorito — 🎉 Vittoria!  —  Tempo: %1 s").arg(m_seconds));
    else     m_statusLbl->setText(QString("Prato Fiorito — 💥 BOOM!  —  Tempo: %1 s").arg(m_seconds));
}

void MinesPage::updateStatus(){
    m_statusLbl->setText(QString("Prato Fiorito — 🙂  Mine: %1  —  Scoperte: %2/%3")
                         .arg(m_mines)
                         .arg((m_rows*m_cols - m_mines) - m_toReveal)
                         .arg(m_rows*m_cols - m_mines));
}

void MinesPage::keyPressEvent(QKeyEvent *e){
    if (e->key()==Qt::Key_Escape){ emit backRequested(); return; }
    if (e->key()==Qt::Key_N){ newGame(); return; }
    QWidget::keyPressEvent(e);
}

#include "minespage.moc"
