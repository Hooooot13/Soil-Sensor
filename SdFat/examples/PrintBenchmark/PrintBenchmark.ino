/*
 * This sketch is a simple Print benchmark.
 */
#include <SdFat.h>
#include <SdFatUtil.h>

// SD chip select pin
const uint8_t chipSelect = SS;

// number of lines to print
#define N_PRINT 20000

#define PRINT_DOUBLE false

// file system
SdFat sd;

// test file
SdFile file;

// Serial output stream
ArduinoOutStream cout(Serial);
//------------------------------------------------------------------------------
// store error strings in flash to save RAM
#define error(s) sd.errorHalt_P(PSTR(s))
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  while (!Serial) {}  // wait for Leonardo
}
//------------------------------------------------------------------------------
void loop() {
  uint32_t maxLatency;
  uint32_t minLatency;
  uint32_t totalLatency;

  while (Serial.read() >= 0) {}
  // pstr stores strings in flash to save RAM
  cout << pstr("Type any character to start\n");
  while (Serial.read() <= 0) {}
  delay(400);  // catch Due reset problem

  cout << pstr("Free RAM: ") << FreeRam() << endl;

  // initialize the SD card at SPI_FULL_SPEED for best performance.
  // try SPI_HALF_SPEED if bus errors occur.
  if (!sd.begin(chipSelect, SPI_FULL_SPEED)) sd.initErrorHalt();

  cout << pstr("Type is FAT") << int(sd.vol()->fatType()) << endl;

  // open or create file - truncate existing file.
  if (!file.open("BENCH.TXT", O_CREAT | O_TRUNC | O_RDWR)) {
    error("open failed");
  }  
  cout << pstr("Starting print test.  Please wait.\n");

  // do write test

  maxLatency = 0;
  minLatency = 999999;
  totalLatency = 0;
  uint32_t t = millis();
  for (uint32_t i = 0; i < N_PRINT; i++) {
    uint32_t m = micros();
#if PRINT_DOUBLE
    file.println((double)0.01*i);
#else  // PRINT_DOUBLE
    file.println(i);
#endif  // PRINT_DOUBLW
   if (file.writeError) {
      error("write failed");
    }
    m = micros() - m;
    if (maxLatency < m) maxLatency = m;
    if (minLatency > m) minLatency = m;
    totalLatency += m;
  }
  file.sync();
  t = millis() - t;
  double s = file.fileSize();
  cout << pstr("Time ") << 0.001*t << pstr(" sec\n");
  cout << pstr("File size ") << 0.001*s << pstr("KB\n");
  cout << pstr("Write ") << s/t << pstr(" KB/sec\n");
  cout << pstr("Maximum latency: ") << maxLatency;
  cout << pstr(" usec, Minimum Latency: ") << minLatency;
  cout << pstr(" usec, Avg Latency: ") << totalLatency/N_PRINT << pstr(" usec\n\n");
 
  file.close();
  cout << pstr("Done!\n\n");
}
