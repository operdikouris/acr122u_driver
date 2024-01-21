#include <stdio.h>
#include <usb.h>
#include <stdbool.h>
#include "inttypes.h"
#include "../inc/configure.h"

uint16_t acr122u_vendor_id=0x072F;
uint16_t acr122u_product_id= 0x2200;

usb_dev_handle *handle;
struct usb_device *udev;

uint32_t uiEndPointIn;
uint32_t uiEndPointOut;
uint32_t uiMaxPacketSize;

static int
acr122_usb_bulk_read(unsigned char* received_data, const size_t received_size, const int timeout)
{
  int res = usb_bulk_read(handle, uiEndPointIn, received_data, received_size, timeout);
 #ifdef USB_TRACE
    printf("< ");
    for (int i=0; i<res; i++)
      printf("%02x ", received_data[i]);
    printf("\n");
  #endif
  return res;
}

static int
acr122_usb_bulk_write(unsigned char* send_data, const size_t send_size, const int timeout)
{
  #ifdef USB_TRACE
    printf("> ");
    for (int i=0; i<send_size; i++)
      printf("%02x ", send_data[i]);
    printf("\n");
  #endif
  int res = usb_bulk_write(handle, uiEndPointOut, send_data, send_size, timeout);
  // printf("usb_bulk_write returned (%d)\n", res);
  if (res > 0) {
    // HACK This little hack is a well know problem of USB, see http://www.libusb.org/ticket/6 for more details
    if ((res % uiMaxPacketSize) == 0) {
      usb_bulk_write(handle, uiEndPointOut, "\0", 0, timeout);
    }
 
  }
   return res;
}

// Find transfer endpoints for bulk transfers
static void
acr122_usb_get_end_points(struct usb_device *dev)
{
  uint32_t uiIndex;
  uint32_t uiEndPoint;
  struct usb_interface_descriptor *puid = dev->config->interface->altsetting;

  // 3 Endpoints maximum: Interrupt In, Bulk In, Bulk Out
  for (uiIndex = 0; uiIndex < puid->bNumEndpoints; uiIndex++) {
    // Only accept bulk transfer endpoints (ignore interrupt endpoints)
    if (puid->endpoint[uiIndex].bmAttributes != USB_ENDPOINT_TYPE_BULK)
      continue;

    // Copy the endpoint to a local var, makes it more readable code
    uiEndPoint = puid->endpoint[uiIndex].bEndpointAddress;

    // Test if we dealing with a bulk IN endpoint
    if ((uiEndPoint & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_IN) {
      uiEndPointIn = uiEndPoint;
      uiMaxPacketSize = puid->endpoint[uiIndex].wMaxPacketSize;
    }
    // Test if we dealing with a bulk OUT endpoint
    if ((uiEndPoint & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_OUT) {
      uiEndPointOut = uiEndPoint;
      uiMaxPacketSize = puid->endpoint[uiIndex].wMaxPacketSize;
    }
  }
}

int usb_prepare(void)
{
  static bool usb_initialized = false;
  if (!usb_initialized) {
    usb_init();
    usb_initialized = true;
  }

  int res;
  // usb_find_busses will find all of the busses on the system. Returns the
  // number of changes since previous call to this function (total of new
  // busses and busses removed).
  if ((res = usb_find_busses()) < 0) {
    //log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "Unable to find USB busses (%s)", _usb_strerror(res));
    return -1;
  }
  // usb_find_devices will find all of the devices on each bus. This should be
  // called after usb_find_busses. Returns the number of changes since the
  // previous call to this function (total of new device and devices removed).
  if ((res = usb_find_devices()) < 0) {
   // log_put(LOG_GROUP, LOG_CATEGORY, NFC_LOG_PRIORITY_ERROR, "Unable to find USB devices (%s)", _usb_strerror(res));
    return -1;
  }
  return 0;
}



static int
acr122_usb_scan()
{
  size_t device_found = 0;
  uint32_t uiBusIndex = 0;
  struct usb_bus *bus;
  for (bus = usb_get_busses(); bus; bus = bus->next) 
  {
    struct usb_device *dev;

    for (dev = bus->devices; dev; dev = dev->next, uiBusIndex++) 
    {
      if ((acr122u_vendor_id == dev->descriptor.idVendor) &&
          (acr122u_product_id == dev->descriptor.idProduct)) 
      {
        // Make sure there are 2 endpoints available
        // with libusb-win32 we got some null pointers so be robust before looking at endpoints:
        if (dev->config == NULL || dev->config->interface == NULL || dev->config->interface->altsetting == NULL) 
        {
          // Nope, we maybe want the next one, let's try to find another
          continue;
        }
        if (dev->config->interface->altsetting->bNumEndpoints < 2) 
        {
          // Nope, we maybe want the next one, let's try to find another
          continue;
        }

        handle = usb_open(dev);
        if (handle == NULL)
          continue;

        // usb_close(udev);
        udev = dev;
        return 1;
      }
      
    }
  }

  return 0;
}

int init_usb ()
{
    usb_prepare();
  
  printf("searching for ACR122U device ...\n");

  if (acr122_usb_scan())
    printf("ACR122U found !\n");
  else 
  {
    printf("device not found\n");
    return 0;
  }
  
  // Reset device
  usb_reset(handle);
// Retrieve end points
  acr122_usb_get_end_points(udev);
  // Claim interface
  int res = usb_claim_interface(handle, 0);
  if (res < 0) {
    
    usb_close(handle);
    // we failed to use the specified device
  return 1;
  }

  if (udev->config->interface->altsetting->bAlternateSetting > 0) {
    res = usb_set_altinterface(handle, 0);
    if (res < 0) {
      usb_close(handle);
      // we failed to use the specified device
      return 1;
    }
  }
}

int receive_from_reader (char* received_data, const size_t received_size)
{
    return (acr122_usb_bulk_read(received_data, received_size, 4000));
}

int send_to_reader (char* send_data, const size_t send_size)
{
    return (acr122_usb_bulk_write(send_data, send_size, 1000));
}