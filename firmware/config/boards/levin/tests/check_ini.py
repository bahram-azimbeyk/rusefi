from pathlib import Path
import re, sys
p = Path(sys.argv[1]) if len(sys.argv)>1 else Path("firmware/tunerstudio/generated/rusefi_levin.ini")
s=p.read_bytes().decode("latin-1") # Only ASCII INI syntax is inspected; Java output charset is host-dependent.
def choices(name):
 m=re.search(r"^#define "+name+r"=(.*)$",s,re.M)
 assert m, name
 return re.findall(r'"([^\"]*)"',m[1])
def idx(pin): return 2+16*(ord(pin[0])-ord("A"))+int(pin[1:])
outs={"B15":"INJ 1","A8":"INJ 2","B13":"INJ 3","B14":"INJ 4","E13":"INJ 5","B12":"INJ 6","E7":"INJ 7","E10":"INJ 8","C13":"IGN 1","E6":"IGN 2","E5":"IGN 3","E4":"IGN 4","E3":"IGN 5","E2":"IGN 6","B9":"IGN 7","D12":"IGN 8","E8":"TACHO","E15":"PNP PWM OUT","D10":"IDLE 1","D9":"IDLE 2","E11":"FUEL PUMP","E9":"FAN","D15":"PWM OUT 1","C6":"PWM OUT 2","D11":"PWM OUT 3","D8":"PWM OUT 4"}
ins={"B6":"VSS","D14":"GAUX 2","D3":"CRANK","D4":"CAM","D13":"AUX PULSE 1","E14":"GAUX 1","D7":"AUX PULSE 4","C7":"AUX PULSE 3","B8":"AUX PULSE 2"}
for name,mapping in [("output_pin_e_list",outs),("brain_input_pin_e_list",ins),("switch_input_pin_e_list",ins),("gpio_list",dict(outs,**ins))]:
 values=choices(name)
 assert values[0]=="NONE"
 assert sum(x not in ("NONE","INVALID") for x in values)==len(mapping),name
 for pin,label in mapping.items(): assert values[idx(pin)]==label,(name,pin)
 for pin in ["A9","A10","A15","B10","B11","C10","C11","C12","D0","D1","D2"]: assert values[idx(pin)]=="INVALID",(name,pin)
analog=choices("adc_channel_e_list")
for i,label in {1:"IAT",2:"CLT",3:"TPS",4:"MAP",9:"O2",11:"ANALOG IN 2",12:"ANALOG IN 3",13:"ANALOG IN 1"}.items(): assert analog[i]==label
fields=[line.split(",")[1].strip() for line in s.splitlines() if line.strip().startswith("field =") and "," in line]
for name in ["baroSensor_hwChannel","vbattAdcChannel","canRxPin","canTxPin","sdCardCsPin","spi3sckPin","idle_stepperDirectionPin","idle_stepperStepPin","stepperEnablePin"]: assert name not in fields,name
for name in ["clt_bias_resistor","iat_bias_resistor","vbattDividerCoeff"]: assert name in fields,name
assert "@@if_" not in s
print("PASS: 26 outputs, 9 digital inputs, 8 analog inputs, enum IDs, simple labels, hidden internal pins, editable calibration")
