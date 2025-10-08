#include "homepage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QToolButton>
#include <QGridLayout>
#include <QGraphicsDropShadowEffect>
#include <QStyle>
#include <QSizePolicy>
#include <QIcon>

HomePage::HomePage(QWidget *parent) : QWidget(parent) {
    // Layout radice
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(8);

    // Titolo
    auto *title = new QLabel("Benvenuto in sto' giochino");
    title->setAlignment(Qt::AlignCenter);
    QFont f = title->font(); f.setPointSize(20); f.setBold(true);
    title->setFont(f);
    root->addWidget(title);
    root->addSpacing(12);

    // Griglia di "card"
    auto *grid = new QGridLayout();
    grid->setSpacing(16);

    auto makeCard = [&](const QString &text, const QIcon &icon) {
        auto *b = new QToolButton;
        b->setText(text);
        b->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        b->setIcon(icon);
        b->setIconSize(QSize(56,56));
        b->setMinimumSize(160,120);
        b->setCursor(Qt::PointingHandCursor);

        auto *fx = new QGraphicsDropShadowEffect(b);
        fx->setBlurRadius(24);
        fx->setOffset(0,6);
        fx->setColor(QColor(0,0,0,80));
        b->setGraphicsEffect(fx);
        return b;
    };


    auto *btnMemory = makeCard("Memory (Coppie)", QIcon(":/icons/icons/play-card-star.svg"));
    auto *btnCode   = makeCard("Becca il Codice", QIcon(":/icons/icons/circle-key.svg"));
    auto *btnHang   = makeCard("Impiccato",       QIcon(":/icons/icons/hanger.svg"));
    auto *btnAnagr  = makeCard("Anagrammi",       QIcon(":/icons/icons/arrows-random.svg"));
    auto *btnSudoku = makeCard("Sudoku",          QIcon(":/icons/icons/grid-dots.svg"));
    auto *btn2048   = makeCard("2048",            QIcon(":/icons/icons/number-64-small.svg"));

    // Disposizione 2 colonne
    int r=0, c=0;
    auto place = [&](QWidget *w) { grid->addWidget(w, r, c); if (++c>=2) { c=0; ++r; } };
    place(btnCode); place(btnHang); place(btnAnagr); place(btnMemory); place(btnSudoku); place(btn2048);

    root->addLayout(grid);
    root->addStretch();

    // Collegamenti
    connect(btnCode,   &QToolButton::clicked, this, &HomePage::startCodeClicked);
    connect(btnHang,   &QToolButton::clicked, this, &HomePage::startHangmanClicked);
    connect(btnAnagr,  &QToolButton::clicked, this, &HomePage::startAnagramClicked);
    connect(btnMemory, &QToolButton::clicked, this, &HomePage::startMemoryClicked);
    connect(btnSudoku, &QToolButton::clicked, this, &HomePage::startSudokuClicked);
    connect(btn2048, &QToolButton::clicked, this, &HomePage::start2048Clicked);
}
