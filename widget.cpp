#include "widget.h"
#include "./ui_widget.h"
#include "ImageToPDF.h"
#include "PDFToImage.h"
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPdfWriter>
#include <QPainter>
#include <QDir>
#include <QLabel>
#include <QImage>
#include <QPageSize>
#include <QApplication>
#include <QMessageBox>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QTimer>
#include <Qpdfdocument>
#include <algorithm>
#include <QDateTime>
#include <QFileInfo>
#include <QDebug>
#include <QDesktopServices>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    DesktopLocation = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    folderPath = DesktopLocation;
    folderPath_out = DesktopLocation;
    QFileInfo fileInfo(folderPath);
    folderName = fileInfo.fileName();
    ui->picture_pos->setText(DesktopLocation);
    ui->pdf_pos->setText(DesktopLocation);
    setWindowIcon(QIcon(":/logo.ico"));

    ui->direct_y->setChecked(true);
    ui->ratio_a4->setChecked(true);
    ui->ratio_name->setChecked(true);
    ui->ratio_0->setChecked(true);
    connect(ui->picture_btn, &QPushButton::clicked, this, &Widget::openFolder);
    connect(ui->pdf_btn, &QPushButton::pressed, this, &Widget::preconvert);
    connect(ui->convert_btn, &QPushButton::released, this, &Widget::convertToPdf);
    connect(ui->ratio_origin, &QRadioButton::clicked, [=](){
            ui->direct_x->setChecked(true);
            emit changeRadioButton();

    });
    connect(this, &Widget::changeRadioButton, [=](){
        QTimer::singleShot(50, [=](){
            ui->groupBox->setEnabled(false);
        });
    });
    connect(ui->ratio_width, &QRadioButton::clicked, [=](){

        ui->direct_x->setChecked(true);
        emit changeRadioButton();

    });
    connect(this, &Widget::changeRadioButton, [=](){
        QTimer::singleShot(50, [=](){
            ui->groupBox->setEnabled(false);
        });
    });
    connect(ui->ratio_a4, &QRadioButton::clicked, [=](){
        ui->groupBox->setEnabled(true);
    });
    connect(ui->ratio_1609, &QRadioButton::clicked, [=](){
        ui->groupBox->setEnabled(true);
    });
    connect(ui->ratio_1610, &QRadioButton::clicked, [=](){
        ui->groupBox->setEnabled(true);
    });
    connect(ui->ratio_0403, &QRadioButton::clicked, [=](){
        ui->groupBox->setEnabled(true);
    });

    //页面跳转
    QStackedWidget * judgePage = new QStackedWidget;
    ImageToPDF * page2 = new ImageToPDF;
    PDFToImage * page3 = new PDFToImage;
    judgePage->addWidget(this);
    judgePage->addWidget(page2);
    judgePage->addWidget(page3);
    connect(ui->image_page,&QPushButton::clicked,[=](){
        judgePage->setCurrentIndex(1);
    });
    connect(ui->pdf_page,&QPushButton::clicked,[=](){
        judgePage->setCurrentIndex(2);
    });
    connect(page2, &ImageToPDF::emitChangePage1, [=](){
        judgePage->setCurrentIndex(0);
    });
    connect(page2, &ImageToPDF::emitChangePage3, [=](){
        judgePage->setCurrentIndex(2);
    });

    connect(page3, &PDFToImage::emitChangePage2, [=](){
        judgePage->setCurrentIndex(1);
    });
    connect(page3, &PDFToImage::emitChangePage1, [=](){
        judgePage->setCurrentIndex(0);
    });

    judgePage->setCurrentIndex(0);
    judgePage->setFixedSize(600,400);
    judgePage->show();

}

Widget::~Widget()
{
    delete ui;
}

void Widget::openFolder() {
    DesktopLocation = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    folderPath = QFileDialog::getExistingDirectory(this, tr("选择图片所在文件夹"),DesktopLocation);//保存图片所在文件夹的路径
    QFileInfo fileInfo(folderPath);
    folderName = fileInfo.fileName();

    ui->picture_pos->setText(folderPath);
    ui->pdf_pos->setText(folderPath);
}

void Widget::preconvert(){
    folderPath_out = QFileDialog::getExistingDirectory(this, tr("选择pdf文件保存位置"), folderPath);

    ui->pdf_pos->setText(folderPath_out);


}
void Widget::convertToPdf() {

    dpi = 300;

    if (ui->ratio_a4->isChecked() == true){
        scale_height = dpi / mmPerInch * 297.0 ;
        scale_width = dpi / mmPerInch * pdfWidthMM;
    }
    else if (ui->ratio_1609->isChecked() == true){
        scale_height = dpi / mmPerInch * pdfWidthMM *16.0/9.0;
        scale_width = dpi / mmPerInch * pdfWidthMM;
    }
    else if (ui->ratio_1610->isChecked() == true){
        scale_height = dpi / mmPerInch * pdfWidthMM *16.0/10.0;
        scale_width = dpi / mmPerInch * pdfWidthMM;
    }
    else {
        scale_height = dpi / mmPerInch * pdfWidthMM *16.0/12.0;
        scale_width = dpi / mmPerInch * pdfWidthMM;
    }
    if (ui->direct_x->isChecked() == true)
        direct = 1;
    if (ui->direct_y->isChecked() == true) {
        direct = 0;
    }

    QStringList imageFiles = getImageFiles(folderPath);

    mergeImagesIntoPdf(imageFiles);

    if(ui->pdf_pos->text() == ""){
        QMessageBox::information(this, "", "请选择pdf保存路径后重试");
        if (ui->picture_pos->text() == "")
            QMessageBox::information(this, "", "请选择图片所在文件夹后重试");
    }
    else if (ui->picture_pos->text() == ""){
        QMessageBox::information(this, "", "请选择图片所在文件夹后重试");
    }
    else{
        QString pdfPath = folderPath + "/"+folderName+".pdf";
        const QString S = "转换已完成,文件保存至"+pdfPath;
        QMessageBox::information(this,"",S);

        QDesktopServices::openUrl(QUrl::fromLocalFile(pdfPath));
    }
}
QMap<QString, QString> Widget::fileMap;
QStringList Widget::getImageFiles(const QString &folderPath) {
    QDir dir(folderPath);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp";
    QStringList slist = dir.entryList(filters, QDir::Files);

    for (const QString &s : slist){
        QString fullPath = folderPath + "/" + s;
        fileMap.insert(s, fullPath);
    }

    return slist;
}

void Widget::mergeImagesIntoPdf(const QStringList imageFiles) {//将图片画在pdf上
    // 创建PDF写入器
    QStringList sortedFiles = imageFiles;
    if (ui->ratio_birth->isChecked() == true){
        if (ui->ratio_0->isChecked() == true){
            std::sort(sortedFiles.begin(), sortedFiles.end(), &Widget::compareFileCreationDate);

        }
        else{
            std::sort(sortedFiles.begin(), sortedFiles.end(), &Widget::compareFileCreationDateReverse);

        }
    }
    else if (ui->ratio_name->isChecked() == true){
        if (ui->ratio_0->isChecked() == true){
            std::sort(sortedFiles.begin(), sortedFiles.end(), &Widget::compareFileNames);

        }
        else{
            std::sort(sortedFiles.begin(), sortedFiles.end(), &Widget::compareFileNamesReverse);

        }
    }
    else if (ui->ratio_change->isChecked() == true){
        if (ui->ratio_0->isChecked() == true){
            std::sort(sortedFiles.begin(), sortedFiles.end(), &Widget::compareFileModificationDate);

        }
        else{
            std::sort(sortedFiles.begin(), sortedFiles.end(), &Widget::compareFileModificationDateReverse);

        }
    }

    int count = 0;
    pdfName = ui->pdf_pos->text() +  "/" +folderName+".pdf";
    QPdfWriter pdfWriter(pdfName);

    //设置分辨率
    pdfWriter.setResolution(dpi);

    //设置页面方向为横向
    if (direct == 1)
        pdfWriter.setPageOrientation(QPageLayout::Landscape);


    //设置第一页的页面大小
    QImage image(ui->picture_pos->text() + "/" + sortedFiles[0]);
    if (ui->ratio_origin->isChecked() == true){
        QPageSize customPageSize(QSize(image.height()*dpi/300.0,image.width()*dpi/300.0), QPageSize::Unit::Point);
        pdfWriter.setPageSize(customPageSize);
        QSizeF pageSizePx = pdfWriter.pageLayout().fullRectPixels(pdfWriter.resolution()).size();
        scaledSize = QSizeF(image.size()).scaled(pageSizePx, Qt::KeepAspectRatio);
        imagePosition = QPointF(0,0);


    }
    else if (ui->ratio_width->isChecked() == true){

        // 计算目标页面尺寸（保持等宽，等比缩放）
        QPageSize pageSize(QSize(scale_width*image.height()/image.width(), scale_width), QPageSize::Point);
        QPageSize pageSize2(QSize(scale_width,scale_width*image.height()/image.width()), QPageSize::Point);
        pdfWriter.setPageSize(pageSize);

        QSizeF pageSizePx = pdfWriter.pageLayout().fullRectPixels(pdfWriter.resolution()).size();
        scaledSize = QSizeF(image.size()).scaled(pageSizePx, Qt::KeepAspectRatio);
        imagePosition = QPointF(0,0);
    }

    //绘制图片内容
    QPainter painter(&pdfWriter);


    for (const QString &imageFile : sortedFiles) {
        QImage image(ui->picture_pos->text() + "/" + imageFile);

        if (!image.isNull()) {
            if (ui->ratio_origin->isChecked() == true){
                QPageSize customPageSize(QSize(image.height()*dpi/300.0,image.width()*dpi/300.0), QPageSize::Unit::Point);
                pdfWriter.setPageSize(customPageSize);
                 QSizeF pageSizePx = pdfWriter.pageLayout().fullRectPixels(pdfWriter.resolution()).size();
                 scaledSize = QSizeF(image.size()).scaled(pageSizePx, Qt::KeepAspectRatio);
                imagePosition = QPointF(-40.0*dpi/300.0, -40.0*dpi/300.0);

            }
            else if (ui->ratio_width->isChecked() == true){

                // 计算目标页面尺寸（保持等宽，等比缩放）


                QPageSize pageSize(QSize(scale_width*image.height()/image.width(), scale_width), QPageSize::Point);
                pdfWriter.setPageSize(pageSize);
                QSizeF pageSizePx = pdfWriter.pageLayout().fullRectPixels(pdfWriter.resolution()).size();
                scaledSize = pageSizePx;
                imagePosition = QPointF(-40.0*dpi/300.0, -40.0*dpi/300.0);
                qDebug() <<"PageSize == "<<pageSizePx;
            }
            // 计算图像在PDF页面中的适合大小
            else{
                QPageSize pageSize(QSize(scale_height, scale_width), QPageSize::Point);
                QSizeF imageSize = image.size();
                QSizeF pageSizePx = pdfWriter.pageLayout().fullRectPixels(pdfWriter.resolution()).size();
                scaledSize = imageSize.scaled(pageSizePx, Qt::KeepAspectRatio);
                imagePosition= QPointF((pageSizePx.width() - scaledSize.width()) / 2.0 - 40.0*dpi/300.0  ,(pageSizePx.height() - scaledSize.height()) / 2.0 - 40.0*dpi/300.0);
            }

            if (count == 0){

                painter.drawImage(QRectF(imagePosition, scaledSize), image);
            }
            else{

                pdfWriter.newPage();
                painter.drawImage(QRectF(imagePosition, scaledSize), image);
                qDebug() <<"scaledSize == "<<scaledSize;
                qDebug() << imagePosition;
            }

        }
        count++;
    }
}


// 自定义比较函数，用于按照文件名的数字顺序或名称顺序排序
// 将 compareFileNames 函数定义为静态成员函数
bool Widget::compareFileNames(const QString& file1, const QString& file2) {
    QString name1 = QFileInfo(file1).baseName();
    QString name2 = QFileInfo(file2).baseName();

    // 提取文件名中的数字部分

    QString numberPart1;
    QString numberPart2;

    for (const QChar& ch : name1) {
        if (ch.isDigit()) {
            numberPart1.append(ch);
        }
    }
    for (const QChar& ch : name2) {
        if (ch.isDigit()) {
            numberPart2.append(ch);
        }
    }
    int x = std::max(numberPart1.size(),numberPart2.size());
    // 如果都包含数字，则按数字顺序排序
    if (!numberPart1.isEmpty() && !numberPart2.isEmpty()) {
        numberPart1 = numberPart1.rightJustified(x,'0');
        numberPart2 = numberPart2.rightJustified(x,'0');
        return numberPart1 < numberPart2;
    }

    // 否则按名称顺序排序
    return name1 < name2;
}

bool Widget::compareFileNamesReverse(const QString& file1, const QString& file2) {
    QString name1 = QFileInfo(file1).baseName();
    QString name2 = QFileInfo(file2).baseName();

    // 提取文件名中的数字部分

    QString numberPart1;
    QString numberPart2;

    for (const QChar& ch : name1) {
        if (ch.isDigit()) {
            numberPart1.append(ch);
        }
    }
    for (const QChar& ch : name2) {
        if (ch.isDigit()) {
            numberPart2.append(ch);
        }
    }
    int x = std::max(numberPart1.size(),numberPart2.size());
    // 如果都包含数字，则按数字顺序排序
    if (!numberPart1.isEmpty() && !numberPart2.isEmpty()) {
        numberPart1 = numberPart1.rightJustified(x,'0');
        numberPart2 = numberPart2.rightJustified(x,'0');
        return numberPart1 > numberPart2;
    }

    // 否则按名称顺序排序
    return name1 > name2;
}

// void Widget::test(){
//     QFileInfo file(sss[1]);
//     QMessageBox::information(this,"",file.baseName());
//     if(file.birthTime().isNull())
//         QMessageBox::information(this,"","isNull");
//     else if(file.birthTime().isValid())
//         QMessageBox::information(this,"","isvalid");
// }


bool Widget::compareFileCreationDate(const QString& file1, const QString& file2) {
    QFileInfo fileInfo1(fileMap.value(file1));
    QFileInfo fileInfo2(fileMap.value(file2));

    // 获取文件1的创建日期
    QDateTime creationDateTime1 = fileInfo1.birthTime();  // 将日期和时间一起转换为字符串

    // 获取文件2的创建日期
    QDateTime creationDateTime2 = fileInfo2.birthTime();

    // 比较创建日期，返回较早的日期排在前面
    return creationDateTime1 < creationDateTime2;
}

bool Widget::compareFileCreationDateReverse(const QString& file1, const QString& file2) {
    QFileInfo fileInfo1(fileMap.value(file1));
    QFileInfo fileInfo2(fileMap.value(file2));

    // 获取文件1的创建日期
    QDateTime creationDateTime1 = fileInfo1.birthTime();  // 将日期和时间一起转换为字符串

    // 获取文件2的创建日期
    QDateTime creationDateTime2 = fileInfo2.birthTime();

    // 比较创建日期，返回较早的日期排在前面
    return creationDateTime1 > creationDateTime2;
}



bool Widget::compareFileModificationDate(const QString& file1, const QString& file2) {
    QFileInfo fileInfo1(fileMap.value(file1));
    QFileInfo fileInfo2(fileMap.value(file2));
    // 获取文件1的修改日期
    QDateTime modificationDateTime1 = fileInfo1.lastModified();

    // 获取文件2的修改日期
    QDateTime modificationDateTime2 = fileInfo2.lastModified();

    // 比较修改日期，返回较早的日期排在前面
    return modificationDateTime1 < modificationDateTime2;
}

bool Widget::compareFileModificationDateReverse(const QString& file1, const QString& file2) {
    QFileInfo fileInfo1(fileMap.value(file1));
    QFileInfo fileInfo2(fileMap.value(file2));

    // 获取文件1的修改日期
    QDateTime modificationDateTime1 = fileInfo1.lastModified();

    // 获取文件2的修改日期
    QDateTime modificationDateTime2 = fileInfo2.lastModified();

    // 比较修改日期，返回较早的日期排在前面
    return modificationDateTime1 < modificationDateTime2;
}

