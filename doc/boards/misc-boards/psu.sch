EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 6 6
Title "Pédale Vite v2 — Auxiliary boards"
Date "2019-10-22"
Rev ""
Comp "Laurent DE SORAS"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ac-dc:IRM-02-5 U501
U 1 1 5D924859
P 5100 3600
F 0 "U501" H 5500 3850 50  0000 C CNN
F 1 "RAC15-05SK" H 5500 3450 50  0000 C CNN
F 2 "pedale-vite:Converter_ACDC_RECOM_RAC15-xxSK_THT" H 5500 3350 50  0001 C CNN
F 3 "" H 5500 3250 50  0001 C CNN
	1    5100 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 3500 5100 3500
Wire Wire Line
	4900 3600 5100 3600
Wire Wire Line
	5900 3500 6000 3500
Wire Wire Line
	5900 3600 6000 3600
$Comp
L conn:CONN_01X02 J502
U 1 1 5D920686
P 6200 3550
F 0 "J502" H 6200 3700 50  0000 C CNN
F 1 "5VDC" H 6200 3400 50  0000 C CNN
F 2 "Connectors_Terminal_Blocks:TerminalBlock_Altech_AK300-2_P5.00mm" H 6200 3550 50  0001 C CNN
F 3 "" H 6200 3550 50  0001 C CNN
	1    6200 3550
	1    0    0    1   
$EndComp
$Comp
L conn:CONN_01X02 J501
U 1 1 5D9206F7
P 4700 3550
F 0 "J501" H 4700 3700 50  0000 C CNN
F 1 "230VAC" H 4700 3400 50  0000 C CNN
F 2 "Connectors_Terminal_Blocks:TerminalBlock_Altech_AK300-2_P5.00mm" H 4700 3550 50  0001 C CNN
F 3 "" H 4700 3550 50  0001 C CNN
	1    4700 3550
	-1   0    0    1   
$EndComp
$Comp
L power:PWR_FLAG #FLG01
U 1 1 5D920A47
P 5000 3300
F 0 "#FLG01" H 5000 3375 50  0001 C CNN
F 1 "PWR_FLAG" H 5000 3450 50  0000 C CNN
F 2 "" H 5000 3300 50  0001 C CNN
F 3 "" H 5000 3300 50  0001 C CNN
	1    5000 3300
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG02
U 1 1 5D920A6B
P 5000 3800
F 0 "#FLG02" H 5000 3875 50  0001 C CNN
F 1 "PWR_FLAG" H 5000 3950 50  0000 C CNN
F 2 "" H 5000 3800 50  0001 C CNN
F 3 "" H 5000 3800 50  0001 C CNN
	1    5000 3800
	-1   0    0    1   
$EndComp
Wire Wire Line
	5000 3800 5000 3600
Connection ~ 5000 3600
Wire Wire Line
	5000 3300 5000 3500
Connection ~ 5000 3500
$Comp
L misc-boards-rescue:Mounting_Hole MK501
U 1 1 5D9223FA
P 7100 3400
AR Path="/5D9223FA" Ref="MK501"  Part="1" 
AR Path="/5D9245F3/5D9223FA" Ref="MK501"  Part="1" 
F 0 "MK501" H 7100 3600 50  0000 C CNN
F 1 "Mounting_Hole" H 7100 3525 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3" H 7100 3400 50  0001 C CNN
F 3 "" H 7100 3400 50  0001 C CNN
	1    7100 3400
	1    0    0    -1  
$EndComp
$Comp
L misc-boards-rescue:Mounting_Hole MK502
U 1 1 5D922408
P 7100 3800
AR Path="/5D922408" Ref="MK502"  Part="1" 
AR Path="/5D9245F3/5D922408" Ref="MK502"  Part="1" 
F 0 "MK502" H 7100 4000 50  0000 C CNN
F 1 "Mounting_Hole" H 7100 3925 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3" H 7100 3800 50  0001 C CNN
F 3 "" H 7100 3800 50  0001 C CNN
	1    7100 3800
	1    0    0    -1  
$EndComp
$Comp
L misc-boards-rescue:Mounting_Hole MK503
U 1 1 5D92240F
P 7700 3800
AR Path="/5D92240F" Ref="MK503"  Part="1" 
AR Path="/5D9245F3/5D92240F" Ref="MK503"  Part="1" 
F 0 "MK503" H 7700 4000 50  0000 C CNN
F 1 "Mounting_Hole" H 7700 3925 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3" H 7700 3800 50  0001 C CNN
F 3 "" H 7700 3800 50  0001 C CNN
	1    7700 3800
	1    0    0    -1  
$EndComp
$EndSCHEMATC
