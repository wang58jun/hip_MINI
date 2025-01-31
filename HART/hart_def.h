/*
*********************************************************************************************************
* HART Common Table Definitions
*********************************************************************************************************
*/
#ifndef _HART_DEF_H
#define _HART_DEF_H

/*
*********************************************************************************************************
*                                              Generic
*********************************************************************************************************
*/
  /*   240  Note- Enumeration may be used for manufacturer specific definitions */
  /*   241  Note- Enumeration may be used for manufacturer specific definitions */
  /*   242  Note- Enumeration may be used for manufacturer specific definitions */
  /*   243  Note- Enumeration may be used for manufacturer specific definitions */
  /*   244  Note- Enumeration may be used for manufacturer specific definitions */
  /*   245  Note- Enumeration may be used for manufacturer specific definitions */
  /*   246  Note- Enumeration may be used for manufacturer specific definitions */
  /*   247  Note- Enumeration may be used for manufacturer specific definitions */
  /*   248  Note- Enumeration may be used for manufacturer specific definitions */
  /*   249  Note- Enumeration may be used for manufacturer specific definitions */
#define  COMM_TBL_NOT_USED         250      /* not_used                                                */
#define  COMM_TBL_NONE             251      /* none                                                    */
#define  COMM_TBL_UNKNOWN          252      /* unknown                                                 */
#define  COMM_TBL_SPEC             253      /* special                                                 */

#define  MS_TO_TIME(x)             (x * 32)

/*
*********************************************************************************************************
*                                         Device Variable Status
*********************************************************************************************************
*/
#define  DV_STATUS_GOOD            0xC0
#define  DV_STATUS_POOR            0x40
#define  DV_STATUS_FIXED           0x80
#define  DV_STATUS_BAD             0x00

#define  DV_STATUS_CONST           0x30
#define  DV_STATUS_LL              0x10
#define  DV_STATUS_UL              0x20
#define  DV_STATUS_NL              0x00

/*
*********************************************************************************************************
*                                   Table 1. Expanded Device Type Codes
*********************************************************************************************************
*/
#define  DEV_TYPE_NULL             0x0000u
#define  FCG_GENERIC               0xf982u

/*
*********************************************************************************************************
*                                     Table 2. Engineering Unit Codes
*********************************************************************************************************
*/
/* ----------------------- Temperature ------------------------ */
#define  UNIT_degC                 32       /* degrees_celsius                                         */
#define  UNIT_degF                 33       /* degrees_fahrenheit                                      */
#define  UNIT_degR                 34       /* degrees_rankine                                         */
#define  UNIT_Kelvin               35       /* degrees_kelvin                                          */

/* -------------------------- Current ------------------------- */
#define  UNIT_mA                   39       /* milliamperes                                            */

/* ----------------------- Miscellaneous ---------------------- */
#define  UNIT_mV                   36
#define  UNIT_Ohm                  37
#define  UNIT_Hz                   38       /* hertz                                                   */
#define  UNIT_microSiemens         56       /* microsiemens                                            */
#define  UNIT_Percent              57       /* percent                                                 */
#define  UNIT_pH                   59       /* pH                                                      */

/*
*********************************************************************************************************
*                                     Table 3. transfer function code
*********************************************************************************************************
*/
#define  XF_LINEAR                 0        /* linear, Equation y=mx+b                                 */
#define  XF_SQRT_ROOT              1        /* square_root, Equation y=sqrt(x)                         */
#define  XF_SQRT_ROOT3             2        /* square_root_to_the_third_power_sup, Equation y=sqrt(x^3)*/
#define  XF_SQRT_ROOT5             5        /* square_root_to_the_fifth_power_sup, Equation y=sqrt(x^5)*/
#define  XF_SPEC_CURVE             4        /* special_curve                                           */
#define  XF_SQUARE                 5        /* square, Equation y=x^2                                  */
#define  XF_DISCRETE               230      /* discrete_switch                                         */
#define  XF_SQRT_ROOT_SPEC_CURVE   231      /* square_root_plus_special_curve                          */
#define  XF_SQRT_ROOT3_SPEC_CURVE  232      /* square_root_to_the_third_power_plus_special_curve       */
#define  XF_SQRT_ROOT5_SPEC_CURVE  233      /* square_root_to_the_fifth_power_plus_special_curve       */

/*
*********************************************************************************************************
*                                        Table 4. material code
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             Table 5. NULL
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      Table 6. alarm selection code
*********************************************************************************************************
*/
#define  ALARM_MODE_HIGH           0        /* high                                                    */
#define  ALARM_MODE_LOW            1        /* low                                                     */
#define  ALARM_MODE_HOLD           239      /* hold_last_output_value                                  */

/*
*********************************************************************************************************
*                                       Table 7. write protect code
*********************************************************************************************************
*/
#define  WT_PROTECT_NO             0        /* not_write_protected                                     */
#define  WT_PROTECT_YES            1        /* write_protected                                         */

/*
*********************************************************************************************************
*                            Table 8. Company (Manufacturer) Identification Codes
*********************************************************************************************************
*/
#define  MANU_ID_NULL              0x0000u
#define  MANU_ID_FCG               0x00f9u

/*
*********************************************************************************************************
*                                   Table 9. burst mode control code
*********************************************************************************************************
*/
#define  BURST_MODE_OFF            0        /* off                                                     */
#define  BURST_MODE_ON             1        /* on                                                      */

/*
*********************************************************************************************************
*                                  Tables 10. Physical Signaling Codes
*********************************************************************************************************
*/
#define  PSC_BELL_202_CURRENT      0        /* bell_202_current                                        */
#define  PSC_BELL_202_VOLTAGE      1        /* bell_202_voltage                                        */
#define  PSC_RS_485                2        /* rs_485                                                  */
#define  PSC_RS_232                3        /* rs_232                                                  */
#define  PSC_SPEC                  6        /* special                                                 */

/*
*********************************************************************************************************
*                                      Tables 11. Flag Assignments
*********************************************************************************************************
*/
#define  FLAG_MULTI_SENS_DEV       0x01     /* multi_sensor_fld_dev                                    */
#define  FLAG_EEPROM_CTRL          0x02     /* EEPROM_Control                                          */
#define  FLAG_PROT_BRIDGE_DEV      0x04     /* protocol_bridge_device                                  */
#define  FLAG_PSK_CAPABLE_DEV      0x40     /* C8PSK_capable_device                                    */
#define  FLAG_PSK_LOOP_CURR_DIS    0x80     /* C8PSK_loop_current_disabled                             */

/*
*********************************************************************************************************
*                              Tables 12. Transfer Service Function Codes
*                          See Block Data Transfer Specification (HCF_SPEC-190)
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                              Tables 13. Transfer Service Identifier Codes
*                          See Block Data Transfer Specification (HCF_SPEC-190)
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      Tables 14. Operating Mode Codes
*********************************************************************************************************
*/
#define  OPERAT_MODE_NORMAL        0        /* normal                                                  */

/*
*********************************************************************************************************
*                                   Tables 15. Analog Output Numbers Codes
*********************************************************************************************************
*/
#define  AO_NUM_CODE_CH0           0        /* analog_output_0                                         */
#define  AO_NUM_CODE_CH1           1        /* analog_output_1                                         */
#define  AO_NUM_CODE_CH2           2        /* analog_output_2                                         */
#define  AO_NUM_CODE_CH3           3        /* analog_output_3                                         */
#define  AO_NUM_CODE_CH4           4        /* analog_output_4                                         */

/*
*********************************************************************************************************
*                                     Tables 16. Loop Current Mode Codes
*********************************************************************************************************
*/
#define  LOOP_CURRENT_MODE_DIS     0        /* disabled_loop_current_mode                              */
#define  LOOP_CURRENT_MODE_EN      1        /* enabled_loop_current_mode                               */

/*
*********************************************************************************************************
*                                    Table 17. Extended Device Status Codes
*********************************************************************************************************
*/
#define  MAINTENANCE_REQUIRED      0x01     /* maintenance_required                                    */
#define  DEVICE_VARIABLE_ALERT     0x02     /* device_variable_alert                                   */
#define  CRITICAL_POWER_FAILURE    0x04     /* critical_power_failure                                  */
#define  DEVICE_VARIABLE_FAILURE   0x08     /* device_variable_failure                                 */
#define  OUT_OF_SPECIFICATION      0x10     /* out_of_specification                                    */
#define  FUNCTION_CHECK            0x20     /* function_check                                          */

/*
*********************************************************************************************************
*                                        Table 18. Lock Device Codes
*********************************************************************************************************
*/
#define  UNLOCK                    0        /* unlock                                                  */
#define  LOCK_TEMPORARY            1        /* temporary_lock                                          */
#define  LOCK_PERMANENT            2        /* permanent_lock                                          */

/*
*********************************************************************************************************
*                                     Table 19. Write Device Variable Codes
*********************************************************************************************************
*/
#define  WT_DEV_VAR_NORMAL         0        /* normal_device_variable                                  */
#define  WT_DEV_VAR_FIX            1        /* fix_value                                               */

/*
*********************************************************************************************************
*                                     Table 20. Device Variable Family Codes
*********************************************************************************************************
*/
  /* codes 0-3 are reserved and must never be used */
#define  FAMILY_TEMPERATURE        4        /* temperature_family                                      */
#define  FAMILY_PRESSURE           5        /* pressure_family                                         */
#define  FAMILY_ACTUATOR           6        /* actuator_family                                         */
#define  FAMILY_SIMPLE_PID         7        /* simple_pid_family                                       */
#define  FAMILY_PH                 8        /* ph_family                                               */
#define  FAMILY_CONDUCTIVITY       9        /* conductivity_family                                     */
#define  FAMILY_TOTALIZER          10       /* totalizer_family                                        */
#define  FAMILY_LEVEL              11       /* level_family                                            */
#define  FAMILY_VORTEX_FLOW        12       /* vortex_flow_family                                      */
#define  FAMILY_MAG_FLOW           13       /* mag_flow_family                                         */
#define  FAMILY_CORIOLIS_FLOW      14       /* coriolis_flow_family                                    */

/*
*********************************************************************************************************
*                                Table 21. Device Variable Classification Codes
*********************************************************************************************************
*/
#define  CLASS_NONE                0        /* device_variable_not_classified                          */
#define  CLASS_TEMPERATURE         64       /* temperature_classification                              */
#define  CLASS_PRESSURE            65       /* pressure_classification                                 */
#define  CLASS_VOLUME_FLOW         66       /* volumetric_flow_classification                          */
#define  CLASS_VELOCITY            67       /* velocity_classification                                 */
#define  CLASS_VOLUME              68       /* volume_classification                                   */
#define  CLASS_LENGTH              69       /* length_classification                                   */
#define  CLASS_TIME                70       /* time_classification                                     */
#define  CLASS_MASS                71       /* mass_classification                                     */
#define  CLASS_MASS_FLOW           72       /* mass_flow_classification                                */
#define  CLASS_MASS_PER_VOLUME     73       /* mass_per_volume_classification                          */
#define  CLASS_VISCOSITY           74       /* viscosity_classification                                */
#define  CLASS_ANGULAR_VELOCITY    75       /* angular_velocity_classification                         */
#define  CLASS_AREA                76       /* area_classification                                     */
#define  CLASS_ENERGY_WORK         77       /* energy_work_classification                              */
#define  CLASS_FORCE               78       /* force_classification                                    */
#define  CLASS_POWER               79       /* power_classification                                    */
#define  CLASS_FREQUENCY           80       /* frequency_classification                                */
#define  CLASS_ANALYTICAL          81       /* analytical_classification                               */
#define  CLASS_CAPACITANCE         82       /* capacitance_classification                              */
#define  CLASS_EMF                 83       /* emf_classification                                      */
#define  CLASS_CURRENT             84       /* current_classification                                  */
#define  CLASS_RESISTANCE          85       /* resistance_classification                               */
#define  CLASS_ANGLE               86       /* angle_classification                                    */
#define  CLASS_CONDUCTANCE         87       /* conductance_classification                              */
#define  CLASS_VOLUME_PER_VOLUME   88       /* volume_per_volume_classification                        */
#define  CLASS_VOLUME_PER_MASS     89       /* volume_per_mass_classification                          */
#define  CLASS_CONCENTRATION       90       /* concentration_classification                            */
#define  CLASS_VALVE_ACTUATOR      91       /* valve_actuator_classification                           */
#define  CLASS_LEVEL               92       /* level_classification                                    */
#define  CLASS_VORTEX_FLOW         93       /* vortex_flow_classification                              */
#define  CLASS_MAG_FLOW            94       /* mag_flow_classification                                 */
#define  CLASS_CORIOLIS_FLOW       95       /* coriolis_flow_classification                            */

/*
*********************************************************************************************************
*                                       Table 22. Trim Point Codes
*********************************************************************************************************
*/
#define  TRIM_SUPPORT_NONE         0
#define  TRIM_LOWER_SUPPORT        1        /* lower_trim_point_supported                              */
#define  TRIM_UPPER_SUPPORT        2        /* upper_trim_point_supported                              */
#define  TRIM_LOWER_UPPER_SUPPORT  3        /* lower_and_upper_trim_point_supported                    */

/*
*********************************************************************************************************
*                                       Table 23. Capture Mode Codes
*********************************************************************************************************
*/
#define  CAPTURE_DISABLE           0        /* disabled_capture                                        */
#define  CAPTURE_FROM_SPEC_DEV     1        /* enabled_capture_from_specified_field_device             */
#define  CAPTURE_FROM_BACK_MSG     2        /* enabled_capture_from_back_messages                      */

/*
*********************************************************************************************************
*                                    Table 24. Physical Layer Type Codes
*********************************************************************************************************
*/
#define  PHY_ASYN_FSK              0        /* asynchronous_fsk                                        */
#define  PHY_SYN_PSK               1        /* synchronous_psk                                         */

/*
*********************************************************************************************************
*                                       Table 25. Lock Device Status
*********************************************************************************************************
*/
#define  DEV_LOCKED                0x01     /* device_is_locked                                        */
#define  PERMANENT_LOCKED          0x02     /* lock_is_permanent                                       */
#define  LOCKED_BY_PRIMARY         0x04     /* locked_by_primary_master                                */

/*
*********************************************************************************************************
*                                      Table 26. Analog Channel Flags
*********************************************************************************************************
*/
#define  ANALOG_INPUT_CHANNEL      0x01     /* This Analog Channel is a Field Dev analog input channel.*/
                                            /* In other words, the Field Device has an ADC connected 	 */
                                            /* to this channel when this bit is set.                   */

/*
*********************************************************************************************************
*                                      Table 29. Standardized Status 0
*********************************************************************************************************
*/
#define  SIMULATION_ACTIVE         0x01     /* The device is in simulation mode                        */
#define  NV_MEM_FAIL               0x02     /* The Non-Volatile memory check is invalid or maybe corrupt */
#define  RAM_ERROR                 0x04     /* The RAM memory check is invalid or maybe corrupt        */
#define  WDOG_RESET_EXE            0x08     /* A watchdog reset has been performed                     */
#define  VOL_COND_OUT_RNG          0x10     /* A voltage condition is outside its allowable range      */
#define  ENV_COND_OUT_RNG          0x20     /* An internal or environmental condition is beyond acceptable limits */   
#define  ELECTRONIC_FAIL           0x40     /* A hardware problem not related to the sensor has been detected */
#define  DEV_CFG_LOCKED            0x80     /* Device is in write-protect or is locked                 */

/*
*********************************************************************************************************
*                                       Table 30. Standardized Status 1
*********************************************************************************************************
*/
#define  STS_SIMULATION_ACTIVE     0x01     /* Status Simulation Mode has been enabled                 */
#define  DISCRETE_VAR_SIM_ACTIVE   0x02     /* Discrete variables are in simulation mode               */
#define  EVENT_NOTIFY_OVERFLOW     0x04     /* One or more Event Specification overflows in an event   */
#define  BAT_PWR_NEED_MAINTAIN     0x08     /* Battery or Power Supply requires maintenance            */
#define  SECURE_C8PSK_MODE         0x10     /* Device is locked into C8PSK-only mode for security      */

/*
*********************************************************************************************************
*                                  Table 33. Burst Message Trigger Mode Codes
*********************************************************************************************************
*/
#define  BT_MODE_CONT              0        /* continuous                                              */
#define  BT_MODE_WINDOW            1        /* Window                                                  */
#define  BT_MODE_RISE              2        /* Rising                                                  */
#define  BT_MODE_FALL              3        /* Falling                                                 */
#define  BT_MODE_CHANG             4        /* On-change                                               */

/*
*********************************************************************************************************
*                                          Table 38. Time-set Codes
*********************************************************************************************************
*/
#define  RD_RCV_TIME               0        /* Read Receive Time                                       */
#define  WT_DATE_TIME              1        /* Write Date & Time                                       */

/*
*********************************************************************************************************
*                                    Table 54. SI Units Control Codes
*********************************************************************************************************
*/
#define  SI_UNIT_UNLIMIT           0        /* No restrictions                                         */
#define  SI_UNIT_LIMIT             1        /* Unit code limited to SI Unit only                       */

/*
*********************************************************************************************************
*                                      Table 66. Squawk Control Codes
*********************************************************************************************************
*/
#define  SQUAWK_OFF                0        /* Squawk off                                              */
#define  SQUAWK_ON                 1        /* Squawk on                                               */
#define  SQUAWK_ONCE               2        /* Squawk once                                             */

/*
*********************************************************************************************************
*                                  Table 70. Condensed Status Mapping Codes
*********************************************************************************************************
*/
#define  CSMC_NO_EFFECT            0        /* No effect ("N")                                         */
#define  CSMC_MAINTEN_REQ          1        /* Maintenance Required ("M")                              */
#define  CSMC_FAIL                 3        /* Failure ("F")                                           */
#define  CSMC_OUT_SPEC             4        /* Out of Specification ("S")                              */
#define  CSMC_FUNC_CHK             5        /* Function Check ("C")                                    */
#define  CSMC_NOT_DEF              6        /* Not Defined ("U")                                       */

/*
*********************************************************************************************************
*                                    Table 71. Status Simulation Mode Codes
*********************************************************************************************************
*/
#define  STATUS_SIM_DIS            0        /* Status Simulation Disabled                              */
#define  STATUS_SIM_EN             1        /* Status Simulation Enabled                               */

/*
*********************************************************************************************************
*                                             DEVICE VARIABLES
*********************************************************************************************************
*/
#define  DEV_VAR_BV_CODE           242      /* Device Variable Code 242: Battery Voltage (Unit in V)   */
#define  DEV_VAR_BL_CODE           243      /* Device Variable Code 243: Battery Life (Unit in days)   */
#define  DEV_VAR_PR_CODE           244      /* Device Variable Code 244: Percent Range (Unit in %)     */
#define  DEV_VAR_LC_CODE           245      /* Device Variable Code 245: Loop Current (Unit in mA)     */
#define  DEV_VAR_PV_CODE           246      /* Device Variable Code 246: Primary Variable              */
#define  DEV_VAR_SV_CODE           247      /* Device Variable Code 246: Secondary Variable            */
#define  DEV_VAR_TV_CODE           248      /* Device Variable Code 246: Tertiary Variable             */
#define  DEV_VAR_QV_CODE           249      /* Device Variable Code 246: Quaternary Variable           */

#endif /* _HART_DEF_H */
