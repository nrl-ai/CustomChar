#ifndef QRREADER_H
#define QRREADER_H

#include <zbar.h>
#include <opencv2/opencv.hpp>

class QRReader {
public:
    QRReader();
    int readQRFromCamera();
};

#endif // QRREADER_H