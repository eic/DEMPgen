///*--------------------------------------------------*/
/// eic.cc:
/// Original author: Dr. Ahmed Zafar
/// Date: 2015-2018
///
///*--------------------------------------------------*/
/// Modifier: Wenliang (Bill) Li
/// Date: Feb 24 2020
/// Email: wenliang.billlee@gmail.com
///
/// Comment: Feb 24, 2020: the main function is excuted in main.cc

#include "eic.h"

using std::setw;
using std::setprecision;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using namespace std;

extern char* DEMPgen_Path;

//---------------------------------------------------------
// g++ -o pim pimFermi.C `root-config --cflags --glibs`
//---------------------------------------------------------

//int main() {
// 
//  eic();
//  
//  return 0;
//}

void eic() {

  Int_t target_direction, kinematics_type;
  Double_t EBeam, HBeam;
 
  cout << "Target Direction (1->Up, 2->Down): "; cin >> target_direction; cout << endl;
  cout << "Kinematics type (1->FF, 2->TSSA): ";  cin >> kinematics_type;  cout << endl;
  cout << "Enter the number of events: ";        cin >> fNEvents;         cout << endl;
  cout << "Enter the file number: ";             cin >> fNFile;           cout << endl;
  cout << "Enter the electron beam energy: ";    cin >> EBeam;            cout << endl;
  cout << "Enter the hadron beam energy: ";      cin >> HBeam;            cout << endl;
 
  //	eic(target_direction, kinematics_type, fNEvents);

}

/*--------------------------------------------------*/
// 18/01/23 - SJDK- This function is never used since eic() is only called with a json object as the argument. Commented out for now, delete later?
/* void eic(int event_number, int target_direction, int kinematics_type, TString file_name, int fEIC_seed, TString Ejectile, TString RecoilHadron, TString det_location, TString OutputType, double EBeam, double HBeam) {

   TString targetname;
   TString charge;

   if( target_direction == 1 ) targetname = "up";
   if( target_direction == 2 ) targetname = "down";
	
   gKinematics_type = kinematics_type;
   gfile_name = file_name;

   fNFile = 1;

   fNEvents = event_number;

   fSeed = fEIC_seed;
   cout << EBeam << " elec " << HBeam << " RecoilHadrons" << endl; 
   fEBeam = EBeam;
   fPBeam = HBeam;

   pim* myPim = new pim(fSeed);
   myPim->Initilize();
   // 09/02/22 - SJDK - Special case for the kaon, if RecoilHadron not specified, default to Lambda
   if (Ejectile == "K+"){
   if (RecoilHadron != "Lambda" && RecoilHadron != "Sigma0"){
   RecoilHadron = "Lambda";
   }
   else{
   RecoilHadron = ExtractParticle(RecoilHadron);
   }
   Reaction* r1 = new Reaction(Ejectile, RecoilHadron);
   r1->process_reaction();
   delete r1;
   }
   else if (Ejectile == "pi+" || Ejectile == "Pion+" ||  Ejectile == "Pi+"){
   RecoilHadron = "Neutron";
   Ejectile = ExtractParticle(particle);
   charge = ExtractCharge(Ejectile);
   Reaction* r1 = new Reaction(Ejectile, RecoilHadron);
   r1->process_reaction();
   delete r1;
   }
   else if (Ejectile == "pi0" || Ejectile == "Pion0" || Ejectile == "Pi0"){
   RecoilHadron = "Proton";
   Ejectile = ExtractParticle(Ejectile);
   charge = ExtractCharge(Ejectile);
   //Reaction* r1 = new Reaction(Ejectile);
   Reaction* r1 = new Reaction(Ejectile, RecoilHadron);
   r1->process_reaction();
   delete r1;
   }
   else{
   Ejectile = ExtractParticle(Ejectile);
   charge = ExtractCharge(Ejectile);
   Reaction* r1 = new Reaction(Ejectile);
   r1->process_reaction();
   delete r1;
   }
   }
*/

/*--------------------------------------------------*/
/*--------------------------------------------------*/
// SJDK 21/12/22 - Note that this is the one that actually gets used, reads in the .json file
void eic(Json::Value obj) {
   	
  TString targetname;  
  TString charge;

  int target_direction = obj["Targ_dir"].asInt();
  gKinematics_type     = obj["Kinematics_type"].asInt();

  if( target_direction == 1 ) targetname = "up";
  if( target_direction == 2 ) targetname = "down";
 
  gfile_name = obj["file_name"].asString();
 
  gPi0_decay = obj["pi0_decay"].asBool();

  fNFile = 1;
  fNEvents = obj["n_events"].asUInt64();

  fSeed = obj["generator_seed"].asInt();

  pim* myPim = new pim(fSeed);
  myPim->Initilize();
 
  //  	TDatime dsTime;
  //  	cout << "Start Time:   " << dsTime.GetHour() << ":" << dsTime.GetMinute() << endl;
  // 21/12/22 - SJDK - Should do a check if these are defined or not, should crash if not defined or set defaults, see other quantities below
  TString ROOTFile = obj["ROOTOut"].asString();
  if (ROOTFile == "True" || ROOTFile == "true" || ROOTFile == "TRUE"){
    gROOTOut = true;
    cout << "ROOT output file enabled." << endl;
  }
  else{
    gROOTOut = false;
    cout << "ROOT output file disabled." << endl;
  }

  TString Ejectile = obj["ejectile"].asString();
  TString RecoilHadron = obj["recoil_hadron"].asString(); // 09/02/22 - SJDK - Added in RecoilHadron type argument for K+
  // SJDK - 08/02/22 - This is terrible, need to change this, Ejectile should just be K+
  // Add a new flag which, RecoilHadron - where this is specified too, then add conditionals elsewhere based on this
  // New conditional, special case for Kaon	
  Ejectile = ExtractParticle(Ejectile);
  charge = ExtractCharge(Ejectile);
  if(RecoilHadron == "Sigma" || RecoilHadron == "sigma"){ // SJDK - 31/01/23 - If RecoilHadron specified as Sigma, interpret this as Sigma0. Also correct for lower case
    RecoilHadron = "Sigma0";
  }
  if (RecoilHadron == "lambda"){ // SJDK - 31/01/23 - Make Lambda selection case insensitive
    RecoilHadron = "Lambda"; 
  }
  if (Ejectile == "K+"){
    if (RecoilHadron != "Lambda" && RecoilHadron != "Sigma0"){
      RecoilHadron = "Lambda";
      cout << "! WARNING !" << endl;
      cout << "! WARNING !- K+ production specified but RecoilHadron not recognised, deaulting to Lambda - ! WARNING!" << endl;
      cout << "! WARNING !" << endl;
    }
    else{
      RecoilHadron = ExtractParticle(RecoilHadron);
    }
  }
  // SJDK - 19/12/22 - Specify RecoilHadron to neutron/proton for pi+/pi0 production, for pi0 production, may want to adjust? 
  else if (Ejectile == "pi+" || Ejectile == "Pion+" || Ejectile == "Pi+"){
    RecoilHadron = "Neutron";
  }
  else if (Ejectile == "pi0" || Ejectile == "Pion0" || Ejectile == "Pi0"){
    RecoilHadron = "Proton";
  }
  else { // SJDK -09/02/22 - Note that in future this could be changed to get different RecoilHadrons in other reactions if desired
    RecoilHadron = "";
  }

  // SJDK 03/04/23 - Change to how Qsq range is set/chosen, could add as an override variable later too
  // Set min/max Qsq values depending upon Ejectile type
  if (Ejectile == "pi+" || Ejectile == "Pion+" || Ejectile == "Pi+"){
    fQsq_Min = 3.5; fQsq_Max = 35.0; // Love Preet changed to 3.5 as SigT parameterization starts from Q2 = 3.5
    fW_Min = 2.0; fW_Max = 10.2;
    fT_Max_Default = 1.3;
  }
  else if (Ejectile == "pi0" || Ejectile == "Pion0" || Ejectile == "Pi0"){
    fQsq_Min = 5.0; fQsq_Max = 1000.0;
    fW_Min = 2.0; fW_Max = 10.0;
    fT_Max_Default = 0.5;
  }
  else if (Ejectile == "K+"){
    fQsq_Min = 1.0; fQsq_Max = 35.0;
    fW_Min = 2.0; fW_Max = 10.0;
    fT_Max_Default = 2.0;
  }
  else{
    fQsq_Min = 5.0; fQsq_Max = 35.0;
    fW_Min = 2.0; fW_Max = 10.0;
    fT_Max_Default = 1.3;
  }

  // SJDK - 01/06/21
  // Set beam energies from .json read in
  if (obj.isMember("ebeam")){
    fEBeam = obj["ebeam"].asDouble();
  }
  else{
    fEBeam = 5;
    cout << "Electron beam energy not specified in .json file, defaulting to 5 GeV." << endl;
  }
  if (obj.isMember("hbeam")){
    fPBeam = obj["hbeam"].asDouble();
    fHBeam = obj["hbeam"].asDouble();
  }
  else{
    fPBeam = 100;
    fHBeam = 100;
    cout << "Ion beam energy not specified in .json file, defaulting to 100 GeV." << endl;
  }

  if (obj.isMember("hbeam_part")){
    gBeamPart = obj["hbeam_part"].asString();
  } 
  else {
    gBeamPart = "Proton";
  }

  // SJDK - 12/01/22
  // Set output type as a .json read in
  // Should be Pythia6, LUND or HEPMC3
  if (obj.isMember("OutputType")){
    gOutputType = obj["OutputType"].asString();
    if (gOutputType == "Pythia6"){
      cout << "Using Pythia6 output format for Fun4All" << endl;
    }
    else if (gOutputType == "LUND"){
      cout << "Using LUND output format" << endl;
    }
    else if (gOutputType == "HEPMC3"){
      cout << "Using HEPMC3 output format for ePIC" << endl;
    }
    else{
      cout << "Output type not recognised!" << endl;
      cout << "Setting output type to HEPMC3 (ePIC) by default!" << endl;
      gOutputType = "HEPMC3";
    }
  }
  else{
    cout << "Output type not specified in .json file!" << endl;
    cout << "Setting output type to HEPMC3 (ePIC) by default!" << endl;
    gOutputType = "HEPMC3";
  }
  ///*--------------------------------------------------*/
  /// The detector selection is determined here
  /// The incidence proton phi angle is 
  if (obj.isMember("det_location")){
    gDet_location = obj["det_location"].asString();
    if (gDet_location == "ip8") {
      fProton_incidence_phi = 0.0;
    } 
    else if (gDet_location == "ip6") {
      fProton_incidence_phi = fPi;
    }
    else {
      fProton_incidence_phi = 0.0;
      cout << "The interaction point requested is not recognized!" << endl;
      cout << "Therefore ip6 is used by default." << endl;
    }
  }
  else{ // 21/12/22 - This could probably be combined with the else statement above in some way
    fProton_incidence_phi = 0.0;
    cout << "The interaction points was not specified in the .json file!" << endl;
    cout << "Therefore ip6 is used by default" << endl;
  }

  if (obj.isMember("Ee_Low")){
    fScatElec_E_Lo = obj["Ee_Low"].asDouble();
  }
  else{
    fScatElec_E_Lo = 0.5;
    cout << "Minumum scattered electron energy not specified in .json file, defaulting to 0.5*EBeam." << endl;
  }

  if (obj.isMember("Ee_High")){
    fScatElec_E_Hi = obj["Ee_High"].asDouble();
  }
  else{
    fScatElec_E_Hi = 2.5;
    cout << "Max scattered electron energy not specified in .json file, defaulting to 2.5*EBeam." << endl;
  }

  if (obj.isMember("e_Theta_Low")){
    fScatElec_Theta_I = obj["e_Theta_Low"].asDouble() * fDEG2RAD;
  }
  else{
    fScatElec_Theta_I = 60.0 * fDEG2RAD;
    cout << "Min scattered electron theta not specified in .json file, defaulting to 60 degrees." << endl;
  }

  if (obj.isMember("e_Theta_High")){
    fScatElec_Theta_F = obj["e_Theta_High"].asDouble() * fDEG2RAD;
  }
  else{
    fScatElec_Theta_F = 175.0 * fDEG2RAD;
    cout << "Max scattered electron theta not specified in .json file, defaulting to 175 degrees." << endl;
  }

  if (obj.isMember("EjectileX_Theta_Low")){
    fEjectileX_Theta_I = obj["EjectileX_Theta_Low"].asDouble() * fDEG2RAD;
  }
  else{
    fEjectileX_Theta_I = 0.0 * fDEG2RAD;
    cout << "Min ejectile X theta not specified in .json file, defaulting to 0 degrees." << endl;
  }

  if (obj.isMember("EjectileX_Theta_High")){
    fEjectileX_Theta_F = obj["EjectileX_Theta_High"].asDouble() * fDEG2RAD;
  }
  else{
    fEjectileX_Theta_F = 60.0 * fDEG2RAD;
    cout << "Max ejectile X theta not specified in .json file, defaulting to 60 degrees." << endl;
  }

  // 06/09/23 - SJDK - Added string to check method chosen
  TString CalcMethod = obj["calc_method"].asString(); 
  if(CalcMethod == "Analytical"){
    UseSolve = false;
  }
  else if (CalcMethod == "Solve"){
    UseSolve = true;
  }
  else{
    cout << "! WARNING !" << endl  << "! WARNING !- Calculation method not specified or not recognised, defaulting to Analytical - ! WARNING !" << endl << "! WARNING !" << endl;
    UseSolve = false;
  }

  // 17/07/25 - SJDK - Add new check to get t_max value
  if (obj.isMember("Kin_tMax")){
    if( obj["Kin_tMax"].asDouble() < 0 ){
      if( abs(obj["Kin_tMax"].asDouble()) > fT_Max_Default){
	cout << "! WARNING !" << endl << "! WARNING ! - Max -t entered as a negative number and absolute value exceeds limit for reaction, seting to reaction max! - ! WARNING !"<< endl << "! WARNING !"<< endl;
	fT_Max = fT_Max_Default;
      }
      else{
	cout << "! WARNING !" << endl << "! WARNING ! - Max -t entered as a negative number, taking absolute value! - ! WARNING !"<< endl << "! WARNING !"<< endl;
	fT_Max = abs(obj["Kin_tMax"].asDouble());
      }
    }
    else if (obj["Kin_tMax"].asDouble() == 0 || obj["Kin_tMax"].asDouble() > fT_Max_Default){
      cout << "! WARNING !" << endl << "! WARNING ! - Max -t set to 0 or exceeding paramaterisation limit for reaction, setting to reaction max! - ! WARNING !"<< endl << "! WARNING !"<< endl;
      fT_Max = fT_Max_Default;
    }
    else{
      fT_Max = obj["Kin_tMax"].asDouble();
    }
  }
  else{
    fT_Max = 1.0;
    cout << "! WARNING !" << endl << "! WARNING ! - Max -t not specified, defaulting to 1! - ! WARNING !"<< endl << "! WARNING !"<< endl;
  }
  SigPar = ReadCrossSectionPar(Ejectile, RecoilHadron);
	
  if(Ejectile != "pi0"){ // Default case now
    Reaction* r1 = new Reaction(Ejectile, RecoilHadron);
    r1->process_reaction();
    delete r1;
  }
  else{  // Treat pi0 slightly differently for now
    Reaction* r1 = new Reaction(Ejectile);
    r1->process_reaction();
    delete r1;
  }
}

/*--------------------------------------------------*/
/*--------------------------------------------------*/

void SetEICSeed (int seed) {
  fSeed = seed;
}

///*--------------------------------------------------*/
///*--------------------------------------------------*/
///  Some utility functions

///*--------------------------------------------------*/
/// Extracting the particle type

TString ExtractParticle(TString particle) {

  /// Make the input particle case insansitive
  particle.ToLower();
  if (particle.Contains("on")) {
    particle.ReplaceAll("on", "");
  };
 	
  if (particle.Contains("plus")) {
    particle.ReplaceAll("plus", "+");
  }

  if (particle.Contains("minus")) {
    particle.ReplaceAll("minus", "-");
  }

  if (particle.Contains("zero")) {
    particle.ReplaceAll("zero", "0");
  }

  particle[0] = toupper(particle[0]);
  cout << "Particle: " << particle << endl;
  return particle;

}

///*--------------------------------------------------*/
/// Extracting the particle charge

TString ExtractCharge(TString particle) {

  TString charge;

  if (particle.Contains("+") || particle.Contains("plus")) {
    charge = "+";
  } else if (particle.Contains("-") || particle.Contains("minus")) {
    charge = "-";
  } else {
    charge = "0";
  }
  return charge;
}

vector<vector<vector<vector<double>>>> ReadCrossSectionPar(TString EjectileX, TString RecoilHad){
  
  string sigL_ParamFile, sigT_ParamFile;
 
  if (EjectileX == "Pi+" && RecoilHad == "Neutron"){
    // When pion model parameterised in some way, add Pi+/Neutron case here - 
  }
  else if (EjectileX == "Pi-" && RecoilHad == "Proton"){
    // When pion model parameterised in some way, add Pi-/Proton case here - 
  }
  else if (EjectileX == "K+" && RecoilHad == "Lambda"){
    sigL_ParamFile = Form("%s/src/eic_evgen/CrossSection_Params/KPlusLambda_Param_sigL", DEMPgen_Path);
    sigT_ParamFile = Form("%s/src/eic_evgen/CrossSection_Params/KPlusLambda_Param_sigT", DEMPgen_Path);
  }
  else if (EjectileX == "K+" && RecoilHad == "Sigma0"){
    sigL_ParamFile = Form("%s/src/eic_evgen/CrossSection_Params/KPlusSigma_Param_sigL", DEMPgen_Path);
    sigT_ParamFile = Form("%s/src/eic_evgen/CrossSection_Params/KPlusSigma_Param_sigT", DEMPgen_Path);
  }
  else if (EjectileX == "Pi0"){
    // When pi0 model parameterised, add it here
  }
  else{
    cerr << " !!!!! " << endl << "Warning!" << endl << "Combination of specified ejectile and recoil hadron not found!" << "Cross section parameters cannot be read, check inputs!" << endl << "Warning!" << endl << " !!!!! " << endl;
    exit(-1);
  }
 
  //....................................................................................................
  // Love's model parameters (Gojko, Stephen and Nishchey helped me to understand this part) 
  //....................................................................................................
  double ptmp;
  std::vector<std::vector<std::vector<std::vector<double>>>> p_vec;
  fstream file_vgl; // The parameterization file we will open and loop over

  for (int i = 0; i < 2; i++){
    if(i == 0){
      file_vgl.open(sigL_ParamFile, ios::in); 
    }
    if(i == 1){
      file_vgl.open(sigT_ParamFile, ios::in); 
    }
    p_vec.push_back(std::vector<std::vector<std::vector<double>>>());
    for(int j=0; j <9; j++){// Loop over all values of W - 2 to 10
      p_vec[i].push_back(std::vector<std::vector<double>>());
      for(int k=0; k<35; k++){ // Loop over all values of Q2 - 1 to 35 for each w
	p_vec[i][j].push_back(std::vector<double>());
      
	for(int l=0; l<13; l++){ //Loop over all columns at once
	  file_vgl>>ptmp;
	  p_vec[i][j][k].push_back(ptmp);
	}
      }
    }
    file_vgl.close();// Need to close the file at end of each loop over i 
  }       
  return p_vec;
}
