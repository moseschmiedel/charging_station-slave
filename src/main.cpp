#include <Arduino.h>
#include <Dezibot.h>
#include <autocharge/Autocharge.hpp>

// put function declarations here:
void start_chg(SlaveData &slave)
{
  Serial.printf("Execute 'start_chg for slave %u'\n", slave.id);
}
void end_chg(SlaveData &slave)
{
  Serial.printf("Execute 'end_chg for slave %u'\n", slave.id);
}

auto chargingSlaves = Fifo<SlaveData *>();

Master master = Master(chargingSlaves,
                       start_chg,
                       end_chg);

void setup()
{
  // put your setup code here, to run once:
  master.begin();
  Serial.begin(115200);
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(1000);
  master.step();
}