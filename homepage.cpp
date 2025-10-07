#include "homepage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>

HomePage::HomePage(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel("Benvenuto in sto' giochino");
    title->setAlignment(Qt::AlignCenter);
    QFont f = title->font(); f.setPointSize(20); f.setBold(true);
    title->setFont(f);

    // Bottoni dei giochi
    auto *btnCode    = new QPushButton("Becca il Codice");
    auto *btnHangman = new QPushButton("Impiccato");
    auto *btnAnagram = new QPushButton("Anagrammi");
    auto *btnMemory  = new QPushButton("Memory (Coppie)");
    auto *btnSudoku  = new QPushButton("Sudoku");

    btnCode->setMinimumHeight(40);
    btnHangman->setMinimumHeight(40);
    for (auto *b : {btnCode, btnHangman, btnAnagram, btnMemory, btnSudoku}) b->setMinimumHeight(40);


    layout->setContentsMargins(16,16,16,16);
    layout->addWidget(title);
    layout->addSpacing(12);
    layout->addWidget(btnCode);
    layout->addWidget(btnHangman);
    layout->addWidget(btnAnagram);
    layout->addWidget(btnMemory);
    layout->addWidget(btnSudoku);
    layout->addStretch();

    connect(btnCode,    &QPushButton::clicked, this, &HomePage::startCodeClicked);
    connect(btnHangman, &QPushButton::clicked, this, &HomePage::startHangmanClicked);
    connect(btnAnagram, &QPushButton::clicked, this, &HomePage::startAnagramClicked);
    connect(btnMemory,  &QPushButton::clicked, this, &HomePage::startMemoryClicked);
    connect(btnSudoku,  &QPushButton::clicked, this, &HomePage::startSudokuClicked);
}
