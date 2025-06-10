#ifndef IMAGETOPDF_H
#define IMAGETOPDF_H

#include <QWidget>
#include <QList>
#include <QPair>
#include <QMap>

namespace Ui {
class ImageToPDF;
}

class ImageToPDF : public QWidget
{
    Q_OBJECT

public:
    explicit ImageToPDF(QWidget *parent = nullptr);
    ~ImageToPDF();
    QList<QPair<QString,QString>> imgAdrPairList;
    QMap<QString,QString> imgAdrMap;
    QString currentItemName;
    QString currentItemAddress;//当前图片地址
    QString imgSavePath;
    QString DesktopLocation;
protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void mousePressEvent(QMouseEvent *event);
public slots:
    void change_page1();
    void change_page3();
signals:
    void emitChangePage1();
    void emitChangePage3();
    void deleteListItem();
private:
    Ui::ImageToPDF *ui;
};

#endif // IMAGETOPDF_H
