#include <Arduino.h>
#include <Dezibot.h>
#include <autocharge/Autocharge.hpp>

bool findAFriend(Slave *slave);

// put function declarations here:
void step_work(Slave *slave)
{
  Serial.printf("Execute 'step_work' for slave %u\n", slave->communication.getNodeId());
  slave->requestCharge();
  slave->multiColorLight.setTopLeds(RED);
  delay(3000);
}

bool step_to_charge(Slave *slave, MasterData &master)
{
  uint32_t last_toggle = 0;
  bool leds_on = false;
  /*
    while (findAFriend(slave) != true)
    {
      uint32_t now = millis();
      if (now - last_toggle > 1000)
      {
        if (leds_on)
        {
          slave->multiColorLight.turnOffLed(TOP);
          leds_on = false;
        }
        else
        {
          slave->multiColorLight.setTopLeds(YELLOW);
          leds_on = true;
        }
        last_toggle = now;
      }
    }
  */
  slave->multiColorLight.blink(3, YELLOW, TOP, 1000);
  slave->multiColorLight.turnOffLed(TOP);

  return true;
}
void step_wait_charge(Slave *slave, MasterData &master)
{
  slave->multiColorLight.setTopLeds(YELLOW);
  delay(3000);
}
bool requestedStop = false;
bool step_into_charge(Slave *slave, MasterData &master)
{
  slave->multiColorLight.blink(3, GREEN, TOP, 1000);
  slave->multiColorLight.turnOffLed(TOP);
  requestedStop = false;
  return true;
}

void step_charge(Slave *slave, MasterData &master)
{
  slave->multiColorLight.setTopLeds(GREEN);
  delay(15000); // the dezibot should wait here until it is charged full
  if (!requestedStop)
  {
    slave->requestStopCharge();
    requestedStop = true;
  }
}
bool step_exit_charge(Slave *slave, MasterData &master)
{
  slave->multiColorLight.blink(3, RED, TOP, 1000);
  slave->multiColorLight.turnOffLed(TOP);
  delay(3000);
  return true;
}

auto chargingSlaves = Fifo<SlaveData *>();

auto master = MasterData(4200495932);

Slave slave = Slave(SlaveState::WORK, master, step_work, step_to_charge, step_wait_charge, step_into_charge, step_charge, step_exit_charge);

void setup()
{
  delay(2000);
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("+------------------------+");
  Serial.println("| Charging Station Slave |");
  Serial.println("+------------------------+");
  Serial.println();
  slave.begin();
  Serial.println("Setup complete");
  slave.multiColorLight.setTopLeds(RED);
}

void loop()
{
  // put your main code here, to run repeatedly:
  slave.step();
}

const int centeredThreshold = 50;
const int STEP_TO_CHARGE_THRESHOLD = 1000;

bool findAFriend(Slave *slave)
{
  int32_t leftValue = (int32_t)slave->lightDetection.getAverageValue(IR_LEFT, 20, 1);
  int32_t rightValue = (int32_t)slave->lightDetection.getAverageValue(IR_RIGHT, 20, 1);
  switch (slave->lightDetection.getBrightest(IR))
  {
  case IR_FRONT:
    // correct Stearing to be centered
    if (abs(leftValue - rightValue) < centeredThreshold)
    {
      slave->motion.move();
    }
    else
    {
      if (leftValue > rightValue)
      {
        slave->motion.rotateAntiClockwise();
      }
      else
      {
        slave->motion.rotateClockwise();
      }
    }
    break;
  case IR_LEFT:
    slave->motion.rotateAntiClockwise();
    break;
  case IR_RIGHT:
    slave->motion.rotateClockwise();
    break;
  case IR_BACK:
    if (leftValue > rightValue)
    {
      slave->motion.rotateAntiClockwise();
    }
    else
    {
      slave->motion.rotateClockwise();
    }
    break;
  }

  if (slave->lightDetection.getAverageValue(IR_FRONT, 20, 1) > STEP_TO_CHARGE_THRESHOLD)
  {
    return true;
  }

  return false;
}