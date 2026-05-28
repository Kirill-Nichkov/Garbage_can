#include "map_widget.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QMouseEvent>
#include <QPainter>
#include <QToolTip>

#include <algorithm>

#include "fill_level_widget.h"

MapWidget::MapWidget(std::vector<GarbageBin>* bins, QWidget* parent)
    : QWidget(parent),
      bins_(bins),
      background_(backgroundPath()),
      hoveredBinIndex_(-1) {
    setMouseTracking(true);
    setMinimumSize(900, 560);
}

void MapWidget::setBinClickedHandler(std::function<void(std::size_t)> handler) {
    binClickedHandler_ = std::move(handler);
}

void MapWidget::refreshBins() {
    update();
}

void MapWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (!background_.isNull()) {
        const QPixmap scaled = background_.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        const QPoint topLeft((width() - scaled.width()) / 2, (height() - scaled.height()) / 2);
        painter.drawPixmap(topLeft, scaled);
    } else {
        painter.fillRect(rect(), QColor(0, 0, 0));
    }

    painter.fillRect(rect(), QColor(0, 0, 0, 58));

    for (std::size_t index = 0; index < bins_->size(); ++index) {
        drawBin(painter, bins_->at(index), binRectFor(index), static_cast<int>(index) == hoveredBinIndex_);
    }
}

void MapWidget::mouseMoveEvent(QMouseEvent* event) {
    const int index = binIndexAt(event->pos());

    if (index != hoveredBinIndex_) {
        hoveredBinIndex_ = index;
        update();
    }

    if (hoveredBinIndex_ >= 0) {
        setCursor(Qt::PointingHandCursor);
        QToolTip::showText(event->globalPos(), QString::fromStdWString(bins_->at(static_cast<std::size_t>(hoveredBinIndex_)).streetName()), this);
    } else {
        unsetCursor();
        QToolTip::hideText();
    }
}

void MapWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        return;
    }

    const int index = binIndexAt(event->pos());
    if (index >= 0 && binClickedHandler_) {
        binClickedHandler_(static_cast<std::size_t>(index));
    }
}

void MapWidget::leaveEvent(QEvent* event) {
    Q_UNUSED(event);
    hoveredBinIndex_ = -1;
    unsetCursor();
    QToolTip::hideText();
    update();
}

QRect MapWidget::binRectFor(std::size_t index) const {
    const MapCoordinate coordinate = bins_->at(index).mapCoordinate();
    const int binWidth = 48;
    const int binHeight = 74;
    const int centerX = static_cast<int>(coordinate.x * width());
    const int centerY = static_cast<int>(coordinate.y * height());
    return QRect(centerX - binWidth / 2, centerY - binHeight / 2, binWidth, binHeight);
}

int MapWidget::binIndexAt(const QPoint& position) const {
    for (int index = static_cast<int>(bins_->size()) - 1; index >= 0; --index) {
        if (binRectFor(static_cast<std::size_t>(index)).adjusted(-6, -6, 6, 6).contains(position)) {
            return index;
        }
    }

    return -1;
}

QString MapWidget::backgroundPath() const {
    const QString fileName = QString::fromUtf8(u8"background.png");
    const QString currentPath = QDir::current().filePath(fileName);
    if (QFileInfo::exists(currentPath)) {
        return currentPath;
    }

    const QDir applicationDir(QCoreApplication::applicationDirPath());
    const QString applicationPath = applicationDir.filePath(fileName);
    if (QFileInfo::exists(applicationPath)) {
        return applicationPath;
    }

    const QDir parentDir(applicationDir.filePath(QString::fromUtf8(u8"..")));
    return parentDir.filePath(fileName);
}

void MapWidget::drawBin(QPainter& painter, const GarbageBin& bin, const QRect& rect, bool hovered) {
    const int fillLevel = bin.quantizedFillLevel();
    const QRect inner = rect.adjusted(7, 8, -7, -7);
    const int fillHeight = static_cast<int>(inner.height() * fillLevel / 100.0);
    const QRect fillRect(inner.left(), inner.bottom() - fillHeight + 1, inner.width(), fillHeight);

    painter.save();

    if (hovered) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 140));
        painter.drawRoundedRect(rect.adjusted(-7, -7, 7, 7), 6, 6);
    }

    painter.setPen(QPen(QColor(0, 0, 0), 2));
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(rect, 5, 5);

    if (fillHeight > 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(fillColorForLevel(fillLevel));
        painter.drawRect(fillRect);
    }

    painter.setPen(QPen(QColor(0, 0, 0), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(inner, 3, 3);

    QFont idFont = painter.font();
    idFont.setPointSize(16);
    idFont.setBold(true);
    painter.setFont(idFont);
    painter.setPen(QColor(0, 0, 0));
    painter.drawText(rect.adjusted(0, 4, 0, -4), Qt::AlignCenter, QString::number(bin.id()));

    painter.restore();
}
