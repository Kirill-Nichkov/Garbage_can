#pragma once

#include <QPixmap>
#include <QWidget>

#include <cstddef>
#include <functional>
#include <vector>

#include "garbage_bin.h"

class MapWidget : public QWidget {
public:
    explicit MapWidget(std::vector<GarbageBin>* bins, QWidget* parent = nullptr);

    void setBinClickedHandler(std::function<void(std::size_t)> handler);
    void refreshBins();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QRect binRectFor(std::size_t index) const;
    int binIndexAt(const QPoint& position) const;
    QString backgroundPath() const;
    void drawBin(QPainter& painter, const GarbageBin& bin, const QRect& rect, bool hovered);

    std::vector<GarbageBin>* bins_;
    QPixmap background_;
    int hoveredBinIndex_;
    std::function<void(std::size_t)> binClickedHandler_;
};
