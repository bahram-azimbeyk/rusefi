#pragma once
enum class UnexpectedCode { High,Low };
struct SensorResult { float Value; bool Valid; SensorResult(float v):Value(v),Valid(true){} SensorResult(UnexpectedCode):Value(0),Valid(false){} };
enum class SensorType {BarometricPressure,MapSlow,CompressorDischargePressure,ThrottleInletPressure,MapFast,MapFast2,Map,Map2,MapSlow2};
