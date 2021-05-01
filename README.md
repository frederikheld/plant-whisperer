# plant-whisperer

Little hacking project together with [@Jonnysplsh](https://github.com/Jonnysplsh/plant-whisperer). I tried to figure out how to use the MH-Z19C CO2 sensor together with a Elegoo microphone module without the one interfering with the other. Some notes on my findings and the solution can be found [in this document](https://www.notion.so/Investigation-on-Plant-Whisperer-sensor-issues-8aec2f37122c421aad824b1a2a6fca39).

The `main` directory contains the Arduino sketch in which both sensors run in a common scheduler. This file also contains advanced filtering and compensation for microphone noise and drift.
