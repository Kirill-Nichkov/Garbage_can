#pragma once

#include <QColor>
#include <QWidget>

QColor fillColorForLevel(int quantizedFillLevel);

class FillLevelWidget : public QWidget {
public:
    explicit FillLevelWidget(QWidget* parent = nullptr);

    void setFillLevel(int quantizedFillLevel);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int quantizedFillLevel_;
};
