/*****************************************************************************
 * Copyright (C) 2009   this file is part of the NPTool Project              *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: N. de Sereville  contact address: deserevi@ipno.in2p3.fr *
 *                                                                           *
 * Creation Date  : 15/07/09                                                 *
 * Last update    : 12/10/09                                                 *
 *---------------------------------------------------------------------------*
 * Decription: Define a module of annular shape for the Hyde tracker      *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *    + 12/10/09: Change scorer scheme (N. de Sereville)                     *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

// C++ headers
#include <sstream>
#include <string>
#include <cmath>

// G4 Geometry headers
#include "G4Box.hh"
#include "G4Tubs.hh"

// G4 various headers
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"
#include "G4PVDivision.hh"

// G4 sensitive
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"

// NPTool headers
#include "HydeTrackerAnnular.hh"
#include "GeneralScorers.hh"
#include "HydeScorers.hh"
#include "THydeTrackerData.h"
#include "RootOutput.h"

// CLHEP
#include "CLHEP/Random/RandGauss.h"

using namespace std;
using namespace CLHEP;
using namespace HYDANNULAR;

using namespace HYDSCORERS;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HydeTrackerAnnular::HydeTrackerAnnular()
{
   ms_InterCoord = 0;
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HydeTrackerAnnular::~HydeTrackerAnnular()
{
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void HydeTrackerAnnular::AddModule(G4double PosZ,
                                      G4double Rmin,
                                      G4double Rmax,
                                      bool wFirstStage,
                                      bool wSecondStage,
                                      bool wThirdStage)
{
   m_PosZ.push_back(PosZ);
   m_Rmin.push_back(Rmin);
   m_Rmax.push_back(Rmax);

   m_wFirstStage.push_back(wFirstStage)   ;
   m_wSecondStage.push_back(wSecondStage) ;
   m_wThirdStage.push_back(wThirdStage)   ;
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void HydeTrackerAnnular::VolumeMaker(G4int TelescopeNumber   ,
                                        G4ThreeVector MMpos     ,
                                        G4RotationMatrix* MMrot ,
                                        bool wFirstStage                ,
                                        bool wSecondStage              ,
                                        bool wThirdStage               ,
                                        G4LogicalVolume* world)
{
   G4double NbrTelescopes = TelescopeNumber  ;
   G4String DetectorNumber                   ;
   ostringstream Number                      ;
   Number << NbrTelescopes                   ;
   DetectorNumber = Number.str()             ;

   ////////////////////////////////////////////////////////////////
   /////////////////Element  Definition ///////////////////////////
   ////////////////////////////////////////////////////////////////
   G4String symbol                      ;
   G4double density = 0. , a = 0, z = 0 ;
   G4int ncomponents = 0, natoms = 0    ;

   G4Element* H   = new G4Element("Hydrogen" , symbol = "H"  , z = 1  , a = 1.01   * g / mole);
   G4Element* C   = new G4Element("Carbon"   , symbol = "C"  , z = 6  , a = 12.011 * g / mole);
   G4Element* N   = new G4Element("Nitrogen" , symbol = "N"  , z = 7  , a = 14.01  * g / mole);
   G4Element* O   = new G4Element("Oxigen"   , symbol = "O"  , z = 8  , a = 16.00  * g / mole);
   G4Element* I   = new G4Element("Iode"     , symbol = "I"  , z = 53 , a = 126.9  * g / mole);
   G4Element* Cs  = new G4Element("Cesium"   , symbol = "Cs" , z = 55 , a = 132.9  * g / mole);

   G4Element* Co  = new G4Element("Cobalt"  , symbol = "Co" , z = 27 , a = 58.933 * g / mole);
   G4Element* Cr  = new G4Element("Cromium"  , symbol = "Cr" , z = 24 , a = 51.996 * g / mole);
   G4Element* Ni  = new G4Element("Nickel"   , symbol = "Ni" , z = 28 , a = 58.69  * g / mole);
   G4Element* Fe  = new G4Element("Iron"     , symbol = "Fe" , z = 26 , a = 55.847 * g / mole);
   G4Element* W   = new G4Element("Tungsten" , symbol = "W"  , z = 74 , a = 183.5  * g / mole);

   ////////////////////////////////////////////////////////////////
   /////////////////Material Definition ///////////////////////////
   ////////////////////////////////////////////////////////////////
   // Si
   a = 28.0855 * g / mole;
   density = 2.321 * g / cm3;
   G4Material* Silicon = new G4Material("Si", z = 14., a, density);

   // Al
//   density = 2.702 * g / cm3;
//   a = 26.98 * g / mole;
//   G4Material* Aluminium = new G4Material("Aluminium", z = 13., a, density);

   // Iron
//   density = 7.874 * g / cm3;
//   a = 55.847 * g / mole;
//   G4Material* Iron = new G4Material("Iron", z = 26., a, density);

   // CsI
   density = 4.51 * g / cm3;
   G4Material* CsI = new G4Material("CsI", density, ncomponents = 2);
   CsI->AddElement(Cs , natoms = 1);
   CsI->AddElement(I  , natoms = 1);

   //  Vacuum
   density = 0.000000001 * mg / cm3;
   G4Material* Vacuum = new G4Material("Vacuum", density, ncomponents = 2);
   Vacuum->AddElement(N, .7);
   Vacuum->AddElement(O, .3);

   //  Mylar
   density = 1.397 * g / cm3;
   G4Material* Myl = new G4Material("Mylar", density, ncomponents = 3);
   Myl->AddElement(C, natoms = 10);
   Myl->AddElement(H, natoms = 8);
   Myl->AddElement(O, natoms = 4);

   // Havar
   G4Material* Harvar = new G4Material("Havar", 8.3*g / cm3, 5);
   Harvar->AddElement(Co , 42);
   Harvar->AddElement(Cr , 20);
   Harvar->AddElement(Ni , 13);
   Harvar->AddElement(Fe , 19);
   Harvar->AddElement(W  ,  1);

   ////////////////////////////////////////////////////////////////
   ////////////// Starting Volume Definition //////////////////////
   ////////////////////////////////////////////////////////////////
   // Little trick to avoid warning in compilation: Use a PVPlacement "buffer".
   // If don't you will have a Warning unused variable 'myPVP'
   G4PVPlacement* PVPBuffer ;

   // Name of the module
   G4String Name = "HYDAnnular" + DetectorNumber;

   // Definition of the volume containing the sensitive detector
   G4Tubs* solidMM = new G4Tubs(Name, 
                                Rmin,
                                Rmax,
                                Length/2,
                                0*deg, 
                                360*deg);

//   G4LogicalVolume* logicMM = new G4LogicalVolume(solidMM, Iron, Name, 0, 0, 0);
   G4LogicalVolume* logicMM = new G4LogicalVolume(solidMM, Vacuum, Name, 0, 0, 0);

   PVPBuffer     = new G4PVPlacement(G4Transform3D(*MMrot, MMpos) ,
                                     logicMM                      ,
                                     Name                         ,
                                     world                        ,
                                     false                        ,
                                     0);

   logicMM->SetVisAttributes(G4VisAttributes::Invisible);
   if (m_non_sensitive_part_visiualisation) logicMM->SetVisAttributes(G4VisAttributes(G4Colour(0.90, 0.90, 0.90)));

   // Definition of a vaccuum volume
   G4ThreeVector positionVacBox = G4ThreeVector(0, 0, VacBox_PosZ);

   G4Tubs* solidVacBox = new G4Tubs("solidVacBox", 
                                    FirstStageRmin,
                                    FirstStageRmax,
                                    VacBoxThickness/2,
                                    0*deg, 
                                    360*deg); 

   G4LogicalVolume* logicVacBox = new G4LogicalVolume(solidVacBox, Vacuum, "logicVacBox", 0, 0, 0);

   PVPBuffer = new G4PVPlacement(0, positionVacBox, logicVacBox, "G" + DetectorNumber + "VacBox", logicMM, false, 0);

   logicVacBox->SetVisAttributes(G4VisAttributes::Invisible);

   // Add a degrader plate between Si and CsI:
   /*
      G4Box* Degrader = new G4Box("Degrader" , 50*mm , 50*mm , 0.1*mm );
      G4LogicalVolume* logicDegrader = new G4LogicalVolume( Degrader , Harvar, "logicDegrader",0,0,0);
      PVPBuffer = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicDegrader,"Degrader",logicVacBox,false,0) ;
   */

   //Place two marker to identify the u and v axis on silicon face:
   //marker are placed a bit before the silicon itself so they don't perturbate simulation
   //Uncomment to help debugging or if you want to understand the way the code work.
   //I should recommand to Comment it during simulation to avoid perturbation of simulation
   //Remember G4 is limitationg step on geometry constraints.
  /* 
         G4ThreeVector positionMarkerU = CT*0.98 + MMu*SiliconFace/4;
         G4Box*          solidMarkerU = new G4Box( "solidMarkerU" , SiliconFace/4 , 1*mm , 1*mm )              ;
         G4LogicalVolume* logicMarkerU = new G4LogicalVolume( solidMarkerU , Vacuum , "logicMarkerU",0,0,0)       ;
         PVPBuffer = new G4PVPlacement(G4Transform3D(*MMrot,positionMarkerU),logicMarkerU,"MarkerU",world,false,0) ;

         G4VisAttributes* MarkerUVisAtt= new G4VisAttributes(G4Colour(0.,0.,0.5));//blue
         logicMarkerU->SetVisAttributes(MarkerUVisAtt);

         G4ThreeVector positionMarkerV = CT*0.98 + MMv*SiliconFace/4;
         G4Box*          solidMarkerV = new G4Box( "solidMarkerU" , 1*mm , SiliconFace/4 , 1*mm )              ;
         G4LogicalVolume* logicMarkerV = new G4LogicalVolume( solidMarkerV , Vacuum , "logicMarkerV",0,0,0)       ;
         PVPBuffer = new G4PVPlacement(G4Transform3D(*MMrot,positionMarkerV),logicMarkerV,"MarkerV",world,false,0) ;

         G4VisAttributes* MarkerVVisAtt= new G4VisAttributes(G4Colour(0.,0.5,0.5));//green
         logicMarkerV->SetVisAttributes(MarkerVVisAtt);
   */

   ////////////////////////////////////////////////////////////////
   /////////////////// First Stage Construction////////////////////
   ////////////////////////////////////////////////////////////////
   if (wFirstStage) {
      // Aluminium dead layers
      G4ThreeVector positionAluStripFront = G4ThreeVector(0, 0, AluStripFront_PosZ);
      G4ThreeVector positionAluStripBack  = G4ThreeVector(0, 0, AluStripBack_PosZ);

      G4Tubs* solidAluStrip = new G4Tubs("AluBox", 
                                         FirstStageRmin,
                                         FirstStageRmax,
                                         AluStripThickness/2, 
                                         0*deg, 
                                         360*deg); 

//      G4LogicalVolume* logicAluStrip = new G4LogicalVolume(solidAluStrip, Aluminium, "logicAluStrip", 0, 0, 0);
      G4LogicalVolume* logicAluStrip = new G4LogicalVolume(solidAluStrip, Vacuum, "logicAluStrip", 0, 0, 0);

      PVPBuffer = new G4PVPlacement(0, positionAluStripFront, logicAluStrip, "G" + DetectorNumber + "AluStripFront", logicMM, false, 0);
      PVPBuffer = new G4PVPlacement(0, positionAluStripBack,  logicAluStrip, "G" + DetectorNumber + "AluStripBack",  logicMM, false, 0);

      logicAluStrip->SetVisAttributes(G4VisAttributes::Invisible);

      // Silicon detector itself
      G4ThreeVector  positionSilicon = G4ThreeVector(0, 0, Silicon_PosZ);

      G4Tubs* solidSilicon = new G4Tubs("solidSilicon", 
                                         FirstStageRmin,
                                         FirstStageRmax,
                                         FirstStageThickness/2, 
                                         0*deg, 
                                         360*deg); 
      G4LogicalVolume* logicSilicon = new G4LogicalVolume(solidSilicon, Silicon, "logicSilicon", 0, 0, 0);

      PVPBuffer = new G4PVPlacement(0, positionSilicon, logicSilicon, Name + "_Silicon", logicMM, false, 0);

      // Set First Stage sensible
      logicSilicon->SetSensitiveDetector(m_FirstStageScorer);

      ///Visualisation of Silicon Strip
//      G4VisAttributes* SiliconVisAtt = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5)) ;
      G4VisAttributes* SiliconVisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 0.9)) ;
      logicSilicon->SetVisAttributes(SiliconVisAtt)                        ;
   }

   ////////////////////////////////////////////////////////////////
   //////////////// Second Stage  Construction ////////////////////
   ////////////////////////////////////////////////////////////////
   if (wSecondStage) {
   }

   ////////////////////////////////////////////////////////////////
   ///////////////// Third Stage Construction /////////////////////
   ////////////////////////////////////////////////////////////////
   if (wThirdStage) {
      // Third stage silicon detector
      G4ThreeVector  positionThirdStage = G4ThreeVector(0, 0, ThirdStage_PosZ);

      G4Tubs* solidThirdStage = new G4Tubs("solidThirdStage", 
                                            FirstStageRmin,
                                            FirstStageRmax,
                                            ThirdStageThickness/2, 
                                            0*deg, 
                                            360*deg); 

      G4LogicalVolume* logicThirdStage = new G4LogicalVolume(solidThirdStage, Silicon, "logicThirdStage", 0, 0, 0);

      PVPBuffer = new G4PVPlacement(0, positionThirdStage, logicThirdStage, Name + "_ThirdStage", logicMM, false, 0);

      ///Visualisation of Third Stage
      G4VisAttributes* ThirdStageVisAtt = new G4VisAttributes(G4Colour(0.0, 0.9, 0.)) ;
      logicThirdStage->SetVisAttributes(ThirdStageVisAtt)                        ;
//      logicThirdStage->SetVisAttributes(G4VisAttributes::Invisible);

      // Set Third Stage sensible
      logicThirdStage->SetSensitiveDetector(m_ThirdStageScorer);
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Virtual Method of VDetector class

// Read stream at Configfile to pick-up parameters of detector (Position,...)
// Called in DetecorConstruction::ReadDetextorConfiguration Method
void HydeTrackerAnnular::ReadConfiguration(string Path)
{
   ifstream ConfigFile           ;
   ConfigFile.open(Path.c_str()) ;
   string LineBuffer          ;
   string DataBuffer          ;

   G4double Z = 0, Rmin = 0, Rmax = 0;
   int FIRSTSTAGE = 0 , SECONDSTAGE = 0 , THIRDSTAGE = 0;


   bool ReadingStatus = false ;
   bool check_Z    = false ;
   bool check_Rmin = false ;
   bool check_Rmax = false ;
   bool check_FirstStage = false ;
   bool check_SecondStage = false ;
   bool check_ThirdStage = false ;

   while (!ConfigFile.eof()) {
      getline(ConfigFile, LineBuffer);
      if (LineBuffer.compare(0, 10, "HYDAnnular") == 0) {
         G4cout << "///" << G4endl           ;
         G4cout << "Annular element found: " << G4endl   ;
         ReadingStatus = true ;}
      
   while(ReadingStatus){

               ConfigFile >> DataBuffer;      
      //   Comment Line 
      if (DataBuffer.compare(0, 1, "%") == 0) {/*do nothing */;}
      

         
         //Position method
         else if (DataBuffer.compare(0, 2, "Z=") == 0) {
            check_Z = true;
            ConfigFile >> DataBuffer ;
            Z = atof(DataBuffer.c_str()) ;
            Z = Z * mm;
            cout << "Z:  " << Z / mm << endl;
         }

         //Position method
         else if (DataBuffer.compare(0, 5, "RMIN=") == 0) {
            check_Rmin = true;
            ConfigFile >> DataBuffer ;
            Rmin = atof(DataBuffer.c_str()) ;
            Rmin = Rmin * mm;
            cout << "Rmin:  " << Rmin / mm << endl;
         }

         //Position method
         else if (DataBuffer.compare(0, 5, "RMAX=") == 0) {
            check_Rmax = true;
            ConfigFile >> DataBuffer ;
            Rmax = atof(DataBuffer.c_str()) ;
            Rmax = Rmax * mm;
            cout << "Rmax:  " << Rmax / mm << endl;
         }

         else if (DataBuffer.compare(0, 11, "FIRSTSTAGE=") == 0) {
            check_FirstStage = true ;
            ConfigFile >> DataBuffer;
            FIRSTSTAGE = atof(DataBuffer.c_str()) ;
         }

         else if (DataBuffer.compare(0, 12, "SECONDSTAGE=") == 0) {
            check_SecondStage = true ;
            ConfigFile >> DataBuffer;
            SECONDSTAGE = atof(DataBuffer.c_str()) ;
         }

         else if (DataBuffer.compare(0, 11, "THIRDSTAGE=") == 0) {
            check_ThirdStage = true ;
            ConfigFile >> DataBuffer;
            THIRDSTAGE = atof(DataBuffer.c_str()) ;
         }

         else if (DataBuffer.compare(0, 4, "VIS=") == 0) {
            ConfigFile >> DataBuffer;
            if (DataBuffer.compare(0, 3, "all") == 0) m_non_sensitive_part_visiualisation = true;
         }

      else {
        G4cout << "WARNIG: Wrong Token Sequence, HydeTrackerAnnular: Annular Element not added" << G4endl;
           }

         if (check_Z && check_Rmin && check_Rmax && check_FirstStage && check_SecondStage && check_ThirdStage) {
            
            ReadingStatus = false ;
             check_Z    = false ;
            check_Rmin = false ;
            check_Rmax = false ;
            check_FirstStage = false ;
             check_SecondStage = false ;
             check_ThirdStage = false ;
            
            AddModule(Z,
                      Rmin,
                      Rmax,
                      FIRSTSTAGE  == 1 ,
                      SECONDSTAGE == 1 ,
                      THIRDSTAGE  == 1);
         }
         
         
      }
   }
}

// Construct detector and inialise sensitive part.
// Called After DetecorConstruction::AddDetector Method
void HydeTrackerAnnular::ConstructDetector(G4LogicalVolume* world)
{
   G4RotationMatrix* MMrot    = NULL                   ;
//   G4ThreeVector     MMpos    = G4ThreeVector(0, 0, 0) ;
   MMpos = G4ThreeVector(0, 0, 0) ;

   bool FirstStage  = true ;
   bool SecondStage = true ;
   bool ThirdStage  = true ;

   G4int NumberOfModule = m_PosZ.size() ;

   for (G4int i = 0; i < NumberOfModule; i++) {
      // translation to position the module
      MMpos = G4ThreeVector(0, 0, m_PosZ[i]);

      // Passage Matrix from Lab Referential to Module Referential
      // Identity matrix by default
      MMrot = new G4RotationMatrix();
      if (MMpos.z() < 0) MMrot->rotateX(180*deg);

      // Presence of 1st, 2nd and 3rd stage
      FirstStage  = m_wFirstStage[i]  ;
      SecondStage = m_wSecondStage[i] ;
      ThirdStage  = m_wThirdStage[i]  ;

      // Build geometry and declare scorers
      VolumeMaker(i + 1, MMpos, MMrot, FirstStage, SecondStage, ThirdStage , world);
   }

   delete MMrot ;
}



// Connect the HydeTrackingData class to the output TTree
// of the simulation
void HydeTrackerAnnular::InitializeRootOutput()
{
}



// Set the TinteractionCoordinates object from VDetector to the present class
void HydeTrackerAnnular::SetInterCoordPointer(TInteractionCoordinates* interCoord)
{
   ms_InterCoord = interCoord;
}



// Read sensitive part and fill the Root tree.
// Called at in the EventAction::EndOfEventAvtion
void HydeTrackerAnnular::ReadSensitive(const G4Event* event)
{
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Used to Read Event Map of detector //////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
   // First Stage
   std::map<G4int, G4int*>::iterator    DetectorNumber_itr;
   std::map<G4int, G4double*>::iterator Energy_itr;
   std::map<G4int, G4double*>::iterator Time_itr;
   std::map<G4int, G4double*>::iterator X_itr;
   std::map<G4int, G4double*>::iterator Y_itr;
   std::map<G4int, G4double*>::iterator Pos_X_itr;
   std::map<G4int, G4double*>::iterator Pos_Y_itr;
   std::map<G4int, G4double*>::iterator Pos_Z_itr;
   std::map<G4int, G4double*>::iterator Ang_Theta_itr;
   std::map<G4int, G4double*>::iterator Ang_Phi_itr;

   G4THitsMap<G4int>*    DetectorNumberHitMap;
   G4THitsMap<G4double>* EnergyHitMap;
   G4THitsMap<G4double>* TimeHitMap;
   G4THitsMap<G4double>* XHitMap;
   G4THitsMap<G4double>* YHitMap;
   G4THitsMap<G4double>* PosXHitMap;
   G4THitsMap<G4double>* PosYHitMap;
   G4THitsMap<G4double>* PosZHitMap;
   G4THitsMap<G4double>* AngThetaHitMap;
   G4THitsMap<G4double>* AngPhiHitMap;

   // NULL pointer are given to avoid warning at compilation
   // Third Stage
   std::map<G4int, G4double*>::iterator ThirdStageEnergy_itr;
   G4THitsMap<G4double>* ThirdStageEnergyHitMap = NULL;


   // Read the Scorer associated to the first Stage
   //Detector Number
   G4int StripDetCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("FirstStageScorerHYDAnnular/DetectorNumber")    ;
   DetectorNumberHitMap = (G4THitsMap<G4int>*)(event->GetHCofThisEvent()->GetHC(StripDetCollectionID))         ;
   DetectorNumber_itr =  DetectorNumberHitMap->GetMap()->begin()                                               ;

   //Energy
   G4int StripEnergyCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("FirstStageScorerHYDAnnular/StripEnergy")   ;
   EnergyHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(StripEnergyCollectionID))                    ;
   Energy_itr = EnergyHitMap->GetMap()->begin()                                                          ;

   //Time of Flight
   G4int StripTimeCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("FirstStageScorerHYDAnnular/StripTime")    ;
   TimeHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(StripTimeCollectionID))                        ;
   Time_itr = TimeHitMap->GetMap()->begin()                                                              ;

   //Strip Number X
   G4int StripXCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("FirstStageScorerHYDAnnular/StripNumberX")    ;
   XHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(StripXCollectionID))                              ;
   X_itr = XHitMap->GetMap()->begin()                                                                    ;

   //Strip Number Y
   G4int StripYCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("FirstStageScorerHYDAnnular/StripNumberY")    ;
   YHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(StripYCollectionID))                              ;
   Y_itr = YHitMap->GetMap()->begin()                                                                    ;

   //Interaction Coordinate X
   G4int InterCoordXCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("FirstStageScorerHYDAnnular/InterCoordX")    ;
   PosXHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordXCollectionID))                              ;
   Pos_X_itr = PosXHitMap->GetMap()->begin()                                                                    ;

   //Interaction Coordinate Y
   G4int InterCoordYCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("FirstStageScorerHYDAnnular/InterCoordY")    ;
   PosYHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordYCollectionID))                              ;
   Pos_Y_itr = PosYHitMap->GetMap()->begin()                                                                    ;

   //Interaction Coordinate Z
   G4int InterCoordZCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("FirstStageScorerHYDAnnular/InterCoordZ")    ;
   PosZHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordZCollectionID))                              ;
   Pos_Z_itr = PosXHitMap->GetMap()->begin()                                                                    ;

   //Interaction Coordinate Angle Theta
   G4int InterCoordAngThetaCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("FirstStageScorerHYDAnnular/InterCoordAngTheta")    ;
   AngThetaHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordAngThetaCollectionID))                              ;
   Ang_Theta_itr = AngThetaHitMap->GetMap()->begin()                                                                    ;

   //Interaction Coordinate Angle Phi
   G4int InterCoordAngPhiCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("FirstStageScorerHYDAnnular/InterCoordAngPhi")    ;
   AngPhiHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordAngPhiCollectionID))                              ;
   Ang_Phi_itr = AngPhiHitMap->GetMap()->begin()                                                                    ;

   // Read the Scorer associated to the Third Stage 
   //Energy
   G4int ThirdStageEnergyCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("ThirdStageScorerHYDAnnular/ThirdStageEnergy")      ;
   ThirdStageEnergyHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(ThirdStageEnergyCollectionID))                      ;
   ThirdStageEnergy_itr = ThirdStageEnergyHitMap->GetMap()->begin()                                                       ;

   // Check the size of different map
   G4int sizeN = DetectorNumberHitMap->entries();
   G4int sizeE = EnergyHitMap->entries();
   G4int sizeT = TimeHitMap->entries();
   G4int sizeX = XHitMap->entries();
   G4int sizeY = YHitMap->entries();

   if (sizeE != sizeT || sizeT != sizeX || sizeX != sizeY) {
      G4cout << "No match size Si Event Map: sE:"
      << sizeE << " sT:" << sizeT << " sX:" << sizeX << " sY:" << sizeY << endl ;
      return;
   }

   // Loop on FirstStage number
   for (G4int l = 0; l < sizeN; l++) {
      G4double N     = *(DetectorNumber_itr->second);
      G4int NTrackID =   DetectorNumber_itr->first - N;

      if (N > 0) {
         // Fill detector number
         ms_Event->SetHYDTrkFirstStageFrontEDetectorNbr(m_index["Annular"] + N);
         ms_Event->SetHYDTrkFirstStageFrontTDetectorNbr(m_index["Annular"] + N);
         ms_Event->SetHYDTrkFirstStageBackEDetectorNbr(m_index["Annular"] + N);
         ms_Event->SetHYDTrkFirstStageBackTDetectorNbr(m_index["Annular"] + N);

         // Energy
         for (G4int l = 0 ; l < sizeE ; l++) {
            G4int ETrackID  =   Energy_itr->first - N;
            G4double E     = *(Energy_itr->second);
            if (ETrackID == NTrackID) {
               ms_Event->SetHYDTrkFirstStageFrontEEnergy(RandGauss::shoot(E, ResoFirstStage));
               ms_Event->SetHYDTrkFirstStageBackEEnergy(RandGauss::shoot(E, ResoFirstStage));
            }
            Energy_itr++;
         }

            //  Time
            Time_itr = TimeHitMap->GetMap()->begin();
            for (G4int h = 0 ; h < sizeT ; h++) {
               G4int TTrackID  =   Time_itr->first - N;
               G4double T     = *(Time_itr->second)      ;
               if (TTrackID == NTrackID) {
                  T = RandGauss::shoot(T, ResoTimePPAC)   ;
                  ms_Event->SetHYDTrkFirstStageFrontTTime(RandGauss::shoot(T, ResoTimeGpd)) ;
                  ms_Event->SetHYDTrkFirstStageBackTTime(RandGauss::shoot(T, ResoTimeGpd)) ;
               }
               Time_itr++;
            }

            // X
            X_itr = XHitMap->GetMap()->begin();
            for (G4int h = 0 ; h < sizeX ; h++) {
               G4int XTrackID  =   X_itr->first - N;
               G4double X     = *(X_itr->second);
               if (XTrackID == NTrackID) {
                  ms_Event->SetHYDTrkFirstStageFrontEStripNbr(X)   ;
                  ms_Event->SetHYDTrkFirstStageFrontTStripNbr(X)   ;
               }

               X_itr++;
            }

            // Y
            Y_itr = YHitMap->GetMap()->begin()  ;
            for (G4int h = 0 ; h < sizeY ; h++) {
               G4int YTrackID  =   Y_itr->first  - N  ;
               G4double Y     = *(Y_itr->second)      ;
               if (YTrackID == NTrackID) {
                  ms_Event->SetHYDTrkFirstStageBackEStripNbr(Y)   ;
                  ms_Event->SetHYDTrkFirstStageBackTStripNbr(Y)   ;
               }

               Y_itr++;
            }

            // Pos X
            Pos_X_itr = PosXHitMap->GetMap()->begin();
            for (G4int h = 0 ; h < sizeX ; h++) {
               G4int PosXTrackID =   Pos_X_itr->first - N    ;
               G4double PosX     = *(Pos_X_itr->second)      ;
               if (PosXTrackID == NTrackID) {
                  ms_InterCoord->SetDetectedPositionX(PosX) ;
               }
               Pos_X_itr++;
            }

            // Pos Y
            Pos_Y_itr = PosYHitMap->GetMap()->begin();
            for (G4int h = 0 ; h < sizeX ; h++) {
               G4int PosYTrackID =   Pos_Y_itr->first - N    ;
               G4double PosY     = *(Pos_Y_itr->second)      ;
               if (PosYTrackID == NTrackID) {
                  ms_InterCoord->SetDetectedPositionY(PosY) ;
               }
               Pos_Y_itr++;
            }

            // Pos Z
            Pos_Z_itr = PosZHitMap->GetMap()->begin();
            for (G4int h = 0 ; h < sizeX ; h++) {
               G4int PosZTrackID =   Pos_Z_itr->first - N    ;
               G4double PosZ     = *(Pos_Z_itr->second)      ;
               if (PosZTrackID == NTrackID) {
                  ms_InterCoord->SetDetectedPositionZ(PosZ) ;
               }
               Pos_Z_itr++;
            }

            // Angle Theta
            Ang_Theta_itr = AngThetaHitMap->GetMap()->begin();
            for (G4int h = 0 ; h < sizeX ; h++) {
               G4int AngThetaTrackID =   Ang_Theta_itr->first - N    ;
               G4double AngTheta     = *(Ang_Theta_itr->second)      ;
               if (AngThetaTrackID == NTrackID) {
                  ms_InterCoord->SetDetectedAngleTheta(AngTheta) ;
               }
               Ang_Theta_itr++;
            }

            // Angle Phi
            Ang_Phi_itr = AngPhiHitMap->GetMap()->begin();
            for (G4int h = 0 ; h < sizeX ; h++) {
               G4int AngPhiTrackID =   Ang_Phi_itr->first - N;
               G4double AngPhi     = *(Ang_Phi_itr->second);
               if (AngPhiTrackID == NTrackID) {
                  ms_InterCoord->SetDetectedAnglePhi(AngPhi);
               }
               Ang_Phi_itr++;
            }

            // Second Stage

            // Third Stage
               ThirdStageEnergy_itr = ThirdStageEnergyHitMap->GetMap()->begin()  ;
               for (G4int h = 0 ; h < ThirdStageEnergyHitMap->entries() ; h++) {
                  G4int ThirdStageEnergyTrackID  =   ThirdStageEnergy_itr->first - N;
                  G4double ThirdStageEnergy      = *(ThirdStageEnergy_itr->second);

                  if (ThirdStageEnergyTrackID == NTrackID) {
                     ms_Event->SetHYDTrkThirdStageEEnergy(RandGauss::shoot(ThirdStageEnergy, ResoThirdStage));
                     ms_Event->SetHYDTrkThirdStageEPadNbr(1);
                     ms_Event->SetHYDTrkThirdStageTPadNbr(1);
                     ms_Event->SetHYDTrkThirdStageTTime(1);
                     ms_Event->SetHYDTrkThirdStageTDetectorNbr(m_index["Annular"] + N);
                     ms_Event->SetHYDTrkThirdStageEDetectorNbr(m_index["Annular"] + N);
                  }

                  ThirdStageEnergy_itr++;
               }

         DetectorNumber_itr++;
      }

      // clear map for next event
      DetectorNumberHitMap ->clear();
      EnergyHitMap   ->clear();
      TimeHitMap     ->clear();
      XHitMap        ->clear();
      YHitMap        ->clear();
      PosXHitMap     ->clear();
      PosYHitMap     ->clear();
      PosZHitMap     ->clear();
      AngThetaHitMap ->clear();
      AngPhiHitMap   ->clear();
      ThirdStageEnergyHitMap ->clear();
   }
}



void HydeTrackerAnnular::InitializeScorers()
{
   // First stage Associate Scorer
   m_FirstStageScorer = new G4MultiFunctionalDetector("FirstStageScorerHYDAnnular");
   G4VPrimitiveScorer* DetNbr                           = new GENERALSCORERS::PSDetectorNumber("DetectorNumber", "HYDAnnular", 0);
   G4VPrimitiveScorer* TOF                              = new GENERALSCORERS::PSTOF("StripTime","HYDAnnular", 0);
   G4VPrimitiveScorer* InteractionCoordinatesX          = new GENERALSCORERS::PSInteractionCoordinatesX("InterCoordX","HYDAnnular", 0);
   G4VPrimitiveScorer* InteractionCoordinatesY          = new GENERALSCORERS::PSInteractionCoordinatesY("InterCoordY","HYDAnnular", 0);
   G4VPrimitiveScorer* InteractionCoordinatesZ          = new GENERALSCORERS::PSInteractionCoordinatesZ("InterCoordZ","HYDAnnular", 0);
   G4VPrimitiveScorer* InteractionCoordinatesAngleTheta = new GENERALSCORERS::PSInteractionCoordinatesAngleTheta("InterCoordAngTheta","HYDAnnular", 0);
   G4VPrimitiveScorer* InteractionCoordinatesAnglePhi   = new GENERALSCORERS::PSInteractionCoordinatesAnglePhi("InterCoordAngPhi","HYDAnnular", 0);
   G4VPrimitiveScorer* Energy                           = new HYDScorerFirstStageEnergy("StripEnergy", "HYDAnnular", 0);
   G4VPrimitiveScorer* StripPositionX                   = new HYDScorerFirstStageFrontStripAnnular("StripNumberX", 0, FirstStageRmax, 128);
   G4VPrimitiveScorer* StripPositionY                   = new HYDScorerFirstStageBackStripAnnular("StripNumberY", 0, FirstStageRmax, 128);

   //and register it to the multifunctionnal detector
   m_FirstStageScorer->RegisterPrimitive(DetNbr);
   m_FirstStageScorer->RegisterPrimitive(Energy);
   m_FirstStageScorer->RegisterPrimitive(TOF);
   m_FirstStageScorer->RegisterPrimitive(StripPositionX);
   m_FirstStageScorer->RegisterPrimitive(StripPositionY);
   m_FirstStageScorer->RegisterPrimitive(InteractionCoordinatesX);
   m_FirstStageScorer->RegisterPrimitive(InteractionCoordinatesY);
   m_FirstStageScorer->RegisterPrimitive(InteractionCoordinatesZ);
   m_FirstStageScorer->RegisterPrimitive(InteractionCoordinatesAngleTheta);
   m_FirstStageScorer->RegisterPrimitive(InteractionCoordinatesAnglePhi);

   // Second stage Associate Scorer
   m_SecondStageScorer = new G4MultiFunctionalDetector("SecondStageScorerHYDAnnular");
   G4VPrimitiveScorer* SecondStageEnergy = new HYDScorerSecondStageEnergy("SecondStageEnergy", "HYDAnnular", 0);
   m_SecondStageScorer->RegisterPrimitive(SecondStageEnergy);

   //  Third stage Associate Scorer 
   m_ThirdStageScorer = new G4MultiFunctionalDetector("ThirdStageScorerHYDAnnular");
   G4VPrimitiveScorer* ThirdStageEnergy = new HYDScorerThirdStageEnergy("ThirdStageEnergy", "HYDAnnular", 0);
   m_ThirdStageScorer->RegisterPrimitive(ThirdStageEnergy);

   //  Add All Scorer to the Global Scorer Manager
   G4SDManager::GetSDMpointer()->AddNewDetector(m_FirstStageScorer);
   G4SDManager::GetSDMpointer()->AddNewDetector(m_SecondStageScorer);
   G4SDManager::GetSDMpointer()->AddNewDetector(m_ThirdStageScorer);
}