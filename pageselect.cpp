#include "pageselect.h"
#include "imginput.h"
#include "widget.h"
#include <QStackedWidget>
#include <QWidget>
#include <QPushButton>
#include <QLabel>


pageSelect::pageSelect(QWidget *parent)
    : QWidget{parent}
{
    Widget * page1 = new Widget;
    imginput * page2 = new imginput;
    QStackedWidget * judge = new QStackedWidget();
    judge->addWidget(page1);
    judge->addWidget(page2);
    connect(page1->)
    judge->setCurrentIndex(page_num);
    judge->show();
}
