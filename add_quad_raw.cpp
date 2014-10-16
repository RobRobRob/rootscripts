#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TPad.h>
#include <TGraph.h>
// Global variables
const int sample_length = 2048;
const int number_of_anodes = 8;

void print_vec(std::vector<float> vec){
	int length = vec.size();
	for (int i = 0; i < length; ++i){
		std::cout << i << ": " << vec.at(i) << std::endl;
	}
}

std::vector<short> make_event(std::vector<float> nca, std::vector<float> ca){
	std::vector<short> event(sample_length * 2);
	for (int i = 0; i < sample_length; ++i){
		event[i] = short(nca[i]);
		event[i + sample_length] = short(ca[i]);
	}
	return event;
}

// Main function
void add_quad_raw(std::string inputname_raw, std::string inputname_cal){

	// Input
	TFile *input_file_raw = new TFile (inputname_raw.c_str(), "READ");
	TFile *input_file_cal = new TFile (inputname_cal.c_str(), "READ");

    TTree *input_tree_events = dynamic_cast<TTree*>( input_file_raw -> Get("events"));
    TTree *input_tree_runs = dynamic_cast<TTree*>( input_file_raw -> Get("runs"));
    TTree *input_tree_cal = dynamic_cast<TTree*>( input_file_cal -> Get("cal"));

    if(input_tree_events==NULL || input_tree_runs==NULL || input_tree_cal == NULL){
    	std::cout << "File could not be read" << std::endl;
    }
    else{
    std::vector<short> *raw_trans_samples_val = NULL;

    std::vector<float> *cal_cpg_ph_ca = NULL;
    std::vector<float> *cal_cpg_ph_nca = NULL;
    std::vector<int> *flag_bad_pulse = NULL;
    std::vector<int> *cal_det = NULL;
    std::vector<int> *raw_trans_samples_val_n = NULL;
 	Int_t info_idx = 0;
 	Double_t info_time = 0;
 	Double_t info_systime = 0;
 	Int_t info_devid = 1;
 	std::vector<Int_t> *raw_trig = NULL;
 	std::vector<Int_t> *raw_trans_trigPos = NULL;

    std::vector<short> raw_trans_samples_val_added(2 * sample_length);
	Int_t info_idx_added = -1;
 	Double_t info_time_added = -1;
	Double_t info_systime_added = -1;
	Int_t info_devid_added = 1;
 	std::vector<int> raw_trig_added(2);
 	std::vector<int> raw_trans_trigPos_added(2);
	std::vector<int> raw_trans_samples_ch_added(2);
	std::vector<int> raw_trans_samples_val_n_added(2);
	Int_t raw_flags_added = 0;
	
    // Output
    std::string outputname = input_file_raw -> GetName();
    outputname.insert(outputname.size() - 5, "_added");

    TFile *output_file = new TFile(outputname.c_str(), "RECREATE");
    std::cout << "File created " << outputname << std::endl;
    output_file -> cd();
    input_tree_events -> SetBranchStatus("*", false);
    input_tree_events -> SetBranchStatus("info_run", true);
    TTree *output_tree_events = input_tree_events-> CloneTree(0);
    input_tree_runs -> SetBranchStatus("*", true);
    TTree *output_tree_runs = input_tree_runs -> CloneTree(input_tree_runs -> GetEntries());	
    output_tree_runs -> Write();

    output_tree_events -> Branch("info_idx", &info_idx_added);
    output_tree_events -> Branch("info_time", &info_time_added);
    output_tree_events -> Branch("info_systime", &info_systime_added);
    output_tree_events -> Branch("info_devid", &info_devid_added);
    output_tree_events -> Branch("raw_trans_trigPos", &raw_trans_trigPos_added);
    output_tree_events -> Branch("raw_trans_samples_ch", &raw_trans_samples_ch_added);
    output_tree_events -> Branch("raw_trans_samples_val_n", &raw_trans_samples_val_n_added);
    output_tree_events -> Branch("raw_flags", &raw_flags_added);
    output_tree_events -> Branch("raw_trig", &raw_trig_added);
    output_tree_events -> Branch("raw_trans_samples_val", &raw_trans_samples_val_added);

    input_tree_events -> SetBranchStatus("*", true);
    input_tree_events -> SetBranchStatus("info_run", false);
    input_tree_events -> SetBranchAddress("raw_trans_samples_val", &raw_trans_samples_val); 
	input_tree_events -> SetBranchAddress("info_idx", &info_idx);
	input_tree_events -> SetBranchAddress("info_time", &info_time);
	input_tree_events -> SetBranchAddress("info_systime", &info_systime);
	input_tree_events -> SetBranchAddress("info_devid", &info_devid);
	input_tree_events -> SetBranchAddress("raw_trig", &raw_trig);
	input_tree_events -> SetBranchAddress("raw_trans_trigPos", &raw_trans_trigPos);
	input_tree_events -> SetBranchAddress("raw_trans_samples_val_n", &raw_trans_samples_val_n);


	input_tree_cal -> SetBranchStatus("*", false);
    input_tree_cal -> SetBranchStatus("cal_cpg_ph_ca", true);
    input_tree_cal -> SetBranchStatus("cal_cpg_ph_nca", true);
    input_tree_cal -> SetBranchStatus("flag_bad_pulse", true);
    input_tree_cal -> SetBranchStatus("cal_det", true);
    input_tree_cal -> SetBranchAddress("cal_cpg_ph_ca", &cal_cpg_ph_ca);
    input_tree_cal -> SetBranchAddress("cal_cpg_ph_nca", &cal_cpg_ph_nca);
    input_tree_cal -> SetBranchAddress("flag_bad_pulse", &flag_bad_pulse);
    input_tree_cal -> SetBranchAddress("cal_det", &cal_det); 

	// // Variables
	// Data
	std::vector<float> added_ca(sample_length);
	std::vector<float> added_nca(sample_length);
	// Calculations
	int i_whole = 0;
    float difference = 0; // CA - NCA per grid
    float threshold = 0.0; // Must be reached for acceptance as CA
    bool whole_bad = false; // At least one good signal
    bool is_ca = false; // Is treshold reached?
    bool correct_lenght = true; // Has event sample_lenght
    float nca_rel = 1.0; // Gain * mean * df
    float ca_rel = 1.0; // Gain * mean

    std::vector<float> mean_rel(4);
    mean_rel[0] = 248.5 / 248.5;
    mean_rel[1] = 1 / (256.8 / 248.5);
    mean_rel[2] = 1 / (244.5 / 248.5);
    mean_rel[3] = 1 / (241.3 / 248.5);
	// from gain_correction
	std::vector<float> gain;
	gain.push_back(1.000);
	gain.push_back(0.983);  
	gain.push_back(0.962);
	gain.push_back(1.029);
	gain.push_back(0.937);
	gain.push_back(0.888);
	gain.push_back(0.910);
	gain.push_back(0.890);

    // calculated by hand 
    std::vector<float> df(4);
    df[0] = 0.882;
    df[1] = 0.860;
    df[2] = 0.757;
    df[3] = 0.828;
	// counting
	int entry_length = input_tree_events -> GetEntries();
	entry_length /= 1; // % of whole data
	// // Check event length
	// for(int i_entry = 0; i_entry < entry_length; ++i_entry){
	// 	input_tree_events -> GetEntry(i_entry);
	// 	if (raw_trans_samples_val -> size() != 8 * 1024){
	// 		std::cout << "incorrect lenght at event " << i_entry << std::endl;
	// 		std::cout << "lenght is " << raw_trans_samples_val -> size() << std::endl;
	// 		if (raw_trans_samples_val -> size() >= 8 * 1024){
	// 		 	std::cout << "Event too long!" << std::endl;
	// 		 	break;
	// 		} 
	// 	}
	// }
	std::cout << "start adding " << entry_length << " events" << std::endl;
	for(int i_entry = 0; i_entry < entry_length; ++i_entry){
		// std::cout << i_entry << std::endl;
		if (i_entry % (entry_length / 10) == 0){
			std::cout << int(i_entry * 100.0 / entry_length) << "% done" << std::endl;
			std::cout << i_entry << " of " << entry_length << std::endl;
		    output_tree_events -> AutoSave();
		}
		i_whole = 0 ;
		input_tree_events -> GetEntry(i_entry);
		input_tree_cal -> GetEntry(i_entry);
		whole_bad = true;
		correct_lenght = true;
        for(int i = 0; i < (cal_det -> size()); i++){
            if (flag_bad_pulse -> at(i) == 0){
            	whole_bad = false; // At least one non-bad
        	}
        	if (raw_trans_samples_val_n -> at(i) != sample_length){
        		correct_lenght = false;
        	}
        }

        if (!whole_bad && correct_lenght){
            is_ca = false;
            // clear vectors
			for(int i = 0; i < sample_length; ++i){
				added_ca[i] = 0;
				added_nca[i] = 0;
			}
    		for(int i_ca = 0; i_ca < cal_cpg_ph_ca -> size(); i_ca++){
                // Conditions
                difference = cal_cpg_ph_ca->at(i_ca) - df[i_ca] * cal_cpg_ph_nca->at(i_ca);
                difference *= mean_rel[i_ca];
                if (difference > threshold && flag_bad_pulse -> at(i_ca) == 0){
                	is_ca = true;
                }
                // All NCAs
                nca_rel = gain[2 * i_ca] * df[i_ca] * mean_rel[i_ca];
				for(int i_sample = 0; i_sample < sample_length; ++i_sample){
					added_nca[i_sample] += nca_rel * raw_trans_samples_val -> at(i_whole);
					i_whole++;
				}
				// Physical CA as CA or NCA	
				if (is_ca){
					ca_rel = gain[2 * i_ca + 1] * mean_rel[i_ca];
					for (int i_sample = 0; i_sample < sample_length; ++i_sample){
						added_ca[i_sample] += ca_rel * raw_trans_samples_val -> at(i_whole);
						i_whole++;
					}
				}
				else{
					nca_rel = gain[2 * i_ca + 1] * df[i_ca] * mean_rel[i_ca];
					for (int i_sample = 0; i_sample < sample_length; ++i_sample){
						added_nca[i_sample] += nca_rel * raw_trans_samples_val -> at(i_whole);
						i_whole++;
					}							
				}
			}
			raw_trans_samples_val_added = make_event(added_nca, added_ca);
			info_idx_added = info_idx;
			info_time_added = info_time;
		 	info_systime_added = info_systime;
		 	info_devid_added = info_devid;
		 	raw_trans_trigPos_added[0] = 640;
			raw_trans_trigPos_added[1] = 640;
			raw_trans_samples_ch_added[0] = 1; // Obvious choice
			raw_trans_samples_ch_added[1] = 2;
			raw_trans_samples_val_n_added[0] = sample_length; // Only good events in the first place
			raw_trans_samples_val_n_added[1] = sample_length; // So length is fixed
			raw_flags_added = 0; // Physics pulses
		 	raw_trig_added[0] = 1; // Obvious choice
			raw_trig_added[1] = 2;
			output_tree_events -> Fill();
        }
	}
	std::cout << "all done" << std::endl;
	// Closing
	input_file_raw -> Close();
	input_file_cal -> Close();
    output_tree_events -> Print();
    output_tree_runs -> Print();
    output_file -> Write();
    output_file -> Close();
    delete input_file_raw;
    delete input_file_cal;
    delete output_file;
	}
}
