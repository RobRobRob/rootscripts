/*
Calculate values of a measurment for Quad Detectors from an processed file
    - Adds all Anodes to get better z-Pos value
    - Adds Energies of subgrids with clear CA Signal
    - Takes rdip and ert from grid with most energy
*/
// STD libs
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <map>
// ROOT libs
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
// Costum
#include <readrootrc.cpp>

// Calculate Energy and zpos independent
void quad_adder(std::string inputname_grid){

    // An already processed file
    TFile *input_grid = new TFile (inputname_grid.c_str(), "READ");

    TTree *input_tree = dynamic_cast<TTree*>( input_grid->Get("cal"));
      
    if(input_tree != NULL){
        
        std::vector<float> *cal_cpg_ph_ca = NULL;
        std::vector<float> *cal_cpg_ph_nca = NULL;
        std::vector<float> *cal_edep = NULL;
        std::vector<float> *cal_ipos_z = NULL;
        std::vector<float> *cal_cpg_bl_pre_ca_level = NULL;
        std::vector<float> *cal_cpg_bl_post_ca_level = NULL;
        std::vector<float> *cal_cpg_bl_pre_nca_level = NULL;
        std::vector<float> *cal_cpg_bl_post_nca_level = NULL;
        std::vector<float> *cal_cpg_bl_pre_ca_rms = NULL;
        std::vector<float> *cal_cpg_bl_post_ca_rms = NULL;
        std::vector<float> *cal_cpg_bl_pre_nca_rms = NULL;
        std::vector<float> *cal_cpg_bl_post_nca_rms = NULL;
        std::vector<int> *cal_cpg_diff_ert = NULL;
        std::vector<float> *cal_cpg_diff_rdip = NULL;
        std::vector<int> *cal_det = NULL;
        std::vector<int> *flag_bad_pulse = NULL;
        std::vector<int> *raw_trig = NULL;
        std::vector<int> *cal_tracelength = NULL;
        std::vector<float> *cal_cpg_ca_max = NULL;
        std::vector<float> *cal_cpg_ca_min = NULL;
        std::vector<float> *cal_cpg_nca_max = NULL;
        std::vector<float> *cal_cpg_nca_min = NULL;
        std::vector<float> *cal_cpg_ca_max_change = NULL;
        std::vector<float> *cal_cpg_diff_max_change = NULL;
        std::vector<float> *cal_cpg_ph_diff = NULL;
        
        input_tree -> SetBranchStatus("*", false);
        input_tree -> SetBranchStatus("cal_cpg_ph_ca", true);
        input_tree -> SetBranchStatus("cal_cpg_ph_nca", true);
        input_tree -> SetBranchStatus("cal_edep", true);
        input_tree -> SetBranchStatus("cal_ipos_z", true);
        input_tree -> SetBranchStatus("flag_bad_pulse", true);
        input_tree -> SetBranchStatus("raw_trig", true);
        input_tree -> SetBranchStatus("cal_det", true);
        input_tree -> SetBranchStatus("cal_cpg_bl_pre_ca_level", true);
        input_tree -> SetBranchStatus("cal_cpg_bl_post_ca_level", true);
        input_tree -> SetBranchStatus("cal_cpg_bl_pre_ca_rms", true);
        input_tree -> SetBranchStatus("cal_cpg_bl_post_ca_rms", true);
        input_tree -> SetBranchStatus("cal_cpg_bl_pre_nca_level", true);
        input_tree -> SetBranchStatus("cal_cpg_bl_post_nca_level", true);
        input_tree -> SetBranchStatus("cal_cpg_bl_pre_nca_rms", true);
        input_tree -> SetBranchStatus("cal_cpg_bl_post_nca_rms", true);
        input_tree -> SetBranchStatus("cal_cpg_diff_ert", true);
        input_tree -> SetBranchStatus("cal_cpg_diff_rdip", true);
        input_tree -> SetBranchStatus("cal_tracelength", true);
        input_tree -> SetBranchStatus("cal_cpg_ca_max", true);
        input_tree -> SetBranchStatus("cal_cpg_ca_min", true);
        input_tree -> SetBranchStatus("cal_cpg_nca_max", true);
        input_tree -> SetBranchStatus("cal_cpg_nca_min", true);
        input_tree -> SetBranchStatus("cal_cpg_ca_max_change", true);
        input_tree -> SetBranchStatus("cal_cpg_diff_max_change", true);
        input_tree -> SetBranchStatus("cal_cpg_ph_diff", true);

        input_tree -> SetBranchAddress("cal_cpg_ph_ca", &cal_cpg_ph_ca);
        input_tree -> SetBranchAddress("cal_cpg_ph_nca", &cal_cpg_ph_nca);
        input_tree -> SetBranchAddress("cal_edep", &cal_edep);
        input_tree -> SetBranchAddress("cal_ipos_z", &cal_ipos_z);
        input_tree -> SetBranchAddress("flag_bad_pulse", &flag_bad_pulse);
        input_tree -> SetBranchAddress("raw_trig", &raw_trig);
        input_tree -> SetBranchAddress("cal_det", &cal_det);
        input_tree -> SetBranchAddress("cal_cpg_bl_pre_ca_level", &cal_cpg_bl_pre_ca_level);
        input_tree -> SetBranchAddress("cal_cpg_bl_post_ca_level", &cal_cpg_bl_post_ca_level);
        input_tree -> SetBranchAddress("cal_cpg_bl_pre_ca_rms", &cal_cpg_bl_pre_ca_rms);
        input_tree -> SetBranchAddress("cal_cpg_bl_post_ca_rms", &cal_cpg_bl_post_ca_rms);
        input_tree -> SetBranchAddress("cal_cpg_bl_pre_nca_level", &cal_cpg_bl_pre_nca_level);
        input_tree -> SetBranchAddress("cal_cpg_bl_post_nca_level", &cal_cpg_bl_post_nca_level);
        input_tree -> SetBranchAddress("cal_cpg_bl_pre_nca_rms", &cal_cpg_bl_pre_nca_rms);
        input_tree -> SetBranchAddress("cal_cpg_bl_post_nca_rms", &cal_cpg_bl_post_nca_rms);
        input_tree -> SetBranchAddress("cal_cpg_diff_ert", &cal_cpg_diff_ert);
        input_tree -> SetBranchAddress("cal_cpg_diff_rdip", &cal_cpg_diff_rdip);
        input_tree -> SetBranchAddress("cal_tracelength", &cal_tracelength);
        input_tree -> SetBranchAddress("cal_cpg_ca_max", &cal_cpg_ca_max); 
        input_tree -> SetBranchAddress("cal_cpg_ca_min", &cal_cpg_ca_min); 
        input_tree -> SetBranchAddress("cal_cpg_nca_max", &cal_cpg_nca_max);
        input_tree -> SetBranchAddress("cal_cpg_nca_min", &cal_cpg_nca_min); 
        input_tree -> SetBranchAddress("cal_cpg_ca_max_change", &cal_cpg_ca_max_change);
        input_tree -> SetBranchAddress("cal_cpg_diff_max_change", &cal_cpg_diff_max_change);
        input_tree -> SetBranchAddress("cal_cpg_ph_diff", &cal_cpg_ph_diff);

        std::string outputname = input_grid->GetName();
        outputname.insert(outputname.size() - 5, "_new_processed");
        
        TFile *output = new TFile (outputname.c_str(), "RECREATE");
        std::cout << "File created " << outputname << std::endl;
        output->cd();
        TH1F *hist_pos = new TH1F ("cal_ipos_z_added", "cal_ipos_z_added", 150, -1.0, 2.0);
        TH1F *hist_edep = new TH1F ("cal_edep_added", "cal_edep_added", 200 , 0, 800);
        TH2F *hist_ca_nca = new TH2F ("ca_nca_added", "ca_nca_added", 175, -50, 300, 175, -50, 300);

        TH1F *cal_edep_grid1    = new TH1F ("cal_edep_grid1", "cal_edep_grid1", 200, 0, 800);
        TH1F *cal_ipos_z_grid1  = new TH1F ("cal_ipos_z_grid1", "cal_ipos_z_grid1", 150, -1.0, 2.0);
        TH2F *cal_ca_nca_grid1  = new TH2F ("cal_ca_nca_grid1", "cal_ca_nca_grid1", 175, -50, 300, 175, -50, 300);
        TH1F *cal_edep_grid2    = new TH1F ("cal_edep_grid2", "cal_edep_grid2", 200, 0, 800);
        TH1F *cal_ipos_z_grid2  = new TH1F ("cal_ipos_z_grid2", "cal_ipos_z_grid2", 150, -1.0, 2.0);
        TH2F *cal_ca_nca_grid2  = new TH2F ("cal_ca_nca_grid2", "cal_ca_nca_grid2", 175, -50, 300, 175, -50, 300);
        TH1F *cal_edep_grid3    = new TH1F ("cal_edep_grid3", "cal_edep_grid3", 200, 0, 800);
        TH1F *cal_ipos_z_grid3  = new TH1F ("cal_ipos_z_grid3", "cal_ipos_z_grid3", 150, -1.0, 2.0);
        TH2F *cal_ca_nca_grid3  = new TH2F ("cal_ca_nca_grid3", "cal_ca_nca_grid3", 175, -50, 300, 175, -50, 300);
        TH1F *cal_edep_grid4    = new TH1F ("cal_edep_grid4", "cal_edep_grid4", 200, 0, 800);
        TH1F *cal_ipos_z_grid4  = new TH1F ("cal_ipos_z_grid4", "cal_ipos_z_grid4", 150, -1.0, 2.0);
        TH2F *cal_ca_nca_grid4  = new TH2F ("cal_ca_nca_grid4", "cal_ca_nca_grid4", 175, -50, 300, 175, -50, 300);

        // New values, _all values are simple addition of all 4 subgrid, others use some sort of selection
        std::vector<float> ncas;
        std::vector<float> ncas_all;
        float nca = 0;
        std::vector<float> ncas_weighted;
        std::vector<float> ncas_weighted_all;
        float nca_weighted = 0;
        float nca_weighted_all = 0;
        float nca_all = 0;
        std::vector<float> cas;
        std::vector<float> cas_all;
        float ca = 0;
        float ca_all = 0;
        std::vector<float> energies;
        std::vector<float> energies_all;
        float e_added_all = 0;
        float e_added = 0;
        float new_pos = 0;
        float new_pos_all = 0;
        float rdip = 0;
        int ert = 0;

        bool global_bad_flag = false; // Whole Events is rejected
        bool energy_reached = false; // Low-energy threshold reached
        bool ca_found = false; // For selection
        bool nca_found = false; // For selection
        int sample_length = 1024; // Samples per Pulse
        int entry_length = int(input_tree->GetEntries()); // get number of entries 

        Readrootrc *r1 = new Readrootrc();
        std::vector<float> df = r1 -> Get_df();
        std::vector<float> p1 = r1 -> Get_p1();
        std::cout << "df, p1" << std::endl;
        for (unsigned int i = 0; i < p1.size(); ++i){
            std::cout << df[i] << ", " << p1[i] << std::endl;
        }
        // Find the correct low-energy threshold for the whole measurment
        float treshold_of_measurment = 18; // Initial value
        float global_tresh = 0;
        for (unsigned int i = 0; i < df.size(); ++i){
            if (global_tresh < (treshold_of_measurment + df[i] * treshold_of_measurment) * p1[i]){
                global_tresh = (treshold_of_measurment + df[i] * treshold_of_measurment) * p1[i];
            }
        }
        std::cout << "Energy Treshold: " << global_tresh << std::endl;

        TTree *output_tree = new TTree ("cal", "cal");

        output_tree -> Branch("nca_sum", &nca);
        output_tree -> Branch("nca_sum_weighted", &nca_weighted);
        output_tree -> Branch("nca_all_sum", &nca_all);
        output_tree -> Branch("nca_all_sum_weighted", &nca_weighted_all);
        output_tree -> Branch("ncas", &ncas);
        output_tree -> Branch("ncas_all", &ncas_all);
        output_tree -> Branch("ca_sum", &ca);
        output_tree -> Branch("ca_all_sum", &ca_all);
        output_tree -> Branch("cas", &cas);
        output_tree -> Branch("cas_all", &cas_all);
        output_tree -> Branch("cal_edep", &e_added);
        output_tree -> Branch("cal_edep_all", &e_added_all);
        output_tree -> Branch("cal_ipos_z", &new_pos);
        output_tree -> Branch("cal_ipos_z_all", &new_pos_all);
        output_tree -> Branch("rdip", &rdip);
        output_tree -> Branch("ert", &ert);
        output_tree -> Branch("flag_bad_pulse", &global_bad_flag);
        output_tree -> Branch("cal_cpg_bl_pre_ca_level", &cal_cpg_bl_pre_ca_level);
        output_tree -> Branch("cal_cpg_bl_post_ca_level", &cal_cpg_bl_post_ca_level);
        output_tree -> Branch("cal_cpg_bl_pre_ca_rms", &cal_cpg_bl_pre_ca_rms);
        output_tree -> Branch("cal_cpg_bl_post_ca_rms", &cal_cpg_bl_post_ca_rms);
        output_tree -> Branch("cal_cpg_bl_pre_nca_level", &cal_cpg_bl_pre_nca_level);
        output_tree -> Branch("cal_cpg_bl_post_nca_level", &cal_cpg_bl_post_nca_level);
        output_tree -> Branch("cal_cpg_bl_pre_nca_rms", &cal_cpg_bl_pre_nca_rms);
        output_tree -> Branch("cal_cpg_bl_post_nca_rms", &cal_cpg_bl_post_nca_rms);
        output_tree -> Branch("cal_cpg_ca_max", &cal_cpg_ca_max); 
        output_tree -> Branch("cal_cpg_ca_min", &cal_cpg_ca_min); 
        output_tree -> Branch("cal_cpg_nca_max", &cal_cpg_nca_max);
        output_tree -> Branch("cal_cpg_nca_min", &cal_cpg_nca_min); 
        output_tree -> Branch("cal_cpg_ca_max_change", &cal_cpg_ca_max_change);
        output_tree -> Branch("cal_cpg_diff_max_change", &cal_cpg_diff_max_change);
        output_tree -> Branch("cal_cpg_ph_diff", &cal_cpg_ph_diff);


        std::cout << "Sample Length: " << sample_length << std::endl;
        std::cout << "Start calculation of values" << std::endl;
        for(int i_entry=0; i_entry < entry_length; ++i_entry){
            if (i_entry % (entry_length / 10) == 0){
                std::cout << int(i_entry * 100.0 / entry_length) << "% done" << std::endl;
                std::cout << i_entry << " of " << entry_length << std::endl;
            }
            // Histogramms for single grids, no bad pulses
            for (std::vector<int>::iterator it = cal_det -> begin(); it != cal_det -> end(); ++it){
                switch(*it){
                    case 1:
                        if (!flag_bad_pulse -> at(0)){
                            cal_edep_grid1 -> Fill(cal_edep -> at(0));
                            cal_ipos_z_grid1 -> Fill(cal_ipos_z -> at(0));
                            cal_ca_nca_grid1 -> Fill(cal_cpg_ph_ca -> at(0), -cal_cpg_ph_nca -> at(0));
                        }
                    case 2:
                        if (!flag_bad_pulse -> at(1)){
                            cal_edep_grid2 -> Fill(cal_edep -> at(1));
                            cal_ipos_z_grid2 -> Fill(cal_ipos_z -> at(1));
                            cal_ca_nca_grid2 -> Fill(cal_cpg_ph_ca -> at(1), -cal_cpg_ph_nca -> at(1));
                        }
                    case 3:
                        if (!flag_bad_pulse -> at(2)){
                            cal_edep_grid3 -> Fill(cal_edep -> at(2));
                            cal_ipos_z_grid3 -> Fill(cal_ipos_z -> at(2));
                            cal_ca_nca_grid3 -> Fill(cal_cpg_ph_ca -> at(2), -cal_cpg_ph_nca -> at(2));
                        }
                    case 4:
                        if (!flag_bad_pulse -> at(3)){
                            cal_edep_grid4 -> Fill(cal_edep -> at(3));
                            cal_ipos_z_grid4 -> Fill(cal_ipos_z -> at(3));
                            cal_ca_nca_grid4 -> Fill(cal_cpg_ph_ca -> at(3), -cal_cpg_ph_nca -> at(3));
                        }
                }
            }
            
            energies.clear();
            energies_all.clear();
            ncas.clear();
            ncas_all.clear();
            ncas_weighted.clear();
            ncas_weighted_all.clear();
            cas.clear();
            cas_all.clear();

            global_bad_flag = false;
            energy_reached = false;

            input_tree -> GetEntry(i_entry); 
            
            for(unsigned int i_ca = 0; i_ca <cal_cpg_ph_ca -> size(); i_ca++){
                ca_found = false;
                nca_found = false;
                // Select Anodes
                ca_found  |= cal_cpg_ph_ca -> at(i_ca) > std::abs(cal_cpg_bl_pre_ca_rms -> at(i_ca)) * 2.0;
                nca_found |= ca_found;
                nca_found |= std::abs(cal_cpg_ph_nca -> at(i_ca)) > std::abs(cal_cpg_bl_pre_nca_rms -> at(i_ca)) / 2.0;
                nca_found |= std::abs(cal_cpg_ph_ca -> at(i_ca) - cal_cpg_ph_nca -> at(i_ca)) > std::abs(cal_cpg_bl_pre_nca_rms -> at(i_ca))/2.0;
                nca_found |= std::abs(cal_cpg_ph_ca -> at(i_ca) - cal_cpg_ph_nca -> at(i_ca)) < std::abs(cal_cpg_bl_pre_nca_rms -> at(i_ca))/2.0 && std::abs(cal_cpg_ph_nca -> at(i_ca)) > std::abs(cal_cpg_bl_pre_ca_rms -> at(i_ca));
                // True CA
                if (ca_found) cas.push_back(cal_cpg_ph_ca -> at(i_ca) * p1.at(i_ca)/p1.at(0));
                cas_all.push_back(cal_cpg_ph_ca -> at(i_ca) * p1.at(i_ca)/p1.at(0));
                // True NCA 
                if (nca_found){
                    ncas.push_back(cal_cpg_ph_nca -> at(i_ca) * p1.at(i_ca)/p1.at(0));
                    ncas_weighted.push_back(df[i_ca] * cal_cpg_ph_nca -> at(i_ca) * p1.at(i_ca)/p1.at(0));
                }
                ncas_all.push_back(cal_cpg_ph_nca -> at(i_ca) * p1.at(i_ca)/p1.at(0));
                ncas_weighted_all.push_back(df[i_ca] * cal_cpg_ph_nca -> at(i_ca) * p1.at(i_ca)/p1.at(0));

                if (cal_edep -> at(i_ca) > global_tresh) energy_reached = true;
                if (ca_found && nca_found) energies.push_back(cal_edep -> at(i_ca));

                energies_all.push_back(cal_edep -> at(i_ca));

            }
            // calculate new values
            e_added = std::accumulate(energies.begin(), energies.end(), 0.0);
            e_added_all = std::accumulate(energies_all.begin(), energies_all.end(), 0.0);
            nca = std::accumulate(ncas.begin(), ncas.end(), 0.0);
            nca_all = std::accumulate(ncas_all.begin(), ncas_all.end(), 0.0);
            ca = std::accumulate(cas.begin(), cas.end(), 0.0);
            ca_all = std::accumulate(cas_all.begin(), cas_all.end(), 0.0);
            nca_weighted = std::accumulate(ncas_weighted.begin(), ncas_weighted.end(), 0.0);
            nca_weighted_all = std::accumulate(ncas_weighted_all.begin(), ncas_weighted_all.end(), 0.0);
            new_pos = (ca + nca) / (ca - nca_weighted);
            new_pos_all = (ca_all + nca_all) / (ca_all - nca_weighted_all);

            // rdip & ert from grid with highest energy
            for (unsigned int i_ca = 0; i_ca < energies_all.size(); ++i_ca){
                if (energies_all[i_ca] == *std::max_element(energies.begin(), energies.end())){
                    rdip = cal_cpg_diff_rdip -> at(i_ca);
                    ert = cal_cpg_diff_ert -> at(i_ca);
                }
            }
            // // Check for bad pulse; Not all badflag conditions are working here
            // Sample lenght
            for (unsigned int i_ca = 0; i_ca < cal_cpg_ph_ca -> size(); i_ca++){
                global_bad_flag |= (cal_tracelength -> at(i_ca) != sample_length);
            }
            // NCA amplitude too low wrt CA amplitude (non-physical)
            global_bad_flag |= (nca < -1.5*ca);
            // NCA amplitude too high wrt CA amplitude (non-physical)
            global_bad_flag |= (nca > ca/2.);
            // CA == 0, if no CA is found in whole event
            global_bad_flag |= (ca == 0);
            // NCA == 0, if no NCA is found in whole event
            global_bad_flag |= (nca_all == 0);
            // Energy <= global_tresh, at least one grid
            global_bad_flag |= !energy_reached;
            // Energy <= global_tresh, whole event (There could be some grid with E<0)
            global_bad_flag |= (e_added <= global_tresh);

            if(global_bad_flag == 0){
                hist_edep -> Fill(e_added);
                hist_pos -> Fill(new_pos);
                hist_ca_nca -> Fill(ca, -nca);
            }
            output_tree -> Fill();
        }

        // std::cout << "Results of pol1 fit to z_pos" << std::endl;
        // hist_pos -> Fit("pol1", "0", "", 0.2, 0.97);
        hist_pos -> SetXTitle("rel. z-Position");
        hist_pos -> SetYTitle("Counts");

        hist_edep -> SetXTitle("Energy [keV]");
        hist_edep -> SetYTitle("Counts");

        output_tree -> Write();
        output -> Write();
        output -> Close();
        delete output; 
        input_grid -> Close();
        delete input_grid;       
    }
    else{
        std::cout << "Input File not valid" << std::endl;
    }          
}