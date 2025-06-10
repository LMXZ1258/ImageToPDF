#include "ImageToPDF.h"
#include "ui_ImageToPDF.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QListWidget>
#include <QDebug>
#include <QFileDialog>
#include <QDrag>
#include <QUrl>
#include <QPixmap>
#include <QBuffer>
#include <QStandardPaths>
ImageToPDF::ImageToPDF(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImageToPDF)
{

    setAcceptDrops(true);
    imgSavePath = DesktopLocation;
    DesktopLocation = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    ui->setupUi(this);
    ui->pos->setText(DesktopLocation);
    ui->listWidget->setDragDropMode(QAbstractItemView::InternalMove);
    ui->listWidget->setDragEnabled(true);
    ui->listWidget->setDefaultDropAction(Qt::MoveAction);

    connect(ui->normal_page, &QPushButton::clicked, this, &ImageToPDF::change_page1);
    connect(ui->pdf_page, &QPushButton::clicked, this, &ImageToPDF::change_page3);
    connect(ui->listWidget, &QListWidget::itemClicked, [=](QListWidgetItem *item) {
        QString imagePath = imgAdrMap.value(item->text());
        QImage image(imagePath);
        if (!image.isNull()) {
            // 计算缩放比例
            double scale = static_cast<double>(ui->showpic->width()) / image.width();
            // 缩放图片
            QImage scaledImage = image.scaled(ui->showpic->width(), image.height() * scale, Qt::KeepAspectRatio);
            // 显示缩放后的图片
            ui->showpic->setPixmap(QPixmap::fromImage(scaledImage));
        } else {
            qDebug() << "Failed to load image from" << imagePath;
        }
    });

    connect(ui->choosepos, &QPushButton::clicked, [=]() {

        imgSavePath = QFileDialog::getExistingDirectory(this, tr("选择图片所存文件夹"), DesktopLocation);
        ui->pos->setText(imgSavePath);
    });

    connect(ui->savebtn, &QPushButton::clicked, [=]() {
        for (int i = 0; i < ui->listWidget->count(); i++) {
            QString imgName = QString::number(i + 1) + ".png"; // 按列表顺序编号图片名称
            QFile imgFile = imgAdrMap.value(ui->listWidget->item(i)->text());
            if (imgFile.copy(imgSavePath + "/" + imgName)) {
                qDebug() << "SUCCESS";
            } else {
                qDebug() << "FAIL";
            }
        }
    });
    connect(ui->listWidget,&QListWidget::doubleClicked,[=](){
        ui->listWidget->takeItem(ui->listWidget->currentRow());
    });

}

ImageToPDF::~ImageToPDF()
{
    delete ui;
}

//实现页面跳转
void ImageToPDF::change_page1()
{
    emit emitChangePage1();
}
void ImageToPDF::change_page3()
{
    emit emitChangePage3();
}

void ImageToPDF::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls() || event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void ImageToPDF::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        QStringList imageExtensions = {"JPG", "PNG", "JPEG"};

        for (const QUrl &url : event->mimeData()->urls()) {
            QString filename = url.toLocalFile();
            QFileInfo fileInfo(filename);
            QString ext = fileInfo.suffix().toUpper();
            if (imageExtensions.contains(ext)) {
                ui->listWidget->addItem(fileInfo.fileName());
                imgAdrPairList.append(QPair<QString, QString>(fileInfo.fileName(), filename));
                imgAdrMap.insert(fileInfo.fileName(), filename);
            }
        }

        event->acceptProposedAction();
    } else if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        QByteArray itemData = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        while (!dataStream.atEnd()) {
            int row, col;
            QMap<int, QVariant> roleDataMap;
            dataStream >> row >> col >> roleDataMap;

            QListWidgetItem *item = ui->listWidget->takeItem(row); // 从原始位置中移除项目
            ui->listWidget->insertItem(event->position().toPoint().y() / ui->listWidget->sizeHintForRow(0), item); // 在新位置插入项目
        }

        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void ImageToPDF::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QListWidgetItem *selectedItem = ui->listWidget->itemAt(event->pos());
        if (selectedItem) {
            QDrag *drag = new QDrag(this);

            // Create a MIME data object
            QMimeData *mimeData = new QMimeData;

            // Set the MIME data with the selected item's text
            mimeData->setText(selectedItem->text());

            // Set the MIME data with the selected item's image
            QString imagePath = imgAdrMap.value(selectedItem->text());
            QImage image(imagePath);
            QByteArray byteArray;
            QBuffer buffer(&byteArray);
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, "JPG"); // Save the image to a byte array in JPG format
            mimeData->setData("image/jpg", byteArray);

            // Set the MIME data on the drag object
            drag->setMimeData(mimeData);

            // Start the drag operation
            drag->exec(Qt::CopyAction | Qt::MoveAction);
        }
    }
}
