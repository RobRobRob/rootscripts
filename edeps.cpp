#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>

#include <TFile.h>
#include <TTree.h>
#include <TH2I.h>

// Main function
void edeps(std::string inputname_cal){

	// Input
	TFile *input_file_cal = new TFile (inputname_cal.c_str(), "READ");

    TTree *input_tree_cal = dynamic_cast<TTree*>( input_file_cal -> Get("cal"));

    if(input_tree_cal == NULL){
    	std::cout << "File could not be read" << std::endl;
    }
    else{
    std::vector<float> *cal_edep = NULL;
    std::vector<int> *flag_bad_pulse = NULL;
	
    // Output
    std::string outputname = input_file_cal -> GetName();
    outputname.insert(outputname.size() - 5, "_energies");

    TFile *output_file = new TFile(outputname.c_str(), "RECREATE");
    std::cout << "File created " << outputname << std::endl;
    output_file -> cd();
    TH1D *h1_edep_all = new TH1D("All Pulses", "All Pulses", 1000, 0, 9000);
    TH1D *h1_edep_all_no_bad = new TH1D("All Pulses, flag_bad_pulse == 0", "All Pulses, flag_bad_pulse == 0", 1000, 0, 9000);
    TH1D *h1_edep_det1 = new TH1D("Grid 1", "Grid 1", 1000, 0, 9000);
    TH1D *h1_edep_det2 = new TH1D("Grid 2", "Grid 2", 1000, 0, 9000);
    TH1D *h1_edep_det3 = new TH1D("Grid 3", "Grid 3", 1000, 0, 9000);
    TH1D *h1_edep_det4 = new TH1D("Grid 4", "Grid 4", 1000, 0, 9000);
    TH1D *h1_edep_det1_no_bad = new TH1D("Grid 1,, flag_bad_pulse == 0", "Grid 1,, flag_bad_pulse == 0", 1000, 0, 9000);
    TH1D *h1_edep_det2_no_bad = new TH1D("Grid 2,, flag_bad_pulse == 0", "Grid 2,, flag_bad_pulse == 0", 1000, 0, 9000);
    TH1D *h1_edep_det3_no_bad = new TH1D("Grid 3,, flag_bad_pulse == 0", "Grid 3,, flag_bad_pulse == 0", 1000, 0, 9000);
    TH1D *h1_edep_det4_no_bad = new TH1D("Grid 4,, flag_bad_pulse == 0", "Grid 4,, flag_bad_pulse == 0", 1000, 0, 9000);

    h1_edep_all -> GetXaxis() -> SetTitle("Energy [keV]");
    h1_edep_all -> GetYaxis() -> SetTitle("Counts");
    h1_edep_all_no_bad -> GetXaxis() -> SetTitle("Energy [keV]");
    h1_edep_all_no_bad -> GetYaxis() -> SetTitle("Counts");
    h1_edep_det1 -> GetXaxis() -> SetTitle("Energy [keV]");
    h1_edep_det1 -> GetYaxis() -> SetTitle("Counts");
    h1_edep_det2 -> GetXaxis() -> SetTitle("Energy [keV]");
    h1_edep_det2 -> GetYaxis() -> SetTitle("Counts");
    h1_edep_det3 -> GetXaxis() -> SetTitle("Energy [keV]");
    h1_edep_det3 -> GetYaxis() -> SetTitle("Counts");
    h1_edep_det4 -> GetXaxis() -> SetTitle("Energy [keV]");
    h1_edep_det4 -> GetYaxis() -> SetTitle("Counts");
    h1_edep_det1_no_bad -> GetXaxis() -> SetTitle("Energy [keV]");
    h1_edep_det1_no_bad -> GetYaxis() -> SetTitle("Counts");
    h1_edep_det2_no_bad -> GetXaxis() -> SetTitle("Energy [keV]");
    h1_edep_det2_no_bad -> GetYaxis() -> SetTitle("Counts");
    h1_edep_det3_no_bad -> GetXaxis() -> SetTitle("Energy [keV]");
    h1_edep_det3_no_bad -> GetYaxis() -> SetTitle("Counts");
    h1_edep_det4_no_bad -> GetXaxis() -> SetTitle("Energy [keV]");
    h1_edep_det4_no_bad -> GetYaxis() -> SetTitle("Counts");

	input_tree_cal -> SetBranchStatus("*", false);
    input_tree_cal -> SetBranchStatus("cal_edep", true);
    input_tree_cal -> SetBranchStatus("flag_bad_pulse", true);
    input_tree_cal -> SetBranchAddress("cal_edep", &cal_edep);
    input_tree_cal -> SetBranchAddress("flag_bad_pulse", &flag_bad_pulse);

	// counting
	int entry_length = input_tree_cal -> GetEntries();

	std::cout << "start with " << entry_length << " events" << std::endl;
	for(int i_entry = 0; i_entry < entry_length; ++i_entry){
		// std::cout << i_entry << std::endl;
		if (i_entry % (entry_length / 10) == 0){
			std::cout << int(i_entry * 100.0 / entry_length) << "% done" << std::endl;
			std::cout << i_entry << " of " << entry_length << std::endl;
		}
		input_tree_cal -> GetEntry(i_entry);
    	for(int i_ca = 0; i_ca < cal_edep -> size(); i_ca++){
    		h1_edep_all -> Fill(cal_edep -> at(i_ca));
    		if (flag_bad_pulse -> at(i_ca) == 0){
    			h1_edep_all_no_bad -> Fill(cal_edep -> at(i_ca));
    		}
    	}
    	h1_edep_det1 -> Fill(cal_edep -> at(0));
    	h1_edep_det2 -> Fill(cal_edep -> at(1));
    	h1_edep_det3 -> Fill(cal_edep -> at(2));
    	h1_edep_det4 -> Fill(cal_edep -> at(3));
    	if (flag_bad_pulse -> at(0) == 0){
        	h1_edep_det1_no_bad -> Fill(cal_edep -> at(0));
        }
        if (flag_bad_pulse -> at(1) == 0){
	    	h1_edep_det2_no_bad -> Fill(cal_edep -> at(1));
	    }
	    if (flag_bad_pulse -> at(2) == 0){
	    	h1_edep_det3_no_bad -> Fill(cal_edep -> at(2));
	    }
	    if (flag_bad_pulse -> at(3) == 0){
	    	h1_edep_det4_no_bad -> Fill(cal_edep -> at(3));
    	}
    }
	std::cout << "all done" << std::endl;
	// Closing
	input_file_cal -> Close();
    output_file -> Write();
    output_file -> Close();
    delete input_file_cal;
    delete output_file;
	}
}
