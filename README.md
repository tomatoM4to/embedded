```bash
gcc -o main \
    main.c \
    button.c \
    camara.c \
    dcmotor.c \
    fnd.c \
    lcd.c \
    led.c \
    potenandjodo.c \
    remotecontrol.c \
    servomoter.c \
    sound.c \
    stepmotor.c \
    tostring.c \
    ultrasonic.c \
    TM1637.c \
    irpigpio.c \
    -lwiringPi \
    -lwiringPiDev \
    -lpthread \
    -lpigpio \
    -I. \
    -fcommon
```

```bash
gcc -o main main.c button.c camera.c dcmotor.c fnd.c lcd.c led.c potenandjodo.c remotecontrol.c servomoter.c sound.c stepmotor.c tostring.c ultrasonic.c TM1637.c irpigpio.c -lwiringPi -lwiringPiDev -lpthread -lpigpio -I. -fcommon
```
