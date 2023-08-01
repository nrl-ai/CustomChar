#include "customchar/perception/QRReader.h"
#include <iostream>

QRReader::QRReader() {}

int QRReader::readQRFromCamera() {
    cv::VideoCapture cap(0);

    if(!cap.isOpened()) {
        std::cout << "Error: Unable to open the camera" << std::endl;
        return -1;
    }

    zbar::ImageScanner scanner;
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);

    cv::Mat frame;
    cv::namedWindow("QR code reader", 1);

    while(1) {
        cap >> frame;

        if(frame.empty()) {
            std::cout << "Error: Unable to capture an image from the camera" << std::endl;
            return -1;
        }

        zbar::Image image(frame.cols, frame.rows, "Y800", frame.data, frame.cols*frame.rows);
        scanner.scan(image);

        for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
            std::cout << "Decoded: " << symbol->get_data() << std::endl;
        }

        cv::imshow("QR code reader", frame);
        if(cv::waitKey(30) >= 0) break;
    }

    return 0;
}