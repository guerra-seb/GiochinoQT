#include <QApplication>
#include "mainwindow.h"
#include "thememanager.h"
#include <QCoreApplication>

static const char* APP_STYLE_LIGHT = R"(
* { font-family: 'Segoe UI','Noto Sans','Arial'; font-size: 14px; }
QWidget { background:#ffffff; color:#111; }
QPushButton {
  background:#e9eef5; border:1px solid #c7d1e0; border-radius:10px; padding:6px 12px;
}
QPushButton:hover { background:#f3f6fb; }
QPushButton:pressed { background:#dbe4f3; }
QLineEdit {
  background:#fff; color:#111; border:1px solid #c7d1e0; border-radius:8px; padding:4px 6px;
  selection-background-color:#cde1ff; selection-color:#000;
}
QGroupBox { border:1px solid #c7d1e0; border-radius:12px; margin-top:12px; }
QGroupBox::title { subcontrol-origin: margin; left:10px; padding:0 4px; }
QToolButton {
  background:#ffffff; border:1px solid #d9e0ea; border-radius:16px; padding:12px;
}
QToolButton:hover { background:#f6f9fe; }
QStatusBar { background:#f7f7f9; }
)";

static const char* APP_STYLE_DARK = R"(
* { font-family: 'Segoe UI','Noto Sans','Arial'; font-size: 14px; }
QWidget { background:#121419; color:#e9edf3; }
QPushButton {
  background:#1e2530; border:1px solid #2d3847; border-radius:10px; padding:6px 12px;
}
QPushButton:hover { background:#242d3b; }
QPushButton:pressed { background:#1a202a; }
QLineEdit {
  background:#0f1318; color:#e9edf3; border:1px solid #2d3847; border-radius:8px; padding:4px 6px;
  selection-background-color:#35507a; selection-color:#fff;
}
QGroupBox { border:1px solid #2d3847; border-radius:12px; margin-top:12px; }
QGroupBox::title { subcontrol-origin: margin; left:10px; padding:0 4px; }
QToolButton {
  background:#131922; border:1px solid #2d3847; border-radius:16px; padding:12px;
}
QToolButton:hover { background:#1a2230; }
QStatusBar { background:#0e1218; }
)";


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    qApp->setStyleSheet(APP_STYLE_LIGHT);   // oppure APP_STYLE_DARK
    ThemeManager::instance().applySavedTheme(); // COSA NUOVA
    MainWindow w;
    w.resize(600, 400);
    w.show();
    return app.exec();
}
