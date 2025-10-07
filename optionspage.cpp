#include "optionspage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

OptionsPage::OptionsPage(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    auto *lbl = new QLabel("Pagina Opzioni ⚙️");
    lbl->setAlignment(Qt::AlignCenter);

    auto *btnHome = new QPushButton("Torna alla Home");

    layout->setContentsMargins(16,16,16,16);
    layout->addWidget(lbl);
    layout->addStretch();
    layout->addWidget(btnHome, 0, Qt::AlignHCenter);

    connect(btnHome, &QPushButton::clicked, this, &OptionsPage::homeRequested);
}
