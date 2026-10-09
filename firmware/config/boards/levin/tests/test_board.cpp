#include "pch.h"
#include "board_configuration.cpp"
#include "init_map.cpp"
#include "linear_func.cpp"
#include <iostream>
void LinearFunc::showInfo(float) const {}
void near(float a,float b){ assert(std::abs(a-b)<0.002f); }
int main(){
 setup_custom_board_overrides(); assert(custom_board_DefaultConfiguration && custom_board_ConfigOverrides);
 custom_board_DefaultConfiguration(); custom_board_ConfigOverrides(); assert(sdDefaults);assert(config.clt.config.bias_resistor==2490 && config.iat.config.bias_resistor==2490);
 const Gpio inj[]={Gpio::B15,Gpio::A8,Gpio::B13,Gpio::B14,Gpio::E13,Gpio::B12,Gpio::E7,Gpio::E10};
 const Gpio ign[]={Gpio::C13,Gpio::E6,Gpio::E5,Gpio::E4,Gpio::E3,Gpio::E2,Gpio::B9,Gpio::D12};
 for(int i=0;i<8;i++){assert(config.injectionPins[i]==inj[i]);assert(config.ignitionPins[i]==ign[i]);}
 for(int i=8;i<12;i++){assert(config.injectionPins[i]==Gpio::Unassigned);assert(config.ignitionPins[i]==Gpio::Unassigned);}
 assert(config.clt.adcChannel==1 && config.iat.adcChannel==0 && config.map.sensor.hwChannel==3 && config.afr.hwChannel==8);
 // Simulate a foreign tune trying to change the PCB-only wiring.
 config.idle.stepperDirectionPin=Gpio::E0;config.idle.stepperStepPin=Gpio::E1;config.stepperEnablePin=Gpio::E2;
 config.canRxPin=Gpio::B8;config.canTxPin=Gpio::B9;config.can2RxPin=Gpio::B12;config.can2TxPin=Gpio::B13;
 config.spi3sckPin=Gpio::B3;config.spi3misoPin=Gpio::B4;config.spi3mosiPin=Gpio::B5;config.sdCardCsPin=Gpio::A15;
 config.uartConsoleSerialSpeed=9600;config.baroSensor.hwChannel=12;config.vbattAdcChannel=13;config.clt.config.bias_resistor=2700;
 config.useHbridgesToDriveIdleStepper=true;config.useRawOutputToDriveIdleStepper=true;
 // External channels must retain a deliberate user reassignment.
 config.fanPin=Gpio::E10;config.injectionPins[7]=Gpio::Unassigned;config.ignitionPins[7]=Gpio::Unassigned;
 config.triggerInputPins[0]=Gpio::B6;config.camInputs[0]=Gpio::D13;config.idle.solenoidPin=Gpio::D15;
 custom_board_ConfigOverrides();
 assert(config.idle.stepperDirectionPin==Gpio::B10 && config.idle.stepperStepPin==Gpio::B11 && config.stepperEnablePin==Gpio::A15);
 assert(config.canRxPin==Gpio::D0 && config.canTxPin==Gpio::D1 && config.can2RxPin==Gpio::Unassigned && config.can2TxPin==Gpio::Unassigned);
 assert(config.spi3sckPin==Gpio::C10 && config.spi3misoPin==Gpio::C11 && config.spi3mosiPin==Gpio::C12 && config.sdCardCsPin==Gpio::D2 && config.sdCardSpiDevice==3);
 assert(config.uartConsoleSerialSpeed==115200 && config.baroSensor.hwChannel==5 && config.vbattAdcChannel==4);
 assert(config.clt.config.bias_resistor==2700 && config.iat.config.bias_resistor==2490);
 assert(!config.useHbridgesToDriveIdleStepper && !config.useRawOutputToDriveIdleStepper);
 assert(config.fanPin==Gpio::E10 && config.injectionPins[7]==Gpio::Unassigned && config.ignitionPins[7]==Gpio::Unassigned);
 assert(config.triggerInputPins[0]==Gpio::B6 && config.camInputs[0]==Gpio::D13 && config.idle.solenoidPin==Gpio::D15);
 // Execute the actual MAP/baro init and linear converter, with HAL/registration stubbed.
 config.map.sensor.type=MT_CUSTOM;config.map.sensor.lowValue=10;config.map.sensor.highValue=350;
 initMap();near(baroConverter.convert(3.9475f).Value,100);near(mapConverter.convert(5).Value,350);
 config.map.sensor.highValue=450;config.mapLowValueVoltage=0.5f;config.mapHighValueVoltage=4.5f;
 initMap();near(baroConverter.convert(3.9475f).Value,100);near(mapConverter.convert(4.5f).Value,450);
 config.baroSensor.lowValue=5;config.baroSensor.highValue=105;initMap();near(baroConverter.convert(2.5f).Value,55);
 config.baroSensor.type=MT_GM_1_BAR;initMap();near(baroConverter.convert(5).Value,105);
 assert(!baroConverter.convert(30).Valid);
 std::cout<<"PASS: defaults, fixed wiring after import, external reassignment, independent custom/preset barometer conversion\n";
}
