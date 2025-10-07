#include "memorypage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QElapsedTimer>
#include <QFont>
#include <QtGlobal>
#include <algorithm>

#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
  #include <QRandomGenerator>
  static int rndRange(int maxExclusive){ return (int)QRandomGenerator::global()->bounded(maxExclusive); }
  template<typename It> static void shuffleRange(It f, It l){ std::shuffle(f, l, *QRandomGenerator::global()); }
#else
  #include <random>
  static std::mt19937& rng(){ static std::mt19937 r{ std::random_device{}() }; return r; }
  static int rndRange(int maxExclusive){ std::uniform_int_distribution<int> d(0,maxExclusive-1); return d(rng()); }
  template<typename It> static void shuffleRange(It f, It l){ std::shuffle(f, l, rng()); }
#endif

MemoryPage::MemoryPage(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(12);

    m_title = new QLabel("Memory — trova le coppie");
    m_title->setAlignment(Qt::AlignCenter);
    QFont tf = m_title->font(); tf.setPointSize(16); tf.setBold(true);
    m_title->setFont(tf);
    root->addWidget(m_title);

    m_info = new QLabel;
    m_info->setAlignment(Qt::AlignCenter);
    root->addWidget(m_info);

    m_result = new QLabel;
    m_result->setAlignment(Qt::AlignCenter);
    root->addWidget(m_result);

    m_grid = new QGridLayout;
    m_grid->setHorizontalSpacing(6);
    m_grid->setVerticalSpacing(6);
    root->addLayout(m_grid);

    auto *nav = new QHBoxLayout;
    auto *btnBack  = new QPushButton("Torna indietro");
    auto *btnSize  = new QPushButton("Cambia dimensione");
    auto *btnNew   = new QPushButton("Nuova partita");
    nav->addWidget(btnBack);
    nav->addStretch();
    nav->addWidget(btnSize);
    nav->addWidget(btnNew);
    root->addLayout(nav);

    connect(btnBack, &QPushButton::clicked, this, &MemoryPage::backRequested);
    connect(btnSize, &QPushButton::clicked, this, &MemoryPage::changeSizeRequested);
    connect(btnNew,  &QPushButton::clicked, this, [this]{ startNew(m_rows, m_cols); });

    m_tick = new QTimer(this);
    m_elapsed = new QElapsedTimer();
    connect(m_tick, &QTimer::timeout, this, &MemoryPage::updateInfo);

    startNew(4,4);
}

void MemoryPage::buildSymbols(int rows, int cols){
    // genera (rows*cols)/2 simboli distinti, qui numeri a due cifre: 01..50
    int pairs = (rows*cols)/2;
    QVector<QString> base; base.reserve(pairs);
    for (int i=1;i<=pairs;i++) base.push_back(QString("%1").arg(i,2,10,QChar('0')));
    m_symbols = base;
    m_symbols += base;             // duplica
    shuffleRange(m_symbols.begin(), m_symbols.end());
}

void MemoryPage::buildGrid(){
    // pulisci griglia e bottoni
    QLayoutItem *it;
    while ((it = m_grid->takeAt(0)) != nullptr) {
        if (auto *w = it->widget()) w->deleteLater();
        delete it;
    }
    m_cards.clear();
    m_cards.resize(m_symbols.size());

    // dimensiona bottoni
    int h = m_rows, w = m_cols;
    for (int i=0;i<h;i++){
        for (int j=0;j<w;j++){
            int idx = i*w + j;
            auto *b = new QPushButton("?");
            b->setMinimumSize(40,40);
            b->setCheckable(false);
            m_grid->addWidget(b, i, j);
            m_cards[idx] = b;
            connect(b, &QPushButton::clicked, this, [this, idx]{ onCardClicked(idx); });
        }
    }
}

void MemoryPage::startNew(int rows, int cols){
    // fallback su 4x4 se config non valida o dispari
    if (rows<=0 || cols<=0 || (rows*cols)%2!=0) { rows=4; cols=4; }
    m_rows = rows;
    m_cols = cols;
    m_first = m_second = -1;
    m_lock = false;
    m_moves = 0;
    m_matchedPairs = 0;
    m_result->clear();
    m_result->setStyleSheet("");
    m_title->setText(QString("Memory — %1 x %2").arg(m_rows).arg(m_cols));

    buildSymbols(m_rows, m_cols);
    buildGrid();

    m_elapsed->restart();
    m_tick->start(1000);
    updateInfo();
}

void MemoryPage::updateInfo(){
    qint64 secs = m_elapsed->isValid() ? m_elapsed->elapsed()/1000 : 0;
    int mm = (int)(secs/60), ss = (int)(secs%60);
    m_info->setText(QString("Mosse: %1   —   Tempo: %2:%3")
                    .arg(m_moves).arg(mm,2,10,QChar('0')).arg(ss,2,10,QChar('0')));
}

void MemoryPage::reveal(int idx){
    auto *b = m_cards[idx];
    b->setText(m_symbols[idx]);
    b->setEnabled(false);
}

void MemoryPage::hidePair(int a, int b){
    auto *ba = m_cards[a];
    auto *bb = m_cards[b];
    ba->setText("?");
    bb->setText("?");
    ba->setEnabled(true);
    bb->setEnabled(true);
}

void MemoryPage::onCardClicked(int idx){
    if (m_lock) return;
    auto *b = m_cards[idx];
    if (!b->isEnabled()) return;

    reveal(idx);

    if (m_first < 0){
        m_first = idx;
        return;
    }
    if (m_second < 0 && idx != m_first){
        m_second = idx;
        ++m_moves;
        updateInfo();

        if (m_symbols[m_first] == m_symbols[m_second]){
            // coppia trovata (restano disabilitati)
            m_first = m_second = -1;
            ++m_matchedPairs;
            if (m_matchedPairs == (m_rows*m_cols)/2){
                m_tick->stop();
                m_result->setText("BRAVO!");
                m_result->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
            }
        } else {
            // mismatch → breve pausa e richiudi
            m_lock = true;
            m_result->setText("SEI SCARSO!");
            m_result->setStyleSheet("color: red; font-weight: bold; font-size: 18px;");
            int a = m_first, b = m_second;
            QTimer::singleShot(700, this, [this,a,b]{
                hidePair(a,b);
                m_first = m_second = -1;
                m_lock = false;
                m_result->clear();
                m_result->setStyleSheet("");
            });
        }
    }
}
