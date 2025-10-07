#include "memoryconfigpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>

MemoryConfigPage::MemoryConfigPage(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(12);

    auto *title = new QLabel("Memory — scegli la dimensione");
    title->setAlignment(Qt::AlignCenter);
    QFont tf = title->font(); tf.setPointSize(16); tf.setBold(true);
    title->setFont(tf);
    root->addWidget(title);

    // riga con due scelte (anche rettangolari)
    auto rowRect = [&](int r1,int c1,int r2,int c2){
        auto *h = new QHBoxLayout;
        auto mk = [&](int r,int c){
            auto *btn = new QPushButton(QString("%1 x %2").arg(r).arg(c));
            btn->setMinimumHeight(40);
            QObject::connect(btn, &QPushButton::clicked, this, [this,r,c]{ choose(r,c); });
            return btn;
        };
        h->addStretch();
        h->addWidget(mk(r1,c1));
        h->addSpacing(12);
        h->addWidget(mk(r2,c2));
        h->addStretch();
        root->addLayout(h);
    };

    // Nuove modalità + quelle già esistenti
    rowRect(3,2, 3,4);
    rowRect(4,4, 6,6);
    rowRect(8,8,10,10);

    auto *nav = new QHBoxLayout;
    auto *back = new QPushButton("Torna indietro");
    nav->addWidget(back);
    nav->addStretch();
    root->addLayout(nav);
    connect(back, &QPushButton::clicked, this, &MemoryConfigPage::backRequested);
}

void MemoryConfigPage::choose(int r, int c) {
    emit startRequested(r, c);
}
