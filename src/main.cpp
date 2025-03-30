#include <Arduino.h>
#include <Dezibot.h>
#include <autocharge/Autocharge.hpp>

// put function declarations here:
void step_work(Slave *slave)
{
  Serial.printf("Execute 'step_work' for slave %u\n", slave->communication.getNodeId());
}

bool step_to_charge(Slave *slave, MasterData &master) {}
void step_wait_charge(Slave *slave, MasterData &master) {}
bool step_into_charge(Slave *slave, MasterData &master) {}
void step_charge(Slave *slave, MasterData &master) {}
bool step_exit_charge(Slave *slave, MasterData &master) {}

auto chargingSlaves = Fifo<SlaveData *>();

auto master = MasterData(4200495932);

Slave slave = Slave(SlaveState::WORK, master, step_work, step_to_charge, step_wait_charge, step_into_charge, step_charge, step_exit_charge);

void setup()
{
  delay(2000);
  // put your setup code here, to run once:
  Serial.println("+------------------------+");
  Serial.println("| Charging Station Slave |");
  Serial.println("+------------------------+");
  Serial.println();
  slave.begin();
  Serial.begin(115200);
  Serial.println("Setup complete");
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(5000);
  slave.requestCharge();
}
