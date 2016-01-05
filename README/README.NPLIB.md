README file for the NPLib part of NPTool
========================================

Contents
--------
Directory $NPTOOL/NPLib/ :

- __Core__         - directory containing the NPTool core libraries
- __Physics__      - directory containing physics tools 
- __Detectors__    - directory containing all the detectors
- __Calibration__  - directory containing detector calibration tools
- __Online__       - directory containing the NPTool Online module



### Detectors
This directory contains a list of sub directories, each holding the
files to describe a detector. Usually a detector is defined by three
C++ classes:

- __TdetectorData__: this class stores the raw data associated to `detector`.
The raw data can either be generated by a nuclear physics experiment or 
by the _npsimulation_ utility.
- __TdetectorPhysics__: this class performs the physical analysis of the
`detector`, e.g. remove bad channels, apply calibrations, ...
- __TdetectorSpectra__: this class defines and fill spectra associated to
`detector`, e.g. multiplicity spectrum, hit pattern, ...

Note that the  __TdetectorSpectra__ class is optional. If you plan to
include a new detector from scratch, consider using the `nptool-wizard`
facility which will create default files.