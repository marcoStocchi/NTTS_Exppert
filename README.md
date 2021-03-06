# Exppert
#### An open-source tool for experimenting with noise based perturbation schemes
#### (F. Ricciato, M. Stocchi, F. Bach, A. Bujnowska, W. Kloek)

Copyright (c) European Commission, 2021.



#### Platform Requirements

The present makefile system has been tested on Red Hat Enterprise (R) Linux 6.1 (Santiago) and on Fedora 31 Server.

#### Dependencies

- GCC (ver. >= 4.4.7)
- OpenMP (libGomp) (ver. >= 3.0)
- Boost C++ libraries (ver. >= 1.69)
- OpenSSL (libSSL, libCrypto) (ver. >= 1.0.1)
- libSSH (ver. >= 4.2.3)
- R (libR) (ver. >= 3.5.1)
- R package Ptable (https://github.com/sdcTools/ptable)
- input dataset (https://github.com/eurostat/confly/blob/master/data/hc_9_2_synth.csv)

#### build from sources
Install all the dependencies, then 

```sh
$ git clone https://github.com/marcoStocchi/NTTS_Exppert.git exppert
$ cd exppert/
$ make test
$ make
```

The target "make test" should help you diagnose the paths used by the build.
To change them, manually modify the file "make_path" and/or "makefile".
