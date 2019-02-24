#ifndef DEVICE_DETECT_H
#define DEVICE_DETECT_H

#include <vector>

#include <QtCore/QString>

struct DeviceInfo {
    QString id;
    QString name;
};

void InitCOM();
bool GetCameras(std::vector<DeviceInfo> &list);
bool GetMicphones(std::vector<DeviceInfo> &list);

#endif //DEVICE_DETECT_H