#include "anagrampage.h"
#include "words.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QtGlobal>
#include <algorithm>

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
  #include <QRandomGenerator>
  static int randIndex(int maxExclusive) {
      return (int)QRandomGenerator::global()->bounded(maxExclusive);
  }
  template<typename It>
  static void shuffleRange(It first, It last) {
      // usa QRandomGenerator come URBG
      std::shuffle(first, last, *QRandomGenerator::global());
  }
#else
  #include <random>
  static std::mt19937& rng() { static std::mt19937 r{ std::random_device{}() }; return r; }
  static int randIndex(int maxExclusive) {
      std::uniform_int_distribution<int> d(0, maxExclusive - 1);
      return d(rng());
  }
  template<typename It>
  static void shuffleRange(It first, It last) {
      std::shuffle(first, last, rng());
  }
#endif

AnagramPage::AnagramPage(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(12);

    m_titleLabel = new QLabel("Anagrammi — ricomponi la parola");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    QFont tf = m_titleLabel->font(); tf.setPointSize(16); tf.setBold(true);
    m_titleLabel->setFont(tf);
    root->addWidget(m_titleLabel);

    m_wordLabel = new QLabel;                 // es: _ _ _ _ _
    m_wordLabel->setAlignment(Qt::AlignCenter);
    QFont wf = m_wordLabel->font(); wf.setPointSize(22); wf.setLetterSpacing(QFont::AbsoluteSpacing, 2);
    m_wordLabel->setFont(wf);
    root->addWidget(m_wordLabel);

    m_guessLabel = new QLabel;                // es: C A S _
    m_guessLabel->setAlignment(Qt::AlignCenter);
    QFont gf = m_guessLabel->font(); gf.setPointSize(20);
    m_guessLabel->setFont(gf);
    root->addWidget(m_guessLabel);

    m_infoLabel = new QLabel("Seleziona le lettere nell'ordine giusto.");
    m_infoLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_infoLabel);

    m_resultLabel = new QLabel;
    m_resultLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_resultLabel);

    // Tastiera
    m_keys = new QGridLayout;
    m_keys->setHorizontalSpacing(6);
    m_keys->setVerticalSpacing(6);
    root->addLayout(m_keys);

    // Barra comandi
    auto *nav = new QHBoxLayout;
    auto *btnBack    = new QPushButton("Torna indietro");
    auto *btnHint    = new QPushButton("Indizio");
    auto *btnClear   = new QPushButton("Cancella");
    auto *btnSubmit  = new QPushButton("Invia");
    auto *btnNew     = new QPushButton("Nuova parola");
    nav->addWidget(btnBack);
    nav->addStretch();
    nav->addWidget(btnHint);
    nav->addWidget(btnClear);
    nav->addWidget(btnSubmit);
    nav->addWidget(btnNew);
    root->addLayout(nav);

    connect(btnBack,   &QPushButton::clicked, this, &AnagramPage::backRequested);
    connect(btnHint,   &QPushButton::clicked, this, &AnagramPage::onHint);
    connect(btnClear,  &QPushButton::clicked, this, [this]{ onBackspace(); });
    connect(btnSubmit, &QPushButton::clicked, this, &AnagramPage::onSubmit);
    connect(btnNew,    &QPushButton::clicked, this, &AnagramPage::newWord);

    newWord();
}

void AnagramPage::setLettersEnabled(bool en) {
    for (auto *b : m_letterBtns) b->setEnabled(en);
}

void AnagramPage::setSecret(const QString &w) {
    m_secret = w.toUpper();
    m_guess.clear();
    m_usedStack.clear();

    // label parola target come trattini
    QString blanks;
    for (int i=0;i<m_secret.size();++i) {
        if (i) blanks += ' ';
        blanks += '_';
    }
    m_wordLabel->setText(blanks);

    // guess iniziale vuota
    m_guessLabel->setText("");
    m_resultLabel->clear();
    m_resultLabel->setStyleSheet("");
    m_infoLabel->setText(QString("Lunghezza: %1").arg(m_secret.size()));
}

void AnagramPage::buildKeyboard(const QString &letters) {
    // pulisci tastiera precedente
    QLayoutItem *item;
    while ((item = m_keys->takeAt(0)) != nullptr) {
        if (auto *w = item->widget()) w->deleteLater();
        delete item;
    }
    m_letterBtns.clear();

    // disporre le lettere in righe da max 10
    for (int i=0; i<letters.size(); ++i) {
        QChar ch = letters[i];
        auto *b = new QPushButton(QString(ch));
        b->setMinimumSize(36,36);
        int row = i / 10;
        int col = i % 10;
        m_keys->addWidget(b, row, col);
        m_letterBtns.push_back(b);
        connect(b, &QPushButton::clicked, this, [this, b]{ appendLetterFromButton(b); });
    }
}

void AnagramPage::newWord() {
    // scegli parola casuale
    int idx = randIndex(WORDS_N);
    setSecret(QString::fromLatin1(WORDS[idx]));

    // genera lettere mescolate (solo quelle della parola, niente esche per ora)
    QString letters = m_secret;
    // mescola
    QVector<QChar> vec; vec.reserve(letters.size());
    for (QChar c : letters) vec.push_back(c);
    shuffleRange(vec.begin(), vec.end());
    letters.clear(); for (QChar c : vec) letters.append(c);

    buildKeyboard(letters);
    setLettersEnabled(true);
}

void AnagramPage::appendLetterFromButton(QPushButton *b) {
    if (!b || !b->isEnabled()) return;
    m_guess += b->text();
    b->setEnabled(false);
    m_usedStack.push_back(b);

    // aggiorna guess label con spazi
    QString spaced;
    for (int i=0;i<m_guess.size();++i) {
        if (i) spaced += ' ';
        spaced += m_guess[i];
    }
    m_guessLabel->setText(spaced);
}

void AnagramPage::onBackspace() {
    if (m_guess.isEmpty()) return;
    m_guess.chop(1);
    if (!m_usedStack.isEmpty()) {
        auto *b = m_usedStack.back();
        m_usedStack.pop_back();
        if (b) b->setEnabled(true);
    }
    QString spaced;
    for (int i=0;i<m_guess.size();++i) {
        if (i) spaced += ' ';
        spaced += m_guess[i];
    }
    m_guessLabel->setText(spaced);
}

void AnagramPage::onSubmit() {
    if (m_guess.size() != m_secret.size()) {
        m_infoLabel->setText("Completa tutte le lettere prima di inviare.");
        return;
    }
    if (m_guess == m_secret) {
        m_resultLabel->setText("BRAVO!");
        m_resultLabel->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
        m_infoLabel->setText("Hai ricomposto correttamente. 'Nuova parola' per rigiocare.");
        setLettersEnabled(false);
    } else {
        // feedback: quante lettere in posizione giusta
        int correct = 0;
        for (int i=0;i<m_secret.size();++i) if (m_guess[i] == m_secret[i]) ++correct;
        m_resultLabel->setText("SEI SCARSO!");
        m_resultLabel->setStyleSheet("color: red; font-weight: bold; font-size: 18px;");
        m_infoLabel->setText(QString("Lettere giuste al posto giusto: %1/%2").arg(correct).arg(m_secret.size()));
    }
}

void AnagramPage::onHint() {
    // rivela la prossima lettera corretta
    int pos = m_guess.size();
    if (pos >= m_secret.size()) return;
    QChar need = m_secret[pos];

    // trova un bottone ancora abilitato con quella lettera
    for (auto *b : m_letterBtns) {
        if (b->isEnabled() && b->text().size()==1 && b->text()[0] == need) {
            appendLetterFromButton(b);
            m_infoLabel->setText("Indizio usato: rivelata una lettera.");
            return;
        }
    }
    // se non trovato (esaurita quella lettera nella tastiera), non fare nulla
}
