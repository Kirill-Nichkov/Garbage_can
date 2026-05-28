#include "fill_level_widget.h"

#include <QPainter>

#include <algorithm>

QColor fillColorForLevel(int quantizedFillLevel) {
    if (quantizedFillLevel >= 100) {
        return QColor(196, 48, 43);
    }

    if (quantizedFillLevel >= 60) {
        return QColor(225, 176, 45);
    }

    return QColor(55, 157, 82);
}

FillLevelWidget::FillLevelWidget(QWidget* parent)
    : QWidget(parent), quantizedFillLevel_(0) {
    setMinimumSize(170, 300);
}

void FillLevelWidget::setFillLevel(int quantizedFillLevel) {
    quantizedFillLevel_ = std::clamp(quantizedFillLevel, 0, 100);
    update();
}

QSize FillLevelWidget::sizeHint() const {
    return QSize(210, 360);
}

void FillLevelWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRect outer = rect().adjusted(18, 18, -18, -18);
    const QRect inner = outer.adjusted(10, 10, -10, -10);
    const int fillHeight = static_cast<int>(inner.height() * quantizedFillLevel_ / 100.0);
    const QRect fillRect(inner.left(),
                         inner.bottom() - fillHeight + 1,
                         inner.width(),
                         fillHeight);

    painter.setPen(QPen(QColor(38, 43, 48), 3));
    painter.setBrush(QColor(236, 239, 241));
    painter.drawRoundedRect(outer, 5, 5);

    if (fillHeight > 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(fillColorForLevel(quantizedFillLevel_));
        painter.drawRect(fillRect);
    }

    painter.setPen(QPen(QColor(38, 43, 48), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(inner, 3, 3);

    QFont percentFont = painter.font();
    percentFont.setPointSize(22);
    percentFont.setBold(true);
    painter.setFont(percentFont);
    painter.setPen(QColor(28, 32, 36));
    painter.drawText(outer, Qt::AlignCenter, QString::number(quantizedFillLevel_) + QString::fromUtf8(u8"%"));
}
