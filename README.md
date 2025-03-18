# RL78_F13_DataFlash
 RL78_F13_DataFlash

1. initial F13 EVB , to test data flash library

- UART : P15,P16 , to printf message

2. refer to document : 

- R01US0055ED : Data Flash Access Library Type T04 (Pico), European Release

- R01US0049EJ : RL78 Family Data Flash Library Type04 User's Manual in Japan and Other Asian Countries (Japanese Release)

- R20UT0749EJ : RL78 Family Data Flash Library Type04 Package Ver.3.00 Release Note in Japan and Other Asian Countries (Japanese Release)

- R20UT2944EJ : RL78 Family Self RAM list of Flash Self Programming Library

download Data Flash Library Type04 for RL78 Family

https://www.renesas.com/en/document/upr/data-flash-library-type04-package-ver300-rl78-family?r=488896

3. 

code generator setting

![image](https://github.com/released/RL78_F13_DataFlash/blob/main/code_generator_1.jpg)

CC-RL(Build Tool) > Link Options > Section > Section start address , add ROM : PFDL_COD and RAM : 0xFDF88 

![image](https://github.com/released/RL78_F13_DataFlash/blob/main/F13_Link_section.jpg)

![image](https://github.com/released/RL78_F13_DataFlash/blob/main/F13_SelfRAM.jpg)

compile error : E0562351 

![image](https://github.com/released/RL78_F13_DataFlash/blob/main/E0562351_SELFRAM.jpg)

4. 

**key point** : must disable interrupt before call flash driver , and enable interrupt after call flash driver

refer to Data_Flash_init , Data_Flash_read , Data_Flash_write_test

```
  DI();
  
  // when use flash driver function
  
  EI();  
```

5. 

when power on will read data flash result 

press digit 1 , will write data flash 

press digit 2 , will read data flash 


below is log message

![image](https://github.com/released/RL78_F13_DataFlash/blob/main/log.jpg)


