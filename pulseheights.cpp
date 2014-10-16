#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>

#include <TFile.h>
#include <TTree.h>
#include <TH2I.h>

// Main function
void pulseheights(std::string inputname_cal){

	// Input
	TFile *input_file_cal = new TFile (inputname_cal.c_str(), "READ");

    TTree *input_tree_cal = dynamic_cast<TTree*>( input_file_cal -> Get("cal"));

    if(input_tree_cal == NULL){
    	std::cout << "File could not be read" << std::endl;
    }
    else{
    std::vector<float> *cal_cpg_ph_ca = NULL;
    std::vector<float> *cal_cpg_ph_nca = NULL;
    std::vector<int> *flag_bad_pulse = NULL;
	
    // Output
    std::string outputname = input_file_cal -> GetName();
    outputname.insert(outputname.size() - 5, "_pulses");

    TFile *output_file = new TFile(outputname.c_str(), "RECREATE");
    std::cout << "File created " << outputname << std::endl;
    output_file -> cd();
    TH2I *h1_ca_nca_all = new TH2I("All Pulses", "All Pulses", 4500, -1000, 3500, 4500, -2500, 2000);
    TH2I *h1_ca_nca_all_no_bad = new TH2I("All Pulses, flag_bad_pulse == 0", "All Pulses, flag_bad_pulse == 0", 4500, -1000, 3500, 4500, -2500, 2000);
    TH2I *h1_ca_nca_det1 = new TH2I("Grid 1", "Grid 1", 4500, -1000, 3500, 4500, -2500, 2000);
    TH2I *h1_ca_nca_det2 = new TH2I("Grid 2", "Grid 2", 4500, -1000, 3500, 4500, -2500, 2000);
    TH2I *h1_ca_nca_det3 = new TH2I("Grid 3", "Grid 3", 4500, -1000, 3500, 4500, -2500, 2000);
    TH2I *h1_ca_nca_det4 = new TH2I("Grid 4", "Grid 4", 4500, -1000, 3500, 4500, -2500, 2000);
    TH2I *h1_ca_nca_det1_no_bad = new TH2I("Grid 1,, flag_bad_pulse == 0", "Grid 1,, flag_bad_pulse == 0", 4500, -1000, 3500, 4500, -2500, 2000);
    TH2I *h1_ca_nca_det2_no_bad = new TH2I("Grid 2,, flag_bad_pulse == 0", "Grid 2,, flag_bad_pulse == 0", 4500, -1000, 3500, 4500, -2500, 2000);
    TH2I *h1_ca_nca_det3_no_bad = new TH2I("Grid 3,, flag_bad_pulse == 0", "Grid 3,, flag_bad_pulse == 0", 4500, -1000, 3500, 4500, -2500, 2000);
    TH2I *h1_ca_nca_det4_no_bad = new TH2I("Grid 4,, flag_bad_pulse == 0", "Grid 4,, flag_bad_pulse == 0", 4500, -1000, 3500, 4500, -2500, 2000);

    h1_ca_nca_all -> GetXaxis() -> SetTitle("CA");
    h1_ca_nca_all -> GetYaxis() -> SetTitle("NCA");
    h1_ca_nca_all_no_bad -> GetXaxis() -> SetTitle("CA");
    h1_ca_nca_all_no_bad -> GetYaxis() -> SetTitle("NCA");
    h1_ca_nca_det1 -> GetXaxis() -> SetTitle("CA");
    h1_ca_nca_det1 -> GetYaxis() -> SetTitle("NCA");
    h1_ca_nca_det2 -> GetXaxis() -> SetTitle("CA");
    h1_ca_nca_det2 -> GetYaxis() -> SetTitle("NCA");
    h1_ca_nca_det3 -> GetXaxis() -> SetTitle("CA");
    h1_ca_nca_det3 -> GetYaxis() -> SetTitle("NCA");
    h1_ca_nca_det4 -> GetXaxis() -> SetTitle("CA");
    h1_ca_nca_det4 -> GetYaxis() -> SetTitle("NCA");
    h1_ca_nca_det1_no_bad -> GetXaxis() -> SetTitle("CA");
    h1_ca_nca_det1_no_bad -> GetYaxis() -> SetTitle("NCA");
    h1_ca_nca_det2_no_bad -> GetXaxis() -> SetTitle("CA");
    h1_ca_nca_det2_no_bad -> GetYaxis() -> SetTitle("NCA");
    h1_ca_nca_det3_no_bad -> GetXaxis() -> SetTitle("CA");
    h1_ca_nca_det3_no_bad -> GetYaxis() -> SetTitle("NCA");
    h1_ca_nca_det4_no_bad -> GetXaxis() -> SetTitle("CA");
    h1_ca_nca_det4_no_bad -> GetYaxis() -> SetTitle("NCA");

	input_tree_cal -> SetBranchStatus("*", false);
    input_tree_cal -> SetBranchStatus("cal_cpg_ph_ca", true);
    input_tree_cal -> SetBranchStatus("cal_cpg_ph_nca", true);
    input_tree_cal -> SetBranchStatus("flag_bad_pulse", true);
    input_tree_cal -> SetBranchAddress("cal_cpg_ph_ca", &cal_cpg_ph_ca);
    input_tree_cal -> SetBranchAddress("cal_cpg_ph_nca", &cal_cpg_ph_nca);
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
    	for(int i_ca = 0; i_ca < cal_cpg_ph_ca -> size(); i_ca++){
    		h1_ca_nca_all -> Fill(cal_cpg_ph_ca -> at(i_ca), cal_cpg_ph_nca -> at(i_ca));
    		if (flag_bad_pulse -> at(i_ca) == 0){
    			h1_ca_nca_all_no_bad -> Fill(cal_cpg_ph_ca -> at(i_ca), cal_cpg_ph_nca -> at(i_ca));
    		}
    	}
    	h1_ca_nca_det1 -> Fill(cal_cpg_ph_ca -> at(0), cal_cpg_ph_nca -> at(0));
    	h1_ca_nca_det2 -> Fill(cal_cpg_ph_ca -> at(1), cal_cpg_ph_nca -> at(1));
    	h1_ca_nca_det3 -> Fill(cal_cpg_ph_ca -> at(2), cal_cpg_ph_nca -> at(2));
    	h1_ca_nca_det4 -> Fill(cal_cpg_ph_ca -> at(3), cal_cpg_ph_nca -> at(3));
    	if (flag_bad_pulse -> at(0) == 0){
        	h1_ca_nca_det1_no_bad -> Fill(cal_cpg_ph_ca -> at(0), cal_cpg_ph_nca -> at(0));
        }
        if (flag_bad_pulse -> at(1) == 0){
	    	h1_ca_nca_det2_no_bad -> Fill(cal_cpg_ph_ca -> at(1), cal_cpg_ph_nca -> at(1));
	    }
	    if (flag_bad_pulse -> at(2) == 0){
	    	h1_ca_nca_det3_no_bad -> Fill(cal_cpg_ph_ca -> at(2), cal_cpg_ph_nca -> at(2));
	    }
	    if (flag_bad_pulse -> at(3) == 0){
	    	h1_ca_nca_det4_no_bad -> Fill(cal_cpg_ph_ca -> at(3), cal_cpg_ph_nca -> at(3));
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
