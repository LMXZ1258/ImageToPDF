#ifndef PAGESELECT_H
#define PAGESELECT_H

#include <QWidget>

class pageSelect : public QWidget
{
    Q_OBJECT
public:
    explicit pageSelect(QWidget *parent = nullptr);
    int page_num = 0;
signals:
};

#endif // PAGESELECT_H
