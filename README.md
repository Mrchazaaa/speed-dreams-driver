# charlierobot

![alt text](./charlierobot.png)

A speed-dreams-2 driver I've developed, to test my implementations of ABS and other driver assistance systems for my third year project.

In the academic community surrounding automotive-safety, it is vital to incorporate the behaviour of widespread software control systems (such as Adaptive Cruise Control, Traction Control Systems, etc) into models used in conducting accurate research. However, source code for popular commercial implementations of these systems is highly guarded by manufacturers and thus it is very difficult for researchers to develop models that accurately reflect the systems used in the real world.

The topic of my dissertation focused around a self-defined project entitled: "Construction of Automotive Control Software" , I produced an anti-lock braking system (ABS) which attempted to mimic the timing characteristics of commercial ABS products, by implementing an older ABS algorithm defined in a paper published by Bosch. However, this paper did not completely describe the system's construction (particularly in determining vehicle speed in the ABS Electronic Control Unit) and so work from several other research areas were combined to produce a working Anti-lock Braking System that, atleast to some degree, reflects those used in the real world.

One of the biggest challenges faced in producing this work was determining vehicle longitudinal velocity whilst only being able to discern circumferential speeds of individual wheels (these values would not directly reflect the actual velocity of the vehicle, under emergency braking conditions, due to large braking forces locking up the wheels). To solve this I implemented an Extended Kalman filter which estimates vehicle longitudinal velocity by utilising an accurate tire physics model and various other wheel speed data. Below, I've included several graphs showing actual vehicle velocity vs velocity calculated by my EKF to demonstrate how it converges on the real value with more accuracy over time.
