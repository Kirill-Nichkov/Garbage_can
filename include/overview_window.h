#pragma once

#include <QDateTime>
#include <QDialog>
#include <QLabel>
#include <QPushButton>

#include <functional>

#include "fill_level_widget.h"
#include "garbage_bin.h"

class OverviewWindow : public QDialog {
public:
    OverviewWindow(GarbageBin* bin,
                   std::function<void()> schedulePickupHandler,
                   std::function<QDateTime()> currentTimeProvider,
                   QWidget* parent = nullptr);

    void updateContents();

private:
    QString formatDateTime(const QDateTime& dateTime) const;
    QString projectedFullTimeText() const;
    QString scheduledPickupText() const;

    GarbageBin* bin_;
    std::function<void()> schedulePickupHandler_;
    std::function<QDateTime()> currentTimeProvider_;
    FillLevelWidget* fillLevelWidget_;
    QLabel* fillPercentLabel_;
    QLabel* statusLabel_;
    QLabel* demandLabel_;
    QLabel* currentFillLabel_;
    QLabel* fullTimeLabel_;
    QLabel* scheduledPickupLabel_;
    QPushButton* pickupButton_;
};
