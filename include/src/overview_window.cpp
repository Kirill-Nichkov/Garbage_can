#include "overview_window.h"

#include <QHBoxLayout>
#include <QLocale>
#include <QVBoxLayout>

#include <cmath>

namespace {

QString label(const wchar_t* text) {
    return QString::fromWCharArray(text);
}

}  // namespace

OverviewWindow::OverviewWindow(GarbageBin* bin,
                               std::function<void()> schedulePickupHandler,
                               std::function<QDateTime()> currentTimeProvider,
                               QWidget* parent)
    : QDialog(parent),
      bin_(bin),
      schedulePickupHandler_(std::move(schedulePickupHandler)),
      currentTimeProvider_(std::move(currentTimeProvider)),
      fillLevelWidget_(new FillLevelWidget(this)),
      fillPercentLabel_(new QLabel(this)),
      statusLabel_(new QLabel(this)),
      demandLabel_(new QLabel(this)),
      currentFillLabel_(new QLabel(this)),
      fullTimeLabel_(new QLabel(this)),
      scheduledPickupLabel_(new QLabel(this)),
      pickupButton_(new QPushButton(label(L"Вывоз мусора"), this)) {
    setWindowTitle(label(L"Обзор бака"));
    setMinimumSize(820, 520);
    setStyleSheet(QString::fromUtf8(u8"QDialog { background: #f3f5f4; } QLabel { color: #202428; } QPushButton { background: #d8dee2; border: 1px solid #7e858b; border-radius: 4px; padding: 12px 22px; font-size: 17px; } QPushButton:hover { background: #cbd4da; }"));

    QLabel* fillTitleLabel = new QLabel(label(L"Заполненность бака"), this);
    QFont fillTitleFont = fillTitleLabel->font();
    fillTitleFont.setPointSize(15);
    fillTitleFont.setBold(true);
    fillTitleLabel->setFont(fillTitleFont);
    fillTitleLabel->setAlignment(Qt::AlignCenter);

    QFont percentFont = fillPercentLabel_->font();
    percentFont.setPointSize(18);
    percentFont.setBold(true);
    fillPercentLabel_->setFont(percentFont);
    fillPercentLabel_->setAlignment(Qt::AlignCenter);

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(fillTitleLabel);
    leftLayout->addWidget(fillLevelWidget_, 1);
    leftLayout->addWidget(fillPercentLabel_);

    QFont statusFont = statusLabel_->font();
    statusFont.setPointSize(28);
    statusFont.setBold(true);
    statusLabel_->setFont(statusFont);

    QLabel* analyticsTitleLabel = new QLabel(label(L"Аналитика:"), this);
    QFont analyticsFont = analyticsTitleLabel->font();
    analyticsFont.setPointSize(23);
    analyticsFont.setBold(true);
    analyticsTitleLabel->setFont(analyticsFont);

    QFont analyticsLineFont = demandLabel_->font();
    analyticsLineFont.setPointSize(17);
    demandLabel_->setFont(analyticsLineFont);
    currentFillLabel_->setFont(analyticsLineFont);
    fullTimeLabel_->setFont(analyticsLineFont);
    scheduledPickupLabel_->setFont(analyticsLineFont);
    fullTimeLabel_->setWordWrap(true);

    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(18);
    rightLayout->addWidget(statusLabel_);
    rightLayout->addSpacing(20);
    rightLayout->addWidget(analyticsTitleLabel);
    rightLayout->addWidget(demandLabel_);
    rightLayout->addWidget(currentFillLabel_);
    rightLayout->addWidget(fullTimeLabel_);
    rightLayout->addSpacing(8);
    rightLayout->addWidget(scheduledPickupLabel_);
    rightLayout->addStretch(1);
    rightLayout->addWidget(pickupButton_, 0, Qt::AlignLeft);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(40, 32, 40, 32);
    mainLayout->setSpacing(54);
    mainLayout->addLayout(leftLayout, 4);
    mainLayout->addLayout(rightLayout, 6);

    connect(pickupButton_, &QPushButton::clicked, this, [this]() {
        schedulePickupHandler_();
        updateContents();
    });

    updateContents();
}

void OverviewWindow::updateContents() {
    const int visibleFill = bin_->quantizedFillLevel();
    const QLocale locale(QLocale::Russian, QLocale::Russia);

    fillLevelWidget_->setFillLevel(visibleFill);
    fillPercentLabel_->setText(QString::fromUtf8(u8"%1%").arg(visibleFill));
    statusLabel_->setText(label(L"Статус: Активен"));
    demandLabel_->setText(label(L"Востребованность: ~ %1%/ч").arg(locale.toString(bin_->approximatedFillRatePerHour(), 'f', 1)));
    currentFillLabel_->setText(label(L"Заполненность: %1%").arg(visibleFill));
    fullTimeLabel_->setText(projectedFullTimeText());
    scheduledPickupLabel_->setText(scheduledPickupText());
}

QString OverviewWindow::formatDateTime(const QDateTime& dateTime) const {
    return dateTime.toString(QString::fromUtf8(u8"dd.MM.yyyy HH:mm"));
}

QString OverviewWindow::projectedFullTimeText() const {
    if (bin_->quantizedFillLevel() >= 100) {
        return label(L"Бак заполнится на 100% в %1").arg(formatDateTime(currentTimeProvider_()));
    }

    const double rate = bin_->approximatedFillRatePerHour();
    if (rate <= 0.01) {
        return label(L"Бак заполнится на 100% в: нет данных");
    }

    const int remainingFill = 100 - bin_->quantizedFillLevel();
    const int hoursToFull = std::max(1, static_cast<int>(std::ceil(static_cast<double>(remainingFill) / rate)));
    const QDateTime fullTime = currentTimeProvider_().addSecs(hoursToFull * 3600);
    return label(L"Бак заполнится на 100% в %1").arg(formatDateTime(fullTime));
}

QString OverviewWindow::scheduledPickupText() const {
    if (!bin_->scheduledPickupHour().has_value()) {
        return label(L"Вывоз не назначен");
    }

    const QDateTime pickupTime = QDateTime::fromSecsSinceEpoch(bin_->scheduledPickupHour().value() * 3600);
    return label(L"Вывоз назначен на %1").arg(formatDateTime(pickupTime));
}
