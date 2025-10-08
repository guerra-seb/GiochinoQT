#include "secondpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QFont>
#include <QPair>
#include <algorithm>

// QRandomGenerator è disponibile da Qt 5.10 in poi. Se sei su Qt < 5.10, usiamo C++ <random>.
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
  #include <QRandomGenerator>
#else
  #include <random>
  static int randomDigitFallback() {
      static std::mt19937 rng{ std::random_device{}() };
      static std::uniform_int_distribution<int> dist(0, 9);
      return dist(rng);
  }
#endif

SecondPage::SecondPage(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(12);

    // Titolo
    auto *title = new QLabel("Becca il Codice — indovina il codice di 4 cifre");
    title->setAlignment(Qt::AlignCenter);
    QFont tf = title->font(); tf.setPointSize(16); tf.setBold(true);
    title->setFont(tf);
    root->addWidget(title);

    // Display del codice inserito
    m_display = new QLineEdit;
    m_display->setReadOnly(true);          // si usa la tastiera numerica
    m_display->setAlignment(Qt::AlignCenter);
    m_display->setMaxLength(m_codeLen);
    QFont df = m_display->font(); df.setPointSize(18);
    m_display->setFont(df);
    root->addWidget(m_display);

    // Tastiera numerica 0-9 + controlli
    auto *grid = new QGridLayout; // 3x4 numeri + riga controlli
    grid->setHorizontalSpacing(8);
    grid->setVerticalSpacing(8);

    // Bottoni 1..9
    for (int i = 1; i <= 9; ++i) {
        int r = (i-1) / 3;
        int c = (i-1) % 3;
        auto *btn = new QPushButton(QString::number(i));
        btn->setMinimumSize(64, 48);
        connect(btn, &QPushButton::clicked, this, [this, i]{ onDigitPressed(i); });
        grid->addWidget(btn, r, c);
    }

    // Riga finale: Canc | 0 | ←
    {
        auto *btnClear = new QPushButton("Canc");
        btnClear->setMinimumSize(64, 48);
        connect(btnClear, &QPushButton::clicked, this, &SecondPage::onClear);
        grid->addWidget(btnClear, 3, 0);

        auto *btnZero = new QPushButton("0");
        btnZero->setMinimumSize(64, 48);
        connect(btnZero, &QPushButton::clicked, this, [this]{ onDigitPressed(0); });
        grid->addWidget(btnZero, 3, 1);

        auto *btnBack = new QPushButton(QString::fromUtf8("\xE2\x86\x90")); // ←
        btnBack->setMinimumSize(64, 48);
        connect(btnBack, &QPushButton::clicked, this, &SecondPage::onBackspace);
        grid->addWidget(btnBack, 3, 2);
    }

    // Pulsante Invio (sottomette il tentativo)
    auto *btnSubmit = new QPushButton("Invio");
    btnSubmit->setMinimumHeight(48);
    connect(btnSubmit, &QPushButton::clicked, this, &SecondPage::onSubmit);
    grid->addWidget(btnSubmit, 4, 0, 1, 3); // span su 3 colonne

    root->addLayout(grid);

    // Risultati & indizi
    m_resultLabel = new QLabel;
    m_resultLabel->setAlignment(Qt::AlignCenter);
    m_hintLabel = new QLabel("Inserisci il codice...");
    m_hintLabel->setAlignment(Qt::AlignCenter);
    m_hintLabel->setWordWrap(true);
    root->addWidget(m_resultLabel);
    root->addWidget(m_hintLabel);

    // Cronologia tentativi
    m_history = new QListWidget;
    m_history->setMinimumHeight(120);
    root->addWidget(m_history);

    // Navigazione pagina + nuovo codice
    auto *nav = new QHBoxLayout;
    auto *btnBackPage = new QPushButton("Torna indietro");
    auto *btnNewCode  = new QPushButton("Nuovo codice");
    nav->addWidget(btnBackPage);
    nav->addStretch();
    nav->addWidget(btnNewCode);
    root->addLayout(nav);

    connect(btnBackPage, &QPushButton::clicked, this, &SecondPage::backRequested);
    connect(btnNewCode,  &QPushButton::clicked, this, &SecondPage::newSecret);

    newSecret(); // genera il primo codice
}

void SecondPage::newSecret() {
    // Genera un codice numerico (ripetizioni permesse)
    QString s;
    s.reserve(m_codeLen);
    for (int i = 0; i < m_codeLen; ++i) {
    #if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
        int d = QRandomGenerator::global()->bounded(10); // 0..9
    #else
        int d = randomDigitFallback(); // 0..9
    #endif
        s.append(QChar('0' + d));
    }
    m_secret = s;

    // Reset UI
    m_display->clear();
    m_resultLabel->clear();
    m_hintLabel->setText("Nuovo codice generato. Prova a indovinare!");
    m_resultLabel->setStyleSheet("");
    m_history->clear();
}

void SecondPage::onDigitPressed(int d) {
    if (m_display->text().size() < m_codeLen)
        m_display->setText(m_display->text() + QString::number(d));
}

void SecondPage::onBackspace() {
    auto t = m_display->text();
    if (!t.isEmpty()) {
        t.chop(1);
        m_display->setText(t);
    }
}

void SecondPage::onClear() {
    m_display->clear();
}

QPair<int,int> SecondPage::computeBullsCows(const QString &guess) const {
    // Bulls = cifre giuste al posto giusto
    // Cows  = cifre presenti ma in posizione diversa
    int bulls = 0, cows = 0;
    int sc[10] = {0};
    int gc[10] = {0};

    for (int i = 0; i < m_codeLen; ++i) {
        if (guess[i] == m_secret[i]) ++bulls;
        ++sc[m_secret[i].digitValue()];
        ++gc[guess[i].digitValue()];
    }
    for (int d = 0; d < 10; ++d)
        cows += std::min(sc[d], gc[d]);
    cows -= bulls; // rimuovi quelli già contati come bulls

    return {bulls, cows};
}

void SecondPage::onSubmit() {
    const QString g = m_display->text();
    if (g.size() != m_codeLen) {
        m_hintLabel->setText(QString("Inserisci %1 cifre.").arg(m_codeLen));
        return;
    }

    auto res = computeBullsCows(g);
    int bulls = res.first;
    int cows  = res.second;

    // Aggiorna cronologia
    m_history->addItem(QString("%1  →  %2 giuste al posto giusto, %3 giuste ma posto sbagliato")
                       .arg(g).arg(bulls).arg(cows));
    m_history->scrollToBottom();

    if (bulls == m_codeLen) {
        m_resultLabel->setText("BRAVO!");
        m_resultLabel->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
        m_hintLabel->setText("Hai indovinato! Premi 'Nuovo codice' per rigiocare.");
    } else {
        m_resultLabel->setText("SEI SCARSO!");
        m_resultLabel->setStyleSheet("color: red; font-weight: bold; font-size: 18px;");
        m_hintLabel->setText(QString("Indizio: %1 al posto giusto, %2 presenti ma in altra posizione.")
                             .arg(bulls).arg(cows));
    }

    m_display->clear(); // pronta per il prossimo tentativo
}
