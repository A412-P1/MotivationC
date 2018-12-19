# MotivationC

This program has been developed as part of a semester at AAU. The programs goal is to provide motivating output based on garbage sorted. This application specifically addresses the motivational process, and as such, garbage disposal is simmulated through manual entry with the following syntax:
```
fractiontype weight
```
where fractiontype is one of four fractiontypes (*residual, paper, plastic or metal*) and weigth is a postive integer. An example is given below:
```
residual 1250
```
Included is 3 data files: *save, user.stats* and *all_user.stats*. These are datafiles used at runtime.

DEMO_MODE and ANIMATIONs are enabled by default, but can be changed in *motiveringsprogram.c*. DEMO_MODE is used for demonstration purposes; it skips the rating period and provides a new rating on each manual entry.
