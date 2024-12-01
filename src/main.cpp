#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "drvAMC.hpp"
#include "ni-daqmx.hpp"

#include <fstream>

void cb(int nb_channels, int samplingSize, double *data, uint64_t timestamp_sec, uint64_t timestamp_nsec, void *obj)
{
  static int j = 0;
  std::ofstream &outFile = *static_cast<std::ofstream *>(obj);
  if (outFile.is_open())
  {
    outFile.write(reinterpret_cast<const char *>(data), 6 * sizeof(double));
  }
  else
  {
    std::cerr << "Error opening file for writing." << std::endl;
  }
  if (j++ % 1000 == 0)
  {
    printf("\xd \bFx=%.3f Fy=%.3f Fz=%.3f Tx=%.3f Ty=%.3f Tz=%.3f", data[0], data[1], data[2], data[3], data[4], data[5]);
    fflush(stdout);
  }
}

int main(int argc, char **argv)
{
  NI::ATI::FT6_sensor sensor(2);
  sensor.start_thread();

  std::string filename = "data.bin";

  std::ofstream outFile(filename, std::ios::binary);

  sensor.set_callback(cb, &outFile);
  AMC::Driver drv(3);
  try
  {
    drv.open_connection("192.168.127.151", 5000, 10);
  }
  catch (const std::string &e)
  {
    std::cerr << e << '\n';
  }

  usleep(1000000);

  drv.reset();
  drv.set_target_position(2);
  drv.set_Kd(10000);
  std::cout << "waiting for position" << std::endl;

  usleep(3000000);
  drv.set_current(0);

  //close the file
  outFile.close();

  // fingerTip.position(2) = drv.get_position();
  // drv.set_Kp(interaction.forceAmp());
  // drv.set_Kp(6000);
  // drv.set_current(-4000);
  return 0;
}
