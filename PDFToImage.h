#ifndef PDFTOIMAGE_H
#define PDFTOIMAGE_H

#include <QWidget>

namespace Ui {
class PDFToImage;
}

class PDFToImage : public QWidget
{
    Q_OBJECT

public:
    explicit PDFToImage(QWidget *parent = nullptr);
    ~PDFToImage();
    QString packagePath;
    QString pdfPath;
    void convertToImages();
    QString DesktopLocation;
    int scale_height;
    int scale_width;
private:
    Ui::PDFToImage *ui;
public slots:
    void change_page1();
    void change_page2();
signals:
    void emitChangePage1();
    void emitChangePage2();
};

#endif // PDFTOIMAGE_H
