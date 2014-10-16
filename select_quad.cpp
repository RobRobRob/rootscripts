#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TTree.h>


// Script to add quads on ca/nca level
// Calculate Energy and zpos independent
void select_quad(std::string inputname_grid) {

    TFile *input_grid = new TFile (inputname_grid.c_str(), "READ");

    TTree *input_tree = dynamic_cast<TTree*>( input_grid->Get("cal"));
      
    TH1D *hist_pos = new TH1D ("Rel. z-Position", "Rel.  z-Position", 500, -1.0, 2.0);
    TH1D *hist_edep_onlygood = new TH1D ("edep_hist_onlygood", "edep_hist_onlygood",1000 , 0, 1000);

      
    if(input_tree!=NULL){
        
        std::vector<float> *cal_cpg_ph_ca = NULL;
        std::vector<float> *cal_cpg_ph_nca = NULL;
        std::vector<float> *cal_edep = NULL;
        std::vector<float> *cal_cpg_bl_pre_ca_level = NULL;
        std::vector<float> *cal_cpg_bl_post_ca_level = NULL;
        std::vector<float> *cal_cpg_bl_pre_ca_rms = NULL;
        std::vector<float> *cal_cpg_bl_post_ca_rms = NULL;
        std::vector<int> *cal_cpg_diff_ert = NULL;
        std::vector<float> *cal_cpg_diff_rdip = NULL;
        std::vector<int> *cal_det = NULL;
        std::vector<int> *flag_bad_pulse = NULL;
        std::vector<int> *raw_trig = NULL;
        
        input_tree -> SetBranchStatus("*", false);

        input_tree -> SetBranchStatus("cal_cpg_ph_ca", true);
        input_tree -> SetBranchStatus("cal_cpg_ph_nca", true);
        input_tree -> SetBranchStatus("cal_edep", true);
        input_tree -> SetBranchStatus("flag_bad_pulse", true);
        input_tree -> SetBranchStatus("raw_trig", true);
        input_tree -> SetBranchStatus("cal_det", true);
        input_tree -> SetBranchStatus("cal_cpg_bl_pre_ca_level", true);
        input_tree -> SetBranchStatus("cal_cpg_bl_post_ca_level", true);
        input_tree -> SetBranchStatus("cal_cpg_bl_pre_ca_rms", true);
        input_tree -> SetBranchStatus("cal_cpg_bl_post_ca_rms", true);
        input_tree -> SetBranchStatus("cal_cpg_diff_ert", true);
        input_tree -> SetBranchStatus("cal_cpg_diff_rdip", true);

        input_tree -> SetBranchAddress("cal_cpg_ph_ca", &cal_cpg_ph_ca);
        input_tree -> SetBranchAddress("cal_cpg_ph_nca", &cal_cpg_ph_nca);
        input_tree -> SetBranchAddress("cal_edep", &cal_edep);
        input_tree -> SetBranchAddress("flag_bad_pulse", &flag_bad_pulse);
        input_tree -> SetBranchAddress("raw_trig", &raw_trig);
        input_tree -> SetBranchAddress("cal_det", &cal_det);
        input_tree -> SetBranchAddress("cal_cpg_bl_pre_ca_level", &cal_cpg_bl_pre_ca_level);
        input_tree -> SetBranchAddress("cal_cpg_bl_post_ca_level", &cal_cpg_bl_post_ca_level);
        input_tree -> SetBranchAddress("cal_cpg_bl_pre_ca_rms", &cal_cpg_bl_pre_ca_rms);
        input_tree -> SetBranchAddress("cal_cpg_bl_post_ca_rms", &cal_cpg_bl_post_ca_rms);
        input_tree -> SetBranchAddress("cal_cpg_diff_ert", &cal_cpg_diff_ert);
        input_tree -> SetBranchAddress("cal_cpg_diff_rdip", &cal_cpg_diff_rdip);       
       
        std::string outputname = input_grid->GetName();
        outputname.insert(outputname.size() - 5, "_new_processed");
        
        TFile *output = new TFile (outputname.c_str(), "RECREATE");
        std::cout <<"File created "<< outputname << std::endl;
        output->cd();
        
        TTree *output_tree = new TTree ("new_cal", "new_cal");
        
        float new_pos = 0;
        float nca = 0;
        float ca = 0;
        float cathode = 0;
        float rdip = 0;
        int ert = 0;
        std::vector<float> energies(4);
        float energy_onlygood = 0;
        float diff_sig = 0;
        float bl_diff = 0;
        float bl_tresh = 0;
        bool whole_bad = 0;
        bool is_ca = 0;
        float threshold = 0;
        
        // // calculated by hand 
        // std::vector<float> df;
        // df.push_back(0.765);
        // df.push_back(0.799);
        // df.push_back(0.657);
        // df.push_back(0.775);
        // calculated by hand 
        std::vector<float> df;
        df.push_back(0.882);
        df.push_back(0.860);
        df.push_back(0.757);
        df.push_back(0.828);

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

        std::vector<float> p1;
        p1.push_back(2.654);
        p1.push_back(2.572);
        p1.push_back(2.696);
        p1.push_back(2.734);

        output_tree -> Branch("nca", &nca);
        output_tree -> Branch("ca", &ca);
        output_tree -> Branch("cathode", &cathode);
        output_tree -> Branch("new_pos", &new_pos);
        output_tree -> Branch("edep", &energy_onlygood);
        output_tree -> Branch("rdip", &rdip);
        output_tree -> Branch("ert", &ert);

        int entry_length = int(input_tree->GetEntries());

        for(int i_entry=0; i_entry < entry_length; ++i_entry){
            if (i_entry % (entry_length / 10) == 0){
                std::cout << int(i_entry * 100.0 / entry_length) << "% done" << std::endl;
                std::cout << i_entry << " of " << entry_length << std::endl;
            }
    		whole_bad = true;

    		input_tree->GetEntry(i_entry); 
    		
            // Is whole event bad?
            for(int i_ca = 0; i_ca < int(cal_cpg_ph_ca -> size()); i_ca++){
                if (flag_bad_pulse -> at(i_ca) == 0){
                     whole_bad = false;
                }  
            }

    		if(whole_bad == false){
    			new_pos = 0;
    			nca = 0;
    			ca = 0;
                nca = 0;
    			cathode = 0;
                energy_onlygood = 0;
                diff_sig = 0;
                is_ca = false;
                // Calculate zpos
                for (int i_ca = 0; i_ca < int(cal_cpg_ph_ca -> size()); i_ca++){
                    // Conditions
                    threshold = cal_cpg_ph_ca->at(i_ca) - df[i_ca] * cal_cpg_ph_nca->at(i_ca);
                    threshold *= (p1[0] / p1[i_ca]);
                    if (threshold > 10.0){
                        is_ca = true;
                    }
                    // Get cathode signal always from all dets, but look for ca/nca assingment
                    if(is_ca && flag_bad_pulse -> at(i_ca) == 0){
                        // Trigger -> CA as CA
                        ca += cal_cpg_ph_ca->at(i_ca) * (p1[0] / p1[i_ca]);
                    }
                    else{
                        // No Trigger -> CA as NCA
                        nca += df[i_ca] * cal_cpg_ph_ca->at(i_ca) *  (p1[0] / p1[i_ca]);
                    }   
                    // True NCA 
                    nca += df[i_ca] * cal_cpg_ph_nca->at(i_ca) * (p1[0] / p1[i_ca]);
                    cathode += cal_cpg_ph_ca->at(i_ca) + cal_cpg_ph_nca->at(i_ca);
                    cathode *= (p1[0] / p1[i_ca]);
                }
                diff_sig = ca - nca;
                new_pos = cathode / diff_sig;

                // Calculate Energy
                for (int i_ca = 0; i_ca < int(cal_cpg_ph_ca->size()); i_ca++){
                    energies[i_ca] = cal_edep -> at(i_ca);
                }
                // calculate new values
                float e_max = *std::max_element(energies.begin(), energies.end());
                for (int i_ca = 0; i_ca < int(cal_cpg_ph_ca->size()); ++i_ca){
                    bl_diff = cal_cpg_bl_post_ca_level -> at(i_ca) - cal_cpg_bl_pre_ca_level -> at(i_ca);
                    bl_tresh = (cal_cpg_bl_pre_ca_rms -> at(i_ca) + cal_cpg_bl_pre_ca_rms -> at(i_ca)) / 2; 
                    if (flag_bad_pulse -> at(i_ca) == 0 && bl_diff > bl_tresh){
                        energy_onlygood += energies[i_ca];
                    }
                    if (energies[i_ca] == e_max){
                        rdip = cal_cpg_diff_rdip -> at(i_ca);
                        ert = cal_cpg_diff_ert -> at(i_ca);
                    }
                }
                if (energy_onlygood >= 0.0){
                    hist_edep_onlygood -> Fill(energy_onlygood);        
                    hist_pos -> Fill(new_pos);
                    output_tree -> Fill(); 
                }           

            }	
        }
        hist_pos -> SetXTitle("rel. z-Position");
        hist_pos -> SetYTitle("Counts");
        hist_pos -> Write();

        hist_edep_onlygood -> SetXTitle("Energy [Channels]");
        hist_edep_onlygood -> SetYTitle("Counts");
        hist_edep_onlygood -> Write();
        output_tree -> Write();
        output -> Write();
        output -> Close();
        delete output; 
        input_grid -> Close();
        delete input_grid;       
    }		   
}
