#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <string>

#include <TCanvas.h>
#include <TFile.h>
#include <TGaxis.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TPad.h>
#include <TStyle.h>
#include <TTree.h>


// Script to add quads on ca/nca level
// Calculate Energy and zpos independent
void add_quad(std::string inputname_grid) {

    TFile *input_grid = new TFile (inputname_grid.c_str(), "READ");

    TTree *input_tree = dynamic_cast<TTree*>( input_grid->Get("cal"));
      
    TH1D *hist_pos = new TH1D ("Rel. z-Position", "Rel.  z-Position", 500, -1.0, 2.0);
    TH1D *hist_edep = new TH1D ("edep_hist", "edep_hist",1024 , 0, 1024);
    TH2D *hist_e_pos = new TH2D ("edep_pos", "edep_pos", 1024 , 0 , 1024, 256, -2.0, 3.0);
      
    if(input_tree!=NULL){
        
        std::vector<float> *cal_cpg_ph_ca = NULL;
        std::vector<float> *cal_cpg_ph_nca = NULL;
        std::vector<float> *cal_edep = NULL;
        std::vector<float> *cal_cpg_ph_cathode = NULL;
        std::vector<int> *cal_det = NULL;
        std::vector<int> *flag_bad_pulse = NULL;
        std::vector<int> *raw_trig = NULL;
        
        input_tree->SetBranchStatus("*", false);

        input_tree->SetBranchStatus("cal_cpg_ph_ca", true);
        input_tree->SetBranchStatus("cal_cpg_ph_nca", true);
        input_tree->SetBranchStatus("cal_edep", true);
        input_tree->SetBranchStatus("flag_bad_pulse", true);
        input_tree->SetBranchStatus("raw_trig", true);
        input_tree->SetBranchStatus("cal_det", true);

        input_tree->SetBranchAddress("cal_cpg_ph_ca", &cal_cpg_ph_ca);
        input_tree->SetBranchAddress("cal_cpg_ph_nca", &cal_cpg_ph_nca);
        input_tree->SetBranchAddress("cal_edep", &cal_edep);
        input_tree->SetBranchAddress("flag_bad_pulse", &flag_bad_pulse);
        input_tree->SetBranchAddress("raw_trig", &raw_trig);
        input_tree->SetBranchAddress("cal_det", &cal_det);	
        
       
        std::string outputname = input_grid->GetName();
        outputname.insert(outputname.size() - 5, "_new_processed");
        
        TFile *output = new TFile (outputname.c_str(), "RECREATE");
        std::cout <<"File created "<< outputname << std::endl;
        output->cd();
        
        TTree *output_tree = new TTree ("new_cal", "new_cal");
        
        float new_pos;
        float nca;
        float ca;
        float nca_good;
        float ca_good;
        float cathode;
        float energy;
        float diff_sig;
        bool whole_bad;
        bool is_ca;
        bool is_noise;
        int ntrigger;
        float threshold;
        
        // // for gamma from robert
        // // from 20140919_141421.rootrc
        // std::vector<float> df;
        // df.push_back(0.850);
        // df.push_back(0.836);
        // df.push_back(0.764);
        // df.push_back(0.807);

        // for gamma from robert
        // calculated by hand 
        std::vector<float> df;
        df.push_back(0.891);
        df.push_back(0.953);
        df.push_back(0.806);
        df.push_back(0.836);

        // // for alpha from daniela
        // std::vector<float> df;
        // df.push_back(0.7634);
        // df.push_back(0.8584);
        // df.push_back(0.7594);
        // df.push_back(0.8286);

        // // from gain_correction
        // std::vector<float> gain;
        // gain.push_back(1.000);
        // gain.push_back(0.983);  
        // gain.push_back(0.962);
        // gain.push_back(1.029);
        // gain.push_back(0.937);
        // gain.push_back(0.888);
        // gain.push_back(0.910);
        // gain.push_back(0.890);

        // from gain_correction
        std::vector<float> gain;
        gain.push_back(1.000);
        gain.push_back(1.000);
        gain.push_back(1.000);
        gain.push_back(1.000);
        gain.push_back(1.000);
        gain.push_back(1.000);
        gain.push_back(1.000);
        gain.push_back(1.000);
        gain.push_back(1.000);


        // from gain_correction
        std::vector<float> mean;
        mean.push_back(252.0);
        mean.push_back(264.3);
        mean.push_back(253.5);
        mean.push_back(238.8);

        // NCA trigger
        std::vector<bool> det_trig;
        det_trig.push_back(false);
        det_trig.push_back(false);
        det_trig.push_back(false);
        det_trig.push_back(false);
       
        // CA trigger
        std::vector<bool> canal_trig;
        canal_trig.push_back(false);
        canal_trig.push_back(false);
        canal_trig.push_back(false);
        canal_trig.push_back(false);
        
        output_tree->Branch("nca", &nca);
        output_tree->Branch("ca", &ca);
        output_tree->Branch("cathode", &cathode);
        output_tree->Branch("new_pos", &new_pos);
        output_tree->Branch("edep", &energy);

        for(int i_entry=0; i_entry < input_tree->GetEntries(); ++i_entry){
    		whole_bad = true;
    	    ntrigger = 0;
    		input_tree->GetEntry(i_entry); 
    		
            // Is whole event bad?
            for(int i = 0; i < (cal_det -> size()); i++){
                if (flag_bad_pulse -> at(i) == 0){
                     whole_bad = false;
                }  
            }

    		if(whole_bad == false){
    			new_pos = 0;
    			nca = 0;
    			ca = 0;
                nca_good = 0;
                ca_good = 0;
    			cathode = 0;
    			energy = 0;
                diff_sig = 0;
                is_ca = false;
                is_noise = true;
                // Calculate Energy and zpos
    			for (int i_ca = 0; i_ca < cal_cpg_ph_ca->size(); i_ca++){
                    // Conditions
                    threshold = cal_cpg_ph_ca->at(i_ca) - df[i_ca] * cal_cpg_ph_nca->at(i_ca);
                    threshold *= (mean[0] / mean[i_ca]);
                    if (threshold > 10.0){
                        is_ca = true;
                    }
                    if (flag_bad_pulse -> at(i_ca) == 0 && cal_cpg_ph_ca->at(i_ca) > 0.0 && cal_cpg_ph_nca->at(i_ca) < 0.0){
                        is_noise = false;
                    }

                    // Get cathode signal always from all dets, but look for ca/nca assingment
                    if(is_ca){
                        // Trigger -> CA as CA
                        ca += cal_cpg_ph_ca->at(i_ca) * gain[2 * i_ca + 1] * (mean[0] / mean[i_ca]);
                    }
                    else{
                        // No Trigger -> CA as NCA
                        nca += df[i_ca] * cal_cpg_ph_ca->at(i_ca) * gain[2 * i_ca + 1] * (mean[0] / mean[i_ca]);
                    }   
                    // True NCA 
                    nca += df[i_ca] * cal_cpg_ph_nca->at(i_ca) * gain[2 * i_ca] * (mean[0] / mean[i_ca]);
                    cathode += cal_cpg_ph_ca->at(i_ca) * gain[2 * i_ca + 1] + cal_cpg_ph_nca->at(i_ca) * gain[2 * i_ca];
                    cathode *= (mean[0] / mean[i_ca]);
                    // energy only from non-bad grids
                    if (!is_noise){
                        if(is_ca){
                            // Trigger -> CA as CA
                            ca_good += cal_cpg_ph_ca->at(i_ca) * gain[2 * i_ca + 1] * (mean[0] / mean[i_ca]);
                        }
                        else{
                            // No Trigger -> CA as NCA
                            nca_good += cal_cpg_ph_ca->at(i_ca) * gain[2 * i_ca + 1] * (mean[0] / mean[i_ca]);
                        }   
                        // True NCA 
                        nca_good += df[i_ca] * cal_cpg_ph_nca->at(i_ca) * gain[2 * i_ca] * (mean[0] / mean[i_ca]);
                    }

    			}
                // calculate new values
                // 2.71 for physical energy
                energy = 2.71 * (ca_good - nca_good);
                diff_sig = ca - nca;
                new_pos = cathode / diff_sig;
                // It is possible that no energy could be reconstructed
                if (energy > 1.0){
                    hist_edep -> Fill(energy);    
                    hist_pos -> Fill(new_pos);
                    hist_e_pos -> Fill(energy, new_pos);      
                    output_tree -> Fill();  
                }
            }	
        }
        hist_pos -> SetXTitle("rel. z-Position");
        hist_pos -> SetYTitle("Counts");
        hist_pos -> Write();

        hist_edep -> SetXTitle("Energy [Channels]");
        hist_edep -> SetYTitle("Counts");
        hist_edep -> Write();
        hist_e_pos -> Write();
        output_tree -> Write();
        output -> Write();
        output -> Close();
        delete output; 
        input_grid -> Close();
        delete input_grid;       
    }		   
}
