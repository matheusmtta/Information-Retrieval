#include <CkSpider.h>
#include <iostream>
#include <chrono>
#include <string>

using namespace std;

void pageDisplay(int idx, string url, string title){
	if (idx == 0) cout << "\n";
	cout << "Page " << idx << "\n";
	cout << title << "\r\n";
	cout << url   << "\r\n" << "\r\n";
}

int main(){
	//Primeira URL a ser coletada
	//e número de URLs a serem
	//posteriormente pesquisadas
	string firstURL; cin >> firstURL;
	int n; cin >> n;

	//Inicializa spider com a primeira
	//URL dada no input e exibe seu título
	//e sua url
	CkSpider spider;

	spider.Initialize(firstURL.c_str());
	spider.AddUnspidered(firstURL.c_str());

	auto initialExeTime = chrono::high_resolution_clock::now();

	//Procura por outras n páginas a partir da url
	//disponibilizada inicialmente
	for (int i = 0; i < n+1; i++){
		bool found = spider.CrawlNext();
		
		//Caso encontremos, exibimos a página
		if (found)
			pageDisplay(i, spider.lastUrl(), spider.lastHtmlTitle());
		//Caso contrário,
		else {
			//Não existem mais páginas a serem coletadas na url inicial
			if (!spider.get_NumUnspidered())
				cout << "Nao existem mais paginas a serem coletadas" << endl;
			//Ocorreu um erro durante a coleta
			else
				cout << spider.lastErrorText() << "\r\n" << endl;
		}

		spider.SleepMs(1000);
	}

	auto finalExeTime = std::chrono::high_resolution_clock::now();

	double exeTime = chrono::duration_cast<std::chrono::microseconds>(finalExeTime - initialExeTime).count(); 
	double avgExeTime = (exeTime*1e-6)/(n+1) - 1;

	cout << "Tempo de coleta de páginas médio: " << avgExeTime << endl;
}