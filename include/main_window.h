#pragma once

#include <QDateTime>
#include <QLabel>
#include <QMainWindow>

#include <cstddef>
#include <vector>

#include "garbage_bin.h"
#include "map_widget.h"
#include "overview_window.h"

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QWidget* createTopPanel();
    void createBins();
    void refreshUi();
    void advanceOneHour();
    void openOverview(std::size_t binIndex);
    void schedulePickup(std::size_t binIndex);
    QDateTime nextPickupTime() const;
    QString formatDateTime(const QDateTime& dateTime) const;
    long long hourIndexFor(const QDateTime& dateTime) const;

    QLabel* timeLabel_;
    MapWidget* mapWidget_;
    OverviewWindow* overviewWindow_;
    std::vector<GarbageBin> bins_;
    QDateTime currentTime_;
};
