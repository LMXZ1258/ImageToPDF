#include "PDFToImage.h"
#include "ui_PDFToImage.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QImage>
#include <QPdfDocument>
#include <QImage>
#include <QPainter>

PDFToImage::PDFToImage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PDFToImage)
{
    ui->setupUi(this);
    DesktopLocation = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    connect(ui->normal_page, &QPushButton::clicked, this, &PDFToImage::change_page1);
    connect(ui->image_page, &QPushButton::clicked, this, &PDFToImage::change_page2);
    packagePath = DesktopLocation ;
    ui->pdf_pos->setText(DesktopLocation);
    ui->pack_pos->setText(DesktopLocation);
    connect(ui->pdf_sel, &QPushButton::clicked, [=]() {
        pdfPath = QFileDialog::getOpenFileName(this, tr("选择pdf文件"), DesktopLocation, "PDF Files(*.pdf)");
        ui->pdf_pos->setText(pdfPath);
    });
    connect(ui->pack_sel, &QPushButton::clicked, [=]() {
        packagePath = QFileDialog::getExistingDirectory(this, tr("选择输出文件夹"), DesktopLocation);
        ui->pack_pos->setText(packagePath);
    });
    connect(ui->convertBtn, &QPushButton::clicked, this, &PDFToImage::convertToImages);
}

PDFToImage::~PDFToImage()
{
    delete ui;
}

void PDFToImage::change_page1()
{
    emit emitChangePage1();
}

void PDFToImage::change_page2()
{
    emit emitChangePage2();
}



void PDFToImage::convertToImages()
{
    QPdfDocument pdf;
    pdf.load(pdfPath);


    int numPages = pdf.pageCount();

    for (int i = 0; i < numPages; ++i)
    {
        QImage image;
        image = pdf.render(i,pdf.pagePointSize(i).toSize());
        QString imagePath = QString("%1/%2.jpg").arg(packagePath).arg(i + 1);
        image.save(imagePath);
    }
    const QString S = "转换已完成,文件保存至"+packagePath;
    QMessageBox::information(this,"",S);
}
