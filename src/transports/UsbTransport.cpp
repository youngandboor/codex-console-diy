#include "transports/UsbTransport.h"

#include <USB.h>

#include <cstring>

namespace codex {
namespace {

const uint8_t kUsbReportDescriptor[] = {
    0x06, 0x00, 0xFF, 0x09, 0x01, 0xA1, 0x01, 0x85, 0x06, 0x15,
    0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95, 0x3F, 0x09, 0x01,
    0x81, 0x02, 0x95, 0x3F, 0x09, 0x02, 0x91, 0x02, 0xC0,
};

}  // namespace

UsbTransport::UsbTransport() : hid_() {
  USBHID::addDevice(this, sizeof(kUsbReportDescriptor));
}

void UsbTransport::begin(QueueHandle_t incomingQueue) {
  incomingQueue_ = incomingQueue;
  USB.VID(0x303A);
  USB.PID(0x8360);
  USB.firmwareVersion(0x0101);
  USB.productName("Codex Micro");
  USB.manufacturerName("Work Louder");
  USB.serialNumber("__MAC__");
  USB.usbAttributes(0x80);  // Bus-powered while attached; the charger supplies SYS from VBUS.
  USB.usbPower(500);
  hid_.begin();
  USB.begin();
}

bool UsbTransport::connected() const {
  return static_cast<bool>(USB) && const_cast<USBHID&>(hid_).ready();
}

bool UsbTransport::sendReport(const ReportBody& report) {
  if (!connected()) {
    return false;
  }
  return hid_.SendReport(kReportId, report.data(), report.size());
}

uint16_t UsbTransport::_onGetDescriptor(uint8_t* buffer) {
  std::memcpy(buffer, kUsbReportDescriptor, sizeof(kUsbReportDescriptor));
  return sizeof(kUsbReportDescriptor);
}

void UsbTransport::_onOutput(uint8_t reportId, const uint8_t* buffer, uint16_t length) {
  if (reportId != kReportId || incomingQueue_ == nullptr || buffer == nullptr ||
      length == 0 || length > kReportBodySize) {
    return;
  }
  IncomingReport report;
  report.source = TransportKind::kUsb;
  report.length = static_cast<uint8_t>(length);
  std::memcpy(report.bytes, buffer, length);
  xQueueSend(incomingQueue_, &report, 0);
}

}  // namespace codex
