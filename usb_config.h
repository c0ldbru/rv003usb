#ifndef _USB_CONFIG_H
#define _USB_CONFIG_H

//Defines the number of endpoints for this device. (Always add one for EP0). For two EPs, this should be 3.  For one, 2.
#define ENDPOINTS 2

/*	
	CH32V003FUN DevBoard:
	PD3 D+
	PD4 D-
	PD5 D-_PU

	CH32V003J4M6:
	PC1 D+
	PC2 D-
	PC4 D-_PU
*/

#define USB_PORT D     // [A,C,D] GPIO Port to use with D+, D- and DPU
#define USB_PIN_DP 3   // [0-4] GPIO Number for USB D+ Pin
#define USB_PIN_DM 4   // [0-4] GPIO Number for USB D- Pin
#define USB_PIN_DPU 5  // [0-7] GPIO for feeding the 1.5k Pull-Up on USB D- Pin; Comment out if not used / tied to 3V3!

#define RV003USB_OPTIMIZE_FLASH 1

#ifndef __ASSEMBLER__

#include <tinyusb_hid.h>

#ifdef INSTANCE_DESCRIPTORS
//Taken from http://www.usbmadesimple.co.uk/ums_ms_desc_dev.htm
static const uint8_t device_descriptor[] = {
	18, //Length
	1,  //Type (Device)
	0x10, 0x01, //Spec
	0x0, //Device Class
	0x0, //Device Subclass
	0x0, //Device Protocol  (000 = use config descriptor)
	0x08, //Max packet size for EP0 (This has to be 8 because of the USB Low-Speed Standard)
	0x09, 0x12, //ID Vendor   //TODO: register this in http://pid.codes/howto/ or somewhere.
	0x03, 0xb0, //ID Product
	0x02, 0x00, //ID Rev
	1, //Manufacturer string
	2, //Product string
	3, //Serial string
	1, //Max number of configurations
};

static const uint8_t special_hid_desc[] = { 
  0x05, 0x01, // Usage Page (Generic Desktop)
  0x09, 0x06, // Usage (Keyboard)
  0xA1, 0x01, // Collection (Application)
  0x05, 0x07, // Usage Page (Key Codes)
  0x19, 0xE0, // Usage Minimum (224)
  0x29, 0xE7, // Usage Maximum (231)
  0x15, 0x00, // Logical Minimum (0)
  0x25, 0x01, // Logical Maximum (1)
  0x75, 0x01, // Report Size (1)
  0x95, 0x08, // Report Count (8)
  0x81, 0x02, // Input (Data, Variable, Absolute)
  0x95, 0x01, // Report Count (1)
  0x75, 0x08, // Report Size (8)
  0x81, 0x03, // Input (Constant)
  0x95, 0x05, // Report Count (5)
  0x75, 0x01, // Report Size (1)
  0x05, 0x08, // Usage Page (LEDs)
  0x19, 0x01, // Usage Minimum (1)
  0x29, 0x05, // Usage Maximum (5)
  0x91, 0x02, // Output (Data, Variable, Absolute)
  0x95, 0x01, // Report Count (1)
  0x75, 0x03, // Report Size (3)
  0x91, 0x03, // Output (Constant)
  0x95, 0x06, // Report Count (6)
  0x75, 0x08, // Report Size (8)
  0x15, 0x00, // Logical Minimum (0)
  0x25, 0x65, // Logical Maximum (101)
  0x05, 0x07, // Usage Page (Key Codes)
  0x19, 0x00, // Usage Minimum (0)
  0x29, 0x65, // Usage Maximum (101)
  0x81, 0x00, // Input (Data, Array)
  0xC0        // End Collection
};


static const uint8_t config_descriptor[] = {  //Mostly stolen from a USB mouse I found.
	// configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
	9, 					// bLength;
	2,					// bDescriptorType;
	0x22, 0x00,			// wTotalLength

	0x01,					// bNumInterfaces (Normally 1)  (If we need an additional HID interface add here)
	0x01,					// bConfigurationValue
	0x00,					// iConfiguration
	0x80,					// bmAttributes (was 0xa0)
	0x32,					// bMaxPower (100mA for low speed devices)

	9,                    // bLength
	4,                    // bDescriptorType (Interface)
	0,                    // bInterfaceNumber
	0,                    // bAlternateSetting
	1,                    // bNumEndpoints
	0x03,                 // bInterfaceClass (0x03 = HID)
	0x01,                 // bInterfaceSubClass (Boot Interface)
	0x01,                 // bInterfaceProtocol (Keyboard)
	0,                    // iInterface

	9,					// bLength
	0x21,					// bDescriptorType (HID)
	0x10,0x01,		//bcd 1.1
	0x00, //country code
	0x01, //Num descriptors
	0x22, //DescriptorType[0] (HID)
	0x3F, 0x00,  // 63 bytes (0x3F in hex)

	7, //endpoint descriptor (For endpoint 1)
	0x05, //Endpoint Descriptor (Must be 5)
	0x81, //Endpoint Address
	0x03, //Attributes
	0x08,	0x00, //Size
	0x0A, // 10ms interval for low speed devices
};


//Ever wonder how you have more than 6 keys down at the same time on a USB keyboard?  It's easy. Enumerate two keyboards!

#define STR_MANUFACTURER u"rot13labs"
#define STR_PRODUCT      u"bootloader"
#define STR_SERIAL       u"NBTT" // Need to change to BOOT when we finally decide on a flashing mechanism.

struct usb_string_descriptor_struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	const uint16_t wString[];
};

const static struct usb_string_descriptor_struct string0 __attribute__((section(".rodata"))) = {
	4,
	3,
	{0x0409}
};
const static struct usb_string_descriptor_struct string1 __attribute__((section(".rodata")))  = {
    20,  // 9 characters * 2 (Unicode) + 2 (for bLength and bDescriptorType)
    3,   // bDescriptorType
    STR_MANUFACTURER
};
const static struct usb_string_descriptor_struct string2 __attribute__((section(".rodata")))  = {
    22,  // 10 characters * 2 (Unicode) + 2 (for bLength and bDescriptorType)
    3,   // bDescriptorType
    STR_PRODUCT
};
const static struct usb_string_descriptor_struct string3 __attribute__((section(".rodata")))  = {
    10,  // 4 characters * 2 (Unicode) + 2 (for bLength and bDescriptorType)
    3,   // bDescriptorType
    STR_SERIAL
};

// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
const static struct descriptor_list_struct {
	uint32_t	lIndexValue;
	const uint8_t	*addr;
	uint8_t		length;
} descriptor_list[] __attribute__((section(".rodata"))) = {
	{0x00000100, device_descriptor, sizeof(device_descriptor)},
	{0x00000200, config_descriptor, sizeof(config_descriptor)},
	{0x00002200, special_hid_desc, sizeof(special_hid_desc)},
	{0x00000300, (const uint8_t *)&string0, 4},
	{0x04090301, (const uint8_t *)&string1, sizeof(STR_MANUFACTURER)},
	{0x04090302, (const uint8_t *)&string2, sizeof(STR_PRODUCT)},	
	{0x04090303, (const uint8_t *)&string3, sizeof(STR_SERIAL)}
};
#define DESCRIPTOR_LIST_ENTRIES ((sizeof(descriptor_list))/(sizeof(struct descriptor_list_struct)) )

#endif // INSTANCE_DESCRIPTORS

#endif // __ASSEMBLER__
#endif 
