/*****************************************************************************
 * Copyright (C) 2009   this file is part of the NPTool Project              *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: Adrien MATTA  contact address: matta@ipno.in2p3.fr       *
 *                                                                           *
 * Creation Date  : January 2009                                             *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *  This class describe a 20um Silicium detector                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *****************************************************************************/

// C++ headers
#include <sstream>
#include <cmath>

//G4 Geometry object
#include "G4Tubs.hh"

//G4 sensitive
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"

//G4 various object
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

// NPTool header
#include "Plastic.hh"
#include "PlasticScorers.hh"
#include "RootOutput.h"

// CLHEP header
#include "CLHEP/Random/RandGauss.h"

using namespace std;
using namespace CLHEP;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace PLASTIC
{
   // Energy and time Resolution
   const G4double ResoTime    = 4.2      	;// = 10ns of Resolution   //   Unit is MeV/2.35
   const G4double ResoEnergy  = 0.42   		;// = 1MeV of Resolution   //   Unit is MeV/2.35

}

using namespace PLASTIC ;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Plastic Specific Method
Plastic::Plastic()
{
	InitializeMaterial();
	m_Event = new TPlasticData() ;
}

Plastic::~Plastic()
{
	delete m_MaterialPlastic	; 
	delete m_MaterialLead		;
	delete m_PlasticScorer		;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void Plastic::AddPlastic(	G4double    R        			,
        					G4double    Theta    			,
         			 		G4double    Phi         		,
         			 		G4double	 PlasticThickness	,
         			 		G4double	 PlasticRadius		,
         			 		G4double 	LeadThickness		)
{

   m_R.push_back(R)              		;
   m_Theta.push_back(Theta)         	;
   m_Phi.push_back(Phi)          		;
   m_PlasticThickness.push_back(PlasticThickness)					;
 	m_PlasticRadius.push_back(PlasticRadius)						;
 	m_LeadThickness.push_back(LeadThickness);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Virtual Method of VDetector class


// Read stream at Configfile to pick-up parameters of detector (Position,...)
// Called in DetecorConstruction::ReadDetextorConfiguration Method
void Plastic::ReadConfiguration(string Path)
{
   ifstream ConfigFile           ;
   ConfigFile.open(Path.c_str()) ;
   string LineBuffer          ;
   string DataBuffer          ;

   G4double Theta = 0 , Phi = 0 , R = 0 , Thickness = 0 , Radius = 0 , LeadThickness = 0;

	bool check_Theta = false   ;
	bool check_Phi  = false  ;
	bool check_R     = false   ;
	bool check_Thickness = false  		;
	bool check_Radius = false  			;
	bool check_LeadThickness = false		;
	bool ReadingStatus = false ;
	

 while (!ConfigFile.eof()) 
 	{
      
	      	getline(ConfigFile, LineBuffer);

			//	If line is a Start Up MUST2 bloc, Reading toggle to true      
	      	if (LineBuffer.compare(0, 7, "Plastic") == 0) 
		      	{
		        	 G4cout << "///" << G4endl           ;
		       		  G4cout << "Platic found: " << G4endl   ;        
		        	 ReadingStatus = true ;
		        	
			   	}
			
			//	Else don't toggle to Reading Block Status
			else ReadingStatus = false ;
			
			//	Reading Block
			while(ReadingStatus)
				{
						// Pickup Next Word 
					ConfigFile >> DataBuffer ;

					//	Comment Line 
					if (DataBuffer.compare(0, 1, "%") == 0) {/*do nothing */;}

						//	Finding another telescope (safety), toggle out
					else if (DataBuffer.compare(0, 6, "Plastic") == 0) {
						cout << "WARNING: Another Telescope is find before standard sequence of Token, Error may occured in Telecope definition" << endl ;
						ReadingStatus = false ;
					}
										
					//Angle method
					else if (DataBuffer.compare(0, 6, "THETA=") == 0) {
						check_Theta = true;
						ConfigFile >> DataBuffer ;
						Theta = atof(DataBuffer.c_str()) ;
						Theta = Theta * deg;
						cout << "Theta:  " << Theta / deg << endl;
					}

					else if (DataBuffer.compare(0, 4, "PHI=") == 0) {
						check_Phi = true;
						ConfigFile >> DataBuffer ;
						Phi = atof(DataBuffer.c_str()) ;
						Phi = Phi * deg;
						cout << "Phi:  " << Phi / deg << endl;
					}

					else if (DataBuffer.compare(0, 2, "R=") == 0) {
						check_R = true;
						ConfigFile >> DataBuffer ;
						R = atof(DataBuffer.c_str()) ;
						R = R * mm;
						cout << "R:  " << R/mm << endl;
					}
					
					else if (DataBuffer.compare(0, 7, "Radius=") == 0) {
						check_Radius = true;
						ConfigFile >> DataBuffer ;
						Radius = atof(DataBuffer.c_str()) ;
						Radius = Radius * mm;
						cout << "Plastic Radius:  " << Radius/mm << endl;
					}
					
					else if (DataBuffer.compare(0, 10, "Thickness=") == 0) {
						check_Thickness = true;
						ConfigFile >> DataBuffer ;
						Thickness = atof(DataBuffer.c_str()) ;
						Thickness = Thickness * mm;
						cout << "Plastic Thickness:  " << Thickness/mm << endl;
					}
					
					else if (DataBuffer.compare(0, 14, "LeadThickness=") == 0) {
						check_LeadThickness = true;
						ConfigFile >> DataBuffer ;
						LeadThickness = atof(DataBuffer.c_str()) ;
						LeadThickness = LeadThickness * mm;
						cout << "Lead Thickness :  " << LeadThickness/mm << endl;
					}
			      
			         	///////////////////////////////////////////////////
						//	If no Detector Token and no comment, toggle out
			         else 
			         	{ReadingStatus = false; G4cout << "Wrong Token Sequence: Getting out " << DataBuffer << G4endl ;}
			         
			         	/////////////////////////////////////////////////
			         	//	If All necessary information there, toggle out
			         
			         if ( check_Theta && check_Phi && check_R && check_Thickness && check_Radius && check_LeadThickness) 
			         	{ 
		         		  AddPlastic(	R       	,
		                  				Theta    	,
		                  				Phi   		,
		                  				Thickness	,
		                  				Radius		,
		                  				LeadThickness);
					         
					        //	Reinitialisation of Check Boolean 
					        
							check_Theta = false   			;
							check_Phi  = false  			;
							check_R     = false   			;
							check_Thickness = false  		;
							check_Radius = false  			;
							check_LeadThickness = false		;
							ReadingStatus = false 			;	
							cout << "///"<< endl ;	         
			         	}
			         	
				}
	}

}

// Construct detector and inialise sensitive part.
// Called After DetecorConstruction::AddDetector Method
void Plastic::ConstructDetector(G4LogicalVolume* world)
{
   	G4ThreeVector Det_pos = G4ThreeVector(0, 0, 0)  ;
	
   for (unsigned short i = 0 ; i < m_R.size() ; i++) 
   	{
         G4double wX = m_R[i] * sin(m_Theta[i] ) * cos(m_Phi[i] )   ;
         G4double wY = m_R[i] * sin(m_Theta[i] ) * sin(m_Phi[i] )   ;
         G4double wZ = m_R[i] * cos(m_Theta[i] )             		;

         Det_pos = G4ThreeVector(wX, wY, wZ)                 ;
         G4LogicalVolume* logicPlastic = NULL ;
			
		VolumeMaker(Det_pos , i+1, world) ;
    }

}

void Plastic::VolumeMaker(G4ThreeVector Det_pos, int DetNumber, G4LogicalVolume* world)
	{
		////////////////////////////////////////////////////////////////
		////////////// Starting Volume Definition //////////////////////
		////////////////////////////////////////////////////////////////		
		G4PVPlacement* PVPBuffer ;
		
		// Name of the module
   		std::ostringstream DetectorNumber         			;
   		DetectorNumber << DetNumber                 	  	;
		G4String Name = "Plastic" + DetectorNumber.str()	;
		
		int i = DetNumber-1;

		// Definition of the volume containing the sensitive detector
		if(m_PlasticThickness[i]>0 && m_PlasticRadius[i]>0)
			{ 
				G4Tubs* solidPlastic = new G4Tubs(	Name					, 
			                            			0						,
			                            			m_PlasticRadius[i]		,
			                            			m_PlasticThickness[i]/2	,
			                            			0*deg					, 
			                            			360*deg					);
		                            		
				G4LogicalVolume* logicPlastic = new G4LogicalVolume(solidPlastic, m_MaterialPlastic, Name, 0, 0, 0);
				logicPlastic->SetSensitiveDetector(m_PlasticScorer);
				
				G4VisAttributes* PlastVisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 0.9)) ;
   				logicPlastic->SetVisAttributes(PlastVisAtt) ;
 				
 				
			  
				PVPBuffer = new G4PVPlacement(	0				,
												Det_pos			,
		                                     	logicPlastic    ,
		                                     	Name            ,
		                                     	world           ,
		                                     	false           ,
		                                     	0				);	
		                                     	
		       
		                                     	
			}
		
                                     	
        if(m_LeadThickness[i]>0&& m_PlasticRadius[i]>0)
        	{
    			G4Tubs* solidLead = new G4Tubs(	Name					,	 
		                            			0						,
		                            			m_PlasticRadius[i]		,
		                            			m_LeadThickness[i]/2	,
		                            			0*deg					, 
		                            			360*deg					);
		                            		
				G4LogicalVolume* logicLead = new G4LogicalVolume(solidLead, m_MaterialLead, Name, 0, 0, 0);
				G4VisAttributes* LeadVisAtt = new G4VisAttributes(G4Colour(0.1, 0.1, 0.1)) ;
   				logicLead->SetVisAttributes(LeadVisAtt) ;
   				
				PVPBuffer = new G4PVPlacement(	0																		,
												Det_pos+(m_PlasticThickness[i]/2+m_LeadThickness[i]/2)*Det_pos.unit()	,
		                                     	logicLead    															,
		                                     	Name            														,	
		                                     	world           														,
		                                     	false           														,
		                                     	0																		);
        	
        	}
	}

// Add Detector branch to the EventTree.
// Called After DetecorConstruction::AddDetector Method
void Plastic::InitializeRootOutput()
{
   RootOutput *pAnalysis = RootOutput::getInstance();
   TTree *pTree = pAnalysis->GetTree();
   pTree->Branch("Plastic", "TPlasticData", &m_Event) ;
}

// Read sensitive part and fill the Root tree.
// Called at in the EventAction::EndOfEventAvtion
void Plastic::ReadSensitive(const G4Event* event)
{
   G4String DetectorNumber 	;
   m_Event->Clear()			;
   
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Used to Read Event Map of detector //////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
  
	std::map<G4int, G4int*>::iterator DetectorNumber_itr    ;
	std::map<G4int, G4double*>::iterator Energy_itr     	;
	std::map<G4int, G4double*>::iterator Time_itr    		;
	
	G4THitsMap<G4int>*	  DetectorNumberHitMap				;      
	G4THitsMap<G4double>* EnergyHitMap              		;
	G4THitsMap<G4double>* TimeHitMap             			;
	
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

    // Read the Scorer associate to the Silicon Strip
    
	//Detector Number
	G4int StripDetCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("PlasticScorer/PlasticNumber")  	;
	DetectorNumberHitMap = (G4THitsMap<G4int>*)(event->GetHCofThisEvent()->GetHC(StripDetCollectionID))       	;
	DetectorNumber_itr =  DetectorNumberHitMap->GetMap()->begin()                                               ;

	//Energy
	G4int StripEnergyCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("PlasticScorer/Energy")   	;
	EnergyHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(StripEnergyCollectionID))           ;
	Energy_itr = EnergyHitMap->GetMap()->begin()                                                          		;

	//Time of Flight
	G4int StripTimeCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("PlasticScorer/Time")    		;
	TimeHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(StripTimeCollectionID))               ;
	Time_itr = TimeHitMap->GetMap()->begin()                                                              		;

  	G4int sizeN = DetectorNumberHitMap->entries() 	;
    G4int sizeE = EnergyHitMap->entries() 			;
    G4int sizeT = TimeHitMap->entries() 			;

    // Loop on Plastic Number
    for (G4int l = 0 ; l < sizeN ; l++) {
        G4int N     =      *(DetectorNumber_itr->second)    ;
        G4int NTrackID  =   DetectorNumber_itr->first - N  ;
        
      
        if (N > 0) {
        
	        m_Event->SetPlasticNumber(N) ;

			//  Energy
	        Energy_itr = EnergyHitMap->GetMap()->begin();
	        for (G4int h = 0 ; h < sizeE ; h++) {
	            G4int ETrackID  =   Energy_itr->first  - N      ;
	            G4double E     = *(Energy_itr->second)      	;

	            if (ETrackID == NTrackID) {
	                m_Event->SetEnergy(RandGauss::shoot(E, ResoEnergy))    ;
	            }
	            
	            Energy_itr++;
	        	}


	        //  Time
	        Time_itr = TimeHitMap->GetMap()->begin();
	        for (G4int h = 0 ; h < sizeT ; h++) {
	            G4int TTrackID  =   Time_itr->first   - N    ;
	            G4double T     = *(Time_itr->second)      ;

	            if (TTrackID == NTrackID) {
	                m_Event->SetTime(RandGauss::shoot(T, ResoTime)) ;
	            }
	            
	            Time_itr++;
	        }

        }

        DetectorNumber_itr++;
    }
    
    // clear map for next event
    DetectorNumberHitMap    ->clear();
    EnergyHitMap   			->clear() ;     
}

////////////////////////////////////////////////////////////////
void Plastic::InitializeMaterial()
	{
	
		////////////////////////////////////////////////////////////////
		/////////////////Element  Definition ///////////////////////////
		////////////////////////////////////////////////////////////////
		   G4String symbol               			;
		   G4double density = 0. , a = 0, z = 0   	;
		   G4int ncomponents = 0, natoms = 0		;

			// for Plastic
		   G4Element* H   = new G4Element("Hydrogen" , symbol = "H"  , z = 1  , a = 1.01   * g / mole);
		   G4Element* C   = new G4Element("Carbon"   , symbol = "C"  , z = 6  , a = 12.011 * g / mole);
			
		////////////////////////////////////////////////////////////////
		/////////////////Material Definition ///////////////////////////
		////////////////////////////////////////////////////////////////

		   // Pb
		   a = 207.2 * g / mole;
		   density = 11.34 * g / cm3;
		   m_MaterialLead = new G4Material("Lead", z = 82 , a, density);


		   // Plastic
		   density = 1.243 * g / cm3;
		   m_MaterialPlastic = new G4Material("Plastic", density, ncomponents = 2);
		   m_MaterialPlastic->AddElement(H , natoms = 10);
		   m_MaterialPlastic->AddElement(C  , natoms = 14);
	
	}

////////////////////////////////////////////////////////////////	
void Plastic::InitializeScorers() 
	{ 
		m_PlasticScorer = new G4MultiFunctionalDetector("PlasticScorer") ;
		G4SDManager::GetSDMpointer()->AddNewDetector(m_PlasticScorer);
		
		G4VPrimitiveScorer* DetNbr = new PSDetectorNumber("PlasticNumber", 0)  ;
		G4VPrimitiveScorer* Energy = new PSEnergy("Energy", 0)             					;
		G4VPrimitiveScorer* Time = new PSTOF("Time", 0)             					;
		 
		//and register it to the multifunctionnal detector
		m_PlasticScorer->RegisterPrimitive(DetNbr)             				;
		m_PlasticScorer->RegisterPrimitive(Energy)             				;
		m_PlasticScorer->RegisterPrimitive(Time)             				;		
		
		
	}
////////////////////////////////////////////////////////////////