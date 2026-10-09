#pragma once
#include <cassert>
#include <cmath>
#include <cstddef>
#include <stdexcept>
enum class Gpio { Unassigned, A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,B0,B1,B2,B3,B4,B5,B6,B7,B8,B9,B10,B11,B12,B13,B14,B15,C0,C1,C2,C3,C4,C5,C6,C7,C8,C9,C10,C11,C12,C13,C14,C15,D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10,D11,D12,D13,D14,D15,E0,E1,E2,E3,E4,E5,E6,E7,E8,E9,E10,E11,E12,E13,E14,E15 };
enum { EFI_ADC_0=0,EFI_ADC_1=1,EFI_ADC_2=2,EFI_ADC_3=3,EFI_ADC_4=4,EFI_ADC_5=5,EFI_ADC_6=6,EFI_ADC_7=7,EFI_ADC_8=8,EFI_ADC_9=9,EFI_ADC_10=10,EFI_ADC_11=11,EFI_ADC_12=12,EFI_ADC_13=13,EFI_ADC_14=14,EFI_ADC_15=15, OM_DEFAULT=0, OM_INVERTED=1, SPI_DEVICE_3=3 };
enum air_pressure_sensor_type_e { MT_CUSTOM,MT_DENSO183,MT_MPX4100,MT_MPX4250,MT_MPX4250A,MT_HONDA3BAR,MT_DODGE_NEON_2003,MT_SUBY_DENSO,MT_GM_3_BAR,MT_GM_2_BAR,MT_GM_1_BAR,MT_TOYOTA_89420_02010,MT_MAZDA_1_BAR,MT_BOSCH_2_5,MT_MPXH6400,MT_MPXH6300,MT_BOSCH_3_BAR };
struct Pressure { int hwChannel=-1; air_pressure_sensor_type_e type=MT_CUSTOM; float lowValue=0,highValue=100; };
struct Therm { int adcChannel=-1; struct {float bias_resistor=0;} config; };
struct Config {
 struct {Gpio stepperDirectionPin{},stepperStepPin{},solenoidPin{};} idle;
 Gpio stepperEnablePin{},canRxPin{},canTxPin{},can2RxPin{},can2TxPin{},spi3sckPin{},spi3misoPin{},spi3mosiPin{},sdCardCsPin{};
 Gpio injectionPins[12]{},ignitionPins[12]{},triggerInputPins[2]{},camInputs[4]{},triggerSimulatorPins[2]{};
 Gpio fuelPumpPin{},fanPin{},tachOutputPin{},secondSolenoidPin{};
 int stepperEnablePinMode=0,sdCardCsPinMode=0,sdCardSpiDevice=0,vbattAdcChannel=-1,tps1_1AdcChannel=-1,uartConsoleSerialSpeed=0,stepperDirectionPinMode=0;
 bool useHbridgesToDriveIdleStepper=false,useRawOutputToDriveIdleStepper=false,is_enabled_spi_1=false,is_enabled_spi_2=false,is_enabled_spi_3=false,useStepperIdle=false;
 float adcVcc=0,analogInputDividerCoefficient=0,vbattDividerCoeff=0,mapLowValueVoltage=0,mapHighValueVoltage=5,mapErrorDetectionTooLow=-10,mapErrorDetectionTooHigh=500;
 Pressure baroSensor,afr; struct {Pressure sensor;} map; Therm clt,iat;
 int throttleInletPressureChannel=-1,compressorDischargePressureChannel=-1;
};
inline Config config; inline Config* engineConfiguration=&config;
inline bool sdDefaults=false;
inline void setDefaultSdCardParameters(){sdDefaults=true;}
inline void (*custom_board_DefaultConfiguration)()=nullptr;
inline void (*custom_board_ConfigOverrides)()=nullptr;
inline bool isAdcChannelValid(int v){return v>=0 && v<16;}
enum class ObdCode { CUSTOM_ERR_MAP_TYPE };
template<class... T> void firmwareError(T...){throw std::runtime_error("firmware error");}
#define MS2NT(x) (x)
#define INTERPOLATION_A(x1,y1,x2,y2) (((y2)-(y1))/((x2)-(x1)))
