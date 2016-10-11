# LineFollower
LTS 2016

//////////////////////////////////////////
//         Wiring Mega <-> UNO          //
//                                      //
//        UNO       MEGA                //
//        A5        21 SCL              //
//        A4        20 SDA              // 
//        ----------------              //
//        10        RESET               //
//        11        51                  //
//        12        50                  //
//        13        52                  //
//        ---- Vin+GND ----             //
//////////////////////////////////////////

Untuk mode programming:
1. Upload ArduinoISP ke UNO
2. pilih board Mega
3. Upload Using Programmer (Arduino as ISP)

Untuk Monitoring:
1. Upload Program MonitoringViaUNO ke UNO
2. Buka serial (pastikan Port sudah tepat)
