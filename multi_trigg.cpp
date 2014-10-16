#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <string>

#include <TCanvas.h>
#include <TFile.h>
#include <TGaxis.h>
#include <TH1F.h>
#include <TPad.h>
#include <TStyle.h>
#include <TTree.h>

void multi_trigg(std::string inputname) {
	TCanvas *canv1 = new TCanvas("Multi Trigger", "Multi Trigger"); 
    TPad *pad1 = new TPad("pad1", "", 0.0,0.5,0.5,1.0);
    pad1->Draw();
    TPad *pad2 = new TPad("pad2", "", 0.5,0.5,1.0,1.0);
    pad2->Draw();
    TPad *pad3 = new TPad("pad3", "", 0.0,0.0,0.5,0.5);
    pad3->Draw();
    TPad *pad4 = new TPad("pad4", "", 0.5,0.0,1.0,0.5);
    pad4->Draw();

    TH1I *h1_ca_trigg = new TH1I("CA Trigger", "CA Trigger", 5, 0, 5);
    TH1I *h1_nca_trigg = new TH1I("NCA Trigger", "NCA Trigger", 5, 0, 5);
    TH1I *h1_all_trigg = new TH1I("Alle Trigger", "Alle Trigger", 9, 0, 9);
	TH1I *h1_true_event = new TH1I("(CA & NCA) Trigger + Channel > 5", "(CA & NCA) Trigger + Channel > 5", 5, 0, 5);

    TFile *current_file = new TFile(inputname.c_str(), "READ");
    TTree *input_tree = (TTree*) current_file -> Get("cal");

    std::vector<float> *cal_cpg_ph_ca = NULL;
    std::vector<float> *cal_cpg_ph_nca = NULL;
    std::vector<int> *cal_det = NULL;
    std::vector<int> *raw_trig = NULL;
    std::vector<int> *flag_bad_pulse = NULL;


    std::vector<bool> det_trig;
    det_trig.push_back(false);
    det_trig.push_back(false);
    det_trig.push_back(false);
    det_trig.push_back(false);
   
    std::vector<bool> canal_trig;
    canal_trig.push_back(false);
    canal_trig.push_back(false);
    canal_trig.push_back(false);
    canal_trig.push_back(false);    

    input_tree -> SetBranchStatus("*", false);
    input_tree -> SetBranchStatus("cal_det", true);
    input_tree -> SetBranchStatus("cal_cpg_ph_ca", true);
    input_tree -> SetBranchStatus("cal_cpg_ph_nca", true);
    input_tree -> SetBranchStatus("raw_trig", true);
    input_tree -> SetBranchStatus("flag_bad_pulse", true);

    input_tree -> SetBranchAddress("cal_det", &cal_det);
    input_tree -> SetBranchAddress("cal_cpg_ph_ca", &cal_cpg_ph_ca);
    input_tree -> SetBranchAddress("cal_cpg_ph_nca", &cal_cpg_ph_nca);
    input_tree -> SetBranchAddress("raw_trig", &raw_trig);
    input_tree -> SetBranchAddress("flag_bad_pulse", &flag_bad_pulse);    

    // number of events for loop
    int i_number_events = input_tree -> GetEntries();

    int ca_triggs;
    int nca_triggs;
    int all_triggs;
    int true_event;

    int thresh = 5;
    for(int i = 0; i < i_number_events; i++){
        for (int j = 0; j < det_trig.size(); ++j){
            det_trig[j] = false;
            canal_trig[j] = false;
        }
        for (std::vector<int>::iterator it = raw_trig->begin(); it != raw_trig->end(); ++it) {
            switch(*it){
	            case 1 :
                    det_trig[0] = true;                 
                    break;
                case 2 : 
                    canal_trig[0] = true;
                    break;
                case 3 :
                    det_trig[1] = true;
                    break;
                case 4 :
                    canal_trig[1] = true;
                    break;
                case 5 :
                    det_trig[2] = true;             
                    break;
                case 6 :
                    canal_trig[2] = true;                   
                    break;
                case 7 :
                    det_trig[3] = true;             
                    break;
                case 8 :
                    canal_trig[3] = true;                   
                    break;
                default:
                    std::cout<< "no trigger" << std::endl;
            }      
        }
    	// std::cout << i << std::endl;
        input_tree -> GetEntry(i);
        ca_triggs = 0;
        nca_triggs = 0;
        all_triggs = 0;
        true_event = 0;
        for (std::vector<int>::iterator it = raw_trig->begin(); it != raw_trig->end(); ++it) {
        	// std::cout << *it << std::endl; 
	    	if (*it % 2 == 0){
	    		// std::cout << "nca" << std::endl;
	    		ca_triggs++;
	    	}
	    	if (*it % 2 == 1){
	    		// std::cout << "ca" << std::endl;
	    		nca_triggs++;
	    	}
	    	all_triggs++;
	    }
        for (int j = 0; j < 4; ++j){
            if (det_trig[j] == true && canal_trig[j] == true && (cal_cpg_ph_ca->at(j) - cal_cpg_ph_nca->at(j)) > thresh){           
            	true_event++;
            }
        }
	    // std::cout << ca_triggs << std::endl;
	    // std::cout << nca_triggs << std::endl;
	    // std::cout << all_triggs << std::endl;
	    h1_ca_trigg -> Fill(ca_triggs);
	    h1_nca_trigg -> Fill(nca_triggs);
	    h1_all_trigg -> Fill(all_triggs);
	    h1_true_event -> Fill(true_event);

	}
	pad1 -> cd();
	h1_ca_trigg -> Draw();
	pad2 -> cd();
	h1_nca_trigg -> Draw();
	pad3 -> cd();
	h1_all_trigg -> Draw();
	pad4 -> cd();
	h1_true_event -> Draw();

    std::cout << "" << std::endl;
    current_file -> Close();
    delete current_file;
}