// Macro para analise de dados com root
// Pedro Mariano
// roda com: root posicao_nw_variosarq.C

//********************************************************************

// Acessa os arquivos .points variando o b e faz 7 gráficos para cada b
// 1. Nw nucleo A
// 2. Nw nucleo B
// 3. Nbin
// 4. Core A
// 5. Mantle A
// 6. Distribuicao em X nucleo A
// 7. Distribuicao em Y nucleo A
// Printa os graficos e salva num arquivo .root com nome
// Pb-p_b[0,1,2,...]._8.8TeV_nucleonmodel_5000evt_analise.root

// ref: https://root-forum.cern.ch/t/tree-and-histogram-read-data-from-txt-file/19800/2
//********************************************************************

#include "TString.h"
#include "TSystem.h"
#include "TInterpreter.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TH1.h"

#include <fstream>
#include <cstdio>
#include <cmath>

// utilizar esse programa para analisar Pb-p

void posicao_nw_variosarq()
{
	int b;
	for (b = 0; b <= 4; b++)
	{

		// montar a string variando o b

		string file = "Pb-p_b"; // Pb-p
		file += to_string(b);
		file += "._8.8TeV_nucleonmodel_5000evt.points"; // Pb-p

		// converter string para const char*
		char *writable = new char[file.size() + 1];
		std::copy(file.begin(), file.end(), writable);
		writable[file.size()] = '\0'; // don't forget the terminating 0

		// arquivo que ele vai ler
		const char *file_in = writable;

		// mudar a string para file_out
		file.resize(file.size() - 7);
		file += "_analise.root";
		printf("file = %s \n", file.c_str());

		// arquivo que ele vai escrever
		const char *file_out = file.c_str();

		ifstream in;
		in.open(file_in);

		Float_t x, y;
		Int_t nlines = 0;
		TFile *f = new TFile(file_out, "RECREATE");
		TNtuple *ntuple_A = new TNtuple("Nw A", "data from .points", "x:y");
		TNtuple *ntuple_B = new TNtuple("Nw B", "data from .points", "x:y");
		TNtuple *ntuple_nbin = new TNtuple("Nbin", "data from .points", "x:y");
		TNtuple *ntuple_core_A = new TNtuple("Core A", "data from .points", "x:y");
		TNtuple *ntuple_mantle_A = new TNtuple("Mantle A", "data from .points", "x:y");

		Int_t l, k_int, k_int2, pos1, pos2, c, m = 0;
		Float_t n = 0;
		string str;
		Int_t contagem = 0;

		string line;
		while (getline(in, line))
		{
			contagem++;
			if (m != 0)
			{
				nlines++;
				pos1 = line.find(" ");
				pos2 = line.find(" ", pos1 + 1);

				x = stof(line.substr(0, pos1));
				y = stof(line.substr(pos1 + 1, pos2 - pos1));
				c = stoi(line.substr(pos2 + 1));

				if (c > 0)
				{ // Nw nucleo A
					ntuple_A->Fill(x, y);
				}
				if (c < 0)
				{ // Nw nucleo B
					ntuple_B->Fill(x, y);
				}
				if (c == 0)
				{ // colisoes binarias
					ntuple_nbin->Fill(x, y);
				}
				if (c > 1)
				{ // core A
					ntuple_core_A->Fill(x, y);
				}
				if (c == 1)
				{ // mantle A
					ntuple_mantle_A->Fill(x, y);
				}
				m = m - 1;
			}
			else
			{
				if (floor(n) == n)
				{ // primeira parte de um bloco
					k_int = line.find(" ");
					k_int2 = line.find(" ", k_int + 1);
					str = line.substr(k_int + 1, k_int2 - k_int - 1);
					m = stoi(str);
				}
				else
				{ // segunda parte de um bloco
					m = stoi(line.substr(0));
				}
				n += 0.5;
			}
		}

		printf(" found %d points \n", nlines);

		in.close();

		Int_t xbins = 100;
		Int_t ybins = 100;

		// ajustar pelas medidas
		// Double_t xmin = ntuple_nbin->GetMinimum("x");
		// Double_t xmax = ntuple_nbin->GetMaximum("x");
		// Double_t difx = xmax-xmin;
		// xmax += difx*0.1;
		// xmin -= difx*0.1;
		// Double_t ymin = ntuple_nbin->GetMinimum("y");
		// Double_t ymax = ntuple_nbin->GetMaximum("y");
		// Double_t dify = ymax-ymin;
		// ymax += dify*0.1;
		// ymin -= dify*0.1;

		// padronizado
		Double_t xmin, xmax, ymin, ymax;
		xmax = 5;
		xmin = -5;
		ymax = 5;
		ymin = -5;

		// GRAFICOS
		// 1. Nw nucleo A ----------------------------------------------------------------------------
		auto c1 = new TCanvas("c1", "c1", 600, 400);
		string str1 = "Pb Nw distribution - b=";
		str1 += to_string(b);
		TH2F *h1 = new TH2F("h1", str1.c_str(), xbins, xmin, xmax, ybins, ymin, ymax);

		ntuple_A->Project("h1", "y:x");

		h1->GetXaxis()->SetTitle("X (fm)");
		h1->GetYaxis()->SetTitle("Y (fm)");
		h1->Draw("colz");

		gStyle->SetOptStat(1110);
		gPad->Update();
		TPaveStats *st = (TPaveStats *)h1->FindObject("stats");
		st->SetX1NDC(0.12); // new x start position
		st->SetX2NDC(0.28); // new x end position

		str1 += ".jpg";
		c1->Print(str1.c_str());

		// 2. Nw nucleo B ----------------------------------------------------------------------------
		auto c2 = new TCanvas("c2", "c2", 600, 400);
		string str2 = "p Nw distribution - b=";
		str2 += to_string(b);
		TH2F *h2 = new TH2F("h2", str2.c_str(), xbins, xmin, xmax, ybins, ymin, ymax);

		ntuple_B->Project("h2", "y:x");

		h2->GetXaxis()->SetTitle("X (fm)");
		h2->GetYaxis()->SetTitle("Y (fm)");
		h2->Draw("colz");

		gStyle->SetOptStat(1110);
		gPad->Update();
		TPaveStats *st2 = (TPaveStats *)h2->FindObject("stats");
		st2->SetX1NDC(0.12); // new x start position
		st2->SetX2NDC(0.28); // new x end position

		str2 += ".jpg";
		c2->Print(str2.c_str());

		// 3. Nbin -------------------------------------------------------------------------------
		auto c3 = new TCanvas("c3", "c3", 600, 400);
		string str3 = "Pb-p Nbin distribution - b=";
		str3 += to_string(b);
		TH2F *h3 = new TH2F("h3", str3.c_str(), xbins, xmin, xmax, ybins, ymin, ymax);

		ntuple_nbin->Project("h3", "y:x");

		h3->GetXaxis()->SetTitle("X (fm)");
		h3->GetYaxis()->SetTitle("Y (fm)");
		h3->Draw("colz");

		gStyle->SetOptStat(1110);
		gPad->Update();
		TPaveStats *st3 = (TPaveStats *)h3->FindObject("stats");
		st3->SetX1NDC(0.12); // new x start position
		st3->SetX2NDC(0.28); // new x end position

		str3 += ".jpg";
		c3->Print(str3.c_str());

		// 4. Core A ----------------------------------------------------------------------------
		auto c4 = new TCanvas("c4", "c4", 600, 400);
		string str4 = "Pb core distribution - b=";
		str4 += to_string(b);
		TH2F *h4 = new TH2F("h4", str4.c_str(), xbins, xmin, xmax, ybins, ymin, ymax);

		ntuple_core_A->Project("h4", "y:x");

		h4->GetXaxis()->SetTitle("X (fm)");
		h4->GetYaxis()->SetTitle("Y (fm)");
		h4->Draw("colz");

		gStyle->SetOptStat(1110);
		gPad->Update();
		TPaveStats *st4 = (TPaveStats *)h4->FindObject("stats");
		st4->SetX1NDC(0.12); // new x start position
		st4->SetX2NDC(0.28); // new x end position

		str4 += ".jpg";
		c4->Print(str4.c_str());

		// 5. Mantle A -------------------------------------------------------------------------
		auto c5 = new TCanvas("c5", "c5", 600, 400);
		string str5 = "Pb mantle distribution - b=";
		str5 += to_string(b);
		TH2F *h5 = new TH2F("h5", str5.c_str(), xbins, xmin, xmax, ybins, ymin, ymax);

		ntuple_mantle_A->Project("h5", "y:x");

		h5->GetXaxis()->SetTitle("X (fm)");
		h5->GetYaxis()->SetTitle("Y (fm)");
		h5->Draw("colz");

		gStyle->SetOptStat(1110);
		gPad->Update();
		TPaveStats *st5 = (TPaveStats *)h5->FindObject("stats");
		st5->SetX1NDC(0.12); // new x start position
		st5->SetX2NDC(0.28); // new x end position

		str5 += ".jpg";
		c5->Print(str5.c_str());

		// 6. Distribuicao em X nucleo A ----------------------------------------------------------------------------
		auto c6 = new TCanvas("c6", "c6", 600, 400);
		string str6 = "Pb Nw distribution in X - b=";
		str6 += to_string(b);
		TH1F *h6 = new TH1F("h6", str6.c_str(), xbins, xmin, xmax);

		ntuple_A->Project("h6", "x");

		h6->GetXaxis()->SetTitle("X (fm)");
		h6->Draw();

		gStyle->SetOptStat(1110);
		gPad->Update();
		TPaveStats *st6 = (TPaveStats *)h6->FindObject("stats");
		st6->SetX1NDC(0.12); // new x start position
		st6->SetX2NDC(0.28); // new x end position

		str6 += ".jpg";
		c6->Print(str6.c_str());

		// 7. Distribuicao em Y nucleo A ----------------------------------------------------------------------------
		auto c7 = new TCanvas("c7", "c7", 600, 400);
		string str7 = "Pb Nw distribution in Y - b=";
		str7 += to_string(b);
		TH1F *h7 = new TH1F("h7", str7.c_str(), xbins, xmin, xmax);

		ntuple_A->Project("h7", "y");

		h7->GetXaxis()->SetTitle("X (fm)");
		h7->Draw();

		gStyle->SetOptStat(1110);
		gPad->Update();
		TPaveStats *st7 = (TPaveStats *)h7->FindObject("stats");
		st7->SetX1NDC(0.12); // new x start position
		st7->SetX2NDC(0.28); // new x end position

		str7 += ".jpg";
		c7->Print(str7.c_str());

		//  ---------------------------------------------------------------------------------

		// write file .root
		f->Write();

	} // fecha o for que roda cada arquivo

} // fecha o void
