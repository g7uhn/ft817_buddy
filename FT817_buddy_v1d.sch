EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_Module:Arduino_Nano_v3.x A1
U 1 1 5F16D7F8
P 8200 2050
F 0 "A1" H 8700 3000 50  0000 C CNN
F 1 "Arduino_Nano_v3.x" H 9000 2850 50  0000 C CNN
F 2 "Module:Arduino_Nano" H 8200 2050 50  0001 C CIN
F 3 "http://www.mouser.com/pdfdocs/Gravitech_Arduino_Nano3_0.pdf" H 8200 2050 50  0001 C CNN
	1    8200 2050
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:AP1117-50 U1
U 1 1 5F175150
P 3150 2700
F 0 "U1" H 3150 2942 50  0000 C CNN
F 1 "AP1117-50" H 3150 2851 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 3150 2900 50  0001 C CNN
F 3 "http://www.diodes.com/datasheets/AP1117.pdf" H 3250 2450 50  0001 C CNN
	1    3150 2700
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x04 U2
U 1 1 5F177244
P 5350 2650
F 0 "U2" V 5350 2900 50  0000 C CNN
F 1 "B0505S-1W" V 5500 2600 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 5350 2650 50  0001 C CNN
F 3 "~" H 5350 2650 50  0001 C CNN
	1    5350 2650
	0    -1   -1   0   
$EndComp
$Comp
L Isolator:ADuM1201AR U3
U 1 1 5F1786F8
P 5400 1800
F 0 "U3" H 5400 2267 50  0000 C CNN
F 1 "ADuM1201AR" H 5400 2176 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 5400 1400 50  0001 C CIN
F 3 "https://www.analog.com/media/en/technical-documentation/data-sheets/ADuM1200_1201.pdf" H 5400 1700 50  0001 C CNN
	1    5400 1800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x05 J1
U 1 1 5F17A067
P 2150 1800
F 0 "J1" H 2068 1375 50  0000 C CNN
F 1 "FT817_ACC" H 2068 1466 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Horizontal" H 2150 1800 50  0001 C CNN
F 3 "~" H 2150 1800 50  0001 C CNN
	1    2150 1800
	-1   0    0    1   
$EndComp
Text GLabel 5250 2850 3    50   Input ~ 0
GND1
Text GLabel 5350 2850 3    50   Input ~ 0
VDD1
Text GLabel 5450 2850 3    50   Input ~ 0
GND2
Text GLabel 5550 2850 3    50   Input ~ 0
VDD2
Text GLabel 4900 2000 0    50   Input ~ 0
GND1
Text GLabel 5900 2000 2    50   Input ~ 0
GND2
Text GLabel 4700 1600 0    50   Input ~ 0
VDD1
Text GLabel 6100 1600 2    50   Input ~ 0
VDD2
Text GLabel 4250 2700 2    50   Input ~ 0
VDD1
Text GLabel 3150 3100 3    50   Input ~ 0
GND1
Text GLabel 2350 1700 2    50   Input ~ 0
GND1
Text GLabel 2350 1800 2    50   Input ~ 0
FT817_Vout
Text GLabel 1200 2700 0    50   Input ~ 0
FT817_Vout
Wire Wire Line
	3450 2700 3600 2700
Text GLabel 2350 2000 2    50   Input ~ 0
FT817_RXD
Text GLabel 2350 1900 2    50   Input ~ 0
FT817_TXD
Text GLabel 2350 1600 2    50   Input ~ 0
SHIELD
Text GLabel 4900 1900 0    50   Input ~ 0
FT817_TXD
Text GLabel 4900 1700 0    50   Input ~ 0
FT817_RXD
Text GLabel 8400 1050 1    50   Input ~ 0
VDD2
Text GLabel 8200 3050 3    50   Input ~ 0
GND2
Text GLabel 8300 3050 3    50   Input ~ 0
GND2
Text GLabel 5900 1900 2    50   Input ~ 0
ARD_RXD
Text GLabel 5900 1700 2    50   Input ~ 0
ARD_TXD
Text GLabel 7700 2550 0    50   Input ~ 0
ARD_TXD
Text GLabel 7700 2650 0    50   Input ~ 0
ARD_RXD
Text GLabel 7700 2250 0    50   Input ~ 0
BACKLIGHT
$Comp
L Connector_Generic:Conn_01x08 J2
U 1 1 5F182868
P 3350 4200
F 0 "J2" H 3430 4192 50  0000 L CNN
F 1 "ARD_SPI" H 3430 4101 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 3350 4200 50  0001 C CNN
F 3 "~" H 3350 4200 50  0001 C CNN
	1    3350 4200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 5F1877DB
P 1850 4950
F 0 "R1" V 1643 4950 50  0000 C CNN
F 1 "1k" V 1734 4950 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 1780 4950 50  0001 C CNN
F 3 "~" H 1850 4950 50  0001 C CNN
	1    1850 4950
	0    1    1    0   
$EndComp
Text GLabel 1700 4950 0    50   Input ~ 0
BACKLIGHT
Text GLabel 2300 5150 3    50   Input ~ 0
GND2
Wire Wire Line
	2300 4750 2300 4500
Wire Wire Line
	2300 4500 3150 4500
Text GLabel 3150 4600 0    50   Input ~ 0
GND2
Text GLabel 3150 4400 0    50   Input ~ 0
3V3
Text GLabel 8300 1050 1    50   Input ~ 0
3V3
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 5F18C3DC
P 3350 4850
F 0 "J3" H 3430 4842 50  0000 L CNN
F 1 "5VDC" H 3430 4751 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3350 4850 50  0001 C CNN
F 3 "~" H 3350 4850 50  0001 C CNN
	1    3350 4850
	1    0    0    -1  
$EndComp
Text GLabel 3150 4950 0    50   Input ~ 0
GND2
Text GLabel 3150 4850 0    50   Input ~ 0
VDD2
Text GLabel 3150 4300 0    50   Input ~ 0
CLK
Text GLabel 3150 4200 0    50   Input ~ 0
DIN
Text GLabel 3150 4100 0    50   Input ~ 0
DC
Text GLabel 3150 4000 0    50   Input ~ 0
CE
Text GLabel 3150 3900 0    50   Input ~ 0
RST
$Comp
L Transistor_BJT:BC817 Q1
U 1 1 5F18D8AF
P 2200 4950
F 0 "Q1" H 2391 4996 50  0000 L CNN
F 1 "BC817" H 2391 4905 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 2400 4875 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC817.pdf" H 2200 4950 50  0001 L CNN
	1    2200 4950
	1    0    0    -1  
$EndComp
Text GLabel 7700 2150 0    50   Input ~ 0
CLK
Text GLabel 7700 2050 0    50   Input ~ 0
DIN
Text GLabel 7700 1950 0    50   Input ~ 0
DC
Text GLabel 7700 1850 0    50   Input ~ 0
CE
Text GLabel 7700 1750 0    50   Input ~ 0
RST
Text GLabel 7700 1650 0    50   Input ~ 0
SW7
Text GLabel 7700 2450 0    50   Input ~ 0
SW8
Text GLabel 7700 2350 0    50   Input ~ 0
SW9
Text GLabel 7700 4150 0    50   Input ~ 0
GND2
Text GLabel 7700 4650 0    50   Input ~ 0
GND2
Text GLabel 7700 5150 0    50   Input ~ 0
GND2
Text GLabel 8100 4150 2    50   Input ~ 0
SW7
Text GLabel 8100 4650 2    50   Input ~ 0
SW8
Text GLabel 8100 5150 2    50   Input ~ 0
SW9
$Comp
L Connector_Generic:Conn_01x08 J4
U 1 1 5F1808F0
P 4500 4200
F 0 "J4" H 4580 4192 50  0000 L CNN
F 1 "DISP_PINS" H 4580 4101 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 4500 4200 50  0001 C CNN
F 3 "~" H 4500 4200 50  0001 C CNN
	1    4500 4200
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x08 J5
U 1 1 5F181317
P 5400 4200
F 0 "J5" H 5480 4192 50  0000 L CNN
F 1 "DISPLAY" H 5480 4101 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 5400 4200 50  0001 C CNN
F 3 "~" H 5400 4200 50  0001 C CNN
	1    5400 4200
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x08 J6
U 1 1 5F181D1F
P 6150 4200
F 0 "J6" H 6230 4192 50  0000 L CNN
F 1 "DISPLAY_SUPPORT" H 6230 4101 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 6150 4200 50  0001 C CNN
F 3 "~" H 6150 4200 50  0001 C CNN
	1    6150 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 3900 5200 3900
Wire Wire Line
	4700 4000 5200 4000
Wire Wire Line
	4700 4100 5200 4100
Wire Wire Line
	4700 4200 5200 4200
Wire Wire Line
	4700 4300 5200 4300
Wire Wire Line
	4700 4400 5200 4400
Wire Wire Line
	4700 4500 5200 4500
Wire Wire Line
	4700 4600 5200 4600
$Comp
L Device:R R2
U 1 1 5F195A56
P 9450 1900
F 0 "R2" V 9243 1900 50  0000 C CNN
F 1 "47k" V 9334 1900 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 9380 1900 50  0001 C CNN
F 3 "~" H 9450 1900 50  0001 C CNN
	1    9450 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9450 2050 9850 2050
Connection ~ 9450 2050
Text GLabel 9550 1550 2    50   Input ~ 0
VDD2
Wire Wire Line
	9550 1550 9450 1550
Wire Wire Line
	9450 1550 9450 1750
Text GLabel 10250 3850 2    50   Input ~ 0
GND2
Text GLabel 10250 4300 2    50   Input ~ 0
GND2
Text GLabel 10250 3400 2    50   Input ~ 0
GND2
Text GLabel 10250 2950 2    50   Input ~ 0
GND2
Text GLabel 10250 2500 2    50   Input ~ 0
GND2
Text GLabel 10250 2050 2    50   Input ~ 0
GND2
$Comp
L Device:R R3
U 1 1 5F1979C6
P 9450 2350
F 0 "R3" V 9243 2350 50  0000 C CNN
F 1 "10k" V 9334 2350 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 9380 2350 50  0001 C CNN
F 3 "~" H 9450 2350 50  0001 C CNN
	1    9450 2350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 5F197D64
P 9450 2800
F 0 "R4" V 9243 2800 50  0000 C CNN
F 1 "10k" V 9334 2800 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 9380 2800 50  0001 C CNN
F 3 "~" H 9450 2800 50  0001 C CNN
	1    9450 2800
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 5F198129
P 9450 3250
F 0 "R5" V 9243 3250 50  0000 C CNN
F 1 "22k" V 9334 3250 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 9380 3250 50  0001 C CNN
F 3 "~" H 9450 3250 50  0001 C CNN
	1    9450 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R6
U 1 1 5F198430
P 9450 3700
F 0 "R6" V 9243 3700 50  0000 C CNN
F 1 "47k" V 9334 3700 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 9380 3700 50  0001 C CNN
F 3 "~" H 9450 3700 50  0001 C CNN
	1    9450 3700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R7
U 1 1 5F19881E
P 9450 4150
F 0 "R7" V 9243 4150 50  0000 C CNN
F 1 "100k" V 9334 4150 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 9380 4150 50  0001 C CNN
F 3 "~" H 9450 4150 50  0001 C CNN
	1    9450 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	9450 2200 9450 2050
Wire Wire Line
	9450 2500 9450 2650
Wire Wire Line
	9450 2950 9450 3100
Wire Wire Line
	9450 3400 9450 3550
Wire Wire Line
	9450 3850 9450 4000
Wire Wire Line
	9450 4300 9850 4300
Wire Wire Line
	9850 3850 9450 3850
Connection ~ 9450 3850
Wire Wire Line
	9850 3400 9450 3400
Connection ~ 9450 3400
Wire Wire Line
	9850 2950 9450 2950
Connection ~ 9450 2950
Wire Wire Line
	9850 2500 9450 2500
Connection ~ 9450 2500
Wire Wire Line
	8700 2050 9450 2050
$Comp
L Device:C_Small C2
U 1 1 5F1AF0B1
P 2650 2900
F 0 "C2" H 2742 2946 50  0000 L CNN
F 1 "100nF" H 2742 2855 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 2650 2900 50  0001 C CNN
F 3 "~" H 2650 2900 50  0001 C CNN
	1    2650 2900
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C3
U 1 1 5F1AFB6D
P 3600 2900
F 0 "C3" H 3692 2946 50  0000 L CNN
F 1 "10uF" H 3692 2855 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 3600 2900 50  0001 C CNN
F 3 "~" H 3600 2900 50  0001 C CNN
	1    3600 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 3000 3150 3100
Wire Wire Line
	2650 3000 2650 3100
Wire Wire Line
	2650 3100 3150 3100
Wire Wire Line
	3600 3000 3600 3100
Wire Wire Line
	3600 3100 3150 3100
Connection ~ 3150 3100
Wire Wire Line
	2650 2800 2650 2700
Connection ~ 2650 2700
Wire Wire Line
	2650 2700 2850 2700
Wire Wire Line
	3600 2700 3600 2800
Connection ~ 3600 2700
$Comp
L Device:C_Small C1
U 1 1 5F1B2BDE
P 2250 2900
F 0 "C1" H 2342 2946 50  0000 L CNN
F 1 "10uF" H 2342 2855 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 2250 2900 50  0001 C CNN
F 3 "~" H 2250 2900 50  0001 C CNN
	1    2250 2900
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C4
U 1 1 5F1B3078
P 4000 2900
F 0 "C4" H 4092 2946 50  0000 L CNN
F 1 "100nF" H 4092 2855 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4000 2900 50  0001 C CNN
F 3 "~" H 4000 2900 50  0001 C CNN
	1    4000 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 2700 2250 2700
Wire Wire Line
	2250 2800 2250 2700
Connection ~ 2250 2700
Wire Wire Line
	2250 2700 2650 2700
Wire Wire Line
	2250 3000 2250 3100
Wire Wire Line
	2250 3100 2650 3100
Connection ~ 2650 3100
Wire Wire Line
	3600 2700 4000 2700
Wire Wire Line
	3600 3100 4000 3100
Wire Wire Line
	4000 3100 4000 3000
Connection ~ 3600 3100
Wire Wire Line
	4000 2800 4000 2700
Connection ~ 4000 2700
Wire Wire Line
	4000 2700 4250 2700
$Comp
L Device:C_Small C6
U 1 1 5F1B7D20
P 6000 1400
F 0 "C6" H 6092 1446 50  0000 L CNN
F 1 "100nF" H 6092 1355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 6000 1400 50  0001 C CNN
F 3 "~" H 6000 1400 50  0001 C CNN
	1    6000 1400
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C5
U 1 1 5F1B84ED
P 4800 1400
F 0 "C5" H 4600 1450 50  0000 L CNN
F 1 "100nF" H 4500 1350 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4800 1400 50  0001 C CNN
F 3 "~" H 4800 1400 50  0001 C CNN
	1    4800 1400
	1    0    0    -1  
$EndComp
Text GLabel 4800 1200 1    50   Input ~ 0
GND1
Text GLabel 6000 1200 1    50   Input ~ 0
GND2
Wire Wire Line
	4700 1600 4800 1600
Wire Wire Line
	5900 1600 6000 1600
Wire Wire Line
	6000 1500 6000 1600
Connection ~ 6000 1600
Wire Wire Line
	6000 1600 6100 1600
Wire Wire Line
	6000 1300 6000 1200
Wire Wire Line
	4800 1200 4800 1300
Wire Wire Line
	4800 1500 4800 1600
Connection ~ 4800 1600
Wire Wire Line
	4800 1600 4900 1600
$Comp
L Switch:SW_SPST SW10
U 1 1 5F1BF8EB
P 1900 2700
F 0 "SW10" H 1900 2935 50  0000 C CNN
F 1 "ON_OFF" H 1900 2844 50  0000 C CNN
F 2 "Button_Switch_THT:SW_Slide_1P2T_CK_OS102011MS2Q" H 1900 2700 50  0001 C CNN
F 3 "~" H 1900 2700 50  0001 C CNN
	1    1900 2700
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 5F1C2900
P 10050 2050
F 0 "SW1" H 10050 2335 50  0000 C CNN
F 1 "SW_Push" H 10050 2244 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 10050 2250 50  0001 C CNN
F 3 "~" H 10050 2250 50  0001 C CNN
	1    10050 2050
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 5F1C5093
P 10050 2500
F 0 "SW2" H 10050 2785 50  0000 C CNN
F 1 "SW_Push" H 10050 2694 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 10050 2700 50  0001 C CNN
F 3 "~" H 10050 2700 50  0001 C CNN
	1    10050 2500
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW3
U 1 1 5F1C5884
P 10050 2950
F 0 "SW3" H 10050 3235 50  0000 C CNN
F 1 "SW_Push" H 10050 3144 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 10050 3150 50  0001 C CNN
F 3 "~" H 10050 3150 50  0001 C CNN
	1    10050 2950
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW4
U 1 1 5F1C5C20
P 10050 3400
F 0 "SW4" H 10050 3685 50  0000 C CNN
F 1 "SW_Push" H 10050 3594 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 10050 3600 50  0001 C CNN
F 3 "~" H 10050 3600 50  0001 C CNN
	1    10050 3400
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW5
U 1 1 5F1C5F37
P 10050 3850
F 0 "SW5" H 10050 4135 50  0000 C CNN
F 1 "SW_Push" H 10050 4044 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 10050 4050 50  0001 C CNN
F 3 "~" H 10050 4050 50  0001 C CNN
	1    10050 3850
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW6
U 1 1 5F1C6224
P 10050 4300
F 0 "SW6" H 10050 4585 50  0000 C CNN
F 1 "SW_Push" H 10050 4494 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 10050 4500 50  0001 C CNN
F 3 "~" H 10050 4500 50  0001 C CNN
	1    10050 4300
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW7
U 1 1 5F1C6662
P 7900 4150
F 0 "SW7" H 7900 4435 50  0000 C CNN
F 1 "SW_Push" H 7900 4344 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 7900 4350 50  0001 C CNN
F 3 "~" H 7900 4350 50  0001 C CNN
	1    7900 4150
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW8
U 1 1 5F1C7A78
P 7900 4650
F 0 "SW8" H 7900 4935 50  0000 C CNN
F 1 "SW_Push" H 7900 4844 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 7900 4850 50  0001 C CNN
F 3 "~" H 7900 4850 50  0001 C CNN
	1    7900 4650
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW9
U 1 1 5F1C7CAD
P 7900 5150
F 0 "SW9" H 7900 5435 50  0000 C CNN
F 1 "SW_Push" H 7900 5344 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 7900 5350 50  0001 C CNN
F 3 "~" H 7900 5350 50  0001 C CNN
	1    7900 5150
	1    0    0    -1  
$EndComp
$Comp
L Device:Polyfuse_Small F1
U 1 1 5F1D7A02
P 1450 2700
F 0 "F1" V 1245 2700 50  0000 C CNN
F 1 "Polyfuse_Small" V 1336 2700 50  0000 C CNN
F 2 "Fuse:Fuse_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 1500 2500 50  0001 L CNN
F 3 "~" H 1450 2700 50  0001 C CNN
	1    1450 2700
	0    1    1    0   
$EndComp
Wire Wire Line
	1200 2700 1350 2700
Wire Wire Line
	1550 2700 1700 2700
$EndSCHEMATC
