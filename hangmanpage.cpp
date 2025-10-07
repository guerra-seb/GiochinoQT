#include "hangmanpage.h"
#include "words.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
  #include <QRandomGenerator>
#else
  #include <random>
  static int randomIndexFallback(int maxExclusive) {
      static std::mt19937 rng{ std::random_device{}() };
      std::uniform_int_distribution<int> dist(0, maxExclusive - 1);
      return dist(rng);
  }
#endif

HangmanPage::HangmanPage(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(12);

    m_titleLabel = new QLabel("Impiccato — indovina la parola");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    QFont tf = m_titleLabel->font(); tf.setPointSize(16); tf.setBold(true);
    m_titleLabel->setFont(tf);
    root->addWidget(m_titleLabel);

    m_wordLabel = new QLabel;
    m_wordLabel->setAlignment(Qt::AlignCenter);
    QFont wf = m_wordLabel->font(); wf.setPointSize(22); wf.setLetterSpacing(QFont::AbsoluteSpacing, 2);
    m_wordLabel->setFont(wf);
    root->addWidget(m_wordLabel);

    m_infoLabel = new QLabel("Scegli una lettera.");
    m_infoLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_infoLabel);

    m_resultLabel = new QLabel;
    m_resultLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_resultLabel);

    // Tastiera A-Z
    m_keys = new QGridLayout;
    m_keys->setHorizontalSpacing(6);
    m_keys->setVerticalSpacing(6);
    const QString letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < letters.size(); ++i) {
        QChar ch = letters[i];
        auto *b = new QPushButton(QString(ch));
        b->setMinimumSize(36,36);
        int row = i / 10; // 3 righe: 10+10+6
        int col = i % 10;
        if (row == 2) col = i - 20; // ultime 6 colonne
        m_keys->addWidget(b, row, col);
        m_letterBtns.push_back(b);
        connect(b, &QPushButton::clicked, this, [this, ch, b]{ b->setEnabled(false); onLetter(ch); });
    }
    root->addLayout(m_keys);

    // Navigazione e controllo
    auto *nav = new QHBoxLayout;
    auto *btnBack = new QPushButton("Torna indietro");
    auto *btnNew  = new QPushButton("Nuova parola");
    nav->addWidget(btnBack);
    nav->addStretch();
    nav->addWidget(btnNew);
    root->addLayout(nav);

    connect(btnBack, &QPushButton::clicked, this, &HangmanPage::backRequested);
    connect(btnNew,  &QPushButton::clicked, this, &HangmanPage::newWord);

    newWord();
}

void HangmanPage::setLettersEnabled(bool en) {
    for (auto *b : m_letterBtns) b->setEnabled(en);
}

void HangmanPage::newWord() {
    // scegli parola casuale
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    int idx = (int)QRandomGenerator::global()->bounded(WORDS_N);
#else
    int idx = randomIndexFallback(WORDS_N);
#endif
    setSecret(QString::fromLatin1(WORDS[idx]));

    // reset UI stato
    m_wrong = 0;
    m_resultLabel->clear();
    m_resultLabel->setStyleSheet("");
    m_infoLabel->setText(QString("Tentativi rimasti: %1").arg(m_maxWrong - m_wrong));
    setLettersEnabled(true);
}

void HangmanPage::setSecret(const QString &w) {
    m_secret = w.toUpper();
    m_progress = QString(m_secret.size(), QChar('_'));
    refreshWordLabel();
}

void HangmanPage::refreshWordLabel() {
    // visualizza con spazi: C _ S A
    QString spaced;
    for (int i=0;i<m_progress.size();++i) {
        if (i) spaced += ' ';
        spaced += m_progress[i];
    }
    m_wordLabel->setText(spaced);
}

void HangmanPage::onLetter(QChar ch) {
    ch = ch.toUpper();
    bool hit = false;
    for (int i=0;i<m_secret.size();++i) {
        if (m_secret[i] == ch) {
            m_progress[i] = ch;
            hit = true;
        }
    }
    if (hit) {
        refreshWordLabel();
        if (!m_progress.contains('_')) {
            m_resultLabel->setText("BRAVO!");
            m_resultLabel->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
            m_infoLabel->setText("Hai indovinato! Premi 'Nuova parola' per rigiocare.");
            setLettersEnabled(false);
        }
    } else {
        ++m_wrong;
        m_infoLabel->setText(QString("Tentativi rimasti: %1").arg(m_maxWrong - m_wrong));
        if (m_wrong >= m_maxWrong) {
            m_resultLabel->setText("SEI SCARSO!");
            m_resultLabel->setStyleSheet("color: red; font-weight: bold; font-size: 18px;");
            m_infoLabel->setText(QString("La parola era: %1").arg(m_secret));
            setLettersEnabled(false);
        }
    }
}
