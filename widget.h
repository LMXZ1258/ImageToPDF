#ifndef WIDGET_H
#define WIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    int page_num = 0;
    QString DesktopLocation;
    QString pdfName;

    static bool compareFileNames(const QString& file1, const QString& file2);
    static bool compareFileCreationDate(const QString& file1, const QString& file2);
    static bool compareFileModificationDate(const QString& file1, const QString& file2);

    static bool compareFileNamesReverse(const QString& file1, const QString& file2);
    static bool compareFileCreationDateReverse(const QString& file1, const QString& file2);
    static bool compareFileModificationDateReverse(const QString& file1, const QString& file2);

    int sort_image = 1;
    int sort_image_2 = 0;
    static QMap<QString, QString> fileMap;
    // void test();
signals:
    void changeRadioButton();
private slots:
    void openFolder(); // 打开文件夹的槽函数
    void convertToPdf(); // 转换为PDF的槽函数
    void preconvert(); //转换之前的操作
private:
    QString folderPath; // 文件夹路径
    QString folderPath_out;//文件输出路径
    QString folderName;
    QStringList getImageFiles(const QString &folderPath);
    void mergeImagesIntoPdf(const QStringList imageFiles); // 将图片合并到PDF中
    int direct = 0;
    int scale = 0;
    int dpi = 300;
    double mmPerInch = 25.4;
    double pdfWidthMM = 210.0;
    double scale_height = dpi / mmPerInch * 297.0;
    double scale_width = dpi / mmPerInch * pdfWidthMM;
    QSizeF scaledSize;
    QPointF imagePosition;
    // signals:

     Ui::Widget *ui;

};
#endif // WIDGET_H
