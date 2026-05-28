#include "main_window.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QTime>
#include <QVBoxLayout>

namespace {

QString label(const wchar_t* text) {
    return QString::fromWCharArray(text);
}

}  // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      timeLabel_(nullptr),
      mapWidget_(nullptr),
      overviewWindow_(nullptr),
      currentTime_(QDateTime::currentDateTime()) {
    currentTime_.setTime(QTime(currentTime_.time().hour(), 0, 0));
    createBins();

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(createTopPanel());

    mapWidget_ = new MapWidget(&bins_, centralWidget);
    mapWidget_->setBinClickedHandler([this](std::size_t binIndex) {
        openOverview(binIndex);
    });
    layout->addWidget(mapWidget_, 1);

    setCentralWidget(centralWidget);
    setWindowTitle(label(L"Панель мусора"));
    resize(1120, 720);
    refreshUi();
}

QWidget* MainWindow::createTopPanel() {
    QWidget* topPanel = new QWidget(this);
    topPanel->setFixedHeight(66);
    topPanel->setStyleSheet(QString::fromUtf8(u8"background: #000000; border-bottom: 1px solid #000000;"));

    timeLabel_ = new QLabel(topPanel);
    QFont timeFont = timeLabel_->font();
    timeFont.setPointSize(19);
    timeFont.setBold(true);
    timeLabel_->setFont(timeFont);

    QLabel* modeLabel = new QLabel(label(L"Ручной режим"), topPanel);
    QFont modeFont = modeLabel->font();
    modeFont.setPointSize(17);
    modeLabel->setFont(modeFont);

    QPushButton* advanceButton = new QPushButton(label(L"+1 час"), topPanel);
    advanceButton->setMinimumWidth(112);
    advanceButton->setStyleSheet(QString::fromUtf8(u8"QPushButton { background: #000000; border: 1px solid #000000; border-radius: 4px; padding: 8px 14px; font-size: 15px; } QPushButton:hover { background: #000000; }"));
    connect(advanceButton, &QPushButton::clicked, this, [this]() {
        advanceOneHour();
    });

    QHBoxLayout* layout = new QHBoxLayout(topPanel);
    layout->setContentsMargins(24, 10, 24, 10);
    layout->setSpacing(22);
    layout->addWidget(timeLabel_);
    layout->addWidget(modeLabel);
    layout->addStretch(1);
    layout->addWidget(advanceButton);

    return topPanel;
}

void MainWindow::createBins() {
    bins_.emplace_back(1, L"ул. Черемша", MapCoordinate{0.40, 0.34}, 18.0, 5.5);
    bins_.emplace_back(2, L"пр. Мира", MapCoordinate{0.70, 0.32}, 52.0, 7.0);
    bins_.emplace_back(3, L"ул. Лесная", MapCoordinate{0.35, 0.76}, 74.0, 4.0);
    bins_.emplace_back(4, L"ул. Южная", MapCoordinate{0.73, 0.70}, 94.0, 3.0);
}

void MainWindow::refreshUi() {
    timeLabel_->setText(label(L"Мировое время: %1").arg(formatDateTime(currentTime_)));
    mapWidget_->refreshBins();

    if (overviewWindow_) {
        overviewWindow_->updateContents();
    }
}

void MainWindow::advanceOneHour() {
    currentTime_ = currentTime_.addSecs(3600);
    const long long currentHour = hourIndexFor(currentTime_);

    for (GarbageBin& bin : bins_) {
        if (bin.pickupDue(currentHour)) {
            bin.applyScheduledPickup();
        } else {
            bin.advanceOneHour();
        }
    }

    refreshUi();
}

void MainWindow::openOverview(std::size_t binIndex) {
    if (overviewWindow_) {
        overviewWindow_->close();
        overviewWindow_ = nullptr;
    }

    overviewWindow_ = new OverviewWindow(&bins_.at(binIndex),
                                         [this, binIndex]() {
                                             schedulePickup(binIndex);
                                         },
                                         [this]() {
                                             return currentTime_;
                                         },
                                         this);
    overviewWindow_->setAttribute(Qt::WA_DeleteOnClose);
    connect(overviewWindow_, &QObject::destroyed, this, [this]() {
        overviewWindow_ = nullptr;
    });
    overviewWindow_->show();
}

void MainWindow::schedulePickup(std::size_t binIndex) {
    bins_.at(binIndex).schedulePickup(hourIndexFor(nextPickupTime()));
    refreshUi();
}

QDateTime MainWindow::nextPickupTime() const {
    QDateTime pickupTime = currentTime_.addSecs(3600);
    pickupTime.setTime(QTime(pickupTime.time().hour(), 0, 0));

    if (pickupTime.time().hour() < 9) {
        pickupTime.setTime(QTime(9, 0, 0));
        return pickupTime;
    }

    if (pickupTime.time().hour() > 21) {
        pickupTime = pickupTime.addDays(1);
        pickupTime.setTime(QTime(9, 0, 0));
    }

    return pickupTime;
}

QString MainWindow::formatDateTime(const QDateTime& dateTime) const {
    return dateTime.toString(QString::fromUtf8(u8"dd.MM.yyyy HH:mm"));
}

long long MainWindow::hourIndexFor(const QDateTime& dateTime) const {
    return dateTime.toSecsSinceEpoch() / 3600;
}
