

// include the SPI library:
#include <SPI.h>
#include <Wire.h>

enum {
  GPA_OE = 0x00,
  GPA_ODATA,
  GPA_IDATA,

  GPB_OE = 0x04,
  GPB_ODATA,
  GPB_IDATA,

  GPC_OE = 0x08,
  GPC_ODATA,
  GPC_IDATA,
  #define GPC_ALT_UART_TX   0x01
  #define GPC_ALT_UART_RX   0x02
  #define GPC_ALT_UART_MASK (GPC_ALT_UART_TX | GPC_ALT_UART_RX)
  GPC_ALT,

  GPD_OE = 0x0C,
  GPD_ODATA,
  GPD_IDATA,

  GPE_OE = 0x10,
  GPE_ODATA,
  GPE_IDATA,

  SK6805_CTRL = 0x14,
  SK6805_DATA,

  DAC_DATA0 = 0x16,
  DAC_DATA1,

  UART_DATA = 0x18,
  #define UART_STAT_TX_BUSY  0x10
  #define UART_STAT_RX_DV    0x01
  UART_STAT,

  GPZ_OE = 0x1C,
  GPZ_ODATA,
  GPZ_IDATA,

  ADC_DATA = 0x1F,
};

const byte WRITE = 0b10000000;   // SPI2GPIO write

// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;
const int resetPin       =  9;

unsigned regRead(int address) {
  unsigned v;

  // take the SS pin low to select the chip:
  digitalWrite(slaveSelectPin, LOW);
  //  send in the address and value via SPI:
  SPI.transfer(address | 0x0);
  v = SPI.transfer(0x0);
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin, HIGH);
  return v;
}

unsigned regWrite(int address, int value) {
  unsigned v;
  // take the SS pin low to select the chip:
  digitalWrite(slaveSelectPin, LOW);
  //  send in the address and value via SPI:
  SPI.transfer(address | WRITE);
  v = SPI.transfer(value);
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin, HIGH);
  return v;
}



void setup() {
  int v;

  #if 1
  Serial.begin(115200);
  #else
  pinMode(0, INPUT);
  pinMode(1, INPUT);
  #endif

  // set the slaveSelectPin as an output:
  pinMode(slaveSelectPin, OUTPUT);

  // initialize SPI:
  SPI.begin();
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE3));


  // set the reset Pin as an output
  pinMode(resetPin,       OUTPUT);

  // reset FPGA logic
  digitalWrite(resetPin, LOW);
  delay(1);
  digitalWrite(resetPin, HIGH);

  Wire.begin();
}

int button_chk(void) {
  unsigned v;

  v = regRead(GPE_IDATA);
  if ((v & 0x7F) != 0x70)
    return -1;

  Serial.print("Switch on K1 ");
  for (;;)  if (0 != (regRead(GPE_IDATA) & 0x01)) break;
  Serial.println("OK");

  Serial.print("Switch on K2 ");
  for (;;)  if (0 != (regRead(GPE_IDATA) & 0x02)) break;
  Serial.println("OK");

  Serial.print("Switch on K3 ");
  for (;;)  if (0 != (regRead(GPE_IDATA) & 0x04)) break;
  Serial.println("OK");

  Serial.print("Switch on K4 ");
  for (;;)  if (0 != (regRead(GPE_IDATA) & 0x08)) break;
  Serial.println("OK");

  return 0;
}

static int button_checked = 0;
void loop() {

  int r;
  
  if (button_checked == 0) {
    Serial.print("BTN : ");
    r = button_chk();
    if (r < 0) {
      Serial.print("FAIL ");
      Serial.println(r);
    } else {
      Serial.println("OK");
    }

    button_checked = 1;
  }

  Serial.println();
  delay(1500);
}
